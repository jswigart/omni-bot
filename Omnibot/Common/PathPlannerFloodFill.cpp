////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "PathPlannerFloodFill.h"
#include "IGameManager.h"
#include "IGame.h"
#include "Client.h"
#include "GoalManager.h"
#include "Timer.h"
#include "FileSystem.h"
#include "gmUtilityLib.h"
#include "PathPlannerFloodFillPathInterface.h"
#include "RenderBuffer.h"
#include "InterfaceFuncs.h"

using namespace std;

//////////////////////////////////////////////////////////////////////////

bool PathPlannerFloodFill::Obstacle::Expired() const
{
	if ( mExpireTime >= 0 && IGame::GetTime() > mExpireTime )
		return true;
	return false;
}

bool PathPlannerFloodFill::Obstacle::IsActive() const
{
	return mActive;
}

PathPlannerFloodFill::Obstacle::Obstacle() 
	: mActive( false )
	, mExpireTime( -1 )
{
}

PathPlannerFloodFill::Obstacle::~Obstacle()
{
}

//////////////////////////////////////////////////////////////////////////

PathPlannerFloodFill::PathPlannerFloodFill()
{
	// TEMP
	m_PlannerFlags.SetFlag(NAV_VIEW);
	m_PlannerFlags.SetFlag(NAVMESH_STEPPROCESS);

	m_CursorColor = COLOR::BLUE;

	mInfluenceBufferId = 0;
	mUpdateInfluenceBuffer = false;
	mInfluence = NULL;
}

PathPlannerFloodFill::~PathPlannerFloodFill()
{
	Shutdown();
}

bool PathPlannerFloodFill::Init( System & system )
{
	InitCommands();
	
	return true;
}

void PathPlannerFloodFill::RegisterScriptFunctions(gmMachine *a_machine)
{
}
struct compare 
{
	bool operator ()(const type_info & a, const type_info & b) const {
		return a.before( b );
	}
};

void PathPlannerFloodFill::Update( System & system )
{
	Prof(PathPlannerFloodFill);
	
	UpdateObstacles();
	
	if(m_PlannerFlags.CheckFlag(NAV_VIEW))
	{
		bool influenceDone = true;
		if ( mInfluence )
		{
			static int iterations = 200;
			influenceDone = mInfluence->UpdateInfluences( iterations );
		}
		
		float influenceMinWeight = 0.0f;
		float influenceMaxWeight = 1.0f;

		if ( mInfluence )
		{
			mInfluence->GetWeightRange( influenceMinWeight, influenceMaxWeight );
		}

		struct RenderSpanCell : SpanMap::RenderFunctor
		{
		public:
			RenderSpanCell( RenderBuffer::QuadList & lst, const Vector3f & pos, float radius )
				: mList( lst )
				, mViewPos( pos )
				, mRadiusSq( radius * radius )
			{
			}

			virtual void RenderCell( const Vector3f & pos, float cellSize, float influenceRatio )
			{
				if ( (mViewPos - pos).SquaredLength() > mRadiusSq )
					return;

				static obuint8 alpha = 255;

				RenderBuffer::Quad q;
				q.v[ 0 ] = pos + Vector3f( -cellSize, -cellSize, 0.0f );
				q.v[ 1 ] = pos + Vector3f(  cellSize, -cellSize, 0.0f );
				q.v[ 2 ] = pos + Vector3f(  cellSize,  cellSize, 0.0f );
				q.v[ 3 ] = pos + Vector3f( -cellSize,  cellSize, 0.0f );
				if ( influenceRatio == -1.0f )
					q.c = COLOR::BLACK.fade( 100 );
				else
					q.c = GetCoolWarmColor( influenceRatio ).fade( alpha );
				mList.push_back( q );
			}

			RenderBuffer::QuadList &	mList;
			Vector3f					mViewPos;
			float						mRadiusSq;
		};

		if ( System::mInstance->mGame->RendersToGame() )
		{
			static float radius = 1024.0f;

			Vector3f eyePos;
			Utils::GetLocalEyePosition( eyePos );

			RenderBuffer::QuadList prims;
			prims.reserve( mSpanMap.GetNumSpans() * 2 );

			RenderSpanCell renderCb( prims, eyePos, radius );
			mSpanMap.RenderWithCallback( renderCb, influenceDone ? mInfluence : NULL );

			for ( size_t i = 0; i < prims.size(); ++i )
				RenderBuffer::AddQuad( prims[ i ] );
		}
		else
		{
			if ( ( mUpdateInfluenceBuffer || mInfluenceBufferId == 0 ) && mSpanMap.GetNumSpans() > 0 )
			{
				Vector3f eyePos;
				Utils::GetLocalEyePosition( eyePos );

				mUpdateInfluenceBuffer = false;

				RenderBuffer::QuadList prims;
				prims.reserve( mSpanMap.GetNumSpans() * 2 );

				RenderSpanCell renderCb( prims, eyePos, FLT_MAX );
				mSpanMap.RenderWithCallback( renderCb, influenceDone ? mInfluence : NULL );

				RenderBuffer::StaticBufferCreate( mInfluenceBufferId, prims );
			}

			if ( mInfluenceBufferId != 0 && mSpanMap.GetNumSpans() == 0 )
			{
				RenderBuffer::StaticBufferDelete( mInfluenceBufferId );
				mInfluenceBufferId = 0;
			}

			if ( mInfluenceBufferId != 0 )
			{
				RenderBuffer::StaticBufferDraw( mInfluenceBufferId );
			}
		}

		//////////////////////////////////////////////////////////////////////////
		Vector3f vLocalPos, vLocalAim, vAimPos, vAimNormal;
		Utils::GetLocalEyePosition(vLocalPos);
		Utils::GetLocalFacing(vLocalAim);
		if(Utils::GetLocalAimPoint(vAimPos, &vAimNormal))
		{
			RenderBuffer::AddLine(vAimPos,
				vAimPos + vAimNormal * 16.f,
				m_CursorColor,
				IGame::GetDeltaTimeSecs()*2.f);

			RenderBuffer::AddLine(vAimPos,
				vAimPos + vAimNormal.Perpendicular() * 16.f, m_CursorColor,
				IGame::GetDeltaTimeSecs()*2.f);
		}
		m_CursorColor = COLOR::BLUE; // back to default

		//////////////////////////////////////////////////////////////////////////

		Vector3List::const_iterator cIt = m_StartPositions.begin();
		for(; cIt != m_StartPositions.end(); ++cIt)
		{
			AABB aabb(Vector3f::ZERO);
			aabb.Expand(Vector3f(mFillOptions.m_GridRadius, mFillOptions.m_GridRadius, 0.0f));
			aabb.Expand(Vector3f(-mFillOptions.m_GridRadius, -mFillOptions.m_GridRadius, 0.0f));
			aabb.m_Maxs[2] = mFillOptions.m_CharacterHeight - mFillOptions.m_CharacterStepHeight;
			aabb.Translate(*cIt);
			RenderBuffer::AddAABB(aabb, COLOR::BLACK);
		}
	}
}

void PathPlannerFloodFill::UpdateObstacles()
{
	for ( size_t o = 0; o < mObstacles.size(); ++o )
	{
		Obstacle & obs = mObstacles[ o ];

		Vector3f vel;
		BitFlag64 flags;
		Box3f obb;
		if ( !EngineFuncs::EntityVelocity( obs.mEntity, vel ) || 
			!InterfaceFuncs::GetEntityFlags( obs.mEntity, flags ) ||
			!EngineFuncs::EntityWorldOBB( obs.mEntity, obb ) )
		{
			obs.mExpireTime = 0;
		}
		else
		{
			const bool nowActive = vel.IsZero() && !flags.CheckFlag( ENT_FLAG_DISABLED );
			if ( obs.mActive != nowActive )
			{
				obs.mActive = nowActive;
			}

			if ( obs.mActive && obb != obs.mObb )
			{
				// update the old bounds and the new
				mSpanMap.MarkDirtyBounds( obs.mObb );
				obs.mObb = obb;
				mSpanMap.MarkDirtyBounds( obs.mObb );
			}
		}
	}
}

void PathPlannerFloodFill::UpdateFloodFill()
{

}

void PathPlannerFloodFill::Shutdown()
{
	Unload();
}

bool PathPlannerFloodFill::Load(const std::string &_mapname, bool _dl)
{
	if(_mapname.empty())
		return false;

	// Initialize a map that can contain the entire level
	AABB mapbounds;
	g_EngineFuncs->GetMapExtents( mapbounds );

	VectorQueue empty;
	mSpanFrontier.swap( empty );

	RenderBuffer::StaticBufferDelete( mInfluenceBufferId );
	mInfluenceBufferId = 0;

	OB_DELETE( mInfluence );

	mSpanMap.Clear();
	mSpanMap.Init( Vector3f(mapbounds.m_Mins), Vector3f(mapbounds.m_Maxs), 16.0f );

	EngineFuncs::ConsoleMessage(va("Created %d x %d span map",
		mSpanMap.GetNumCellsX(), mSpanMap.GetNumCellsY() ) );
	
	// Automatically seed the flood fill with known features
	std::vector< AutoNavFeature > features;
	features.resize( 4096 );

	const int numFeatures = g_EngineFuncs->GetAutoNavFeatures( &features[ 0 ], features.size() );
	for ( int i = 0; i < numFeatures; ++i )
	{
		mSpanFrontier.push( features[ i ].m_Position );
		mSpanFrontier.push( features[ i ].m_TargetPosition );
	}
	
	return false;
}

bool PathPlannerFloodFill::Save(const std::string &_mapname)
{
	if(_mapname.empty())
		return false;

	return false;
}

bool PathPlannerFloodFill::IsReady() const
{
	return true;//!mRuntimeSectors.empty();
}

bool PathPlannerFloodFill::GetNavFlagByName(const std::string &_flagname, NavFlags &_flag) const
{
	_flag = 0;
	return false;
}

void PathPlannerFloodFill::Unload()
{
	OB_DELETE( mInfluence );
}

void PathPlannerFloodFill::RegisterGameGoals()
{
}

static bool GetGroundPos( const Vector3f & pos, Vector3f & groundPosOut )
{
	obTraceResult tr;
	EngineFuncs::TraceLine(tr,
		pos,
		pos - Vector3f::UNIT_Z * 100000.0f,
		NULL/*&bounds*/,
		TR_MASK_FLOODFILL,
		-1,
		False);
	if ( tr.m_Fraction > 0.0f && tr.m_Fraction < 1.0f )
	{
		groundPosOut = tr.m_Endpos;
		return true;
	}
	return false;
}

static bool GetHeightAtPos( const Vector3f & pos, float & height )
{
	obTraceResult tr;
	EngineFuncs::TraceLine(tr,
		pos + Vector3f( 0.0f, 0.0f, 1.0f ),
		pos + Vector3f::UNIT_Z * 100000.0f,
		NULL/*&bounds*/,
		TR_MASK_FLOODFILL,
		-1,
		False);
	if ( tr.m_Fraction > 0.0f && tr.m_Fraction < 1.0f )
	{
		height = tr.m_Endpos[ 2 ] - pos.Z();
		return true;
	}
	return false;
}

static bool TestForValidNode( Vector3f & spanPos, float & spanHeight )
{
	const float SpanHeightMin = 32.0f;
	const float SpanStepHeight = 32.0f;

	const Vector3f dn0(0.f,0.f,SpanStepHeight);
	const Vector3f dn1(0.f,0.f,-1024.f);

	const Vector3f up0(0.f,0.f,1.f);
	const Vector3f up1(0.f,0.f,1024.f);

	obTraceResult tr;
	EngineFuncs::TraceLine(tr,spanPos+dn0,spanPos+dn1,NULL,TR_MASK_FLOODFILL,-1,False);
	if ( tr.m_Fraction < 1.0f )
	{
		if ( tr.m_Normal[ 2 ] < 0.707f )
			return false;

		spanPos = tr.m_Endpos;
		EngineFuncs::TraceLine(tr,spanPos+up0,spanPos+up1,NULL,TR_MASK_FLOODFILL,-1,False);

		spanHeight = (spanPos+up1).Z() - spanPos.Z();
		if ( tr.m_Fraction < 1.0f )
			spanHeight = tr.m_Endpos[2] - spanPos.Z();

		if ( spanHeight > SpanHeightMin )
		{
			return true;
		}
	}
	return false;
}

int PathPlannerFloodFill::Process_FloodFill()
{
	Prof(Process_FloodFill);
	
	const Vector3f step[] =
	{
		Vector3f( -mFillOptions.m_GridRadius, 0.0f, mFillOptions.m_CharacterStepHeight ),
		Vector3f(  mFillOptions.m_GridRadius, 0.0f, mFillOptions.m_CharacterStepHeight ),
		Vector3f( 0.0f, -mFillOptions.m_GridRadius, mFillOptions.m_CharacterStepHeight ),
		Vector3f( 0.0f,  mFillOptions.m_GridRadius, mFillOptions.m_CharacterStepHeight ),
	};
	const int stepdirs = sizeof(step) / sizeof(step[0]);

	Timer tme;

	while ( !mSpanFrontier.empty()  )
	{
		Vector3f spanPos = mSpanFrontier.front();
		mSpanFrontier.pop();

		float spanHeight = 0.0f;
		if ( TestForValidNode( spanPos, spanHeight ) && mSpanMap.AddOpenSpan( spanPos, spanHeight ) )
		{
			//RenderBuffer::AddCircle( spanPos, 16.0f, COLOR::GREEN, 0.1f );

			for ( int i = 0; i < stepdirs; ++i )
			{
				Vector3f expandPos = spanPos + step[ i ];

				obTraceResult tr;
				EngineFuncs::TraceLine(tr,
					spanPos + Vector3f(0,0,4),
					expandPos,
					NULL,
					TR_MASK_FLOODFILL,
					-1,
					False);

				if ( tr.m_Fraction == 1.0f )
					mSpanFrontier.push( expandPos );
			}
		}

		if(tme.GetElapsedSeconds() > 0.01)
			return Function_InProgress;
	}

	mSpanMap.IndexSpanNodes();
	
	delete mInfluence;
	mInfluence = NULL;
	
	return Function_Finished;
}

void PathPlannerFloodFill::AddFloodStart(const Vector3f &_vec)
{
	m_StartPositions.push_back(_vec);
	EngineFuncs::ConsoleMessage("Added Flood Fill Start");
}

void PathPlannerFloodFill::ClearFloodStarts()
{
	EngineFuncs::ConsoleMessage(va("Clearing %d flood start nodes.", m_StartPositions.size()));
	m_StartPositions.clear();
}

void PathPlannerFloodFill::SaveFloodStarts()
{
	std::string strMap = g_EngineFuncs->GetMapName();
	strMap += ".navstarts";

	char strBuffer[1024] = {};
	sprintf(strBuffer, "nav/%s", strMap.c_str());

	File f;
	f.OpenForWrite(strBuffer, File::Text);
	if(f.IsOpen())
	{
		f.WriteInt32((obuint32)m_StartPositions.size());
		Vector3List::const_iterator cIt = m_StartPositions.begin();
		for(; cIt != m_StartPositions.end(); ++cIt)
		{
			f.WriteFloat((*cIt).X());
			f.WriteFloat((*cIt).Y());
			f.WriteFloat((*cIt).Z());
			f.WriteNewLine();
		}
		f.Close();
	}
	EngineFuncs::ConsoleMessage(va("Saved %d nav starts from %s",
		m_StartPositions.size(), strMap.c_str()));
}

void PathPlannerFloodFill::LoadFloodStarts()
{
	std::string strMap = g_EngineFuncs->GetMapName();
	strMap += ".navstarts";

	char strBuffer[1024] = {};
	sprintf(strBuffer, "nav/%s", strMap.c_str());

	File f;
	f.OpenForRead(strBuffer, File::Text);
	if(f.IsOpen())
	{
		obuint32 iNumPositions = 0;
		f.ReadInt32(iNumPositions);

		m_StartPositions.resize(0);
		m_StartPositions.reserve(iNumPositions);

		Vector3f vPos;
		for(obuint32 i = 0; i < iNumPositions; ++i)
		{
			f.ReadFloat(vPos.X());
			f.ReadFloat(vPos.Y());
			f.ReadFloat(vPos.Z());
			m_StartPositions.push_back( vPos );
		}
		f.Close();
	}
	EngineFuncs::ConsoleMessage(va("Loaded %d nav starts from %s",
		m_StartPositions.size(), strMap.c_str()));
}

void PathPlannerFloodFill::FloodFill(const FloodFillOptions &_options)
{
	if( System::mInstance->mGame->RemoveUpdateFunction("NavMesh_FloodFill"))
		return;

	EngineFuncs::ConsoleMessage("Initializing Flood Fill.");

	mFillOptions = _options;

	VectorQueue empty;
	mSpanFrontier.swap( empty );

	for(obuint32 i = 0; i < m_StartPositions.size(); ++i)
	{
		mSpanFrontier.push( m_StartPositions[ i ] );
	}

	mSpanMap.ClearSpans();
	OB_DELETE( mInfluence );

	FunctorPtr f(new ObjFunctor<PathPlannerFloodFill>(this, &PathPlannerFloodFill::Process_FloodFill));

	System::mInstance->mGame->AddUpdateFunction("NavMesh_FloodFill", f);
}

//////////////////////////////////////////////////////////////////////////

bool PathPlannerFloodFill::GetNavInfo(const Vector3f &pos,obint32 &_id,std::string &_name)
{
	return false;
}

void PathPlannerFloodFill::AddEntityConnection(const Event_EntityConnection &_conn)
{
}

void PathPlannerFloodFill::RemoveEntityConnection(GameEntity _ent)
{
}

Vector3f PathPlannerFloodFill::GetDisplayPosition(const Vector3f &_pos)
{
	return _pos;
}

PathPlannerFloodFill::SpanMap::InfluenceMap * PathPlannerFloodFill::AllocInfluenceMap()
{
	SpanMap::InfluenceMap * influence = mSpanMap.CreateInfluenceLayer();
	mActiveInfluences.push_back( influence );
	return influence;
}

//////////////////////////////////////////////////////////////////////////

void PathPlannerFloodFill::EntityCreated( const EntityInstance &ei )
{
	if ( ei.m_EntityCategory.CheckFlag(ENT_CAT_OBSTACLE) )
	{
		Obstacle obs;
		obs.mEntity = ei.m_Entity;
		mObstacles.push_back( obs );
	}
}

void PathPlannerFloodFill::EntityDeleted( const EntityInstance &ei )
{
	for ( size_t i = 0; i < mObstacles.size(); ++i )
	{
		if ( mObstacles[ i ].mEntity == ei.m_Entity )
		{
			// mark it for immediate removal
			mObstacles[ i ].mExpireTime = 0;
		}
	}
}

PathInterface * PathPlannerFloodFill::AllocPathInterface( Client * client )
{
	return new FloodFillPathInterface( client, this ); 
}
