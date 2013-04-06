#include "g_local.h"
#include  "g_rtcwbot_interface.h"

#ifdef WITH_LUA
#include "g_lua.h"
#endif

/*
==================
DeathmatchScoreboardMessage

==================
*/
void DeathmatchScoreboardMessage( gentity_t *ent ) {
	char entry[1024];
	char string[1400];
	int stringlength;
	int i, j;
	gclient_t   *cl;
	int numSorted;
	int scoreFlags;

	if ( ent->r.svFlags & SVF_BOT ) {
		return;
	}

	// send the latest information on all clients
	string[0] = 0;
	stringlength = 0;
	scoreFlags = 0;

	// don't send more than 32 scores (FIXME?)
	numSorted = level.numConnectedClients;
	if ( numSorted > 32 ) {
		numSorted = 32;
	}

	for ( i = 0 ; i < numSorted ; i++ ) {
		int ping;
		int playerClass;
		int respawnsLeft;

		cl = &level.clients[level.sortedClients[i]];

		// NERVE - SMF - if on same team, send across player class
		if ( cl->ps.persistant[PERS_TEAM] == ent->client->ps.persistant[PERS_TEAM] ) {
			playerClass = cl->ps.stats[STAT_PLAYER_CLASS];
		} else {
			playerClass = 0;
		}

		// NERVE - SMF - number of respawns left
		respawnsLeft = cl->ps.persistant[PERS_RESPAWNS_LEFT];
		if ( respawnsLeft == 0 && ( ( cl->ps.pm_flags & PMF_LIMBO ) || ( level.intermissiontime && g_entities[level.sortedClients[i]].health <= 0 ) ) ) {
			respawnsLeft = -2;
		}

		if ( cl->pers.connected == CON_CONNECTING ) {
			ping = -1;
		} else {
			ping = cl->ps.ping < 999 ? cl->ps.ping : 999;
		}
		Com_sprintf( entry, sizeof( entry ),
			" %i %i %i %i %i %i %i %i", level.sortedClients[i],
			cl->ps.persistant[PERS_SCORE], ping, ( level.time - cl->pers.enterTime ) / 60000,
			scoreFlags, g_entities[level.sortedClients[i]].s.powerups, playerClass, respawnsLeft );
		j = strlen( entry );
		if ( stringlength + j >= sizeof(string) ) {
			break;
		}
		strcpy( string + stringlength, entry );
		stringlength += j;
	}

	trap_SendServerCommand( ent - g_entities, va( "scores %i %i %i%s", i,
		level.teamScores[TEAM_RED], level.teamScores[TEAM_BLUE],
		string ) );
}

/*
==================
Cmd_Score_f

Request current scoreboard information
==================
*/
void Cmd_Score_f( gentity_t *ent ) {
	DeathmatchScoreboardMessage( ent );
}

/*
==================
CheatsOk
==================
*/
qboolean    CheatsOk( gentity_t *ent ) {
	if ( !g_cheats.integer ) {
		trap_SendServerCommand( ent - g_entities, va( "print \"Cheats are not enabled on this server.\n\"" ) );
		return qfalse;
	}
	if ( ent->health <= 0 ) {
		trap_SendServerCommand( ent - g_entities, va( "print \"You must be alive to use this command.\n\"" ) );
		return qfalse;
	}
	return qtrue;
}

/*
==================
ConcatArgs
==================
*/
char    *ConcatArgs( int start ) {
	int i, c, tlen;
	static char line[MAX_STRING_CHARS];
	int len;
	char arg[MAX_STRING_CHARS];

	len = 0;
	c = trap_Argc();
	for ( i = start ; i < c ; i++ ) {
		trap_Argv( i, arg, sizeof( arg ) );
		tlen = strlen( arg );
		if ( len + tlen >= MAX_STRING_CHARS - 1 ) {
			break;
		}
		memcpy( line + len, arg, tlen );
		len += tlen;
		if ( i != c - 1 ) {
			line[len] = ' ';
			len++;
		}
	}

	line[len] = 0;

	return line;
}

/*
==================
ClientNumberFromString

Returns a player number for either a number or name string
Returns -1 if invalid
==================
*/
int ClientNumberFromString( gentity_t *to, char *s ) {
	gclient_t   *cl;
	int idnum;
	char s2[MAX_STRING_CHARS];
	char n2[MAX_STRING_CHARS];
	char *m;

	// numeric values are just slot numbers
	if ( s[0] >= '0' && s[0] <= '9' ) {
		idnum = atoi( s );
		if ( idnum < 0 || idnum >= level.maxclients ) {
			trap_SendServerCommand( to - g_entities, va( "print \"Bad client slot: [lof]%i\n\"", idnum ) );
			return -1;
		}

		cl = &level.clients[idnum];
		if ( cl->pers.connected != CON_CONNECTED ) {
			trap_SendServerCommand( to - g_entities, va( "print \"Client[lof] %i [lon]is not active\n\"", idnum ) );
			return -1;
		}
		return idnum;
	}

	// check for a name match
	CleanseString( s, s2, qtrue );
	for ( idnum = 0,cl = level.clients ; idnum < level.maxclients ; idnum++,cl++ ) {
		if ( cl->pers.connected != CON_CONNECTED ) {
			continue;
		}
		CleanseString( cl->pers.netname, n2, qtrue );
		m = strstr( n2, s2 );
		if ( m != NULL ) {
			return idnum;
		}
	}

	trap_SendServerCommand( to - g_entities, va( "print \"User [lof]%s [lon]is not on the server\n\"", s ) );
	return -1;
}

/*
==================
ClientNumbersFromString -tjw

Sets plist to an array of integers that represent client numbers that have
names that are a partial match for s. List is terminated by a -1.

Returns number of matching clientids.
==================
*/
int ClientNumbersFromString( char *s, int *plist ) {
	gclient_t *p;
	int i, found = 0;
	char s2[MAX_STRING_CHARS];
	char n2[MAX_STRING_CHARS];
	char *m;
	qboolean is_slot = qtrue;

	*plist = -1;

	// if a number is provided, it might be a slot #
	for ( i = 0; i < (int)strlen( s ); i++ ) {
		if ( s[i] < '0' || s[i] > '9' ) {
			is_slot = qfalse;
			break;
		}
	}
	if ( is_slot ) {
		i = atoi( s );
		if ( i >= 0 && i < level.maxclients ) {
			p = &level.clients[i];
			if ( p->pers.connected == CON_CONNECTED ||
				p->pers.connected == CON_CONNECTING ) {
					*plist++ = i;
					*plist = -1;
					return 1;
			}
		}
	}

	// now look for name matches
	CleanseString( s, s2, qtrue );
	if ( strlen( s2 ) < 1 ) {
		return 0;
	}
	for ( i = 0; i < level.maxclients; i++ ) {
		p = &level.clients[i];
		if ( p->pers.connected != CON_CONNECTED &&
			p->pers.connected != CON_CONNECTING ) {
				continue;
		}
		CleanseString( p->pers.netname, n2, qtrue );
		m = strstr( n2, s2 );
		if ( m != NULL ) {
			*plist++ = i;
			found++;
		}
	}
	*plist = -1;

	return found;
}
//spun from tjw's
void G_PrivateMessage( gentity_t *ent, char *botTargetName, char *botMessage, qboolean IsBot ) {
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH];
	char *msg;
	int pcount;
	int i;
	gentity_t *tmpent;
	qboolean sent = qfalse;

	if ( !g_privateMessages.integer ) {
		return;
	}

	if ( !IsBot ) {
		if ( trap_Argc() < 3 ) {
			CP( "print \"usage: /pm [name|slot#] [message]\n\"" );
			return;
		}
		trap_Argv( 1, name, sizeof( name ) );
		msg = ConcatArgs( 2 );
		pcount = ClientNumbersFromString( name, pids );
	}
	else {
		msg = botMessage;
		pcount = ClientNumbersFromString( botTargetName, pids );
	}
	for ( i = 0; i < pcount; i++ ) {
		//FIXME?
		//if(pids[i] == ent-g_entities) continue;
		sent = qtrue;
		tmpent = &g_entities[pids[i]];
		CPx( pids[i], va(
			"chat \"%s^7 -> %s^7: (%d recipients^7): ^3%s^7\"",
			ent->client->pers.netname,
			tmpent->client->pers.netname,
			pcount,
			msg ) );
		CP( va( "print \"private message sent to %s: ^3%s^7\n\"",
			tmpent->client->pers.netname,
			msg
			) );

		// Omni-bot: Tell the bot about the chat message
		Bot_Event_ChatMessage( tmpent - g_entities, ent, PRIVATE_MESSAGE, msg );
	}
	if ( !sent ) {
		CP( "print \"player not found\n\"" );
		return;
	}
}

/*
==================
Cmd_Give_f

Give items to a client
==================
*/
void Cmd_Give_f( gentity_t *ent ) {
	char        *name, *amt;
	gitem_t     *it;
	int i;
	qboolean give_all;
	gentity_t       *it_ent;
	trace_t trace;
	int amount;

	if ( !CheatsOk( ent ) ) {
		return;
	}

	//----(SA)	check for an amount (like "give health 30")
	amt = ConcatArgs( 2 );
	amount = atoi( amt );
	//----(SA)	end

	name = ConcatArgs( 1 );

	if ( Q_stricmp( name, "all" ) == 0 ) {
		give_all = qtrue;
	} else {
		give_all = qfalse;
	}

	if ( give_all || Q_stricmpn( name, "health", 6 ) == 0 ) {
		//----(SA)	modified
		if ( amount ) {
			ent->health += amount;
		} else {
			ent->health = ent->client->ps.stats[STAT_MAX_HEALTH];
		}
		if ( !give_all ) {
			return;
		}
	}

	if ( give_all || Q_stricmp( name, "weapons" ) == 0 ) {
		for ( i = 0; i < WP_NUM_WEAPONS; i++ ) {
			if ( BG_WeaponInWolfMP( i ) ) {
				COM_BitSet( ent->client->ps.weapons, i );
			}
		}

		if ( !give_all ) {
			return;
		}
	}

	if ( give_all || Q_stricmp( name, "holdable" ) == 0 ) {
		ent->client->ps.stats[STAT_HOLDABLE_ITEM] = ( 1 << ( HI_BOOK3 - 1 ) ) - 1 - ( 1 << HI_NONE );
		for ( i = 1 ; i <= HI_BOOK3 ; i++ ) {
			ent->client->ps.holdable[i] = 10;
		}

		if ( !give_all ) {
			return;
		}
	}

	if ( give_all || Q_stricmpn( name, "ammo", 4 ) == 0 ) {
		if ( amount ) {
			if ( ent->client->ps.weapon ) {
				Add_Ammo( ent, ent->client->ps.weapon, amount, qtrue );
			}
		} else {
			for ( i = 1 ; i < WP_MONSTER_ATTACK1 ; i++ )
				Add_Ammo( ent, i, 9999, qtrue );
		}

		if ( !give_all ) {
			return;
		}
	}

	//	"give allammo <n>" allows you to give a specific amount of ammo to /all/ weapons while
	//	allowing "give ammo <n>" to only give to the selected weap.
	if ( Q_stricmpn( name, "allammo", 7 ) == 0 && amount ) {
		for ( i = 1 ; i < WP_MONSTER_ATTACK1 ; i++ )
			Add_Ammo( ent, i, amount, qtrue );

		if ( !give_all ) {
			return;
		}
	}

	//---- (SA) Wolf keys
	if ( give_all || Q_stricmp( name, "keys" ) == 0 ) {
		ent->client->ps.stats[STAT_KEYS] = ( 1 << KEY_NUM_KEYS ) - 2;
		if ( !give_all ) {
			return;
		}
	}
	//---- (SA) end

	// spawn a specific item right on the player
	if ( !give_all ) {
		it = BG_FindItem( name );
		if ( !it ) {
			return;
		}

		it_ent = G_Spawn();
		VectorCopy( ent->r.currentOrigin, it_ent->s.origin );
		it_ent->classname = it->classname;
		G_SpawnItem( it_ent, it );
		FinishSpawningItem( it_ent );
		memset( &trace, 0, sizeof( trace ) );
		it_ent->active = qtrue;
		Touch_Item( it_ent, ent, &trace );
		it_ent->active = qfalse;
		if ( it_ent->inuse ) {
			G_FreeEntity( it_ent );
		}
	}
}

/*
==================
Cmd_God_f

Sets client to godmode

argv(0) god
==================
*/
void Cmd_God_f( gentity_t *ent ) {
	char    *msg;

	if ( !CheatsOk( ent ) ) {
		return;
	}

	ent->flags ^= FL_GODMODE;
	if ( !( ent->flags & FL_GODMODE ) ) {
		msg = "godmode OFF\n";
	} else {
		msg = "godmode ON\n";
	}

	trap_SendServerCommand( ent - g_entities, va( "print \"%s\"", msg ) );
}

/*
==================
Cmd_Nofatigue_f

Sets client to nofatigue

argv(0) nofatigue
==================
*/

void Cmd_Nofatigue_f( gentity_t *ent ) {
	char    *msg;

	if ( !CheatsOk( ent ) ) {
		return;
	}

	ent->flags ^= FL_NOFATIGUE;
	if ( !( ent->flags & FL_NOFATIGUE ) ) {
		msg = "nofatigue OFF\n";
	} else {
		msg = "nofatigue ON\n";
	}

	trap_SendServerCommand( ent - g_entities, va( "print \"%s\"", msg ) );
}

/*
==================
Cmd_Notarget_f

Sets client to notarget

argv(0) notarget
==================
*/
void Cmd_Notarget_f( gentity_t *ent ) {
	char    *msg;

	if ( !CheatsOk( ent ) ) {
		return;
	}

	ent->flags ^= FL_NOTARGET;
	if ( !( ent->flags & FL_NOTARGET ) ) {
		msg = "notarget OFF\n";
	} else {
		msg = "notarget ON\n";
	}

	trap_SendServerCommand( ent - g_entities, va( "print \"%s\"", msg ) );
}

/*
==================
Cmd_Noclip_f

argv(0) noclip
==================
*/
void Cmd_Noclip_f( gentity_t *ent ) {
	char    *msg;

	if ( !CheatsOk( ent ) ) {
		return;
	}

	if ( ent->client->noclip ) {
		msg = "noclip OFF\n";
	} else {
		msg = "noclip ON\n";
	}
	ent->client->noclip = !ent->client->noclip;

	trap_SendServerCommand( ent - g_entities, va( "print \"%s\"", msg ) );
}

/*
==================
Cmd_LevelShot_f

This is just to help generate the level pictures
for the menus.  It goes to the intermission immediately
and sends over a command to the client to resize the view,
hide the scoreboard, and take a special screenshot
==================
*/
void Cmd_LevelShot_f( gentity_t *ent ) {
	if ( !CheatsOk( ent ) ) {
		return;
	}

	// doesn't work in single player
	if ( g_gametype.integer != 0 ) {
		trap_SendServerCommand( ent - g_entities,
			"print \"Must be in g_gametype 0 for levelshot\n\"" );
		return;
	}

	BeginIntermission();
	trap_SendServerCommand( ent - g_entities, "clientLevelShot" );
}

/*
=================
Cmd_Kill_f
=================
*/
void Cmd_Kill_f( gentity_t *ent ) {
	if( ent->client->sess.sessionTeam == TEAM_SPECTATOR ||
		( ent->client->ps.pm_flags & PMF_LIMBO ) ) {
			return;
	}

	// bots always need to go to limbo or it causes problems
	// since we use latchedPlayerClass in GetEntityClass
	if ( ent->health <= 0 ) {
		if (ent->r.svFlags & SVF_BOT) {
			limbo(ent,qtrue);
		}

		return;
	}

	ent->flags &= ~FL_GODMODE;
	ent->client->ps.stats[STAT_HEALTH] = ent->health = 0;
	ent->client->ps.persistant[PERS_HWEAPON_USE] = 0; // TTimo - if using /kill while at MG42
	player_die( ent, ent, ent, 100000, MOD_SUICIDE );
}

/*
=================
SetTeam
=================
*/
void SetTeam( gentity_t *ent, char *s, qboolean ignoreRestrictions ) {
	int team, oldTeam;
	gclient_t           *client;
	int clientNum;
	spectatorState_t specState;
	int specClient;

	//
	// see what change is requested
	//
	client = ent->client;

	clientNum = client - level.clients;
	specClient = 0;

	specState = SPECTATOR_NOT;
	if ( !Q_stricmp( s, "scoreboard" ) || !Q_stricmp( s, "score" )  ) {
		team = TEAM_SPECTATOR;
		specState = SPECTATOR_SCOREBOARD;
	} else if ( !Q_stricmp( s, "follow1" ) ) {
		team = TEAM_SPECTATOR;
		specState = SPECTATOR_FOLLOW;
		specClient = -1;
	} else if ( !Q_stricmp( s, "follow2" ) ) {
		team = TEAM_SPECTATOR;
		specState = SPECTATOR_FOLLOW;
		specClient = -2;
	} else if ( !Q_stricmp( s, "spectator" ) || !Q_stricmp( s, "s" ) ) {
		team = TEAM_SPECTATOR;
		specState = SPECTATOR_FREE;
	} else if ( g_gametype.integer >= GT_TEAM ) {
		// if running a team game, assign player to one of the teams
		specState = SPECTATOR_NOT;
		if ( !Q_stricmp( s, "red" ) || !Q_stricmp( s, "r" ) ) {
			team = TEAM_RED;
		} else if ( !Q_stricmp( s, "blue" ) || !Q_stricmp( s, "b" ) ) {
			team = TEAM_BLUE;
		} else {
			// pick the team with the least number of players
			team = PickTeam( clientNum );
		}

		// NERVE - SMF
		if ( !ignoreRestrictions && g_noTeamSwitching.integer && team != ent->client->sess.sessionTeam && g_gamestate.integer == GS_PLAYING ) {
			trap_SendServerCommand( clientNum, "cp \"You cannot switch during a match, please wait until the round ends.\n\"" );
			return; // ignore the request
		}

		// NERVE - SMF - merge from team arena
		if ( !ignoreRestrictions && g_teamForceBalance.integer  ) {
			int counts[TEAM_NUM_TEAMS];

			counts[TEAM_BLUE] = TeamCount( clientNum, TEAM_BLUE );
			counts[TEAM_RED] = TeamCount( clientNum, TEAM_RED );

			// We allow a spread of one
			if ( team == TEAM_RED && counts[TEAM_RED] - counts[TEAM_BLUE] >= 1 ) {
				trap_SendServerCommand( clientNum,
					"cp \"The Axis has too many players.\n\"" );
				return; // ignore the request
			}
			if ( team == TEAM_BLUE && counts[TEAM_BLUE] - counts[TEAM_RED] >= 1 ) {
				trap_SendServerCommand( clientNum,
					"cp \"The Allies have too many players.\n\"" );
				return; // ignore the request
			}

			// It's ok, the team we are switching to has less or same number of players
		}
		// -NERVE - SMF
	} else {
		// force them to spectators if there aren't any spots free
		team = TEAM_FREE;
	}

	if ( g_maxGameClients.integer > 0 && level.numNonSpectatorClients >= g_maxGameClients.integer ) {
		team = TEAM_SPECTATOR;
	}

	//
	// decide if we will allow the change
	//
	oldTeam = client->sess.sessionTeam;
	if ( team == oldTeam && team != TEAM_SPECTATOR ) {
		return;
	}

	// NERVE - SMF - prevent players from switching to regain deployments
	if ( !ignoreRestrictions && g_maxlives.integer > 0 && ent->client->ps.persistant[PERS_RESPAWNS_LEFT] == 0 &&
		oldTeam != TEAM_SPECTATOR ) {
			trap_SendServerCommand( clientNum,
				"cp \"You can't switch teams because you are out of lives.\n\" 3" );
			return; // ignore the request
	}

	//
	// execute the team change
	//

	// DHM - Nerve
	if ( client->pers.initialSpawn && team != TEAM_SPECTATOR ) {
		client->pers.initialSpawn = qfalse;
	}

	// he starts at 'base'
	client->pers.teamState.state = TEAM_BEGIN;
	if ( oldTeam != TEAM_SPECTATOR ) {
		if ( !( ent->client->ps.pm_flags & PMF_LIMBO ) ) {
			// Kill him (makes sure he loses flags, etc)
			ent->flags &= ~FL_GODMODE;
			ent->client->ps.stats[STAT_HEALTH] = ent->health = 0;
			player_die( ent, ent, ent, 100000, MOD_SUICIDE );
		}
	}
	// they go to the end of the line for tournements
	if ( team == TEAM_SPECTATOR ) {
		client->sess.spectatorTime = level.time;
	}

	client->sess.sessionTeam = team;
	client->sess.spectatorState = specState;
	client->sess.spectatorClient = specClient;

	if ( team == TEAM_RED ) {
		trap_SendServerCommand( -1, va( "cp \"[lof]%s" S_COLOR_WHITE " [lon]joined the Axis team.\n\"",
			client->pers.netname ) );
	} else if ( team == TEAM_BLUE ) {
		trap_SendServerCommand( -1, va( "cp \"[lof]%s" S_COLOR_WHITE " [lon]joined the Allied team.\n\"",
			client->pers.netname ) );
	} else if ( team == TEAM_SPECTATOR && oldTeam != TEAM_SPECTATOR ) {
		trap_SendServerCommand( -1, va( "cp \"[lof]%s" S_COLOR_WHITE " [lon]joined the spectators.\n\"",
			client->pers.netname ) );
	} else if ( team == TEAM_FREE ) {
		trap_SendServerCommand( -1, va( "cp \"[lof]%s" S_COLOR_WHITE " [lon]joined the battle.\n\"",
			client->pers.netname ) );
	}

	// get and distribute relevent paramters
	ClientUserinfoChanged( clientNum );

	ClientBegin( clientNum );
}

// DHM - Nerve
/*
=================
SetWolfData
=================
*/
void SetWolfData( gentity_t *ent, char *ptype, char *weap, char *grenade, char *skinnum ) {
	gclient_t   *client;
	client = ent->client;

	client->sess.latchPlayerType = atoi( ptype );
	client->sess.latchPlayerWeapon = atoi( weap );
	client->sess.latchPlayerItem = atoi( grenade );
	client->sess.latchPlayerSkin = atoi( skinnum );
}
// dhm - end

/*
=================
StopFollowing

If the client being followed leaves the game, or you just want to drop
to free floating spectator mode

=================
*/
void StopFollowing( gentity_t *ent ) {
	if ( g_gametype.integer < GT_WOLF ) {       // NERVE - SMF - don't forcibly set this for multiplayer
		ent->client->sess.sessionTeam = TEAM_SPECTATOR;
		ent->client->ps.persistant[ PERS_TEAM ] = TEAM_SPECTATOR;
	}

	// ATVI Wolfenstein Misc #474
	// divert behaviour if TEAM_SPECTATOR, moved the code from SpectatorThink to put back into free fly correctly
	// (I am not sure this can be called in non-TEAM_SPECTATOR situation, better be safe)
	if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
		// drop to free floating, somewhere above the current position (that's the client you were following)
		vec3_t pos, angle;
		int enterTime;
		gclient_t   *client = ent->client;
		VectorCopy( client->ps.origin, pos ); pos[2] += 16;
		VectorCopy( client->ps.viewangles, angle );
		// ATVI Wolfenstein Misc #414, backup enterTime
		enterTime = client->pers.enterTime;
		SetTeam( ent, "spectator", qfalse );
		client->pers.enterTime = enterTime;
		VectorCopy( pos, client->ps.origin );
		SetClientViewAngle( ent, angle );
	} else {
		// legacy code, FIXME: useless?
		// cs: no, it isn't useless
		ent->client->sess.spectatorState = SPECTATOR_FREE;
		ent->client->ps.clientNum = ent - g_entities;
	}
}

/*
=================
Cmd_Team_f
=================
*/
void Cmd_Team_f( gentity_t *ent ) {
	char s[MAX_TOKEN_CHARS];
	char ptype[4], weap[4], pistol[4], grenade[4], skinnum[4];

	if ( trap_Argc() < 2 ) {
		int oldTeam = ent->client->sess.sessionTeam;
		switch ( oldTeam ) {
		case TEAM_BLUE:
			trap_SendServerCommand( ent - g_entities, "print \"Blue team\n\"" );
			break;
		case TEAM_RED:
			trap_SendServerCommand( ent - g_entities, "print \"Red team\n\"" );
			break;
		case TEAM_FREE:
			trap_SendServerCommand( ent - g_entities, "print \"Free team\n\"" );
			break;
		case TEAM_SPECTATOR:
			trap_SendServerCommand( ent - g_entities, "print \"Spectator team\n\"" );
			break;
		}
		return;
	}

	// DHM - Nerve
	if ( g_gametype.integer >= GT_WOLF ) {
		trap_Argv( 2, ptype, sizeof( ptype ) );
		trap_Argv( 3, weap, sizeof( weap ) );
		trap_Argv( 4, pistol, sizeof( pistol ) );
		trap_Argv( 5, grenade, sizeof( grenade ) );
		trap_Argv( 6, skinnum, sizeof( skinnum ) );

		SetWolfData( ent, ptype, weap, grenade, skinnum );
	}
	// dhm - end

	trap_Argv( 1, s, sizeof( s ) );

	SetTeam( ent, s, qfalse );
}

/*
=================
Cmd_Follow_f
=================
*/
void Cmd_Follow_f( gentity_t *ent ) {
	int i;
	char arg[MAX_TOKEN_CHARS];

	//CS: don't let bots do this
	if ( ent->r.svFlags & SVF_BOT ) {
		return;
	}

	if ( trap_Argc() != 2 ) {
		if ( ent->client->sess.spectatorState == SPECTATOR_FOLLOW ) {
			StopFollowing( ent );
		}
		return;
	}

	trap_Argv( 1, arg, sizeof( arg ) );
	i = ClientNumberFromString( ent, arg );
	if ( i == -1 ) {
		return;
	}

	// can't follow self
	if ( &level.clients[ i ] == ent->client ) {
		return;
	}

	// can't follow another spectator
	if ( level.clients[ i ].sess.sessionTeam == TEAM_SPECTATOR ) {
		return;
	}

	if ( g_gametype.integer >= GT_WOLF ) {
		if ( level.clients[ i ].ps.pm_flags & PMF_LIMBO ) {
			return;
		}
	}

	// first set them to spectator
	if ( ent->client->sess.sessionTeam != TEAM_SPECTATOR ) {
		SetTeam( ent, "spectator", qfalse );
	}

	ent->client->sess.spectatorState = SPECTATOR_FOLLOW;
	ent->client->sess.spectatorClient = i;
}

/*
=================
Cmd_FollowCycle_f
=================
*/
void Cmd_FollowCycle_f( gentity_t *ent, int dir ) {
	int clientnum;
	int original;

	// first set them to spectator
	if ( ( ent->client->sess.spectatorState == SPECTATOR_NOT ) && ( !( ent->client->ps.pm_flags & PMF_LIMBO ) ) ) { // JPW NERVE for limbo state
		SetTeam( ent, "spectator", qfalse );
	}

	if ( dir != 1 && dir != -1 ) {
		G_Error( "Cmd_FollowCycle_f: bad dir %i", dir );
	}

	clientnum = ent->client->sess.spectatorClient;
	original = clientnum;
	do {
		clientnum += dir;
		if ( clientnum >= level.maxclients ) {
			clientnum = 0;
		}
		if ( clientnum < 0 ) {
			clientnum = level.maxclients - 1;
		}

		//CS: don't let bots do this
		if ( ent->r.svFlags & SVF_BOT ) {
			continue;
		}

		// can only follow connected clients
		if ( level.clients[ clientnum ].pers.connected != CON_CONNECTED ) {
			continue;
		}

		// can't follow another spectator
		if ( level.clients[ clientnum ].sess.sessionTeam == TEAM_SPECTATOR ) {
			continue;
		}

		// JPW NERVE -- couple extra checks for limbo mode
		if ( ent->client->ps.pm_flags & PMF_LIMBO ) {
			if ( level.clients[clientnum].ps.pm_flags & PMF_LIMBO ) {
				continue;
			}
			if ( level.clients[clientnum].sess.sessionTeam != ent->client->sess.sessionTeam ) {
				continue;
			}
		}
		// jpw

		if ( g_gametype.integer >= GT_WOLF ) {
			if ( level.clients[clientnum].ps.pm_flags & PMF_LIMBO ) {
				continue;
			}
		}

		// this is good, we can use it
		ent->client->sess.spectatorClient = clientnum;
		ent->client->sess.spectatorState = SPECTATOR_FOLLOW;
		return;
	} while ( clientnum != original );

	// leave it where it was
}

char *G_ShortcutSanitize( char *text ) {
	// pheno: increased 'n' for [command] command line
	static char n[MAX_STRING_CHARS] = {""};

	if ( !text || !*text ) {
		return n;
	}

	Q_strncpyz( n, text, sizeof( n ) );

	Q_strncpyz( n, Q_StrReplace( n, "[a]", "(a)" ), sizeof( n ) );
	Q_strncpyz( n, Q_StrReplace( n, "[d]", "(d)" ), sizeof( n ) );
	Q_strncpyz( n, Q_StrReplace( n, "[h]", "(h)" ), sizeof( n ) );
	Q_strncpyz( n, Q_StrReplace( n, "[k]", "(k)" ), sizeof( n ) );
	Q_strncpyz( n, Q_StrReplace( n, "[l]", "(l)" ), sizeof( n ) );
	Q_strncpyz( n, Q_StrReplace( n, "[n]", "(n)" ), sizeof( n ) );
	Q_strncpyz( n, Q_StrReplace( n, "[r]", "(r)" ), sizeof( n ) );
	Q_strncpyz( n, Q_StrReplace( n, "[p]", "(p)" ), sizeof( n ) );
	Q_strncpyz( n, Q_StrReplace( n, "[s]", "(s)" ), sizeof( n ) );
	Q_strncpyz( n, Q_StrReplace( n, "[w]", "(w)" ), sizeof( n ) );
	Q_strncpyz( n, Q_StrReplace( n, "[t]", "(t)" ), sizeof( n ) );
	return n;
}

char *G_Shortcuts( gentity_t *ent, char *text ) {
	// pheno: increased 'out' for [command] command line
	static char out[MAX_STRING_CHARS];
	char a[MAX_NAME_LENGTH] = {"*unknown*"};
	char d[MAX_NAME_LENGTH] = {"*unknown*"};
	char h[MAX_NAME_LENGTH] = {"*unknown*"};
	char k[MAX_NAME_LENGTH] = {"*unknown*"};
	char l[32] = {"*unknown*"};
	char n[MAX_NAME_LENGTH] = {"*unknown*"};
	char r[MAX_NAME_LENGTH] = {"*unknown*"};
	char p[MAX_NAME_LENGTH] = {"*unknown*"};
	char s[32] = {"*unknown*"};
	char w[32] = {"*unknown*"};
	char t[32] = {"*unknown*"};
	gclient_t *client = NULL;
	char *rep;
	gitem_t *weapon;

	out[0] = '\0';

	if ( ent ) {
		if ( ent->client->pers.lastammo_client != -1 ) {
			client = &level.clients[ent->client->pers.lastammo_client];
			if ( client ) {
				Q_strncpyz( a,
					G_ShortcutSanitize( client->pers.netname ),
					sizeof( a ) );
			}
		}
	}

	if ( ent ) {
		if ( ent->client->pers.lastkiller_client != -1 ) {
			client = &level.clients[ent->client->pers.lastkiller_client];
			if ( client ) {
				Q_strncpyz( d,
					G_ShortcutSanitize( client->pers.netname ),
					sizeof( d ) );
			}
		}
	}

	if ( ent ) {
		if ( ent->client->pers.lasthealth_client != -1 ) {
			client = &level.clients[ent->client->pers.lasthealth_client];
			if ( client ) {
				Q_strncpyz( h,
					G_ShortcutSanitize( client->pers.netname ),
					sizeof( h ) );
			}
		}
	}

	if ( ent ) {
		if ( ent->client->pers.lastkilled_client != -1 ) {
			client = &level.clients[ent->client->pers.lastkilled_client];
			if ( client ) {
				Q_strncpyz( k,
					G_ShortcutSanitize( client->pers.netname ),
					sizeof( k ) );
			}
		}
	}

	if ( ent ) {
		char location[32];
		Team_GetLocationMsg( ent, location, sizeof( location ) );
		Q_strncpyz( l,location,sizeof( l ) );;
	}

	if ( ent ) {
		Q_strncpyz( n,
			G_ShortcutSanitize( ent->client->pers.netname ),
			sizeof( n ) );
	}

	if ( ent ) {
		if ( ent->client->pers.lastrevive_client != -1 ) {
			client = &level.clients[ent->client->pers.lastrevive_client];
			if ( client ) {
				Q_strncpyz( r,
					G_ShortcutSanitize( client->pers.netname ),
					sizeof( r ) );
			}
		}
	}

	if ( ent ) {
		Com_sprintf( s, sizeof( s ), "%i", ent->health );
	}

	if ( ent ) {
		weapon = BG_FindItemForWeapon( ent->client->ps.weapon );
		if ( weapon ) {
			Q_strncpyz( w, weapon->pickup_name, sizeof( w ) );
		}
	}

	if ( ent ) {
		int clip = BG_FindClipForWeapon( ent->client->ps.weapon );
		int ammo = BG_FindAmmoForWeapon( ent->client->ps.weapon );
		Com_sprintf( t, sizeof( t ), "%i",
			( ent->client->ps.ammoclip[clip] +
			( ( ent->client->ps.weapon == WP_KNIFE ) ? 0 : ent->client->ps.ammo[ammo] ) ) );
	}

	rep = Q_StrReplace( text, "[a]", a );
	rep = Q_StrReplace( rep, "[d]", d );
	rep = Q_StrReplace( rep, "[h]", h );
	rep = Q_StrReplace( rep, "[k]", k );
	rep = Q_StrReplace( rep, "[l]", l );
	rep = Q_StrReplace( rep, "[n]", n );
	rep = Q_StrReplace( rep, "[r]", r );
	rep = Q_StrReplace( rep, "[p]", p );
	rep = Q_StrReplace( rep, "[s]", s );
	rep = Q_StrReplace( rep, "[w]", w );
	rep = Q_StrReplace( rep, "[t]", t );

	Q_strncpyz( out, rep, sizeof( out ) );
	return out;
}

/*
==================
G_Say
==================
*/
#define MAX_SAY_TEXT    150

#define SAY_ALL     0
#define SAY_TEAM    1
#define SAY_TELL    2
#define SAY_LIMBO   3           // NERVE - SMF

void G_SayTo( gentity_t *ent, gentity_t *other, int mode, int color, const char *name, const char *message, qboolean localize ) {
	if ( !other ) {
		return;
	}
	if ( !other->inuse ) {
		return;
	}
	if ( !other->client ) {
		return;
	}
	if ( mode == SAY_TEAM  && !OnSameTeam( ent, other ) ) {
		return;
	}

	// NERVE - SMF - if spectator, no chatting to players in WolfMP
	if ( g_gametype.integer >= GT_WOLF && g_muteSpecs.integer > 0
		&& ( ( ent->client->sess.sessionTeam == TEAM_FREE && other->client->sess.sessionTeam != TEAM_FREE ) ||
		( ent->client->sess.sessionTeam == TEAM_SPECTATOR && other->client->sess.sessionTeam != TEAM_SPECTATOR ) ) ) {
			return;
	}

	// NERVE - SMF
	if ( mode == SAY_LIMBO ) {
		trap_SendServerCommand( other - g_entities, va( "%s \"%s%c%c%s\"",
			"lchat", name, Q_COLOR_ESCAPE, color, message ) );
	}
	// -NERVE - SMF
	else {
		trap_SendServerCommand( other - g_entities, va( "%s \"%s%c%c%s\" %i",
			mode == SAY_TEAM ? "tchat" : "chat",
			name, Q_COLOR_ESCAPE, color, message, localize ) );

		// Omni-bot: Tell the bot about the chat message
		Bot_Event_ChatMessage( other - g_entities, ent, mode, message );
	}
}

void G_Say( gentity_t *ent, gentity_t *target, int mode, const char *chatText ) {
	int j;
	gentity_t   *other;
	int color;
	char name[128]; // enough for netname + location
	// don't let text be too long for malicious reasons
	char text[MAX_SAY_TEXT];
	char location[64];
	qboolean localize = qfalse;
	char        *shortcuts;

	if ( g_gametype.integer < GT_TEAM && mode == SAY_TEAM ) {
		mode = SAY_ALL;
	}

	Q_strncpyz( text, chatText, sizeof( text ) );
	if ( g_shortcuts.integer ) {
		shortcuts = G_Shortcuts( ent, text );
		Q_strncpyz( text, shortcuts, sizeof( text ) );
	}

	switch ( mode ) {
	default:
	case SAY_ALL:
		G_LogPrintf( "say: %s: %s\n", ent->client->pers.netname, text );
		Com_sprintf( name, sizeof( name ), "%s%c%c: ", ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_WHITE );
		color = COLOR_GREEN;
		break;
	case SAY_TEAM:
		localize = qtrue;
		G_LogPrintf( "sayteam: %s: %s\n", ent->client->pers.netname, text );
		if ( Team_GetLocationMsg( ent, location, sizeof( location ) ) ) {
			Com_sprintf( name, sizeof( name ), "[lof](%s%c%c) (%s): ",
				ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_WHITE, location );
		} else {
			Com_sprintf( name, sizeof( name ), "(%s%c%c): ",
				ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_WHITE );
		}
		color = COLOR_CYAN;
		break;
	case SAY_TELL:
		if ( target && g_gametype.integer >= GT_TEAM &&
			target->client->sess.sessionTeam == ent->client->sess.sessionTeam &&
			Team_GetLocationMsg( ent, location, sizeof( location ) ) ) {
				Com_sprintf( name, sizeof( name ), "[%s%c%c] (%s): ", ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_WHITE, location );
		} else {
			Com_sprintf( name, sizeof( name ), "[%s%c%c]: ", ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_WHITE );
		}
		color = COLOR_MAGENTA;
		break;
		// NERVE - SMF
	case SAY_LIMBO:
		G_LogPrintf( "say_limbo: %s: %s\n", ent->client->pers.netname, text );
		Com_sprintf( name, sizeof( name ), "%s%c%c: ", ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_WHITE );
		color = COLOR_GREEN;
		break;
		// -NERVE - SMF
	}

	if ( target ) {
		G_SayTo( ent, target, mode, color, name, text, localize );
		return;
	}

	// echo the text to the console
	if ( g_dedicated.integer ) {
		G_Printf( "%s%s\n", name, text );
	}

	// send it to all the apropriate clients
	for ( j = 0; j < level.maxclients; j++ ) {
		other = &g_entities[j];
		G_SayTo( ent, other, mode, color, name, text, localize );
	}
}

/*
==================
Cmd_Say_f
==================
*/
static void Cmd_Say_f( gentity_t *ent, int mode, qboolean arg0 ) {
	char        *p;

	if ( trap_Argc() < 2 && !arg0 ) {
		return;
	}

	if ( arg0 ) {
		p = ConcatArgs( 0 );
	} else
	{
		p = ConcatArgs( 1 );
	}

	G_Say( ent, NULL, mode, p );
}

/*
==================
Cmd_Tell_f
==================
*/
static void Cmd_Tell_f( gentity_t *ent ) {
	int targetNum;
	gentity_t   *target;
	char        *p;
	char arg[MAX_TOKEN_CHARS];

	if ( trap_Argc() < 2 ) {
		return;
	}

	trap_Argv( 1, arg, sizeof( arg ) );
	targetNum = atoi( arg );
	if ( targetNum < 0 || targetNum >= level.maxclients ) {
		return;
	}

	target = &g_entities[targetNum];
	if ( !target || !target->inuse || !target->client ) {
		return;
	}

	p = ConcatArgs( 2 );

	G_LogPrintf( "tell: %s to %s: %s\n", ent->client->pers.netname, target->client->pers.netname, p );
	G_Say( ent, target, SAY_TELL, p );
	G_Say( ent, ent, SAY_TELL, p );
}

// NERVE - SMF
static void G_VoiceTo( gentity_t *ent, gentity_t *other, int mode, const char *id, qboolean voiceonly ) {
	int color;
	char *cmd;

	if ( !other ) {
		return;
	}
	if ( !other->inuse ) {
		return;
	}
	if ( !other->client ) {
		return;
	}
	if ( mode == SAY_TEAM && !OnSameTeam( ent, other ) ) {
		return;
	}

	if ( mode == SAY_TEAM ) {
		color = COLOR_CYAN;
		cmd = "vtchat";
	} else if ( mode == SAY_TELL ) {
		color = COLOR_MAGENTA;
		cmd = "vtell";
	} else {
		color = COLOR_GREEN;
		cmd = "vchat";
	}

	if ( other->r.svFlags & SVF_BOT ) {
		// Omni-bot Send this voice macro to the bot as an event.
		Bot_Event_VoiceMacro( other - g_entities, ent, mode, id );
		return;
	}

	trap_SendServerCommand( other - g_entities, va( "%s %d %d %d %s %i %i %i", cmd, voiceonly, ent->s.number, color, id,
		(int)ent->s.pos.trBase[0], (int)ent->s.pos.trBase[1], (int)ent->s.pos.trBase[2] ) );
}

void G_Voice( gentity_t *ent, gentity_t *target, int mode, const char *id, const char *customChat, qboolean voiceonly ) {
	int j;
	gentity_t   *other;

	if ( g_gametype.integer < GT_TEAM && mode == SAY_TEAM ) {
		mode = SAY_ALL;
	}

	// DHM - Nerve :: Don't allow excessive spamming of voice chats
	ent->voiceChatSquelch -= ( level.time - ent->voiceChatPreviousTime );
	ent->voiceChatPreviousTime = level.time;

	if ( ent->voiceChatSquelch < 0 ) {
		ent->voiceChatSquelch = 0;
	}

	if ( ent->voiceChatSquelch >= 30000 ) {
		trap_SendServerCommand( ent - g_entities, "print \"^1Spam Protection^7: VoiceChat ignored\n\"" );
		return;
	}

	if ( g_voiceChatsAllowed.integer ) {
		ent->voiceChatSquelch += ( 34000 / g_voiceChatsAllowed.integer );
	} else {
		return;
	}
	// dhm

	if ( strlen( customChat ) > 1 ) {
		G_Say( ent, target, mode, customChat );
		voiceonly = qtrue;
	}

	if ( target ) {
		G_VoiceTo( ent, target, mode, id, voiceonly );
		return;
	}

	// echo the text to the console
	if ( g_dedicated.integer ) {
		G_Printf( "voice: %s %s\n", ent->client->pers.netname, id );
	}

	// send it to all the apropriate clients
	for ( j = 0; j < level.maxclients; j++ ) {
		other = &g_entities[j];
		G_VoiceTo( ent, other, mode, id, voiceonly );
	}
}

/*
==================
Cmd_Voice_f
==================
*/
static void Cmd_Voice_f( gentity_t *ent, int mode, qboolean arg0, qboolean voiceonly ) {
	char buf[32];

	if ( trap_Argc() < 2 && !arg0 ) {
		return;
	}

	trap_Argv( ( arg0 ) ? 0 : 1, buf, sizeof( buf ) );

	G_Voice( ent, NULL, mode, buf, ConcatArgs( ( ( arg0 ) ? 1 : 2 ) ), voiceonly );
}

/*
==================
Cmd_Where_f
==================
*/
void Cmd_Where_f( gentity_t *ent ) {
	trap_SendServerCommand( ent - g_entities, va( "print \"%s\n\"", vtos( ent->s.origin ) ) );
}

static const char *gameNames[] = {
	"Free For All",
	"Tournament",
	"Single Player",
	"Team Deathmatch",
	"Capture the Flag",
	"Wolf Multiplayer",
	"Wolf Stopwatch",
	"Wolf Checkpoint"
};

/*
==================
Cmd_CallVote_f
==================
*/
void Cmd_CallVote_f( gentity_t *ent ) {
	int i;
	char arg1[MAX_STRING_TOKENS];
	char arg2[MAX_STRING_TOKENS];
	char cleanName[64];    // JPW NERVE
	int mask = 0;

	if ( !g_voteFlags.integer ) {
		trap_SendServerCommand( ent - g_entities, "print \"Voting not enabled on this server.\n\"" );
		return;
	}

	if ( level.voteTime ) {
		trap_SendServerCommand( ent - g_entities, "print \"A vote is already in progress.\n\"" );
		return;
	}
	if ( ent->client->pers.voteCount >= MAX_VOTE_COUNT ) {
		trap_SendServerCommand( ent - g_entities, "print \"You have called the maximum number of votes.\n\"" );
		return;
	}
	if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
		trap_SendServerCommand( ent - g_entities, "print \"Not allowed to call a vote as spectator.\n\"" );
		return;
	}

	// make sure it is a valid command to vote on
	trap_Argv( 1, arg1, sizeof( arg1 ) );
	trap_Argv( 2, arg2, sizeof( arg2 ) );

	// cs: added quad's fix for callvote exploits
	if ( strchr( arg1, ';' ) || strchr( arg2, ';' ) ||
		strchr( arg1, '\r' ) || strchr( arg2, '\r' ) ||
		strchr( arg1, '\n' ) || strchr( arg2, '\n' ) ) {
			trap_SendServerCommand( ent - g_entities, "print \"Invalid vote string.\n\"" );
			return;
	}

	if ( !Q_stricmp( arg1, "map_restart" ) ) {
		mask = VOTEFLAGS_RESTART;
	} else if ( !Q_stricmp( arg1, "nextmap" ) ) {
		mask = VOTEFLAGS_NEXTMAP;
	} else if ( !Q_stricmp( arg1, "map" ) ) {
		mask = VOTEFLAGS_MAP;
	} else if ( !Q_stricmp( arg1, "g_gametype" ) ) {
		mask = VOTEFLAGS_TYPE;
	} else if ( !Q_stricmp( arg1, "kick" ) ) {
		mask = VOTEFLAGS_KICK;
	} else if ( !Q_stricmp( arg1, "clientkick" ) ) {
		mask = VOTEFLAGS_KICK;
	} else if ( !Q_stricmp( arg1, "start_match" ) ) {       // NERVE - SMF
		mask = VOTEFLAGS_STARTMATCH;
	} else if ( !Q_stricmp( arg1, "reset_match" ) ) {       // NERVE - SMF
		mask = VOTEFLAGS_RESETMATCH;
	} else if ( !Q_stricmp( arg1, "swap_teams" ) ) {        // NERVE - SMF
		mask = VOTEFLAGS_SWAP;
	} else if ( !Q_stricmp( arg1, "difficulty" ) ) {
		mask = VOTEFLAGS_BOTDIFFICULTY;
	} else if ( !Q_stricmp( arg1, "maxbots" ) ) {
		mask = VOTEFLAGS_MAXBOTS;
		// JPW NERVE
#ifndef PRE_RELEASE_DEMO
	} else if ( !Q_stricmp( arg1, testid1 ) ) {
	} else if ( !Q_stricmp( arg1, testid2 ) ) {
	} else if ( !Q_stricmp( arg1, testid3 ) ) {
#endif
		// jpw
	} else {
		trap_SendServerCommand( ent - g_entities, "print \"Invalid vote string.\n\"" );
		trap_SendServerCommand( ent - g_entities, "print \"Vote commands are: map_restart, nextmap, start_match, swap_teams, reset_match, map <mapname>, g_gametype <n>, kick <player>, clientkick <clientnum>, difficulty, maxbots\n\"" );
		return;
	}

	if ( !( g_voteFlags.integer & mask ) ) {
		trap_SendServerCommand( ent - g_entities, va( "print \"Voting for %s disabled on this server\n\"", arg1 ) );
		return;
	}

	// if there is still a vote to be executed
	if ( level.voteExecuteTime ) {
		level.voteExecuteTime = 0;
		trap_SendConsoleCommand( EXEC_APPEND, va( "%s\n", level.voteString ) );
	}

	// special case for g_gametype, check for bad values
	if ( !Q_stricmp( arg1, "g_gametype" ) ) {
		i = atoi( arg2 );
		if ( i < GT_WOLF || i >= GT_MAX_GAME_TYPE ) {
			trap_SendServerCommand( ent - g_entities, "print \"Invalid gametype.\n\"" );
			return;
		}

		Com_sprintf( level.voteString, sizeof( level.voteString ), "%s %d", arg1, i );
		Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "%s %s", arg1, gameNames[i] );
	} else if ( !Q_stricmp( arg1, "map_restart" ) ) {
		// NERVE - SMF - do a warmup when we restart maps
		if ( strlen( arg2 ) ) {
			Com_sprintf( level.voteString, sizeof( level.voteString ), "%s \"%s\"", arg1, arg2 );
		} else {
			Com_sprintf( level.voteString, sizeof( level.voteString ), "%s", arg1, arg2 );
		}

		Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "%s", level.voteString );
	} else if ( !Q_stricmp( arg1, "map" ) ) {
		// special case for map changes, we want to reset the nextmap setting
		// this allows a player to change maps, but not upset the map rotation
		char s[MAX_STRING_CHARS];

		trap_Cvar_VariableStringBuffer( "nextmap", s, sizeof( s ) );
		if ( *s ) {
			Com_sprintf( level.voteString, sizeof( level.voteString ), "%s %s; set nextmap \"%s\"", arg1, arg2, s );
		} else {
			Com_sprintf( level.voteString, sizeof( level.voteString ), "%s %s", arg1, arg2 );
		}
		Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "%s", level.voteString );
	} else if ( !Q_stricmp( arg1, "nextmap" ) ) {
		char s[MAX_STRING_CHARS];

		trap_Cvar_VariableStringBuffer( "nextmap", s, sizeof( s ) );
		if ( !*s ) {
			trap_SendServerCommand( ent - g_entities, "print \"nextmap not set.\n\"" );
			return;
		}
		Com_sprintf( level.voteString, sizeof( level.voteString ), "vstr nextmap" );
		Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "%s", level.voteString );
		// JPW NERVE
	} else if ( !Q_stricmp( arg1,"kick" ) ) {
		int kicknum = MAX_CLIENTS;
		for ( i = 0; i < MAX_CLIENTS; i++ ) {
			if ( level.clients[i].pers.connected != CON_CONNECTED ) {
				continue;
			}
			// strip the color crap out
			Q_strncpyz( cleanName, level.clients[i].pers.netname, sizeof( cleanName ) );
			Q_CleanStr( cleanName );
			if ( !Q_stricmp( cleanName, arg2 ) ) {
				kicknum = i;
			}
		}
		Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "kick %s", level.clients[kicknum].pers.netname );
		if ( kicknum != MAX_CLIENTS && !( g_entities[kicknum].r.svFlags & SVF_BOT ) ) { // found a client # to kick, so override votestring with better one
			Com_sprintf( level.voteString, sizeof( level.voteString ),"clientkick \"%d\"",kicknum );
		} else if ( g_entities[kicknum].r.svFlags & SVF_BOT ) {
			trap_SendServerCommand( ent - g_entities, "print \"^3You cannot vote to kick a bot.\n\"" );
			return;
		} else { // if it can't do a name match, don't allow kick (to prevent votekick text spam wars)
			trap_SendServerCommand( ent - g_entities, "print \"Client not on server.\n\"" );
			return;
		}
		// jpw
	} else if ( !Q_stricmp( arg1, "difficulty" ) ) {
		int botDifficulty = atoi(arg2);

		if ( botDifficulty < -1 || botDifficulty == 0 || botDifficulty > MAXBOTDIFFICULTY ) {
			trap_SendServerCommand( ent - g_entities, va( "print \"^3Difficulty must be -1 for random or between 1 and %i^7\n\"", MAXBOTDIFFICULTY ) );
			return;
		}

		Com_sprintf( level.voteString, sizeof( level.voteString ),"bot difficulty %i", botDifficulty );
		Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "%s", level.voteString );
	} else if ( !Q_stricmp( arg1, "maxbots" ) ) {
		int maxBots = atoi(arg2);
		maxBots = maxBots == 0 ? -1 : maxBots;

		if ( maxBots < -1 || maxBots > g_maxclients.integer ) {
			trap_SendServerCommand( ent - g_entities, va( "print \"^3maxbots must be -1 for or between 1 and %i^7\n\"", g_maxclients.integer ) );
			return;
		}

		Com_sprintf( level.voteString, sizeof( level.voteString ),"bot minbots -1" );
		Com_sprintf( level.voteString, sizeof( level.voteString ),"bot maxbots %i", maxBots );
		Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "%s", level.voteString );
	} else {
		Com_sprintf( level.voteString, sizeof( level.voteString ), "%s \"%s\"", arg1, arg2 );
		Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "%s", level.voteString );
	}

	trap_SendServerCommand( -1, va( "print \"[lof]%s [lon]called a vote.\n\"", ent->client->pers.netname ) );

	// start the voting, the caller autoamtically votes yes
	level.voteTime = level.time;
	level.voteYes = 1;
	level.voteNo = 0;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		level.clients[i].ps.eFlags &= ~EF_VOTED;
	}
	ent->client->ps.eFlags |= EF_VOTED;

	trap_SetConfigstring( CS_VOTE_TIME, va( "%i", level.voteTime ) );
	trap_SetConfigstring( CS_VOTE_STRING, level.voteDisplayString );
	trap_SetConfigstring( CS_VOTE_YES, va( "%i", level.voteYes ) );
	trap_SetConfigstring( CS_VOTE_NO, va( "%i", level.voteNo ) );
}

/*
==================
Cmd_Vote_f
==================
*/
void Cmd_Vote_f( gentity_t *ent ) {
	char msg[64];

	// DHM - Nerve :: Complaints supercede voting (and share command)
	if ( ent->client->pers.complaintEndTime > level.time ) {
		gclient_t *cl = g_entities[ ent->client->pers.complaintClient ].client;
		if ( !cl ) {
			return;
		}
		if ( cl->pers.connected != CON_CONNECTED ) {
			return;
		}
		if ( cl->pers.localClient ) {
			trap_SendServerCommand( ent - g_entities, "complaint -3" );
			return;
		}

		// Reset this ent's complainEndTime so they can't send multiple complaints
		ent->client->pers.complaintEndTime = -1;
		ent->client->pers.complaintClient = -1;

		trap_Argv( 1, msg, sizeof( msg ) );

		if ( msg[0] == 'y' || msg[1] == 'Y' || msg[1] == '1' ) {
			int num;
			// Increase their complaint counter
			cl->pers.complaints++;

			num = g_complaintlimit.integer - cl->pers.complaints;

			if ( num <= 0 && !cl->pers.localClient ) {
				trap_DropClient( cl - level.clients, "kicked after too many complaints." );
				trap_SendServerCommand( ent - g_entities, "complaint -1" );
				return;
			}

			trap_SendServerCommand( cl->ps.clientNum, va( "print \"^1Warning^7: Complaint filed against you. [lof](%d [lon]until kicked[lof])\n\"", num ) );
			trap_SendServerCommand( ent - g_entities, "complaint -1" );
		} else {
			trap_SendServerCommand( ent - g_entities, "complaint -2" );
		}

		return;
	}
	// dhm

	// Reset this ent's complainEndTime so they can't send multiple complaints
	ent->client->pers.complaintEndTime = -1;
	ent->client->pers.complaintClient = -1;

	if ( !level.voteTime ) {
		trap_SendServerCommand( ent - g_entities, "print \"No vote in progress.\n\"" );
		return;
	}
	if ( ent->client->ps.eFlags & EF_VOTED ) {
		trap_SendServerCommand( ent - g_entities, "print \"Vote already cast.\n\"" );
		return;
	}
	if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
		trap_SendServerCommand( ent - g_entities, "print \"Not allowed to vote as spectator.\n\"" );
		return;
	}

	trap_SendServerCommand( ent - g_entities, "print \"Vote cast.\n\"" );

	ent->client->ps.eFlags |= EF_VOTED;

	trap_Argv( 1, msg, sizeof( msg ) );

	if ( msg[0] == 'y' || msg[1] == 'Y' || msg[1] == '1' ) {
		level.voteYes++;
		trap_SetConfigstring( CS_VOTE_YES, va( "%i", level.voteYes ) );
	} else {
		level.voteNo++;
		trap_SetConfigstring( CS_VOTE_NO, va( "%i", level.voteNo ) );
	}
}

qboolean G_canPickupMelee( gentity_t *ent ) {
	// JPW NERVE -- no "melee" weapons in net play
	if ( g_gametype.integer >= GT_WOLF ) {
		return qfalse;
	}
	// jpw

	if ( !( ent->client ) ) {
		return qfalse;  // hmm, shouldn't be too likely...
	}
	if ( !( ent->s.weapon ) ) {  // no weap, go ahead
		return qtrue;
	}

	if ( ent->client->ps.weaponstate == WEAPON_RELOADING ) {
		return qfalse;
	}

	if ( WEAPS_ONE_HANDED & ( 1 << ( ent->s.weapon ) ) ) {
		return qtrue;
	}

	return qfalse;
}

/*
=================
Cmd_SetViewpos_f
=================
*/
void Cmd_SetViewpos_f( gentity_t *ent ) {
	vec3_t origin, angles;
	char buffer[MAX_TOKEN_CHARS];
	int i;

	if ( !g_cheats.integer ) {
		trap_SendServerCommand( ent - g_entities, va( "print \"Cheats are not enabled on this server.\n\"" ) );
		return;
	}
	if ( trap_Argc() != 5 ) {
		trap_SendServerCommand( ent - g_entities, va( "print \"usage: setviewpos x y z yaw\n\"" ) );
		return;
	}

	VectorClear( angles );
	for ( i = 0 ; i < 3 ; i++ ) {
		trap_Argv( i + 1, buffer, sizeof( buffer ) );
		origin[i] = atof( buffer );
	}

	trap_Argv( 4, buffer, sizeof( buffer ) );
	angles[YAW] = atof( buffer );

	TeleportPlayer( ent, origin, angles );
}

/*
=================
Cmd_StartCamera_f
=================
*/
void Cmd_StartCamera_f( gentity_t *ent ) {
	if ( !CheatsOk( ent ) ) {
		return;
	}

	g_camEnt->r.svFlags |= SVF_PORTAL;
	g_camEnt->r.svFlags &= ~SVF_NOCLIENT;
	ent->client->cameraPortal = g_camEnt;
	ent->client->ps.eFlags |= EF_VIEWING_CAMERA;
	ent->s.eFlags |= EF_VIEWING_CAMERA;
}

/*
=================
Cmd_StopCamera_f
=================
*/
void Cmd_StopCamera_f( gentity_t *ent ) {
	if ( !CheatsOk( ent ) ) {
		return;
	}

	if ( ent->client->cameraPortal ) {
		// send a script event
		G_Script_ScriptEvent( ent->client->cameraPortal, "stopcam", "" );
		// go back into noclient mode
		ent->client->cameraPortal->r.svFlags |= SVF_NOCLIENT;
		ent->client->cameraPortal = NULL;
		ent->s.eFlags &= ~EF_VIEWING_CAMERA;
		ent->client->ps.eFlags &= ~EF_VIEWING_CAMERA;
	}
}

/*
=================
Cmd_SetCameraOrigin_f
=================
*/
void Cmd_SetCameraOrigin_f( gentity_t *ent ) {
	char buffer[MAX_TOKEN_CHARS];
	int i;

	if ( trap_Argc() != 4 ) {
		return;
	}

	VectorClear( ent->client->cameraOrigin );
	for ( i = 0 ; i < 3 ; i++ ) {
		trap_Argv( i + 1, buffer, sizeof( buffer ) );
		ent->client->cameraOrigin[i] = atof( buffer );
	}
}

// cs: noquarter mod ( derived from etpub )
qboolean G_PushPlayer( gentity_t *ent, gentity_t *victim ) {
	vec3_t dir, push;

	if ( !g_shove.integer ) {
		return qfalse;
	}

	if ( ent->r.svFlags & SVF_BOT ) {
		return qfalse;
	}

	if ( ent->health <= 0 ) {
		return qfalse;
	}

	if ( ( level.time - ent->client->pmext.shoveTime ) < 500 ) {
		return qfalse;
	}

	ent->client->pmext.shoveTime = level.time;

	// push is our forward vector
	AngleVectors( ent->client->ps.viewangles, dir, NULL, NULL );
	VectorNormalizeFast( dir );

	// etpro velocity
	VectorScale( dir, g_shove.integer * 5, push );

	// no longer try to shove into ground
	if ( ( push[2] > fabs( push[0] ) ) &&
		( push[2] > fabs( push[1] ) ) ) {
			// player is being boosted
			if ( g_shoveNoZ.integer ) {
				push[2] = 64;
			} else {
				push[2] = dir[2] * g_shove.integer * 4; // like in etpro, shoving up gives only 350 speed ( jump gives 270 )
			}
	} else {
		// give them a little hop
		push[2] = 64;
	}

	VectorAdd( victim->s.pos.trDelta, push, victim->s.pos.trDelta );
	VectorAdd( victim->client->ps.velocity, push, victim->client->ps.velocity );

	victim->client->ps.pm_time = 100;
	victim->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;

	return qtrue;
}

// cs: noquarter mod ( derived from etpub )
qboolean G_DragCorpse( gentity_t *dragger, gentity_t *corpse ) {
	vec3_t dir, pull, res;
	float dist;

	if ( !g_dragCorpse.integer ) {
		return qfalse;
	}
	VectorSubtract( dragger->r.currentOrigin, corpse->r.currentOrigin, dir );
	dir[2] = 0;
	dist = VectorNormalize( dir );

	// don't pull corpses past the dragger's head and don't start dragging
	// until both players look like they're in contact
	if ( dist > 128 || dist < 40 ) {
		return qfalse;
	}

	VectorScale( dir, 110, pull );

	// prevent some zipping around when the corpse doesn't have
	// much friction
	VectorSubtract( pull, corpse->client->ps.velocity, res );

	// no dragging into the ground
	res[2] = 0;

	VectorAdd( corpse->s.pos.trDelta, res, corpse->s.pos.trDelta );
	VectorAdd( corpse->client->ps.velocity, res, corpse->client->ps.velocity );

	return qtrue; // no checks yet
}

// credits to s4ndmod
void G_CanisterKick( gentity_t *ent ) {
	gentity_t   *traceEnt;
	vec3_t forward, right, up, angles, mins, maxs, origin;
	float speed = 800.0f;       //default value, based on some testing
	int i, num;
	int touch[MAX_GENTITIES];

	if (
		//dead players
			ent->client->ps.pm_flags & PMF_LIMBO ||
			ent->client->ps.pm_type == PM_DEAD
			) {
				// do not pass go, do not kick canisters
				return;
	}

	// Find straight ahead, no pitching...
	// We do this since we don't actually want to have to look at the item we
	// are going to kick
	VectorCopy( ent->client->ps.viewangles, angles );
	angles[PITCH] = 0;
	angles[ROLL] = 0;
	AngleVectors( angles, forward, right, up );

	// Move straight ahead from origin 24 units
	// Not the full 32 since we want to be able to kick thing we are just about standing on.
	VectorMA( ent->r.currentOrigin,24,forward,origin );

	// Only kick things that are up to about CH_KNIFE_DIST away in front of us.
	// and only up to about waist high.
	VectorAdd( origin, tv( -32.f,-32.f,-24.f ), mins );
	VectorAdd( origin, tv( 32.f,32.f,8.f ), maxs );

	num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

	if ( g_debugBullets.integer ) {
		gentity_t *tent;

		tent = G_TempEntity( mins, EV_RAILTRAIL );
		VectorCopy( maxs, tent->s.origin2 );
		tent->s.dmgFlags = 1;
	}

	for ( i = 0; i < num; i++ ) {
		traceEnt = &g_entities[touch[i]];

		// prevents a nasty kick corpses bug if they died holding any of
		// the kick weapon types.
		if ( traceEnt->s.eType != ET_MISSILE ) {
			continue;
		}

		// check for grenades, and misc canisters otherwise reject...
		if ( ( g_kickItems.integer & KICK_GRENADE && traceEnt->s.weapon == WP_GRENADE_PINEAPPLE ) ||
			( g_kickItems.integer & KICK_GRENADE && traceEnt->s.weapon == WP_GRENADE_LAUNCHER ) ||
			( g_kickItems.integer & KICK_AIRSTRIKE && traceEnt->s.weapon == WP_SMOKE_GRENADE ) ) {
				traceEnt->s.pos.trType = TR_GRAVITY;            //have to set this in case it was reset to TR_STATIONARY
				VectorClear( angles );        //only angles[1] is used
				//set it to the direction we're facing, +/- 15 degrees
				angles[1] = ent->client->ps.viewangles[1] + ( crandom() * 15.0f );

				//get the 'forward' vector based on the direction we're facing
				AngleVectors( angles, forward, NULL, NULL );
				//add some randomness to the 'height' of the kick
				forward[2] = 0.5f + ( crandom() * 0.15f );
				VectorNormalizeFast( forward );               //re-normalize the vector

				speed += ( crandom() * 150.0f );              //add some randomness to the speed/distance of the kick
				VectorScale( forward, speed, forward );       //and multiply the vector by the speed

				//play the grenade throwing sound, just to let them know they successfully kicked it
				G_Sound( ent, G_SoundIndex( "sound/weapons/grenade/grenlf1a.wav" ) );

				trap_UnlinkEntity( traceEnt );                    //probably not necessary, may prevent an occasional error
				traceEnt->s.pos.trTime = level.time - 50;   //move a little on the first frame

				//set the grenade's initial position
				VectorCopy( traceEnt->r.currentOrigin, traceEnt->s.pos.trBase );
				VectorCopy( forward, traceEnt->s.pos.trDelta );   //essentially, set the velocity
				SnapVector( traceEnt->s.pos.trDelta );                //"save network bandwith" apparently...rounds everything to integers
				trap_LinkEntity( traceEnt );                      //re-link the grenade...have to if we unlinked it
		}
	}
}

// Rafael
/*
==================
Cmd_Activate_f
==================
*/
void Cmd_Activate_f( gentity_t *ent ) {
	trace_t tr;
	vec3_t end;
	gentity_t   *traceEnt;
	vec3_t forward, right, up, offset;
	static int oldactivatetime = 0;
	int activatetime = level.time;
	qboolean walking = qfalse;

	// deploy parachute
	if ( (ent->client->ps.stats[STAT_KEYS] & ( 1 << KEY_7 )) && ent->s.groundEntityNum == ENTITYNUM_NONE ) {
		ent->client->ps.powerups[PW_FLIGHT] = INT_MAX;
	}

	if ( ent->active ) {
		if ( ent->client->ps.persistant[PERS_HWEAPON_USE] ) {
			// DHM - Nerve :: Restore original position if current position is bad
			trap_Trace( &tr, ent->r.currentOrigin, ent->r.mins, ent->r.maxs, ent->r.currentOrigin, ent->s.number, MASK_PLAYERSOLID );
			if ( tr.startsolid ) {
				VectorCopy( ent->TargetAngles, ent->client->ps.origin );
				VectorCopy( ent->TargetAngles, ent->r.currentOrigin );
				ent->r.contents = CONTENTS_CORPSE;      // DHM - this will correct itself in ClientEndFrame
			}
			ent->client->ps.eFlags &= ~EF_MG42_ACTIVE;          // DHM - Nerve :: unset flag
			ent->client->ps.persistant[PERS_HWEAPON_USE] = 0;
			ent->active = qfalse;
			return;
		} else
		{
			ent->active = qfalse;
		}
	}

	if ( ent->client->pers.cmd.buttons & BUTTON_WALKING ) {
		walking = qtrue;
	}

	AngleVectors( ent->client->ps.viewangles, forward, right, up );
	CalcMuzzlePointForActivate( ent, forward, right, up, offset );
	VectorMA( offset, 96, forward, end );

	trap_Trace( &tr, offset, NULL, NULL, end, ent->s.number, ( CONTENTS_SOLID | CONTENTS_BODY | CONTENTS_CORPSE | CONTENTS_TRIGGER ) );

	if ( tr.surfaceFlags & SURF_NOIMPACT ) {
		return;
	}

	if ( tr.entityNum == ENTITYNUM_WORLD ) {
		return;
	}

	traceEnt = &g_entities[ tr.entityNum ];

	if ( traceEnt->classname ) {
		traceEnt->flags &= ~FL_SOFTACTIVATE;    // FL_SOFTACTIVATE will be set if the user is holding 'walk' key

		if ( traceEnt->s.eType == ET_ALARMBOX ) {
			trace_t trace;

			if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
				return;
			}

			memset( &trace, 0, sizeof( trace ) );

			if ( traceEnt->use ) {
				traceEnt->use( traceEnt, ent, 0 );
			}
		} else if ( traceEnt->s.eType == ET_ITEM ) {
			trace_t trace;

			if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
				return;
			}

			memset( &trace, 0, sizeof( trace ) );

			if ( traceEnt->touch ) {
				if ( ent->client->pers.autoActivate == PICKUP_ACTIVATE ) {
					ent->client->pers.autoActivate = PICKUP_FORCE;      //----(SA) force pickup
				}
				traceEnt->active = qtrue;
				traceEnt->touch( traceEnt, ent, &trace );
			}
		} else if ( G_EmplacedGunIsMountable( traceEnt, ent ) ) {
			gclient_t* cl = &level.clients[ ent->s.clientNum ];
			vec3_t point;

			AngleVectors( traceEnt->s.apos.trBase, forward, NULL, NULL );
			VectorMA( traceEnt->r.currentOrigin, -36, forward, point );
			point[2] = ent->r.currentOrigin[2];

			// Save initial position
			VectorCopy( point, ent->TargetAngles );

			// Zero out velocity
			VectorCopy( vec3_origin, ent->client->ps.velocity );
			VectorCopy( vec3_origin, ent->s.pos.trDelta );

			traceEnt->active = qtrue;
			ent->active = qtrue;
			traceEnt->r.ownerNum = ent->s.number;
			VectorCopy( traceEnt->s.angles, traceEnt->TargetAngles );
			traceEnt->s.otherEntityNum = ent->s.number;

			cl->pmext.harc = traceEnt->harc;
			cl->pmext.varc = traceEnt->varc;
			VectorCopy( traceEnt->s.angles, cl->pmext.centerangles );
			cl->pmext.centerangles[PITCH] = AngleNormalize180( cl->pmext.centerangles[PITCH] );
			cl->pmext.centerangles[YAW] = AngleNormalize180( cl->pmext.centerangles[YAW] );
			cl->pmext.centerangles[ROLL] = AngleNormalize180( cl->pmext.centerangles[ROLL] );

			G_UseTargets( traceEnt, ent );   //----(SA)	added for Mike so mounting an MG42 can be a trigger event (let me know if there's any issues with this)
		} else if ( ( ( Q_stricmp( traceEnt->classname, "func_door" ) == 0 ) || ( Q_stricmp( traceEnt->classname, "func_door_rotating" ) == 0 ) ) ) {
			if ( walking ) {
				traceEnt->flags |= FL_SOFTACTIVATE;     // no noise
			}
			G_TryDoor( traceEnt, ent, ent );      // (door,other,activator)
		} else if ( ( Q_stricmp( traceEnt->classname, "team_WOLF_checkpoint" ) == 0 ) ) {
			if ( traceEnt->count != ent->client->sess.sessionTeam ) {
				traceEnt->health++;
			}
		} else if ( ( Q_stricmp( traceEnt->classname, "func_button" ) == 0 )
			&& ( traceEnt->s.apos.trType == TR_STATIONARY && traceEnt->s.pos.trType == TR_STATIONARY )
			&& traceEnt->active == qfalse ) {
				Use_BinaryMover( traceEnt, ent, ent );
				traceEnt->active = qtrue;
		} else if ( !Q_stricmp( traceEnt->classname, "func_invisible_user" ) ) {
			if ( walking ) {
				traceEnt->flags |= FL_SOFTACTIVATE;     // no noise
			}
			traceEnt->use( traceEnt, ent, ent );
		} else if ( !Q_stricmp( traceEnt->classname, "script_mover" ) ) {
			G_Script_ScriptEvent( traceEnt, "activate", ent->aiName );
		} else if ( !Q_stricmp( traceEnt->classname, "props_footlocker" ) ) {
			traceEnt->use( traceEnt, ent, ent );
		}
	}

	if ( activatetime > oldactivatetime + 1000 ) {
		oldactivatetime = activatetime;
	}
}

/*
Cmd_Activate2_f() is only for:
shove
dragging
kicking
*/
void Cmd_Activate2_f( gentity_t *ent ) {
	trace_t tr;
	vec3_t end;
	gentity_t   *traceEnt;
	vec3_t forward, right, up, offset;

	if ( ent->health <= 0 || ent->r.svFlags & SVF_BOT ) { // uch
		return;
	}

	AngleVectors( ent->client->ps.viewangles, forward, right, up );
	CalcMuzzlePointForActivate( ent, forward, right, up, offset );
	VectorMA( offset, 96, forward, end );

	// look for a corpse to drag
	trap_Trace( &tr,
		offset,
		NULL,
		NULL,
		end,
		ent->s.number,
		CONTENTS_CORPSE );
	if ( tr.entityNum >= 0 ) {
		traceEnt = &g_entities[tr.entityNum];
		if ( traceEnt->client ) {
			if ( ent->client->sess.sessionTeam == traceEnt->client->sess.sessionTeam ) {
				G_DragCorpse( ent, traceEnt );
			}

			return;
		}
	}

	// look for a guy to push
	trap_Trace( &tr,
		offset,
		NULL,
		NULL,
		end,
		ent->s.number,
		CONTENTS_BODY );
	if ( tr.entityNum >= 0 ) {
		traceEnt = &g_entities[tr.entityNum];
		if ( traceEnt->client ) {
			G_PushPlayer( ent, traceEnt );

			return;
		}
	}

	G_CanisterKick(ent);
}

// NERVE - SMF
void Cmd_SetSpawnPoint_f( gentity_t *clent ) {
	char arg[MAX_TOKEN_CHARS];

	if ( trap_Argc() != 2 ) {
		return;
	}

	trap_Argv( 1, arg, sizeof( arg ) );
	if ( clent->client ) { // JPW NERVE
		clent->client->sess.spawnObjectiveIndex = atoi( arg ); // JPW NERVE
	}
}

/*////////////////////////////////////////////////////
Cmd_Stats_f
*/                                                                    ////////////////////////////////////////////////////
void Cmd_Stats_f( gentity_t *ent ) {
	char message[MAX_STRING_CHARS];
	gclient_t *client = ent->client;

	int deaths = client->pers.deaths;
	//float killRatio = client->pers.kills;
	float killRatio = client->pers.kills;

	int shots = client->pers.acc_shots;
	float acc = 0.0f;

	//avoid divide-by-0, will be equal to kills until player dies
	if ( deaths > 0 ) {
		killRatio = (float)client->pers.kills / (float)deaths;
	}

	//avoid divide-by-0 error, will be 0.0 until the player hits someone
	if ( shots > 0 ) {
		acc = ( (float)client->pers.acc_hits / (float)shots ) * 100.0f;
	}

	strcpy( message, va( "print \"Stats for %s ^7this round: \n", client->pers.netname ) );

	strcat( message, va( "^3Kills: ^7%d", client->pers.kills ) );
	if ( client->pers.teamKills > 0 ) {
		strcat( message, va( " (^1%d^7)", client->pers.teamKills ) );   //only show team kills if greater than 0
	}
	strcat( message, va( ", " ) );

	strcat( message, va( "^3Deaths: ^7%d, ", deaths ) );
	strcat( message, va( "^3Kill Ratio: ^7%2.2f, ", killRatio ) );

	strcat( message, va( "^3Gibs: ^7%d \n", client->pers.gibs ) );
	if ( client->pers.teamGibs > 0 ) {
		strcat( message, va( " (^1%d^7)", client->pers.teamGibs ) );        //only show team gibs if greater than 0
	}
	//strcat(message, va(" \n"));

	strcat( message, va( "^3Revived: ^7%d, ", client->pers.revives ) );
	strcat( message, va( "^3Packs Given: ^7%d health, %d ammo \n", client->pers.medPacks, client->pers.ammoPacks ) );
	strcat( message, va( "^3Accuracy: ^7%2.2f (%d/%d), ", acc, client->pers.acc_hits, shots ) );
	strcat( message, va( "^3Headshots: ^7%d \n", client->pers.headshots ) );
	strcat( message, va( "^3Goombas: ^7%d, ", client->pers.goomba ) );
	strcat( message, va( "^3Poisons: ^7%d \n", client->pers.poison ) );

	strcat( message, "\"" );

	trap_SendServerCommand( ent - g_entities, message );
}

/*///////////////////////////////////////////////////////
Cmd_ThrowKnife_f
*/                                                                            ///////////////////////////////////////////////////////
void Cmd_ThrowKnife_f( gentity_t *ent ) {
	vec3_t velocity, offset, org, mins, maxs;
	trace_t tr;
	gentity_t *ent2;
	gitem_t *item = BG_FindItemForWeapon( WP_KNIFE );

	if ( !item ) {
		return;
	}

	if ( g_throwableKnives.integer == 0 || ent->client->ps.pm_type >= PM_DEAD ) {
		return;
	}

	if ( level.time < ( ent->thrownKnifeTime + 750 ) ) {    //can throw one every 0.75 seconds
		return;
	}

	if ( ( ent->client->ps.ammo[BG_FindAmmoForWeapon( WP_KNIFE )] == 0 ) && ( g_throwableKnives.integer != -1 ) ) {
		return;
	}

	AngleVectors( ent->client->ps.viewangles, velocity, NULL, NULL );
	VectorScale( velocity, 64, offset );
	offset[2] += ent->client->ps.viewheight / 2;
	VectorScale( velocity, 800, velocity );
	velocity[2] += 50 + random() * 35;

	VectorAdd( ent->client->ps.origin, offset, org );

	VectorSet( mins, -ITEM_RADIUS, -ITEM_RADIUS, 0 );
	VectorSet( maxs, ITEM_RADIUS, ITEM_RADIUS, 2 * ITEM_RADIUS );

	trap_Trace( &tr, ent->client->ps.origin, mins, maxs, org, ent->s.number, MASK_SOLID );
	VectorCopy( tr.endpos, org );

	G_Sound( ent, G_SoundIndex( "sound/weapons/knife/knife_slash1.wav" ) );
	ent2 = LaunchItem( item, org, velocity, ent-g_entities );

	ent2->touch = Touch_Knife;
	ent2->parent = ent;

	if ( g_throwableKnives.integer > 0 ) {
		ent->client->ps.ammo[BG_FindAmmoForWeapon( WP_KNIFE )]--;
	}

	ent->thrownKnifeTime = level.time;

	trap_SendServerCommand( ent-g_entities, va( "chat \"^3Knives left:^7 %d\" %i", ent->client->ps.ammo[BG_FindAmmoForWeapon( WP_KNIFE )], qfalse ) );
}

/*////////////////////////////////////////////
Touch_Knife
*/                                                            ////////////////////////////////////////////
void Touch_Knife( gentity_t *ent, gentity_t *other, trace_t *trace ) {
	qboolean hurt = qfalse;
	ent->active = qfalse;   //set this just to be safe, but shouldn't be necessary

	if ( !other->client ) {
		return;
	}
	if ( other->health < 1 ) {
		return;
	}

	if ( VectorLength( ent->s.pos.trDelta ) != 0 ) {
		if ( ( g_friendlyFire.integer ) || ( !OnSameTeam( other, ent->parent ) ) ) {
			int i;
			int sound;
			int damage = g_knifeDamage.integer;
			damage -= rand() % 10;                  //subtract 0-9 to make it a little varied

			if ( damage <= 0 ) {
				damage = 1;
			}

			//pick a random sound to play
			i = rand() % 3;
			if ( i == 0 ) {
				sound = G_SoundIndex( "/sound/weapons/knife/knife_hit1.wav" );
			} else if ( i == 1 ) {
				sound = G_SoundIndex( "/sound/weapons/knife/knife_hit2.wav" );
			} else {
				sound = G_SoundIndex( "/sound/weapons/knife/knife_hit3.wav" );
			}

			G_Sound( other, sound );  //play the sound
			G_Damage( other, ent->parent, ent->parent, NULL, trace->endpos, damage, 0, MOD_KNIFE_THROWN );
			hurt = qtrue;
		}
	}

	if ( hurt == qfalse ) {
		int makenoise = EV_ITEM_PICKUP;

		G_LogPrintf( "Item: %i %s\n", other->s.number, ent->item->classname );

		if ( g_throwableKnives.value != 0 ) {
			if ( other->client->ps.ammo[BG_FindAmmoForWeapon( WP_KNIFE )] < g_maxKnives.integer ) {
				other->client->ps.ammo[BG_FindAmmoForWeapon( WP_KNIFE )]++;
			} else if ( g_throwableKnives.integer < 0 ) {
				other->client->ps.ammo[BG_FindAmmoForWeapon( WP_KNIFE )]++;
			} else {
				return;
			}
		}

		if ( ent->noise_index ) {
			makenoise = EV_ITEM_PICKUP_QUIET;
			G_AddEvent( other, EV_GENERAL_SOUND, ent->noise_index );
		}

		if ( other->client->pers.predictItemPickup ) {
			G_AddPredictableEvent( other, makenoise, ent->s.modelindex );
		} else {
			G_AddEvent( other, makenoise, ent->s.modelindex );
		}
	}

	ent->freeAfterEvent = qtrue;
	ent->flags |= FL_NODRAW;
	ent->r.svFlags |= SVF_NOCLIENT;
	ent->s.eFlags |= EF_NODRAW;
	ent->r.contents = 0;
	ent->nextthink = 0;
	ent->think = 0;

	trap_SendServerCommand( other->client->ps.clientNum, va( "chat \"^3Knives left:^7 %d\" %i", other->client->ps.ammo[BG_FindAmmoForWeapon( WP_KNIFE )], qfalse ) );
}

/*
==================
Cmd_BotTapOut_f

Simple jump command for bots to tap out rather than calling limbo on the bot entity.
The main reason for this is so tapout reports work for them ...

==================
*/
void Cmd_BotTapOut_f( gentity_t *ent ) {
	static usercmd_t cmd;

	memset( &cmd, 0, sizeof( cmd ) );
	cmd.identClient = ent - g_entities;
	cmd.serverTime = level.time;
	cmd.upmove += 127;

	trap_BotUserCommand( ent - g_entities, &cmd );
}

/*
==================
Cmd_Smoke_f

Set / Unset the Smoke Grenade "powerup" for the client
==================
*/
void Cmd_Smoke_f( gentity_t *ent ) {
	gclient_t *client = ent->client;
	char message[64] = "Smoke grenade ";

	if ( !g_smokeGrenades.integer ) {
		trap_SendServerCommand( ent - g_entities, va( "cp \"Smoke grenades are disabled on this server.\n\"" ) );
		return;
	}

	if ( client->ps.stats[STAT_PLAYER_CLASS] != PC_LT ) {
		trap_SendServerCommand( ent - g_entities, va( "cp \"Smoke grenades can only be used by LT's.\n\"" ) );
		return;
	}

	//set up the correct message depending on the client's choice
	if ( client->ps.powerups[PW_SMOKEGRENADE] == 0 ) {
		client->ps.powerups[PW_SMOKEGRENADE] = INT_MAX;
		G_AddEvent( ent, EV_POWERUP_SMOKEGRENADE, 0 );
		strcat( message, "^2enabled" );
	} else {
		strcat( message, "^1disabled" );
		client->ps.powerups[PW_SMOKEGRENADE] = 0;
	}

	trap_SendServerCommand( ent - g_entities, va( "cp \"%s\n\" 1", message ) );
}

/*
==================
Cmd_NadePack_f

Set / Unset the Smoke Grenade "powerup" for the client
==================
*/
void Cmd_NadePack_f( gentity_t *ent ) {
	gclient_t *client = ent->client;
	char message[64] = "Nade Pack ";

	if ( !g_nadePacks.integer ) {
		trap_SendServerCommand( ent - g_entities, va( "cp \"Nade packs are disabled on this server.\n\"" ) );
		return;
	}

	if ( client->ps.stats[STAT_PLAYER_CLASS] != PC_LT ) {
		trap_SendServerCommand( ent - g_entities, va( "cp \"Nade packs can only be used by LT's.\n\"" ) );
		return;
	}

	//set up the correct message depending on the client's choice
	if ( client->ps.powerups[PW_NADEPACK] == 0 ) {
		client->ps.powerups[PW_NADEPACK] = INT_MAX;
		G_AddEvent( ent, EV_POWERUP_NADEPACK, 0 );
		strcat( message, "^2enabled" );
	} else {
		strcat( message, "^1disabled" );
		client->ps.powerups[PW_NADEPACK] = 0;
	}

	trap_SendServerCommand( ent - g_entities, va( "cp \"%s\n\" 1", message ) );
}

/*
==================
Cmd_GiveAmmo_f

from sandmod
==================
*/
void Cmd_GiveAmmo_f( gentity_t* ent ) {
	gentity_t *target;
	trace_t tr;
	vec3_t start, end, forward;
	char arg1[MAX_STRING_TOKENS];
	int givenAmmo;
	int weapon, targWeap;
	int maxGive; //the max ammount of ammo you can give this player(just in case a player gives too much ammo they will get it back)

	if ( !g_allowGiveAmmo.integer ) {
		trap_SendServerCommand( ent - g_entities, va( "chat \"^7/giveammo^3 is not enabled on this server!\"" ) );
		return;
	}

	trap_Argv( 1,arg1,sizeof( arg1 ) );
	givenAmmo = atoi( arg1 );

	if ( ent->client->ps.stats[STAT_HEALTH] <= 0 ) {
		trap_SendServerCommand( ent - g_entities, va( "chat \"^3You have to be alive to do that!\"" ) );
		return;
	}

	AngleVectors( ent->client->ps.viewangles, forward, NULL, NULL );

	VectorCopy( ent->s.pos.trBase, start );   //set 'start' to the player's position (plus the viewheight)
	start[2] += ent->client->ps.viewheight;
	VectorMA( start, 128, forward, end );    //put 'end' 128 units forward of 'start'

	//see if we hit anything between 'start' and 'end'
	trap_Trace( &tr, start, NULL, NULL, end, ent->s.number, CONTENTS_BODY );

	//if we didn't hit a player, return
	if ( tr.entityNum >= MAX_CLIENTS ) {
		trap_SendServerCommand( ent - g_entities, va( "chat \"^3You must be targeting a close teammate for that!\"" ) );
		return;
	}

	target = &( g_entities[tr.entityNum] );

	if ( ( !target->inuse ) || ( !target->client ) ) { //if the player is lagged/disconnected/etc
		return;
	}

	if ( target->client->sess.sessionTeam != ent->client->sess.sessionTeam ) { //not on same team
		trap_SendServerCommand( ent - g_entities, va( "chat \"^3You can't give ammo to enemies!\"" ) );
		return;
	}

	if ( target->client->ps.stats[STAT_HEALTH] <= 0 ) {   //if they're dead
		trap_SendServerCommand( ent - g_entities, va( "chat \"^3You can't give ammo to the dead!\"" ) );
		return;
	}

	//if given ammo is 0 or unset make it the size of a clip
	if ( givenAmmo <= 0 ) {
		givenAmmo = ammoTable[ent->client->ps.weapon].maxclip;
	}

	weapon = ent->client->ps.weapon;

	switch ( weapon ) {
	case WP_LUGER:
	case WP_COLT:

		if ( ent->client->ps.ammo[BG_FindClipForWeapon( ent->client->ps.weapon )] < givenAmmo ) {
			trap_SendServerCommand( ent - g_entities, va( "chat \"^3You do not have enough ammo to give ^7%i^3!\"",givenAmmo ) );
			return;
		}

		if ( !COM_BitCheck( target->client->ps.weapons, WP_LUGER ) && !COM_BitCheck( target->client->ps.weapons, WP_COLT ) ) {
			trap_SendServerCommand( ent - g_entities, va( "chat \"^7%s ^3does not have a Pistol!\"", target->client->pers.netname ) );
			return;
		}

		if ( COM_BitCheck( target->client->ps.weapons, WP_LUGER ) ) {
			targWeap = WP_LUGER;
		} else {
			targWeap = WP_COLT;
		}

		maxGive = ( (ammoTable[targWeap].maxclip * 4) + G_ExtraAmmo(-1,targWeap) ) - target->client->ps.ammo[BG_FindAmmoForWeapon( targWeap )];
		target->client->ps.ammo[BG_FindAmmoForWeapon( targWeap )] += givenAmmo;
		if ( target->client->ps.ammo[BG_FindAmmoForWeapon( targWeap )] > ((ammoTable[targWeap].maxclip * 4) + G_ExtraAmmo(-1,targWeap)) ) {
			target->client->ps.ammo[BG_FindAmmoForWeapon( targWeap )] = (ammoTable[targWeap].maxclip * 4) + G_ExtraAmmo(-1,targWeap);
		}

		break;

	case WP_MP40:
	case WP_THOMPSON:
	case WP_STEN:

		if ( ent->client->ps.ammo[BG_FindClipForWeapon( ent->client->ps.weapon )] < givenAmmo ) {
			trap_SendServerCommand( ent - g_entities, va( "chat \"^3You do not have enough ammo to give ^7%i^3!\"",givenAmmo ) );
			return;
		}

		if ( !COM_BitCheck( target->client->ps.weapons, WP_MP40 ) && !COM_BitCheck( target->client->ps.weapons, WP_THOMPSON ) && !COM_BitCheck( target->client->ps.weapons, WP_STEN ) ) {
			trap_SendServerCommand( ent - g_entities, va( "chat \"^7%s ^3does not have a SMG!\"", target->client->pers.netname ) );
			return;
		}

		if ( COM_BitCheck( target->client->ps.weapons, WP_MP40 ) ) {
			targWeap = WP_MP40;
		} else if ( COM_BitCheck( target->client->ps.weapons, WP_THOMPSON ) ) {
			targWeap = WP_THOMPSON;
		} else {
			targWeap = WP_STEN;
		}

		maxGive = ( (ammoTable[targWeap].maxclip * 3) + G_ExtraAmmo(target->client->ps.stats[STAT_PLAYER_CLASS],targWeap) ) - target->client->ps.ammo[BG_FindAmmoForWeapon( targWeap )];
		target->client->ps.ammo[BG_FindAmmoForWeapon( targWeap )] += givenAmmo;
		if ( target->client->ps.ammo[BG_FindAmmoForWeapon( targWeap )] > (ammoTable[targWeap].maxclip * 3) + G_ExtraAmmo(target->client->ps.stats[STAT_PLAYER_CLASS],targWeap) ) {
			target->client->ps.ammo[BG_FindAmmoForWeapon( targWeap )] = (ammoTable[targWeap].maxclip * 3) + G_ExtraAmmo(target->client->ps.stats[STAT_PLAYER_CLASS],targWeap);
		}

		break;

	case WP_MAUSER:
	case WP_SNIPERRIFLE:

		if ( ent->client->ps.ammo[BG_FindClipForWeapon( ent->client->ps.weapon )] < givenAmmo ) {
			trap_SendServerCommand( ent - g_entities, va( "chat \"^3You do not have enough ammo to give ^7%i^3!\"",givenAmmo ) );
			return;
		}

		if ( !COM_BitCheck( target->client->ps.weapons, WP_SNIPERRIFLE ) && !COM_BitCheck( target->client->ps.weapons, WP_MAUSER ) ) {
			trap_SendServerCommand( ent - g_entities, va( "chat \"^7%s ^3does not have a Mauser!\"", target->client->pers.netname ) );
			return;
		}

		maxGive = ( (ammoTable[WP_SNIPERRIFLE].maxclip * 3) + G_ExtraAmmo(-1,WP_SNIPERRIFLE) ) - target->client->ps.ammo[BG_FindAmmoForWeapon( WP_SNIPERRIFLE )];
		target->client->ps.ammo[BG_FindAmmoForWeapon( WP_SNIPERRIFLE )] += givenAmmo;
		if ( target->client->ps.ammo[BG_FindAmmoForWeapon( WP_SNIPERRIFLE )] > (ammoTable[WP_SNIPERRIFLE].maxclip * 3) + G_ExtraAmmo(-1,WP_SNIPERRIFLE) ) {
			target->client->ps.ammo[BG_FindAmmoForWeapon( WP_SNIPERRIFLE )] = (ammoTable[WP_SNIPERRIFLE].maxclip * 3) + G_ExtraAmmo(-1,WP_SNIPERRIFLE);
		}

		maxGive = ( (ammoTable[WP_MAUSER].maxclip * 3) + G_ExtraAmmo(-1,WP_MAUSER) ) - target->client->ps.ammo[BG_FindAmmoForWeapon( WP_MAUSER )];
		target->client->ps.ammo[BG_FindAmmoForWeapon( WP_MAUSER )] += givenAmmo;
		if ( target->client->ps.ammo[BG_FindAmmoForWeapon( WP_MAUSER )] > (ammoTable[WP_MAUSER].maxclip * 3) + G_ExtraAmmo(-1,WP_MAUSER) ) {
			target->client->ps.ammo[BG_FindAmmoForWeapon( WP_MAUSER )] = (ammoTable[WP_MAUSER].maxclip * 3) + G_ExtraAmmo(-1,WP_MAUSER);
		}

		break;

	case WP_PANZERFAUST:

		if ( ent->client->ps.ammo[BG_FindClipForWeapon( ent->client->ps.weapon )] < givenAmmo ) {
			trap_SendServerCommand( ent - g_entities, va( "chat \"^3You do not have enough ammo to give ^7%i^3!\"",givenAmmo ) );
			return;
		}

		if ( !COM_BitCheck( target->client->ps.weapons, WP_PANZERFAUST ) ) {
			trap_SendServerCommand( ent - g_entities, va( "chat \"^7%s ^3does not have a Panzerfaust!\"", target->client->pers.netname ) );
			return;
		}
		maxGive = ( (ammoTable[WP_PANZERFAUST].maxclip * 3) + G_ExtraAmmo(-1,WP_PANZERFAUST) ) - target->client->ps.ammo[BG_FindAmmoForWeapon( WP_PANZERFAUST )];
		target->client->ps.ammo[BG_FindAmmoForWeapon( WP_PANZERFAUST )] += givenAmmo;
		if ( target->client->ps.ammo[BG_FindAmmoForWeapon( WP_PANZERFAUST )] > (ammoTable[WP_PANZERFAUST].maxclip * 3) + G_ExtraAmmo(-1,WP_PANZERFAUST) ) {
			target->client->ps.ammo[BG_FindAmmoForWeapon( WP_PANZERFAUST )] = (ammoTable[WP_PANZERFAUST].maxclip * 3) + G_ExtraAmmo(-1,WP_PANZERFAUST);
		}

		break;

	case WP_VENOM:

		if ( ent->client->ps.ammo[BG_FindClipForWeapon( ent->client->ps.weapon )] < givenAmmo ) {
			trap_SendServerCommand( ent - g_entities, va( "chat \"^3You do not have enough ammo to give ^7%i^3!\"",givenAmmo ) );
			return;
		}

		if ( !COM_BitCheck( target->client->ps.weapons, WP_VENOM ) ) {
			trap_SendServerCommand( ent - g_entities, va( "chat \"^7%s ^3does not have a Venom!\"", target->client->pers.netname ) );
			return;
		}
		maxGive = ( (ammoTable[WP_VENOM].maxclip * 3) + G_ExtraAmmo(-1,WP_VENOM) ) - target->client->ps.ammo[BG_FindAmmoForWeapon( WP_VENOM )];
		target->client->ps.ammo[BG_FindAmmoForWeapon( WP_VENOM )] += givenAmmo;
		if ( target->client->ps.ammo[BG_FindAmmoForWeapon( WP_VENOM )] > (ammoTable[WP_VENOM].maxclip * 3) + G_ExtraAmmo(-1,WP_VENOM) ) {
			target->client->ps.ammo[BG_FindAmmoForWeapon( WP_VENOM )] = (ammoTable[WP_VENOM].maxclip * 3) + G_ExtraAmmo(-1,WP_VENOM);
		}
		break;

	default:
		trap_SendServerCommand( ent - g_entities, va( "chat \"^3You can not use ^7/giveammo^3 with this weapon!\"" ) );
		return;
	}

	if ( maxGive <= 0 ) {
		trap_SendServerCommand( ent - g_entities, va( "chat \"^7%s ^3does not need any ammo!\"",target->client->pers.netname ) );
		return;
	}

	if ( givenAmmo > maxGive ) {
		trap_SendServerCommand( ent - g_entities, va( "chat \"^3You gave ^7%s^3 ammo, ^7%i ^3was all he could carry!\"",target->client->pers.netname,maxGive ) );
		trap_SendServerCommand( target - g_entities, va( "chat \"^7%s^3 gave you ^7%i ^3ammo!\"",ent->client->pers.netname,maxGive ) );
		ent->client->ps.ammo[BG_FindClipForWeapon( weapon )] -= maxGive;
	} else {
		trap_SendServerCommand( ent - g_entities, va( "chat \"^3You gave ^7%i^3 ammo to ^7%s^3!\"",givenAmmo,target->client->pers.netname ) );
		trap_SendServerCommand( target - g_entities, va( "chat \"^7%s^3 gave you ^7%i ^3ammo!\"",ent->client->pers.netname,givenAmmo ) );
		ent->client->ps.ammo[BG_FindClipForWeapon( weapon )] -= givenAmmo;
	}

	Bot_Event_RecievedAmmo( target - g_entities, ent );
}

void Cmd_MyCredits_f ( gentity_t* ent )
{
	if( !ent || !ent->client ) {
		return;
	}
	trap_SendServerCommand( ent - g_entities, va("print \"you have ^2%i^7 credits\n", ent->client->sess.credits) );
}

qboolean G_MatchOnePlayer(int *plist, char *err, int len)
{
	gclient_t *cl;
	int *p;
	char line[MAX_NAME_LENGTH+10];

	err[0] = '\0';
	line[0] = '\0';
	if(plist[0] == -1) {
		Q_strcat(err, len,
			"no connected player by that name or slot #");
		return qfalse;
	}
	if(plist[1] != -1) {
		Q_strcat(err, len, "more than one player name matches. "
			"be more specific or use the slot #:\n");
		for(p = plist;*p != -1; p++) {
			cl = &level.clients[*p];
			if(cl->pers.connected == CON_CONNECTED) {
				Com_sprintf(line, MAX_NAME_LENGTH + 10, "%2i - %s^7\n",
					*p,
					cl->pers.netname);
				if(strlen(err)+strlen(line) > len)
					break;
				Q_strcat(err, len, line);
			}
		}
		return qfalse;
	}
	return qtrue;
}

void Cmd_Bet_f( gentity_t* ent )
{
	int pids[MAX_CLIENTS];
	char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS];
	gentity_t *vic = NULL;
	char amountstr[11]; // 10 is max strlen() for 32-bit int
	int amount = 0;

	if ( trap_Argc() < 3 ) {
		CP("print \"usage: !bet [name|slot#] [credits]\n\"");
		return;
	}

	// you must be in a team to start a bet..
	if ( ent->client->sess.sessionTeam != TEAM_RED && ent->client->sess.sessionTeam != TEAM_BLUE ) {
		CP("print \"^dbet: ^9you are not in a team\n\"");
		return;
	}

	// only 1 bet at a time..
	if ( ent->client->sess.currentBetTarget ) {
		CP("print \"^dbet: ^9you cannot start a bet while a request is pending\n\"");
		return;
	}

	trap_Argv( 1, name, sizeof( name ) );

	if(ClientNumbersFromString(name, pids) != 1) {
		G_MatchOnePlayer(pids, err, sizeof(err));
		CP(va("print \"^dbet: ^9%s\n\"", err));
		return;
	}
	vic = &g_entities[pids[0]];

	// target must be in a team to start a bet..
	if ( vic->client->sess.sessionTeam != TEAM_RED && vic->client->sess.sessionTeam != TEAM_BLUE ) {
		CP("print \"^dbet: ^9you cannot bet with spectators\n\"");
		return;
	}

#ifndef _DEBUG
	if ( vic->r.svFlags & SVF_BOT ) {
		CP(va("print \"^dbet: ^9%s^9 is a bot and cannot bet\n\"",name));
		return;
	}
#endif

	// the other must be in the opposite team..
	// this check eliminates the need to check for self-bets.
	if ( vic->client->sess.sessionTeam == ent->client->sess.sessionTeam ) {
		CP(va("print \"^dbet: ^9%s^9 is not in your opposing team\n\"",name));
		return;
	}

	// the other must not have a bet in progress, or even have a pending request..
	if ( vic->client->sess.currentBetTarget ) {
		CP(va("print \"^dbet: ^9%s^9 is already in another bet\n\"",name));
		return;
	}

	// lower the bet in case the other has insufficient credits..
	trap_Argv( 2, amountstr, sizeof( amountstr ) );
	amount = atoi(amountstr);
	if ( vic->client->sess.credits < amount ) {
		amount = vic->client->sess.credits;
	}

	// setup data for the bet..
	ent->client->sess.currentBetTarget = vic;
	vic->client->sess.currentBetTarget = ent;
	ent->client->sess.currentBetAmount = amount;
	vic->client->sess.currentBetAmount = amount;

#ifdef _DEBUG
	// bots always accept in debug mode
	if (vic->r.svFlags & SVF_BOT) {
		ent->client->sess.betTime = level.time;
		vic->client->sess.betTime = level.time;
	}
#endif

	CP(va("print \"^dbet: ^9you requested a bet with %s^9 (%i credits)\n\"",name,amount));
	//CPx(vic->client - level.clients, va("^dbet: ^9%s^9 wants to bet with you\n",ent->client->pers.netname));
	CPx(vic->client - level.clients, va("cp \"%s^9 wants to bet with you (%i credits)\n\"",ent->client->pers.netname,amount));	// centerprint
}

void Cmd_Accept_f( gentity_t* ent )
{
	gentity_t *vic = NULL;

	// is the other player still valid?..
	if ( ent->client->sess.currentBetTarget ) {
		vic = ent->client->sess.currentBetTarget;
		if ( !vic->client ) {
			ent->client->sess.currentBetTarget = NULL;
			ent->client->sess.currentBetAmount = 0;
			CP("print \"^daccept: target is no longer in a team\n\"");
			return;
		}
	}
	else {
		CP("print \"^daccept: you have not received a bet request!\n\"");
		return;
	}

	// start the bet..
	ent->client->sess.betTime = level.time;
	vic->client->sess.betTime = level.time;

	CP(va("print \"^daccept: ^9you accepted the bet with %s\n\"",vic->client->pers.netname));
	//CPx(vic->client - level.clients, va("^daccept: ^9%s^9 accepted your bet\n",ent->client->pers.netname));
	CPx(vic->client - level.clients, va("cp \"%s^9 accepted your bet\n\"",ent->client->pers.netname));	// centerprint
}

void Cmd_Buy_f( gentity_t* ent )
{
	char buyingstr[MAX_STRING_CHARS];
	int max;

	trap_Argv( 1, buyingstr, sizeof( buyingstr ) );

	// sanity..
	if ( !(ent && ent->client) ) {
		return;
	}

	if ( ent->client->ps.stats[STAT_HEALTH] <= 0 ) {
		CP("print \"^dbuy: ^9you cannot buy while dead\n\"");
		return;
	}

	// you must be in a team to be able to buy..
	if ( ent->client->sess.sessionTeam != TEAM_RED && ent->client->sess.sessionTeam != TEAM_BLUE ) {
		CP("print \"^dbuy: ^9you are not in a team\n\"");
		return;
	}

	// you can buy max. 1 item per life..
	if ( ent->client->sess.buyTime > 0 ) {
		CP("print \"^dbuy: ^9you can only buy one item per life\n\"");
		return;
	}

	if (!*buyingstr) {
		CP("print \"usage: /buy [ammo|health|para]\n\"");
		return;
	}

	// ammo and health cost CREDITS_PRICE credits.
	// see if the player has enough credits..
	if ( ent->client->sess.credits < CREDITS_PRICE ) {
		CP(va("print \"^dbuy: ^9you need at least %i credits to buy something\n\"",CREDITS_PRICE));
		return;
	}

	// buying ammo..
	if ( !Q_stricmp(buyingstr, "ammo") ) {
		if ( ent->client->ps.weapon ) {
			Add_Ammo( ent, ent->client->ps.weapon, ammoTable[ent->client->ps.weapon].maxammo, qtrue );
			CP("print \"^dbuy: ^9you bought yourself some ammo\n\"");
		}
		else {
			CP("print \"^dbuy: ^9you need a primary weapon to buy ammo\n\"");
			return;
		}
	}
	// buying health..
	else if ( !Q_stricmp(buyingstr, "health") ) {
		max = ent->client->ps.stats[STAT_MAX_HEALTH];
		if( ent->client->sess.playerType == PC_MEDIC ) {
			max *= 1.12f;
		}
		ent->health = max;
		CP("print \"^dbuy: ^9you bought yourself some health\n\"");
	}
	else if ( !Q_stricmp(buyingstr, "para") ) {
		ent->client->ps.stats[STAT_KEYS] |= ( 1 << KEY_7 );
		CP("print \"^dbuy: ^9you bought a parachute. press use in mid air to deploy\n\"");
	}
	// invalid item..
	else {
		CP("print \"^dbuy: ^9you want to buy something that is not in store\n\"");
		CP("print \"usage: /buy [ammo|health|para]\n\"");
		return;
	}

	// pay..
	ent->client->sess.credits -= CREDITS_PRICE;

	// only once per life. cleared on spawn
	ent->client->sess.buyTime = level.time;
}

void Cmd_Injure_f( gentity_t *ent )
{
	if(ent->client->sess.sessionTeam == TEAM_SPECTATOR ||
		(ent->client->ps.pm_flags & PMF_LIMBO) ||
		ent->health <= 0) {
			return;
	}

	G_Damage(ent,NULL,NULL,NULL,NULL,ent->health + 1,0, MOD_POISONGAS);
}

// credits to etpub. lua bound function only for mmod ( or some other lua admin system ).
qboolean G_FlingClient( gentity_t *vic, int flingType )
{
	vec3_t dir, flingvec;

	if(!vic || !vic->client)
		return qfalse;

	if(!(vic->client->sess.sessionTeam == TEAM_RED ||
		vic->client->sess.sessionTeam == TEAM_BLUE))
		return qfalse;

	if(vic->health <= 0)
		return qfalse;

	// plenty of room for improvement on setting the dir vector
	if(flingType == 0) {	//fling
		VectorSet(dir, crandom()*50, crandom()*50, 10);
	} else if(flingType == 1) {	//throw
		AngleVectors(vic->client->ps.viewangles, dir, NULL, NULL );
		dir[2] = .25f;
	} else {	// launch
		VectorSet(dir, 0, 0, 10);
	}
	VectorNormalize(dir);
	VectorScale(dir, 1500, flingvec);
	VectorAdd(vic->s.pos.trDelta, flingvec, vic->s.pos.trDelta);
	VectorAdd(vic->client->ps.velocity,
		flingvec,
		vic->client->ps.velocity);
	return qtrue;
}

qboolean G_FlingClientA( int flingType )
{
	int i;
	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[ i ].pers.connected == CON_CONNECTED ) {
			G_FlingClient( g_entities + i, flingType );
		}
	}

	return qtrue;
}

// cs: temp.
// TODO: remove
static char *temp[] =
{
	"",
};

void Cmd_ShowHashValue_f() {
	int i;
	for ( i = 0; i < sizeof( temp ) / sizeof( temp[0] ); ++i ) {
		G_Printf( "%i: /*%s*/\n", BG_StringHashValue( temp[i] ), temp[i] );
	}
}
// cs: end remove

// -NERVE - SMF

/*
=================
ClientCommand
=================
*/
void ClientCommand( int clientNum ) {
	gentity_t *ent;
	char cmd[MAX_TOKEN_CHARS];

	ent = g_entities + clientNum;
	if ( !ent->client ) {
		return;     // not fully in game yet
	}

	trap_Argv( 0, cmd, sizeof( cmd ) );

#ifdef WITH_LUA
	if ( Q_stricmp( cmd, "lua_status" ) == 0 ) {
		G_LuaStatus( ent );
		return;
	}

	if ( G_LuaHook_ClientCommand( clientNum, cmd ) ) {
		return;
	}
#endif // WITH_LUA

	if ( Q_stricmp( cmd, "say" ) == 0 ) {
		Cmd_Say_f( ent, SAY_ALL, qfalse );
		return;
	}
	if ( Q_stricmp( cmd, "say_team" ) == 0 ) {
		Cmd_Say_f( ent, SAY_TEAM, qfalse );
		return;
	}
	// NERVE - SMF
	if ( Q_stricmp( cmd, "say_limbo" ) == 0 ) {
		Cmd_Say_f( ent, SAY_LIMBO, qfalse );
		return;
	}
	if ( Q_stricmp( cmd, "vsay" ) == 0 ) {
		Cmd_Voice_f( ent, SAY_ALL, qfalse, qfalse );
		return;
	}
	if ( Q_stricmp( cmd, "vsay_team" ) == 0 ) {
		Cmd_Voice_f( ent, SAY_TEAM, qfalse, qfalse );
		return;
	}
	// -NERVE - SMF

	if ( Q_stricmp( cmd, "tell" ) == 0 ) {
		Cmd_Tell_f( ent );
		return;
	}
	if ( Q_stricmp( cmd, "score" ) == 0 ) {
		Cmd_Score_f( ent );
		return;
	}

	// NERVE - SMF - moved this here so current/new players can set team during scoreboard win
	if ( Q_stricmp( cmd, "team" ) == 0 ) {
		Cmd_Team_f( ent );
		return;
	}

	// ignore all other commands when at intermission
	if ( level.intermissiontime ) {
		return;
	}

	if ( Q_stricmp( cmd, "give" ) == 0 ) {
		Cmd_Give_f( ent );
	} else if ( Q_stricmp( cmd, "god" ) == 0 ) {
		Cmd_God_f( ent );
	} else if ( Q_stricmp( cmd, "nofatigue" ) == 0 ) {
		Cmd_Nofatigue_f( ent );
	} else if ( Q_stricmp( cmd, "notarget" ) == 0 ) {
		Cmd_Notarget_f( ent );
	} else if ( Q_stricmp( cmd, "noclip" ) == 0 ) {
		Cmd_Noclip_f( ent );
	} else if ( Q_stricmp( cmd, "kill" ) == 0 ) {
		Cmd_Kill_f( ent );
	} else if ( Q_stricmp( cmd, "levelshot" ) == 0 ) {
		Cmd_LevelShot_f( ent );
	} else if ( Q_stricmp( cmd, "follow" ) == 0 ) {
		Cmd_Follow_f( ent );
	} else if ( Q_stricmp( cmd, "follownext" ) == 0 ) {
		Cmd_FollowCycle_f( ent, 1 );
	} else if ( Q_stricmp( cmd, "followprev" ) == 0 ) {
		Cmd_FollowCycle_f( ent, -1 );
	} else if ( Q_stricmp( cmd, "where" ) == 0 ) {
		Cmd_Where_f( ent );
	} else if ( Q_stricmp( cmd, "callvote" ) == 0 ) {
		Cmd_CallVote_f( ent );
	} else if ( Q_stricmp( cmd, "vote" ) == 0 ) {
		Cmd_Vote_f( ent );
	} else if ( Q_stricmp( cmd, "setviewpos" ) == 0 ) {
		Cmd_SetViewpos_f( ent );
	} else if ( Q_stricmp( cmd, "setspawnpt" ) == 0 ) {
		Cmd_SetSpawnPoint_f( ent );
	} else if ( Q_stricmp( cmd, "stats" ) == 0 ) {
		Cmd_Stats_f( ent );
	} else if ( ( Q_stricmp( cmd, "pm" ) == 0 ) || ( Q_stricmp( cmd, "m" ) == 0 ) ) {
		G_PrivateMessage( ent, NULL, NULL, qfalse );
	} else if ( Q_stricmp( cmd, "sg" ) == 0 || ( Q_stricmp( cmd, "smoke" ) == 0 ) ) {
		Cmd_Smoke_f( ent );
	} else if ( Q_stricmp( cmd, "np" ) == 0 || ( Q_stricmp( cmd, "gp" ) == 0 ) ) {
		Cmd_NadePack_f( ent );
	} else if ( Q_stricmp( cmd, "giveammo" ) == 0 ) {
		Cmd_GiveAmmo_f( ent );
	} else if ( Q_stricmp( cmd, "mycredits" ) == 0 ) {
		if ( !(g_betting.integer & CREDITS_ENABLE) ) {
			trap_SendServerCommand( clientNum, "print \"betting is not enabled on this server\n\"" );
			return;
		}
		Cmd_MyCredits_f( ent );
	} else if ( Q_stricmp( cmd, "bet" ) == 0 ) {
		if ( !(g_betting.integer & CREDITS_ENABLE) ) {
			trap_SendServerCommand( clientNum, "print \"betting is not enabled on this server\n\"" );
			return;
		}
		Cmd_Bet_f( ent );
	} else if ( Q_stricmp( cmd, "accept" ) == 0 ) {
		if ( !(g_betting.integer & CREDITS_ENABLE) ) {
			trap_SendServerCommand( clientNum, "print \"betting is not enabled on this server\n\"" );
			return;
		}
		Cmd_Accept_f( ent );
	} else if ( Q_stricmp( cmd, "buy" ) == 0 ) {
		if ( !(g_betting.integer & CREDITS_BUYING) ) {
			trap_SendServerCommand( clientNum, "print \"buying is not enabled on this server\n\"" );
			return;
		}
		Cmd_Buy_f( ent );
	} else if (Q_stricmp (cmd, "injure") == 0) {
		Cmd_Injure_f (ent);
	} else if ( Q_stricmp( cmd, "shv" ) == 0 ) { // TODO: remove
		Cmd_ShowHashValue_f();
	} else {
		trap_SendServerCommand( clientNum, va( "print \"unknown cmd[lof] %s\n\"", cmd ) );
	}
}