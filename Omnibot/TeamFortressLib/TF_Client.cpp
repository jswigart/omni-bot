////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "TF_Client.h"
#include "TF_BaseStates.h"
#include "TF_Messages.h"
#include "FilterClosestTF.h"

#include "BotTargetingSystem.h"
#include "BotWeaponSystem.h"

#include "IGame.h"
#include "IGameManager.h"
#include "ScriptManager.h"

TF_Client::TF_Client()
{
	mStepHeight = 18.0f;
}

TF_Client::~TF_Client()
{
}

void TF_Client::Init( int _gameid )
{
	Client::Init( _gameid );

	// We want to use a custom targeting filter.
	FilterPtr filter( new FilterClosestTF( this, AiState::SensoryMemory::EntEnemy ) );
	filter->AddCategory( ENT_CAT_SHOOTABLE );
	GetTargetingSystem()->SetDefaultTargetingFilter( filter );

	/*using namespace AiState;
	FINDSTATE(ws,WeaponSystem,GetStateRoot());
	if(ws)
	{
	ws->AddWeaponToInventory(TF_WP_GRENADE1);
	ws->AddWeaponToInventory(TF_WP_GRENADE2);
	}*/
}

void TF_Client::ProcessEvent( const Message& message, CallbackParameters &cb )
{
	switch ( message.Id() )
	{
		CASE_MSG( EvEntityGotArmorFromEngineer )
		{
			cb.CallScript();
			cb.AddEntity( "who", msg->mEngineer );
			cb.AddFloat( "valueBefore", msg->mValueBefore );
			cb.AddFloat( "valueAfter", msg->mValueAfter );
			break;
		}
		CASE_MSG( EvEntityGaveArmorAsEngineer )
		{
			cb.CallScript();
			cb.AddEntity( "who", msg->mRecipient );
			cb.AddFloat( "valueBefore", msg->mValueBefore );
			cb.AddFloat( "valueAfter", msg->mValueAfter );
			break;
		}
		CASE_MSG( EvEntityGotHealthFromMedic )
		{
			cb.CallScript();
			cb.AddEntity( "whodoneit", msg->mMedic );
			cb.AddFloat( "valueBefore", msg->mValueBefore );
			cb.AddFloat( "valueAfter", msg->mValueAfter );
			break;
		}
		CASE_MSG( EvEntityGaveHealthAsMedic )
		{
			cb.CallScript();
			cb.AddEntity( "whoihealed", msg->mRecipient );
			cb.AddFloat( "valueBefore", msg->mValueBefore );
			cb.AddFloat( "valueAfter", msg->mValueAfter );
			break;
		}
		CASE_MSG( EvEntityInfected )
		{
			cb.CallScript();
			cb.AddEntity( "infectedBy", msg->mInfectedBy );
			break;
		}
		CASE_MSG( EvEntityCured )
		{
			cb.CallScript();
			cb.AddEntity( "curedBy", msg->mCuredBy );
			break;
		}
		CASE_MSG( EvEntityBurnLevel )
		{
			cb.CallScript();
			cb.AddEntity( "burnedBy", msg->mBurnedBy );
			cb.AddInt( "burnLevel", msg->mBurnLevel );
			break;
		}
		CASE_MSG( EvEntityGotAmmoFromDispenser )
		{	
			cb.CallScript();
			break;
		}
		CASE_MSG( EvEntityRadioTagUpdate )
		{
			cb.CallScript();
			if ( msg->mDetected.IsValid() )
			{
				GetSensoryMemory()->UpdateWithTouchSource( msg->mDetected );
				cb.AddEntity( "detected", msg->mDetected );
			}
			else
			{
				DBG_MSG( 0, this, kError, "TF_MSG_RADIOTAG_UPDATE got bad entity" );
			}
			break;
		}
		CASE_MSG( EvEntityRadarDetectEntity )
		{
			cb.CallScript();
			if ( msg->mDetected.IsValid() )
			{
				GetSensoryMemory()->UpdateWithTouchSource( msg->mDetected );
				cb.AddEntity( "detected", msg->mDetected );
			}
			else
			{
				DBG_MSG( 0, this, kError, "TF_MSG_RADAR_DETECT_ENEMY got bad entity" );
			}
			break;
		}
		CASE_MSG( EvEntityCantDisguiseAsTeam )
		{
			cb.CallScript();
			cb.AddInt( "team", msg->mTeamId );
			break;
		}
		CASE_MSG( EvEntityCantDisguiseAsClass )
		{
			cb.CallScript();
			cb.AddInt( "class", msg->mClassId );
			break;
		}
		CASE_MSG( EvEntityDisguising )
		{
			cb.CallScript();
			cb.AddInt( "asTeam", msg->mTeamId );
			cb.AddInt( "asClass", msg->mClassId );
			break;
		}
		CASE_MSG( EvEntityDisguised )
		{
			cb.CallScript();
			cb.AddInt( "asTeam", msg->mTeamId );
			cb.AddInt( "asClass", msg->mClassId );
			break;
		}
		//////////////////////////////////////////////////////////////////////////
		CASE_MSG( EvEntityAlreadyBuilt )
		{
			cb.CallScript();
			cb.AddInt( "classId", msg->mClassId );
			//DBG_MSG(0, .mClient, kNormal, "Sentry Already Built");
			break;
		}
		CASE_MSG( EvEntityCantBuild )
		{
			cb.CallScript();
			cb.AddInt( "classId", msg->mClassId );
			//DBG_MSG(0, .mClient, kNormal, "Sentry Can't Build");
			break;
		}
		CASE_MSG( EvEntityBuilding )
		{
			cb.CallScript();
			cb.AddInt( "classId", msg->mClassId );
			cb.AddEntity( "entity", msg->mEntity );
			break;
		}
		CASE_MSG( EvEntityBuildCancel )
		{
			cb.CallScript();
			cb.AddInt( "classId", msg->mClassId );
			cb.AddEntity( "entity", msg->mEntity );
			break;
		}
		CASE_MSG( EvEntityBuilt )
		{
			cb.CallScript();
			cb.AddInt( "classId", msg->mClassId );
			cb.AddEntity( "entity", msg->mEntity );
			break;
		}
		CASE_MSG( EvEntityBuildDestroyed )
		{
			cb.CallScript();
			cb.AddInt( "classId", msg->mClassId );
			cb.AddEntity( "entity", msg->mEntity );
			break;
		}
		CASE_MSG( EvEntityBuildSpotEnemy )
		{
			cb.CallScript();
			cb.AddInt( "classId", msg->mClassId );
			cb.AddEntity( "entity", msg->mEntity );
			cb.AddEntity( "enemy", msg->mEnemy );
			break;
		}
		CASE_MSG( EvEntityBuildAimed )
		{
			cb.CallScript();
			cb.AddInt( "classId", msg->mClassId );
			cb.AddEntity( "entity", msg->mEntity );
			cb.AddVector( "enemy", msg->mDirection );
			break;
		}
		CASE_MSG( EvEntityBuildDamaged )
		{
			cb.CallScript();
			cb.AddInt( "classId", msg->mClassId );
			cb.AddEntity( "entity", msg->mEntity );
			cb.AddEntity( "inflictor", msg->mInflictor );
			break;
		}		
		CASE_MSG( EvEntityBuildUpgraded )
		{
			cb.CallScript();
			cb.AddInt( "classId", msg->mClassId );
			cb.AddEntity( "entity", msg->mEntity );
			cb.AddInt( "level", msg->mLevel );
			break;
		}
		CASE_MSG( EvEntityBuildDetonated )
		{
			cb.CallScript();
			cb.AddInt( "classId", msg->mClassId );
			cb.AddEntity( "entity", msg->mEntity );
			break;
		}
		CASE_MSG( EvEntityBuildDismantled )
		{
			cb.CallScript();
			cb.AddInt( "classId", msg->mClassId );
			cb.AddEntity( "entity", msg->mEntity );
			break;
		}
		CASE_MSG( EvEntityBuildSabotaged )
		{
			cb.CallScript();
			cb.AddInt( "classId", msg->mClassId );
			cb.AddEntity( "entity", msg->mEntity );
			cb.AddEntity( "byWho", msg->mByWhom );
			break;
		}
		CASE_MSG( EvEntityBuildEnemyUsed )
		{
			cb.CallScript();
			cb.AddInt( "classId", msg->mClassId );
			cb.AddEntity( "entity", msg->mEntity );
			cb.AddEntity( "byWho", msg->mByWhom );
			break;
		}
		CASE_MSG( EvEntityBuildNotEnoughAmmo )
		{
			cb.CallScript();
			cb.AddInt( "classId", msg->mClassId );
			break;
		}
		CASE_MSG( EvEntityBuildDetonateIt )
		{
			cb.CallScript();
			cb.AddInt( "classId", msg->mClassId );
			cb.AddEntity( "entity", msg->mEntity );
			break;
		}
		//////////////////////////////////////////////////////////////////////////
		CASE_MSG( EvEntityCalledForMedic )
		{
			cb.CallScript();
			cb.AddEntity( "who", msg->mEntity );
			break;
		}
		//////////////////////////////////////////////////////////////////////////
	}
	Client::ProcessEvent( message, cb );
}

float TF_Client::GetGameVar( GameVar _var ) const
{
	switch ( _var )
	{
		case JumpGapOffset:
			return 64.0f;
	}
	return 0.0f;
}

bool TF_Client::DoesBotHaveFlag( MapGoalPtr _mapgoal )
{
	return _mapgoal ? _mapgoal->GetOwner() == GetGameEntity() : false;
}

bool TF_Client::CanBotSnipe()
{
	if ( GetClass() == TF_CLASS_SNIPER )
	{
		// Make sure we have a sniping weapon.
		if ( GetWeaponSystem()->HasAmmo( TF_WP_SNIPER_RIFLE ) )
			return true;
	}
	return false;
}

bool TF_Client::GetSniperWeapon( int &nonscoped, int &scoped )
{
	nonscoped = 0;
	scoped = 0;

	if ( GetClass() == TF_CLASS_SNIPER )
	{
		if ( GetWeaponSystem()->HasWeapon( TF_WP_SNIPER_RIFLE ) )
		{
			nonscoped = TF_WP_SNIPER_RIFLE;
			scoped = TF_WP_SNIPER_RIFLE;
			return true;
		}
	}
	return false;
}

//float TF_Client::NavCallback(const NavFlags &_flag, Waypoint *from, Waypoint *to)
//{
//	using namespace AiState;
//
//	WeaponSystem *wsys = GetWeaponSystem();
//	if(_flag & F_TF_NAV_ROCKETJUMP)
//	{
//		if(wsys->HasWeapon(TF_Options::ROCKETJUMP_WPN) && wsys->HasAmmo(TF_Options::ROCKETJUMP_WPN))
//			return 1.f;
//	}
//
//	if(_flag & F_TF_NAV_CONCJUMP)
//	{
//		if(wsys->HasWeapon(TF_WP_GRENADE_CONC) && wsys->HasAmmo(TF_WP_GRENADE_CONC))
//			return 1.f;
//	}
//
//	if(_flag & F_NAV_TELEPORT)
//	{
//		// todo:
//		return 1.f;
//	}
//
//	if(_flag & F_TF_NAV_DOUBLEJUMP)
//	{
//		if(GetClass() == TF_CLASS_SCOUT)
//			return 1.f;
//	}
//
//	return 0.f;
//}

void TF_Client::ProcessGotoNode( const PathInterface::PathCorner corners[ 2 ], const size_t numEdges )
{
	/*if ( numEdges == 0 )
		return;*/

	//const bool OnGround = GetEntityFlags().CheckFlag(ENT_FLAG_ONGROUND);
	//if(OnGround)
	// mDoubleJumping = false;

	//if(edges[ 0 ].mNavFlags & F_TF_NAV_DOUBLEJUMP)
	//{
	//	if(!mDoubleJumping && (GetPosition()-edges[ 0 ].mPos).Length() < pt.mRadius)
	//	{
	//		//const float Speed = GetVelocity().Length();
	//		const Vector3f Vel = Normalize(GetVelocity());
	//		const Vector3f IdealVel = Normalize(pt.mPt - GetPosition());
	//		const float fDot = IdealVel.Dot(Vel);

	//		if(OnGround && fDot > 0.9f)
	//		{
	//		 mDoubleJumping = true;
	//			PressButton(BOT_BUTTON_JUMP);
	//		}

	//		static float JumpVelocity = 10.f;
	//		if(!OnGround && GetVelocity().Z() > JumpVelocity)
	//		{
	//		 mDoubleJumping = true;
	//		}

	//	 mDoubleJumpHeight = GetPosition().Z();
	//	}
	//}

	//if(.mDoubleJumping)
	//{
	//	const float NextHeight = GetPosition().Z() + GetVelocity().Z() * IGame::GetDeltaTimeSecs();
	//	if(NextHeight < .mDoubleJumpHeight)
	//	{
	//		PressButton(BOT_BUTTON_JUMP);
	//	 mDoubleJumping = false;
	//	}
	//	return;
	//}

	//.mDoubleJumping = false;
}

void TF_Client::ProcessGotoNode( const Path &_path )
{
	//const bool OnGround = GetEntityFlags().CheckFlag(ENT_FLAG_ONGROUND);
	//if(OnGround)
	// mDoubleJumping = false;

	//Path::PathPoint pt;
	//_path.GetCurrentPt(pt);

	//if(pt.mNavFlags & F_TF_NAV_DOUBLEJUMP)
	//{
	//	if(!mDoubleJumping && (GetPosition()-pt.mPt).Length() < pt.mRadius)
	//	{
	//		//const float Speed = GetVelocity().Length();
	//		const Vector3f Vel = Normalize(GetVelocity());
	//		const Vector3f IdealVel = Normalize(pt.mPt - GetPosition());
	//		const float fDot = IdealVel.Dot(Vel);

	//		if(OnGround && fDot > 0.9f)
	//		{
	//		 mDoubleJumping = true;
	//			PressButton(BOT_BUTTON_JUMP);
	//		}

	//		static float JumpVelocity = 10.f;
	//		if(!OnGround && GetVelocity().Z() > JumpVelocity)
	//		{
	//		 mDoubleJumping = true;
	//		}

	//	 mDoubleJumpHeight = GetPosition().Z();
	//	}
	//}

	//if(.mDoubleJumping)
	//{
	//	const float NextHeight = GetPosition().Z() + GetVelocity().Z() * IGame::GetDeltaTimeSecs();
	//	if(NextHeight < .mDoubleJumpHeight)
	//	{
	//		PressButton(BOT_BUTTON_JUMP);
	//	 mDoubleJumping = false;
	//	}
	//	return;
	//}

	//.mDoubleJumping = false;
}

void TF_Client::SetupBehaviorTree()
{
	using namespace AiState;
	delete GetStateRoot()->RemoveState( "ReturnTheFlag" );

	GetStateRoot()->AppendTo( "HighLevel", new Sentry );
	GetStateRoot()->AppendTo( "HighLevel", new Dispenser );
	GetStateRoot()->AppendTo( "HighLevel", new Detpack );
	GetStateRoot()->AppendTo( "HighLevel", new PipeTrap );
	GetStateRoot()->AppendTo( "MotorControl", new RocketJump );
	GetStateRoot()->AppendTo( "MotorControl", new ConcussionJump );
	GetStateRoot()->AppendTo( "LowLevel", new ThrowGrenade );
}

void TF_Client::Update()
{
	using namespace AiState;
	if ( TF_Options::POLL_SENTRY_STATUS )
	{
		ParamsSentryStatus_TF statusSentry;
		ParamsDispenserStatus_TF statusDispenser;
		ParamsTeleporterStatus_TF statusTeleporter;
		Event_DetpackStatus_TF statusDetpack;
		gTeamFortressFuncs->GetBuildStatus( GetGameEntity(), statusSentry );
		gTeamFortressFuncs->GetBuildStatus( GetGameEntity(), statusDispenser );
		gTeamFortressFuncs->GetBuildStatus( GetGameEntity(), statusTeleporter );
		gTeamFortressFuncs->GetBuildStatus( GetGameEntity(), statusDetpack );

		FINDSTATEIF_OPT( Sentry, GetStateRoot(), UpdateSentryStatus( statusSentry ) );
		FINDSTATEIF_OPT( Dispenser, GetStateRoot(), UpdateDispenserStatus( statusDispenser ) );
		FINDSTATEIF_OPT( Detpack, GetStateRoot(), UpdateDetpackStatus( statusDetpack ) );
		FINDSTATEIF_OPT( Teleporter, GetStateRoot(), UpdateTeleporterStatus( statusTeleporter ) );
	}
	Client::Update();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////