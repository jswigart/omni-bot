////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "Client.h"
#include "IGameManager.h"
#include "IGame.h"
#include "ScriptManager.h"
#include "WeaponDatabase.h"
#include "BotPathing.h"
#include "BotTargetingSystem.h"
#include "BotSteeringSystem.h"
#include "MovementCaps.h"
#include "InterfaceFuncs.h"
#include "ScriptGoal.h"

#include "RenderBuffer.h"

#include "gmBot.h"
#include "gmScriptGoal.h"
#include "gmCall.h"

//////////////////////////////////////////////////////////////////////////

Client::HoldButtons::HoldButtons()
{
	for ( int i = 0; i < NumButtons; ++i )
		mStopHoldTime[ i ] = 0;
}

Client::Client()
	: mDesiredTeam( RANDOM_TEAM_IF_NO_TEAM )
	, mDesiredClass( RANDOM_CLASS_IF_NO_CLASS )
	, mStepHeight( 0.0f )
	, mStateRoot( NULL )
	, mPosition( Vector3f::ZERO )
	, mEyePosition( Vector3f::ZERO )
	, mMoveVector( Vector3f::ZERO )
	, mVelocity( Vector3f::ZERO )
	, mFacingVector( Vector3f::UNIT_Y )
	, mUpVector( Vector3f::UNIT_Z )
	, mRightVector( Vector3f::UNIT_X )
	, mWorldBounds()
	, mButtonFlags( 0 )
	, mFieldOfView( 90.0f )
	, mMaxViewDistance( 10000.0f )
	, mRoleMask( 0 )
	, mTeam( 0 )
	, mGameID( -1 )
	, mScriptObject( 0 )
	, mCurrentTurnSpeed( 0.0f )
	, mMaxTurnSpeed( 720.0f )
	, mAimStiffness( 75.0f )
	, mAimDamping( 10.0f )
	, mAimTolerance( 48.0f )
	, mProfileType( PROFILE_NONE )
{
	memset( &mClientInput, 0, sizeof( mClientInput ) );
#ifdef _DEBUG
	mDebugFlags.SetFlag( BOT_DEBUG_FPINFO );
#endif

	mStuckBounds.Set( Vector3f( -32.f, -32.f, -32.f ), Vector3f( 32.f, 32.f, 32.f ) );

	// Initialize default movement capability
	mMovementCaps.SetFlag( Movement::MOVE_WALK, true );
	mMovementCaps.SetFlag( Movement::MOVE_JUMP, true );
}

Client::~Client()
{
	gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
	if ( mScriptObject )
	{
		pMachine->RemoveCPPOwnedGMObject( mScriptObject );
		gmBot::NullifyObject( mScriptObject );
		mScriptObject = NULL;
	}
}

void Client::Update()
{
	using namespace AiState;

	// Set dirty flags to invalidate caches.
	mInternalFlags.SetFlag( FL_DIRTYEYEPOS );
	
	// update my locally properties with the one the game has for me.
	EngineFuncs::EntityPosition( mGameEntity, mPosition );
	EngineFuncs::EntityLocalAABB( mGameEntity, mLocalBounds );
	EngineFuncs::EntityWorldOBB( mGameEntity, mWorldBounds );
	EngineFuncs::EntityGroundEntity( mGameEntity, mMoveEntity );

	// Update the bots orientation if we haven't turned this frame(script might be controlling)
	EngineFuncs::EntityOrientation( mGameEntity, mFacingVector, mRightVector, mUpVector );
	mOrientation = Matrix3f( mRightVector, mFacingVector, mUpVector, true );

	EngineFuncs::EntityVelocity( mGameEntity, mVelocity );

	Msg_PlayerMaxSpeed maxSpeed;
	if ( InterfaceFuncs::GetMaxSpeed( mGameEntity, maxSpeed ) )
	{
		mMaxSpeed = maxSpeed.mMaxSpeed;
	}

	CheckTeamEvent();
	CheckClassEvent();

	// Update my internal state
	IGame::GetEntityInfo( GetGameEntity(), mEntInfo );
	
	if ( CheckUserFlag( Client::FL_DISABLED ) )
	{
		// stop when disabled.
		mMoveVector = Vector3f::ZERO;
	}
	else
	{
		CheckStuck();

		//////////////////////////////////////////////////////////////////////////
		// Process Sounds
		//const int numMsgs = g_SoundDepot.GetNumMessagesForSubscriber(.mSoundSubscriber);
		//if(numMsgs>0)
		//{
		//	Event_Sound *snds = (Event_Sound*)StackAlloc(sizeof(Event_Sound)*numMsgs);
		//	int n = g_SoundDepot.Collect(snds,numMsgs,.mSoundSubscriber);
		//	OBASSERT(n==numMsgs,"Problem getting events.");
		//	for(int i = 0; i < numMsgs; ++i)
		//	{
		//		//snds[i]
		//	}
		//}
		//////////////////////////////////////////////////////////////////////////

		// Check if we're dead or alive so we know what update function to call.
		if ( mStateRoot )
		{
			mStateRoot->RootUpdate();

			FINDSTATE( path, FollowPath, GetStateRoot() );
			path->RenderDebug();
		}

		// Purge expired blackboard records.
		GetBB().PurgeExpiredRecords();

		// Set any buttons that should be held down.
		if ( !mButtonFlags.CheckFlag( BOT_BUTTON_RESPAWN ) )
		{
			for ( int i = 0; i < HoldButtons::NumButtons; ++i )
			{
				// If it's not already set.
				if ( !IsButtonDown( i ) )
				{
					// Should this button be held down?
					if ( mHoldButtons.mStopHoldTime[ i ] > ( uint32_t )IGame::GetTime() )
					{
						PressButton( i );
					}
				}
			}
		}
	}

	// Update my input with the engine
	mClientInput.mMoveDir[ 0 ] = mMoveVector.X();
	mClientInput.mMoveDir[ 1 ] = mMoveVector.Y();
	mClientInput.mMoveDir[ 2 ] = mMoveVector.Z();
	mClientInput.mFacing[ 0 ] = mFacingVector.X();
	mClientInput.mFacing[ 1 ] = mFacingVector.Y();
	mClientInput.mFacing[ 2 ] = mFacingVector.Z();
	mClientInput.mButtonFlags = mButtonFlags;
	mClientInput.mCurrentWeapon = GetWeaponSystem()->GetDesiredWeaponID();
	{
		//rmt_ScopedCPUSample( UpdateBotInput );
		UpdateBotInput();
	}

#ifdef _DEBUG
	OBASSERT( mFacingVector != Vector3f::ZERO, "Zero Facing Vector" );
#endif

	// Zero the button flags here, if we do it before the UpdateBotInput, any script
	// controlling the bot will get overridden.
	mButtonFlags.ClearAll();
}

void Client::UpdateBotInput()
{
	gEngineFuncs->UpdateBotInput( mGameID, mClientInput );
}

const char *Client::GetName( bool _clean ) const
{
	return gEngineFuncs->GetEntityName( GetGameEntity() );
}

Vector3f Client::GetEyePosition()
{
	if ( mInternalFlags.CheckFlag( FL_DIRTYEYEPOS ) )
	{
		EngineFuncs::EntityEyePosition( mGameEntity, mEyePosition );
		mInternalFlags.ClearFlag( FL_DIRTYEYEPOS );
	}
	return mEyePosition;
}

Vector3f Client::GetEyeGroundOffset()
{
	return GetEyePosition() - GetWorldBounds().GetCenterBottom();
}

gmUserObject *Client::GetScriptObject()
{
	return mScriptObject;
}

gmVariable Client::GetScriptVariable()
{
	return mScriptObject ? gmVariable( mScriptObject ) : gmVariable::s_null;
}

void Client::ProcessEvent( const MessageHelper &_message, CallbackParameters &_cb )
{
	switch ( _message.GetMessageId() )
	{
		HANDLER( GAME_CLIENTDISCONNECTED )
		{
			_cb.CallScript( true );
			const Event_SystemClientDisConnected *m = _message.Get<Event_SystemClientDisConnected>();
			GetBB().RemoveBBRecordByPoster( m->mGameId, bbk_All );
			GetBB().RemoveBBRecordByTarget( m->mGameId, bbk_All );
			break;
		}
		HANDLER( MESSAGE_SPAWN )
		{
			// Clear the goal list so the bot can re-evaluate the goals
			_cb.CallScript();
			ResetStuckTime();
			ReleaseAllHeldButtons();
			break;
		}
		HANDLER( MESSAGE_DEATH )
		{
			const Event_Death *m = _message.Get<Event_Death>();
			_cb.CallScript();
			_cb.AddEntity( "inflictor", m->mWhoKilledMe );
			_cb.AddString( "meansofdeath", m->mMeansOfDeath );
			break;
		}
		HANDLER( MESSAGE_HEALED )
		{
			const Event_Healed *m = _message.Get<Event_Healed>();
			_cb.CallScript();
			_cb.AddEntity( "who", m->mWhoHealedMe );
			break;
		}
		HANDLER( MESSAGE_REVIVED )
		{
			const Event_Revived *m = _message.Get<Event_Revived>();
			_cb.CallScript();
			_cb.AddEntity( "who", m->mWhoRevivedMe );
			break;
		}
		HANDLER( MESSAGE_KILLEDSOMEONE )
		{
			const Event_KilledSomeone *m = _message.Get<Event_KilledSomeone>();
			_cb.CallScript();
			_cb.AddEntity( "victim", m->mWhoIKilled );
			_cb.AddString( "meansofdeath", m->mMeansOfDeath );
			break;
		}
		HANDLER( MESSAGE_CHANGETEAM )
		{
			const Event_ChangeTeam *m = _message.Get<Event_ChangeTeam>();
			_cb.CallScript();
			_cb.AddInt( "newteam", m->mNewTeam );
			mTeam = m->mNewTeam;
			break;
		}
		HANDLER( MESSAGE_CHANGECLASS )
		{
			const Event_ChangeClass *m = _message.Get<Event_ChangeClass>();
			_cb.CallScript();
			_cb.AddInt( "classId", m->mNewClass );

			if ( GetProfileType() != Client::PROFILE_CUSTOM )
				LoadProfile( Client::PROFILE_CLASS );
			break;
		}
		HANDLER( MESSAGE_SPECTATED )
		{
			const Event_Spectated *m = _message.Get<Event_Spectated>();
			m;
			if ( IsDebugEnabled( BOT_DEBUG_FPINFO ) )
			{
				std::stringstream strOutString;

				using namespace AiState;
				FINDSTATE( hl, HighLevel, GetStateRoot() );
				if ( hl != NULL && hl->GetActiveState() )
				{
					State *ActiveState = hl->GetActiveState();
					while ( ActiveState->GetActiveState() )
						ActiveState = ActiveState->GetActiveState();

					/*strOutString << hl->GetCurrentState()->GetName() << " : ";*/
					strOutString << ActiveState->GetName() << std::endl;
					ActiveState->GetDebugString( strOutString );
					strOutString << std::endl;
				}
				else
				{
					strOutString << "No Goal" << std::endl;
				}

				const MemoryRecord *pTargetRecord = GetTargetingSystem()->GetCurrentTargetRecord();
				if ( pTargetRecord )
				{
					//strOutString << "Target: " <<
					//	pTargetRecord->mTargetInfo.mDistanceTo <<
					//	" units away." << std::endl;
					strOutString
						<< "Target: "
						<< EngineFuncs::EntityName( pTargetRecord->GetEntity(), "<unknown>" )
						<< " "
						<< pTargetRecord->mTargetInfo.mDistanceTo
						<< " units away."
						<< std::endl;
				}
				else
					strOutString << "No Target" << std::endl;

				GetWeaponSystem()->GetSpectateMessage( strOutString );
				//gEngineFuncs->PrintScreenText(NULL,
				//	IGame::GetDeltaTimeSecs()*2.f, COLOR::WHITE, strOutString.str().c_str());
				RenderBuffer::AddString2d( Vector2f( 0.5f, 0.5f ), COLOR::WHITE, strOutString.str().c_str() );
			}
			break;
		}
		HANDLER( MESSAGE_SCRIPTMSG )
		{
			const Event_ScriptMessage *m = _message.Get<Event_ScriptMessage>();
			_cb.CallScript();
			_cb.AddString( "msg", m->mMessageName );
			_cb.AddString( "data1", m->mMessageData1 );
			_cb.AddString( "data2", m->mMessageData2 );
			_cb.AddString( "data3", m->mMessageData3 );
			break;
		}
		HANDLER( MESSAGE_PROXIMITY_TRIGGER )
		{
			const AiState::Event_ProximityTrigger *m = _message.Get<AiState::Event_ProximityTrigger>();
			_cb.CallScript();
			_cb.AddString( "owner", Utils::HashToString( m->mOwnerState ).c_str() );
			_cb.AddEntity( "ent", m->mEntity );
			_cb.AddVector( "position", m->mPosition );
			break;
		}
		HANDLER( MESSAGE_ADDWEAPON )
		{
			const Event_AddWeapon *m = _message.Get<Event_AddWeapon>();
			if ( !GetWeaponSystem()->HasWeapon( m->mWeaponId ) )
			{
				// Add weapons according to the event.
				if ( GetWeaponSystem()->AddWeaponToInventory( m->mWeaponId ) )
				{
					_cb.CallScript();
					_cb.AddInt( "weapon", m->mWeaponId );
				}
				else
				{
					int NumElements = 0;
					const IntEnum *Enum = 0;
					System::mInstance->mGame->GetWeaponEnumeration( Enum, NumElements );

					bool foundName = false;
					for ( int i = 0; i < NumElements; ++i )
					{
						if ( Enum[ i ].mValue == m->mWeaponId )
						{
							foundName = true;
							EngineFuncs::ConsoleError( va( "AddWeaponToInventory: Unknown Weapon( %s )", Enum[ i ].mKey ) );
							break;
						}
					}

					if ( !foundName )
					{
						EngineFuncs::ConsoleError( va( "AddWeaponToInventory: Unknown Weapon Id( %d )", m->mWeaponId ) );
					}
				}
			}
			break;
		}
		HANDLER( MESSAGE_REMOVEWEAPON )
		{
			const Event_RemoveWeapon *m = _message.Get<Event_RemoveWeapon>();
			if ( GetWeaponSystem()->HasWeapon( m->mWeaponId ) )
			{
				_cb.CallScript();
				_cb.AddInt( "weapon", m->mWeaponId );
				GetWeaponSystem()->RemoveWeapon( m->mWeaponId );
			}
			break;
		}
		HANDLER( MESSAGE_REFRESHALLWEAPONS )
		{
			GetWeaponSystem()->RefreshAllWeapons();
			break;
		}
		HANDLER( MESSAGE_REFRESHWEAPON )
		{
			const Event_RefreshWeapon *m = _message.Get<Event_RefreshWeapon>();
			GetWeaponSystem()->RefreshWeapon( m->mWeaponId );

			_cb.CallScript();
			_cb.AddInt( "weapon", m->mWeaponId );
			break;
		}
		HANDLER( MESSAGE_ENT_ENTER_RADIUS )
		{
			const Event_EntEnterRadius *m = _message.Get<Event_EntEnterRadius>();
			//_cb.CallScript();
			_cb.AddEntity( "ent", m->mEntity );
			break;
		}
		HANDLER( MESSAGE_ENT_LEAVE_RADIUS )
		{
			const Event_EntLeaveRadius *m = _message.Get<Event_EntLeaveRadius>();
			//_cb.CallScript();
			_cb.AddEntity( "ent", m->mEntity );
			break;
		}
		HANDLER( ACTION_WEAPON_CHANGE )
		{
			const Event_WeaponChanged *m = _message.Get<Event_WeaponChanged>();
			if ( GetWeaponSystem()->HasWeapon( m->mWeaponId ) )
			{
				_cb.CallScript();
				_cb.AddInt( "weapon", m->mWeaponId );

				// Special case signal.
				GetStateRoot()->SignalThreads( gmVariable( Utils::MakeId32( (int16_t)ACTION_WEAPON_CHANGE, (int16_t)m->mWeaponId ) ) );
			}
			break;
		}
		HANDLER( ACTION_WEAPON_FIRE )
		{
			const Event_WeaponFire *m = _message.Get<Event_WeaponFire>();
			_cb.CallScript();
			_cb.AddInt( "weapon", m->mWeaponId );
			_cb.AddEntity( "projectile", m->mProjectile );

			// Shot fired callback.
			WeaponPtr curWpn = GetWeaponSystem()->GetCurrentWeapon();
			if ( curWpn && curWpn->GetWeaponID() == m->mWeaponId )
				curWpn->ShotFired( m->mFireMode, m->mProjectile );

			// Special case signal.
			GetStateRoot()->SignalThreads( gmVariable( Utils::MakeId32( (int16_t)ACTION_WEAPON_FIRE, (int16_t)m->mWeaponId ) ) );
			break;
		}
		HANDLER( PERCEPT_FEEL_PLAYER_USE )
		{
			const Event_PlayerUsed *m = _message.Get<Event_PlayerUsed>();
			_cb.CallScript();
			_cb.AddEntity( "toucher", m->mWhoDidIt );
			break;
		}
		HANDLER( PERCEPT_FEEL_PAIN )
		{
			const Event_TakeDamage *m = _message.Get<Event_TakeDamage>();
			_cb.CallScript();
			_cb.AddEntity( "inflictor", m->mInflictor );
			_cb.AddInt( "previoushealth", GetCurrentHealth() );
			gEngineFuncs->GetEntityInfo( GetGameEntity(), mEntInfo );
			_cb.AddInt( "currenthealth", GetCurrentHealth() );
			_cb.AddString( "damageType", m->mDamageType );

			if ( m->mInflictor.IsValid() && IGame::IsEntityValid( m->mInflictor ) )
			{
				GetSensoryMemory()->UpdateWithTouchSource( m->mInflictor );
			}
			break;
		}
		HANDLER( PERCEPT_HEAR_GLOBALVOICEMACRO )
			HANDLER( PERCEPT_HEAR_TEAMVOICEMACRO )
			HANDLER( PERCEPT_HEAR_PRIVATEVOICEMACRO )
		{
			const Event_VoiceMacro *m = _message.Get<Event_VoiceMacro>();
			if ( m->mWhoSaidIt != GetGameEntity() ) // Ignore messages from myself.
			{
				_cb.CallScript();

				int macroId = HandleVoiceMacroEvent( _message );

				_cb.AddEntity( "who", m->mWhoSaidIt );
				_cb.AddInt( "macro", macroId );

				// signal any pending threads
				gmVariable s( Utils::MakeId32( (int16_t)PERCEPT_HEAR_VOICEMACRO, (int16_t)macroId ) );
				GetStateRoot()->SignalThreads( s );
			}
			else
				_cb.DontPropogateEvent();
			break;
		}
		HANDLER( PERCEPT_HEAR_GLOBALCHATMSG )
			HANDLER( PERCEPT_HEAR_TEAMCHATMSG )
			HANDLER( PERCEPT_HEAR_PRIVCHATMSG )
			HANDLER( PERCEPT_HEAR_GROUPCHATMSG )
		{
			const Event_ChatMessage *m = _message.Get<Event_ChatMessage>();
			if ( m->mWhoSaidIt != GetGameEntity() ) // Ignore messages from myself.
			{
				_cb.CallScript();
				_cb.AddEntity( "who", m->mWhoSaidIt );
				_cb.AddString( "msg", m->mMessage );
			}
			else
				_cb.DontPropogateEvent();
			break;
		}
		HANDLER( PERCEPT_HEAR_SOUND )
		{
			const Event_Sound *m = _message.Get<Event_Sound>();
			if ( m->mSource != GetGameEntity() )
			{
				_cb.CallScript();
				_cb.AddEntity( "source", m->mSource );
				_cb.AddVector( "origin", m->mOrigin[ 0 ], m->mOrigin[ 1 ], m->mOrigin[ 2 ] );
				_cb.AddInt( "soundId", m->mSoundType );
				_cb.AddString( "soundName", m->mSoundName );
				GetSensoryMemory()->UpdateWithSoundSource( m );
			}
			break;
		}
		HANDLER( PERCEPT_SENSE_ENTITY )
		{
			const Event_EntitySensed *m = _message.Get<Event_EntitySensed>();
			_cb.CallScript();
			_cb.AddInt( "sensedclass", m->mEntityClass );
			_cb.AddEntity( "sensedentity", m->mEntity );
			break;
		}
	}
}

void Client::Init( int _gameid )
{
	// Set the game id, which is the index into the mClients array.
	mGameID = _gameid;
	mGameEntity = gEngineFuncs->EntityFromID( mGameID );

	//.mSoundSubscriber = g_SoundDepot.Subscribe("Client");

	// mark name as taken
	if ( const char *pName = gEngineFuncs->GetEntityName( GetGameEntity() ) )
		mNameReference = NameManager::GetInstance()->GetName( pName );

	// Only create these if they aren't already defined,
	// the mod may override them with derived classes.

	// Add this bot to the global script table.
	mScriptObject = ScriptManager::GetInstance()->AddBotToGlobalTable( this );

	gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();

	InitBehaviorTree();
	InitScriptGoals();

	LoadProfile( PROFILE_CUSTOM );

	//////////////////////////////////////////////////////////////////////////
	// Call any map callbacks.
	gmCall call;
	if ( call.BeginGlobalFunction( pMachine, "OnBotJoin", gmVariable::s_null, true ) )
	{
		OBASSERT( mScriptObject, "No Script object." );
		call.AddParamUser( mScriptObject );
		call.End();
	}
	//////////////////////////////////////////////////////////////////////////
}

void Client::Shutdown()
{
	//////////////////////////////////////////////////////////////////////////
	// Call any map callbacks.
	gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
	gmCall call;
	if ( call.BeginGlobalFunction( pMachine, "OnBotLeave", gmVariable::s_null, true ) )
	{
		OBASSERT( mScriptObject, "No Script object." );
		call.AddParamUser( mScriptObject );
		call.End();
	}

	if ( mStateRoot )
	{
		mStateRoot->ExitAll();
	}
	OB_DELETE( mStateRoot );

	ScriptManager::GetInstance()->RemoveFromGlobalTable( this );
}

void Client::CheckStuck()
{
	if ( mStuckBounds.Contains( GetPosition() ) )
		mStuckTime += IGame::GetDeltaTime();
	else
	{
		ResetStuckTime();
		mStuckBounds.Set( Vector3f( -32.f, -32.f, -32.f ), Vector3f( 32.f, 32.f, 32.f ) );
		mStuckBounds.SetCenter( GetPosition() );
	}

	/*if(.mVelocity.SquaredLength() < (.mMinSpeed*.mMinSpeed))
	return true;
	else
	return false;*/
}

bool Client::TurnTowardFacing( const Vector3f &_facing )
{
	return TurnTowardPosition( GetEyePosition() + _facing );
}

bool Client::TurnTowardPosition( const Vector3f &_pos )
{
	// return true if we're facing close enough
	Vector3f newFacing = ( _pos - GetEyePosition() );
	newFacing.Normalize();

	//{
	//	float heading = 0.f, pitch = 0.f, radius = 0.f;
	//	newFacing.ToSpherical(heading, pitch, radius);
	//
	//	//Mathf::UnitCircleNormalize();
	//}

	if ( newFacing == Vector3f::ZERO )
		return false;
	OBASSERT( mFacingVector != Vector3f::ZERO, "Zero Facing Vector" );

	// See how close we are to
	float fDot = mFacingVector.Dot( newFacing );

	// Determine the angle between the 2 different facings.
	float fAngle = Mathf::ACos( fDot );

	Ray3f rFacing( GetEyePosition(), GetFacingVector() );
	DistPoint3Ray3f dist( _pos, rFacing );
	float fDistance = dist.Get();

	// If it's very close, just snap it.
	bool bInTolerance = fDistance < mAimTolerance ? true : false;

	// Calculate the frame time.
	const float fFrameTime = IGame::GetDeltaTimeSecs();

	// Calculate the Turn speed and clamp it to the max.
	const float fTurnSpeedRadians = Mathf::DegToRad( GetMaxTurnSpeed() );
	mCurrentTurnSpeed += ( fFrameTime * ( ( mAimStiffness * fAngle ) - ( mAimDamping * mCurrentTurnSpeed ) ) );
	mCurrentTurnSpeed = ClampT<float>( mCurrentTurnSpeed, -fTurnSpeedRadians, fTurnSpeedRadians );

	OBASSERT( mFacingVector != Vector3f::ZERO, "Zero Facing Vector" );

	Quaternionf qQuat, qPartial;
	qQuat.Align( mFacingVector, newFacing );

	if ( fAngle > Mathf::ZERO_TOLERANCE )
	{
		qPartial.Slerp( ( mCurrentTurnSpeed / fAngle ) * fFrameTime, Quaternionf::IDENTITY, qQuat );
		mFacingVector = qPartial.Rotate( mFacingVector );
		mFacingVector.Normalize();
	}
	else
	{
		qPartial = qQuat;
		mFacingVector = newFacing;
	}
	assert( mFacingVector != Vector3f::ZERO );
	return bInTolerance;
}

bool Client::HasLineOfSightTo( const Vector3f &_pos, GameEntity _entity, int customTraceMask )
{
	Vector3f vStart = GetEyePosition();
	return HasLineOfSightTo( vStart, _pos, _entity, mGameID, customTraceMask );
}

bool Client::HasLineOfSightTo( const Vector3f &_pos1, const Vector3f &_pos2,
	GameEntity _ent, int _ignoreent, int customTraceMask )
{
	//rmt_ScopedCPUSample( HasLineOfSightTo );
	obTraceResult tr;
	EngineFuncs::TraceLine( tr, _pos1, _pos2,
		NULL, customTraceMask ? customTraceMask : TR_MASK_SHOT | TR_MASK_SMOKEBOMB, _ignoreent, True );
	return ( tr.mFraction == 1.0f ) || ( ( _ent.IsValid() ) && ( tr.mHitEntity == _ent ) );
}

bool Client::MoveTo( const Vector3f &_pos, float _tolerance, MoveMode _m )
{
	GetSteeringSystem()->SetTarget( _pos, _tolerance, _m );
	return ( ( _pos - GetPosition() ).SquaredLength() <= ( _tolerance * _tolerance ) );
}

void Client::EnableDebug( const int _flag, bool _enable )
{
	if ( _enable )
	{
		mDebugFlags.SetFlag( _flag );
	}
	else
	{
		mDebugFlags.ClearFlag( _flag );
	}

	if ( mDebugFlags.CheckFlag( BOT_DEBUG_LOG ) )
	{
		if ( IsDebugEnabled( BOT_DEBUG_LOG ) )
		{
			mDebugLog.OpenForWrite( va( "user/log_%s.rtf", GetName() ), File::Text );

			if ( mDebugLog.IsOpen() )
			{
				mDebugLog.WriteString( "Debug Log : " );
				mDebugLog.WriteString( GetName() );
				mDebugLog.WriteNewLine();
			}
		}
		else
		{
			mDebugLog.Close();
		}
	}

	EngineFuncs::ConsoleMessage( va( "debugging for %s: %s.",
		GetName(),
		IsDebugEnabled( _flag ) ? "enabled" : "disabled" ) );
}

void Client::OutputDebug( MessageType _type, const char * _str )
{
#ifndef _DEBUG
	if(_type == kDebug)
		return;
#endif

	EngineFuncs::ConsoleMessage( va( "%s: %s", GetName( true ), _str ) );

	if ( IsDebugEnabled( BOT_DEBUG_LOG ) )
	{
		// dump to cout
		if ( mDebugLog.IsOpen() )
		{
			mDebugLog.WriteString( _str );
			mDebugLog.WriteNewLine();
		}
	}
}

void Client::LoadProfile( ProfileType _type )
{
	std::string strProfileName;

	switch ( _type )
	{
		case PROFILE_CUSTOM:
		{
			const char *pName = GetName();
			if ( pName )
			{
				strProfileName = NameManager::GetInstance()->GetProfileForName( pName );
			}
		}
		break;
		case PROFILE_CLASS:
		{
			strProfileName = NameManager::GetInstance()->GetProfileForClass( GetClass() );
		}
		break;
		default:
			break;
	}

	// Load the profile.
	if ( !strProfileName.empty() )
	{
		if ( !strProfileName.empty() && mScriptObject )
		{
			int threadId;
			gmVariable thisVar( mScriptObject );
			if ( ScriptManager::GetInstance()->ExecuteFile( filePath( "scripts/%s", strProfileName.c_str() ), threadId, &thisVar ) ||
				ScriptManager::GetInstance()->ExecuteFile( filePath( "global_scripts/%s", strProfileName.c_str() ), threadId, &thisVar ) )
			{
				DBG_MSG( BOT_DEBUG_SCRIPT, this, kNormal, va( "Profile Loaded: %s", strProfileName.c_str() ) );
			}
			else
			{
				DBG_MSG( BOT_DEBUG_SCRIPT, this, kError, va( "Unable to load profile: %s", strProfileName.c_str() ) );
			}

			mProfileType = _type;
		}
	}
}

void Client::ClearProfile()
{
	// No class profile for this new class, so clear the profile
	ScriptManager::GetInstance()->RemoveFromGlobalTable( this );
	ScriptManager::GetInstance()->AddBotToGlobalTable( this );
	mProfileType = PROFILE_NONE;
}

void Client::ProcessEventImpl( const MessageHelper &_message, uint32_t _targetState )
{
	//rmt_ScopedCPUSample( ClientProcessEvent );

	gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();

	DisableGCInScope gcEn( pMachine );

	CallbackParameters cb( _message.GetMessageId(), pMachine );
	cb.SetTargetState( _targetState );
	ProcessEvent( _message, cb );

	if ( IsDebugEnabled( BOT_DEBUG_EVENTS ) )
		cb.PrintDebug();

	// Events to Behavior Tree
	if ( GetStateRoot() && cb.ShouldPropogateEvent() )
		GetStateRoot()->CheckForCallbacks( _message, cb );
}

void Client::PropogateDeletedThreads( const int *_threadIds, int _numThreads )
{
	mStateRoot->PropogateDeletedThreads( _threadIds, _numThreads );
}

bool Client::DistributeUnhandledCommand( const StringVector & args )
{
	return mStateRoot->StateCommand( args );
}

void Client::ChangeTeam( int _team )
{
	// needs testing
	gEngineFuncs->ChangeTeam( GetGameID(), _team, NULL );
}

void Client::ChangeClass( int _class )
{
	// needs testing
	gEngineFuncs->ChangeClass( GetGameID(), _class, NULL );
}

Vector3f Client::ToLocalSpace( const Vector3f &_worldpos )
{
	Matrix3f mTransform( GetRightVector(), GetFacingVector(), GetUpVector(), true );
	mTransform.Inverse();
	return ( _worldpos - GetPosition() ) * mTransform;
}

Vector3f Client::ToWorldSpace( const Vector3f &_localpos )
{
	Matrix3f mTransform( GetRightVector(), GetFacingVector(), GetUpVector(), true );
	return GetPosition() + _localpos * mTransform;
}

void Client::GameCommand( const char* _msg, ... )
{
	const int iBufferSize = 1024;
	char buffer[ iBufferSize ] = { 0 };
	va_list list;
	va_start( list, _msg );
#ifdef WIN32
	_vsnprintf( buffer, iBufferSize, _msg, list );
#else
	vsnprintf(buffer, iBufferSize, _msg, list);
#endif
	va_end( list );
	gEngineFuncs->BotCommand( GetGameID(), buffer );
}

void Client::CheckTeamEvent()
{
	// Check our team.
	int iCurrentTeam = gEngineFuncs->GetEntityTeam( GetGameEntity() );
	if ( iCurrentTeam != mTeam )
	{
		// Update our team.
		mTeam = iCurrentTeam;

		// Send a change team event.
		Event_ChangeTeam d = { iCurrentTeam };
		SendEvent( MessageHelper( MESSAGE_CHANGETEAM, &d, sizeof( d ) ) );
	}
}

void Client::CheckClassEvent()
{
	EntityInfo currentInfo;
	IGame::GetEntityInfo( GetGameEntity(), currentInfo );
	if ( currentInfo.mClassId != mEntInfo.mClassId )
	{
		// Send a change class event.
		Event_ChangeClass d = { currentInfo.mClassId };
		SendEvent( MessageHelper( MESSAGE_CHANGECLASS, &d, sizeof( d ) ) );
	}
}

bool Client::IsAllied( const GameEntity _ent ) const
{
	return InterfaceFuncs::IsAllied( GetGameEntity(), _ent );
}

void Client::PressButton( int _button )
{
	mButtonFlags.SetFlag( _button );
}

void Client::ReleaseButton( int _button )
{
	mButtonFlags.ClearFlag( _button );
}

bool Client::IsButtonDown( int _button ) const
{
	return mButtonFlags.CheckFlag( _button );
}

void Client::HoldButton( const BitFlag64 &_buttons, int _mstime )
{
	for ( int i = 0; i < HoldButtons::NumButtons; ++i )
	{
		if ( _buttons.CheckFlag( i ) )
		{
			mHoldButtons.mStopHoldTime[ i ] =
				( _mstime > 0 ) ? IGame::GetTime() + _mstime : std::numeric_limits<int>::max();
		}
	}
}

void Client::ReleaseHeldButton( const BitFlag64 &_buttons )
{
	for ( int i = 0; i < HoldButtons::NumButtons; ++i )
	{
		if ( _buttons.CheckFlag( i ) )
		{
			mHoldButtons.mStopHoldTime[ i ] = 0;
		}
	}
}

void Client::ReleaseAllHeldButtons()
{
	for ( int i = 0; i < HoldButtons::NumButtons; ++i )
	{
		mHoldButtons.mStopHoldTime[ i ] = 0;
	}
}

bool Client::CanGetPowerUp( int32_t _powerup ) const
{
	return !GetPowerUpFlags().CheckFlag( _powerup );
}

//////////////////////////////////////////////////////////////////////////

AiState::WeaponSystem *Client::GetWeaponSystem()
{
	using namespace AiState;

	FINDSTATE( weaponsys, WeaponSystem, GetStateRoot() );
	return weaponsys;
}

AiState::TargetingSystem *Client::GetTargetingSystem()
{
	using namespace AiState;

	FINDSTATE( targetsys, TargetingSystem, GetStateRoot() );
	return targetsys;
}

AiState::SteeringSystem *Client::GetSteeringSystem()
{
	using namespace AiState;

	FINDSTATE( steersys, SteeringSystem, GetStateRoot() );
	return steersys;
}

AiState::SensoryMemory *Client::GetSensoryMemory()
{
	using namespace AiState;

	FINDSTATE( sensory, SensoryMemory, GetStateRoot() );
	return sensory;
}

void Client::InitBehaviorTree()
{
	mStateRoot = new AiState::Root;
	mStateRoot->FixRoot();
	SetupBehaviorTree();
	mStateRoot->FixRoot();
	mStateRoot->SetClient( this );
	mStateRoot->InitializeStates();
}

bool Client::AddScriptGoal( const std::string &_name )
{
	using namespace AiState;

	bool bSuccess = false;

	gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
	gmTableObject *pScriptGoals = pMachine->GetGlobals()->Get( pMachine, "ScriptGoals" ).GetTableObjectSafe();
	if ( pScriptGoals )
	{
		gmVariable v = pScriptGoals->Get( pMachine, _name.c_str() );
		if ( v.m_type == gmScriptGoal::GetType() )
		{
			gmScriptGoal::gmBindUserObject *pBnd = gmScriptGoal::GetUserBoundObject( pMachine, v );
			if ( pBnd )
			{
				ScriptGoal *pNewScriptGoal = pBnd->m_object->Clone();

				if ( pBnd->m_object->GetParentNameHash() )
				{
					if ( GetStateRoot()->AppendTo( pBnd->m_object->GetParentNameHash(), pNewScriptGoal ) )
					{
						pNewScriptGoal->FixRoot();
						pNewScriptGoal->SetClient( this );

						if ( pNewScriptGoal->OnInit( pMachine ) )
						{
							bSuccess = true;
						}
						else
						{
							delete GetStateRoot()->RemoveState( pNewScriptGoal->GetName().c_str() );
						}
					}
				}
				else if ( pBnd->m_object->GetInsertBeforeHash() )
				{
					if ( GetStateRoot()->InsertBefore( pBnd->m_object->GetInsertBeforeHash(), pNewScriptGoal ) )
					{
						pNewScriptGoal->FixRoot();
						pNewScriptGoal->SetClient( this );

						if ( pNewScriptGoal->OnInit( pMachine ) )
						{
							bSuccess = true;
						}
						else
						{
							delete GetStateRoot()->RemoveState( pNewScriptGoal->GetName().c_str() );
						}
					}
				}
				else if ( pBnd->m_object->GetInsertAfterHash() )
				{
					if ( GetStateRoot()->InsertBefore( pBnd->m_object->GetInsertAfterHash(), pNewScriptGoal ) )
					{
						pNewScriptGoal->FixRoot();
						pNewScriptGoal->SetClient( this );

						if ( pNewScriptGoal->OnInit( pMachine ) )
						{
							bSuccess = true;
						}
						else
						{
							delete GetStateRoot()->RemoveState( pNewScriptGoal->GetName().c_str() );
						}
					}
				}
			}
		}
	}
	return bSuccess;
}

void Client::InitScriptGoals()
{
	using namespace AiState;

	gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
	gmTableObject *pScriptGoals = pMachine->GetGlobals()->Get( pMachine, "ScriptGoals" ).GetTableObjectSafe();
	if ( pScriptGoals )
	{
		gmTableIterator tIt;
		gmTableNode *pNode = pScriptGoals->GetFirst( tIt );
		while ( pNode )
		{
			if ( pNode->m_value.m_type == gmScriptGoal::GetType() )
			{
				gmScriptGoal::gmBindUserObject *pBnd = gmScriptGoal::GetUserBoundObject( pMachine, pNode->m_value );
				if ( pBnd )
				{
					if ( pBnd->m_object->IsAutoAdd() )
					{
						ScriptGoal *pNewScriptGoal = pBnd->m_object->Clone();
						if ( pBnd->m_object->GetParentNameHash() )
						{
							if ( GetStateRoot()->AppendTo( pBnd->m_object->GetParentNameHash(), pNewScriptGoal ) )
							{
								pNewScriptGoal->FixRoot();
								pNewScriptGoal->SetClient( this );

								if ( !pNewScriptGoal->OnInit( pMachine ) )
								{
									delete GetStateRoot()->RemoveState( pNewScriptGoal->GetName().c_str() );
								}
							}
						}
						else if ( pBnd->m_object->GetInsertBeforeHash() )
						{
							if ( GetStateRoot()->InsertBefore( pBnd->m_object->GetInsertBeforeHash(), pNewScriptGoal ) )
							{
								pNewScriptGoal->FixRoot();
								pNewScriptGoal->SetClient( this );

								if ( !pNewScriptGoal->OnInit( pMachine ) )
								{
									delete GetStateRoot()->RemoveState( pNewScriptGoal->GetName().c_str() );
								}
							}
						}
						else if ( pBnd->m_object->GetInsertAfterHash() )
						{
							if ( GetStateRoot()->InsertBefore( pBnd->m_object->GetInsertAfterHash(), pNewScriptGoal ) )
							{
								pNewScriptGoal->FixRoot();
								pNewScriptGoal->SetClient( this );

								if ( !pNewScriptGoal->OnInit( pMachine ) )
								{
									delete GetStateRoot()->RemoveState( pNewScriptGoal->GetName().c_str() );
								}
							}
						}
					}
				}
			}
			pNode = pScriptGoals->GetNext( tIt );
		}
	}
}