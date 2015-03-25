#include "g_local.h"
#include  "g_rtcwbot_interface.h"

#ifdef WITH_LUA
#include "g_lua.h"
#endif

// g_client.c -- client functions that don't happen every frame

// Ridah, new bounding box
//static vec3_t	playerMins = {-15, -15, -24};
//static vec3_t	playerMaxs = {15, 15, 32};
vec3_t playerMins = {-18, -18, -24};
vec3_t playerMaxs = {18, 18, 48};
// done.

/*QUAKED info_player_deathmatch (1 0 1) (-16 -16 -24) (16 16 32) initial
potential spawning position for deathmatch games.
The first time a player enters the game, they will be at an 'initial' spot.
Targets will be fired when someone spawns in on them.
"nobots" will prevent bots from using this spot.
"nohumans" will prevent non-bots from using this spot.
If the start position is targeting an entity, the players camera will start out facing that ent (like an info_notnull)
*/
void SP_info_player_deathmatch( gentity_t *ent ) {
	int i;
	vec3_t dir;

	G_SpawnInt( "nobots", "0", &i );
	if ( i ) {
		ent->flags |= FL_NO_BOTS;
	}
	G_SpawnInt( "nohumans", "0", &i );
	if ( i ) {
		ent->flags |= FL_NO_HUMANS;
	}

	ent->enemy = G_PickTarget( ent->target );
	if ( ent->enemy ) {
		VectorSubtract( ent->enemy->s.origin, ent->s.origin, dir );
		vectoangles( dir, ent->s.angles );
	}
}

//----(SA) added
/*QUAKED info_player_checkpoint (1 0 0) (-16 -16 -24) (16 16 32) a b c d
these are start points /after/ the level start
the letter (a b c d) designates the checkpoint that needs to be complete in order to use this start position
*/
void SP_info_player_checkpoint( gentity_t *ent ) {
	ent->classname = "info_player_checkpoint";
	SP_info_player_deathmatch( ent );
}

//----(SA) end

/*QUAKED info_player_start (1 0 0) (-16 -16 -24) (16 16 32)
equivelant to info_player_deathmatch
*/
void SP_info_player_start( gentity_t *ent ) {
	ent->classname = "info_player_deathmatch";
	SP_info_player_deathmatch( ent );
}

/*QUAKED info_player_intermission (1 0 1) (-16 -16 -24) (16 16 32) AXIS ALLIED
The intermission will be viewed from this point.  Target an info_notnull for the view direction.
*/
void SP_info_player_intermission( gentity_t *ent ) {
}

/*
=======================================================================

SelectSpawnPoint

=======================================================================
*/

/*
================
SpotWouldTelefrag

================
*/
qboolean SpotWouldTelefrag( gentity_t *spot ) {
	int i, num;
	int touch[MAX_GENTITIES];
	gentity_t   *hit;
	vec3_t mins, maxs;

	VectorAdd( spot->s.origin, playerMins, mins );
	VectorAdd( spot->s.origin, playerMaxs, maxs );
	num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

	for ( i = 0 ; i < num ; i++ ) {
		hit = &g_entities[touch[i]];
		if ( hit->client && hit->client->ps.stats[STAT_HEALTH] > 0 ) {
			return qtrue;
		}
	}

	return qfalse;
}

/*
================
SelectNearestDeathmatchSpawnPoint

Find the spot that we DON'T want to use
================
*/
#define MAX_SPAWN_POINTS    128
gentity_t *SelectNearestDeathmatchSpawnPoint( vec3_t from ) {
	gentity_t   *spot;
	vec3_t delta;
	float dist, nearestDist;
	gentity_t   *nearestSpot;

	nearestDist = 999999;
	nearestSpot = NULL;
	spot = NULL;

	while ( ( spot = G_Find( spot, FOFS( classname ), "info_player_deathmatch" ) ) != NULL ) {
		VectorSubtract( spot->s.origin, from, delta );
		dist = VectorLength( delta );
		if ( dist < nearestDist ) {
			nearestDist = dist;
			nearestSpot = spot;
		}
	}

	return nearestSpot;
}

/*
================
SelectRandomDeathmatchSpawnPoint

go to a random point that doesn't telefrag
================
*/
#define MAX_SPAWN_POINTS    128
gentity_t *SelectRandomDeathmatchSpawnPoint( void ) {
	gentity_t   *spot;
	int count;
	int selection;
	gentity_t   *spots[MAX_SPAWN_POINTS];

	count = 0;
	spot = NULL;

	while ( ( spot = G_Find( spot, FOFS( classname ), "info_player_deathmatch" ) ) != NULL ) {
		if ( SpotWouldTelefrag( spot ) ) {
			continue;
		}
		spots[ count ] = spot;
		count++;
	}

	if ( !count ) { // no spots that won't telefrag
		return G_Find( NULL, FOFS( classname ), "info_player_deathmatch" );
	}

	selection = rand() % count;
	return spots[ selection ];
}

/*
===========
SelectSpawnPoint

Chooses a player start, deathmatch start, etc
============
*/
gentity_t *SelectSpawnPoint( vec3_t avoidPoint, vec3_t origin, vec3_t angles ) {
	gentity_t   *spot;
	gentity_t   *nearestSpot;

	nearestSpot = SelectNearestDeathmatchSpawnPoint( avoidPoint );

	spot = SelectRandomDeathmatchSpawnPoint();
	if ( spot == nearestSpot ) {
		// roll again if it would be real close to point of death
		spot = SelectRandomDeathmatchSpawnPoint();
		if ( spot == nearestSpot ) {
			// last try
			spot = SelectRandomDeathmatchSpawnPoint();
		}
	}

	// find a single player start spot
	if ( !spot ) {
		G_Error( "Couldn't find a spawn point" );
		return 0; // for compiler warning
	}

	VectorCopy( spot->s.origin, origin );
	origin[2] += 9;
	VectorCopy( spot->s.angles, angles );

	return spot;
}

/*
===========
SelectInitialSpawnPoint

Try to find a spawn point marked 'initial', otherwise
use normal spawn selection.
============
*/
gentity_t *SelectInitialSpawnPoint( vec3_t origin, vec3_t angles ) {
	gentity_t   *spot;

	spot = NULL;
	while ( ( spot = G_Find( spot, FOFS( classname ), "info_player_deathmatch" ) ) != NULL ) {
		if ( spot->spawnflags & 1 ) {
			break;
		}
	}

	if ( !spot || SpotWouldTelefrag( spot ) ) {
		return SelectSpawnPoint( vec3_origin, origin, angles );
	}

	VectorCopy( spot->s.origin, origin );
	origin[2] += 9;
	VectorCopy( spot->s.angles, angles );

	return spot;
}

/*
===========
SelectSpectatorSpawnPoint

============
*/
gentity_t *SelectSpectatorSpawnPoint( vec3_t origin, vec3_t angles ) {
	FindIntermissionPoint();

	VectorCopy( level.intermission_origin, origin );
	VectorCopy( level.intermission_angle, angles );

	return NULL;
}

/*
=======================================================================

BODYQUE

=======================================================================
*/

/*
===============
InitBodyQue
===============
*/
void InitBodyQue( void ) {
	int i;
	gentity_t   *ent;

	level.bodyQueIndex = 0;
	for ( i = 0; i < BODY_QUEUE_SIZE ; i++ ) {
		ent = G_Spawn();
		ent->classname = "bodyque";
		ent->neverFree = qtrue;
		level.bodyQue[i] = ent;
	}
}

/*
=============
BodySink

After sitting around for five seconds, fall into the ground and dissapear
=============
*/
void BodySink( gentity_t *ent ) {
	if ( level.time - ent->timestamp > 6500 ) {
		// the body ques are never actually freed, they are just unlinked
		trap_UnlinkEntity( ent );
		ent->physicsObject = qfalse;
		return;
	}
	ent->nextthink = level.time + 100;
	ent->s.pos.trBase[2] -= 1;
}

/*
=============
CopyToBodyQue

A player is respawning, so make an entity that looks
just like the existing corpse to leave behind.
=============
*/
void CopyToBodyQue( gentity_t *ent ) {
	gentity_t       *body;
	int contents, i;

	trap_UnlinkEntity( ent );

	// if client is in a nodrop area, don't leave the body
	contents = trap_PointContents( ent->s.origin, -1 );
	if ( contents & CONTENTS_NODROP ) {
		return;
	}

	// grab a body que and cycle to the next one
	body = level.bodyQue[ level.bodyQueIndex ];
	level.bodyQueIndex = ( level.bodyQueIndex + 1 ) % BODY_QUEUE_SIZE;

	//trap_UnlinkEntity (body);

	body->s = ent->s;
	body->s.eFlags = EF_DEAD;       // clear EF_TALK, etc

	if ( ent->client->ps.eFlags & EF_HEADSHOT ) {
		body->s.eFlags |= EF_HEADSHOT;          // make sure the dead body draws no head (if killed that way)
	}

	body->s.eType = ET_CORPSE;
	body->classname = "corpse";
	body->s.powerups = 0;   // clear powerups
	body->s.loopSound = 0;  // clear lava burning
	body->s.number = body - g_entities;
	body->timestamp = level.time;
	body->physicsObject = qtrue;
	body->physicsBounce = 0;        // don't bounce
	if ( body->s.groundEntityNum == ENTITYNUM_NONE ) {
		body->s.pos.trType = TR_GRAVITY;
		body->s.pos.trTime = level.time;
		VectorCopy( ent->client->ps.velocity, body->s.pos.trDelta );
	} else {
		body->s.pos.trType = TR_STATIONARY;
	}
	body->s.event = 0;

	// DHM - Clear out event system
	for ( i = 0; i < MAX_EVENTS; i++ )
		body->s.events[i] = 0;
	body->s.eventSequence = 0;

	// change the animation to the last-frame only, so the sequence
	// doesn't repeat anew for the body
	switch ( body->s.legsAnim & ~ANIM_TOGGLEBIT ) {
	case BOTH_DEATH1:
	case BOTH_DEAD1:
		body->s.torsoAnim = body->s.legsAnim = BOTH_DEAD1;
		break;
	case BOTH_DEATH2:
	case BOTH_DEAD2:
		body->s.torsoAnim = body->s.legsAnim = BOTH_DEAD2;
		break;
	case BOTH_DEATH3:
	case BOTH_DEAD3:
	default:
		body->s.torsoAnim = body->s.legsAnim = BOTH_DEAD3;
		break;
	}

	body->r.svFlags = ent->r.svFlags & ~SVF_BOT;
	VectorCopy( ent->r.mins, body->r.mins );
	VectorCopy( ent->r.maxs, body->r.maxs );
	VectorCopy( ent->r.absmin, body->r.absmin );
	VectorCopy( ent->r.absmax, body->r.absmax );

	body->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;
	// DHM - Nerve :: allow bullets to pass through bbox
	body->r.contents = 0;
	body->r.ownerNum = ent->r.ownerNum;

	body->nextthink = level.time + 5000;
	body->think = BodySink;

	body->die = body_die;

	// don't take more damage if already gibbed
	if ( ent->health <= GIB_HEALTH ) {
		body->takedamage = qfalse;
	} else {
		body->takedamage = qtrue;
	}

	VectorCopy( body->s.pos.trBase, body->r.currentOrigin );
	trap_LinkEntity( body );
}

//======================================================================

/*
==================
SetClientViewAngle

==================
*/
void SetClientViewAngle( gentity_t *ent, vec3_t angle ) {
	int i;

	// set the delta angle
	for ( i = 0 ; i < 3 ; i++ ) {
		int cmdAngle;

		cmdAngle = ANGLE2SHORT( angle[i] );
		ent->client->ps.delta_angles[i] = cmdAngle - ent->client->pers.cmd.angles[i];
	}
	VectorCopy( angle, ent->s.angles );
	VectorCopy( ent->s.angles, ent->client->ps.viewangles );
}

/* JPW NERVE
================
limbo
================
*/
void limbo( gentity_t *ent, qboolean makeCorpse ) {
	if ( !( ent->client->ps.pm_flags & PMF_LIMBO ) ) {
		int i,contents;
		int startclient = ent->client->ps.clientNum;

		// DHM - Nerve :: First save off persistant info we'll need for respawn
		for ( i = 0; i < MAX_PERSISTANT; i++ )
			ent->client->saved_persistant[i] = ent->client->ps.persistant[i];
		// dhm

		ent->client->ps.pm_flags |= PMF_LIMBO;

		if ( !(ent->r.svFlags & SVF_BOT) ) {
			ent->client->ps.pm_flags |= PMF_FOLLOW;
		}

		if ( !ent->client->sess.botSuicidePersist || g_gamestate.integer != GS_PLAYING ) {
			ent->client->sess.botSuicide = qfalse;
		}

		if ( makeCorpse ) {
			CopyToBodyQue( ent ); // make a nice looking corpse
		} else {
			trap_UnlinkEntity( ent );
		}

		// DHM - Nerve :: reset these values
		ent->client->ps.viewlocked = 0;
		ent->client->ps.viewlocked_entNum = 0;

		ent->r.maxs[2] = 0;
		ent->r.currentOrigin[2] += 8;
		contents = trap_PointContents( ent->r.currentOrigin, -1 ); // drop stuff
		ent->s.weapon = ent->client->limboDropWeapon; // stored in player_die()
		if ( makeCorpse && !( contents & CONTENTS_NODROP ) ) {
			TossClientItems( ent );
		}
		//bots can't follow
		if ( ent->r.svFlags & SVF_BOT ) {
			ent->client->sess.spectatorClient = ent->client->ps.clientNum;
			ent->client->sess.spectatorState = SPECTATOR_FREE;
		} else
		{
			ent->client->sess.spectatorClient = startclient;
			Cmd_FollowCycle_f( ent,1 ); // get fresh spectatorClient

			if ( ent->client->sess.spectatorClient == startclient ) {
				// No one to follow, so just stay put
				ent->client->sess.spectatorState = SPECTATOR_FREE;
			} else {
				ent->client->sess.spectatorState = SPECTATOR_FOLLOW;
			}
		}

		if ( ent->client->sess.sessionTeam == TEAM_RED ) {
			ent->client->deployQueueNumber = level.redNumWaiting;
			level.redNumWaiting++;
		} else if ( ent->client->sess.sessionTeam == TEAM_BLUE ) {
			ent->client->deployQueueNumber = level.blueNumWaiting;
			level.blueNumWaiting++;
		}

		for ( i = 0 ; i < level.maxclients ; i++ ) {
			if ( ( &g_entities[i] )->r.svFlags & SVF_BOT ) {
				continue;
			}
		}

		for ( i = 0 ; i < level.maxclients ; i++ ) {
			if ( level.clients[i].ps.pm_flags & PMF_LIMBO
				&& level.clients[i].sess.spectatorClient == ent->s.number ) {
					Cmd_FollowCycle_f( &g_entities[i], 1 );
			}
		}
	}
}

/* JPW NERVE
================
reinforce
================
// -- called when time expires for a team deployment cycle and there is at least one guy ready to go
*/
void reinforce( gentity_t *ent ) {
	int p;
	gclient_t *rclient;

	if ( !( ent->client->ps.pm_flags & PMF_LIMBO ) ) {
		G_Printf( "player already deployed, skipping\n" );
		return;
	}

	// DHM - Nerve :: restore persistant data now that we're out of Limbo
	rclient = ent->client;
	for ( p = 0; p < MAX_PERSISTANT; p++ )
		rclient->ps.persistant[p] = rclient->saved_persistant[p];
	// dhm

	respawn( ent );
}
// jpw

/*
================
respawn
================
*/
void respawn( gentity_t *ent ) {
	ent->client->ps.pm_flags &= ~PMF_LIMBO; // JPW NERVE turns off limbo

	// DHM - Nerve :: Decrease the number of respawns left
	if ( g_maxlives.integer > 0 && ent->client->ps.persistant[PERS_RESPAWNS_LEFT] > 0 ) {
		ent->client->ps.persistant[PERS_RESPAWNS_LEFT]--;
	}

	G_DPrintf( "Respawning %s, %i lives left\n", ent->client->pers.netname, ent->client->ps.persistant[PERS_RESPAWNS_LEFT] );

	// DHM - Nerve :: Already handled in 'limbo()'
	if ( g_gametype.integer < GT_WOLF ) {
		CopyToBodyQue( ent );
	}

	ClientSpawn( ent, qfalse );
}

// NERVE - SMF - merge from team arena
/*
================
TeamCount

Returns number of players on a team
================
*/
team_t TeamCount( int ignoreClientNum, int team ) {
	int i;
	int count = 0;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( i == ignoreClientNum ) {
			continue;
		}
		if ( level.clients[i].pers.connected == CON_DISCONNECTED ) {
			continue;
		}
		if ( level.clients[i].sess.sessionTeam == team ) {
			count++;
		}
	}

	return count;
}
// -NERVE - SMF

/*
================
PickTeam

================
*/
team_t PickTeam( int ignoreClientNum ) {
	int i;
	int counts[TEAM_NUM_TEAMS];

	memset( counts, 0, sizeof( counts ) );

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( i == ignoreClientNum ) {
			continue;
		}
		if ( level.clients[i].pers.connected == CON_DISCONNECTED ) {
			continue;
		}
		if ( level.clients[i].sess.sessionTeam == TEAM_BLUE ) {
			counts[TEAM_BLUE]++;
		} else if ( level.clients[i].sess.sessionTeam == TEAM_RED ) {
			counts[TEAM_RED]++;
		}
	}

	if ( counts[TEAM_BLUE] > counts[TEAM_RED] ) {
		return TEAM_RED;
	}
	if ( counts[TEAM_RED] > counts[TEAM_BLUE] ) {
		return TEAM_BLUE;
	}
	// equal team count, so join the team with the lowest score
	if ( level.teamScores[TEAM_BLUE] > level.teamScores[TEAM_RED] ) {
		return TEAM_RED;
	}
	return TEAM_BLUE;
}

/*
===========
ForceClientSkin

Forces a client's skin (for teamplay)
===========
*/
void ForceClientSkin( gclient_t *client, char *model, const char *skin ) {
	char *p;

	if ( ( p = strchr( model, '/' ) ) != NULL ) {
		*p = 0;
	}

	Q_strcat( model, MAX_QPATH, "/" );
	Q_strcat( model, MAX_QPATH, skin );
}

// NERVE - SMF
/*
===========
SetWolfUserVars
===========
*/
void SetWolfUserVars( gentity_t *ent, char *wolfinfo ) {
	gclient_t *client;
	int mask, team;

	client = ent->client;
	if ( !client ) {
		return;
	}

	// check if we have a valid snapshot
	mask = MP_TEAM_MASK;
	team = ( client->pers.cmd.mpSetup & mask ) >> MP_TEAM_OFFSET;

	if ( !team ) {
		return;
	}

	// set player class
	mask = MP_CLASS_MASK;
	client->sess.latchPlayerType = ( client->pers.cmd.mpSetup & mask ) >> MP_CLASS_OFFSET;

	// set weapon
	mask = MP_WEAPON_MASK;
	client->sess.latchPlayerWeapon = ( client->pers.cmd.mpSetup & mask ) >> MP_WEAPON_OFFSET;
}

// -NERVE - SMF

// DHM - Nerve
/*
===========
SetWolfSkin

Forces a client's skin (for Wolfenstein teamplay)
===========
*/

#define MULTIPLAYER_ALLIEDMODEL "multi"
#define MULTIPLAYER_AXISMODEL   "multi_axis"

void SetWolfSkin( gclient_t *client, char *model ) {
	switch ( client->sess.sessionTeam ) {
	case TEAM_RED:
		Q_strcat( model, MAX_QPATH, "red" );
		break;
	case TEAM_BLUE:
		Q_strcat( model, MAX_QPATH, "blue" );
		break;
	default:
		Q_strcat( model, MAX_QPATH, "red" );
		break;
	}

	switch ( client->sess.playerType ) {
	case PC_SOLDIER:
		Q_strcat( model, MAX_QPATH, "soldier" );
		break;
	case PC_MEDIC:
		Q_strcat( model, MAX_QPATH, "medic" );
		break;
	case PC_ENGINEER:
		Q_strcat( model, MAX_QPATH, "engineer" );
		break;
	case PC_LT:
		Q_strcat( model, MAX_QPATH, "lieutenant" );
		break;
	default:
		Q_strcat( model, MAX_QPATH, "soldier" );
		break;
	}

	// DHM - A skinnum will be in the session data soon...
	switch ( client->sess.playerSkin ) {
	case 1:
		Q_strcat( model, MAX_QPATH, "1" );
		break;
	case 2:
		Q_strcat( model, MAX_QPATH, "2" );
		break;
	case 3:
		Q_strcat( model, MAX_QPATH, "3" );
		break;
	default:
		Q_strcat( model, MAX_QPATH, "1" );
		break;
	}
}

void AddPlayerWeapon(gclient_t *client, weapon_t weapon, int ammo, int ammoclip)
{
	COM_BitSet( client->ps.weapons, weapon );
	client->ps.ammo[BG_FindAmmoForWeapon( weapon )] = ammo;
	client->ps.ammoclip[BG_FindClipForWeapon( weapon )] = ammoclip;
}

// cs: todo: rewrite this function ...
void SetWolfSpawnWeapons( gclient_t *client ) {
	int pc = client->sess.playerType;
	int starthealth = 100,i,numMedics = 0;   // JPW NERVE
	// extra clips - capped
	int extraMed = ( g_medExtraClips.integer <= 9 ) ? g_medExtraClips.integer : 9;
	int extraLT  = ( g_ltExtraClips.integer <= 9 ) ? g_ltExtraClips.integer : 9;
	int extraEng = ( g_engExtraClips.integer <= 9 ) ? g_engExtraClips.integer : 9;
	int extraSold = ( g_soldExtraClips.integer <= 9 ) ? g_soldExtraClips.integer : 9;
	int extraPistol = ( g_pistolExtraClips.integer <= 3 ) ? g_pistolExtraClips.integer : 3;
	int extraMauser = ( g_mauserExtraClips.integer <= 3 ) ? g_mauserExtraClips.integer : 3;
	int extraPanz   = ( g_panzerExtraClips.integer <= 3 ) ? g_panzerExtraClips.integer : 3;
	int extraVenom  = ( g_venomExtraClips.integer <= 3 ) ? g_venomExtraClips.integer : 3;
	int classModifier = 0;

	if ( client->sess.sessionTeam == TEAM_SPECTATOR ) {
		return;
	}
	
	// Reset special weapon time
	client->ps.classWeaponTime = -999999;

	// Communicate it to cgame
	client->ps.stats[STAT_PLAYER_CLASS] = pc;

	// Abuse teamNum to store player class as well (can't see stats for all clients in cgame)
	client->ps.teamNum = pc;

	// JPW NERVE -- zero out all ammo counts
	memset( client->ps.ammo,MAX_WEAPONS,sizeof( int ) );

	// All players start with a knife (not OR-ing so that it clears previous weapons)
	client->ps.weapons[0] = 0;
	client->ps.weapons[1] = 0;

	AddPlayerWeapon(client, WP_KNIFE, g_throwableKnives.integer > 0 ? g_throwableKnives.integer : 0, 1);
	client->ps.weapon = WP_KNIFE;
	client->ps.weaponstate = WEAPON_READY;

	// extra ammo modifier
	switch(pc)
	{
	case PC_ENGINEER:
		classModifier = extraEng + 1;
		break;
	case PC_MEDIC:
		classModifier = extraMed;
		break;
	case PC_LT:
		classModifier = extraLT + 1;
		break;
	case PC_SOLDIER:
		classModifier = extraSold + 2;
		break;
	default:
		break;
	}

	// Engineer gets dynamite
	if ( pc == PC_ENGINEER ) {
		AddPlayerWeapon(client, WP_DYNAMITE, 0, 1);
		AddPlayerWeapon(client, WP_PLIERS, 1, 1);

		if ( g_binocs.integer & BINOCS_ENG ) {
			client->ps.stats[STAT_KEYS] |= ( 1 << INV_BINOCS );
		}
	}

	// Medics always get syringe and medkit
	if ( pc == PC_MEDIC ) {
		AddPlayerWeapon(client, WP_MEDIC_SYRINGE, 0, 10);
		AddPlayerWeapon(client, WP_MEDKIT, 1, 1);

		if ( g_binocs.integer & BINOCS_MEDIC ) {
			client->ps.stats[STAT_KEYS] |= ( 1 << INV_BINOCS );
		}
	}

	if ( pc == PC_SOLDIER && (g_binocs.integer & BINOCS_SOLDIER) ) {
		client->ps.stats[STAT_KEYS] |= ( 1 << INV_BINOCS );
	}

	if ( g_knifeonly.integer != 1 && !g_instaGib.integer )
	{
		// Lieutenant gets binoculars, ammo pack, artillery, and a grenade
		if ( pc == PC_LT ) {
			client->ps.stats[STAT_KEYS] |= ( 1 << INV_BINOCS );
			AddPlayerWeapon(client, WP_AMMO, 0, 1);
			AddPlayerWeapon(client, WP_ARTY, 0, 1);
			AddPlayerWeapon(client, WP_BINOCULARS, 0, 1);
			AddPlayerWeapon(client, WP_SMOKE_GRENADE, 1, 1);
			AddPlayerWeapon(client, client->sess.sessionTeam == TEAM_RED ? WP_GRENADE_LAUNCHER : WP_GRENADE_PINEAPPLE, 0, g_LTNades.integer ? g_LTNades.integer : 1);
		}

		// Everyone gets a pistol
		AddPlayerWeapon(client, client->sess.sessionTeam == TEAM_RED ? WP_LUGER : WP_COLT, 24 + ( 8 * extraPistol ), 8);

		// Everyone except Medic and LT get some grenades
		if ( ( pc != PC_LT ) && ( pc != PC_MEDIC ) ) { // JPW NERVE
			AddPlayerWeapon(client, client->sess.sessionTeam == TEAM_RED ? WP_GRENADE_LAUNCHER : WP_GRENADE_PINEAPPLE, 0, 4 + 4 * ( pc == PC_ENGINEER ));
		}

		// JPW NERVE
		if ( pc == PC_MEDIC ) {
			AddPlayerWeapon(client, client->sess.sessionTeam == TEAM_RED ? WP_GRENADE_LAUNCHER : WP_GRENADE_PINEAPPLE, 0, g_MedNades.integer ? g_MedNades.integer : 1);
		}
		// jpw

		// 2 handed weapons
		if ( pc == PC_SOLDIER || pc == PC_LT || !(g_unlockWeapons.integer & WEAPONUNLOCK_NOSPAWN) ) {
			// JPW NERVE -- if LT is selected but illegal weapon, set to team-specific SMG
			if ( !(g_unlockWeapons.integer & WEAPONUNLOCK_WEAPONS) && ( pc == PC_LT ) && ( client->sess.playerWeapon > 5 ) ) {
				if ( client->sess.sessionTeam == TEAM_RED ) {
					client->sess.playerWeapon = 3;
				} else {
					client->sess.playerWeapon = 4;
				}
			}
			// jpw
			switch ( client->sess.playerWeapon ) {
			case 3:     // WP_MP40
				AddPlayerWeapon(client, WP_MP40, 32 * classModifier, 32);
				client->ps.weapon = WP_MP40;
				break;

			case 4:     // WP_THOMPSON
				AddPlayerWeapon(client, WP_THOMPSON, 30 * classModifier, 30);
				client->ps.weapon = WP_THOMPSON;
				break;

			case 5:     // WP_STEN
				AddPlayerWeapon(client, WP_STEN, 32 * classModifier, 32);
				client->ps.weapon = WP_STEN;
				break;

			case 6:     // WP_MAUSER, WP_SNIPERRIFLE
				if ( pc != PC_SOLDIER && !g_unlockWeapons.integer ) {
					return;
				}

				AddPlayerWeapon(client, WP_SNIPERRIFLE, 10 + ( 10 * extraMauser ), 10);
				AddPlayerWeapon(client, WP_MAUSER, 10 + ( 10 * extraMauser ), 10);
				client->ps.weapon = WP_MAUSER;
				break;

			case 8:     // WP_PANZERFAUST
				if ( pc != PC_SOLDIER && !g_unlockWeapons.integer ) {
					return;
				}

				AddPlayerWeapon(client, WP_PANZERFAUST, 4 + extraPanz, 0);
				client->ps.weapon = WP_PANZERFAUST;
				break;

			case 9:     // WP_VENOM
				if ( pc != PC_SOLDIER && !g_unlockWeapons.integer ) {
					return;
				}

				AddPlayerWeapon(client, WP_VENOM, 500 * extraVenom, 500);
				client->ps.weapon = WP_VENOM;
				break;

			case 10:    // WP_FLAMETHROWER
				if ( pc != PC_SOLDIER && !g_unlockWeapons.integer ) {
					return;
				}

				AddPlayerWeapon(client, WP_FLAMETHROWER, 0, 200);
				client->ps.weapon = WP_FLAMETHROWER;
				break;

			default:    // give MP40 if given invalid weapon number
				if ( client->sess.sessionTeam == TEAM_RED ) { // JPW NERVE
					AddPlayerWeapon(client, WP_MP40, 32 * classModifier, 32);
					client->ps.weapon = WP_MP40;
				} else { // TEAM_BLUE
					AddPlayerWeapon(client, WP_THOMPSON, 30 * classModifier, 30);
					client->ps.weapon = WP_THOMPSON;
				}
				break;
			}
		} else { // medic or engineer gets assigned MP40 or Thompson with one magazine ammo
			if ( client->sess.sessionTeam == TEAM_RED ) {
				AddPlayerWeapon(client, WP_MP40, 32 * classModifier, 32);
				client->ps.weapon = WP_MP40;
			} else {
				AddPlayerWeapon(client, WP_THOMPSON, 30 * classModifier, 30);
				client->ps.weapon = WP_THOMPSON;
			}
		}
	} else if ( g_instaGib.integer == 1 ) {
		AddPlayerWeapon(client, WP_MAUSER,  30 + ( 10 * extraMauser ), 10);
		client->ps.weapon = WP_MAUSER;

		// Lieutenant gets binoculars, ammo pack, artillery, and a grenade
		if ( pc == PC_LT ) {
			client->ps.stats[STAT_KEYS] |= ( 1 << INV_BINOCS );
			AddPlayerWeapon(client, WP_AMMO, 0, 1);
			AddPlayerWeapon(client, WP_ARTY, 0, 1);
			AddPlayerWeapon(client, WP_BINOCULARS, 0, 1);
			AddPlayerWeapon(client, WP_SMOKE_GRENADE, 1, 1);
		}

		// Everyone gets a pistol
		AddPlayerWeapon(client, client->sess.sessionTeam == TEAM_RED ? WP_LUGER : WP_COLT, 24 + ( 8 * extraPistol ), 8);
	}

	// JPW NERVE -- medics on each team make cumulative health bonus -- this gets overridden for "revived" players
	// count up # of medics on team
	for ( i = 0; i < level.maxclients; i++ ) {
		if ( level.clients[i].pers.connected != CON_CONNECTED ) {
			continue;
		}
		if ( level.clients[i].sess.sessionTeam != client->sess.sessionTeam ) {
			continue;
		}
		if ( level.clients[i].ps.stats[STAT_PLAYER_CLASS] != PC_MEDIC ) {
			continue;
		}
		numMedics++;
	}

	// compute health mod
	starthealth = 100 + 10 * numMedics;
	if ( starthealth > 125 ) {
		starthealth = 125;
	}

	// give everybody health mod in stat_max_health
	for ( i = 0; i < level.maxclients; i++ ) {
		if ( level.clients[i].pers.connected != CON_CONNECTED ) {
			continue;
		}
		if ( level.clients[i].sess.sessionTeam == client->sess.sessionTeam ) {
			client->ps.stats[STAT_MAX_HEALTH] = starthealth;
		}
	}
	// jpw
}
// dhm - end

/*
===========
ClientCheckName
============
*/
static void ClientCleanName( const char *in, char *out, int outSize ) {
	int len, colorlessLen;
	char ch;
	char    *p;
	int spaces;

	//save room for trailing null byte
	outSize--;

	len = 0;
	colorlessLen = 0;
	p = out;
	*p = 0;
	spaces = 0;

	while ( 1 ) {
		ch = *in++;
		if ( !ch ) {
			break;
		}

		// don't allow leading spaces
		if ( !*p && ch == ' ' ) {
			continue;
		}

		// check colors
		if ( ch == Q_COLOR_ESCAPE ) {
			// solo trailing carat is not a color prefix
			if ( !*in ) {
				break;
			}

			// don't allow black in a name, period
			//why not?
			//			if( ColorIndex(*in) == 0 ) {
			//				in++;
			//				continue;
			//			}

			// make sure room in dest for both chars
			if ( len > outSize - 2 ) {
				break;
			}

			*out++ = ch;
			*out++ = *in++;
			len += 2;
			continue;
		}

		// don't allow too many consecutive spaces
		if ( ch == ' ' ) {
			spaces++;
			if ( spaces > 3 ) {
				continue;
			}
		} else {
			spaces = 0;
		}

		if ( len > outSize - 1 ) {
			break;
		}

		*out++ = ch;
		colorlessLen++;
		len++;
	}
	*out = 0;

	// don't allow empty names
	if ( *p == 0 || colorlessLen == 0 ) {
		Q_strncpyz( p, "UnnamedPlayer", outSize );
	}
}

/*
==================
G_CheckForExistingModelInfo

If this player model has already been parsed, then use the existing information.
Otherwise, set the modelInfo pointer to the first free slot.

returns qtrue if existing model found, qfalse otherwise
==================
*/
qboolean G_CheckForExistingModelInfo( gclient_t *cl, char *modelName, animModelInfo_t **modelInfo ) {
	int i;
	animModelInfo_t *trav, *firstFree = NULL;
	gclient_t *cl_trav;
	char modelsUsed[MAX_ANIMSCRIPT_MODELS];

	for ( i = 0, trav = level.animScriptData.modelInfo; i < MAX_ANIMSCRIPT_MODELS; i++, trav++ ) {
		if ( trav->modelname[0] ) {
			if ( !Q_stricmp( trav->modelname, modelName ) ) {
				// found a match, use this modelinfo
				*modelInfo = trav;
				level.animScriptData.clientModels[cl->ps.clientNum] = i + 1;
				return qtrue;
			}
		} else if ( !firstFree ) {
			firstFree = trav;
			level.animScriptData.clientModels[cl->ps.clientNum] = i + 1;
		}
	}

	// set the modelInfo to the first free slot
	if ( !firstFree ) {
		// attempt to free a model that is no longer being used
		memset( modelsUsed, 0, sizeof( modelsUsed ) );
		for ( i = 0, cl_trav = level.clients; i < MAX_CLIENTS; i++, cl_trav++ ) {
			if ( cl_trav != cl && g_entities[cl_trav->ps.clientNum].inuse && cl_trav->modelInfo ) {
				modelsUsed[ (int)( cl_trav->modelInfo - level.animScriptData.modelInfo ) ] = 1;
			}
		}
		// now use the first slot that isn't being utilized
		for ( i = 0, trav = level.animScriptData.modelInfo; i < MAX_ANIMSCRIPT_MODELS; i++, trav++ ) {
			if ( !modelsUsed[i] ) {
				firstFree = trav;
				level.animScriptData.clientModels[cl->ps.clientNum] = i + 1;
				break;
			}
		}
	}

	if ( !firstFree ) {
		G_Error( "unable to find a free modelinfo slot, cannot continue\n" );
	} else {
		*modelInfo = firstFree;
		// clear the structure out ready for use
		memset( *modelInfo, 0, sizeof( *modelInfo ) );
	}
	// qfalse signifies that we need to parse the information from the script files
	return qfalse;
}

/*
=============
G_ParseAnimationFiles
=============
*/
static char text[100000];                   // <- was causing callstacks >64k

qboolean G_ParseAnimationFiles( char *modelname, gclient_t *cl ) {
	char filename[MAX_QPATH];
	fileHandle_t f;
	int len;

	// set the name of the model in the modelinfo structure
	Q_strncpyz( cl->modelInfo->modelname, modelname, sizeof( cl->modelInfo->modelname ) );

	// load the cfg file
	Com_sprintf( filename, sizeof( filename ), "models/players/%s/wolfanim.cfg", modelname );
	len = trap_FS_FOpenFile( filename, &f, FS_READ );
	if ( len <= 0 ) {
		G_Printf( "G_ParseAnimationFiles(): file '%s' not found\n", filename );     //----(SA)	added
		return qfalse;
	}
	if ( len >= sizeof( text ) - 1 ) {
		G_Printf( "File %s too long\n", filename );
		return qfalse;
	}
	trap_FS_Read( text, len, f );
	text[len] = 0;
	trap_FS_FCloseFile( f );

	// parse the text
	BG_AnimParseAnimConfig( cl->modelInfo, filename, text );

	// load the script file
	Com_sprintf( filename, sizeof( filename ), "models/players/%s/wolfanim.script", modelname );
	len = trap_FS_FOpenFile( filename, &f, FS_READ );
	if ( len <= 0 ) {
		if ( cl->modelInfo->version > 1 ) {
			return qfalse;
		}
		// try loading the default script for old legacy models
		Com_sprintf( filename, sizeof( filename ), "models/players/default.script", modelname );
		len = trap_FS_FOpenFile( filename, &f, FS_READ );
		if ( len <= 0 ) {
			return qfalse;
		}
	}
	if ( len >= sizeof( text ) - 1 ) {
		G_Printf( "File %s too long\n", filename );
		return qfalse;
	}
	trap_FS_Read( text, len, f );
	text[len] = 0;
	trap_FS_FCloseFile( f );

	// parse the text
	BG_AnimParseAnimScript( cl->modelInfo, &level.animScriptData, cl->ps.clientNum, filename, text );

	return qtrue;
}

/*
===========
ClientUserInfoChanged

Called from ClientConnect when the player first connects and
directly by the server system when the player updates a userinfo variable.

The game can override any of the settings and call trap_SetUserinfo
if desired.
============
*/
void ClientUserinfoChanged( int clientNum ) {
	gentity_t *ent;
	char    *s;
	char model[MAX_QPATH], modelname[MAX_QPATH];

	//----(SA) added this for head separation
	char head[MAX_QPATH];

	char oldname[MAX_STRING_CHARS];
	gclient_t   *client;
	char    *c1;
	char userinfo[MAX_INFO_STRING];

	ent = g_entities + clientNum;
	client = ent->client;

	client->ps.clientNum = clientNum;

	trap_GetUserinfo( clientNum, userinfo, sizeof( userinfo ) );

	// check for malformed or illegal info strings
	if ( !Info_Validate( userinfo ) ) {
		Q_strncpyz( userinfo, "\\name\\badinfo", sizeof( userinfo ) );
	}

	// check for local client
	s = Info_ValueForKey( userinfo, "ip" );
	if ( s && !strcmp( s, "localhost" ) ) {
		client->pers.localClient = qtrue;
	}

	// cs: added for zinx etpro antiwarp
	client->pers.pmoveMsec = pmove_msec.integer;

	if ( ent->r.svFlags & SVF_BOT ) {
		client->pers.autoActivate = PICKUP_TOUCH;
		client->pers.bAutoReloadAux = qtrue;
		client->pmext.bAutoReload = qtrue;
		client->pers.predictItemPickup = qfalse;
		client->pers.pmoveFixed = qfalse;
	} else {
		// check the item prediction
		s = Info_ValueForKey( userinfo, "cg_predictItems" );
		if ( !atoi( s ) ) {
			client->pers.predictItemPickup = qfalse;
		} else {
			client->pers.predictItemPickup = qtrue;
		}

		// check the auto activation
		s = Info_ValueForKey( userinfo, "cg_autoactivate" );
		if ( !atoi( s ) ) {
			client->pers.autoActivate = PICKUP_ACTIVATE;
		} else {
			client->pers.autoActivate = PICKUP_TOUCH;
		}

		// check the auto reload setting
		s = Info_ValueForKey( userinfo, "cg_autoReload" );
		if ( atoi( s ) ) {
			client->pers.bAutoReloadAux = qtrue;
			client->pmext.bAutoReload = qtrue;
		} else {
			client->pers.bAutoReloadAux = qfalse;
			client->pmext.bAutoReload = qfalse;
		}
	}
	// set name
	Q_strncpyz( oldname, client->pers.netname, sizeof( oldname ) );
	s = Info_ValueForKey( userinfo, "name" );
	ClientCleanName( s, client->pers.netname, sizeof( client->pers.netname ) );

	if ( client->sess.sessionTeam == TEAM_SPECTATOR ) {
		if ( client->sess.spectatorState == SPECTATOR_SCOREBOARD ) {
			Q_strncpyz( client->pers.netname, "scoreboard", sizeof( client->pers.netname ) );
		}
	}

	if ( client->pers.connected == CON_CONNECTED ) {
		if ( strcmp( oldname, client->pers.netname ) ) {
			trap_SendServerCommand( -1, va( "print \"[lof]%s" S_COLOR_WHITE " [lon]renamed to[lof] %s\n\"", oldname,
				client->pers.netname ) );
		}
	}

	// set max health
	client->pers.maxHealth = atoi( Info_ValueForKey( userinfo, "handicap" ) );
	if ( client->pers.maxHealth < 1 || client->pers.maxHealth > 100 ) {
		client->pers.maxHealth = 100;
	}
	client->ps.stats[STAT_MAX_HEALTH] = client->pers.maxHealth;

	// set model
	if ( g_forceModel.integer ) {
		Q_strncpyz( model, DEFAULT_MODEL, sizeof( model ) );
		Q_strcat( model, sizeof( model ), "/default" );
	} else {
		Q_strncpyz( model, Info_ValueForKey( userinfo, "model" ), sizeof( model ) );
	}

	// RF, reset anims so client's dont freak out
	client->ps.legsAnim = 0;
	client->ps.torsoAnim = 0;

	// DHM - Nerve :: Forcibly set both model and skin for multiplayer.
	if ( g_gametype.integer >= GT_WOLF ) {
		// To communicate it to cgame
		client->ps.stats[ STAT_PLAYER_CLASS ] = client->sess.playerType;

		if ( client->sess.sessionTeam == TEAM_BLUE ) {
			Q_strncpyz( model, MULTIPLAYER_ALLIEDMODEL, MAX_QPATH );
		} else {
			Q_strncpyz( model, MULTIPLAYER_AXISMODEL, MAX_QPATH );
		}

		Q_strcat( model, MAX_QPATH, "/" );

		SetWolfSkin( client, model );

		Q_strncpyz( head, "", MAX_QPATH );
		SetWolfSkin( client, head );
	}

	// strip the skin name
	Q_strncpyz( modelname, model, sizeof( modelname ) );
	if ( strstr( modelname, "/" ) ) {
		modelname[ strstr( modelname, "/" ) - modelname ] = 0;
	} else if ( strstr( modelname, "\\" ) ) {
		modelname[ strstr( modelname, "\\" ) - modelname ] = 0;
	}

	if ( !G_CheckForExistingModelInfo( client, modelname, &client->modelInfo ) ) {
		if ( !G_ParseAnimationFiles( modelname, client ) ) {
			G_Error( "Failed to load animation scripts for model %s\n", modelname );
		}
	}

	// team`
	// DHM - Nerve :: Already took care of models and skins above
	if ( g_gametype.integer < GT_WOLF ) {
		//----(SA) added this for head separation
		// set head
		if ( g_forceModel.integer ) {
			Q_strncpyz( head, DEFAULT_HEAD, sizeof( head ) );
		} else {
			Q_strncpyz( head, Info_ValueForKey( userinfo, "head" ), sizeof( head ) );
		}

		//----(SA) end

		switch ( client->sess.sessionTeam ) {
		case TEAM_RED:
			ForceClientSkin( client, model, "red" );
			break;
		case TEAM_BLUE:
			ForceClientSkin( client, model, "blue" );
			break;
		default: // TEAM_FREE, TEAM_SPECTATOR, TEAM_NUM_TEAMS not handled in switch
			break;
		}
		if ( g_gametype.integer >= GT_TEAM && client->sess.sessionTeam == TEAM_SPECTATOR ) {
			// don't ever use a default skin in teamplay, it would just waste memory
			ForceClientSkin( client, model, "red" );
		}
	}
	//dhm - end

	// colors
	c1 = Info_ValueForKey( userinfo, "color" );

	// send over a subset of the userinfo keys so other clients can
	// print scoreboards, display models, and play custom sounds

	//----(SA) modified these for head separation

	s = va( "n\\%s\\t\\%i\\model\\%s\\head\\%s\\c1\\%s\\hc\\%i\\w\\%i\\l\\%i",
		client->pers.netname, client->sess.sessionTeam, model, head, c1,
		client->pers.maxHealth, client->sess.wins, client->sess.losses );

	//----(SA) end

	trap_SetConfigstring( CS_PLAYERS + clientNum, s );

#ifdef WITH_LUA
	G_LuaHook_ClientUserinfoChanged( clientNum );
#endif

	// this is not the userinfo actually, it's the config string
	G_LogPrintf( "ClientUserinfoChanged: %i %s\n", clientNum, s );
	G_DPrintf( "ClientUserinfoChanged: %i :: %s\n", clientNum, s );
}

/*
===========
ClientConnect

Called when a player begins connecting to the server.
Called again for every map change or tournement restart.

The session information will be valid after exit.

Return NULL if the client should be allowed, otherwise return
a string with the reason for denial.

Otherwise, the client will be sent the current gamestate
and will eventually get to ClientBegin.

firstTime will be qtrue the very first time a client connects
to the server machine, but qfalse on map changes and tournement
restarts.
============
*/
char *ClientConnect( int clientNum, qboolean firstTime, qboolean isBot ) {
	char        *value;
	gclient_t   *client;
	char userinfo[MAX_INFO_STRING];
	gentity_t   *ent;
#ifdef WITH_LUA
	char reason[MAX_STRING_CHARS] = "";        // for lua
#endif

	ent = &g_entities[ clientNum ];

	trap_GetUserinfo( clientNum, userinfo, sizeof( userinfo ) );

	// IP filtering
	// https://zerowing.idsoftware.com/bugzilla/show_bug.cgi?id=500
	// recommanding PB based IP / GUID banning, the builtin system is pretty limited
	// check to see if they are on the banned IP list
	value = Info_ValueForKey( userinfo, "ip" );
	if ( G_FilterPacket( value ) ) {
		return "You are banned from this server.";
	}

	// Xian - check for max lives enforcement ban
	// cs: don't enforce for bots
	if ( !isBot && !(ent->r.svFlags & SVF_BOT) && g_enforcemaxlives.integer && ( g_maxlives.integer > 0 || g_axismaxlives.integer > 0 || g_alliedmaxlives.integer > 0 ) ) {
		value = Info_ValueForKey( userinfo, "cl_guid" );
		if ( G_FilterMaxLivesPacket( value ) ) {
			return "Max Lives Enforcement Temp Ban";
		}
	}
	// End Xian

	// we don't check password for bots and local client
	// NOTE: local client <-> "ip" "localhost"
	//   this means this client is not running in our current process
	if ( !isBot && !( ent->r.svFlags & SVF_BOT ) && ( strcmp( Info_ValueForKey( userinfo, "ip" ), "localhost" ) != 0 ) ) {
		// check for a password
		value = Info_ValueForKey( userinfo, "password" );
		if ( g_password.string[0] && Q_stricmp( g_password.string, "none" ) &&
			strcmp( g_password.string, value ) != 0 ) {
				return "Invalid password";
		}
	}

#ifdef WITH_LUA
	if ( G_LuaHook_ClientConnect( clientNum, firstTime, isBot, reason ) && !isBot && !( ent->r.svFlags & SVF_BOT ) ) {
		return va( "%s\n", reason );
	}
#endif

	// they can connect
	ent->client = level.clients + clientNum;
	client = ent->client;

	memset( client, 0, sizeof( *client ) );

	client->pers.connected = CON_CONNECTING;
	client->pers.connectTime = level.time;          // DHM - Nerve

	if ( firstTime ) {
		client->pers.initialSpawn = qtrue;              // DHM - Nerve
	}
	client->pers.complaints = 0;                    // DHM - Nerve

	// read or initialize the session data
	if ( firstTime || ( g_gametype.integer < GT_WOLF && level.newSession ) ) {
		G_InitSessionData( client, userinfo );
	}
	G_ReadSessionData( client );

	if ( isBot ) {
		ent->r.svFlags |= SVF_BOT;
		ent->inuse = qtrue;
	}

	// get and distribute relevent paramters
	G_LogPrintf( "ClientConnect: %i\n", clientNum );
	ClientUserinfoChanged( clientNum );

	Bot_Event_ClientConnected( clientNum, isBot );

	// don't do the "xxx connected" messages if they were caried over from previous level
	if ( firstTime ) {
		trap_SendServerCommand( -1, va( "print \"[lof]%s" S_COLOR_WHITE " [lon]connected\n\"", client->pers.netname ) );
	}

	// count current clients and rank for scoreboard
	CalculateRanks();

	return NULL;
}

/*================
setSeedPings
Sets random number for a bot's ping.
The seed will give it an initial 'ping' and then will fluctuate +/-  from it
================*/
void setSeedPings( gentity_t* ent ) {
	if ( ent->client ) {
		ent->client->pers.seedPing = ( ( rand() % 70 ) + 30 );  // 30 to 100 for pings
	}
}

/*
===========
ClientBegin

called when a client has finished connecting, and is ready
to be placed into the level.  This will happen every level load,
and on transition between teams, but doesn't happen on respawns
============
*/
void ClientBegin( int clientNum ) {
	gentity_t   *ent;
	gclient_t   *client;
	//gentity_t	*tent;
	int flags;
	int spawn_count;                // DHM - Nerve

	ent = g_entities + clientNum;
	client = level.clients + clientNum;

	if ( ent->r.linked ) {
		trap_UnlinkEntity( ent );
	}

	G_InitGentity( ent );
	ent->touch = 0;
	ent->pain = 0;
	ent->client = client;

	client->pers.connected = CON_CONNECTED;
	// ATVI Wolfenstein Misc #414
	// don't reset the enterTime during a map_restart, we only want this when user explicitely changes team (and upon entering map)
	if ( !trap_Cvar_VariableIntegerValue( "sv_serverRestarting" ) ) {
		client->pers.enterTime = level.time;
	}
	client->pers.teamState.state = TEAM_BEGIN;

	client->pers.lastkilled_client = -1;
	client->pers.lastammo_client = -1;
	client->pers.lasthealth_client = -1;
	client->pers.lastrevive_client = -1;
	client->pers.lastkiller_client = -1;

	// save eflags around this, because changing teams will
	// cause this to happen with a valid entity, and we
	// want to make sure the teleport bit is set right
	// so the viewpoint doesn't interpolate through the
	// world to the new position
	// DHM - Nerve :: Also save PERS_SPAWN_COUNT, so that CG_Respawn happens
	spawn_count = client->ps.persistant[PERS_SPAWN_COUNT];
	flags = client->ps.eFlags;
	memset( &client->ps, 0, sizeof( client->ps ) );
	client->ps.eFlags = flags;
	client->ps.persistant[PERS_SPAWN_COUNT] = spawn_count;

	// MrE: use capsule for collision
	//client->ps.eFlags |= EF_CAPSULE;
	//ent->r.svFlags |= SVF_CAPSULE;

	client->pers.complaintClient = -1;
	client->pers.complaintEndTime = -1;

	//clear stats
	client->pers.kills = 0;
	client->pers.teamKills = 0;
	client->pers.gibs = 0;
	client->pers.teamGibs = 0;
	client->pers.revives = 0;
	client->pers.medPacks = 0;
	client->pers.ammoPacks = 0;
	client->pers.acc_hits = 0;
	client->pers.acc_shots = 0;
	client->pers.headshots = 0;
	client->pers.damage_given = 0;
	client->pers.damage_received = 0;
	client->pers.team_damage = 0;
	client->pers.goomba = 0;
	client->pers.poison = 0;
	client->pers.knifeKill = 0;
	client->pers.killSpreeMax = 0;
	client->pers.deathSpreeMax = 0;

	//CS: make sure this isn't set
	client->sess.botSuicide = qfalse;
	client->sess.botSuicidePersist = qfalse;
	client->sess.botPush = ent->r.svFlags & SVF_BOT ? qtrue : qfalse;

	// locate ent at a spawn point
	ClientSpawn( ent, qfalse );

	// Xian -- Changed below for team independant maxlives

	if ( g_maxlives.integer > 0 ) {
		ent->client->ps.persistant[PERS_RESPAWNS_LEFT] = ( g_maxlives.integer - 1 );
	} else {
		ent->client->ps.persistant[PERS_RESPAWNS_LEFT] = -1;
	}

	if ( g_axismaxlives.integer > 0 || g_alliedmaxlives.integer > 0 ) {
		if ( client->sess.sessionTeam == TEAM_RED ) {
			ent->client->ps.persistant[PERS_RESPAWNS_LEFT] = ( g_axismaxlives.integer - 1 );
		} else if ( client->sess.sessionTeam == TEAM_BLUE ) {
			ent->client->ps.persistant[PERS_RESPAWNS_LEFT] = ( g_alliedmaxlives.integer - 1 );
		} else {
			ent->client->ps.persistant[PERS_RESPAWNS_LEFT] = -1;
		}
	}

	// DHM - Nerve :: Start players in limbo mode if they change teams during the match
	/*if ( g_gametype.integer >= GT_WOLF && client->sess.sessionTeam != TEAM_SPECTATOR
	&& (level.time - client->pers.connectTime) > 60000 ) {*/
	if ( g_gametype.integer >= GT_WOLF && client->sess.sessionTeam != TEAM_SPECTATOR
		&& ( level.time - level.startTime > FRAMETIME * GAME_INIT_FRAMES ) ) {
			if ( g_gamestate.integer == GS_PLAYING ) {
				ent->r.contents = CONTENTS_CORPSE;
				ent->client->ps.pm_type = PM_DEAD;
				ent->health = 0;
				ent->client->ps.stats[STAT_HEALTH] = 0;

				if ( g_maxlives.integer > 0 ) {
					ent->client->ps.persistant[PERS_RESPAWNS_LEFT]++;
				}

				limbo( ent, qfalse );
			}
	}

	if ( client->sess.sessionTeam != TEAM_SPECTATOR ) {
		trap_SendServerCommand( -1, va( "print \"[lof]%s" S_COLOR_WHITE " [lon]entered the game\n\"", client->pers.netname ) );
	}
	G_LogPrintf( "ClientBegin: %i\n", clientNum );

#ifdef WITH_LUA
	G_LuaHook_ClientBegin( clientNum );
#endif

	// Xian - Check for maxlives enforcement
	if ( g_enforcemaxlives.integer == 1 && ( g_maxlives.integer > 0 || g_axismaxlives.integer > 0 || g_alliedmaxlives.integer > 0 ) ) {
		char *value;
		char userinfo[MAX_INFO_STRING];
		trap_GetUserinfo( clientNum, userinfo, sizeof( userinfo ) );
		value = Info_ValueForKey( userinfo, "cl_guid" );
		G_LogPrintf( "EnforceMaxLives-GUID: %s\n", value );
		AddMaxLivesGUID( value );
	}
	// End Xian

	if ( g_fakePing.integer && ent->r.svFlags & SVF_BOT ) {
		setSeedPings( ent );
	}

	// count current clients and rank for scoreboard
	CalculateRanks();

#ifdef _DEBUG
	if ( client->sess.credits < 1000 ) {
		client->sess.credits = 1000;
	}
#endif
}

/*
===========
ClientSpawn

Called every time a client is placed fresh in the world:
after the first ClientBegin, and after each respawn
Initializes all non-persistant parts of playerState
============
*/
void ClientSpawn( gentity_t *ent, qboolean revived ) {
	int index;
	vec3_t spawn_origin, spawn_angles;
	gclient_t   *client;
	int i;
	clientPersistant_t saved;
	clientSession_t savedSess;
	int persistant[MAX_PERSISTANT];
	gentity_t   *spawnPoint;
	int flags;
	int savedPing;
	int savedTeam;
	qboolean savedVoted = qfalse;         // NERVE - SMF

	index = ent - g_entities;
	client = ent->client;

	// find a spawn point
	// do it before setting health back up, so farthest
	// ranging doesn't count this client

	if ( revived ) {
		spawnPoint = ent;
		VectorCopy( ent->s.origin, spawn_origin );
		spawn_origin[2] += 9;   // spawns seem to be sunk into ground?
		VectorCopy( ent->s.angles, spawn_angles );
	}
	else {
		client->sess.buyTime    = 0; // reset so they can buy again
		client->ps.stats[STAT_KEYS] = ~( 1 << KEY_7 ); // clear parachute

		if ( client->sess.sessionTeam == TEAM_SPECTATOR ) {
			spawnPoint = SelectSpectatorSpawnPoint(spawn_origin, spawn_angles );
		}
		else if ( g_gametype.integer >= GT_TEAM ) {
			spawnPoint = SelectCTFSpawnPoint(
				client->sess.sessionTeam,
				client->pers.teamState.state,
				spawn_origin, spawn_angles, client->sess.spawnObjectiveIndex );
		}
		else {
			do {
				// the first spawn should be at a good looking spot
				if ( !client->pers.initialSpawn && client->pers.localClient ) {
					client->pers.initialSpawn = qtrue;
					spawnPoint = SelectInitialSpawnPoint( spawn_origin, spawn_angles );
				} else {
					// don't spawn near existing origin if possible
					spawnPoint = SelectSpawnPoint(
						client->ps.origin,
						spawn_origin, spawn_angles );
				}
				break;
			} while ( 1 );
		}
	}

	client->pers.teamState.state = TEAM_ACTIVE;

	// toggle the teleport bit so the client knows to not lerp
	flags = ent->client->ps.eFlags & EF_TELEPORT_BIT;
	flags ^= EF_TELEPORT_BIT;

	// clear everything but the persistant data

	saved = client->pers;
	savedSess = client->sess;
	savedPing = client->ps.ping;
	savedTeam = client->ps.teamNum;

	// NERVE - SMF
	if ( client->ps.eFlags & EF_VOTED ) {
		savedVoted = qtrue;
	}

	for ( i = 0 ; i < MAX_PERSISTANT ; i++ ) {
		persistant[i] = client->ps.persistant[i];
	}

	memset( client, 0, sizeof( *client ) );

	client->pers = saved;
	client->sess = savedSess;
	client->ps.ping = savedPing;
	client->ps.teamNum = savedTeam;

	for ( i = 0 ; i < MAX_PERSISTANT ; i++ ) {
		client->ps.persistant[i] = persistant[i];
	}

	// increment the spawncount so the client will detect the respawn
	client->ps.persistant[PERS_SPAWN_COUNT]++;
	client->ps.persistant[PERS_TEAM] = client->sess.sessionTeam;

	client->airOutTime = level.time + 12000;

	// clear entity values
	client->ps.stats[STAT_MAX_HEALTH] = client->pers.maxHealth;
	client->ps.eFlags = flags;
	// MrE: use capsules for AI and player
	//client->ps.eFlags |= EF_CAPSULE;

	// TTimo
	if ( savedVoted ) {
		client->ps.eFlags |= EF_VOTED;
	}

	ent->s.groundEntityNum = ENTITYNUM_NONE;
	ent->client = &level.clients[index];
	ent->takedamage = qtrue;
	ent->inuse = qtrue;
	ent->classname = "player";
	ent->r.contents = CONTENTS_BODY;
	ent->clipmask = MASK_PLAYERSOLID;

	// DHM - Nerve :: Init to -1 on first spawn;
	if ( !revived ) {
		ent->props_frame_state = -1;
	}

	ent->die = player_die;
	ent->waterlevel = 0;
	ent->watertype = 0;
	ent->flags = 0;
	ent->poisoned = qfalse;

	VectorCopy( playerMins, ent->r.mins );
	VectorCopy( playerMaxs, ent->r.maxs );

	// Ridah, setup the bounding boxes and viewheights for prediction
	VectorCopy( ent->r.mins, client->ps.mins );
	VectorCopy( ent->r.maxs, client->ps.maxs );

	client->ps.crouchViewHeight = CROUCH_VIEWHEIGHT;
	client->ps.standViewHeight = DEFAULT_VIEWHEIGHT;
	client->ps.deadViewHeight = DEAD_VIEWHEIGHT;

	client->ps.crouchMaxZ = client->ps.maxs[2] - ( client->ps.standViewHeight - client->ps.crouchViewHeight );

	client->ps.runSpeedScale = 0.8;
	client->ps.sprintSpeedScale = 1.1;
	client->ps.crouchSpeedScale = 0.25;

	// Rafael
	client->ps.sprintTime = 20000;
	client->ps.sprintExertTime = 0;

	client->ps.friction = 1.0;
	// done.

	// TTimo
	// retrieve from the persistant storage (we use this in pmoveExt_t beause we need it in bg_*)
	client->pmext.bAutoReload = client->pers.bAutoReloadAux;
	// done

	client->ps.clientNum = index;

	trap_GetUsercmd( client - level.clients, &ent->client->pers.cmd );  // NERVE - SMF - moved this up here

	SetWolfUserVars( ent, NULL );           // NERVE - SMF

	// DHM - Nerve :: Add appropriate weapons
	if ( g_gametype.integer >= GT_WOLF ) {
		if ( !revived ) {
			qboolean update = qfalse;

			if ( client->sess.playerType != client->sess.latchPlayerType ) {
				update = qtrue;
			}

			client->sess.playerType = client->sess.latchPlayerType;
			client->sess.playerWeapon = client->sess.latchPlayerWeapon;
			client->sess.playerItem = client->sess.latchPlayerItem;
			client->sess.playerSkin = client->sess.latchPlayerSkin;

			if ( update ) {
				ClientUserinfoChanged( index );
			}
		}

		// TTimo keep it isolated from spectator to be safe still
		if ( client->sess.sessionTeam != TEAM_SPECTATOR ) {
			// Xian - Moved the invul. stuff out of SetWolfSpawnWeapons and put it here for clarity
			if ( g_fastres.integer == 1 && revived ) {
				client->ps.powerups[PW_INVULNERABLE] = level.time + g_fastResMsec.integer;
			} else {
				client->ps.powerups[PW_INVULNERABLE] = level.time + 3000;
			}
		}

		// End Xian

		SetWolfSpawnWeapons( client ); // JPW NERVE -- increases stats[STAT_MAX_HEALTH] based on # of medics in game
	}
	// dhm - end

	// JPW NERVE ***NOTE*** the following line is order-dependent and must *FOLLOW* SetWolfSpawnWeapons() in multiplayer
	// SetWolfSpawnWeapons() now adds medic team bonus and stores in ps.stats[STAT_MAX_HEALTH].
	ent->health = client->ps.stats[STAT_HEALTH] = client->ps.stats[STAT_MAX_HEALTH];

	G_SetOrigin( ent, spawn_origin );
	VectorCopy( spawn_origin, client->ps.origin );

	// the respawned flag will be cleared after the attack and jump keys come up
	client->ps.pm_flags |= PMF_RESPAWNED;

	SetClientViewAngle( ent, spawn_angles );

	if ( ent->client->sess.sessionTeam != TEAM_SPECTATOR ) {
		//G_KillBox( ent );
		trap_LinkEntity( ent );
	}

	client->respawnTime = level.time;
	client->inactivityTime = level.time + g_inactivity.integer * 1000;
	client->latched_buttons = 0;
	client->latched_wbuttons = 0;   //----(SA)	added

	if ( level.intermissiontime ) {
		MoveClientToIntermission( ent );
	} else {
		// fire the targets of the spawn point
		if ( !revived ) {
			G_UseTargets( spawnPoint, ent );
		}
	}

#ifdef WITH_LUA
	G_LuaHook_ClientSpawn( ent - g_entities, revived );
#endif

	// run a client frame to drop exactly to the floor,
	// initialize animations and other things
	client->ps.commandTime = level.time - 100;
	ent->client->pers.cmd.serverTime = level.time;
	ClientThink( ent - g_entities );

	// positively link the client, even if the command times are weird
	if ( ent->client->sess.sessionTeam != TEAM_SPECTATOR ) {
		BG_PlayerStateToEntityState( &client->ps, &ent->s, qtrue );
		VectorCopy( ent->client->ps.origin, ent->r.currentOrigin );
		trap_LinkEntity( ent );
	}

	// run the presend to set anything else
	ClientEndFrame( ent );

	// clear entity state values
	BG_PlayerStateToEntityState( &client->ps, &ent->s, qtrue );

	// https://zerowing.idsoftware.com/bugzilla/show_bug.cgi?id=569
	G_ResetMarkers( ent );

	if ( !revived && client->sess.sessionTeam != TEAM_SPECTATOR ) {
		// RF, call entity scripting event
		G_Script_ScriptEvent( ent, "playerstart", "" );
	}
}

/*
===========
ClientDisconnect

Called when a player drops from the server.
Will not be called between levels.

This should NOT be called directly by any game logic,
call trap_DropClient(), which will call this and do
server system housekeeping.
============
*/
void ClientDisconnect( int clientNum ) {
	gentity_t   *ent;
	gentity_t   *flag = NULL;
	gitem_t     *item = NULL;
	vec3_t launchvel;
	int i;

	ent = g_entities + clientNum;
	if ( !ent->client ) {
		return;
	}

#ifdef WITH_LUA
	G_LuaHook_ClientDisconnect( clientNum );
#endif

	//////////////////////////////////////////////////////////////////////////
	Bot_Event_ClientDisConnected( clientNum );
	//////////////////////////////////////////////////////////////////////////

	// stop any following clients
	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[i].sess.sessionTeam == TEAM_SPECTATOR
			&& level.clients[i].sess.spectatorState == SPECTATOR_FOLLOW
			&& level.clients[i].sess.spectatorClient == clientNum ) {
				StopFollowing( &g_entities[i] );
		}
		if ( g_gametype.integer >= GT_WOLF
			&& level.clients[i].ps.pm_flags & PMF_LIMBO
			&& level.clients[i].sess.spectatorClient == clientNum ) {
				Cmd_FollowCycle_f( &g_entities[i], 1 );
		}
	}

	// NERVE - SMF - remove complaint client
	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[i].pers.complaintClient == clientNum ) {
			level.clients[i].pers.complaintClient = -1;
			level.clients[i].pers.complaintEndTime = 0;

			trap_SendServerCommand( i, "complaint -2" );
			break;
		}
	}

	// send effect if they were completely connected
	if ( ent->client->pers.connected == CON_CONNECTED
		&& ent->client->sess.sessionTeam != TEAM_SPECTATOR ) {
			// New code for tossing flags
			if ( g_gametype.integer >= GT_WOLF ) {
				if ( ent->client->ps.powerups[PW_REDFLAG] ) {
					item = BG_FindItemByIndex( IL_TEAM_CTF_REDFLAG );

					ent->client->ps.powerups[PW_REDFLAG] = 0;
				}
				if ( ent->client->ps.powerups[PW_BLUEFLAG] ) {
					item = BG_FindItemByIndex( IL_TEAM_CTF_BLUEFLAG );

					ent->client->ps.powerups[PW_BLUEFLAG] = 0;
				}

				if ( item ) {
					launchvel[0] = crandom() * 20;
					launchvel[1] = crandom() * 20;
					launchvel[2] = 10 + random() * 10;

					flag = LaunchItem( item,ent->r.currentOrigin,launchvel,ent->s.number );
					flag->s.modelindex2 = ent->s.otherEntityNum2; // JPW NERVE FIXME set player->otherentitynum2 with old modelindex2 from flag and restore here
					flag->message = ent->message;   // DHM - Nerve :: also restore item name
					// Clear out player's temp copies
					ent->s.otherEntityNum2 = 0;
					ent->message = NULL;
				}
			}
	}

	G_LogPrintf( "ClientDisconnect: %i\n", clientNum );

	trap_UnlinkEntity( ent );
	ent->s.modelindex = 0;
	ent->inuse = qfalse;
	ent->classname = "disconnected";
	ent->client->pers.connected = CON_DISCONNECTED;
	ent->client->ps.persistant[PERS_TEAM] = TEAM_FREE;
	ent->client->ps.persistant[PERS_HWEAPON_USE] = 0;
	ent->client->sess.sessionTeam = TEAM_FREE;
	// JPW NERVE -- mg42 additions
	ent->active = qfalse;
	// jpw
	ent->r.svFlags &= ~SVF_BOT;

	trap_SetConfigstring( CS_PLAYERS + clientNum, "" );

	CalculateRanks();
}

/*
==================
G_RetrieveMoveSpeedsFromClient
==================
*/
void G_RetrieveMoveSpeedsFromClient( int entnum, char *moveSpeedText ) {
	char *text_p, *token;
	animation_t *anim;
	animModelInfo_t *modelInfo;

	text_p = moveSpeedText;

	// get the model name
	token = COM_Parse( &text_p );
	if ( !token || !token[0] ) {
		G_Error( "G_RetrieveMoveSpeedsFromClient: internal error" );
		return; // for compiler warning
	}

	modelInfo = BG_ModelInfoForModelname( token );

	if ( !modelInfo ) {
		// ignore it
		return;
	}

	while ( 1 ) {
		token = COM_Parse( &text_p );
		if ( !token || !token[0] ) {
			break;
		}

		// this is a name
		anim = BG_AnimationForString( token, modelInfo );
		if ( anim->moveSpeed == 0 ) {
			G_Error( "G_RetrieveMoveSpeedsFromClient: trying to set movespeed for non-moving animation" );
		}

		// get the movespeed
		token = COM_Parse( &text_p );
		if ( !token || !token[0] ) {
			G_Error( "G_RetrieveMoveSpeedsFromClient: missing movespeed" );
		}
		anim->moveSpeed = atoi( token );
	}
}