////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "MapGoal.h"
#include "gmbinder2/gmbinder2_class.h"
#include "ScriptManager.h"
#include "PathPlannerBase.h"
#include "TriggerManager.h"
#include "IGameManager.h"
#include "Path.h"
#include "RenderBuffer.h"

#include "gmCall.h"
#include "gmBot.h"
#include "gmSchemaLib.h"

#include <boost/algorithm/string.hpp>

static int32_t sNextSerialNum = 0;

//////////////////////////////////////////////////////////////////////////
#define GETMACHINE() ScriptManager::GetInstance()->GetMachine()
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

int32_t GetMapGoalSerial()
{
	return ++sNextSerialNum;
}

MapGoal::MapGoal( const char *_goaltype )
	: mGoalType( _goaltype ? _goaltype : "" )
	, mGoalTypeHash( _goaltype ? Utils::MakeHash32( _goaltype ) : 0 )
{
	_Init();
}

float MapGoal::GetRadius() const
{
	return std::max( mRadius, mMinRadius );
}

void MapGoal::CopyFrom( MapGoal *_other )
{
	_Init();

	mGoalType = _other->mGoalType;
	mRadius = _other->mRadius;
	mMinRadius = _other->mMinRadius;
	mDefaultPriority = _other->mDefaultPriority;
	mRolePriorityBonus = _other->mRolePriorityBonus;
	mRandomUsePoint = _other->mRandomUsePoint;
	mRange = _other->mRange;

	mAvailableTeams = _other->mAvailableTeams;

	mRoleMask = _other->mRoleMask;

	for ( int i = 0; i < NUM_TRACK_CATS; ++i )
		mMaxUsers[ i ] = _other->mMaxUsers[ i ];

	// Copy goal priorities
	for ( int t = 0; t < ClassPriority::MaxTeams; ++t )
	{
		for ( int c = 0; c < ClassPriority::MaxClasses; ++c )
		{
			mClassPriority.Priorities[ t ][ c ] = _other->mClassPriority.Priorities[ t ][ c ];
		}
	}
	
	mVersion = _other->mVersion;
	
	mCallback_OnInit = _other->mCallback_OnInit;
	mCallback_OnUpgrade = _other->mCallback_OnUpgrade;
	mCallback_OnRender = _other->mCallback_OnRender;
	mCallback_OnSerialize = _other->mCallback_OnSerialize;
	mCallback_OnSetProperty = _other->mCallback_OnSetProperty;
	mCallback_OnHelp = _other->mCallback_OnHelp;
	mCallback_OnUpdate = _other->mCallback_OnUpdate;
	mCallback_OnHudDisplay = _other->mCallback_OnHudDisplay;
	mCallback_OnCalcPriority = _other->mCallback_OnCalcPriority;

	gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
	gmBind2::Class<MapGoal>::CloneTable( pMachine, _other->GetScriptObject( pMachine ), GetScriptObject( pMachine ) );

	// copy select flags.
	PROPERTY_PROPOGATE( DefaultDrawFlags );
	PROPERTY_PROPOGATE( RenderHeight );
	PROPERTY_PROPOGATE( DefaultRenderRadius );
	PROPERTY_PROPOGATE( DisableWithEntityFlag );
	PROPERTY_PROPOGATE( DeleteWithEntityFlag );
	PROPERTY_PROPOGATE( DisableForControllingTeam );
	PROPERTY_PROPOGATE( DynamicPosition );
	PROPERTY_PROPOGATE( DynamicOrientation );
	PROPERTY_PROPOGATE( RemoveWithEntity );
	PROPERTY_PROPOGATE( DontSave );
	PROPERTY_PROPOGATE( CreateOnLoad );
}

void MapGoal::_Init()
{
	mAvailableTeams.ClearAll();

	mPosition = Vector3f::ZERO;
	mInterfacePosition = Vector3f::ZERO;
	mOrientation = Matrix3f::IDENTITY;
	mLocalBounds = AABB( Vector3f::ZERO, Vector3f::ZERO );
	mRadius = 0.f;
	mMinRadius = 0.f;
	mNavFlags = 0;
	mSerialNum = 0;
	mDefaultPriority = 1.f;
	mRolePriorityBonus = 0.f;
	mRandomUsePoint = 0;
	mRange = 0;

	mSerialNum = GetMapGoalSerial();

	// By default, max out the counters.
	for ( int i = 0; i < NUM_TRACK_CATS; ++i )
		mMaxUsers[ i ] = 10000;

	ResetGoalPriorities();

	mRoleMask = BitFlag32( 0 );

	mGoalState = 0;

	mVersion = 0;

	mControllingTeam = 0;

	mNeedsSynced = false;

	//////////////////////////////////////////////////////////////////////////
	// init flags
	PROPERTY_INIT( DefaultDrawFlags, DrawAll );
	PROPERTY_INIT( DefaultRenderRadius, 2048.f );
	PROPERTY_INIT( DeleteMe, false );
	PROPERTY_INIT( DynamicPosition, false );
	PROPERTY_INIT( DynamicOrientation, false );
	PROPERTY_INIT( PropertiesBound, false );
	PROPERTY_INIT( RemoveWithEntity, true );
	PROPERTY_INIT( InterfaceGoal, false );
	PROPERTY_INIT( Disabled, false );
	PROPERTY_INIT( InUse, false );
	PROPERTY_INIT( DisableForControllingTeam, false );
	PROPERTY_INIT( DontSave, false );
	PROPERTY_INIT( RenderGoal, false );
	PROPERTY_INIT( RenderRoutes, false );
	PROPERTY_INIT( CreateOnLoad, true );
}

MapGoal::~MapGoal()
{
	//IGameManager::GetInstance()->SyncRemoteDelete( va( "MapGoal:%s", GetName().c_str() ) );

	gmBind2::Class<MapGoal>::NullifyUserObject( mScriptObject );
}

gmGCRoot<gmUserObject> MapGoal::GetScriptObject( gmMachine *_machine ) const
{
	if ( !mScriptObject )
		mScriptObject = gmBind2::Class<MapGoal>::WrapObject( _machine, const_cast<MapGoal*>( this ), true );
	return mScriptObject;
}

MapGoalPtr MapGoal::GetSmartPtr()
{
	MapGoalPtr ptr = mWeakPtr.lock();
	return ptr;
}

void MapGoal::SetSmartPtr( MapGoalPtr ptr )
{
	mWeakPtr = ptr;
}

bool MapGoal::LoadFromFile( const filePath & _file )
{
	gmGCRoot<gmUserObject> mgref = GetScriptObject( ScriptManager::GetInstance()->GetMachine() );
	gmVariable varThis( mgref );

	int iThreadId;
	if ( ScriptManager::GetInstance()->ExecuteFile( _file, iThreadId, &varThis ) )
	{
		return true;
	}

	return false;
}

void MapGoal::GenerateName( int _instance, bool _skipdupecheck )
{
	int32_t iNavId = gEngineFuncs->IDFromEntity( GetEntity() );
	if ( mTagName.empty() )
	{
		System::mInstance->mNavigation->GetNavInfo( GetPosition(), iNavId, mTagName );
	}

	std::string gtype = GetGoalType();
	std::transform( gtype.begin(), gtype.end(), gtype.begin(), toUpper() );

	if ( !mTagName.empty() )
		mName = va( "%s_%s", gtype.c_str(), mTagName.c_str() ).c_str();
	else
	{
		mName = va( "%s_%d", gtype.c_str(), iNavId ).c_str();
	}
	boost::replace_all( mName, " ", "_" );

	//////////////////////////////////////////////////////////////////////////
	// Dupe name handling, append an instance number
	if ( _instance > 0 )
	{
		mName += va( "_%d", _instance ).c_str();
	}

	// see if it already exists
	if ( !_skipdupecheck )
	{
		MapGoalPtr exists = System::mInstance->mGoalManager->GetGoal( mName );
		if ( exists && exists.get() != this )
		{
			GenerateName( _instance + 1 );
		}
	}
	CheckForPersistentPriority();
}

bool MapGoal::IsAvailable( int _team ) const
{
	if ( GetDisabled() )
		return false;
	if ( GetDisableForControllingTeam() && mControllingTeam == _team )
		return false;
	return mAvailableTeams.CheckFlag( _team ) && !GetDeleteMe();
}

void MapGoal::SetAvailable( int _team, bool _available )
{
	if ( _team == 0 )
	{
		for ( int t = 1; t <= 4; ++t )
			mAvailableTeams.SetFlag( t, _available );
	}
	else
	{
		mAvailableTeams.SetFlag( _team, _available );
	}
}

bool MapGoal::IsAvailableInitial( int _team ) const
{
	return mAvailableTeamsInit.CheckFlag( _team );
}

void MapGoal::SetAvailableInitial( int _team, bool _available )
{
	if ( _team == 0 )
	{
		for ( int t = 1; t <= 4; ++t )
			mAvailableTeamsInit.SetFlag( t, _available );
	}
	else
	{
		mAvailableTeamsInit.SetFlag( _team, _available );
	}
}

void MapGoal::Update()
{
	{
		//////////////////////////////////////////////////////////////////////////
		if ( GetEntity().IsValid() )
		{
			IGame::GetEntityInfo( GetEntity(), mEntInfo );

			if ( GetRemoveWithEntity() )
			{
				if ( !IGame::IsEntityValid( GetEntity() ) )
				{
					SetDeleteMe( true );
					return;
				}
			}
			if ( GetDeleteWithEntityFlag().AnyFlagSet() )
			{
				if ( ( mEntInfo.mFlags & GetDeleteWithEntityFlag() ).AnyFlagSet() )
				{
					SetDeleteMe( true );
					return;
				}
			}
			if ( GetDisableWithEntityFlag().AnyFlagSet() )
			{
				if ( ( mEntInfo.mFlags & GetDisableWithEntityFlag() ).AnyFlagSet() )
				{
					SetDisabled( true );
				}
				else
				{
					SetDisabled( false );
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////
		_CheckControllingTeam();
		//////////////////////////////////////////////////////////////////////////

		if ( mEntInfo.mGroup == ENT_GRP_FLAG )
			_UpdateFlagState();
				
		//////////////////////////////////////////////////////////////////////////
		if ( mCallback_OnUpdate )
		{
			if ( !mActiveThread[ ON_UPDATE ].IsActive() )
			{
				gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
				gmCall call;
				gmGCRoot<gmUserObject> mgref = GetScriptObject( pMachine );
				if ( call.BeginFunction( pMachine, mCallback_OnUpdate, gmVariable( mgref ) ) )
				{
					if ( call.End() == gmThread::EXCEPTION )
					{
					}

					mActiveThread[ ON_UPDATE ] = call.GetThreadId();
					if ( call.DidReturnVariable() )
						mActiveThread[ ON_UPDATE ] = 0;
				}
			}
		}
	}
}

void MapGoal::_UpdateFlagState()
{
	GameEntity owner;
	FlagState newFlagState;
	if ( gEngineFuncs->GetFlagState( GetEntity(), newFlagState, owner ) )
	{
		SetOwner( owner );

		if ( newFlagState != mGoalState )
		{
			const char *pFlagState = 0;
			switch ( newFlagState )
			{
				case S_FLAG_NOT_A_FLAG:
					pFlagState = "deleted";
					break;
				case S_FLAG_AT_BASE:
					pFlagState = "returned";
					break;
				case S_FLAG_DROPPED:
					pFlagState = "dropped";
					break;
				case S_FLAG_CARRIED:
					pFlagState = "pickedup";
					break;
				case S_FLAG_UNAVAILABLE:
					pFlagState = "unavailable";
					break;
				case S_FLAG_UNKNOWN:
					pFlagState = "unknown";
					break;
			}

			if ( pFlagState )
			{
				TriggerInfo ti;
				ti.mEntity = GetEntity();
				ti.mActivator = owner;
				Utils::VarArgs( ti.mTagName, TriggerBufferSize, "Flag %s %s", GetTagName().c_str(), pFlagState );
				Utils::StringCopy( ti.mAction, pFlagState, TriggerBufferSize );
				TriggerManager::GetInstance()->HandleTrigger( ti );
			}

			mGoalState = newFlagState;
		}
	}
}

//void MapGoal::_UpdateFlagHoldState()
//{
//	if(GetEntity().IsValid())
//	{
//		const int newControllingTeam = gEngineFuncs->GetControllingTeam(GetEntity());
//		if(newControllingTeam != mGoalState)
//		{
//			TriggerInfo ti;
//			ti.mEntity = GetEntity();
//			ti.mActivator = GameEntity();
//			varArgs(ti.mTagName,
//				TriggerBufferSize,
//				"%s to team %d",
//				GetName().c_str(),
//				newControllingTeam);
//			Utils::StringCopy(ti.mAction, "controlling team", TriggerBufferSize);
//			TriggerManager::GetInstance()->HandleTrigger(ti);
//
//			mGoalState = newControllingTeam;
//		}
//	}
//}

void MapGoal::_CheckControllingTeam()
{
	if ( GetEntity().IsValid() )
	{
		const int newControllingTeam = gEngineFuncs->GetControllingTeam( GetEntity() );
		if ( newControllingTeam != mControllingTeam )
		{
			mControllingTeam = newControllingTeam;

			TriggerInfo ti;
			ti.mEntity = GetEntity();
			ti.mActivator = GameEntity();
			Utils::VarArgs( ti.mTagName,
				TriggerBufferSize,
				"%s to team %d",
				GetName().c_str(),
				newControllingTeam );
			Utils::StringCopy( ti.mAction, "controlling team", TriggerBufferSize );
			TriggerManager::GetInstance()->HandleTrigger( ti );
		}
	}
}

void MapGoal::InternalInitEntityState()
{
	// cache the values.
	if ( GetEntity().IsValid() )
	{
		bool b1 = EngineFuncs::EntityLocalAABB( GetEntity(), mLocalBounds );
		bool b2 = EngineFuncs::EntityPosition( GetEntity(), mPosition );
		
		b1; b2;

		// cache the auto detected position
		if ( b2 ) mInterfacePosition = mPosition;
		
		Vector3f vFwd, vRight, vUp;
		bool b3 = EngineFuncs::EntityOrientation( GetEntity(), vFwd, vRight, vUp );
		if ( b3 ) mOrientation = Matrix3f( vRight, vFwd, vUp, true );

	}

	if ( mLocalBounds.IsZero() )
	{
		mLocalBounds.Expand( 5.f );
	}
}

bool MapGoal::InternalInit( gmGCRoot<gmTableObject> &_propmap, bool _newgoal )
{
	CheckPropertiesBound();

	//////////////////////////////////////////////////////////////////////////
	gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();

	//////////////////////////////////////////////////////////////////////////
	if ( _newgoal )
	{
		if ( mCallback_OnInit )
		{
			gmCall call;

			gmGCRoot<gmUserObject> mgref = GetScriptObject( pMachine );
			if ( call.BeginFunction( pMachine, mCallback_OnInit, gmVariable( mgref ) ) )
			{
				call.AddParamTable( _propmap );
				call.End();
			}
		}
	}
	else
	{
		if ( mCallback_OnUpgrade )
		{
			while ( true )
			{
				gmCall call;

				gmGCRoot<gmUserObject> mgref = GetScriptObject( pMachine );
				if ( call.BeginFunction( pMachine, mCallback_OnUpgrade, gmVariable( mgref ) ) )
				{
					gmVariable gmVersionBefore = _propmap->Get( pMachine, "Version" );
					call.AddParamTable( _propmap );
					const int ThreadState = call.End();
					gmVariable gmVersionAfter = _propmap->Get( pMachine, "Version" );

					//meh
					_propmap->Set( pMachine, "OldType", gmVariable::s_null );

					if ( !gmVersionBefore.IsInt() ||
						!gmVersionAfter.IsInt() ||
						ThreadState == gmThread::EXCEPTION )
					{
						EngineFuncs::ConsoleMessage( va( "%s goal could not upgrade properly, disabling",
							GetName().c_str() ).c_str() );
						SetDisabled( true );
						return false;
					}
					else if ( gmVersionBefore.GetInt() != gmVersionAfter.GetInt() )
					{
						//EngineFuncs::ConsoleMessage("%s goal updated, version %d to %d",
						//	GetName().c_str(),gmVersionBefore.GetInt(),gmVersionAfter.GetInt());
					}
					else if ( gmVersionAfter.GetInt() == mVersion )
					{
						//EngineFuncs::ConsoleMessage("%s goal up to date, version %d",
						//	GetName().c_str(),gmVersionAfter.GetInt());
						break;
					}
					else
					{
						EngineFuncs::ConsoleMessage( va( "%s goal could not upgrade properly, disabling",
							GetName().c_str() ).c_str() );
						SetDisabled( true );
						return false;
					}
				}
			}
		}
	}
	return true;
}

void MapGoal::SetPosition( const Vector3f &_pos )
{
	mPosition = _pos;
}

const Vector3f &MapGoal::GetPosition()
{
	if ( GetDynamicPosition() )
	{
		EngineFuncs::EntityPosition( GetEntity(), mPosition );
	}
	return mPosition;
}

void MapGoal::SetPosition_Script( const Vec3 &_pos )
{
	mPosition = Vector3f( _pos.x, _pos.y, _pos.z );
}

Vec3 MapGoal::GetPosition_Script()
{
	return Vec3( GetPosition() );
}

void MapGoal::SetRange_Script( const int &_range )
{
	mRange = _range;
}

int MapGoal::GetRange_Script()
{
	return GetRange();
}

void MapGoal::SetFacing( const Vector3f &_facing )
{
	mOrientation = Matrix3f( _facing.Cross( Vector3f::UNIT_Z ), _facing, Vector3f::UNIT_Z, true );
}

void MapGoal::SetFacing_Script( const Vec3 &_facing )
{
	Vector3f facing( _facing.x, _facing.y, _facing.z );
	mOrientation = Matrix3f(
		facing.Cross( Vector3f::UNIT_Z ),
		facing,
		Vector3f::UNIT_Z, true );
}

Vector3f MapGoal::GetFacing()
{
	return GetMatrix().GetColumn( 1 );
}

Vec3 MapGoal::GetFacing_Script()
{
	return Vec3( GetFacing() );
}

void MapGoal::SetMatrix( const Matrix3f &_mat )
{
	mOrientation = _mat;
}

Matrix3f MapGoal::GetMatrix()
{
	if ( GetDynamicOrientation() )
	{
		Vector3f vFwd, vRight, vUp;
		bool b = EngineFuncs::EntityOrientation( GetEntity(), vFwd, vRight, vUp );
		if ( b ) mOrientation = Matrix3f( vRight, vFwd, vUp, false );
	}
	return mOrientation;
}

void MapGoal::SetGoalBounds( const AABB &_bounds )
{
	mLocalBounds = _bounds;
}

void MapGoal::SetBounds_Script( const Vec3 &_mins, const Vec3 &_maxs )
{
	for ( int i = 0; i < 3; ++i )
	{
		mLocalBounds.mMins[ i ] = _mins[ i ];
		mLocalBounds.mMaxs[ i ] = _maxs[ i ];
	}
}

Vec3 MapGoal::GetBoundsCenter_Script()
{
	Box3f box = GetWorldBounds();
	return Vec3( box.Center );
}

Box3f MapGoal::GetWorldBounds()
{
	Box3f obb;
	obb.Identity( 8.f );
	obb.Center = GetPosition();
	EngineFuncs::EntityOrientation( GetEntity(), obb.Axis[ 0 ], obb.Axis[ 1 ], obb.Axis[ 2 ] );
	EngineFuncs::EntityWorldOBB( GetEntity(), obb );
	return obb;
}

const AABB &MapGoal::GetLocalBounds() const
{
	return mLocalBounds;
}

void MapGoal::AddUsePoint( const Vector3f &_pos, bool _relative )
{
	mLocalUsePoints.resize( mLocalUsePoints.size() + 1 );
	mLocalUsePoints[ mLocalUsePoints.size() - 1 ] = _pos;

	mRelativeUsePoints.resize( mLocalUsePoints.size() );
	mRelativeUsePoints.set( mRelativeUsePoints.size() - 1, _relative );
}

Vector3f MapGoal::GetWorldUsePoint( int32_t _index )
{
	if ( !mLocalUsePoints.empty() )
	{
		if ( _index == -1 || _index < 0 || _index >= (int32_t)mLocalUsePoints.size() )
		{
			int iRand = Mathf::IntervalRandomInt( 0, (int)mLocalUsePoints.size() );

			Vector3f vUsePt = mLocalUsePoints[ iRand ];

			if ( mRelativeUsePoints.test( iRand ) )
			{
				vUsePt = GetPosition() + GetMatrix() * vUsePt;
			}
			return vUsePt;
		}

		Vector3f vUsePt = mLocalUsePoints[ _index ];

		if ( mRelativeUsePoints.test( _index ) )
		{
			vUsePt = GetPosition() + GetMatrix() * vUsePt;
		}
		return vUsePt;
	}
	return GetPosition();
}

void MapGoal::GetAllUsePoints( Vector3List &_pv )
{
	_pv.reserve( GetNumUsePoints() );
	for ( int i = 0; i < GetNumUsePoints(); ++i )
	{
		_pv.push_back( GetWorldUsePoint( i ) );
	}
}

bool MapGoal::AddRoute_Script( const std::string &_start, const std::string &_end, float _weight )
{
	MapGoalPtr mgStart = System::mInstance->mGoalManager->GetGoal( _start );
	MapGoalPtr mgEnd = System::mInstance->mGoalManager->GetGoal( _end );
	return AddRoute( mgStart, mgEnd, _weight );
}

bool MapGoal::AddRoute( const MapGoalPtr &_routeStart, const MapGoalPtr &_midpt, float _weight )
{
	if ( _routeStart && _midpt )
	{
		// find whether this route already exists
		Routes::const_iterator cIt = mRoutes.begin(), cItEnd = mRoutes.end();
		for ( ; cIt != cItEnd; ++cIt )
		{
			const Route &o = ( *cIt );
			if ( o.mStart == _routeStart && o.mEnd == _midpt )
				return true;
		}

		Route r;
		r.mStart = _routeStart;
		r.mEnd = _midpt;
		r.mWeight = _weight;

		mRoutes.reserve( mRoutes.size() + 1 );
		mRoutes.push_back( r );
		return true;
	}
	return false;
}

bool MapGoal::RouteTo( Client *_bot, DestinationVector &_dest, float _minradius )
{
	Routes routes;

	float fTolerance = _bot->GetWorldBounds().Extent[ 2 ];

	float fTotalWeight = 0.f;
	Routes::const_iterator cIt = mRoutes.begin(), cItEnd = mRoutes.end();
	for ( ; cIt != cItEnd; ++cIt )
	{
		const Route &r = ( *cIt );

		// RouteTo can be called when a bot is not yet within ROUTE goal radius,
		// because FollowPath calculates 2D distance (that is less than 3D distance).
		// Omni-bot 0.81 compared 3D distance here and skipped ROUTE goals on stairs, hillsides etc.
		Vector3f vDist = _bot->GetPosition() - r.mStart->GetPosition();
		if ( vDist.X() * vDist.X() + vDist.Y() * vDist.Y() <= Mathf::Sqr( r.mStart->GetRadius() ) &&
			abs( vDist.Z() - fTolerance ) - 2 * fTolerance <= r.mStart->GetRadius() &&

			// Most maps have all routes enabled for all teams,
			// that's why it's better to check availability after checking radius.
			r.mEnd->IsAvailable( _bot->GetTeam() ) &&
			r.mStart->IsAvailable( _bot->GetTeam() ) )
		{
			routes.push_back( r );
			fTotalWeight += r.mWeight;
		}
	}

	if ( !routes.empty() )
	{
		int iIndex = (int)routes.size() - 1;

		float fWght = Mathf::IntervalRandom( 0.f, fTotalWeight );
		for ( uint32_t i = 0; i < routes.size(); ++i )
		{
			fWght -= routes[ i ].mWeight;
			if ( fWght <= 0.f )
			{
				iIndex = i;
				break;
			}
		}

		Destination d;
		d.mPosition = routes[ iIndex ].mEnd->GetPosition();
		d.mRadius = std::max( routes[ iIndex ].mEnd->GetRadius(), _minradius );
		_dest.push_back( d );

		return true;
	}

	if ( GetNumUsePoints() > 0 )
	{
		if ( mRandomUsePoint )
		{
			int iRand = Mathf::IntervalRandomInt( 0, (int)GetNumUsePoints() );
			Destination d;
			d.mPosition = GetWorldUsePoint( iRand );
			d.mRadius = std::max( GetRadius(), _minradius );
			_dest.push_back( d );
		}
		else
		{
			for ( int32_t i = 0; i < GetNumUsePoints(); ++i )
			{
				Destination d;
				d.mPosition = GetWorldUsePoint( i );
				d.mRadius = std::max( GetRadius(), _minradius );
				_dest.push_back( d );
			}
		}
	}
	else
	{
		Destination d;
		d.mPosition = GetWorldUsePoint();
		d.mRadius = std::max( GetRadius(), _minradius );
		_dest.push_back( d );
	}
	return false;
}

void MapGoal::SetProperty( const std::string &_propname, const obUserData &_val )
{
	gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();

	DisableGCInScope gcEn( pMachine );

	gmVariable var = Utils::UserDataToGmVar( pMachine, _val );

	bool Processed = false;

	std::stringstream err;

	if ( !Processed )
	{
		if ( _val.IsString() )
		{
			PropertyMap pm;
			pm.AddProperty( _propname, _val.GetString() );
			Processed = FromPropertyMap( pm, err );
		}
	}

	if ( !Processed )
	{
		Processed = FromScriptVar( pMachine, _propname.c_str(), var, err );
	}

	if ( !Processed )
	{
		if ( mCallback_OnSetProperty )
		{
			gmGCRoot<gmUserObject> mgref = GetScriptObject( pMachine );

			gmCall call;
			if ( call.BeginFunction( pMachine, mCallback_OnSetProperty, gmVariable( mgref ) ) )
			{
				call.AddParamString( _propname.c_str() );
				call.AddParam( var );
				call.End();
			}
		}
	}

	// rebuild the name
	GenerateName( 0 );

	if ( !Processed && !err.str().empty() )
		EngineFuncs::ConsoleError( va( "%s", err.str().c_str() ).c_str() );
}

void MapGoal::RenderDebug( bool _editing, bool _highlighted )
{
	{
		if ( GetRenderGoal() )
		{
			if ( mCallback_OnRender )
			{
				if ( !mActiveThread[ ON_RENDER ].IsActive() )
				{
					gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();

					gmGCRoot<gmUserObject> mgref = GetScriptObject( pMachine );

					gmCall call;
					if ( call.BeginFunction( pMachine, mCallback_OnRender, gmVariable( mgref ) ) )
					{
						call.AddParamInt( _editing ? 1 : 0 );
						call.AddParamInt( _highlighted ? 1 : 0 );
						if ( call.End() == gmThread::EXCEPTION )
						{
							/*SetEnable(false, va("Error in Update Callback in Goal: %s", GetName().c_str()));
							return State_Finished;*/
						}/**/

						mActiveThread[ ON_RENDER ] = call.GetThreadId();
						if ( call.DidReturnVariable() )
							mActiveThread[ ON_RENDER ] = 0;
					}
				}
			}
			else
			{
				RenderDefault();
			}
		}
		else
		{
			mActiveThread[ ON_RENDER ].Reset();
		}

		//////////////////////////////////////////////////////////////////////////
		if ( GetRenderRoutes() )
		{
			DrawRoute( COLOR::GREEN, 2.f );
		}
	}
}

void MapGoal::RenderDefault()
{
	const BitFlag32 bf( GetDefaultDrawFlags() );

	Vector3f vRenderPos = GetPosition();
	obColor vRenderColor = COLOR::GREEN; // customize?

	if ( bf.CheckFlag( DrawCenterBounds ) && GetEntity().IsValid() )
	{
		vRenderPos = GetWorldBounds().Center;
	}

	vRenderPos.Z() += GetRenderHeight();

	Vector3f vLocalPos, vLocalFace;
	if ( !Utils::GetLocalFacing( vLocalFace ) || !Utils::GetLocalPosition( vLocalPos ) )
		return;

	//////////////////////////////////////////////////////////////////////////
	if ( GetDefaultRenderRadius() < Utils::FloatMax )
	{
		if ( Length( vLocalPos, vRenderPos ) > GetDefaultRenderRadius() )
		{
			return;
		}
	}
	//////////////////////////////////////////////////////////////////////////

	std::string txtOut;

	if ( bf.CheckFlag( DrawName ) )
	{
		txtOut += GetName();
		txtOut += "\n";
	}
	if ( bf.CheckFlag( DrawGroup ) )
	{
		if ( !GetGroupName().empty() )
		{
			txtOut += " Group: ";
			txtOut += GetGroupName();
			txtOut += "\n";
		}
	}
	if ( bf.CheckFlag( DrawRole ) )
	{
		const std::string strRole = Utils::BuildRoleName( GetRoleMask().GetRawFlags() );
		if ( strRole != "None" )
		{
			txtOut += "Roles: ";
			txtOut += strRole;
			txtOut += "\n";
		}
	}
	if ( bf.CheckFlag( DrawInitialAvail ) )
	{
		// goals created by goal_create command are always initially enabled for all teams
		if ( mAvailableTeamsInit.GetRawFlags() != 30 || !GetCreateOnLoad() )
		{
			txtOut += "Initial: ";
			txtOut += Utils::GetTeamString( mAvailableTeamsInit.GetRawFlags() );
			txtOut += "\n";
		}
	}
	if ( bf.CheckFlag( DrawCurrentAvail ) )
	{
		txtOut += "Active: ";
		txtOut += Utils::GetTeamString( mAvailableTeams.GetRawFlags() );
		txtOut += "\n";
	}
	if ( bf.CheckFlag( DrawRandomUsePoint ) )
	{
		int rup = GetRandomUsePoint();
		if ( rup > 0 )
		{
			txtOut += "RandomUsePoint: ";
			txtOut += std::string( va( "%i", rup ).c_str() );
			txtOut += "\n";
		}
	}
	if ( bf.CheckFlag( DrawRangeLimit ) )
	{
		int range = GetRange();
		if ( range > 0 )
		{
			txtOut += "Range: ";
			txtOut += std::string( va( "%i", range ).c_str() );
			txtOut += "\n";
		}
	}

	// bounds
	if ( bf.CheckFlag( DrawBounds ) )
	{
		/*if(mbUseOrientedBox)
		RenderBuffer::AddOBB(GetPosition(),GetMatrix(),mOrientedBounds,COLOR::MAGENTA);
		else*/
		RenderBuffer::AddOBB( GetWorldBounds(), COLOR::ORANGE );
	}

	// radius
	if ( bf.CheckFlag( DrawRadius ) )
	{
		if ( GetRadius() != 0.f )
			RenderBuffer::AddCircle( GetPosition(), GetRadius(), COLOR::ORANGE );
		else
			RenderBuffer::AddLine( GetPosition(), GetPosition() + Vector3f::UNIT_Z * 32.f, COLOR::ORANGE );
	}

	// use pts
	for ( int i = 0; i < GetNumUsePoints(); ++i )
	{
		const Vector3f vUsePt = GetWorldUsePoint( i );
		RenderBuffer::AddLine( vUsePt, vUsePt + Vector3f::UNIT_Z * 32.f, COLOR::GREEN );
	}

	if ( bf.CheckFlag( DrawDisabled ) )
	{
		if ( GetDisabled() )
		{
			txtOut += "DISABLED";
			txtOut += "\n";
			vRenderColor = COLOR::RED;
		}
	}

	if ( mExtraDebugText )
	{
		gmStringObject *so = mExtraDebugText;
		const char *str = so->GetString();
		if ( str )
		{
			txtOut += str;
			txtOut += "\n";
		}
	}

	RenderBuffer::AddString3d( vRenderPos, vRenderColor, txtOut.c_str() );
}

Vector3f MapGoal::CalculateFarthestFacing()
{
	const float fRayLength = 5000.0f;

	float fBestVectorLength = 0.0f;
	Vector3f vBestVector = Vector3f::UNIT_Z;

	obTraceResult tr;

	// Calculate the farthest looking vector from this point, at 5 degree intervals.
	static float fInterval = 5.0f;
	for ( float fAng = fInterval; fAng <= 360.0f; fAng += fInterval )
	{
		Quaternionf quat( Vector3f::UNIT_Z, Mathf::DEG_TO_RAD * fAng );
		Vector3f vVec = quat.Rotate( Vector3f::UNIT_Y*fRayLength );
		EngineFuncs::TraceLine( tr, GetPosition(), GetPosition() + vVec, NULL, TR_MASK_SHOT, -1, false );
		float fSightLength = tr.mFraction * fRayLength;
		if ( fSightLength > fBestVectorLength )
		{
			fBestVectorLength = fSightLength;
			vBestVector = vVec;
		}
	}
	vBestVector.Normalize();
	return vBestVector;
}

void MapGoal::CheckPropertiesBound()
{
	if ( !GetPropertiesBound() )
	{
		BindProperties();
		SetPropertiesBound( true );
	}
}

void MapGoal::BindProperties()
{
	BindProperty( "Name", mName, Prop::PF_READONLY );
	BindProperty( "TagName", mTagName );
	BindProperty( "Group", mGroupName );
	BindProperty( "Type", mGoalType );
	BindProperty( "Entity", mEntity );
	BindProperty( "SerialNum", mSerialNum );
	BindProperty( "Priority", mDefaultPriority );
	BindProperty( "Radius", mRadius );
	BindProperty( "RandomUsePoint", mRandomUsePoint );
	BindProperty( "Range", mRange );

	{
		int EnumSize = 0;
		const IntEnum *Enum = 0;
		System::mInstance->mGame->GetTeamEnumeration( Enum, EnumSize );
		BindProperty( "Available", mAvailableTeamsInit, 0, Enum, EnumSize );
	}

	{
		int EnumSize = 0;
		const IntEnum *Enum = 0;
		System::mInstance->mGame->GetRoleEnumeration( Enum, EnumSize );
		BindProperty( "Roles", mRoleMask, 0, Enum, EnumSize );
	}

	BindProperty( "Position", mPosition, Prop::PF_POSITION );
	//BindProperty("Facing",mFacing,Prop::PF_FACING);
	//BindFunction("DebugRender",this,&MapGoal::ToggleRender);
}

void MapGoal::ResetGoalPriorities()
{
	for ( int t = 0; t < ClassPriority::MaxTeams; ++t )
	{
		for ( int c = 0; c < ClassPriority::MaxClasses; ++c )
		{
			mClassPriority.Priorities[ t ][ c ] = -1.f;
		}
	}
}

void MapGoal::SetPriorityForClass( int _teamid, int _classId, float _priority )
{
	//////////////////////////////////////////////////////////////////////////
	if ( _teamid )
		_teamid = ( 1 << _teamid );
	else
		_teamid = -1;

	if ( _classId )
		_classId = ( 1 << _classId );
	else
		_classId = -1;

	//////////////////////////////////////////////////////////////////////////
	for ( int t = 1; t < ClassPriority::MaxTeams; ++t )
	{
		for ( int c = 1; c < ClassPriority::MaxClasses; ++c )
		{
			if ( ( 1 << t )&_teamid && ( 1 << c )&_classId )
			{
				mClassPriority.Priorities[ t - 1 ][ c - 1 ] = _priority;
			}
		}
	}
}

float MapGoal::GetPriorityForClient( Client *_client )
{
	if ( mCallback_OnCalcPriority )
	{
		if ( !mActiveThread[ ON_CALC_PRIORITY ].IsActive() )
		{
			gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
			gmCall call;
			gmGCRoot<gmUserObject> mgref = GetScriptObject( pMachine );
			if ( call.BeginFunction( pMachine, mCallback_OnCalcPriority, gmVariable( mgref ) ) )
			{
				if ( call.End() == gmThread::EXCEPTION )
				{
				}

				mActiveThread[ ON_CALC_PRIORITY ] = call.GetThreadId();
				if ( call.DidReturnVariable() )
					mActiveThread[ ON_CALC_PRIORITY ] = 0;
			}
		}
	}
		

	float prio = GetPriorityForClass( _client->GetTeam(), _client->GetClass() );
	if ( prio > 0.f && GetRoleMask().AnyFlagSet() )
	{
		if ( ( GetRoleMask() & _client->GetRoleMask() ).AnyFlagSet() )
		{
			prio += mRolePriorityBonus;
		}
	}
	return prio;
}

float MapGoal::GetPriorityForClass( int _teamid, int _classId )
{
	if ( _teamid > 0 && _teamid < ClassPriority::MaxTeams &&
		_classId>0 && _classId < ClassPriority::MaxClasses )
	{
		float classPrio = mClassPriority.Priorities[ _teamid - 1 ][ _classId - 1 ];
		if ( classPrio != -1.f )
		{
			return classPrio;
		}
	}
	return GetDefaultPriority();
}

//////////////////////////////////////////////////////////////////////////

struct PersistentPriority
{
	std::string	mExpression;
	int		mTeam;
	int		mClass;
	float	mPriority;
};

typedef std::vector<PersistentPriority> PersPriorityList;
PersPriorityList gPriorityList;

void MapGoal::ClassPriority::GetPriorityText( std::string &_txtout ) const
{
	//////////////////////////////////////////////////////////////////////////
	int CurrentIndex = 0;
	enum
	{
		MaxClassPriority = 32
	};
	struct PrioritySummary
	{
		int32_t		mTeamId;
		int32_t		mClassId;
		float		mPriority;
	};
	PrioritySummary Summary[ MaxClassPriority ];

	// Search all entries to build a list of summarized priority information
	for ( int t = 1; t <= ClassPriority::MaxTeams && CurrentIndex < MaxClassPriority - 1; ++t )
	{
		if ( !Utils::TeamExists( t ) )
			continue;

		for ( int c = 1; c <= ClassPriority::MaxClasses && CurrentIndex < MaxClassPriority - 1; ++c )
		{
			if ( !Utils::ClassExists( c ) )
				continue;

			// class and team ids start at 1
			const float CurrentPriority = Priorities[ t - 1 ][ c - 1 ];

			if ( CurrentPriority == -1.f )
				continue;

			bool bFound = false;

			// search for matching entry.
			for ( int i = 0; i < CurrentIndex; ++i )
			{
				if ( Summary[ i ].mPriority == CurrentPriority && Summary[ i ].mTeamId & ( 1 << t ) )
				{
					Summary[ i ].mTeamId |= ( 1 << t );
					Summary[ i ].mClassId |= ( 1 << c );
					bFound = true;
				}
			}

			if ( !bFound )
			{
				Summary[ CurrentIndex ].mPriority = CurrentPriority;
				Summary[ CurrentIndex ].mTeamId = ( 1 << t );
				Summary[ CurrentIndex ].mClassId = ( 1 << c );
				++CurrentIndex;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// print the output
	for ( int i = 0; i < CurrentIndex; ++i )
	{
		if ( Summary[ i ].mClassId && Summary[ i ].mTeamId )
		{
			std::string groupname, classname;
			System::mInstance->mGame->FindClassName( groupname, classname, ENT_GRP_PLAYER, Summary[ i ].mClassId );

			_txtout = "    "; // indent
			_txtout += Utils::GetTeamString( Summary[ i ].mTeamId );
			_txtout += " ";
			_txtout += Utils::GetClassString( Summary[ i ].mClassId );
			_txtout += " ";
			_txtout += va( " %.2f", Summary[ i ].mPriority ).c_str();
			EngineFuncs::ConsoleMessage( _txtout.c_str() );
		}
	}
}

void MapGoal::SetPersistentPriorityForClass( const std::string &_exp, int _team, int _class, float _priority )
{
	for ( uint32_t i = 0; i < gPriorityList.size(); ++i )
	{
		if ( _exp == gPriorityList[ i ].mExpression )
		{
			gPriorityList[ i ].mTeam = _team;
			gPriorityList[ i ].mClass = _class;
			gPriorityList[ i ].mPriority = _priority;
			return;
		}
	}

	PersistentPriority pp;
	pp.mExpression = _exp;
	pp.mTeam = _team;
	pp.mClass = _class;
	pp.mPriority = _priority;
	gPriorityList.push_back( pp );
}

void MapGoal::CheckForPersistentPriority()
{
	for ( uint32_t i = 0; i < gPriorityList.size(); ++i )
	{
		if ( Utils::RegexMatch( gPriorityList[ i ].mExpression.c_str(), GetName().c_str() ) )
		{
			SetPriorityForClass(
				gPriorityList[ i ].mTeam,
				gPriorityList[ i ].mClass,
				gPriorityList[ i ].mPriority );
			return;
		}
	}
}

void MapGoal::DrawRoute( const obColor _color, float _duration )
{
	PathInterface * path = System::mInstance->mNavigation->AllocPathInterface();

	if ( path != NULL )
	{
		for ( size_t i = 0; i < mRoutes.size(); ++i )
		{
			const Route &r = mRoutes[ i ];

			path->UpdateSourcePosition( r.mStart->GetPosition() );
			path->UpdateGoalPosition( r.mEnd->GetPosition(), 0.0f );
			path->UpdatePath( false );

			static const int MaxCorners = 256;
			PathInterface::PathCorner corners[ MaxCorners ];
			const size_t numCorners = path->GetPathCorners( corners, MaxCorners );
			for ( size_t i = 1; i < numCorners; ++i )
			{
				RenderBuffer::AddLine( corners[ i - 1 ].mPos, corners[ i ].mPos, _color );
			}
		}
	}

	delete path;
}

void MapGoal::DrawPathsToGoals()
{
	std::vector<GameEntity> validStarts;
	validStarts.reserve( 32 );

	IGame::EntityIterator it;
	while ( IGame::IterateEntity( it ) )
	{
		if ( it.GetEnt().mEntInfo.mGroup == ENT_GRP_PLAYERSTART )
			validStarts.push_back( it.GetEnt().mEntity );
	}

	for ( size_t i = 0; i < validStarts.size(); ++i )
	{
		Vector3f startPos;
		if ( EngineFuncs::EntityPosition( validStarts[ i ], startPos ) )
		{

		}
	}
}

void MapGoal::FromScriptTable( gmMachine *_machine, gmTableObject *_tbl, bool _caseSense )
{
	gmTableObject *mytbl = gmBind2::Class<MapGoal>::GetTable( GetScriptObject( _machine ) );
	if ( mytbl )
	{
		gmTableIterator tIt;
		gmTableNode *pNode = _tbl->GetFirst( tIt );
		while ( pNode )
		{
			gmTableNode * existingNode = !_caseSense ? mytbl->GetTableNode( _machine, pNode->m_key, false ) : 0;
			if ( existingNode )
			{
				existingNode->m_value = pNode->m_value;
			}
			else
			{
				mytbl->Set( _machine, pNode->m_key, pNode->m_value );
			}
			pNode = _tbl->GetNext( tIt );
		}
	}
}

gmVariable MapGoal::GetProperty( const char *_name )
{
	gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();

	gmTableObject *tbl = gmBind2::Class<MapGoal>::GetTable( GetScriptObject( pMachine ) );
	if ( tbl )
		return pMachine->Lookup( _name, tbl );
	return gmVariable::s_null;
}

bool MapGoal::GetProperty( const char *_name, Vector3f &_var )
{
	gmVariable gmVar = GetProperty( _name );
	return gmVar.GetVector( _var );
}

bool MapGoal::GetProperty( const char *_name, float &_var )
{
	gmVariable gmVar = GetProperty( _name );
	return gmVar.GetFloatSafe( _var, 0.f );
}

bool MapGoal::GetProperty( const char *_name, bool &_var )
{
	gmVariable gmVar = GetProperty( _name );
	int intvar = 0;
	if ( gmVar.GetIntSafe( intvar, 0 ) )
	{
		_var = intvar != 0;
		return true;
	}
	return false;
}
bool MapGoal::GetProperty( const char *_name, int &_var )
{
	gmVariable gmVar = GetProperty( _name );
	return gmVar.GetIntSafe( _var, 0 );
}

bool MapGoal::GetProperty( const char *_name, std::string &_var )
{
	gmVariable gmVar = GetProperty( _name );
	const char *cstr = gmVar.GetCStringSafe();
	if ( cstr )
		_var = cstr;

	return cstr != 0;
}
bool MapGoal::GetProperty( const char *_name, Seconds &_var )
{
	gmVariable gmVar = GetProperty( _name );
	float Secs = 0.f;
	if ( gmVar.GetFloatSafe( Secs, 0.f ) )
	{
		_var = Seconds( Secs );
		return true;
	}
	return false;
}

bool MapGoal::SaveToTable( gmMachine *_machine, gmGCRoot<gmTableObject> &_savetable, ErrorObj &_err )
{
	gmGCRoot<gmTableObject> GoalTable( _machine->AllocTableObject(), _machine );

	if ( mCallback_OnSerialize )
	{
		gmCall call;
		gmGCRoot<gmUserObject> mgref = GetScriptObject( _machine );
		if ( call.BeginFunction( _machine, mCallback_OnSerialize, gmVariable( mgref ) ) )
		{
			call.AddParamTable( GoalTable );
			const int ThreadState = call.End();
			if ( ThreadState != gmThread::KILLED )
			{
				_err.AddError( "Error Calling Script Serialize function!" );
				return false;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Standard info.
	GoalTable->Set( _machine, "Version", gmVariable( mVersion ) );
	GoalTable->Set( _machine, "GoalType", GetGoalType().c_str() );
	GoalTable->Set( _machine, "Name", GetName().c_str() );
	GoalTable->Set( _machine, "TagName", GetTagName().c_str() );
	GoalTable->Set( _machine, "GroupName", GetGroupName().c_str() );
	GoalTable->Set( _machine, "Position", gmVariable( mInterfacePosition.IsZero() ? mPosition : mInterfacePosition ) );
	GoalTable->Set( _machine, "Radius", gmVariable( mRadius ) );
	GoalTable->Set( _machine, "MinRadius", gmVariable( mMinRadius ) );

	// There's no reason to save SerialNum, because function LoadFromTable never loaded SerialNum from file.
	//GoalTable->Set(_machine,"SerialNum",gmVariable(mSerialNum));

	GoalTable->Set( _machine, "CreateOnLoad", gmVariable( mCreateOnLoad ) );
	if ( mRandomUsePoint ) GoalTable->Set( _machine, "RandomUsePoint", gmVariable( mRandomUsePoint ) );
	if ( mRange ) GoalTable->Set( _machine, "Range", gmVariable( mRange ) );

	/*gmGCRoot<gmUserObject> userBounds = gmBind2::Class<BoundingBox>::WrapObject(_machine,&mBounds,true);
	GoalTable->Set(_machine,"Bounds",gmVariable(userBounds));*/

	Vector3f Euler( 0, 0, 0 );
	mOrientation.ExtractEulerZXY( Euler.X(), Euler.Y(), Euler.Z() );
	GoalTable->Set( _machine, "Orientation", gmVariable( Euler ) );
	//////////////////////////////////////////////////////////////////////////

	GoalTable->Set( _machine, "TeamAvailability", gmVariable( mAvailableTeamsInit.GetRawFlags() ) );

	GoalTable->Set( _machine, "Roles", gmVariable::s_null );
	if ( mRoleMask.AnyFlagSet() )
	{
		gmTableObject * roleTable = _machine->AllocTableObject();

		int NumElements = 0;
		const IntEnum *Enum = 0;
		System::mInstance->mGame->GetRoleEnumeration( Enum, NumElements );
		for ( int i = 0; i < 32; ++i )
		{
			if ( mRoleMask.CheckFlag( i ) )
			{
				for ( int e = 0; e < NumElements; ++e )
				{
					if ( Enum[ e ].mValue == i )
					{
						roleTable->Set( _machine, roleTable->Count(), Enum[ e ].mKey );
						break;
					}
				}
			}
		}
		GoalTable->Set( _machine, "Roles", gmVariable( roleTable ) );
	}

	//////////////////////////////////////////////////////////////////////////
	_savetable->Set( _machine, GetName().c_str(), gmVariable( GoalTable ) );
	return true;
}

bool MapGoal::LoadFromTable( gmMachine *_machine, gmGCRoot<gmTableObject> &_loadtable, ErrorObj &_err )
{
	gmGCRoot<gmTableObject> proptable;
	proptable.Set( _machine->AllocTableObject(), _machine );

	_loadtable->CopyTo( _machine, proptable );

	if ( const char *TagName = proptable->Get( _machine, "TagName" ).GetCStringSafe( 0 ) )
		mTagName = TagName;
	else
	{
		_err.AddError( "Goal.TagName Field Missing!" );
		return false;
	}
	if ( const char *Name = proptable->Get( _machine, "Name" ).GetCStringSafe( 0 ) )
		mName = Name;
	else
	{
		_err.AddError( "Goal.Name Field Missing!" );
		return false;
	}
	if ( const char *GroupName = proptable->Get( _machine, "GroupName" ).GetCStringSafe( 0 ) )
		mGroupName = GroupName;
	else
	{
		_err.AddError( "Goal.GroupName Field Missing!" );
		return false;
	}
	if ( !proptable->Get( _machine, "Version" ).GetInt( mVersion, 0 ) )
	{
		_err.AddError( "Goal.Version Field Missing!" );
		return false;
	}
	if ( !proptable->Get( _machine, "Position" ).GetVector( mPosition ) )
	{
		_err.AddError( "Goal.Position Field Missing!" );
		return false;
	}
	if ( !proptable->Get( _machine, "Radius" ).GetFloatSafe( mRadius ) )
	{
		_err.AddError( "Goal.Radius Field Missing!" );
		return false;
	}
	if ( !proptable->Get( _machine, "MinRadius" ).GetFloatSafe( mMinRadius ) )
	{
		_err.AddError( "Goal.MinRadius Field Missing!" );
		return false;
	}

	// Command /bot goal_create does not generate unique serial numbers !
	// If some goals are deleted, then newly created goals can have same serial number as existing goals.
	// Because serial numbers in the file are not unique, it would be very dangerous to load them.
	// Fortunatelly, omni-bot never loaded serial numbers from a file, because here is another bug.
	// Function GetIntSafe cannot change value of mSerialNum, because parameter is not passed by reference (see declaration in gmVariable.h)
	//if(!proptable->Get(_machine,"SerialNum").GetIntSafe(mSerialNum))
	//{
	//	_err.AddError("Goal.SerialNum Field Missing!");
	//	return false;
	//}

	Vector3f Euler( 0, 0, 0 );
	if ( !proptable->Get( _machine, "Orientation" ).GetVector( Euler ) )
	{
		_err.AddError( "Goal.Orientation Field Missing!" );
		return false;
	}
	mOrientation.MakeEulerZXY( Euler.X(), Euler.Y(), Euler.Z() );

	int InitialTeams = 0;
	if ( !proptable->Get( _machine, "TeamAvailability" ).GetIntSafe( InitialTeams, 0 ) )
	{
		//_err.AddError("Goal.TeamAvailability Field Missing!");
		//return false;
	}
	mAvailableTeamsInit = BitFlag32( InitialTeams );
	mAvailableTeams = mAvailableTeamsInit;

	mRoleMask.ClearAll();
	if ( gmTableObject * roleTable = proptable->Get( _machine, "Roles" ).GetTableObjectSafe() )
	{
		int NumElements = 0;
		const IntEnum *Enum = 0;
		System::mInstance->mGame->GetRoleEnumeration( Enum, NumElements );

		gmTableIterator tIt;
		gmTableNode * pNodeRole = roleTable->GetFirst( tIt );
		while ( pNodeRole )
		{
			const char * roleName = pNodeRole->m_value.GetCStringSafe( 0 );
			if ( roleName )
			{
				for ( int e = 0; e < NumElements; ++e )
				{
					if ( !Utils::StringCompareNoCase( roleName, Enum[ e ].mKey ) )
					{
						mRoleMask.SetFlag( Enum[ e ].mValue );
					}
				}
			}
			pNodeRole = roleTable->GetNext( tIt );
		}
	}

	int CreateOnLoad = GetCreateOnLoad();
	if ( !proptable->Get( _machine, "CreateOnLoad" ).GetIntSafe( CreateOnLoad, CreateOnLoad ) )
	{
		//_err.AddError("Goal.TeamAvailability Field Missing!");
		//return false;
	}
	SetCreateOnLoad( CreateOnLoad != 0 );

	if ( !proptable->Get( _machine, "RandomUsePoint" ).GetInt( mRandomUsePoint, 0 ) )
	{
		//_err.AddError("Goal.RandomUsePoint Field Missing!");
		//return false;
	}

	if ( !proptable->Get( _machine, "Range" ).GetInt( mRange, 0 ) )
	{
		//_err.AddError("Goal.Range Field Missing!");
		//return false;
	}

	// clear out the properties we don't want to pass along.
	//proptable->Set(_machine,"Version",gmVariable::s_null);
	proptable->Set( _machine, "Name", gmVariable::s_null );
	proptable->Set( _machine, "TagName", gmVariable::s_null );
	proptable->Set( _machine, "GroupName", gmVariable::s_null );
	proptable->Set( _machine, "Position", gmVariable::s_null );
	proptable->Set( _machine, "Radius", gmVariable::s_null );
	proptable->Set( _machine, "MinRadius", gmVariable::s_null );
	proptable->Set( _machine, "SerialNum", gmVariable::s_null );
	proptable->Set( _machine, "GoalType", gmVariable::s_null );
	proptable->Set( _machine, "Orientation", gmVariable::s_null );
	proptable->Set( _machine, "TeamAvailability", gmVariable::s_null );
	proptable->Set( _machine, "Roles", gmVariable::s_null );

	return InternalInit( proptable, false );
}

void MapGoal::ShowHelp()
{
	if ( mCallback_OnHelp )
	{
		gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();

		gmGCRoot<gmUserObject> mgref = GetScriptObject( pMachine );

		gmCall call;
		if ( call.BeginFunction( pMachine, mCallback_OnHelp, gmVariable( mgref ) ) )
		{
			call.End();
		}
	}
}

void MapGoal::HudDisplay()
{
#ifdef ENABLE_DEBUG_WINDOW
	if(DW.Hud.mPropertySheet)
	{
		GatherProperties(DW.Hud.mPropertySheet);

		if(mHudDisplay)
		{
			gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();

			gmGCRoot<gmUserObject> mgref = GetScriptObject(pMachine);

			gmCall call;
			if(call.BeginFunction(pMachine, mHudDisplay, gmVariable(mgref)))
			{
				call.AddParamUser(DW.Hud.mUserObject);
				call.End();
			}
		}
	}
#endif
}

void MapGoal::CreateGuiFromSchema( gmMachine *a_machine, gmTableObject *a_schema )
{
#ifdef ENABLE_DEBUG_WINDOW
	gmTableIterator it;
	gmTableNode *pNode = a_schema->GetFirst(it);
	while(pNode)
	{
		const char *PropName = pNode->mkey.GetCStringSafe();
		if(PropName)
		{
			gmVariable thisObj(GetScriptObject(a_machine));

			gmVariable current(gmVariable::s_null);
			const gmSchema::ElementType et = gmSchema::GetElementType(a_machine,pNode->m_value);
			switch (et)
			{
				case gmSchema::EL_TABLEOF:
				{
					break;
				}
				case gmSchema::EL_ENUM:
				{
					const gmTableObject *options = gmSchema::GetEnumOptions(a_machine,pNode->m_value,thisObj,current);
					if(options)
					{
						MgListModel *lm = new MgListModel(options);
						gcn::DropDown *widget = new gcn::DropDown(lm);
						widget->getScrollArea()->setHeight(widget->getFont()->getHeight() * 4);
						widget->setActionEventId(PropName);
						widget->addActionListener(this);
						widget->addDeathListener(lm);

						DW.Hud.mPropertySheet->addProperty(PropName,widget);
					}
					break;
				}
				case gmSchema::EL_FLOATRANGE:
				{
					break;
				}
				case gmSchema::EL_INTRANGE:
				{
					break;
				}
				case gmSchema::EL_NUMRANGE:
				{
					float minrange, maxrange;
					if(gmSchema::GetNumRange(a_machine,pNode->m_value,thisObj,current,minrange,maxrange))
					{
						gcn::Slider *widget = new gcn::Slider(minrange, maxrange);
						widget->setActionEventId(PropName);
						widget->addActionListener(this);
						widget->setValue(/*current*/0.f);
						DW.Hud.mPropertySheet->addProperty(PropName,widget);
					}
					break;
				}
				case gmSchema::EL_VARTYPE:
				{
					break;
				}
				case gmSchema::EL_NONE:
				default:
				{
					break;
				}
			}
		}
		pNode = a_schema->GetNext(it);
	}
#endif
}

#ifdef ENABLE_DEBUG_WINDOW
void MapGoal::action(const gcn::ActionEvent& actionEvent)
{
}
#endif

#ifdef ENABLE_REMOTE_DEBUGGING
void MapGoal::Sync( RemoteLib::DebugConnection * connection ) {
	//if ( fullSync || mNeedsSynced ) {
	//	snapShot.Clear();
	//}

	//RemoteLib::DataBufferStatic<2048> localBuffer;
	//localBuffer.beginWrite( RemoteLib::DataBuffer::WriteModeAllOrNone );

	//MapGoalSnapshot newSnapShot = snapShot;

	//const Box3f worldbounds = GetWorldBounds();
	//const float heading = Mathf::RadToDeg( worldbounds.Axis[ 0 ].XYHeading() );
	//const float pitch = Mathf::RadToDeg( worldbounds.Axis[ 0 ].GetPitch() );

	//newSnapShot.Sync( "name", GetName().c_str(), localBuffer );
	//newSnapShot.Sync( "tagName", GetTagName().c_str(), localBuffer );
	//newSnapShot.Sync( "entityid", GetEntity().AsInt(), localBuffer );
	//newSnapShot.Sync( "ownerid", GetOwner().AsInt(), localBuffer );
	//newSnapShot.Sync( "x", worldbounds.Center.X(), localBuffer );
	//newSnapShot.Sync( "y", worldbounds.Center.Y(), localBuffer );
	//newSnapShot.Sync( "z", worldbounds.Center.Z(), localBuffer );
	//newSnapShot.Sync( "yaw", -Mathf::RadToDeg( heading ), localBuffer );
	//newSnapShot.Sync( "pitch", Mathf::RadToDeg( pitch ), localBuffer );
	//newSnapShot.Sync( "sizex", worldbounds.Extent[ 0 ], localBuffer );
	//newSnapShot.Sync( "sizey", worldbounds.Extent[ 1 ], localBuffer );
	//newSnapShot.Sync( "sizez", worldbounds.Extent[ 2 ], localBuffer );
	//newSnapShot.Sync( "defaultPriority", GetDefaultPriority(), localBuffer );
	//newSnapShot.Sync( "usersInProgress", GetCurrentUsers( MapGoal::TRACK_INPROGRESS ), localBuffer );
	//newSnapShot.Sync( "maxUsersInProgress", GetMaxUsers( MapGoal::TRACK_INPROGRESS ), localBuffer );
	//newSnapShot.Sync( "usersInUse", GetCurrentUsers( MapGoal::TRACK_INUSE ), localBuffer );
	//newSnapShot.Sync( "maxUsersInUse", GetMaxUsers( MapGoal::TRACK_INUSE ), localBuffer );
	//newSnapShot.Sync( "availableTeamMask", GetAvailableFlags().GetRawFlags(), localBuffer );
	//newSnapShot.Sync( "roleMask", GetRoleMask().GetRawFlags(), localBuffer );
	//// todo: routes, team/class priorities, etc

	//const uint32 writeErrors = localBuffer.endWrite();
	//assert( writeErrors == 0 );

	//if ( localBuffer.getBytesWritten() > 0 && writeErrors == 0 ) {
	//	db.beginWrite( RemoteLib::DataBuffer::WriteModeAllOrNone );
	//	db.startSizeHeader();
	//	db.writeInt32( RemoteLib::ID_qmlComponent );
	//	db.writeInt32( GetSerialNum() );
	//	db.writeSmallString( "mapgoal" );
	//	db.append( localBuffer );
	//	db.endSizeHeader();

	//	if ( db.endWrite() == 0 ) {
	//		// mark the stuff we synced as done so we don't keep spamming it
	//		snapShot = newSnapShot;
	//	}
	//}
}
#endif

//////////////////////////////////////////////////////////////////////////

static int MG_HandleMaxUsers( gmThread *a_thread, MapGoal::TrackingCat _cat )
{
	int32_t iMaxUsers = 0;

	MapGoal *NativePtr = 0;
	if ( !gmBind2::Class<MapGoal>::FromThis( a_thread, NativePtr ) || !NativePtr )
	{
		GM_EXCEPTION_MSG( "Script Function on NULL MapGoal" );
		return GM_EXCEPTION;
	}

	switch ( a_thread->GetNumParams() )
	{
		case 1:
		{
			GM_CHECK_INT_PARAM( newval, 0 );
			iMaxUsers = NativePtr->GetMaxUsers( _cat );
			NativePtr->SetMaxUsers( _cat, newval );
		}
		case 0:
		{
			iMaxUsers = NativePtr->GetMaxUsers( _cat );
			break;
		}
		default:
			GM_EXCEPTION_MSG( "Expected 0 or 1 param." );
			return GM_EXCEPTION;
	}

	a_thread->PushInt( iMaxUsers );
	return GM_OK;
}

static int gmfMaxUsers_InProgress( gmThread *a_thread )
{
	return MG_HandleMaxUsers( a_thread, MapGoal::TRACK_INPROGRESS );
}

static int gmfMaxUsers_InUse( gmThread *a_thread )
{
	return MG_HandleMaxUsers( a_thread, MapGoal::TRACK_INUSE );
}

static int gmfGetEntity( gmThread *a_thread )
{
	MapGoal *NativePtr = 0;
	if ( !gmBind2::Class<MapGoal>::FromThis( a_thread, NativePtr ) || !NativePtr )
	{
		GM_EXCEPTION_MSG( "Script Function on NULL MapGoal" );
		return GM_EXCEPTION;
	}

	if ( NativePtr->GetEntity().IsValid() )
		a_thread->PushEntity( NativePtr->GetEntity().AsInt() );
	else
		a_thread->PushNull();
	return GM_OK;
}

static int gmfGetOwner( gmThread *a_thread )
{
	MapGoal *NativePtr = 0;
	if ( !gmBind2::Class<MapGoal>::FromThis( a_thread, NativePtr ) || !NativePtr )
	{
		GM_EXCEPTION_MSG( "Script Function on NULL MapGoal" );
		return GM_EXCEPTION;
	}

	if ( NativePtr->GetOwner().IsValid() )
		a_thread->PushEntity( NativePtr->GetOwner().AsInt() );
	else
		a_thread->PushNull();
	return GM_OK;
}

static int gmfAddUsePoint( gmThread *a_thread )
{
	MapGoal *NativePtr = 0;
	if ( !gmBind2::Class<MapGoal>::FromThis( a_thread, NativePtr ) || !NativePtr )
	{
		GM_EXCEPTION_MSG( "Script Function on NULL MapGoal" );
		return GM_EXCEPTION;
	}

	GM_CHECK_NUM_PARAMS( 1 );
	GM_CHECK_VECTOR_PARAM( v, 0 );
	GM_INT_PARAM( rel, 1, 0 );

	NativePtr->AddUsePoint( Vector3f( v.x, v.y, v.z ), rel != 0 );
	return GM_OK;
}

static int gmfLimitToWeapon( gmThread *a_thread )
{
	MapGoal *NativePtr = 0;
	if ( !gmBind2::Class<MapGoal>::FromThis( a_thread, NativePtr ) || !NativePtr )
	{
		GM_EXCEPTION_MSG( "Script Function on NULL MapGoal" );
		return GM_EXCEPTION;
	}

	return NativePtr->GetLimitWeapons().FromScript( a_thread );
}

static int gmfDisableWithEntityFlags( gmThread *a_thread )
{
	MapGoal *NativePtr = 0;
	if ( !gmBind2::Class<MapGoal>::FromThis( a_thread, NativePtr ) || !NativePtr )
	{
		GM_EXCEPTION_MSG( "Script Function on NULL MapGoal" );
		return GM_EXCEPTION;
	}

	BitFlag64 bf;
	for ( int p = 0; p < a_thread->GetNumParams(); ++p )
	{
		GM_CHECK_INT_PARAM( fl, p );
		bf.SetFlag( fl, true );
	}
	NativePtr->SetDisableWithEntityFlag( bf );
	return GM_OK;
}

static int gmfDeleteWithEntityFlags( gmThread *a_thread )
{
	MapGoal *NativePtr = 0;
	if ( !gmBind2::Class<MapGoal>::FromThis( a_thread, NativePtr ) || !NativePtr )
	{
		GM_EXCEPTION_MSG( "Script Function on NULL MapGoal" );
		return GM_EXCEPTION;
	}

	BitFlag64 bf;
	for ( int p = 0; p < a_thread->GetNumParams(); ++p )
	{
		GM_CHECK_INT_PARAM( fl, p );
		bf.SetFlag( fl, true );
	}
	NativePtr->SetDeleteWithEntityFlag( bf );
	return GM_OK;
}

static int gmfGetPriorityForClient( gmThread *a_thread )
{
	MapGoal *NativePtr = 0;
	if ( !gmBind2::Class<MapGoal>::FromThis( a_thread, NativePtr ) || !NativePtr )
	{
		GM_EXCEPTION_MSG( "Script Function on NULL MapGoal" );
		return GM_EXCEPTION;
	}

	if ( a_thread->GetNumParams() == 1 )
	{
		GM_CHECK_NUM_PARAMS( 1 );
		GM_CHECK_GMBIND_PARAM( Client*, gmBot, bot, 0 );
		a_thread->PushFloat( NativePtr->GetPriorityForClient( bot ) );
	}
	else if ( a_thread->GetNumParams() == 2 )
	{
		GM_CHECK_INT_PARAM( teamId, 0 );
		GM_CHECK_INT_PARAM( classId, 1 );
		a_thread->PushFloat( NativePtr->GetPriorityForClass( teamId, classId ) );
	}
	else
	{
		GM_EXCEPTION_MSG( "expected (int,int), or (bot)" );
		return GM_EXCEPTION;
	}
	return GM_OK;
}

static int gmfSetRoles( gmThread *a_thread )
{
	MapGoal *NativePtr = 0;
	if ( !gmBind2::Class<MapGoal>::FromThis( a_thread, NativePtr ) || !NativePtr )
	{
		GM_EXCEPTION_MSG( "Script Function on NULL MapGoal" );
		return GM_EXCEPTION;
	}

	GM_CHECK_NUM_PARAMS( 1 );

	BitFlag32 rolemask = NativePtr->GetRoleMask(); // cs: preserve current mask
	for ( int p = 0; p < a_thread->GetNumParams(); ++p )
	{
		GM_CHECK_INT_PARAM( r, p );
		rolemask.SetFlag( r, true );
	}
	NativePtr->SetRoleMask( rolemask );
	return GM_OK;
}

static int gmfClearRoles( gmThread *a_thread )
{
	MapGoal *NativePtr = 0;
	if ( !gmBind2::Class<MapGoal>::FromThis( a_thread, NativePtr ) || !NativePtr )
	{
		GM_EXCEPTION_MSG( "Script Function on NULL MapGoal" );
		return GM_EXCEPTION;
	}

	GM_CHECK_NUM_PARAMS( 1 );

	BitFlag32 rolemask = NativePtr->GetRoleMask(); // cs: preserve current mask
	for ( int p = 0; p < a_thread->GetNumParams(); ++p )
	{
		GM_CHECK_INT_PARAM( r, p );
		rolemask.SetFlag( r, false );
	}
	NativePtr->SetRoleMask( rolemask );
	return GM_OK;
}

static int gmfHasRole( gmThread *a_thread )
{
	MapGoal *NativePtr = 0;
	if ( !gmBind2::Class<MapGoal>::FromThis( a_thread, NativePtr ) || !NativePtr )
	{
		GM_EXCEPTION_MSG( "Script Function on NULL MapGoal" );
		return GM_EXCEPTION;
	}

	GM_CHECK_NUM_PARAMS( 1 );
	for ( int p = 0; p < a_thread->GetNumParams(); ++p )
	{
		GM_CHECK_INT_PARAM( r, p );
		if ( NativePtr->GetRoleMask().CheckFlag( r ) )
		{
			a_thread->PushInt( 1 );
			return GM_OK;
		}
	}
	a_thread->PushInt( 0 );
	return GM_OK;
}

static int gmfSetBaseGoalType( gmThread *a_thread )
{
	MapGoal *NativePtr = 0;
	if ( !gmBind2::Class<MapGoal>::FromThis( a_thread, NativePtr ) || !NativePtr )
	{
		GM_EXCEPTION_MSG( "Script Function on NULL MapGoal" );
		return GM_EXCEPTION;
	}

	GM_CHECK_NUM_PARAMS( 1 );
	GM_CHECK_STRING_PARAM( basetype, 0 );

	filePath script( "scripts/mapgoals/%s", basetype );

	bool Good = false;
	try
	{
		if ( NativePtr->LoadFromFile( script ) )
		{
			Good = true;
		}
	}
	catch ( const std::exception & )
	{
	}

	if ( !Good )
	{
		GM_EXCEPTION_MSG( "Unable to set base goal type: %s", script );
		return GM_EXCEPTION;
	}

	return GM_OK;
}

static int gmfCreateGuiFromSchema( gmThread *a_thread )
{
	GM_CHECK_USER_PARAM_TYPE( gmSchema::GM_SCHEMA, 0 );

	MapGoal *NativePtr = 0;
	if ( !gmBind2::Class<MapGoal>::FromThis( a_thread, NativePtr ) || !NativePtr )
	{
		GM_EXCEPTION_MSG( "Script Function on NULL MapGoal" );
		return GM_EXCEPTION;
	}

	gmTableObject *Schema = static_cast<gmTableObject*>( a_thread->Param( 0 ).GetUserSafe( gmSchema::GM_SCHEMA ) );
	NativePtr->CreateGuiFromSchema( a_thread->GetMachine(), Schema );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void MapGoal_AsString( MapGoal *a_var, char * a_buffer, int a_bufferSize )
{
	_gmsnprintf( a_buffer, a_bufferSize, "MapGoal(%s::%s)", a_var->GetGoalType().c_str(), a_var->GetName().c_str() );
}

//////////////////////////////////////////////////////////////////////////

void MapGoal::Bind( gmMachine* _m )
{
	gmBind2::Class<MapGoal>( "MapGoal", _m )
		//.constructor()
		.asString( MapGoal_AsString )

		.func( &MapGoal::IsAvailable, "IsAvailable", "Is goal available for a given team." )
		.func( &MapGoal::SetAvailable, "SetAvailable", "Set goal available for a given team." )

		.func( &MapGoal::IsAvailableInitial, "IsAvailableInitial", "Is goal available for team initially." )
		.func( &MapGoal::SetAvailableInitial, "SetAvailableInitial", "Set goal available for team initially." )

		.func( &MapGoal::GetGoalState, "GetGoalState" )
		.func( &MapGoal::GetGoalType, "GetGoalType" )
		.func( &MapGoal::GetTagName, "GetTagName" )
		.func( &MapGoal::GetName, "GetName", "Get the full name of the goal." )

		.func( &MapGoal::GetGroupName, "GetGroupName", "Get the current group this goal is assigned to." )
		.func( &MapGoal::SetGroupName, "SetGroupName", "Set the current group this goal is assigned to." )

		.func( gmfSetBaseGoalType, "SetBaseGoalType", "Clones a base goal type. Should be done first thing in goal script." )

		.func( &MapGoal::GetPosition_Script, "GetPosition", "Get Goal Position" )
		.func( &MapGoal::SetPosition_Script, "SetPosition", "Set Goal Position" )
		.func( &MapGoal::GetFacing_Script, "GetFacing", "Get Goal Facing" )
		.func( &MapGoal::SetFacing_Script, "SetFacing", "Set Goal Position" )

		.func( &MapGoal::GetDisabled, "IsDisabled", "Get whether the goal is currently disabled." )
		.func( &MapGoal::SetDisabled, "DisableGoal", "Set whether the goal is currently disabled." )

		.func( &MapGoal::GetRange_Script, "GetRange", "Get current range limit for the goal" )
		.func( &MapGoal::SetRange_Script, "SetRange", "Set the current range limit for the goal" )

		//.func(&MapGoal::GetWorldBounds,	"GetBounds")
		//.func(&MapGoal::GetLocalBounds,	"GetLocalBounds")
		//.func(&MapGoal::SetGoalBounds,	"SetBounds")
		.func( &MapGoal::SetBounds_Script, "SetBounds", "Set the object space bounding box of the goal." )
		.func( &MapGoal::GetBoundsCenter_Script, "GetCenterBounds", "Get the center of the bounding box." )
		//.func(&MapGoal::GetMatrix,		"GetMatrix")
		//.func(&MapGoal::SetMatrix,		"SetMatrix")

		.func( &MapGoal::GetRadius, "GetRadius", "Get the Goal Radius" )
		.func( &MapGoal::SetRadius, "SetRadius", "Set the Goal Radius" )

		//.func(&MapGoal::SetProperties,	"SetProperties")

		.func( gmfAddUsePoint, "AddUsePoint", "Adds a 'use' point to the goal." )
		.func( &MapGoal::GetWorldUsePoint, "GetUsePoint", "Gets a use point in world space, by index." )
		.func( &MapGoal::GetNumUsePoints, "GetNumUsePoint", "Gets the number of use points currently defined." )

		.func( gmfSetRoles, "SetRoles", "Sets the roles that are allowed to use this goal." )
		.func( gmfClearRoles, "ClearRoles", "Removes the given roles from this goal." )
		.func( gmfHasRole, "HasRole", "Return true if the goal has any of the roles provided as params." )

		.func( gmfDisableWithEntityFlags, "DisableIfEntityFlag", "Sets one or more entity flags that will cause the goal to be disabled." )
		.func( gmfDeleteWithEntityFlags, "DeleteIfEntityFlag", "Sets one or more entity flags that will cause the goal to be deleted." )

		/*.func(&MapGoal::DrawBounds,			"DrawBounds")
		.func(&MapGoal::DrawRadius,			"DrawRadius")
		.func(&MapGoal::DrawUsePoints,		"DrawUsePoints")
		.func(&MapGoal::DrawRoute,			"DrawRoutes")*/

		//.func(&MapGoal::SetEnableDraw,	"SetEnableDraw")

		//.func(&MapGoal::AddRoute,			"AddRoute")

		.func( &MapGoal::SetPriorityForClass, "SetGoalPriority", "Sets the priority for a given class/team." )
		.func( gmfGetPriorityForClient, "GetGoalPriority", "Gets the priority for a given class/team." )
		.func( &MapGoal::ResetGoalPriorities, "ResetGoalPriorities", "Clears all the current priorities." )

		.func( &MapGoal::SetDeleteMe, "SetRemoveFlag", "Mark the goal for deletion." )

		.func( &MapGoal::RenderDefault, "RenderDefault", "Render the default debug options." )
		.var( &MapGoal::mDefaultRenderRadius, "DefaultRenderRadius", "Radius in which debug options will be displayed." )
		.var( &MapGoal::mRenderHeight, "DefaultRenderHeight", "Goal height offset where rendering will take place." )
		.var_bitfield( &MapGoal::mDefaultDrawFlags, DrawName, "RenderDefaultName", "Draw the name of the goal." )
		.var_bitfield( &MapGoal::mDefaultDrawFlags, DrawGroup, "RenderDefaultGroup", "Draw the group of the goal." )
		.var_bitfield( &MapGoal::mDefaultDrawFlags, DrawRole, "RenderDefaultRole", "Draw the roles for the goal." )
		.var_bitfield( &MapGoal::mDefaultDrawFlags, DrawBounds, "RenderDefaultBounds", "Draw the bounds of the goal." )
		.var_bitfield( &MapGoal::mDefaultDrawFlags, DrawRadius, "RenderDefaultRadius", "Draw the radius of the goal." )
		.var_bitfield( &MapGoal::mDefaultDrawFlags, DrawInitialAvail, "RenderDefaultInitialAvailability", "Draw the initial availability of the goal." )
		.var_bitfield( &MapGoal::mDefaultDrawFlags, DrawCurrentAvail, "RenderDefaultCurrentAvailability", "Draw the current availability of the goal." )
		.var_bitfield( &MapGoal::mDefaultDrawFlags, DrawCenterBounds, "RenderDefaultAtCenterBounds", "Draw debug options using the center of the bounding box." )
		.var_bitfield( &MapGoal::mDefaultDrawFlags, DrawRandomUsePoint, "RenderRandomUsePoint", "Draw whether or not the goal randomly selects a usepoint." )
		.var_bitfield( &MapGoal::mDefaultDrawFlags, DrawRangeLimit, "RenderRangeLimit", "Draw the goals current range limit if greater than 0." )

		.func( gmfMaxUsers_InProgress, "MaxUsers_InProgress", "Set the max number of 'inprogress' users that can use the goal" )
		.func( gmfMaxUsers_InUse, "MaxUsers_InUse", "Set the max number of 'inuse' users that can use the goal" )

		.func( gmfGetEntity, "GetEntity", "Get the entity of the goal, if any." )
		.func( gmfGetOwner, "GetOwner", "Gets the entity owner of the goal, if any." )

		.func( gmfCreateGuiFromSchema, "CreateGuiFromSchema", "Create Gui elements for schema properties." )

		.func( &MapGoal::AddRoute_Script, "AddRoute", "Adds a route for this goal." )

		.func( gmfLimitToWeapon, "LimitToWeapon", "Adds a list of weapons that are required from any user of the goal." )

		.var( &MapGoal::mVersion, "Version", "int", "Gets the goal version." )

		.var( &MapGoal::mGoalState, "GoalState", "int", "Gets the goal state" )
		
		.var( &MapGoal::mCallback_OnInit, "InitNewGoal", "Callback", "Called on goal creation to initialize any internal variables." )
		.var( &MapGoal::mCallback_OnUpgrade, "UpgradeVersion", "Callback", "Called to upgrade the goal to the latest used version." )
		.var( &MapGoal::mCallback_OnRender, "Render", "Callback", "Called when draw_goals is enabled for this goal. Used to render itself." )
		.var( &MapGoal::mCallback_OnUpdate, "Update", "Callback", "Called every frame to update the state of the goal if needed." )
		.var( &MapGoal::mCallback_OnSerialize, "SaveToTable", "Callback", "Called when the goals are saved to a file. Allows the goal to serialize persistent information." )
		.var( &MapGoal::mCallback_OnSetProperty, "SetProperty", "Callback", "Called on bot goal_setproperty x y, where x is a property name and y is a value or keyword." )
		.var( &MapGoal::mCallback_OnHelp, "Help", "Callback", "Called on bot goal_help to print requirements and available properties for the goal." )
		.var( &MapGoal::mCallback_OnHudDisplay, "HudDisplay", "Callback", "Called when goal is highlighted to create gui elements for debug visualization." )
		.var( &MapGoal::mCallback_OnCalcPriority, "GetPriority", "Callback", "Called when goal is highlighted to create gui elements for debug visualization." )

		.var( &MapGoal::mExtraDebugText, "ExtraDebugText", "std::string", "Additional debug text to render in RenderDefault function." )

		.var( &MapGoal::mRadius, "Radius", "float", "Radius of the goal." )
		.var( &MapGoal::mMinRadius, "MinRadius", "float", "Minimum allowed radius of the goal." )
		//.var(&MapGoal::mBounds,				"Bounds")

		.var( &MapGoal::mGoalType, "GoalType", "std::string", "Type of goal." )

		.var( &MapGoal::mDefaultPriority, "DefaultPriority", "float", "Default priority of the goal, if no class/team specific priorities." )
		.var( &MapGoal::mRolePriorityBonus, "RolePriorityBonus", "float", "Role priority bonus of the goal, for users matching role." )

		.var_readonly( &MapGoal::mSerialNum, "SerialNum", "int readonly", "Auto generated unique serial number of the goal." )

		// flags
		.var( &MapGoal::mDeleteMe, "MarkForRemoval", "bool", "Mark the goal for deletion." )
		.var( &MapGoal::mDynamicPosition, "DynamicPosition", "bool", "Goal should update its position from its entity." )
		.var( &MapGoal::mDynamicOrientation, "DynamicOrientation", "bool", "Goal should update its orientation from its entity." )
		.var( &MapGoal::mRemoveWithEntity, "RemoveWithEntity", "bool", "Goal should be removed if its entity is removed." )
		.var( &MapGoal::mInUse, "InUse", "bool", "Goal is inuse and should not be chosen." )
		.var( &MapGoal::mDisableForControllingTeam, "DisableForControllingTeam", "bool", "Goal will be made unavailable for the team which controls it(GetOwner)" )
		.var( &MapGoal::mDontSave, "DontSave", "bool", "Dont save this goal into the map goal script." )
		.var( &MapGoal::mRenderGoal, "RenderGoal", "bool", "Enable rendering for this goal." )
		.var( &MapGoal::mRenderRoutes, "RenderRoutes", "bool", "Enable rendering of the routes for this goal." )
		.var( &MapGoal::mCreateOnLoad, "CreateOnLoad", "bool", "False to not create the goal at load time, but keep the data around for when created by the interface." )
		;
}