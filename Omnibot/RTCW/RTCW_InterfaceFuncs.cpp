////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "RTCW_InterfaceFuncs.h"
#include "RTCW_Client.h"

#include "BotWeaponSystem.h"

namespace InterfaceFuncs
{
	bool SendPrivateMessage(Client *_bot, char *_targName, char *_message)
	{
		if (_targName && _message)
		{
			RTCW_SendPM data = { _targName, _message };
			Utils::StringCopy(data.mTargetName, _targName, sizeof(data.mTargetName) / sizeof(data.mTargetName[0]));
			Utils::StringCopy(data.mMessage, _message, sizeof(data.mMessage) / sizeof(data.mMessage[0]));
			MessageHelper msg(RTCW_MSG_SENDPM, &data, sizeof(data));
			InterfaceMsg(msg, _bot->GetGameEntity());
		}
		return true;
	}

	bool IsWeaponOverheated(Client *_bot, RTCW_Weapon _weapon)
	{
		RTCW_WeaponOverheated data = { _weapon, False };
		MessageHelper msg(RTCW_MSG_WPOVERHEATED, &data, sizeof(data));
		InterfaceMsg(msg, _bot->GetGameEntity());
		return data.mIsOverheated == True;
	}

	void GetMountedGunHeat(Client *_bot, const GameEntity _gun, int &_cur, int &_max)
	{
		RTCW_WeaponHeatLevel data = { _gun, 0, 0 };
		MessageHelper msg(RTCW_MSG_GETGUNHEAT, &data, sizeof(data));
		InterfaceMsg(msg, _bot->GetGameEntity());
		_cur = data.mCurrent;
		_max = data.mMax;
	}

	ExplosiveState GetExplosiveState(Client *_bot, const GameEntity _dynamite)
	{
		RTCW_ExplosiveState data = { _dynamite, XPLO_INVALID };
		MessageHelper msg(RTCW_MSG_GEXPLOSIVESTATE, &data, sizeof(data));
		InterfaceMsg(msg, _bot ? _bot->GetGameEntity() : GameEntity());
		return data.mState;
	}

	ConstructableState IsDestroyable(Client *_bot, const GameEntity _ent)
	{
		RTCW_Destroyable data = { _ent, CONST_INVALID };
		MessageHelper msg(RTCW_MSG_GDYNDESTROYABLE, &data, sizeof(data));
		InterfaceMsg(msg, _bot ? _bot->GetGameEntity() : GameEntity());
		return data.mState;
	}

	bool HasFlag(Client *_bot)
	{
		RTCW_HasFlag data = { False };
		MessageHelper msg(RTCW_MSG_GHASFLAG, &data, sizeof(data));
		InterfaceMsg(msg, _bot->GetGameEntity());
		return data.mHasFlag == True;
	}

	bool ItemCanBeGrabbed(Client *_bot, const GameEntity _ent)
	{
		RTCW_CanBeGrabbed data = { _ent, False };
		MessageHelper msg(RTCW_MSG_GCANBEGRABBED, &data, sizeof(data));
		InterfaceMsg(msg, _bot->GetGameEntity());
		return data.mCanBeGrabbed == True;
	}

	bool SelectPrimaryWeapon(Client *_bot, RTCW_Weapon _weapon)
	{
		RTCW_SelectWeapon data = { _weapon };
		MessageHelper msg(RTCW_MSG_PICKWEAPON, &data, sizeof(data));
		InterfaceMsg(msg, _bot->GetGameEntity());
		return data.mGood == True;
	}

	bool SelectSecondaryWeapon(Client *_bot, RTCW_Weapon _weapon)
	{
		RTCW_SelectWeapon data = { _weapon };
		MessageHelper msg(RTCW_MSG_PICKWEAPON2, &data, sizeof(data));
		InterfaceMsg(msg, _bot->GetGameEntity());
		return data.mGood == True;
	}

	float GetReinforceTime(Client *_bot)
	{
		RTCW_ReinforceTime data = { 0 };
		MessageHelper msg(RTCW_MSG_REINFORCETIME, &data, sizeof(data));
		InterfaceMsg(msg, _bot->GetGameEntity());
		return (float)data.mReinforceTime / 1000.0f;
	}

	bool IsMedicNear(Client *_bot)
	{
		RTCW_MedicNear data = { False };
		MessageHelper msg(RTCW_MSG_ISMEDICNEAR, &data, sizeof(data));
		InterfaceMsg(msg, _bot->GetGameEntity());
		return data.mMedicNear == True;
	}

	bool GoToLimbo(Client *_bot)
	{
		RTCW_GoLimbo data = { False };
		MessageHelper msg(RTCW_MSG_GOTOLIMBO, &data, sizeof(data));
		InterfaceMsg(msg, _bot->GetGameEntity());
		return data.mGoLimbo == True;
	}

	GameEntity GetMountedPlayerOnMG42(Client *_bot, GameEntity _gun)
	{
		RTCW_MG42MountedPlayer data = { _gun, GameEntity() };
		MessageHelper msg(RTCW_MSG_ISGUNMOUNTED, &data, sizeof(data));
		InterfaceMsg(msg, _bot->GetGameEntity());
		return data.mMountedEntity;
	}

	bool IsMountableGunRepairable(Client *_bot, GameEntity _gun)
	{
		RTCW_MG42MountedRepairable data = { _gun, False };
		MessageHelper msg(RTCW_MSG_ISGUNREPAIRABLE, &data, sizeof(data));
		InterfaceMsg(msg, _bot->GetGameEntity());
		return data.mRepairable == True;
	}

	int GetGunHealth(Client *_bot, const GameEntity _gun)
	{
		RTCW_MG42Health data = { _gun, 0 };
		MessageHelper msg(RTCW_MSG_GETGUNHEALTH, &data, sizeof(data));
		InterfaceMsg(msg, _bot->GetGameEntity());
		return data.mHealth;
	}

	void GetCurrentCursorHint(Client *_bot, int &_type, int &_val)
	{
		RTCW_CursorHint data = { 0, 0 };
		MessageHelper msg(RTCW_MSG_GETHINT, &data, sizeof(data));
		InterfaceMsg(msg, _bot->GetGameEntity());

		_type = data.mType;
		_val = data.mValue;
	}

	void ChangeSpawnPoint(Client *_bot, int _spawnpoint)
	{
		RTCW_SpawnPoint data = { _spawnpoint };
		MessageHelper msg(RTCW_MSG_CHANGESPAWNPOINT, &data, sizeof(data));
		InterfaceMsg(msg, _bot->GetGameEntity());
	}

	bool GetMg42Properties(Client *_bot, RTCW_MG42Info &_data)
	{
		memset(&_data, 0, sizeof(_data));
		MessageHelper msg(RTCW_MSG_MOUNTEDMG42INFO, &_data, sizeof(RTCW_MG42Info));
		return SUCCESS(InterfaceMsg(msg, _bot->GetGameEntity()));
	}

	bool CanSnipe(Client *_bot)
	{
		if(_bot->GetClass() == RTCW_CLASS_SOLDIER)
		{
			// Make sure we have a sniping weapon.
			if (_bot->GetWeaponSystem()->HasAmmo(RTCW_WP_MAUSER) )
				return true;
		}
		return false;
	}

	int GetSpawnPoint(Client *_bot)
	{
		RTCW_GetSpawnPoint data = { 0 };
		MessageHelper msg(RTCW_MSG_GETSPAWNPOINT, &data, sizeof(data));
		InterfaceMsg(msg, _bot->GetGameEntity());
		return data.mSpawnPoint;
	}

	void SetSuicide(Client *_bot, int _sui, int _pers)
	{
		RTCW_SetSuicide data = { _sui, _pers };
		MessageHelper msg(RTCW_MSG_SETSUICIDE, &data, sizeof(data));
		InterfaceMsg(msg, _bot->GetGameEntity());
	}

	void DisableBotPush(Client *_bot, int _push )
	{
		RTCW_DisableBotPush data = { _push };
		MessageHelper msg(RTCW_MSG_DISABLEBOTPUSH, &data, sizeof(data));
		InterfaceMsg(msg, _bot->GetGameEntity());
	}

	int GetPlayerClass(GameEntity _ent)
	{
		RTCW_GetPlayerClass data = { _ent, 0 };
		MessageHelper msg(RTCW_MSG_GETPLAYERCLASS, &data, sizeof(data));
		InterfaceMsg(msg, _ent);
		return data.mPlayerClass;
	}
};