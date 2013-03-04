--[[
    Name: rtcw_constants.lua
    Purpose: some constants from the game
]]--

-- means of death
MOD_UNKNOWN =                   0
MOD_SHOTGUN =                   1
MOD_GAUNTLET =                  2
MOD_MACHINEGUN =                3
MOD_GRENADE =                   4
MOD_GRENADE_SPLASH =            5
MOD_ROCKET =                    6
MOD_ROCKET_SPLASH =             7
MOD_RAILGUN =                   8
MOD_LIGHTNING =                 9
MOD_BFG =                       10
MOD_BFG_SPLASH =                11
MOD_KNIFE =                     12
MOD_KNIFE2 =                    13
MOD_KNIFE_STEALTH =             14
MOD_LUGER =                     15
MOD_COLT =                      16
MOD_MP40 =                      17
MOD_THOMPSON =                  18
MOD_STEN =                      19
MOD_MAUSER =                    20
MOD_SNIPERRIFLE =               21
MOD_GARAND =                    22
MOD_SNOOPERSCOPE =              23
MOD_SILENCER =                  24
MOD_AKIMBO =                    25
MOD_BAR =                       26
MOD_FG42 =                      27
MOD_FG42SCOPE =                 28
MOD_PANZERFAUST =               29
MOD_ROCKET_LAUNCHER =           30
MOD_GRENADE_LAUNCHER =          31
MOD_VENOM =                     32
MOD_VENOM_FULL =                33
MOD_FLAMETHROWER =              34
MOD_TESLA =                     35
MOD_SPEARGUN =                  36
MOD_SPEARGUN_CO2 =              37
MOD_GRENADE_PINEAPPLE =         38
MOD_CROSS =                     39
MOD_MORTAR =                    40
MOD_MORTAR_SPLASH =             41
MOD_KICKED =                    42
MOD_GRABBER =                   43
MOD_DYNAMITE =                  44
MOD_DYNAMITE_SPLASH =           45
MOD_AIRSTRIKE =                 46
MOD_SYRINGE =                   47
MOD_AMMO =                      48
MOD_ARTY =                      49
MOD_WATER =                     50
MOD_SLIME =                     51
MOD_LAVA =                      52
MOD_CRUSH =                     53
MOD_TELEFRAG =                  54
MOD_FALLING =                   55
MOD_SUICIDE =                   56
MOD_TARGET_LASER =              57
MOD_TRIGGER_HURT =              58
MOD_GRAPPLE =                   59
MOD_EXPLOSIVE =                 60
MOD_POISONGAS =                 61
MOD_ZOMBIESPIT =                62
MOD_ZOMBIESPIT_SPLASH =         63
MOD_ZOMBIESPIRIT =              64
MOD_ZOMBIESPIRIT_SPLASH =       65
MOD_LOPER_LEAP =                66
MOD_LOPER_GROUND =              67
MOD_LOPER_HIT =                 68
MOD_LT_AMMO =                   69
MOD_LT_AIRSTRIKE =              70
MOD_ENGINEER =                  71
MOD_MEDIC =                     72
MOD_BAT =                       73
MOD_KNIFE_THROWN =              74
MOD_GOOMBA =                    75
MOD_POISON =                    76

ModNames =
{
	[ 0 ] = "MOD_UNKNOWN", 		-- force 0 based
	"MOD_SHOTGUN",              --1
	"MOD_GAUNTLET",             --2
	"MOD_MACHINEGUN",           --3
	"MOD_GRENADE",              --4
	"MOD_GRENADE_SPLASH",       --5
	"MOD_ROCKET",               --6
	"MOD_ROCKET_SPLASH",        --7
	"MOD_RAILGUN",              --8
	"MOD_LIGHTNING",            --9
	"MOD_BFG",                  --10
	"MOD_BFG_SPLASH",           --11
	"MOD_KNIFE",                --12
	"MOD_KNIFE2",               --13
	"MOD_KNIFE_STEALTH",        --14
	"MOD_LUGER",                --15
	"MOD_COLT",                 --16
	"MOD_MP40",                 --17
	"MOD_THOMPSON",             --18
	"MOD_STEN",                 --19
	"MOD_MAUSER",               --20
	"MOD_SNIPERRIFLE",          --21
	"MOD_GARAND",               --22
	"MOD_SNOOPERSCOPE",         --23
	"MOD_SILENCER",             --24
	"MOD_AKIMBO",               --25
	"MOD_BAR",                  --26
	"MOD_FG42",                 --27
	"MOD_FG42SCOPE",            --28
	"MOD_PANZERFAUST",          --29
	"MOD_ROCKET_LAUNCHER",      --30
	"MOD_GRENADE_LAUNCHER",     --31
	"MOD_VENOM",                --32
	"MOD_VENOM_FULL",           --33
	"MOD_FLAMETHROWER",         --34
	"MOD_TESLA",                --35
	"MOD_SPEARGUN",             --36
	"MOD_SPEARGUN_CO2",         --37
	"MOD_GRENADE_PINEAPPLE",    --38
	"MOD_CROSS",                --39
	"MOD_MORTAR",               --40
	"MOD_MORTAR_SPLASH",        --41
	"MOD_KICKED",               --42
	"MOD_GRABBER",              --43
	"MOD_DYNAMITE",             --44
	"MOD_DYNAMITE_SPLASH",      --45
	"MOD_AIRSTRIKE",            --46
	"MOD_SYRINGE",              --47
	"MOD_AMMO",                 --48
	"MOD_ARTY",                 --49
	"MOD_WATER",                --50
	"MOD_SLIME",                --51
	"MOD_LAVA",                 --52
	"MOD_CRUSH",                --53
	"MOD_TELEFRAG",             --54
	"MOD_FALLING",              --55
	"MOD_SUICIDE",              --56
	"MOD_TARGET_LASER",         --57
	"MOD_TRIGGER_HURT",         --58
	"MOD_GRAPPLE",              --59
	"MOD_EXPLOSIVE",            --60
	"MOD_POISONGAS",            --61
	"MOD_ZOMBIESPIT",           --62
	"MOD_ZOMBIESPIT_SPLASH",    --63
	"MOD_ZOMBIESPIRIT",         --64
	"MOD_ZOMBIESPIRIT_SPLASH",  --65
	"MOD_LOPER_LEAP",           --66
	"MOD_LOPER_GROUND",         --67
	"MOD_LOPER_HIT",            --68
	"MOD_LT_AMMO",              --69
	"MOD_LT_AIRSTRIKE",         --70
	"MOD_ENGINEER",             --71
	"MOD_MEDIC",                --72
	"MOD_BAT",                  --73
	"MOD_KNIFE_THROWN",         --74
	"MOD_GOOMBA",               --75
	"MOD_POISON",               --76
}

-- weapons
WP_NONE =                       0
WP_KNIFE =                      1
WP_LUGER =                      2
WP_MP40 =                       3
WP_MAUSER =                     4
WP_FG42 =                       5
WP_GRENADE_LAUNCHER =           6
WP_PANZERFAUST =                7
WP_VENOM =                      8
WP_FLAMETHROWER =               9
WP_TESLA =                      10
WP_SPEARGUN =                   11
WP_KNIFE2 =                     12
WP_COLT =                       13
WP_THOMPSON =                   14
WP_GARAND =                     15
WP_BAR =                        16
WP_GRENADE_PINEAPPLE =          17
WP_ROCKET_LAUNCHER =            18
WP_SNIPERRIFLE =                19
WP_SNOOPERSCOPE =               20
WP_VENOM_FULL =                 21
WP_SPEARGUN_CO2 =               22
WP_FG42SCOPE =                  23
WP_BAR2 =                       24
WP_STEN =                       25
WP_MEDIC_SYRINGE =              26
WP_AMMO =                       27
WP_ARTY =                       28
WP_SILENCER =                   29
WP_AKIMBO =                     30
WP_CROSS =                      31
WP_DYNAMITE =                   32
WP_DYNAMITE2 =                  33
WP_PROX =                       34
WP_MONSTER_ATTACK1 =            35
WP_MONSTER_ATTACK2 =            36
WP_MONSTER_ATTACK3 =            37
WP_SMOKETRAIL =                 38
WP_GAUNTLET =                   39
WP_SNIPER =                     40
WP_MORTAR =                     41
VERYBIGEXPLOSION =              42
WP_MEDKIT =                     43
WP_PLIERS =                     44
WP_SMOKE_GRENADE =              45
WP_BINOCULARS =                 46
WP_NUM_WEAPONS =                47

-- classes
PC_SOLDIER =                    0
PC_MEDIC =                      1
PC_ENGINEER =                   2
PC_LT =                         3

-- teams
TEAM_FREE =						0
TEAM_RED =						1
TEAM_AXIS =						TEAM_RED
TEAM_BLUE =						2
TEAM_ALLIES =					TEAM_BLUE
TEAM_SPECTATOR =				3
TEAM_NUM_TEAMS =				4

-- config strings
CS_SERVERINFO =                 0
CS_SYSTEMINFO =                 1
CS_MUSIC =                      2
CS_MESSAGE =                    3
CS_MOTD =                       4
CS_WARMUP =                     5
CS_SCORES1 =                    6
CS_SCORES2 =                    7
CS_VOTE_TIME =                  8
CS_VOTE_STRING =                9
CS_VOTE_YES =                   10
CS_VOTE_NO =                    11
CS_GAME_VERSION =               12
CS_LEVEL_START_TIME =           13
CS_INTERMISSION =               14
CS_MULTI_INFO =                 15
CS_MULTI_MAPWINNER =            16
CS_MULTI_MAPDESC =              17
CS_MULTI_OBJECTIVE1 =           18
CS_MULTI_OBJECTIVE2 =           19
CS_MULTI_OBJECTIVE3 =           20
CS_MULTI_OBJECTIVE4 =           21
CS_MULTI_OBJECTIVE5 =           22
CS_MULTI_OBJECTIVE6 =           23
CS_MULTI_OBJ1_STATUS =          24
CS_MULTI_OBJ2_STATUS =          25
CS_MULTI_OBJ3_STATUS =          26
CS_MULTI_OBJ4_STATUS =          27
CS_MULTI_OBJ5_STATUS =          28
CS_MULTI_OBJ6_STATUS =          29
CS_SHADERSTATE =                30
CS_ITEMS =                      31
CS_SCREENFADE =                 32
CS_FOGVARS =                    33
CS_SKYBOXORG =                  34
CS_TARGETEFFECT =               35
CS_WOLFINFO =                   36
CS_REINFSEEDS =                 37
CS_MODELS =                     64
CS_SOUNDS =                     320
CS_PLAYERS =                    576
CS_LOCATIONS =                  640
CS_PARTICLES =                  704
CS_MULTI_SPAWNTARGETS =         832
CS_OID_TRIGGERS =               848
CS_DLIGHTS =                    864
CS_CLIPBOARDS =                 992
CS_SPLINES =                    1058
CS_TAGCONNECTS =                1122
CS_MAX =                        1154

-- game types
GT_FFA =						0
GT_TOURNAMENT =					1
GT_SINGLE_PLAYER =				2
GT_TEAM =						3
GT_CTF =						4
GT_WOLF =						5
GT_WOLF_STOPWATCH = 			6
GT_WOLF_CP =					7
GT_WOLF_CPH =					8
GT_MAX_GAME_TYPE =				9

-- game states
GS_INITIALIZE =					-1
GS_PLAYING =					0
GS_WARMUP_COUNTDOWN = 			1
GS_WARMUP =						2
GS_INTERMISSION =				3
GS_WAITING_FOR_PLAYERS =		4
GS_REST =						5

-- connection states
CON_DISCONNECTED =				0
CON_CONNECTING =				1
CON_CONNECTED =					2

-- spectator states
SPECTATOR_NOT =					0
SPECTATOR_FREE =				1
SPECTATOR_FOLLOW =				2
SPECTATOR_SCOREBOARD =			3

-- powerups
PW_NONE = 						0
PW_QUAD = 						1
PW_SMOKEGRENADE = 				2
PW_HASTE = 						3
PW_INVIS = 						4
PW_REGEN = 						5
PW_FLIGHT = 					6
PW_INVULNERABLE = 				7
PW_FIRE = 						8
PW_ELECTRIC = 					9
PW_BREATHER = 					10
PW_NOFATIGUE = 					11
PW_REDFLAG = 					12
PW_BLUEFLAG = 					13
PW_NADEPACK = 					14
PW_NUM_POWERUPS = 				15

-- maxs
MAX_CLIENTS =					64

-- messages
SAY_ALL = 						0
SAY_TEAM =						1
SAY_TELL =						2
PRIVATE_MESSAGE =				3

-- ps.persistant
PERS_SCORE =					0
PERS_HITS =						1
PERS_RANK =						2
PERS_TEAM =						3
PERS_SPAWN_COUNT =				4
PERS_REWARD_COUNT =				5
PERS_REWARD =					6
PERS_ATTACKER =					7
PERS_KILLED =					8
PERS_RESPAWNS_LEFT =			9
PERS_KILLSPREE =				10
PERS_ACCURACTY_HITS =			11
PERS_HWEAPON_USE =				12
PERS_WOLFKICK =					13
PERS_BLEH2 =					14
PERS_KILLS =					15

-- ps->stats
STAT_HEALTH =					0
STAT_HOLDABLE_ITEM =			1
STAT_ARMOR =					2
STAT_KEYS =						3
STAT_DEAD_YAW =					4
STAT_CLIENTS_READY =			5
STAT_MAX_HEALTH =				6
STAT_PLAYER_CLASS =				7
STAT_CAPTUREHOLD_RED =			8
STAT_CAPTUREHOLD_BLUE =			9

-- events
EV_NONE =                		0
EV_FOOTSTEP =                	1
EV_FOOTSTEP_METAL =             2
EV_FOOTSTEP_WOOD =              3
EV_FOOTSTEP_GRASS =             4
EV_FOOTSTEP_GRAVEL =            5
EV_FOOTSTEP_ROOF =              6
EV_FOOTSTEP_SNOW =              7
EV_FOOTSTEP_CARPET =            8
EV_FOOTSPLASH =                	9
EV_FOOTWADE =                	10
EV_SWIM =                		11
EV_STEP_4 =                		12
EV_STEP_8 =                		13
EV_STEP_12 =                	14
EV_STEP_16 =                	15
EV_FALL_SHORT =                	16
EV_FALL_MEDIUM =                17
EV_FALL_FAR =                	18
EV_FALL_NDIE =                	19
EV_FALL_DMG_10 =                20
EV_FALL_DMG_15 =                21
EV_FALL_DMG_25 =                22
EV_FALL_DMG_50 =                23
EV_JUMP_PAD =                	24
EV_JUMP =                		25
EV_WATER_TOUCH =                26
EV_WATER_LEAVE =                27
EV_WATER_UNDER =                28
EV_WATER_CLEAR =                29
EV_ITEM_PICKUP =                30
EV_ITEM_PICKUP_QUIET =          31
EV_GLOBAL_ITEM_PICKUP =         32
EV_NOITEM =                		33
EV_NOAMMO =                		34
EV_EMPTYCLIP =                	35
EV_FILL_CLIP =                	36
EV_MG42_FIXED =                	37
EV_WEAP_OVERHEAT =              38
EV_CHANGE_WEAPON =              39
EV_FIRE_WEAPON =                40
EV_FIRE_WEAPONB =               41
EV_FIRE_WEAPON_LASTSHOT =       42
EV_FIRE_QUICKGREN =             43
EV_NOFIRE_UNDERWATER =          44
EV_FIRE_WEAPON_MG42 =           45
EV_USE_ITEM0 =                	46
EV_USE_ITEM1 =                	47
EV_USE_ITEM2 =                	48
EV_USE_ITEM3 =                	49
EV_USE_ITEM4 =                	50
EV_USE_ITEM5 =                	51
EV_USE_ITEM6 =                	52
EV_USE_ITEM7 =                	53
EV_USE_ITEM8 =                	54
EV_USE_ITEM9 =                	55
EV_USE_ITEM10 =                	56
EV_USE_ITEM11 =                	57
EV_USE_ITEM12 =                	58
EV_USE_ITEM13 =                	59
EV_USE_ITEM14 =                	60
EV_USE_ITEM15 =                	61
EV_ITEM_RESPAWN =               62
EV_ITEM_POP =                	63
EV_PLAYER_TELEPORT_IN =         64
EV_PLAYER_TELEPORT_OUT =        65
EV_GRENADE_BOUNCE =             66
EV_GENERAL_SOUND =              67
EV_GLOBAL_SOUND =               68
EV_GLOBAL_CLIENT_SOUND =        69
EV_BULLET_HIT_FLESH =           70
EV_BULLET_HIT_WALL =            71
EV_MISSILE_HIT =                72
EV_MISSILE_MISS =               73
EV_RAILTRAIL =                	74
EV_VENOM =                		75
EV_VENOMFULL =                	76
EV_BULLET =                		77
EV_LOSE_HAT =                	78
EV_GIB_HEAD =                	79
EV_PAIN =                		80
EV_CROUCH_PAIN =                81
EV_DEATH1 =                		82
EV_DEATH2 =                		83
EV_DEATH3 =                		84
EV_OBITUARY =                	85
EV_STOPSTREAMINGSOUND =         86
EV_POWERUP_NADEPACK =           87
EV_POWERUP_SMOKEGRENADE =       88
EV_POWERUP_REGEN =              89
EV_GIB_PLAYER =                	90
EV_DEBUG_LINE =                	91
EV_STOPLOOPINGSOUND =           92
EV_TAUNT =                		93
EV_SMOKE =                		94
EV_SPARKS =                		95
EV_SPARKS_ELECTRIC =            96
EV_BATS =                		97
EV_BATS_UPDATEPOSITION =        98
EV_BATS_DEATH =                	99
EV_EXPLODE =                	100
EV_EFFECT =                		101
EV_MORTAREFX =                	102
EV_SPINUP =                		103
EV_TESTID1 =                	104
EV_TESTID2 =                	105
EV_ENDTEST =                	106
EV_SNOW_ON =                	107
EV_SNOW_OFF =                	108
EV_MISSILE_MISS_SMALL =         109
EV_MISSILE_MISS_LARGE =         110
EV_WOLFKICK_HIT_FLESH =         111
EV_WOLFKICK_HIT_WALL =          112
EV_WOLFKICK_MISS =              113
EV_SPIT_HIT =                	114
EV_SPIT_MISS =                	115
EV_SHARD =                		116
EV_JUNK =                		117
EV_EMITTER =                	118
EV_OILPARTICLES =               119
EV_OILSLICK =                	120
EV_OILSLICKREMOVE =             121
EV_MG42EFX =                	122
EV_FLAMEBARREL_BOUNCE =         123
EV_FLAKGUN1 =                	124
EV_FLAKGUN2 =                	125
EV_FLAKGUN3 =                	126
EV_FLAKGUN4 =                	127
EV_EXERT1 =                		128
EV_EXERT2 =                		129
EV_EXERT3 =                		130
EV_SNOWFLURRY =                	131
EV_CONCUSSIVE =                	132
EV_DUST =                		133
EV_RUMBLE_EFX =                	134
EV_GUNSPARKS =                	135
EV_FLAMETHROWER_EFFECT =        136
EV_SNIPER_SOUND =               137
EV_POPUP =                		138
EV_POPUPBOOK =                	139
EV_GIVEPAGE =                	140
EV_MG42BULLET_HIT_FLESH =       141
EV_MG42BULLET_HIT_WALL =        142
EV_MEDIC_CALL =                	143
EV_BOT_DEBUG_LINE =             144
EV_BOT_DEBUG_RADIUS =           145
EV_MAX_EVENTS =                	146

-- success
return 1
