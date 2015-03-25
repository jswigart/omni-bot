////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "ET_InterfaceFuncs.h"
#include "ET_Client.h"

namespace InterfaceFuncs
{
	bool IsWeaponOverheated( Client *_bot, ET_Weapon _weapon )
	{
		ET_WeaponOverheated data = { _weapon, False };
		MessageHelper msg( ET_MSG_WPOVERHEATED, &data, sizeof( data ) );
		InterfaceMsg( msg, _bot->GetGameEntity() );
		return data.mIsOverheated == True;
	}

	void GetMountedGunHeat( Client *_bot, const GameEntity _gun, int &_cur, int &_max )
	{
		ET_WeaponHeatLevel data = { _gun, 0, 0 };
		MessageHelper msg( ET_MSG_GETGUNHEAT, &data, sizeof( data ) );
		InterfaceMsg( msg, _bot->GetGameEntity() );
		_cur = data.mCurrent;
		_max = data.mMax;
	}

	ExplosiveState GetExplosiveState( Client *_bot, const GameEntity _dynamite )
	{
		ET_ExplosiveState data = { _dynamite, XPLO_INVALID };
		MessageHelper msg( ET_MSG_GEXPLOSIVESTATE, &data, sizeof( data ) );
		InterfaceMsg( msg, _bot ? _bot->GetGameEntity() : GameEntity() );
		return data.mState;
	}

	ConstructableState GetConstructableState( Client *_bot, const GameEntity _constructable )
	{
		ET_ConstructionState data = { _constructable, CONST_INVALID };
		MessageHelper msg( ET_MSG_GCONSTRUCTABLE, &data, sizeof( data ) );
		InterfaceMsg( msg, _bot->GetGameEntity() );
		return data.mState;
	}

	ConstructableState IsDestroyable( Client *_bot, const GameEntity _ent )
	{
		ET_Destroyable data = { _ent, CONST_INVALID };
		MessageHelper msg( ET_MSG_GDYNDESTROYABLE, &data, sizeof( data ) );
		InterfaceMsg( msg, _bot ? _bot->GetGameEntity() : GameEntity() );
		return data.mState;
	}

	bool HasFlag( Client *_bot )
	{
		ET_HasFlag data = { False };
		MessageHelper msg( ET_MSG_GHASFLAG, &data, sizeof( data ) );
		InterfaceMsg( msg, _bot->GetGameEntity() );
		return data.mHasFlag == True;
	}

	bool ItemCanBeGrabbed( Client *_bot, const GameEntity _ent )
	{
		ET_CanBeGrabbed data = { _ent, False };
		MessageHelper msg( ET_MSG_GCANBEGRABBED, &data, sizeof( data ) );
		InterfaceMsg( msg, _bot->GetGameEntity() );
		return data.mCanBeGrabbed == True;
	}

	void NumTeamMines( Client *_bot, int &_current, int &_max )
	{
		ET_TeamMines data = { 0, 0 };
		MessageHelper msg( ET_MSG_GNUMTEAMMINES, &data, sizeof( data ) );
		InterfaceMsg( msg, _bot->GetGameEntity() );
		_current = data.mCurrent;
		_max = data.mMax;
	}

	obBool IsWaitingForMedic( const GameEntity _ent )
	{
		ET_WaitingForMedic data = { False };
		MessageHelper msg( ET_MSG_ISWAITINGFORMEDIC, &data, sizeof( data ) );
		InterfaceMsg( msg, _ent );
		return data.mWaitingForMedic == True ? True : False;
	}

	bool SelectPrimaryWeapon( Client *_bot, ET_Weapon _weapon )
	{
		ET_SelectWeapon data = { _weapon };
		MessageHelper msg( ET_MSG_PICKWEAPON, &data, sizeof( data ) );
		InterfaceMsg( msg, _bot->GetGameEntity() );
		return data.mGood == True;
	}

	bool SelectSecondaryWeapon( Client *_bot, ET_Weapon _weapon )
	{
		ET_SelectWeapon data = { _weapon };
		MessageHelper msg( ET_MSG_PICKWEAPON2, &data, sizeof( data ) );
		InterfaceMsg( msg, _bot->GetGameEntity() );
		return data.mGood == True;
	}

	float GetReinforceTime( Client *_bot )
	{
		ET_ReinforceTime data = { 0 };
		MessageHelper msg( ET_MSG_REINFORCETIME, &data, sizeof( data ) );
		InterfaceMsg( msg, _bot->GetGameEntity() );
		return (float)data.mReinforceTime / 1000.0f;
	}

	bool IsMedicNear( Client *_bot )
	{
		ET_MedicNear data = { False };
		MessageHelper msg( ET_MSG_ISMEDICNEAR, &data, sizeof( data ) );
		InterfaceMsg( msg, _bot->GetGameEntity() );
		return data.mMedicNear == True;
	}

	bool GoToLimbo( Client *_bot )
	{
		ET_GoLimbo data = { False };
		MessageHelper msg( ET_MSG_GOTOLIMBO, &data, sizeof( data ) );
		InterfaceMsg( msg, _bot->GetGameEntity() );
		return data.mGoLimbo == True;
	}

	GameEntity GetMountedPlayerOnMG42( Client *_bot, GameEntity _gun )
	{
		ET_MG42MountedPlayer data = { _gun, GameEntity() };
		MessageHelper msg( ET_MSG_ISGUNMOUNTED, &data, sizeof( data ) );
		InterfaceMsg( msg, _bot->GetGameEntity() );
		return data.mMountedEntity;
	}

	bool IsMountableGunRepairable( Client *_bot, GameEntity _gun )
	{
		ET_MG42MountedRepairable data = { _gun, False };
		MessageHelper msg( ET_MSG_ISGUNREPAIRABLE, &data, sizeof( data ) );
		InterfaceMsg( msg, _bot->GetGameEntity() );
		return data.mRepairable == True;
	}

	int GetGunHealth( Client *_bot, const GameEntity _gun )
	{
		ET_MG42Health data = { _gun, 0 };
		MessageHelper msg( ET_MSG_GETGUNHEALTH, &data, sizeof( data ) );
		InterfaceMsg( msg, _bot->GetGameEntity() );
		return data.mHealth;
	}

	void GetCurrentCursorHint( Client *_bot, int &_type, int &_val )
	{
		ET_CursorHint data = { 0, 0 };
		MessageHelper msg( ET_MSG_GETHINT, &data, sizeof( data ) );
		InterfaceMsg( msg, _bot->GetGameEntity() );

		_type = data.mType;
		_val = data.mValue;
	}

	void ChangeSpawnPoint( Client *_bot, int _spawnpoint )
	{
		ET_SpawnPoint data = { _spawnpoint };
		MessageHelper msg( ET_MSG_CHANGESPAWNPOINT, &data, sizeof( data ) );
		InterfaceMsg( msg, _bot->GetGameEntity() );
	}

	bool GetMg42Properties( Client *_bot, ET_MG42Info &_data )
	{
		memset( &_data, 0, sizeof( _data ) );
		MessageHelper msg( ET_MSG_MOUNTEDMG42INFO, &_data, sizeof( ET_MG42Info ) );
		return SUCCESS( InterfaceMsg( msg, _bot->GetGameEntity() ) );
	}

	bool GetCabinetData( GameEntity _ent, ET_CabinetData &data )
	{
		MessageHelper msg( ET_MSG_CABINETDATA, &data, sizeof( ET_CabinetData ) );
		return SUCCESS( InterfaceMsg( msg, _ent ) );
	}

	bool FireTeamCreate( Client *_bot )
	{
		MessageHelper msg( ET_MSG_FIRETEAM_CREATE );
		return SUCCESS( InterfaceMsg( msg, _bot->GetGameEntity() ) );
	}

	bool FireTeamDisband( Client *_bot )
	{
		MessageHelper msg( ET_MSG_FIRETEAM_DISBAND );
		return SUCCESS( InterfaceMsg( msg, _bot->GetGameEntity() ) );
	}

	bool FireTeamLeave( Client *_bot )
	{
		MessageHelper msg( ET_MSG_FIRETEAM_LEAVE );
		return SUCCESS( InterfaceMsg( msg, _bot->GetGameEntity() ) );
	}

	bool FireTeamApply( Client *_bot, int _fireteamnum )
	{
		ET_FireTeamApply data = { _fireteamnum };
		MessageHelper msg( ET_MSG_FIRETEAM_APPLY, &data, sizeof( ET_FireTeamApply ) );
		return SUCCESS( InterfaceMsg( msg, _bot->GetGameEntity() ) );
	}

	bool FireTeamInvite( Client *_bot, GameEntity _target )
	{
		ET_FireTeam data = { _target };
		MessageHelper msg( ET_MSG_FIRETEAM_INVITE, &data, sizeof( ET_FireTeam ) );
		return SUCCESS( InterfaceMsg( msg, _bot->GetGameEntity() ) );
	}

	bool FireTeamWarn( Client *_bot, GameEntity _target )
	{
		ET_FireTeam data = { _target };
		MessageHelper msg( ET_MSG_FIRETEAM_WARN, &data, sizeof( ET_FireTeam ) );
		return SUCCESS( InterfaceMsg( msg, _bot->GetGameEntity() ) );
	}

	bool FireTeamKick( Client *_bot, GameEntity _target )
	{
		ET_FireTeam data = { _target };
		MessageHelper msg( ET_MSG_FIRETEAM_KICK, &data, sizeof( ET_FireTeam ) );
		return SUCCESS( InterfaceMsg( msg, _bot->GetGameEntity() ) );
	}

	bool FireTeamPropose( Client *_bot, GameEntity _target )
	{
		ET_FireTeam data = { _target };
		MessageHelper msg( ET_MSG_FIRETEAM_PROPOSE, &data, sizeof( ET_FireTeam ) );
		return SUCCESS( InterfaceMsg( msg, _bot->GetGameEntity() ) );
	}

	bool FireTeamGetInfo( Client *_bot, ET_FireTeamInfo &_data )
	{
		MessageHelper msg( ET_MSG_FIRETEAM_INFO, &_data, sizeof( ET_FireTeamInfo ) );
		return SUCCESS( InterfaceMsg( msg, _bot->GetGameEntity() ) );
	}

	void DisableBotPush( Client *_bot, int _push )
	{
		ET_DisableBotPush data = { _push };
		MessageHelper msg( ET_MSG_DISABLEBOTPUSH, &data, sizeof( data ) );
		InterfaceMsg( msg, _bot->GetGameEntity() );
	}
};