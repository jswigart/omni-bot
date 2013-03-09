/*
* name:		g_spawn.c
*
* desc:
*
*/

#include "g_local.h"

qboolean    G_SpawnString( const char *key, const char *defaultString, char **out ) {
	int i;

	if ( !level.spawning ) {
		*out = (char *)defaultString;
	}

	for ( i = 0 ; i < level.numSpawnVars ; i++ ) {
		if ( !strcmp( key, level.spawnVars[i][0] ) ) {
			*out = level.spawnVars[i][1];
			return qtrue;
		}
	}

	*out = (char *)defaultString;
	return qfalse;
}

qboolean    G_SpawnFloat( const char *key, const char *defaultString, float *out ) {
	char        *s;
	qboolean present;

	present = G_SpawnString( key, defaultString, &s );
	*out = atof( s );
	return present;
}

qboolean    G_SpawnInt( const char *key, const char *defaultString, int *out ) {
	char        *s;
	qboolean present;

	present = G_SpawnString( key, defaultString, &s );
	*out = atoi( s );
	return present;
}

qboolean    G_SpawnVector( const char *key, const char *defaultString, float *out ) {
	char        *s;
	qboolean present;

	present = G_SpawnString( key, defaultString, &s );
	sscanf( s, "%f %f %f", &out[0], &out[1], &out[2] );
	return present;
}

//
// fields are needed for spawning from the entity string
//
typedef enum {
	F_INT,
	F_FLOAT,
	F_LSTRING,          // string on disk, pointer in memory, TAG_LEVEL
	F_GSTRING,          // string on disk, pointer in memory, TAG_GAME
	F_VECTOR,
	F_ANGLEHACK,
	F_ENTITY,           // index on disk, pointer in memory
	F_ITEM,             // index on disk, pointer in memory
	F_CLIENT,           // index on disk, pointer in memory
	F_IGNORE
} fieldtype_t;

typedef struct
{
	char    *name;
	int ofs;
	fieldtype_t type;
} field_t;

field_t fields[] = {
	{"classname",   FOFS( classname ),    F_LSTRING},
	{"origin",      FOFS( s.origin ),     F_VECTOR},
	{"model",       FOFS( model ),        F_LSTRING},
	{"model2",      FOFS( model2 ),       F_LSTRING},
	{"spawnflags",  FOFS( spawnflags ),   F_INT},
	{"speed",       FOFS( speed ),        F_FLOAT},
	{"closespeed",  FOFS( closespeed ),   F_FLOAT},   //----(SA)	added
	{"target",      FOFS( target ),       F_LSTRING},
	{"targetname",  FOFS( targetname ),   F_LSTRING},
	{"message",     FOFS( message ),      F_LSTRING},
	{"popup",       FOFS( message ),      F_LSTRING}, // (SA) mutually exclusive from 'message', but makes the ent more logical for the level designer
	{"book",        FOFS( message ),      F_LSTRING}, // (SA) mutually exclusive from 'message', but makes the ent more logical for the level designer
	{"team",        FOFS( team ),         F_LSTRING},
	{"wait",        FOFS( wait ),         F_FLOAT},
	{"random",      FOFS( random ),       F_FLOAT},
	{"count",       FOFS( count ),        F_INT},
	{"health",      FOFS( health ),       F_INT},
	{"light",       0,                  F_IGNORE},
	{"dmg",         FOFS( damage ),       F_INT},
	{"angles",      FOFS( s.angles ),     F_VECTOR},
	{"angle",       FOFS( s.angles ),     F_ANGLEHACK},
	// JOSEPH 9-27-99
	{"duration",    FOFS( duration ),     F_FLOAT},
	{"rotate",      FOFS( rotate ),       F_VECTOR},
	// END JOSEPH
	{"degrees",     FOFS( angle ),        F_FLOAT},
	{"time",        FOFS( speed ),        F_FLOAT},

	// Ridah, AI fields
	{"ainame",      FOFS( aiName ),       F_LSTRING},
	// done.

	// (SA) dlight lightstyles (made all these unique variables for testing)
	{"_color",      FOFS( dl_color ),     F_VECTOR},      // color of the light	(the underscore is inserted by the color picker in QER)
	{"color",       FOFS( dl_color ),     F_VECTOR},      // color of the light
	{"stylestring", FOFS( dl_stylestring ), F_LSTRING},   // user defined stylestring "fffndlsfaaaaaa" for example
	// done

	//----(SA)
	{"shader",      FOFS( dl_shader ), F_LSTRING},    // shader to use for a target_effect or dlight

	// (SA) for target_unlock
	{"key",         FOFS( key ),      F_INT},
	// done

	// (SA) for item placement
	{"stand",       FOFS( s.frame ),      F_INT},
	// (SA)	end

	// Rafael - mg42
	{"harc",        FOFS( harc ),         F_FLOAT},
	{"varc",        FOFS( varc ),         F_FLOAT},
	// done.

	// Rafael - sniper
	{"delay",   FOFS( delay ),        F_FLOAT},
	{"radius",  FOFS( radius ),       F_INT},

	// Ridah, for reloading savegames at correct mission spot
	{"missionlevel",    FOFS( missionLevel ), F_INT},

	// Rafel
	{"start_size", FOFS( start_size ), F_INT},
	{"end_size", FOFS( end_size ), F_INT},

	{"shard", FOFS( count ), F_INT},

	// Rafael
	{"spawnitem",       FOFS( spawnitem ),            F_LSTRING},

	{"track",           FOFS( track ),                F_LSTRING},

	{"scriptName",      FOFS( scriptName ),           F_LSTRING},

	{NULL}
};

typedef struct {
	char    *name;
	int hash;
	void ( *spawn )( gentity_t *ent );
} spawn_t;

void SP_info_player_start( gentity_t *ent );
void SP_info_player_checkpoint( gentity_t *ent );
void SP_info_player_deathmatch( gentity_t *ent );
void SP_info_player_intermission( gentity_t *ent );
void SP_info_firstplace( gentity_t *ent );
void SP_info_secondplace( gentity_t *ent );
void SP_info_thirdplace( gentity_t *ent );
void SP_info_podium( gentity_t *ent );

void SP_func_plat( gentity_t *ent );
void SP_func_static( gentity_t *ent );
void SP_func_leaky( gentity_t *ent ); //----(SA)	added
void SP_func_rotating( gentity_t *ent );
void SP_func_bobbing( gentity_t *ent );
void SP_func_pendulum( gentity_t *ent );
void SP_func_button( gentity_t *ent );
void SP_func_explosive( gentity_t *ent );
void SP_func_door( gentity_t *ent );
void SP_func_train( gentity_t *ent );
void SP_func_timer( gentity_t *self );
// JOSEPH 1-26-00
void SP_func_train_rotating( gentity_t *ent );
void SP_func_secret( gentity_t *ent );
// END JOSEPH
// Rafael
void SP_func_door_rotating( gentity_t *ent );
// RF
void SP_func_bats( gentity_t *self );

void SP_trigger_always( gentity_t *ent );
void SP_trigger_multiple( gentity_t *ent );
void SP_trigger_push( gentity_t *ent );
void SP_trigger_teleport( gentity_t *ent );
void SP_trigger_hurt( gentity_t *ent );

//---- (SA) Wolf triggers
void SP_trigger_concussive_dust( gentity_t *ent ); // JPW NERVE
void SP_trigger_once( gentity_t *ent );
//---- done

void SP_target_remove_powerups( gentity_t *ent );
void SP_target_give( gentity_t *ent );
void SP_target_delay( gentity_t *ent );
void SP_target_speaker( gentity_t *ent );
void SP_target_print( gentity_t *ent );
void SP_target_laser( gentity_t *self );
void SP_target_character( gentity_t *ent );
void SP_target_score( gentity_t *ent );
void SP_target_teleporter( gentity_t *ent );
void SP_target_relay( gentity_t *ent );
void SP_target_kill( gentity_t *ent );
void SP_target_position( gentity_t *ent );
void SP_target_location( gentity_t *ent );
void SP_target_push( gentity_t *ent );
void SP_target_script_trigger( gentity_t *ent );

//---- (SA) Wolf targets
// targets
void SP_target_alarm( gentity_t *ent );
void SP_target_counter( gentity_t *ent );
void SP_target_lock( gentity_t *ent );
void SP_target_effect( gentity_t *ent );
void SP_target_fog( gentity_t *ent );
void SP_target_autosave( gentity_t *ent );

// entity visibility dummy
void SP_misc_vis_dummy( gentity_t *ent );
void SP_misc_vis_dummy_multiple( gentity_t *ent );

//----(SA) done

void SP_light( gentity_t *self );
void SP_info_null( gentity_t *self );
void SP_info_notnull( gentity_t *self );
void SP_info_camp( gentity_t *self );
void SP_path_corner( gentity_t *self );

void SP_misc_teleporter_dest( gentity_t *self );
void SP_misc_model( gentity_t *ent );
void SP_misc_gamemodel( gentity_t *ent );
void SP_misc_portal_camera( gentity_t *ent );
void SP_misc_portal_surface( gentity_t *ent );
void SP_misc_light_surface( gentity_t *ent );
void SP_misc_grabber_trap( gentity_t *ent );
void SP_misc_spotlight( gentity_t *ent ); //----(SA)	added

void SP_shooter_rocket( gentity_t *ent );
void SP_shooter_grenade( gentity_t *ent );

void SP_team_CTF_redplayer( gentity_t *ent );
void SP_team_CTF_blueplayer( gentity_t *ent );

void SP_team_CTF_redspawn( gentity_t *ent );
void SP_team_CTF_bluespawn( gentity_t *ent );

// JPW NERVE for multiplayer spawnpoint selection
void SP_team_WOLF_objective( gentity_t *ent );
// jpw

void SP_team_WOLF_checkpoint( gentity_t *ent );     // DHM - Nerve

// JOSEPH 1-18-00
void SP_props_box_32( gentity_t *self );
void SP_props_box_48( gentity_t *self );
void SP_props_box_64( gentity_t *self );
// END JOSEPH

// Rafael particles
void SP_Snow( gentity_t *ent );
void SP_target_smoke( gentity_t *ent );
void SP_Bubbles( gentity_t *ent );
// done.

// (SA) dlights
void SP_dlight( gentity_t *ent );
// done
void SP_corona( gentity_t *ent );

// Rafael mg42
void SP_mg42( gentity_t *ent );
// done.

// Rafael sniper
void SP_shooter_sniper( gentity_t *ent );
void SP_sniper_brush( gentity_t *ent );
// done

//----(SA)
void SP_shooter_zombiespit( gentity_t *ent );
void SP_shooter_mortar( gentity_t *ent );
void SP_shooter_tesla( gentity_t *ent );

// alarm
void SP_alarm_box( gentity_t *ent );
//----(SA)	end

void SP_trigger_flagonly( gentity_t *ent );     // DHM - Nerve
void SP_trigger_objective_info( gentity_t *ent );   // DHM - Nerve

void SP_gas( gentity_t *ent );
void SP_target_rumble( gentity_t *ent );
void SP_func_train_particles( gentity_t *ent );

// Rafael
void SP_trigger_aidoor( gentity_t *ent );
void SP_SmokeDust( gentity_t *ent );
void SP_Dust( gentity_t *ent );
void SP_props_sparks( gentity_t *ent );
void SP_props_gunsparks( gentity_t *ent );

// Props
void SP_Props_Bench( gentity_t *ent );
void SP_Props_Radio( gentity_t *ent );
void SP_Props_Chair( gentity_t *ent );
void SP_Props_ChairHiback( gentity_t *ent );
void SP_Props_ChairSide( gentity_t *ent );
void SP_Props_ChairChat( gentity_t *ent );
void SP_Props_ChairChatArm( gentity_t *ent );
void SP_Props_DamageInflictor( gentity_t *ent );
void SP_Props_Locker_Tall( gentity_t *ent );
void SP_Props_Desklamp( gentity_t *ent );
void SP_Props_Flamebarrel( gentity_t *ent );
void SP_crate_64( gentity_t *ent );
void SP_Props_Flipping_Table( gentity_t *ent );
void SP_crate_32( gentity_t *self );
void SP_Props_Crate32x64( gentity_t *ent );
void SP_Props_58x112tablew( gentity_t *ent );
void SP_props_castlebed( gentity_t *ent );
void SP_Props_RadioSEVEN( gentity_t *ent );
void SP_propsFireColumn( gentity_t *ent );
void SP_props_flamethrower( gentity_t *ent );

void SP_func_tramcar( gentity_t *ent );
void func_invisible_user( gentity_t *ent );

void SP_lightJunior( gentity_t *self );

void SP_props_me109( gentity_t *ent );
void SP_misc_flak( gentity_t *ent );
void SP_plane_waypoint( gentity_t *self );

void SP_props_snowGenerator( gentity_t *ent );
void SP_truck_cam( gentity_t *self );

void SP_screen_fade( gentity_t *ent );
void SP_camera_reset_player( gentity_t *ent );
void SP_camera_cam( gentity_t *ent );
void SP_props_decoration( gentity_t *ent );
void SP_props_decorBRUSH( gentity_t *ent );
void SP_props_statue( gentity_t *ent );
void SP_props_statueBRUSH( gentity_t *ent );
void SP_skyportal( gentity_t *ent );

// RF, scripting
void SP_script_model_med( gentity_t *ent );
void SP_script_mover( gentity_t *ent );
void SP_script_multiplayer( gentity_t *ent );         // DHM - Nerve

void SP_props_footlocker( gentity_t *self );
void SP_misc_firetrails( gentity_t *ent );
void SP_trigger_deathCheck( gentity_t *ent );
void SP_misc_spawner( gentity_t *ent );
void SP_props_decor_Scale( gentity_t *ent );

spawn_t spawns[] = {
	/* classname					hash		spawn function                  */
	/*--------------------------------------------------------------------------*/
	{"info_player_start",           232439,     SP_info_player_start},
	{"info_player_checkpoint",      302525,     SP_info_player_checkpoint},
	{"info_player_deathmatch",      299584,     SP_info_player_deathmatch},
	{"info_player_intermission",    337887,     SP_info_player_intermission},
	{"info_null",                   118853,     SP_info_null},
	{"info_notnull",                162313,     SP_info_notnull},
	{"info_camp",                   115623,     SP_info_camp},
	{"func_plat",                   117593,     SP_func_plat},
	{"func_button",                 147774,     SP_func_button},
	{"func_explosive",              190086,     SP_func_explosive},
	{"func_door",                   117990,     SP_func_door},
	{"func_static",                 145176,     SP_func_static},
	{"func_leaky",                  130567,     SP_func_leaky},
	{"func_rotating",               174383,     SP_func_rotating},
	{"func_bobbing",                155092,     SP_func_bobbing},
	{"func_pendulum",               174721,     SP_func_pendulum},
	{"func_train",                  131522,     SP_func_train},
	{"func_group",                  133454,     SP_info_null},
	{"func_train_rotating",         260141,     SP_func_train_rotating},
	{"func_secret",                 144980,     SP_func_secret},
	{"func_door_rotating",          245642,     SP_func_door_rotating},
	{"func_train_particles",        273350,     SP_func_train_particles},
	{"func_timer",                  131913,     SP_func_timer},
	{"func_tramcar",                157955,     SP_func_tramcar},
	{"func_invisible_user",         259901,     func_invisible_user},
	{"func_bats",                   116749,     SP_func_bats},
	{"trigger_always",              189303,     SP_trigger_always},
	{"trigger_multiple",            218436,     SP_trigger_multiple},
	{"trigger_push",                161723,     SP_trigger_push},
	{"trigger_teleport",            218920,     SP_trigger_teleport},
	{"trigger_hurt",                162138,     SP_trigger_hurt},
	{"trigger_concussive_dust",     323081,     SP_trigger_concussive_dust},
	{"trigger_once",                158246,     SP_trigger_once},
	{"trigger_aidoor",              186846,     SP_trigger_aidoor},
	{"trigger_deathCheck",          239364,     SP_trigger_deathCheck},
	{"target_give",                 144932,     SP_target_give},
	{"target_remove_powerups",      310062,     SP_target_remove_powerups},
	{"target_delay",                157980,     SP_target_delay},
	{"target_speaker",              186830,     SP_target_speaker},
	{"target_print",                161786,     SP_target_print},
	{"target_laser",                158982,     SP_target_laser},
	{"target_score",                159593,     SP_target_score},
	{"target_teleporter",           233283,     SP_target_teleporter},
	{"target_relay",                159744,     SP_target_relay},
	{"target_kill",                 145059,     SP_target_kill},
	{"target_position",             205077,     SP_target_position},
	{"target_location",             201493,     SP_target_location},
	{"target_push",                 147593,     SP_target_push},
	{"target_script_trigger",       290764,     SP_target_script_trigger},
	{"target_alarm",                157716,     SP_target_alarm},
	{"target_counter",              189582,     SP_target_counter},
	{"target_lock",                 144666,     SP_target_lock},
	{"target_effect",               170317,     SP_target_effect},
	{"target_fog",                  130619,     SP_target_fog},
	{"target_autosave",             203400,     SP_target_autosave},
	{"target_rumble",               173574,     SP_target_rumble},
	{"light",                       64871,      SP_light},
	{"lightJunior",                 148749,     SP_lightJunior},
	{"path_corner",                 145496,     SP_path_corner},
	{"misc_teleporter_dest",        275587,     SP_misc_teleporter_dest},
	{"misc_model",                  129891,     SP_misc_model},
	{"misc_gamemodel",              183465,     SP_misc_gamemodel},
	{"misc_portal_surface",         258541,     SP_misc_portal_surface},
	{"misc_portal_camera",          241192,     SP_misc_portal_camera},
	{"misc_vis_dummy",              189875,     SP_misc_vis_dummy},
	{"misc_vis_dummy_multiple",     322910,     SP_misc_vis_dummy_multiple},
	{"misc_light_surface",          242045,     SP_misc_light_surface},
	{"misc_grabber_trap",           226369,     SP_misc_grabber_trap},
	{"misc_spotlight",              189922,     SP_misc_spotlight},
	{"misc_mg42",                   102542,     SP_mg42},
	{"misc_flak",                   115208,     SP_misc_flak},
	{"misc_firetrails",             201683,     SP_misc_firetrails},
	{"shooter_rocket",              190060,     SP_shooter_rocket},
	{"shooter_grenade",             200492,     SP_shooter_grenade},
	{"shooter_zombiespit",          249994,     SP_shooter_zombiespit},
	{"shooter_mortar",              191742,     SP_shooter_mortar},
	{"shooter_tesla",               175392,     SP_shooter_tesla},
	{"alarm_box",                   116811,     SP_alarm_box},
	{"shooter_sniper",              191219,     SP_shooter_sniper},
	{"sniper_brush",                161845,     SP_sniper_brush},
	{"team_CTF_redplayer",          242140,     SP_team_CTF_redplayer},
	{"team_CTF_blueplayer",         257089,     SP_team_CTF_blueplayer},
	{"team_CTF_redspawn",           228504,     SP_team_CTF_redspawn},
	{"team_CTF_bluespawn",          243353,     SP_team_CTF_bluespawn},
	{"team_WOLF_objective",         257045,     SP_team_WOLF_objective},
	{"team_WOLF_checkpoint",        272169,     SP_team_WOLF_checkpoint},
	{"misc_snow256",                140615,     SP_Snow},
	{"misc_snow128",                140360,     SP_Snow},
	{"misc_snow64",                 133978,     SP_Snow},
	{"misc_snow32",                 133336,     SP_Snow},
	{"target_smoke",                159960,     SP_target_smoke},
	{"misc_bubbles8",               163959,     SP_Bubbles},
	{"misc_bubbles16",              170170,     SP_Bubbles},
	{"misc_bubbles32",              169904,     SP_Bubbles},
	{"misc_bubbles64",              170561,     SP_Bubbles},
	{"misc_spawner",                160773,     SP_misc_spawner},
	{"props_box_32",                146743,     SP_props_box_32},
	{"props_box_48",                147652,     SP_props_box_48},
	{"props_box_64",                147390,     SP_props_box_64},
	{"props_smokedust",             207894,     SP_SmokeDust},
	{"props_dust",                  136723,     SP_Dust},
	{"props_sparks",                164179,     SP_props_sparks},
	{"props_gunsparks",             207746,     SP_props_gunsparks},
	{"plane_waypoint",              190139,     SP_plane_waypoint},
	{"props_me109",                 126099,     SP_props_me109},
	{"props_bench",                 145062,     SP_Props_Bench},
	{"props_radio",                 146959,     SP_Props_Radio},
	{"props_chair",                 145972,     SP_Props_Chair},
	{"props_chair_hiback",          239755,     SP_Props_ChairHiback},
	{"props_chair_side",            214081,     SP_Props_ChairSide},
	{"props_chair_chat",            213464,     SP_Props_ChairChat},
	{"props_chair_chatarm",         256996,     SP_Props_ChairChatArm},
	{"props_damageinflictor",       288434,     SP_Props_DamageInflictor},
	{"props_locker_tall",           231342,     SP_Props_Locker_Tall},
	{"props_desklamp",              189160,     SP_Props_Desklamp},
	{"props_flamebarrel",           229457,     SP_Props_Flamebarrel},
	{"props_crate_64",              173251,     SP_crate_64},
	{"props_flippy_table",          246341,     SP_Props_Flipping_Table},
	{"props_crate_32",              172594,     SP_crate_32},
	{"props_crate_32x64",           202810,     SP_Props_Crate32x64},
	{"props_58x112tablew",          213367,     SP_Props_58x112tablew},
	{"props_castlebed",             200610,     SP_props_castlebed},
	{"props_radioSEVEN",            218889,     SP_Props_RadioSEVEN},
	{"props_snowGenerator",         266285,     SP_props_snowGenerator},
	{"props_FireColumn",            219444,     SP_propsFireColumn},
	{"props_decoration",            217910,     SP_props_decoration},
	{"props_decorBRUSH",            219090,     SP_props_decorBRUSH},
	{"props_statue",                164409,     SP_props_statue},
	{"props_statueBRUSH",           237306,     SP_props_statueBRUSH},
	{"props_skyportal",             209091,     SP_skyportal},
	{"props_footlocker",            219873,     SP_props_footlocker},
	{"props_flamethrower",          249286,     SP_props_flamethrower},
	{"props_decoration_scale",      302476,     SP_props_decor_Scale},
	{"truck_cam",                   117100,     SP_truck_cam},
	{"screen_fade",                 140619,     SP_screen_fade},
	{"camera_reset_player",         257157,     SP_camera_reset_player},
	{"camera_cam",                  125602,     SP_camera_cam},
	{"dlight",                      77307,      SP_dlight},
	{"corona",                      77995,      SP_corona},
	{"test_gas",                    105063,     SP_gas},
	{"trigger_flagonly",            216488,     SP_trigger_flagonly},
	{"trigger_objective_info",      301509,     SP_trigger_objective_info},
	{"script_model_med",            213570,     SP_script_model_med},
	{"script_mover",                162988,     SP_script_mover},
	{"script_multiplayer",          250417,     SP_script_multiplayer},

	{0,0,0},
};

/*
===============
G_CallSpawn

Finds the spawn function for the entity and calls it,
returning qfalse if not found
===============
*/
qboolean G_CallSpawn( gentity_t *ent ) {
	spawn_t *s;
	gitem_t *item;
	int hash;

	if ( !ent->classname ) {
		G_Printf( "G_CallSpawn: NULL classname\n" );
		return qfalse;
	}

	if ( g_deathmatch.integer ) {
		if ( !strcmp( "func_explosive", ent->classname ) ) {
			return qfalse;
		}

		if ( !strcmp( "trigger_hurt", ent->classname ) ) {
			return qfalse;
		}

		// don't spawn the flags in cp
		if ( g_gametype.integer == 7 && !strcmp( "team_WOLF_checkpoint", ent->classname ) ) {
			return qfalse;
		}
	}

	// check item spawn functions
	for ( item = bg_itemlist + 1 ; item->classname ; item++ ) {
		if ( !strcmp( item->classname, ent->classname ) ) {
			// found it
			// DHM - Nerve :: allow flags in GTWOLF
			if ( item->giType == IT_TEAM && ( g_gametype.integer != GT_CTF && g_gametype.integer < GT_WOLF ) ) {
				return qfalse;
			}
			G_SpawnItem( ent, item );
			return qtrue;
		}
	}

	// check normal spawn functions
	hash = BG_StringHashValue( ent->classname );
	for ( s = spawns ; s->name ; s++ ) {
		if ( s->hash == hash ) {
			// found it
			s->spawn( ent );

			// RF, entity scripting
			if ( ent->s.number >= MAX_CLIENTS && ent->scriptName ) {
				G_Script_ScriptParse( ent );
				G_Script_ScriptEvent( ent, "spawn", "" );
			}

			return qtrue;
		}
	}
	G_Printf( "%s doesn't have a spawn function\n", ent->classname );
	return qfalse;
}

/*
=============
G_NewString

Builds a copy of the string, translating \n to real linefeeds
so message texts can be multi-line
=============
*/
char *G_NewString( const char *string ) {
	char    *newb, *new_p;
	int i,l;

	l = strlen( string ) + 1;

	newb = G_Alloc( l );

	new_p = newb;

	// turn \n into a real linefeed
	for ( i = 0 ; i < l ; i++ ) {
		if ( string[i] == '\\' && i < l - 1 ) {
			i++;
			if ( string[i] == 'n' ) {
				*new_p++ = '\n';
			} else {
				*new_p++ = '\\';
			}
		} else {
			*new_p++ = string[i];
		}
	}

	return newb;
}

/*
===============
G_ParseField

Takes a key/value pair and sets the binary values
in a gentity
===============
*/
void G_ParseField( const char *key, const char *value, gentity_t *ent ) {
	field_t *f;
	byte    *b;
	float v;
	vec3_t vec;

	for ( f = fields ; f->name ; f++ ) {
		if ( !Q_stricmp( f->name, key ) ) {
			// found it
			b = (byte *)ent;

			switch ( f->type ) {
			case F_LSTRING:
				*(char **)( b + f->ofs ) = G_NewString( value );
				break;
			case F_VECTOR:
				sscanf( value, "%f %f %f", &vec[0], &vec[1], &vec[2] );
				( (float *)( b + f->ofs ) )[0] = vec[0];
				( (float *)( b + f->ofs ) )[1] = vec[1];
				( (float *)( b + f->ofs ) )[2] = vec[2];
				break;
			case F_INT:
				*(int *)( b + f->ofs ) = atoi( value );
				break;
			case F_FLOAT:
				*(float *)( b + f->ofs ) = atof( value );
				break;
			case F_ANGLEHACK:
				v = atof( value );
				( (float *)( b + f->ofs ) )[0] = 0;
				( (float *)( b + f->ofs ) )[1] = v;
				( (float *)( b + f->ofs ) )[2] = 0;
				break;
			default:
			case F_IGNORE:
				break;
			}
			return;
		}
	}
}

/*
===================
G_SpawnGEntityFromSpawnVars

Spawn an entity and fill in all of the level fields from
level.spawnVars[], then call the class specfic spawn function
===================
*/
void G_SpawnGEntityFromSpawnVars( void ) {
	int i;
	gentity_t   *ent;

	// get the next free entity
	ent = G_Spawn();

	for ( i = 0 ; i < level.numSpawnVars ; i++ ) {
		G_ParseField( level.spawnVars[i][0], level.spawnVars[i][1], ent );
	}

	if ( g_gametype.integer >= GT_TEAM ) {
		G_SpawnInt( "notteam", "0", &i );
		if ( i ) {
			G_FreeEntity( ent );
			return;
		}
	} else {
		G_SpawnInt( "notfree", "0", &i );
		if ( i ) {
			G_FreeEntity( ent );
			return;
		}
	}

	if ( ent->targetname && *ent->targetname ) {
		ent->targetnamehash = BG_StringHashValue( ent->targetname );
	} else {
		ent->targetnamehash = -1;
	}

	// move editor origin to pos
	VectorCopy( ent->s.origin, ent->s.pos.trBase );
	VectorCopy( ent->s.origin, ent->r.currentOrigin );

	// if we didn't get a classname, don't bother spawning anything
	if ( !G_CallSpawn( ent ) ) {
		G_FreeEntity( ent );
	}
}

/*
====================
G_AddSpawnVarToken
====================
*/
char *G_AddSpawnVarToken( const char *string ) {
	int l;
	char    *dest;

	l = strlen( string );
	if ( level.numSpawnVarChars + l + 1 > MAX_SPAWN_VARS_CHARS ) {
		G_Error( "G_AddSpawnVarToken: MAX_SPAWN_VARS" );
	}

	dest = level.spawnVarChars + level.numSpawnVarChars;
	memcpy( dest, string, l + 1 );

	level.numSpawnVarChars += l + 1;

	return dest;
}

/*
====================
G_ParseSpawnVars

Parses a brace bounded set of key / value pairs out of the
level's entity strings into level.spawnVars[]

This does not actually spawn an entity.
====================
*/
qboolean G_ParseSpawnVars( void ) {
	char keyname[MAX_TOKEN_CHARS];
	char com_token[MAX_TOKEN_CHARS];

	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;

	// parse the opening brace
	if ( !trap_GetEntityToken( com_token, sizeof( com_token ) ) ) {
		// end of spawn string
		return qfalse;
	}
	if ( com_token[0] != '{' ) {
		G_Error( "G_ParseSpawnVars: found %s when expecting {",com_token );
	}

	// go through all the key / value pairs
	while ( 1 ) {
		// parse key
		if ( !trap_GetEntityToken( keyname, sizeof( keyname ) ) ) {
			G_Error( "G_ParseSpawnVars: EOF without closing brace" );
		}

		if ( keyname[0] == '}' ) {
			break;
		}

		// parse value
		if ( !trap_GetEntityToken( com_token, sizeof( com_token ) ) ) {
			G_Error( "G_ParseSpawnVars: EOF without closing brace" );
		}

		if ( com_token[0] == '}' ) {
			G_Error( "G_ParseSpawnVars: closing brace without data" );
		}
		if ( level.numSpawnVars == MAX_SPAWN_VARS ) {
			G_Error( "G_ParseSpawnVars: MAX_SPAWN_VARS" );
		}
		level.spawnVars[ level.numSpawnVars ][0] = G_AddSpawnVarToken( keyname );
		level.spawnVars[ level.numSpawnVars ][1] = G_AddSpawnVarToken( com_token );
		level.numSpawnVars++;
	}

	return qtrue;
}

qboolean G_SpawnStringExt( const char *key, const char *defaultString, char **out, const char* file, int line ) {
	int i;

	if ( !level.spawning ) {
		*out = (char *)defaultString;
		// Gordon: 26/11/02: re-enabling
		// see InitMover
		G_Error( "G_SpawnString() called while not spawning, file %s, line %i", file, line );
	}

	for ( i = 0 ; i < level.numSpawnVars ; i++ ) {
		if ( !strcmp( key, level.spawnVars[i][0] ) ) {
			*out = level.spawnVars[i][1];
			return qtrue;
		}
	}

	*out = (char *)defaultString;
	return qfalse;
}

qboolean    G_SpawnVector2DExt( const char *key, const char *defaultString, float *out, const char* file, int line ) {
	char        *s;
	qboolean present;

	present = G_SpawnStringExt( key, defaultString, &s, file, line );
	sscanf( s, "%f %f", &out[0], &out[1] );
	return present;
}

/*QUAKED worldspawn (0 0 0) ? NO_GT_WOLF NO_STOPWATCH NO_CHECKPOINT

Every map should have exactly one worldspawn.
"music"     Music wav file
"gravity"   800 is default gravity
"message" Text to print during connection process
"ambient"  Ambient light value (must use '_color')
"_color"    Ambient light color (must be used with 'ambient')
"sun"        Shader to use for 'sun' image
*/
void SP_worldspawn( void ) {
	char    *s;
	gitem_t *item; // JPW NERVE

	G_SpawnString( "classname", "", &s );
	if ( Q_stricmp( s, "worldspawn" ) ) {
		G_Error( "SP_worldspawn: The first entity isn't 'worldspawn'" );
	}

	// make some data visible to connecting client
	trap_SetConfigstring( CS_GAME_VERSION, GAME_VERSION );

	trap_SetConfigstring( CS_LEVEL_START_TIME, va( "%i", level.startTime ) );

	G_SpawnString( "music", "", &s );
	trap_SetConfigstring( CS_MUSIC, s );

	G_SpawnString( "message", "", &s );
	trap_SetConfigstring( CS_MESSAGE, s );              // map specific message

	trap_SetConfigstring( CS_MOTD, g_motd.string );     // message of the day

	G_SpawnString( "gravity", "800", &s );
	trap_Cvar_Set( "g_gravity", s );

	G_SpawnString( "spawnflags", "0", &s );
	g_entities[ENTITYNUM_WORLD].spawnflags = atoi( s );
	g_entities[ENTITYNUM_WORLD].r.worldflags = g_entities[ENTITYNUM_WORLD].spawnflags;

	g_entities[ENTITYNUM_WORLD].s.number = ENTITYNUM_WORLD;
	g_entities[ENTITYNUM_WORLD].classname = "worldspawn";

	// see if we want a warmup time
	trap_SetConfigstring( CS_WARMUP, "" );
	if ( g_restarted.integer ) {
		trap_Cvar_Set( "g_restarted", "0" );
		level.warmupTime = 0;
	}

	// JPW NERVE change minigun overheat time for single player -- this array gets reloaded every time the server is reset,
	// so this is as good a place as any to do stuff like this
	{
		int i;
		ammoTable[WP_VENOM].maxHeat *= 0.25;
		for ( i = 0; i < strlen( testid2 ); i++ )
			testid2[i] -= ( i + 1 );
		ammoTable[WP_DYNAMITE].uses = 0; // regens based on recharge time
		// reset ammo for subs to be distinct for multiplayer (so running out of rifle ammo doesn't deplete sidearm)
		// if player runs out of SMG ammunition, it shouldn't *also* deplete pistol ammunition.  If you change this, change
		// g_spawn.c as well
		item = BG_FindItemByIndex( IL_WEAPON_THOMPSON );
		item->giAmmoIndex = WP_THOMPSON;

		item = BG_FindItemByIndex( IL_WEAPON_STEN );
		item->giAmmoIndex = WP_STEN;

		for ( i = 0; i < strlen( testid1 ); i++ )
			testid1[i] -= ( i + 1 );

		item = BG_FindItemByIndex( IL_WEAPON_MP40 );
		item->giAmmoIndex = WP_MP40;

		ammoTable[WP_VENOM_FULL].nextShotTime = 500;
		for ( i = 0; i < strlen( testid3 ); i++ )
			testid3[i] -= ( i + 1 );

		ammoTable[WP_PANZERFAUST].fireDelayTime = 750;
		item = BG_FindItemByIndex( IL_WEAPON_PANZERFAUST );
		item->world_model[4] = "models/multiplayer/panzerfaust/multi_pf.md3";
	}
}

/*
==============
G_SpawnEntitiesFromString

Parses textual entity definitions out of an entstring and spawns gentities.
==============
*/
void G_SpawnEntitiesFromString( void ) {
	// allow calls to G_Spawn*()
	level.spawning = qtrue;
	level.numSpawnVars = 0;

	// the worldspawn is not an actual entity, but it still
	// has a "spawn" function to perform any global setup
	// needed by a level (setting configstrings or cvars, etc)
	if ( !G_ParseSpawnVars() ) {
		G_Error( "SpawnEntities: no entities" );
	}
	SP_worldspawn();

	// parse ents
	while ( G_ParseSpawnVars() ) {
		G_SpawnGEntityFromSpawnVars();
	}

	level.spawning = qfalse;            // any future calls to G_Spawn*() will be errors
}