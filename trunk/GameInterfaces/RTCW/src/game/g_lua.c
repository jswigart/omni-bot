#include "g_lua.h"

// TODO: aiming for compatibility with ETPro lua mods
// http://wolfwiki.anime.net/index.php/Lua_Mod_API

lua_vm_t * lVM[LUA_NUM_VM];

void QDECL LOG( const char *fmt, ... ) {
	va_list argptr;
	char buff[1024], string[1024];

	va_start( argptr, fmt );
	Q_vsnprintf( buff, sizeof( buff ), fmt, argptr );
	va_end( argptr );

	//if ( g_dedicated.integer ) {
	trap_Printf( buff );
	//}

	if ( level.logFile ) {
		int tens;
		int sec = level.time / 1000;
		int min = sec / 60;
		sec -= min * 60;
		tens = sec / 10;
		sec -= tens * 10;

		Com_sprintf( string, sizeof( string ), "%i:%i%i %s", min, tens, sec, buff );
		trap_FS_Write( string, strlen( string ), level.logFile );
	}
}

/***************************/
/* Lua et library handlers */
/***************************/

// ET Library Calls {{{
// et.RegisterModname( modname )
static int _et_RegisterModname( lua_State *L ) {
	const char *modname = luaL_checkstring( L, 1 );
	if ( modname ) {
		lua_vm_t * vm = G_LuaGetVM( L );
		if ( vm ) {
			Q_strncpyz( vm->mod_name, modname, sizeof( vm->mod_name ) );
		}
	}
	return 0;
}

// vmnumber = et.FindSelf()
static int _et_FindSelf( lua_State *L ) {
	lua_vm_t * vm = G_LuaGetVM( L );
	if ( vm ) {
		lua_pushinteger( L, vm->id );
	} else {
		lua_pushnil( L );
	}
	return 1;
}

// modname, signature = et.FindMod( vmnumber )
static int _et_FindMod( lua_State *L ) {
	int vmnumber = luaL_checkint( L, 1 );
	lua_vm_t * vm = lVM[vmnumber];
	if ( vm ) {
		lua_pushstring( L, vm->mod_name );
		lua_pushstring( L, vm->mod_signature );
	} else {
		lua_pushnil( L );
		lua_pushnil( L );
	}
	return 2;
}

// success = et.IPCSend( vmnumber, message )
static int _et_IPCSend( lua_State *L ) {
	int vmnumber = luaL_checkint( L, 1 );
	const char *message = luaL_checkstring( L, 2 );

	lua_vm_t *sender = G_LuaGetVM( L );
	lua_vm_t *vm = lVM[vmnumber];

	if ( !vm || vm->err ) {
		lua_pushinteger( L, 0 );
		return 1;
	}

	// Find callback
	if ( !G_LuaGetNamedFunction( vm, "et_IPCReceive" ) ) {
		lua_pushinteger( L, 0 );
		return 1;
	}

	// Arguments
	if ( sender ) {
		lua_pushinteger( vm->L, sender->id );
	} else {
		lua_pushnil( vm->L );
	}
	lua_pushstring( vm->L, message );

	// Call
	if ( !G_LuaCall( vm, "et.IPCSend", 2, 0 ) ) {
		//G_LuaStopVM(vm);
		lua_pushinteger( L, 0 );
		return 1;
	}

	// Success
	lua_pushinteger( L, 1 );
	return 1;
}
// }}}

// Printing {{{
// et.G_Print( text )
static int _et_G_Print( lua_State *L ) {
	char text[1024];
	Q_strncpyz( text, luaL_checkstring( L, 1 ), sizeof( text ) );
	trap_Printf( text );
	return 0;
}

// et.G_LogPrint( text )
static int _et_G_LogPrint( lua_State *L ) {
	char text[1024];
	Q_strncpyz( text, luaL_checkstring( L, 1 ), sizeof( text ) );
	LOG( text );
	return 0;
}
// }}}

// Argument Handling {{{
// args = et.ConcatArgs( index )
static int _et_ConcatArgs( lua_State *L ) {
	int index = luaL_checkint( L, 1 );
	lua_pushstring( L, ConcatArgs( index ) );
	return 1;
}

// argcount = et.trap_Argc()
static int _et_trap_Argc( lua_State *L ) {
	lua_pushinteger( L, trap_Argc() );
	return 1;
}

// arg = et.trap_Argv( argnum )
static int _et_trap_Argv( lua_State *L ) {
	char buff[MAX_STRING_CHARS];
	int argnum = luaL_checkint( L, 1 );
	trap_Argv( argnum, buff, sizeof( buff ) );
	lua_pushstring( L, buff );
	return 1;
}
// }}}

// Cvars {{{
// cvarvalue = et.trap_Cvar_Get( cvarname )
static int _et_trap_Cvar_Get( lua_State *L ) {
	char buff[MAX_CVAR_VALUE_STRING];
	const char *cvarname = luaL_checkstring( L, 1 );
	trap_Cvar_VariableStringBuffer( cvarname, buff, sizeof( buff ) );
	lua_pushstring( L, buff );
	return 1;
}

// et.trap_Cvar_Set( cvarname, cvarvalue )
static int _et_trap_Cvar_Set( lua_State *L ) {
	const char *cvarname = luaL_checkstring( L, 1 );
	const char *cvarvalue = luaL_checkstring( L, 2 );
	trap_Cvar_Set( cvarname, cvarvalue );
	return 0;
}
// }}}

// Config Strings {{{
// configstringvalue = et.trap_GetConfigstring( index )
static int _et_trap_GetConfigstring( lua_State *L ) {
	char buff[MAX_STRING_CHARS];
	int index = luaL_checkint( L, 1 );
	trap_GetConfigstring( index, buff, sizeof( buff ) );
	lua_pushstring( L, buff );
	return 1;
}

// et.trap_SetConfigstring( index, configstringvalue )
static int _et_trap_SetConfigstring( lua_State *L ) {
	int index = luaL_checkint( L, 1 );
	const char *csv = luaL_checkstring( L, 2 );
	trap_SetConfigstring( index, csv );
	return 0;
}
// }}}

// Server {{{
// et.trap_SendConsoleCommand( when, command )
static int _et_trap_SendConsoleCommand( lua_State *L ) {
	int when = luaL_checkint( L, 1 );
	const char *cmd = luaL_checkstring( L, 2 );
	trap_SendConsoleCommand( when, cmd );
	return 0;
}
// }}}

// Clients {{{
// et.trap_DropClient( clientnum, reason, ban_time )
static int _et_trap_DropClient( lua_State *L ) {
	int clientnum = luaL_checkint( L, 1 );
	const char *reason = luaL_checkstring( L, 2 );
	//int ban = trap_Cvar_VariableIntegerValue("g_defaultBanTime");
	//ban = luaL_optint(L, 3, ban);
	trap_DropClient( clientnum, reason /*, ban*/ );
	return 0;
}

// et.trap_SendServerCommand( clientnum, command )
static int _et_trap_SendServerCommand( lua_State *L ) {
	int clientnum = luaL_checkint( L, 1 );
	const char *cmd = luaL_checkstring( L, 2 );
	trap_SendServerCommand( clientnum, cmd );
	return 0;
}

// et.G_Say( clientNum, mode, text )
static int _et_G_Say( lua_State *L ) {
	int clientnum = luaL_checkint( L, 1 );
	int mode = luaL_checkint( L, 2 );
	const char *text = luaL_checkstring( L, 3 );
	G_Say( g_entities + clientnum, NULL, mode, text );
	return 0;
}

// et.ClientUserinfoChanged( clientNum )
static int _et_ClientUserinfoChanged( lua_State *L ) {
	int clientnum = luaL_checkint( L, 1 );
	ClientUserinfoChanged( clientnum );
	return 0;
}

// clientnum = et.ClientNumberFromString( string )
// redeye - searches for one partial match with 'string', if one is found the clientnum
// is returned, if there is none or more than one match -1 is returned
static int _et_ClientNumberFromString( lua_State *L ) {
	const char *search = luaL_checkstring( L, 1 );
	int pids[MAX_CLIENTS];

	// only send exact matches, otherwise -1
	if ( ClientNumbersFromString( (char *) search, pids ) == 1 ) {
		lua_pushinteger( L, pids[0] );
	} else {
		lua_pushinteger( L, -1 );
	}

	return 1;
}
// }}}

// Userinfo {{{
// userinfo = et.trap_GetUserinfo( clientnum )
static int _et_trap_GetUserinfo( lua_State *L ) {
	char buff[MAX_STRING_CHARS];
	int clientnum = luaL_checkint( L, 1 );
	trap_GetUserinfo( clientnum, buff, sizeof( buff ) );
	lua_pushstring( L, buff );
	return 1;
}

// et.trap_SetUserinfo( clientnum, userinfo )
static int _et_trap_SetUserinfo( lua_State *L ) {
	int clientnum = luaL_checkint( L, 1 );
	const char *userinfo = luaL_checkstring( L, 2 );
	trap_SetUserinfo( clientnum, userinfo );
	return 0;
}
// }}}

// String Utility Functions {{{
// infostring = et.Info_RemoveKey( infostring, key )
static int _et_Info_RemoveKey( lua_State *L ) {
	char buff[MAX_INFO_STRING];
	const char *key = luaL_checkstring( L, 2 );
	Q_strncpyz( buff, luaL_checkstring( L, 1 ), sizeof( buff ) );
	Info_RemoveKey( buff, key );
	lua_pushstring( L, buff );
	return 1;
}

// infostring = et.Info_SetValueForKey( infostring, key, value )
static int _et_Info_SetValueForKey( lua_State *L ) {
	char buff[MAX_INFO_STRING];
	const char *key = luaL_checkstring( L, 2 );
	const char *value = luaL_checkstring( L, 3 );
	Q_strncpyz( buff, luaL_checkstring( L, 1 ), sizeof( buff ) );
	Info_SetValueForKey( buff, key, value );
	lua_pushstring( L, buff );
	return 1;
}

// keyvalue = et.Info_ValueForKey( infostring, key )
static int _et_Info_ValueForKey( lua_State *L ) {
	const char *infostring = luaL_checkstring( L, 1 );
	const char *key = luaL_checkstring( L, 2 );
	lua_pushstring( L, Info_ValueForKey( infostring, key ) );
	return 1;
}

// cleanstring = et.Q_CleanStr( string )
static int _et_Q_CleanStr( lua_State *L ) {
	char buff[MAX_STRING_CHARS];
	Q_strncpyz( buff, luaL_checkstring( L, 1 ), sizeof( buff ) );
	Q_CleanStr( buff );
	lua_pushstring( L, buff );
	return 1;
}
// }}}

// ET Filesystem {{{
// fd, len = et.trap_FS_FOpenFile( filename, mode )
static int _et_trap_FS_FOpenFile( lua_State *L ) {
	fileHandle_t fd;
	int len;
	const char *filename = luaL_checkstring( L, 1 );
	int mode = luaL_checkint( L, 2 );
	len = trap_FS_FOpenFile( filename, &fd, mode );
	lua_pushinteger( L, fd );
	lua_pushinteger( L, len );
	return 2;
}

// filedata = et.trap_FS_Read( fd, count )
static int _et_trap_FS_Read( lua_State *L ) {
	char *filedata = "";
	fileHandle_t fd = luaL_checkint( L, 1 );
	int count = luaL_checkint( L, 2 );
	filedata = malloc( count + 1 );
	trap_FS_Read( filedata, count, fd );
	*( filedata + count ) = '\0';
	lua_pushstring( L, filedata );
	return 1;
}

// count = et.trap_FS_Write( filedata, count, fd )
static int _et_trap_FS_Write( lua_State *L ) {
	const char *filedata = luaL_checkstring( L, 1 );
	int count = luaL_checkint( L, 2 );
	fileHandle_t fd = luaL_checkint( L, 3 );
	lua_pushinteger( L, trap_FS_Write( filedata, count, fd ) );
	return 1;
}

// et.trap_FS_Rename( oldname, newname )
static int _et_trap_FS_Rename( lua_State *L ) {
	const char *oldname = luaL_checkstring( L, 1 );
	const char *newname = luaL_checkstring( L, 2 );
	trap_FS_Rename( oldname, newname );
	return 0;
}

// et.trap_FS_FCloseFile( fd )
static int _et_trap_FS_FCloseFile( lua_State *L ) {
	fileHandle_t fd = luaL_checkint( L, 1 );
	trap_FS_FCloseFile( fd );
	return 0;
}
// }}}

// Indexes {{{
// soundindex = et.G_SoundIndex( filename )
static int _et_G_SoundIndex( lua_State *L ) {
	const char *filename = luaL_checkstring( L, 1 );
	lua_pushinteger( L, G_SoundIndex( filename ) );
	return 1;
}

// modelindex = et.G_ModelIndex( filename )
static int _et_G_ModelIndex( lua_State *L ) {
	const char *filename = luaL_checkstring( L, 1 );
	lua_pushinteger( L, G_ModelIndex( (char *)filename ) );
	return 1;
}
// }}}

// Sound {{{
// et.G_Sound( entnum, soundindex )
static int _et_G_Sound( lua_State *L ) {
	int entnum = luaL_checkint( L, 1 );
	int soundindex = luaL_checkint( L, 2 );
	G_Sound( g_entities + entnum, soundindex );
	return 0;
}
// }}}

// Miscellaneous {{{
// milliseconds = et.trap_Milliseconds()
static int _et_trap_Milliseconds( lua_State *L ) {
	lua_pushinteger( L, trap_Milliseconds() );
	return 1;
}

// et.G_Damage( target, inflictor, attacker, damage, dflags, mod )
static int _et_G_Damage( lua_State *L ) {
	int target = luaL_checkint( L, 1 );
	int inflictor = luaL_checkint( L, 2 );
	int attacker = luaL_checkint( L, 3 );
	int damage = luaL_checkint( L, 4 );
	int dflags = luaL_checkint( L, 5 );
	int mod = luaL_checkint( L, 6 );

	G_Damage( g_entities + target,
		g_entities + inflictor,
		g_entities + attacker,
		NULL,
		NULL,
		damage,
		dflags,
		mod );

	return 0;
}
// }}}

// Entities {{{
// client entity fields
static const gentity_field_t gclient_fields[] = {
	//								NAME							ALIAS						TYPE				FLAGS
	_et_gclient_addfield(       inactivityTime,                                             FIELD_INT,          0                                       ),
	_et_gclient_addfield(       inactivityWarning,                                          FIELD_INT,          0                                       ),

	_et_gclient_addfield(       pers.connected,                                             FIELD_INT,          0                                       ),
	_et_gclient_addfield(       pers.netname,                                               FIELD_STRING,       FIELD_FLAG_NOPTR                        ),
	_et_gclient_addfield(       pers.localClient,                                           FIELD_INT,          0                                       ),
	_et_gclient_addfield(       pers.initialSpawn,                                          FIELD_INT,          0                                       ),
	_et_gclient_addfield(       pers.enterTime,                                             FIELD_INT,          0                                       ),
	_et_gclient_addfield(       pers.connectTime,                                           FIELD_INT,          0                                       ),
	_et_gclient_addfield(       pers.teamState.state,                                       FIELD_INT,          0                                       ),
	_et_gclient_addfield(       pers.voteCount,                                             FIELD_INT,          0                                       ),
	_et_gclient_addfield(       pers.teamVoteCount,                                         FIELD_INT,          0                                       ),
	_et_gclient_addfield(       pers.complaints,                                            FIELD_INT,          0                                       ),
	_et_gclient_addfield(       pers.complaintClient,                                       FIELD_INT,          0                                       ),
	_et_gclient_addfield(       pers.complaintEndTime,                                      FIELD_INT,          0                                       ),
	_et_gclient_addfield(       pers.lastReinforceTime,                                     FIELD_INT,          0                                       ),
	_et_gclient_addfield(       pers.damage_given,                                          FIELD_INT,          0                                       ),
	_et_gclient_addfield(       pers.damage_received,                                       FIELD_INT,          0                                       ),
	_et_gclient_addfield(       pers.kills,                                                 FIELD_INT,          0                                       ),
	_et_gclient_addfield(       pers.deaths,                                                FIELD_INT,          0                                       ),
	_et_gclient_addfield(       pers.suicides,                                              FIELD_INT,          0                                       ),
	_et_gclient_addfield(       pers.teamKills,                                             FIELD_INT,          0                                       ),
	_et_gclient_addfield(       pers.team_damage,                                           FIELD_INT,          0                                       ),
	_et_gclient_addfield(       pers.gibs,                                                  FIELD_INT,          0                                       ),
	_et_gclient_addfield(       pers.teamGibs,                                              FIELD_INT,          0                                       ),
	_et_gclient_addfield(       pers.revives,                                               FIELD_INT,          0                                       ),
	_et_gclient_addfield(       pers.medPacks,                                              FIELD_INT,          0                                       ),
	_et_gclient_addfield(       pers.ammoPacks,                                             FIELD_INT,          0                                       ),
	_et_gclient_addfield(       pers.acc_hits,                                              FIELD_INT,          0                                       ),
	_et_gclient_addfield(       pers.acc_shots,                                             FIELD_INT,          0                                       ),
	_et_gclient_addfield(       pers.headshots,                                             FIELD_INT,          0                                       ),
	_et_gclient_addfield(       pers.goomba,                                                FIELD_INT,          0                                       ),
	_et_gclient_addfield(       pers.poison,                                                FIELD_INT,          0                                       ),
	_et_gclient_addfield(       pers.knifeKill,                                             FIELD_INT,          0                                       ),
	_et_gclient_addfield(       pers.killSpreeMax,                                          FIELD_INT,          0                                       ),
	_et_gclient_addfield(       pers.deathSpreeMax,                                         FIELD_INT,          0                                       ),

	_et_gclient_addfield(       ps.stats,                                                   FIELD_INT_ARRAY,    0                                       ),
	_et_gclient_addfield(       ps.persistant,                                              FIELD_INT_ARRAY,    0                                       ),
	_et_gclient_addfield(       ps.ping,                                                    FIELD_INT,          0                                       ),
	_et_gclient_addfield(       ps.powerups,                                                FIELD_INT_ARRAY,    0                                       ),
	_et_gclient_addfield(       ps.origin,                                                  FIELD_VEC3,         0                                       ),
	_et_gclient_addfield(       ps.ammo,                                                    FIELD_INT_ARRAY,    0                                       ),
	_et_gclient_addfield(       ps.ammoclip,                                                FIELD_INT_ARRAY,    0                                       ),
	_et_gclient_addfield(       ps.persistant[PERS_SCORE],                                  FIELD_INT,          0                                       ),

	_et_gclient_addfield(       sess.sessionTeam,                                           FIELD_INT,          0                                       ),
	_et_gclient_addfield(       sess.spectatorTime,                                         FIELD_INT,          0                                       ),
	_et_gclient_addfield(       sess.spectatorState,                                        FIELD_INT,          0                                       ),
	_et_gclient_addfield(       sess.spectatorClient,                                       FIELD_ENTITY,       0                                       ),
	_et_gclient_addfield(       sess.playerType,                                            FIELD_INT,          0                                       ),
	_et_gclient_addfield(       sess.playerWeapon,                                          FIELD_INT,          0                                       ),
	_et_gclient_addfield(       sess.playerWeapon2,                                         FIELD_INT,          0                                       ),
	_et_gclient_addfield(       sess.spawnObjectiveIndex,                                   FIELD_INT,          0                                       ),
	_et_gclient_addfield(       sess.latchPlayerType,                                       FIELD_INT,          0                                       ),
	_et_gclient_addfield(       sess.latchPlayerWeapon,                                     FIELD_INT,          0                                       ),
	_et_gclient_addfield(       sess.latchPlayerWeapon2,                                    FIELD_INT,          0                                       ),

	// credits
	_et_gclient_addfield(       sess.credits,                                               FIELD_INT,          0                                       ),
	_et_gclient_addfield(       sess.currentBetTarget,                                      FIELD_ENTITY,       0                                       ),
	_et_gclient_addfield(       sess.currentBetAmount,                                      FIELD_INT,          0                                       ),
	_et_gclient_addfield(       sess.betTime,                                               FIELD_INT,          0                                       ),
	_et_gclient_addfield(       sess.buyTime,                                               FIELD_INT,          0                                       ),

	// TODO: sess.aWeaponStats
	//_et_gclient_addfield(sess.aWeaponStats, FIELD_?_ARRAY, 0),

	// To be compatible with ETPro:
	_et_gclient_addfieldalias(  client.inactivityTime,          inactivityTime,             FIELD_INT,          0                                       ),
	_et_gclient_addfieldalias(  client.inactivityWarning,       inactivityWarning,          FIELD_INT,          0                                       ),
	// origin: use ps.origin instead of r.currentOrigin
	//         for client entities
	_et_gclient_addfieldalias(  origin,                         ps.origin,                  FIELD_VEC3,         0                                       ),

	{ NULL },
};

// entity fields
static const gentity_field_t gentity_fields[] = {
	//								NAME							ALIAS						TYPE				FLAGS
	_et_gentity_addfield(       activator,                                                  FIELD_ENTITY,       FIELD_FLAG_READONLY ),
	_et_gentity_addfield(       chain,                                                      FIELD_ENTITY,       0                   ),
	_et_gentity_addfield(       classname,                                                  FIELD_STRING,       0                   ),
	_et_gentity_addfield(       clipmask,                                                   FIELD_INT,          0                   ),
	_et_gentity_addfield(       closespeed,                                                 FIELD_FLOAT,        0                   ),
	_et_gentity_addfield(       count,                                                      FIELD_INT,          0                   ),
	_et_gentity_addfield(       count2,                                                     FIELD_INT,          0                   ),
	_et_gentity_addfield(       damage,                                                     FIELD_INT,          0                   ),
	_et_gentity_addfield(       delay,                                                      FIELD_FLOAT,        0                   ),
	_et_gentity_addfield(       dl_atten,                                                   FIELD_INT,          0                   ),
	_et_gentity_addfield(       dl_color,                                                   FIELD_VEC3,         0                   ),
	_et_gentity_addfield(       dl_shader,                                                  FIELD_STRING,       FIELD_FLAG_READONLY ),
	_et_gentity_addfield(       dl_stylestring,                                             FIELD_STRING,       FIELD_FLAG_READONLY ),
	_et_gentity_addfield(       duration,                                                   FIELD_FLOAT,        0                   ),
	_et_gentity_addfield(       end_size,                                                   FIELD_INT,          0                   ),
	_et_gentity_addfield(       enemy,                                                      FIELD_ENTITY,       0                   ),
	_et_gentity_addfield(       flags,                                                      FIELD_INT,          FIELD_FLAG_READONLY ),
	_et_gentity_addfield(       harc,                                                       FIELD_FLOAT,        0                   ),
	_et_gentity_addfield(       health,                                                     FIELD_INT,          0                   ),
	_et_gentity_addfield(       inuse,                                                      FIELD_INT,          FIELD_FLAG_READONLY ),
	_et_gentity_addfield(       isProp,                                                     FIELD_INT,          FIELD_FLAG_READONLY ),
	_et_gentity_addfield(       item,                                                       FIELD_STRING,       0                   ),
	_et_gentity_addfield(       key,                                                        FIELD_INT,          0                   ),
	_et_gentity_addfield(       message,                                                    FIELD_STRING,       0                   ),
	_et_gentity_addfield(       methodOfDeath,                                              FIELD_INT,          0                   ),
	_et_gentity_addfield(       mg42BaseEnt,                                                FIELD_INT,          0                   ),
	_et_gentity_addfield(       missionLevel,                                               FIELD_INT,          0                   ),
	_et_gentity_addfield(       model,                                                      FIELD_STRING,       FIELD_FLAG_READONLY ),
	_et_gentity_addfield(       model2,                                                     FIELD_STRING,       FIELD_FLAG_READONLY ),
	_et_gentity_addfield(       nextTrain,                                                  FIELD_ENTITY,       0                   ),
	_et_gentity_addfield(       noise_index,                                                FIELD_INT,          0                   ),
	_et_gentity_addfield(       prevTrain,                                                  FIELD_ENTITY,       0                   ),
	_et_gentity_addfield(       props_frame_state,                                          FIELD_INT,          FIELD_FLAG_READONLY ),
	_et_gentity_addfield(       r.absmax,                                                   FIELD_VEC3,         FIELD_FLAG_READONLY ),
	_et_gentity_addfield(       r.absmin,                                                   FIELD_VEC3,         FIELD_FLAG_READONLY ),
	_et_gentity_addfield(       r.bmodel,                                                   FIELD_INT,          FIELD_FLAG_READONLY ),
	_et_gentity_addfield(       r.contents,                                                 FIELD_INT,          0                   ),
	_et_gentity_addfield(       r.currentAngles,                                            FIELD_VEC3,         0                   ),
	_et_gentity_addfield(       r.currentOrigin,                                            FIELD_VEC3,         0                   ),
	_et_gentity_addfield(       r.eventTime,                                                FIELD_INT,          0                   ),
	_et_gentity_addfield(       r.linkcount,                                                FIELD_INT,          FIELD_FLAG_READONLY ),
	_et_gentity_addfield(       r.linked,                                                   FIELD_INT,          FIELD_FLAG_READONLY ),
	_et_gentity_addfield(       r.maxs,                                                     FIELD_VEC3,         0                   ),
	_et_gentity_addfield(       r.mins,                                                     FIELD_VEC3,         0                   ),
	_et_gentity_addfield(       r.ownerNum,                                                 FIELD_INT,          0                   ),
	_et_gentity_addfield(       r.singleClient,                                             FIELD_INT,          0                   ),
	_et_gentity_addfield(       r.svFlags,                                                  FIELD_INT,          0                   ),
	_et_gentity_addfield(       r.worldflags,                                               FIELD_INT,          FIELD_FLAG_READONLY ),
	_et_gentity_addfield(       radius,                                                     FIELD_INT,          0                   ),
	_et_gentity_addfield(       random,                                                     FIELD_FLOAT,        0                   ),
	_et_gentity_addfield(       rotate,                                                     FIELD_VEC3,         0                   ),
	_et_gentity_addfield(       s.angles,                                                   FIELD_VEC3,         0                   ),
	_et_gentity_addfield(       s.angles2,                                                  FIELD_VEC3,         0                   ),
	_et_gentity_addfield(       s.apos,                                                     FIELD_TRAJECTORY,   0                   ),
	_et_gentity_addfield(       s.clientNum,                                                FIELD_INT,          0                   ),
	_et_gentity_addfield(       s.constantLight,                                            FIELD_INT,          0                   ),
	_et_gentity_addfield(       s.density,                                                  FIELD_INT,          0                   ),
	_et_gentity_addfield(       s.dl_intensity,                                             FIELD_INT,          0                   ),
	_et_gentity_addfield(       s.dmgFlags,                                                 FIELD_INT,          0                   ),
	_et_gentity_addfield(       s.eFlags,                                                   FIELD_INT,          0                   ),
	_et_gentity_addfield(       s.eType,                                                    FIELD_INT,          0                   ),
	_et_gentity_addfield(       s.effect1Time,                                              FIELD_INT,          0                   ),
	_et_gentity_addfield(       s.effect2Time,                                              FIELD_INT,          0                   ),
	_et_gentity_addfield(       s.effect3Time,                                              FIELD_INT,          0                   ),
	_et_gentity_addfield(       s.frame,                                                    FIELD_INT,          0                   ),
	_et_gentity_addfield(       s.groundEntityNum,                                          FIELD_INT,          FIELD_FLAG_READONLY ),
	_et_gentity_addfield(       s.loopSound,                                                FIELD_INT,          0                   ),
	_et_gentity_addfield(       s.modelindex,                                               FIELD_INT,          0                   ),
	_et_gentity_addfield(       s.modelindex2,                                              FIELD_INT,          0                   ),
	_et_gentity_addfield(       s.number,                                                   FIELD_INT,          FIELD_FLAG_READONLY ),
	_et_gentity_addfield(       s.onFireEnd,                                                FIELD_INT,          0                   ),
	_et_gentity_addfield(       s.onFireStart,                                              FIELD_INT,          0                   ),
	_et_gentity_addfield(       s.origin,                                                   FIELD_VEC3,         0                   ),
	_et_gentity_addfield(       s.origin2,                                                  FIELD_VEC3,         0                   ),
	_et_gentity_addfield(       s.pos,                                                      FIELD_TRAJECTORY,   0                   ),
	_et_gentity_addfield(       s.powerups,                                                 FIELD_INT,          FIELD_FLAG_READONLY ),
	_et_gentity_addfield(       s.solid,                                                    FIELD_INT,          0                   ),
	_et_gentity_addfield(       s.teamNum,                                                  FIELD_INT,          0                   ),
	_et_gentity_addfield(       s.time,                                                     FIELD_INT,          0                   ),
	_et_gentity_addfield(       s.time2,                                                    FIELD_INT,          0                   ),
	_et_gentity_addfield(       s.weapon,                                                   FIELD_INT,          FIELD_FLAG_READONLY ),
	_et_gentity_addfield(       s.eventParm,                                                FIELD_INT,          0                   ),
	_et_gentity_addfield(       scriptName,                                                 FIELD_STRING,       FIELD_FLAG_READONLY ),
	_et_gentity_addfield(       spawnflags,                                                 FIELD_INT,          FIELD_FLAG_READONLY ),
	_et_gentity_addfield(       spawnitem,                                                  FIELD_STRING,       FIELD_FLAG_READONLY ),
	_et_gentity_addfield(       speed,                                                      FIELD_INT,          0                   ),
	_et_gentity_addfield(       splashDamage,                                               FIELD_INT,          0                   ),
	_et_gentity_addfield(       splashMethodOfDeath,                                        FIELD_INT,          0                   ),
	_et_gentity_addfield(       splashRadius,                                               FIELD_INT,          0                   ),
	_et_gentity_addfield(       start_size,                                                 FIELD_INT,          0                   ),
	_et_gentity_addfield(       tagName,                                                    FIELD_STRING,       FIELD_FLAG_READONLY ),
	_et_gentity_addfield(       tagParent,                                                  FIELD_ENTITY,       0                   ),
	_et_gentity_addfield(       takedamage,                                                 FIELD_INT,          0                   ),
	_et_gentity_addfield(       target,                                                     FIELD_STRING,       0                   ),
	_et_gentity_addfield(       TargetAngles,                                               FIELD_VEC3,         0                   ),
	_et_gentity_addfield(       TargetFlag,                                                 FIELD_INT,          FIELD_FLAG_READONLY ),
	_et_gentity_addfield(       targetname,                                                 FIELD_STRING,       FIELD_FLAG_READONLY ),
	_et_gentity_addfield(       teamchain,                                                  FIELD_ENTITY,       0                   ),
	_et_gentity_addfield(       teammaster,                                                 FIELD_ENTITY,       0                   ),
	_et_gentity_addfield(       track,                                                      FIELD_STRING,       FIELD_FLAG_READONLY ),
	_et_gentity_addfield(       varc,                                                       FIELD_FLOAT,        0                   ),
	_et_gentity_addfield(       wait,                                                       FIELD_FLOAT,        0                   ),
	_et_gentity_addfield(       waterlevel,                                                 FIELD_INT,          FIELD_FLAG_READONLY ),
	_et_gentity_addfield(       watertype,                                                  FIELD_INT,          FIELD_FLAG_READONLY ),

	// To be compatible with ETPro:
	// origin: use r.currentOrigin instead of ps.origin
	//         for non client entities
	_et_gentity_addfieldalias(  origin,                         r.currentOrigin,            FIELD_VEC3,         0                   ),

	{ NULL },
};

// gentity fields helper functions
gentity_field_t *_et_gentity_getfield( gentity_t *ent, char *fieldname ) {
	int i;

	// search through client fields first
	if ( ent->client ) {
		for ( i = 0; gclient_fields[i].name; i++ ) {
			if ( Q_stricmp( fieldname, gclient_fields[i].name ) == 0 ) {
				return (gentity_field_t *)&gclient_fields[i];
			}
		}
	}

	for ( i = 0; gentity_fields[i].name; i++ ) {
		if ( Q_stricmp( fieldname, gentity_fields[i].name ) == 0 ) {
			return (gentity_field_t *)&gentity_fields[i];
		}
	}

	return 0;
}

void _et_gentity_getvec3( lua_State *L, vec3_t vec3 ) {
	lua_newtable( L );
	lua_pushnumber( L, vec3[0] );
	lua_rawseti( L, -2, 1 );
	lua_pushnumber( L, vec3[1] );
	lua_rawseti( L, -2, 2 );
	lua_pushnumber( L, vec3[2] );
	lua_rawseti( L, -2, 3 );
}

void _et_gentity_setvec3( lua_State *L, vec3_t *vec3 ) {
	lua_pushnumber( L, 1 );
	lua_gettable( L, -2 );
	( *vec3 )[0] = (float)lua_tonumber( L, -1 );
	lua_pop( L, 1 );
	lua_pushnumber( L, 2 );
	lua_gettable( L, -2 );
	( *vec3 )[1] = (float)lua_tonumber( L, -1 );
	lua_pop( L, 1 );
	lua_pushnumber( L, 3 );
	lua_gettable( L, -2 );
	( *vec3 )[2] = (float)lua_tonumber( L, -1 );
	lua_pop( L, 1 );
}

void _et_gentity_gettrajectory( lua_State *L, trajectory_t *traj ) {
	int index;

	lua_newtable( L );
	index = lua_gettop( L );
	lua_pushstring( L, "trTime" );
	lua_pushinteger( L, traj->trTime );
	lua_settable( L, -3 );
	lua_pushstring( L, "trType" );
	lua_pushinteger( L, traj->trType );
	lua_settable( L, -3 );
	lua_pushstring( L, "trDuration" );
	lua_pushinteger( L, traj->trDuration );
	lua_settable( L, -3 );
	lua_settop( L, index );
	lua_pushstring( L, "trBase" );
	_et_gentity_getvec3( L, traj->trBase );
	lua_settable( L, -3 );
	lua_settop( L, index );
	lua_pushstring( L, "trDelta" );
	_et_gentity_getvec3( L, traj->trDelta );
	lua_settable( L, -3 );
}

void _et_gentity_settrajectory( lua_State *L, trajectory_t *traj ) {
	lua_pushstring( L, "trType" );
	lua_gettable( L, -2 );
	traj->trType = (float)lua_tonumber( L, -1 );
	lua_pop( L, 1 );
	lua_pushstring( L, "trTime" );
	lua_gettable( L, -2 );
	traj->trTime = (float)lua_tonumber( L, -1 );
	lua_pop( L, 1 );
	lua_pushstring( L, "trDuration" );
	lua_gettable( L, -2 );
	traj->trDuration = (float)lua_tonumber( L, -1 );
	lua_pop( L, 1 );
	lua_pushstring( L, "trBase" );
	lua_gettable( L, -2 );
	_et_gentity_setvec3( L, (vec3_t *)traj->trBase );
	lua_pop( L, 1 );
	lua_pushstring( L, "trDelta" );
	lua_gettable( L, -2 );
	_et_gentity_setvec3( L, (vec3_t *)traj->trDelta );
	lua_pop( L, 1 );
}

// entnum = et.G_Spawn()
static int _et_G_Spawn( lua_State *L ) {
	gentity_t *entnum = G_Spawn();
	lua_pushinteger( L, entnum - g_entities );
	return 1;
}

// entnum = et.G_TempEntity( origin, event )
static int _et_G_TempEntity( lua_State *L ) {
	vec3_t origin;
	int event = luaL_checkint( L, 2 );
	lua_pop( L, 1 );
	_et_gentity_setvec3( L, &origin );
	lua_pushinteger( L, G_TempEntity( origin, event ) - g_entities );
	return 1;
}

// et.G_FreeEntity( entnum )
static int _et_G_FreeEntity( lua_State *L ) {
	int entnum = luaL_checkint( L, 1 );
	G_FreeEntity( g_entities + entnum );
	return 0;
}

// TODO:
// spawnval = et.G_GetSpawnVar( entnum, key )
// et.G_SetSpawnVar( entnum, key, value )
// integer entnum = et.G_SpawnGEntityFromSpawnVars( string spawnvar, string spawnvalue, ... )

// et.trap_LinkEntity( entnum )
static int _et_trap_LinkEntity( lua_State *L ) {
	int entnum = luaL_checkint( L, 1 );
	trap_LinkEntity( g_entities + entnum );
	return 0;
}

// et.trap_UnlinkEntity( entnum )
static int _et_trap_UnlinkEntity( lua_State *L ) {
	int entnum = luaL_checkint( L, 1 );
	trap_UnlinkEntity( g_entities + entnum );
	return 0;
}

// (variable) = et.gentity_get( entnum, fieldname, arrayindex )
int _et_gentity_get( lua_State *L ) {
	gentity_t *ent = g_entities + luaL_checkint( L, 1 );
	const char *fieldname = luaL_checkstring( L, 2 );
	gentity_field_t *field = _et_gentity_getfield( ent, (char *)fieldname );
	unsigned long addr;

	// break on invalid gentity field
	if ( !field ) {
		luaL_error( L, "tried to get invalid gentity field \"%s\"", fieldname );
		return 0;
	}

	if ( field->flags & FIELD_FLAG_GENTITY ) {
		addr = (unsigned long)ent;
	} else {
		addr = (unsigned long)ent->client;
	}

	// for NULL entities, return nil (prevents server crashes!)
	if ( !addr ) {
		lua_pushnil( L );
		return 1;
	}

	addr += (unsigned long)field->mapping;

	switch ( field->type ) {
	case FIELD_INT:
		lua_pushinteger( L, *(int *)addr );
		return 1;
	case FIELD_STRING:
		if ( field->flags & FIELD_FLAG_NOPTR ) {
			lua_pushstring( L, (char *)addr );
		} else {
			lua_pushstring( L, *(char **)addr );
		}
		return 1;
	case FIELD_FLOAT:
		lua_pushnumber( L, *(float *)addr );
		return 1;
	case FIELD_ENTITY:
		lua_pushinteger( L, ( *(gentity_t **)addr ) - g_entities );
		return 1;
	case FIELD_VEC3:
		_et_gentity_getvec3( L, *(vec3_t *)addr );
		return 1;
	case FIELD_INT_ARRAY:
		lua_pushinteger( L, ( *(int *)( addr + ( sizeof( int ) * luaL_optint( L, 3, 0 ) ) ) ) );
		return 1;
	case FIELD_TRAJECTORY:
		_et_gentity_gettrajectory( L, (trajectory_t *)addr );
		return 1;
	case FIELD_FLOAT_ARRAY:
		lua_pushnumber( L, ( *(float *)( addr + ( sizeof( int ) * luaL_optint( L, 3, 0 ) ) ) ) );
		return 1;
	}
	return 0;
}

// et.gentity_set( entnum, fieldname, arrayindex, (value) )
static int _et_gentity_set( lua_State *L ) {
	gentity_t *ent = g_entities + luaL_checkint( L, 1 );
	const char *fieldname = luaL_checkstring( L, 2 );
	gentity_field_t *field = _et_gentity_getfield( ent, (char *)fieldname );
	unsigned long addr;
	const char *buffer;

	// break on invalid gentity field
	if ( !field ) {
		luaL_error( L, "tried to set invalid gentity field \"%s\"", fieldname );
		return 0;
	}

	// break on read-only gentity field
	if ( field->flags & FIELD_FLAG_READONLY ) {
		luaL_error( L, "tried to set read-only gentity field \"%s\"", fieldname );
		return 0;
	}

	if ( field->flags & FIELD_FLAG_GENTITY ) {
		addr = (unsigned long)ent;
	} else {
		addr = (unsigned long)ent->client;
	}

	// for NULL entities, return nil (prevents server crashes!)
	if ( !addr ) {
		lua_pushnil( L );
		return 1;
	}

	addr += (unsigned long)field->mapping;

	switch ( field->type ) {
	case FIELD_INT:
		*(int *)addr = luaL_checkint( L, 3 );
		break;
	case FIELD_STRING:
		buffer = luaL_checkstring( L, 3 );
		if ( field->flags & FIELD_FLAG_NOPTR ) {
			Q_strncpyz( (char *)addr, buffer, strlen( (char *)addr ) );
		} else {
			free( *(char **)addr );
			*(char **)addr = malloc( strlen( buffer ) );
			Q_strncpyz( *(char **)addr, buffer, strlen( buffer ) );
		}
		break;
	case FIELD_FLOAT:
		*(float *)addr = (float)luaL_checknumber( L, 3 );
		break;
	case FIELD_ENTITY:
		*(gentity_t **)addr = g_entities + luaL_checkint( L, 3 );
		break;
	case FIELD_VEC3:
		_et_gentity_setvec3( L, (vec3_t *)addr );
		break;
	case FIELD_INT_ARRAY:
		*(int *)( addr + ( sizeof( int ) * luaL_checkint( L, 3 ) ) ) = luaL_checkint( L, 4 );
		break;
	case FIELD_TRAJECTORY:
		_et_gentity_settrajectory( L, (trajectory_t *)addr );
		break;
	case FIELD_FLOAT_ARRAY:
		*(float *)( addr + ( sizeof( int ) * luaL_checkint( L, 3 ) ) ) = luaL_checknumber( L, 4 );
		return 1;
	}
	return 0;
}

// et.G_AddEvent( ent, event, eventparm )
static int _et_G_AddEvent( lua_State *L ) {
	int ent = luaL_checkint( L, 1 );
	int event = luaL_checkint( L, 2 );
	int eventparm = luaL_checkint( L, 3 );
	G_AddEvent( g_entities + ent, event, eventparm );
	return 0;
}
// }}}

// et.GetTimeStamp()
// note that this is not a true unix timestamp, but close. should only be used to calc differences of points in 'time'
static int _et_GetTimeStamp( lua_State *L ) {
	unsigned long long timestamp;
	qtime_t q;
	trap_RealTime( &q );
	timestamp = ((q.tm_year * 31556926ULL) + (q.tm_yday * 86400ULL ) + (q.tm_hour * 3600ULL) + (q.tm_min * 60ULL) + q.tm_sec) - 2208932660ULL;
	lua_pushinteger( L, timestamp );
	return 1;
}
// }}}

// et.IsBot
static int _et_IsBot( lua_State *L ) {
	gentity_t *ent = g_entities + luaL_checkint( L, 1 );
	int bot = ent->client && ent->r.svFlags & SVF_BOT ? 1 : 0;
	return bot;
}
// }}}

// et.SetTeam( clientNum, team )
static int _et_SetTeam( lua_State *L ) {
	int clientNum = luaL_checkint( L, 1 );
	const char *team = luaL_checkstring( L, 2 );

	if ( (g_entities + clientNum)->client && ((g_entities + clientNum)->client->pers.connected == CON_CONNECTED) ) {
		SetTeam(g_entities + clientNum, (char *)team, qtrue);
	}
	return 0;
}
// }}}

// et.GetLevelTime()
static int _et_GetLevelTime( lua_State *L ) {
	lua_pushinteger( L, level.time );
	return 1;
}
// }}}

// et.GetMapName()
static int _et_GetMapName( lua_State *L ) {
	lua_pushstring( L, level.rawmapname );
	return 1;
}
// }}}

// et.FlingClient( clientNum, flingType )
static int _et_FlingClient( lua_State *L ) {
	int clientNum = luaL_checkint( L, 1 );
	int flingType = luaL_checkint( L, 2 );

	if ( (g_entities + clientNum)->client && ((g_entities + clientNum)->client->pers.connected == CON_CONNECTED) ) {
		if ( G_FlingClient(g_entities + clientNum, flingType) ) {
			return 1;
		}
	}
	return 0;
}
// }}}

// et.FlingClientA( flingType )
static int _et_FlingClientA( lua_State *L ) {
	int flingType = luaL_checkint( L, 1 );

	G_FlingClientA(flingType);
	return 1;
}
// }}}

// et.SetMap( flingType )
static int _et_SetMap( lua_State *L ) {
	char s[MAX_STRING_CHARS];
	const char *mapName = luaL_checkstring( L, 1 );

	// preserve nextmap so rotation can continue
	trap_Cvar_VariableStringBuffer( "nextmap", s, sizeof( s ) );
	if ( *s ) {
		trap_SendConsoleCommand( EXEC_APPEND, va( "map %s; set nextmap \"%s\"\n", mapName, s ) );
	} else {
		trap_SendConsoleCommand( EXEC_APPEND, va( "map %s;\n", mapName ) );
	}
	return 1;
}
// }}}

// et.lol( clientNum )
static int _et_lol( lua_State *L ) {
	int clientNum = luaL_checkint( L, 1 );
	vec3_t dir;
	int it, grenadeWPID, degPerNade;
	float x, y;
	gentity_t *nade;
	gentity_t *ent = g_entities + clientNum;

	if ( !ent->client ) {
		return 0;
	}

	grenadeWPID = ent->client->sess.sessionTeam == TEAM_RED ? WP_GRENADE_LAUNCHER : WP_GRENADE_PINEAPPLE;

	degPerNade = 36; // 360 / 10
	for( it = 0; it < 10; it++ ) {
		x = 200 * cos( DEG2RAD(degPerNade * it) );
		y = 200 * sin( DEG2RAD(degPerNade * it) );
		VectorSet( dir, x, y, 30 );
		VectorSubtract( ent->client->ps.velocity, dir, dir );
		dir[2] = 60;
		nade = fire_grenade( ent, ent->r.currentOrigin,
			dir, grenadeWPID );
		nade->nextthink = level.time + 1000;
	}

	return 1;
}
// }}}

// et library initialisation array
static const luaL_Reg etlib[] = {
	// ET Library Calls
	{ "RegisterModname",            _et_RegisterModname         },
	{ "FindSelf",                   _et_FindSelf                },
	{ "FindMod",                    _et_FindMod                 },
	{ "IPCSend",                    _et_IPCSend                 },
	// Printing
	{ "G_Print",                    _et_G_Print                 },
	{ "G_LogPrint",                 _et_G_LogPrint              },
	// Argument Handling
	{ "ConcatArgs",                 _et_ConcatArgs              },
	{ "trap_Argc",                  _et_trap_Argc               },
	{ "trap_Argv",                  _et_trap_Argv               },
	// Cvars
	{ "trap_Cvar_Get",              _et_trap_Cvar_Get           },
	{ "trap_Cvar_Set",              _et_trap_Cvar_Set           },
	// Config Strings
	{ "trap_GetConfigstring",       _et_trap_GetConfigstring    },
	{ "trap_SetConfigstring",       _et_trap_SetConfigstring    },
	// Server
	{ "trap_SendConsoleCommand",    _et_trap_SendConsoleCommand },
	// Clients
	{ "trap_DropClient",            _et_trap_DropClient         },
	{ "trap_SendServerCommand",     _et_trap_SendServerCommand  },
	{ "G_Say",                      _et_G_Say                   },
	{ "ClientUserinfoChanged",      _et_ClientUserinfoChanged   },
	{ "ClientNumberFromString",     _et_ClientNumberFromString  },
	// Userinfo
	{ "trap_GetUserinfo",           _et_trap_GetUserinfo        },
	{ "trap_SetUserinfo",           _et_trap_SetUserinfo        },
	// String Utility Functions
	{ "Info_RemoveKey",             _et_Info_RemoveKey          },
	{ "Info_SetValueForKey",        _et_Info_SetValueForKey     },
	{ "Info_ValueForKey",           _et_Info_ValueForKey        },
	{ "Q_CleanStr",                 _et_Q_CleanStr              },
	// ET Filesystem
	{ "trap_FS_FOpenFile",          _et_trap_FS_FOpenFile       },
	{ "trap_FS_Read",               _et_trap_FS_Read            },
	{ "trap_FS_Write",              _et_trap_FS_Write           },
	{ "trap_FS_Rename",             _et_trap_FS_Rename          },
	{ "trap_FS_FCloseFile",         _et_trap_FS_FCloseFile      },
	// Indexes
	{ "G_SoundIndex",               _et_G_SoundIndex            },
	{ "G_ModelIndex",               _et_G_ModelIndex            },
	// Sound
	{ "G_Sound",                    _et_G_Sound                 },
	// Miscellaneous
	{ "trap_Milliseconds",          _et_trap_Milliseconds       },
	{ "G_Damage",                   _et_G_Damage                },
	// Entities
	{ "G_Spawn",                    _et_G_Spawn                 },
	{ "G_TempEntity",               _et_G_TempEntity            },
	{ "G_FreeEntity",               _et_G_FreeEntity            },
	{ "trap_LinkEntity",            _et_trap_LinkEntity         },
	{ "trap_UnlinkEntity",          _et_trap_UnlinkEntity       },
	{ "gentity_get",                _et_gentity_get             },
	{ "gentity_set",                _et_gentity_set             },
	{ "G_AddEvent",                 _et_G_AddEvent              },
	{ "GetTimeStamp",               _et_GetTimeStamp            },
	{ "IsBot",                      _et_IsBot                   },
	{ "SetTeam",                    _et_SetTeam                 },
	{ "GetLevelTime",               _et_GetLevelTime            },
	{ "GetMapName",                 _et_GetMapName              },
	{ "FlingClient",                _et_FlingClient             },
	{ "FlingClientA",               _et_FlingClientA            },
	{ "SetMap",                     _et_SetMap                  },
	{ "lol",                        _et_lol                     },

	{ NULL },
};

/*************/
/* Lua API   */
/*************/

//{{{
/** G_LuaInit()
* Initialises the Lua API interface
*/
qboolean G_LuaInit() {
	int i, num_vm = 0, len, flen = 0;
	char buff[MAX_CVAR_VALUE_STRING], *crt, *code, *signature;
	fileHandle_t f;
	lua_vm_t *vm;

	if ( !lua_modules.string[0] ) {
		return qtrue;
	}

	Q_strncpyz( buff, lua_modules.string, sizeof( buff ) );
	len = strlen( buff );
	crt = buff;

	for ( i = 0; i < LUA_NUM_VM; i++ )
		lVM[i] = NULL;

	for ( i = 0; i <= len; i++ )
		if ( buff[i] == ' ' || buff[i] == '\0' || buff[i] == ',' || buff[i] == ';' ) {
			buff[i] = '\0';

			// try to open lua file
			flen = trap_FS_FOpenFile( crt, &f, FS_READ );
			if ( flen < 0 ) {
				LOG( "Lua API: can not open file %s\n", crt );
			} else if ( flen > LUA_MAX_FSIZE ) {
				// quad: Let's not load arbitrarily big files to memory.
				// If your lua file exceeds the limit, let me know.
				LOG( "Lua API: ignoring file %s (too big)\n", crt );
				trap_FS_FCloseFile( f );
			} else {
				code = malloc( flen + 1 );
				if ( !code ) { return qfalse; }
				trap_FS_Read( code, flen, f );
				*( code + flen ) = '\0';
				trap_FS_FCloseFile( f );
				signature = G_SHA1( code );

				if ( Q_stricmp( lua_allowedModules.string, "" ) &&
					!strstr( lua_allowedModules.string, signature ) ) {
						// don't load disallowed lua modules into vm
						LOG( "Lua API: Lua module [%s] [%s] disallowed by ACL\n", crt, signature );
				} else {
					// Init lua_vm_t struct
					vm = (lua_vm_t*) malloc( sizeof( lua_vm_t ) );
					if (!vm) { return qfalse; }
					vm->id = -1;
					Q_strncpyz( vm->file_name, crt, sizeof( vm->file_name ) );
					Q_strncpyz( vm->mod_name, "", sizeof( vm->mod_name ) );
					Q_strncpyz( vm->mod_signature, signature, sizeof( vm->mod_signature ) );
					vm->code = code;
					vm->code_size = flen;
					vm->err = 0;

					// Start lua virtual machine
					if ( G_LuaStartVM( vm ) == qfalse ) {
						G_LuaStopVM( vm );
						vm = NULL;
					} else {
						vm->id = num_vm;
						lVM[num_vm] = vm;
						num_vm++;
					}
				}
			}

			// prepare for next iteration
			if ( i + 1 < len ) {
				crt = buff + i + 1;
			} else {
				crt = NULL;
			}
			if ( num_vm >= LUA_NUM_VM ) {
				LOG( "Lua API: too many lua files specified, only the first %d have been loaded\n", LUA_NUM_VM );
				break;
			}
		}
		return qtrue;
}

/** G_LuaCall( func, vm, nargs, nresults )
* Calls a function already on the stack.
*/
qboolean G_LuaCall( lua_vm_t* vm, char *func, int nargs, int nresults ) {
	int res = lua_pcall( vm->L, nargs, nresults, 0 );
	if ( res == LUA_ERRRUN ) {
		// pheno: made output more ETPro compatible
		LOG( "Lua API: %s error running lua script: %s\n", func, lua_tostring( vm->L, -1 ) );
		lua_pop( vm->L, 1 );
		vm->err++;
		return qfalse;
	} else if ( res == LUA_ERRMEM ) {
		LOG( "Lua API: memory allocation error #2 ( %s )\n", vm->file_name );
		vm->err++;
		return qfalse;
	} else if ( res == LUA_ERRERR ) {
		LOG( "Lua API: traceback error ( %s )\n", vm->file_name );
		vm->err++;
		return qfalse;
	}
	return qtrue;
}

/** G_LuaGetNamedFunction( vm, name )
* Finds a function by name and puts it onto the stack.
* If the function does not exist, returns qfalse.
*/
qboolean G_LuaGetNamedFunction( lua_vm_t *vm, char *name ) {
	if ( vm->L ) {
		lua_getglobal( vm->L, name );
		if ( lua_isfunction( vm->L, -1 ) ) {
			return qtrue;
		} else {
			lua_pop( vm->L, 1 );
			return qfalse;
		}
	}
	return qfalse;
}

/** G_LuaStartVM( vm )
* Starts one individual virtual machine.
*/
qboolean G_LuaStartVM( lua_vm_t *vm ) {
	int res = 0;
	char basepath[MAX_QPATH];
	char homepath[MAX_QPATH];
	char gamepath[MAX_QPATH];
	const char *luaPath, *luaCPath;

	// Open a new lua state
	vm->L = luaL_newstate();
	if ( !vm->L ) {
		LOG( "Lua API: Lua failed to initialise.\n" );
		return qfalse;
	}

	// Initialise the lua state
	luaL_openlibs( vm->L );

	// sqlite3
	luaopen_lsqlite3(vm->L);

	// set LUA_PATH and LUA_CPATH
	trap_Cvar_VariableStringBuffer( "fs_basepath", basepath, sizeof( basepath ) );
	trap_Cvar_VariableStringBuffer( "fs_homepath", homepath, sizeof( homepath ) );
	trap_Cvar_VariableStringBuffer( "fs_game", gamepath, sizeof( gamepath ) );

	luaPath = va( "%s%s%s%s?.lua;%s%s%s%slualibs%s?.lua",
		homepath, LUA_DIRSEP, gamepath, LUA_DIRSEP,
		homepath, LUA_DIRSEP, gamepath, LUA_DIRSEP, LUA_DIRSEP );

	luaCPath = va( "%s%s%s%slualibs%s?.%s",
		homepath, LUA_DIRSEP, gamepath, LUA_DIRSEP, LUA_DIRSEP, EXTENSION );

	// add fs_basepath if different from fs_homepath
	if ( Q_stricmp( basepath, homepath ) ) {
		luaPath = va( "%s%s%s%s?.lua;%s%s%s%slualibs%s?.lua;%s",
			basepath, LUA_DIRSEP, gamepath, LUA_DIRSEP,
			basepath, LUA_DIRSEP, gamepath, LUA_DIRSEP, LUA_DIRSEP, luaPath );

		luaCPath = va( "%s%s%s%slualibs%s?.%s;%s",
			basepath, LUA_DIRSEP, gamepath, LUA_DIRSEP, LUA_DIRSEP, EXTENSION,
			luaCPath );
	}

	lua_getglobal( vm->L, LUA_LOADLIBNAME );
	if ( lua_istable( vm->L, -1 ) ) {
		lua_pushstring( vm->L, luaPath );
		lua_setfield( vm->L, -2, "path" );
		lua_pushstring( vm->L, luaCPath );
		lua_setfield( vm->L, -2, "cpath" );
	}
	lua_pop( vm->L, 1 );

	// register globals
	lua_registerglobal( vm->L, "LUA_PATH", luaPath );
	lua_registerglobal( vm->L, "LUA_CPATH", luaCPath );
	lua_registerglobal( vm->L, "LUA_DIRSEP", LUA_DIRSEP );

	// register predefined constants
	lua_newtable( vm->L );
	lua_regconstinteger( vm->L, CS_PLAYERS );
	lua_regconstinteger( vm->L, EXEC_NOW );
	lua_regconstinteger( vm->L, EXEC_INSERT );
	lua_regconstinteger( vm->L, EXEC_APPEND );
	lua_regconstinteger( vm->L, FS_READ );
	lua_regconstinteger( vm->L, FS_WRITE );
	lua_regconstinteger( vm->L, FS_APPEND );
	lua_regconstinteger( vm->L, FS_APPEND_SYNC );
	lua_regconstinteger( vm->L, SAY_ALL );
	lua_regconststring( vm->L, HOSTARCH );
	lua_setglobal( vm->L, "et" );

	// register functions
	luaL_register( vm->L, "et", etlib );

	// Load the code
	LOG( "Lua API: Loading %s\n", vm->file_name );

	res = luaL_loadbuffer( vm->L, vm->code, vm->code_size, vm->file_name );
	if ( res == LUA_ERRSYNTAX ) {
		LOG( "Lua API: syntax error during pre-compilation: %s\n",
			lua_tostring( vm->L, -1 ) );
		lua_pop( vm->L, 1 );
		vm->err++;
		return qfalse;
	} else if ( res == LUA_ERRMEM ) {
		LOG( "Lua API: memory allocation error #1 ( %s )\n", vm->file_name );
		vm->err++;
		return qfalse;
	}

	// Execute the code
	if ( !G_LuaCall( vm, "G_LuaStartVM", 0, 0 ) ) {
		return qfalse;
	}

	return qtrue;
}

/** G_LuaStopVM( vm )
* Stops one virtual machine, and calls its et_Quit callback.
*/
void G_LuaStopVM( lua_vm_t *vm ) {
	if ( vm == NULL ) {
		return;
	}
	if ( vm->code != NULL ) {
		free( vm->code );
		vm->code = NULL;
	}
	if ( vm->L ) {
		if ( G_LuaGetNamedFunction( vm, "et_Quit" ) ) {
			G_LuaCall( vm, "et_Quit", 0, 0 );
		}
		lua_close( vm->L );
		vm->L = NULL;
	}
	if ( vm->id >= 0 ) {
		if ( lVM[vm->id] == vm ) {
			lVM[vm->id] = NULL;
		}
		if ( !vm->err ) {
			LOG( "Lua API: Lua module [%s] [%s] unloaded.\n", vm->file_name, vm->mod_signature );
		}
	}
	free( vm );
}

/** G_LuaShutdown()
* Shuts down everything related to Lua API.
*/
void G_LuaShutdown() {
	int i;
	lua_vm_t *vm;
	for ( i = 0; i < LUA_NUM_VM; i++ ) {
		vm = lVM[i];
		if ( vm ) {
			G_LuaStopVM( vm );
		}
	}
}

/** G_LuaStatus( ent )
* Prints information on the Lua virtual machines.
*/
void G_LuaStatus( gentity_t *ent ) {
	int i, cnt = 0;
	for ( i = 0; i < LUA_NUM_VM; i++ )
		if ( lVM[i] ) {
			cnt++;
		}

		if ( cnt == 0 ) {
			G_Printf( "Lua API: no scripts loaded." );
			return;
		} else if ( cnt == 1 ) {
			G_Printf( "Lua API: showing lua information ( 1 module loaded )" );
		} else {
			G_Printf( "Lua API: showing lua information ( %d modules loaded )", cnt );
		}
		G_Printf( "\n%-2s %-10s %-40s %-11s", "VM", "Modname", "Signature", "Filename" );
		G_Printf( "\n-- ---------- ---------------------------------------- -----------" );
		for ( i = 0; i < LUA_NUM_VM; i++ ) {
			if ( lVM[i] ) {
				G_Printf( "\n%2d %-10s %-40s %-11s", lVM[i]->id, lVM[i]->mod_name, lVM[i]->mod_signature, lVM[i]->file_name );
			}
		}
		G_Printf( "\n-- ---------- ---------------------------------------- -----------" );
}

/** G_LuaGetVM
* Retrieves the VM for a given lua_State
*/
lua_vm_t * G_LuaGetVM( lua_State *L ) {
	int i;
	for ( i = 0; i < LUA_NUM_VM; i++ )
		if ( lVM[i] && lVM[i]->L == L ) {
			return lVM[i];
		}
		return NULL;
}
//}}}

/*****************************/
/* Lua API hooks / callbacks */
/*****************************/

//{{{
/** G_LuaHook_InitGame
* et_InitGame( levelTime, randomSeed, restart ) callback
*/
void G_LuaHook_InitGame( int levelTime, int randomSeed, int restart ) {
	int i;
	lua_vm_t *vm;
	for ( i = 0; i < LUA_NUM_VM; i++ ) {
		vm = lVM[i];
		if ( vm ) {
			if ( vm->id < 0 /*|| vm->err*/ ) {
				continue;
			}
			if ( !G_LuaGetNamedFunction( vm, "et_InitGame" ) ) {
				continue;
			}
			// Arguments
			lua_pushinteger( vm->L, levelTime );
			lua_pushinteger( vm->L, randomSeed );
			lua_pushinteger( vm->L, restart );
			// Call
			if ( !G_LuaCall( vm, "et_InitGame", 3, 0 ) ) {
				//G_LuaStopVM(vm);
				continue;
			}
			// Return values
		}
	}
}

/** G_LuaHook_ShutdownGame
* et_ShutdownGame( restart )  callback
*/
void G_LuaHook_ShutdownGame( int restart ) {
	int i;
	lua_vm_t *vm;
	for ( i = 0; i < LUA_NUM_VM; i++ ) {
		vm = lVM[i];
		if ( vm ) {
			if ( vm->id < 0 /*|| vm->err*/ ) {
				continue;
			}
			if ( !G_LuaGetNamedFunction( vm, "et_ShutdownGame" ) ) {
				continue;
			}
			// Arguments
			lua_pushinteger( vm->L, restart );
			// Call
			if ( !G_LuaCall( vm, "et_ShutdownGame", 1, 0 ) ) {
				//G_LuaStopVM(vm);
				continue;
			}
			// Return values
		}
	}
}

/** G_LuaHook_LogExit
* et_LogExit()  callback
*/
void G_LuaHook_LogExit() {
	int i;
	lua_vm_t *vm;
	for ( i = 0; i < LUA_NUM_VM; i++ ) {
		vm = lVM[i];
		if ( vm ) {
			if ( vm->id < 0 /*|| vm->err*/ ) {
				continue;
			}
			if ( !G_LuaGetNamedFunction( vm, "et_LogExit" ) ) {
				continue;
			}

			// Call
			if ( !G_LuaCall( vm, "et_LogExit", 0, 0 ) ) {
				//G_LuaStopVM(vm);
				continue;
			}
			// Return values
		}
	}
}

/** G_LuaHook_RunFrame
* et_RunFrame( levelTime )  callback
*/
void G_LuaHook_RunFrame( int levelTime ) {
	int i;
	lua_vm_t *vm;
	for ( i = 0; i < LUA_NUM_VM; i++ ) {
		vm = lVM[i];
		if ( vm ) {
			if ( vm->id < 0 /*|| vm->err*/ ) {
				continue;
			}
			if ( !G_LuaGetNamedFunction( vm, "et_RunFrame" ) ) {
				continue;
			}
			// Arguments
			lua_pushinteger( vm->L, levelTime );
			// Call
			if ( !G_LuaCall( vm, "et_RunFrame", 1, 0 ) ) {
				//G_LuaStopVM(vm);
				continue;
			}
			// Return values
		}
	}
}

/** G_LuaHook_ClientConnect
* rejectreason = et_ClientConnect( clientNum, firstTime, isBot ) callback
*/
qboolean G_LuaHook_ClientConnect( int clientNum, qboolean firstTime, qboolean isBot, char *reason ) {
	int i;
	lua_vm_t *vm;
	for ( i = 0; i < LUA_NUM_VM; i++ ) {
		vm = lVM[i];
		if ( vm ) {
			if ( vm->id < 0 /*|| vm->err*/ ) {
				continue;
			}
			if ( !G_LuaGetNamedFunction( vm, "et_ClientConnect" ) ) {
				continue;
			}
			// Arguments
			lua_pushinteger( vm->L, clientNum );
			lua_pushinteger( vm->L, (int)firstTime );
			lua_pushinteger( vm->L, (int)isBot );
			// Call
			if ( !G_LuaCall( vm, "et_ClientConnect", 3, 1 ) ) {
				//G_LuaStopVM(vm);
				continue;
			}
			// Return values
			if ( lua_isstring( vm->L, -1 ) ) {
				Q_strncpyz( reason, lua_tostring( vm->L, -1 ), MAX_STRING_CHARS );
				return qtrue;
			}
		}
	}
	return qfalse;
}

/** G_LuaHook_ClientDisconnect
* et_ClientDisconnect( clientNum ) callback
*/
void G_LuaHook_ClientDisconnect( int clientNum ) {
	int i;
	lua_vm_t *vm;
	for ( i = 0; i < LUA_NUM_VM; i++ ) {
		vm = lVM[i];
		if ( vm ) {
			if ( vm->id < 0 /*|| vm->err*/ ) {
				continue;
			}
			if ( !G_LuaGetNamedFunction( vm, "et_ClientDisconnect" ) ) {
				continue;
			}
			// Arguments
			lua_pushinteger( vm->L, clientNum );
			// Call
			if ( !G_LuaCall( vm, "et_ClientDisconnect", 1, 0 ) ) {
				//G_LuaStopVM(vm);
				continue;
			}
			// Return values
		}
	}
}

/** G_LuaHook_ClientBegin
* et_ClientBegin( clientNum ) callback
*/
void G_LuaHook_ClientBegin( int clientNum ) {
	int i;
	lua_vm_t *vm;
	for ( i = 0; i < LUA_NUM_VM; i++ ) {
		vm = lVM[i];
		if ( vm ) {
			if ( vm->id < 0 /*|| vm->err*/ ) {
				continue;
			}
			if ( !G_LuaGetNamedFunction( vm, "et_ClientBegin" ) ) {
				continue;
			}
			// Arguments
			lua_pushinteger( vm->L, clientNum );
			// Call
			if ( !G_LuaCall( vm, "et_ClientBegin", 1, 0 ) ) {
				//G_LuaStopVM(vm);
				continue;
			}
			// Return values
		}
	}
}

/** void G_LuaHook_ClientUserinfoChanged(int clientNum);
* et_ClientUserinfoChanged( clientNum ) callback
*/
void G_LuaHook_ClientUserinfoChanged( int clientNum ) {
	int i;
	lua_vm_t *vm;
	for ( i = 0; i < LUA_NUM_VM; i++ ) {
		vm = lVM[i];
		if ( vm ) {
			if ( vm->id < 0 /*|| vm->err*/ ) {
				continue;
			}
			if ( !G_LuaGetNamedFunction( vm, "et_ClientUserinfoChanged" ) ) {
				continue;
			}
			// Arguments
			lua_pushinteger( vm->L, clientNum );
			// Call
			if ( !G_LuaCall( vm, "et_ClientUserinfoChanged", 1, 0 ) ) {
				//G_LuaStopVM(vm);
				continue;
			}
			// Return values
		}
	}
}

/** G_LuaHook_ClientSpawn
* et_ClientSpawn( clientNum, revived ) callback
*/
void G_LuaHook_ClientSpawn( int clientNum, qboolean revived ) {
	int i;
	lua_vm_t *vm;
	for ( i = 0; i < LUA_NUM_VM; i++ ) {
		vm = lVM[i];
		if ( vm ) {
			if ( vm->id < 0 /*|| vm->err*/ ) {
				continue;
			}
			if ( !G_LuaGetNamedFunction( vm, "et_ClientSpawn" ) ) {
				continue;
			}
			// Arguments
			lua_pushinteger( vm->L, clientNum );
			lua_pushinteger( vm->L, (int)revived );
			// Call
			if ( !G_LuaCall( vm, "et_ClientSpawn", 2, 0 ) ) {
				//G_LuaStopVM(vm);
				continue;
			}
			// Return values
		}
	}
}

/** G_LuaHook_ClientCommand
* intercepted = et_ClientCommand( clientNum, command ) callback
*/
qboolean G_LuaHook_ClientCommand( int clientNum, char *command ) {
	int i;
	lua_vm_t *vm;
	for ( i = 0; i < LUA_NUM_VM; i++ ) {
		vm = lVM[i];
		if ( vm ) {
			if ( vm->id < 0 /*|| vm->err*/ ) {
				continue;
			}
			if ( !G_LuaGetNamedFunction( vm, "et_ClientCommand" ) ) {
				continue;
			}
			// Arguments
			lua_pushinteger( vm->L, clientNum );
			lua_pushstring( vm->L, command );
			// Call
			if ( !G_LuaCall( vm, "et_ClientCommand", 2, 1 ) ) {
				//G_LuaStopVM(vm);
				continue;
			}
			// Return values
			if ( lua_isnumber( vm->L, -1 ) ) {
				if ( lua_tointeger( vm->L, -1 ) == 1 ) {
					return qtrue;
				}
			}
		}
	}
	return qfalse;
}

/** G_LuaHook_ConsoleCommand
* intercepted = et_ConsoleCommand( command ) callback
*/
qboolean G_LuaHook_ConsoleCommand( char *command ) {
	int i;
	lua_vm_t *vm;
	for ( i = 0; i < LUA_NUM_VM; i++ ) {
		vm = lVM[i];
		if ( vm ) {
			if ( vm->id < 0 /*|| vm->err*/ ) {
				continue;
			}
			if ( !G_LuaGetNamedFunction( vm, "et_ConsoleCommand" ) ) {
				continue;
			}
			// Arguments
			lua_pushstring( vm->L, command );
			// Call
			if ( !G_LuaCall( vm, "et_ConsoleCommand", 1, 1 ) ) {
				//G_LuaStopVM(vm);
				continue;
			}
			// Return values
			if ( lua_isnumber( vm->L, -1 ) ) {
				if ( lua_tointeger( vm->L, -1 ) == 1 ) {
					return qtrue;
				}
			}
		}
	}
	return qfalse;
}

/** G_LuaHook_Print
* et_Print( text ) callback
*/
void G_LuaHook_Print( char *text ) {
	int i;
	lua_vm_t *vm;
	for ( i = 0; i < LUA_NUM_VM; i++ ) {
		vm = lVM[i];
		if ( vm ) {
			if ( vm->id < 0 /*|| vm->err*/ ) {
				continue;
			}
			if ( !G_LuaGetNamedFunction( vm, "et_Print" ) ) {
				continue;
			}
			// Arguments
			lua_pushstring( vm->L, text );
			// Call
			if ( !G_LuaCall( vm, "et_Print", 1, 0 ) ) {
				//G_LuaStopVM(vm);
				continue;
			}
			// Return values
		}
	}
}

/** G_LuaHook_Obituary
* (customObit) = et_Obituary( victim, killer, meansOfDeath ) callback
*/
qboolean G_LuaHook_Obituary( int victim, int killer, int meansOfDeath, char *customObit ) {
	int i;
	lua_vm_t *vm;
	for ( i = 0; i < LUA_NUM_VM; i++ ) {
		vm = lVM[i];
		if ( vm ) {
			if ( vm->id < 0 /*|| vm->err*/ ) {
				continue;
			}
			if ( !G_LuaGetNamedFunction( vm, "et_Obituary" ) ) {
				continue;
			}
			// Arguments
			lua_pushinteger( vm->L, victim );
			lua_pushinteger( vm->L, killer );
			lua_pushinteger( vm->L, meansOfDeath );
			// Call
			if ( !G_LuaCall( vm, "et_Obituary", 3, 1 ) ) {
				//G_LuaStopVM(vm);
				continue;
			}
			// Return values
			if ( lua_isstring( vm->L, -1 ) ) {
				Q_strncpyz( customObit, lua_tostring( vm->L, -1 ), MAX_STRING_CHARS );
				return qtrue;
			}
		}
	}
	return qfalse;
}
//}}}