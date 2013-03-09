// cg_servercmds.c -- reliably sequenced text commands sent by the server
// these are processed at snapshot transition time, so there will definately
// be a valid snapshot this frame

#include "cg_local.h"
#include "../ui/ui_shared.h" // bk001205 - for Q3_ui as well

void CG_StartShakeCamera( float param );                                // NERVE - SMF

/*
=================
CG_ParseScores

=================
*/
static void CG_ParseScores( void ) {
	int i, powerups;

	cg.numScores = atoi( CG_Argv( 1 ) );
	if ( cg.numScores > MAX_CLIENTS ) {
		cg.numScores = MAX_CLIENTS;
	}

	cg.teamScores[0] = atoi( CG_Argv( 2 ) );
	cg.teamScores[1] = atoi( CG_Argv( 3 ) );

	memset( cg.scores, 0, sizeof( cg.scores ) );
	for ( i = 0 ; i < cg.numScores ; i++ ) {
		//
		cg.scores[i].client = atoi( CG_Argv( i * 8 + 4 ) );
		cg.scores[i].score = atoi( CG_Argv( i * 8 + 5 ) );
		cg.scores[i].ping = atoi( CG_Argv( i * 8 + 6 ) );
		cg.scores[i].time = atoi( CG_Argv( i * 8 + 7 ) );
		cg.scores[i].scoreFlags = atoi( CG_Argv( i * 8 + 8 ) );
		powerups = atoi( CG_Argv( i * 8 + 9 ) );
		cg.scores[i].playerClass = atoi( CG_Argv( i * 8 + 10 ) );       // NERVE - SMF
		cg.scores[i].respawnsLeft = atoi( CG_Argv( i * 8 + 11 ) );      // NERVE - SMF

		if ( cg.scores[i].client < 0 || cg.scores[i].client >= MAX_CLIENTS ) {
			cg.scores[i].client = 0;
		}
		cgs.clientinfo[ cg.scores[i].client ].score = cg.scores[i].score;
		cgs.clientinfo[ cg.scores[i].client ].powerups = powerups;

		cg.scores[i].team = cgs.clientinfo[cg.scores[i].client].team;
	}
}

/*
=================
CG_ParseTeamInfo

=================
*/
static void CG_ParseTeamInfo( void ) {
	int i;
	int client;

	// NERVE - SMF
	cg.identifyClientNum = atoi( CG_Argv( 1 ) );
	cg.identifyClientHealth = atoi( CG_Argv( 2 ) );
	// -NERVE - SMF

	numSortedTeamPlayers = atoi( CG_Argv( 3 ) );

	for ( i = 0 ; i < numSortedTeamPlayers ; i++ ) {
		client = atoi( CG_Argv( i * 5 + 4 ) );

		sortedTeamPlayers[i] = client;

		cgs.clientinfo[ client ].location = atoi( CG_Argv( i * 5 + 5 ) );
		cgs.clientinfo[ client ].health = atoi( CG_Argv( i * 5 + 6 ) );
		cgs.clientinfo[ client ].powerups = atoi( CG_Argv( i * 5 + 7 ) );

		cg_entities[ client ].currentState.teamNum = atoi( CG_Argv( i * 5 + 8 ) );
	}
}

/*
================
CG_ParseServerinfo

This is called explicitly when the gamestate is first received,
and whenever the server updates any serverinfo flagged cvars
================
*/
void CG_ParseServerinfo( void ) {
	const char  *info;
	char    *mapname;

	info = CG_ConfigString( CS_SERVERINFO );
	cgs.gametype = atoi( Info_ValueForKey( info, "g_gametype" ) );
	cgs.antilag = atoi( Info_ValueForKey( info, "g_antilag" ) );
	if ( !cgs.localServer ) {
		trap_Cvar_Set( "g_gametype", va( "%i", cgs.gametype ) );
		trap_Cvar_Set( "g_antilag", va( "%i", cgs.antilag ) );
	}
	cgs.dmflags = 0; //atoi( Info_ValueForKey( info, "dmflags" ) );				// NERVE - SMF - no longer serverinfo
	cgs.teamflags = 0; //atoi( Info_ValueForKey( info, "teamflags" ) );
	cgs.fraglimit = 0; //atoi( Info_ValueForKey( info, "fraglimit" ) );			// NERVE - SMF - no longer serverinfo
	cgs.capturelimit = 0; //atoi( Info_ValueForKey( info, "capturelimit" ) );	// NERVE - SMF - no longer serverinfo
	cgs.timelimit = atof( Info_ValueForKey( info, "timelimit" ) );
	cgs.maxclients = atoi( Info_ValueForKey( info, "sv_maxclients" ) );
	mapname = Info_ValueForKey( info, "mapname" );
	Com_sprintf( cgs.mapname, sizeof( cgs.mapname ), "maps/%s.bsp", mapname );

	// JPW NERVE
	// prolly should parse all CS_SERVERINFO keys automagically, but I don't want to break anything that might be improperly set for wolf SP, so I'm just parsing MP relevant stuff here
	trap_Cvar_Set( "g_medicChargeTime",Info_ValueForKey( info,"g_medicChargeTime" ) );
	trap_Cvar_Set( "g_engineerChargeTime",Info_ValueForKey( info,"g_engineerChargeTime" ) );
	trap_Cvar_Set( "g_soldierChargeTime",Info_ValueForKey( info,"g_soldierChargeTime" ) );
	trap_Cvar_Set( "g_LTChargeTime",Info_ValueForKey( info,"g_LTChargeTime" ) );
	trap_Cvar_Set( "g_redlimbotime",Info_ValueForKey( info,"g_redlimbotime" ) );
	// DHM - TEMP FIX
	cg_redlimbotime.integer = atoi( Info_ValueForKey( info,"g_redlimbotime" ) );
	//trap_Cvar_Set("g_bluelimbotime",Info_ValueForKey(info,"g_bluelimbotime"));
	cg_bluelimbotime.integer = atoi( Info_ValueForKey( info,"g_bluelimbotime" ) );
	// jpw

	cgs.minclients = atoi( Info_ValueForKey( info, "g_minGameClients" ) );      // NERVE - SMF

	// TTimo - make this available for ingame_callvote
	trap_Cvar_Set( "cg_ui_voteFlags", Info_ValueForKey( info, "g_voteFlags" ) );
}

/*
==================
CG_ParseWolfinfo

NERVE - SMF
==================
*/
void CG_ParseWolfinfo( void ) {
	const char  *info;
	info = CG_ConfigString( CS_WOLFINFO );

	cgs.currentRound = atoi( Info_ValueForKey( info, "g_currentRound" ) );
	cgs.nextTimeLimit = atof( Info_ValueForKey( info, "g_nextTimeLimit" ) );
	cgs.gamestate = atoi( Info_ValueForKey( info, "gamestate" ) );

	if ( !cgs.localServer ) {
		trap_Cvar_Set( "gamestate", va( "%i", cgs.gamestate ) );
	}
}

/*
==================
CG_ParseWarmup
==================
*/
static void CG_ParseWarmup( void ) {
	const char  *info;
	int warmup;

	info = CG_ConfigString( CS_WARMUP );

	warmup = atoi( info );
	cg.warmupCount = 0;
	cg.warmup = warmup;
}

/*
=====================
CG_ParseScreenFade
=====================
*/
static void CG_ParseScreenFade( void ) {
	const char  *info;
	char *token;

	info = CG_ConfigString( CS_SCREENFADE );

	token = COM_Parse( (char **)&info );
	cgs.fadeAlpha = atof( token );

	token = COM_Parse( (char **)&info );
	cgs.fadeStartTime = atoi( token );
	token = COM_Parse( (char **)&info );
	cgs.fadeDuration = atoi( token );

	if ( cgs.fadeStartTime + cgs.fadeDuration < cg.time ) {
		cgs.fadeAlphaCurrent = cgs.fadeAlpha;
	}
}

/*
==============
CG_ParseFog
float near dist
float far dist
float density
float[3] r,g,b
int		time
==============
*/
static void CG_ParseFog( void ) {
	const char  *info;
	char *token;
	float ne, fa, r, g, b, density;
	int time;

	info = CG_ConfigString( CS_FOGVARS );

	token = COM_Parse( (char **)&info );    ne = atof( token );
	token = COM_Parse( (char **)&info );    fa = atof( token );
	token = COM_Parse( (char **)&info );    density = atof( token );
	token = COM_Parse( (char **)&info );    r = atof( token );
	token = COM_Parse( (char **)&info );    g = atof( token );
	token = COM_Parse( (char **)&info );    b = atof( token );
	token = COM_Parse( (char **)&info );    time = atoi( token );

	if ( fa ) {    // far of '0' from a target_fog means "return to map fog"
		trap_R_SetFog( FOG_SERVER, (int)ne, (int)fa, r, g, b, density + .1 );
		trap_R_SetFog( FOG_CMD_SWITCHFOG, FOG_SERVER, time, 0, 0, 0, 0 );
	} else {
		trap_R_SetFog( FOG_CMD_SWITCHFOG, FOG_MAP, time, 0, 0, 0, 0 );
	}
}

// Parse reinforcement offsets
void CG_ParseReinforcementTimes( const char *pszReinfSeedString ) {
	const char *tmp = pszReinfSeedString, *tmp2;
	unsigned int i, j, dwDummy, dwOffset[TEAM_NUM_TEAMS];

#define GETVAL( x,y ) if ( ( tmp = strchr( tmp, ' ' ) ) == NULL ) {return; } x = atoi( ++tmp ) / y;

	dwOffset[TEAM_BLUE] = atoi( pszReinfSeedString ) >> REINF_BLUEDELT;
	GETVAL( dwOffset[TEAM_RED], ( 1 << REINF_REDDELT ) );
	tmp2 = tmp;

	for ( i = TEAM_RED; i <= TEAM_BLUE; i++ ) {
		tmp = tmp2;
		for ( j = 0; j < MAX_REINFSEEDS; j++ ) {
			if ( j == dwOffset[i] ) {
				GETVAL( cgs.aReinfOffset[i], aReinfSeeds[j] );
				cgs.aReinfOffset[i] *= 1000;
				break;
			}
			GETVAL( dwDummy, 1 );
		}
	}
}

/*
================
CG_SetConfigValues

Called on load to set the initial values from configure strings
================
*/
void CG_SetConfigValues( void ) {
	cgs.scores1 = atoi( CG_ConfigString( CS_SCORES1 ) );
	cgs.scores2 = atoi( CG_ConfigString( CS_SCORES2 ) );
	cgs.levelStartTime = atoi( CG_ConfigString( CS_LEVEL_START_TIME ) );

	cg.warmup = atoi( CG_ConfigString( CS_WARMUP ) );

	CG_ParseReinforcementTimes( CG_ConfigString( CS_REINFSEEDS ) );
}

/*
=====================
CG_ShaderStateChanged
=====================
*/
void CG_ShaderStateChanged( void ) {
	char originalShader[MAX_QPATH];
	char newShader[MAX_QPATH];
	char timeOffset[16];
	const char *o;
	char *n,*t;

	o = CG_ConfigString( CS_SHADERSTATE );
	while ( o && *o ) {
		n = strstr( o, "=" );
		if ( n && *n ) {
			strncpy( originalShader, o, n - o );
			originalShader[n - o] = 0;
			n++;
			t = strstr( n, ":" );
			if ( t && *t ) {
				strncpy( newShader, n, t - n );
				newShader[t - n] = 0;
			} else {
				break;
			}
			t++;
			o = strstr( t, "@" );
			if ( o ) {
				strncpy( timeOffset, t, o - t );
				timeOffset[o - t] = 0;
				o++;
				trap_R_RemapShader( originalShader, newShader, timeOffset );
			}
		} else {
			break;
		}
	}
}

/*
================
CG_ConfigStringModified

================
*/
static void CG_ConfigStringModified( void ) {
	const char  *str;
	int num;

	num = atoi( CG_Argv( 1 ) );

	// get the gamestate from the client system, which will have the
	// new configstring already integrated
	trap_GetGameState( &cgs.gameState );

	// look up the individual string that was modified
	str = CG_ConfigString( num );

	// do something with it if necessary
	if ( num == CS_MUSIC ) {
		CG_StartMusic();
	} else if ( num == CS_SERVERINFO ) {
		CG_ParseServerinfo();
	} else if ( num == CS_WARMUP ) {
		CG_ParseWarmup();
	} else if ( num == CS_WOLFINFO ) {      // NERVE - SMF
		CG_ParseWolfinfo();
	} else if ( num == CS_SCORES1 ) {
		cgs.scores1 = atoi( str );
	} else if ( num == CS_SCORES2 ) {
		cgs.scores2 = atoi( str );
	} else if ( num == CS_LEVEL_START_TIME ) {
		cgs.levelStartTime = atoi( str );
	} else if ( num == CS_REINFSEEDS ) {
		CG_ParseReinforcementTimes( str );
	} else if ( num == CS_VOTE_TIME ) {
		cgs.voteTime = atoi( str );
		cgs.voteModified = qtrue;
	} else if ( num == CS_VOTE_YES ) {
		cgs.voteYes = atoi( str );
		cgs.voteModified = qtrue;
	} else if ( num == CS_VOTE_NO ) {
		cgs.voteNo = atoi( str );
		cgs.voteModified = qtrue;
	} else if ( num == CS_VOTE_STRING ) {
		Q_strncpyz( cgs.voteString, str, sizeof( cgs.voteString ) );
	} else if ( num == CS_INTERMISSION ) {
		cg.intermissionStarted = atoi( str );
	} else if ( num == CS_SCREENFADE ) {
		CG_ParseScreenFade();
	} else if ( num == CS_FOGVARS ) {
		CG_ParseFog();
	} else if ( num >= CS_MODELS && num < CS_MODELS + MAX_MODELS ) {
		cgs.gameModels[ num - CS_MODELS ] = trap_R_RegisterModel( str );
	} else if ( num >= CS_SOUNDS && num < CS_SOUNDS + MAX_MODELS ) {
		if ( str[0] != '*' ) {  // player specific sounds don't register here
			// Ridah, register sound scripts seperately
			if ( !strstr( str, ".wav" ) ) {
				CG_SoundScriptPrecache( str );
			} else {
				cgs.gameSounds[ num - CS_SOUNDS] = trap_S_RegisterSound( str );
			}
		}
	} else if ( num >= CS_PLAYERS && num < CS_PLAYERS + MAX_CLIENTS ) {
		CG_NewClientInfo( num - CS_PLAYERS );
	}
	// Rafael particle configstring
	else if ( num >= CS_PARTICLES && num < CS_PARTICLES + MAX_PARTICLES_AREAS ) {
		CG_NewParticleArea( num );
	}
	//----(SA)	have not reached this code yet so I don't know if I really need this here
	else if ( num >= CS_DLIGHTS && num < CS_DLIGHTS + MAX_DLIGHTS ) {
		//		CG_Printf(">>>>>>>>>>>got configstring for dlight: %d\ntell Sherman!!!!!!!!!!", num-CS_DLIGHTS);
		//----(SA)
	} else if ( num == CS_SHADERSTATE ) {
		CG_ShaderStateChanged();
	}
}

/*
=======================
CG_AddToTeamChat

=======================
*/
static void CG_AddToTeamChat( const char *str ) {
	int len;
	char *p, *ls;
	int lastcolor;
	int chatHeight;

	if ( cg_teamChatHeight.integer < TEAMCHAT_HEIGHT ) {
		chatHeight = cg_teamChatHeight.integer;
	} else {
		chatHeight = TEAMCHAT_HEIGHT;
	}

	if ( chatHeight <= 0 || cg_teamChatTime.integer <= 0 ) {
		// team chat disabled, dump into normal chat
		cgs.teamChatPos = cgs.teamLastChatPos = 0;
		return;
	}

	len = 0;

	p = cgs.teamChatMsgs[cgs.teamChatPos % chatHeight];
	*p = 0;

	lastcolor = '7';

	ls = NULL;
	while ( *str ) {
		if ( len > TEAMCHAT_WIDTH - 1 ) {
			if ( ls ) {
				str -= ( p - ls );
				str++;
				p -= ( p - ls );
			}
			*p = 0;

			cgs.teamChatMsgTimes[cgs.teamChatPos % chatHeight] = cg.time;

			cgs.teamChatPos++;
			p = cgs.teamChatMsgs[cgs.teamChatPos % chatHeight];
			*p = 0;
			*p++ = Q_COLOR_ESCAPE;
			*p++ = lastcolor;
			len = 0;
			ls = NULL;
		}

		if ( Q_IsColorString( str ) ) {
			*p++ = *str++;
			lastcolor = *str;
			*p++ = *str++;
			continue;
		}
		if ( *str == ' ' ) {
			ls = p;
		}
		*p++ = *str++;
		len++;
	}
	*p = 0;

	cgs.teamChatMsgTimes[cgs.teamChatPos % chatHeight] = cg.time;
	cgs.teamChatPos++;

	if ( cgs.teamChatPos - cgs.teamLastChatPos > chatHeight ) {
		cgs.teamLastChatPos = cgs.teamChatPos - chatHeight;
	}
}

/*
=======================
CG_AddToNotify

=======================
*/
void CG_AddToNotify( const char *str ) {
	int len;
	char *p, *ls;
	int lastcolor;
	int chatHeight;
	float notifytime;
	char var[MAX_TOKEN_CHARS];

	trap_Cvar_VariableStringBuffer( "con_notifytime", var, sizeof( var ) );
	notifytime = atof( var ) * 1000;

	chatHeight = NOTIFY_HEIGHT;

	if ( chatHeight <= 0 || notifytime <= 0 ) {
		// team chat disabled, dump into normal chat
		cgs.notifyPos = cgs.notifyLastPos = 0;
		return;
	}

	len = 0;

	p = cgs.notifyMsgs[cgs.notifyPos % chatHeight];
	*p = 0;

	lastcolor = '7';

	ls = NULL;
	while ( *str ) {
		if ( len > NOTIFY_WIDTH - 1 || ( *str == '\n' && ( *( str + 1 ) != 0 ) ) ) {
			if ( ls ) {
				str -= ( p - ls );
				str++;
				p -= ( p - ls );
			}
			*p = 0;

			cgs.notifyMsgTimes[cgs.notifyPos % chatHeight] = cg.time;

			cgs.notifyPos++;
			p = cgs.notifyMsgs[cgs.notifyPos % chatHeight];
			*p = 0;
			*p++ = Q_COLOR_ESCAPE;
			*p++ = lastcolor;
			len = 0;
			ls = NULL;
		}

		if ( Q_IsColorString( str ) ) {
			*p++ = *str++;
			lastcolor = *str;
			*p++ = *str++;
			continue;
		}
		if ( *str == ' ' ) {
			ls = p;
		}
		while ( *str == '\n' ) {
			// TTimo gcc warning: value computed is not used
			// was *str++;
			str++;
		}

		if ( *str ) {
			*p++ = *str++;
			len++;
		}
	}
	*p = 0;

	cgs.notifyMsgTimes[cgs.notifyPos % chatHeight] = cg.time;
	cgs.notifyPos++;

	if ( cgs.notifyPos - cgs.notifyLastPos > chatHeight ) {
		cgs.notifyLastPos = cgs.notifyPos - chatHeight;
	}
}

/*
===============
CG_SendMoveSpeed
===============
*/
void CG_SendMoveSpeed( animation_t *animList, int numAnims, char *modelName ) {
	animation_t *anim;
	int i;
	char text[10000];

	if ( !cgs.localServer ) {
		return;
	}

	text[0] = 0;
	Q_strcat( text, sizeof( text ), modelName );

	for ( i = 0, anim = animList; i < numAnims; i++, anim++ ) {
		if ( anim->moveSpeed <= 0 ) {
			continue;
		}

		// add this to the list
		Q_strcat( text, sizeof( text ), va( " %s %i", anim->name, anim->moveSpeed ) );
	}

	// send the movespeeds to the server
	trap_SendMoveSpeedsToGame( 0, text );
}

/*
===============
CG_SendMoveSpeeds

send moveSpeeds for all unique models
===============
*/
void CG_SendMoveSpeeds( void ) {
	int i;
	animModelInfo_t *modelInfo;

	for ( i = 0, modelInfo = cgs.animScriptData.modelInfo; i < MAX_ANIMSCRIPT_MODELS; i++, modelInfo++ ) {
		if ( !modelInfo->modelname[0] ) {
			continue;
		}

		// send this model
		CG_SendMoveSpeed( modelInfo->animations, modelInfo->numAnimations, modelInfo->modelname );
	}
}

/*
===============
CG_MapRestart

The server has issued a map_restart, so the next snapshot
is completely new and should not be interpolated to.

A tournement restart will clear everything, but doesn't
require a reload of all the media
===============
*/
static void CG_MapRestart( void ) {
	int i;
	if ( cg_showmiss.integer ) {
		CG_Printf( "CG_MapRestart\n" );
	}

	memset( &cg.lastWeapSelInBank[0], 0, MAX_WEAP_BANKS * sizeof( int ) );  // clear weapon bank selections

	cg.centerPrintTime = 0; // reset centerprint counter so previous messages don't re-appear
	cg.itemPickupTime = 0;  // reset item pickup counter so previous messages don't re-appear
	cg.cursorHintFade = 0;  // reset cursor hint timer

	// DHM - Nerve :: Reset complaint system
	cgs.complaintClient = -1;
	cgs.complaintEndTime = 0;

	// (SA) clear zoom (so no warpies)
	cg.zoomedBinoc = qfalse;
	cg.zoomedBinoc = cg.zoomedScope = qfalse;
	cg.zoomTime = 0;
	cg.zoomval = 0;

	// reset fog to world fog (if present)
	trap_R_SetFog( FOG_CMD_SWITCHFOG, FOG_MAP,20,0,0,0,0 );

	CG_InitLocalEntities();
	CG_InitMarkPolys();

	//Rafael particles
	CG_ClearParticles();
	// done.

	for ( i = 1; i < MAX_PARTICLES_AREAS; i++ )
	{
		{
			int rval;

			rval = CG_NewParticleArea( CS_PARTICLES + i );
			if ( !rval ) {
				break;
			}
		}
	}

	// Ridah
	CG_ClearFlameChunks();
	CG_SoundInit();
	// done.

	// make sure the "3 frags left" warnings play again
	cg.fraglimitWarnings = 0;

	cg.timelimitWarnings = 0;

	cg.intermissionStarted = qfalse;

	cgs.voteTime = 0;

	cg.lightstylesInited    = qfalse;

	cg.mapRestart = qtrue;

	CG_StartMusic();

	trap_S_ClearLoopingSounds( qtrue );

	cg.latchVictorySound = qfalse;          // NERVE - SMF
	// JPW NERVE -- reset render flags
	cg_fxflags = 0;
	// jpw

	// we really should clear more parts of cg here and stop sounds
	cg.v_dmg_time = 0;
	cg.v_noFireTime = 0;
	cg.v_fireTime = 0;

	// play the "fight" sound if this is a restart without warmup
	if ( cg.warmup == 0 && cg_announcer.integer ) {
		trap_S_StartLocalSound( cgs.media.countFightSound, CHAN_ANNOUNCER );
		CG_CenterPrint( "^1FIGHT!", 320, SMALLCHAR_WIDTH );
	}

	trap_Cvar_Set( "cg_thirdPerson", "0" );
}

/*
=================
CG_RequestMoveSpeed
=================
*/
void CG_RequestMoveSpeed( const char *modelname ) {
	animModelInfo_t *modelInfo;

	modelInfo = BG_ModelInfoForModelname( (char *)modelname );

	if ( !modelInfo ) {
		// ignore it
		return;
	}

	// send it
	CG_SendMoveSpeed( modelInfo->animations, modelInfo->numAnimations, (char *)modelname );
}

// NERVE - SMF
#define MAX_VOICEFILESIZE   16384
#define MAX_VOICEFILES      8
#define MAX_VOICECHATS      64
#define MAX_VOICESOUNDS     64
#define MAX_CHATSIZE        64
#define MAX_HEADMODELS      64

typedef struct voiceChat_s
{
	char id[64];
	int numSounds;
	sfxHandle_t sounds[MAX_VOICESOUNDS];
	char chats[MAX_VOICESOUNDS][MAX_CHATSIZE];
	qhandle_t sprite[MAX_VOICESOUNDS];          // DHM - Nerve
} voiceChat_t;

typedef struct voiceChatList_s
{
	char name[64];
	int gender;
	int numVoiceChats;
	voiceChat_t voiceChats[MAX_VOICECHATS];
} voiceChatList_t;

typedef struct headModelVoiceChat_s
{
	char headmodel[64];
	int voiceChatNum;
} headModelVoiceChat_t;

voiceChatList_t voiceChatLists[MAX_VOICEFILES];
headModelVoiceChat_t headModelVoiceChat[MAX_HEADMODELS];

/*
=================
CG_ParseVoiceChats
=================
*/
int CG_ParseVoiceChats( const char *filename, voiceChatList_t *voiceChatList, int maxVoiceChats ) {
	int len, i;
	int current = 0;
	fileHandle_t f;
	char buf[MAX_VOICEFILESIZE];
	char **p, *ptr;
	char *token;
	voiceChat_t *voiceChats;

	len = trap_FS_FOpenFile( filename, &f, FS_READ );
	if ( !f ) {
		trap_Print( va( S_COLOR_RED "voice chat file not found: %s\n", filename ) );
		return qfalse;
	}
	if ( len >= MAX_VOICEFILESIZE ) {
		trap_Print( va( S_COLOR_RED "voice chat file too large: %s is %i, max allowed is %i", filename, len, MAX_VOICEFILESIZE ) );
		trap_FS_FCloseFile( f );
		return qfalse;
	}

	trap_FS_Read( buf, len, f );
	buf[len] = 0;
	trap_FS_FCloseFile( f );

	ptr = buf;
	p = &ptr;

	Com_sprintf( voiceChatList->name, sizeof( voiceChatList->name ), "%s", filename );
	voiceChats = voiceChatList->voiceChats;
	for ( i = 0; i < maxVoiceChats; i++ ) {
		voiceChats[i].id[0] = 0;
	}
	token = COM_ParseExt( p, qtrue );
	if ( !token || token[0] == 0 ) {
		return qtrue;
	}
	if ( !Q_stricmp( token, "female" ) ) {
		voiceChatList->gender = GENDER_FEMALE;
	} else if ( !Q_stricmp( token, "male" ) ) {
		voiceChatList->gender = GENDER_MALE;
	} else if ( !Q_stricmp( token, "neuter" ) ) {
		voiceChatList->gender = GENDER_NEUTER;
	} else {
		trap_Print( va( S_COLOR_RED "expected gender not found in voice chat file: %s\n", filename ) );
		return qfalse;
	}

	voiceChatList->numVoiceChats = 0;
	while ( 1 ) {
		token = COM_ParseExt( p, qtrue );
		if ( !token || token[0] == 0 ) {
			return qtrue;
		}
		Com_sprintf( voiceChats[voiceChatList->numVoiceChats].id, sizeof( voiceChats[voiceChatList->numVoiceChats].id ), "%s", token );
		token = COM_ParseExt( p, qtrue );
		if ( Q_stricmp( token, "{" ) ) {
			trap_Print( va( S_COLOR_RED "expected { found %s in voice chat file: %s\n", token, filename ) );
			return qfalse;
		}
		voiceChats[voiceChatList->numVoiceChats].numSounds = 0;
		current = voiceChats[voiceChatList->numVoiceChats].numSounds;

		while ( 1 ) {
			token = COM_ParseExt( p, qtrue );
			if ( !token || token[0] == 0 ) {
				return qtrue;
			}
			if ( !Q_stricmp( token, "}" ) ) {
				break;
			}
			voiceChats[voiceChatList->numVoiceChats].sounds[current] = trap_S_RegisterSound( token /*, compress */ );
			token = COM_ParseExt( p, qtrue );
			if ( !token || token[0] == 0 ) {
				return qtrue;
			}
			Com_sprintf( voiceChats[voiceChatList->numVoiceChats].chats[current], MAX_CHATSIZE, "%s", token );

			// DHM - Nerve :: Specify sprite shader to show above player's head
			token = COM_ParseExt( p, qfalse );
			if ( !Q_stricmp( token, "}" ) || !token || token[0] == 0 ) {
				voiceChats[voiceChatList->numVoiceChats].sprite[current] = trap_R_RegisterShader( "sprites/voiceChat" );
				COM_RestoreParseSession( p );
			} else {
				voiceChats[voiceChatList->numVoiceChats].sprite[current] = trap_R_RegisterShader( token );
				if ( voiceChats[voiceChatList->numVoiceChats].sprite[current] == 0 ) {
					voiceChats[voiceChatList->numVoiceChats].sprite[current] = trap_R_RegisterShader( "sprites/voiceChat" );
				}
			}
			// dhm - end

			voiceChats[voiceChatList->numVoiceChats].numSounds++;
			current = voiceChats[voiceChatList->numVoiceChats].numSounds;

			if ( voiceChats[voiceChatList->numVoiceChats].numSounds >= MAX_VOICESOUNDS ) {
				break;
			}
		}

		voiceChatList->numVoiceChats++;
		if ( voiceChatList->numVoiceChats >= maxVoiceChats ) {
			return qtrue;
		}
	}
	return qtrue;
}

/*
=================
CG_LoadVoiceChats
=================
*/
void CG_LoadVoiceChats( void ) {
	int size;

	size = trap_MemoryRemaining();
	CG_ParseVoiceChats( "scripts/wm_axis_chat.voice", &voiceChatLists[0], MAX_VOICECHATS );
	CG_ParseVoiceChats( "scripts/wm_allies_chat.voice", &voiceChatLists[1], MAX_VOICECHATS );
	CG_Printf( "voice chat memory size = %d\n", size - trap_MemoryRemaining() );
}

/*
=================
CG_HeadModelVoiceChats
=================
*/
int CG_HeadModelVoiceChats( char *filename ) {
	int len, i;
	fileHandle_t f;
	char buf[MAX_VOICEFILESIZE];
	char **p, *ptr;
	char *token;

	len = trap_FS_FOpenFile( filename, &f, FS_READ );
	if ( !f ) {
		trap_Print( va( "voice chat file not found: %s\n", filename ) );
		return -1;
	}
	if ( len >= MAX_VOICEFILESIZE ) {
		trap_Print( va( S_COLOR_RED "voice chat file too large: %s is %i, max allowed is %i", filename, len, MAX_VOICEFILESIZE ) );
		trap_FS_FCloseFile( f );
		return -1;
	}

	trap_FS_Read( buf, len, f );
	buf[len] = 0;
	trap_FS_FCloseFile( f );

	ptr = buf;
	p = &ptr;

	token = COM_ParseExt( p, qtrue );
	if ( !token || token[0] == 0 ) {
		return -1;
	}

	for ( i = 0; i < MAX_VOICEFILES; i++ ) {
		if ( !Q_stricmp( token, voiceChatLists[i].name ) ) {
			return i;
		}
	}

	//FIXME: maybe try to load the .voice file which name is stored in token?

	return -1;
}

/*
=================
CG_GetVoiceChat
=================
*/
int CG_GetVoiceChat( voiceChatList_t *voiceChatList, const char *id, sfxHandle_t *snd, qhandle_t *sprite, char **chat ) {
	int i, rnd;

	for ( i = 0; i < voiceChatList->numVoiceChats; i++ ) {
		if ( !Q_stricmp( id, voiceChatList->voiceChats[i].id ) ) {
			rnd = random() * voiceChatList->voiceChats[i].numSounds;
			*snd = voiceChatList->voiceChats[i].sounds[rnd];
			*sprite = voiceChatList->voiceChats[i].sprite[rnd];
			*chat = voiceChatList->voiceChats[i].chats[rnd];
			return qtrue;
		}
	}
	return qfalse;
}

/*
=================
CG_VoiceChatListForClient
=================
*/
voiceChatList_t *CG_VoiceChatListForClient( int clientNum ) {
	clientInfo_t *ci;
	int voiceChatNum, i, j, k, gender;
	char filename[128], *headModelName;

	// NERVE - SMF
	if ( cgs.gametype >= GT_WOLF ) {
		if ( cgs.clientinfo[ clientNum ].team == TEAM_RED ) {
			return &voiceChatLists[0];
		} else {
			return &voiceChatLists[1];
		}
	}
	// -NERVE - SMF

	if ( clientNum < 0 || clientNum >= MAX_CLIENTS ) {
		clientNum = 0;
	}
	ci = &cgs.clientinfo[ clientNum ];

	headModelName = ci->headModelName;
	if ( headModelName[0] == '*' ) {
		headModelName++;
	}

	// find the voice file for the head model the client uses
	for ( i = 0; i < MAX_HEADMODELS; i++ ) {
		if ( !Q_stricmp( headModelVoiceChat[i].headmodel, headModelName ) ) {
			break;
		}
	}

	if ( i < MAX_HEADMODELS ) {
		return &voiceChatLists[headModelVoiceChat[i].voiceChatNum];
	}

	// find a <headmodelname>.vc file
	for ( i = 0; i < MAX_HEADMODELS; i++ ) {
		if ( !strlen( headModelVoiceChat[i].headmodel ) ) {
			Com_sprintf( filename, sizeof( filename ), "scripts/%s.vc", headModelName );
			voiceChatNum = CG_HeadModelVoiceChats( filename );
			if ( voiceChatNum == -1 ) {
				break;
			}
			Com_sprintf( headModelVoiceChat[i].headmodel, sizeof( headModelVoiceChat[i].headmodel ),
				"%s", headModelName );
			headModelVoiceChat[i].voiceChatNum = voiceChatNum;
			return &voiceChatLists[headModelVoiceChat[i].voiceChatNum];
		}
	}

	gender = ci->gender;
	for ( k = 0; k < 2; k++ ) {
		// just pick the first with the right gender
		for ( i = 0; i < MAX_VOICEFILES; i++ ) {
			if ( strlen( voiceChatLists[i].name ) ) {
				if ( voiceChatLists[i].gender == gender ) {
					// store this head model with voice chat for future reference
					for ( j = 0; j < MAX_HEADMODELS; j++ ) {
						if ( !strlen( headModelVoiceChat[j].headmodel ) ) {
							Com_sprintf( headModelVoiceChat[j].headmodel, sizeof( headModelVoiceChat[j].headmodel ),
								"%s", headModelName );
							headModelVoiceChat[j].voiceChatNum = i;
							break;
						}
					}
					return &voiceChatLists[i];
				}
			}
		}

		// fall back to male gender because we don't have neuter in the mission pack
		if ( gender == GENDER_MALE ) {
			break;
		}

		gender = GENDER_MALE;
	}

	// store this head model with voice chat for future reference
	for ( j = 0; j < MAX_HEADMODELS; j++ ) {
		if ( !strlen( headModelVoiceChat[j].headmodel ) ) {
			Com_sprintf( headModelVoiceChat[j].headmodel, sizeof( headModelVoiceChat[j].headmodel ),
				"%s", headModelName );
			headModelVoiceChat[j].voiceChatNum = 0;
			break;
		}
	}

	// just return the first voice chat list
	return &voiceChatLists[0];
}

#define MAX_VOICECHATBUFFER     32

typedef struct bufferedVoiceChat_s
{
	int clientNum;
	sfxHandle_t snd;
	qhandle_t sprite;
	int voiceOnly;
	char cmd[MAX_SAY_TEXT];
	char message[MAX_SAY_TEXT];
	vec3_t origin;          // NERVE - SMF
} bufferedVoiceChat_t;

bufferedVoiceChat_t voiceChatBuffer[MAX_VOICECHATBUFFER];

/*
=================
CG_PlayVoiceChat
=================
*/
void CG_PlayVoiceChat( bufferedVoiceChat_t *vchat ) {
	if ( !cg_noVoiceChats.integer ) {
		trap_S_StartLocalSound( vchat->snd, CHAN_VOICE );

		// DHM - Nerve :: Show icon above head
		if ( vchat->clientNum == cg.snap->ps.clientNum ) {
			cg.predictedPlayerEntity.voiceChatSprite = vchat->sprite;
			if ( vchat->sprite == cgs.media.voiceChatShader ) {
				cg.predictedPlayerEntity.voiceChatSpriteTime = cg.time + cg_voiceSpriteTime.integer;
			} else {
				cg.predictedPlayerEntity.voiceChatSpriteTime = cg.time + cg_voiceSpriteTime.integer * 2;
			}
		} else {
			cg_entities[ vchat->clientNum ].voiceChatSprite = vchat->sprite;
			VectorCopy( vchat->origin, cg_entities[ vchat->clientNum ].lerpOrigin );            // NERVE - SMF
			if ( vchat->sprite == cgs.media.voiceChatShader ) {
				cg_entities[ vchat->clientNum ].voiceChatSpriteTime = cg.time + cg_voiceSpriteTime.integer;
			} else {
				cg_entities[ vchat->clientNum ].voiceChatSpriteTime = cg.time + cg_voiceSpriteTime.integer * 2;
			}
		}
		// dhm - end
	}

	if ( !vchat->voiceOnly && !cg_noVoiceText.integer ) {
		CG_AddToTeamChat( vchat->message );
		CG_Printf( va( "[skipnotify]: %s\n", vchat->message ) ); // JPW NERVE
	}

	voiceChatBuffer[cg.voiceChatBufferOut].snd = 0;
}

/*
=====================
CG_PlayBufferedVoieChats
=====================
*/
void CG_PlayBufferedVoiceChats( void ) {
	if ( cg.voiceChatTime < cg.time ) {
		if ( cg.voiceChatBufferOut != cg.voiceChatBufferIn && voiceChatBuffer[cg.voiceChatBufferOut].snd ) {
			//
			CG_PlayVoiceChat( &voiceChatBuffer[cg.voiceChatBufferOut] );
			//
			cg.voiceChatBufferOut = ( cg.voiceChatBufferOut + 1 ) % MAX_VOICECHATBUFFER;
			cg.voiceChatTime = cg.time + 1000;
		}
	}
}

/*
=====================
CG_AddBufferedVoiceChat
=====================
*/
void CG_AddBufferedVoiceChat( bufferedVoiceChat_t *vchat ) {
	// JPW NERVE new system doesn't buffer but overwrites vchats FIXME put this on a cvar to choose which to use
	memcpy( &voiceChatBuffer[0],vchat,sizeof( bufferedVoiceChat_t ) );
	cg.voiceChatBufferIn = 0;
	CG_PlayVoiceChat( &voiceChatBuffer[0] );
}

/*
=================
CG_VoiceChatLocal
=================
*/
void CG_VoiceChatLocal( int mode, qboolean voiceOnly, int clientNum, int color, const char *cmd, vec3_t origin ) {
	char *chat;
	voiceChatList_t *voiceChatList;
	clientInfo_t *ci;
	sfxHandle_t snd;
	qhandle_t sprite;
	bufferedVoiceChat_t vchat;
	const char *loc;            // NERVE - SMF

	if ( clientNum < 0 || clientNum >= MAX_CLIENTS ) {
		clientNum = 0;
	}
	ci = &cgs.clientinfo[ clientNum ];

	cgs.currentVoiceClient = clientNum;

	voiceChatList = CG_VoiceChatListForClient( clientNum );

	if ( CG_GetVoiceChat( voiceChatList, cmd, &snd, &sprite, &chat ) ) {
		//
		if ( mode == SAY_TEAM || !cg_teamChatsOnly.integer ) {
			vchat.clientNum = clientNum;
			vchat.snd = snd;
			vchat.sprite = sprite;
			vchat.voiceOnly = voiceOnly;
			VectorCopy( origin, vchat.origin );     // NERVE - SMF
			Q_strncpyz( vchat.cmd, cmd, sizeof( vchat.cmd ) );

			// NERVE - SMF - get location
			loc = CG_ConfigString( CS_LOCATIONS + ci->location );
			if ( !loc || !*loc ) {
				loc = " ";
			}
			// -NERVE - SMF

			if ( mode == SAY_TELL ) {
				Com_sprintf( vchat.message, sizeof( vchat.message ), "[%s]%c%c[%s]: %c%c%s",
					ci->name, Q_COLOR_ESCAPE, COLOR_YELLOW, CG_TranslateString( loc ), Q_COLOR_ESCAPE, color, CG_TranslateString( chat ) );
			} else if ( mode == SAY_TEAM ) {
				Com_sprintf( vchat.message, sizeof( vchat.message ), "(%s)%c%c(%s): %c%c%s",
					ci->name, Q_COLOR_ESCAPE, COLOR_YELLOW, CG_TranslateString( loc ), Q_COLOR_ESCAPE, color, CG_TranslateString( chat ) );
			} else {
				Com_sprintf( vchat.message, sizeof( vchat.message ), "%s %c%c(%s): %c%c%s",
					ci->name, Q_COLOR_ESCAPE, COLOR_YELLOW, CG_TranslateString( loc ), Q_COLOR_ESCAPE, color, CG_TranslateString( chat ) );
			}
			CG_AddBufferedVoiceChat( &vchat );
		}
	}
}

/*
=================
CG_VoiceChat
=================
*/
void CG_VoiceChat( int mode ) {
	const char *cmd;
	int clientNum, color;
	qboolean voiceOnly;
	vec3_t origin;          // NERVE - SMF

	voiceOnly = atoi( CG_Argv( 1 ) );
	clientNum = atoi( CG_Argv( 2 ) );
	color = atoi( CG_Argv( 3 ) );

	// NERVE - SMF - added origin
	origin[0] = atoi( CG_Argv( 5 ) );
	origin[1] = atoi( CG_Argv( 6 ) );
	origin[2] = atoi( CG_Argv( 7 ) );

	cmd = CG_Argv( 4 );

	if ( cg_noTaunt.integer != 0 ) {
		if ( !strcmp( cmd, VOICECHAT_KILLINSULT )  || !strcmp( cmd, VOICECHAT_TAUNT ) || \
			!strcmp( cmd, VOICECHAT_DEATHINSULT ) || !strcmp( cmd, VOICECHAT_KILLGAUNTLET ) ||	\
			!strcmp( cmd, VOICECHAT_PRAISE ) ) {
				return;
		}
	}

	CG_VoiceChatLocal( mode, voiceOnly, clientNum, color, cmd, origin );
}
// -NERVE - SMF

/*
=================
CG_RemoveChatEscapeChar
=================
*/
static void CG_RemoveChatEscapeChar( char *text ) {
	int i, l;

	l = 0;
	for ( i = 0; text[i]; i++ ) {
		if ( text[i] == '\x19' ) {
			continue;
		}
		text[l++] = text[i];
	}
	text[l] = '\0';
}

/*
=================
CG_LocalizeServerCommand

NERVE - SMF - localize string sent from server

- localization is ON by default.
- use [lof] in string to turn OFF
- use [lon] in string to turn back ON
=================
*/
const char* CG_LocalizeServerCommand( const char *buf ) {
	static char token[MAX_TOKEN_CHARS];
	char temp[MAX_TOKEN_CHARS];
	qboolean togloc = qtrue;
	const char *s;
	int i, prev;

	memset( token, 0, sizeof( token ) );
	s = buf;
	prev = 0;

	for ( i = 0; *s; i++, s++ ) {
		if ( *s == '[' && ( !Q_strncmp( s, "[lon]", 5 ) || !Q_strncmp( s, "[lof]", 5 ) ) ) {
			if ( togloc ) {
				memset( temp, 0, sizeof( temp ) );
				strncpy( temp, buf + prev, i - prev );
				strcat( token, CG_TranslateString( temp ) );
			} else {
				strncat( token, buf + prev, i - prev );
			}

			if ( s[3] == 'n' ) {
				togloc = qtrue;
			} else {
				togloc = qfalse;
			}

			i += 5;
			s += 5;
			prev = i;
		}
	}

	if ( togloc ) {
		memset( temp, 0, sizeof( temp ) );
		strncpy( temp, buf + prev, i - prev ); // cs: isn't i - prev always going to be 0 here?
		strcat( token, CG_TranslateString( temp ) );
	} else {
		strncat( token, buf + prev, i - prev ); // cs: isn't i - prev always going to be 0 here?
	}

	return token;
}
// -NERVE - SMF

/*
=================
CG_ServerCommand

The string has been tokenized and can be retrieved with
Cmd_Argc() / Cmd_Argv()
=================
*/
static void CG_ServerCommand( void ) {
	const char  *cmd;
	char text[MAX_SAY_TEXT];
	int hash;

	cmd = CG_Argv( 0 );

	if ( !cmd[0] ) {
		// server claimed the command
		return;
	}

	hash = BG_StringHashValue( cmd );
	switch ( hash )
	{
	case 65811: /*tinfo*/
		CG_ParseTeamInfo();
		return;

	case 79587: /*scores*/
		CG_ParseScores();
		return;

	case 25221: /*cp*/
		{
			int args = trap_Argc();
			char *s;

			if ( args >= 3 ) {
				s = CG_TranslateString( CG_Argv( 1 ) );

				if ( args == 4 ) {
					s = va( "%s%s", CG_Argv( 3 ), s );
				}
				// Martin - OSP style
				// cs: not in warmup. lazy way to make it so award prints don't spam this stuff
				if ( cgs.gamestate == GS_PLAYING && cg_printObjectiveInfo.integer > 0 && ( args == 4 || atoi( CG_Argv( 2 ) ) > 1 ) ) {
					CG_Printf( "[cgnotify]*** ^3INFO: ^5%s\n", CG_LocalizeServerCommand( CG_Argv( 1 ) ) );
				}
				CG_PriorityCenterPrint( s, SCREEN_HEIGHT - ( SCREEN_HEIGHT * 0.25 ), SMALLCHAR_WIDTH, atoi( CG_Argv( 2 ) ) );
			} else {
				CG_CenterPrint( CG_LocalizeServerCommand( CG_Argv( 1 ) ), SCREEN_HEIGHT - ( SCREEN_HEIGHT * 0.25 ), SMALLCHAR_WIDTH );  //----(SA)	modified
			}
			return;
		}

	case 25581: /*cs*/
		CG_ConfigStringModified();
		return;

	case 63379: /*shake*/
		CG_StartShakeCamera( atof( CG_Argv( 1 ) ) );
		return;

	case 67401: /*print*/
		CG_Printf( "[cgnotify]%s", CG_LocalizeServerCommand( CG_Argv( 1 ) ) );
		return;

	case 50150: /*chat*/
		{
			const char *s;

			if ( cg_teamChatsOnly.integer ) {
				return;
			}

			if ( atoi( CG_Argv( 2 ) ) ) {
				s = CG_LocalizeServerCommand( CG_Argv( 1 ) );
			} else {
				s = CG_Argv( 1 );
			}

			trap_S_StartLocalSound( cgs.media.talkSound, CHAN_LOCAL_SOUND );
			Q_strncpyz( text, s, MAX_SAY_TEXT );
			CG_RemoveChatEscapeChar( text );
			CG_AddToTeamChat( text ); // JPW NERVE
			CG_Printf( "[skipnotify]%s\n", text ); // JPW NERVE

			// NERVE - SMF - we also want this to display in limbo chat
			if ( cgs.gametype >= GT_WOLF ) {
				trap_UI_LimboChat( text );
			}

			return;
		}

	case 64370: /*tchat*/
		{
			const char *s;

			if ( atoi( CG_Argv( 2 ) ) ) {
				s = CG_LocalizeServerCommand( CG_Argv( 1 ) );
			} else {
				s = CG_Argv( 1 );
			}

			trap_S_StartLocalSound( cgs.media.talkSound, CHAN_LOCAL_SOUND );
			Q_strncpyz( text, s, MAX_SAY_TEXT );
			CG_RemoveChatEscapeChar( text );
			CG_AddToTeamChat( text );
			CG_Printf( "[skipnotify]%s\n", text ); // JPW NERVE

			// NERVE - SMF - we also want this to display in limbo chat
			if ( cgs.gametype >= GT_WOLF ) {
				trap_UI_LimboChat( text );
			}

			return;
		}

	case 64608: /*vchat*/
		CG_VoiceChat( SAY_ALL );
		return;

	case 78944: /*vtchat*/
		CG_VoiceChat( SAY_TEAM );
		return;

	case 66643: /*vtell*/
		CG_VoiceChat( SAY_TELL );
		return;

	case 63418: /*lchat*/
		trap_S_StartLocalSound( cgs.media.talkSound, CHAN_LOCAL_SOUND );
		Q_strncpyz( text, CG_Argv( 1 ), MAX_SAY_TEXT );
		CG_RemoveChatEscapeChar( text );
		trap_UI_LimboChat( text );
		CG_Printf( "[skipnotify]%s\n", text ); // JPW NERVE
		return;

	case 37909: /*oid*/
		CG_ObjectivePrint( CG_Argv( 1 ), SMALLCHAR_WIDTH );
		return;

	case 118983: /*complaint*/
		cgs.complaintEndTime = cg.time + 20000;
		cgs.complaintClient = atoi( CG_Argv( 1 ) );

		if ( cgs.complaintClient < 0 ) {
			cgs.complaintEndTime = cg.time + 10000;
		}
		return;

	case 147165: /*map_restart*/
		CG_MapRestart();
		return;

	case 67010: /*mvspd*/
		CG_RequestMoveSpeed( CG_Argv( 1 ) );
		return;

	case 144301: /*remapShader*/
		if ( trap_Argc() == 4 ) {
			char shader1[MAX_QPATH];
			char shader2[MAX_QPATH];
			char shader3[MAX_QPATH];

			Q_strncpyz( shader1, CG_Argv( 1 ), sizeof( shader1 ) );
			Q_strncpyz( shader2, CG_Argv( 2 ), sizeof( shader2 ) );
			Q_strncpyz( shader3, CG_Argv( 3 ), sizeof( shader3 ) );

			trap_R_RemapShader( shader1, shader2, shader3 );
		}
		return;

	case 155508: /*loaddeferred*/
		CG_LoadDeferredPlayers();
		return;

	case 204408: /*clientLevelShot*/
		cg.levelShot = qtrue;
		return;

	default:
		CG_Printf( "Unknown client game command: %s\n", cmd );
	}
}

/*
====================
CG_ExecuteNewServerCommands

Execute all of the server commands that were received along
with this this snapshot.
====================
*/
void CG_ExecuteNewServerCommands( int latestSequence ) {
	while ( cgs.serverCommandSequence < latestSequence ) {
		if ( trap_GetServerCommand( ++cgs.serverCommandSequence ) ) {
			CG_ServerCommand();
		}
	}
}