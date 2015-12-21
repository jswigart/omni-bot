////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __TF_INTERFACEFUNCS_H__
#define __TF_INTERFACEFUNCS_H__

#include "TF_Messages.h"

namespace InterfaceFuncs
{
	int GetPlayerPipeCount( Client *_bot );
	void DisguiseOptions( Client *_bot, TF_DisguiseOptions &_disguiseoptions );
	void Disguise( Client *_bot, int32_t _team, int32_t _class );
	void GetDisguiseInfo( GameEntity _ent, int &_team, int &_class );
	void GetDisguiseInfo( const EntityInfo & entInfo, int &_team, int &_class );
	void Cloak( Client *_bot, bool _silent );

	TF_BuildInfo GetBuildInfo( Client *_bot );
	TF_HealTarget GetHealTargetInfo( Client *_bot );

	bool LockPlayerPosition( GameEntity _ent, bool _lock );

	void ShowHudHint( GameEntity _player, int32_t _id, const char *_msg );
	void ShowHudMenu( TF_HudMenu &_data );
	void ShowHudText( TF_HudText &_data );
};

#endif
