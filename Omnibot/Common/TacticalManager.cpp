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
#include "InterfaceFuncs.h"
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
	: mShowTokenClient( 1 )
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
					beh.mDesirability, beh.mNavigationCost, beh.mFinalCost ) );
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
