////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "D3_Client.h"
#include "D3_Messages.h"

#include "RenderBuffer.h"

#include "IGame.h"
#include "IGameManager.h"
#include "BotWeaponSystem.h"
#include "BotTargetingSystem.h"
#include "FilterAllType.h"

namespace AiState
{
	class ShootExplodingBarrel : public StateChild, public AimerUser
	{
	public:

		void RenderDebug()
		{
			if ( mShootBarrel.IsValid() )
			{
				Box3f obb;
				EngineFuncs::EntityWorldOBB( mShootBarrel, obb );
				RenderBuffer::AddOBB( obb, COLOR::GREEN );
			}
		}

		float GetPriority()
		{
			const MemoryRecord *pTarget = GetClient()->GetTargetingSystem()->GetCurrentTargetRecord();
			if ( !pTarget )
				return 0.f;

			if ( mShootBarrel.IsValid() )
				return 1.f;

			Vector3f vTargetPos = pTarget->PredictPosition( 1.f );

			static float fSplashRadius = 256.f;

			SensoryMemory *sensory = GetClient()->GetSensoryMemory();

			// Check for exploding barrels near my target.
			MemoryRecords records;
			Vector3List recordpos;

			FilterAllType filter( GetClient(), AiState::SensoryMemory::EntAny, records );
			filter.MemorySpan( Utils::SecondsToMilliseconds( 7.f ) );
			filter.AddGroup( ENT_GRP_PROP_EXPLODE );
			//filter.AddClass( ENT_CLASS_EXPLODING_BARREL );
			sensory->QueryMemory( filter );
			sensory->GetRecordInfo( records, &recordpos, NULL );

			for ( uint32_t i = 0; i < recordpos.size(); ++i )
			{
				if ( SquaredLength( recordpos[ i ], vTargetPos ) < Mathf::Sqr( fSplashRadius ) )
				{
					MemoryRecord *pRec = sensory->GetMemoryRecord( records[ i ] );
					if ( pRec )
					{
						mShootBarrel = pRec->GetEntity();
						return 1.f;
					}
				}
			}
			return 0.f;
		}

		void Enter()
		{
			FINDSTATEIF( Aimer, GetParent(), AddAimRequest( Priority::Medium, this, GetNameHash() ) );
		}

		void Exit()
		{
			mShootBarrel.Reset();
			FINDSTATEIF( Aimer, GetParent(), ReleaseAimRequest( GetNameHash() ) );
		}

		State::StateStatus Update( float fDt )
		{
			if ( !mShootBarrel.IsValid() || !gEngineFuncs->IsAlive( mShootBarrel ) )
				return State_Finished;
			return State_Busy;
		}

		bool GetAimPosition( Vector3f &_aimpos )
		{
			Box3f obb;
			if ( mShootBarrel.IsValid() && EngineFuncs::EntityWorldOBB( mShootBarrel, obb ) )
			{
				_aimpos = obb.Center;
				return true;
			}
			return false;
		}

		void OnTarget()
		{
			FINDSTATE( wsys, WeaponSystem, GetParent() );
			if ( wsys )
			{
				WeaponPtr wpn = wsys->GetCurrentWeapon();
				if ( wpn )
				{
					if ( wsys->ReadyToFire() )
					{
						wpn->PreShoot( Primary );
						wpn->Shoot();
					}
					else
					{
						wpn->StopShooting( Primary );
					}
				}
			}
		}

		ShootExplodingBarrel() : StateChild( "ShootExplodingBarrel" )
		{
		}
	private:
		GameEntity mShootBarrel;
	};
};

//////////////////////////////////////////////////////////////////////////

D3_Client::D3_Client()
{
	mStepHeight = 15.0f; // it's actually 16
}

D3_Client::~D3_Client()
{
}

NavFlags D3_Client::GetTeamFlag( int _team ) const
{
	static const NavFlags defaultTeam = 0;
	switch ( _team )
	{
		case D3_TEAM_RED:
			return NAVFLAGS_TEAM1_ONLY;
		case D3_TEAM_BLUE:
			return NAVFLAGS_TEAM2_ONLY;
		default:
			return defaultTeam;
	}
}

void D3_Client::GetNavFlags( NavFlags & includeFlags, NavFlags & excludeFlags )
{
	includeFlags = NAVFLAGS_WALK | NAVFLAGS_CROUCH;

	switch ( GetTeam() )
	{
		case D3_TEAM_RED:
			excludeFlags = (NavFlags)( ~NAVFLAGS_TEAM1_ONLY & NAVFLAGS_ALLTEAMS );
			break;
		case D3_TEAM_BLUE:
			excludeFlags = (NavFlags)( ~NAVFLAGS_TEAM2_ONLY & NAVFLAGS_ALLTEAMS );
			break;
	}
}

float D3_Client::GetGameVar( GameVar _var ) const
{
	switch ( _var )
	{
		case JumpGapOffset:
			return 0.0f;
	}
	return 0.0f;
}

bool D3_Client::DoesBotHaveFlag( MapGoalPtr _mapgoal )
{
	int iTeamFlags = 0;
	switch ( GetTeam() )
	{
		case D3_TEAM_RED:
			iTeamFlags = D3_PWR_REDFLAG;
			break;
		case D3_TEAM_BLUE:
			iTeamFlags = D3_PWR_BLUEFLAG;
			break;
	}

	if ( HasPowerup( iTeamFlags ) )
		return true;
	return false;
}

void D3_Client::SetupBehaviorTree()
{
	using namespace AiState;
	FINDSTATEIF( LowLevel, GetStateRoot(), AppendState( new ShootExplodingBarrel ) )
}