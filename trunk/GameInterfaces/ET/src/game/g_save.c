/*
 * name:		g_save.c
 *
 */

#ifdef SAVEGAME_SUPPORT

#include "../game/g_local.h"
#include "../game/q_shared.h"
#include "../game/botlib.h"		//bot lib interface
#include "../game/be_aas.h"
#include "../game/be_ea.h"
#include "../game/be_ai_gen.h"
#include "../game/be_ai_goal.h"
#include "../game/be_ai_move.h"
#include "../botai/botai.h"			//bot ai interface
#include "../botai/ai_main.h"
#include "../botai/chars.h"
#include "../botai/ai_team.h"
#include "../botai/	ai_dmq3.h"
#include "../game/be_ai_chat.h"

extern bot_waypoint_t botai_waypoints[MAX_BOTAIWAYPOINTS];
extern bot_waypoint_t *botai_freewaypoints;

/*
Wolf savegame system.

Using the "checkpoint" system, we only need to save at specific locations, but various entities
may have changed behind us, so therefore we need to save as much as possible, but without going
overboard.

For now, everything is saved from the entity, client and cast_state structures, except the fields
defined in the ignoreField structures below. Any pointer fields need to be specified in the
saveField structures below.

!! NOTE: when working on Wolf patches, make sure you only add fields to the very end of the three
  main structures saved here (entity, client, cast_state). If any fields are inserted in the middle
  of these structures, savegames will become corrupted, and there is no way of checking for this,
  so it'll just crash.
*/

//vmCvar_t	musicCvar;
//char		musicString[MAX_STRING_CHARS];

static int ver;

typedef enum {
	F_NONE,
	F_STRING,
	F_ENTITY,			// index on disk, pointer in memory
	F_ITEM,				// index on disk, pointer in memory
	F_CLIENT,			// index on disk, pointer in memory
	F_FUNCTION,
	F_SPLINE,			// index on disk, pointer in memory
	F_AIFUNCTION,
	F_BOTWAYPOINT,		// index on disk, pointer in memory
	F_BOTSCRIPTDATA,	// index on disk, pointer in memory
} saveFieldtype_t;

typedef struct {
	int		ofs;
	saveFieldtype_t	type;
} saveField_t;

//.......................................................................................
// these are the fields that cannot be saved directly, so they need to be converted
static saveField_t gentityFields[] = {
	{FOFS(client),		F_CLIENT},
	{FOFS(classname),	F_STRING},
	{FOFS(model),		F_STRING},
	{FOFS(model2),		F_STRING},
	{FOFS(parent),		F_ENTITY},
	{FOFS(nextTrain),	F_ENTITY},
	{FOFS(prevTrain),	F_ENTITY},
	{FOFS(message),		F_STRING},
	{FOFS(target),		F_STRING},
	{FOFS(targetname),	F_STRING},
	{FOFS(team),		F_STRING},
	{FOFS(targetShaderName),	F_STRING},
	{FOFS(targetShaderNewName),	F_STRING},	
	{FOFS(target_ent),	F_ENTITY},
	{FOFS(think),		F_FUNCTION},
	{FOFS(reached),		F_FUNCTION},
	{FOFS(blocked),		F_FUNCTION},
	{FOFS(touch),		F_FUNCTION},
	{FOFS(use),			F_FUNCTION},
	{FOFS(pain),		F_FUNCTION},
	{FOFS(die),			F_FUNCTION},
	{FOFS(chain),		F_ENTITY},
	{FOFS(enemy),		F_ENTITY},
	{FOFS(activator),	F_ENTITY},
	{FOFS(teamchain),	F_ENTITY},
	{FOFS(teammaster),	F_ENTITY},
	{FOFS(item),		F_ITEM},
	{FOFS(aiName),		F_STRING},
	{FOFS(AIScript_AlertEntity), F_FUNCTION},
	{FOFS(aiSkin),		F_STRING},
	{FOFS(dl_stylestring),	F_STRING},
	{FOFS(dl_shader),	F_STRING},
	{FOFS(melee),		F_ENTITY},
	{FOFS(spawnitem),	F_STRING},
	{FOFS(track),		F_STRING},
	{FOFS(scriptName),	F_STRING},
	{FOFS(tagName),		F_STRING},
	{FOFS(tagParent),	F_ENTITY},
	{FOFS(constages),	F_STRING},
	{FOFS(desstages),	F_STRING},
	{FOFS(botNextStaticEntity),	F_ENTITY},
	{FOFS(backspline),	F_SPLINE},

	{0, 0}
};

static saveField_t gclientFields[] = {
	{CFOFS(hook),				F_ENTITY},
	{CFOFS(persistantPowerup),	F_ENTITY},
	{CFOFS(cameraPortal),		F_ENTITY},
	{CFOFS(touchingTOI),		F_ENTITY},
	{CFOFS(landmineSpotted),	F_ENTITY},
	{CFOFS(tempHead),			F_ENTITY},
	{CFOFS(tempLeg),			F_ENTITY},

	{0, 0}
};

static saveField_t botwaypointFields[] = {
	{BWPOFS(next),			F_BOTWAYPOINT},
	{BWPOFS(prev),			F_BOTWAYPOINT},

	{0, 0}
};

static saveField_t botstateFields[] = {
	{BFOFS(ainode),			F_AIFUNCTION},
	{BFOFS(ainodeText),		F_STRING},
	{BFOFS(checkpoints),	F_BOTWAYPOINT},		// for waypoints we need to store the botai_waypoints seperately
	{BFOFS(patrolpoints),	F_BOTWAYPOINT},
	{BFOFS(curpatrolpoint),	F_BOTWAYPOINT},
	{BFOFS(script.data),	F_BOTSCRIPTDATA},
	{BFOFS(indicator),		F_ENTITY},

	{0, 0}
};

//.......................................................................................
// this is where we define fields or sections of structures that we should totally ignore
typedef struct {
	int		ofs;
	int		len;
} ignoreField_t;

static ignoreField_t gentityIgnoreFields[] = {
	// don't process events that have already occured before the game was saved
	//{FOFS(s.events[0]),		sizeof(int) * MAX_EVENTS},
	//{FOFS(s.eventParms[0]),	sizeof(int) * MAX_EVENTS},
	//{FOFS(s.eventSequence),	sizeof(int)},

	{FOFS(numScriptEvents),	sizeof(int)},
	{FOFS(scriptEvents),	sizeof(g_script_event_t *) },	// gets created upon parsing the script file, this is static while playing

	{0, 0}
};

static ignoreField_t gclientIgnoreFields[] = {
	// don't process events that have already occured before the game was saved
	//{CFOFS(ps.events[0]),		sizeof(int) * MAX_EVENTS},
	//{CFOFS(ps.eventParms[0]),	sizeof(int) * MAX_EVENTS},
	//{CFOFS(ps.eventSequence),	sizeof(int)},
	//{CFOFS(ps.oldEventSequence),sizeof(int)},

	{0, 0}
};

static ignoreField_t botstateIgnoreFields[] = {
	{BFOFS(objectiveMatrix),	sizeof(botMatrix_t *)},		// these are based on the playerclass stored in the botstate and further static
	{BFOFS(behaviourMatrix),	sizeof(botMatrix_t *)},
	{BFOFS(engagementMatrix),	sizeof(botMatrix_t *)},
	{BFOFS(character),			sizeof(int)},				// these are all returned from the botlib
	{BFOFS(gs),					sizeof(int)},
	{BFOFS(ws),					sizeof(int)},
	{BFOFS(cs),					sizeof(int)},
	{BFOFS(ms),					sizeof(int)},
	{BFOFS(attribs),			sizeof(float)*BOT_MAX_ATTRIBUTES},

	{0, 0}
};

//.......................................................................................
// persistant data is optionally carried across level changes
// !! WARNING: cannot save pointer or string variables
typedef struct {
	int		ofs;
	int		len;
} persField_t;

static persField_t gentityPersFields[] = {
	{FOFS(health),				sizeof(int)},
	{0, 0}
};

static persField_t gclientPersFields[] = {
	{CFOFS(ps.weapon),			sizeof(int)},
	{CFOFS(ps.ammo[0]),			sizeof(int) * MAX_WEAPONS},
	{CFOFS(ps.ammoclip[0]),		sizeof(int) * MAX_WEAPONS},	//----(SA)	added for ammo in clip
	{CFOFS(ps.persistant[0]),	sizeof(int) * MAX_PERSISTANT},
	{CFOFS(ps.stats[0]),		sizeof(int) * MAX_STATS},
	{CFOFS(ps.weapons[0]),		sizeof(int) * MAX_WEAPONS/(sizeof(int)*8)},	// //----(SA)	added.  weapons owned got moved outside stats[]
	{CFOFS(ps.powerups[0]),		sizeof(int) * MAX_POWERUPS},
	{CFOFS(ps.holding),			sizeof(int)},					//----(SA)	added
	{0, 0}
};

//.......................................................................................
// this stores all functions in the game code
typedef struct {
	char *funcStr;
	byte *funcPtr;
} funcList_t;

//-----------------
// MSVC likes to needlessly(?) warn about these defines, so disable certain warnings temporarily
#ifdef _WIN32
#pragma warning( push )
#pragma warning( disable : 4054 )
#endif
//-----------------

#include "g_func_decs.h"	// declare all game functions

funcList_t gameFuncList[] = {
	#include "g_funcs.h"
};

#include "../botai/ai_func_decs.h"	// declare all ai functions

funcList_t aiFuncList[] = {
	#include "../botai/ai_funcs.h"
};

//-----------------
#ifdef _WIN32
#pragma warning( pop )	// return previous warning state
#endif
//-----------------


//=========================================================

/*
===============
G_SaveWriteError
===============
*/
void G_SaveWriteError(void) {
// TTimo
#ifdef __linux__
	G_Error( "Unable to save game.\n\nPlease check that you have at least 5mb free of disk space in your home directory.");
#else
	G_Error( "Insufficient free disk space.\n\nPlease free at least 5mb of free space on game drive.");
#endif  
}

static int saveByteCount;

/*
===============
G_SaveWrite

  FS_Write doesnt always accurately return how many bytes were written, so tally them up, and
  check them before we rename to the real file
===============
*/
int G_SaveWrite( const void *buffer, int len, fileHandle_t f ) {
	saveByteCount += len;

	return trap_FS_Write( buffer, len, f );
}

//=========================================================

funcList_t *G_FindFuncAtAddress( byte *adr )
{
	int i;

	for (i=0; gameFuncList[i].funcStr; i++) {
		if (gameFuncList[i].funcPtr == adr) {
			return &gameFuncList[i];
		}
	}
	return NULL;
}

byte *G_FindFuncByName( char *name )
{
	int i;

	for (i=0; gameFuncList[i].funcStr; i++) {
		if (!strcmp(name, gameFuncList[i].funcStr)) {
			return gameFuncList[i].funcPtr;
		}
	}
	return NULL;
}

funcList_t *AI_FindFuncAtAddress( byte *adr )
{
	int i;

	for (i=0; aiFuncList[i].funcStr; i++) {
		if (aiFuncList[i].funcPtr == adr) {
			return &aiFuncList[i];
		}
	}
	return NULL;
}

byte *AI_FindFuncByName( char *name )
{
	int i;

	for (i=0; aiFuncList[i].funcStr; i++) {
		if (!strcmp(name, aiFuncList[i].funcStr)) {
			return aiFuncList[i].funcPtr;
		}
	}
	return NULL;
}

void WriteField1 (saveField_t *field, byte *base)
{
	void		*p;
	int			len;
	int			index;
	funcList_t	*func;

	p = (void *)(base + field->ofs);
	switch (field->type)
	{
	case F_STRING:
		if ( *(char **)p )
			len = strlen(*(char **)p) + 1;
		else
			len = 0;
		*(int *)p = len;
		break;
	case F_ENTITY:
		if ( *(gentity_t **)p == NULL)
			index = -1;
		else
			index = *(gentity_t **)p - g_entities;
		if (index >= MAX_GENTITIES || index < -1)
			G_Error( "WriteField1: entity out of range (%i)", index );
		*(int *)p = index;
		break;
	case F_CLIENT:
		if ( *(gclient_t **)p == NULL)
			index = -1;
		else
			index = *(gclient_t **)p - level.clients;
		if (index >= MAX_CLIENTS || index < -1)
			G_Error( "WriteField1: client out of range (%i)", index );
		*(int *)p = index;
		break;
	case F_ITEM:
		if ( *(gitem_t **)p == NULL)
			index = -1;
		else
			index = *(gitem_t **)p - bg_itemlist;
		*(int *)p = index;
		break;

	//	match this with a function address in the function list, which is built using the
	//	"extractfuncs.bat" in the utils folder. We then save the string equivalent
	//	of the function. This effectively gives us cross-version save games.
	case F_FUNCTION:
		if (*(byte **)p == NULL)
			len = 0;
		else {
			func = G_FindFuncAtAddress( *(byte **)p );
			if (!func) {
				G_Error ("WriteField1: unknown function, cannot save game");
			}
			len = strlen(func->funcStr) + 1;
		}
		*(int *)p = len;
		break;

	case F_SPLINE:
		if ( *(splinePath_t **)p == NULL)
			index = -1;
		else
			index = *(splinePath_t **)p - splinePaths;
		*(int *)p = index;
		break;

	//	match this with a function address in the function list, which is built using the
	//	"extractfuncs.bat" in the utils folder. We then save the string equivalent
	//	of the function. This effectively gives us cross-version save games.
	case F_AIFUNCTION:
		if (*(byte **)p == NULL)
			len = 0;
		else {
			func = AI_FindFuncAtAddress( *(byte **)p );
			if (!func) {
				G_Error ("WriteField1: unknown ai function, cannot save game");
			}
			len = strlen(func->funcStr) + 1;
		}
		*(int *)p = len;
		break;

	case F_BOTWAYPOINT:
		if ( *(bot_waypoint_t **)p == NULL)
			index = -1;
		else
			index = *(bot_waypoint_t **)p - botai_waypoints;
		*(int *)p = index;
		break;

	case F_BOTSCRIPTDATA:
		if ( *(bot_script_data_t **)p == NULL)
			index = -1;
		else {
			index = Bot_FindSriptGlobalData( *(bot_script_data_t **)p );
			if( index < 0 ) {
				G_Error ("WriteField1: unknown bot script data, cannot save game");
			}
		}
		*(int *)p = index;
		break;

	default:
		G_Error ("WriteField1: unknown field type");
	}
}

void WriteField2 (fileHandle_t f, saveField_t *field, byte *base)
{
	int			len;
	void		*p;
	funcList_t	*func;

	p = (void *)(base + field->ofs);
	switch (field->type)
	{
	case F_STRING:
		if ( *(char **)p )
		{
			len = strlen(*(char **)p) + 1;
			if (!G_SaveWrite (*(char **)p, len, f)) G_SaveWriteError();
		}
		break;
	case F_FUNCTION:
		if ( *(byte **)p )
		{
			func = G_FindFuncAtAddress( *(byte **)p );
			if (!func) {
				G_Error ("WriteField2: unknown function, cannot save game");
			}
			len = strlen(func->funcStr) + 1;
			if (!G_SaveWrite (func->funcStr, len, f)) G_SaveWriteError();
		}
		break;
	case F_AIFUNCTION:
		if ( *(byte **)p )
		{
			func = AI_FindFuncAtAddress( *(byte **)p );
			if (!func) {
				G_Error ("WriteField2: unknown function, cannot save game");
			}
			len = strlen(func->funcStr) + 1;
			if (!G_SaveWrite (func->funcStr, len, f)) G_SaveWriteError();
		}
		break;
	default:
		break;
	}
}

void ReadField (fileHandle_t f, saveField_t *field, byte *base)
{
	void		*p;
	int			len;
	int			index;
	char		funcStr[512];

	p = (void *)(base + field->ofs);
	switch (field->type)
	{
	case F_STRING:
		len = *(int *)p;
		if (!len)
			*(char **)p = NULL;
		else
		{
			*(char **)p = G_Alloc (len);
			trap_FS_Read (*(char **)p, len, f);
		}
		break;
	case F_ENTITY:
		index = *(int *)p;
		if (index >= MAX_GENTITIES || index < -1)
			G_Error( "ReadField: entity out of range (%i)", index );
		if ( index == -1 )
			*(gentity_t **)p = NULL;
		else
			*(gentity_t **)p = &g_entities[index];
		break;
	case F_CLIENT:
		index = *(int *)p;
		if (index >= MAX_CLIENTS || index < -1)
			G_Error( "ReadField: client out of range (%i)", index );
		if ( index == -1 )
			*(gclient_t **)p = NULL;
		else
			*(gclient_t **)p = &level.clients[index];
		break;
	case F_ITEM:
		index = *(int *)p;
		if ( index == -1 )
			*(gitem_t **)p = NULL;
		else
			*(gitem_t **)p = &bg_itemlist[index];
		break;

	//relative to code segment
	case F_FUNCTION:
		len = *(int *)p;
		if (!len)
			*(byte **)p = NULL;
		else
		{
			//funcStr = G_Alloc (len);
			if (len > sizeof(funcStr)) {
				G_Error ("ReadField: function name is greater than buffer (%i chars)", sizeof(funcStr));
			}
			trap_FS_Read (funcStr, len, f);
			if ((*(byte **)p = G_FindFuncByName( funcStr )) == NULL) {
				G_Error ("ReadField: unknown function '%s'\ncannot load game", funcStr);
			}
		}
		break;

	case F_SPLINE:
		index = *(int *)p;
		if ( index == -1 )
			*(splinePath_t **)p = NULL;
		else
			*(splinePath_t **)p = &splinePaths[index];
		break;

	//relative to code segment
	case F_AIFUNCTION:
		len = *(int *)p;
		if (!len)
			*(byte **)p = NULL;
		else
		{
			//funcStr = G_Alloc (len);
			if (len > sizeof(funcStr)) {
				G_Error ("ReadField: function name is greater than buffer (%i chars)", sizeof(funcStr));
			}
			trap_FS_Read (funcStr, len, f);
			if ((*(byte **)p = AI_FindFuncByName( funcStr )) == NULL) {
				G_Error ("ReadField: unknown function '%s'\ncannot load game", funcStr);
			}
		}
		break;

	case F_BOTWAYPOINT:
		index = *(int *)p;
		if ( index == -1 )
			*(bot_waypoint_t **)p = NULL;
		else
			*(bot_waypoint_t **)p = &botai_waypoints[index];
		break;

	case F_BOTSCRIPTDATA:
		index = *(int *)p;
		if ( index == -1 )
			*(bot_script_data_t **)p = NULL;
		else
			*(bot_script_data_t **)p = botCharacterScriptData[index].data;
		break;

	default:
		G_Error ("ReadField: unknown field type");
	}
}

//=========================================================

#define SAVE_ENCODE_COUNT_BYTES		1

/*
===============
G_Save_Encode

  returns the number of bytes written to "out"
===============
*/
int G_Save_Encode( byte *raw, byte *out, int rawsize, int outsize ) {
	int rawcount, oldrawcount, outcount;
	int mode;
	byte count;		//DAJ was int but caused endian bugs

	rawcount = 0;
	outcount = 0;
	while (rawcount < rawsize) {
		oldrawcount = rawcount;
		// is this a non-zero?
		if (raw[rawcount]) {
			mode = 1;
		} else {
			mode = 0;
		}
		// calc the count
		count = 0;
		while (rawcount < rawsize && (raw[rawcount] != 0) == mode && count < (((1<<(SAVE_ENCODE_COUNT_BYTES*8-1))-1))) {
			rawcount++;
			count++;
		}
		// write the count, followed by data if required
		memcpy( out + outcount, &count, SAVE_ENCODE_COUNT_BYTES );
		// switch the sign bit if zeros
		if (!mode) {
			out[outcount+SAVE_ENCODE_COUNT_BYTES-1] |= (1<<7);
			outcount += SAVE_ENCODE_COUNT_BYTES;
		} else {
			outcount += SAVE_ENCODE_COUNT_BYTES;
			// write the data
			memcpy( out + outcount, raw + oldrawcount, count );
			outcount += count;
		}
	}

	return outcount;
}

/*
===============
G_Save_Decode
===============
*/
void G_Save_Decode( byte *in, int insize, byte *out, int outsize ) {
	int incount, outcount;
	byte count;		//DAJ was in but caused endian bugs
	//
	incount = 0;
	outcount = 0;
	while (incount < insize) {
		// read the count
		count = 0;
		memcpy( &count, in+incount, SAVE_ENCODE_COUNT_BYTES );
		incount += SAVE_ENCODE_COUNT_BYTES;
		// if it's negative, zero it out
		if (count & (1<<((SAVE_ENCODE_COUNT_BYTES*8)-1))) {
			count &= ~(1<<((SAVE_ENCODE_COUNT_BYTES*8)-1));
			memset( out+outcount, 0, count );
			outcount += count;
		} else {
			// copy the data from "in"
			memcpy( out+outcount, in+incount, count );
			outcount += count;
			incount += count;
		}
	}
}

//=========================================================

byte clientBuf[ 2 * sizeof(gentity_t) ];

/*
===============
WriteClient
===============
*/
void WriteClient(fileHandle_t f, gclient_t *cl)
{
	saveField_t	*field;
	gclient_t	temp;
	int length;

	// copy the structure across, then process the fields
	temp = *cl;

	// first, kill all events (assume they have been processed)
	memset( temp.ps.events, 0, sizeof(temp.ps.events) );
	memset( temp.ps.eventParms, 0, sizeof(temp.ps.eventParms) );
	temp.ps.eventSequence = 0;
	temp.ps.oldEventSequence = 0;
	temp.ps.entityEventSequence = 0;

	// change the pointers to lengths or indexes
	for (field=gclientFields ; field->type ; field++)
	{
		WriteField1 (field, (byte *)&temp);
	}

	// write the block
	//if (!G_SaveWrite (&temp, sizeof(temp), f)) G_SaveWriteError();
	length = G_Save_Encode( (byte *)&temp, clientBuf, sizeof( temp ), sizeof( clientBuf ) );
	if (!G_SaveWrite (&length, sizeof(length), f)) G_SaveWriteError();
	if (!G_SaveWrite (&clientBuf, length, f)) G_SaveWriteError();

	// now write any allocated data following the edict
	for (field=gclientFields ; field->type ; field++)
	{
		WriteField2 (f, field, (byte *)cl);
	}
	
}

/*
===============
ReadClient
===============
*/
void ReadClient (fileHandle_t f, gclient_t *client, int size)
{
	saveField_t	*field;
	ignoreField_t *ifield;
	gclient_t	temp;
	gentity_t	*ent;
	int decodedSize;

	// read the encoded chunk
	trap_FS_Read (&decodedSize, sizeof(int), f);
	if (decodedSize > sizeof(clientBuf)) {
		G_Error("G_LoadGame: encoded chunk is greater than buffer");
	}
	trap_FS_Read ( clientBuf, decodedSize, f );\
	// decode it
	G_Save_Decode( clientBuf, decodedSize, (byte *)&temp, sizeof(temp) );

	// convert any feilds back to the correct data
	for (field=gclientFields ; field->type ; field++)
	{
		ReadField (f, field, (byte *)&temp);
	}

	// backup any fields that we don't want to read in
	for (ifield=gclientIgnoreFields ; ifield->len ; ifield++)
	{
		memcpy( ((byte *)&temp) + ifield->ofs, ((byte *)client) + ifield->ofs, ifield->len );
	}

	// now copy the temp structure into the existing structure
	memcpy( client, &temp, size );

	// make sure they face the right way
	//client->ps.pm_flags |= PMF_RESPAWNED;
	// don't allow full run speed for a bit
	//if (client->ps.clientNum == 0) {	// only set this for the player
		client->ps.pm_flags |= PMF_TIME_LOAD;
		client->ps.pm_time = 1000;
	//}

	ent = &g_entities[client->ps.clientNum];

#ifndef NO_BOT_SUPPORT
	if( ent->r.svFlags & SVF_BOT ) {
		if( trap_BotAllocateClient( client->ps.clientNum ) < 0 ) {
			G_Error("G_LoadGame: can't allocate required client for bot");
		}
	}
#endif

	// make sure they face the right way
	trap_GetUsercmd( ent->client - level.clients, &ent->client->pers.cmd );
	SetClientViewAngle( ent, ent->client->ps.viewangles );

	// dead characters should stay on last frame after a loadgame
	if (client->ps.eFlags & EF_DEAD) {
		client->ps.eFlags |= EF_FORCE_END_FRAME;
	}

	// RF, disabled, not required now with screen fading, causes characters to possibly spawn events
	// before they are known in the cgame
	// run a client frame to drop exactly to the floor,
	// initialize animations and other things
	//trap_GetUsercmd( ent-g_entities, &ent->client->pers.cmd );
	//ent->client->ps.commandTime = ent->client->pers.cmd.serverTime - 100;
	//ClientThink( ent-g_entities );

	ClientUserinfoChanged( ent->client - level.clients );

	// tell the client to reset it's cgame stuff
	if (!(ent->r.svFlags & SVF_BOT)) {
		vmCvar_t cvar;
		// tell it which weapon to use after spawning in
		trap_Cvar_Register( &cvar, "cg_loadWeaponSelect", "0", CVAR_ROM );
		trap_Cvar_Set( "cg_loadWeaponSelect", va("%i", client->ps.weapon ) );
		//
		trap_SendServerCommand( client->ps.clientNum, "map_restart\n" );
	}
}

//=========================================================

byte entityBuf[ 2 * sizeof(gentity_t) ];

/*
===============
WriteEntity
===============
*/
void WriteEntity(fileHandle_t f, gentity_t *ent)
{
	saveField_t	*field;
	gentity_t	temp;
	int length;

	// copy the structure across, then process the fields
	temp = *ent;

	// first, kill all events (assume they have been processed)
	memset( temp.s.events, 0, sizeof(temp.s.events) );
	memset( temp.s.eventParms, 0, sizeof(temp.s.eventParms) );
	temp.s.eventSequence = 0;

	// change the pointers to lengths or indexes
	for (field=gentityFields ; field->type ; field++)
	{
		WriteField1 (field, (byte *)&temp);
	}

	// write the block
	//if (!G_SaveWrite (&temp, sizeof(temp), f)) G_SaveWriteError();
	length = G_Save_Encode( (byte *)&temp, entityBuf, sizeof( temp ), sizeof( entityBuf ) );
	if (!G_SaveWrite (&length, sizeof(length), f)) G_SaveWriteError();
	if (!G_SaveWrite (&entityBuf, length, f)) G_SaveWriteError();

	// now write any allocated data following the edict
	for (field=gentityFields ; field->type ; field++)
	{
		WriteField2 (f, field, (byte *)ent);
	}
	
}

/*
===============
ReadEntity
===============
*/
void ReadEntity (fileHandle_t f, gentity_t *ent, int size)
{
	saveField_t	*field;
	ignoreField_t *ifield;
	gentity_t	temp, backup, backup2;
//	vmCvar_t	cvar;
	int decodedSize;

	backup = *ent;

	// read the encoded chunk
	trap_FS_Read (&decodedSize, sizeof(int), f);
	if (decodedSize > sizeof(entityBuf)) {
		G_Error("G_LoadGame: encoded chunk is greater than buffer");
	}
	trap_FS_Read ( entityBuf, decodedSize, f );
	// decode it
	G_Save_Decode( entityBuf, decodedSize, (byte *)&temp, sizeof(temp) );

	// convert any fields back to the correct data
	for (field=gentityFields ; field->type ; field++)
	{
		ReadField (f, field, (byte *)&temp);
	}

	// backup any fields that we don't want to read in
	for (ifield=gentityIgnoreFields ; ifield->len ; ifield++)
	{
		memcpy( ((byte *)&temp) + ifield->ofs, ((byte *)ent) + ifield->ofs, ifield->len );
	}

	// kill all events (assume they have been processed)
	if (!temp.freeAfterEvent) {
		temp.s.event = 0;
		memset( temp.s.events, 0, sizeof(temp.s.events) );
		memset( temp.s.eventParms, 0, sizeof(temp.s.eventParms) );
		temp.s.eventSequence = 0;
		temp.eventTime = 0;
	}

	// now copy the temp structure into the existing structure
	memcpy( ent, &temp, size );

	// notify server of changes in position/orientation
	if (ent->r.linked /*&& (!(ent->r.svFlags & SVF_BOT) || !ent->aiInactive)*/) {
		trap_LinkEntity( ent );
	} else {
		trap_UnlinkEntity( ent );
	}

	// if this is a mover, check areaportals
	if (ent->s.eType == ET_MOVER && ent->moverState != backup.moverState) {
		if ( ent->teammaster == ent || !ent->teammaster ) {
			if ( ent->moverState == MOVER_POS1ROTATE || ent->moverState == MOVER_POS1 ) {
				// closed areaportal
				trap_AdjustAreaPortalState( ent, qfalse );
			} else {	// must be open
				// portals are always opened before the mover starts to open, so we must move
				// it back to the start position, link, set portals, then move it back
				backup2 = *ent;
				*ent = backup;
				// link it at original position
				trap_LinkEntity( ent );
				// set portals
				trap_AdjustAreaPortalState( ent, qtrue );
				// put it back
				*ent = backup2;
				trap_LinkEntity( ent );
			}
		}
	}

	// check for blocking AAS at save time
	if (ent->AASblocking) {
		G_SetAASBlockingEntity( ent, AAS_AREA_DISABLED );
	}

	// check for this being a tagconnect entity
	if (ent->tagName && ent->tagParent) {	// the parent might not be there yet
		G_ProcessTagConnect( ent, qfalse );
	}
}

//=========================================================

/*
==============
WriteTime
==============
*/
void WriteTime(fileHandle_t f) {
	qtime_t tm;

	// just save it all so it can be interpreted as desired
	trap_RealTime(&tm);
	G_SaveWrite ( &tm.tm_sec,		sizeof(tm.tm_sec),	f);		/* seconds after the minute - [0,59] */
	G_SaveWrite ( &tm.tm_min,		sizeof(tm.tm_min),	f);		/* minutes after the hour - [0,59] */
	G_SaveWrite ( &tm.tm_hour,	sizeof(tm.tm_hour),	f);		/* hours since midnight - [0,23] */
	G_SaveWrite ( &tm.tm_mday,	sizeof(tm.tm_mday),	f);		/* day of the month - [1,31] */
	G_SaveWrite ( &tm.tm_mon,		sizeof(tm.tm_mon),	f);		/* months since January - [0,11] */
	G_SaveWrite ( &tm.tm_year,	sizeof(tm.tm_year),	f);		/* years since 1900 */
	G_SaveWrite ( &tm.tm_wday,	sizeof(tm.tm_wday),	f);		/* days since Sunday - [0,6] */
	G_SaveWrite ( &tm.tm_yday,	sizeof(tm.tm_yday),	f);		/* days since January 1 - [0,365] */
	G_SaveWrite ( &tm.tm_isdst,	sizeof(tm.tm_isdst),f);		/* daylight savings time flag */
}

/*
==============
ReadTime
==============
*/
void ReadTime(fileHandle_t f, qtime_t *tm) {
	trap_FS_Read (&tm->tm_sec, sizeof(tm->tm_sec), f);
	trap_FS_Read (&tm->tm_min, sizeof(tm->tm_min), f);
	trap_FS_Read (&tm->tm_hour, sizeof(tm->tm_hour), f);
	trap_FS_Read (&tm->tm_mday, sizeof(tm->tm_mday), f);
	trap_FS_Read (&tm->tm_mon, sizeof(tm->tm_mon), f);
	trap_FS_Read (&tm->tm_year, sizeof(tm->tm_year), f);
	trap_FS_Read (&tm->tm_wday, sizeof(tm->tm_wday), f);
	trap_FS_Read (&tm->tm_yday, sizeof(tm->tm_yday), f);
	trap_FS_Read (&tm->tm_isdst, sizeof(tm->tm_isdst), f);
}

/*
==============
G_Save_TimeStr
==============
*/
char *G_Save_TimeStr(void) {
	qtime_t tm;
	//
	trap_RealTime(&tm);
	//
	return va("%2i:%s%i:%s%i %s",
				(1 + (tm.tm_hour+11) % 12),		// 12 hour format
				(tm.tm_min > 9 ? "" : "0"),		// minute padding
				tm.tm_min,
				(tm.tm_sec > 9 ? "" : "0"),		// second padding
				tm.tm_sec,
				(tm.tm_hour < 12 ? "am" : "pm") );
}

static char *monthStr[12] =
{
	"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
};

/*
==============
G_Save_DateStr
==============
*/
char *G_Save_DateStr(void) {
	qtime_t tm;
	//
	trap_RealTime(&tm);
	//
	return va("%s %i, %i",
				monthStr[tm.tm_mon],
				tm.tm_mday,
				1900 + tm.tm_year );
}

//=========================================================

byte botwaypointBuf[ 2 * sizeof(bot_waypoint_t) ];

/*
===============
WriteBotWaypoint
===============
*/
void WriteBotWaypoint(fileHandle_t f, bot_waypoint_t *wp)
{
	saveField_t	*field;
	bot_waypoint_t temp;
	int length;

	// copy the structure across, then process the fields
	temp = *wp;

	// change the pointers to lengths or indexes
	for (field=botwaypointFields ; field->type ; field++)
	{
		WriteField1 (field, (byte *)&temp);
	}

	// write the block
	length = G_Save_Encode( (byte *)&temp, botwaypointBuf, sizeof( temp ), sizeof( botwaypointBuf ) );
	if (!G_SaveWrite (&length, sizeof(length), f)) G_SaveWriteError();
	if (!G_SaveWrite (&botwaypointBuf, length, f)) G_SaveWriteError();

	// now write any allocated data following the edict
	for (field=botwaypointFields ; field->type ; field++)
	{
		WriteField2 (f, field, (byte *)wp);
	}
	
}

/*
===============
ReadBotWaypoint
===============
*/
void ReadBotWaypoint (fileHandle_t f, bot_waypoint_t *wp, int size)
{
	saveField_t	*field;
	bot_waypoint_t	temp;
	int decodedSize;

	// read the encoded chunk
	trap_FS_Read (&decodedSize, sizeof(int), f);
	if (decodedSize > sizeof(botwaypointBuf)) {
		G_Error("G_LoadGame: encoded chunk is greater than buffer");
	}
	trap_FS_Read ( botwaypointBuf, decodedSize, f );\
	// decode it
	G_Save_Decode( botwaypointBuf, decodedSize, (byte *)&temp, sizeof(temp) );

	// convert any feilds back to the correct data
	for (field=botwaypointFields ; field->type ; field++)
	{
		ReadField (f, field, (byte *)&temp);
	}

	// now copy the temp structure into the existing structure
	memcpy( wp, &temp, size );
}

//=========================================================

byte botstateBuf[ 2 * sizeof(bot_state_t) ];

/*
===============
WriteBotState
===============
*/
void WriteBotState(fileHandle_t f, bot_state_t *bs)
{
	saveField_t	*field;
	bot_state_t temp;
	int length;

	// copy the structure across, then process the fields
	temp = *bs;

	// change the pointers to lengths or indexes
	for (field=botstateFields ; field->type ; field++)
	{
		WriteField1 (field, (byte *)&temp);
	}

	// write the block
	length = G_Save_Encode( (byte *)&temp, botstateBuf, sizeof( temp ), sizeof( botstateBuf ) );
	if (!G_SaveWrite (&length, sizeof(length), f)) G_SaveWriteError();
	if (!G_SaveWrite (&botstateBuf, length, f)) G_SaveWriteError();

	// now write any allocated data following the edict
	for (field=botstateFields ; field->type ; field++)
	{
		WriteField2 (f, field, (byte *)bs);
	}
	
}

/*
===============
ReadBotState
===============
*/
void ReadBotState (fileHandle_t f, bot_state_t *bs, int size)
{
	saveField_t	*field;
	ignoreField_t *ifield;
	bot_state_t	temp;
	int decodedSize;
	char filename[MAX_QPATH], name[MAX_QPATH], gender[MAX_QPATH];
	int errnum;

	// read the encoded chunk
	trap_FS_Read (&decodedSize, sizeof(int), f);
	if (decodedSize > sizeof(botstateBuf)) {
		G_Error("G_LoadGame: encoded chunk is greater than buffer");
	}
	trap_FS_Read ( botstateBuf, decodedSize, f );\
	// decode it
	G_Save_Decode( botstateBuf, decodedSize, (byte *)&temp, sizeof(temp) );

	// convert any feilds back to the correct data
	for (field=botstateFields ; field->type ; field++)
	{
		ReadField (f, field, (byte *)&temp);
	}

	// backup any fields that we don't want to read in
	for (ifield=botstateIgnoreFields ; ifield->len ; ifield++)
	{
		memcpy( ((byte *)&temp) + ifield->ofs, ((byte *)bs) + ifield->ofs, ifield->len );
	}

	// now copy the temp structure into the existing structure
	memcpy( bs, &temp, size );

	// initialize character and set botstates in engine
	//load the bot character
	bs->character = trap_BotLoadCharacter(bs->settings.characterfile, bs->settings.skill);
	if (!bs->character) {
		G_Error(S_COLOR_RED, "Fatal: couldn't load skill %d from %s\n", bs->settings.skill, bs->settings.characterfile);		
	}

	//allocate a goal state
	bs->gs = trap_BotAllocGoalState(bs->client);
	//load the item weights
	trap_Characteristic_String(bs->character, CHARACTERISTIC_ITEMWEIGHTS, filename, MAX_QPATH);
	errnum = trap_BotLoadItemWeights(bs->gs, filename);
	if (errnum != BLERR_NOERROR) {
		trap_BotFreeGoalState(bs->gs);
		G_Error(S_COLOR_RED, "Fatal: ReadBotState failed\n" );
	}
	//allocate a weapon state
	bs->ws = trap_BotAllocWeaponState();
	//load the weapon weights
	trap_Characteristic_String(bs->character, CHARACTERISTIC_WEAPONWEIGHTS, filename, MAX_QPATH);
	errnum = trap_BotLoadWeaponWeights(bs->ws, filename);
	if (errnum != BLERR_NOERROR) {
		trap_BotFreeGoalState(bs->gs);
		trap_BotFreeWeaponState(bs->ws);
		G_Error(S_COLOR_RED, "Fatal: ReadBotState failed\n" );
	}
	//allocate a chat state
	bs->cs = trap_BotAllocChatState();
	//load the chat file
	trap_Characteristic_String(bs->character, CHARACTERISTIC_CHAT_FILE, filename, MAX_QPATH);
	trap_Characteristic_String(bs->character, CHARACTERISTIC_CHAT_NAME, name, MAX_QPATH);
	errnum = trap_BotLoadChatFile(bs->cs, filename, name);
	if (errnum != BLERR_NOERROR) {
		trap_BotFreeChatState(bs->cs);
		trap_BotFreeGoalState(bs->gs);
		trap_BotFreeWeaponState(bs->ws);
		G_Error(S_COLOR_RED, "Fatal: ReadBotState failed\n" );
	}
	//get the gender characteristic
	trap_Characteristic_String(bs->character, CHARACTERISTIC_GENDER, gender, MAX_QPATH);
	//set the chat gender
	if (*gender == 'f' || *gender == 'F') trap_BotSetChatGender(bs->cs, CHAT_GENDERFEMALE);
	else if (*gender == 'm' || *gender == 'M') trap_BotSetChatGender(bs->cs, CHAT_GENDERMALE);
	else trap_BotSetChatGender(bs->cs, CHAT_GENDERLESS);

	bs->ms = trap_BotAllocMoveState();

	bs->walker = trap_Characteristic_BFloat(bs->character, CHARACTERISTIC_WALKER, 0, 1);
	
	BotGetInitialAttributes( bs );

	trap_EA_ResetInput(bs->client, NULL);

	BotAI_SetNumBots(BotAI_GetNumBots()+1);
}

//=========================================================

static char	infoString[SAVE_INFOSTRING_LENGTH];

/*
===============
G_SaveGame

  returns qtrue if successful

  TODO: have G_SaveWrite return the number of byte's written, so if it doesn't
  succeed, we can abort the save, and not save the file. This means we should
  save to a temporary name, then copy it across to the real name after success,
  so full disks don't result in lost saved games.
===============
*/
qboolean G_SaveGame(char *username)
{
	char	filename[MAX_QPATH];
	char	mapstr[MAX_QPATH];
	//char	leveltime[MAX_QPATH];
	//char	healthstr[MAX_QPATH];
	vmCvar_t	mapname;
	fileHandle_t f;
	int	i, len;
	gentity_t	*ent;
	gclient_t	*cl;
	bot_waypoint_t *wp;
	bot_state_t *bs;
	//int		playtime, minutes;
	char	cl_profileStr[256];
	long	hash;
	char	*ch;
	char	userinfo[MAX_INFO_STRING];

	//if (reloading)
	//	return qtrue;	// actually this should be qtrue, but we should make it silent during reloading

	if(g_entities[0].health <= 0)	// no save when dead
		return qtrue;

	if(g_gametype.integer != GT_SINGLE_PLAYER) {	// don't allow saves in MP
		return qtrue;
	}

	G_DPrintf("G_SaveGame '%s'\n", username);

	if (!username) {
		username = "current";
	}

	// validate the filename
	for (i=0; i<strlen(username); i++) {
		if (!Q_isforfilename( username[i] ) && username[i] != '\\') {	// (allow '\\' so games can be saved in subdirs)
			G_Printf( "G_SaveGame: '%s'.  Invalid character (%c) in filename. Must use alphanumeric characters only.\n", username, username[i]);
			return qtrue;
		}
	}

	saveByteCount= 0;

	trap_Cvar_VariableStringBuffer( "cl_profile", cl_profileStr, 256 );

	// generate hash for profile
	for( hash = 0, ch = (char *)cl_profileStr; *ch != '\0'; ch++ ) {
		hash += (long)(tolower(*ch))*((ch-cl_profileStr)+119);
	}

	// open the file
	Com_sprintf( filename, MAX_QPATH, "profiles/%s/save/temp.sav", cl_profileStr );
	if (trap_FS_FOpenFile( filename, &f, FS_WRITE ) < 0) {
		G_Error( "G_SaveGame: cannot open file for saving\n" );
	}

	// write the version
	i = SAVE_VERSION;
	ver = SAVE_VERSION;
	if (!G_SaveWrite (&i, sizeof(i), f)) {
		G_SaveWriteError();
	}

	// write the mapname
	trap_Cvar_Register( &mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM );
	Com_sprintf( mapstr, MAX_QPATH, mapname.string );
	if (!G_SaveWrite (mapstr, MAX_QPATH, f)) {
		G_SaveWriteError();
	}

	// write out the level time
	if (!G_SaveWrite (&level.time, sizeof(level.time), f)) {
		G_SaveWriteError();
	}

	// write out current time/date info
	WriteTime(f);

	// write profile hash
	if (!G_SaveWrite (&hash, sizeof(hash), f)) {
		G_SaveWriteError();
	}

	// create and write the info string
	// (SA) I made another cvar so there's no confusion
	/*Q_strncpyz( mapstr, mapname.string, sizeof(mapstr) );
	for (i=0; i<strlen(mapstr); i++) mapstr[i] = toupper(mapstr[i]);
	memset( infoString, 0, sizeof(infoString) );

	trap_Cvar_VariableStringBuffer("svg_timestring", leveltime, sizeof(leveltime));
	if(!strlen(leveltime))
		Com_sprintf(leveltime, sizeof(leveltime), "Leveltime");

	trap_Cvar_VariableStringBuffer("svg_healthstring", healthstr, sizeof(healthstr));
	if(!strlen(healthstr))
		Com_sprintf(healthstr, sizeof(healthstr), "Health");


//	Com_sprintf( infoString, sizeof(infoString), "Mission: %s\nDate: %s\nTime: %s\nGametime: %s\nHealth: %i",
	Com_sprintf( infoString, sizeof(infoString), "%s\n%s: %s\n%s: %i",
		mapstr,
		leveltime,
//		G_Save_DateStr(),
//		G_Save_TimeStr(),
		va("%2ih%s%im%s%is",
				(((playtime/1000)/60)/60),		// hour
				(minutes > 9 ? "" : "0"),		// minute padding
				minutes,
				((playtime/1000)%60 > 9 ? "" : "0"),		// second padding
				((playtime/1000)%60) ),
		healthstr,
		g_entities[0].health );
	// write it out
	// length
	i = strlen( infoString );
	if (!G_SaveWrite (&i, sizeof(i), f)) {
		G_SaveWriteError();
	}
	// string
	if (!G_SaveWrite (infoString, strlen(infoString), f)) {
		G_SaveWriteError();
	}*/

//----(SA)	added

//----(SA)	end

	// write music
/*	trap_Cvar_Register( &musicCvar, "s_currentMusic", "", CVAR_ROM );
	if (!G_SaveWrite (musicCvar.string, MAX_QPATH, f)) {
		G_SaveWriteError();
	}*/

//	trap_Cvar_VariableStringBuffer( "sg_fog", infoString, sizeof(infoString) );
	trap_GetConfigstring( CS_FOGVARS, infoString, sizeof(infoString) );

	i = strlen( infoString );
	if (!G_SaveWrite (&i, sizeof(i), f)) {
		G_SaveWriteError();
	}
	// if there's fog info to save
	if( i ) {
		if (!G_SaveWrite (infoString, strlen(infoString), f)) {
			G_SaveWriteError();
		}
	}

	if( ver >= 31 ) {
		trap_GetConfigstring( CS_GLOBALFOGVARS, infoString, sizeof(infoString) );

		i = strlen( infoString );
		if (!G_SaveWrite (&i, sizeof(i), f)) {
			G_SaveWriteError();
		}
		// if there's fog info to save
		if( i ) {
			if (!G_SaveWrite (infoString, strlen(infoString), f)) {
				G_SaveWriteError();
			}
		}
	}

	// write out the entity structures
	i = sizeof(gentity_t);
	if (!G_SaveWrite (&i, sizeof(i), f)) {
		G_SaveWriteError();
	}
	for (i=0 ; i<level.num_entities ; i++)
	{
		ent = &g_entities[i];
		if (!ent->inuse || ent->s.number == ENTITYNUM_WORLD)
			continue;
		if (!G_SaveWrite (&i, sizeof(i), f)) {
			G_SaveWriteError();
		}
		WriteEntity (f, ent);
	}
	i = -1;
	if (!G_SaveWrite (&i, sizeof(i), f)) {
		G_SaveWriteError();
	}

	// write out the client structures
	i = sizeof(gclient_t);
	if (!G_SaveWrite (&i, sizeof(i), f)) {
		G_SaveWriteError();
	}
	for (i=0 ; i<MAX_CLIENTS ; i++)
	{
		cl = &level.clients[i];
		if (cl->pers.connected != CON_CONNECTED)
			continue;
		if (!G_SaveWrite (&i, sizeof(i), f)) {
			G_SaveWriteError();
		}

		// store userinfo if bot
		if( g_entities[i].r.svFlags & SVF_BOT ) {
			trap_GetUserinfo(i, userinfo, sizeof(userinfo));
			len = strlen( userinfo );
			if (!G_SaveWrite (&len, sizeof(len), f)) {
				G_SaveWriteError();
			}
			if( len ) {
				if (!G_SaveWrite (userinfo, strlen(userinfo), f)) {
					G_SaveWriteError();
				}
			}
		}

		WriteClient (f, cl);
	}
	i = -1;
	if (!G_SaveWrite (&i, sizeof(i), f)) {
		G_SaveWriteError();
	}

	// write out the botwaypoint linked list
	i = sizeof(bot_waypoint_t);
	if (!G_SaveWrite (&i, sizeof(i), f)) {
		G_SaveWriteError();
	}
	for (i=0 ; i<MAX_BOTAIWAYPOINTS ; i++)
	{
		wp = &botai_waypoints[i];
		WriteBotWaypoint(f, wp);
	}
	if( botai_freewaypoints )
		i = botai_freewaypoints - botai_waypoints;
	else
		i = -1;
	if (!G_SaveWrite (&i, sizeof(i), f)) {
		G_SaveWriteError();
	}

	// write out the botstate structures
	i = sizeof(bot_state_t);
	if (!G_SaveWrite (&i, sizeof(i), f)) {
		G_SaveWriteError();
	}
	for (i=0 ; i<MAX_CLIENTS ; i++)
	{
		bs = &botstates[i];
		if (!bs || !bs->inuse)
			continue;
		// if the parent client isn't connected
		if (level.clients[i].pers.connected != CON_CONNECTED)
			continue;
		if (!G_SaveWrite (&i, sizeof(i), f)) {
			G_SaveWriteError();
		}
		WriteBotState (f, bs);
	}
	i = -1;
	if (!G_SaveWrite (&i, sizeof(i), f)) {
		G_SaveWriteError();
	}

	trap_FS_FCloseFile( f );

	// check the byte count
	if ((len = trap_FS_FOpenFile( filename, &f, FS_READ )) != saveByteCount) {
		trap_FS_FCloseFile( f );
		G_SaveWriteError();
		return qfalse;
	}

	trap_FS_FCloseFile( f );

	// now rename the file to the actual file
	Com_sprintf( mapstr, MAX_QPATH, "profiles/%s/save/%s.sav", cl_profileStr, username );
	trap_FS_Rename( filename, mapstr );

	// double check that it saved ok
	if ((len = trap_FS_FOpenFile( mapstr, &f, FS_READ )) != saveByteCount) {
		trap_FS_FCloseFile( f );
		G_SaveWriteError();
		return qfalse;
	}

	trap_FS_FCloseFile( f );
#ifdef __MACOS__
	trap_FS_CopyFile( mapstr, va("profiles/%s/save/current.sav", cl_profileStr );
	
#endif

	return qtrue;
}

/*
===============
G_LoadGame

  Always loads in "current.svg". So if loading a specific savegame, first copy it to that.
===============
*/
void G_LoadGame(void) {
	char			filename[MAX_QPATH];
	char			mapname[MAX_QPATH];
	fileHandle_t	f;
	int				i, leveltime, size, last, len;
	gentity_t		*ent;
	gclient_t		*cl;
	bot_waypoint_t	*wp;
	bot_state_t		*bs;
	qtime_t			tm;
	qboolean		serverEntityUpdate = qfalse;
	char			*p;
	int				k;
	char			cl_profileStr[256];
	long			hash, fhash;
	char			*ch;
	char			userinfo[MAX_INFO_STRING];

	if(g_gametype.integer != GT_SINGLE_PLAYER) {	// don't allow loads in MP
		return;
	}

	if (saveGamePending)
		return;

	G_DPrintf("G_LoadGame '%s'\n", filename);

	trap_Cvar_VariableStringBuffer( "cl_profile", cl_profileStr, 256 );

	// generate hash for profile
	for( hash = 0, ch = (char *)cl_profileStr; *ch != '\0'; ch++ ) {
		hash += (long)(tolower(*ch))*((ch-cl_profileStr)+119);
	}

	// enforce the "current" savegame, since that is used for all loads
	Com_sprintf( filename, sizeof(filename), "profiles/%s/save/current.sav", cl_profileStr );

	// open the file
	if (trap_FS_FOpenFile( filename, &f, FS_READ ) < 0) {
		G_Error( "G_LoadGame: savegame '%s' not found\n", filename );
	}

	// read the version
	trap_FS_Read (&i, sizeof(i), f);

	if (i != SAVE_VERSION) {
		trap_FS_FCloseFile( f );
		G_Error( "G_LoadGame: savegame '%s' is wrong version (%i, should be %i)\n", filename, i, SAVE_VERSION );
	}
	ver = i;

	// read the mapname (this is only used in the sever exe, so just discard it)
	trap_FS_Read (mapname, MAX_QPATH, f);

	// read the level time
	trap_FS_Read (&i, sizeof(i), f);	
	leveltime = i;

	// NOTE: do not change the above order without also changing the server code

	// read current time/date info
	ReadTime(f, &tm);

	// read profile hash
	trap_FS_Read( &fhash, sizeof(fhash), f );
	if( hash != fhash ) {
		trap_FS_FCloseFile( f );
		G_Error( "G_LoadGame: savegame '%s' is for another profile\n", filename, i, SAVE_VERSION );
	}

	// read music
	/*trap_FS_Read(musicString, MAX_QPATH, f);

	if(strlen(musicString)) {
		trap_Cvar_Register( &musicCvar, "s_currentMusic", "", CVAR_ROM );	// get current music
		if(Q_stricmp(musicString, musicCvar.string)) {		// it's different than what's playing, so fade out and queue up
//			trap_SendServerCommand(-1, "mu_fade 0 1000\n");
//			trap_SetConfigstring( CS_MUSIC_QUEUE, musicString);
			trap_SendServerCommand(-1, va("mu_start %s 1000\n", musicString) );		// (SA) trying this instead
		}
	}*/

	// get length
	trap_FS_Read (&i, sizeof(i), f);
	// get fog string
	if( i ) {
		trap_FS_Read (infoString, i, f);
		infoString[i] = 0;
	} else {
		Q_strncpyz( infoString, "none", sizeof(infoString) );
	}

	// set the configstring so the 'savegame current' has good fog
	if(!Q_stricmp(infoString, "none"))	// no fog
	{
		trap_Cvar_Set( "r_savegameFogColor", "none" );
	} else {

		// send it off to get set on the client
		for(p = &infoString[0],k=0;*p;p++) {
			if(*p == ' ')
				k++;
			if(k==6) {	// the last parameter
				infoString[p-infoString + 1] = '0';
				infoString[p-infoString + 2] = 0;
				break;
			}
		}
		trap_Cvar_Set( "r_savegameFogColor", infoString );
	}

	if( ver >= 31 ) {
		// get length
		trap_FS_Read (&i, sizeof(i), f);
		// get fog string
		if( i ) {
			trap_FS_Read (infoString, i, f);
			infoString[i] = 0;
		} else {
			Q_strncpyz( infoString, "none", sizeof(infoString) );
		}

		// set the configstring so the 'savegame current' has good fog
		if(!Q_stricmp(infoString, "none"))	// no fog
		{
			trap_Cvar_Set( "r_savegameGlobalFog", "none" );
		} else {

			// send it off to get set on the client
			for(p = &infoString[0],k=0;*p;p++) {
				if(*p == ' ')
					k++;
				if(k==6) {	// the last parameter
					infoString[p-infoString + 1] = '0';
					infoString[p-infoString + 2] = 0;
					break;
				}
			}
			trap_Cvar_Set( "r_savegameGlobalFog", infoString );
		}
	}

	// reset all AAS blocking entities
	trap_AAS_SetAASBlockingEntity( vec3_origin, vec3_origin, -1 );

	// read the entity structures
	trap_FS_Read (&i, sizeof(i), f);
	size = i;
	last = 0;
	while (1)
	{
		trap_FS_Read (&i, sizeof(i), f);
		ent = &g_entities[i];
		// free any bot data if present
		if (i < 0)
			break;
		if (i >= MAX_GENTITIES) {
			trap_FS_FCloseFile( f );
			G_Error( "G_LoadGame: entitynum out of range (%i, MAX = %i)\n", i, MAX_GENTITIES );
		}
//		if( i < MAX_CLIENTS && ent->inuse && ent->r.svFlags & SVF_BOT ) {
//			BotAIShutdownClient(i);
//			trap_DropClient( i, "" );
//		}
		if (i >= level.num_entities) {	// notify server
			level.num_entities = i + 1;
			serverEntityUpdate = qtrue;
		}
		//ent = &g_entities[i];
		ReadEntity (f, ent, size);
		// free all entities that we skipped
		for ( ; last < i; last++) {
			if (g_entities[last].inuse && i != ENTITYNUM_WORLD) {
				if (last < MAX_CLIENTS) {
					// free any bot data if present
//					if( g_entities[last].r.svFlags & SVF_BOT ) {
//						BotAIShutdownClient(last);
//					}
					trap_DropClient( last, "", 0 );
				} else {
					G_FreeEntity( &g_entities[last] );
				}
			}
		}
		last = i+1;
	}

	// clear all remaining entities
	for (ent=&g_entities[last] ; last < MAX_GENTITIES ; last++, ent++) {
		memset (ent, 0, sizeof(*ent));
		ent->classname = "freed";
		ent->freetime = level.time;
		ent->inuse = qfalse;
	}

	// read the client structures
	trap_FS_Read (&i, sizeof(i), f);
	size = i;
	while (1)
	{
		trap_FS_Read (&i, sizeof(i), f);
		if (i < 0)
			break;
		if (i > MAX_CLIENTS) {
			trap_FS_FCloseFile( f );
			G_Error( "G_LoadGame: clientnum out of range\n" );
		}
		cl = &level.clients[i];

		// load userinfo if bot
		if( g_entities[i].r.svFlags & SVF_BOT ) {
			// get length
			trap_FS_Read (&len, sizeof(len), f);
			// get fog string
			if( len )
				trap_FS_Read (userinfo, len, f);
			userinfo[len] = 0;
			trap_SetUserinfo( i, userinfo );
		}

		/*if (cl->pers.connected == CON_DISCONNECTED) {
			trap_FS_FCloseFile( f );
			G_Error( "G_LoadGame: client mis-match in savegame" );
		}*/
		ReadClient (f, cl, size);
	}

	BotAI_SetNumBots(0);

	// inform server of entity count if it has increased
	if (serverEntityUpdate) {
		// let the server system know that there are more entities
		trap_LocateGameData( level.gentities, level.num_entities, sizeof( gentity_t ), 
			&level.clients[0].ps, sizeof( level.clients[0] ) );
	}

	// read the botwaypoint linked list
	trap_FS_Read (&i, sizeof(i), f);
	size = i;
	for( i = 0; i < MAX_BOTAIWAYPOINTS; i++ ) {
		wp = &botai_waypoints[i];
		ReadBotWaypoint (f, wp, size);
	}
	trap_FS_Read (&i, sizeof(i), f);
	botai_freewaypoints = &botai_waypoints[i];

	// read the botstate structures
	trap_FS_Read (&i, sizeof(i), f);
	size = i;
	while (1)
	{
		trap_FS_Read (&i, sizeof(i), f);
		if (i < 0)
			break;
		if (i > MAX_CLIENTS) {
			trap_FS_FCloseFile( f );
			G_Error( "G_LoadGame: botstatenum out of range\n" );
		}

		bs = &botstates[i];

		ReadBotState (f, bs, size);
	}

	// read music
/*	trap_FS_Read(musicString, MAX_QPATH, f);

	if(strlen(musicString)) {
		trap_Cvar_Register( &musicCvar, "s_currentMusic", "", CVAR_ROM );	// get current music
		if(Q_stricmp(musicString, musicCvar.string)) {		// it's different than what's playing, so fade out and queue up
			trap_SendServerCommand(-1, "mu_fade 0 1000\n");
			trap_SetConfigstring( CS_MUSIC_QUEUE, musicString);
		}
	}*/

//----(SA)	end moved



	trap_FS_FCloseFile( f );

	level.lastLoadTime = leveltime;

/*
	// always save to the "current" savegame
	last = level.time;
	level.time = leveltime;	// use the value we just for the save time
	G_SaveGame(NULL);
	// additionally update the last game that was loaded
	trap_Cvar_VariableStringBuffer( "savegame_filename", mapname, sizeof(mapname) );
	if (strlen( mapname ) > 0 && !strstr( mapname, "autosave" )) {
		// clear it out so we dont lose it after a map_restart
		trap_Cvar_Set( "savegame_filename", "" );
		if (strstr(mapname, ".svg")) mapname[strstr(mapname, ".sav") - mapname] = '\0';
		if (strstr(mapname, "/")) {
			G_SaveGame( strstr(mapname, "/") + 1 );
		} else {
			G_SaveGame( mapname );
		}
	}
	// restore the correct level.time
	level.time = last;
*/
}

//=========================================================

/*
===============
PersWriteClient
===============
*/
void PersWriteClient(fileHandle_t f, gclient_t *cl)
{
	persField_t	*field;

	// save the fields
	for (field=gclientPersFields ; field->len ; field++)
	{	// write the block
		G_SaveWrite ( (void *)((byte *)cl + field->ofs), field->len, f);
	}	
}

/*
===============
PersReadClient
===============
*/
void PersReadClient (fileHandle_t f, gclient_t *cl)
{
	persField_t	*field;

	// read the fields
	for (field=gclientPersFields ; field->len ; field++)
	{	// read the block
		trap_FS_Read ( (void *)((byte *)cl + field->ofs), field->len, f);
	}	
}

//=========================================================

/*
===============
PersWriteEntity
===============
*/
void PersWriteEntity(fileHandle_t f, gentity_t *ent)
{
	persField_t	*field;

	// save the fields
	for (field=gentityPersFields ; field->len ; field++)
	{	// write the block
		G_SaveWrite ( (void *)((byte *)ent + field->ofs), field->len, f);
	}	
}

/*
===============
PersReadEntity
===============
*/
void PersReadEntity (fileHandle_t f, gentity_t *cl)
{
	persField_t	*field;

	// read the fields
	for (field=gentityPersFields ; field->len ; field++)
	{	// read the block
		trap_FS_Read ( (void *)((byte *)cl + field->ofs), field->len, f);
	}	
}



//=========================================================

/*
===============
G_SavePersistant

  returns qtrue if successful

  NOTE: only saves the local player's data, doesn't support AI characters

  TODO: have G_SaveWrite return the number of byte's written, so if it doesn't
  succeed, we can abort the save, and not save the file. This means we should
  save to a temporary name, then copy it across to the real name after success,
  so full disks don't result in lost saved games.
===============
*/
qboolean G_SavePersistant(char *nextmap)
{
	char filename[MAX_QPATH];
	fileHandle_t f;
	int	persid;
	char cl_profileStr[256];

	saveByteCount= 0;

	trap_Cvar_VariableStringBuffer( "cl_profile", cl_profileStr, 256 );

	// open the file
	Com_sprintf( filename, MAX_QPATH, "profiles/%s/save/temp.psw", cl_profileStr );
	if (trap_FS_FOpenFile( filename, &f, FS_WRITE ) < 0) {
		G_Error( "G_SavePersistant: cannot open '%s' for saving\n", filename );
	}

	// write the mapname
	G_SaveWrite (nextmap, MAX_QPATH, f);

	// save out the pers id
	persid = trap_Milliseconds() + (rand() & 0xffff);
	G_SaveWrite (&persid, sizeof(persid), f);
	trap_Cvar_Set("persid", va("%i", persid) );

	// write out the entity structure
	PersWriteEntity (f, &g_entities[0]);

	// write out the client structure
	PersWriteClient (f, &level.clients[0]);

	trap_FS_FCloseFile( f );

	// now check that it is the correct size
	Com_sprintf( filename, MAX_QPATH, "profiles/%s/save/temp.psw", cl_profileStr );
	if (trap_FS_FOpenFile( filename, &f, FS_READ ) < saveByteCount) {
		trap_FS_FCloseFile( f );
		G_SaveWriteError();
		return qfalse;
	}
	trap_FS_FCloseFile( f );

	// rename it to the real file
	trap_FS_Rename( va("profiles/%s/save/temp.psw", cl_profileStr), va("profiles/%s/save/current.psw", cl_profileStr)  );

	// now check that it is the correct size
	Com_sprintf( filename, MAX_QPATH, "profiles/%s/save/current.psw", cl_profileStr  );
	if (trap_FS_FOpenFile( filename, &f, FS_READ ) < saveByteCount) {
		trap_FS_FCloseFile( f );
		G_SaveWriteError();
		return qfalse;
	}
	trap_FS_FCloseFile( f );

	return qtrue;
}

/*
===============
G_LoadPersistant
===============
*/
void G_LoadPersistant(void)
{
	fileHandle_t f;
	char	filename[MAX_QPATH];
	char	mapstr[MAX_QPATH];
	vmCvar_t	cvar_mapname;
	int		persid;
	char	cl_profileStr[256];

	trap_Cvar_VariableStringBuffer( "cl_profile", cl_profileStr, 256 );

	Com_sprintf( filename, MAX_QPATH, "profiles/%s/save/current.psw", cl_profileStr  );

	// open the file
	if (trap_FS_FOpenFile( filename, &f, FS_READ ) < 0) {
		// not here, we shall assume they didn't want one
		return;
	}

	// read the mapname, if it's not the same, then ignore the file
	trap_FS_Read (mapstr, MAX_QPATH, f);
	trap_Cvar_Register( &cvar_mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM );
	if (Q_stricmp( cvar_mapname.string, mapstr )) {
		trap_FS_FCloseFile( f );
		return;
	}

	// check the pers id
	trap_FS_Read (&persid, sizeof(persid), f);
	if (persid != trap_Cvar_VariableIntegerValue("persid")) {
		trap_FS_FCloseFile( f );
		return;
	}

	// read the entity structure
	PersReadEntity (f, &g_entities[0]);

	// read the client structure
	PersReadClient (f, &level.clients[0]);

	trap_FS_FCloseFile( f );

	// clear out the persid, since the persistent data has been read
	trap_Cvar_Set("persid", "0");
}

#endif // SAVEGAME_SUPPORT
