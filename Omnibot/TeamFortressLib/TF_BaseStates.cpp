////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "TF_BaseStates.h"
#include "TF_Game.h"
#include "FilterClosestTF.h"
#include "System.h"
#include "BotPathing.h"
#include "BotSteeringSystem.h"
#include "BotWeaponSystem.h"
#include "BotTargetingSystem.h"

#include "ScriptManager.h"
#include "FilterAllType.h"
#include "gmScriptGoal.h"

#include "RenderBuffer.h"

#include <iomanip>

const float SENTRY_UPGRADE_PRIORITY = 0.5f;

namespace TF_Options
{
	int BUILD_AMMO_TYPE = TF_WP_SPANNER;

	int SENTRY_BUILD_AMMO = 130;
	int SENTRY_UPGRADE_AMMO = 130;
	int SENTRY_REPAIR_AMMO = 20;
	int SENTRY_UPGRADE_WPN = TF_WP_SPANNER;

	int BUILD_ATTEMPT_DELAY = 1000;

	int TELEPORT_BUILD_AMMO = 125;
	int DISPENSER_BUILD_AMMO = 100;

	int PIPE_WEAPON = TF_WP_PIPELAUNCHER;
	int PIPE_WEAPON_WATCH = TF_WP_GRENADE_LAUNCHER;
	int PIPE_AMMO = TF_WP_GRENADE_LAUNCHER;
	int PIPE_MAX_DEPLOYED = 8;

	int ROCKETJUMP_WPN = TF_WP_ROCKET_LAUNCHER;

	float GRENADE_VELOCITY = 500.f;

	bool POLL_SENTRY_STATUS = false;

	bool REPAIR_ON_SABOTAGED = false;

	int DisguiseTeamFlags[ TF_TEAM_MAX ] =
	{
		TF_TEAM_NONE,
		TF_TEAM_NONE,
		TF_TEAM_NONE,
		TF_TEAM_NONE,
		TF_TEAM_NONE,
	};
};

namespace AiState
{
	int SentryBuild::BuildEquipWeapon = TF_WP_NONE;

	SentryBuild::SentryBuild()
		: StateChild( "SentryBuild" )
		, FollowPathUser( "SentryBuild" )
		, mNeedAmmoAmount( 0 )
		, mCantBuild( false )
	{
		REGISTER_STATE( SentryBuild, SG_NONE );
		REGISTER_STATE( SentryBuild, SG_GETTING_AMMO );
		REGISTER_STATE( SentryBuild, SG_BUILDING );
		REGISTER_STATE( SentryBuild, SG_AIMING );
		REGISTER_STATE( SentryBuild, SG_AIMED );
		REGISTER_STATE( SentryBuild, SG_UPGRADING );
		REGISTER_STATE( SentryBuild, SG_REPAIRING );
		REGISTER_STATE( SentryBuild, SG_RESUPPLY );
		REGISTER_STATE( SentryBuild, SG_DONE );
	}

	bool SentryBuild::HasEnoughAmmo( int _ammotype, int _ammorequired )
	{
		int iAmmo = 0, iMaxAmmo = 0;
		gEngineFuncs->GetCurrentAmmo( GetClient()->GetGameEntity(), _ammotype, Primary, iAmmo, iMaxAmmo );
		if ( iAmmo < _ammorequired )
		{
			return false;
		}
		return true;
	}

	STATE_ENTER( SentryBuild, SG_NONE )
	{
	}
	STATE_EXIT( SentryBuild, SG_NONE )
	{
	}
	STATE_UPDATE( SentryBuild, SG_NONE )
	{
		FINDSTATE( sg, Sentry, GetParent() );
		if ( sg )
		{
			FINDSTATE( sg, Sentry, GetParent() );
			if ( sg && sg->HasSentry() && sg->SentryFullyBuilt() )
			{
				if ( sg->GetSentryStatus().mLevel < 3 )
				{
					SetNextState( SG_UPGRADING );
					return;
				}

				const Sentry::SentryStatus &ss = sg->GetSentryStatus();
				if ( (ss.mSabotaged && TF_Options::REPAIR_ON_SABOTAGED) ||
					 (ss.mHealth < ss.mMaxHealth) )
				{
					SetNextState( SG_REPAIRING );
					return;
				}

				if ( ss.mRockets[ 0 ] < ss.mRockets[ 1 ] / 2 ||
					 ss.mShells[ 0 ] < ss.mShells[ 1 ] / 2 )
				{
					SetNextState( SG_RESUPPLY );
					return;
				}
				SetNextState( SG_DONE );
			}
			else
			{
				SetNextState( SG_BUILDING );
			}
		}
	}

	STATE_ENTER( SentryBuild, SG_GETTING_AMMO )
	{
		SensoryMemory *sensory = GetClient()->GetSensoryMemory();
		MemoryRecords records;
		Vector3List recordpos;

		FilterAllType filter( GetClient(), AiState::SensoryMemory::EntAny, records );
		filter.MemorySpan( Utils::SecondsToMilliseconds( 7.f ) );
		filter.AddGroup( ENT_GRP_RESUPPLY );
		sensory->QueryMemory( filter );

		sensory->GetRecordInfo( records, &recordpos, NULL );

		mAmmoPack.Reset();

		if ( !recordpos.empty() )
		{
			FINDSTATEIF( Aimer, GetRootState(), ReleaseAimRequest( GetNameHash() ) );
			FINDSTATEIF( Navigator, GetRootState(), Goto( this, recordpos ) );

			const int x = GetDestinationIndex();
			mAmmoPack = records[ x ];
		}
		else
		{
			if ( mMapGoalSentry )
				BlackboardDelay( 10.f, mMapGoalSentry->GetSerialNum() );
			SetNextState( SG_DONE );
		}
	}
	STATE_EXIT( SentryBuild, SG_GETTING_AMMO )
	{
	}
	STATE_UPDATE( SentryBuild, SG_GETTING_AMMO )
	{
		if ( HasEnoughAmmo( TF_Options::BUILD_AMMO_TYPE, mNeedAmmoAmount ) )
		{
			SetNextState( SG_NONE );
			return;
		}

		if ( mAmmoPack.IsValid() )
		{
			SensoryMemory *sensory = GetClient()->GetSensoryMemory();
			if ( sensory )
			{
				const MemoryRecord *mr = sensory->GetMemoryRecord( mAmmoPack );
				if ( !mr )
				{
					mAmmoPack.Reset();
					SetNextState( SG_GETTING_AMMO );
					return;
				}
				if ( mr->mTargetInfo.mEntInfo.mFlags.CheckFlag( ENT_FLAG_DISABLED ) )
				{
					mAmmoPack.Reset();
					SetNextState( SG_GETTING_AMMO );
					return;
				}
			}
		}

		// check if the position is bad.
	}

	STATE_ENTER( SentryBuild, SG_BUILDING )
	{
		FINDSTATEIF( Navigator, GetRootState(), Goto( this ) );
	}
	STATE_EXIT( SentryBuild, SG_BUILDING )
	{
		FINDSTATEIF( Aimer, GetRootState(), ReleaseAimRequest( GetNameHash() ) );
	}
	STATE_UPDATE( SentryBuild, SG_BUILDING )
	{
		if ( GetClient()->HasEntityFlag( TF_ENT_FLAG_BUILDING_SG ) )
			return;

		FINDSTATE( sg, Sentry, GetParent() );
		if ( sg && sg->SentryFullyBuilt() )
		{
			SetNextState( SG_AIMING );
			return;
		}

		//////////////////////////////////////////////////////////////////////////
		mNeedAmmoAmount = TF_Options::SENTRY_BUILD_AMMO;
		if ( !HasEnoughAmmo( TF_Options::BUILD_AMMO_TYPE, mNeedAmmoAmount ) )
		{
			SetNextState( SG_GETTING_AMMO );
			return;
		}
		//////////////////////////////////////////////////////////////////////////

		if ( DidPathFail() )
		{
			BlackboardDelay( 10.f, mMapGoalSentry->GetSerialNum() );
			SetNextState( SG_DONE );
			return;
		}

		if ( DidPathSucceed() )
		{
			FINDSTATEIF( Aimer, GetRootState(), AddAimRequest( Priority::Medium, this, GetNameHash() ) );

			if ( BuildEquipWeapon )
				FINDSTATEIF( WeaponSystem, GetRootState(), AddWeaponRequest( Priority::Medium, GetNameHash(), BuildEquipWeapon ) );

			if ( mCantBuild )
			{
				GetClient()->SetMovementVector( -mMapGoalSentry->GetFacing() );
				GetClient()->PressButton( BOT_BUTTON_WALK );
				mNextBuildTry = 0;
			}
			else
				GetClient()->SetMovementVector( Vector3f::ZERO );
		}
	}

	STATE_ENTER( SentryBuild, SG_AIMING )
	{
		FINDSTATEIF( Aimer, GetRootState(), AddAimRequest( Priority::Medium, this, GetNameHash() ) );
	}
	STATE_EXIT( SentryBuild, SG_AIMING )
	{
		FINDSTATEIF( Aimer, GetRootState(), ReleaseAimRequest( GetNameHash() ) );
		FINDSTATEIF( WeaponSystem, GetRootState(), ReleaseWeaponRequest( GetNameHash() ) );
	}
	STATE_UPDATE( SentryBuild, SG_AIMING )
	{
	}

	STATE_ENTER( SentryBuild, SG_AIMED )
	{
	}
	STATE_EXIT( SentryBuild, SG_AIMED )
	{
	}
	STATE_UPDATE( SentryBuild, SG_AIMED )
	{
		SetNextState( SG_DONE );
	}

	STATE_ENTER( SentryBuild, SG_UPGRADING )
	{
		FINDSTATEIF( Navigator, GetRootState(), Goto( this ) );
	}
	STATE_EXIT( SentryBuild, SG_UPGRADING )
	{
		FINDSTATEIF( Aimer, GetRootState(), ReleaseAimRequest( GetNameHash() ) );
		FINDSTATEIF( WeaponSystem, GetRootState(), ReleaseWeaponRequest( GetNameHash() ) );
	}
	STATE_UPDATE( SentryBuild, SG_UPGRADING )
	{
		//////////////////////////////////////////////////////////////////////////
		mNeedAmmoAmount = TF_Options::SENTRY_UPGRADE_AMMO;
		if ( !HasEnoughAmmo( TF_Options::BUILD_AMMO_TYPE, mNeedAmmoAmount ) )
		{
			SetNextState( SG_GETTING_AMMO );
			return;
		}
		//////////////////////////////////////////////////////////////////////////

		FINDSTATE( sg, Sentry, GetRootState() );
		if ( sg && sg->GetSentryStatus().mLevel == 3 )
		{
			SetNextState( SG_DONE );
			return;
		}

		if ( DidPathSucceed() )
		{
			FINDSTATEIF( Aimer, GetRootState(), AddAimRequest( Priority::Medium, this, GetNameHash() ) );
			FINDSTATEIF( WeaponSystem, GetRootState(), AddWeaponRequest( Priority::Medium, GetNameHash(), TF_Options::SENTRY_UPGRADE_WPN ) );

			FINDSTATE( sg, Sentry, GetRootState() );
			if ( sg )
				GetClient()->GetSteeringSystem()->SetTarget( sg->GetSentryStatus().mPosition );
		}
	}

	STATE_ENTER( SentryBuild, SG_REPAIRING )
	{
		FINDSTATEIF( Navigator, GetRootState(), Goto( this ) );
	}
	STATE_EXIT( SentryBuild, SG_REPAIRING )
	{
		FINDSTATEIF( Aimer, GetRootState(), ReleaseAimRequest( GetNameHash() ) );
		FINDSTATEIF( WeaponSystem, GetRootState(), ReleaseWeaponRequest( GetNameHash() ) );
	}
	STATE_UPDATE( SentryBuild, SG_REPAIRING )
	{
		//////////////////////////////////////////////////////////////////////////
		mNeedAmmoAmount = TF_Options::SENTRY_REPAIR_AMMO;
		if ( !HasEnoughAmmo( TF_Options::BUILD_AMMO_TYPE, mNeedAmmoAmount ) )
		{
			SetNextState( SG_GETTING_AMMO );
			return;
		}
		//////////////////////////////////////////////////////////////////////////

		FINDSTATE( sg, Sentry, GetRootState() );
		if ( sg && sg->GetSentryStatus().mHealth == sg->GetSentryStatus().mMaxHealth )
		{
			SetNextState( SG_DONE );
			return;
		}

		if ( DidPathSucceed() )
		{
			FINDSTATEIF( Aimer, GetRootState(), AddAimRequest( Priority::Medium, this, GetNameHash() ) );
			FINDSTATEIF( WeaponSystem, GetRootState(), AddWeaponRequest( Priority::Medium, GetNameHash(), TF_Options::SENTRY_UPGRADE_WPN ) );

			FINDSTATE( sg, Sentry, GetRootState() );
			if ( sg )
				GetClient()->GetSteeringSystem()->SetTarget( sg->GetSentryStatus().mPosition );
		}
	}

	STATE_ENTER( SentryBuild, SG_RESUPPLY )
	{
		FINDSTATEIF( Navigator, GetRootState(), Goto( this ) );
	}
	STATE_EXIT( SentryBuild, SG_RESUPPLY )
	{
		FINDSTATEIF( Aimer, GetRootState(), ReleaseAimRequest( GetNameHash() ) );
		FINDSTATEIF( WeaponSystem, GetRootState(), ReleaseWeaponRequest( GetNameHash() ) );
	}
	STATE_UPDATE( SentryBuild, SG_RESUPPLY )
	{
		//////////////////////////////////////////////////////////////////////////
		int iShells = 0, iShellsMax = 0;
		int iSpanner = 0, iSpannerMax = 0;
		gEngineFuncs->GetCurrentAmmo( GetClient()->GetGameEntity(), TF_WP_SHOTGUN, Primary, iShells, iShellsMax );
		gEngineFuncs->GetCurrentAmmo( GetClient()->GetGameEntity(), TF_WP_SPANNER, Primary, iSpanner, iSpannerMax );
		if ( iSpanner == 0 || iShells == 0 )
		{
			mNeedAmmoAmount = 200;
			SetNextState( SG_GETTING_AMMO );
			return;
		}
		//////////////////////////////////////////////////////////////////////////

		FINDSTATE( sg, Sentry, GetRootState() );
		if ( sg )
		{
			if ( sg->GetSentryStatus().mShells[ 0 ] == sg->GetSentryStatus().mShells[ 1 ] &&
				 sg->GetSentryStatus().mRockets[ 0 ] == sg->GetSentryStatus().mRockets[ 1 ] )
			{
				SetNextState( SG_DONE );
				return;
			}
		}

		if ( DidPathSucceed() )
		{
			FINDSTATEIF( Aimer, GetRootState(), AddAimRequest( Priority::Medium, this, GetNameHash() ) );
			FINDSTATEIF( WeaponSystem, GetRootState(), AddWeaponRequest( Priority::Medium, GetNameHash(), TF_Options::SENTRY_UPGRADE_WPN ) );

			FINDSTATE( sg, Sentry, GetRootState() );
			if ( sg )
				GetClient()->GetSteeringSystem()->SetTarget( sg->GetSentryStatus().mPosition );
		}
	}

	STATE_ENTER( SentryBuild, SG_DONE )
	{
	}
	STATE_EXIT( SentryBuild, SG_DONE )
	{
	}
	STATE_UPDATE( SentryBuild, SG_DONE )
	{
	}

	void SentryBuild::GetDebugString( std::stringstream &out )
	{
		if ( IsActive() )
		{
			switch ( GetCurrentStateId() )
			{
			case SG_NONE:
				break;
			case SG_GETTING_AMMO:
				out << "Getting Ammo ";
				break;
			case SG_BUILDING:
				out << (mCantBuild ? "Cant Build " : "Building ");
				break;
			case SG_AIMING:
				out << "Aiming ";
				break;
			case SG_AIMED:
				out << "Aimed ";
				break;
			case SG_UPGRADING:
				out << "Upgrading ";
				break;
			case SG_REPAIRING:
				out << "Repairing ";
				break;
			case SG_RESUPPLY:
				out << "Resupply ";
				break;
			case SG_DONE:
				out << "Done ";
				break;
			}

			if ( mBuiltSentry )
				out << mBuiltSentry->GetName() << " ";
			else if ( mMapGoalSentry )
				out << mMapGoalSentry->GetName() << " ";

			FINDSTATE( sg, Sentry, GetParent() );
			if ( sg && sg->HasSentry() && sg->SentryFullyBuilt() )
			{
				const Sentry::SentryStatus &ss = sg->GetSentryStatus();
				const float hlthpc = 100.f * ((float)ss.mHealth / (float)ss.mMaxHealth);

				out << std::setprecision( 3 );
				out << "H(" << hlthpc << ") ";
				out << "S(" << ss.mShells[ 0 ] << "/" << ss.mShells[ 1 ] << ") ";
				out << "R(" << ss.mRockets[ 0 ] << "/" << ss.mRockets[ 1 ] << ") ";
			}
		}
	}
	bool SentryBuild::GetNextDestination( DestinationVector &_desination, bool &_final, bool &_skiplastpt )
	{
		FINDSTATE( sg, Sentry, GetParent() );
		if ( sg && sg->HasSentry() )
		{
			_desination.push_back( Destination( sg->GetSentryStatus().mPosition, 64.f ) );
			_final = true;
		}
		else
		{
			if ( mMapGoalSentry->RouteTo( GetClient(), _desination ) )
				_final = false;
			else
				_final = true;
		}
		return true;
	}
	bool SentryBuild::GetAimPosition( Vector3f &_aimpos )
	{
		switch ( GetCurrentStateId() )
		{
		case SG_BUILDING:
		{
			_aimpos = GetClient()->GetEyePosition() + mMapGoalSentry->GetFacing() * 1024.f;
			break;
		}
		case SG_AIMING:
		{
			_aimpos = GetClient()->GetEyePosition() - mMapGoalSentry->GetFacing() * 1024.f;

			Vector3f vAimPos;
			if ( mMapGoalSentry->GetProperty( "AimPosition", vAimPos ) )
			{
				if ( !vAimPos.IsZero() )
					_aimpos = vAimPos;
				else
					SetNextState( SG_AIMED );
			}
			break;
		}
		case SG_UPGRADING:
		case SG_REPAIRING:
		case SG_RESUPPLY:
		{
			FINDSTATE( sg, Sentry, GetParent() );
			if ( sg )
			{
				_aimpos = sg->GetSentryStatus().mPosition;
				return true;
			}
		}
		}
		return true;
	}
	void SentryBuild::OnTarget()
	{
		FINDSTATE( sg, Sentry, GetParent() );
		if ( !sg )
			return;

		switch ( GetCurrentStateId() )
		{
		case SG_BUILDING:
		{
			if ( !sg->HasSentry() && IGame::GetTime() >= mNextBuildTry )
			{
				GetClient()->PressButton( TF_BOT_BUTTON_BUILDSENTRY );
				mNextBuildTry = IGame::GetTime() + TF_Options::BUILD_ATTEMPT_DELAY;
			}
			break;
		}
		case SG_AIMING:
		{
			GetClient()->PressButton( TF_BOT_BUTTON_AIMSENTRY );
			SetNextState( SG_AIMED );
			break;
		}
		case SG_UPGRADING:
		case SG_REPAIRING:
		case SG_RESUPPLY:
		{
			FINDSTATE( ws, WeaponSystem, GetRootState() );
			if ( ws && ws->CurrentWeaponIs( TF_Options::SENTRY_UPGRADE_WPN ) )
				ws->FireWeapon();
			break;
		}
		}
	}
	float SentryBuild::GetPriority()
	{
		// If we built somewhere, but that spot is now unavailable, abandon it.
		if ( mBuiltSentry )
		{
			if ( !mBuiltSentry->IsAvailable( GetClient()->GetTeam() ) )
			{
				GetClient()->PressButton( TF_BOT_BUTTON_DETSENTRY );
				mBuiltSentry.reset();
			}
		}

		if ( !mMapGoalSentry )
		{
			FINDSTATE( sg, Sentry, GetParent() );
			if ( sg && sg->HasSentry() && sg->SentryFullyBuilt() )
			{
				if ( IsActive() )
					return mSentryPriority;

				if ( sg->GetSentryStatus().mLevel < 3 )
				{
					return mSentryPriority;
				}

				if ( sg->GetSentryStatus().mHealth < sg->GetSentryStatus().mMaxHealth )
				{
					return mSentryPriority;
				}

				const Sentry::SentryStatus &status = sg->GetSentryStatus();
				if ( status.mRockets[ 0 ] < status.mRockets[ 1 ] / 2 ||
					 status.mShells[ 0 ] < status.mShells[ 1 ] / 2 )
				{
					return mSentryPriority;
				}
				return 0.f;
			}

			GoalManager::Query qry( 0xca96590a /* sentry */, GetClient() );
			System::mInstance->mGoalManager->GetGoals( qry );
			qry.GetBest( mMapGoalSentry );
		}
		mSentryPriority = mMapGoalSentry ? mMapGoalSentry->GetPriorityForClient( GetClient() ) : 0.f;
		return mSentryPriority;
	}
	void SentryBuild::Enter()
	{
		mNextBuildTry = 0;
		mCantBuild = false;

		SetNextState( SG_NONE );
	}
	void SentryBuild::Exit()
	{
		FINDSTATEIF( Navigator, GetRootState(), Stop( true ) );

		SetNextState( SG_NONE );
		UpdateFsm( 0.f );

		mMapGoalSentry.reset();
		FINDSTATEIF( Aimer, GetRootState(), ReleaseAimRequest( GetNameHash() ) );
		GetClient()->PressButton( TF_BOT_BUTTON_CANCELBUILD );
	}
	State::StateStatus SentryBuild::Update( float fDt )
	{
		UpdateFsm( fDt );

		if ( GetCurrentStateId() == SG_DONE )
			return State_Finished;

		if ( DidPathFail() )
		{
			// Delay it from being used for a while.
			if ( mMapGoalSentry )
				BlackboardDelay( 10.f, mMapGoalSentry->GetSerialNum() );
			return State_Finished;
		}

		return State_Busy;
	}
	void SentryBuild::ProcessEvent( const Message & message, CallbackParameters & cb )
	{
		switch ( message.Id() )
		{
			CASE_MSG( EvEntityBuilding )
			{
				if ( msg->mClassId == TF_CLASSEX_SENTRY )
				{
					if ( mMapGoalSentry )
						mBuiltSentry = mMapGoalSentry;
				}
				break;
			}
			CASE_MSG( EvEntityBuilt )
			{
				if ( msg->mClassId == TF_CLASSEX_SENTRY )
				{
					if ( mMapGoalSentry )
						mBuiltSentry = mMapGoalSentry;
				}
				break;
			}
			CASE_MSG( EvEntityBuildCancel )
			{
				if ( msg->mClassId == TF_CLASSEX_SENTRY )
				{
					mBuiltSentry.reset();
				}
				break;
			}
			CASE_MSG( EvEntityBuildDestroyed )
			{
				if ( msg->mClassId == TF_CLASSEX_SENTRY )
				{
					mBuiltSentry.reset();
				}
				break;
			}
			CASE_MSG( EvEntityBuildDetonated )
			{
				if ( msg->mClassId == TF_CLASSEX_SENTRY )
				{
					mBuiltSentry.reset();
				}
				break;
			}
			CASE_MSG( EvEntityBuildDismantled )
			{
				if ( msg->mClassId == TF_CLASSEX_SENTRY )
				{
					mBuiltSentry.reset();
				}
				break;
			}
			CASE_MSG( EvEntityCantBuild )
			{
				if ( msg->mClassId == TF_CLASSEX_SENTRY )
				{
					mCantBuild = true;
				}
				break;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	SentryAlly::SentryAlly()
		: StateChild( "SentryAlly" )
		, FollowPathUser( "SentryAlly" )
	{
	}
	void SentryAlly::GetDebugString( std::stringstream &out )
	{
		if ( IsActive() )
		{
			switch ( mState )
			{
			case UPGRADING:
				out << "Upgrading Sentry ";
				break;
			case REPAIRING:
				out << "Repairing Sentry ";
				break;
			case RESUPPLY:
				out << "Resupply Sentry ";
				break;
			}

			const MemoryRecord *rec = GetClient()->GetSensoryMemory()->GetMemoryRecord( mAllySentry );
			if ( rec )
			{
				GameEntity owner = EngineFuncs::EntityOwner( rec->GetEntity() );
				if ( owner.IsValid() )
				{
					std::string ownerName = EngineFuncs::EntityName( owner );
					if ( !ownerName.empty() )
						out << "Owner: " << ownerName;
				}
			}
		}
	}
	bool SentryAlly::GetNextDestination( DestinationVector &_desination, bool &_final, bool &_skiplastpt )
	{
		const MemoryRecord *rec = GetClient()->GetSensoryMemory()->GetMemoryRecord( mAllySentry );
		if ( rec )
		{
			_desination.push_back( Destination( rec->GetLastSensedPosition(), 64.f ) );
			_final = true;
			return true;
		}
		return false;
	}
	bool SentryAlly::GetAimPosition( Vector3f &_aimpos )
	{
		const MemoryRecord *rec = GetClient()->GetSensoryMemory()->GetMemoryRecord( mAllySentry );
		_aimpos = rec->GetLastSensedPosition();
		return rec != NULL;
	}
	void SentryAlly::OnTarget()
	{
		FINDSTATE( ws, WeaponSystem, GetRootState() );
		if ( ws && ws->CurrentWeaponIs( TF_Options::SENTRY_UPGRADE_WPN ) )
			ws->FireWeapon();
	}
	float SentryAlly::GetPriority()
	{
		if ( IsActive() )
			return GetLastPriority();

		FINDSTATE( sensory, SensoryMemory, GetRootState() );
		if ( sensory )
		{
			FINDSTATE( sg, Sentry, GetParent() );

			int iAmmo = 0, iMaxAmmo = 0;
			gEngineFuncs->GetCurrentAmmo( GetClient()->GetGameEntity(), TF_Options::BUILD_AMMO_TYPE, Primary, iAmmo, iMaxAmmo );

			MemoryRecords rcs;
			FilterAllType filter( GetClient(), AiState::SensoryMemory::EntAny, rcs );
			filter.MemorySpan( Utils::SecondsToMilliseconds( 7.f ) );
			filter.AddClass( TF_CLASSEX_SENTRY );
			sensory->QueryMemory( filter );

			for ( uint32_t i = 0; i < rcs.size(); ++i )
			{
				const MemoryRecord *pRec = sensory->GetMemoryRecord( rcs[ i ] );
				if ( pRec )
				{
					if ( sg && sg->HasSentry() && sg->GetSentryStatus().mEntity == pRec->GetEntity() )
						continue;

					const BitFlag64 entflags = pRec->mTargetInfo.mEntInfo.mFlags;
					if ( !entflags.CheckFlag( TF_ENT_FLAG_BUILDINPROGRESS ) && iAmmo >= TF_Options::SENTRY_UPGRADE_AMMO )
					{
						mState = UPGRADING;
						mAllySentry = rcs[ i ];
						return GetLastPriority();
					}

					if ( pRec->mTargetInfo.mEntInfo.mHealth.Percent() && iAmmo >= TF_Options::SENTRY_REPAIR_AMMO )
					{
						mState = REPAIRING;
						mAllySentry = rcs[ i ];
						return GetLastPriority();
					}
				}
			}
		}
		return 0.f;
	}
	void SentryAlly::Enter()
	{
		FINDSTATEIF( Navigator, GetRootState(), Goto( this ) );
	}
	void SentryAlly::Exit()
	{
		FINDSTATEIF( Navigator, GetRootState(), Stop( true ) );
		FINDSTATEIF( Aimer, GetRootState(), ReleaseAimRequest( GetNameHash() ) );
		FINDSTATEIF( WeaponSystem, GetRootState(), ReleaseWeaponRequest( GetNameHash() ) );
	}
	State::StateStatus SentryAlly::Update( float fDt )
	{
		//////////////////////////////////////////////////////////////////////////
		if ( DidPathFail() )
		{
			// Delay it from being used for a while.
			//BlackboardDelay(10.f, mMapGoalSentry->GetSerialNum());
			return State_Finished;
		}
		//////////////////////////////////////////////////////////////////////////

		// Abandon ally upgrade if it gets upgraded to l3 in the mean time.
		const MemoryRecord *rec = GetClient()->GetSensoryMemory()->GetMemoryRecord( mAllySentry );
		if ( !rec )
			return State_Finished;

		int iAmmo = 0, iMaxAmmo = 0;
		gEngineFuncs->GetCurrentAmmo( GetClient()->GetGameEntity(), TF_Options::BUILD_AMMO_TYPE, Primary, iAmmo, iMaxAmmo );

		switch ( mState )
		{
		case UPGRADING:
		{
			if ( iAmmo < TF_Options::SENTRY_UPGRADE_AMMO )
				return State_Finished;

			if ( rec->mTargetInfo.mEntInfo.mClassId == TF_CLASSEX_SENTRY )
				return State_Finished;
			break;
		}
		case REPAIRING:
		{
			if ( iAmmo < TF_Options::SENTRY_REPAIR_AMMO )
				return State_Finished;

			if ( rec->mTargetInfo.mEntInfo.mHealth.Percent() >= 1.0 )
			{
				return State_Finished;
			}
			break;
		}
		case RESUPPLY:
		{
			break;
		}
		}

		if ( DidPathSucceed() )
		{
			FINDSTATEIF( Aimer, GetRootState(), AddAimRequest( Priority::Medium, this, GetNameHash() ) );
			FINDSTATEIF( WeaponSystem, GetRootState(), AddWeaponRequest( Priority::Medium, GetNameHash(), TF_Options::SENTRY_UPGRADE_WPN ) );

			GetClient()->GetSteeringSystem()->SetTarget( rec->mTargetInfo.mLastPosition );
		}
		return State_Busy;
	}
	//////////////////////////////////////////////////////////////////////////
	int DispenserBuild::BuildEquipWeapon = TF_WP_NONE;

	DispenserBuild::DispenserBuild()
		: StateChild( "DispenserBuild" )
		, FollowPathUser( "DispenserBuild" )
		, mCantBuild( false )
	{
	}
	void DispenserBuild::GetDebugString( std::stringstream &out )
	{
		if ( IsActive() )
		{
			switch ( mState )
			{
			default:
			case DISP_NONE:
				break;
			case DISP_GETTING_AMMO:
				out << "Getting Ammo";
				break;
			case DISP_BUILDING:
				out << (mCantBuild ? "Cant Build " : "Building ");
				if ( mMapGoalDisp )
					out << mMapGoalDisp->GetName();
				break;
			}
		}
	}
	bool DispenserBuild::GetNextDestination( DestinationVector &_desination, bool &_final, bool &_skiplastpt )
	{
		if ( mMapGoalDisp->RouteTo( GetClient(), _desination ) )
			_final = false;
		else
			_final = true;
		return true;
	}
	bool DispenserBuild::GetAimPosition( Vector3f &_aimpos )
	{
		switch ( mState )
		{
		case DISP_BUILDING:
			_aimpos = GetClient()->GetEyePosition() + mMapGoalDisp->GetFacing() * 1024.f;
			break;
		default:
			return false;
		}
		return true;
	}
	void DispenserBuild::OnTarget()
	{
		FINDSTATE( disp, Dispenser, GetParent() );
		if ( !disp )
			return;

		switch ( mState )
		{
		case DISP_BUILDING:
		{
			if ( !disp->HasDispenser() && IGame::GetTime() >= mNextBuildTry )
			{
				GetClient()->PressButton( TF_BOT_BUTTON_BUILDDISPENSER );
				mNextBuildTry = IGame::GetTime() + TF_Options::BUILD_ATTEMPT_DELAY;
			}
			break;
		}
		default:
			break;
		}
	}
	float DispenserBuild::GetPriority()
	{
		if ( mBuiltDisp )
		{
			if ( !mBuiltDisp->IsAvailable( GetClient()->GetTeam() ) )
			{
				GetClient()->PressButton( TF_BOT_BUTTON_DETDISPENSER );
				mBuiltDisp.reset();
			}
		}

		if ( !mMapGoalDisp )
		{
			FINDSTATE( disp, Dispenser, GetParent() );
			if ( disp && disp->HasDispenser() )
				return 0.f;

			GoalManager::Query qry( 0x4961e1a8 /* dispenser */, GetClient() );
			System::mInstance->mGoalManager->GetGoals( qry );
			qry.GetBest( mMapGoalDisp );
		}
		return mMapGoalDisp ? mMapGoalDisp->GetPriorityForClient( GetClient() ) : 0.f;
	}
	void DispenserBuild::Enter()
	{
		mNextAmmoCheck = 0;
		mNextBuildTry = 0;
		mState = DISP_BUILDING;
		mCantBuild = false;
		FINDSTATEIF( Navigator, GetRootState(), Goto( this ) );
	}
	void DispenserBuild::Exit()
	{
		FINDSTATEIF( Navigator, GetRootState(), Stop( true ) );
		mState = DISP_NONE;
		mMapGoalDisp.reset();
		FINDSTATEIF( Aimer, GetRootState(), ReleaseAimRequest( GetNameHash() ) );
		GetClient()->PressButton( TF_BOT_BUTTON_CANCELBUILD );
		FINDSTATEIF( WeaponSystem, GetRootState(), ReleaseWeaponRequest( GetNameHash() ) );
	}
	State::StateStatus DispenserBuild::Update( float fDt )
	{
		//////////////////////////////////////////////////////////////////////////
		if ( DidPathFail() )
		{
			// Delay it from being used for a while.
			BlackboardDelay( 10.f, mMapGoalDisp->GetSerialNum() );
			return State_Finished;
		}
		//////////////////////////////////////////////////////////////////////////
		if ( GetClient()->HasEntityFlag( TF_ENT_FLAG_BUILDING_DISP ) )
			return State_Busy;

		FINDSTATE( disp, Dispenser, GetParent() );
		if ( disp && disp->DispenserFullyBuilt() )
		{
			mBuiltDisp = mMapGoalDisp;
			return State_Finished;
		}
		//////////////////////////////////////////////////////////////////////////
		// Need ammo?
		if ( IGame::GetTime() >= mNextAmmoCheck )
		{
			mNextAmmoCheck = IGame::GetTime() + 1000;

			int iAmmo = 0, iMaxAmmo = 0;
			gEngineFuncs->GetCurrentAmmo( GetClient()->GetGameEntity(), TF_Options::BUILD_AMMO_TYPE, Primary, iAmmo, iMaxAmmo );
			if ( mState == DISP_GETTING_AMMO && iAmmo >= TF_Options::DISPENSER_BUILD_AMMO )
				return State_Finished;
			if ( mState != DISP_GETTING_AMMO && iAmmo < TF_Options::DISPENSER_BUILD_AMMO )
			{
				SensoryMemory *sensory = GetClient()->GetSensoryMemory();

				MemoryRecords records;
				Vector3List recordpos;

				FilterAllType filter( GetClient(), AiState::SensoryMemory::EntAny, records );
				filter.MemorySpan( Utils::SecondsToMilliseconds( 7.f ) );
				filter.AddGroup( ENT_GRP_RESUPPLY );
				sensory->QueryMemory( filter );

				sensory->GetRecordInfo( records, &recordpos, NULL );

				if ( !recordpos.empty() )
				{
					mState = DISP_GETTING_AMMO;
					FINDSTATEIF( Aimer, GetRootState(), ReleaseAimRequest( GetNameHash() ) );
					FINDSTATEIF( Navigator, GetRootState(), Goto( this, recordpos ) );
				}
				else
				{
					//BlackboardDelay(10.f, mMapGoalSentry->GetSerialNum());
					return State_Finished;
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////
		if ( mState == DISP_BUILDING && !mMapGoalDisp->IsAvailable( GetClient()->GetTeam() ) )
			return State_Finished;

		if ( DidPathSucceed() )
		{
			if ( mState == DISP_GETTING_AMMO )
				return State_Finished;

			FINDSTATEIF( Aimer, GetRootState(), AddAimRequest( Priority::Medium, this, GetNameHash() ) );
			if ( BuildEquipWeapon )
				FINDSTATEIF( WeaponSystem, GetRootState(), AddWeaponRequest( Priority::Medium, GetNameHash(), BuildEquipWeapon ) );

			if ( mCantBuild )
			{
				GetClient()->SetMovementVector( -mMapGoalDisp->GetFacing() );
				GetClient()->PressButton( BOT_BUTTON_WALK );
				mNextBuildTry = 0;
			}
			else
				GetClient()->SetMovementVector( Vector3f::ZERO );
		}
		return State_Busy;
	}
	void DispenserBuild::ProcessEvent( const Message & message, CallbackParameters & cb )
	{
		switch ( message.Id() )
		{
			CASE_MSG( EvEntityBuilding )
			{
				if ( msg->mClassId == TF_CLASSEX_DISPENSER )
				{
					if ( mMapGoalDisp )
						mBuiltDisp = mMapGoalDisp;
				}
				break;
			}
			CASE_MSG( EvEntityBuilt )
			{
				if ( msg->mClassId == TF_CLASSEX_DISPENSER )
				{
					if ( mMapGoalDisp )
						mBuiltDisp = mMapGoalDisp;
				}
				break;
			}
			CASE_MSG( EvEntityBuildCancel )
			{
				if ( msg->mClassId == TF_CLASSEX_DISPENSER )
				{
					mBuiltDisp.reset();
				}
				break;
			}
			CASE_MSG( EvEntityBuildDestroyed )
			{
				if ( msg->mClassId == TF_CLASSEX_DISPENSER )
				{
					mBuiltDisp.reset();
				}
				break;
			}
			CASE_MSG( EvEntityBuildDetonated )
			{
				if ( msg->mClassId == TF_CLASSEX_DISPENSER )
				{
					mBuiltDisp.reset();
				}
				break;
			}
			CASE_MSG( EvEntityBuildDismantled )
			{
				if ( msg->mClassId == TF_CLASSEX_DISPENSER )
				{
					mBuiltDisp.reset();
				}
				break;
			}
			CASE_MSG( EvEntityCantBuild )
			{
				if ( msg->mClassId == TF_CLASSEX_DISPENSER )
				{
					mCantBuild = true;
				}
				break;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Sentry::Sentry() : StatePrioritized( "Sentry" )
	{
		AppendState( new SentryBuild );
		AppendState( new SentryAlly );

		LimitToClass().SetFlag( TF_CLASS_ENGINEER );
		SetAlwaysRecieveEvents( true );
	}
	void Sentry::GetDebugString( std::stringstream &out )
	{
		switch ( mSentryStatus.mStatus )
		{
		case BUILDABLE_BUILDING:
		case BUILDABLE_BUILT:
			out << mSentryStatus.mHealth << "/" << mSentryStatus.mMaxHealth;
			break;
		case BUILDABLE_INVALID:
		default:
			out << "<none>";
			break;
		}
	}
	void Sentry::UpdateSentryStatus( const ParamsSentryStatus_TF & stats )
	{
		if ( stats.mEntity.IsValid() )
		{
			if ( mSentryStatus.mEntity != stats.mEntity )
			{
				EvEntityBuilding::Msg msgBuild;
				msgBuild.mData.mClassId = TF_CLASSEX_SENTRY;
				msgBuild.mData.mEntity = stats.mEntity;
				GetClient()->SendEvent( msgBuild );

				EvEntityBuilt::Msg msgBuilt;
				msgBuilt.mData.mClassId = TF_CLASSEX_SENTRY;
				msgBuilt.mData.mEntity = stats.mEntity;
				GetClient()->SendEvent( msgBuilt );
			}

			mSentryStatus.mHealth = stats.mHealth;
			mSentryStatus.mMaxHealth = stats.mMaxHealth;
			mSentryStatus.mShells[ 0 ] = stats.mShells[ 0 ];
			mSentryStatus.mShells[ 1 ] = stats.mShells[ 1 ];
			mSentryStatus.mRockets[ 0 ] = stats.mRockets[ 0 ];
			mSentryStatus.mRockets[ 1 ] = stats.mRockets[ 1 ];
			mSentryStatus.mLevel = stats.mLevel;

			/*DBG_MSG(0, GetClient(), kNormal, "Sentry Stats");
			DBG_MSG(0, GetClient(), kNormal, va("Level: %d", mSentryStatus.mLevel));
			DBG_MSG(0, GetClient(), kNormal, va("Health: %d/%d", mSentryStatus.mHealth, mSentryStatus.mMaxHealth));
			DBG_MSG(0, GetClient(), kNormal, va("Shells: %d/%d", mSentryStatus.mShells[0], mSentryStatus.mShells[1]));
			DBG_MSG(0, GetClient(), kNormal, va("Rockets: %d/%d", mSentryStatus.mRockets[0], mSentryStatus.mRockets[1]));*/
		}
		else
		{
			if ( mSentryStatus.mEntity != stats.mEntity )
			{
				EvEntityBuildDestroyed::Msg msg;
				msg.mData.mClassId = TF_CLASSEX_SENTRY;
				msg.mData.mEntity = mSentryStatus.mEntity;
				GetClient()->SendEvent( msg );
			}
		}
	}
	State::StateStatus Sentry::Update( float fDt )
	{
		return State_Busy;
	}
	void Sentry::ProcessEvent( const Message & message, CallbackParameters & cb )
	{
		switch ( message.Id() )
		{
			CASE_MSG( EvEntityBuilding )
			{
				if ( msg->mClassId == TF_CLASSEX_SENTRY )
				{
					mSentryStatus.Reset();
					mSentryStatus.mStatus = BUILDABLE_BUILDING;
					mSentryStatus.mEntity = msg->mEntity;
					DBG_MSG( 0, GetClient(), kNormal, "Sentry Building" );
				}
				break;
			}
			CASE_MSG( EvEntityBuilt )
			{
				if ( msg->mClassId == TF_CLASSEX_SENTRY )
				{
					mSentryStatus.mEntity = msg->mEntity;
					mSentryStatus.mLevel = 1;

					EngineFuncs::EntityPosition( msg->mEntity, mSentryStatus.mPosition );
					EngineFuncs::EntityOrientation( msg->mEntity, mSentryStatus.mFacing, NULL, NULL );
					mSentryStatus.mStatus = BUILDABLE_BUILT;
					DBG_MSG( 0, GetClient(), kNormal, "Sentry Built" );
				}
				break;
			}
			CASE_MSG( EvEntityBuildCancel )
			{
				if ( msg->mClassId == TF_CLASSEX_SENTRY )
				{
					mSentryStatus.Reset();
				}
				break;
			}
			CASE_MSG( EvEntityBuildDestroyed )
			{
				if ( msg->mClassId == TF_CLASSEX_SENTRY )
				{
					mSentryStatus.Reset();
					DBG_MSG( 0, GetClient(), kNormal, "Sentry Destroyed" );
				}
				break;
			}
			CASE_MSG( EvEntityBuildDetonated )
			{
				if ( msg->mClassId == TF_CLASSEX_SENTRY )
				{
					mSentryStatus.Reset();
					DBG_MSG( 0, GetClient(), kNormal, "Sentry Detonated" );
				}
				break;
			}
			CASE_MSG( EvEntityBuildDismantled )
			{
				if ( msg->mClassId == TF_CLASSEX_SENTRY )
				{
					mSentryStatus.Reset();
					DBG_MSG( 0, GetClient(), kNormal, "Sentry Dismantled" );
				}
				break;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Dispenser::Dispenser() : StatePrioritized( "Dispenser" )
	{
		AppendState( new DispenserBuild );

		LimitToClass().SetFlag( TF_CLASS_ENGINEER );
		SetAlwaysRecieveEvents( true );
	}
	void Dispenser::GetDebugString( std::stringstream &out )
	{
		switch ( mDispenserStatus.mStatus )
		{
		case BUILDABLE_BUILDING:
		case BUILDABLE_BUILT:
			out << mDispenserStatus.mHealth;
			return;
		case BUILDABLE_INVALID:
		default:
			out << "<none>";
			break;
		}
	}
	void Dispenser::UpdateDispenserStatus( const ParamsDispenserStatus_TF & stats )
	{
		if ( stats.mEntity.IsValid() )
		{
			if ( mDispenserStatus.mEntity != stats.mEntity )
			{
				EvEntityBuilding::Msg msgBuild;
				msgBuild.mData.mClassId = TF_CLASSEX_DISPENSER;
				msgBuild.mData.mEntity = stats.mEntity;
				GetClient()->SendEvent( msgBuild );

				EvEntityBuilt::Msg msgBuilt;
				msgBuilt.mData.mClassId = TF_CLASSEX_DISPENSER;
				msgBuilt.mData.mEntity = stats.mEntity;
				GetClient()->SendEvent( msgBuilt );
			}
			
			mDispenserStatus.mHealth = stats.mHealth;
			mDispenserStatus.mShells = stats.mShells;
			mDispenserStatus.mNails = stats.mNails;
			mDispenserStatus.mRockets = stats.mRockets;
			mDispenserStatus.mCells = stats.mCells;
			mDispenserStatus.mArmor = stats.mArmor;

			/*DBG_MSG(0, GetClient(), kNormal, "Dispenser Stats");
			DBG_MSG(0, GetClient(), kNormal, va("Health: %d", mDispenserStatus.mHealth));
			DBG_MSG(0, GetClient(), kNormal, va("Shells: %d", mDispenserStatus.mShells));
			DBG_MSG(0, GetClient(), kNormal, va("Nails: %d", mDispenserStatus.mNails));
			DBG_MSG(0, GetClient(), kNormal, va("Rockets: %d", mDispenserStatus.mRockets));
			DBG_MSG(0, GetClient(), kNormal, va("Cells: %d", mDispenserStatus.mCells));
			DBG_MSG(0, GetClient(), kNormal, va("Armor: %d", mDispenserStatus.mArmor));*/
		}
		else
		{
			if ( mDispenserStatus.mEntity != stats.mEntity )
			{
				EvEntityBuildDestroyed::Msg msg;
				msg.mData.mClassId = TF_CLASSEX_DISPENSER;
				msg.mData.mEntity = mDispenserStatus.mEntity;
				GetClient()->SendEvent( msg );
			}
		}
	}
	State::StateStatus Dispenser::Update( float fDt )
	{
		return State_Busy;
	}
	void Dispenser::ProcessEvent( const Message & message, CallbackParameters & cb )
	{
		switch ( message.Id() )
		{
			CASE_MSG( EvEntityBuilding )
			{
				if ( msg->mClassId == TF_CLASSEX_DISPENSER )
				{
					mDispenserStatus.Reset();
					mDispenserStatus.mStatus = BUILDABLE_BUILDING;
					mDispenserStatus.mEntity = msg->mEntity;
					DBG_MSG( 0, GetClient(), kNormal, "Dispenser Building" );
				}
				break;
			}
			CASE_MSG( EvEntityBuilt )
			{
				if ( msg->mClassId == TF_CLASSEX_DISPENSER )
				{
					mDispenserStatus.mEntity = msg->mEntity;
					EngineFuncs::EntityPosition( msg->mEntity, mDispenserStatus.mPosition );
					EngineFuncs::EntityOrientation( msg->mEntity, mDispenserStatus.mFacing, NULL, NULL );
					mDispenserStatus.mStatus = BUILDABLE_BUILT;
					DBG_MSG( 0, GetClient(), kNormal, "Dispenser Built" );
				}
				break;
			}
			CASE_MSG( EvEntityBuildCancel )
			{
				if ( msg->mClassId == TF_CLASSEX_DISPENSER )
				{
					mDispenserStatus.Reset();
				}
				break;
			}
			CASE_MSG( EvEntityBuildDestroyed )
			{
				if ( msg->mClassId == TF_CLASSEX_DISPENSER )
				{
					mDispenserStatus.Reset();
					DBG_MSG( 0, GetClient(), kNormal, "Dispenser Destroyed" );
				}
				break;
			}
			CASE_MSG( EvEntityBuildDetonated )
			{
				if ( msg->mClassId == TF_CLASSEX_DISPENSER )
				{
					mDispenserStatus.Reset();
					DBG_MSG( 0, GetClient(), kNormal, "Dispenser Detonated" );
				}
				break;
			}
			CASE_MSG( EvEntityBuildDismantled )
			{
				if ( msg->mClassId == TF_CLASSEX_DISPENSER )
				{
					mDispenserStatus.Reset();
					DBG_MSG( 0, GetClient(), kNormal, "Dispenser Dismantled" );
				}
				break;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	DetpackBuild::DetpackBuild()
		: StateChild( "DetpackBuild" )
		, FollowPathUser( "DetpackBuild" )
		, mDetpackFuse( TF_BOT_BUTTON_BUILDDETPACK_5 )
		, mCantBuild( false )
	{
	}
	void DetpackBuild::GetDebugString( std::stringstream &out )
	{
		if ( IsActive() )
		{
			switch ( mState )
			{
			case NONE:
				break;
			case DETPACK_BUILDING:
				out << (mCantBuild ? "Cant Build " : "Building ");
				if ( mMapGoal )
					out << mMapGoal->GetName();
			}
		}
	}
	bool DetpackBuild::GetNextDestination( DestinationVector &_desination, bool &_final, bool &_skiplastpt )
	{
		if ( mMapGoal->RouteTo( GetClient(), _desination ) )
			_final = false;
		else
			_final = true;
		return true;
	}
	bool DetpackBuild::GetAimPosition( Vector3f &_aimpos )
	{
		switch ( mState )
		{
		case DETPACK_BUILDING:
			_aimpos = GetClient()->GetEyePosition() + mMapGoal->GetFacing() * 1024.f;
			break;
		default:
			return false;
		}
		return true;
	}
	void DetpackBuild::OnTarget()
	{
		FINDSTATE( detp, Detpack, GetParent() );
		if ( !detp )
			return;

		switch ( mState )
		{
		case DETPACK_BUILDING:
		{
			if ( !detp->HasDetpack() && IGame::GetTime() >= mNextBuildTry )
			{
				GetClient()->PressButton( TF_BOT_BUTTON_BUILDDETPACK_5 );
				mNextBuildTry = IGame::GetTime() + TF_Options::BUILD_ATTEMPT_DELAY;
			}
			break;
		}
		default:
			break;
		}
	}
	float DetpackBuild::GetPriority()
	{
		if ( !mMapGoal )
		{
			FINDSTATE( detp, Detpack, GetParent() );
			if ( detp && detp->HasDetpack() )
				return 0.f;

			int iDetpacks = 0, iMaxDetpacks = 0;
			gEngineFuncs->GetCurrentAmmo( GetClient()->GetGameEntity(), TF_WP_DETPACK, Primary, iDetpacks, iMaxDetpacks );
			if ( iDetpacks == 0 )
				return 0.f;

			GoalManager::Query qry( 0x3b15b60b /* detpack */, GetClient() );
			System::mInstance->mGoalManager->GetGoals( qry );
			qry.GetBest( mMapGoal );
		}
		return mMapGoal ? mMapGoal->GetPriorityForClient( GetClient() ) : 0.f;
	}
	void DetpackBuild::Enter()
	{
		mNextBuildTry = 0;
		mState = DETPACK_BUILDING;
		mCantBuild = false;

		float fuse = 5.f;
		if ( mMapGoal->GetProperty( "Fuse", fuse ) )
		{
			if ( fuse > 30.f )
				mDetpackFuse = TF_BOT_BUTTON_BUILDDETPACK_30;
			else if ( fuse > 20.f )
				mDetpackFuse = TF_BOT_BUTTON_BUILDDETPACK_20;
			else if ( fuse > 10.f )
				mDetpackFuse = TF_BOT_BUTTON_BUILDDETPACK_10;
			else
				mDetpackFuse = TF_BOT_BUTTON_BUILDDETPACK_5;
		}

		FINDSTATEIF( Navigator, GetRootState(), Goto( this ) );
	}
	void DetpackBuild::Exit()
	{
		FINDSTATEIF( Navigator, GetRootState(), Stop( true ) );
		mState = NONE;
		mMapGoal.reset();
		FINDSTATEIF( Aimer, GetRootState(), ReleaseAimRequest( GetNameHash() ) );
		GetClient()->PressButton( TF_BOT_BUTTON_CANCELBUILD );
	}
	State::StateStatus DetpackBuild::Update( float fDt )
	{
		//////////////////////////////////////////////////////////////////////////
		if ( DidPathFail() )
		{
			// Delay it from being used for a while.
			BlackboardDelay( 10.f, mMapGoal->GetSerialNum() );
			return State_Finished;
		}

		//////////////////////////////////////////////////////////////////////////
		if ( mState == DETPACK_BUILDING && !mMapGoal->IsAvailable( GetClient()->GetTeam() ) )
			return State_Finished;

		if ( DidPathSucceed() )
		{
			FINDSTATE( detp, Detpack, GetParent() );
			if ( detp && detp->DetpackFullyBuilt() )
				return State_Finished;

			FINDSTATEIF( Aimer, GetRootState(), AddAimRequest( Priority::Medium, this, GetNameHash() ) );

			if ( mCantBuild )
			{
				GetClient()->SetMovementVector( -mMapGoal->GetFacing() );
				GetClient()->PressButton( BOT_BUTTON_WALK );
				mNextBuildTry = 0;
			}
			else
				GetClient()->SetMovementVector( Vector3f::ZERO );
		}
		return State_Busy;
	}
	void DetpackBuild::ProcessEvent( const Message & message, CallbackParameters & cb )
	{
		switch ( message.Id() )
		{
			CASE_MSG( EvEntityCantBuild )
			{
				if ( msg->mClassId == TF_CLASSEX_DETPACK )
				{
					mCantBuild = true;
				}
				break;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////

	Detpack::Detpack() : StatePrioritized( "Detpack" )
	{
		AppendState( new DetpackBuild );

		LimitToClass().SetFlag( TF_CLASS_DEMOMAN );
		SetAlwaysRecieveEvents( true );
	}

	void Detpack::UpdateDetpackStatus( const Event_DetpackStatus_TF& stats )
	{
		mDetpackStatus.mEntity = stats.mEntity;
		if ( mDetpackStatus.mEntity.IsValid() )
		{
			mDetpackStatus.mStatus = BUILDABLE_BUILT;
		}
		else
		{
			mDetpackStatus.mStatus = BUILDABLE_INVALID;
		}
	}

	State::StateStatus Detpack::Update( float fDt )
	{
		return State_Busy;
	}

	void Detpack::ProcessEvent( const Message & message, CallbackParameters & cb )
	{
		switch ( message.Id() )
		{
			CASE_MSG( EvEntityBuilding )
			{
				if ( msg->mClassId == TF_CLASSEX_DETPACK )
				{
					mDetpackStatus.mEntity = msg->mEntity;
					mDetpackStatus.mStatus = BUILDABLE_BUILDING;
					DBG_MSG( 0, GetClient(), kNormal, "Detpack Building" );
				}
				break;
			}
			CASE_MSG( EvEntityBuilt )
			{
				if ( msg->mClassId == TF_CLASSEX_DETPACK )
				{
					mDetpackStatus.mEntity = msg->mEntity;
					mDetpackStatus.mStatus = BUILDABLE_BUILDING;
					DBG_MSG( 0, GetClient(), kNormal, "Detpack Building" );
					EngineFuncs::EntityPosition( msg->mEntity, mDetpackStatus.mPosition );
					mDetpackStatus.mStatus = BUILDABLE_BUILT;
					DBG_MSG( 0, GetClient(), kNormal, "Detpack Built" );
				}
				break;
			}
			CASE_MSG( EvEntityBuildCancel )
			{
				if ( msg->mClassId == TF_CLASSEX_DETPACK )
				{
					memset( &mDetpackStatus, 0, sizeof( mDetpackStatus ) );
					mDetpackStatus.mEntity.Reset();
				}
				break;
			}
			CASE_MSG( EvEntityBuildDetonated )
			{
				if ( msg->mClassId == TF_CLASSEX_DETPACK )
				{
					memset( &mDetpackStatus, 0, sizeof( mDetpackStatus ) );
					mDetpackStatus.mEntity.Reset();
				}
				break;
			}
			CASE_MSG( EvEntityBuildDismantled )
			{
				if ( msg->mClassId == TF_CLASSEX_DETPACK )
				{
					memset( &mDetpackStatus, 0, sizeof( mDetpackStatus ) );
					mDetpackStatus.mEntity.Reset();
				}
				break;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	PipeTrap::PipeTrap()
		: StateChild( "PipeTrap" )
		, FollowPathUser( "PipeTrap" )
		, mSubstate( IDLE )
	{
		LimitToClass().SetFlag( TF_CLASS_DEMOMAN );
		SetAlwaysRecieveEvents( true );

		mPipes.mPipeCount = 0;

		mNumTraps = 0;
		mNumWaits = 0;

		mCurrentTrap = 0;
		mCurrentWait = 0;

		mPlaceOrder = OrderRandomAll;
		mWaitOrder = OrderRandomAll;
	}
	void PipeTrap::GetDebugString( std::stringstream &out )
	{
		if ( IsActive() )
		{
			if ( mMapGoal )
				out << mMapGoal->GetName() << " ";
			out << "# Pipes: " << GetPipeCount();
		}
	}
	void PipeTrap::RenderDebug()
	{
		for ( int i = 0; i < Pipes::MaxPipes; ++i )
		{
			if ( mPipes.mPipes[ i ].mEntity.IsValid() )
			{
				Box3f b;
				if ( EngineFuncs::EntityWorldOBB( mPipes.mPipes[ i ].mEntity, b ) )
				{
					RenderBuffer::AddOBB( b, mPipes.mPipes[ i ].mMoving ? COLOR::GREEN : COLOR::RED );
				}
			}
		}
	}

	// FollowPathUser functions.
	bool PipeTrap::GetNextDestination( DestinationVector &_desination, bool &_final, bool &_skiplastpt )
	{
		if ( !mMapGoal )
			return false;

		if ( mMapGoal->RouteTo( GetClient(), _desination ) )
			_final = false;
		else
			_final = true;
		return true;
	}

	// AimerUser functions.
	bool PipeTrap::GetAimPosition( Vector3f &_aimpos )
	{
		switch ( mSubstate )
		{
		case LAY_PIPES:
			_aimpos = GetClient()->GetEyePosition() + mTraps[ mCurrentTrap ].mFacing * 256.f;
			break;
		case WATCH_PIPES:
			_aimpos = GetClient()->GetEyePosition() + mWait[ mCurrentWait ].mFacing * 256.f;
			break;
		case IDLE:
		case GETTING_AMMO:
			break;
		}
		return true;
	}

	void PipeTrap::OnTarget()
	{
		if ( mSubstate == LAY_PIPES )
		{
			if ( GetClient()->GetSteeringSystem()->InTargetRadius() )
			{
				FINDSTATE( ws, WeaponSystem, GetRootState() );
				if ( ws && ws->CurrentWeaponIs( TF_Options::PIPE_WEAPON ) && (IGame::GetFrameNumber() & 2) )
					ws->FireWeapon();
			}
		}
	}

	bool PipeTrap::CacheGoalInfo( MapGoalPtr mg )
	{
		mCurrentTrap = 0;
		mCurrentWait = 0;

		mNumTraps = 0;
		mNumWaits = 0;

		mg->GetProperty( "PlaceOrder", mPlaceOrder );
		mg->GetProperty( "WaitOrder", mWaitOrder );

		for ( int i = 0; i < MaxPlacementPts; ++i )
		{
			mTraps[ mNumTraps ].Reset();
			if ( mg->GetProperty( va( "Place[%d].Position", i ).c_str(), mTraps[ mNumTraps ].mSource ) &&
				 mg->GetProperty( va( "Place[%d].Facing", i ).c_str(), mTraps[ mNumTraps ].mFacing ) )
			{
				if ( !mTraps[ mNumTraps ].mFacing.IsZero() )
				{
					++mNumTraps;
				}
			}
		}

		for ( int i = 0; i < MaxWaitPositions; ++i )
		{
			mWait[ mNumWaits ].Reset();

			mg->GetProperty( va( "Wait[%d].Position", i ).c_str(), mWait[ mNumWaits ].mPosition );
			mg->GetProperty( va( "Wait[%d].Facing", i ).c_str(), mWait[ mNumWaits ].mFacing );
			/*mg->GetProperty("MinWaitTime",.mMinWaitTime);
			mg->GetProperty("MaxWaitTime",.mMaxWaitTime);*/

			if ( !mWait[ mNumWaits ].mPosition.IsZero() && !mWait[ mNumWaits ].mFacing.IsZero() )
			{
				++mNumWaits;
			}
		}
		if ( mNumWaits == 0 )
		{
			mWait[ mNumWaits ].mPosition = mg->GetPosition();
			mWait[ mNumWaits ].mFacing = mg->GetFacing();
			++mNumWaits;
		}

		//////////////////////////////////////////////////////////////////////////
		switch ( mPlaceOrder )
		{
		case OrderRandomAll:
			std::random_shuffle( mTraps, mTraps + mNumTraps );
			break;
		case OrderRandomPick1:
			std::random_shuffle( mTraps, mTraps + mNumTraps );
			mNumTraps = ClampT<int>( mNumTraps, 0, 1 );
			break;
		case OrderSequentialAll:
			break;
		}
		//////////////////////////////////////////////////////////////////////////
		switch ( mWaitOrder )
		{
		case OrderRandomAll:
			std::random_shuffle( mWait, mWait + mNumWaits );
			break;
		case OrderRandomPick1:
			std::random_shuffle( mWait, mWait + mNumWaits );
			mNumWaits = ClampT<int>( mNumWaits, 0, 1 );
			break;
		case OrderSequentialAll:
			break;
		}
		//////////////////////////////////////////////////////////////////////////
		return mNumTraps > 0;
	}

	float PipeTrap::GetPriority()
	{
		mSubstate = LAY_PIPES;

		if ( GetPipeCount() >= TF_Options::PIPE_MAX_DEPLOYED )
		{
			mSubstate = WATCH_PIPES;
			return 1.f;
		}

		if ( !mMapGoal )
		{
			GoalManager::Query qry( 0x7e67445e /* pipetrap */, GetClient() );
			System::mInstance->mGoalManager->GetGoals( qry );

			if ( !qry.GetBest( mMapGoal ) || !CacheGoalInfo( mMapGoal ) )
				mMapGoal.reset();
		}
		return mMapGoal ? mMapGoal->GetPriorityForClient( GetClient() ) : 0.f;
	}
	void PipeTrap::Enter()
	{
		//////////////////////////////////////////////////////////////////////////
		// Cache pipe trap goal information
		//////////////////////////////////////////////////////////////////////////
		FINDSTATEIF( Navigator, GetRootState(), Goto( this ) );

		mExpireTime = 0;
	}
	void PipeTrap::Exit()
	{
		FINDSTATEIF( Navigator, GetRootState(), Stop( true ) );

		mSubstate = IDLE;

		FINDSTATEIF( Aimer, GetRootState(), ReleaseAimRequest( GetNameHash() ) );
		FINDSTATEIF( WeaponSystem, GetRootState(), ReleaseWeaponRequest( GetNameHash() ) );

		FINDSTATEIF( ProximityWatcher, GetRootState(), RemoveWatch( GetNameHash() ) );
		mWatchFilter.reset();
	}
	State::StateStatus PipeTrap::Update( float fDt )
	{
		// Update Pipe Positions
		mPipes.mPipeCount = 0;

		//////////////////////////////////////////////////////////////////////////
		// Make sure pipes are valid and update their positions.
		bool bRebuildWatchPositions = false;
		for ( int i = 0; i < Pipes::MaxPipes; ++i )
		{
			if ( mPipes.mPipes[ i ].mEntity.IsValid() )
			{
				if ( !IGame::IsEntityValid( mPipes.mPipes[ i ].mEntity ) )
				{
					mPipes.mPipes[ i ].mEntity.Reset();
					bRebuildWatchPositions = true;
					continue;
				}

				mPipes.mPipeCount++;

				Vector3f vVelocity( Vector3f::ZERO );
				if ( mPipes.mPipes[ i ].mMoving )
				{
					if ( !EngineFuncs::EntityVelocity( mPipes.mPipes[ i ].mEntity, vVelocity ) ||
						 !EngineFuncs::EntityPosition( mPipes.mPipes[ i ].mEntity, mPipes.mPipes[ i ].mPosition ) )
					{
						mPipes.mPipes[ i ].mEntity.Reset();
						continue;
					}

					if ( vVelocity.SquaredLength() <= Mathf::EPSILON )
					{
						mPipes.mPipes[ i ].mMoving = false;

						if ( mWatchFilter )
						{
							mWatchFilter->AddPosition( mPipes.mPipes[ i ].mPosition );
						}
						else
						{
							mWatchFilter.reset( new FilterClosestTF( GetClient(), AiState::SensoryMemory::EntEnemy ) );
							mWatchFilter->AddClass( FilterSensory::ANYPLAYERCLASS );
							mWatchFilter->AddGroup( ENT_GRP_BUILDABLE );
							mWatchFilter->AddClass( TF_CLASSEX_DISPENSER );
							mWatchFilter->AddPosition( mPipes.mPipes[ i ].mPosition );
							mWatchFilter->SetMaxDistance( 100.f );
							FINDSTATEIF( ProximityWatcher, GetRootState(), AddWatch( GetNameHash(), mWatchFilter, false ) );
						}
					}
				}
			}
		}

		//////////////////////////////////////////////////////////////////////////
		// Rebuild the positions so it's not watching for targets at old expired pipe positions
		if ( bRebuildWatchPositions && mWatchFilter )
		{
			mWatchFilter->ClearPositions();
			for ( int i = 0; i < Pipes::MaxPipes; ++i )
			{
				if ( mPipes.mPipes[ i ].mEntity.IsValid() && !mPipes.mPipes[ i ].mMoving )
				{
					mWatchFilter->AddPosition( mPipes.mPipes[ i ].mPosition );
				}
			}
		}

		//////////////////////////////////////////////////////////////////////////
		const bool bGoForAmmo = ShouldGoForAmmo();
		if ( mSubstate != GETTING_AMMO && bGoForAmmo )
		{
			SensoryMemory *sensory = GetClient()->GetSensoryMemory();

			MemoryRecords records;
			Vector3List recordpos;

			FilterAllType filter( GetClient(), AiState::SensoryMemory::EntAny, records );
			filter.MemorySpan( Utils::SecondsToMilliseconds( 7.f ) );
			filter.AddGroup( ENT_GRP_RESUPPLY );
			sensory->QueryMemory( filter );

			sensory->GetRecordInfo( records, &recordpos, NULL );

			if ( !recordpos.empty() )
			{
				mSubstate = GETTING_AMMO;
				FINDSTATEIF( Aimer, GetRootState(), ReleaseAimRequest( GetNameHash() ) );
				FINDSTATEIF( Navigator, GetRootState(), Goto( this, recordpos ) );
			}
			else
			{
				//BlackboardDelay(10.f, mMapGoalSentry->GetSerialNum());
				return State_Finished;
			}
		}
		else if ( mSubstate == GETTING_AMMO && !bGoForAmmo )
			return State_Finished;

		//////////////////////////////////////////////////////////////////////////
		if ( DidPathFail() )
		{
			// Delay it from being used for a while.
			BlackboardDelay( 10.f, mMapGoal->GetSerialNum() );
			return State_Finished;
		}
		//////////////////////////////////////////////////////////////////////////
		FINDSTATE( ws, WeaponSystem, GetRootState() );
		FINDSTATE( ts, TargetingSystem, GetRootState() );

		Priority::ePriority AimPriority = Priority::Low;
		Priority::ePriority WeaponPriority = ts&&ts->HasTarget() ? Priority::Min : Priority::Low;

		int32_t wpn = TF_Options::PIPE_WEAPON;
		switch ( mSubstate )
		{
		case LAY_PIPES:
		{
			mExpireTime = 0; // reset expire

			wpn = TF_Options::PIPE_WEAPON;
			if ( GetPipeCount() >= TF_Options::PIPE_MAX_DEPLOYED )
			{
				mSubstate = WATCH_PIPES;
				mCurrentWait = rand() % mNumWaits;
			}
			break;
		}
		case WATCH_PIPES:
		{
			if ( GetPipeCount() < TF_Options::PIPE_MAX_DEPLOYED )
			{
				mSubstate = LAY_PIPES;
				break;
			}

			WeaponPriority = Priority::VeryLow;

			wpn = TF_Options::PIPE_WEAPON_WATCH;

			// reload pipe launcher before switching to GL
			/*WeaponPtr curWpn = ws->GetCurrentWeapon();
			if(curWpn->GetWeaponID()==TF_Options::PIPE_WEAPON)
			{
			if(curWpn->CanReload()!=InvalidFireMode)
			{
			wpn = TF_Options::PIPE_WEAPON;
			curWpn->ReloadWeapon();
			}
			}*/
			break;
		}
		case IDLE:
		case GETTING_AMMO:
			break;
		}

		if ( DidPathSucceed() )
		{
			FINDSTATEIF( Aimer, GetRootState(), AddAimRequest( AimPriority, this, GetNameHash() ) );
			ws->AddWeaponRequest( WeaponPriority, GetNameHash(), wpn );

			SteeringSystem *steer = GetClient()->GetSteeringSystem();
			switch ( mSubstate )
			{
			case LAY_PIPES:
			{
				steer->SetTarget( mTraps[ mCurrentTrap ].mSource );
				break;
			}
			case WATCH_PIPES:
			{
				steer->SetTarget( mWait[ mCurrentWait ].mPosition );
				//////////////////////////////////////////////////////////////////////////
				// Cycle?
				if ( mNumWaits > 1 && mWaitOrder != OrderRandomPick1 )
				{
					if ( mExpireTime == 0 && steer->InTargetRadius() )
					{
						mExpireTime = IGame::GetTime() +
							Mathf::IntervalRandomInt( mWait[ mCurrentWait ].mMinWaitTime, mWait[ mCurrentWait ].mMaxWaitTime );
					}
					else if ( IGame::GetTime() > mExpireTime )
					{
						mExpireTime = 0;

						switch ( mWaitOrder )
						{
						case OrderRandomAll:
							mCurrentWait = rand() % mNumWaits;
							break;
						case OrderSequentialAll:
							mCurrentWait = (mCurrentWait + 1) % mNumWaits;
							break;
						}
					}
				}
				break;
			}
			case IDLE:
				break;
			case GETTING_AMMO:
				return State_Finished;
			}
		}
		return State_Busy;
	}

	void PipeTrap::ProcessEvent( const Message & message, CallbackParameters & cb )
	{
		switch ( message.Id() )
		{
			CASE_MSG( EvProximityTrigger )
			{
				if ( msg->mOwnerState == GetNameHash() )
				{
					if ( IGame::GetFrameNumber() & 1 )
						GetClient()->PressButton( TF_BOT_BUTTON_DETPIPES );
					//GetClient()->SendEvent(Message(TF_MSG_DETPIPES))
				}
				break;
			}
			CASE_MSG( EvWeaponFire )
			{
				if ( msg->mProjectile.IsValid() )
				{
					EntityInfo entInfo;
					if ( IGame::GetEntityInfo( msg->mProjectile, entInfo ) )
					{
						if ( entInfo.mGroup == ENT_GRP_PROJECTILE && entInfo.mClassId == TF_CLASSEX_PIPE )
						{
							for ( int i = 0; i < Pipes::MaxPipes; ++i )
							{
								if ( !mPipes.mPipes[ i ].mEntity.IsValid() )
								{
									//Utils::OutputDebug(kInfo, "--Adding Pipe To Pipe List--");
									if ( EngineFuncs::EntityPosition( msg->mProjectile, mPipes.mPipes[ i ].mPosition ) )
									{
										mCurrentTrap = (mCurrentTrap + 1) % mNumTraps;

										mPipes.mPipes[ i ].mEntity = msg->mProjectile;
										mPipes.mPipes[ i ].mMoving = true;
										mPipes.mPipeCount++;
									}
									break;
								}
							}
						}
					}
				}
				break;
			}
		}
	}

	bool PipeTrap::ShouldGoForAmmo()
	{
		int iPipes = 0, iMaxPipes = 0;
		gEngineFuncs->GetCurrentAmmo( GetClient()->GetGameEntity(), TF_Options::PIPE_AMMO, Primary, iPipes, iMaxPipes );
		return iPipes == 0;
	}
	//////////////////////////////////////////////////////////////////////////
	RocketJump::RocketJump()
		: StateChild( "RocketJump" )
		, mIsDone( false )
	{
		LimitToClass().SetFlag( TF_CLASS_SOLDIER );
		//LimitToClass().SetFlag(TF_CLASS_PYRO);
	}

	bool RocketJump::GetAimPosition( Vector3f &_aimpos )
	{
		Vector3f vFlatFacing = Vector3f( mNextPt.mPt - GetClient()->GetPosition() ).Flatten();
		_aimpos = GetClient()->GetEyePosition() + Utils::ChangePitch( vFlatFacing, -89.f ) * 32.f;
		return true;
	}

	void RocketJump::OnTarget()
	{
		bool bInRange = SquaredLength2d( GetClient()->GetPosition(), mNextPt.mPt ) <= Mathf::Sqr( mNextPt.mRadius );
		if ( bInRange )
		{
			FINDSTATE( ws, WeaponSystem, GetRootState() );
			if ( ws && ws->CurrentWeaponIs( TF_Options::ROCKETJUMP_WPN ) )
			{
				GetClient()->PressButton( BOT_BUTTON_JUMP );
				ws->FireWeapon();
				mIsDone = true;
			}
		}
	}

	void RocketJump::Enter()
	{
		mIsDone = false;
		FINDSTATEIF( Aimer, GetParent(), AddAimRequest( Priority::Override, this, GetNameHash() ) );
		FINDSTATEIF( WeaponSystem, GetRootState(), AddWeaponRequest( Priority::Override, GetNameHash(), TF_Options::ROCKETJUMP_WPN ) );
	}

	void RocketJump::Exit()
	{
		FINDSTATEIF( Aimer, GetParent(), ReleaseAimRequest( GetNameHash() ) );
		FINDSTATEIF( WeaponSystem, GetRootState(), ReleaseWeaponRequest( GetNameHash() ) );
	}

	void RocketJump::RenderDebug()
	{
		if ( IsActive() )
		{
			RenderBuffer::AddLine( GetClient()->GetEyePosition(), mNextPt.mPt, COLOR::GREEN );
		}
	}

	float RocketJump::GetPriority()
	{
		FINDSTATE( nav, Navigator, GetParent() );
		if ( nav )
		{
			if ( nav->IsMoving() && nav->IsOnCustomLink( NAVFLAGS_ROCKETJUMP ) )
			{
				return 1.f;
			}
		}
		return 0.f;
	}

	State::StateStatus RocketJump::Update( float fDt )
	{
		return mIsDone ? State_Finished : State_Busy;
	}
	void RocketJump::ProcessEvent( const Message & message, CallbackParameters & cb )
	{
		switch ( message.Id() )
		{
			CASE_MSG( EvWeaponFire )
			{
				if ( msg->mProjectile.IsValid() )
				{
					EntityInfo entInfo;

					if ( IGame::GetEntityInfo( msg->mProjectile, entInfo ) && entInfo.mClassId == TF_CLASSEX_ROCKET )
					{
						mIsDone = true;
					}
				}
				break;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	ConcussionJump::ConcussionJump() : StateChild( "ConcussionJump" )
	{
		LimitToClass().SetFlag( TF_CLASS_MEDIC );
		LimitToClass().SetFlag( TF_CLASS_SCOUT );
	}
	void ConcussionJump::RenderDebug()
	{
		if ( IsActive() )
		{
			RenderBuffer::AddLine( GetClient()->GetEyePosition(), mNextPt.mPt, COLOR::GREEN );
		}
	}
	bool ConcussionJump::GetAimPosition( Vector3f &_aimpos )
	{
		_aimpos = mNextPt.mPt;
		return true;
	}
	void ConcussionJump::OnTarget()
	{
	}
	float ConcussionJump::GetPriority()
	{
		/*FINDSTATE(fp,FollowPath,GetParent());
		if(fp)
		{
		if(fp->IsMoving())
		{
		const Path &p = fp->GetCurrentPath();
		if(!p.IsEndOfPath())
		{
		Path::PathPoint pt;

		int ix = p.GetCurrentPtIndex();
		for(int i = ix; i < p.GetNumPts(); ++i)
		{
		p.GetCurrentPt(pt);
		if(pt.mNavFlags & F_TF_NAV_CONCJUMP)
		{
		mNextPt = pt;
		return 1.f;
		}
		}
		}
		}
		}*/
		return 0.f;
	}
	void ConcussionJump::Enter()
	{
		FINDSTATEIF( Aimer, GetRootState(), AddAimRequest( Priority::Override, this, GetNameHash() ) );
	}	void ConcussionJump::Exit()
	{
		FINDSTATEIF( Aimer, GetRootState(), ReleaseAimRequest( GetNameHash() ) );
	}
	State::StateStatus ConcussionJump::Update( float fDt )
	{
		return State_Busy;
	}
	void ConcussionJump::ProcessEvent( const Message & message, CallbackParameters & cb )
	{
		/*switch(message.Id())
		{
		CASE_MSG(ACTION_WEAPON_FIRE)
		{
		const Event_WeaponFire *m = message.Get<Event_WeaponFire>();
		if(m && m->mProjectile.IsValid())
		{
		if(gEngineFuncs->GetEntityClass(m->mProjectile) == TF_CLASSEX_CONC_GRENADE)
		{
		mIsDone = true;
		}
		}
		break;
		}
		}*/
	}
	//////////////////////////////////////////////////////////////////////////
	ThrowGrenade::ThrowGrenade()
		: StateChild( "ThrowGrenade" )
		, mPrimaryGrenade( 0 )
		, mSecondaryGrenade( 0 )
		, mGren1Ammo( 0 )
		, mGren1AmmoMax( 0 )
		, mGren2Ammo( 0 )
		, mGren2AmmoMax( 0 )
		, mGrenType( 0 )
		, mLastThrowTime( 0 )
	{
	}
	void ThrowGrenade::GetDebugString( std::stringstream &out )
	{
		if ( IsActive() )
		{
			out << (mGrenType == TF_BOT_BUTTON_GREN1 ?
					 "Throwing Primary Grenade" :
					 "Throwing Secondary Grenade");
		}
	}
	void ThrowGrenade::RenderDebug()
	{
		if ( IsActive() )
			RenderBuffer::AddLine( GetClient()->GetEyePosition(), mAimPos, mOnTarget ? COLOR::GREEN : COLOR::RED );
	}
	bool ThrowGrenade::GetAimPosition( Vector3f &_aimpos )
	{
		_aimpos = mAimPos;
		return true;
	}
	void ThrowGrenade::OnTarget()
	{
		mOnTarget = true;
	}
	void ThrowGrenade::OnSpawn()
	{
		_UpdateGrenadeTypes();
	}
	float ThrowGrenade::GetPriority()
	{
		const float FF_MIN_GREN_DIST = 200.f;
		const float FF_MAX_GREN_DIST = 800.f;

		if ( !IsActive() )
		{
			const MemoryRecord *pRecord = GetClient()->GetTargetingSystem()->GetCurrentTargetRecord();

			if ( pRecord && pRecord->IsShootable() )
			{
				/*const GameEntity &vTargetEntity = mClient->GetTargetingSystem()->GetCurrentTarget();
				const TargetInfo *pTargetInfo = mClient->GetSensoryMemory()->GetTargetInfo(vTargetEntity);*/
				const TargetInfo &targetInfo = pRecord->mTargetInfo;

				// Range check.
				if ( !InRangeT<float>( targetInfo.mDistanceTo, FF_MIN_GREN_DIST, FF_MAX_GREN_DIST ) )
					return 0.f;

				// Calculate the desirability to use this item.
				float fDesirability = 0.0;
				float fGren1Desir = TF_Game::_GetDesirabilityFromTargetClass( mPrimaryGrenade, targetInfo.mEntInfo.mClassId );
				float fGren2Desir = TF_Game::_GetDesirabilityFromTargetClass( mSecondaryGrenade, targetInfo.mEntInfo.mClassId );

				if ( fGren1Desir >= fGren2Desir )
				{
					fDesirability = fGren1Desir;
					mGrenType = TF_BOT_BUTTON_GREN1;
				}
				else
				{
					fDesirability = fGren2Desir;
					mGrenType = TF_BOT_BUTTON_GREN2;
				}

				// Prefer more when reloading.
				if ( GetClient()->GetWeaponSystem()->IsReloading() )
					fDesirability += 0.1f;

				// Take the last thrown time into account.
				int iTimeSinceLast = IGame::GetTime() - mLastThrowTime;
				fDesirability *= ClampT<float>( (float)iTimeSinceLast / 7500.f, 0.f, 2.f );

				// How about some randomness?
				//dDesirability *= Mathd::UnitRandom() * 1.1;

				// With bias
				mGrenType = TF_BOT_BUTTON_GREN1;

				const float fUseThreshhold = 0.9f;
				if ( fDesirability >= fUseThreshhold )
				{
					//Dbg::dout << Dbg::info << "Using " << GetItemName() << std::endl;
					mAimMode = GRENADE_AIM;
					return 1.f;
				}
			}
			return 0.f;
		}
		return 1.f;
	}
	void ThrowGrenade::Enter()
	{
		mOnTarget = false;
		mLastThrowTime = IGame::GetTime();
		//FINDSTATEIF(Aimer,GetRootState(),AddAimRequest(Priority::High,this,GetNameHash()));
	}
	void ThrowGrenade::Exit()
	{
		FINDSTATEIF( Aimer, GetRootState(), ReleaseAimRequest( GetNameHash() ) );
	}
	State::StateStatus ThrowGrenade::Update( float fDt )
	{
		const Vector3f vEyePos = GetClient()->GetEyePosition();
		const float fProjectileSpeed = _GetProjectileSpeed( mGrenType );
		//const float fProjectileGravity = _GetProjectileGravity(.mGrenType);
		const float fTimeToDet = 4.0f - GetStateTime();

		// We need to aim the grenade.
		const MemoryRecord *pRecord = GetClient()->GetTargetingSystem()->GetCurrentTargetRecord();
		if ( pRecord )
		{
			const TargetInfo &targetInfo = pRecord->mTargetInfo;
			mAimPos = Utils::PredictFuturePositionOfTarget( GetClient()->GetPosition(),
															fProjectileSpeed, targetInfo, Vector3f::ZERO );

			// Add a vertical aim offset
			mAimPos.Z() += targetInfo.mDistanceTo * 0.25f;

			bool bThrow = fTimeToDet < 1.0;

			// Calculate the timer for the grenade based on the distance to the aim point.
			float fExplodeDistToTarget = (mAimPos - GetClient()->GetPosition()).Length();

			if ( !bThrow )
			{
				// How far will the grenade go with the time left?
				const float fFakeGrenadeRange = (fTimeToDet - 0.15f) * fProjectileSpeed;
				const float fGrenadeRange = fTimeToDet * fProjectileSpeed;

				if ( fExplodeDistToTarget >= fFakeGrenadeRange )
				{
					mAimMode = GRENADE_AIM;
					FINDSTATEIF( Aimer, GetRootState(), AddAimRequest( Priority::High, this, GetNameHash() ) );
				}
				if ( mOnTarget && fExplodeDistToTarget >= fGrenadeRange )
				{
					bThrow = true;
				}
			}

			if ( bThrow )
			{
				return State_Finished;
			}
		}
		else
		{
			if ( mOnTarget && mAimMode == GRENADE_RID )
				return State_Finished;

			if ( fTimeToDet <= 1.f )
			{
				mAimMode = GRENADE_RID;
				FINDSTATEIF( Aimer, GetRootState(), AddAimRequest( Priority::High, this, GetNameHash() ) );
				mAimPos = vEyePos - GetClient()->GetVelocity() * 256.f; // face back
			}
		}
		mOnTarget = false;
		GetClient()->PressButton( mGrenType );
		return State_Busy;
	}
	void ThrowGrenade::ProcessEvent( const Message & message, CallbackParameters & cb )
	{
	}
	void ThrowGrenade::_UpdateAmmo()
	{
		gEngineFuncs->GetCurrentAmmo( GetClient()->GetGameEntity(), TF_WP_GRENADE1, Primary, mGren1Ammo, mGren1AmmoMax );
		gEngineFuncs->GetCurrentAmmo( GetClient()->GetGameEntity(), TF_WP_GRENADE2, Primary, mGren2Ammo, mGren2AmmoMax );
	}
	void ThrowGrenade::_UpdateGrenadeTypes()
	{
		switch ( GetClient()->GetClass() )
		{
		case TF_CLASS_SCOUT:
			mPrimaryGrenade = TF_WP_GRENADE_CALTROPS;
			mSecondaryGrenade = TF_WP_GRENADE_CONC;
			break;
		case TF_CLASS_SNIPER:
			mPrimaryGrenade = TF_WP_GRENADE;
			mSecondaryGrenade = TF_WP_NONE;
			break;
		case TF_CLASS_SOLDIER:
			mPrimaryGrenade = TF_WP_GRENADE;
			mSecondaryGrenade = TF_WP_GRENADE_NAIL;
			break;
		case TF_CLASS_DEMOMAN:
			mPrimaryGrenade = TF_WP_GRENADE;
			mSecondaryGrenade = TF_WP_GRENADE_MIRV;
			break;
		case TF_CLASS_MEDIC:
			mPrimaryGrenade = TF_WP_GRENADE;
			mSecondaryGrenade = TF_WP_GRENADE_CONC;
			break;
		case TF_CLASS_HWGUY:
			mPrimaryGrenade = TF_WP_GRENADE;
			mSecondaryGrenade = TF_WP_GRENADE_MIRV;
			break;
		case TF_CLASS_PYRO:
			mPrimaryGrenade = TF_WP_GRENADE;
			mSecondaryGrenade = TF_WP_GRENADE_NAPALM;
			break;
		case TF_CLASS_SPY:
			mPrimaryGrenade = TF_WP_GRENADE;
			mSecondaryGrenade = TF_WP_GRENADE_GAS;
			break;
		case TF_CLASS_ENGINEER:
			mPrimaryGrenade = TF_WP_GRENADE;
			mSecondaryGrenade = TF_WP_GRENADE_EMP;
			break;
		case TF_CLASS_CIVILIAN:
		default:
			mPrimaryGrenade = 0;
			mSecondaryGrenade = 0;
			break;
		}
	}
	float ThrowGrenade::_GetProjectileSpeed( int _type ) const
	{
		return TF_Options::GRENADE_VELOCITY;
		//return 500.f;
	}
	float ThrowGrenade::_GetProjectileGravity( int _type ) const
	{
		return 1.f;
	}
	//////////////////////////////////////////////////////////////////////////
	Teleporter::Teleporter() : StatePrioritized( "Teleporter" )
	{
		AppendState( new TeleporterBuild );

		LimitToClass().SetFlag( TF_CLASS_ENGINEER );
		SetAlwaysRecieveEvents( true );
	}
	void Teleporter::UpdateTeleporterStatus( const ParamsTeleporterStatus_TF &_stats )
	{
		if ( mTeleporterStatus.mEntityEntrance.IsValid() )
		{
			mTeleporterStatus.mStatusEntrance = BUILDABLE_BUILT;
		}
		else
		{
			mTeleporterStatus.mStatusEntrance = BUILDABLE_INVALID;
		}

		if ( mTeleporterStatus.mEntityExit.IsValid() )
		{
			mTeleporterStatus.mStatusExit = BUILDABLE_BUILT;
		}
		else
		{
			mTeleporterStatus.mStatusExit = BUILDABLE_INVALID;
		}

		mTeleporterStatus.mEntityEntrance = _stats.mEntityEntrance;
		mTeleporterStatus.mEntityExit = _stats.mEntityExit;
	}
	State::StateStatus Teleporter::Update( float fDt )
	{
		return State_Busy;
	}
	void Teleporter::ProcessEvent( const Message & message, CallbackParameters & cb )
	{
		switch ( message.Id() )
		{
			CASE_MSG( EvEntityBuilding )
			{
				if ( msg->mClassId == TF_CLASSEX_TELEPORTER_ENTRANCE )
				{
					mTeleporterStatus.mStatusEntrance = BUILDABLE_BUILDING;
					mTeleporterStatus.mEntityEntrance = msg->mEntity;
					DBG_MSG( 0, GetClient(), kNormal, "Tele Entrance Building" );
				}
				else if ( msg->mClassId == TF_CLASSEX_TELEPORTER_EXIT )
				{
					mTeleporterStatus.mStatusExit = BUILDABLE_BUILDING;
					mTeleporterStatus.mEntityExit = msg->mEntity;
					DBG_MSG( 0, GetClient(), kNormal, "Tele Exit Building" );
				}
				break;
			}
			CASE_MSG( EvEntityBuilt )
			{
				if ( msg->mClassId == TF_CLASSEX_TELEPORTER_ENTRANCE )
				{
					mTeleporterStatus.mEntityEntrance = msg->mEntity;
					EngineFuncs::EntityPosition( msg->mEntity, mTeleporterStatus.mEntrancePos );
					mTeleporterStatus.mStatusEntrance = BUILDABLE_BUILT;
					DBG_MSG( 0, GetClient(), kNormal, "Tele Entrance Built" );
				}
				else if ( msg->mClassId == TF_CLASSEX_TELEPORTER_EXIT )
				{
					mTeleporterStatus.mEntityExit = msg->mEntity;
					EngineFuncs::EntityPosition( msg->mEntity, mTeleporterStatus.mExitPos );
					mTeleporterStatus.mStatusExit = BUILDABLE_BUILT;
					DBG_MSG( 0, GetClient(), kNormal, "Tele Exit Built" );
				}
				break;
			}
			CASE_MSG( EvEntityBuildDestroyed )
			{
				if ( msg->mClassId == TF_CLASSEX_TELEPORTER_ENTRANCE )
				{
					mTeleporterStatus.mEntityEntrance.Reset();
				}
				else if ( msg->mClassId == TF_CLASSEX_TELEPORTER_EXIT )
				{
					mTeleporterStatus.mEntityExit.Reset();
				}
				break;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	int TeleporterBuild::BuildEquipWeapon = TF_WP_NONE;

	TeleporterBuild::TeleporterBuild()
		: StateChild( "TeleporterBuild" )
		, FollowPathUser( "TeleporterBuild" )
		, mCantBuild( false )
	{
	}
	void TeleporterBuild::GetDebugString( std::stringstream &out )
	{
		if ( IsActive() )
		{
			switch ( mState )
			{
			case NONE:
				break;
			case GETTING_AMMO:
				out << "Getting Ammo";
				break;
			case TELE_BUILDING_ENTRANCE:
				out << (mCantBuild ? "Cant Build " : "Building ");
				if ( mMapGoalTeleEntrance )
					out << mMapGoalTeleEntrance->GetName();
				break;
			case TELE_BUILDING_EXIT:
				out << (mCantBuild ? "Cant Build " : "Building ");
				if ( mMapGoalTeleExit )
					out << mMapGoalTeleExit->GetName();
				break;
			}
		}
	}
	bool TeleporterBuild::GetNextDestination( DestinationVector &_desination, bool &_final, bool &_skiplastpt )
	{
		switch ( mState )
		{
		case TELE_BUILDING_ENTRANCE:
			if ( mMapGoalTeleEntrance->RouteTo( GetClient(), _desination ) )
				_final = false;
			else
				_final = true;
			break;
		case TELE_BUILDING_EXIT:
			if ( mMapGoalTeleExit->RouteTo( GetClient(), _desination ) )
				_final = false;
			else
				_final = true;
			break;

		case GETTING_AMMO:
		default:
			return false;
		}
		return true;
	}
	bool TeleporterBuild::GetAimPosition( Vector3f &_aimpos )
	{
		switch ( mState )
		{
		case TELE_BUILDING_ENTRANCE:
			_aimpos = GetClient()->GetEyePosition() + mMapGoalTeleEntrance->GetFacing() * 1024.f;
			break;
		case TELE_BUILDING_EXIT:
			_aimpos = GetClient()->GetEyePosition() + mMapGoalTeleExit->GetFacing() * 1024.f;
			break;
		default:
			return false;
		}
		return true;
	}
	void TeleporterBuild::OnTarget()
	{
		FINDSTATE( tele, Teleporter, GetParent() );
		if ( !tele )
			return;

		switch ( mState )
		{
		case TELE_BUILDING_ENTRANCE:
		{
			if ( !tele->HasTeleporterEntrance() && IGame::GetTime() >= mNextBuildTry )
			{
				GetClient()->PressButton( TF_BOT_BUTTON_BUILD_TELE_ENTRANCE );
				mNextBuildTry = IGame::GetTime() + TF_Options::BUILD_ATTEMPT_DELAY;
			}
			break;
		}
		case TELE_BUILDING_EXIT:
		{
			if ( !tele->HasTeleporterExit() && IGame::GetTime() >= mNextBuildTry )
			{
				GetClient()->PressButton( TF_BOT_BUTTON_BUILD_TELE_EXIT );
				mNextBuildTry = IGame::GetTime() + TF_Options::BUILD_ATTEMPT_DELAY;
			}
			break;
		}
		default:
			break;
		}
	}
	float TeleporterBuild::GetPriority()
	{
		if ( mMapGoalTeleEntrance )
		{
			if ( !mMapGoalTeleEntrance->IsAvailable( GetClient()->GetTeam() ) )
			{
				GetClient()->PressButton( TF_BOT_BUTTON_DET_TELE_ENTRANCE );
				mMapGoalTeleEntrance.reset();
			}
		}
		if ( mMapGoalTeleExit )
		{
			if ( !mMapGoalTeleExit->IsAvailable( GetClient()->GetTeam() ) )
			{
				GetClient()->PressButton( TF_BOT_BUTTON_DET_TELE_EXIT );
				mMapGoalTeleExit.reset();
			}
		}

		if ( IsActive() )
			return GetLastPriority();

		mMapGoalTeleEntrance.reset();
		mMapGoalTeleExit.reset();

		FINDSTATE( tp, Teleporter, GetParent() );
		if ( tp == NULL )
			return 0.0f;

		if ( !tp->HasTeleporterEntrance() )
		{
			GoalManager::Query qry( 0xed2f457b /* TeleEntrance */, GetClient() );
			System::mInstance->mGoalManager->GetGoals( qry );
			qry.GetBest( mMapGoalTeleEntrance );
			mState = TELE_BUILDING_ENTRANCE;

			if ( mMapGoalTeleEntrance )
				return mMapGoalTeleEntrance->GetPriorityForClient( GetClient() );
		}
		if ( !tp->HasTeleporterExit() )
		{
			GoalManager::Query qry( 0x31bccf2d /* TeleExit */, GetClient() );
			if ( mBuiltTeleEntrance )
			{
				const std::string &groupName = mBuiltTeleEntrance->GetGroupName();
				if ( !groupName.empty() )
					qry.Group( groupName.c_str() );
			}

			System::mInstance->mGoalManager->GetGoals( qry );
			qry.GetBest( mMapGoalTeleExit );
			mState = TELE_BUILDING_EXIT;

			if ( mMapGoalTeleExit )
				return mMapGoalTeleExit->GetPriorityForClient( GetClient() );
		}
		return 0.f;
	}
	void TeleporterBuild::Enter()
	{
		mNextAmmoCheck = 0;
		mNextBuildTry = 0;
		mCantBuild = false;
		FINDSTATEIF( Navigator, GetRootState(), Goto( this ) );
	}
	void TeleporterBuild::Exit()
	{
		FINDSTATEIF( Navigator, GetRootState(), Stop( true ) );

		mState = NONE;

		mMapGoalTeleEntrance.reset();
		mMapGoalTeleExit.reset();

		FINDSTATEIF( Aimer, GetRootState(), ReleaseAimRequest( GetNameHash() ) );
		GetClient()->PressButton( TF_BOT_BUTTON_CANCELBUILD );
		FINDSTATEIF( WeaponSystem, GetRootState(), ReleaseWeaponRequest( GetNameHash() ) );
	}
	State::StateStatus TeleporterBuild::Update( float fDt )
	{
		FINDSTATE( tp, Teleporter, GetParent() );
		if ( tp == NULL )
			return State_Finished;

		switch ( mState )
		{
		case TELE_BUILDING_ENTRANCE:
		{
			if ( !mMapGoalTeleEntrance )
				return State_Finished;

			if ( tp->HasTeleporterEntrance() )
				return State_Finished;
			break;
		}
		case TELE_BUILDING_EXIT:
		{
			if ( !mMapGoalTeleExit )
				return State_Finished;

			if ( tp->HasTeleporterExit() )
				return State_Finished;
			break;
		}
		case GETTING_AMMO:
		default:
			break;
		}

		//////////////////////////////////////////////////////////////////////////
		if ( DidPathFail() )
		{
			// Delay it from being used for a while.
			if ( mState == TELE_BUILDING_ENTRANCE && mMapGoalTeleEntrance )
				BlackboardDelay( 10.f, mMapGoalTeleEntrance->GetSerialNum() );
			else if ( mState == TELE_BUILDING_EXIT && mMapGoalTeleExit )
				BlackboardDelay( 10.f, mMapGoalTeleExit->GetSerialNum() );

			return State_Finished;
		}
		//////////////////////////////////////////////////////////////////////////
		if ( GetClient()->HasEntityFlag( TF_ENT_FLAG_BUILDING_ENTRANCE ) || GetClient()->HasEntityFlag( TF_ENT_FLAG_BUILDING_EXIT ) )
			return State_Busy;

		//////////////////////////////////////////////////////////////////////////
		// Need ammo?
		if ( IGame::GetTime() >= mNextAmmoCheck )
		{
			mNextAmmoCheck = IGame::GetTime() + 1000;

			int iAmmo = 0, iMaxAmmo = 0;
			gEngineFuncs->GetCurrentAmmo( GetClient()->GetGameEntity(), TF_Options::BUILD_AMMO_TYPE, Primary, iAmmo, iMaxAmmo );
			if ( mState == GETTING_AMMO && iAmmo >= TF_Options::TELEPORT_BUILD_AMMO )
				return State_Finished;
			if ( mState != GETTING_AMMO && iAmmo < TF_Options::TELEPORT_BUILD_AMMO )
			{
				SensoryMemory *sensory = GetClient()->GetSensoryMemory();

				MemoryRecords records;
				Vector3List recordpos;

				FilterAllType filter( GetClient(), AiState::SensoryMemory::EntAny, records );
				filter.MemorySpan( Utils::SecondsToMilliseconds( 7.f ) );
				filter.AddGroup( ENT_GRP_RESUPPLY );
				sensory->QueryMemory( filter );

				sensory->GetRecordInfo( records, &recordpos, NULL );

				if ( !recordpos.empty() )
				{
					mState = GETTING_AMMO;
					FINDSTATEIF( Aimer, GetRootState(), ReleaseAimRequest( GetNameHash() ) );
					FINDSTATEIF( Navigator, GetRootState(), Goto( this, recordpos ) );
				}
				else
				{
					//BlackboardDelay(10.f, mMapGoalSentry->GetSerialNum());
					return State_Finished;
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////
		if ( DidPathSucceed() )
		{
			if ( mState == GETTING_AMMO )
				return State_Finished;

			FINDSTATEIF( Aimer, GetRootState(), AddAimRequest( Priority::Medium, this, GetNameHash() ) );
			if ( BuildEquipWeapon )
				FINDSTATEIF( WeaponSystem, GetRootState(), AddWeaponRequest( Priority::Medium, GetNameHash(), BuildEquipWeapon ) );

			if ( mCantBuild )
			{
				if ( mState == TELE_BUILDING_ENTRANCE && mMapGoalTeleEntrance )
					GetClient()->SetMovementVector( -mMapGoalTeleEntrance->GetFacing() );
				else if ( mState == TELE_BUILDING_EXIT && mMapGoalTeleExit )
					GetClient()->SetMovementVector( -mMapGoalTeleExit->GetFacing() );

				GetClient()->PressButton( BOT_BUTTON_WALK );
				mNextBuildTry = 0;
			}
			else
				GetClient()->SetMovementVector( Vector3f::ZERO );
		}
		return State_Busy;
	}
	void TeleporterBuild::ProcessEvent( const Message & message, CallbackParameters & cb )
	{
		switch ( message.Id() )
		{
			CASE_MSG( EvEntityBuilding )
			{
				if ( msg->mClassId == TF_CLASSEX_TELEPORTER_ENTRANCE )
				{
					mBuiltTeleEntrance = mMapGoalTeleEntrance;
				}
				else if ( msg->mClassId == TF_CLASSEX_TELEPORTER_EXIT )
				{
					mBuiltTeleExit = mMapGoalTeleExit;
				}
				break;
			}			
			CASE_MSG( EvEntityBuildCancel )
			{
				if ( msg->mClassId == TF_CLASSEX_TELEPORTER_ENTRANCE )
				{
					mBuiltTeleEntrance.reset();
				}
				else if ( msg->mClassId == TF_CLASSEX_TELEPORTER_EXIT )
				{
					mBuiltTeleExit.reset();
				}
				break;
			}
			CASE_MSG( EvEntityBuildDestroyed )
			{
				if ( msg->mClassId == TF_CLASSEX_TELEPORTER_ENTRANCE )
				{
					mBuiltTeleEntrance.reset();
				}
				else if ( msg->mClassId == TF_CLASSEX_TELEPORTER_EXIT )
				{
					mBuiltTeleExit.reset();
				}
				break;
			}
			CASE_MSG( EvEntityCantBuild )
			{
				if ( msg->mClassId == TF_CLASSEX_TELEPORTER_ENTRANCE )
				{
					mCantBuild = true;
				}
				else if ( msg->mClassId == TF_CLASSEX_TELEPORTER_EXIT )
				{
					mCantBuild = true;
				}
				break;
			}
		}
	}
};