// cg_scoreboard -- draw the scoreboard on top of the game screen
#include "cg_local.h"


#define SCOREBOARD_WIDTH    ( 31 * BIGCHAR_WIDTH )

/*
=================
CG_DrawScoreboard
=================
*/
static void CG_DrawClientScore( int x, int y, score_t *score, float *color, float fade ) {
	char string[1024];
	vec3_t headAngles;
	clientInfo_t    *ci;

	if ( score->client < 0 || score->client >= cgs.maxclients ) {
		Com_Printf( "Bad score->client: %i\n", score->client );
		return;
	}

	ci = &cgs.clientinfo[score->client];

	// draw the handicap or bot skill marker
	if ( ci->botSkill > 0 && ci->botSkill <= 5 ) {
		CG_DrawPic( 0, y - 8, 32, 32, cgs.media.botSkillShaders[ ci->botSkill - 1 ] );
	} else if ( ci->handicap < 100 ) {
		Com_sprintf( string, sizeof( string ), "%i", ci->handicap );
		CG_DrawSmallStringColor( 8, y, string, color );
	}

	// draw the face
	VectorClear( headAngles );
	headAngles[YAW] = 180;

	CG_DrawHead( x - ICON_SIZE, y - ( ICON_SIZE - BIGCHAR_HEIGHT ) / 2, ICON_SIZE, ICON_SIZE,
				 score->client, headAngles );

	if ( ci->powerups & ( 1 << PW_REDFLAG ) ) {
		CG_DrawFlagModel( x - ICON_SIZE - ICON_SIZE / 2, y - ( ICON_SIZE - BIGCHAR_HEIGHT ) / 2, ICON_SIZE, ICON_SIZE,
						  TEAM_RED );
	} else if ( ci->powerups & ( 1 << PW_BLUEFLAG ) ) {
		CG_DrawFlagModel( x - ICON_SIZE - ICON_SIZE / 2, y - ( ICON_SIZE - BIGCHAR_HEIGHT ) / 2, ICON_SIZE, ICON_SIZE,
						  TEAM_BLUE );
	}

	// draw the score line
	if ( score->ping == -1 ) {
		Com_sprintf( string, sizeof( string ),
					 "connecting     %s", ci->name );
	} else if ( ci->team == TEAM_SPECTATOR ) {
		Com_sprintf( string, sizeof( string ),
					 "SPECT %4i %4i %s", score->ping, score->time, ci->name );
	} else {
		Com_sprintf( string, sizeof( string ),
					 "%5i %4i %4i %s", score->score, score->ping, score->time, ci->name );
	}

	// highlight your position
	if ( score->client == cg.snap->ps.clientNum ) {
		float hcolor[4];
		int rank;

		if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR
			 || cgs.gametype >= GT_TEAM ) {
			rank = -1;
		} else {
			rank = cg.snap->ps.persistant[PERS_RANK] & ~RANK_TIED_FLAG;
		}

		if ( rank == 0 ) {
			hcolor[0] = 0;
			hcolor[1] = 0;
			hcolor[2] = 0.7;
		} else if ( rank == 1 ) {
			hcolor[0] = 0.7;
			hcolor[1] = 0;
			hcolor[2] = 0;
		} else if ( rank == 2 ) {
			hcolor[0] = 0.7;
			hcolor[1] = 0.7;
			hcolor[2] = 0;
		} else {
			hcolor[0] = 0.7;
			hcolor[1] = 0.7;
			hcolor[2] = 0.7;
		}

		hcolor[3] = fade * 0.7;
		CG_FillRect( x - 2, y,  SCOREBOARD_WIDTH, BIGCHAR_HEIGHT + 1, hcolor );
	}

	CG_DrawBigString( x, y, string, fade );

	// add the "ready" marker for intermission exiting
	if ( cg.snap->ps.stats[ STAT_CLIENTS_READY ] & ( 1 << score->client ) ) {
		CG_DrawBigStringColor( 0, y, "READY", color );
	}
}

/*
=================
CG_TeamScoreboard
=================
*/
static int CG_TeamScoreboard( int x, int y, team_t team, float fade ) {
	int i;
	score_t *score;
	float color[4];
	int count;
	int lineHeight;
	clientInfo_t    *ci;

	color[0] = color[1] = color[2] = 1.0;
	color[3] = fade;

	count = 0;
	lineHeight = 40;
	// don't draw more than 9 rows
	for ( i = 0 ; i < cg.numScores && count < 9 ; i++ ) {
		score = &cg.scores[i];
		ci = &cgs.clientinfo[ score->client ];

		if ( team != ci->team ) {
			continue;
		}

		CG_DrawClientScore( x, y + lineHeight * count, score, color, fade );
		count++;
	}

	return y + count * lineHeight + 20;
}

/*
=================
WM_DrawObjectives
=================
*/
// NERVE - SMF
static int INFO_PLAYER_WIDTH    = 200;
static int INFO_CLASS_WIDTH     = 80;
static int INFO_SCORE_WIDTH     = 50;
static int INFO_LATENCY_WIDTH   = 60;
static int INFO_TOTAL_WIDTH     = 0;
static int INFO_TEAM_HEIGHT     = 24;
static int INFO_BORDER          = 2;
static int INFO_LINE_HEIGHT     = 30;

int WM_DrawObjectives( int x, int y, int width, float fade ) {
	const char *s, *buf, *str;
	char teamstr[32];
	int i, height, tempy, rows;
	vec4_t hcolor;

	rows = 7;
	height = SMALLCHAR_HEIGHT * rows;

	hcolor[0] = hcolor[1] = hcolor[2] = 1;
	hcolor[3] = 1.f;
	trap_R_SetColor( hcolor );
	if ( cg.snap->ps.pm_type != PM_INTERMISSION ) {
		CG_DrawPic( x - 10, y, width + 20, height + SMALLCHAR_HEIGHT + 10 + 1, trap_R_RegisterShaderNoMip( "ui_mp/assets/mp_score_objectives" ) );
	}
	VectorSet( hcolor, 1, 1, 1 );
	trap_R_SetColor( NULL );

	VectorSet( hcolor, ( 68.f / 255.f ), ( 0.f / 255.f ), ( 0.f / 255.f ) );
	hcolor[3] = 1 * fade;

	// draw header
	if ( cg.snap->ps.pm_type != PM_INTERMISSION ) {
		CG_DrawSmallString( x, y, CG_TranslateString( "Goals" ), fade );
	}
	y += SMALLCHAR_HEIGHT + 3;

	// draw color bands
	tempy = y;

	if ( cg.snap->ps.pm_type != PM_INTERMISSION ) {
		for ( i = 0; i < rows; i++ ) {
			hcolor[3] = fade * 0.3;
			if ( i % 2 == 0 ) {
				VectorSet( hcolor, ( 0.f / 255.f ), ( 113.f / 255.f ), ( 163.f / 255.f ) );         // LIGHT BLUE
			} else {
				VectorSet( hcolor, ( 0.f / 255.f ), ( 92.f / 255.f ), ( 133.f / 255.f ) );          // DARK BLUE
			}
			hcolor[3] = 0;

			CG_FillRect( x, y, width, SMALLCHAR_HEIGHT, hcolor );

			y += SMALLCHAR_HEIGHT;
		}
	}
	hcolor[3] = 1;

	y = tempy;

	if ( cg.snap->ps.pm_type == PM_INTERMISSION ) {
		const char *shader = NULL, *flagshader = NULL, *nameshader = NULL;

		s = CG_ConfigString( CS_MULTI_MAPWINNER );
		buf = Info_ValueForKey( s, "winner" );

		if ( atoi( buf ) == -1 ) {
			str = "ITS A TIE!";
		} else if ( atoi( buf ) ) {
			str = "ALLIES";
			shader = "ui_mp/assets/portraits/allies_win";
			flagshader = "ui_mp/assets/portraits/allies_win_flag.tga";
			nameshader = "ui_mp/assets/portraits/text_allies.tga";

			if ( !cg.latchVictorySound ) {
				cg.latchVictorySound = qtrue;
				trap_S_StartLocalSound( trap_S_RegisterSound( "sound/multiplayer/music/l_complete_2.wav" ), CHAN_LOCAL_SOUND );

				if ( cg_announcer.integer ) {
					trap_S_StartLocalSound( cgs.media.winAllies, CHAN_ANNOUNCER );
				}
			}
		} else {
			str = "AXIS";
			shader = "ui_mp/assets/portraits/axis_win";
			flagshader = "ui_mp/assets/portraits/axis_win_flag.tga";
			nameshader = "ui_mp/assets/portraits/text_axis.tga";

			if ( !cg.latchVictorySound ) {
				cg.latchVictorySound = qtrue;
				trap_S_StartLocalSound( trap_S_RegisterSound( "sound/multiplayer/music/s_stinglow.wav" ), CHAN_LOCAL_SOUND );

				if ( cg_announcer.integer ) {
					trap_S_StartLocalSound( cgs.media.winAxis, CHAN_ANNOUNCER );
				}
			}
		}

		y += SMALLCHAR_HEIGHT * ( ( rows - 2 ) / 2 );

		if ( flagshader ) {
			CG_DrawPic( 10, 10, 210, 136, trap_R_RegisterShaderNoMip( flagshader ) );
			CG_DrawPic( 415, 10, 210, 136, trap_R_RegisterShaderNoMip( flagshader ) );
		}

		if ( shader ) {
			CG_DrawPic( 229, 10, 182, 136, trap_R_RegisterShaderNoMip( shader ) );
		}
		if ( nameshader ) {
			CG_DrawPic( 50, 50, 127, 64, trap_R_RegisterShaderNoMip( nameshader ) );
			CG_DrawPic( 455, 50, 127, 64, trap_R_RegisterShaderNoMip( "ui_mp/assets/portraits/text_win.tga" ) );
		}
		return y;
	}
// JPW NERVE -- mission time & reinforce time
	else {
		int msec, mins, seconds, tens;
		tempy = y;
		y += SMALLCHAR_HEIGHT * ( rows - 1 );
		msec = ( cgs.timelimit * 60.f * 1000.f ) - ( cg.time - cgs.levelStartTime );

		seconds = msec / 1000;
		mins = seconds / 60;
		seconds -= mins * 60;
		tens = seconds / 10;
		seconds -= tens * 10;

		if ( msec < 0 ) {
			s = va( "%s %s", CG_TranslateString( "Mission time:" ),  CG_TranslateString( "Sudden Death" ) );
		} else {
			s = va( "%s   %2.0f:%i%i", CG_TranslateString( "Mission time:" ), (float)mins, tens, seconds ); // float cast to line up with reinforce time

		}
		CG_DrawSmallString( x,y,s,fade );

		if ( cgs.clientinfo[cg.snap->ps.clientNum].team == TEAM_RED ) {
			msec = cg_redlimbotime.integer - ( ( cgs.aReinfOffset[TEAM_RED] + cg.time - cgs.levelStartTime ) % cg_redlimbotime.integer ) + 1000;
		} else if ( cgs.clientinfo[cg.snap->ps.clientNum].team == TEAM_BLUE ) {
			msec = cg_bluelimbotime.integer - ( ( cgs.aReinfOffset[TEAM_BLUE] + cg.time - cgs.levelStartTime ) % cg_bluelimbotime.integer ) + 1000;
		} else { // no team (spectator mode)
			msec = 0;
		}

		if ( msec ) {
			seconds = msec / 1000;
			mins = seconds / 60;
			seconds -= mins * 60;
			tens = seconds / 10;
			seconds -= tens * 10;

			s = va( "%s %2.0f:%i%i", CG_TranslateString( "Reinforce time:" ), (float)mins, tens, seconds );
			CG_DrawSmallString( x + 425,y,s,fade );
		}

		// NERVE - SMF
		if ( cgs.gametype == GT_WOLF_STOPWATCH ) {
			int w;
			s = va( "%s %i", CG_TranslateString( "Stopwatch Round" ), cgs.currentRound + 1 );
			w = CG_DrawStrlen( s ) * SMALLCHAR_WIDTH;
			CG_DrawSmallString( x + 300 - w / 2,y,s,fade );
		}
		// -NERVE - SMF

		y = tempy;
	}
// jpw

	// determine character's team
	if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED ) {
		strcpy( teamstr, "short_axis_desc" );
	} else {
		strcpy( teamstr, "short_allied_desc" );
	}

	s = CG_ConfigString( CS_MULTI_INFO );
	buf = Info_ValueForKey( s, "numobjectives" );

	if ( buf && atoi( buf ) ) {
		int num = atoi( buf );
		int strwidth, status;

		for ( i = 0; i < num; i++ ) {
			s = CG_ConfigString( CS_MULTI_OBJECTIVE1 + i );
			buf = Info_ValueForKey( s, teamstr );
			if ( !buf || !strlen( buf ) ) {
				str = va( "%s %i", CG_TranslateString( "Objective" ), i + 1 );
			} else {
				str = va( "%s", CG_TranslateString( buf ) );
			}

			// draw text
			strwidth = CG_DrawStrlen( str ) * SMALLCHAR_WIDTH;
			CG_DrawSmallString( x + width / 2 - strwidth / 2 - 12, y, str, fade );

			// draw status flags
			s = CG_ConfigString( CS_MULTI_OBJ1_STATUS + i );
			status = atoi( Info_ValueForKey( s, "status" ) );

			if ( status == 0 ) {
				CG_DrawPic( x, y + 1, 24, 14, trap_R_RegisterShaderNoMip( "ui_mp/assets/ger_flag.tga" ) );
			} else if ( status == 1 ) {
				CG_DrawPic( x, y + 1, 24, 14, trap_R_RegisterShaderNoMip( "ui_mp/assets/usa_flag.tga" ) );
			}

			y += SMALLCHAR_HEIGHT;
		}
	}

	return y;
}

static void WM_DrawClientScore( int x, int y, score_t *score, float *color, float fade ) {
	int maxchars, offset;
	float tempx;
	vec4_t hcolor;
	clientInfo_t *ci;

	if ( y + SMALLCHAR_HEIGHT >= 470 ) {
		return;
	}

	ci = &cgs.clientinfo[score->client];

	if ( score->client == cg.snap->ps.clientNum ) {
		tempx = x;

		hcolor[3] = fade * 0.3;
		VectorSet( hcolor, 0.4452, 0.1172, 0.0782 );            // DARK-RED

		CG_FillRect( tempx, y + 1, INFO_PLAYER_WIDTH - INFO_BORDER, SMALLCHAR_HEIGHT - 1, hcolor );
		tempx += INFO_PLAYER_WIDTH;

		if ( ci->team == TEAM_SPECTATOR ) {
			int width;
			width = INFO_CLASS_WIDTH + INFO_SCORE_WIDTH + INFO_LATENCY_WIDTH;

			CG_FillRect( tempx, y + 1, width - INFO_BORDER, SMALLCHAR_HEIGHT - 1, hcolor );
			tempx += width;
		} else {
			CG_FillRect( tempx, y + 1, INFO_CLASS_WIDTH - INFO_BORDER, SMALLCHAR_HEIGHT - 1, hcolor );
			tempx += INFO_CLASS_WIDTH;

			CG_FillRect( tempx, y + 1, INFO_SCORE_WIDTH - INFO_BORDER, SMALLCHAR_HEIGHT - 1, hcolor );
			tempx += INFO_SCORE_WIDTH;

			CG_FillRect( tempx, y + 1, INFO_LATENCY_WIDTH - INFO_BORDER, SMALLCHAR_HEIGHT - 1, hcolor );
			tempx += INFO_LATENCY_WIDTH;
		}
	}

	tempx = x;

	// DHM - Nerve
	VectorSet( hcolor, 1, 1, 1 );
	hcolor[3] = fade;

	maxchars = 17;
	offset = 0;

	if ( ci->team != TEAM_SPECTATOR ) {
		if ( ci->powerups & ( ( 1 << PW_REDFLAG ) | ( 1 << PW_BLUEFLAG ) ) ) {
			CG_DrawPic( tempx - 4, y - 4, 24, 24, trap_R_RegisterShader( "models/multiplayer/treasure/treasure" ) );
			offset += 16;
			tempx += 16;
			maxchars -= 2;
		}

		// draw the skull icon if out of lives
		if ( score->respawnsLeft == -2 ) {
			CG_DrawPic( tempx, y, 18, 18, cgs.media.scoreEliminatedShader );
			offset += 18;
			tempx += 18;
			maxchars -= 2;
		}
	}

	// draw name
	CG_DrawStringExt( tempx, y, ci->name, hcolor, qfalse, qfalse, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, maxchars );
	tempx += INFO_PLAYER_WIDTH - offset;
	// dhm - nerve

	if ( ci->team == TEAM_SPECTATOR ) {
		const char *s;
		int w, totalwidth;

		totalwidth = INFO_CLASS_WIDTH + INFO_SCORE_WIDTH + INFO_LATENCY_WIDTH - 8;

		s = CG_TranslateString( "^3SPECTATOR" );
		w = CG_DrawStrlen( s ) * SMALLCHAR_WIDTH;

		CG_DrawSmallString( tempx + totalwidth - w, y, s, fade );
		return;
	} else if ( cg.snap->ps.persistant[PERS_TEAM] == ci->team ) {
		int val = score->playerClass; // cg_entities[ ci->clientNum ].currentState.teamNum;
		const char *s;

		if ( val == 0 ) {
			s = "Soldr";
		} else if ( val == 1 ) {
			s = "Medic";
		} else if ( val == 2 ) {
			s = "Engr";
		} else if ( val == 3 ) {
			s = "Lieut";
		} else {
			s = "";
		}

		CG_DrawSmallString( tempx, y, CG_TranslateString( s ), fade );
	}
	tempx += INFO_CLASS_WIDTH;

	CG_DrawSmallString( tempx, y, va( "%4i", score->score ), fade );
	tempx += INFO_SCORE_WIDTH;

	CG_DrawSmallString( tempx, y, va( "%4i", score->ping ), fade );
	tempx += INFO_LATENCY_WIDTH;
}

const char* WM_TimeToString( float msec ) {
	int mins, seconds, tens;

	seconds = msec / 1000;
	mins = seconds / 60;
	seconds -= mins * 60;
	tens = seconds / 10;
	seconds -= tens * 10;

	return va( "%i:%i%i", mins, tens, seconds );
}

static int WM_DrawInfoLine( int x, int y, float fade ) {
	int w, defender, winner;
	const char *s;

	if ( cg.snap->ps.pm_type != PM_INTERMISSION ) {
		return y;
	}

	w = 300;
	CG_DrawPic( 320 - w / 2, y, w, INFO_LINE_HEIGHT, trap_R_RegisterShaderNoMip( "ui_mp/assets/mp_line_strip.tga" ) );

	s = CG_ConfigString( CS_MULTI_INFO );
	defender = atoi( Info_ValueForKey( s, "defender" ) );

	s = CG_ConfigString( CS_MULTI_MAPWINNER );
	winner = atoi( Info_ValueForKey( s, "winner" ) );

	if ( cgs.currentRound ) {
		// first round
		s = va( CG_TranslateString( "Clock is now set to %s!" ), WM_TimeToString( cgs.nextTimeLimit * 60.f * 1000.f ) );
	} else {
		// second round
		if ( !defender ) {
			if ( winner != defender ) {
				s = "Allies sucessfully beat the clock!";
			} else {
				s = "Allies couldn't beat the clock!";
			}
		} else {
			if ( winner != defender ) {
				s = "Axis sucessfully beat the clock!";
			} else {
				s = "Axis couldn't beat the clock!";
			}
		}

		s = CG_TranslateString( s );
	}

	w = CG_DrawStrlen( s ) * SMALLCHAR_WIDTH;

	CG_DrawSmallString( 320 - w / 2, ( y + INFO_LINE_HEIGHT / 2 ) - SMALLCHAR_HEIGHT / 2, s, fade );
	return y + INFO_LINE_HEIGHT + 10;
}

static int WM_TeamScoreboard( int x, int y, team_t team, float fade, int maxrows ) {
	vec4_t hcolor;
	float tempx, tempy;
	int height, width;
	int i, rows;

	height = SMALLCHAR_HEIGHT * maxrows;
	width = INFO_PLAYER_WIDTH + INFO_CLASS_WIDTH + INFO_SCORE_WIDTH + INFO_LATENCY_WIDTH;
	rows = height / SMALLCHAR_HEIGHT - 2;

	hcolor[0] = hcolor[1] = hcolor[2] = 1;
	hcolor[3] = 1.f;
	trap_R_SetColor( hcolor );
	CG_DrawPic( x - 15, y - 4, width + 30, height + 1, trap_R_RegisterShaderNoMip( "ui_mp/assets/mp_score_team.tga" ) );
	trap_R_SetColor( NULL );

	if ( team == TEAM_RED ) {
		VectorSet( hcolor, ( 68.f / 255.f ), ( 0.f / 255.f ), ( 0.f / 255.f ) );
	} else if ( team == TEAM_BLUE ) {
		VectorSet( hcolor, ( 0.f / 255.f ), ( 45.f / 255.f ), ( 68.f / 255.f ) );
	}

	hcolor[3] = 1 * fade;

	// draw header
	if ( team == TEAM_RED ) {
		CG_DrawSmallString( x, y, va( "%s [%d] (%d %s)", CG_TranslateString( "Axis" ), cg.teamScores[0], cg.teamPlayers[team], CG_TranslateString( "players" ) ), fade );
	} else if ( team == TEAM_BLUE ) {
		CG_DrawSmallString( x, y, va( "%s [%d] (%d %s)", CG_TranslateString( "Allies" ), cg.teamScores[1], cg.teamPlayers[team], CG_TranslateString( "players" ) ), fade );
	}
	y += SMALLCHAR_HEIGHT + 4;

	// save off y val
	tempy = y;

	// draw color bands
	for ( i = 0; i < rows; i++ ) {
		hcolor[3] = fade * 0.3;
		if ( i % 2 == 0 ) {
			VectorSet( hcolor, ( 0.f / 255.f ), ( 163.f / 255.f ), ( 113.f / 255.f ) );         // LIGHT BLUE
		} else {
			VectorSet( hcolor, ( 0.f / 255.f ), ( 133.f / 255.f ), ( 92.f / 255.f ) );          // DARK BLUE
		}
		hcolor[3] = 0;

		CG_FillRect( x, y, width, SMALLCHAR_HEIGHT, hcolor );

		y += SMALLCHAR_HEIGHT;
	}
	hcolor[3] = 1;

	y = tempy;

	tempx = x;

	// draw player info headings
	CG_DrawSmallString( tempx, y, CG_TranslateString( "Name" ), fade );
	tempx += INFO_PLAYER_WIDTH;

	CG_DrawSmallString( tempx, y, CG_TranslateString( "Class" ), fade );
	tempx += INFO_CLASS_WIDTH;

	CG_DrawSmallString( tempx, y, CG_TranslateString( "Score" ), fade );
	tempx += INFO_SCORE_WIDTH;

	CG_DrawSmallString( tempx, y, CG_TranslateString( "Ping" ), fade );
	tempx += INFO_LATENCY_WIDTH;

	y += SMALLCHAR_HEIGHT;

	// draw player info
	VectorSet( hcolor, 1, 1, 1 );
	hcolor[3] = fade;

	cg.teamPlayers[team] = 0; // JPW NERVE
	for ( i = 0; i < cg.numScores; i++ ) {
		if ( team != cgs.clientinfo[ cg.scores[i].client ].team ) {
			continue;
		}
		cg.teamPlayers[team]++; // JPW NERVE // one frame latency, but who cares?
		WM_DrawClientScore( x, y, &cg.scores[i], hcolor, fade );
		y += SMALLCHAR_HEIGHT;
	}

	// draw spectators
	y += SMALLCHAR_HEIGHT;

	for ( i = 0; i < cg.numScores; i++ ) {
		if ( cgs.clientinfo[ cg.scores[i].client ].team != TEAM_SPECTATOR ) {
			continue;
		}
		if ( team == TEAM_RED && ( i % 2 ) ) {
			continue;
		}
		if ( team == TEAM_BLUE && ( ( i + 1 ) % 2 ) ) {
			continue;
		}

		WM_DrawClientScore( x, y, &cg.scores[i], hcolor, fade );
		y += SMALLCHAR_HEIGHT;
	}

	return y;
}
// -NERVE - SMF

/*
=================
CG_DrawScoreboard

Draw the normal in-game scoreboard
=================
*/
qboolean CG_DrawScoreboard( void ) {
	int x = 0, y = 0;
	float fade;
	float   *fadeColor;
	char    *s;

	// don't draw amuthing if the menu or console is up
	if ( cg_paused.integer ) {
		cg.deferredPlayerLoading = 0;
		return qfalse;
	}

	// don't draw scoreboard during death while warmup up
	if ( cg.warmup && !cg.showScores ) {
		return qfalse;
	}

	// NERVE - SMF - added mp wolf check
	if ( cg.showScores || ( cg.predictedPlayerState.pm_type == PM_DEAD && cgs.gametype < GT_WOLF ) ||
		 cg.predictedPlayerState.pm_type == PM_INTERMISSION ) {
		fade = 1.0;
		fadeColor = colorWhite;
	} else {
		fadeColor = CG_FadeColor( cg.scoreFadeTime, FADE_TIME );

		if ( !fadeColor ) {
			// next time scoreboard comes up, don't print killer
			cg.deferredPlayerLoading = 0;
			cg.killerName[0] = 0;
			return qfalse;
		}
		fade = *fadeColor;
	}


	// fragged by ... line
	if ( cg.killerName[0] ) {
		int w;
		s = va( "Killed by %s", cg.killerName );
		w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;
		x = ( SCREEN_WIDTH - w ) / 2;
		y = 40;
		CG_DrawBigString( x, y, s, fade );
	}

	// current rank
	// NERVE - SMF
	if ( cgs.gametype >= GT_WOLF ) {
		INFO_PLAYER_WIDTH   = 140;
		INFO_SCORE_WIDTH    = 50;
		INFO_CLASS_WIDTH    = 50;
		INFO_LATENCY_WIDTH  = 40;
		INFO_TEAM_HEIGHT    = 24;
		INFO_BORDER         = 2;
		INFO_TOTAL_WIDTH    = INFO_PLAYER_WIDTH + INFO_CLASS_WIDTH + INFO_SCORE_WIDTH + INFO_LATENCY_WIDTH;

		x = 20;
		y = 10;

		WM_DrawObjectives( x, y, 595, fade );

		if ( cgs.gametype == GT_WOLF_STOPWATCH && ( cg.snap->ps.pm_type == PM_INTERMISSION ) ) {
			y = WM_DrawInfoLine( x, 155, fade );

			WM_TeamScoreboard( x, y, TEAM_RED, fade, 18 );
			x = 335;
			WM_TeamScoreboard( x, y, TEAM_BLUE, fade, 18 );
		} else {
			y = 155;

			WM_TeamScoreboard( x, y, TEAM_RED, fade, 20 );
			x = 335;
			WM_TeamScoreboard( x, y, TEAM_BLUE, fade, 20 );
		}
	}
	// -NERVE - SMF
	else if ( cgs.gametype >= GT_TEAM ) {
		//
		// teamplay scoreboard
		//
		if ( cg.teamScores[0] >= cg.teamScores[1] ) {
			y = CG_TeamScoreboard( x, y, TEAM_RED, fade );
			y = CG_TeamScoreboard( x, y, TEAM_BLUE, fade );
		} else {
			y = CG_TeamScoreboard( x, y, TEAM_BLUE, fade );
			y = CG_TeamScoreboard( x, y, TEAM_RED, fade );
		}
		y = CG_TeamScoreboard( x, y, TEAM_SPECTATOR, fade );

	} else {    //----(SA) modified
		//
		// free for all scoreboard
		//
		y = CG_TeamScoreboard( x, y, TEAM_FREE, fade );
		y = CG_TeamScoreboard( x, y, TEAM_SPECTATOR, fade );
	}

	// load any models that have been deferred
	if ( ++cg.deferredPlayerLoading > 1 ) {
		CG_LoadDeferredPlayers();
	}

	return qtrue;
}

//================================================================================

/*
=================
CG_DrawTourneyScoreboard

Draw the oversize scoreboard for tournements
=================
*/
void CG_DrawTourneyScoreboard( void ) {
	vec4_t color;

	// request more scores regularly
	if ( cg.scoresRequestTime + 2000 < cg.time ) {
		cg.scoresRequestTime = cg.time;
		trap_SendClientCommand( "score" );
	}

	// draw the dialog background
	color[0] = color[1] = color[2] = 0;
	color[3] = 1;
	CG_FillRect( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, color );

	if ( cgs.gametype >= GT_WOLF ) {
		int x,y;

		INFO_PLAYER_WIDTH   = 140;
		INFO_SCORE_WIDTH    = 50;
		INFO_CLASS_WIDTH    = 50;
		INFO_LATENCY_WIDTH  = 40;
		INFO_TEAM_HEIGHT    = 24;
		INFO_BORDER         = 2;
		INFO_TOTAL_WIDTH    = INFO_PLAYER_WIDTH + INFO_CLASS_WIDTH + INFO_SCORE_WIDTH + INFO_LATENCY_WIDTH;

		x = 20;
		y = 10;

		WM_DrawObjectives( x, y, 595, 1.f );

		if ( cgs.gametype == GT_WOLF_STOPWATCH && ( cg.snap->ps.pm_type == PM_INTERMISSION ) ) {
			y = WM_DrawInfoLine( x, 155, 1.f );

			WM_TeamScoreboard( x, y, TEAM_RED, 1.f, 18 );
			x = 335;
			WM_TeamScoreboard( x, y, TEAM_BLUE, 1.f, 18 );
		} else {
			y = 155;

			WM_TeamScoreboard( x, y, TEAM_RED, 1.f, 20 );
			x = 335;
			WM_TeamScoreboard( x, y, TEAM_BLUE, 1.f, 20 );
		}
	}
}

