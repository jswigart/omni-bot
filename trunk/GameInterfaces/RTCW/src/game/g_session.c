#include "g_local.h"

/*
=======================================================================

SESSION DATA

Session data is the only data that stays persistant across level loads
and tournament restarts.
=======================================================================
*/

/*
================
G_WriteClientSessionData

Called on game shutdown
================
*/
void G_WriteClientSessionData( gclient_t *client ) {
	const char  *s;
	const char  *var;

	s = va( "%i %i %i %i %i %i %i %i %i %i %i %i %i %i %i",      // DHM - Nerve
		client->sess.sessionTeam,
		client->sess.spectatorTime,
		client->sess.spectatorState,
		client->sess.spectatorClient,
		client->sess.wins,
		client->sess.losses,
		client->sess.playerType,        // DHM - Nerve
		client->sess.playerWeapon,      // DHM - Nerve
		client->sess.playerItem,        // DHM - Nerve
		client->sess.playerSkin,        // DHM - Nerve
		client->sess.spawnObjectiveIndex, // DHM - Nerve
		client->sess.latchPlayerType,   // DHM - Nerve
		client->sess.latchPlayerWeapon, // DHM - Nerve
		client->sess.latchPlayerItem,   // DHM - Nerve
		client->sess.latchPlayerSkin    // DHM - Nerve
		);

	var = va( "session%i", (int)(client - level.clients) );

	trap_Cvar_Set( var, s );
}

/*
================
G_ReadSessionData

Called on a reconnect
================
*/
void G_ReadSessionData( gclient_t *client ) {
	char s[MAX_STRING_CHARS];
	const char  *var;
	qboolean test;

	var = va( "session%i", (int)(client - level.clients) );
	trap_Cvar_VariableStringBuffer( var, s, sizeof( s ) );

	sscanf( s, "%i %i %i %i %i %i %i %i %i %i %i %i %i %i %i",      // DHM - Nerve
		(int *)&client->sess.sessionTeam,
		&client->sess.spectatorTime,
		(int *)&client->sess.spectatorState,
		&client->sess.spectatorClient,
		&client->sess.wins,
		&client->sess.losses,
		&client->sess.playerType,       // DHM - Nerve
		&client->sess.playerWeapon,     // DHM - Nerve
		&client->sess.playerItem,       // DHM - Nerve
		&client->sess.playerSkin,       // DHM - Nerve
		&client->sess.spawnObjectiveIndex, // DHM - Nerve
		&client->sess.latchPlayerType,  // DHM - Nerve
		&client->sess.latchPlayerWeapon, // DHM - Nerve
		&client->sess.latchPlayerItem,  // DHM - Nerve
		&client->sess.latchPlayerSkin   // DHM - Nerve
		);

	// NERVE - SMF
	if ( g_altStopwatchMode.integer ) {
		test = qtrue;
	} else {
		test = g_currentRound.integer == 1;
	}

	if ( g_gametype.integer == GT_WOLF_STOPWATCH && level.warmupTime > 0 && test ) {
		if ( client->sess.sessionTeam == TEAM_RED ) {
			client->sess.sessionTeam = TEAM_BLUE;
		} else if ( client->sess.sessionTeam == TEAM_BLUE ) {
			client->sess.sessionTeam = TEAM_RED;
		}
	}

	if ( g_swapteams.integer ) {
		trap_Cvar_Set( "g_swapteams", "0" );

		if ( client->sess.sessionTeam == TEAM_RED ) {
			client->sess.sessionTeam = TEAM_BLUE;
		} else if ( client->sess.sessionTeam == TEAM_BLUE ) {
			client->sess.sessionTeam = TEAM_RED;
		}
	}
}

/*
================
G_InitSessionData

Called on a first-time connect
================
*/
void G_InitSessionData( gclient_t *client, char *userinfo ) {
	clientSession_t *sess;
	const char      *value;

	sess = &client->sess;

	// initial team determination
	if ( g_gametype.integer >= GT_TEAM ) {
		// always spawn as spectator in team games
		sess->sessionTeam = TEAM_SPECTATOR;
	} else {
		value = Info_ValueForKey( userinfo, "team" );
		if ( value[0] == 's' ) {
			// a willing spectator, not a waiting-in-line
			sess->sessionTeam = TEAM_SPECTATOR;
		} else {
			if ( g_maxGameClients.integer > 0 &&
				level.numNonSpectatorClients >= g_maxGameClients.integer ) {
					sess->sessionTeam = TEAM_SPECTATOR;
			} else {
				sess->sessionTeam = TEAM_FREE;
			}
		}
	}

	sess->spectatorState = SPECTATOR_FREE;
	sess->spectatorTime = level.time;

	// DHM - Nerve
	sess->latchPlayerType = sess->playerType = 0;
	sess->latchPlayerWeapon = sess->playerWeapon = 0;
	sess->latchPlayerItem = sess->playerItem = 0;
	sess->latchPlayerSkin = sess->playerSkin = 0;
	sess->botSuicide = qfalse;
	sess->botPush = qtrue;

	sess->spawnObjectiveIndex = 0;
	// dhm - end

	// credits
	sess->credits = 50;
	sess->currentBetTarget = 0;
	sess->currentBetAmount = 0;
	sess->betTime = 0;
	sess->buyTime = 0;

	G_WriteClientSessionData( client );
}

/*
==================
G_InitWorldSession

==================
*/
void G_InitWorldSession( void ) {
	char s[MAX_STRING_CHARS];
	int gt;

	trap_Cvar_VariableStringBuffer( "session", s, sizeof( s ) );
	gt = atoi( s );

	// if the gametype changed since the last session, don't use any
	// client sessions
	if ( g_gametype.integer != gt ) {
		level.newSession = qtrue;
		G_Printf( "Gametype changed, clearing session data.\n" );
	}
}

/*
==================
G_WriteSessionData

==================
*/
void G_WriteSessionData( void ) {
	int i;

	trap_Cvar_Set( "session", va( "%i", g_gametype.integer ) );

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[i].pers.connected == CON_CONNECTED ) {
			G_WriteClientSessionData( &level.clients[i] );
		}
	}
}