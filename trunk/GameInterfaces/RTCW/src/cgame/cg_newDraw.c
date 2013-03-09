#include "cg_local.h"
#include "../ui/ui_shared.h"

extern displayContextDef_t cgDC;

// set in CG_ParseTeamInfo
int drawTeamOverlayModificationCount = -1;
int CG_DrawField( int x, int y, int width, int value, int charWidth, int charHeight, qboolean dodrawpic, qboolean leftAlign );      // NERVE - SMF

static void CG_SetSelectedPlayerName() {
	if ( cg_currentSelectedPlayer.integer >= 0 && cg_currentSelectedPlayer.integer < numSortedTeamPlayers ) {
		clientInfo_t *ci = cgs.clientinfo + sortedTeamPlayers[cg_currentSelectedPlayer.integer];
		if ( ci ) {
			trap_Cvar_Set( "cg_selectedPlayerName", ci->name );
			trap_Cvar_Set( "cg_selectedPlayer", va( "%d", sortedTeamPlayers[cg_currentSelectedPlayer.integer] ) );
		}
	} else {
		trap_Cvar_Set( "cg_selectedPlayerName", "Everyone" );
	}
}
int CG_GetSelectedPlayer() {
	if ( cg_currentSelectedPlayer.integer < 0 || cg_currentSelectedPlayer.integer >= numSortedTeamPlayers ) {
		cg_currentSelectedPlayer.integer = 0;
	}
	return cg_currentSelectedPlayer.integer;
}

void CG_SelectNextPlayer() {
	if ( cg_currentSelectedPlayer.integer >= 0 && cg_currentSelectedPlayer.integer < numSortedTeamPlayers ) {
		cg_currentSelectedPlayer.integer++;
	} else {
		cg_currentSelectedPlayer.integer = 0;
	}
	CG_SetSelectedPlayerName();
}

void CG_SelectPrevPlayer() {
	if ( cg_currentSelectedPlayer.integer > 0 && cg_currentSelectedPlayer.integer < numSortedTeamPlayers ) {
		cg_currentSelectedPlayer.integer--;
	} else {
		cg_currentSelectedPlayer.integer = numSortedTeamPlayers;
	}
	CG_SetSelectedPlayerName();
}

static void CG_DrawPlayerArmorValue( rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle ) {
	char num[16];
	int value;
	playerState_t   *ps;

	ps = &cg.snap->ps;

	// NERVE - SMF - don't draw armor in wolfMP
	if ( cgs.gametype >= GT_WOLF ) {
		return;
	}

	value = ps->stats[STAT_ARMOR];

	if ( shader ) {
		trap_R_SetColor( color );
		CG_DrawPic( rect->x, rect->y, rect->w, rect->h, shader );
		trap_R_SetColor( NULL );
	} else {
		Com_sprintf( num, sizeof( num ), "%i", value );
		value = CG_Text_Width( num, scale, 0 );
		CG_Text_Paint( rect->x + ( rect->w - value ) / 2, rect->y + rect->h, scale, color, num, 0, 0, textStyle );
	}
}

/*
==============
weapIconDrawSize
==============
*/
static int weapIconDrawSize( int weap ) {
	switch ( weap ) {
		// weapons to not draw
	case WP_KNIFE:
	case WP_KNIFE2:
		return 0;

		// weapons with 'wide' icons
	case WP_THOMPSON:
	case WP_MP40:
	case WP_STEN:
	case WP_MAUSER:
	case WP_GARAND:
	case WP_VENOM:
	case WP_TESLA:
	case WP_ROCKET_LAUNCHER:
	case WP_PANZERFAUST:
	case WP_FLAMETHROWER:
	case WP_SPEARGUN:
	case WP_BAR:        //----(SA)	added
	case WP_BAR2:       //----(SA)	added
	case WP_FG42:
	case WP_FG42SCOPE:
	case WP_SNOOPERSCOPE:
	case WP_SNIPERRIFLE:
		return 2;
	}

	return 1;
}

/*
==============
CG_DrawPlayerWeaponIcon
==============
*/
static void CG_DrawPlayerWeaponIcon( rectDef_t *rect, qboolean drawHighlighted, int align ) {
	int size;
	int realweap;                   // DHM - Nerve
	qhandle_t icon;
	float scale,halfScale;
	// JPW NERVE
	vec4_t hcolor;

	VectorSet( hcolor,1.f,1.f,1.f );
	hcolor[3] = cg_hudAlpha.value;
	// jpw

	if ( !cg_drawIcons.integer ) {
		return;
	}

	realweap = cg.predictedPlayerState.weapon;

	size = weapIconDrawSize( realweap );

	if ( !size ) {
		return;
	}

	if ( cg_drawWeaponIconFlash.integer ) {
		if ( cg.snap->ps.weaponstate == WEAPON_RELOADING ) {
			VectorCopy( colorYellow, hcolor );
			icon = cg_weapons[ realweap ].weaponIcon[1];
		} else if ( cg.snap->ps.weaponstate == WEAPON_FIRING ) {
			VectorCopy( colorRed, hcolor );
			icon = cg_weapons[ realweap ].weaponIcon[1];
		} else if ( cg.snap->ps.weaponstate == WEAPON_READY ) {
			VectorCopy( colorYellow, hcolor );
			icon = cg_weapons[ realweap ].weaponIcon[0];
		} else {
			VectorCopy( colorWhite, hcolor );
			icon = cg_weapons[ realweap ].weaponIcon[1];
		}
		hcolor[3] = 4.0f;
	} else {
		if ( drawHighlighted ) {
			icon = cg_weapons[ realweap ].weaponIcon[1];
		} else {
			icon = cg_weapons[ realweap ].weaponIcon[0];
		}
	}

	// pulsing grenade icon to help the player 'count' in their head
	if ( cg.predictedPlayerState.grenadeTimeLeft ) {   // grenades and dynamite set this
		// these time differently
		if ( realweap == WP_DYNAMITE || realweap == WP_DYNAMITE2 ) {
			if ( ( ( cg.grenLastTime ) % 1000 ) > ( ( cg.predictedPlayerState.grenadeTimeLeft ) % 1000 ) ) {
				trap_S_StartLocalSound( cgs.media.grenadePulseSound4, CHAN_LOCAL_SOUND );
			}
		} else {
			if ( ( ( cg.grenLastTime ) % 1000 ) < ( ( cg.predictedPlayerState.grenadeTimeLeft ) % 1000 ) ) {
				switch ( cg.predictedPlayerState.grenadeTimeLeft / 1000 ) {
				case 3:
					trap_S_StartLocalSound( cgs.media.grenadePulseSound4, CHAN_LOCAL_SOUND );
					break;
				case 2:
					trap_S_StartLocalSound( cgs.media.grenadePulseSound3, CHAN_LOCAL_SOUND );
					break;
				case 1:
					trap_S_StartLocalSound( cgs.media.grenadePulseSound2, CHAN_LOCAL_SOUND );
					break;
				case 0:
					trap_S_StartLocalSound( cgs.media.grenadePulseSound1, CHAN_LOCAL_SOUND );
					break;
				}
			}
		}

		scale = (float)( ( cg.predictedPlayerState.grenadeTimeLeft ) % 1000 ) / 100.0f;
		halfScale = scale * 0.5f;

		cg.grenLastTime = cg.predictedPlayerState.grenadeTimeLeft;
	} else {
		scale = halfScale = 0;
	}

	if ( icon ) {
		float x, y, w, h;

		if ( size == 1 ) { // draw half width to match the icon asset
			// start at left
			x = rect->x - halfScale;
			y = rect->y - halfScale;
			w = rect->w / 2 + scale;
			h = rect->h + scale;

			switch ( align ) {
			case ITEM_ALIGN_CENTER:
				x += rect->w / 4;
				break;
			case ITEM_ALIGN_RIGHT:
				x += rect->w / 2;
				break;
			case ITEM_ALIGN_LEFT:
			default:
				break;
			}
		} else {
			x = rect->x - halfScale;
			y = rect->y - halfScale;
			w = rect->w + scale;
			h = rect->h + scale;
		}

		trap_R_SetColor( hcolor ); // JPW NERVE
		CG_DrawPic( x, y, w, h, icon );
	}
}

/*
==============
CG_DrawPlayerAmmoIcon
==============
*/
static void CG_DrawPlayerAmmoIcon( rectDef_t *rect, qboolean draw2D ) {
	centity_t   *cent;
	vec3_t angles;
	vec3_t origin;

	cent = &cg_entities[cg.snap->ps.clientNum];

	if ( draw2D || ( !cg_draw3dIcons.integer && cg_drawIcons.integer ) ) {
		qhandle_t icon;
		icon = cg_weapons[ cg.predictedPlayerState.weapon ].ammoIcon;
		if ( icon ) {
			CG_DrawPic( rect->x, rect->y, rect->w, rect->h, icon );
		}
	} else if ( cg_draw3dIcons.integer ) {
		if ( cent->currentState.weapon && cg_weapons[ cent->currentState.weapon ].ammoModel ) {
			VectorClear( angles );
			origin[0] = 70;
			origin[1] = 0;
			origin[2] = 0;
			angles[YAW] = 90 + 20 * sin( cg.time / 1000.0 );
			CG_Draw3DModel( rect->x, rect->y, rect->w, rect->h, cg_weapons[ cent->currentState.weapon ].ammoModel, 0, origin, angles );
		}
	}
}

extern void CG_CheckForCursorHints( void );
#define CURSORHINT_SCALE    10

/*
==============
CG_DrawCursorHints

cg_cursorHints.integer ==
0:	no hints
1:	sin size pulse
2:	one way size pulse
3:	alpha pulse
4+:	static image

==============
*/
static void CG_DrawCursorhint( rectDef_t *rect ) {
	float       *color;
	qhandle_t icon = 0, icon2 = 0;
	float scale, halfscale;

	if ( !cg_cursorHints.integer ) {
		return;
	}

	CG_CheckForCursorHints();

	switch ( cg.cursorHintIcon ) {
	case HINT_NONE:
	case HINT_FORCENONE:
		icon = 0;
		break;
	case HINT_DOOR:
		icon = cgs.media.doorHintShader;
		break;
	case HINT_DOOR_ROTATING:
		icon = cgs.media.doorRotateHintShader;
		break;
	case HINT_DOOR_LOCKED:
		icon = cgs.media.doorLockHintShader;
		break;
	case HINT_DOOR_ROTATING_LOCKED:
		icon = cgs.media.doorRotateLockHintShader;
		break;
	case HINT_MG42:
		icon = cgs.media.mg42HintShader;
		break;
	case HINT_BREAKABLE:
	case HINT_BREAKABLE_DYNAMITE:

		// DHM - Nerve :: We use HINT_BREAKABLE_DYNAMITE in multiplayer
		if ( cgs.gametype >= GT_WOLF && cg.cursorHintIcon == HINT_BREAKABLE_DYNAMITE ) {
			icon = cgs.media.dynamiteHintShader;
		} else {
			icon = cgs.media.breakableHintShader;
		}

		break;
	case HINT_CHAIR:
		icon = cgs.media.notUsableHintShader;

		// only show 'pickupable' if you're not armed, or are armed with a single handed weapon
		if ( !( cg.predictedPlayerState.weapon ) ||
			WEAPS_ONE_HANDED & ( 1 << ( cg.predictedPlayerState.weapon ) )
			) {
				icon = cgs.media.chairHintShader;
		}
		break;
	case HINT_ALARM:
		icon = cgs.media.alarmHintShader;
		break;
	case HINT_HEALTH:
		icon = cgs.media.healthHintShader;
		break;
	case HINT_TREASURE:
		icon = cgs.media.treasureHintShader;
		break;
	case HINT_KNIFE:
		icon = cgs.media.knifeHintShader;
		break;
	case HINT_LADDER:
		icon = cgs.media.ladderHintShader;
		break;
	case HINT_BUTTON:
		icon = cgs.media.buttonHintShader;
		break;
	case HINT_WATER:
		icon = cgs.media.waterHintShader;
		break;
	case HINT_CAUTION:
		icon = cgs.media.cautionHintShader;
		break;
	case HINT_DANGER:
		icon = cgs.media.dangerHintShader;
		break;
	case HINT_SECRET:
		icon = cgs.media.secretHintShader;
		break;
	case HINT_QUESTION:
		icon = cgs.media.qeustionHintShader;
		break;
	case HINT_EXCLAMATION:
		icon = cgs.media.exclamationHintShader;
		break;
	case HINT_CLIPBOARD:
		icon = cgs.media.clipboardHintShader;
		break;
	case HINT_WEAPON:
		icon = cgs.media.weaponHintShader;
		break;
	case HINT_AMMO:
		icon = cgs.media.ammoHintShader;
		break;
	case HINT_ARMOR:
		icon = cgs.media.armorHintShader;
		break;
	case HINT_POWERUP:
		icon = cgs.media.powerupHintShader;
		break;
	case HINT_HOLDABLE:
		icon = cgs.media.holdableHintShader;
		break;
	case HINT_INVENTORY:
		icon = cgs.media.inventoryHintShader;
		break;
	case HINT_EXIT:
		icon = cgs.media.exitHintShader;
		break;

		// cs: no shaders registered
	case HINT_PLYR_FRIEND:
	case HINT_PLYR_NEUTRAL:
	case HINT_PLYR_ENEMY:
	case HINT_PLYR_UNKNOWN:
		break;

		// DHM - Nerve :: multiplayer hints
	case HINT_BUILD:
		icon = cgs.media.buildHintShader;
		break;
	case HINT_DISARM:
		icon = cgs.media.disarmHintShader;
		break;
	case HINT_REVIVE:
		icon = cgs.media.reviveHintShader;
		break;
	case HINT_DYNAMITE:
		icon = cgs.media.dynamiteHintShader;
		break;
		// dhm - end

	case HINT_ACTIVATE:
	case HINT_PLAYER:
	default:
		icon = cgs.media.usableHintShader;
		break;
	}

	if ( !icon ) {
		return;
	}

	// color
	color = CG_FadeColor( cg.cursorHintTime, cg.cursorHintFade );
	if ( !color ) {
		trap_R_SetColor( NULL );
		return;
	}

	if ( cg_cursorHints.integer == 3 ) {
		color[3] *= 0.5 + 0.5 * sin( (float)cg.time / 150.0 );
	}

	// size
	if ( cg_cursorHints.integer >= 3 ) {   // no size pulsing
		scale = halfscale = 0;
	} else {
		if ( cg_cursorHints.integer == 2 ) {
			scale = (float)( ( cg.cursorHintTime ) % 1000 ) / 100.0f; // one way size pulse
		} else {
			scale = CURSORHINT_SCALE * ( 0.5 + 0.5 * sin( (float)cg.time / 150.0 ) ); // sin pulse
		}
		halfscale = scale * 0.5f;
	}

	// set color and draw the hint
	trap_R_SetColor( color );
	CG_DrawPic( rect->x - halfscale, rect->y - halfscale, rect->w + scale, rect->h + scale, icon );

	if ( icon2 ) {
		CG_DrawPic( rect->x - halfscale, rect->y - halfscale, rect->w + scale, rect->h + scale, icon2 );
	}

	trap_R_SetColor( NULL );

	// draw status bar under the cursor hint
	if ( cg.cursorHintValue ) {
		Vector4Set( color, 0, 0, 1, 0.5f );
		CG_FilledBar( rect->x, rect->y + rect->h + 4, rect->w, 8, color, NULL, NULL, (float)cg.cursorHintValue / 255.0f, 0 );
	}
}

/*
==============
CG_DrawPlayerAmmoValue
0 - ammo
1 - clip
==============
*/
int CG_DrawFieldWidth( int x, int y, int width, int value, int charWidth, int charHeight );

static void CG_DrawPlayerAmmoValue( rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle, int type ) {
	int ammovalue, clipvalue;
	centity_t   *cent;
	playerState_t   *ps;
	int weap, startx;
	qboolean special = qfalse;
	qboolean skipammo = qfalse;

	cent = &cg_entities[cg.snap->ps.clientNum];
	ps = &cg.snap->ps;

	weap = cent->currentState.weapon;

	if ( !weap ) {
		return;
	}

	switch ( weap ) {      // some weapons don't draw ammo count text
	case WP_KNIFE:
	case WP_KNIFE2:
	case WP_MEDKIT:
	case WP_PLIERS:
	case WP_SMOKE_GRENADE:
	case WP_AMMO:       // JPW NERVE
	case WP_DYNAMITE:     // JPW NERVE
		return;

	case WP_AKIMBO:
		special = qtrue;
		break;

	case WP_MEDIC_SYRINGE:     // JPW NERVE
	case WP_GRENADE_LAUNCHER:
	case WP_GRENADE_PINEAPPLE:
	case WP_DYNAMITE2:
	case WP_TESLA:
	case WP_FLAMETHROWER:
		skipammo = qtrue;
		if ( type == 0 ) {  // don't draw reserve value, just clip (since these weapons have all their ammo in the clip)
			return;
		}
		break;

	default:
		break;
	}

	// ammo
	ammovalue = cg.snap->ps.ammo[BG_FindAmmoForWeapon( weap )];

	// clip
	clipvalue = ps->ammoclip[BG_FindClipForWeapon( weap )];
	if ( special ) {
		if ( weapAlts[weap] ) {
			clipvalue = ps->ammoclip[weapAlts[weap]];
		}
	}

	// NERVE - SMF
	if ( !skipammo ) {
		startx = CG_DrawFieldWidth( rect->x, rect->y, 4, ammovalue, 20 * scale, 32 * scale );
	} else {
		startx = 0;
	}

	if ( ammovalue > -1 && type == 0 ) {
		if ( shader ) {
			trap_R_SetColor( color );
			CG_DrawPic( rect->x, rect->y, rect->w, rect->h, shader );
			trap_R_SetColor( NULL );
		} else {
			trap_R_SetColor( color );

			// draw the clip count
			CG_DrawField( rect->x, rect->y, 4, ammovalue, 20 * scale, 32 * scale, qtrue, qfalse );
		}

		// draw the slash
		if ( type == 0 ) {
			CG_DrawPic( rect->x - startx - 16, rect->y, 16, 28, trap_R_RegisterShader( "gfx/2d/numbers/slash" ) );
		}
	}

	if ( clipvalue > -1 && type == 1 ) {
		if ( shader ) {
			trap_R_SetColor( color );
			CG_DrawPic( rect->x, rect->y, rect->w, rect->h, shader );
			trap_R_SetColor( NULL );
		} else {
			trap_R_SetColor( color );

			// draw the clip count
			CG_DrawField( rect->x - startx, rect->y, 4, clipvalue, 20 * scale, 32 * scale, qtrue, qfalse );
		}
	}
}

static void CG_DrawPlayerHead( rectDef_t *rect, qboolean draw2D ) {
	vec3_t angles;
	float size;
	float frac;
	float x = rect->x;

	VectorClear( angles );

	if ( cg.damageTime && cg.time - cg.damageTime < DAMAGE_TIME ) {
		float stretch;
		frac = (float)( cg.time - cg.damageTime ) / DAMAGE_TIME;
		size = rect->w * 1.25 * ( 1.5 - frac * 0.5 );

		stretch = size - rect->w * 1.25;
		// kick in the direction of damage
		x -= stretch * 0.5 + cg.damageX * stretch * 0.5;

		cg.headStartYaw = 180 + cg.damageX * 45;

		cg.headEndYaw = 180 + 20 * cos( crandom() * M_PI );
		cg.headEndPitch = 5 * cos( crandom() * M_PI );

		cg.headStartTime = cg.time;
		cg.headEndTime = cg.time + 100 + random() * 2000;
	} else {
		if ( cg.time >= cg.headEndTime ) {
			// select a new head angle
			cg.headStartYaw = cg.headEndYaw;
			cg.headStartPitch = cg.headEndPitch;
			cg.headStartTime = cg.headEndTime;
			cg.headEndTime = cg.time + 100 + random() * 2000;

			cg.headEndYaw = 180 + 20 * cos( crandom() * M_PI );
			cg.headEndPitch = 5 * cos( crandom() * M_PI );
		}

		size = rect->w * 1.25;
	}

	// if the server was frozen for a while we may have a bad head start time
	if ( cg.headStartTime > cg.time ) {
		cg.headStartTime = cg.time;
	}

	frac = ( cg.time - cg.headStartTime ) / (float)( cg.headEndTime - cg.headStartTime );
	frac = frac * frac * ( 3 - 2 * frac );
	angles[YAW] = cg.headStartYaw + ( cg.headEndYaw - cg.headStartYaw ) * frac;
	angles[PITCH] = cg.headStartPitch + ( cg.headEndPitch - cg.headStartPitch ) * frac;

	CG_DrawHead( x, rect->y, rect->w, rect->h, cg.snap->ps.clientNum, angles );
}

static void CG_DrawSelectedPlayerHealth( rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle ) {
	clientInfo_t *ci;
	char num[16];

	ci = cgs.clientinfo + sortedTeamPlayers[CG_GetSelectedPlayer()];
	if ( ci ) {
		if ( shader ) {
			trap_R_SetColor( color );
			CG_DrawPic( rect->x, rect->y, rect->w, rect->h, shader );
			trap_R_SetColor( NULL );
		} else {
			int value;
			Com_sprintf( num, sizeof( num ), "%i", ci->health );
			value = CG_Text_Width( num, scale, 0 );
			CG_Text_Paint( rect->x + ( rect->w - value ) / 2, rect->y + rect->h, scale, color, num, 0, 0, textStyle );
		}
	}
}

static void CG_DrawSelectedPlayerArmor( rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle ) {
	clientInfo_t *ci;
	char num[16];

	ci = cgs.clientinfo + sortedTeamPlayers[CG_GetSelectedPlayer()];
	if ( ci ) {
		if ( ci->armor > 0 ) {
			if ( shader ) {
				trap_R_SetColor( color );
				CG_DrawPic( rect->x, rect->y, rect->w, rect->h, shader );
				trap_R_SetColor( NULL );
			} else {
				int value;
				Com_sprintf( num, sizeof( num ), "%i", ci->armor );
				value = CG_Text_Width( num, scale, 0 );
				CG_Text_Paint( rect->x + ( rect->w - value ) / 2, rect->y + rect->h, scale, color, num, 0, 0, textStyle );
			}
		}
	}
}

qhandle_t CG_StatusHandle( int task ) {
	return 0;
}

static void CG_DrawSelectedPlayerStatus( rectDef_t *rect ) {
}

static void CG_DrawPlayerStatus( rectDef_t *rect ) {
}

static void CG_DrawSelectedPlayerName( rectDef_t *rect, float scale, vec4_t color, qboolean voice, int textStyle ) {
	clientInfo_t *ci;
	ci = cgs.clientinfo + sortedTeamPlayers[CG_GetSelectedPlayer()];
	if ( ci ) {
		CG_Text_Paint( rect->x, rect->y + rect->h, scale, color, ci->name, 0, 0, textStyle );
	}
}

static void CG_DrawSelectedPlayerLocation( rectDef_t *rect, float scale, vec4_t color, int textStyle ) {
	clientInfo_t *ci;
	ci = cgs.clientinfo + sortedTeamPlayers[CG_GetSelectedPlayer()];
	if ( ci ) {
		const char *p = CG_TranslateString( CG_ConfigString( CS_LOCATIONS + ci->location ) );

		if ( !p || !*p ) {
			p = "unknown";
		}
		CG_Text_Paint( rect->x, rect->y + rect->h, scale, color, p, 0, 0, textStyle );
	}
}

static void CG_DrawPlayerLocation( rectDef_t *rect, float scale, vec4_t color, int textStyle  ) {
	clientInfo_t *ci = &cgs.clientinfo[cg.snap->ps.clientNum];
	if ( ci ) {
		const char *p = CG_TranslateString( CG_ConfigString( CS_LOCATIONS + ci->location ) );
		if ( !p || !*p ) {
			p = "unknown";
		}
		CG_Text_Paint( rect->x, rect->y + rect->h, scale, color, p, 0, 0, textStyle );
	}
}

static void CG_DrawSelectedPlayerWeapon( rectDef_t *rect ) {
	clientInfo_t *ci;
	ci = cgs.clientinfo + sortedTeamPlayers[CG_GetSelectedPlayer()];
	if ( ci ) {
		if ( cg_weapons[ci->curWeapon].weaponIcon[1] ) {
			CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cg_weapons[ci->curWeapon].weaponIcon[1] );  // (SA) using the 'selected' version of the icon
		} else {
			CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cgs.media.deferShader );
		}
	}
}

static void CG_DrawPlayerScore( rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle ) {
	char num[16];
	int value = cg.snap->ps.persistant[PERS_SCORE];

	if ( shader ) {
		trap_R_SetColor( color );
		CG_DrawPic( rect->x, rect->y, rect->w, rect->h, shader );
		trap_R_SetColor( NULL );
	} else {
		Com_sprintf( num, sizeof( num ), "%i", value );
		value = CG_Text_Width( num, scale, 0 );
		CG_Text_Paint( rect->x + ( rect->w - value ) / 2, rect->y + rect->h, scale, color, num, 0, 0, textStyle );
	}
}

static void CG_DrawHoldableItem( rectDef_t *rect, float scale, qboolean draw2D ) {
}

static void CG_DrawPlayerItem( rectDef_t *rect, float scale, qboolean draw2D ) {
	int value;
	vec3_t origin;

	value = cg.snap->ps.stats[STAT_HOLDABLE_ITEM];

	if ( value ) {
		CG_RegisterItemVisuals( value );

		if ( qtrue ) {
			CG_RegisterItemVisuals( value );
			CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cg_items[ value ].icons[0] );
		} else {
			vec3_t angles;
			VectorClear( angles );
			origin[0] = 90;
			origin[1] = 0;
			origin[2] = -10;
			angles[YAW] = ( cg.time & 2047 ) * 360 / 2048.0;
			CG_Draw3DModel( rect->x, rect->y, rect->w, rect->h, cg_items[ value ].models[0], 0, origin, angles );
		}
	}
}

static void CG_DrawSelectedPlayerPowerup( rectDef_t *rect, qboolean draw2D ) {
	clientInfo_t *ci;
	ci = cgs.clientinfo + sortedTeamPlayers[CG_GetSelectedPlayer()];

	if ( ci ) {
		int j;
		float x = rect->x;
		float y = rect->y;

		for ( j = 0; j < PW_NUM_POWERUPS; j++ ) {
			if ( ci->powerups & ( 1 << j ) ) {
				gitem_t *item;
				item = BG_FindItemForPowerup( j );
				if ( item ) {
					CG_DrawPic( x, y, rect->w, rect->h, trap_R_RegisterShader( item->icon ) );
					x += 3;
					y += 3;
					return;
				}
			}
		}
	}
}

static void CG_DrawSelectedPlayerHead( rectDef_t *rect, qboolean draw2D, qboolean voice ) {
	clientInfo_t    *ci;
	ci = cgs.clientinfo + sortedTeamPlayers[CG_GetSelectedPlayer()];

	if ( ci ) {
		if ( cg_draw3dIcons.integer ) {
			float len;
			vec3_t origin;
			vec3_t mins, maxs, angles;
			clipHandle_t cm = ci->headModel;

			if ( !cm ) {
				return;
			}

			// offset the origin y and z to center the head
			trap_R_ModelBounds( cm, mins, maxs );

			origin[2] = -0.5 * ( mins[2] + maxs[2] );
			origin[1] = 0.5 * ( mins[1] + maxs[1] );

			// calculate distance so the head nearly fills the box
			// assume heads are taller than wide
			len = 0.7 * ( maxs[2] - mins[2] );
			origin[0] = len / 0.268;    // len / tan( fov/2 )

			// allow per-model tweaking
			VectorAdd( origin, ci->modelInfo->headOffset, origin );

			angles[PITCH] = 0;
			angles[YAW] = 180;
			angles[ROLL] = 0;

			CG_Draw3DModel( rect->x, rect->y, rect->w, rect->h, ci->headModel, ci->headSkin, origin, angles );
		} else if ( cg_drawIcons.integer ) {
			CG_DrawPic( rect->x, rect->y, rect->w, rect->h, ci->modelIcon );
		}

		// if they are deferred, draw a cross out
		if ( ci->deferred ) {
			CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cgs.media.deferShader );
		}
	}
}

static void CG_DrawPlayerHealth( rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle ) {
	playerState_t   *ps;
	int value;
	vec4_t color2;

	ps = &cg.snap->ps;

	if ( cgs.gametype >= GT_WOLF && ( ps->pm_flags & PMF_FOLLOW ) ) {
		value = cgs.clientinfo[ ps->clientNum ].health;
	} else {
		value = ps->stats[STAT_HEALTH];
	}

	// DHM - Nerve :: Don't show negative health
	if ( value < 0 ) {
		value = 0;
	}

	if ( shader ) {
		trap_R_SetColor( color );
		CG_DrawPic( rect->x, rect->y, rect->w, rect->h, shader );
		trap_R_SetColor( NULL );
	} else {
		trap_R_SetColor( color );
		CG_DrawField( rect->x, rect->y, 3, value, 20 * scale, 32 * scale, qtrue, qtrue );           // NERVE - SMF

		// DHM - Nerve :: temp display of number of lifes left

		if ( ps->persistant[PERS_RESPAWNS_LEFT] >= 0 ) {
			VectorSet( color2, 1.0f, 1.0f, 1.0f );
			color2[3] = color[3];
			trap_R_SetColor( color2 );

			if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE ) {
				CG_DrawPic( rect->x + 92, rect->y - 2, 64, 32, cgs.media.hudAlliedHelmet );
			} else {
				CG_DrawPic( rect->x + 92, rect->y - 2, 64, 32, cgs.media.hudAxisHelmet );
			}

			trap_R_SetColor( color );
			CG_DrawField( rect->x + 144, rect->y, 3, ps->persistant[PERS_RESPAWNS_LEFT], 20 * scale, 32 * scale, qtrue, qtrue );
		}

		if ( cgs.clientinfo[cg.snap->ps.clientNum].powerups & ( 1 << PW_INVULNERABLE ) ) {
			CG_DrawPic( rect->x + 6, rect->y - 44, 36, 36, cgs.media.spawnInvincibleShader );
			return;
		}

		// draw some indicators for active smoke grenade and nade pack powerups
		VectorSet( color2, 0.0f, 1.0f, 0.0f );
		color2[3] = color[3];
		trap_R_SetColor( color2 );

		if ( cgs.clientinfo[cg.snap->ps.clientNum].powerups & ( 1 << PW_SMOKEGRENADE ) ) {
			CG_DrawPic( rect->x, rect->y - 36, 24, 24, cgs.media.smokeGrenadeIcon );
		}

		if ( cgs.clientinfo[cg.snap->ps.clientNum].powerups & ( 1 << PW_NADEPACK ) ) {
			CG_DrawPic( rect->x + 24, rect->y - 36, 24, 24, cgs.media.grenadePackIcon );
		}
	}
}

static void CG_DrawRedScore( rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle ) {
	int value;
	char num[16];
	if ( cgs.scores1 == SCORE_NOT_PRESENT ) {
		Com_sprintf( num, sizeof( num ), "-" );
	} else {
		Com_sprintf( num, sizeof( num ), "%i", cgs.scores1 );
	}
	value = CG_Text_Width( num, scale, 0 );
	CG_Text_Paint( rect->x + rect->w - value, rect->y + rect->h, scale, color, num, 0, 0, textStyle );
}

static void CG_DrawBlueScore( rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle ) {
	int value;
	char num[16];

	if ( cgs.scores2 == SCORE_NOT_PRESENT ) {
		Com_sprintf( num, sizeof( num ), "-" );
	} else {
		Com_sprintf( num, sizeof( num ), "%i", cgs.scores2 );
	}
	value = CG_Text_Width( num, scale, 0 );
	CG_Text_Paint( rect->x + rect->w - value, rect->y + rect->h, scale, color, num, 0, 0, textStyle );
}

static void CG_DrawRedName( rectDef_t *rect, float scale, vec4_t color, int textStyle ) {
}

static void CG_DrawBlueName( rectDef_t *rect, float scale, vec4_t color, int textStyle ) {
}

static void CG_DrawBlueFlagName( rectDef_t *rect, float scale, vec4_t color, int textStyle ) {
}

static void CG_DrawBlueFlagStatus( rectDef_t *rect, qhandle_t shader ) {
}

static void CG_DrawBlueFlagHead( rectDef_t *rect ) {
}

static void CG_DrawRedFlagName( rectDef_t *rect, float scale, vec4_t color, int textStyle ) {
}

static void CG_DrawRedFlagStatus( rectDef_t *rect, qhandle_t shader ) {
}

static void CG_DrawRedFlagHead( rectDef_t *rect ) {
}

static void CG_HarvesterSkulls( rectDef_t *rect, float scale, vec4_t color, qboolean force2D, int textStyle ) {
}

static void CG_OneFlagStatus( rectDef_t *rect ) {
}

static void CG_DrawCTFPowerUp( rectDef_t *rect ) {
}

static void CG_DrawTeamColor( rectDef_t *rect, vec4_t color ) {
	CG_DrawTeamBackground( rect->x, rect->y, rect->w, rect->h, color[3], cg.snap->ps.persistant[PERS_TEAM] );
}

static void CG_DrawAreaHoldable( rectDef_t *rect, int align, float spacing, float scale, vec4_t color ) {
}

static void CG_DrawAreaWeapons( rectDef_t *rect, int align, float spacing, float scale, vec4_t color ) {
}

static void CG_DrawAreaPowerUp( rectDef_t *rect, int align, float spacing, float scale, vec4_t color ) {
	char num[16];
	int sorted[MAX_POWERUPS];
	int sortedTime[MAX_POWERUPS];
	int i, j, k;
	int active;
	playerState_t   *ps;
	int t;
	gitem_t *item;
	float f;
	rectDef_t r2;
	float *inc;
	r2.x = rect->x;
	r2.y = rect->y;
	r2.w = rect->w;
	r2.h = rect->h;

	inc = ( align == HUD_VERTICAL ) ? &r2.y : &r2.x;

	ps = &cg.snap->ps;

	if ( ps->stats[STAT_HEALTH] <= 0 ) {
		return;
	}

	// sort the list by time remaining
	active = 0;
	for ( i = 0 ; i < MAX_POWERUPS ; i++ ) {
		if ( !ps->powerups[ i ] ) {
			continue;
		}
		t = ps->powerups[ i ] - cg.time;
		// ZOID--don't draw if the power up has unlimited time (999 seconds)
		// This is true of the CTF flags
		if ( t <= 0 || t >= 999000 ) {
			continue;
		}

		// insert into the list
		for ( j = 0 ; j < active ; j++ ) {
			if ( sortedTime[j] >= t ) {
				for ( k = active - 1 ; k >= j ; k-- ) {
					sorted[k + 1] = sorted[k];
					sortedTime[k + 1] = sortedTime[k];
				}
				break;
			}
		}
		sorted[j] = i;
		sortedTime[j] = t;
		active++;
	}

	// draw the icons and timers
	for ( i = 0 ; i < active ; i++ ) {
		item = BG_FindItemForPowerup( sorted[i] );

		if ( item ) {
			t = ps->powerups[ sorted[i] ];
			if ( t - cg.time >= POWERUP_BLINKS * POWERUP_BLINK_TIME ) {
				trap_R_SetColor( NULL );
			} else {
				vec4_t modulate;

				f = (float)( t - cg.time ) / POWERUP_BLINK_TIME;
				f -= (int)f;
				modulate[0] = modulate[1] = modulate[2] = modulate[3] = f;
				trap_R_SetColor( modulate );
			}

			CG_DrawPic( r2.x, r2.y, r2.w * .75, r2.h, trap_R_RegisterShader( item->icon ) );

			Com_sprintf( num, sizeof( num ), "%i", sortedTime[i] / 1000 );
			CG_Text_Paint( r2.x + ( r2.w * .75 ) + 3, r2.y + r2.h, scale, color, num, 0, 0, 0 );
			*inc += r2.w + spacing;
		}
	}
	trap_R_SetColor( NULL );
}

float CG_GetValue( int ownerDraw, int type ) {
	centity_t   *cent;
	clientInfo_t *ci;
	playerState_t   *ps;

	cent = &cg_entities[cg.snap->ps.clientNum];
	ps = &cg.snap->ps;

	switch ( ownerDraw )
	{
	case CG_SELECTEDPLAYER_ARMOR:
		ci = cgs.clientinfo + sortedTeamPlayers[CG_GetSelectedPlayer()];
		return ci->armor;
		break;
	case CG_SELECTEDPLAYER_HEALTH:
		ci = cgs.clientinfo + sortedTeamPlayers[CG_GetSelectedPlayer()];
		return ci->health;
		break;

	case CG_PLAYER_ARMOR_VALUE:
		return ps->stats[STAT_ARMOR];
		break;
	case CG_PLAYER_AMMO_VALUE:
		if ( cent->currentState.weapon ) {
			if ( type == RANGETYPE_RELATIVE ) {
				int weap = BG_FindAmmoForWeapon( cent->currentState.weapon );
				return (float)ps->ammo[weap] / (float)ammoTable[weap].maxammo;
			} else {
				return ps->ammo[BG_FindAmmoForWeapon( cent->currentState.weapon )];
			}
		}
		break;
	case CG_PLAYER_AMMOCLIP_VALUE:
		if ( cent->currentState.weapon ) {
			if ( type == RANGETYPE_RELATIVE ) {
				return (float)ps->ammoclip[BG_FindClipForWeapon( cent->currentState.weapon )] / (float)ammoTable[cent->currentState.weapon].maxclip;
			} else {
				return ps->ammoclip[BG_FindClipForWeapon( cent->currentState.weapon )];
			}
		}
		break;
	case CG_PLAYER_SCORE:
		return cg.snap->ps.persistant[PERS_SCORE];
		break;
	case CG_PLAYER_HEALTH:
		if ( cgs.gametype >= GT_WOLF && ( ps->pm_flags & PMF_FOLLOW ) ) {
			ci = &cgs.clientinfo[ ps->clientNum ];
			return ci->health;
		} else {
			return ps->stats[STAT_HEALTH];
		}
		break;
	case CG_RED_SCORE:
		return cgs.scores1;
		break;
	case CG_BLUE_SCORE:
		return cgs.scores2;
		break;
	case CG_PLAYER_WEAPON_STABILITY:    //----(SA)	added
		return ps->aimSpreadScale;
		break;

#define BONUSTIME 60000.0f
#define SPRINTTIME 20000.0f

	case CG_STAMINA:    //----(SA)	added
		if ( type == RANGETYPE_RELATIVE ) {
			return (float)cg.snap->ps.sprintTime / SPRINTTIME;
		} else {
			return cg.snap->ps.sprintTime;
		}
		break;
	default:
		break;
	}

	return -1;
}

qboolean CG_OtherTeamHasFlag() {
	return qfalse;
}

qboolean CG_YourTeamHasFlag() {
	return qfalse;
}

// THINKABOUTME: should these be exclusive or inclusive..
//
qboolean CG_OwnerDrawVisible( int flags ) {
	//----(SA)	added
	if ( flags & CG_SHOW_NOT_V_BINOC ) {      // if looking through binocs
		if ( cg.zoomedBinoc ) {
			return qfalse;
		}
	}

	if ( flags & CG_SHOW_NOT_V_SNIPER ) {     // if looking through sniper scope
		if ( cg.weaponSelect == WP_SNIPERRIFLE ) {
			return qfalse;
		}
	}

	if ( flags & CG_SHOW_NOT_V_SNOOPER ) {        // if looking through snooper scope
		if ( cg.weaponSelect == WP_SNOOPERSCOPE ) {
			return qfalse;
		}
	}

	if ( flags & CG_SHOW_NOT_V_FGSCOPE ) {        // if looking through fg42 scope
		if ( cg.weaponSelect == WP_FG42SCOPE ) {
			return qfalse;
		}
	}

	//----(SA)	end

	if ( flags & CG_SHOW_TEAMINFO ) {
		return ( cg_currentSelectedPlayer.integer == numSortedTeamPlayers );
	}

	if ( flags & CG_SHOW_NOTEAMINFO ) {
		return !( cg_currentSelectedPlayer.integer == numSortedTeamPlayers );
	}

	if ( flags & CG_SHOW_OTHERTEAMHASFLAG ) {
		return CG_OtherTeamHasFlag();
	}

	if ( flags & CG_SHOW_YOURTEAMHASENEMYFLAG ) {
		return CG_YourTeamHasFlag();
	}

	if ( flags & CG_SHOW_ANYTEAMGAME ) {
		if ( cgs.gametype >= GT_TEAM ) {
			return qtrue;
		}
	}

	if ( flags & CG_SHOW_ANYNONTEAMGAME ) {
		if ( cgs.gametype < GT_TEAM ) {
			return qtrue;
		}
	}

	if ( flags & CG_SHOW_CTF ) {
		if ( cgs.gametype == GT_CTF ) {
			return qtrue;
		}
	}

	if ( flags & CG_SHOW_HEALTHCRITICAL ) {
		if ( cg.snap->ps.stats[STAT_HEALTH] < 25 ) {
			return qtrue;
		}
	}

	if ( flags & CG_SHOW_HEALTHOK ) {
		if ( cg.snap->ps.stats[STAT_HEALTH] > 25 ) {
			return qtrue;
		}
	}

	if ( flags & CG_SHOW_DURINGINCOMINGVOICE ) {
	}

	//----(SA)	added
	if ( flags & CG_SHOW_NOT_V_CLEAR ) {
		// if /not/ looking through binocs,snooper or sniper
		if ( !cg.zoomedBinoc && !( cg.weaponSelect == WP_SNIPERRIFLE ) && !( cg.weaponSelect == WP_SNOOPERSCOPE ) && !( cg.weaponSelect == WP_FG42SCOPE ) ) {
			return qfalse;
		}
	}

	if ( flags & ( CG_SHOW_NOT_V_BINOC | CG_SHOW_NOT_V_SNIPER | CG_SHOW_NOT_V_SNOOPER | CG_SHOW_NOT_V_FGSCOPE ) ) {
		// setting any of these does not necessarily disable drawing in regular view
		// CG_SHOW_NOT_V_CLEAR must also be set to hide for reg view
		if ( !( flags & CG_SHOW_NOT_V_CLEAR ) ) {
			return qtrue;
		}
	}

	//----(SA)	end

	return qfalse;
}

static void CG_DrawPlayerHasFlag( rectDef_t *rect, qboolean force2D ) {
}

static void CG_DrawAreaSystemChat( rectDef_t *rect, float scale, vec4_t color, qhandle_t shader ) {
	CG_Text_Paint( rect->x, rect->y + rect->h, scale, color, systemChat, 0, 0, 0 );
}

static void CG_DrawAreaTeamChat( rectDef_t *rect, float scale, vec4_t color, qhandle_t shader ) {
	CG_Text_Paint( rect->x, rect->y + rect->h, scale, color,teamChat1, 0, 0, 0 );
}

static void CG_DrawAreaChat( rectDef_t *rect, float scale, vec4_t color, qhandle_t shader ) {
	CG_Text_Paint( rect->x, rect->y + rect->h, scale, color, teamChat2, 0, 0, 0 );
}

const char *CG_GetKillerText() {
	const char *s = "";
	if ( cg.killerName[0] ) {
		s = va( "Fragged by %s", cg.killerName );
	}
	return s;
}

static void CG_DrawKiller( rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle ) {
	// fragged by ... line
	if ( cg.killerName[0] ) {
		int x = rect->x + rect->w / 2;
		CG_Text_Paint( x - CG_Text_Width( CG_GetKillerText(), scale, 0 ) / 2, rect->y + rect->h, scale, color, CG_GetKillerText(), 0, 0, textStyle );
	}
}

static void CG_DrawCapFragLimit( rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle ) {
	int limit = ( cgs.gametype >= GT_CTF ) ? cgs.capturelimit : cgs.fraglimit;
	CG_Text_Paint( rect->x, rect->y, scale, color, va( "%2i", limit ),0, 0, textStyle );
}

static void CG_Draw1stPlace( rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle ) {
	if ( cgs.scores1 != SCORE_NOT_PRESENT ) {
		CG_Text_Paint( rect->x, rect->y, scale, color, va( "%2i", cgs.scores1 ),0, 0, textStyle );
	}
}

static void CG_Draw2ndPlace( rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle ) {
	if ( cgs.scores2 != SCORE_NOT_PRESENT ) {
		CG_Text_Paint( rect->x, rect->y, scale, color, va( "%2i", cgs.scores2 ),0, 0, textStyle );
	}
}

const char *CG_GetGameStatusText() {
	const char *s = "";
	if ( cgs.gametype < GT_TEAM ) {
		if ( cg.snap->ps.persistant[PERS_TEAM] != TEAM_SPECTATOR ) {
			s = va( "%s place with %i",CG_PlaceString( cg.snap->ps.persistant[PERS_RANK] + 1 ),cg.snap->ps.persistant[PERS_SCORE] );
		}
	} else {
		if ( cg.teamScores[0] == cg.teamScores[1] ) {
			s = va( "Teams are tied at %i", cg.teamScores[0] );
		} else if ( cg.teamScores[0] >= cg.teamScores[1] ) {
			s = va( "Red leads Blue, %i to %i", cg.teamScores[0], cg.teamScores[1] );
		} else {
			s = va( "Blue leads Red, %i to %i", cg.teamScores[1], cg.teamScores[0] );
		}
	}
	return s;
}

static void CG_DrawGameStatus( rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle ) {
	CG_Text_Paint( rect->x, rect->y + rect->h, scale, color, CG_GetGameStatusText(), 0, 0, textStyle );
}

const char *CG_GameTypeString() {
	if ( cgs.gametype == GT_FFA ) {
		return "Free For All";
	} else if ( cgs.gametype == GT_TEAM ) {
		return "Team Deathmatch";
	} else if ( cgs.gametype == GT_CTF ) {
		return "Capture the Flag";
	}
	return "";
}
static void CG_DrawGameType( rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle ) {
	CG_Text_Paint( rect->x, rect->y + rect->h, scale, color, CG_GameTypeString(), 0, 0, textStyle );
}

static void CG_Text_Paint_Limit( float *maxX, float x, float y, float scale, vec4_t color, const char* text, float adjust, int limit ) {
	vec4_t newColor;
	glyphInfo_t *glyph;

	if ( text ) {
		int len, count;
		const char *s = text;
		float max = *maxX;
		float useScale;
		fontInfo_t *font = &cgDC.Assets.textFont;

		if ( scale <= cg_smallFont.value ) {
			font = &cgDC.Assets.smallFont;
		} else if ( scale > cg_bigFont.value ) {
			font = &cgDC.Assets.bigFont;
		}

		useScale = scale * font->glyphScale;
		trap_R_SetColor( color );
		len = strlen( text );

		if ( limit > 0 && len > limit ) {
			len = limit;
		}

		count = 0;
		while ( s && *s && count < len )
		{
			glyph = &font->glyphs[(unsigned char)*s];           // NERVE - SMF - this needs to be an unsigned cast for localization
			if ( Q_IsColorString( s ) ) {
				memcpy( newColor, g_color_table[ColorIndex( *( s + 1 ) )], sizeof( newColor ) );
				newColor[3] = color[3];
				trap_R_SetColor( newColor );
				s += 2;
				continue;
			} else
			{
				float yadj = useScale * glyph->top;
				if ( CG_Text_Width( s, useScale, 1 ) + x > max ) {
					*maxX = 0;
					break;
				}
				CG_Text_PaintChar( x, y - yadj,
					glyph->imageWidth,
					glyph->imageHeight,
					useScale,
					glyph->s,
					glyph->t,
					glyph->s2,
					glyph->t2,
					glyph->glyph );

				x += ( glyph->xSkip * useScale ) + adjust;
				*maxX = x;
				count++;
				s++;
			}
		}
		trap_R_SetColor( NULL );
	}
}

#define PIC_WIDTH 12

void CG_DrawNewTeamInfo( rectDef_t *rect, float text_x, float text_y, float scale, vec4_t color, qhandle_t shader ) {
}

void CG_DrawTeamSpectators( rectDef_t *rect, float scale, vec4_t color, qhandle_t shader ) {
	if ( cg.spectatorLen ) {
		float maxX;

		if ( cg.spectatorWidth == -1 ) {
			cg.spectatorWidth = 0;
			cg.spectatorPaintX = rect->x + 1;
			cg.spectatorPaintX2 = -1;
		}

		if ( cg.spectatorOffset > cg.spectatorLen ) {
			cg.spectatorOffset = 0;
			cg.spectatorPaintX = rect->x + 1;
			cg.spectatorPaintX2 = -1;
		}

		if ( cg.time > cg.spectatorTime ) {
			cg.spectatorTime = cg.time + 10;
			if ( cg.spectatorPaintX <= rect->x + 2 ) {
				if ( cg.spectatorOffset < cg.spectatorLen ) {
					cg.spectatorPaintX += CG_Text_Width( &cg.spectatorList[cg.spectatorOffset], scale, 1 ) - 1;
					cg.spectatorOffset++;
				} else {
					cg.spectatorOffset = 0;
					if ( cg.spectatorPaintX2 >= 0 ) {
						cg.spectatorPaintX = cg.spectatorPaintX2;
					} else {
						cg.spectatorPaintX = rect->x + rect->w - 2;
					}
					cg.spectatorPaintX2 = -1;
				}
			} else {
				cg.spectatorPaintX--;
				if ( cg.spectatorPaintX2 >= 0 ) {
					cg.spectatorPaintX2--;
				}
			}
		}

		maxX = rect->x + rect->w - 2;
		CG_Text_Paint_Limit( &maxX, cg.spectatorPaintX, rect->y + rect->h - 3, scale, color, &cg.spectatorList[cg.spectatorOffset], 0, 0 );
		if ( cg.spectatorPaintX2 >= 0 ) {
			float maxX2 = rect->x + rect->w - 2;
			CG_Text_Paint_Limit( &maxX2, cg.spectatorPaintX2, rect->y + rect->h - 3, scale, color, cg.spectatorList, 0, cg.spectatorOffset );
		}
		if ( cg.spectatorOffset && maxX > 0 ) {
			// if we have an offset ( we are skipping the first part of the string ) and we fit the string
			if ( cg.spectatorPaintX2 == -1 ) {
				cg.spectatorPaintX2 = rect->x + rect->w - 2;
			}
		} else {
			cg.spectatorPaintX2 = -1;
		}
	}
}

void CG_DrawMedal( int ownerDraw, rectDef_t *rect, float scale, vec4_t color, qhandle_t shader ) {
	score_t *score = &cg.scores[cg.selectedScore];
	float value = 0;
	char *text = NULL;
	color[3] = 0.25;

	switch ( ownerDraw ) {
	case CG_ACCURACY:
		value = score->accuracy;
		break;
	case CG_ASSISTS:
		value = score->assistCount;
		break;
	case CG_DEFEND:
		value = score->defendCount;
		break;
	case CG_EXCELLENT:
		value = score->excellentCount;
		break;
	case CG_IMPRESSIVE:
		value = score->impressiveCount;
		break;
	case CG_PERFECT:
		value = score->perfect;
		break;
	case CG_GAUNTLET:
		value = score->guantletCount;
		break;
	case CG_CAPTURES:
		value = score->captures;
		break;
	}

	if ( value > 0 ) {
		if ( ownerDraw != CG_PERFECT ) {
			if ( ownerDraw == CG_ACCURACY ) {
				text = va( "%i%%", (int)value );
				if ( value > 50 ) {
					color[3] = 1.0;
				}
			} else {
				text = va( "%i", (int)value );
				color[3] = 1.0;
			}
		} else {
			if ( value ) {
				color[3] = 1.0;
			}
			text = "Wow";
		}
	}

	trap_R_SetColor( color );
	CG_DrawPic( rect->x, rect->y, rect->w, rect->h, shader );

	if ( text ) {
		color[3] = 1.0;
		value = CG_Text_Width( text, scale, 0 );
		CG_Text_Paint( rect->x + ( rect->w - value ) / 2, rect->y + rect->h + 10, scale, color, text, 0, 0, 0 );
	}
	trap_R_SetColor( NULL );
}

/*
==============
CG_DrawWeapStability
draw a bar showing current stability level (0-255), max at current weapon/ability, and 'perfect' reference mark

probably only drawn for scoped weapons
==============
*/
void CG_DrawWeapStability( rectDef_t *rect, vec4_t color, int align ) {
	vec4_t goodColor = {0, 1, 0, 0.5f}, badColor = {1, 0, 0, 0.5f};

	if ( !cg_drawSpreadScale.integer ) {
		return;
	}

	if ( cg_drawSpreadScale.integer == 1 && !( cg.weaponSelect == WP_SNOOPERSCOPE || cg.weaponSelect == WP_SNIPERRIFLE ) ) {
		return;
	}

	if ( !( cg.snap->ps.aimSpreadScale ) ) {
		return;
	}

	CG_FilledBar( rect->x, rect->y, rect->w, rect->h, goodColor, badColor, NULL, (float)cg.snap->ps.aimSpreadScale / 255.0f, 2 | 4 | 256 ); // flags (BAR_CENTER|BAR_VERT|BAR_LERP_COLOR)
}

/*
==============
CG_DrawWeapHeat
==============
*/
void CG_DrawWeapHeat( rectDef_t *rect, int align ) {
	vec4_t color = {1, 0, 0, 0.2f}, color2 = {1, 0, 0, 0.5f};
	int flags = 0;

	if ( !( cg.snap->ps.curWeapHeat ) ) {
		return;
	}

	if ( align != HUD_HORIZONTAL ) {
		flags |= 4;   // BAR_VERT
	}
	flags |= 1;       // BAR_LEFT			- this is hardcoded now, but will be decided by the menu script
	flags |= 16;      // BAR_BG			- draw the filled contrast box
	//	flags|=32;		// BAR_BGSPACING_X0Y5	- different style

	flags |= 256;     // BAR_COLOR_LERP
	CG_FilledBar( rect->x, rect->y, rect->w, rect->h, color, color2, NULL, (float)cg.snap->ps.curWeapHeat / 255.0f, flags );
}

/*
==============
CG_DrawFatigue
==============
*/

static void CG_DrawFatigue( rectDef_t *rect, vec4_t color, int align ) {
	vec4_t colorBonus = {1, 1, 0, 0.45f};   // yellow (a little more solid for the 'bonus' stamina)
	int flags = 0;

	if ( align != HUD_HORIZONTAL ) {
		flags |= 4;   // BAR_VERT
		flags |= 1;   // BAR_LEFT (left, when vertical means grow 'up')
	}

	CG_FilledBar( rect->x, rect->y + 6, rect->w, rect->h * 0.82f, color, NULL, NULL, (float)cg.snap->ps.sprintTime / SPRINTTIME, flags );

	// fill in the left side of the bar with the counter for the nofatigue powerup
	if ( cg.snap->ps.powerups[PW_NOFATIGUE] ) {
		CG_FilledBar( rect->x, rect->y, rect->w / 2, rect->h, colorBonus, NULL, NULL, cg.snap->ps.powerups[PW_NOFATIGUE] / BONUSTIME, flags );
	}

	colorBonus[2] = 1.0f;
	colorBonus[3] = cg_hudAlpha.value;
	trap_R_SetColor( colorBonus ); // JPW NERVE
}

// DHM - Nerve
static void CG_DrawWeapRecharge( rectDef_t *rect, vec4_t color, int align ) {
	int flags = 0;
	vec4_t bgcolor = {1.0f, 1.0f, 1.0f, 0.25f};

	if ( align != HUD_HORIZONTAL ) {
		flags |= 4;   // BAR_VERT
		flags |= 1;   // BAR_LEFT (left, when vertical means grow 'up')
	}
	flags |= 16;

	// JPW NERVE -- added drawWeaponPercent in multiplayer
	if ( cgs.gametype >= GT_WOLF ) {
		float barFrac, chargeTime;
		qboolean fade = qfalse;
		// DHM - Only draw bar if weapon uses it
		int weap = cg.snap->ps.weapon;

		if ( !( cg.snap->ps.eFlags & EF_ZOOMING ) ) {
			if ( weap != WP_PANZERFAUST && weap != WP_DYNAMITE && weap != WP_MEDKIT && weap != WP_SMOKE_GRENADE && weap != WP_PLIERS && weap != WP_AMMO ) {
				fade = qtrue;
			}
		}

		if ( cg.snap->ps.stats[ STAT_PLAYER_CLASS ] == PC_ENGINEER ) {
			chargeTime = cg_engineerChargeTime.value;
		} else if ( cg.snap->ps.stats[ STAT_PLAYER_CLASS ] == PC_MEDIC ) {
			chargeTime = cg_medicChargeTime.value;
		} else if ( cg.snap->ps.stats[ STAT_PLAYER_CLASS ] == PC_LT ) {
			chargeTime = cg_LTChargeTime.value;
		} else {
			chargeTime = cg_soldierChargeTime.value;
		}

		barFrac = (float)( cg.time - cg.snap->ps.classWeaponTime ) / chargeTime;

		if ( barFrac > 1.0 ) {
			barFrac = 1.0;
		}

		color[0] = 1.0f;
		color[1] = color[2] = barFrac;
		color[3] = 0.25 + barFrac * 0.5;

		if ( fade ) {
			bgcolor[3] *= 0.4f;
			color[3] *= 0.4;
		}

		CG_FilledBar( rect->x, rect->y + 6, rect->w, rect->h * 0.84f, color, NULL, bgcolor, barFrac, flags );

		color[1] = color[2] = 1.0f;
		color[3] = cg_hudAlpha.value;
		trap_R_SetColor( color );
	}
}
// dhm - end

/*
==============
CG_OwnerDraw
==============
*/
void CG_OwnerDraw( float x, float y, float w, float h, float text_x, float text_y, int ownerDraw, int ownerDrawFlags, int align, float special, float scale, vec4_t color, qhandle_t shader, int textStyle ) {
	rectDef_t rect;

	if ( cg_drawStatus.integer == 0 ) {
		return;
	}

	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;

	switch ( ownerDraw ) {
	case CG_PLAYER_WEAPON_ICON2D:
		CG_DrawPlayerWeaponIcon( &rect, ownerDrawFlags & CG_SHOW_HIGHLIGHTED, align );
		break;
		/*case CG_PLAYER_ARMOR_ICON:
		CG_DrawPlayerArmorIcon( &rect, ownerDrawFlags & CG_SHOW_2DONLY );
		break;
		case CG_PLAYER_ARMOR_ICON2D:
		CG_DrawPlayerArmorIcon( &rect, qtrue );
		break;*/
	case CG_PLAYER_ARMOR_VALUE:
		CG_DrawPlayerArmorValue( &rect, scale, color, shader, textStyle );
		break;
	case CG_PLAYER_AMMO_ICON:
		CG_DrawPlayerAmmoIcon( &rect, ownerDrawFlags & CG_SHOW_2DONLY );
		break;
	case CG_PLAYER_AMMO_ICON2D:
		CG_DrawPlayerAmmoIcon( &rect, qtrue );
		break;
	case CG_PLAYER_AMMO_VALUE:
		CG_DrawPlayerAmmoValue( &rect, scale, color, shader, textStyle, 0 );
		break;
	case CG_CURSORHINT:
		CG_DrawCursorhint( &rect );
		break;
	case CG_PLAYER_AMMOCLIP_VALUE:
		CG_DrawPlayerAmmoValue( &rect, scale, color, shader, textStyle, 1 );
		break;
	case CG_SELECTEDPLAYER_HEAD:
		CG_DrawSelectedPlayerHead( &rect, ownerDrawFlags & CG_SHOW_2DONLY, qfalse );
		break;
	case CG_VOICE_HEAD:
		CG_DrawSelectedPlayerHead( &rect, ownerDrawFlags & CG_SHOW_2DONLY, qtrue );
		break;
	case CG_VOICE_NAME:
		CG_DrawSelectedPlayerName( &rect, scale, color, qtrue, textStyle );
		break;
	case CG_SELECTEDPLAYER_STATUS:
		CG_DrawSelectedPlayerStatus( &rect );
		break;
	case CG_SELECTEDPLAYER_ARMOR:
		CG_DrawSelectedPlayerArmor( &rect, scale, color, shader, textStyle );
		break;
	case CG_SELECTEDPLAYER_HEALTH:
		CG_DrawSelectedPlayerHealth( &rect, scale, color, shader, textStyle );
		break;
	case CG_SELECTEDPLAYER_NAME:
		CG_DrawSelectedPlayerName( &rect, scale, color, qfalse, textStyle );
		break;
	case CG_SELECTEDPLAYER_LOCATION:
		CG_DrawSelectedPlayerLocation( &rect, scale, color, textStyle );
		break;
	case CG_SELECTEDPLAYER_WEAPON:
		CG_DrawSelectedPlayerWeapon( &rect );
		break;
	case CG_SELECTEDPLAYER_POWERUP:
		CG_DrawSelectedPlayerPowerup( &rect, ownerDrawFlags & CG_SHOW_2DONLY );
		break;
	case CG_PLAYER_WEAPON_HEAT:
		CG_DrawWeapHeat( &rect, align );
		break;
	case CG_PLAYER_WEAPON_STABILITY:
		CG_DrawWeapStability( &rect, color, align );
		break;
	case CG_STAMINA:
		CG_DrawFatigue( &rect, color, align );
		break;
		// DHM - Nerve
	case CG_PLAYER_WEAPON_RECHARGE:
		CG_DrawWeapRecharge( &rect, color, align );
		break;
		// dhm - end
	case CG_PLAYER_HEAD:
		CG_DrawPlayerHead( &rect, ownerDrawFlags & CG_SHOW_2DONLY );
		break;
	case CG_PLAYER_HOLDABLE:
		CG_DrawHoldableItem( &rect, scale, ownerDrawFlags & CG_SHOW_2DONLY );
		break;
	case CG_PLAYER_ITEM:
		CG_DrawPlayerItem( &rect, scale, ownerDrawFlags & CG_SHOW_2DONLY );
		break;
	case CG_PLAYER_SCORE:
		CG_DrawPlayerScore( &rect, scale, color, shader, textStyle );
		break;
	case CG_PLAYER_HEALTH:
		CG_DrawPlayerHealth( &rect, scale, color, shader, textStyle );
		break;
	case CG_RED_SCORE:
		CG_DrawRedScore( &rect, scale, color, shader, textStyle );
		break;
	case CG_BLUE_SCORE:
		CG_DrawBlueScore( &rect, scale, color, shader, textStyle );
		break;
	case CG_RED_NAME:
		CG_DrawRedName( &rect, scale, color, textStyle );
		break;
	case CG_BLUE_NAME:
		CG_DrawBlueName( &rect, scale, color, textStyle );
		break;
	case CG_BLUE_FLAGHEAD:
		CG_DrawBlueFlagHead( &rect );
		break;
	case CG_BLUE_FLAGSTATUS:
		CG_DrawBlueFlagStatus( &rect, shader );
		break;
	case CG_BLUE_FLAGNAME:
		CG_DrawBlueFlagName( &rect, scale, color, textStyle );
		break;
	case CG_RED_FLAGHEAD:
		CG_DrawRedFlagHead( &rect );
		break;
	case CG_RED_FLAGSTATUS:
		CG_DrawRedFlagStatus( &rect, shader );
		break;
	case CG_RED_FLAGNAME:
		CG_DrawRedFlagName( &rect, scale, color, textStyle );
		break;
	case CG_HARVESTER_SKULLS:
		CG_HarvesterSkulls( &rect, scale, color, qfalse, textStyle );
		break;
	case CG_HARVESTER_SKULLS2D:
		CG_HarvesterSkulls( &rect, scale, color, qtrue, textStyle );
		break;
	case CG_ONEFLAG_STATUS:
		CG_OneFlagStatus( &rect );
		break;
	case CG_PLAYER_LOCATION:
		CG_DrawPlayerLocation( &rect, scale, color, textStyle );
		break;
	case CG_TEAM_COLOR:
		CG_DrawTeamColor( &rect, color );
		break;
	case CG_CTF_POWERUP:
		CG_DrawCTFPowerUp( &rect );
		break;
	case CG_AREA_WEAPON:
		CG_DrawAreaWeapons( &rect, align, special, scale, color );
		break;
	case CG_AREA_HOLDABLE:
		CG_DrawAreaHoldable( &rect, align, special, scale, color );
		break;
	case CG_AREA_POWERUP:
		CG_DrawAreaPowerUp( &rect, align, special, scale, color );
		break;
	case CG_PLAYER_STATUS:
		CG_DrawPlayerStatus( &rect );
		break;
	case CG_PLAYER_HASFLAG:
		CG_DrawPlayerHasFlag( &rect, qfalse );
		break;
	case CG_PLAYER_HASFLAG2D:
		CG_DrawPlayerHasFlag( &rect, qtrue );
		break;
	case CG_AREA_SYSTEMCHAT:
		CG_DrawAreaSystemChat( &rect, scale, color, shader );
		break;
	case CG_AREA_TEAMCHAT:
		CG_DrawAreaTeamChat( &rect, scale, color, shader );
		break;
	case CG_AREA_CHAT:
		CG_DrawAreaChat( &rect, scale, color, shader );
		break;
	case CG_GAME_TYPE:
		CG_DrawGameType( &rect, scale, color, shader, textStyle );
		break;
	case CG_GAME_STATUS:
		CG_DrawGameStatus( &rect, scale, color, shader, textStyle );
		break;
	case CG_KILLER:
		CG_DrawKiller( &rect, scale, color, shader, textStyle );
		break;
	case CG_ACCURACY:
	case CG_ASSISTS:
	case CG_DEFEND:
	case CG_EXCELLENT:
	case CG_IMPRESSIVE:
	case CG_PERFECT:
	case CG_GAUNTLET:
	case CG_CAPTURES:
		CG_DrawMedal( ownerDraw, &rect, scale, color, shader );
		break;
	case CG_SPECTATORS:
		CG_DrawTeamSpectators( &rect, scale, color, shader );
		break;
	case CG_TEAMINFO:
		if ( cg_currentSelectedPlayer.integer == numSortedTeamPlayers ) {
			CG_DrawNewTeamInfo( &rect, text_x, text_y, scale, color, shader );
		}
		break;
	case CG_CAPFRAGLIMIT:
		CG_DrawCapFragLimit( &rect, scale, color, shader, textStyle );
		break;
	case CG_1STPLACE:
		CG_Draw1stPlace( &rect, scale, color, shader, textStyle );
		break;
	case CG_2NDPLACE:
		CG_Draw2ndPlace( &rect, scale, color, shader, textStyle );
		break;
	default:
		break;
	}
}

void CG_MouseEvent( int x, int y ) {
	int n;

	if ( ( cg.predictedPlayerState.pm_type == PM_NORMAL || cg.predictedPlayerState.pm_type == PM_SPECTATOR ) && cg.showScores == qfalse ) {
		trap_Key_SetCatcher( 0 );
		return;
	}

	cgs.cursorX += x;
	if ( cgs.cursorX < 0 ) {
		cgs.cursorX = 0;
	} else if ( cgs.cursorX > 640 ) {
		cgs.cursorX = 640;
	}

	cgs.cursorY += y;
	if ( cgs.cursorY < 0 ) {
		cgs.cursorY = 0;
	} else if ( cgs.cursorY > 480 ) {
		cgs.cursorY = 480;
	}

	n = Display_CursorType( cgs.cursorX, cgs.cursorY );
	cgs.activeCursor = 0;
	if ( n == CURSOR_ARROW ) {
		cgs.activeCursor = cgs.media.selectCursor;
	} else if ( n == CURSOR_SIZER ) {
		cgs.activeCursor = cgs.media.sizeCursor;
	}

	if ( cgs.capturedItem ) {
		Display_MouseMove( cgs.capturedItem, x, y );
	} else {
		Display_MouseMove( NULL, cgs.cursorX, cgs.cursorY );
	}
}

/*
==================
CG_HideTeamMenus
==================

*/
void CG_HideTeamMenu() {
	Menus_CloseByName( "teamMenu" );
	Menus_CloseByName( "getMenu" );
}

/*
==================
CG_ShowTeamMenus
==================

*/
void CG_ShowTeamMenu() {
	Menus_OpenByName( "teamMenu" );
}

/*
==================
CG_EventHandling
==================
type 0 - no event handling
1 - team menu
2 - hud editor

*/
void CG_EventHandling( int type ) {
	cgs.eventHandling = type;
	if ( type == CGAME_EVENT_NONE ) {
		CG_HideTeamMenu();
	}
}

void CG_KeyEvent( int key, qboolean down ) {
	if ( !down ) {
		return;
	}

	if ( cg.predictedPlayerState.pm_type == PM_NORMAL || ( cg.predictedPlayerState.pm_type == PM_SPECTATOR && cg.showScores == qfalse ) ) {
		CG_EventHandling( CGAME_EVENT_NONE );
		trap_Key_SetCatcher( 0 );
		return;
	}

	Display_HandleKey( key, down, cgs.cursorX, cgs.cursorY );

	if ( cgs.capturedItem ) {
		cgs.capturedItem = NULL;
	} else if ( key == K_MOUSE2 && down ) {
		cgs.capturedItem = Display_CaptureItem( cgs.cursorX, cgs.cursorY );
	}
}

// prevent centerview exploits
qboolean CG_CheckCenterView() {
	if ( !cg_centerView.integer ) {
		return qfalse;
	}
	if ( cg.pmext.blockCenterViewTime && cg.time < cg.pmext.blockCenterViewTime ) {
		return qfalse;
	}
	return qtrue;
}

int CG_ClientNumFromName( const char *p ) {
	int i;
	for ( i = 0; i < cgs.maxclients; i++ ) {
		if ( cgs.clientinfo[i].infoValid && Q_stricmp( cgs.clientinfo[i].name, p ) == 0 ) {
			return i;
		}
	}
	return -1;
}

void CG_ShowResponseHead() {
	Menus_OpenByName( "voiceMenu" );
	trap_Cvar_Set( "cl_conXOffset", "72" );
	cg.voiceTime = cg.time;
}

void CG_RunMenuScript( char **args ) {
}

void CG_GetTeamColor( vec4_t *color ) {
	if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED ) {
		( *color )[0] = 1;
		( *color )[3] = .25f;
		( *color )[1] = ( *color )[2] = 0;
	} else if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE ) {
		( *color )[0] = ( *color )[1] = 0;
		( *color )[2] = 1;
		( *color )[3] = .25f;
	} else {
		( *color )[0] = ( *color )[2] = 0;
		( *color )[1] = .17f;
		( *color )[3] = .25f;
	}
}