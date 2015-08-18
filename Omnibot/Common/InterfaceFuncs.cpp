////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "InterfaceFuncs.h"
#include "Client.h"

obResult InterfaceMsg( const MessageHelper &_data, const GameEntity _ent )
{
	obResult res = gEngineFuncs->InterfaceSendMessage( _data, _ent );
	return res;
}

namespace InterfaceFuncs
{
	int Addbot( Msg_Addbot &_addbot )
	{
		MessageHelper msg( GEN_MSG_ADDBOT, &_addbot, sizeof( _addbot ) );
		return gEngineFuncs->AddBot( msg );
	}

	void Kickbot( Msg_Kickbot &_kickbot )
	{
		MessageHelper msg( GEN_MSG_KICKBOT, &_kickbot, sizeof( _kickbot ) );
		gEngineFuncs->RemoveBot( msg );
	}

	bool IsAlive( const GameEntity _ent )
	{
		Msg_IsAlive data;
		MessageHelper msg( GEN_MSG_ISALIVE, &data, sizeof( data ) );
		InterfaceMsg( msg, _ent );
		return ( data.mIsAlive == True );
	}

	bool IsAllied( const GameEntity _ent1, const GameEntity _ent2 )
	{
		// Ally check.
		Msg_IsAllied data( _ent2 );
		MessageHelper msg( GEN_MSG_ISALLIED, &data, sizeof( data ) );
		InterfaceMsg( msg, _ent1 );
		return ( data.mIsAllied == True );
	}

	bool IsOutSide( const Vector3f &_pos )
	{
		Msg_IsOutside data;
		data.mPosition[ 0 ] = _pos[ 0 ];
		data.mPosition[ 1 ] = _pos[ 1 ];
		data.mPosition[ 2 ] = _pos[ 2 ];
		data.mIsOutside = False;
		MessageHelper msg( GEN_MSG_ISOUTSIDE, &data, sizeof( data ) );
		InterfaceMsg( msg, GameEntity() );
		return data.mIsOutside == True;
	}

	bool GetMaxSpeed( const GameEntity _ent, Msg_PlayerMaxSpeed &_out )
	{
		MessageHelper msg( GEN_MSG_GETMAXSPEED, &_out, sizeof( Msg_PlayerMaxSpeed ) );
		return SUCCESS( InterfaceMsg( msg, _ent ) );
	}

	int GetEntityTeam( const GameEntity _ent )
	{
		return gEngineFuncs->GetEntityTeam( _ent );
	}

	WeaponStatus GetEquippedWeapon( const GameEntity _ent )
	{
		WeaponStatus data;
		MessageHelper msg( GEN_MSG_GETEQUIPPEDWEAPON, &data, sizeof( data ) );
		InterfaceMsg( msg, _ent );
		return data;
	}

	WeaponStatus GetMountedWeapon( Client *_bot )
	{
		WeaponStatus data;
		MessageHelper msg( GEN_MSG_GETMOUNTEDWEAPON, &data, sizeof( data ) );
		InterfaceMsg( msg, _bot->GetGameEntity() );
		return data;
	}

	bool GetWeaponLimits( Client *_bot, int _weapon, WeaponLimits &_limits )
	{
		_limits.mWeaponId = _weapon;
		MessageHelper msg( GEN_MSG_GETWEAPONLIMITS, &_limits, sizeof( _limits ) );
		InterfaceMsg( msg, _bot->GetGameEntity() );
		return true;
	}

	bool IsReadyToFire( const GameEntity _ent )
	{
		Msg_ReadyToFire data;
		MessageHelper msg( GEN_MSG_ISREADYTOFIRE, &data, sizeof( data ) );
		InterfaceMsg( msg, _ent );
		return data.mReady == True;
	}

	bool IsReloading( const GameEntity _ent )
	{
		Msg_Reloading data;
		MessageHelper msg( GEN_MSG_ISRELOADING, &data, sizeof( data ) );
		InterfaceMsg( msg, _ent );
		return data.mReloading == True;
	}

	bool GetFlagState( const GameEntity _ent, FlagState &_outFlagState, GameEntity &_outEntity )
	{
		Msg_FlagState data;
		data.mOwner = GameEntity();
		data.mFlagState = S_FLAG_NOT_A_FLAG;
		MessageHelper msg( GEN_MSG_GETFLAGSTATE, &data, sizeof( data ) );
		bool bOk = SUCCESS( InterfaceMsg( msg, _ent ) );
		if ( bOk )
		{
			_outFlagState = data.mFlagState;
			_outEntity = data.mOwner;
		}
		return bOk;
	}

	int GetControllingTeam( const GameEntity _ent )
	{
		ControllingTeam data;
		MessageHelper msg( GEN_MSG_GETCONTROLLINGTEAM, &data, sizeof( data ) );
		if ( SUCCESS( InterfaceMsg( msg, _ent ) ) )
			return data.mControllingTeam;
		return 0;
	}

	GameState GetGameState()
	{
		Msg_GameState data;
		MessageHelper msg( GEN_MSG_GAMESTATE, &data, sizeof( data ) );
		InterfaceMsg( msg );
		return data.mGameState;
	}

	float GetGameTimeLeft()
	{
		Msg_GameState data;
		MessageHelper msg( GEN_MSG_GAMESTATE, &data, sizeof( data ) );
		InterfaceMsg( msg );
		return data.mTimeLeft;
	}

	const char *GetGameState( GameState _state )
	{
		switch ( _state )
		{
			case GAME_STATE_PLAYING:
				return "Playing";
			case GAME_STATE_WARMUP:
				return "Warm-up";
			case GAME_STATE_WARMUP_COUNTDOWN:
				return "Warm-up Countdown";
			case GAME_STATE_INTERMISSION:
				return "Intermission";
			case GAME_STATE_WAITINGFORPLAYERS:
				return "Waiting for Players";
			case GAME_STATE_PAUSED:
				return "Paused";
			default:
				return "Invalid";
		}
	}

	obUserData GetEntityStat( const GameEntity _ent, const char *_statname )
	{
		Msg_EntityStat data;
		Utils::StringCopy( data.mStatName, _statname ? _statname : "", sizeof( data.mStatName ) );

		MessageHelper msg( GEN_MSG_ENTITYSTAT, &data, sizeof( data ) );
		InterfaceMsg( msg, _ent );
		return data.mResult;
	}

	obUserData GetTeamStat( int _team, const char *_statname )
	{
		Msg_TeamStat data;
		data.mTeam = _team;
		Utils::StringCopy( data.mStatName, _statname ? _statname : "", sizeof( data.mStatName ) );

		MessageHelper msg( GEN_MSG_TEAMSTAT, &data, sizeof( data ) );
		InterfaceMsg( msg );
		return data.mResult;
	}

	bool IsWeaponCharged( Client *_bot, int _weapon, FireMode _mode )
	{
		WeaponCharged data( _weapon, _mode );
		MessageHelper msg( GEN_MSG_WPCHARGED, &data, sizeof( data ) );
		InterfaceMsg( msg, _bot->GetGameEntity() );
		return data.mIsCharged == True;
	}

	bool IsEntWeaponCharged( GameEntity _ent, int _weapon )
	{
		WeaponCharged data( _weapon );
		MessageHelper msg( GEN_MSG_WPCHARGED, &data, sizeof( data ) );
		InterfaceMsg( msg, _ent );
		return data.mIsCharged == True;
	}

	float WeaponHeat( Client *_bot, FireMode _mode, float &_current, float &_max )
	{
		WeaponHeatLevel data( _mode );
		MessageHelper msg( GEN_MSG_WPHEATLEVEL, &data, sizeof( data ) );
		InterfaceMsg( msg, _bot->GetGameEntity() );
		_current = data.mCurrentHeat;
		_max = data.mMaxHeat;
		return data.mMaxHeat != 0.f ? (float)data.mCurrentHeat / (float)data.mMaxHeat : 0.f;
	}

	void ChangeName( Client *_bot, const char *_newname )
	{
		if ( _newname )
		{
			Msg_ChangeName data = {};
			Utils::StringCopy( data.mNewName, _newname, sizeof( data.mNewName ) / sizeof( data.mNewName[ 0 ] ) );
			MessageHelper msg( GEN_MSG_CHANGENAME, &data, sizeof( data ) );
			InterfaceMsg( msg, _bot->GetGameEntity() );
		}
	}

	bool EntityKill( GameEntity _ent )
	{
		Msg_KillEntity data = { _ent };
		MessageHelper msg( GEN_MSG_ENTITYKILL, &data, sizeof( data ) );
		return InterfaceMsg( msg ) == Success;
	}

	bool ServerCommand( const char *_cmd )
	{
		Msg_ServerCommand data;
		Utils::StringCopy( data.mCommand, _cmd, sizeof( data.mCommand ) );
		MessageHelper msg( GEN_MSG_SERVERCOMMAND, &data, sizeof( data ) );
		return InterfaceMsg( msg ) == Success;
	}

	bool PlaySound( Client *_bot, const char *_sound )
	{
		Event_PlaySound data = {};
		Utils::StringCopy( data.mSoundName, _sound, sizeof( data.mSoundName ) );
		MessageHelper msg( GEN_MSG_PLAYSOUND, &data, sizeof( data ) );
		return InterfaceMsg( msg, _bot->GetGameEntity() ) == Success;
	}

	bool StopSound( Client *_bot, const char *_sound )
	{
		Event_StopSound data = {};
		Utils::StringCopy( data.mSoundName, _sound, sizeof( data.mSoundName ) );
		MessageHelper msg( GEN_MSG_STOPSOUND, &data, sizeof( data ) );
		return InterfaceMsg( msg, _bot->GetGameEntity() ) == Success;
	}

	bool ScriptEvent( const char *_func, const char *_entname, const char *_p1, const char *_p2, const char *_p3 )
	{
		if ( !_func )
			return false;

		Event_ScriptEvent data = {};
		Utils::StringCopy( data.mFunctionName, _func, sizeof( data.mFunctionName ) );
		Utils::StringCopy( data.mEntityName, _entname, sizeof( data.mEntityName ) );
		Utils::StringCopy( data.mParam1, _p1 ? _p1 : "", sizeof( data.mParam1 ) );
		Utils::StringCopy( data.mParam2, _p2 ? _p2 : "", sizeof( data.mParam2 ) );
		Utils::StringCopy( data.mParam3, _p3 ? _p3 : "", sizeof( data.mParam3 ) );
		MessageHelper msg( GEN_MSG_SCRIPTEVENT, &data, sizeof( data ) );
		return InterfaceMsg( msg ) == Success;
	}

	bool GotoWaypoint( const char *_param, const Vector3f &_pos )
	{
		Msg_GotoWaypoint data;
		Utils::StringCopy( data.mWaypointName, _param, sizeof( data.mWaypointName ) );
		data.mOrigin[ 0 ] = _pos[ 0 ];
		data.mOrigin[ 1 ] = _pos[ 1 ];
		data.mOrigin[ 2 ] = _pos[ 2 ];
		MessageHelper msg( GEN_MSG_GOTOWAYPOINT, &data, sizeof( data ) );
		return InterfaceMsg( msg, Utils::GetLocalEntity() ) == Success;
	}

	bool GetVehicleInfo( Client *_bot, VehicleInfo &_vi )
	{
		MessageHelper msg( GEN_MSG_VEHICLEINFO, &_vi, sizeof( _vi ) );
		return InterfaceMsg( msg ) == Success;
	}

	bool IsMoverAt( const Vector3f &_pos1, const Vector3f &_pos2 )
	{
		Msg_MoverAt data;
		data.mPosition[ 0 ] = _pos1.X();
		data.mPosition[ 1 ] = _pos1.Y();
		data.mPosition[ 2 ] = _pos1.Z();

		data.mUnder[ 0 ] = _pos2.X();
		data.mUnder[ 1 ] = _pos2.Y();
		data.mUnder[ 2 ] = _pos2.Z();

		MessageHelper msg( GEN_MSG_MOVERAT, &data, sizeof( data ) );
		return SUCCESS( InterfaceMsg( msg ) ) && data.mEntity.IsValid();
	}

	GameEntity GetMoverAt( const Vector3f &_pos )
	{
		Msg_MoverAt data;
		data.mPosition[ 0 ] = _pos.X();
		data.mPosition[ 1 ] = _pos.Y();
		data.mPosition[ 2 ] = _pos.Z();

		data.mUnder[ 0 ] = _pos.X();
		data.mUnder[ 1 ] = _pos.Y();
		data.mUnder[ 2 ] = _pos.Z() - 1024.0f;

		MessageHelper msg( GEN_MSG_MOVERAT, &data, sizeof( data ) );
		return SUCCESS( InterfaceMsg( msg ) ) ? data.mEntity : GameEntity();
	}
	int GetGameType()
	{
		Event_GameType data = { 0 };
		MessageHelper msg( GEN_MSG_GETGAMETYPE, &data, sizeof( data ) );
		InterfaceMsg( msg );
		return data.mGameType;
	}

	bool SetCvar( const char *_cvar, const char *_value )
	{
		if ( _cvar && _value )
		{
			Event_CvarSet data;
			data.mCvar = _cvar;
			data.mValue = _value;
			MessageHelper msg( GEN_MSG_SETCVAR, &data, sizeof( data ) );
			InterfaceMsg( msg );
		}
		return true;
	}

	int GetCvar( const char *_cvar )
	{
		if ( _cvar )
		{
			Event_CvarGet data;
			data.mCvar = _cvar;
			MessageHelper msg( GEN_MSG_GETCVAR, &data, sizeof( data ) );
			InterfaceMsg( msg );
			return data.mValue;
		}
		return 0;
	}
};