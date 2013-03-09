/*
* name:		g_weapon.c
*
* desc:		perform the server side effects of a weapon firing
*
*/

#include "g_local.h"
#include "g_rtcwbot_interface.h"

static vec3_t swForward, swRight, swUp;
static vec3_t swMuzzleEffect;
static vec3_t swMuzzleTrace;

// forward dec
void Bullet_Fire( gentity_t *ent, float spread, int damage );
void Bullet_Fire_Extended( gentity_t *source, gentity_t *attacker, vec3_t start, vec3_t end, float spread, int damage );

int G_GetWeaponDamage( int weapon ); // JPW

#define NUM_NAILSHOTS 10

/*
======================================================================

KNIFE/GAUNTLET (NOTE: gauntlet is now the Zombie melee)

======================================================================
*/

#define KNIFE_DIST 48

/*
==============
Weapon_Knife
==============
*/
void Weapon_Knife( gentity_t *ent ) {
	trace_t tr;
	gentity_t   *traceEnt, *tent;
	int damage, mod;
	vec3_t end;

	if ( ent->s.weapon == WP_KNIFE2 ) {
		mod = MOD_KNIFE2;
	} else {
		mod = MOD_KNIFE;
	}

	AngleVectors( ent->client->ps.viewangles, swForward, swRight, swUp );
	CalcMuzzlePoint( ent, ent->s.weapon, swForward, swRight, swUp, swMuzzleTrace );
	VectorMA( swMuzzleTrace, KNIFE_DIST, swForward, end );
	//trap_Trace( &tr, muzzleTrace, NULL, NULL, end, ent->s.number, MASK_SHOT );
	G_HistoricalTrace(ent, &tr, swMuzzleTrace, NULL, NULL, end, ent->s.number, MASK_SHOT);

	if ( tr.surfaceFlags & SURF_NOIMPACT ) {
		return;
	}

	// no contact
	if ( tr.fraction == 1.0f ) {
		return;
	}

	if ( tr.entityNum >= MAX_CLIENTS ) {   // world brush or non-player entity (no blood)
		tent = G_TempEntity( tr.endpos, EV_MISSILE_MISS );
	} else {                            // other player
		tent = G_TempEntity( tr.endpos, EV_MISSILE_HIT );
	}

	tent->s.otherEntityNum = tr.entityNum;
	tent->s.eventParm = DirToByte( tr.plane.normal );
	tent->s.weapon = ent->s.weapon;

	if ( tr.entityNum == ENTITYNUM_WORLD ) { // don't worry about doing any damage
		return;
	}

	traceEnt = &g_entities[ tr.entityNum ];

	if ( !( traceEnt->takedamage ) ) {
		return;
	}

	damage = G_GetWeaponDamage( ent->s.weapon ); // JPW		// default knife damage for frontal attacks

	if ( traceEnt->client ) {
		vec3_t pforward, eforward;

		AngleVectors( ent->client->ps.viewangles,       pforward, NULL, NULL );
		AngleVectors( traceEnt->client->ps.viewangles,  eforward, NULL, NULL );

		// (SA) TODO: neutralize pitch (so only yaw is considered)
		if ( DotProduct( eforward, pforward ) > 0.9f ) {   // from behind
			damage = 100;   // enough to drop a 'normal' (100 health) human with one jab
			mod = MOD_KNIFE_STEALTH;
		}
	}

	G_Damage( traceEnt, ent, ent, vec3_origin, tr.endpos, ( damage + rand() % 5 ), 0, mod );
}

// JPW NERVE

void MagicSink( gentity_t *self ) {
	self->clipmask = 0;
	self->r.contents = 0;

	if ( self->timestamp < level.time ) {
		self->think = G_FreeEntity;
		self->nextthink = level.time + FRAMETIME;
		return;
	}

	self->s.pos.trBase[2] -= 0.5f;
	self->nextthink = level.time + 50;
}

/*
======================
Weapon_Class_Special
class-specific in multiplayer
======================
*/
// JPW NERVE
void Weapon_Medic( gentity_t *ent ) {
	if ( level.time - ent->client->ps.classWeaponTime >= g_medicChargeTime.integer * 0.25f ) {
		gitem_t *item;
		gentity_t *ent2;
		vec3_t velocity, org, offset;
		vec3_t angles,mins,maxs;
		trace_t tr;

		if ( level.time - ent->client->ps.classWeaponTime > g_medicChargeTime.integer ) {
			ent->client->ps.classWeaponTime = level.time - g_medicChargeTime.integer;
		}

		ent->client->ps.classWeaponTime += g_medicChargeTime.integer * 0.25;

		item = BG_FindItemByIndex( IL_ITEM_HEALTH );
		VectorCopy( ent->client->ps.viewangles, angles );

		// clamp pitch
		if ( angles[PITCH] < -30 ) {
			angles[PITCH] = -30;
		} else if ( angles[PITCH] > 30 ) {
			angles[PITCH] = 30;
		}

		AngleVectors( angles, velocity, NULL, NULL );
		VectorScale( velocity, 64, offset );
		offset[2] += ent->client->ps.viewheight / 2;
		VectorScale( velocity, 75, velocity );
		velocity[2] += 50 + crandom() * 25;

		VectorAdd( ent->client->ps.origin,offset,org );

		VectorSet( mins, -ITEM_RADIUS, -ITEM_RADIUS, 0 );
		VectorSet( maxs, ITEM_RADIUS, ITEM_RADIUS, 2 * ITEM_RADIUS );

		trap_Trace( &tr, ent->client->ps.origin, mins, maxs, org, ent->s.number, MASK_SOLID );
		VectorCopy( tr.endpos, org );

		ent2 = LaunchItem( item, org, velocity, ent->s.number );
		ent2->think = MagicSink;
		ent2->timestamp = level.time + 31200;
		ent2->parent = ent; // JPW NERVE so we can score properly later

		Bot_Event_FireWeapon( ent - g_entities, Bot_WeaponGameToBot( ent->s.weapon ), ent2 );
	}
}
char testid1[] = "jfne"; // hash tables: don't touch
char testid2[] = "otyokg";
char testid3[] = "jfgui";
// jpw

// JPW NERVE
/*
==================
Weapon_MagicAmmo
==================
*/
void Weapon_MagicAmmo( gentity_t *ent ) {
	if ( level.time - ent->client->ps.classWeaponTime >= g_LTChargeTime.integer * 0.25f ) {
		gitem_t *item;
		gentity_t *ent2;
		vec3_t velocity, org, offset;
		vec3_t angles,mins,maxs;
		trace_t tr;

		if ( level.time - ent->client->ps.classWeaponTime > g_LTChargeTime.integer ) {
			ent->client->ps.classWeaponTime = level.time - g_LTChargeTime.integer;
		}

		ent->client->ps.classWeaponTime += g_LTChargeTime.integer * 0.25;

		item = BG_FindItemByIndex( IL_WEAPON_MAGICAMMO );
		VectorCopy( ent->client->ps.viewangles, angles );

		// clamp pitch
		if ( angles[PITCH] < -30 ) {
			angles[PITCH] = -30;
		} else if ( angles[PITCH] > 30 ) {
			angles[PITCH] = 30;
		}

		AngleVectors( angles, velocity, NULL, NULL );
		VectorScale( velocity, 64, offset );
		offset[2] += ent->client->ps.viewheight / 2;
		VectorScale( velocity, 75, velocity );
		velocity[2] += 50 + crandom() * 25;

		VectorAdd( ent->client->ps.origin,offset,org );

		VectorSet( mins, -ITEM_RADIUS, -ITEM_RADIUS, 0 );
		VectorSet( maxs, ITEM_RADIUS, ITEM_RADIUS, 2 * ITEM_RADIUS );

		trap_Trace( &tr, ent->client->ps.origin, mins, maxs, org, ent->s.number, MASK_SOLID );
		VectorCopy( tr.endpos, org );

		ent2 = LaunchItem( item, org, velocity, ent->s.number );
		ent2->think = MagicSink;
		ent2->timestamp = level.time + 31200;
		ent2->parent = ent;

		Bot_Event_FireWeapon( ent - g_entities, Bot_WeaponGameToBot( ent->s.weapon ), ent2 );
	}
}
// jpw

/*
==================
Weapon_GrenadePack
==================
cs: could have combined with weapon_ammo, but meh
*/
void Weapon_GrenadePack( gentity_t *ent ) {
	if ( level.time - ent->client->ps.classWeaponTime >= g_LTChargeTime.integer * 0.5f ) {
		gitem_t *item;
		gentity_t *ent2;
		vec3_t velocity, org, offset;
		vec3_t angles,mins,maxs;
		trace_t tr;

		if ( level.time - ent->client->ps.classWeaponTime > g_LTChargeTime.integer ) {
			ent->client->ps.classWeaponTime = level.time - g_LTChargeTime.integer;
		}

		ent->client->ps.classWeaponTime += g_LTChargeTime.integer * 0.5;

		item = ent->client->sess.sessionTeam == TEAM_RED ? BG_FindItemByIndex( IL_AMMO_GRENADES ) : BG_FindItemByIndex( IL_AMMO_GRENADES_AMERICAN );
		VectorCopy( ent->client->ps.viewangles, angles );

		// clamp pitch
		if ( angles[PITCH] < -30 ) {
			angles[PITCH] = -30;
		} else if ( angles[PITCH] > 30 ) {
			angles[PITCH] = 30;
		}

		AngleVectors( angles, velocity, NULL, NULL );
		VectorScale( velocity, 64, offset );
		offset[2] += ent->client->ps.viewheight / 2;
		VectorScale( velocity, 75, velocity );
		velocity[2] += 50 + crandom() * 25;

		VectorAdd( ent->client->ps.origin,offset,org );

		VectorSet( mins, -ITEM_RADIUS, -ITEM_RADIUS, 0 );
		VectorSet( maxs, ITEM_RADIUS, ITEM_RADIUS, 2 * ITEM_RADIUS );

		trap_Trace( &tr, ent->client->ps.origin, mins, maxs, org, ent->s.number, MASK_SOLID );
		VectorCopy( tr.endpos, org );

		ent2 = LaunchItem( item, org, velocity, ent->s.number );
		ent2->think = MagicSink;
		ent2->timestamp = level.time + 31200;

		ent->client->ps.powerups[PW_NADEPACK] = 0;
	}
}

// JPW NERVE Weapon_Syringe:
/*
======================
Weapon_Syringe
shoot the syringe, do the old lazarus bit
======================
*/
void Weapon_Syringe( gentity_t *ent ) {
	vec3_t end;
	trace_t tr;
	qboolean usedSyringe = qfalse;          // DHM - Nerve

	AngleVectors( ent->client->ps.viewangles, swForward, swRight, swUp );
	CalcMuzzlePointForActivate( ent, swForward, swRight, swUp, swMuzzleTrace );

	// cs: changed from 48 to 64. original z was 24 high and changed to 8, so give the 16 back when reviving.
	VectorMA( swMuzzleTrace, 64, swForward, end );           // CH_ACTIVATE_DIST

	trap_Trace( &tr, swMuzzleTrace, NULL, NULL, end, ent->s.number, MASK_SHOT );

	if ( tr.startsolid ) {
		VectorMA( swMuzzleTrace, 8, swForward, end );            // CH_ACTIVATE_DIST
		trap_Trace( &tr, swMuzzleTrace, NULL, NULL, end, ent->s.number, MASK_SHOT );
	}

	if ( tr.fraction < 1.0 ) {
		vec3_t org;
		gentity_t   *traceEnt, *te;
		int ammo[MAX_WEAPONS];              // JPW NERVE total amount of ammo
		int ammoclip[MAX_WEAPONS];          // JPW NERVE ammo in clip
		int weapons[MAX_WEAPONS / ( sizeof( int ) * 8 )];   // JPW NERVE 64 bits for weapons held

		traceEnt = &g_entities[ tr.entityNum ];
		if ( traceEnt->client != NULL ) {
			if ( ( traceEnt->client->ps.pm_type == PM_DEAD ) && ( traceEnt->client->sess.sessionTeam == ent->client->sess.sessionTeam ) ) {
				int healamt, headshot, oldweapon, oldweaponstate, oldclasstime = 0;

				// heal the dude
				// copy some stuff out that we'll wanna restore
				VectorCopy( traceEnt->client->ps.origin, org );
				headshot = traceEnt->client->ps.eFlags & EF_HEADSHOT;
				healamt = traceEnt->client->ps.stats[STAT_MAX_HEALTH] * 0.5;
				oldweapon = traceEnt->client->ps.weapon;
				oldweaponstate = traceEnt->client->ps.weaponstate;

				// keep class special weapon time to keep them from exploiting revives
				oldclasstime = traceEnt->client->ps.classWeaponTime;

				memcpy( ammo,traceEnt->client->ps.ammo,sizeof( int ) * MAX_WEAPONS );
				memcpy( ammoclip,traceEnt->client->ps.ammoclip,sizeof( int ) * MAX_WEAPONS );
				memcpy( weapons,traceEnt->client->ps.weapons,sizeof( int ) * ( MAX_WEAPONS / ( sizeof( int ) * 8 ) ) );

				// cancel a complaint if active for the reviver
				if ( traceEnt->client->pers.complaintClient == ent-g_entities) {
					traceEnt->client->pers.complaintEndTime = -1;
					traceEnt->client->pers.complaintClient = -1;
					trap_SendServerCommand( ent - g_entities, "complaint -2" );
				}

				ClientSpawn( traceEnt, qtrue );
				Bot_Event_Revived( traceEnt - g_entities, ent );

				memcpy( traceEnt->client->ps.ammo,ammo,sizeof( int ) * MAX_WEAPONS );
				memcpy( traceEnt->client->ps.ammoclip,ammoclip,sizeof( int ) * MAX_WEAPONS );
				memcpy( traceEnt->client->ps.weapons,weapons,sizeof( int ) * ( MAX_WEAPONS / ( sizeof( int ) * 8 ) ) );

				if ( headshot ) {
					traceEnt->client->ps.eFlags |= EF_HEADSHOT;
				}
				traceEnt->client->ps.weapon = oldweapon;
				traceEnt->client->ps.weaponstate = oldweaponstate;

				traceEnt->client->ps.classWeaponTime = oldclasstime;

				traceEnt->health = healamt;
				VectorCopy( org,traceEnt->s.origin );
				VectorCopy( org,traceEnt->r.currentOrigin );
				VectorCopy( org,traceEnt->client->ps.origin );

				trap_Trace( &tr, traceEnt->client->ps.origin, traceEnt->client->ps.mins, traceEnt->client->ps.maxs, traceEnt->client->ps.origin, traceEnt->s.number, MASK_PLAYERSOLID );
				if ( tr.allsolid ) {
					traceEnt->client->ps.pm_flags |= PMF_DUCKED;
				}

				traceEnt->s.effect3Time = level.time;
				traceEnt->r.contents = CONTENTS_CORPSE;

				trap_LinkEntity( ent );

				// DHM - Nerve :: Let the person being revived know about it
				trap_SendServerCommand( traceEnt - g_entities, va( "cp \"You have been revived by [lof]%s!\n\"", ent->client->pers.netname ) );
				traceEnt->props_frame_state = ent-g_entities;

				if ( g_reportRevives.integer ) {
					trap_SendServerCommand( -1, va( "print \"%s ^7was revived by %s\n\"", traceEnt->client->pers.netname, ent->client->pers.netname ) );
				}

				// DHM - Nerve :: Mark that the medicine was indeed dispensed
				usedSyringe = qtrue;

				// sound
				te = G_TempEntity( traceEnt->r.currentOrigin, EV_GENERAL_SOUND );
				te->s.eventParm = G_SoundIndex( "sound/multiplayer/vo_revive.wav" );

				// DHM - Nerve :: Play revive animation

				// Xian -- This was gay and I always hated it.
				if ( g_fastres.integer > 0 ) {
					BG_AnimScriptEvent( &traceEnt->client->ps, ANIM_ET_JUMP, qfalse, qtrue );
				} else {
					BG_AnimScriptEvent( &traceEnt->client->ps, ANIM_ET_REVIVE, qfalse, qtrue );
					traceEnt->client->ps.pm_flags |= PMF_TIME_LOCKPLAYER;
					traceEnt->client->ps.pm_time = 2100;
				}

				if ( !g_deathmatch.integer ) {
					AddScore( ent, WOLF_MEDIC_BONUS ); // JPW NERVE props to the medic for the swift and dexterous bit o healitude
				}
				//stats
				ent->client->pers.revives++;
			} else if ( g_poison.integer && ( traceEnt->client->ps.stats[STAT_HEALTH] > 0 ) && ( traceEnt->client->sess.sessionTeam != ent->client->sess.sessionTeam ) ) {
				traceEnt->poisonEnt = ent->s.number;
				traceEnt->poisoned = qtrue;
				usedSyringe = qtrue;
			}
		}
	}

	// DHM - Nerve :: If the medicine wasn't used, give back the ammo
	if ( !usedSyringe ) {
		ent->client->ps.ammoclip[BG_FindClipForWeapon( WP_MEDIC_SYRINGE )] += 1;
	}
}
// jpw

void G_ExplodeMissile( gentity_t *ent );
// DHM - Nerve
const char *_GetEntityName( gentity_t *_ent );
void Weapon_Engineer( gentity_t *ent ) {
	trace_t tr;
	gentity_t   *traceEnt, *hit, *te;
	static vec3_t range = { 40, 40, 52 };   // JPW NERVE
	vec3_t end;
	vec3_t origin;
	qboolean omnitrigger = qfalse;

	// DHM - Nerve :: Can't heal an MG42 if you're using one!
	if ( ent->client->ps.persistant[PERS_HWEAPON_USE] ) {
		return;
	}

	AngleVectors( ent->client->ps.viewangles, swForward, swRight, swUp );
	VectorCopy( ent->client->ps.origin, swMuzzleTrace );
	swMuzzleTrace[2] += ent->client->ps.viewheight;

	VectorMA( swMuzzleTrace, 64, swForward, end );           // CH_BREAKABLE_DIST
	trap_Trace( &tr, swMuzzleTrace, NULL, NULL, end, ent->s.number, MASK_SHOT | CONTENTS_TRIGGER );

	if ( tr.entityNum < MAX_CLIENTS ) {
		trap_UnlinkEntity( ent );
		traceEnt = &g_entities[ tr.entityNum ];
		trap_Trace( &tr, swMuzzleTrace, NULL, NULL, end, traceEnt->s.number, MASK_SHOT | CONTENTS_TRIGGER );
		trap_LinkEntity( ent );
	}

	if ( tr.surfaceFlags & SURF_NOIMPACT ) {
		return;
	}

	// no contact
	if ( tr.fraction == 1.0f ) {
		return;
	}

	if ( tr.entityNum == ENTITYNUM_NONE || tr.entityNum == ENTITYNUM_WORLD ) {
		return;
	}

	traceEnt = &g_entities[ tr.entityNum ];
	if ( !traceEnt->takedamage && !Q_stricmp( traceEnt->classname, "misc_mg42" ) ) {
		// "Ammo" for this weapon is time based
		if ( ent->client->ps.classWeaponTime + g_engineerChargeTime.integer < level.time ) {
			ent->client->ps.classWeaponTime = level.time - g_engineerChargeTime.integer;
		}
		ent->client->ps.classWeaponTime += 150;

		if ( ent->client->ps.classWeaponTime > level.time ) {
			ent->client->ps.classWeaponTime = level.time;
			return;     // Out of "ammo"
		}

		if ( traceEnt->health >= 255 ) {
			traceEnt->s.frame = 0;

			if ( traceEnt->mg42BaseEnt > 0 ) {
				g_entities[ traceEnt->mg42BaseEnt ].health = MG42_MULTIPLAYER_HEALTH;
				g_entities[ traceEnt->mg42BaseEnt ].takedamage = qtrue;
				traceEnt->health = 0;
			} else {
				traceEnt->health = MG42_MULTIPLAYER_HEALTH;
			}

			if ( !g_deathmatch.integer ) {
				AddScore( ent, WOLF_REPAIR_BONUS ); // JPW NERVE props to the E for the fixin'
			}
			traceEnt->takedamage = qtrue;
			traceEnt->s.eFlags &= ~EF_SMOKING;

			trap_SendServerCommand( ent - g_entities, "cp \"You have repaired the MG42!\n\"" );
			// JPW NERVE sound effect to go with fixing MG42
			G_AddEvent( ent, EV_MG42_FIXED, 0 );
			// jpw
		} else {
			traceEnt->health += 3;
		}
	} else {
		trap_Trace( &tr, swMuzzleTrace, NULL, NULL, end, ent->s.number, MASK_SHOT );
		if ( tr.surfaceFlags & SURF_NOIMPACT ) {
			return;
		}
		//no contact
		if ( tr.fraction == 1.0f ) {
			return;
		}
		if ( tr.entityNum == ENTITYNUM_NONE || tr.entityNum == ENTITYNUM_WORLD ) {
			return;
		}
		traceEnt = &g_entities[ tr.entityNum ];

		if ( traceEnt->methodOfDeath == MOD_DYNAMITE ) {
			vec3_t mins, maxs;      // JPW NERVE
			int i,num,touch[MAX_GENTITIES],scored = 0, dynamiteDropTeam;;       // JPW NERVE

			// Not armed
			if ( traceEnt->s.teamNum >= 4 ) {
				// Opposing team cannot accidentally arm it
				if ( ( traceEnt->s.teamNum - 4 ) != ent->client->sess.sessionTeam ) {
					return;
				}

				trap_SendServerCommand( ent - g_entities, "cp \"Arming dynamite...\" 1" );

				// Give health until it is full, don't continue
				traceEnt->health += 7;
				if ( traceEnt->health >= 250 ) {
					traceEnt->health = 255;
				} else {
					return;
				}

				// Don't allow disarming for sec (so guy that WAS arming doesn't start disarming it!
				traceEnt->timestamp = level.time + 1000;
				traceEnt->health = 5;

				// set teamnum so we can check it for drop/defuse exploit
				traceEnt->s.teamNum = ent->client->sess.sessionTeam;
				// For dynamic light pulsing
				traceEnt->s.effect1Time = level.time;

				// ARM IT!
				trap_SendServerCommand( ent - g_entities, "cp \"Dynamite is now armed with a 30 second timer!\" 1" );
				traceEnt->nextthink = level.time + g_dynoFuseTime.integer;
				traceEnt->think = G_ExplodeMissile;

				// check if player is in trigger objective field
				// NERVE - SMF - made this the actual bounding box of dynamite instead of range, also must snap origin to line up properly
				if ( !g_deathmatch.integer ) {
					VectorCopy( traceEnt->r.currentOrigin, origin );
					SnapVector( origin );
					VectorAdd( origin, traceEnt->r.mins, mins );
					VectorAdd( origin, traceEnt->r.maxs, maxs );
					num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );
					VectorAdd( origin, traceEnt->r.mins, mins );
					VectorAdd( origin, traceEnt->r.maxs, maxs );

					for ( i = 0 ; i < num ; i++ ) {
						hit = &g_entities[touch[i]];

						if ( !( hit->r.contents & CONTENTS_TRIGGER ) ) {
							continue;
						}
						if ( !strcmp( hit->classname,"trigger_objective_info" ) ) {
							if ( !( hit->spawnflags & ( AXIS_OBJECTIVE | ALLIED_OBJECTIVE ) ) ) {
								continue;
							}

							te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_SOUND );
							// JPW NERVE
							// TTimo gcc: suggest explicit braces to avoid ambiguous `else'
							if ( ent->client != NULL ) {
								const char *Goalname = NULL;

								if ( ( ent->client->sess.sessionTeam == TEAM_BLUE ) && ( hit->spawnflags & AXIS_OBJECTIVE ) ) {
									te->s.eventParm = G_SoundIndex( "sound/multiplayer/allies/a-dynamite_planted.wav" );
									omnitrigger = qtrue;
								} else if ( ( ent->client->sess.sessionTeam == TEAM_RED ) && ( hit->spawnflags & ALLIED_OBJECTIVE ) ) {         // redundant but added for code clarity
									te->s.eventParm = G_SoundIndex( "sound/multiplayer/axis/g-dynamite_planted.wav" );
									omnitrigger = qtrue;
								}

								if ( omnitrigger ) {
									G_Script_ScriptEvent( hit, "dynamited", "" );
									hit->numPlanted += 1;
									// notify omni-bot framework of planted dynamite
									if ( hit->parent && hit->parent->track ) {
										Goalname = _GetEntityName( hit->parent );
									} else {
										Goalname = _GetEntityName( hit );
									}

									Bot_AddDynamiteGoal( traceEnt, traceEnt->s.teamNum, va( "%s_%i", Goalname, hit->numPlanted ) );
								}
							}

							if ( hit->spawnflags & AXIS_OBJECTIVE ) {
								te->s.teamNum = TEAM_RED;
								if ( ent->client->sess.sessionTeam == TEAM_BLUE ) { // transfer score info if this is a bomb scoring objective
									traceEnt->accuracy = hit->accuracy;
								}
							} else if ( hit->spawnflags & ALLIED_OBJECTIVE ) {
								te->s.teamNum = TEAM_BLUE;
								if ( ent->client->sess.sessionTeam == TEAM_RED ) { // ditto other team
									traceEnt->accuracy = hit->accuracy;
								}
							}
							te->r.svFlags |= SVF_BROADCAST;

							if ( ( ( hit->spawnflags & AXIS_OBJECTIVE ) && ( ent->client->sess.sessionTeam == TEAM_BLUE ) ) ||
								( ( hit->spawnflags & ALLIED_OBJECTIVE ) && ( ent->client->sess.sessionTeam == TEAM_RED ) ) ) {
									if ( hit->track ) {
										trap_SendServerCommand( -1, va( "cp \"%s\" 2", va( "Dynamite planted near %s!", hit->track ) ) );
									} else {
										trap_SendServerCommand( -1, va( "cp \"%s\" 2", va( "Dynamite planted near objective #%d!", hit->count ) ) );
									}
							}
							i = num;

							if ( ( !( hit->spawnflags & OBJECTIVE_DESTROYED ) ) &&
								te->s.teamNum && ( te->s.teamNum != ent->client->sess.sessionTeam ) ) {
									AddScore( traceEnt->parent, WOLF_DYNAMITE_PLANT ); // give drop score to guy who dropped it
									traceEnt->parent = ent; // give explode score to guy who armed it
							}
							// jpw
						}
					}
				}
			} else {
				if ( traceEnt->timestamp > level.time ) {
					return;
				}
				if ( traceEnt->health >= 248 ) { // have to do this so we don't score multiple times
					return;
				}
				dynamiteDropTeam = traceEnt->s.teamNum; // set this here since we wack traceent later but want teamnum for scoring

				traceEnt->health += 3;

				trap_SendServerCommand( ent - g_entities, "cp \"Defusing dynamite...\" 1" );

				if ( traceEnt->health >= 248 ) {
					traceEnt->health = 255;
					// Need some kind of event/announcement here

					Add_Ammo( ent, WP_DYNAMITE, 1, qtrue );

					traceEnt->think = G_FreeEntity;
					traceEnt->nextthink = level.time + FRAMETIME;

					// JPW NERVE -- more swipeage -- check if player is in trigger objective field
					if ( !g_deathmatch.integer ) {
						VectorSubtract( ent->client->ps.origin, range, mins );
						VectorAdd( ent->client->ps.origin, range, maxs );
						num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );
						VectorAdd( ent->client->ps.origin, ent->r.mins, mins );
						VectorAdd( ent->client->ps.origin, ent->r.maxs, maxs );

						for ( i = 0 ; i < num ; i++ ) {
							hit = &g_entities[touch[i]];

							if ( !( hit->r.contents & CONTENTS_TRIGGER ) ) {
								continue;
							}
							if ( !strcmp( hit->classname,"trigger_objective_info" ) ) {
								if ( !( hit->spawnflags & ( AXIS_OBJECTIVE | ALLIED_OBJECTIVE ) ) ) {
									continue;
								}

								G_Script_ScriptEvent( hit, "defused", "" );

								traceEnt = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_SOUND );
								traceEnt->r.svFlags |= SVF_BROADCAST;
								if ( ent->client->sess.sessionTeam == TEAM_RED && dynamiteDropTeam != TEAM_RED ) {
									if ( ( hit->spawnflags & AXIS_OBJECTIVE ) && ( !scored ) ) {
										AddScore( ent,WOLF_DYNAMITE_DIFFUSE ); // FIXME add team info to *dynamite* so we don't get points for diffusing own team dynamite
										scored++;
										hit->spawnflags &= ~OBJECTIVE_DESTROYED; // "re-activate" objective since it wasn't destroyed.  kludgy, I know; see G_ExplodeMissile for the other half
									}
									trap_SendServerCommand( -1, "cp \"Axis engineer disarmed the Dynamite!\n\"2" );
									traceEnt->s.eventParm = G_SoundIndex( "sound/multiplayer/axis/g-dynamite_defused.wav" );
									traceEnt->s.teamNum = TEAM_RED;
								} else if ( dynamiteDropTeam != TEAM_BLUE ) { // TEAM_BLUE
									if ( ( hit->spawnflags & ALLIED_OBJECTIVE ) && ( !scored ) ) {
										AddScore( ent,WOLF_DYNAMITE_DIFFUSE );
										scored++;
										hit->spawnflags &= ~OBJECTIVE_DESTROYED; // "re-activate" objective since it wasn't destroyed
									}
									trap_SendServerCommand( -1, "cp \"Allied engineer disarmed the Dynamite!\n\"2" );
									traceEnt->s.eventParm = G_SoundIndex( "sound/multiplayer/allies/a-dynamite_defused.wav" );
									traceEnt->s.teamNum = TEAM_BLUE;
								}
							}
						}
					}
				}
				// jpw
			}
		}
	}
}

// JPW NERVE -- launch airstrike as line of bombs mostly-perpendicular to line of grenade travel
// (close air support should *always* drop parallel to friendly lines, tho accidents do happen)
extern void G_ExplodeMissile( gentity_t *ent );

void G_AirStrikeExplode( gentity_t *self ) {
	self->r.svFlags &= ~SVF_NOCLIENT;
	self->r.svFlags |= SVF_BROADCAST;

	self->think = G_ExplodeMissile;
	self->nextthink = level.time + 50;
}

#define NUMBOMBS 10
#define BOMBSPREAD 150
extern void G_SayTo( gentity_t *ent, gentity_t *other, int mode, int color, const char *name, const char *message, qboolean localize );
void weapon_callAirStrike( gentity_t *ent ) {
	int i;
	vec3_t bombaxis, lookaxis, pos, bomboffset, fallaxis, temp;
	gentity_t *bomb,*te;
	trace_t tr;
	float traceheight, bottomtraceheight;

	if ( !ent || !ent->parent ) {
		return;
	}

	VectorCopy( ent->s.pos.trBase,bomboffset );
	bomboffset[2] += 4096;

	// cancel the airstrike if FF off and player joined spec
	if ( !g_friendlyFire.integer && ent->parent->client && ent->parent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
		ent->splashDamage = 0; // no damage
		ent->think = G_ExplodeMissile;
		ent->nextthink = level.time + crandom() * 50;
		return; // do nothing, don't hurt anyone
	}

	// turn off smoke grenade
	ent->think = G_ExplodeMissile;
	ent->nextthink = level.time + 950 + NUMBOMBS * 100 + crandom() * 50; // 3000 offset is for aircraft flyby

	trap_Trace( &tr, ent->s.pos.trBase, NULL, NULL, bomboffset, ent->s.number, MASK_SHOT );
	if ( ( tr.fraction < 1.0 ) && ( !( tr.surfaceFlags & SURF_NOIMPACT ) ) ) { //SURF_SKY)) ) { // JPW NERVE changed for trenchtoast foggie prollem
		G_SayTo( ent->parent, ent->parent, 2, COLOR_YELLOW, "Pilot: ", "Aborting, can't see target.", qtrue );

		if ( ent->parent->client && ent->parent->client->sess.sessionTeam == TEAM_BLUE ) {
			te = G_TempEntity( ent->parent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/multiplayer/allies/a-aborting.wav" );
			te->s.teamNum = ent->parent->s.clientNum;
		} else {
			te = G_TempEntity( ent->parent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/multiplayer/axis/g-aborting.wav" );
			te->s.teamNum = ent->parent->s.clientNum;
		}

		if ( ( tr.entityNum >= 0 ) && ( tr.entityNum < MAX_CLIENTS ) && ( g_entities[tr.entityNum].inuse ) ) {
			trap_SendServerCommand( -1, va( "print \"%s ^7blocked an airstrike\n\"", g_entities[tr.entityNum].client->pers.netname ) );
			trap_SendServerCommand( tr.entityNum, va( "cp \"You blocked an airstrike!\n\"" ) );
		}

		return;
	}

	if ( ent->parent->client && ent->parent->client->sess.sessionTeam == TEAM_BLUE ) {
		te = G_TempEntity( ent->parent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
		te->s.eventParm = G_SoundIndex( "sound/multiplayer/allies/a-affirmative_omw.wav" );
		te->s.teamNum = ent->parent->s.clientNum;
	} else {
		te = G_TempEntity( ent->parent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
		te->s.eventParm = G_SoundIndex( "sound/multiplayer/axis/g-affirmative_omw.wav" );
		te->s.teamNum = ent->parent->s.clientNum;
	}

	VectorCopy( tr.endpos, bomboffset );
	traceheight = bomboffset[2];
	bottomtraceheight = traceheight - 8192;

	VectorSubtract( ent->s.pos.trBase,ent->parent->client->ps.origin,lookaxis );
	lookaxis[2] = 0;
	VectorNormalize( lookaxis );
	pos[0] = 0;
	pos[1] = 0;
	pos[2] = crandom(); // generate either up or down vector,
	VectorNormalize( pos ); // which adds randomness to pass direction below
	RotatePointAroundVector( bombaxis,pos,lookaxis,90 + crandom() * 30 ); // munge the axis line a bit so it's not totally perpendicular
	VectorNormalize( bombaxis );

	VectorCopy( bombaxis,pos );
	VectorScale( pos,(float)( -0.5f * BOMBSPREAD * NUMBOMBS ),pos );
	VectorAdd( ent->s.pos.trBase, pos, pos ); // first bomb position
	VectorScale( bombaxis,BOMBSPREAD,bombaxis ); // bomb drop direction offset

	for ( i = 0; i < NUMBOMBS; i++ ) {
		bomb = G_Spawn();
		bomb->nextthink = level.time + i * 100 + crandom() * 50 + 1000; // 1000 for aircraft flyby, other term for tumble stagger
		bomb->think = G_AirStrikeExplode;
		bomb->s.eType       = ET_MISSILE;
		bomb->r.svFlags     = SVF_USE_CURRENT_ORIGIN | SVF_NOCLIENT;
		bomb->s.weapon      = WP_ARTY; // might wanna change this
		bomb->r.ownerNum    = ent->s.number;
		bomb->parent        = ent->parent;
		bomb->damage        = 400; // maybe should un-hard-code these?
		bomb->splashDamage  = 400;
		bomb->classname             = "air strike";
		bomb->aiName		    = "air strike";
		bomb->splashRadius          = 400;
		bomb->methodOfDeath         = MOD_AIRSTRIKE;
		bomb->splashMethodOfDeath   = MOD_AIRSTRIKE;
		bomb->clipmask = MASK_MISSILESHOT;
		bomb->s.pos.trType = TR_STATIONARY; // was TR_GRAVITY,  might wanna go back to this and drop from height
		bomboffset[0] = crandom() * 0.5 * BOMBSPREAD;
		bomboffset[1] = crandom() * 0.5 * BOMBSPREAD;
		bomboffset[2] = 0;
		VectorAdd( pos,bomboffset,bomb->s.pos.trBase );

		VectorCopy( bomb->s.pos.trBase,bomboffset ); // make sure bombs fall "on top of" nonuniform scenery
		bomboffset[2] = traceheight;

		VectorCopy( bomboffset, fallaxis );
		fallaxis[2] = bottomtraceheight;

		trap_Trace( &tr, bomboffset, NULL, NULL, fallaxis, ent->s.number, MASK_SHOT );
		if ( tr.fraction != 1.0 ) {
			VectorCopy( tr.endpos,bomb->s.pos.trBase );
		}

		VectorClear( bomb->s.pos.trDelta );

		// Snap origin!
		VectorCopy( bomb->s.pos.trBase, temp );
		temp[2] += 2.f;
		SnapVectorTowards( bomb->s.pos.trBase, temp );          // save net bandwidth

		VectorCopy( bomb->s.pos.trBase, bomb->r.currentOrigin );

		// move pos for next bomb
		VectorAdd( pos,bombaxis,pos );
	}
}

void weapon_smokeGrenade( gentity_t *ent ) {
	ent->s.eFlags |= EF_SMOKINGBLACK;
	ent->s.loopSound = G_SoundIndex( "sound/world/steam.wav" );

	ent->nextthink = level.time + 8000;
	ent->think = G_ExplodeMissile;
}

// JPW NERVE -- sound effect for spotter round, had to do this as half-second bomb warning

void artilleryThink_real( gentity_t *ent ) {
	ent->freeAfterEvent = qtrue;
	trap_LinkEntity( ent );
	G_AddEvent( ent, EV_GENERAL_SOUND, G_SoundIndex( "sound/multiplayer/artillery_01.wav" ) );
}
void artilleryThink( gentity_t *ent ) {
	ent->think = artilleryThink_real;
	ent->nextthink = level.time + 100;

	ent->r.svFlags = SVF_USE_CURRENT_ORIGIN | SVF_BROADCAST;
}

// JPW NERVE -- makes smoke disappear after a bit (just unregisters stuff)
void artilleryGoAway( gentity_t *ent ) {
	ent->freeAfterEvent = qtrue;
	trap_LinkEntity( ent );
}

// JPW NERVE -- generates some smoke debris
void artillerySpotterThink( gentity_t *ent ) {
	gentity_t *bomb;
	vec3_t tmpdir;
	int i;
	ent->think = G_ExplodeMissile;
	ent->nextthink = level.time + 1;
	SnapVector( ent->s.pos.trBase );

	for ( i = 0; i < 7; i++ ) {
		bomb = G_Spawn();
		bomb->s.eType       = ET_MISSILE;
		bomb->r.svFlags     = SVF_USE_CURRENT_ORIGIN;
		bomb->r.ownerNum    = ent->s.number;
		bomb->parent        = ent;
		bomb->nextthink = level.time + 1000 + random() * 300;
		bomb->classname = "WP"; // WP == White Phosphorous, so we can check for bounce noise in grenade bounce routine
		bomb->damage        = 000; // maybe should un-hard-code these?
		bomb->splashDamage  = 000;
		bomb->splashRadius  = 000;
		bomb->s.weapon  = WP_SMOKETRAIL;
		bomb->think = artilleryGoAway;
		bomb->s.eFlags |= EF_BOUNCE;
		bomb->clipmask = MASK_MISSILESHOT;
		bomb->s.pos.trType = TR_GRAVITY; // was TR_GRAVITY,  might wanna go back to this and drop from height
		bomb->s.pos.trTime = level.time;        // move a bit on the very first frame
		bomb->s.otherEntityNum2 = ent->s.otherEntityNum2;
		VectorCopy( ent->s.pos.trBase,bomb->s.pos.trBase );
		tmpdir[0] = crandom();
		tmpdir[1] = crandom();
		tmpdir[2] = 1;
		VectorNormalize( tmpdir );
		tmpdir[2] = 1; // extra up
		VectorScale( tmpdir,500 + random() * 500,tmpdir );
		VectorCopy( tmpdir,bomb->s.pos.trDelta );
		SnapVector( bomb->s.pos.trDelta );          // save net bandwidth
		VectorCopy( ent->s.pos.trBase,bomb->s.pos.trBase );
		VectorCopy( ent->s.pos.trBase,bomb->r.currentOrigin );
	}
}

// JPW NERVE
/*
==================
Weapon_Artillery
==================
*/
void Weapon_Artillery( gentity_t *ent ) {
	if ( ent->client->ps.stats[STAT_PLAYER_CLASS] != PC_LT ) {
		//G_Printf( "not a lieutenant, you can't shoot this!\n" );
		return;
	}
	if ( level.time - ent->client->ps.classWeaponTime > g_LTChargeTime.integer ) {
		trace_t trace;
		int i = 0;
		vec3_t muzzlePoint,end,bomboffset,pos,fallaxis;
		float traceheight, bottomtraceheight;
		gentity_t *bomb,*bomb2,*te;

		AngleVectors( ent->client->ps.viewangles, swForward, swRight, swUp );

		VectorCopy( ent->r.currentOrigin, muzzlePoint );
		muzzlePoint[2] += ent->client->ps.viewheight;

		VectorMA( muzzlePoint, 8192, swForward, end );
		trap_Trace( &trace, muzzlePoint, NULL, NULL, end, ent->s.number, MASK_SHOT );

		if ( trace.surfaceFlags & SURF_NOIMPACT ) {
			return;
		}

		VectorCopy( trace.endpos,pos );
		VectorCopy( pos,bomboffset );
		bomboffset[2] += 4096;

		trap_Trace( &trace, pos, NULL, NULL, bomboffset, ent->s.number, MASK_SHOT );
		if ( ( trace.fraction < 1.0 ) && ( !( trace.surfaceFlags & SURF_NOIMPACT ) ) ) { // JPW NERVE was SURF_SKY)) ) {
			G_SayTo( ent, ent, 2, COLOR_YELLOW, "Fire Mission: ", "Aborting, can't see target.", qtrue );

			if ( ent->client->sess.sessionTeam == TEAM_BLUE ) {
				te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
				te->s.eventParm = G_SoundIndex( "sound/multiplayer/allies/a-art_abort.wav" );
				te->s.teamNum = ent->s.clientNum;
			} else {
				te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
				te->s.eventParm = G_SoundIndex( "sound/multiplayer/axis/g-art_abort.wav" );
				te->s.teamNum = ent->s.clientNum;
			}
			return;
		}
		G_SayTo( ent, ent, 2, COLOR_YELLOW, "Fire Mission: ", "Firing for effect!", qtrue );

		if ( ent->client->sess.sessionTeam == TEAM_BLUE ) {
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/multiplayer/allies/a-firing.wav" );
			te->s.teamNum = ent->s.clientNum;
		} else {
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/multiplayer/axis/g-firing.wav" );
			te->s.teamNum = ent->s.clientNum;
		}

		VectorCopy( trace.endpos, bomboffset );
		traceheight = bomboffset[2];
		bottomtraceheight = traceheight - 8192;

		// "spotter" round (i == 0)
		// i == 1->4 is regular explosives
		for ( i = 0; i < 5; i++ ) {
			bomb = G_Spawn();
			bomb->think = G_AirStrikeExplode;
			bomb->s.eType       = ET_MISSILE;
			bomb->r.svFlags     = SVF_USE_CURRENT_ORIGIN | SVF_NOCLIENT;
			bomb->s.weapon      = WP_ARTY; // might wanna change this
			bomb->r.ownerNum    = ent->s.number;
			bomb->parent        = ent;
			bomb->aiName	    = "artillery";

			if ( i == 0 ) {
				bomb->nextthink = level.time + 5000;
				bomb->r.svFlags     = SVF_USE_CURRENT_ORIGIN | SVF_BROADCAST;
				bomb->classname = "props_explosion"; // was "air strike"
				bomb->damage        = 0; // maybe should un-hard-code these?
				bomb->splashDamage  = 90;
				bomb->splashRadius  = 50;
				//		bomb->s.weapon	= WP_SMOKE_GRENADE;
				// TTimo ambiguous else
				if ( ent->client != NULL ) { // set team color on smoke
					if ( ent->client->sess.sessionTeam == TEAM_RED ) { // store team so we can generate red or blue smoke
						bomb->s.otherEntityNum2 = 1;
					} else {
						bomb->s.otherEntityNum2 = 0;
					}
				}
				bomb->think = artillerySpotterThink;
			} else {
				bomb->nextthink = level.time + 8950 + 2000 * i + crandom() * 800;
				bomb->classname = "air strike";
				bomb->damage        = 0;
				bomb->splashDamage  = 400;
				bomb->splashRadius  = 400;
			}
			bomb->aiName		    = "artillery";
			bomb->methodOfDeath         = MOD_AIRSTRIKE;
			bomb->splashMethodOfDeath   = MOD_AIRSTRIKE;
			bomb->clipmask = MASK_MISSILESHOT;
			bomb->s.pos.trType = TR_STATIONARY; // was TR_GRAVITY,  might wanna go back to this and drop from height
			bomb->s.pos.trTime = level.time;        // move a bit on the very first frame
			if ( i ) { // spotter round is always dead on (OK, unrealistic but more fun)
				bomboffset[0] = crandom() * 250;
				bomboffset[1] = crandom() * 250;
			} else {
				bomboffset[0] = crandom() * 50; // was 0; changed per id request to prevent spotter round assassinations
				bomboffset[1] = crandom() * 50; // was 0;
			}
			bomboffset[2] = 0;
			VectorAdd( pos,bomboffset,bomb->s.pos.trBase );

			VectorCopy( bomb->s.pos.trBase,bomboffset ); // make sure bombs fall "on top of" nonuniform scenery
			bomboffset[2] = traceheight;

			VectorCopy( bomboffset, fallaxis );
			fallaxis[2] = bottomtraceheight;

			trap_Trace( &trace, bomboffset, NULL, NULL, fallaxis, ent->s.number, MASK_SHOT );
			if ( trace.fraction != 1.0 ) {
				VectorCopy( trace.endpos,bomb->s.pos.trBase );
			}

			bomb->s.pos.trDelta[0] = 0; // might need to change this
			bomb->s.pos.trDelta[1] = 0;
			bomb->s.pos.trDelta[2] = 0;
			SnapVector( bomb->s.pos.trDelta );          // save net bandwidth
			VectorCopy( bomb->s.pos.trBase, bomb->r.currentOrigin );

			// build arty falling sound effect in front of bomb drop
			bomb2 = G_Spawn();
			bomb2->think = artilleryThink;
			bomb2->s.eType  = ET_MISSILE;
			bomb2->r.svFlags    = SVF_USE_CURRENT_ORIGIN | SVF_NOCLIENT;
			bomb2->r.ownerNum   = ent->s.number;
			bomb2->parent       = ent;
			bomb2->damage       = 0;
			bomb2->nextthink = bomb->nextthink - 600;
			bomb2->classname = "air strike";
			bomb2->aiName	 = "artillery";
			bomb2->clipmask = MASK_MISSILESHOT;
			bomb2->s.pos.trType = TR_STATIONARY; // was TR_GRAVITY,  might wanna go back to this and drop from height
			bomb2->s.pos.trTime = level.time;       // move a bit on the very first frame
			VectorCopy( bomb->s.pos.trBase,bomb2->s.pos.trBase );
			VectorCopy( bomb->s.pos.trDelta,bomb2->s.pos.trDelta );
			VectorCopy( bomb->s.pos.trBase,bomb2->r.currentOrigin );
		}
		ent->client->ps.classWeaponTime = level.time;
	}

	// Omni-bot - Send a fire event.
	Bot_Event_FireWeapon( ent - g_entities, Bot_WeaponGameToBot( WP_ARTY ), 0 );
}

/*
======================================================================

MACHINEGUN

======================================================================
*/

/*
======================
SnapVectorTowards

Round a vector to integers for more efficient network
transmission, but make sure that it rounds towards a given point
rather than blindly truncating.  This prevents it from truncating
into a wall.
======================
*/

// (SA) modified so it doesn't have trouble with negative locations (quadrant problems)
//			(this was causing some problems with bullet marks appearing since snapping
//			too far off the target surface causes the the distance between the transmitted impact
//			point and the actual hit surface larger than the mark radius.  (so nothing shows) )

void SnapVectorTowards( vec3_t v, vec3_t to ) {
	int i;

	for ( i = 0 ; i < 3 ; i++ ) {
		if ( to[i] <= v[i] ) {
			v[i] = floor( v[i] );
		} else {
			v[i] = ceil( v[i] );
		}
	}
}

// JPW
// mechanism allows different weapon damage for single/multiplayer; we want "balanced" weapons
// in multiplayer but don't want to alter the existing single-player damage items that have already
// been changed
//
// KLUDGE/FIXME: also modded #defines below to become macros that call this fn for minimal impact elsewhere
//
int G_GetWeaponDamage( int weapon ) {
	// multiplayer damage
	switch ( weapon ) {
	case WP_LUGER:
	case WP_SILENCER:
	case WP_COLT: return 18;
	case WP_AKIMBO: return 18;      //----(SA)	added
	case WP_VENOM_FULL: return 10;
	case WP_VENOM: return g_venomDamage.integer;
	case WP_MP40: return 14;
	case WP_THOMPSON: return 18;
	case WP_STEN: return 14;
	case WP_FG42SCOPE:
	case WP_FG42: return 15;
	case WP_BAR:
	case WP_BAR2: return 12;        //----(SA)	added
	case WP_MAUSER: return 80;     // was 25 JPW
	case WP_GARAND: return 75;     // was 25 JPW
	case WP_SNIPERRIFLE: return 80;
	case WP_SNOOPERSCOPE: return 75;
	case WP_NONE: return 0;
	case WP_KNIFE:
	case WP_KNIFE2: return 10;
	case WP_SMOKE_GRENADE: return 140;     // just enough to kill somebody standing on it
	case WP_GRENADE_LAUNCHER: return 250;
	case WP_GRENADE_PINEAPPLE: return 250;
	case WP_DYNAMITE:
	case WP_DYNAMITE2: return 600;
	case WP_ROCKET_LAUNCHER:
	case WP_PANZERFAUST: return 400;
	case WP_MORTAR: return 250;
	case WP_FLAMETHROWER: return 1;
	case WP_TESLA:
	case WP_SPEARGUN:
	case WP_SPEARGUN_CO2:
	case WP_CROSS:
	case WP_GAUNTLET:
	case WP_SNIPER:
	default:    return 1;
	}
}
// JPW - this chunk appears to not be used, right?
/*
#define MACHINEGUN_SPREAD	200
#define	MACHINEGUN_DAMAGE	G_GetWeaponDamage(WP_MACHINEGUN) // JPW
#define	MACHINEGUN_TEAM_DAMAGE	G_GetWeaponDamage(WP_MACHINEGUN) // JPW		// wimpier MG in teamplay
*/
// jpw

// RF, wrote this so we can dynamically switch between old and new values while testing g_userAim
float G_GetWeaponSpread( int weapon ) {
	switch ( weapon ) {
	case WP_LUGER: return 600;
	case WP_SILENCER: return 900;
	case WP_COLT: return 800;
	case WP_AKIMBO: return 800;     //----(SA)added
	case WP_VENOM: return (float)g_venomSpread.integer;
	case WP_MP40: return 400;
	case WP_FG42SCOPE:
	case WP_FG42:   return 500;
	case WP_BAR:
	case WP_BAR2:   return 500;
	case WP_THOMPSON: return 600;
	case WP_STEN: return 200;
	case WP_MAUSER:
		if ( g_instaGib.integer ) {
			return 100;
		} else {
			return 2000;
		}
	case WP_GARAND: return 600;
	case WP_SNIPERRIFLE: return 700;     // was 300
	case WP_SNOOPERSCOPE: return 700;
	}

	G_Printf( "shouldn't ever get here (weapon %d)\n",weapon );
	// jpw
	return 0;   // shouldn't get here
}

#define LUGER_SPREAD    G_GetWeaponSpread( WP_LUGER )
#define LUGER_DAMAGE    G_GetWeaponDamage( WP_LUGER ) // JPW
#define SILENCER_SPREAD G_GetWeaponSpread( WP_SILENCER )
#define COLT_SPREAD     G_GetWeaponSpread( WP_COLT )
#define COLT_DAMAGE     G_GetWeaponDamage( WP_COLT ) // JPW
#define VENOM_SPREAD    G_GetWeaponSpread( WP_VENOM )
#define VENOM_DAMAGE    G_GetWeaponDamage( WP_VENOM ) // JPW
#define MP40_SPREAD     G_GetWeaponSpread( WP_MP40 )
#define MP40_DAMAGE     G_GetWeaponDamage( WP_MP40 ) // JPW
#define THOMPSON_SPREAD G_GetWeaponSpread( WP_THOMPSON )
#define THOMPSON_DAMAGE G_GetWeaponDamage( WP_THOMPSON ) // JPW
#define STEN_SPREAD     G_GetWeaponSpread( WP_STEN )
#define STEN_DAMAGE     G_GetWeaponDamage( WP_STEN ) // JPW
#define MAUSER_SPREAD   G_GetWeaponSpread( WP_MAUSER )
#define MAUSER_DAMAGE   G_GetWeaponDamage( WP_MAUSER ) // JPW
#define SNIPER_SPREAD   G_GetWeaponSpread( WP_SNIPERRIFLE )
#define SNIPER_DAMAGE   G_GetWeaponDamage( WP_SNIPERRIFLE ) // JPW

/*
==============
EmitterCheck
see if a new particle emitter should be created at the bullet impact point
==============
*/
void EmitterCheck( gentity_t *ent, gentity_t *attacker, trace_t *tr ) {
	gentity_t *tent;
	vec3_t origin;

	VectorCopy( tr->endpos, origin );
	SnapVectorTowards( tr->endpos, attacker->s.origin );

	if ( Q_stricmp( ent->classname, "func_leaky" ) == 0 ) {
		tent = G_TempEntity( origin, EV_EMITTER );
		VectorCopy( origin, tent->s.origin );
		tent->s.time = 1234;
		tent->s.density = 9876;
		VectorCopy( tr->plane.normal, tent->s.origin2 );
	}
}

/*
==============
Bullet_Endpos
find target end position for bullet trace based on entities weapon and accuracy
==============
*/
void Bullet_Endpos( gentity_t *ent, float spread, vec3_t *end ) {
	float r, u;
	qboolean randSpread = qtrue;
	int dist = 8192;

	r = crandom() * spread;
	u = crandom() * spread;

	if ( ent->s.weapon == WP_SNOOPERSCOPE || ent->s.weapon == WP_SNIPERRIFLE ) {
		// aim dir already accounted for sway of scoped weapons in CalcMuzzlePoints()
		dist *= 2;
		randSpread = qfalse;
	}

	VectorMA( swMuzzleTrace, dist, swForward, *end );

	if ( randSpread ) {
		VectorMA( *end, r, swRight, *end );
		VectorMA( *end, u, swUp, *end );
	}
}

/*
==============
Bullet_Fire
==============
*/
void Bullet_Fire( gentity_t *ent, float spread, int damage ) {
	vec3_t end;

	Bullet_Endpos(ent, spread, &end);

	if ( g_antilag.integer ) {
		G_HistoricalTraceBegin( ent );
	}

	Bullet_Fire_Extended( ent, ent, swMuzzleTrace, end, spread, damage );

	if ( g_antilag.integer ) {
		G_HistoricalTraceEnd( ent );
	}

	//stats
	ent->client->pers.acc_shots++;
}

/*
==============
Bullet_Fire_Extended
A modified Bullet_Fire with more parameters.
The original Bullet_Fire still passes through here and functions as it always has.

uses for this include shooting through entities (windows, doors, other players, etc.) and reflecting bullets
==============
*/
void Bullet_Fire_Extended( gentity_t *source, gentity_t *attacker, vec3_t start, vec3_t end, float spread, int damage ) {
	trace_t tr;
	gentity_t   *tent;
	gentity_t   *traceEnt;

	//G_HistoricalTrace( source, &tr, start, NULL, NULL, end, source->s.number, MASK_SHOT );
	G_Trace(source, &tr, start, NULL, NULL, end, source->s.number, MASK_SHOT);

	// bullet debugging using Q3A's railtrail
	if ( g_debugBullets.integer & 1 ) {
		tent = G_TempEntity( start, EV_RAILTRAIL );
		VectorCopy( tr.endpos, tent->s.origin2 );
		tent->s.otherEntityNum2 = attacker->s.number;
	}

	traceEnt = &g_entities[ tr.entityNum ];

	EmitterCheck( traceEnt, attacker, &tr );

	// snap the endpos to integers, but nudged towards the line
	SnapVectorTowards( tr.endpos, start );

	// send bullet impact
	if ( traceEnt->takedamage && traceEnt->client ) {
		tent = G_TempEntity( tr.endpos, EV_BULLET_HIT_FLESH );
		tent->s.eventParm = traceEnt->s.number;
		if ( LogAccuracyHit( traceEnt, attacker ) ) {
			attacker->client->ps.persistant[PERS_ACCURACY_HITS]++;
			if ( traceEnt->client->ps.stats[STAT_HEALTH] > 0 ) {
				attacker->client->pers.acc_hits++;
			} else
			{
				//don't penalize for gibbing
				attacker->client->pers.acc_shots--;
			}
		}

		//----(SA)	added
		if ( g_debugBullets.integer >= 2 ) {   // show hit player bb
			gentity_t *bboxEnt;
			vec3_t b1, b2;
			VectorCopy( traceEnt->r.currentOrigin, b1 );
			VectorCopy( traceEnt->r.currentOrigin, b2 );
			VectorAdd( b1, traceEnt->r.mins, b1 );
			VectorAdd( b2, traceEnt->r.maxs, b2 );
			bboxEnt = G_TempEntity( b1, EV_RAILTRAIL );
			VectorCopy( b2, bboxEnt->s.origin2 );
			bboxEnt->s.dmgFlags = 1;    // ("type")
		}
		//----(SA)	end
	} else if ( traceEnt->takedamage && traceEnt->s.eType == ET_BAT ) {
		tent = G_TempEntity( tr.endpos, EV_BULLET_HIT_FLESH );
		tent->s.eventParm = traceEnt->s.number;
	} else {
		trace_t tr2;
		// Ridah, bullet impact should reflect off surface
		vec3_t reflect;
		float dot;

		if ( g_debugBullets.integer <= -2 ) {  // show hit thing bb
			gentity_t *bboxEnt;
			vec3_t b1, b2;
			VectorCopy( traceEnt->r.currentOrigin, b1 );
			VectorCopy( traceEnt->r.currentOrigin, b2 );
			VectorAdd( b1, traceEnt->r.mins, b1 );
			VectorAdd( b2, traceEnt->r.maxs, b2 );
			bboxEnt = G_TempEntity( b1, EV_RAILTRAIL );
			VectorCopy( b2, bboxEnt->s.origin2 );
			bboxEnt->s.dmgFlags = 1;    // ("type")
		}

		tent = G_TempEntity( tr.endpos, EV_BULLET_HIT_WALL );

		G_Trace(source, &tr2, start, NULL, NULL, end, source->s.number, MASK_WATER | MASK_SHOT);

		if((tr.entityNum != tr2.entityNum && tr2.fraction != 1)) {
			vec3_t v;

			VectorSubtract( tr.endpos, start, v );

			tent->s.origin2[0] = (8192 * tr2.fraction) / VectorLength( v );
		} else {
			tent->s.origin2[0] = 0;
		}

		dot = DotProduct( swForward, tr.plane.normal );
		VectorMA( swForward, -2 * dot, tr.plane.normal, reflect );
		VectorNormalize( reflect );

		tent->s.eventParm = DirToByte( reflect );

		if ( traceEnt->flags & FL_DEFENSE_GUARD ) {
			tent->s.otherEntityNum2 = traceEnt->s.number;   // force sparks
		} else {
			tent->s.otherEntityNum2 = ENTITYNUM_NONE;
		}
		// done.
	}
	tent->s.otherEntityNum = attacker->s.number;

	if ( traceEnt->takedamage ) {
		qboolean reflectBool = qfalse;

		if ( reflectBool ) {
			vec3_t reflect_end;
			// reflect this bullet
			G_AddEvent( traceEnt, EV_GENERAL_SOUND, level.bulletRicochetSound );
			CalcMuzzlePoints( traceEnt, traceEnt->s.weapon );

			//----(SA)	modified to use extended version so attacker would pass through
			Bullet_Endpos( traceEnt, spread, &reflect_end );
			Bullet_Fire_Extended( traceEnt, attacker, swMuzzleTrace, reflect_end, spread, damage );
			//----(SA)	end
		} else {
			G_Damage( traceEnt, attacker, attacker, swForward, tr.endpos, damage, 0, ammoTable[attacker->s.weapon].mod );

			// allow bullets to "pass through" func_explosives if they break by taking another simultanious shot
			if ( Q_stricmp( traceEnt->classname, "func_explosive" ) == 0 ) {
				if ( traceEnt->health <= damage ) {
					// start new bullet at position this hit the bmodel and continue to the end position (ignoring shot-through bmodel in next trace)
					// spread = 0 as this is an extension of an already spread shot
					Bullet_Fire_Extended( traceEnt, attacker, tr.endpos, end, 0, damage );
				}
			}
		}
	}
}

/*
======================================================================

GRENADE LAUNCHER

700 has been the standard direction multiplier in fire_grenade()

======================================================================
*/
extern void G_ExplodeMissilePoisonGas( gentity_t *ent );

gentity_t *weapon_grenadelauncher_fire( gentity_t *ent, int grenType ) {
	gentity_t   *m, *te; // JPW NERVE
	trace_t tr;
	vec3_t viewpos;
	float upangle = 0, pitch;               //	start with level throwing and adjust based on angle
	vec3_t tosspos;
	qboolean underhand = qtrue;

	pitch = ent->s.apos.trBase[0];

	// JPW NERVE -- smoke grenades always overhand
	if ( pitch >= 0 ) {
		swForward[2] += 0.5f;
		// Used later in underhand boost
		pitch = 1.3f;
	} else {
		pitch = -pitch;
		pitch = min( pitch, 30 );
		pitch /= 30.f;
		pitch = 1 - pitch;
		swForward[2] += ( pitch * 0.5f );

		// Used later in underhand boost
		pitch *= 0.3f;
		pitch += 1.f;
	}

	VectorNormalizeFast( swForward );         //	make sure forward is normalized

	upangle = -( ent->s.apos.trBase[0] ); //	this will give between	-90 / 90
	upangle = min( upangle, 50 );
	upangle = max( upangle, -50 );        //	now clamped to	-50 / 50	(don't allow firing straight up/down)
	upangle = upangle / 100.0f;           //				   -0.5 / 0.5
	upangle += 0.5f;                    //				    0.0 / 1.0

	if ( upangle < .1 ) {
		upangle = .1;
	}

	// pineapples are not thrown as far as mashers
	if ( grenType == WP_GRENADE_LAUNCHER ) {
		upangle *= 900;
	} else if ( grenType == WP_GRENADE_PINEAPPLE ) {
		upangle *= 900;
	} else if ( grenType == WP_SMOKE_GRENADE ) {
		upangle *= 900;
	} else { // WP_DYNAMITE
		upangle *= 400;
	}

	VectorCopy( swMuzzleEffect, tosspos );

	if ( underhand ) {
		// move a little bit more away from the player (so underhand tosses don't get caught on nearby lips)
		VectorMA( swMuzzleEffect, 8, swForward, tosspos );
		tosspos[2] -= 8;    // lower origin for the underhand throw
		upangle *= pitch;
		SnapVector( tosspos );
	}

	VectorScale( swForward, upangle, swForward );

	// check for valid start spot (so you don't throw through or get stuck in a wall)
	VectorCopy( ent->s.pos.trBase, viewpos );
	viewpos[2] += ent->client->ps.viewheight;

	if ( grenType == WP_DYNAMITE ) {
		trap_Trace( &tr, viewpos, tv( -12.f, -12.f, 0.f ), tv( 12.f, 12.f, 20.f ), tosspos, ent->s.number, MASK_MISSILESHOT );
	} else {
		trap_Trace( &tr, viewpos, tv( -4.f, -4.f, 0.f ), tv( 4.f, 4.f, 6.f ), tosspos, ent->s.number, MASK_MISSILESHOT );
	}

	if ( tr.fraction < 1 ) {   // oops, bad launch spot
		VectorCopy( tr.endpos, tosspos );
		SnapVectorTowards( tosspos, viewpos );
	}

	m = fire_grenade( ent, tosspos, swForward, grenType );

	m->damage = 0;  // Ridah, grenade's don't explode on contact

	// JPW NERVE
	if ( grenType == WP_SMOKE_GRENADE ) {
		if ( ent->client->sess.sessionTeam == TEAM_RED ) { // store team so we can generate red or blue smoke
			m->s.otherEntityNum2 = 1;
		} else {
			m->s.otherEntityNum2 = 0;
		}

		if ( m->r.svFlags & SVF_SMOKEGRENADE ) {
			m->nextthink = level.time + 1000;
			m->think = weapon_smokeGrenade;
		} else {
			m->nextthink = level.time + 4000;
			m->think = weapon_callAirStrike;

			te = G_TempEntity( m->s.pos.trBase, EV_GLOBAL_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/multiplayer/airstrike_01.wav" );
			te->r.svFlags |= SVF_BROADCAST | SVF_USE_CURRENT_ORIGIN;
		}
	}
	// jpw

	// Ridah, return the grenade so we can do some prediction before deciding if we really want to throw it or not
	return m;
}

//----(SA)	modified this entire "venom" section
/*
============================================================================

VENOM GUN TRACING

============================================================================
*/
#define DEFAULT_VENOM_COUNT 10
#define DEFAULT_VENOM_SPREAD 20
#define DEFAULT_VENOM_DAMAGE 15

qboolean VenomPellet( vec3_t start, vec3_t end, gentity_t *ent ) {
	trace_t tr;
	gentity_t       *traceEnt;

	trap_Trace( &tr, start, NULL, NULL, end, ent->s.number, MASK_SHOT );
	traceEnt = &g_entities[ tr.entityNum ];

	// send bullet impact
	if (  tr.surfaceFlags & SURF_NOIMPACT ) {
		return qfalse;
	}

	if ( traceEnt->takedamage ) {
		G_Damage( traceEnt, ent, ent, swForward, tr.endpos, DEFAULT_VENOM_DAMAGE, 0, MOD_VENOM );
		if ( LogAccuracyHit( traceEnt, ent ) ) {
			return qtrue;
		}
	}
	return qfalse;
}

// this should match CG_VenomPattern
void VenomPattern( vec3_t origin, vec3_t origin2, int seed, gentity_t *ent ) {
	int i;
	float r, u;
	vec3_t end;
	vec3_t forward, right, up;
	qboolean hitClient = qfalse;

	// derive the right and up vectors from the forward vector, because
	// the client won't have any other information
	VectorNormalize2( origin2, forward );
	PerpendicularVector( right, forward );
	CrossProduct( forward, right, up );

	// generate the "random" spread pattern
	for ( i = 0 ; i < DEFAULT_VENOM_COUNT ; i++ ) {
		r = Q_crandom( &seed ) * DEFAULT_VENOM_SPREAD;
		u = Q_crandom( &seed ) * DEFAULT_VENOM_SPREAD;
		VectorMA( origin, 8192, forward, end );
		VectorMA( end, r, right, end );
		VectorMA( end, u, up, end );
		if ( VenomPellet( origin, end, ent ) && !hitClient ) {
			hitClient = qtrue;
			ent->client->ps.persistant[PERS_ACCURACY_HITS]++;
		}
	}
}

/*
======================================================================

NAILGUN

======================================================================
*/

void Weapon_Nailgun_Fire( gentity_t *ent ) {
	int count;

	for ( count = 0; count < NUM_NAILSHOTS; count++ ) {
		fire_nail( ent, swMuzzleEffect, swForward, swRight, swUp );
	}
}

/*
======================================================================

PROXIMITY MINE LAUNCHER

======================================================================
*/

void weapon_proxlauncher_fire( gentity_t *ent ) {
	// extra vertical velocity
	swForward[2] += 0.2f;
	VectorNormalize( swForward );
	fire_prox( ent, swMuzzleTrace, swForward );
}

/*
==============
weapon_venom_fire
==============
*/
void weapon_venom_fire( gentity_t *ent, qboolean fullmode, float aimSpreadScale ) {
	gentity_t       *tent;
	float spread =	VENOM_SPREAD;

	if ( fullmode ) {
		tent = G_TempEntity( swMuzzleTrace, EV_VENOMFULL );
	} else {
		tent = G_TempEntity( swMuzzleTrace, EV_VENOM );
	}

	VectorScale( swForward, 4096, tent->s.origin2 );
	SnapVector( tent->s.origin2 );
	tent->s.eventParm = rand() & 255;       // seed for spread pattern
	tent->s.otherEntityNum = ent->s.number;

	if ( (g_unlockWeapons.integer & WEAPONUNLOCK_HANDICAP) && (ent->client && ent->client->ps.stats[STAT_PLAYER_CLASS] != PC_SOLDIER) ) {
		spread *= 7;
	}

	if ( fullmode ) {
		VenomPattern( tent->s.pos.trBase, tent->s.origin2, tent->s.eventParm, ent );
	} else {
		Bullet_Fire( ent, spread * aimSpreadScale, VENOM_DAMAGE );
	}
}

/*
======================================================================

ROCKET

======================================================================
*/

void Weapon_RocketLauncher_Fire( gentity_t *ent ) {
	fire_rocket( ent, swMuzzleEffect, swForward );
}

gentity_t *Weapon_Panzerfaust_Fire( gentity_t *ent ) {
	gentity_t   *m = fire_rocket( ent, swMuzzleEffect, swForward );
	return m;
}

// TTimo - extracted G_FlameDamage to unify with Weapon_FlamethrowerFire usage
void G_BurnMeGood( gentity_t *self, gentity_t *body ) {
	// add the new damage
	body->flameQuota += 5;
	body->flameQuotaTime = level.time;

	// JPW NERVE -- yet another flamethrower damage model, trying to find a feels-good damage combo that isn't overpowered
	if ( body->lastBurnedFrameNumber != level.framenum ) {
		G_Damage( body,self->parent,self->parent,vec3_origin,self->r.currentOrigin,5,0,MOD_FLAMETHROWER );   // was 2 dmg in release ver, hit avg. 2.5 times per frame
		body->lastBurnedFrameNumber = level.framenum;
	}
	// jpw

	// make em burn
	if ( body->client && ( body->health <= 0 || body->flameQuota > 0 ) ) { // JPW NERVE was > FLAME_THRESHOLD
		if ( body->s.onFireEnd < level.time ) {
			body->s.onFireStart = level.time;
		}

		body->s.onFireEnd = level.time + FIRE_FLASH_TIME;
		body->flameBurnEnt = self->s.number;
		// add to playerState for client-side effect
		body->client->ps.onFireStart = level.time;
	}
}

// those are used in the cg_ traces calls
static vec3_t flameChunkMins = {-4, -4, -4};
static vec3_t flameChunkMaxs = { 4,  4,  4};

#define SQR_SIN_T 0.44 // ~ sqr(sin(20))

void Weapon_FlamethrowerFire( gentity_t *ent ) {
	vec3_t start;
	vec3_t trace_start;
	vec3_t trace_end;
	trace_t trace;

	VectorCopy( ent->r.currentOrigin, start );
	start[2] += ent->client->ps.viewheight;
	VectorCopy( start, trace_start );

	VectorMA( start, -8, swForward, start );
	VectorMA( start, 10, swRight, start );
	VectorMA( start, -6, swUp, start );

	// prevent flame thrower cheat, run & fire while aiming at the ground, don't get hurt
	// 72 total box height, 18 xy -> 77 trace radius (from view point towards the ground) is enough to cover the area around the feet
	VectorMA( trace_start, 77.0, swForward, trace_end );
	trap_Trace( &trace, trace_start, flameChunkMins, flameChunkMaxs, trace_end, ent->s.number, MASK_SHOT | MASK_WATER );
	if ( trace.fraction != 1.0 ) {
		// additional checks to filter out false positives
		if ( trace.endpos[2] > ( ent->r.currentOrigin[2] + ent->r.mins[2] - 8 ) && trace.endpos[2] < ent->r.currentOrigin[2] ) {
			// trigger in a 21 radius around origin
			trace_start[0] -= trace.endpos[0];
			trace_start[1] -= trace.endpos[1];
			if ( trace_start[0] * trace_start[0] + trace_start[1] * trace_start[1] < 441 ) {
				// set self in flames
				G_BurnMeGood( ent, ent );
			}
		}
	}

	fire_flamechunk( ent, start, swForward );
}

//======================================================================

/*
==============
AddLean
add leaning offset
==============
*/
void AddLean( gentity_t *ent, vec3_t point ) {
	if ( ent->client ) {
		if ( ent->client->ps.leanf ) {
			vec3_t right;
			AngleVectors( ent->client->ps.viewangles, NULL, right, NULL );
			VectorMA( point, ent->client->ps.leanf, right, point );
		}
	}
}

/*
===============
LogAccuracyHit
===============
*/
qboolean LogAccuracyHit( gentity_t *target, gentity_t *attacker ) {
	if ( !target->takedamage ) {
		return qfalse;
	}

	if ( !attacker ) {
		return qfalse;
	}

	if ( target == attacker ) {
		return qfalse;
	}

	if ( !target->client ) {
		return qfalse;
	}

	if ( !attacker->client ) {
		return qfalse;
	}

	if ( target->client->ps.stats[STAT_HEALTH] <= 0 ) {
		return qfalse;
	}

	if ( OnSameTeam( target, attacker ) ) {
		return qfalse;
	}

	return qtrue;
}

/*
===============
CalcMuzzlePoint

set muzzle location relative to pivoting eye
===============
*/
void CalcMuzzlePoint( gentity_t *ent, int weapon, vec3_t forward, vec3_t right, vec3_t up, vec3_t muzzlePoint ) {
	VectorCopy( ent->r.currentOrigin, muzzlePoint );
	muzzlePoint[2] += ent->client->ps.viewheight;

	// Ridah, offset for more realistic firing from actual gun position
	//----(SA) modified
	switch ( weapon )  // Ridah, changed this so I can predict weapons
	{
	case WP_PANZERFAUST:
		VectorMA( muzzlePoint,10,right,muzzlePoint );
		break;
	case WP_ROCKET_LAUNCHER:
		VectorMA( muzzlePoint, 14, right, muzzlePoint );        //----(SA)	new first person rl position
		break;
	case WP_DYNAMITE:
	case WP_DYNAMITE2:
	case WP_GRENADE_PINEAPPLE:
	case WP_GRENADE_LAUNCHER:
		VectorMA( muzzlePoint, 20, right, muzzlePoint );
		break;
	default:
		VectorMA( muzzlePoint, 6, right, muzzlePoint );
		VectorMA( muzzlePoint, -4, up, muzzlePoint );
		break;
	}

	// done.

	// snap to integer coordinates for more efficient network bandwidth usage
	SnapVector( muzzlePoint );
}

// Rafael - for activate
void CalcMuzzlePointForActivate( gentity_t *ent, vec3_t forward, vec3_t right, vec3_t up, vec3_t muzzlePoint ) {
	VectorCopy( ent->s.pos.trBase, muzzlePoint );
	muzzlePoint[2] += ent->client->ps.viewheight;

	AddLean( ent, muzzlePoint );

	// snap to integer coordinates for more efficient network bandwidth usage
	SnapVector( muzzlePoint );
}
// done.

// Ridah
void CalcMuzzlePoints( gentity_t *ent, int weapon ) {
	vec3_t viewang;

	VectorCopy( ent->client->ps.viewangles, viewang );

	if ( !( ent->r.svFlags & SVF_CASTAI ) && !( ent->r.svFlags & SVF_BOT ) ) {    // non ai's take into account scoped weapon 'sway' (just another way aimspread is visualized/utilized)
		if ( weapon == WP_SNIPERRIFLE || weapon == WP_SNOOPERSCOPE ) {
			float spreadfrac = ent->client->currentAimSpreadScale;

			// rotate 'forward' vector by the sway
			float phase = level.time / 1000.0 * ZOOM_PITCH_FREQUENCY * M_PI * 2;
			viewang[PITCH] += ZOOM_PITCH_AMPLITUDE * sin( phase ) * ( spreadfrac + ZOOM_PITCH_MIN_AMPLITUDE );

			phase = level.time / 1000.0 * ZOOM_YAW_FREQUENCY * M_PI * 2;
			viewang[YAW] += ZOOM_YAW_AMPLITUDE * sin( phase ) * ( spreadfrac + ZOOM_YAW_MIN_AMPLITUDE );
		}
	}

	// set aiming directions
	AngleVectors( viewang, swForward, swRight, swUp );

	//----(SA)	modified the muzzle stuff so that weapons that need to fire down a perfect trace
	//			straight out of the camera (SP5, Mauser right now) can have that accuracy, but
	//			weapons that need an offset effect (bazooka/grenade/etc.) can still look like
	//			they came out of the weap.
	CalcMuzzlePointForActivate( ent, swForward, swRight, swUp, swMuzzleTrace );
	CalcMuzzlePoint( ent, weapon, swForward, swRight, swUp, swMuzzleEffect );
}

/*
===============
FireWeapon
===============
*/
void FireWeapon( gentity_t *ent ) {
	float aimSpreadScale;
	vec3_t viewang;  // JPW NERVE
	gentity_t *pFiredShot = 0; // Omni-bot To tell bots about projectiles
	qboolean callEvent = qtrue;

	// Rafael mg42
	if ( ent->client->ps.persistant[PERS_HWEAPON_USE] && ent->active ) {
		return;
	}

	// Ridah, need to call this for AI prediction also
	CalcMuzzlePoints( ent, ent->s.weapon );

	if ( g_userAim.integer ) {
		aimSpreadScale = ent->client->currentAimSpreadScale;
		aimSpreadScale += 0.15f; // (SA) just adding a temp /maximum/ accuracy for player (this will be re-visited in greater detail :)
		if ( aimSpreadScale > 1 ) {
			aimSpreadScale = 1.0f;  // still cap at 1.0
		}
	} else {
		aimSpreadScale = 1.0;
	}

	// JPW NERVE -- EARLY OUT: if I'm in multiplayer and I have binocs, try to use artillery and then early return b4 switch statement
	if ( g_gametype.integer != GT_SINGLE_PLAYER ) {
		if ( ( ent->client->ps.eFlags & EF_ZOOMING ) && ( ent->client->ps.stats[STAT_KEYS] & ( 1 << INV_BINOCS ) ) &&
			( ent->s.weapon != WP_SNIPERRIFLE ) ) {
				if ( !( ent->client->ps.leanf ) ) {
					Weapon_Artillery( ent );
				}

				return;
		}
	}
	// jpw

	// JPW NERVE -- if jumping, make aim bite ass
	if ( ent->client->ps.groundEntityNum == ENTITYNUM_NONE ) {
		aimSpreadScale = 2.0f;
	}
	// jpw

	// fire the specific weapon
	switch ( ent->s.weapon ) {
	case WP_KNIFE:
	case WP_KNIFE2:
		Weapon_Knife( ent );
		break;
		// NERVE - SMF
	case WP_MEDKIT:
		callEvent = qfalse;
		Weapon_Medic( ent );
		break;
	case WP_PLIERS:
		Weapon_Engineer( ent );
		break;
	case WP_SMOKE_GRENADE:
		{
			float charge = ( g_smokeGrenades.integer ) && ( ent->client->ps.powerups[PW_SMOKEGRENADE] > 0 ) ? 0.25 : 0.5;

			if ( level.time - ent->client->ps.classWeaponTime >= g_LTChargeTime.integer * charge ) {
				if ( level.time - ent->client->ps.classWeaponTime > g_LTChargeTime.integer ) {
					ent->client->ps.classWeaponTime = level.time - g_LTChargeTime.integer;
				}

				if ( ent->client->ps.powerups[PW_SMOKEGRENADE] > 0 ) {
					ent->client->ps.classWeaponTime += g_LTChargeTime.integer * charge;
				} else {
					ent->client->ps.classWeaponTime = level.time;
				}

				pFiredShot = weapon_grenadelauncher_fire( ent,WP_SMOKE_GRENADE );
			}
		}
		break;
		// -NERVE - SMF
	case WP_ARTY:
		//G_Printf( "calling artilery\n" );
		break;
	case WP_MEDIC_SYRINGE:
		Weapon_Syringe( ent );
		break;
	case WP_AMMO:
		callEvent = qfalse;
		if ( g_nadePacks.integer && ent->client && ent->client->ps.powerups[PW_NADEPACK] > 0 ) {
			Weapon_GrenadePack( ent );
		} else {
			Weapon_MagicAmmo( ent );
		}
		break;
		// jpw
	case WP_LUGER:
		Bullet_Fire( ent, LUGER_SPREAD * aimSpreadScale, LUGER_DAMAGE );
		break;
	case WP_COLT:
		Bullet_Fire( ent, COLT_SPREAD * aimSpreadScale, COLT_DAMAGE );
		break;
	case WP_VENOM:
		weapon_venom_fire( ent, qfalse, aimSpreadScale );
		break;
	case WP_SNIPERRIFLE:
		Bullet_Fire( ent, SNIPER_SPREAD * aimSpreadScale, SNIPER_DAMAGE );
		// JPW NERVE -- added muzzle flip in multiplayer
		VectorCopy( ent->client->ps.viewangles,viewang );
		ent->client->sniperRifleMuzzleYaw = crandom() * 0.5; // used in clientthink
		ent->client->sniperRifleFiredTime = level.time;
		SetClientViewAngle( ent,viewang );
		// jpw
		break;
	case WP_MAUSER:
		aimSpreadScale = 1.0;
		Bullet_Fire( ent, MAUSER_SPREAD * aimSpreadScale, MAUSER_DAMAGE );
		break;
	case WP_STEN:
		Bullet_Fire( ent, STEN_SPREAD * aimSpreadScale, STEN_DAMAGE );
		break;
	case WP_MP40:
		Bullet_Fire( ent, MP40_SPREAD * aimSpreadScale, MP40_DAMAGE );
		break;
	case WP_THOMPSON:
		Bullet_Fire( ent, THOMPSON_SPREAD * aimSpreadScale, THOMPSON_DAMAGE );
		break;
	case WP_PANZERFAUST:
		ent->client->ps.classWeaponTime = level.time; // JPW NERVE
		Weapon_RocketLauncher_Fire( ent );
		if ( ent->client ) {
			vec3_t forward;
			AngleVectors( ent->client->ps.viewangles, forward, NULL, NULL );
			VectorMA( ent->client->ps.velocity, -64, forward, ent->client->ps.velocity );
		}
		break;
	case WP_GRENADE_LAUNCHER:
	case WP_GRENADE_PINEAPPLE:
	case WP_DYNAMITE:
	case WP_DYNAMITE2:
		if ( ent->s.weapon == WP_DYNAMITE ) {
			ent->client->ps.classWeaponTime = level.time; // JPW NERVE
		}
		pFiredShot = weapon_grenadelauncher_fire( ent, ent->s.weapon );
		break;
	case WP_FLAMETHROWER:
		// RF, this is done client-side only now
		Weapon_FlamethrowerFire( ent );
		break;
	case WP_MORTAR:
		break;
	default:
		break;
	}

	// Omni-bot - Send a fire event.
	if ( callEvent ) {
		Bot_Event_FireWeapon( ent - g_entities, Bot_WeaponGameToBot( ent->s.weapon ), pFiredShot );
	}
}