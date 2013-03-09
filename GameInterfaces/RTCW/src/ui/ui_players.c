// ui_players.c

#include "ui_local.h"

#define UI_TIMER_GESTURE        2300
#define UI_TIMER_JUMP           1000
#define UI_TIMER_LAND           130
#define UI_TIMER_WEAPON_SWITCH  300
#define UI_TIMER_ATTACK         500
#define UI_TIMER_MUZZLE_FLASH   20
#define UI_TIMER_WEAPON_DELAY   250

#define JUMP_HEIGHT             56

#define SWINGSPEED              0.3

#define SPIN_SPEED              0.9
#define COAST_TIME              1000

static int dp_realtime;
static float jumpHeight;

/*
===============
UI_PlayerInfo_SetWeapon
===============
*/
static void UI_PlayerInfo_SetWeapon( playerInfo_t *pi, weapon_t weaponNum ) {
	gitem_t *   item;
	char path[MAX_QPATH];

	pi->currentWeapon = weaponNum;
tryagain:
	pi->realWeapon = weaponNum;
	pi->weaponModel = 0;
	pi->barrelModel = 0;
	pi->flashModel = 0;

	if ( weaponNum == WP_NONE ) {
		return;
	}

	// NERVE - SMF - multiplayer only hack to show correct panzerfaust and venom barrel
	if ( weaponNum == WP_PANZERFAUST ) {
		pi->weaponModel = trap_R_RegisterModel( "models/multiplayer/panzerfaust/multi_pf.md3" );
		return;
	} else if ( weaponNum == WP_VENOM ) {
		pi->barrelModel = trap_R_RegisterModel( "models/weapons2/venom/venom_barrel.md3" );
	}
	// -NERVE - SMF

	for ( item = bg_itemlist + 1; item->classname ; item++ ) {
		if ( item->giType != IT_WEAPON ) {
			continue;
		}
		if ( item->giTag == weaponNum ) {
			break;
		}
	}

	if ( item->classname ) {
		pi->weaponModel = trap_R_RegisterModel( item->world_model[0] );
	}

	if ( pi->weaponModel == 0 ) {
		if ( weaponNum == WP_MP40 ) {
			weaponNum = WP_NONE;
			goto tryagain;
		}

		weaponNum = WP_MP40;
		goto tryagain;
	}

	strcpy( path, item->world_model[0] );
	COM_StripExtensionSafe( path, path, sizeof( path ) );
	strcat( path, "_flash.md3" );
	pi->flashModel = trap_R_RegisterModel( path );

	switch ( weaponNum ) {
	case WP_GAUNTLET:
		MAKERGB( pi->flashDlightColor, 0.6, 0.6, 1 );
		break;

	case WP_GRENADE_LAUNCHER:
		MAKERGB( pi->flashDlightColor, 1, 0.7, 0.5 );
		break;

	case WP_ROCKET_LAUNCHER:
		MAKERGB( pi->flashDlightColor, 1, 0.75, 0 );
		break;

	case WP_FLAMETHROWER:
		MAKERGB( pi->flashDlightColor, 0.6, 0.6, 1 );
		break;

	default:
		MAKERGB( pi->flashDlightColor, 1, 1, 1 );
		break;
	}
}

/*
===============
UI_ForceLegsAnim
===============
*/
static void UI_ForceLegsAnim( playerInfo_t *pi, int anim ) {
	pi->legsAnim = ( ( pi->legsAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | anim;

	if ( anim == LEGS_JUMP ) {
		pi->legsAnimationTimer = UI_TIMER_JUMP;
	}
}

/*
===============
UI_ForceTorsoAnim
===============
*/
static void UI_ForceTorsoAnim( playerInfo_t *pi, int anim ) {
	pi->torsoAnim = ( ( pi->torsoAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | anim;

	if ( anim == TORSO_GESTURE ) {
		pi->torsoAnimationTimer = UI_TIMER_GESTURE;
	}

	if ( anim == TORSO_ATTACK || anim == TORSO_ATTACK2 ) {
		pi->torsoAnimationTimer = UI_TIMER_ATTACK;
	}
}

/*
======================
UI_PositionEntityOnTag
======================
*/
static void UI_PositionEntityOnTag( refEntity_t *entity, const refEntity_t *parent,
								   clipHandle_t parentModel, char *tagName ) {
									   int i;
									   orientation_t lerped;

									   // lerp the tag
									   trap_CM_LerpTag( &lerped, (const refEntity_t *)parent, (const char *)tagName, 0 );

									   // FIXME: allow origin offsets along tag?
									   VectorCopy( parent->origin, entity->origin );
									   for ( i = 0 ; i < 3 ; i++ ) {
										   VectorMA( entity->origin, lerped.origin[i], parent->axis[i], entity->origin );
									   }

									   // cast away const because of compiler problems
									   MatrixMultiply( lerped.axis, ( (refEntity_t*)parent )->axis, entity->axis );
									   entity->backlerp = parent->backlerp;
}

/*
======================
UI_PositionRotatedEntityOnTag
======================
*/
static void UI_PositionRotatedEntityOnTag( refEntity_t *entity, const refEntity_t *parent,
										  clipHandle_t parentModel, char *tagName ) {
											  int i;
											  orientation_t lerped;
											  vec3_t tempAxis[3];

											  // lerp the tag
											  trap_CM_LerpTag( &lerped, parent, tagName, 0 );

											  // FIXME: allow origin offsets along tag?
											  VectorCopy( parent->origin, entity->origin );
											  for ( i = 0 ; i < 3 ; i++ ) {
												  VectorMA( entity->origin, lerped.origin[i], parent->axis[i], entity->origin );
											  }

											  // cast away const because of compiler problems
											  MatrixMultiply( entity->axis, ( (refEntity_t *)parent )->axis, tempAxis );
											  MatrixMultiply( lerped.axis, tempAxis, entity->axis );
}

/*
==================
UI_SwingAngles
==================
*/
static void UI_SwingAngles( float destination, float swingTolerance, float clampTolerance,
						   float speed, float *angle, qboolean *swinging ) {
							   float swing;
							   float move;
							   float scale;

							   if ( !*swinging ) {
								   // see if a swing should be started
								   swing = AngleSubtract( *angle, destination );
								   if ( swing > swingTolerance || swing < -swingTolerance ) {
									   *swinging = qtrue;
								   }
							   }

							   if ( !*swinging ) {
								   return;
							   }

							   // modify the speed depending on the delta
							   // so it doesn't seem so linear
							   swing = AngleSubtract( destination, *angle );
							   scale = fabs( swing );
							   if ( scale < swingTolerance * 0.5 ) {
								   scale = 0.5;
							   } else if ( scale < swingTolerance ) {
								   scale = 1.0;
							   } else {
								   scale = 2.0;
							   }

							   // swing towards the destination angle
							   if ( swing >= 0 ) {
								   move = uis.frametime * scale * speed;
								   if ( move >= swing ) {
									   move = swing;
									   *swinging = qfalse;
								   }
								   *angle = AngleMod( *angle + move );
							   } else if ( swing < 0 ) {
								   move = uis.frametime * scale * -speed;
								   if ( move <= swing ) {
									   move = swing;
									   *swinging = qfalse;
								   }
								   *angle = AngleMod( *angle + move );
							   }

							   // clamp to no more than tolerance
							   swing = AngleSubtract( destination, *angle );
							   if ( swing > clampTolerance ) {
								   *angle = AngleMod( destination - ( clampTolerance - 1 ) );
							   } else if ( swing < -clampTolerance ) {
								   *angle = AngleMod( destination + ( clampTolerance - 1 ) );
							   }
}

/*
======================
UI_MovedirAdjustment
======================
*/
static float UI_MovedirAdjustment( playerInfo_t *pi ) {
	vec3_t relativeAngles;
	vec3_t moveVector;

	VectorSubtract( pi->viewAngles, pi->moveAngles, relativeAngles );
	AngleVectors( relativeAngles, moveVector, NULL, NULL );
	if ( Q_fabs( moveVector[0] ) < 0.01 ) {
		moveVector[0] = 0.0;
	}
	if ( Q_fabs( moveVector[1] ) < 0.01 ) {
		moveVector[1] = 0.0;
	}

	if ( moveVector[1] == 0 && moveVector[0] > 0 ) {
		return 0;
	}
	if ( moveVector[1] < 0 && moveVector[0] > 0 ) {
		return 22;
	}
	if ( moveVector[1] < 0 && moveVector[0] == 0 ) {
		return 45;
	}
	if ( moveVector[1] < 0 && moveVector[0] < 0 ) {
		return -22;
	}
	if ( moveVector[1] == 0 && moveVector[0] < 0 ) {
		return 0;
	}
	if ( moveVector[1] > 0 && moveVector[0] < 0 ) {
		return 22;
	}
	if ( moveVector[1] > 0 && moveVector[0] == 0 ) {
		return -45;
	}

	return -22;
}

/*
===============
UI_PlayerAngles
===============
*/
static void UI_PlayerAngles( playerInfo_t *pi, vec3_t legs[3], vec3_t torso[3], vec3_t head[3] ) {
	vec3_t legsAngles, torsoAngles, headAngles;
	float dest;
	float adjust;

	VectorCopy( pi->viewAngles, headAngles );
	headAngles[YAW] = AngleMod( headAngles[YAW] );
	VectorClear( legsAngles );
	VectorClear( torsoAngles );

	// --------- yaw -------------

	// allow yaw to drift a bit
	if ( ( pi->legsAnim & ~ANIM_TOGGLEBIT ) != LEGS_IDLE
		|| ( pi->torsoAnim & ~ANIM_TOGGLEBIT ) != TORSO_STAND  ) {
			// if not standing still, always point all in the same direction
			pi->torso.yawing = qtrue;   // always center
			pi->torso.pitching = qtrue; // always center
			pi->legs.yawing = qtrue;    // always center
	}

	// adjust legs for movement dir
	adjust = UI_MovedirAdjustment( pi );
	legsAngles[YAW] = headAngles[YAW] + adjust;
	torsoAngles[YAW] = headAngles[YAW] + 0.25 * adjust;

	// torso
	UI_SwingAngles( torsoAngles[YAW], 25, 90, SWINGSPEED, &pi->torso.yawAngle, &pi->torso.yawing );
	UI_SwingAngles( legsAngles[YAW], 40, 90, SWINGSPEED, &pi->legs.yawAngle, &pi->legs.yawing );

	torsoAngles[YAW] = pi->torso.yawAngle;
	legsAngles[YAW] = pi->legs.yawAngle;

	// --------- pitch -------------

	// only show a fraction of the pitch angle in the torso
	if ( headAngles[PITCH] > 180 ) {
		dest = ( -360 + headAngles[PITCH] ) * 0.75;
	} else {
		dest = headAngles[PITCH] * 0.75;
	}
	UI_SwingAngles( dest, 15, 30, 0.1, &pi->torso.pitchAngle, &pi->torso.pitching );
	torsoAngles[PITCH] = pi->torso.pitchAngle;

	// pull the angles back out of the hierarchial chain
	AnglesSubtract( headAngles, torsoAngles, headAngles );
	AnglesSubtract( torsoAngles, legsAngles, torsoAngles );

	AnglesSubtract( legsAngles, pi->moveAngles, legsAngles );       // NERVE - SMF

	AnglesToAxis( legsAngles, legs );
	AnglesToAxis( torsoAngles, torso );
	AnglesToAxis( headAngles, head );
}

/*
===============
UI_PlayerFloatSprite
===============
*/
static void UI_PlayerFloatSprite( playerInfo_t *pi, vec3_t origin, qhandle_t shader ) {
	refEntity_t ent;

	memset( &ent, 0, sizeof( ent ) );
	VectorCopy( origin, ent.origin );
	ent.origin[2] += 48;
	ent.reType = RT_SPRITE;
	ent.customShader = shader;
	ent.radius = 10;
	ent.renderfx = 0;
	trap_R_AddRefEntityToScene( &ent );
}

/*
======================
UI_MachinegunSpinAngle
======================
*/
float   UI_MachinegunSpinAngle( playerInfo_t *pi ) {
	int delta;
	float angle;
	int torsoAnim;

	delta = dp_realtime - pi->barrelTime;
	if ( pi->barrelSpinning ) {
		angle = pi->barrelAngle + delta * SPIN_SPEED;
	} else {
		float speed;
		if ( delta > COAST_TIME ) {
			delta = COAST_TIME;
		}

		speed = 0.5 * ( SPIN_SPEED + (float)( COAST_TIME - delta ) / COAST_TIME );
		angle = pi->barrelAngle + delta * speed;
	}

	torsoAnim = pi->torsoAnim  & ~ANIM_TOGGLEBIT;
	if ( torsoAnim == TORSO_ATTACK2 ) {
		torsoAnim = TORSO_ATTACK;
	}
	if ( pi->barrelSpinning == !( torsoAnim == TORSO_ATTACK ) ) {
		pi->barrelTime = dp_realtime;
		pi->barrelAngle = AngleMod( angle );
		pi->barrelSpinning = !!( torsoAnim == TORSO_ATTACK );
	}

	return angle;
}

// NERVE - SMF
/*
===============
UI_GetAnimation
===============
*/
static int UI_GetAnimation( playerInfo_t *pi, const char *name ) {
	int i;

	for ( i = 0; i < pi->numAnimations; i++ ) {
		if ( !Q_stricmp( pi->animations[i].name, name ) ) {
			return pi->animations[i].firstFrame;
		}
	}

	return 0;
}

/*
===============
UI_DrawPlayer
===============
*/
void WM_getWeaponAnim( const char **torso_anim, const char **legs_anim );       // NERVE - SMF

void UI_DrawPlayer( float x, float y, float w, float h, playerInfo_t *pi, int time ) {
	refdef_t refdef;
	refEntity_t legs;
	refEntity_t torso;
	refEntity_t head;
	refEntity_t gun;
	refEntity_t barrel;
	refEntity_t backpack;
	refEntity_t helmet;
	//	refEntity_t		barrel;
	refEntity_t flash;
	vec3_t origin;
	int renderfx;
	vec3_t mins = {-16, -16, -24};
	vec3_t maxs = {16, 16, 32};
	float len;
	float xx;
	vec4_t hcolor = { 1, 0, 0, 0.5 };
	const char      *torso_anim = NULL, *legs_anim = NULL;

	if ( !pi->legsModel || !pi->torsoModel || !pi->headModel || !pi->animations[0].numFrames ) {
		return;
	}

	dp_realtime = time;

	if ( pi->pendingWeapon != -1 && dp_realtime > pi->weaponTimer ) {
		pi->weapon = pi->pendingWeapon;
		pi->lastWeapon = pi->pendingWeapon;
		pi->pendingWeapon = -1;
		pi->weaponTimer = 0;
		if ( pi->currentWeapon != pi->weapon ) {
			trap_S_StartLocalSound( trap_S_RegisterSound( "sound/weapons/change.wav" ), CHAN_LOCAL );
		}
	}

	UI_AdjustFrom640( &x, &y, &w, &h );

	y -= jumpHeight;

	memset( &refdef, 0, sizeof( refdef ) );
	memset( &legs, 0, sizeof( legs ) );
	memset( &torso, 0, sizeof( torso ) );
	memset( &head, 0, sizeof( head ) );

	refdef.rdflags = RDF_NOWORLDMODEL;

	AxisClear( refdef.viewaxis );

	refdef.x = x;
	refdef.y = y;
	refdef.width = w;
	refdef.height = h;

	refdef.fov_x = (int)( (float)refdef.width / 640.0f * 90.0f );
	xx = refdef.width / tan( refdef.fov_x / 360 * M_PI );
	refdef.fov_y = atan2( refdef.height, xx );
	refdef.fov_y *= ( 360 / M_PI );

	// calculate distance so the player nearly fills the box
	len = 1.01 * ( maxs[2] - mins[2] );                         // NERVE - SMF - changed from 0.7
	origin[0] = len / tan( DEG2RAD( refdef.fov_x ) * 0.5 );
	origin[1] = 0.5 * ( mins[1] + maxs[1] );
	origin[2] = -0.5 * ( mins[2] + maxs[2] );

	refdef.time = dp_realtime;

	trap_R_SetColor( hcolor );
	trap_R_ClearScene();
	trap_R_SetColor( NULL );

	// get the rotation information
	UI_PlayerAngles( pi, legs.axis, torso.axis, head.axis );

	renderfx = RF_LIGHTING_ORIGIN | RF_NOSHADOW;

	//
	// add the body
	//
	legs.hModel = pi->legsModel;
	legs.customSkin = pi->legsSkin;
	legs.renderfx = renderfx;

	VectorCopy( origin, legs.origin );
	VectorCopy( origin, legs.lightingOrigin );
	VectorCopy( legs.origin, legs.oldorigin );

	WM_getWeaponAnim( &torso_anim, &legs_anim );

	if ( torso_anim ) {
		legs.torsoFrame = UI_GetAnimation( pi, torso_anim );
		legs.oldTorsoFrame = UI_GetAnimation( pi, torso_anim );
	}
	legs.torsoBacklerp = 0; //torso.backlerp;

	if ( legs_anim ) {
		legs.frame = UI_GetAnimation( pi, legs_anim );
		legs.oldframe = UI_GetAnimation( pi, legs_anim );
	}
	legs.backlerp = 0;

	memcpy( legs.torsoAxis, torso.axis, sizeof( torso.axis ) );
	torso = legs;

	trap_R_AddRefEntityToScene( &torso );

	//
	// add the head
	//
	head.hModel = pi->headModel;
	if ( !head.hModel ) {
		return;
	}
	head.customSkin = pi->headSkin;

	VectorCopy( origin, head.lightingOrigin );

	UI_PositionRotatedEntityOnTag( &head, &torso, pi->torsoModel, "tag_head" );

	head.renderfx = renderfx;

	trap_R_AddRefEntityToScene( &head );

	//
	// add the gun
	//
	if ( pi->currentWeapon != WP_NONE ) {
		memset( &gun, 0, sizeof( gun ) );
		gun.hModel = pi->weaponModel;
		VectorCopy( origin, gun.lightingOrigin );
		UI_PositionEntityOnTag( &gun, &torso, pi->torsoModel, "tag_weapon" );
		gun.renderfx = renderfx;
		trap_R_AddRefEntityToScene( &gun );
	}

	//
	// add the gun barrel
	//
	if ( pi->currentWeapon != WP_NONE && pi->barrelModel ) {
		memset( &barrel, 0, sizeof( barrel ) );
		barrel.hModel = pi->barrelModel;
		VectorCopy( origin, barrel.lightingOrigin );
		UI_PositionEntityOnTag( &barrel, &gun, pi->weaponModel, "tag_barrel" );
		barrel.renderfx = renderfx;
		trap_R_AddRefEntityToScene( &barrel );
	}

	//
	// add muzzle flash
	//
	if ( dp_realtime <= pi->muzzleFlashTime ) {
		if ( pi->flashModel ) {
			memset( &flash, 0, sizeof( flash ) );
			flash.hModel = pi->flashModel;
			VectorCopy( origin, flash.lightingOrigin );
			UI_PositionEntityOnTag( &flash, &gun, pi->weaponModel, "tag_flash" );
			flash.renderfx = renderfx;
			trap_R_AddRefEntityToScene( &flash );
		}

		// make a dlight for the flash
		if ( pi->flashDlightColor[0] || pi->flashDlightColor[1] || pi->flashDlightColor[2] ) {
			trap_R_AddLightToScene( flash.origin, 200 + ( rand() & 31 ), pi->flashDlightColor[0],
				pi->flashDlightColor[1], pi->flashDlightColor[2], 0 );
		}
	}

	//
	// add the backpack
	//
	if ( pi->backpackModel ) {
		memset( &backpack, 0, sizeof( backpack ) );
		backpack.hModel = pi->backpackModel;
		VectorCopy( origin, backpack.lightingOrigin );
		UI_PositionEntityOnTag( &backpack, &torso, pi->torsoModel, "tag_back" );
		backpack.renderfx = renderfx;
		trap_R_AddRefEntityToScene( &backpack );
	}

	//
	// add the helmet
	//
	if ( pi->helmetModel ) {
		memset( &helmet, 0, sizeof( helmet ) );
		helmet.hModel = pi->helmetModel;
		VectorCopy( origin, helmet.lightingOrigin );
		UI_PositionEntityOnTag( &helmet, &head, pi->headModel, "tag_mouth" );
		helmet.renderfx = renderfx;
		trap_R_AddRefEntityToScene( &helmet );
	}

	//
	// add the chat icon
	//
	if ( pi->chat ) {
		UI_PlayerFloatSprite( pi, origin, trap_R_RegisterShaderNoMip( "sprites/balloon3" ) );
	}

	//
	// add an accent light
	//
	origin[0] -= 100;   // + = behind, - = in front
	origin[1] += 100;   // + = left, - = right
	origin[2] += 100;   // + = above, - = below
	trap_R_AddLightToScene( origin, 1000, 1.0, 1.0, 1.0, 0 );

	origin[0] -= 100;
	origin[1] -= 100;
	origin[2] -= 100;
	trap_R_AddLightToScene( origin, 1000, 1.0, 1.0, 1.0, 0 );

	trap_R_RenderScene( &refdef );
}

/*
==========================
UI_RegisterClientSkin
==========================
*/
static qboolean UI_RegisterClientSkin( playerInfo_t *pi, const char *modelName, const char *skinName ) {
	char filename[MAX_QPATH];

	Com_sprintf( filename, sizeof( filename ), "models/players/%s/body_%s.skin", modelName, skinName );     // NERVE - SMF - make this work with wolf
	pi->legsSkin = trap_R_RegisterSkin( filename );

	Com_sprintf( filename, sizeof( filename ), "models/players/%s/body_%s.skin", modelName, skinName ); // NERVE - SMF - make this work with wolf
	pi->torsoSkin = trap_R_RegisterSkin( filename );

	Com_sprintf( filename, sizeof( filename ), "models/players/%s/head_%s.skin", modelName, skinName );
	pi->headSkin = trap_R_RegisterSkin( filename );

	if ( !pi->legsSkin || !pi->torsoSkin || !pi->headSkin ) {
		return qfalse;
	}

	return qtrue;
}

/*
============
AnimParseAnimConfig

returns qfalse if error, qtrue otherwise
============
*/
static qboolean AnimParseAnimConfig( playerInfo_t *animModelInfo, const char *filename, const char *input ) {
	char    *text_p, *token;
	animation_t *animations;
	headAnimation_t *headAnims;
	int i, fps, skip = -1;

	animations = animModelInfo->animations;
	animModelInfo->numAnimations = 0;

	text_p = (char *)input;
	COM_BeginParseSession( "AnimParseAnimConfig" );

	animModelInfo->footsteps = FOOTSTEP_NORMAL;
	VectorClear( animModelInfo->headOffset );
	animModelInfo->gender = GENDER_MALE;
	animModelInfo->isSkeletal = qfalse;
	animModelInfo->version = 0;

	// read optional parameters
	while ( 1 ) {
		token = COM_Parse( &text_p );
		if ( !token ) {
			break;
		}
		if ( !Q_stricmp( token, "footsteps" ) ) {
			token = COM_Parse( &text_p );
			if ( !token ) {
				break;
			}
			if ( !Q_stricmp( token, "default" ) || !Q_stricmp( token, "normal" ) ) {
				animModelInfo->footsteps = FOOTSTEP_NORMAL;
			} else if ( !Q_stricmp( token, "boot" ) ) {
				animModelInfo->footsteps = FOOTSTEP_BOOT;
			} else if ( !Q_stricmp( token, "flesh" ) ) {
				animModelInfo->footsteps = FOOTSTEP_FLESH;
			} else if ( !Q_stricmp( token, "mech" ) ) {
				animModelInfo->footsteps = FOOTSTEP_MECH;
			} else if ( !Q_stricmp( token, "energy" ) ) {
				animModelInfo->footsteps = FOOTSTEP_ENERGY;
			}

			continue;
		} else if ( !Q_stricmp( token, "headoffset" ) ) {
			for ( i = 0 ; i < 3 ; i++ ) {
				token = COM_Parse( &text_p );
				if ( !token ) {
					break;
				}
				animModelInfo->headOffset[i] = atof( token );
			}
			continue;
		} else if ( !Q_stricmp( token, "sex" ) ) {
			token = COM_Parse( &text_p );
			if ( !token ) {
				break;
			}
			if ( token[0] == 'f' || token[0] == 'F' ) {
				animModelInfo->gender = GENDER_FEMALE;
			} else if ( token[0] == 'n' || token[0] == 'N' ) {
				animModelInfo->gender = GENDER_NEUTER;
			} else {
				animModelInfo->gender = GENDER_MALE;
			}
			continue;
		} else if ( !Q_stricmp( token, "version" ) ) {
			token = COM_Parse( &text_p );
			if ( !token ) {
				break;
			}
			animModelInfo->version = atoi( token );
			continue;
		} else if ( !Q_stricmp( token, "skeletal" ) ) {
			animModelInfo->isSkeletal = qtrue;
			continue;
		}

		if ( animModelInfo->version < 2 ) {
			// if it is a number, start parsing animations
			if ( token[0] >= '0' && token[0] <= '9' ) {
				text_p -= strlen( token );    // unget the token
				break;
			}
		}

		// STARTANIMS marks the start of the animations
		if ( !Q_stricmp( token, "STARTANIMS" ) ) {
			break;
		}
	}

	// read information for each frame
	for ( i = 0 ; ( animModelInfo->version > 1 ) || ( i < MAX_ANIMATIONS ) ; i++ ) {
		token = COM_Parse( &text_p );
		if ( !token ) {
			break;
		}

		if ( animModelInfo->version > 1 ) {   // includes animation names at start of each line
			if ( !Q_stricmp( token, "ENDANIMS" ) ) {  // end of animations
				break;
			}

			Q_strncpyz( animations[i].name, token, sizeof( animations[i].name ) );
			// convert to all lower case
			Q_strlwr( animations[i].name );
			//
			token = COM_ParseExt( &text_p, qfalse );
		} else {
			// just set it to the equivalent animStrings[]
			Q_strncpyz( animations[i].name, animStrings[i], sizeof( animations[i].name ) );
			// convert to all lower case
			Q_strlwr( animations[i].name );
		}

		animations[i].firstFrame = atoi( token );

		if ( !animModelInfo->isSkeletal ) { // skeletal models dont require adjustment
			// leg only frames are adjusted to not count the upper body only frames
			if ( i == LEGS_WALKCR ) {
				skip = animations[LEGS_WALKCR].firstFrame - animations[TORSO_GESTURE].firstFrame;
			}
			if ( i >= LEGS_WALKCR ) {
				animations[i].firstFrame -= skip;
			}
		}

		token = COM_ParseExt( &text_p, qfalse );
		animations[i].numFrames = atoi( token );

		token = COM_ParseExt( &text_p, qfalse );
		animations[i].loopFrames = atoi( token );

		token = COM_ParseExt( &text_p, qfalse );
		fps = atof( token );
		if ( fps == 0 ) {
			fps = 1;
		}
		animations[i].frameLerp = 1000 / fps;
		animations[i].initialLerp = 1000 / fps;

		// movespeed
		token = COM_ParseExt( &text_p, qfalse );
		animations[i].moveSpeed = atoi( token );

		// animation blending
		token = COM_ParseExt( &text_p, qfalse );    // must be on same line
		if ( !token ) {
			animations[i].animBlend = 0;
		} else {
			animations[i].animBlend = atoi( token );
		}

		// calculate the duration
		animations[i].duration = animations[i].initialLerp
			+ animations[i].frameLerp * animations[i].numFrames
			+ animations[i].animBlend;

		// get the nameHash
		animations[i].nameHash = BG_StringHashValue( animations[i].name );

		if ( !Q_strncmp( animations[i].name, "climb", 5 ) ) {
			animations[i].flags |= ANIMFL_LADDERANIM;
		}
		if ( strstr( animations[i].name, "firing" ) ) {
			animations[i].flags |= ANIMFL_FIRINGANIM;
			animations[i].initialLerp = 40;
		}
	}

	animModelInfo->numAnimations = i;

	if ( animModelInfo->version < 2 && i != MAX_ANIMATIONS ) {
		return qfalse;
	}

	return qtrue;           // NERVE - SMF - blah

	// check for head anims
	token = COM_Parse( &text_p );
	if ( token && token[0] ) {
		if ( animModelInfo->version < 2 || !Q_stricmp( token, "HEADFRAMES" ) ) {
			// read information for each head frame
			for ( i = 0 ; i < MAX_HEAD_ANIMS ; i++ ) {
				token = COM_Parse( &text_p );
				if ( !token || !token[0] ) {
					break;
				}

				if ( animModelInfo->version > 1 ) {   // includes animation names at start of each line
					// just throw this information away, not required for head
					token = COM_ParseExt( &text_p, qfalse );
					if ( !token || !token[0] ) {
						break;
					}
				}

				if ( !i ) {
					skip = atoi( token );
				}

				headAnims[i].firstFrame = atoi( token );
				// modify according to last frame of the main animations, since the head is totally seperate
				headAnims[i].firstFrame -= animations[MAX_ANIMATIONS - 1].firstFrame + animations[MAX_ANIMATIONS - 1].numFrames + skip;

				token = COM_ParseExt( &text_p, qfalse );
				if ( !token || !token[0] ) {
					break;
				}
				headAnims[i].numFrames = atoi( token );

				// skip the movespeed
				token = COM_ParseExt( &text_p, qfalse );
			}

			if ( i != MAX_HEAD_ANIMS ) {
				return qfalse;
			}
		}
	}

	return qtrue;
}

/*
======================
UI_ParseAnimationFile
======================
*/
static qboolean UI_ParseAnimationFile( const char *filename, playerInfo_t *pi ) {
	char        *text_p, *prev;
	int len;
	int i;
	char        *token;
	float fps;
	int skip;
	char text[20000];
	fileHandle_t f;

	token = NULL;
	i = 0;
	fps = 0;
	prev = 0;

	memset( pi->animations, 0, sizeof( animation_t ) * MAX_ANIMATIONS );

	// load the file
	len = trap_FS_FOpenFile( filename, &f, FS_READ );
	if ( len <= 0 ) {
		return qfalse;
	}
	if ( len >= ( sizeof( text ) - 1 ) ) {
		Com_Printf( "File %s too long\n", filename );
		return qfalse;
	}
	trap_FS_Read( text, len, f );
	text[len] = 0;
	trap_FS_FCloseFile( f );

	// parse the text
	text_p = text;
	skip = 0;   // quite the compiler warning

	// NERVE - SMF - new!!!!
	AnimParseAnimConfig( pi, filename, text );
	return qtrue;
}

/*
==========================
UI_RegisterClientModelname
==========================
*/
int WM_getWeaponIndex();

qboolean UI_RegisterClientModelname( playerInfo_t *pi, const char *modelSkinName ) {
	char modelName[MAX_QPATH];
	char skinName[MAX_QPATH];
	char filename[MAX_QPATH];
	char        *slash;
	const char* backpack = NULL;
	const char* helmet = NULL;

	pi->torsoModel = 0;
	pi->headModel = 0;

	if ( !modelSkinName[0] ) {
		return qfalse;
	}

	Q_strncpyz( modelName, modelSkinName, sizeof( modelName ) );

	slash = strchr( modelName, '/' );
	if ( !slash ) {
		// modelName did not include a skin name
		Q_strncpyz( skinName, "default", sizeof( skinName ) );
	} else {
		Q_strncpyz( skinName, slash + 1, sizeof( skinName ) );
		// truncate modelName
		*slash = 0;
	}

	// NERVE - SMF - set weapon
	pi->weapon = WM_getWeaponIndex();
	UI_PlayerInfo_SetWeapon( pi, pi->weapon );

	// NERVE - SMF - determine skin
	{
		const char *team;
		const char *playerClass;
		int var, teamval;

		if ( !strcmp( modelSkinName, "multi" ) ) {
			teamval = 1;
			team = "blue";
		} else {
			teamval = 0;
			team = "red";
		}

		var = trap_Cvar_VariableValue( "mp_playerType" );

		if ( var == 0 ) {
			playerClass = "soldier";

			if ( teamval == 1 ) {
				backpack = "acc/backpack/backpack_sol.md3";
				helmet = "acc/helmet_american/sol.md3";
			} else {
				backpack = "acc/backpack/backpack_german_sol.md3";
				helmet = "acc/helmet_german/helmet_sol.md3";
			}
		} else if ( var == 1 ) {
			playerClass = "medic";

			if ( teamval == 1 ) {
				backpack = "acc/backpack/backpack_med.md3";
				helmet = "acc/helmet_american/med.md3";
			} else {
				backpack = "acc/backpack/backpack_german_med.md3";
				helmet = "acc/helmet_german/helmet_med.md3";
			}
		} else if ( var == 2 ) {
			playerClass = "engineer";

			if ( teamval == 1 ) {
				backpack = "acc/backpack/backpack_eng.md3";
				helmet = "acc/helmet_american/eng.md3";
			} else {
				backpack = "acc/backpack/backpack_german_eng.md3";
				helmet = "acc/helmet_german/helmet_eng.md3";
			}
		} else {
			playerClass = "lieutenant";

			if ( teamval == 1 ) {
				backpack = "acc/backpack/backpack_lieu.md3";
				helmet = "acc/helmet_american/lieu.md3";
			} else {
				backpack = "acc/backpack/backpack_german_lieu.md3";
				helmet = "acc/helmet_german/helmet_leiu.md3";
			}
		}

		strcpy( skinName, va( "%s%s1", team, playerClass ) );
	}
	// -NERVE - SMF

	// load cmodels before models so filecache works
	Com_sprintf( filename, sizeof( filename ), "models/players/%s/body.mds", modelName ); // NERVE - SMF - make this work with wolf
	pi->legsModel = trap_R_RegisterModel( filename );
	if ( !pi->legsModel ) {
		Com_Printf( "Failed to load model file %s\n", filename );
		return qfalse;
	}

	Com_sprintf( filename, sizeof( filename ), "models/players/%s/body.mds", modelName ); // NERVE - SMF - make this work with wolf
	pi->torsoModel = trap_R_RegisterModel( filename );
	if ( !pi->torsoModel ) {
		Com_Printf( "Failed to load model file %s\n", filename );
		return qfalse;
	}

	Com_sprintf( filename, sizeof( filename ), "models/players/%s/head.md3", modelName );
	pi->headModel = trap_R_RegisterModel( filename );
	if ( !pi->headModel ) {
		Com_Printf( "Failed to load model file %s\n", filename );
		return qfalse;
	}

	// NERVE - SMF - load backpack and helmet
	if ( backpack ) {
		pi->backpackModel = trap_R_RegisterModel( va( "models/players/%s/%s", modelName, backpack ) );
	}

	if ( helmet ) {
		pi->helmetModel = trap_R_RegisterModel( va( "models/players/%s/%s", modelName, helmet ) );
	}

	// if any skins failed to load, fall back to default
	if ( !UI_RegisterClientSkin( pi, modelName, skinName ) ) {
		if ( !UI_RegisterClientSkin( pi, modelName, "default" ) ) {
			Com_Printf( "Failed to load skin file: %s : %s\n", modelName, skinName );
			return qfalse;
		}
	}

	// load the animations
	Com_sprintf( filename, sizeof( filename ), "models/players/%s/wolfanim.cfg", modelName );

	if ( !UI_ParseAnimationFile( filename, pi ) ) {         // NERVE - SMF - make this work with wolf
		Com_Printf( "Failed to load animation file %s\n", filename );
		return qfalse;
	}

	return qtrue;
}

/*
===============
UI_PlayerInfo_SetModel
===============
*/
void UI_PlayerInfo_SetModel( playerInfo_t *pi, const char *model ) {
	memset( pi, 0, sizeof( *pi ) );
	UI_RegisterClientModelname( pi, model );
	pi->currentWeapon = pi->weapon;
	pi->lastWeapon = pi->weapon;
	pi->pendingWeapon = -1;
	pi->weaponTimer = 0;
	pi->chat = qfalse;
	pi->newModel = qtrue;
	UI_PlayerInfo_SetWeapon( pi, pi->weapon );
}

/*
===============
UI_PlayerInfo_SetInfo
===============
*/
void UI_PlayerInfo_SetInfo( playerInfo_t *pi, int legsAnim, int torsoAnim, vec3_t viewAngles, vec3_t moveAngles, weapon_t weaponNumber, qboolean chat ) {
	int currentAnim;
	weapon_t weaponNum;

	pi->chat = chat;

	// view angles
	VectorCopy( viewAngles, pi->viewAngles );

	// move angles
	VectorCopy( moveAngles, pi->moveAngles );

	if ( pi->newModel ) {
		pi->newModel = qfalse;

		jumpHeight = 0;
		pi->pendingLegsAnim = 0;
		UI_ForceLegsAnim( pi, legsAnim );
		pi->legs.yawAngle = viewAngles[YAW];
		pi->legs.yawing = qfalse;

		pi->pendingTorsoAnim = 0;
		UI_ForceTorsoAnim( pi, torsoAnim );
		pi->torso.yawAngle = viewAngles[YAW];
		pi->torso.yawing = qfalse;

		if ( weaponNumber != -1 ) {
			pi->weapon = weaponNumber;
			pi->currentWeapon = weaponNumber;
			pi->lastWeapon = weaponNumber;
			pi->pendingWeapon = -1;
			pi->weaponTimer = 0;
			UI_PlayerInfo_SetWeapon( pi, pi->weapon );
		}

		return;
	}

	// weapon
	if ( weaponNumber == -1 ) {
		pi->pendingWeapon = -1;
		pi->weaponTimer = 0;
	} else if ( weaponNumber != WP_NONE ) {
		pi->pendingWeapon = weaponNumber;
		pi->weaponTimer = dp_realtime + UI_TIMER_WEAPON_DELAY;
	}
	weaponNum = pi->lastWeapon;
	pi->weapon = weaponNum;

	if ( torsoAnim == BOTH_DEATH1 || legsAnim == BOTH_DEATH1 ) {
		torsoAnim = legsAnim = BOTH_DEATH1;
		pi->weapon = pi->currentWeapon = WP_NONE;
		UI_PlayerInfo_SetWeapon( pi, pi->weapon );

		jumpHeight = 0;
		pi->pendingLegsAnim = 0;
		UI_ForceLegsAnim( pi, legsAnim );

		pi->pendingTorsoAnim = 0;
		UI_ForceTorsoAnim( pi, torsoAnim );

		return;
	}

	// leg animation
	currentAnim = pi->legsAnim & ~ANIM_TOGGLEBIT;
	if ( legsAnim != LEGS_JUMP && ( currentAnim == LEGS_JUMP || currentAnim == LEGS_LAND ) ) {
		pi->pendingLegsAnim = legsAnim;
	} else if ( legsAnim != currentAnim ) {
		jumpHeight = 0;
		pi->pendingLegsAnim = 0;
		UI_ForceLegsAnim( pi, legsAnim );
	}

	// torso animation
	if ( torsoAnim == TORSO_STAND || torsoAnim == TORSO_STAND2 ) {
		if ( weaponNum == WP_NONE || weaponNum == WP_GAUNTLET ) {
			torsoAnim = TORSO_STAND2;
		} else {
			torsoAnim = TORSO_STAND;
		}
	}

	if ( torsoAnim == TORSO_ATTACK || torsoAnim == TORSO_ATTACK2 ) {
		if ( weaponNum == WP_NONE || weaponNum == WP_GAUNTLET ) {
			torsoAnim = TORSO_ATTACK2;
		} else {
			torsoAnim = TORSO_ATTACK;
		}
		pi->muzzleFlashTime = dp_realtime + UI_TIMER_MUZZLE_FLASH;
		//FIXME play firing sound here
	}

	currentAnim = pi->torsoAnim & ~ANIM_TOGGLEBIT;

	if ( weaponNum != pi->currentWeapon || currentAnim == TORSO_RAISE || currentAnim == TORSO_DROP ) {
		pi->pendingTorsoAnim = torsoAnim;
	} else if ( ( currentAnim == TORSO_GESTURE || currentAnim == TORSO_ATTACK ) && ( torsoAnim != currentAnim ) ) {
		pi->pendingTorsoAnim = torsoAnim;
	} else if ( torsoAnim != currentAnim ) {
		pi->pendingTorsoAnim = 0;
		UI_ForceTorsoAnim( pi, torsoAnim );
	}
}