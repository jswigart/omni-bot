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
#include "Behaviors.h"
#include "MovementCaps.h"
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
	
	// update my locally properties with the one the game has for me.	
	EngineFuncs::EntityPosition( mGameEntity, mPosition );
	EngineFuncs::EntityEyePosition( mGameEntity, mEyePosition );
	EngineFuncs::EntityLocalAABB( mGameEntity, mLocalBounds );
	EngineFuncs::EntityWorldOBB( mGameEntity, mWorldBounds );
	EngineFuncs::EntityGroundEntity( mGameEntity, mMoveEntity );

	// Update the bots orientation if we haven't turned this frame(script might be controlling)
	EngineFuncs::EntityOrientation( mGameEntity, mFacingVector, mRightVector, mUpVector );
	mOrientation = Matrix3f( mRightVector, mFacingVector, mUpVector, true );

	EngineFuncs::EntityVelocity( mGameEntity, mVelocity );

	gEngineFuncs->GetMaxSpeed( mGameEntity, mMaxSpeed );

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

			//FINDSTATEIF( Navigator, GetStateRoot(), RenderDebug() );
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
	
	// Zero the button flags here, if we do it before the UpdateBotInput, any script
	// controlling the bot will get overridden.
	mButtonFlags.ClearAll();
}

void Client::UpdateBotInput()
{
	gEngineFuncs->UpdateBotInput( mGameID, mClientInput );
}

const std::string& Client::GetName( bool _clean ) const
{
	obStringBuffer nameBuffer;
	gEngineFuncs->GetEntityName( GetGameEntity(), nameBuffer );
	mName = nameBuffer.mBuffer;
	return mName;
}

const Vector3f& Client::GetEyePosition()
{
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

void Client::ProcessEvent( const Message &message, CallbackParameters & cb )
{
	switch ( message.Id() )
	{
		CASE_MSG( EvClientDisconnected )
		{
			cb.CallScript( true );
			GetBB().RemoveBBRecordByPoster( msg->mGameId, bbk_All );
			GetBB().RemoveBBRecordByTarget( msg->mGameId, bbk_All );
			break;
		}
		CASE_MSG( EvSpawn )
		{
			// Clear the goal list so the bot can re-evaluate the goals
			cb.CallScript();
			ResetStuckTime();
			ReleaseAllHeldButtons();
			break;
		}
		CASE_MSG( EvDeath )
		{
			cb.CallScript();
			cb.AddEntity( "inflictor", msg->mWhoKilledMe );
			cb.AddString( "meansofdeath", msg->mMeansOfDeath );
			break;
		}
		CASE_MSG( EvHealed )
		{
			cb.CallScript();
			cb.AddEntity( "who", msg->mWhoHealedMe );
			break;
		}
		CASE_MSG( EvRevived )
		{
			cb.CallScript();
			cb.AddEntity( "who", msg->mWhoRevivedMe );
			break;
		}
		CASE_MSG( EvKilledSomeone )
		{
			cb.CallScript();
			cb.AddEntity( "victim", msg->mWhoIKilled );
			cb.AddString( "meansofdeath", msg->mMeansOfDeath );
			break;
		}
		CASE_MSG( EvChangeTeam )
		{
			cb.CallScript();
			cb.AddInt( "newteam", msg->mNewTeam );
			mTeam = msg->mNewTeam;
			break;
		}
		CASE_MSG( EvChangeClass )
		{
			cb.CallScript();
			cb.AddInt( "classId", msg->mNewClass );

			if ( GetProfileType() != Client::PROFILE_CUSTOM )
				LoadProfile( Client::PROFILE_CLASS );
			break;
		}
		CASE_MSG( EvSpectated )
		{
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
		CASE_MSG( EvScriptMessage )
		{
			cb.CallScript();
			cb.AddString( "msg", msg->mMessageName );
			cb.AddString( "data1", msg->mMessageData1 );
			cb.AddString( "data2", msg->mMessageData2 );
			cb.AddString( "data3", msg->mMessageData3 );
			break;
		}
		CASE_MSG( AiState::EvProximityTrigger )
		{
			cb.CallScript();
			cb.AddString( "owner", Utils::HashToString( msg->mOwnerState ).c_str() );
			cb.AddEntity( "ent", msg->mEntity );
			cb.AddVector( "position", msg->mPosition );
			break;
		}
		CASE_MSG( EvAddWeapon )
		{
			if ( !GetWeaponSystem()->HasWeapon( msg->mWeaponId ) )
			{
				// Add weapons according to the event.
				if ( GetWeaponSystem()->AddWeaponToInventory( msg->mWeaponId ) )
				{
					cb.CallScript();
					cb.AddInt( "weapon", msg->mWeaponId );
				}
				else
				{
					int NumElements = 0;
					const IntEnum *Enum = 0;
					System::mInstance->mGame->GetWeaponEnumeration( Enum, NumElements );

					bool foundName = false;
					for ( int i = 0; i < NumElements; ++i )
					{
						if ( Enum[ i ].mValue == msg->mWeaponId )
						{
							foundName = true;
							EngineFuncs::ConsoleError( va( "AddWeaponToInventory: Unknown Weapon( %s )", Enum[ i ].mKey ).c_str() );
							break;
						}
					}

					if ( !foundName )
					{
						EngineFuncs::ConsoleError( va( "AddWeaponToInventory: Unknown Weapon Id( %d )", msg->mWeaponId ).c_str() );
					}
				}
			}
			break;
		}
		CASE_MSG( EvRemoveWeapon )
		{
			if ( GetWeaponSystem()->HasWeapon( msg->mWeaponId ) )
			{
				cb.CallScript();
				cb.AddInt( "weapon", msg->mWeaponId );
				GetWeaponSystem()->RemoveWeapon( msg->mWeaponId );
			}
			break;
		}
		CASE_MSG( EvRefreshAllWeapons )
		{
			GetWeaponSystem()->RefreshAllWeapons();
			break;
		}
		CASE_MSG( EvRefreshWeapon )
		{
			GetWeaponSystem()->RefreshWeapon( msg->mWeaponId );

			cb.CallScript();
			cb.AddInt( "weapon", msg->mWeaponId );
			break;
		}
		CASE_MSG( EvEntEnterRadius )
		{
			//_cb.CallScript();
			cb.AddEntity( "ent", msg->mEntity );
			break;
		}
		CASE_MSG( EvEntLeaveRadius )
		{
			//_cb.CallScript();
			cb.AddEntity( "ent", msg->mEntity );
			break;
		}
		CASE_MSG( EvWeaponChanged )
		{
			if ( GetWeaponSystem()->HasWeapon( msg->mWeaponId ) )
			{
				cb.CallScript();
				cb.AddInt( "weapon", msg->mWeaponId );

				// Special case signal.
				GetStateRoot()->SignalThreads( gmVariable( Utils::MakeId32( (int16_t)MSG_WEAPON_CHANGE, (int16_t)msg->mWeaponId ) ) );
			}
			break;
		}
		CASE_MSG( EvWeaponFire )
		{
			cb.CallScript();
			cb.AddInt( "weapon", msg->mWeaponId );
			cb.AddEntity( "projectile", msg->mProjectile );

			// Shot fired callback.
			WeaponPtr curWpn = GetWeaponSystem()->GetCurrentWeapon();
			if ( curWpn && curWpn->GetWeaponID() == msg->mWeaponId )
				curWpn->ShotFired( msg->mFireMode, msg->mProjectile );

			// Special case signal.
			GetStateRoot()->SignalThreads( gmVariable( Utils::MakeId32( (int16_t)MSG_WEAPON_FIRE, (int16_t)msg->mWeaponId ) ) );
			break;
		}
		CASE_MSG( EvPlayerUsed )
		{
			cb.CallScript();
			cb.AddEntity( "toucher", msg->mWhoDidIt );
			break;
		}
		CASE_MSG( EvTakeDamage )
		{
			cb.CallScript();
			cb.AddEntity( "inflictor", msg->mInflictor );
			cb.AddInt( "previoushealth", GetCurrentHealth() );
			gEngineFuncs->GetEntityInfo( GetGameEntity(), mEntInfo );
			cb.AddInt( "currenthealth", GetCurrentHealth() );
			cb.AddString( "damageType", msg->mDamageType );

			if ( msg->mInflictor.IsValid() && IGame::IsEntityValid( msg->mInflictor ) )
			{
				GetSensoryMemory()->UpdateWithTouchSource( msg->mInflictor );
			}
			break;
		}
		CASE_MSG( EvSound )
		{
			if ( msg->mSource != GetGameEntity() )
			{
				cb.CallScript();
				cb.AddEntity( "source", msg->mSource );
				cb.AddVector( "origin", msg->mOrigin[ 0 ], msg->mOrigin[ 1 ], msg->mOrigin[ 2 ] );
				cb.AddInt( "soundId", msg->mSoundType );
				cb.AddString( "soundName", msg->mSoundName );
				GetSensoryMemory()->UpdateWithSoundSource( msg );
			}
			break;
		}
		CASE_MSG( EvEntitySensed )
		{
			cb.CallScript();
			cb.AddInt( "sensedclass", msg->mEntityClass );
			cb.AddEntity( "sensedentity", msg->mEntity );
			break;
		}
		CASE_MSG( EvMineArmed )
		{
			cb.CallScript();
			cb.AddEntity( "mine", msg->mMine );
			break;
		}
		CASE_MSG( EvMineDetonate )
		{
			cb.CallScript();
			cb.AddEntity( "mine", msg->mMine );
			break;
		}
		CASE_MSG( EvMortarImpact )
		{
			cb.CallScript();
			cb.AddEntity( "owner", msg->mOwner );
			cb.AddVector( "position", msg->mPosition );
			cb.AddFloat( "radius", msg->mRadius );
			break;
		}
		CASE_MSG( EvRecievedAmmo )
		{
			cb.CallScript();
			cb.AddEntity( "who", msg->mFromWho );
			break;
		}
		CASE_MSG( EvEntityDrowning )
		{
			cb.CallScript();
			break;
		}
		CASE_MSG( EvPickedUpItem )
		{
			cb.CallScript();
			cb.AddInt( "Group", msg->mInfo.mGroup );
			cb.AddInt( "ClassId", msg->mInfo.mClassId );
			cb.AddInt( "Energy", msg->mInfo.mEnergy.mNum );
			cb.AddInt( "EnergyMax", msg->mInfo.mEnergy.mMax );
			cb.AddInt( "Health", msg->mInfo.mHealth.mNum );
			cb.AddInt( "MaxHealth", msg->mInfo.mHealth.mMax );
			cb.AddInt( "Armor", msg->mInfo.mArmor.mNum );
			cb.AddInt( "MaxArmor", msg->mInfo.mArmor.mMax );
			break;
		}
	}

	if ( message.Id() == EvChatMessageGlobal::Msg::MessageId ||
		message.Id() == EvChatMessageTeam::Msg::MessageId ||
		message.Id() == EvChatMessagePrivate::Msg::MessageId ||
		message.Id() == EvChatMessageGroup::Msg::MessageId )
	{
		const EvVoiceMacro* msg = EvVoiceMacro::Msg::Cast( message, message.Id() );
		if ( msg->mWhoSaidIt != GetGameEntity() ) // Ignore messages from myself.
		{
			cb.CallScript();
			cb.AddEntity( "who", msg->mWhoSaidIt );
			cb.AddString( "msg", msg->mMessage );
		}
		else
			cb.DontPropogateEvent();
	}

	if ( message.Id() == EvVoiceMacroGlobal::Msg::MessageId ||
		message.Id() == EvVoiceMacroTeam::Msg::MessageId ||
		message.Id() == EvVoiceMacroPrivate::Msg::MessageId ||
		message.Id() == EvVoiceMacroGroup::Msg::MessageId )
	{
		const EvVoiceMacro* msg = EvVoiceMacro::Msg::Cast( message, message.Id() );
		if ( msg->mWhoSaidIt != GetGameEntity() ) // Ignore messages from myself.
		{
			const int macroId = HandleVoiceMacroEvent( msg );
			cb.CallScript();
			cb.AddEntity( "who", msg->mWhoSaidIt );
			cb.AddInt( "macro", macroId );

			gmVariable s( Utils::MakeId32( (int16_t)message.Id(), (int16_t)macroId ) );
			GetStateRoot()->SignalThreads( s );
		}
		else
			cb.DontPropogateEvent();
	}
}

void Client::Init( int _gameid )
{
	// Set the game id, which is the index into the mClients array.
	mGameID = _gameid;
	mGameEntity = gEngineFuncs->EntityFromID( mGameID );

	//.mSoundSubscriber = g_SoundDepot.Subscribe("Client");

	// mark name as taken
	mNameReference = NameManager::GetInstance()->GetName( GetName() );

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

int Client::GetStuckTime() const
{
	return mStuckTime;
}
void Client::ResetStuckTime()
{
	mStuckTime = 0;
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

bool Client::HasLineOfSightTo( const Vector3f &_pos, GameEntity entity, int customTraceMask )
{
	return HasLineOfSightTo( GetEyePosition(), _pos, entity, mGameID, customTraceMask );
}

bool Client::HasLineOfSightTo( const Vector3f &_pos1, const Vector3f &_pos2,
	GameEntity ent, int _ignoreent, int customTraceMask )
{
	//rmt_ScopedCPUSample( HasLineOfSightTo );
	obTraceResult tr;
	EngineFuncs::TraceLine( tr, _pos1, _pos2,
		NULL, customTraceMask ? customTraceMask : TR_MASK_SHOT | TR_MASK_SMOKEBOMB, _ignoreent, true );
	return ( tr.mFraction == 1.0f ) || ( ( ent.IsValid() ) && ( tr.mHitEntity == ent ) );
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
			mDebugLog.OpenForWrite( va( "user/log_%s.rtf", GetName() ).c_str(), File::Text );

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
		GetName().c_str(),
		IsDebugEnabled( _flag ) ? "enabled" : "disabled" ).c_str() );
}

void Client::OutputDebug( MessageType _type, const char * _str )
{
#ifndef _DEBUG
	if(_type == kDebug)
		return;
#endif

	EngineFuncs::ConsoleMessage( va( "%s: %s", GetName( true ), _str ).c_str() );

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
			const std::string& name = GetName();
			strProfileName = NameManager::GetInstance()->GetProfileForName( name );
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
				DBG_MSG( BOT_DEBUG_SCRIPT, this, kNormal, va( "Profile Loaded: %s", strProfileName.c_str() ).c_str() );
			}
			else
			{
				DBG_MSG( BOT_DEBUG_SCRIPT, this, kError, va( "Unable to load profile: %s", strProfileName.c_str() ).c_str() );
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

void Client::ProcessEventImpl( const Message &_message, uint32_t _targetState )
{
	//rmt_ScopedCPUSample( ClientProcessEvent );

	gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();

	DisableGCInScope gcEn( pMachine );

	CallbackParameters cb( _message.Id(), pMachine );
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
		EvChangeTeam::Msg event;
		event.mData.mNewTeam = iCurrentTeam;
		SendEvent( event );
	}
}

void Client::CheckClassEvent()
{
	EntityInfo currentInfo;
	IGame::GetEntityInfo( GetGameEntity(), currentInfo );
	if ( currentInfo.mClassId != mEntInfo.mClassId )
	{
		// Send a change class event.
		EvChangeClass::Msg event;
		event.mData.mNewClass = currentInfo.mClassId;
		SendEvent( event );
	}
}

bool Client::IsAllied( const GameEntity ent ) const
{
	return gEngineFuncs->IsAllied( GetGameEntity(), ent );
}

void Client::PressButton( int button )
{
	mButtonFlags.SetFlag( button );
}

void Client::ReleaseButton( int button )
{
	mButtonFlags.ClearFlag( button );
}

bool Client::IsButtonDown( int button ) const
{
	return mButtonFlags.CheckFlag( button );
}

void Client::HoldButton( const BitFlag64 &buttons, uint32_t timems )
{
	for ( int i = 0; i < HoldButtons::NumButtons; ++i )
	{
		if ( buttons.CheckFlag( i ) )
		{
			mHoldButtons.mStopHoldTime[ i ] =
				( timems > 0 ) ? IGame::GetTime() + timems : std::numeric_limits<int>::max();
		}
	}
}

void Client::ReleaseHeldButton( const BitFlag64 &buttons )
{
	for ( int i = 0; i < HoldButtons::NumButtons; ++i )
	{
		if ( buttons.CheckFlag( i ) )
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

void Client::ProcessStimulusBehavior( Behaviors& behaviors, const StimulusPtr& stim )
{
	switch ( GetTeam() )
	{
		case OB_TEAM_1:
			if ( !stim->mEntInfo.mFlags.CheckFlag( ENT_FLAG_TEAM1 ) )
				return;
			break;
		case OB_TEAM_2:
			if ( !stim->mEntInfo.mFlags.CheckFlag( ENT_FLAG_TEAM2 ) )
				return;
			break;
		case OB_TEAM_3:
			if ( !stim->mEntInfo.mFlags.CheckFlag( ENT_FLAG_TEAM3 ) )
				return;
			break;
		case OB_TEAM_4:
			if ( !stim->mEntInfo.mFlags.CheckFlag( ENT_FLAG_TEAM4 ) )
				return;
			break;
	}
	
	StimulusBehavior beh;
	beh.mUser = GetGameEntity();
	beh.mStimulus = stim;

	const EntityInfo& stimInfo = stim->mEntInfo;
	switch ( stimInfo.mGroup )
	{
		case ENT_GRP_PLAYER:
		case ENT_GRP_MONSTER:
		{
			if ( gEngineFuncs->IsAllied( GetGameEntity(), stim->mEntity ) )
			{
				// escort?
			}
			else
			{
				beh.mAction = BEHAVIOR_KILL;
				beh.mDesirability = 0.11f;
			}
			break;
		}
		case ENT_GRP_BUTTON:
		{
			beh.mAction = BEHAVIOR_PRESSBUTTON;
			beh.mDesirability = 0.1f;
			break;
		}
		case ENT_GRP_RESUPPLY:
		{
			// there are a lot of different elements to a potential resupply, 
			// so take into account all of them to generate our combined desirability
			beh.mAction = BEHAVIOR_RESUPPLY;

			float desirabilityBest = 0.0f;

			// bonus for multiple different types of needed resupply, 
			// so sources that serve multiple will be preferred
			float desirabilityBonus = 0.0f; 

			if ( stimInfo.mHealth.mNum > 0 )
			{				
				if ( GetCurrentHealth() < GetMaxHealth() )
				{
					desirabilityBest = std::max( desirabilityBest, ( 1.0f - ( (float)GetCurrentHealth() / (float)GetMaxHealth() ) ) * 0.5f );
					desirabilityBonus += 0.05f;
				}
			}

			// we need any ammo?
			for ( int i = 0; i < EntityInfo::NUM_AMMO_TYPES; ++i )
			{
				if ( stimInfo.mAmmo[ i ].mNum > 0 )
				{
					int ammoCurrent = 0, ammoMax = 0;
					gEngineFuncs->GetCurrentAmmo( GetGameEntity(), stimInfo.mAmmo[ i ].mWeaponId, Primary, ammoCurrent, ammoMax );
					if ( ammoCurrent < ammoMax )
					{
						desirabilityBest = std::max( desirabilityBest, ( 1.0f - ( (float)ammoCurrent / (float)ammoMax ) ) * 0.3f );
						desirabilityBonus += 0.05f;
					}
				}
			}
			
			if ( stimInfo.mArmor.mNum > 0 )
			{
				if ( GetCurrentArmor() < GetMaxArmor() )
				{
					desirabilityBest = std::max( desirabilityBest, 1.0f - ( (float)GetCurrentArmor() / (float)GetMaxArmor() ) * 0.5f );
					desirabilityBonus += 0.05f;
				}
			}

			/*if ( stimInfo.mEnergy.mNum > 0 )
			{
			if ( GetCurrentArmor() < GetMaxArmor() )
			{
			beh.mDesirability = 1.0f - ( (float)GetCurrentArmor() / (float)GetMaxArmor() ) * 0.5f;
			desirabilityBonus += 0.05;
			}
			}*/

			if ( desirabilityBest > 0.0 )
			{
				beh.mDesirability = desirabilityBest + desirabilityBonus;
				behaviors.push_back( beh );
			}
			break;
		}
		case ENT_GRP_FLAG:
		{
			FlagState flagState;
			GameEntity owner;
			if ( gEngineFuncs->GetFlagState( beh.mStimulus->mEntity, flagState, owner ) )
			{
				if ( owner == GetGameEntity() )
				{
					beh.mAction = BEHAVIOR_CAP_FLAG;
					beh.mDesirability = 1.5f;
				}
				else
				{
					if ( gEngineFuncs->IsAllied( GetGameEntity(), owner ) )
					{
						beh.mAction = BEHAVIOR_DEFEND;
						beh.mDesirability = 1.5f;
					}
				}
			}

			beh.mAction = BEHAVIOR_GET_FLAG;
			beh.mDesirability = 0.5f;
			behaviors.push_back( beh );
			break;
		}
		/*case ENT_GRP_FLAGCAPPOINT:
		{
		break;
		}*/
		case ENT_GRP_CONTROLPOINT:
		{
			beh.mAction = BEHAVIOR_CAP_CONTROL_POINT;
			beh.mDesirability = 0.5f;
			behaviors.push_back( beh );
			break;
		}
		case ENT_GRP_WEAPON:
		{
			if ( GetWeaponSystem()->HasWeapon( stimInfo.mClassId ) )
			{
				int ammoCurrent = 0, ammoMax = 0;
				gEngineFuncs->GetCurrentAmmo( GetGameEntity(), stimInfo.mClassId, Primary, ammoCurrent, ammoMax );
				if ( ammoCurrent < ammoMax )
				{
					beh.mAction = BEHAVIOR_GET_WEAPON;
					beh.mDesirability = 0.1f;
					behaviors.push_back( beh );
				}
			}
			else
			{
				beh.mAction = BEHAVIOR_GET_WEAPON;
				beh.mDesirability = 0.2f; // todo: use per weapon desirability
				behaviors.push_back( beh );
			}			
			break;
		}
		case ENT_GRP_POWERUP:
		{
			if ( CanGetPowerUp( stimInfo.mClassId ) )
			{
				beh.mAction = BEHAVIOR_GET_POWERUP;
				beh.mDesirability = 0.5f;
				behaviors.push_back( beh );
			}
			break;
		}
		/*case ENT_GRP_DISPENSER:
		goalDef.Props.SetString( "Type", "Dispenser" );
		break;
		case ENT_GRP_BUILDABLE:
		goalDef.Props.SetString( "Type", "Buildable" );
		break;
		case ENT_GRP_MOUNTABLE:
		goalDef.Props.SetString( "Type", "Mountable" );
		break;*/
		default:
			return;
	}
}
