////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "InterfaceFuncs.h"
#include "TF_Messages.h"
#include "TF_InterfaceFuncs.h"
#include "TF_BaseStates.h"

namespace InterfaceFuncs
{
	int GetPlayerPipeCount( Client *_bot )
	{
		TF_PlayerPipeCount data = { 0, 0 };
		MessageHelper msg( TF_MSG_PLAYERPIPECOUNT, &data, sizeof( data ) );
		InterfaceMsg( msg, _bot->GetGameEntity() );
		return data.mNumPipes;
	}

	void Disguise( Client *_bot, int32_t _team, int32_t _class )
	{
		TF_Disguise data = { _team, _class };
		MessageHelper msg( TF_MSG_DISGUISE, &data, sizeof( data ) );
		InterfaceMsg( msg, _bot->GetGameEntity() );
	}

	void DisguiseOptions( Client *_bot, TF_DisguiseOptions &_disguiseoptions )
	{
		MessageHelper msg( TF_MSG_CANDISGUISE, &_disguiseoptions, sizeof( TF_DisguiseOptions ) );
		InterfaceMsg( msg, _bot->GetGameEntity() );
	}

	void GetDisguiseInfo( GameEntity _ent, int &_team, int &_class )
	{
		EntityInfo entInfo;
		if ( IGame::GetEntityInfo( _ent, entInfo ) )
		{
			GetDisguiseInfo( entInfo, _team, _class );
		}
	}

	void GetDisguiseInfo( const EntityInfo & entInfo, int &_team, int &_class )
	{
		// Check team.
		_team = TF_TEAM_NONE;
		for ( int i = 0; i < TF_TEAM_MAX; ++i )
		{
			if ( entInfo.mPowerUps.CheckFlag( TF_Options::DisguiseTeamFlags[ i ] ) )
			{
				_team = i;
				break;
			}
		}
		
		// Check class.
		if ( entInfo.mPowerUps.CheckFlag( TF_PWR_DISGUISE_SCOUT ) )
			_class = TF_CLASS_SCOUT;
		else if ( entInfo.mPowerUps.CheckFlag( TF_PWR_DISGUISE_SNIPER ) )
			_class = TF_CLASS_SNIPER;
		else if ( entInfo.mPowerUps.CheckFlag( TF_PWR_DISGUISE_SOLDIER ) )
			_class = TF_CLASS_SOLDIER;
		else if ( entInfo.mPowerUps.CheckFlag( TF_PWR_DISGUISE_DEMOMAN ) )
			_class = TF_CLASS_DEMOMAN;
		else if ( entInfo.mPowerUps.CheckFlag( TF_PWR_DISGUISE_MEDIC ) )
			_class = TF_CLASS_MEDIC;
		else if ( entInfo.mPowerUps.CheckFlag( TF_PWR_DISGUISE_HWGUY ) )
			_class = TF_CLASS_HWGUY;
		else if ( entInfo.mPowerUps.CheckFlag( TF_PWR_DISGUISE_PYRO ) )
			_class = TF_CLASS_PYRO;
		else if ( entInfo.mPowerUps.CheckFlag( TF_PWR_DISGUISE_ENGINEER ) )
			_class = TF_CLASS_ENGINEER;
		else if ( entInfo.mPowerUps.CheckFlag( TF_PWR_DISGUISE_SPY ) )
			_class = TF_CLASS_SPY;
		else if ( entInfo.mPowerUps.CheckFlag( TF_PWR_DISGUISE_CIVILIAN ) )
			_class = TF_CLASS_CIVILIAN;
		else
			_class = TF_CLASS_NONE;
	}

	TF_BuildInfo GetBuildInfo( Client *_bot )
	{
		TF_BuildInfo buildInfo = {};
		MessageHelper msg( TF_MSG_GETBUILDABLES, &buildInfo, sizeof( buildInfo ) );
		InterfaceMsg( msg, _bot->GetGameEntity() );
		return buildInfo;
	}

	TF_HealTarget GetHealTargetInfo( Client *_bot )
	{
		TF_HealTarget buildInfo = {};
		MessageHelper msg( TF_MSG_GETHEALTARGET, &buildInfo, sizeof( buildInfo ) );
		InterfaceMsg( msg, _bot->GetGameEntity() );
		return buildInfo;
	}

	void Cloak( Client *_bot, bool _silent )
	{
		TF_FeignDeath data = { _silent ? True : False };
		MessageHelper msg( TF_MSG_CLOAK, &data, sizeof( data ) );
		InterfaceMsg( msg, _bot->GetGameEntity() );
	}

	bool LockPlayerPosition( GameEntity _ent, obBool _lock )
	{
		TF_LockPosition data = { _ent, _lock, False };
		MessageHelper msg( TF_MSG_LOCKPOSITION, &data, sizeof( data ) );
		InterfaceMsg( msg );
		return data.mSucceeded != False;
	}

	void ShowHudHint( GameEntity _player, int32_t _id, const char *_msg )
	{
		TF_HudHint data;
		if ( _msg && strlen( _msg ) < sizeof( data.mMessage ) )
		{
			data.mTargetPlayer = _player;
			data.mId = _id;
			Utils::StringCopy( data.mMessage, _msg, sizeof( data.mMessage ) );
			MessageHelper msg( TF_MSG_HUDHINT, &data, sizeof( data ) );
			InterfaceMsg( msg );
		}
	}

	void ShowHudMenu( TF_HudMenu &_data )
	{
		MessageHelper msg( TF_MSG_HUDMENU, &_data, sizeof( _data ) );
		InterfaceMsg( msg );
	}

	void ShowHudText( TF_HudText &_data )
	{
		MessageHelper msg( TF_MSG_HUDTEXT, &_data, sizeof( _data ) );
		InterfaceMsg( msg );
	}

	//TF_TeamPipeInfo	.mTeamPipeInfo[TF_TEAM_MAX] = {0,0,0};
	//
	//const TF_TeamPipeInfo *GetTeamPipeInfo(int _team)
	//{
	//	static const TF_TeamPipeInfo s_NullInfo = {0,0,0};
	//	if(InRangeT<int>(_team, TF_TEAM_NONE, TF_TEAM_MAX-1))
	//	{
	//		MessageHelper msg(TF_MSG_TEAMPIPEINFO, &.mTeamPipeInfo[_team], sizeof(.mTeamPipeInfo[_team]));
	//		gEngineFuncs->InterfaceSendMessage(msg, NULL);
	//		return &.mTeamPipeInfo[_team];
	//	}
	//	OBASSERT("Invalid Team!");
	//	return &s_NullInfo;
	//}
};