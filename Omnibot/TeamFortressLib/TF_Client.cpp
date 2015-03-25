////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "TF_Client.h"
#include "TF_BaseStates.h"
#include "FilterClosestTF.h"

#include "TF_InterfaceFuncs.h"
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

void TF_Client::ProcessEvent( const MessageHelper &_message, CallbackParameters &_cb )
{
	switch ( _message.GetMessageId() )
	{
		HANDLER( TF_MSG_GOT_ENGY_ARMOR )
		{
			_cb.CallScript();
			const Event_GotEngyArmor *m = _message.Get<Event_GotEngyArmor>();
			_cb.AddEntity( "whodoneit", m->mFromWho );
			_cb.AddInt( "before", m->mBeforeValue );
			_cb.AddInt( "after", m->mAfterValue );
			break;
		}
		HANDLER( TF_MSG_GAVE_ENGY_ARMOR )
		{
			_cb.CallScript();
			const Event_GaveEngyArmor *m = _message.Get<Event_GaveEngyArmor>();
			_cb.AddEntity( "whoiarmored", m->mToWho );
			_cb.AddInt( "before", m->mBeforeValue );
			_cb.AddInt( "after", m->mAfterValue );
			break;
		}
		HANDLER( TF_MSG_GOT_MEDIC_HEALTH )
		{
			_cb.CallScript();
			const Event_GotMedicHealth *m = _message.Get<Event_GotMedicHealth>();
			_cb.AddEntity( "whodoneit", m->mFromWho );
			_cb.AddInt( "before", m->mBeforeValue );
			_cb.AddInt( "after", m->mAfterValue );
			break;
		}
		HANDLER( TF_MSG_GAVE_MEDIC_HEALTH )
		{
			_cb.CallScript();
			const Event_GaveMedicHealth *m = _message.Get<Event_GaveMedicHealth>();
			_cb.AddEntity( "whoihealed", m->mToWho );
			_cb.AddInt( "before", m->mBeforeValue );
			_cb.AddInt( "after", m->mAfterValue );
			break;
		}
		HANDLER( TF_MSG_INFECTED )
		{
			_cb.CallScript();
			const Event_Infected *m = _message.Get<Event_Infected>();
			_cb.AddEntity( "infected by", m->mFromWho );
			break;
		}
		HANDLER( TF_MSG_CURED )
		{
			_cb.CallScript();
			const Event_Cured *m = _message.Get<Event_Cured>();
			_cb.AddEntity( "cured by", m->mByWho );
			break;
		}
		HANDLER( TF_MSG_BURNLEVEL )
		{
			_cb.CallScript();
			const Event_Burn *m = _message.Get<Event_Burn>();
			_cb.AddEntity( "burned by", m->mByWho );
			_cb.AddInt( "burnlevel", m->mBurnLevel );
			break;
		}
		HANDLER( TF_MSG_GOT_DISPENSER_AMMO )
		{
			_cb.CallScript();
			break;
		}
		HANDLER( TF_MSG_RADIOTAG_UPDATE )
		{
			_cb.CallScript();
			const Event_RadioTagUpdate_TF *m = _message.Get<Event_RadioTagUpdate_TF>();
			if ( m->mDetected.IsValid() )
			{
				GetSensoryMemory()->UpdateWithTouchSource( m->mDetected );
				_cb.AddEntity( "detected", m->mDetected );
			}
			else
			{
				DBG_MSG( 0, this, kError, "TF_MSG_RADIOTAG_UPDATE got bad entity" );
			}
			break;
		}
		HANDLER( TF_MSG_RADAR_DETECT_ENEMY )
		{
			_cb.CallScript();
			const Event_RadarUpdate_TF *m = _message.Get<Event_RadarUpdate_TF>();
			if ( m->mDetected.IsValid() )
			{
				GetSensoryMemory()->UpdateWithTouchSource( m->mDetected );
				_cb.AddEntity( "detected", m->mDetected );
			}
			else
			{
				DBG_MSG( 0, this, kError, "TF_MSG_RADAR_DETECT_ENEMY got bad entity" );
			}
			break;
		}
		HANDLER( TF_MSG_CANTDISGUISE_AS_TEAM )
		{
			_cb.CallScript();
			const Event_CantDisguiseTeamTF *m = _message.Get<Event_CantDisguiseTeamTF>();
			_cb.AddInt( "team", m->mTeamId );
			break;
		}
		HANDLER( TF_MSG_CANTDISGUISE_AS_CLASS )
		{
			_cb.CallScript();
			const Event_CantDisguiseClass_TF *m = _message.Get<Event_CantDisguiseClass_TF>();
			_cb.AddInt( "class", m->mClassId );
			break;
		}
		HANDLER( TF_MSG_DISGUISING )
			HANDLER( TF_MSG_DISGUISED )
		{
			_cb.CallScript();
			const Event_Disguise_TF *m = _message.Get<Event_Disguise_TF>();
			_cb.AddInt( "asTeam", m->mTeamId );
			_cb.AddInt( "asClass", m->mClassId );
			break;
		}
		//////////////////////////////////////////////////////////////////////////
		HANDLER( TF_MSG_SENTRY_NOTENOUGHAMMO )
		{
			_cb.CallScript();
			//DBG_MSG(0, .mClient, kNormal, "Sentry NotEnough Ammo");
			break;
		}
		HANDLER( TF_MSG_SENTRY_ALREADYBUILT )
		{
			_cb.CallScript();
			//DBG_MSG(0, .mClient, kNormal, "Sentry Already Built");
			break;
		}
		HANDLER( TF_MSG_SENTRY_CANTBUILD )
		{
			_cb.CallScript();
			//DBG_MSG(0, .mClient, kNormal, "Sentry Can't Build");
			break;
		}
		HANDLER( TF_MSG_SENTRY_BUILDING )
		{
			_cb.CallScript();
			const Event_SentryBuilding_TF *m = _message.Get<Event_SentryBuilding_TF>();
			OBASSERT( m->mSentry.IsValid(), "Entity Expected" );
			//.mSentryStatus.mEntity = m->mSentry;
			//.mSentryStatus.mStatus = BUILDABLE_BUILDING;
			//DBG_MSG(0, .mClient, kNormal, "Sentry Building");
			_cb.AddEntity( "sg", m->mSentry );
			break;
		}
		HANDLER( TF_MSG_SENTRY_BUILT )
		{
			_cb.CallScript();
			const Event_SentryBuilt_TF *m = _message.Get<Event_SentryBuilt_TF>();
			OBASSERT( m->mSentry.IsValid(), "Entity Expected" );
			//.mSentryStatus.mEntity = m->mSentry;

			//EngineFuncs::EntityPosition(.mSentryStatus.mEntity, .mSentryStatus.mPosition);
			//EngineFuncs::EntityOrientation(.mSentryStatus.mEntity, .mSentryStatus.mFacing, NULL, NULL);
			//.mSentryStatus.mStatus = BUILDABLE_BUILT;
			//DBG_MSG(0, .mClient, kNormal, "Sentry Built");
			_cb.AddEntity( "sg", m->mSentry );
			break;
		}
		HANDLER( TF_MSG_SENTRY_DESTROYED )
		{
			_cb.CallScript();
			//memset(&.mSentryStatus, 0, sizeof(.mSentryStatus));
			//DBG_MSG(0, .mClient, kNormal, "Sentry Destroyed");
			break;
		}
		HANDLER( TF_MSG_SENTRY_SPOTENEMY )
		{
			_cb.CallScript();
			const Event_SentrySpotEnemy_TF *m = _message.Get<Event_SentrySpotEnemy_TF>();
			OBASSERT( m->mSpottedEnemy.IsValid(), "Entity Expected" );
			//DBG_MSG(0, .mClient, kNormal, "Sentry Spot Enemy");
			_cb.AddEntity( "enemy", m->mSpottedEnemy );
			break;
		}
		HANDLER( TF_MSG_SENTRY_DAMAGED )
		{
			_cb.CallScript();
			const Event_SentryTakeDamage_TF *m = _message.Get<Event_SentryTakeDamage_TF>();
			OBASSERT( m->mInflictor.IsValid(), "Entity Expected" );
			//DBG_MSG(0, .mClient, kNormal, "Sentry Damaged");
			_cb.AddEntity( "inflictor", m->mInflictor );
			break;
		}
		HANDLER( TF_MSG_SENTRY_STATS )
		{
			/*const Event_SentryStatus_TF *m = _message.Get<Event_SentryStatus_TF>();
		 mSentryStatus.mHealth			= m->mHealth;
		 mSentryStatus.mMaxHealth		= m->mMaxHealth;
		 mSentryStatus.mShells[0]		= m->mShells[0];
		 mSentryStatus.mShells[1]		= m->mShells[1];
		 mSentryStatus.mRockets[0]		= m->mRockets[0];
		 mSentryStatus.mRockets[1]		= m->mRockets[1];
		 mSentryStatus.mLevel			= m->mLevel;

		 DBG_MSG(0, .mClient, kNormal, "Sentry Stats");
		 DBG_MSG(0, .mClient, kNormal, va("Level: %d/%d", .mSentryStatus.mLevel, 3));
		 DBG_MSG(0, .mClient, kNormal, va("Health: %d/%d", .mSentryStatus.mHealth, .mSentryStatus.mMaxHealth));
		 DBG_MSG(0, .mClient, kNormal, va("Shells: %d/%d", .mSentryStatus.mShells[0], .mSentryStatus.mShells[1]));
		 DBG_MSG(0, .mClient, kNormal, va("Rockets: %d/%d", .mSentryStatus.mRockets[0], .mSentryStatus.mRockets[1]));*/
			break;
		}
		HANDLER( TF_MSG_SENTRY_UPGRADED )
		{
			_cb.CallScript();
			const Event_SentryUpgraded_TF *m = _message.Get<Event_SentryUpgraded_TF>();
			//DBG_MSG(0, .mClient, kNormal, "Sentry Upgraded");
			_cb.AddInt( "level", m->mLevel );
			break;
		}
		HANDLER( TF_MSG_SENTRY_DETONATED )
		{
			_cb.CallScript();
			//DBG_MSG(0, .mClient, kNormal, "Sentry Detonated");
			break;
		}
		HANDLER( TF_MSG_SENTRY_DISMANTLED )
		{
			_cb.CallScript();
			//DBG_MSG(0, .mClient, kNormal, "Sentry Dismantled");
			break;
		}
		HANDLER( TF_MSG_DISPENSER_NOTENOUGHAMMO )
		{
			_cb.CallScript();
			//DBG_MSG(0, .mClient, kNormal, "Dispenser Not Enough Ammo");
			break;
		}
		HANDLER( TF_MSG_DISPENSER_ALREADYBUILT )
		{
			_cb.CallScript();
			//DBG_MSG(0, .mClient, kNormal, "Dispenser Already Built");
			break;
		}
		HANDLER( TF_MSG_DISPENSER_CANTBUILD )
		{
			_cb.CallScript();
			//DBG_MSG(0, .mClient, kNormal, "Dispenser Can't Build");
			break;
		}
		HANDLER( TF_MSG_DISPENSER_BUILDING )
		{
			_cb.CallScript();
			const Event_DispenserBuilding_TF *m = _message.Get<Event_DispenserBuilding_TF>();
			OBASSERT( m->mDispenser.IsValid(), "Entity Expected" );
			//.mDispenserStatus.mEntity = m->mDispenser;
			//.mDispenserStatus.mStatus = BUILDABLE_BUILDING;
			//DBG_MSG(0, .mClient, kNormal, "Dispenser Building");
			_cb.AddEntity( "dispenser", m->mDispenser );
			break;
		}
		HANDLER( TF_MSG_DISPENSER_BUILT )
		{
			_cb.CallScript();
			const Event_DispenserBuilt_TF *m = _message.Get<Event_DispenserBuilt_TF>();
			OBASSERT( m->mDispenser.IsValid(), "Entity Expected" );
			//.mDispenserStatus.mEntity = m->mDispenser;
			//EngineFuncs::EntityPosition(.mDispenserStatus.mEntity, .mDispenserStatus.mPosition);
			//EngineFuncs::EntityOrientation(.mDispenserStatus.mEntity, .mDispenserStatus.mFacing, NULL, NULL);
			//.mDispenserStatus.mStatus = BUILDABLE_BUILT;
			//DBG_MSG(0, .mClient, kNormal, "Dispenser Built");
			_cb.AddEntity( "dispenser", m->mDispenser );
			break;
		}
		HANDLER( TF_MSG_DISPENSER_BUILDCANCEL )
		{
			_cb.CallScript();
			//.mDispenserStatus.mEntity.Reset();
			break;
		}
		HANDLER( TF_MSG_DISPENSER_DESTROYED )
		{
			_cb.CallScript();
			//memset(&.mDispenserStatus, 0, sizeof(.mDispenserStatus));
			//DBG_MSG(0, .mClient, kNormal, "Dispenser Destroyed");
			break;
		}
		HANDLER( TF_MSG_DISPENSER_ENEMYUSED )
		{
			_cb.CallScript();
			const Event_DispenserEnemyUsed_TF *m = _message.Get<Event_DispenserEnemyUsed_TF>();
			OBASSERT( m->mEnemy.IsValid(), "Entity Expected" );
			//DBG_MSG(0, .mClient, kNormal, "Dispenser Enemy Used");
			_cb.AddEntity( "usedby", m->mEnemy );
			break;
		}
		HANDLER( TF_MSG_DISPENSER_DAMAGED )
		{
			_cb.CallScript();
			const Event_DispenserTakeDamage_TF *m = _message.Get<Event_DispenserTakeDamage_TF>();
			OBASSERT( m->mInflictor.IsValid(), "Entity Expected" );
			//DBG_MSG(0, .mClient, kNormal, "Dispenser Damaged");
			_cb.AddEntity( "inflictor", m->mInflictor );
			break;
		}
		HANDLER( TF_MSG_DISPENSER_STATS )
		{
			/*const Event_DispenserStatus_TF *m = _message.Get<Event_DispenserStatus_TF>();
		 mDispenserStatus.mHealth	= m->mHealth;
		 mDispenserStatus.mShells	= m->mShells;
		 mDispenserStatus.mNails	= m->mNails;
		 mDispenserStatus.mRockets	= m->mRockets;
		 mDispenserStatus.mCells	= m->mCells;
		 mDispenserStatus.mArmor	= m->mArmor;

		 DBG_MSG(0, .mClient, kNormal, "Dispenser Stats");
		 DBG_MSG(0, .mClient, kNormal, va("Health: %d", .mDispenserStatus.mHealth));
		 DBG_MSG(0, .mClient, kNormal, va("Shells: %d", .mDispenserStatus.mShells));
		 DBG_MSG(0, .mClient, kNormal, va("Nails: %d", .mDispenserStatus.mNails));
		 DBG_MSG(0, .mClient, kNormal, va("Rockets: %d", .mDispenserStatus.mRockets));
		 DBG_MSG(0, .mClient, kNormal, va("Cells: %d", .mDispenserStatus.mCells));
		 DBG_MSG(0, .mClient, kNormal, va("Armor: %d", .mDispenserStatus.mArmor));*/
			break;
		}
		HANDLER( TF_MSG_DISPENSER_DETONATED )
		{
			_cb.CallScript();
			//DBG_MSG(0, .mClient, kNormal, "Dispenser Detonated");
			break;
		}
		HANDLER( TF_MSG_DISPENSER_DISMANTLED )
		{
			_cb.CallScript();
			//DBG_MSG(0, .mClient, kNormal, "Dispenser Dismantled");
			break;
		}
		HANDLER( TF_MSG_DISPENSER_BLOWITUP )
		{
			_cb.CallScript();
			//DBG_MSG(0, .mClient, kNormal, "Dispenser Blowitup");
			break;
		}
		//////////////////////////////////////////////////////////////////////////
		HANDLER( TF_MSG_CALLFORMEDIC )
		{
			_cb.CallScript();
			const Event_MedicCall *m = _message.Get<Event_MedicCall>();
			OBASSERT( m && m->mByWho.IsValid(), "Invalid Message Params" );
			_cb.AddEntity( "who", m->mByWho );
			break;
		}
		//////////////////////////////////////////////////////////////////////////
	}
	Client::ProcessEvent( _message, _cb );
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
		TF_BuildInfo bi = InterfaceFuncs::GetBuildInfo( this );
		FINDSTATEIF_OPT( Sentry, GetStateRoot(), UpdateSentryStatus( bi.mSentryStats ) );
		FINDSTATEIF_OPT( Dispenser, GetStateRoot(), UpdateDispenserStatus( bi.mDispenserStats ) );
		FINDSTATEIF_OPT( Detpack, GetStateRoot(), UpdateDetpackStatus( bi.mDetpack ) );
		FINDSTATEIF_OPT( Teleporter, GetStateRoot(), UpdateTeleporterStatus( bi.mTeleporterStats ) );
	}
	Client::Update();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////