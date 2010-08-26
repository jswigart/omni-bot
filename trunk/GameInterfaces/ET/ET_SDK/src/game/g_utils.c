/*
 * name:	g_utils.c
 *
 * desc:	misc utility functions for game module
 *
*/

#include "g_local.h"

typedef struct {
  char oldShader[MAX_QPATH];
  char newShader[MAX_QPATH];
  float timeOffset;
} shaderRemap_t;

#define MAX_SHADER_REMAPS 128

int remapCount = 0;
shaderRemap_t remappedShaders[MAX_SHADER_REMAPS];

void AddRemap(const char *oldShader, const char *newShader, float timeOffset) {
	int i;

	for (i = 0; i < remapCount; i++) {
		if (Q_stricmp(oldShader, remappedShaders[i].oldShader) == 0) {
			// found it, just update this one
			strcpy(remappedShaders[i].newShader,newShader);
			remappedShaders[i].timeOffset = timeOffset;
			return;
		}
	}
	if (remapCount < MAX_SHADER_REMAPS) {
		strcpy(remappedShaders[remapCount].newShader,newShader);
		strcpy(remappedShaders[remapCount].oldShader,oldShader);
		remappedShaders[remapCount].timeOffset = timeOffset;
		remapCount++;
	}
}

const char *BuildShaderStateConfig() {
	static char	buff[MAX_STRING_CHARS*4];
	char out[(MAX_QPATH * 2) + 5];
	int i;
  
	memset(buff, 0, MAX_STRING_CHARS);
	for (i = 0; i < remapCount; i++) {
		int i1, i2;

		i1 = G_ShaderIndex(remappedShaders[i].oldShader);
		i2 = G_ShaderIndex(remappedShaders[i].newShader);

		Com_sprintf(out, (MAX_QPATH * 2) + 5, "%i=%i:%5.2f@", i1, i2, remappedShaders[i].timeOffset);
		Q_strcat( buff, sizeof( buff ), out);
	}
	return buff;
}

/*
=========================================================================

model / sound configstring indexes

=========================================================================
*/

/*
================
G_FindConfigstringIndex

================
*/
int G_FindConfigstringIndex( const char *name, int start, int max, qboolean create ) {
	int		i;
	char	s[MAX_STRING_CHARS];

	if ( !name || !name[0] ) {
		return 0;
	}

	for ( i=1 ; i<max ; i++ ) {
		trap_GetConfigstring( start + i, s, sizeof( s ) );
		if ( !s[0] ) {
			break;
		}
		if ( !strcmp( s, name ) ) {
			return i;
		}
	}

	if ( !create ) {
		return 0;
	}

	if ( i == max ) {
		G_Error( "G_FindConfigstringIndex: overflow" );
	}

	trap_SetConfigstring( start + i, name );

	return i;
}


int G_ModelIndex( char *name ) {
	return G_FindConfigstringIndex (name, CS_MODELS, MAX_MODELS, qtrue);
}

int G_SoundIndex( const char *name ) {
	return G_FindConfigstringIndex (name, CS_SOUNDS, MAX_SOUNDS, qtrue);
}

int G_SkinIndex( const char *name ) {
	return G_FindConfigstringIndex (name, CS_SKINS, MAX_CS_SKINS, qtrue);
}

int G_ShaderIndex( char *name ) {
	return G_FindConfigstringIndex (name, CS_SHADERS, MAX_CS_SHADERS, qtrue);
}

int G_CharacterIndex( const char *name ) {
	return G_FindConfigstringIndex (name, CS_CHARACTERS, MAX_CHARACTERS, qtrue);
}

int G_StringIndex( const char* string ) {
	return G_FindConfigstringIndex( string, CS_STRINGS, MAX_CSSTRINGS, qtrue );
}

//=====================================================================


/*
================
G_TeamCommand

Broadcasts a command to only a specific team
================
*/
void G_TeamCommand( team_t team, char *cmd ) {
	int		i;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[i].pers.connected == CON_CONNECTED ) {
			if ( level.clients[i].sess.sessionTeam == team ) {
				trap_SendServerCommand( i, va("%s", cmd ));
			}
		}
	}
}


/*
=============
G_Find

Searches all active entities for the next one that holds
the matching string at fieldofs (use the FOFS() macro) in the structure.

Searches beginning at the entity after from, or the beginning if NULL
NULL will be returned if the end of the list is reached.

=============
*/
gentity_t *G_Find (gentity_t *from, int fieldofs, const char *match)
{
	char	*s;
	gentity_t *max = &g_entities[level.num_entities];

	if (!from)
		from = g_entities;
	else
		from++;


	for ( ; from < max ; from++) {
		if (!from->inuse)
			continue;
		s = *(char **) ((byte *)from + fieldofs);
		if (!s)
			continue;
		if (!Q_stricmp (s, match))
			return from;
	}

	return NULL;
}

/*
=============
G_FindByTargetname
=============
*/
gentity_t* G_FindByTargetname(gentity_t *from, const char* match) {
	gentity_t* max = &g_entities[level.num_entities];
	int hash = BG_StringHashValue( match );

	if (!from) {
		from = g_entities;
	} else {
		from++;
	}

	for ( ; from < max ; from++) {
		if( !from->inuse ) {
			continue;
		}

		if( from->targetnamehash == hash && !Q_stricmp( from->targetname, match ) ) {
			return from;
		}
	}

	return NULL;
}

// digibob: this version should be used for loops, saves the constant hash building
gentity_t* G_FindByTargetnameFast(gentity_t *from, const char* match, int hash) {
	gentity_t* max = &g_entities[level.num_entities];

	if (!from) {
		from = g_entities;
	} else {
		from++;
	}

	for ( ; from < max ; from++) {
		if( !from->inuse ) {
			continue;
		}

		if( from->targetnamehash == hash && !Q_stricmp( from->targetname, match ) ) {
			return from;
		}
	}

	return NULL;
}
/*
=============
G_PickTarget

Selects a random entity from among the targets
=============
*/
#define MAXCHOICES	32

gentity_t *G_PickTarget (char *targetname)
{
	gentity_t	*ent = NULL;
	int		num_choices = 0;
	gentity_t	*choice[MAXCHOICES];

	if (!targetname)
	{
		//G_Printf("G_PickTarget called with NULL targetname\n");
		return NULL;
	}

	while(1)
	{
		ent = G_FindByTargetname (ent, targetname);
		if (!ent)
			break;
		choice[num_choices++] = ent;
		if (num_choices == MAXCHOICES)
			break;
	}

	if (!num_choices)
	{
		G_Printf("G_PickTarget: target %s not found\n", targetname);
		return NULL;
	}

	return choice[rand() % num_choices];
}

qboolean G_AllowTeamsAllowed( gentity_t *ent, gentity_t *activator ) {
	if( ent->allowteams && activator && activator->client ) {
		if( activator->client->sess.sessionTeam != TEAM_SPECTATOR ) {
			int checkTeam = activator->client->sess.sessionTeam;

			if( !(ent->allowteams & checkTeam) ) {
				if( (ent->allowteams & ALLOW_DISGUISED_CVOPS) && activator->client->ps.powerups[PW_OPS_DISGUISED] ) {
					if( checkTeam == TEAM_AXIS )
						checkTeam = TEAM_ALLIES;
					else if( checkTeam == TEAM_ALLIES )
						checkTeam = TEAM_AXIS;
				}

				if( !(ent->allowteams & checkTeam) )
					return qfalse;
			}
		}
	}

	return qtrue;
}

/*
=============
G_UseEntity

Added to allow more checking on what uses what
=============
*/
void G_UseEntity( gentity_t *ent, gentity_t *other, gentity_t *activator ) {

	// check for allowteams
	if( !G_AllowTeamsAllowed( ent, activator ) ) {
		return;
	}

	// Woop we got through, let's use the entity
	ent->use( ent, other, activator );
}

/*
==============================
G_UseTargets

"activator" should be set to the entity that initiated the firing.

Search for (string)targetname in all entities that
match (string)self.target and call their .use function

==============================
*/
void G_UseTargets( gentity_t *ent, gentity_t *activator ) {
	gentity_t		*t;
	int				hash;
	
	if ( !ent ) {
		return;
	}

	if ( !ent->target ) {
		return;
	}

	t = NULL;
	hash = BG_StringHashValue( ent->target );
	while ( (t = G_FindByTargetnameFast( t, ent->target, hash )) != NULL ) {
		if ( t == ent ) {
			G_Printf ("WARNING: Entity used itself.\n");
		} else {
			if ( t->use ) {
				//G_Printf ("ent->classname %s ent->targetname %s t->targetname %s t->s.number %d\n", ent->classname, ent->targetname, t->targetname, t->s.number);

				t->flags |= (ent->flags & FL_KICKACTIVATE);	// (SA) If 'ent' was kicked to activate, pass this along to it's targets.
															//		It may become handy to put a "KICKABLE" flag in ents so that it knows whether to pass this along or not
															//		Right now, the only situation where it would be weird would be an invisible_user that is a 'button' near
															//		a rotating door that it triggers.  Kick the switch and the door next to it flies open.

				t->flags |= (ent->flags & FL_SOFTACTIVATE);	// (SA) likewise for soft activation

				if (	activator &&
						(	(Q_stricmp (t->classname, "func_door") == 0) ||
							(Q_stricmp (t->classname, "func_door_rotating") == 0)
						)
					) {
					// check door usage rules before allowing any entity to trigger a door open
					G_TryDoor(t, ent, activator);		// (door,other,activator)
				} else {
					G_UseEntity( t, ent, activator );
				}
			}
		}
		if ( !ent->inuse ) {
			G_Printf("entity was removed while using targets\n");
			return;
		}
	}
}


/*
=============
TempVector

This is just a convenience function
for making temporary vectors for function calls
=============
*/
/*
float	*tv( float x, float y, float z ) {
	static	int		index;
	static	vec3_t	vecs[8];
	float	*v;

	// use an array so that multiple tempvectors won't collide
	// for a while
	v = vecs[index];
	index = (index + 1)&7;

	v[0] = x;
	v[1] = y;
	v[2] = z;

	return v;
}
*/

/*
=============
VectorToString

This is just a convenience function
for printing vectors
=============
*/
char	*vtos( const vec3_t v ) {
	static	int		index;
	static	char	str[8][32];
	char	*s;

	// use an array so that multiple vtos won't collide
	s = str[index];
	index = (index + 1)&7;

	Com_sprintf (s, 32, "(%i %i %i)", (int)v[0], (int)v[1], (int)v[2]);

	return s;
}
char	*vtosf( const vec3_t v ) {
	static	int		index;
	static	char	str[8][64];
	char	*s;

	// use an array so that multiple vtos won't collide
	s = str[index];
	index = (index + 1)&7;

	Com_sprintf (s, 64, "(%f %f %f)", v[0], v[1], v[2]);

	return s;
}


/*
===============
G_SetMovedir

The editor only specifies a single value for angles (yaw),
but we have special constants to generate an up or down direction.
Angles will be cleared, because it is being used to represent a direction
instead of an orientation.
===============
*/
void G_SetMovedir( vec3_t angles, vec3_t movedir ) {
	static vec3_t VEC_UP		= {0, -1, 0};
	static vec3_t MOVEDIR_UP	= {0, 0, 1};
	static vec3_t VEC_DOWN		= {0, -2, 0};
	static vec3_t MOVEDIR_DOWN	= {0, 0, -1};

	if ( VectorCompare (angles, VEC_UP) ) {
		VectorCopy (MOVEDIR_UP, movedir);
	} else if ( VectorCompare (angles, VEC_DOWN) ) {
		VectorCopy (MOVEDIR_DOWN, movedir);
	} else {
		AngleVectors (angles, movedir, NULL, NULL);
	}
	VectorClear( angles );
}



void G_InitGentity( gentity_t *e ) {
	e->inuse = qtrue;
	e->classname = "noclass";
	e->s.number = e - g_entities;
	e->r.ownerNum = ENTITYNUM_NONE;
	e->aiInactive = 0xffffffff;
	e->nextthink = 0;
	memset(e->goalPriority, 0, sizeof(e->goalPriority));
	e->free = NULL;

	// RF, init scripting
	e->scriptStatus.scriptEventIndex = -1;
	// inc the spawncount
	e->spawnCount++;
	// mark the time
	e->spawnTime = level.time;
}

/*
=================
G_Spawn

Either finds a free entity, or allocates a new one.

  The slots from 0 to MAX_CLIENTS-1 are always reserved for clients, and will
never be used by anything else.

Try to avoid reusing an entity that was recently freed, because it
can cause the client to think the entity morphed into something else
instead of being removed and recreated, which can cause interpolated
angles and bad trails.
=================
*/
gentity_t *G_Spawn( void ) {
	int			i, force;
	gentity_t	*e;

	e = NULL;	// shut up warning
	i = 0;		// shut up warning
	for ( force = 0 ; force < 2 ; force++ ) {
		// if we go through all entities and can't find one to free,
		// override the normal minimum times before use
		e = &g_entities[MAX_CLIENTS];
		for ( i = MAX_CLIENTS ; i<level.num_entities ; i++, e++) {
			if ( e->inuse ) {
				continue;
			}

			// the first couple seconds of server time can involve a lot of
			// freeing and allocating, so relax the replacement policy
			if ( !force && e->freetime > level.startTime + 2000 && level.time - e->freetime < 1000 ) {
				continue;
			}

			// reuse this slot
			G_InitGentity( e );
			return e;
		}
		if ( i != ENTITYNUM_MAX_NORMAL ) {
			break;
		}
	}
	if ( i == ENTITYNUM_MAX_NORMAL ) {
		for (i = 0; i < MAX_GENTITIES; i++) {
			G_Printf("%4i: %s\n", i, g_entities[i].classname);
		}
		G_Error( "G_Spawn: no free entities" );
	}
	
	// open up a new slot
	level.num_entities++;

	// let the server system know that there are more entities
	trap_LocateGameData( level.gentities, level.num_entities, sizeof( gentity_t ), 
		&level.clients[0].ps, sizeof( level.clients[0] ) );

	G_InitGentity( e );
	return e;
}

/*
=================
G_EntitiesFree
=================
*/
qboolean G_EntitiesFree( void ) {
	int			i;
	gentity_t	*e;

	e = &g_entities[MAX_CLIENTS];
	for ( i = MAX_CLIENTS; i < level.num_entities; i++, e++) {
		if ( e->inuse ) {
			continue;
		}
		// slot available
		return qtrue;
	}
	return qfalse;
}

/*
=================
G_FreeEntity

Marks the entity as free
=================
*/
void G_FreeEntity( gentity_t *ed ) {
	int spawnCount;

	if(ed->free) {
		ed->free( ed );
	}

	trap_UnlinkEntity (ed);		// unlink from world

	if ( ed->neverFree ) {
		return;
	}

	spawnCount = ed->spawnCount;

	memset (ed, 0, sizeof(*ed));
	ed->classname = "freed";
	ed->freetime = level.time;
	ed->inuse = qfalse;
	ed->spawnCount = spawnCount;
}

/*
=================
G_TempEntity

Spawns an event entity that will be auto-removed
The origin will be snapped to save net bandwidth, so care
must be taken if the origin is right on a surface (snap towards start vector first)
=================
*/
gentity_t *G_TempEntity( vec3_t origin, int event ) {
	gentity_t		*e;
	vec3_t		snapped;

	e = G_Spawn();
	e->s.eType = ET_EVENTS + event;

	e->classname = "tempEntity";
	e->eventTime = level.time;
	e->r.eventTime = level.time;
	e->freeAfterEvent = qtrue;

	VectorCopy( origin, snapped );
	SnapVector( snapped );		// save network bandwidth
	G_SetOrigin( e, snapped );

	// find cluster for PVS
	trap_LinkEntity( e );

	return e;
}

gentity_t* G_PopupMessage( popupMessageType_t type ) {
	gentity_t* e;

	e = G_Spawn();
	e->s.eType = ET_EVENTS + EV_POPUPMESSAGE;
	e->classname = "messageent";
	e->eventTime = level.time;
	e->r.eventTime = level.time;
	e->freeAfterEvent = qtrue;
	e->r.svFlags = SVF_BROADCAST;
	e->s.effect1Time = type;

	// find cluster for PVS
	trap_LinkEntity( e );

	return e;
}




/*
==============================================================================

Kill box

==============================================================================
*/

/*
=================
G_KillBox

Kills all entities that would touch the proposed new positioning
of ent.  Ent should be unlinked before calling this!
=================
*/
void G_KillBox (gentity_t *ent) {
	int			i, num;
	int			touch[MAX_GENTITIES];
	gentity_t	*hit;
	vec3_t		mins, maxs;

	VectorAdd( ent->client->ps.origin, ent->r.mins, mins );
	VectorAdd( ent->client->ps.origin, ent->r.maxs, maxs );
	num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

	for (i=0 ; i<num ; i++) {
		hit = &g_entities[touch[i]];
		if ( !hit->client ) {
			continue;
		}
		if ( !hit->r.linked ) {	// RF, inactive AI shouldn't be gibbed
			continue;
		}

		// nail it
		G_Damage ( hit, ent, ent, NULL, NULL,
			100000, DAMAGE_NO_PROTECTION, MOD_TELEFRAG);
	}

}

//==============================================================================

/*
===============
G_AddPredictableEvent

Use for non-pmove events that would also be predicted on the
client side: jumppads and item pickups
Adds an event+parm and twiddles the event counter
===============
*/
void G_AddPredictableEvent( gentity_t *ent, int event, int eventParm ) {
	if ( !ent->client ) {
		return;
	}
	BG_AddPredictableEventToPlayerstate( event, eventParm, &ent->client->ps );
}


/*
===============
G_AddEvent

Adds an event+parm and twiddles the event counter
===============
*/
void G_AddEvent( gentity_t *ent, int event, int eventParm ) {
//	int		bits;

	if ( !event ) {
		G_Printf( "G_AddEvent: zero event added for entity %i\n", ent->s.number );
		return;
	}

	// Ridah, use the sequential event list
	if ( ent->client ) {
		// NERVE - SMF - commented in - externalEvents not being handled properly in Wolf right now
		ent->client->ps.events[ent->client->ps.eventSequence & (MAX_EVENTS-1)] = event;
		ent->client->ps.eventParms[ent->client->ps.eventSequence & (MAX_EVENTS-1)] = eventParm;
		ent->client->ps.eventSequence++;
		// -NERVE - SMF

		// NERVE - SMF - commented out
//		bits = ent->client->ps.externalEvent & EV_EVENT_BITS;
//		bits = ( bits + EV_EVENT_BIT1 ) & EV_EVENT_BITS;
//		ent->client->ps.externalEvent = event | bits;
//		ent->client->ps.externalEventParm = eventParm;
//		ent->client->ps.externalEventTime = level.time;
		// -NERVE - SMF
	} else {
		// NERVE - SMF - commented in - externalEvents not being handled properly in Wolf right now
		ent->s.events[ent->s.eventSequence & (MAX_EVENTS-1)] = event;
		ent->s.eventParms[ent->s.eventSequence & (MAX_EVENTS-1)] = eventParm;
		ent->s.eventSequence++;
		// -NERVE - SMF

		// NERVE - SMF - commented out
//		bits = ent->s.event & EV_EVENT_BITS;
//		bits = ( bits + EV_EVENT_BIT1 ) & EV_EVENT_BITS;
//		ent->s.event = event | bits;
//		ent->s.eventParm = eventParm;
		// -NERVE - SMF
	}
	ent->eventTime = level.time;
	ent->r.eventTime = level.time;
}


/*
=============
G_Sound

  Ridah, removed channel parm, since it wasn't used, and could cause confusion
=============
*/
void G_Sound( gentity_t *ent, int soundIndex ) {
	gentity_t	*te;

	te = G_TempEntity( ent->r.currentOrigin, EV_GENERAL_SOUND );
	te->s.eventParm = soundIndex;
}

/*
=============
G_AnimScriptSound
=============
*/
void G_AnimScriptSound( int soundIndex, vec3_t org, int client ) {
	gentity_t *e;
	e = &g_entities[client];
	G_AddEvent( e, EV_GENERAL_SOUND, soundIndex );
}

//==============================================================================


/*
================
G_SetOrigin

Sets the pos trajectory for a fixed position
================
*/
void G_SetOrigin( gentity_t *ent, vec3_t origin ) {
	VectorCopy( origin, ent->s.pos.trBase );
	ent->s.pos.trType = TR_STATIONARY;
	ent->s.pos.trTime = 0;
	ent->s.pos.trDuration = 0;
	VectorClear( ent->s.pos.trDelta );

	VectorCopy( origin, ent->r.currentOrigin );

	if( ent->client ) {
		VectorCopy( origin, ent->client->ps.origin );
	}
}


/*
==============
G_SetAngle
==============
*/
void G_SetAngle( gentity_t *ent, vec3_t angle ) {

	VectorCopy (angle, ent->s.apos.trBase);
	ent->s.apos.trType = TR_STATIONARY;
	ent->s.apos.trTime = 0;
	ent->s.apos.trDuration = 0;
	VectorClear( ent->s.apos.trDelta );

	VectorCopy (angle, ent->r.currentAngles); 

//	VectorCopy (ent->s.angles, ent->s.apos.trDelta );

}

/*
====================
infront
====================
*/

qboolean infront (gentity_t *self, gentity_t *other)
{
	vec3_t	vec;
	float	dot;
	vec3_t	forward;
	
	AngleVectors (self->s.angles, forward, NULL, NULL);
	VectorSubtract (other->r.currentOrigin, self->r.currentOrigin, vec);
	VectorNormalize (vec);
	dot = DotProduct (vec, forward);
	// G_Printf( "other %5.2f\n",	dot);
	if (dot > 0.0)
		return qtrue;
	return qfalse;
}

//RF, tag connections
/*
==================
G_ProcessTagConnect
==================
*/
void G_ProcessTagConnect( gentity_t *ent, qboolean clearAngles ) {
	if (!ent->tagName) {
		G_Error("G_ProcessTagConnect: NULL ent->tagName\n");
	}
	if (!ent->tagParent) {
		G_Error("G_ProcessTagConnect: NULL ent->tagParent\n");
	}
	G_FindConfigstringIndex( va("%i %i %s", ent->s.number, ent->tagParent->s.number, ent->tagName), CS_TAGCONNECTS, MAX_TAGCONNECTS, qtrue );
	ent->s.eFlags |= EF_TAGCONNECT;

	if(ent->client) {
		ent->client->ps.eFlags |= EF_TAGCONNECT;
		ent->client->ps.eFlags &= ~EF_PRONE_MOVING;
		ent->client->ps.eFlags &= ~EF_PRONE;
		ent->s.eFlags &= ~EF_PRONE_MOVING;
		ent->s.eFlags &= ~EF_PRONE;

	}

	if (clearAngles) {
		// clear out the angles so it always starts out facing the tag direction
		VectorClear( ent->s.angles );
		VectorCopy( ent->s.angles, ent->s.apos.trBase );
		ent->s.apos.trTime = level.time;
		ent->s.apos.trDuration = 0;
		ent->s.apos.trType = TR_STATIONARY;
		VectorClear( ent->s.apos.trDelta );
		VectorClear( ent->r.currentAngles );
	}
}

/*
================
DebugLine

  debug polygons only work when running a local game
  with r_debugSurface set to 2
================
*/
int DebugLine(vec3_t start, vec3_t end, int color) {
	vec3_t points[4], dir, cross, up = {0, 0, 1};
	float dot;

	VectorCopy(start, points[0]);
	VectorCopy(start, points[1]);
	//points[1][2] -= 2;
	VectorCopy(end, points[2]);
	//points[2][2] -= 2;
	VectorCopy(end, points[3]);


	VectorSubtract(end, start, dir);
	VectorNormalize(dir);
	dot = DotProduct(dir, up);
	if (dot > 0.99 || dot < -0.99) {
		VectorSet(cross, 1, 0, 0);
	} else {
		CrossProduct(dir, up, cross);
	}

	VectorNormalize(cross);

	VectorMA(points[0], 2, cross, points[0]);
	VectorMA(points[1], -2, cross, points[1]);
	VectorMA(points[2], -2, cross, points[2]);
	VectorMA(points[3], 2, cross, points[3]);

	return trap_DebugPolygonCreate(color, 4, points);
}

/*
================
G_SetEntState

  sets the entstate of an entity.
================
*/
void G_SetEntState( gentity_t *ent, entState_t state ) {
	if( ent->entstate == state ) {
		G_DPrintf( "entity %i already in desired state [%i]\n", ent->s.number, state );
		return;
	}

	switch( state ) {
	case STATE_DEFAULT:				if( ent->entstate == STATE_UNDERCONSTRUCTION ) {
										ent->clipmask = ent->realClipmask;
										ent->r.contents = ent->realContents;
										if( !ent->realNonSolidBModel )
											ent->s.eFlags &= ~EF_NONSOLID_BMODEL;
									}

									ent->entstate = STATE_DEFAULT;
									ent->s.powerups = STATE_DEFAULT;

									if( ent->s.eType == ET_WOLF_OBJECTIVE) {
										char cs[MAX_STRING_CHARS];
										trap_GetConfigstring( ent->count, cs, sizeof(cs) );
										ent->count2 &= ~256;
										Info_SetValueForKey( cs, "t", va( "%i", ent->count2 ) );
										trap_SetConfigstring( ent->count, cs );
									}

									if( ent->s.eType != ET_COMMANDMAP_MARKER )
										trap_LinkEntity( ent );

									// deal with any entities in the solid
									{
										int listedEntities, e;
										int entityList[MAX_GENTITIES];
										gentity_t *check, *block;

										listedEntities = trap_EntitiesInBox( ent->r.absmin, ent->r.absmax, entityList, MAX_GENTITIES );

										for( e = 0; e < listedEntities; e++ ) {
											check = &g_entities[entityList[e]];

											// ignore everything but items, players and missiles (grenades too)
											if( check->s.eType != ET_MISSILE && check->s.eType != ET_ITEM && check->s.eType != ET_PLAYER && !check->physicsObject ) {
												continue;
											}

											if( (block = G_TestEntityPosition(check)) == NULL )
												continue;

											if( block != ent )
												// the entity is blocked by another entity - that block this should take care of this itself
												continue;

											if( check->client || check->s.eType == ET_CORPSE ) {
												// gibs anything player like
												G_Damage( check, ent, ent, NULL, NULL, 9999, DAMAGE_NO_PROTECTION, MOD_CRUSH_CONSTRUCTIONDEATH_NOATTACKER );
											} else if( check->s.eType == ET_ITEM && check->item->giType == IT_TEAM ) {
												// see if it's a critical entity, one that we can't just simply kill (basically flags)
												Team_DroppedFlagThink( check );
											} else {
												// remove the landmine from both teamlists
												if ( check->s.eType == ET_MISSILE && check->methodOfDeath == MOD_LANDMINE ) {
													mapEntityData_t	*mEnt;

													if((mEnt = G_FindMapEntityData(&mapEntityData[0], check-g_entities)) != NULL) {
														G_FreeMapEntityData( &mapEntityData[0], mEnt );
													}

													if((mEnt = G_FindMapEntityData(&mapEntityData[1], check-g_entities)) != NULL) {
														G_FreeMapEntityData( &mapEntityData[1], mEnt );
													}
												}

												// just get rid of it
												G_TempEntity( check->s.origin, EV_ITEM_POP );
												G_FreeEntity( check );
											}
										}
									}

									// if this is an mg42, then we should try and calculate mg42 spots again
									BotCalculateMg42Spots();

									break;
	case STATE_UNDERCONSTRUCTION:	ent->entstate = STATE_UNDERCONSTRUCTION;
									ent->s.powerups = STATE_UNDERCONSTRUCTION;
									ent->realClipmask = ent->clipmask;
									if( ent->s.eType != ET_CONSTRUCTIBLE )	// don't make nonsolid as we want to make them partially solid for staged construction
									ent->clipmask = 0;
									ent->realContents = ent->r.contents;
									if( ent->s.eType != ET_CONSTRUCTIBLE )
									ent->r.contents = 0;
									if( ent->s.eFlags & EF_NONSOLID_BMODEL )
										ent->realNonSolidBModel = qtrue;
									else
										if( ent->s.eType != ET_CONSTRUCTIBLE )
										ent->s.eFlags |= EF_NONSOLID_BMODEL;

									if( !Q_stricmp( ent->classname, "misc_mg42" ) ) {
										// stop using the mg42
										mg42_stopusing( ent );
									}

									if( ent->s.eType == ET_COMMANDMAP_MARKER ) {
										mapEntityData_t	*mEnt;

										if((mEnt = G_FindMapEntityData(&mapEntityData[0], ent-g_entities)) != NULL) {
											G_FreeMapEntityData( &mapEntityData[0], mEnt );
										}
										if((mEnt = G_FindMapEntityData(&mapEntityData[1], ent-g_entities)) != NULL ) {
											G_FreeMapEntityData( &mapEntityData[1], mEnt );
										}
									}

									trap_LinkEntity( ent );
									break;
	case STATE_INVISIBLE:			if( ent->entstate == STATE_UNDERCONSTRUCTION ) {
										ent->clipmask = ent->realClipmask;
										ent->r.contents = ent->realContents;
										if( !ent->realNonSolidBModel )
											ent->s.eFlags &= ~EF_NONSOLID_BMODEL;
									}

									ent->entstate = STATE_INVISIBLE;
									ent->s.powerups = STATE_INVISIBLE;

									if( !Q_stricmp( ent->classname, "misc_mg42" ) ) {
										mg42_stopusing( ent );
									} else if( ent->s.eType == ET_WOLF_OBJECTIVE ) {
										char cs[MAX_STRING_CHARS];
										trap_GetConfigstring( ent->count, cs, sizeof(cs) );
										ent->count2 |= 256;
										Info_SetValueForKey( cs, "t", va( "%i", ent->count2 ) );
										trap_SetConfigstring( ent->count, cs );
									}

									if( ent->s.eType == ET_COMMANDMAP_MARKER ) {
										mapEntityData_t	*mEnt;

										if((mEnt = G_FindMapEntityData(&mapEntityData[0], ent-g_entities)) != NULL) {
											G_FreeMapEntityData( &mapEntityData[0], mEnt );
										}
										if((mEnt = G_FindMapEntityData(&mapEntityData[1], ent-g_entities)) != NULL ) {
											G_FreeMapEntityData( &mapEntityData[1], mEnt );
										}
									}

									trap_UnlinkEntity( ent );
									break;
	}
}

static qboolean G_LoadCampaignsFromFile( const char *filename ) {
	int handle;
	pc_token_t token;
	const char *s;
	qboolean mapFound = qfalse;

	handle = trap_PC_LoadSource( filename );

	if( !handle ) {
		G_Printf( va( S_COLOR_RED "file not found: %s\n", filename ) );
		return qfalse;
	}

	if( !trap_PC_ReadToken( handle, &token ) ) {
		trap_PC_FreeSource( handle );
		return qfalse;
	}
	if( *token.string != '{' ) {
		trap_PC_FreeSource( handle );
		return qfalse;
	}

	while ( trap_PC_ReadToken( handle, &token ) ) {
		if( *token.string == '}' ) {
			level.campaignCount++;

			// zinx - can't handle any more.
			if( level.campaignCount >= MAX_CAMPAIGNS ) {
				break;
			}

			if( !trap_PC_ReadToken( handle, &token ) ) {
				// eof
				trap_PC_FreeSource( handle );
				break;
			}

			if( *token.string != '{' ) {
				G_Printf( va( S_COLOR_RED "unexpected token '%s' inside: %s\n", token.string, filename ) );
				trap_PC_FreeSource( handle );
				return qfalse;
			}
		} else if( !Q_stricmp( token.string, "name" ) ||
				   !Q_stricmp( token.string, "description" ) ||
				   !Q_stricmp( token.string, "image" ) ) {
			if((s = PC_String_Parse(handle)) == NULL) {
				G_Printf( va( S_COLOR_RED "unexpected end of file inside: %s\n", filename ) );
				trap_PC_FreeSource( handle );
				return qfalse;
			}
		} else if( !Q_stricmp( token.string, "shortname" ) ) {
			if((s = PC_String_Parse(handle)) == NULL) {
				G_Printf( va( S_COLOR_RED "unexpected end of file inside: %s\n", filename ) );
				trap_PC_FreeSource( handle );
				return qfalse;
			} else {
				Q_strncpyz( g_campaigns[level.campaignCount].shortname, s, sizeof(g_campaigns[level.campaignCount].shortname) );
			}
		} else if( !Q_stricmp( token.string, "next" ) ) {
			if((s = PC_String_Parse(handle)) == NULL) {
				G_Printf( va( S_COLOR_RED "unexpected end of file inside: %s\n", filename ) );
				trap_PC_FreeSource( handle );
				return qfalse;
			} else {
				Q_strncpyz( g_campaigns[level.campaignCount].shortname, s, sizeof(g_campaigns[level.campaignCount].next) );
			}
		} else if( !Q_stricmp( token.string, "type" ) ) {
			if( !trap_PC_ReadToken( handle, &token ) ) {
				G_Printf( va( S_COLOR_RED "unexpected end of file inside: %s\n", filename ) );
				trap_PC_FreeSource( handle );
				return qfalse;
			}

			if( strstr( token.string, "wolfsp" ) ) {
				g_campaigns[level.campaignCount].typeBits |= (1 << GT_SINGLE_PLAYER);
			}
			if( strstr( token.string, "wolfcoop" ) ) {
				g_campaigns[level.campaignCount].typeBits |= (1 << GT_COOP);
			}
			if( strstr( token.string, "wolfmp" ) ) {
				g_campaigns[level.campaignCount].typeBits |= (1 << GT_WOLF);
			}
			if( strstr( token.string, "wolfsw" ) ) {
				g_campaigns[level.campaignCount].typeBits |= (1 << GT_WOLF_STOPWATCH);
			}
			if( strstr( token.string, "wolflms" ) ) {
				g_campaigns[level.campaignCount].typeBits |= (1 << GT_WOLF_LMS);
			}
		} else if( !Q_stricmp( token.string, "maps" ) ) {
			char *ptr, mapname[128], *mapnamePtr;

			if( !trap_PC_ReadToken( handle, &token ) ) {
				G_Printf( S_COLOR_RED "unexpected end of file inside: %s\n", filename );
				trap_PC_FreeSource( handle );
				return qfalse;
			}

			ptr = token.string;
			while( *ptr ) {
				mapnamePtr = mapname;
				while( *ptr && *ptr != ';' ) {
					*mapnamePtr++ = *ptr++;
				}
				if( *ptr )
					ptr++;
				*mapnamePtr = '\0';

				if( g_gametype.integer == GT_WOLF_CAMPAIGN ) {
					if( !mapFound &&
						!Q_stricmp( g_campaigns[level.campaignCount].shortname, g_currentCampaign.string ) &&					
						!Q_stricmp( mapname, level.rawmapname ) ) {

						if( g_currentCampaignMap.integer == 0 ) {
							level.newCampaign = qtrue;
						} else {
							level.newCampaign = qfalse;
						}

						if( g_campaigns[level.campaignCount].mapCount == g_currentCampaignMap.integer ) {
							g_campaigns[level.campaignCount].current = g_campaigns[level.campaignCount].mapCount;
							mapFound = qtrue;
							//trap_Cvar_Set( "g_currentCampaignMap", va( "%i", g_campaigns[level.campaignCount].mapCount ) );
						}

						level.currentCampaign = level.campaignCount;
					}
				}
				// rain - don't stomp out of bounds
				if (g_campaigns[level.campaignCount].mapCount < MAX_MAPS_PER_CAMPAIGN) {
					Q_strncpyz( g_campaigns[level.campaignCount].mapnames[g_campaigns[level.campaignCount].mapCount], mapname, MAX_QPATH );
					g_campaigns[level.campaignCount].mapCount++;
				} else {
					// rain - yell if there are too many maps in this campaign,
					// and then skip it

					G_Printf("^1Error: Campaign %s (%s) has too many maps\n", g_campaigns[level.campaignCount].shortname, filename);
					// rain - hack - end of campaign will increment this
					// again, so this one will be overwritten
					// rain - clear out this campaign so that everything's
					// okay when when we add the next
					memset(&g_campaigns[level.campaignCount], 0, sizeof(g_campaigns[0]));
					level.campaignCount--;
					
					break;
				}
			}
		}
	}

	return mapFound;
}

qboolean G_MapIsValidCampaignStartMap( void ) {
	int			i;

	for( i = 0; i < level.campaignCount; i++ ) {
		if( !Q_stricmp( g_campaigns[i].mapnames[0], level.rawmapname ) ) {
			return qtrue;
		}
	}

	return qfalse;
}

void G_ParseCampaigns( void ) {
	int			numdirs;
	char		filename[128];
	char		dirlist[1024];
	char*		dirptr;
	int			i;
	int			dirlen;
	qboolean	mapFound = qfalse;

	level.campaignCount = 0;
	level.currentCampaign = -1;
	memset( &g_campaigns, 0, sizeof(g_campaignInfo_t)*MAX_CAMPAIGNS );

	// get all campaigns from .campaign files
	numdirs = trap_FS_GetFileList( "scripts", ".campaign", dirlist, 1024 );
	dirptr  = dirlist;
	for (i = 0; i < numdirs && level.campaignCount < MAX_CAMPAIGNS; i++, dirptr += dirlen+1) {
		dirlen = strlen(dirptr);
		strcpy(filename, "scripts/");
		strcat(filename, dirptr);
		
		if( G_LoadCampaignsFromFile(filename) ) {
			mapFound = qtrue;
		}
	}

	if( g_gametype.integer != GT_WOLF_CAMPAIGN ) {
		trap_Cvar_Set( "g_oldCampaign", "" );
		trap_Cvar_Set( "g_currentCampaign", "" );
		trap_Cvar_Set( "g_currentCampaignMap", "0" );
	} else if( !mapFound ) {
		// map isn't found in the current campaign, see if it's the first map in another campaign
		for( i = 0; i < level.campaignCount; i++ ) {
			if( !Q_stricmp( g_campaigns[i].mapnames[0], level.rawmapname ) ) {
				// someone manually specified a /map command, and it's the first map in a campaign
				trap_Cvar_Set( "g_oldCampaign", g_currentCampaign.string );
				trap_Cvar_Set( "g_currentCampaign", g_campaigns[i].shortname );
				trap_Cvar_Set( "g_currentCampaignMap", "0" );

				level.newCampaign = qtrue;

				g_campaigns[level.campaignCount].current = 0;
				level.currentCampaign = i;

				break;
			}
		}

		if( i == level.campaignCount ) {
			char buf[MAX_STRING_CHARS]; // fretn

			if (trap_Argc() < 1) { // command not found, throw error
				G_Error("Usage 'map <mapname>\n'");
			}

			trap_Argv(0, buf, sizeof(buf));

			if (!buf) { // command not found, throw error
				G_Error("Usage 'map <mapname>\n'");
			}

			// no campaign found, fallback to GT_WOLF
			// and reload the map
			trap_Cvar_Set( "g_gametype", "2");
			trap_SendConsoleCommand( EXEC_APPEND, va("%s %s", buf, level.rawmapname));
		}
	}
}

void G_PrintClientSpammyCenterPrint(int entityNum, char* text) {
	if(!g_entities[entityNum].client) {
		return;
	}

	if(level.time - g_entities[entityNum].client->lastSpammyCentrePrintTime < 1000) {
		return;
	}

	trap_SendServerCommand(entityNum, va("cp \"%s\" 1", text));
	g_entities[entityNum].client->lastSpammyCentrePrintTime = level.time;
}

team_t G_GetTeamFromEntity( gentity_t *ent ) {
	switch( ent->s.eType ) {
		case ET_PLAYER:		if( ent->client ) {
								return( ent->client->sess.sessionTeam );
							} else {
								return( TEAM_FREE );
							}
							break;
		case ET_MISSILE:
		case ET_GENERAL:	switch( ent->methodOfDeath ) {
								case MOD_GRENADE_LAUNCHER:
								case MOD_GRENADE_PINEAPPLE:
								case MOD_PANZERFAUST:
								case MOD_GPG40:
								case MOD_M7:
								case MOD_ARTY:
								case MOD_AIRSTRIKE:
								case MOD_MORTAR:
								case MOD_SMOKEGRENADE:
									return ent->s.teamNum;
								case MOD_SATCHEL:
								case MOD_DYNAMITE:
								case MOD_LANDMINE:
									return ent->s.teamNum % 4;
							}
							break;
		case ET_MOVER:		if( !Q_stricmp( ent->classname, "script_mover" ) ) {
								return ent->s.teamNum;
							}
							break;
		case ET_CONSTRUCTIBLE:	return ent->s.teamNum;
								break;
		case ET_MG42_BARREL: // zinx - fix for #470
			return G_GetTeamFromEntity( &g_entities[ent->r.ownerNum] );

		default:
			break;
	}

	return TEAM_FREE;
}
