// cg_effects.c -- these functions generate localentities, usually as a result
// of event processing

#include "cg_local.h"


/*
==================
CG_BubbleTrail

Bullets shot underwater
==================
*/
void CG_BubbleTrail( vec3_t start, vec3_t end, float size, float spacing ) {
	vec3_t		move;
	vec3_t		vec;
	float		len;
	int			i;

	VectorCopy (start, move);
	VectorSubtract (end, start, vec);
	len = VectorNormalize (vec);

	// advance a random amount first
	i = rand() % (int)spacing;
	VectorMA( move, i, vec, move );

	VectorScale (vec, spacing, vec);

	for ( ; i < len; i += spacing ) {
		localEntity_t	*le;
		refEntity_t		*re;

		le = CG_AllocLocalEntity();
		le->leFlags = LEF_PUFF_DONT_SCALE;
		le->leType = LE_MOVE_SCALE_FADE;
		le->startTime = cg.time;
		le->endTime = cg.time + 1000 + random() * 250;
		le->lifeRate = 1.0 / ( le->endTime - le->startTime );

		re = &le->refEntity;
		re->shaderTime = cg.time / 1000.0f;

		re->reType = RT_SPRITE;
		re->rotation = 0;
//		re->radius = 3;	
		re->radius = size; // (SA)
		re->customShader = cgs.media.waterBubbleShader;
		re->shaderRGBA[0] = 0xff;
		re->shaderRGBA[1] = 0xff;
		re->shaderRGBA[2] = 0xff;
		re->shaderRGBA[3] = 0xff;

		le->color[3] = 1.0;

		le->pos.trType = TR_LINEAR;
		le->pos.trTime = cg.time;
		VectorCopy( move, le->pos.trBase );
		le->pos.trDelta[0] = crandom()*3;
		le->pos.trDelta[1] = crandom()*3;
//		le->pos.trDelta[2] = crandom()*5 + 6;
		le->pos.trDelta[2] = crandom()*5 + 20;	// (SA)

		VectorAdd (move, vec, move);
	}
}

/*
=====================
CG_SmokePuff

Adds a smoke puff or blood trail localEntity.

(SA) boy, it would be nice to have an acceleration vector for this as well.
		big velocity vector with a negative acceleration for deceleration, etc.
		(breath could then come out of a guys mouth at the rate he's walking/running and it
		would slow down once it's created)
=====================
*/

//----(SA)	modified
localEntity_t *CG_SmokePuff( const vec3_t p, const vec3_t vel, 
				   float radius,
				   float r, float g, float b, float a,
				   float duration,
				   int startTime,
				   int fadeInTime,
				   int leFlags,
				   qhandle_t hShader ) {
	static int	seed = 0x92;
	localEntity_t	*le;
	refEntity_t		*re;

	le = CG_AllocLocalEntity();
	le->leFlags = leFlags;
	le->radius = radius;

	re = &le->refEntity;
	re->rotation = Q_random( &seed ) * 360;
	re->radius = radius;
	re->shaderTime = startTime / 1000.0f;

	le->leType = LE_MOVE_SCALE_FADE;
	le->startTime = startTime;
	le->endTime = startTime + duration;
	le->fadeInTime = fadeInTime;
	if ( fadeInTime > startTime )
		le->lifeRate = 1.0 / ( le->endTime - le->fadeInTime );
	else
		le->lifeRate = 1.0 / ( le->endTime - le->startTime );
	le->color[0] = r;
	le->color[1] = g; 
	le->color[2] = b;
	le->color[3] = a;


	le->pos.trType = TR_LINEAR;
	le->pos.trTime = startTime;
	VectorCopy( vel, le->pos.trDelta );
	VectorCopy( p, le->pos.trBase );

	VectorCopy( p, re->origin );
	re->customShader = hShader;

	// rage pro can't alpha fade, so use a different shader
	if ( cgs.glconfig.hardwareType == GLHW_RAGEPRO ) {
		re->customShader = cgs.media.smokePuffRageProShader;
		re->shaderRGBA[0] = 0xff;
		re->shaderRGBA[1] = 0xff;
		re->shaderRGBA[2] = 0xff;
		re->shaderRGBA[3] = 0xff;
	} else {
		re->shaderRGBA[0] = le->color[0] * 0xff;
		re->shaderRGBA[1] = le->color[1] * 0xff;
		re->shaderRGBA[2] = le->color[2] * 0xff;
		re->shaderRGBA[3] = 0xff;
	}
// JPW NERVE
	if (cg_fxflags & 1) {
		re->customShader = getTestShader();
		re->rotation = 180;
	}
// jpw

	re->reType = RT_SPRITE;
	re->radius = le->radius;

	return le;
}

/*
==================
CG_SpawnEffect

Player teleporting in or out
==================
*/
void CG_SpawnEffect( vec3_t org ) {
	localEntity_t	*le;
	refEntity_t		*re;

	return;			// (SA) don't play spawn in effect right now

	le = CG_AllocLocalEntity();
	le->leFlags = 0;
	le->leType = LE_FADE_RGB;
	le->startTime = cg.time;
	le->endTime = cg.time + 500;
	le->lifeRate = 1.0 / ( le->endTime - le->startTime );

	le->color[0] = le->color[1] = le->color[2] = le->color[3] = 1.0;

	re = &le->refEntity;

	re->reType = RT_MODEL;
	re->shaderTime = cg.time / 1000.0f;

	re->customShader = cgs.media.teleportEffectShader;
	re->hModel = cgs.media.teleportEffectModel;
	AxisClear( re->axis );

	VectorCopy( org, re->origin );
	re->origin[2] -= 24;
}

qhandle_t getTestShader(void) {
	switch (rand()%2) {
	case 0:
		return cgs.media.nerveTestShader;
		break;
	case 1:
	default:
		return cgs.media.idTestShader;
		break;
	}
}

/*
====================
CG_MakeExplosion
====================
*/
localEntity_t *CG_MakeExplosion( vec3_t origin, vec3_t dir, 
								qhandle_t hModel, qhandle_t shader,
								int msec, qboolean isSprite ) {
	float			ang;
	localEntity_t	*ex;
	int				offset;
	vec3_t			tmpVec, newOrigin;

	if ( msec <= 0 ) {
		CG_Error( "CG_MakeExplosion: msec = %i", msec );
	}

	// skew the time a bit so they aren't all in sync
	offset = rand() & 63;

	ex = CG_AllocLocalEntity();
	if ( isSprite ) {
		ex->leType = LE_SPRITE_EXPLOSION;

		// randomly rotate sprite orientation
		ex->refEntity.rotation = rand() % 360;
		VectorScale( dir, 16, tmpVec );
		VectorAdd( tmpVec, origin, newOrigin );
	} else {
		ex->leType = LE_EXPLOSION;
		VectorCopy( origin, newOrigin );

		// set axis with random rotate
		if ( !dir ) {
			AxisClear( ex->refEntity.axis );
		} else {
			ang = rand() % 360;
			VectorCopy( dir, ex->refEntity.axis[0] );
			RotateAroundDirection( ex->refEntity.axis, ang );
		}
	}

	ex->startTime = cg.time - offset;
	ex->endTime = ex->startTime + msec;

	// bias the time so all shader effects start correctly
	ex->refEntity.shaderTime = ex->startTime / 1000.0f;

	ex->refEntity.hModel = hModel;
	ex->refEntity.customShader = shader;

	// set origin
	VectorCopy( newOrigin, ex->refEntity.origin );
	VectorCopy( newOrigin, ex->refEntity.oldorigin );

	// Ridah, move away from the wall as the sprite expands
	ex->pos.trType = TR_LINEAR;
	ex->pos.trTime = cg.time;
	VectorCopy( newOrigin, ex->pos.trBase );
	VectorScale( dir, 48, ex->pos.trDelta );
	// done.

	ex->color[0] = ex->color[1] = ex->color[2] = 1.0;

	return ex;
}

/*
=================
CG_AddBloodTrails
=================
*/
void CG_AddBloodTrails( vec3_t origin, vec3_t dir, int speed, int duration, int count, float randScale ) {
	localEntity_t	*le;
	refEntity_t		*re;
	vec3_t	velocity;
	int	i;

	for (i=0; i<count; i++) {
		le = CG_AllocLocalEntity();
		re = &le->refEntity;

		VectorSet( velocity, dir[0] + crandom()*randScale, dir[1] + crandom()*randScale, dir[2] + crandom() * randScale);
		VectorScale( velocity, (float)speed, velocity );

		le->leType = LE_BLOOD;
		le->startTime = cg.time;
		le->endTime = le->startTime + duration; // DHM - Nerve :: (removed) - (int)(0.5 * random() * duration);
		le->lastTrailTime = cg.time;

		VectorCopy( origin, re->origin );
		AxisCopy( axisDefault, re->axis );

		le->pos.trType = TR_GRAVITY_LOW;
		VectorCopy( origin, le->pos.trBase );
		VectorMA( le->pos.trBase, 2 + random()*4, dir, le->pos.trBase );
		VectorCopy( velocity, le->pos.trDelta );
		le->pos.trTime = cg.time;

		le->bounceFactor = 0.9;
	}
}

/*
=================
CG_Bleed

This is the spurt of blood when a character gets hit
=================
*/
void CG_Bleed( vec3_t origin, int entityNum ) {
#define	BLOOD_SPURT_COUNT	4
	int i,j;
	centity_t *cent;

	if( !cg_blood.integer ) {
		return;
	}

#ifdef SAVEGAME_SUPPORT
	if( cg_reloading.integer ) {
		// to dangerous, since we call playerangles() in here, which calls the animation system, which might not be setup yet
		return;
	}
#endif // SAVEGAME_SUPPORT

	cent = &cg_entities[entityNum];

	// Ridah, blood spurts
	if ( entityNum != cg.snap->ps.clientNum )
	{
		vec3_t vhead, vbody, bOrigin, dir, vec, pvec, ndir;

		CG_GetBleedOrigin( vhead, vbody, entityNum );
		
		// project the impact point onto the vector defined by torso -> head
		ProjectPointOntoVector( origin, vbody, vhead, bOrigin );

		// if it's below the waste, or above the head, clamp
		VectorSubtract( vhead, vbody, vec );
		VectorSubtract( bOrigin, vbody, pvec );
		if( DotProduct( pvec, vec ) < 0 ) {
			VectorCopy( vbody, bOrigin );
		} else {
			VectorSubtract( bOrigin, vhead, pvec );
			if( DotProduct( pvec, vec ) > 0 ) {
				VectorCopy( vhead, bOrigin );
			}
		}

		// spawn some blood trails, heading out towards the impact point
		VectorSubtract( origin, bOrigin, dir );
		VectorNormalize( dir );

		{
			float	len;
			vec3_t	vec;

			VectorSubtract( bOrigin, vhead, vec );
			len = VectorLength (vec);

			if( len > 8 ) {
				VectorMA( bOrigin, 8, dir, bOrigin );
			}
		}
		
		// DHM - Nerve :: Made minor adjustments
		for( i = 0; i < BLOOD_SPURT_COUNT; i++ ) {
			VectorCopy(dir, ndir);
			for( j = 0; j < 3; j++ )
				ndir[j] += crandom()*0.3;
			VectorNormalize( ndir );
			CG_AddBloodTrails( bOrigin, ndir,
							   100,	// speed
							   450 + (int)(crandom () * 50),	// duration
							   2 + rand()%2,	// count
							   0.1 );	// rand scale
		}			
	}
}

/*
==================
CG_LaunchGib
==================
*/
void CG_LaunchGib( centity_t *cent, vec3_t origin, vec3_t angles, vec3_t velocity, qhandle_t hModel, float sizeScale, int breakCount ) {
	localEntity_t	*le;
	refEntity_t		*re;
	int i;

	if ( !cg_blood.integer ) {
		return;
	}

	le = CG_AllocLocalEntity();
	re = &le->refEntity;

	le->leType = LE_FRAGMENT;
	le->startTime = cg.time;
	// le->endTime = le->startTime + 60000 + random() * 60000;
	le->endTime = le->startTime + 20000 + (crandom() * 5000);
	le->breakCount = breakCount;
	le->sizeScale = sizeScale;

	VectorCopy( angles, le->angles.trBase );
	VectorCopy( origin, re->origin );
	AnglesToAxis( angles, re->axis );
	if (sizeScale != 1.0) {
		for (i=0;i<3;i++) VectorScale( re->axis[i], sizeScale, re->axis[i] );
	}
	re->hModel = hModel;

	// re->fadeStartTime		= le->endTime - 3000;
	re->fadeStartTime		= le->endTime - 1000;
	re->fadeEndTime			= le->endTime;

		le->leBounceSoundType = LEBS_BLOOD;
		le->leMarkType = LEMT_BLOOD;
		le->pos.trType = TR_GRAVITY;

		le->angles.trDelta[0] = (10 + (rand()&50)) - 30;
//	le->angles.trDelta[0] = (100 + (rand()&500)) - 300;	// pitch
		le->angles.trDelta[1] = (100 + (rand()&500)) - 300;	// (SA) this is the safe one right now (yaw)  turn the others up when I have tumbling things landing properly
		le->angles.trDelta[2] = (10 + (rand()&50)) - 30;
//	le->angles.trDelta[2] = (100 + (rand()&500)) - 300;	// roll

		le->bounceFactor = 0.3;

	VectorCopy( origin, le->pos.trBase );
	VectorCopy( velocity, le->pos.trDelta );
	le->pos.trTime = cg.time;


	le->angles.trType = TR_LINEAR;

	le->angles.trTime = cg.time;

	le->ownerNum = cent->currentState.number;

	// Ridah, if the player is on fire, then spawn some flaming gibs
	if (cent && CG_EntOnFire(cent)) {
		le->onFireStart = cent->currentState.onFireStart;
		le->onFireEnd = re->fadeEndTime+1000;
	}
}

//#define	GIB_VELOCITY	250
//#define	GIB_JUMP		250

#define	GIB_VELOCITY	75
#define	GIB_JUMP		250


/*
==============
CG_LoseHat
==============
*/
void CG_LoseHat( centity_t *cent, vec3_t dir )
{
	clientInfo_t	*ci;
	int				clientNum;
//	int				i, count, tagIndex, gibIndex;
	int				tagIndex;
	vec3_t			origin, velocity;
	bg_character_t	*character;

	clientNum = cent->currentState.clientNum;
	if( clientNum < 0 || clientNum >= MAX_CLIENTS ) {
		CG_Error( "Bad clientNum on player entity");
	}
	ci = &cgs.clientinfo[ clientNum ];
	character = CG_CharacterForClientinfo( ci,  cent );

	// don't launch anything if they don't have one
	if( !character->accModels[ACC_HAT] )
		return;

	tagIndex = CG_GetOriginForTag( cent, &cent->pe.headRefEnt, "tag_mouth", 0, origin, NULL );

	velocity[0] = dir[0]*(0.75+random())*GIB_VELOCITY;
	velocity[1] = dir[1]*(0.75+random())*GIB_VELOCITY;
	velocity[2] = GIB_JUMP - 50 + dir[2]*(0.5+random())*GIB_VELOCITY;

	{
		localEntity_t	*le;
		refEntity_t		*re;

		le = CG_AllocLocalEntity();
		re = &le->refEntity;

		le->leType = LE_FRAGMENT;
		le->startTime = cg.time;
		le->endTime = le->startTime + 20000 + (crandom() * 5000);

		VectorCopy( origin, re->origin );
		AxisCopy( axisDefault, re->axis );
		re->hModel = character->accModels[ACC_HAT];
		re->customSkin = character->accSkins[ACC_HAT];

		re->fadeStartTime		= le->endTime - 1000;
		re->fadeEndTime			= le->endTime;

		// (SA) FIXME: origin of hat md3 is offset from center.  need to center the origin when you toss it
		le->pos.trType = TR_GRAVITY;
		VectorCopy( origin, le->pos.trBase );
		VectorCopy( velocity, le->pos.trDelta );
		le->pos.trTime = cg.time;

		// spin it a bit
		le->angles.trType		= TR_LINEAR;
		VectorCopy( tv(0, 0, 0), le->angles.trBase );
		le->angles.trDelta[0]	= 0;
		le->angles.trDelta[1]	= (100 + (rand()&500)) - 300;
//		le->angles.trDelta[2]	= 0;
		le->angles.trDelta[2]	= 400;	// (SA) this is set with a very particular value to try to get it
										// to flip exactly once before landing (based on player alive
										// (standing) and on level ground) and will be unnecessary when
										// I have things landing properly on their own

		le->angles.trTime		= cg.time;

		le->bounceFactor		= 0.2;

		// Ridah, if the player is on fire, then make the hat on fire
		if( cent && CG_EntOnFire(cent) ) {
			le->onFireStart = cent->currentState.onFireStart;
			le->onFireEnd = cent->currentState.onFireEnd + 4000;
		}
	}
}

/*
======================
CG_GetOriginForTag

  places the position of the tag into "org"

  returns the index of the tag it used, so we can cycle through tag's with the same name
======================
*/
int CG_GetOriginForTag( centity_t *cent, refEntity_t *parent, char *tagName, int startIndex, vec3_t org, vec3_t axis[3] ) {
	int				i;
	orientation_t	lerped;
	int				retval;

	// lerp the tag
	retval = trap_R_LerpTag( &lerped, parent, tagName, startIndex );

	if (retval < 0)
		return retval;

	VectorCopy( parent->origin, org );

	for ( i = 0 ; i < 3 ; i++ ) {
		VectorMA( org, lerped.origin[i], parent->axis[i], org );
	}

	if (axis) {
		// had to cast away the const to avoid compiler problems...
		MatrixMultiply( lerped.axis, ((refEntity_t *)parent)->axis, axis );
	}

	return retval;
}

/*
===================
CG_GibPlayer

Generated a bunch of gibs launching out from the bodies location
===================
*/
#define MAXJUNCTIONS 8

void CG_GibPlayer( centity_t *cent, vec3_t playerOrigin, vec3_t gdir )
{
	int				i, count = 0, tagIndex, gibIndex;
	vec3_t			origin, velocity, dir;
	trace_t			trace;
	qboolean		foundtag;
	clientInfo_t	*ci;
	int				clientNum;
	bg_character_t	*character;
	vec4_t			projection, color;	

	// Rafael
	// BloodCloud
	qboolean		newjunction[MAXJUNCTIONS];
	vec3_t			junctionOrigin[MAXJUNCTIONS];
	int				junction;
	int				j;
	vec3_t			axis[3], angles;

	char *JunctiongibTags[] = {
		// leg tag
		"tag_footright",
		"tag_footleft",
		"tag_legright",
		"tag_legleft",
					
		// torsotags
		"tag_armright",
		"tag_armleft",
					
		"tag_torso",
		"tag_chest"
		};

	char *ConnectTags[] = {
		// legs tags
		"tag_legright",
		"tag_legleft",
		"tag_torso",
		"tag_torso",
					
		// torso tags	
		"tag_chest",
		"tag_chest",

		"tag_chest",
		"tag_torso",
	};

	char *gibTags[] = {
		// tags in the legs
		"tag_footright",
		"tag_footleft",
		"tag_legright",
		"tag_legleft",
		"tag_torso",

		// tags in the torso
		"tag_chest",
		"tag_armright",
		"tag_armleft",
		"tag_head",
		NULL
	};

	if( cg_blood.integer ) {
		// Rafael
		for( i = 0; i < MAXJUNCTIONS; i++ )
			newjunction[i] = qfalse;

		clientNum = cent->currentState.clientNum;
		if( clientNum < 0 || clientNum >= MAX_CLIENTS ) {
			CG_Error( "Bad clientNum on player entity");
		}
		ci = &cgs.clientinfo[ clientNum ];
		character = CG_CharacterForClientinfo( ci, cent );

		// Ridah, fetch the various positions of the tag_gib*'s
		// and spawn the gibs from the correct places (especially the head)
		for( gibIndex = 0, count = 0, foundtag = qtrue; foundtag && gibIndex < MAX_GIB_MODELS && gibTags[gibIndex]; gibIndex++ ) {

			refEntity_t *re = 0;

			foundtag = qfalse;

			if( !character->gibModels[gibIndex] ) {
				continue;
			}

			re = &cent->pe.bodyRefEnt;

			for( tagIndex=0; (tagIndex = CG_GetOriginForTag( cent, re, gibTags[gibIndex], tagIndex, origin, axis )) >= 0; count++, tagIndex++ ) {

				foundtag = qtrue;

				VectorSubtract( origin, re->origin, dir );
				VectorNormalize( dir );

				// spawn a gib
				velocity[0] = dir[0]*(0.5+random())*GIB_VELOCITY*0.3;
				velocity[1] = dir[1]*(0.5+random())*GIB_VELOCITY*0.3;
				velocity[2] = GIB_JUMP + dir[2]*(0.5+random())*GIB_VELOCITY*0.5;

				VectorMA( velocity, GIB_VELOCITY, gdir, velocity );
				AxisToAngles( axis, angles );

				CG_LaunchGib( cent, origin, angles, velocity, character->gibModels[gibIndex], 1.0, 0 );

				for( junction = 0; junction < MAXJUNCTIONS; junction++ ) {
					if( !Q_stricmp (gibTags[gibIndex], JunctiongibTags[junction]) ) {
						VectorCopy (origin, junctionOrigin[junction]);
						newjunction[junction] = qtrue;
					}
				}
			}
		}

		for( i=0; i<MAXJUNCTIONS; i++ ) {
			if( newjunction[i] == qtrue ) {
				for( j=0; j<MAXJUNCTIONS; j++ ) {
					if( !Q_stricmp (JunctiongibTags[j], ConnectTags[i]) ) {
						if( newjunction[j] == qtrue ) {
							// spawn a blood cloud somewhere on the vec from
							VectorSubtract (junctionOrigin[i], junctionOrigin[j], dir);					
							CG_ParticleBloodCloud (cent, junctionOrigin[i], dir);
						}
					}
				}
			}
		}

		// Ridah, spawn a bunch of blood dots around the place
		#define GIB_BLOOD_DOTS	3
		for (i=0, count=0; i<GIB_BLOOD_DOTS*2; i++) {
      // TTimo: unused
			//static vec3_t mins = {-10,-10,-10};
			//static vec3_t maxs = { 10, 10, 10};

			if (i>0) {
				velocity[0] = ((i%2)*2 - 1)*(40 + 40*random());
				velocity[1] = (((i/2)%2)*2 - 1)*(40 + 40*random());
				velocity[2] = (((i<GIB_BLOOD_DOTS)*2)-1)*40;
			} else {
				VectorClear( velocity );
				velocity[2] = -64;
			}

			VectorAdd( playerOrigin, velocity, origin );

			CG_Trace( &trace, playerOrigin, NULL, NULL, origin, -1, CONTENTS_SOLID );
			if (trace.fraction < 1.0) {
				//%	BG_GetMarkDir( velocity, trace.plane.normal, velocity );
				//%	CG_ImpactMark( cgs.media.bloodDotShaders[rand()%5], trace.endpos, velocity, random()*360,
				//%		1,1,1,1, qtrue, 30, qfalse, cg_bloodTime.integer * 1000 );
				#if 0
					BG_GetMarkDir( velocity, trace.plane.normal, projection );
					VectorSubtract( vec3_origin, projection, projection );
					projection[ 3 ] = 64;
					VectorMA( trace.endpos, -8.0f, projection, markOrigin );
					CG_ImpactMark( cgs.media.bloodDotShaders[ rand() % 5 ], markOrigin, projection, 30.0f, random() * 360.0f, 1.0f, 1.0f, 1.0f, 1.0f, cg_bloodTime.integer * 1000 );
				#else
					VectorSet( projection, 0, 0, -1 );
					projection[ 3 ] = 30.0f;
					Vector4Set( color, 1.0f, 1.0f, 1.0f, 1.0f );
					trap_R_ProjectDecal( cgs.media.bloodDotShaders[ rand() % 5 ], 1, (vec3_t*) trace.endpos, projection, color,
						cg_bloodTime.integer * 1000, (cg_bloodTime.integer * 1000) >> 4 );
				#endif
				
				if (count++ > GIB_BLOOD_DOTS)
					break;
			}
		}
	}

	if(!(cent->currentState.eFlags & EF_HEADSHOT)){	// (SA) already lost hat while living
		CG_LoseHat(cent, tv(0, 0, 1));
	}
}


/*
==============
CG_SparklerSparks
==============
*/
void CG_SparklerSparks( vec3_t origin, int count ) {
// these effect the look of the, umm, effect
int	FUSE_SPARK_LIFE			= 100;
int	FUSE_SPARK_LENGTH		= 30;
// these are calculated from the above
int	FUSE_SPARK_SPEED		= (FUSE_SPARK_LENGTH*1000/FUSE_SPARK_LIFE);

	int	i;
	localEntity_t	*le;
	refEntity_t		*re;

	for (i=0; i<count; i++) {

		// spawn the spark
		le = CG_AllocLocalEntity();
		re = &le->refEntity;

		le->leType = LE_FUSE_SPARK;
		le->startTime = cg.time;
		le->endTime = cg.time + FUSE_SPARK_LIFE;
		le->lastTrailTime = cg.time;

		VectorCopy( origin, re->origin );

		le->pos.trType = TR_GRAVITY;
		VectorCopy( origin, le->pos.trBase );
		VectorSet( le->pos.trDelta, crandom(), crandom(), crandom() );
		VectorNormalize( le->pos.trDelta );
		VectorScale( le->pos.trDelta, FUSE_SPARK_SPEED, le->pos.trDelta );
		le->pos.trTime = cg.time;

	}
}

// just a bunch of numbers we can use for pseudo-randomizing based on time
#define	NUMRANDTABLE 257
unsigned short randtable[NUMRANDTABLE] =
{
	0x0000,	0x1021,	0x2042,	0x3063,	0x4084,	0x50a5,	0x60c6,	0x70e7,
	0x8108,	0x9129,	0xa14a,	0xb16b,	0xc18c,	0xd1ad,	0xe1ce,	0xf1ef,
	0x1231,	0x0210,	0x3273,	0x2252,	0x52b5,	0x4294,	0x72f7,	0x62d6,
	0x9339,	0x8318,	0xb37b,	0xa35a,	0xd3bd,	0xc39c,	0xf3ff,	0xe3de,
	0x2462,	0x3443,	0x0420,	0x1401,	0x64e6,	0x74c7,	0x44a4,	0x5485,
	0xa56a,	0xb54b,	0x8528,	0x9509,	0xe5ee,	0xf5cf,	0xc5ac,	0xd58d,
	0x3653,	0x2672,	0x1611,	0x0630,	0x76d7,	0x66f6,	0x5695,	0x46b4,
	0xb75b,	0xa77a,	0x9719,	0x8738,	0xf7df,	0xe7fe,	0xd79d,	0xc7bc,
	0x48c4,	0x58e5,	0x6886,	0x78a7,	0x0840,	0x1861,	0x2802,	0x3823,
	0xc9cc,	0xd9ed,	0xe98e,	0xf9af,	0x8948,	0x9969,	0xa90a,	0xb92b,
	0x5af5,	0x4ad4,	0x7ab7,	0x6a96,	0x1a71,	0x0a50,	0x3a33,	0x2a12,
	0xdbfd,	0xcbdc,	0xfbbf,	0xeb9e,	0x9b79,	0x8b58,	0xbb3b,	0xab1a,
	0x6ca6,	0x7c87,	0x4ce4,	0x5cc5,	0x2c22,	0x3c03,	0x0c60,	0x1c41,
	0xedae,	0xfd8f,	0xcdec,	0xddcd,	0xad2a,	0xbd0b,	0x8d68,	0x9d49,
	0x7e97,	0x6eb6,	0x5ed5,	0x4ef4,	0x3e13,	0x2e32,	0x1e51,	0x0e70,
	0xff9f,	0xefbe,	0xdfdd,	0xcffc,	0xbf1b,	0xaf3a,	0x9f59,	0x8f78,
	0x9188,	0x81a9,	0xb1ca,	0xa1eb,	0xd10c,	0xc12d,	0xf14e,	0xe16f,
	0x1080,	0x00a1,	0x30c2,	0x20e3,	0x5004,	0x4025,	0x7046,	0x6067,
	0x83b9,	0x9398,	0xa3fb,	0xb3da,	0xc33d,	0xd31c,	0xe37f,	0xf35e,
	0x02b1,	0x1290,	0x22f3,	0x32d2,	0x4235,	0x5214,	0x6277,	0x7256,
	0xb5ea,	0xa5cb,	0x95a8,	0x8589,	0xf56e,	0xe54f,	0xd52c,	0xc50d,
	0x34e2,	0x24c3,	0x14a0,	0x0481,	0x7466,	0x6447,	0x5424,	0x4405,
	0xa7db,	0xb7fa,	0x8799,	0x97b8,	0xe75f,	0xf77e,	0xc71d,	0xd73c,
	0x26d3,	0x36f2,	0x0691,	0x16b0,	0x6657,	0x7676,	0x4615,	0x5634,
	0xd94c,	0xc96d,	0xf90e,	0xe92f,	0x99c8,	0x89e9,	0xb98a,	0xa9ab,
	0x5844,	0x4865,	0x7806,	0x6827,	0x18c0,	0x08e1,	0x3882,	0x28a3,
	0xcb7d,	0xdb5c,	0xeb3f,	0xfb1e,	0x8bf9,	0x9bd8,	0xabbb,	0xbb9a,
	0x4a75,	0x5a54,	0x6a37,	0x7a16,	0x0af1,	0x1ad0,	0x2ab3,	0x3a92,
	0xfd2e,	0xed0f,	0xdd6c,	0xcd4d,	0xbdaa,	0xad8b,	0x9de8,	0x8dc9,
	0x7c26,	0x6c07,	0x5c64,	0x4c45,	0x3ca2,	0x2c83,	0x1ce0,	0x0cc1,
	0xef1f,	0xff3e,	0xcf5d,	0xdf7c,	0xaf9b,	0xbfba,	0x8fd9,	0x9ff8,
	0x6e17,	0x7e36,	0x4e55,	0x5e74,	0x2e93,	0x3eb2,	0x0ed1,	0x1ef0
};

#define	LT_MS		100	// random number will change every LT_MS millseconds
#define	LT_RANDMAX	((unsigned short)0xffff)

float lt_random(int thisrandseed, int t) {
	return (float)randtable[(thisrandseed+t+(cg.time/LT_MS)*(cg.time/LT_MS))%NUMRANDTABLE] / (float)LT_RANDMAX;
}

float lt_crandom(int thisrandseed, int t) {
	return ((2.0 * ((float)randtable[(thisrandseed+t+(cg.time/LT_MS)*(cg.time/LT_MS))%NUMRANDTABLE] / (float)LT_RANDMAX)) - 1.0);
}

/*
================
CG_ProjectedSpotLight
================
*/
void CG_ProjectedSpotLight( vec3_t start, vec3_t dir )
{
	vec3_t		end;
	trace_t		tr;
	float		alpha, radius;
	vec4_t		projection;

	
	VectorMA( start, 1000, dir, end );
	CG_Trace( &tr, start, NULL, NULL, end, -1, CONTENTS_SOLID );
	if (tr.fraction == 1.0)
		return;
	//
	alpha = (1.0 - tr.fraction);
	if (alpha > 1.0)
		alpha = 1.0;
	//
	radius = 32 + 64*tr.fraction;
	//%	VectorNegate( dir, projection );
	//%	CG_ImpactMark( cgs.media.spotLightShader, tr.endpos, projection, 0, alpha, alpha, alpha, 1.0, qfalse, radius, qtrue, -2 );
	
	VectorCopy( dir, projection );
	projection[ 3 ] = radius * 2.0f;
	CG_ImpactMark( cgs.media.spotLightShader, tr.endpos, projection, radius, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1 );
}


#define MAX_SPOT_SEGS 20
#define MAX_SPOT_RANGE 2000
/*
==============
CG_Spotlight
	segs:	number of sides on tube. - 999 is a valid value and just means, 'cap to max' (MAX_SPOT_SEGS) or use lod scheme
	range:	effective range of beam
	startWidth: will be optimized for '0' as a value (true cone) to not use quads and not cap the start circle

	-- flags --
	SL_NOTRACE			- don't do a trace check for shortening the beam, always draw at full 'range' length
	SL_TRACEWORLDONLY	- go through everything but the world
	SL_NODLIGHT			- don't put a dlight at the end
	SL_NOSTARTCAP		- dont' cap the start circle
	SL_LOCKTRACETORANGE	- only trace out as far as the specified range (rather than to max spot range)
	SL_NOFLARE			- don't draw a flare when the light is pointing at the camera
	SL_NOIMPACT			- don't draw the impact mark on hit surfaces
	SL_LOCKUV			- lock the texture coordinates at the 'true' length of the requested beam.
	SL_NOCORE			- don't draw the center 'core' beam






  I know, this is a bit kooky right now.  It evolved big, but now that I know what it should do, it'll get
  crunched down to a bunch of table driven stuff.  once it works, I'll make it work well...

==============
*/

void CG_Spotlight(centity_t *cent, float *color, vec3_t realstart, vec3_t lightDir, int segs, float range, int startWidth, float coneAngle, int flags) {
	int			i, j;
	vec3_t		start, traceEnd;
	vec3_t		right, up;
	vec3_t		v1, v2;
	vec3_t		startvec, endvec;	// the vectors to rotate around lightDir to create the circles
	vec3_t		conevec;
	vec3_t		start_points[MAX_SPOT_SEGS], end_points[MAX_SPOT_SEGS];
	vec3_t		coreright;
	polyVert_t	verts[MAX_SPOT_SEGS*4];	// x4 for 4 verts per poly
	polyVert_t	plugVerts[MAX_SPOT_SEGS];
	vec3_t		endCenter;
	polyVert_t	coreverts[4];
	trace_t		tr;
	float alpha;
	float radius = 0.0; // TTimo might be used uninitialized
	float coreEndRadius;
	qboolean	capStart = qtrue;
	float		hitDist;	// the actual distance of the trace impact	(0 is no hit)
	float		beamLen;	// actual distance of the drawn beam
	float		endAlpha	= 0.0;
	vec4_t		colorNorm;	// normalized color vector
	refEntity_t			ent;
	vec3_t		angles;
	vec4_t		projection;

	VectorCopy(realstart, start);

	// normalize color
	colorNorm[3] = 0;	// store normalize multiplier in alpha index
	for(i=0;i<3;i++) {
		if(color[i] > colorNorm[3])
			colorNorm[3] = color[i];	// find largest color value in RGB
	}

	if(colorNorm[3] != 1) {		// it needs to be boosted
		VectorMA(color, 1.0/colorNorm[3], color, colorNorm);	// FIXME: div by 0
	} else {
		VectorCopy(color, colorNorm);
	}
	colorNorm[3] = color[3];


	if(flags & SL_NOSTARTCAP) {
		capStart = qfalse;
	}

	if(startWidth == 0)	{	// cone, not cylinder
		capStart = qfalse;
	}

	if(flags&SL_LOCKTRACETORANGE) {
		VectorMA( start, range, lightDir, traceEnd );			// trace out to 'range'
	} else {
		VectorMA( start, MAX_SPOT_RANGE, lightDir, traceEnd );	// trace all the way out to max dist
	}

	// first trace to see if anything is hit
	if(flags & SL_NOTRACE) {
		tr.fraction = 1.0;	// force no hit
	} else {
		if(flags&SL_TRACEWORLDONLY)
			CG_Trace( &tr, start, NULL, NULL, traceEnd, -1, CONTENTS_SOLID);
		else
			CG_Trace( &tr, start, NULL, NULL, traceEnd, -1, MASK_SHOT);
//		CG_Trace( &tr, start, NULL, NULL, traceEnd, -1, MASK_ALL &~(CONTENTS_MONSTERCLIP|CONTENTS_AREAPORTAL|CONTENTS_CLUSTERPORTAL));
	}


	if(tr.fraction < 1.0) {
		hitDist = beamLen = MAX_SPOT_RANGE * tr.fraction;
		if(beamLen > range)
			beamLen = range;
	} else {
		hitDist = 0;
		beamLen = range;
	}


	if(flags&SL_LOCKUV) {
		if(beamLen < range) {
			endAlpha = 255.0f * ( color[3] - (color[3] * beamLen/range ));
		}
	}


	if(segs >= MAX_SPOT_SEGS)
		segs = MAX_SPOT_SEGS - 1;

	// TODO: adjust segs based on r_lodbias
	// TODO: move much of this to renderer
	

// model at base
	if(cent->currentState.modelindex) {
		memset (&ent, 0, sizeof(ent));
		ent.frame = 0;
		ent.oldframe = 0;
		ent.backlerp = 0;
		VectorCopy( cent->lerpOrigin, ent.origin);
		VectorCopy( cent->lerpOrigin, ent.oldorigin);
		ent.hModel = cgs.gameModels[cent->currentState.modelindex];
	//	AnglesToAxis( cent->lerpAngles, ent.axis );
		vectoangles(lightDir, angles);
		AnglesToAxis( angles, ent.axis );
		trap_R_AddRefEntityToScene (&ent);
		memcpy( &cent->refEnt, &ent, sizeof(refEntity_t) );

		// push start out a bit so the beam fits to the front of the base model
		VectorMA(start, 14, lightDir, start);
	}

//// BEAM

	PerpendicularVector( up, lightDir);
	CrossProduct( lightDir, up, right );

	// find first vert of the start
	VectorScale( right, startWidth, startvec);
	// find the first vert of the end
	RotatePointAroundVector( conevec, up, lightDir, -coneAngle );
	VectorMA(startvec, beamLen, conevec, endvec);	// this applies the offset of the start diameter before finding the end points

	VectorScale(lightDir, beamLen, endCenter);
	VectorSubtract(endCenter, endvec, coreverts[3].xyz);	// get a vector of the radius out at the end for the core to use
	coreEndRadius = VectorLength(coreverts[3].xyz);
#define CORESCALE 0.6f

//
//	generate the flat beam 'core'
//
	if(!(flags & SL_NOCORE)) {
		VectorSubtract( start, cg.refdef_current->vieworg, v1 );
		VectorNormalize( v1 );
		VectorSubtract( traceEnd, cg.refdef_current->vieworg, v2 );
		VectorNormalize( v2 );
		CrossProduct( v1, v2, coreright );
		VectorNormalize( coreright );

		memset(&coreverts[0], 0, 4*sizeof(polyVert_t));
		VectorMA(start, startWidth*0.5f, coreright, coreverts[0].xyz);
		VectorMA(start, -startWidth*0.5f, coreright, coreverts[1].xyz);
		VectorMA(endCenter, -coreEndRadius * CORESCALE, coreright, coreverts[2].xyz);
		VectorAdd(start, coreverts[2].xyz, coreverts[2].xyz);
		VectorMA(endCenter, coreEndRadius * CORESCALE, coreright, coreverts[3].xyz);
		VectorAdd(start, coreverts[3].xyz, coreverts[3].xyz);

		for(i=0;i<4;i++) {
			coreverts[i].modulate[0] = color[0]*200.0f;
			coreverts[i].modulate[1] = color[1]*200.0f;
			coreverts[i].modulate[2] = color[2]*200.0f;
			coreverts[i].modulate[3] = color[3]*200.0f;
			if(i>1) {
				coreverts[i].modulate[3] = 0;
			}
		}

		trap_R_AddPolyToScene( cgs.media.spotLightBeamShader, 4, &coreverts[0]);
	}


//
// generate the beam cylinder
//



	for ( i = 0; i <=segs; i++ ) {
		RotatePointAroundVector( start_points[i], lightDir, startvec, (360.0f/(float)segs)*i );
		VectorAdd( start_points[i], start, start_points[i] );

		RotatePointAroundVector( end_points[i], lightDir, endvec, (360.0f/(float)segs)*i );
		VectorAdd( end_points[i], start, end_points[i] );
	}

	for(i=0;i<segs;i++) {

		j = (i*4);

		VectorCopy(start_points[i], verts[(i*4)].xyz);
		verts[j].st[0]	= 0;
		verts[j].st[1]	= 1;
		verts[j].modulate[0] = color[0]*255.0f;
		verts[j].modulate[1] = color[1]*255.0f;
		verts[j].modulate[2] = color[2]*255.0f;
		verts[j].modulate[3] = color[3]*255.0f;
		j++;

		VectorCopy(end_points[i], verts[j].xyz);
		verts[j].st[0]	= 0;
		verts[j].st[1]	= 0;
		verts[j].modulate[0] = color[0]*255.0f;
		verts[j].modulate[1] = color[1]*255.0f;
		verts[j].modulate[2] = color[2]*255.0f;
		verts[j].modulate[3] = endAlpha;
		j++;

		VectorCopy(end_points[i+1], verts[j].xyz);
		verts[j].st[0]	= 1;
		verts[j].st[1]	= 0;
		verts[j].modulate[0] = color[0]*255.0f;
		verts[j].modulate[1] = color[1]*255.0f;
		verts[j].modulate[2] = color[2]*255.0f;
		verts[j].modulate[3] = endAlpha;
		j++;

		VectorCopy(start_points[i+1], verts[j].xyz);
		verts[j].st[0]	= 1;
		verts[j].st[1]	= 1;
		verts[j].modulate[0] = color[0]*255.0f;
		verts[j].modulate[1] = color[1]*255.0f;
		verts[j].modulate[2] = color[2]*255.0f;
		verts[j].modulate[3] = color[3]*255.0f;

		if(capStart) {
			VectorCopy(start_points[i], plugVerts[i].xyz);
			plugVerts[i].st[0]	= 0;
			plugVerts[i].st[1]	= 0;
			plugVerts[i].modulate[0] = color[0]*255.0f;
			plugVerts[i].modulate[1] = color[1]*255.0f;
			plugVerts[i].modulate[2] = color[2]*255.0f;
			plugVerts[i].modulate[3] = color[3]*255.0f;
		}
	}

	trap_R_AddPolysToScene( cgs.media.spotLightBeamShader, 4, &verts[0], segs );


	// plug up the start circle
	if(capStart) {
		trap_R_AddPolyToScene( cgs.media.spotLightBeamShader, segs, &plugVerts[0]);
	}


	// show the endpoint

	if(!(flags & SL_NOIMPACT)) {
		if(hitDist) {
			VectorMA(startvec, hitDist, conevec, endvec);

			alpha = 0.3f;
			radius = coreEndRadius * (hitDist/beamLen);

			//%	VectorNegate( lightDir, proj );
			//%	CG_ImpactMark( cgs.media.spotLightShader, tr.endpos, proj, 0, colorNorm[0], colorNorm[1], colorNorm[2], alpha, qfalse, radius, qtrue, -1 );
			
			VectorCopy( lightDir, projection );
			projection[ 3 ] = radius * 2.0f;
			CG_ImpactMark( cgs.media.spotLightShader, tr.endpos, projection, radius, colorNorm[ 0 ], colorNorm[ 1 ], colorNorm[ 2 ], 1.0f, 1.0f, -1 );
		}
	}



	// add d light at end
	if(!(flags & SL_NODLIGHT)) {
		vec3_t dlightLoc;
		VectorMA(tr.endpos, 0, lightDir, dlightLoc);	// back away from the hit
		trap_R_AddLightToScene( dlightLoc, radius * 2, 0.3, 1.0, 1.0, 1.0, 0, 0 );
	}



	// draw flare at source
	if(!(flags&SL_NOFLARE)) {
		qboolean	lightInEyes = qfalse;
		vec3_t		camloc, dirtolight;
		float		dot, deg, dist;
		float		flarescale = 0.0; // TTimo: might be used uninitialized

		// get camera position and direction to lightsource
		VectorCopy(cg.snap->ps.origin, camloc);
		camloc[2] += cg.snap->ps.viewheight;
		VectorSubtract (start, camloc, dirtolight);
		dist = VectorNormalize(dirtolight);

		// first use dot to determine if it's facing the camera
		dot = DotProduct( lightDir, dirtolight);

		// it's facing the camera, find out how closely and trace to see if the source can be seen

		deg = RAD2DEG(M_PI-acos(dot));
		if(deg <= 35) {	// start flare a bit before the camera gets inside the cylinder
			lightInEyes = qtrue;
			flarescale = 1-(deg/35);
		}

		if(lightInEyes)	{	// the dot check succeeded, now do a trace
			CG_Trace( &tr, start, NULL, NULL, camloc, -1, MASK_ALL &~(CONTENTS_MONSTERCLIP|CONTENTS_AREAPORTAL|CONTENTS_CLUSTERPORTAL));
			if(tr.fraction != 1)
				lightInEyes = qfalse;

		}

		if(lightInEyes) {
			float coronasize = flarescale;
			if(dist < 512)	// make even bigger if you're close enough
				coronasize *= (512.0f/dist);

			trap_R_AddCoronaToScene( start, colorNorm[0], colorNorm[1], colorNorm[2], coronasize, cent->currentState.number, qtrue);
		} else {
			// even though it's off, still need to add it, but turned off so it can fade in/out properly
			trap_R_AddCoronaToScene( start, colorNorm[0], colorNorm[1], colorNorm[2], 0, cent->currentState.number, qfalse);
		}
	}

}



/*
==============
CG_RumbleEfx 
==============
*/
void CG_RumbleEfx ( float pitch, float yaw ) {
	float	pitchRecoilAdd, pitchAdd;
	float	yawRandom;
	vec3_t	recoil;

	//
	pitchRecoilAdd = 0;
	pitchAdd = 0;
	yawRandom = 0;
	//

	if (pitch < 1)
		pitch = 1;

	pitchRecoilAdd = pow(random(),8)*(10 + VectorLength(cg.snap->ps.velocity)/5);
	pitchAdd = (rand()%(int)pitch) - (pitch*0.5);//5
	yawRandom = yaw;//2

	pitchRecoilAdd *= 0.5;
	pitchAdd *= 0.5;
	yawRandom *= 0.5;

	// calc the recoil

	// xkan, 11/04/2002 - the following used to be "recoil[YAW] = crandom()*yawRandom()"
	// but that seems to skew the effect either to the left or to the right for long streches
	// of time. The idea here is to keep it skewed for short period of time and then switches
	// to the other direction - switch the sign of recoil[YAW] when random() < 0.05 and keep the
	// sign otherwise. This seems better at balancing out the effect.
	if (cg.kickAVel[YAW] > 0)
	{
		if (random() < 0.05)
			recoil[YAW] = -random()*yawRandom;
		else
			recoil[YAW] = random()*yawRandom;
	}
	else if (cg.kickAVel[YAW] < 0)
	{
		if (random() < 0.05)
			recoil[YAW] = random()*yawRandom;
		else
			recoil[YAW] = -random()*yawRandom;
	}
	else
	{
		if (random() < 0.5)
			recoil[YAW] = random()*yawRandom;
		else
			recoil[YAW] = -random()*yawRandom;
	}

	recoil[ROLL] = -recoil[YAW];	// why not
	recoil[PITCH] = -pitchAdd;
	// scale it up a bit (easier to modify this while tweaking)
	VectorScale( recoil, 30, recoil );
	// set the recoil
	VectorCopy( recoil, cg.kickAVel );
	// set the recoil
	cg.recoilPitch -= pitchRecoilAdd;
}

#define MAX_SMOKESPRITES 512
#define SMOKEBOMB_DISTANCEBETWEENSPRITES 16.f
#define SMOKEBOMB_SPAWNRATE 10
#define SMOKEBOMB_SMOKEVELOCITY ((640.f - 16.f)/8)/1000.f	// units per msec

typedef struct smokesprite_s {
	struct smokesprite_s *next;
	struct smokesprite_s *prev;				// this one is only valid for alloced smokesprites

	vec3_t pos;
	vec4_t colour;

	vec3_t dir;
	float dist;
	float size;

	centity_t *smokebomb;
} smokesprite_t;

static smokesprite_t SmokeSprites[MAX_SMOKESPRITES];
static int SmokeSpriteCount = 0;
static smokesprite_t *firstfreesmokesprite;			// pointer to the first free smokepuff in the SmokeSprites pool
static smokesprite_t *lastusedsmokesprite;			// pointer to the last used smokepuff

void InitSmokeSprites( void ) {
	int i;

	memset( &SmokeSprites, 0, sizeof(SmokeSprites) );
	for( i = 0; i < MAX_SMOKESPRITES - 1; i++ ) {
		SmokeSprites[i].next = &SmokeSprites[i+1];
	}

	firstfreesmokesprite = &SmokeSprites[0];
	lastusedsmokesprite = NULL;
	SmokeSpriteCount = 0;
}

static smokesprite_t *AllocSmokeSprite( void ) {
	smokesprite_t *alloc;

	if( SmokeSpriteCount >= MAX_SMOKESPRITES )
		return( NULL );

	alloc = firstfreesmokesprite;

	firstfreesmokesprite = alloc->next;
	
	if( lastusedsmokesprite )
		lastusedsmokesprite->next = alloc;

	alloc->next = NULL;
	alloc->prev = lastusedsmokesprite;
	lastusedsmokesprite = alloc;

	SmokeSpriteCount++;
	return( alloc );
}

// Returns previous alloced smokesprite in list (or NULL when there are no more alloced smokesprites left)
static smokesprite_t *DeAllocSmokeSprite( smokesprite_t *dealloc ) {
	smokesprite_t *ret_smokesprite;

	if( dealloc->prev )
		dealloc->prev->next = dealloc->next;

	if( dealloc->next )
		dealloc->next->prev = dealloc->prev;
	else { // no next particle, so this particle was 'lastusedsmokesprite'
		lastusedsmokesprite = dealloc->prev;
		if( lastusedsmokesprite ) // incase that there was no previous particle (happens when there is only one particle and this one gets dealloced)
			lastusedsmokesprite->next = NULL;
	}

	ret_smokesprite = dealloc->prev;

	memset( dealloc, 0, sizeof(smokesprite_t) );
	dealloc->next = firstfreesmokesprite;
	firstfreesmokesprite = dealloc;

	SmokeSpriteCount--;
	return( ret_smokesprite );
}

static qboolean CG_SmokeSpritePhysics( smokesprite_t *smokesprite, const float dist ) {
	trace_t tr;
	vec3_t oldpos;
	//vec3_t mins, maxs;

	VectorCopy( smokesprite->pos, oldpos );
	VectorMA( oldpos, dist, smokesprite->dir, smokesprite->pos );

	smokesprite->dist += dist;

	smokesprite->size += 1.25f * dist;

	// see if we hit a solid
	// FIXME: use mins and max with smoke sprite  minimum radius and then expand to max possible distance or real current sprite size?
	// would definately look nice I think
	//VectorSet( maxs, .3f * smokesprite->size, .3f * smokesprite->size, .3f * smokesprite->size );
	//VectorNegate( maxs, mins );
	//CG_Trace( &tr, oldpos, mins, maxs, smokesprite->pos, -1, CONTENTS_SOLID );
	CG_Trace( &tr, oldpos, NULL, NULL, smokesprite->pos, -1, CONTENTS_SOLID );

	if( tr.fraction != 1.f ) {
		//float dot;

		if( smokesprite->dist < 24.f ) {
			return( qfalse );
		}
		VectorCopy( tr.endpos, smokesprite->pos );

		// bounce off
		//dot = DotProduct( smokesprite->dir, tr.plane.normal );
		//VectorMA( smokesprite->dir, -2*dot, tr.plane.normal, smokesprite->dir );
		//VectorScale( smokesprite->dir, .25f, smokesprite->dir );
	}// else {
	//	smokesprite->size += 1.25f * dist;
	//}

	return( qtrue );
}

qboolean CG_SpawnSmokeSprite( centity_t *cent, float dist ) {
	smokesprite_t *smokesprite = AllocSmokeSprite();

	if( smokesprite ) {
		smokesprite->smokebomb = cent;
		//VectorCopy( cent->lerpOrigin, smokesprite->pos );
		//smokesprite->pos[2] += 32;
		VectorCopy( cent->origin2, smokesprite->pos );		
		VectorCopy( bytedirs[rand()%NUMVERTEXNORMALS], smokesprite->dir );
		smokesprite->dir[2] *= .5f;
		smokesprite->size = 16.f;
		smokesprite->colour[0] = .35f; // + crandom() * .1f;
		smokesprite->colour[1] = smokesprite->colour[0];
		smokesprite->colour[2] = smokesprite->colour[0];
		smokesprite->colour[3] = .8f;

		// Advance sprite
		if( !CG_SmokeSpritePhysics( smokesprite, dist ) ) {
			DeAllocSmokeSprite( smokesprite );
			return( qfalse );
		} else {
			cent->miscTime++;
		}
	}

	return( qtrue );
}

void CG_RenderSmokeGrenadeSmoke( centity_t *cent, const weaponInfo_t *weapon ) {
	//int numSpritesForRadius, numNewSpritesNeeded = 0;
	int spritesNeeded = 0;
	smokesprite_t *smokesprite;
	float spawnrate = ( 1.f / SMOKEBOMB_SPAWNRATE ) * 1000.f;

	if( cent->currentState.effect1Time == 16 ) {
		cent->miscTime = 0;
		cent->lastFuseSparkTime = 0;	// last spawn time
		cent->muzzleFlashTime = 0;		// delta time
		cent->dl_atten = 0;
		return;
	}

	if( cent->currentState.effect1Time > 16 ) {
		int volume = 16 + ((cent->currentState.effect1Time/640.f)*(100-16));

		if( !cent->dl_atten ||
			cent->currentState.pos.trType != TR_STATIONARY ||
			( cent->currentState.groundEntityNum != ENTITYNUM_WORLD && !VectorCompare( cent->lastLerpOrigin, cent->lerpOrigin ) ) ) {
			trace_t tr;

			VectorCopy( cent->lerpOrigin, cent->origin2 );
			cent->origin2[2] += 32;
			CG_Trace( &tr, cent->currentState.pos.trBase, NULL, NULL, cent->origin2, -1, CONTENTS_SOLID );

			if( tr.startsolid ) {
				cent->dl_atten = 2;	
			} else {
				VectorCopy( tr.endpos, cent->origin2 );
				cent->dl_atten = 1;
			}
		}

		trap_S_AddLoopingSound( cent->lerpOrigin, vec3_origin, weapon->overheatSound, volume, 0 );

		// emitter is stuck in solid
		if( cent->dl_atten == 2 ) {
			return;
		}

		// Number of sprites for radius calculation:
		// lifetime of a sprite : (.5f * radius) / velocity
		// number of sprites in a row: radius / SMOKEBOMB_DISTANCEBETWEENSPRITES
//		numSpritesForRadius = cent->currentState.effect1Time / SMOKEBOMB_DISTANCEBETWEENSPRITES;

//		numSpritesForRadius = cent->currentState.effect1Time / ((((640.f - 16.f)/16)/1000.f) * cg.frametime);
//		numNewSpritesNeeded = numSpritesForRadius - cent->miscTime;

//		CG_Printf( "numSpritesForRadius: %i / numNewSpritesNeeded: %i / cent->miscTime: %i\n", numSpritesForRadius, numNewSpritesNeeded, cent->miscTime );

		if( cg.oldTime && cent->lastFuseSparkTime != cg.time ) {
			cent->muzzleFlashTime += cg.frametime;
			spritesNeeded = cent->muzzleFlashTime / spawnrate;
			cent->muzzleFlashTime -= ( spawnrate * spritesNeeded  );
			cent->lastFuseSparkTime = cg.time;
		}

//		if( spritesNeeded + cent->miscTime < 40 )
//			spritesNeeded = 40 - cent->miscTime; 

		if( !spritesNeeded )
			return;
		else if( spritesNeeded == 1 ) {
			// this is theoretically fine, till the smokegrenade ends up in a solid
			//while( !CG_SpawnSmokeSprite( cent, 0.f ) );

			// this is better
			if( !CG_SpawnSmokeSprite( cent, 0.f ) )
				// try again, just in case, so we don't get lots of gaps and remain quite constant
				CG_SpawnSmokeSprite( cent, 0.f );
		} else {
//			float lerpfrac = 1.0f / (float)spritesNeeded;
			float lerp = 1.0f;
			float dtime;

			for( dtime = spritesNeeded * spawnrate; dtime > 0; dtime-=spawnrate ) {
				// this is theoretically fine, till the smokegrenade ends up in a solid
				//while( !CG_SpawnSmokeSprite( cent, lerp * cg.frametime * SMOKEBOMB_SMOKEVELOCITY ) );

				// this is better
				if( !CG_SpawnSmokeSprite( cent, lerp * cg.frametime * SMOKEBOMB_SMOKEVELOCITY ) )
					// try again, just in case, so we don't get lots of gaps and remain quite constant
					CG_SpawnSmokeSprite( cent, lerp * cg.frametime * SMOKEBOMB_SMOKEVELOCITY );
			}
		}
	} else if ( cent->currentState.effect1Time == -1 ) {
		// unlink smokesprites from smokebomb
		if( cent->miscTime > 0 ) {
			smokesprite = lastusedsmokesprite;
			while( smokesprite ) {
				if( smokesprite->smokebomb == cent ) {
					smokesprite->smokebomb = NULL;
					cent->miscTime--;
				}

				smokesprite = smokesprite->prev;
			}
		}
	}
}

void CG_AddSmokeSprites( void ) {
	smokesprite_t *smokesprite;
	qhandle_t shader;
	byte color[4];
	polyVert_t verts[4];
	vec3_t top, bottom;
	vec3_t right, up, tmp;
	float radius;
	float halfSmokeSpriteWidth, halfSmokeSpriteHeight;
	float dist = SMOKEBOMB_SMOKEVELOCITY * cg.frametime;

	smokesprite = lastusedsmokesprite;
	while( smokesprite ) {
		if( smokesprite->smokebomb && !smokesprite->smokebomb->currentValid ) {
			smokesprite = smokesprite->prev;
			continue;
		}

		// Do physics
		if( !CG_SmokeSpritePhysics( smokesprite, dist ) ) {
			if( smokesprite->smokebomb )
				smokesprite->smokebomb->miscTime--;
			smokesprite = DeAllocSmokeSprite( smokesprite );
			continue;
		}

		if( smokesprite->smokebomb )
			radius = smokesprite->smokebomb->currentState.effect1Time;
		else
			radius = -1.f;

		if( radius < 0 )
			radius = 640.f;	// max radius

		// Expire sprites
		if( smokesprite->dist > radius * .5f ) {
			if( smokesprite->smokebomb )
				smokesprite->smokebomb->miscTime--;

			smokesprite = DeAllocSmokeSprite( smokesprite );
			continue;
		}

		// Now render it
		halfSmokeSpriteWidth = 0.5f * smokesprite->size;
		halfSmokeSpriteHeight = 0.5f * smokesprite->size;

		VectorCopy( cg.refdef_current->viewaxis[1], tmp );
		RotatePointAroundVector( right, cg.refdef_current->viewaxis[0], tmp, 0 );
		CrossProduct( cg.refdef_current->viewaxis[0], right, up );

		VectorMA( smokesprite->pos, halfSmokeSpriteHeight, up, top );
		VectorMA( smokesprite->pos, -halfSmokeSpriteHeight, up, bottom );

		color[0] = smokesprite->colour[0] * 0xff;
		color[1] = smokesprite->colour[1] * 0xff;
		color[2] = smokesprite->colour[2] * 0xff;
		color[3] = smokesprite->colour[3] * 0xff;

		// fadeout
		if( smokesprite->dist > (radius * .5f * .8f) ) {
			color[3] = (smokesprite->colour[3] -  smokesprite->colour[3] * ((smokesprite->dist - (radius * .5f * .8f))/((radius * .5f)-(radius * .5f * .8f)))) * 0xff;
		} else {
			color[3] = smokesprite->colour[3] * 0xff;
		}

		VectorMA( top, halfSmokeSpriteWidth, right, verts[0].xyz );
		verts[0].st[0] = 1;
		verts[0].st[1] = 0;
		memcpy( verts[0].modulate, color, 4 );

		VectorMA( top, -halfSmokeSpriteWidth, right, verts[1].xyz );
		verts[1].st[0] = 0;
		verts[1].st[1] = 0;
		memcpy( verts[1].modulate, color, 4 );

		VectorMA( bottom, -halfSmokeSpriteWidth, right, verts[2].xyz );
		verts[2].st[0] = 0;
		verts[2].st[1] = 1;
		memcpy( verts[2].modulate, color, 4 );

		VectorMA( bottom, halfSmokeSpriteWidth, right, verts[3].xyz );
		verts[3].st[0] = 1;
		verts[3].st[1] = 1;
		memcpy( verts[3].modulate, color, 4 );

		shader = cgs.media.smokePuffShader;

		trap_R_AddPolyToScene( shader, 4, verts );

		smokesprite = smokesprite->prev;
	}
}
