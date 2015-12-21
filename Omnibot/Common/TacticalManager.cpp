////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "TacticalManager.h"
#include "Client.h"
#include "ScriptManager.h"
#include "RenderBuffer.h"
#include "System.h"

//////////////////////////////////////////////////////////////////////////

const BehaviorActionEnum BehaviorActionEnum::sKeyVal [] =
{
	{ "NONE", BEHAVIOR_NONE },
	{ "KILL", BEHAVIOR_KILL },
	{ "PRESSBUTTON", BEHAVIOR_PRESSBUTTON },
	{ "RESUPPLY", BEHAVIOR_RESUPPLY },
	{ "GET_WEAPON", BEHAVIOR_GET_WEAPON },
	{ "GET_POWERUP", BEHAVIOR_GET_POWERUP },
	{ "GET_FLAG", BEHAVIOR_GET_FLAG },
	{ "CAP_FLAG", BEHAVIOR_CAP_FLAG },
	{ "CAP_CONTROL_POINT", BEHAVIOR_CAP_CONTROL_POINT },
	{ "DEFEND", BEHAVIOR_DEFEND },
};
const size_t BehaviorActionEnum::sKeyValCount = sizeof( BehaviorActionEnum::sKeyVal ) / sizeof( BehaviorActionEnum::sKeyVal[ 0 ] );

//////////////////////////////////////////////////////////////////////////

Stimulus::Stimulus()
{
	Clear();
}

Stimulus::~Stimulus()
{
}

void Stimulus::Clear()
{
	mDesirability = 1.0f;
}

//////////////////////////////////////////////////////////////////////////

StimulusBehavior::StimulusBehavior()
	: mAction( BEHAVIOR_NONE )
	, mDesirability( 0.0f )
	, mNavigationCost( 0.0f )
	, mNavigationThreat( 0.0f )
	, mFinalCost( 0.0f )	
{
}

//////////////////////////////////////////////////////////////////////////

StimulusUser::StimulusUser()
{
}

StimulusUser::~StimulusUser()
{
}

void StimulusUser::Clear()
{
	mEntity.Reset();
	mReservedStimulus.Reset();
}

//////////////////////////////////////////////////////////////////////////

TacticalManager::TacticalManager()
	: mShowTokenClient( -1 )
	, mDrawCoverSegments( 0.0f )
{
}

TacticalManager::~TacticalManager()
{
	Shutdown();
}

void TacticalManager::Init( System & system )
{
	rmt_ScopedCPUSample( TacticalManagerInit );
	InitCommands();
}

void TacticalManager::Shutdown()
{
}

void TacticalManager::Reset()
{
}

void TacticalManager::InitCommands()
{
	SetEx( "goal_save", "Saves the goals to their own file.", this, &TacticalManager::cmdSave );
	SetEx( "goal_load", "Loads the goals from their own file.", this, &TacticalManager::cmdLoad );
	SetEx( "show_tokens", "Print out info about all the active stimulus for a player.", this, &TacticalManager::cmdShowTokens );
	SetEx( "show_cover", "Shows cover segments within a distance.", this, &TacticalManager::cmdDrawCoverSegments );	
}

bool TacticalManager::Save( const std::string &_map, ErrorObj &_err )
{
	return false;
}

bool TacticalManager::Load( const std::string &_map, ErrorObj &_err )
{
	return false;
}

void TacticalManager::cmdSave( const StringVector & args )
{
	ErrorObj err;
	Save( gEngineFuncs->GetMapName(), err );
	err.PrintToConsole();
}

void TacticalManager::cmdLoad( const StringVector & args )
{
	ErrorObj err;
	Load( gEngineFuncs->GetMapName(), err );
	err.PrintToConsole();
}

void TacticalManager::cmdShowTokens( const StringVector & args )
{
	const char *strUsage [] =
	{
		"show_tokens enable[int]",
		"> enable: Enable behavior token rendering for a pariticular client",
	};

	CHECK_NUM_PARAMS( args, 2, strUsage );
	CHECK_INT_PARAM( viewmode, 1, strUsage );

	mShowTokenClient = viewmode;
}

void TacticalManager::cmdDrawCoverSegments( const StringVector & args )
{
	const char *strUsage [] =
	{
		"show_cover distance[float]",
		"> enable: Enable visualization of cover segments within a distance",
	};

	CHECK_NUM_PARAMS( args, 2, strUsage );
	CHECK_FLOAT_PARAM( dist, 1, strUsage );

	mDrawCoverSegments = dist;
}

void TacticalManager::Update( System & system )
{
	rmt_ScopedCPUSample( TacticalManagerUpdate );
	
	UpdateBehaviorTokens( system );

	if ( mShowTokenClient >= 0 && mShowTokenClient < Constants::MAX_PLAYERS )
	{
		const StimulusUser& user = mUsers[ mShowTokenClient ];
		if ( user.mEntity.IsValid() )
		{
			for ( size_t b = 0; b < user.mBehaviorActive.size(); ++b )
			{
				const StimulusBehavior& beh = user.mBehaviorActive[ b ];

				std::string actionName;
				if ( !BehaviorActionEnum::NameForValue( beh.mAction, actionName ) )
					continue;

				const std::string userName = EngineFuncs::EntityName( beh.mUser, "<unknown>" );
				const std::string stimName = EngineFuncs::EntityName( beh.mStimulus->mEntity, "<unknown>" );

				const obColor col = obColor::lerp( COLOR::GREEN, COLOR::RED, (float)b / (float)user.mBehaviorActive.size() );
				RenderBuffer::AddLine( user.mUserPos, beh.mStimulus->mPosition, col );

				RenderBuffer::AddString3d( beh.mStimulus->mPosition, col,
					va( "%d: %s - %s\nentity %s\ndesir %.1f, navcost %.1f, final %.1f",
					b, actionName.c_str(), userName.c_str(), stimName.c_str(),
					beh.mDesirability, beh.mNavigationCost, beh.mFinalCost ).c_str() );
			}
		}
	}

	if ( mDrawCoverSegments > 0.0f )
	{
		static float MIN_COVER_LEN = 64.0f;

		Vector3f eyePos;
		if ( Utils::GetLocalEyePosition( eyePos ) )
		{
			// temp populate every frame
			mCoverSegments.resize( 0 );

			static int MAX_BORDER_EDGES = 128;
			std::vector<MeshEdge> borderEdges( MAX_BORDER_EDGES );
			NavFlags inc = NAVFLAGS_WALK, exc = NAVFLAGS_NONE, border = NAVFLAGS_PUSHABLE;
			const size_t cnt = system.mNavigation->FindBorderEdges( inc, exc, border, MIN_COVER_LEN, eyePos, &borderEdges[ 0 ], borderEdges.size() );
			for ( size_t i = 0; i < cnt; ++i )
			{
				CoverSegment seg;
				seg.mEdge[ 0 ] = borderEdges[ i ].mEdge[ 0 ];
				seg.mEdge[ 1 ] = borderEdges[ i ].mEdge[ 1 ];
				seg.mNormal = borderEdges[ i ].mNormal;
				seg.mRef = borderEdges[ i ].mPolyRef;
				seg.mFlags = COVER_NONE;				
				mCoverSegments.push_back( seg );
			}
			
			const float drawdsq = Mathf::Sqr( mDrawCoverSegments );

			for ( size_t i = 0; i < mCoverSegments.size(); ++i )
			{
				const CoverSegment& seg = mCoverSegments[ i ];

				// only draw ones within a distance
				const float dsq = DistPoint3Segment3f( eyePos, Segment3f( seg.mEdge[ 0 ], seg.mEdge[ 1 ] ) ).GetSquared();
				if ( dsq <= drawdsq )
				{
					const Vector3f midPt = ( seg.mEdge[ 0 ] + seg.mEdge[ 1 ] ) * 0.5f;
					RenderBuffer::AddLine( seg.mEdge[ 0 ], seg.mEdge[ 1 ], COLOR::CYAN );

					// draw the orientation
					RenderBuffer::AddArrow( midPt - seg.mNormal * 8.0f, midPt + seg.mNormal * 8.0f, COLOR::GREEN );
				}
			}
		}
	}
}

void TacticalManager::EntityCreated( const EntityInstance &ei )
{
	StimulusPtr stim( new Stimulus );
	stim->mEntity = ei.mEntity;
	stim->mEntInfo = ei.mEntInfo;

	EngineFuncs::EntityPosition( ei.mEntity, stim->mPosition );

	mStimulus[ ei.mEntity.GetIndex() ] = stim;
}

void TacticalManager::EntityDeleted( const EntityInstance &ei )
{
	mStimulus[ ei.mEntity.GetIndex() ].reset();
}

bool BehaviorSort( const StimulusBehavior& beh1, const StimulusBehavior& beh2 )
{
	return beh1.mFinalCost < beh2.mFinalCost;
}

void TacticalManager::UpdateBehaviorTokens( System & system )
{
	rmt_ScopedCPUSample( UpdateBehaviorTokens );

	// do an initial pass to update the stimulus state once
	for ( size_t e = 0; e < Constants::MAX_ENTITIES; ++e )
	{
		StimulusPtr& stim = mStimulus[ e ];
		if ( stim )
		{
			if ( !stim->mEntity.IsValid() || !IGame::GetEntityInfo( stim->mEntity, stim->mEntInfo ) || !EngineFuncs::EntityPosition( stim->mEntity, stim->mPosition ) )
			{
				stim.reset();
				continue;
			}
		}
	}

	std::vector<Vector3f> dstPositions;
	dstPositions.reserve( 128 );

	for ( size_t c = 0; c < Constants::MAX_PLAYERS; ++c )
	{
		StimulusUser& user = mUsers[ c ];

		ClientPtr client = system.mGame->GetClientByIndex( c );
		if ( !client )
		{
			user.Clear();
			continue;
		}

		user.mEntity = client->GetGameEntity();
		if ( !user.mEntity.IsValid() || !EngineFuncs::EntityPosition( user.mEntity, user.mUserPos ) )
		{
			user.Clear();
			continue;
		}

		// build the new active behavior set based on the results of the deferred queries
		user.mBehaviorActive.resize( 0 );

		// use the deferred navigation query to finalize the behavior costs
		if ( user.mQuery && user.mQuery->mFinished )
		{
			if ( user.mQuery->mGoals.size() == user.mBehaviorsAnalyze.size() )
			{
				for ( size_t i = 0; i < user.mQuery->mGoals.size(); ++i )
				{
					DeferredQuery::Goal& goal = user.mQuery->mGoals[ i ];

					// if there's a path to get there
					if ( goal.mNavCost >= 0.0 )
					{
						StimulusBehavior& beh = user.mBehaviorsAnalyze[ i ];
						beh.mNavigationCost = goal.mNavCost;
						beh.mNavigationThreat = goal.mThreatCost;
						beh.mFinalCost = beh.mDesirability * beh.mNavigationCost; // temp
						user.mBehaviorActive.push_back( beh );
					}
				}

				std::sort( user.mBehaviorActive.begin(), user.mBehaviorActive.end(), BehaviorSort );
			}
		}

		// queue up a list of behaviors to analyze for the next frame
		user.mBehaviorsAnalyze.resize( 0 );

		// if this user has a stimulus reserved, maintain the lock
		/*if ( user.mReservedStimulus.IsValid() )
		{
		StimulusBehavior beh;
		beh.mUser = ent;
		beh.mStimulus = user.mReservedStimulus;
		beh.mDesirability = 0.0f;
		beh.mNavigationCost = 0.0f;
		user.mBehaviorsAnalyze.push_back( beh );
		}*/

		for ( size_t e = 0; e < Constants::MAX_ENTITIES; ++e )
		{
			StimulusPtr& stim = mStimulus[ e ];
			if ( !stim || stim->mEntInfo.mFlags.CheckFlag( ENT_FLAG_DISABLED ) )
				continue;

			StimulusBehavior beh;
			beh.mUser = user.mEntity;
			beh.mStimulus = stim;
			client->ProcessStimulusBehavior( user.mBehaviorsAnalyze, stim );
		}

		dstPositions.resize( 0 );
		for ( size_t i = 0; i < user.mBehaviorsAnalyze.size(); ++i )
		{
			dstPositions.push_back( user.mBehaviorsAnalyze[ i ].mStimulus->mPosition );
		}

		if ( dstPositions.size() > 0 )
		{
			NavFlags inc = NAVFLAGS_WALK, exc = NAVFLAGS_NONE;
			client->GetNavFlags( inc, exc );

			// generate the navigation cost of the tokens
			system.mNavigation->QueueBatchQuery( user.mQuery, inc, exc, user.mUserPos, dstPositions );
		}
	}
}

size_t TacticalManager::GetBehaviorsForUser( GameEntity ent, StimulusBehavior* behaviors, size_t maxBehaviors )
{
	size_t count = 0;

	for ( size_t c = 0; c < Constants::MAX_PLAYERS; ++c )
	{
		StimulusUser& user = mUsers[ c ];
		if ( user.mEntity == ent )
		{
			for ( size_t i = 0; i < user.mBehaviorActive.size() && count < maxBehaviors; ++i )
			{
				behaviors[ count++ ] = user.mBehaviorActive[ i ];
			}
		}
	}

	return count;
}
