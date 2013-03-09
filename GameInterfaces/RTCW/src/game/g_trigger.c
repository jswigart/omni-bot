#include "g_local.h"

void InitTrigger( gentity_t *self ) {
	if ( !VectorCompare( self->s.angles, vec3_origin ) ) {
		G_SetMovedir( self->s.angles, self->movedir );
	}

	trap_SetBrushModel( self, self->model );

	self->r.contents = CONTENTS_TRIGGER;        // replaces the -1 from trap_SetBrushModel
	self->r.svFlags = SVF_NOCLIENT;
}

// the wait time has passed, so set back up for another activation
void multi_wait( gentity_t *ent ) {
	ent->nextthink = 0;
}

// the trigger was just activated
// ent->activator should be set to the activator so it can be held through a delay
// so wait for the delay time before firing
void multi_trigger( gentity_t *ent, gentity_t *activator ) {
	ent->activator = activator;
	if ( ent->nextthink ) {
		return;     // can't retrigger until the wait is over
	}

	G_UseTargets( ent, ent->activator );

	if ( ent->wait > 0 ) {
		ent->think = multi_wait;
		ent->nextthink = level.time + ( ent->wait + ent->random * crandom() ) * 1000;
	} else {
		// we can't just remove (self) here, because this is a touch function
		// called while looping through area links...
		ent->touch = 0;
		ent->nextthink = level.time + FRAMETIME;
		ent->think = G_FreeEntity;
	}
}

void Use_Multi( gentity_t *ent, gentity_t *other, gentity_t *activator ) {
	multi_trigger( ent, activator );
}

void Touch_Multi( gentity_t *self, gentity_t *other, trace_t *trace ) {
	if ( !other->client ) {
		return;
	}

	multi_trigger( self, other );
}

void Enable_Trigger_Touch( gentity_t *ent ) {
	gentity_t *targ;

	// find the client number that uses this entity
	targ = AICast_FindEntityForName( ent->aiName );
	if ( !targ ) {
		return;
	} else
	{
		gentity_t *daent;
		trace_t tr;
		int mask = MASK_SHOT;
		int targTemp1, targTemp2;
		int entTemp1, entTemp2;
		qboolean thisone = qfalse;

		// bail if GIBFLAG and targ has been jibbed
		if ( targ->health <= GIB_HEALTH  && ( ent->spawnflags & 2 ) ) {
			return;
		}

		// need to make the ent solid since it is a trigger

		entTemp1 = ent->clipmask;
		entTemp2 = ent->r.contents;

		ent->clipmask   = CONTENTS_SOLID;
		ent->r.contents = CONTENTS_SOLID;

		trap_LinkEntity( ent );

		// same with targ cause targ is dead

		targTemp1 = targ->clipmask;
		targTemp2 = targ->r.contents;

		targ->clipmask   = CONTENTS_SOLID;
		targ->r.contents = CONTENTS_SOLID;

		trap_LinkEntity( targ );

		trap_Trace( &tr, targ->client->ps.origin, targ->r.mins, targ->r.maxs, targ->client->ps.origin, targ->s.number, mask );

		if ( tr.startsolid ) {
			daent = &g_entities[ tr.entityNum ];

			if ( daent == ent ) { // wooo hooo
				multi_trigger( ent, targ );
				thisone = qtrue;
			}
		}

		// ok were done set it contents back

		ent->clipmask = entTemp1;
		ent->r.contents = entTemp2;

		trap_LinkEntity( ent );

		targ->clipmask = targTemp1;
		targ->r.contents = targTemp2;

		trap_LinkEntity( targ );

		if ( ent->s.angles2[YAW] && thisone ) {
			vec3_t kvel, forward, dir;
			float angle = ent->s.angles2[YAW];

			VectorClear( dir );
			VectorClear( targ->client->ps.velocity );

			dir[YAW] = angle;
			AngleVectors( dir, forward, NULL, NULL );

			VectorScale( forward, 32, kvel );
			VectorAdd( targ->client->ps.velocity, kvel, targ->client->ps.velocity );
		}
	}
}

/*QUAKED trigger_multiple (.5 .5 .5) ? AI_Touch
"wait" : Seconds between triggerings, 0.5 default, -1 = one time only.
"random"	wait variance, default is 0
Variable sized repeatable trigger.  Must be targeted at one or more entities.
so, the basic time between firing is a random time between
(wait - random) and (wait + random)
*/
void SP_trigger_multiple( gentity_t *ent ) {
	G_SpawnFloat( "wait", "0.5", &ent->wait );
	G_SpawnFloat( "random", "0", &ent->random );

	if ( ent->random >= ent->wait && ent->wait >= 0 ) {
		ent->random = ent->wait - FRAMETIME;
		G_Printf( "trigger_multiple has random >= wait\n" );
	}

	ent->touch = Touch_Multi;
	ent->use = Use_Multi;

	InitTrigger( ent );
	trap_LinkEntity( ent );
}

/*
==============================================================================

trigger_always

==============================================================================
*/

void trigger_always_think( gentity_t *ent ) {
	G_UseTargets( ent, ent );
	G_FreeEntity( ent );
}

/*QUAKED trigger_always (.5 .5 .5) (-8 -8 -8) (8 8 8)
This trigger will always fire.  It is activated by the world.
*/
void SP_trigger_always( gentity_t *ent ) {
	// we must have some delay to make sure our use targets are present
	ent->nextthink = level.time + 300;
	ent->think = trigger_always_think;
}

/*
==============================================================================

trigger_push

==============================================================================
*/

void trigger_push_touch( gentity_t *self, gentity_t *other, trace_t *trace ) {
	if ( ( self->spawnflags & 4 ) && other->r.svFlags & SVF_CASTAI ) {
		return;
	}

	if ( !other->client ) {
		return;
	}

	if ( other->client->ps.pm_type != PM_NORMAL ) {
		return;
	}
	if ( other->client->ps.powerups[PW_FLIGHT] ) {
		return;
	}

	if ( other->client->ps.velocity[2] < 100 ) {
		// don't play the event sound again if we are in a fat trigger
		G_AddPredictableEvent( other, EV_JUMP_PAD, 0 );
	}
	VectorCopy( self->s.origin2, other->client->ps.velocity );

	if ( self->spawnflags & 2 ) {
		G_FreeEntity( self );
	}
}

/*
=================
AimAtTarget

Calculate origin2 so the target apogee will be hit
=================
*/
void AimAtTarget( gentity_t *self ) {
	gentity_t   *ent;
	vec3_t origin;
	float height, gravity, time, forward;
	float dist;

	VectorAdd( self->r.absmin, self->r.absmax, origin );
	VectorScale( origin, 0.5, origin );

	ent = G_PickTarget( self->target );
	if ( !ent ) {
		G_FreeEntity( self );
		return;
	}

	height = ent->s.origin[2] - origin[2];
	gravity = g_gravity.value;
	time = sqrt( fabs( height / ( 0.5f * gravity ) ) );
	if ( !time ) {
		G_FreeEntity( self );
		return;
	}

	// set s.origin2 to the push velocity
	VectorSubtract( ent->s.origin, origin, self->s.origin2 );
	self->s.origin2[2] = 0;
	dist = VectorNormalize( self->s.origin2 );

	forward = dist / time;
	VectorScale( self->s.origin2, forward, self->s.origin2 );

	self->s.origin2[2] = time * gravity;
}

void trigger_push_use( gentity_t *self, gentity_t *other, gentity_t *activator ) {
	self->touch = trigger_push_touch;
	trap_LinkEntity( self );
}

/*QUAKED trigger_push (.5 .5 .5) ? TOGGLE REMOVEAFTERTOUCH PUSHPLAYERONLY
Must point at a target_position, which will be the apex of the leap.
This will be client side predicted, unlike target_push
*/
void SP_trigger_push( gentity_t *self ) {
	InitTrigger( self );

	// unlike other triggers, we need to send this one to the client
	self->r.svFlags &= ~SVF_NOCLIENT;

	// make sure the client precaches this sound
	G_SoundIndex( "sound/world/jumppad.wav" );

	if ( !( self->spawnflags & 1 ) ) { // toggle
		self->s.eType = ET_PUSH_TRIGGER;
	}

	self->touch = trigger_push_touch;
	self->think = AimAtTarget;

	if ( self->spawnflags & 1 ) { // toggle
		self->use = trigger_push_use;
		self->touch = NULL;
		trap_UnlinkEntity( self );
	} else {
		trap_LinkEntity( self );
	}

	self->nextthink = level.time + FRAMETIME;
}

void Use_target_push( gentity_t *self, gentity_t *other, gentity_t *activator ) {
	if ( !activator->client ) {
		return;
	}

	if ( activator->client->ps.pm_type != PM_NORMAL ) {
		return;
	}
	if ( activator->client->ps.powerups[PW_FLIGHT] ) {
		return;
	}

	VectorCopy( self->s.origin2, activator->client->ps.velocity );

	// play fly sound every 1.5 seconds
	if ( activator->fly_sound_debounce_time < level.time ) {
		activator->fly_sound_debounce_time = level.time + 1500;
		G_Sound( activator, self->noise_index );
	}
}

/*QUAKED target_push (.5 .5 .5) (-8 -8 -8) (8 8 8) bouncepad
Pushes the activator in the direction.of angle, or towards a target apex.
"speed"		defaults to 1000
if "bouncepad", play bounce noise instead of windfly
*/
void SP_target_push( gentity_t *self ) {
	if ( !self->speed ) {
		self->speed = 1000;
	}
	G_SetMovedir( self->s.angles, self->s.origin2 );
	VectorScale( self->s.origin2, self->speed, self->s.origin2 );

	if ( self->spawnflags & 1 ) {
		self->noise_index = G_SoundIndex( "sound/world/jumppad.wav" );
	} else {
		self->noise_index = G_SoundIndex( "sound/misc/windfly.wav" );
	}
	if ( self->target ) {
		VectorCopy( self->s.origin, self->r.absmin );
		VectorCopy( self->s.origin, self->r.absmax );
		self->think = AimAtTarget;
		self->nextthink = level.time + FRAMETIME;
	}
	self->use = Use_target_push;
}

/*
==============================================================================

trigger_teleport

==============================================================================
*/

void trigger_teleporter_touch( gentity_t *self, gentity_t *other, trace_t *trace ) {
	gentity_t   *dest;

	if ( !other->client ) {
		return;
	}
	if ( other->client->ps.pm_type == PM_DEAD ) {
		return;
	}

	dest =  G_PickTarget( self->target );
	if ( !dest ) {
		G_Printf( "Couldn't find teleporter destination\n" );
		return;
	}

	TeleportPlayer( other, dest->s.origin, dest->s.angles );
}

/*QUAKED trigger_teleport (.5 .5 .5) ?
Allows client side prediction of teleportation events.
Must point at a target_position, which will be the teleport destination.
*/
void SP_trigger_teleport( gentity_t *self ) {
	InitTrigger( self );

	// unlike other triggers, we need to send this one to the client
	self->r.svFlags &= ~SVF_NOCLIENT;

	// make sure the client precaches this sound
	G_SoundIndex( "sound/world/jumppad.wav" );

	self->s.eType = ET_TELEPORT_TRIGGER;
	self->touch = trigger_teleporter_touch;

	trap_LinkEntity( self );
}

/*
==============================================================================

trigger_hurt

==============================================================================
*/

/*QUAKED trigger_hurt (.5 .5 .5) ? START_OFF - SILENT NO_PROTECTION SLOW ONCE
Any entity that touches this will be hurt.
It does dmg points of damage each server frame
Targeting the trigger will toggle its on / off state.

SILENT			supresses playing the sound
SLOW			changes the damage rate to once per second
NO_PROTECTION	*nothing* stops the damage

"dmg"			default 5 (whole numbers only)

"life"	time this brush will exist if value is zero will live for ever ei 0.5 sec 2.sec
default is zero

the entity must be used first before it will count down its life
*/
void hurt_touch( gentity_t *self, gentity_t *other, trace_t *trace ) {
	int dflags;

	if ( !other->takedamage ) {
		return;
	}

	if ( self->timestamp > level.time ) {
		return;
	}

	if ( self->spawnflags & 16 ) {
		self->timestamp = level.time + 1000;
	} else {
		self->timestamp = level.time + FRAMETIME;
	}

	// play sound
	if ( !( self->spawnflags & 4 ) ) {
		G_Sound( other, self->noise_index );
	}

	if ( self->spawnflags & 8 ) {
		dflags = DAMAGE_NO_PROTECTION;
	} else {
		dflags = 0;
	}
	G_Damage( other, self, self, NULL, NULL, self->damage, dflags, MOD_TRIGGER_HURT );

	if ( self->spawnflags & 32 ) {
		self->touch = NULL;
	}
}

void hurt_think( gentity_t *ent ) {
	ent->nextthink = level.time + FRAMETIME;

	if ( ent->wait < level.time ) {
		G_FreeEntity( ent );
	}
}

void hurt_use( gentity_t *self, gentity_t *other, gentity_t *activator ) {
	if ( self->touch ) {
		self->touch = NULL;
	} else {
		self->touch = hurt_touch;
	}

	if ( self->delay ) {
		self->nextthink = level.time + 50;
		self->think = hurt_think;
		self->wait = level.time + ( self->delay * 1000 );
	}
}

/*
==============
SP_trigger_hurt
==============
*/
void SP_trigger_hurt( gentity_t *self ) {
	char    *life, *sound; // JPW NERVE
	float dalife;

	InitTrigger( self );

	G_SpawnString( "sound", "sound/world/electro.wav", &sound );

	self->noise_index = G_SoundIndex( sound );

	if ( !self->damage ) {
		self->damage = 5;
	}

	self->r.contents = CONTENTS_TRIGGER;

	self->use = hurt_use;

	// link in to the world if starting active
	if ( !( self->spawnflags & 1 ) ) {
		self->touch = hurt_touch;
	}

	G_SpawnString( "life", "0", &life );
	dalife = atof( life );
	self->delay = dalife;
}

/*
==============================================================================

timer

==============================================================================
*/

/*QUAKED func_timer (0.3 0.1 0.6) (-8 -8 -8) (8 8 8) START_ON
This should be renamed trigger_timer...
Repeatedly fires its targets.
Can be turned on or off by using.

"wait"			base time between triggering all targets, default is 1
"random"		wait variance, default is 0
so, the basic time between firing is a random time between
(wait - random) and (wait + random)

*/
void func_timer_think( gentity_t *self ) {
	G_UseTargets( self, self->activator );
	// set time before next firing
	self->nextthink = level.time + 1000 * ( self->wait + crandom() * self->random );
}

void func_timer_use( gentity_t *self, gentity_t *other, gentity_t *activator ) {
	self->activator = activator;

	// if on, turn it off
	if ( self->nextthink ) {
		self->nextthink = 0;
		return;
	}

	// turn it on
	func_timer_think( self );
}

void SP_func_timer( gentity_t *self ) {
	G_SpawnFloat( "random", "1", &self->random );
	G_SpawnFloat( "wait", "1", &self->wait );

	self->use = func_timer_use;
	self->think = func_timer_think;

	if ( self->random >= self->wait ) {
		self->random = self->wait - FRAMETIME;
		G_Printf( "func_timer at %s has random >= wait\n", vtos( self->s.origin ) );
	}

	if ( self->spawnflags & 1 ) {
		self->nextthink = level.time + FRAMETIME;
		self->activator = self;
	}

	self->r.svFlags = SVF_NOCLIENT;
}

//---- (SA) Wolf triggers

/*QUAKED trigger_once (.5 .5 .5) ? AI_Touch
Must be targeted at one or more entities.
Once triggered, this entity is destroyed
(you can actually do the same thing with trigger_multiple with a wait of -1)
*/
void SP_trigger_once( gentity_t *ent ) {
	ent->wait   = -1;           // this will remove itself after one use
	ent->touch  = Touch_Multi;
	ent->use    = Use_Multi;

	InitTrigger( ent );
	trap_LinkEntity( ent );
}

//---- end

/*QUAKED trigger_deathCheck (.5 .5 .5) ? - GIBFLAG
GIBFLAG entity will never fire its target(s) if aiName entity was gibbed
aiName entity making alertentity call

this entity will test if aiName is in its volume

Must be targeted at one or more entities.
Once triggered, this entity is destroyed
*/
void SP_trigger_deathCheck( gentity_t *ent ) {
	VectorCopy( ent->s.angles, ent->s.angles2 );

	if ( !( ent->aiName ) ) {
		G_Error( "trigger_once_enabledeath does not have an aiName \n" );
	}

	ent->wait   = -1;           // this will remove itself after one use
	ent->AIScript_AlertEntity = Enable_Trigger_Touch;
	ent->use    = Use_Multi;

	InitTrigger( ent );

	trap_LinkEntity( ent );
}

/*QUAKED trigger_aidoor (.5 .5 .5) ?
These entities must be placed on all doors one for each side of the door
this will enable ai's to operate the door and help in preventing ai's and
the player from getting stuck when the door is deciding which way to open
*/

void trigger_aidoor_stayopen( gentity_t * ent, gentity_t * other, trace_t * trace ) {
	gentity_t *door;

	// FIXME: port this code over to moving doors (use MOVER_POSx instead of MOVER_POSxROTATE)
	if ( other->client && other->health > 0 ) {
		if ( !ent->target || !( strlen( ent->target ) ) ) {
			return;
		}

		door = G_Find( NULL, FOFS( targetname ), ent->target );
		if ( !door ) {
			return;
		}

		if ( door->moverState == MOVER_POS2ROTATE ) {
			// door is in open state waiting to close keep it open
			door->nextthink = level.time + door->wait + 3000;
		}
	}
}

void SP_trigger_aidoor( gentity_t *ent ) {
	if ( !ent->targetname ) {
		G_DPrintf( "trigger_aidoor at loc %s does not have a targetname for ai_marker assignments\n", vtos( ent->s.origin ) );
	}

	ent->touch = trigger_aidoor_stayopen;
	InitTrigger( ent );
	trap_LinkEntity( ent );
}

void gas_touch( gentity_t *ent, gentity_t *other, trace_t *trace ) {
	gentity_t       *traceEnt;
	trace_t tr;
	vec3_t dir;
	int damage = 1;

	if ( !other->client ) {
		return;
	}

	if ( ent->s.density == 5 ) {
		ent->touch = NULL;
		damage = 5;
	}

	trap_Trace( &tr, ent->r.currentOrigin, NULL, NULL, other->r.currentOrigin, ent->s.number, MASK_SHOT );

	if ( tr.surfaceFlags & SURF_NOIMPACT ) {
		return;
	}

	traceEnt = &g_entities[ tr.entityNum ];

	if ( traceEnt->takedamage ) {
		VectorClear( dir );

		G_Damage( traceEnt, ent, ent, dir, tr.endpos,
			damage, 0, MOD_POISONGAS );
	}
}

void gas_think( gentity_t *ent ) {
	gentity_t *tent;

	ent->count++;

	if ( ent->health < ent->count ) {
		ent->think = G_FreeEntity;
		if ( ent->s.density == 5 ) {
			ent->nextthink = level.time + FRAMETIME;
		} else {
			ent->nextthink = level.time + 3000;
		}
		return;
	}

	ent->r.maxs[0] = ent->r.maxs[1] = ent->r.maxs[2]++;
	ent->r.mins[0] = ent->r.mins[1] = ent->r.mins[2]--;

	ent->nextthink = level.time + FRAMETIME;

	tent = G_TempEntity( ent->r.currentOrigin, EV_SMOKE );
	VectorCopy( ent->r.currentOrigin, tent->s.origin );

	if ( ent->s.density == 5 ) {
		tent->s.time = 500;
		tent->s.time2 = 100;
		tent->s.density = 5;

		tent->s.angles2[0] = 8;
		tent->s.angles2[1] = 32;
	} else
	{
		tent->s.time = 5000;
		tent->s.time2 = 3000;
		tent->s.density = 5;

		tent->s.angles2[0] = 24;
		tent->s.angles2[1] = 96;
	}

	trap_LinkEntity( ent );
}

/*QUAKED test_gas (0 0.5 0) (-4 -4 -4) (4 4 4)
*/
void SP_gas( gentity_t *self ) {
	self->think = gas_think;
	self->nextthink = level.time + FRAMETIME;
	self->r.contents = CONTENTS_TRIGGER;
	self->touch = gas_touch;
	trap_LinkEntity( self );

	if ( !self->health ) {
		self->health = 100;
	}
}

// DHM - Nerve :: Multiplayer triggers

#define RED_FLAG 1
#define BLUE_FLAG 2

/*QUAKED trigger_flagonly (.5 .5 .5) ? RED_FLAG BLUE_FLAG
Player must be carrying the proper flag for it to trigger.
It will call the "death" function in the object's script.

"scriptName"	The object name in the script file
"score"			score given to player for dropping flag in this field
(default 20)

RED_FLAG -- only trigger if player is carrying red flag
BLUE_FLAG -- only trigger if player is carrying blue flag
*/

void Touch_flagonly( gentity_t *ent, gentity_t *other, trace_t *trace ) {
	if ( !other->client ) {
		return;
	}

	if ( ent->spawnflags & RED_FLAG && other->client->ps.powerups[ PW_REDFLAG ] ) {
		AddScore( other, ent->accuracy ); // JPW NERVE set from map, defaults to 20

		// credits
		other->client->sess.credits += CREDITS_OBJBONUS;

		G_Script_ScriptEvent( ent, "death", "" );

		// Removes itself
		ent->touch = NULL;
		ent->nextthink = level.time + FRAMETIME;
		ent->think = G_FreeEntity;
	} else if ( ent->spawnflags & BLUE_FLAG && other->client->ps.powerups[ PW_BLUEFLAG ] ) {
		AddScore( other, ent->accuracy ); // JPW NERVE set from map, defaults to 20

		// credits
		other->client->sess.credits += CREDITS_OBJBONUS;

		G_Script_ScriptEvent( ent, "death", "" );

		// Removes itself
		ent->touch = NULL;
		ent->nextthink = level.time + FRAMETIME;
		ent->think = G_FreeEntity;
	}
}

void SP_trigger_flagonly( gentity_t *ent ) {
	char *scorestring; // JPW NERVE
	ent->touch  = Touch_flagonly;

	InitTrigger( ent );

	// JPW NERVE -- if this trigger has a "score" field set, then completing an objective
	//  inside of this field will add "score" to the right player team.  storing this
	//  in ent->accuracy since that's unused.
	G_SpawnString( "score", "20", &scorestring );
	ent->accuracy = atof( scorestring );
	// jpw

	trap_LinkEntity( ent );
}

// NERVE - SMF - spawn an explosive indicator
void explosive_indicator_think( gentity_t *ent ) {
	gentity_t *parent;

	parent = &g_entities[ent->r.ownerNum];

	if ( !parent->inuse || Q_stricmp( "trigger_objective_info", parent->classname ) ) {
		ent->think = G_FreeEntity;
		ent->nextthink = level.time + FRAMETIME;
		return;
	}

	ent->nextthink = level.time + FRAMETIME;
}

/*QUAKED trigger_objective_info (.5 .5 .5) ? AXIS_OBJECTIVE ALLIED_OBJECTIVE
Players in this field will see a message saying that they are near an objective.

"track"		Mandatory, this is the text that is appended to "You are near "
*/
#define AXIS_OBJECTIVE      1
#define ALLIED_OBJECTIVE    2

void SP_trigger_objective_info( gentity_t *ent ) {
	char *scorestring;

	if ( !ent->track ) {
		G_Error( "'trigger_objective_info' does not have a 'track' \n" );
	}

	if ( level.numOidTriggers >= MAX_OID_TRIGGERS ) {
		G_Error( "Exceeded maximum number of 'trigger_objective_info' entities\n" );
	}

	// JPW NERVE -- if this trigger has a "score" field set, then blowing up an objective
	//  inside of this field will add "score" to the right player team.  storing this
	//  in ent->accuracy since that's unused.
	G_SpawnString( "score", "0", &scorestring );
	ent->accuracy = atof( scorestring );
	// jpw

	// Arnout: HACK HACK - someone at nerve forgot to add the score field to sub - have to
	// hardcode it cause we don't want people to download the map again
	{
		char mapName[MAX_QPATH];
		trap_Cvar_VariableStringBuffer( "mapname", mapName, sizeof( mapName ) );
		if ( !Q_stricmp( mapName, "mp_sub" ) && !Q_stricmp( ent->track, "the Axis Submarine" ) ) {
			ent->accuracy = 15;
		}
	}

	trap_SetConfigstring( CS_OID_TRIGGERS + level.numOidTriggers, ent->track );
	ent->s.teamNum = level.numOidTriggers;

	level.numOidTriggers++;
	InitTrigger( ent );

	// unlike other triggers, we need to send this one to the client
	// cs: in deathmatch, we don't want the "you are near ..." display
	if ( !g_deathmatch.integer ) {
		ent->r.svFlags &= ~SVF_NOCLIENT;
		ent->s.eType = ET_OID_TRIGGER;
	}

	trap_LinkEntity( ent );

	// NERVE - SMF - spawn an explosive indicator
	// cs: in deathmatch, we don't want the target icon on the radar
	if ( !g_deathmatch.integer && ((ent->spawnflags & AXIS_OBJECTIVE) || (ent->spawnflags & ALLIED_OBJECTIVE)) ) {
		gentity_t *e;
		e = G_Spawn();

		e->r.svFlags = SVF_BROADCAST;
		e->classname = "explosive_indicator";
		e->s.eType = ET_EXPLOSIVE_INDICATOR;
		e->s.pos.trType = TR_STATIONARY;

		if ( ent->spawnflags & AXIS_OBJECTIVE ) {
			e->s.teamNum = 1;
		} else if ( ent->spawnflags & ALLIED_OBJECTIVE ) {
			e->s.teamNum = 2;
		}

		e->r.ownerNum = ent->s.number;
		e->think = explosive_indicator_think;
		e->nextthink = level.time + FRAMETIME;

		VectorCopy( ent->r.mins, e->s.pos.trBase );
		VectorAdd( ent->r.maxs, e->s.pos.trBase, e->s.pos.trBase );
		VectorScale( e->s.pos.trBase, 0.5, e->s.pos.trBase );

		SnapVector( e->s.pos.trBase );

		trap_LinkEntity( e );
	}
	// -NERVE - SMF
}

// dhm - end

// JPW NERVE -- field which is acted upon (cgame side) by screenshakes to drop dust particles
void trigger_concussive_touch( gentity_t *ent, gentity_t *other, trace_t *trace ) {
}

/*QUAKED trigger_concussive_dust (.5 .5 .5) ?
Allows client side prediction of teleportation events.
Must point at a target_position, which will be the teleport destination.
*/
void SP_trigger_concussive_dust( gentity_t *self ) {
	InitTrigger( self );

	self->s.eType = ET_CONCUSSIVE_TRIGGER;
	self->touch = trigger_concussive_touch;

	trap_LinkEntity( self );
}
// jpw