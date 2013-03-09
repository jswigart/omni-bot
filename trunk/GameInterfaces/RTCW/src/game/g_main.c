#include "g_local.h"
#include  "g_rtcwbot_interface.h"

#ifdef WITH_LUA
#include "g_lua.h"
#endif

level_locals_t level;

typedef struct {
	vmCvar_t    *vmCvar;
	char        *cvarName;
	char        *defaultString;
	int cvarFlags;
	int modificationCount;          // for tracking changes
	qboolean trackChange;           // track this variable, and announce if changed
	qboolean teamShader;        // track and if changed, update shader state
} cvarTable_t;

gentity_t g_entities[MAX_GENTITIES];
gclient_t g_clients[MAX_CLIENTS];

gentity_t       *g_camEnt = NULL;   //----(SA)	script camera

// Rafael gameskill
extern int bg_pmove_gameskill_integer;
// done

vmCvar_t g_OmniBotLibrary;
vmCvar_t g_OmniBotPath;
vmCvar_t g_OmniBotEnable;
vmCvar_t g_OmniBotFlags;
vmCvar_t g_OmniBotPlaying;
vmCvar_t g_OmniBotGib;
vmCvar_t g_dropItems;
vmCvar_t g_helmetProtection;
vmCvar_t g_waterRevive;
vmCvar_t g_warmupDamage;
vmCvar_t g_throwableKnives;
vmCvar_t g_maxKnives;
vmCvar_t g_knifeDamage;
vmCvar_t g_stopMovers;
vmCvar_t g_botTeam;
vmCvar_t g_centerView;
vmCvar_t g_instaGib;
vmCvar_t g_goomba;
vmCvar_t g_goombaDmg;
vmCvar_t g_reportGibs;
vmCvar_t g_reportRevives;
vmCvar_t g_reportTaps;
vmCvar_t g_msg1;
vmCvar_t g_msg2;
vmCvar_t g_msg3;
vmCvar_t g_msg4;
vmCvar_t g_msg5;
vmCvar_t g_msg6;
vmCvar_t g_msg7;
vmCvar_t g_msg8;
vmCvar_t g_msg9;
vmCvar_t g_msg10;
vmCvar_t g_msgs;
vmCvar_t g_msgtime;
vmCvar_t g_msgpos;
vmCvar_t g_debugPlayerHitboxes;
vmCvar_t g_awards;
vmCvar_t g_medExtraClips;
vmCvar_t g_ltExtraClips;
vmCvar_t g_engExtraClips;
vmCvar_t g_soldExtraClips;
vmCvar_t g_venomExtraClips;
vmCvar_t g_mauserExtraClips;
vmCvar_t g_pistolExtraClips;
vmCvar_t g_panzerExtraClips;
vmCvar_t g_deathmatch;
vmCvar_t g_poison;
vmCvar_t g_poisonDmg;
vmCvar_t g_LTNades;
vmCvar_t g_MedNades;
vmCvar_t g_smokeGrenades;
vmCvar_t g_nadePacks;
vmCvar_t g_betting; // bit flagged. 0 disable 1 enable 2 buying
vmCvar_t g_muteSpecs;
vmCvar_t g_kickItems; // bit flagged. 0 disable 1 grenades 2 airstrike
vmCvar_t g_binocs; // bit flagged. 0 disable 1 soldiers 2 engineers 4 medics
vmCvar_t g_unlockWeapons;
vmCvar_t g_flagLock; // checkpoints can't be recaptured
vmCvar_t g_venomSpread;
vmCvar_t g_venomDamage;

// zinx etpro antiwarp
vmCvar_t g_antiwarp;
vmCvar_t g_maxWarp;

// from noquarter
vmCvar_t g_multikillTime;
vmCvar_t g_announcer;
vmCvar_t g_shove;
vmCvar_t g_shoveNoZ;
vmCvar_t g_dragCorpse;
// end

// from sandmod
vmCvar_t g_fixedphysics;
vmCvar_t g_fakePing;
vmCvar_t g_LTinfomsg;
vmCvar_t g_allowGiveAmmo;
// end

// from etpub
vmCvar_t g_privateMessages;
vmCvar_t g_shortcuts;
vmCvar_t g_mapConfigs;
// end

#ifdef WITH_LUA
vmCvar_t lua_modules;
vmCvar_t lua_allowedModules;
#endif

vmCvar_t g_gametype;

// Rafael gameskill
vmCvar_t g_gameskill;
// done

vmCvar_t g_dmflags;
vmCvar_t g_fraglimit;
vmCvar_t g_timelimit;
vmCvar_t g_capturelimit;
vmCvar_t g_friendlyFire;
vmCvar_t g_password;
vmCvar_t g_maxclients;
vmCvar_t g_maxGameClients;
vmCvar_t g_minGameClients;          // NERVE - SMF
vmCvar_t g_dedicated;
vmCvar_t g_speed;
vmCvar_t g_gravity;
vmCvar_t g_cheats;
vmCvar_t g_knockback;
vmCvar_t g_forcerespawn;
vmCvar_t g_inactivity;
vmCvar_t g_debugMove;
vmCvar_t g_debugDamage;
vmCvar_t g_debugAlloc;
vmCvar_t g_debugBullets;    //----(SA)	added
vmCvar_t g_weaponRespawn;
vmCvar_t g_motd;
vmCvar_t g_synchronousClients;
vmCvar_t g_warmup;

// NERVE - SMF
vmCvar_t g_warmupLatch;
vmCvar_t g_nextTimeLimit;
vmCvar_t g_showHeadshotRatio;
vmCvar_t g_userTimeLimit;
vmCvar_t g_userAlliedRespawnTime;
vmCvar_t g_userAxisRespawnTime;
vmCvar_t g_currentRound;
vmCvar_t g_noTeamSwitching;
vmCvar_t g_altStopwatchMode;
vmCvar_t g_gamestate;
vmCvar_t g_swapteams;
// -NERVE - SMF

vmCvar_t g_restarted;
vmCvar_t g_log;
vmCvar_t g_logSync;
vmCvar_t g_voteFlags;
vmCvar_t g_complaintlimit;          // DHM - Nerve
vmCvar_t g_maxlives;                // DHM - Nerve
vmCvar_t g_voiceChatsAllowed;       // DHM - Nerve
vmCvar_t g_alliedmaxlives;          // Xian
vmCvar_t g_axismaxlives;            // Xian
vmCvar_t g_fastres;                 // Xian
vmCvar_t g_fastResMsec;
vmCvar_t g_knifeonly;               // Xian
vmCvar_t g_enforcemaxlives;         // Xian

vmCvar_t g_needpass;
vmCvar_t g_weaponTeamRespawn;
vmCvar_t g_doWarmup;
vmCvar_t g_teamAutoJoin;
vmCvar_t g_teamForceBalance;
vmCvar_t g_listEntity;
vmCvar_t g_banIPs;
vmCvar_t g_filterBan;
vmCvar_t g_rankings;
vmCvar_t g_enableBreath;
vmCvar_t pmove_fixed;
vmCvar_t pmove_msec;

// Rafael
vmCvar_t g_autoactivate;
vmCvar_t g_testPain;

vmCvar_t g_missionStats;
vmCvar_t ai_scriptName;         // name of AI script file to run (instead of default for that map)
vmCvar_t g_scriptName;          // name of script file to run (instead of default for that map)

vmCvar_t g_developer;

vmCvar_t g_userAim;

vmCvar_t g_forceModel;

vmCvar_t g_mg42arc;

vmCvar_t g_footstepAudibleRange;
// JPW NERVE multiplayer reinforcement times
vmCvar_t g_redlimbotime;
vmCvar_t g_bluelimbotime;
// charge times for character class special weapons
vmCvar_t g_medicChargeTime;
vmCvar_t g_engineerChargeTime;
vmCvar_t g_LTChargeTime;
vmCvar_t g_soldierChargeTime;
// screen shakey magnitude multiplier
vmCvar_t sv_screenshake;
// jpw

// Gordon
vmCvar_t g_antilag;

vmCvar_t mod_version;
vmCvar_t mod_url;
vmCvar_t url;

vmCvar_t g_dbgRevive;

vmCvar_t g_dynoFuseTime;

cvarTable_t gameCvarTable[] = {
	// debug vars

	{ &g_dynoFuseTime,              "g_dynoFuseTime",               "30000",                CVAR_CHEAT,                                         0,          qfalse },

	// default vars
	{ &g_cheats,                    "sv_cheats",                    "",                     0,                                                  0,          qfalse },
	{ NULL,                         "gamename",                     GAMEVERSION,			CVAR_SERVERINFO | CVAR_ROM,							0,          qfalse },
	{ NULL,                         "gamedate",                     __DATE__,				CVAR_ROM,											0,          qfalse },
	{ &g_restarted,                 "g_restarted",                  "0",                    CVAR_ROM,                                           0,          qfalse },
	{ NULL,                         "sv_mapname",                   "",                     CVAR_SERVERINFO | CVAR_ROM,                         0,          qfalse },
	{ &g_gametype,                  "g_gametype",                   "5",                    CVAR_SERVERINFO | CVAR_LATCH,                       0,          qfalse },
	{ &g_gameskill,                 "g_gameskill",                  "3",                    CVAR_SERVERINFO | CVAR_LATCH,                       0,          qfalse },
	{ &sv_screenshake,              "sv_screenshake",               "5",                    CVAR_ARCHIVE,                                       0,          qfalse },
	{ &g_redlimbotime,              "g_redlimbotime",               "3000",                 CVAR_SERVERINFO | CVAR_LATCH,                       0,          qfalse },
	{ &g_bluelimbotime,             "g_bluelimbotime",              "3000",                 CVAR_SERVERINFO | CVAR_LATCH,                       0,          qfalse },
	{ &g_medicChargeTime,           "g_medicChargeTime",            "45000",                CVAR_SERVERINFO | CVAR_LATCH,                       0,          qfalse },
	{ &g_engineerChargeTime,        "g_engineerChargeTime",         "30000",                CVAR_SERVERINFO | CVAR_LATCH,                       0,          qfalse },
	{ &g_LTChargeTime,              "g_LTChargeTime",               "40000",                CVAR_SERVERINFO | CVAR_LATCH,                       0,          qfalse },
	{ &g_soldierChargeTime,         "g_soldierChargeTime",          "20000",                CVAR_SERVERINFO | CVAR_LATCH,                       0,          qfalse },
	{ &g_maxclients,                "sv_maxclients",                "20",                   CVAR_SERVERINFO | CVAR_LATCH | CVAR_ARCHIVE,        0,          qfalse },
	{ &g_maxGameClients,            "g_maxGameClients",             "0",                    CVAR_SERVERINFO | CVAR_LATCH | CVAR_ARCHIVE,        0,          qfalse },
	{ &g_minGameClients,            "g_minGameClients",             "8",                    CVAR_SERVERINFO,                                    0,          qfalse },
	{ &g_dmflags,                   "dmflags",                      "0",                    CVAR_SERVERINFO,                                    0,          qtrue },
	{ &g_fraglimit,                 "fraglimit",                    "0",                    CVAR_SYSTEMINFO | CVAR_NORESTART,                   0,          qtrue },
	{ &g_timelimit,                 "timelimit",                    "0",                    CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART,    0,          qtrue },
	{ &g_capturelimit,              "capturelimit",                 "8",                    CVAR_ARCHIVE | CVAR_NORESTART,                      0,          qtrue },
	{ &g_synchronousClients,        "g_synchronousClients",         "0",                    CVAR_SYSTEMINFO,                                    0,          qfalse },
	{ &g_friendlyFire,              "g_friendlyFire",               "1",                    CVAR_SERVERINFO | CVAR_ARCHIVE,                     0,          qtrue },
	{ &g_teamForceBalance,          "g_teamForceBalance",           "0",                    CVAR_ARCHIVE },
	{ &g_warmup,                    "g_warmup",                     "20",                   CVAR_ARCHIVE,                                       0,          qtrue },
	{ &g_doWarmup,                  "g_doWarmup",                   "0",                    0,                                                  0,          qtrue },
	{ &g_warmupLatch,               "g_warmupLatch",                "1",                    0,                                                  0,          qfalse },
	{ &g_nextTimeLimit,             "g_nextTimeLimit",              "0",                    CVAR_WOLFINFO,                                      0,          qfalse },
	{ &g_currentRound,              "g_currentRound",               "0",                    CVAR_WOLFINFO,                                      0,          qfalse },
	{ &g_altStopwatchMode,          "g_altStopwatchMode",           "0",                    CVAR_ARCHIVE,                                       0,          qtrue },
	{ &g_gamestate,                 "gamestate",                    "-1",                   CVAR_WOLFINFO | CVAR_ROM,                           0,          qfalse },
	{ &g_noTeamSwitching,           "g_noTeamSwitching",            "0",                    CVAR_ARCHIVE,                                       0,          qtrue },
	{ &g_showHeadshotRatio,         "g_showHeadshotRatio",          "0",                    0,                                                  0,          qfalse },
	{ &g_userTimeLimit,             "g_userTimeLimit",              "0",                    0,                                                  0,          qfalse },
	{ &g_userAlliedRespawnTime,     "g_userAlliedRespawnTime",      "0",                    0,                                                  0,          qfalse },
	{ &g_userAxisRespawnTime,       "g_userAxisRespawnTime",        "0",                    0,                                                  0,          qfalse },
	{ &g_swapteams,                 "g_swapteams",                  "0",                    CVAR_ROM,                                           0,          qfalse },
	{ &g_log,                       "g_log",                        "",                     CVAR_ARCHIVE,                                       0,          qfalse },
	{ &g_logSync,                   "g_logSync",                    "0",                    CVAR_ARCHIVE,                                       0,          qfalse },
	{ &g_password,                  "g_password",                   "",                     CVAR_USERINFO,                                      0,          qfalse },
	{ &g_banIPs,                    "g_banIPs",                     "",                     CVAR_ARCHIVE,                                       0,          qfalse },
	{ &g_filterBan,                 "g_filterBan",                  "1",                    CVAR_ARCHIVE,                                       0,          qfalse },
	{ &g_dedicated,                 "dedicated",                    "0",                    0,                                                  0,          qfalse },
	{ &g_speed,                     "g_speed",                      "320",                  0,                                                  0,          qtrue },
	{ &g_gravity,                   "g_gravity",                    "800",                  0,                                                  0,          qtrue },
	{ &g_knockback,                 "g_knockback",                  "1000",                 0,                                                  0,          qtrue },
	{ &g_weaponRespawn,             "g_weaponrespawn",              "5",                    0,                                                  0,          qtrue },
	{ &g_weaponTeamRespawn,         "g_weaponTeamRespawn",          "30",                   0,                                                  0,          qtrue },
	{ &g_forcerespawn,              "g_forcerespawn",               "0",                    0,                                                  0,          qtrue },
	{ &g_inactivity,                "g_inactivity",                 "0",                    0,                                                  0,          qtrue },
	{ &g_debugMove,                 "g_debugMove",                  "0",                    0,                                                  0,          qfalse },
	{ &g_debugDamage,               "g_debugDamage",                "0",                    CVAR_CHEAT,                                         0,          qfalse },
	{ &g_debugAlloc,                "g_debugAlloc",                 "0",                    0,                                                  0,          qfalse },
	{ &g_debugBullets,              "g_debugBullets",               "0",                    CVAR_CHEAT,                                         0,          qfalse },
	{ &g_motd,                      "g_motd",                       "",                     CVAR_ARCHIVE,                                       0,          qfalse },
	{ &g_voteFlags,                 "g_voteFlags",                  "255",                  CVAR_ARCHIVE | CVAR_SERVERINFO,                     0,          qfalse },
	{ &g_listEntity,                "g_listEntity",                 "0",                    0,                                                  0,          qfalse },
	{ &g_complaintlimit,            "g_complaintlimit",             "3",                    CVAR_ARCHIVE,                                       0,          qtrue },
	{ &g_maxlives,                  "g_maxlives",                   "0",                    CVAR_ARCHIVE | CVAR_LATCH | CVAR_SERVERINFO,        0,          qtrue },
	{ &g_voiceChatsAllowed,         "g_voiceChatsAllowed",          "4",                    CVAR_ARCHIVE,                                       0,          qfalse },
	{ &g_alliedmaxlives,            "g_alliedmaxlives",             "0",                    CVAR_LATCH | CVAR_SERVERINFO,                       0,          qtrue },
	{ &g_axismaxlives,              "g_axismaxlives",               "0",                    CVAR_LATCH | CVAR_SERVERINFO,                       0,          qtrue },
	{ &g_fastres,                   "g_fastres",                    "0",                    CVAR_ARCHIVE,                                       0,          qtrue },
	{ &g_fastResMsec,               "g_fastResMsec",                "1000",                 CVAR_ARCHIVE,                                       0,          qtrue },
	{ &g_knifeonly,                 "g_knifeonly",                  "0",                    0,                                                  0,          qtrue },
	{ &g_enforcemaxlives,           "g_enforcemaxlives",            "1",                    CVAR_ARCHIVE,                                       0,          qtrue },
	{ &g_enableBreath,              "g_enableBreath",               "1",                    CVAR_SERVERINFO,                                    0,          qtrue },
	{ &g_testPain,                  "g_testPain",                   "0",                    CVAR_CHEAT,                                         0,          qfalse },
	{ &g_missionStats,              "g_missionStats",               "0",                    CVAR_ROM,                                           0,          qfalse },
	{ &g_developer,                 "developer",                    "0",                    CVAR_TEMP,                                          0,          qfalse },
	{ &g_rankings,                  "g_rankings",                   "0",                    0,                                                  0,          qfalse },
	{ &g_userAim,                   "g_userAim",                    "1",                    CVAR_CHEAT,                                         0,          qfalse },
	{ &pmove_fixed,                 "pmove_fixed",                  "0",                    CVAR_SYSTEMINFO,                                    0,          qfalse },
	{ &pmove_msec,                  "pmove_msec",                   "8",                    CVAR_SYSTEMINFO,                                    0,          qfalse },
	{ &g_mg42arc,                   "g_mg42arc",                    "0",                    CVAR_TEMP,                                          0,          qfalse },
	{ &g_footstepAudibleRange,      "g_footstepAudibleRange",       "256",                  CVAR_CHEAT,                                         0,          qfalse },
	{ &g_scriptName,                "g_scriptName",                 "",                     CVAR_ROM,                                           0,          qfalse },
	{ &ai_scriptName,               "ai_scriptName",                "",                     CVAR_ROM,                                           0,          qfalse },
	{ &mod_version,                 "mod_version",                  OB_VERSION,             CVAR_SERVERINFO | CVAR_ROM,                         0,          qfalse },
	{ &mod_url,                     "mod_url",                      "www.omni-bot.com",     CVAR_SERVERINFO | CVAR_ROM,                         0,          qfalse },
	{ &url,                         "URL",                          "",                     CVAR_SERVERINFO | CVAR_ARCHIVE,                     0,          qfalse },
	{ &g_antilag,                   "g_antilag",                    "0",                    0,													0,          qfalse },
	{ &g_dbgRevive,                 "g_dbgRevive",                  "0",                    0,                                                  0,          qfalse },

	// mod vars
	{ &g_OmniBotLibrary,			"omnibot_library",				"omnibot_rtcw",			CVAR_ROM,											0,			qfalse },
	{ &g_OmniBotPath,               "omnibot_path",                 "",                     CVAR_ARCHIVE | CVAR_NORESTART,                      0,          qfalse },
	{ &g_OmniBotEnable,             "omnibot_enable",               "1",                    CVAR_ARCHIVE | CVAR_NORESTART,                      0,          qfalse },
	{ &g_OmniBotPlaying,            "omnibot_playing",              "0",                    CVAR_SERVERINFO | CVAR_ROM,                         0,          qfalse },
	{ &g_OmniBotFlags,              "omnibot_flags",                "0",                    CVAR_ARCHIVE | CVAR_NORESTART,                      0,          qfalse },
	{ &g_OmniBotGib,                "g_botGib",                     "1",                    0,                                                  0,          qfalse },
	{ &g_dropItems,                 "g_dropItems",                  "0",                    0,                                                  0,          qfalse },
	{ &g_helmetProtection,          "g_helmetProtection",           "0",                    0,                                                  0,          qtrue },
	{ &g_waterRevive,               "g_waterRevive",                "0",                    0,                                                  0,          qtrue },
	{ &g_warmupDamage,              "g_warmupDamage",               "0",                    0,                                                  0,          qtrue },
	{ &g_throwableKnives,           "g_throwableKnives",            "0",                    0,                                                  0,          qtrue },
	{ &g_maxKnives,                 "g_maxKnives",                  "5",                    0,                                                  0,          qtrue },
	{ &g_knifeDamage,               "g_knifeDamage",                "35",                   0,                                                  0,          qfalse },
	{ &g_stopMovers,                "g_stopMovers",                 "0",                    0,                                                  0,          qfalse },
	{ &g_botTeam,                   "g_botTeam",                    "0",                    0,                                                  0,          qfalse },
	{ &g_centerView,                "g_centerView",                 "0",                    0,                                                  0,          qfalse },
	{ &g_instaGib,                  "g_instaGib",                   "0",                    0,                                                  0,          qtrue },
	{ &g_goomba,                    "g_goomba",                     "0",                    0,                                                  0,          qtrue },
	{ &g_goombaDmg,                 "g_goombaDmg",                  "1",                    0,                                                  0,          qfalse },
	{ &g_reportGibs,                "g_reportGibs",                 "0",                    0,                                                  0,          qfalse },
	{ &g_reportRevives,             "g_reportRevives",              "0",                    0,                                                  0,          qfalse },
	{ &g_reportTaps,                "g_reportTaps",                 "0",                    0,                                                  0,          qfalse },
	{ &g_msg1,                      "g_msg1",                       "",                     0,                                                  0,          qfalse },
	{ &g_msg2,                      "g_msg2",                       "",                     0,                                                  0,          qfalse },
	{ &g_msg3,                      "g_msg3",                       "",                     0,                                                  0,          qfalse },
	{ &g_msg4,                      "g_msg4",                       "",                     0,                                                  0,          qfalse },
	{ &g_msg5,                      "g_msg5",                       "",                     0,                                                  0,          qfalse },
	{ &g_msg6,                      "g_msg6",                       "",                     0,                                                  0,          qfalse },
	{ &g_msg7,                      "g_msg7",                       "",                     0,                                                  0,          qfalse },
	{ &g_msg8,                      "g_msg8",                       "",                     0,                                                  0,          qfalse },
	{ &g_msg9,                      "g_msg9",                       "",                     0,                                                  0,          qfalse },
	{ &g_msg10,                     "g_msg10",                      "",                     0,                                                  0,          qfalse },
	{ &g_msgs,                      "g_msgs",                       "0",                    0,                                                  0,          qfalse },
	{ &g_msgtime,                   "g_msgtime",                    "45",                   0,                                                  0,          qfalse },
	{ &g_msgpos,                    "g_msgpos",                     "0",                    0,                                                  0,          qfalse },
	{ &g_debugPlayerHitboxes,       "g_debugPlayerHitboxes",        "0",                    CVAR_CHEAT,                                         0,          qfalse },
	{ &g_awards,                    "g_awards",                     "0",                    0,                                                  0,          qtrue },
	{ &g_soldExtraClips,            "g_soldExtraClips",             "0",                    0,                                                  0,          qfalse },
	{ &g_medExtraClips,             "g_medExtraClips",              "0",                    0,                                                  0,          qfalse },
	{ &g_ltExtraClips,              "g_ltExtraClips",               "0",                    0,                                                  0,          qfalse },
	{ &g_engExtraClips,             "g_engExtraClips",              "0",                    0,                                                  0,          qfalse },
	{ &g_pistolExtraClips,          "g_pistolExtraClips",           "0",                    0,                                                  0,          qfalse },
	{ &g_mauserExtraClips,          "g_mauserExtraClips",           "0",                    0,                                                  0,          qfalse },
	{ &g_venomExtraClips,           "g_venomExtraClips",            "0",                    0,                                                  0,          qfalse },
	{ &g_panzerExtraClips,          "g_panzerExtraClips",           "0",                    0,                                                  0,          qfalse },
	{ &g_deathmatch,                "g_deathmatch",                 "0",                    CVAR_SYSTEMINFO | CVAR_LATCH,                       0,          qfalse },
	{ &g_poison,                    "g_poison",                     "0",                    0,                                                  0,          qfalse },
	{ &g_poisonDmg,                 "g_poisonDmg",                  "10",                   0,                                                  0,          qfalse },
	{ &g_LTNades,                   "g_LTNades",                    "1",                    0,                                                  0,          qfalse },
	{ &g_MedNades,                  "g_MedNades",                   "1",                    0,                                                  0,          qfalse },
	{ &g_smokeGrenades,             "g_smokeGrenades",              "0",                    0,                                                  0,          qfalse },
	{ &g_nadePacks,                 "g_nadePacks",                  "0",                    0,                                                  0,          qfalse },
	{ &g_betting,                   "g_betting",                    "0",                    0,                                                  0,          qfalse },
	{ &g_muteSpecs,                 "g_muteSpecs",                  "0",                    0,                                                  0,          qfalse },
	{ &g_kickItems,                 "g_kickItems",                  "0",                    0,                                                  0,          qfalse },
	{ &g_binocs,                    "g_binocs",                     "0",                    0,                                                  0,          qfalse },
	{ &g_unlockWeapons,             "g_unlockWeapons",              "0",                    0,                                                  0,          qfalse },
	{ &g_flagLock,                  "g_flagLock",                   "0",                    CVAR_LATCH,                                         0,          qfalse },
	{ &g_venomSpread,               "g_venomSpread",                "600",                  0,                                                  0,          qfalse },
	{ &g_venomDamage,               "g_venomDamage",                "20",                   0,                                                  0,          qfalse },

	// zinx etpro antiwarp
	{ &g_maxWarp,                   "g_maxWarp",                    "4",                    0 },
	{ &g_antiwarp,                  "g_antiwarp",                   "1",                    0 },

#ifdef WITH_LUA
	{ &lua_modules,                 "lua_modules",                  "",                     0 },
	{ &lua_allowedModules,          "lua_allowedModules",           "",                     0 },
#endif

	// from noquarter
	{ &g_shove,                     "g_shove",                      "0",                    CVAR_ARCHIVE,       0,      qtrue },
	{ &g_shoveNoZ,                  "g_shoveNoZ",                   "1",                    CVAR_ARCHIVE,       0,      qtrue },
	{ &g_dragCorpse,                "g_dragCorpse",                 "0",                    CVAR_ARCHIVE,       0,      qtrue },
	{ &g_announcer,                 "g_announcer",                  "0",                    0 },
	{ &g_multikillTime,             "g_multikillTime",              "1000",                 0 },
	// end noquarter

	// from sandmod
	{ &g_fixedphysics,              "g_fixedphysics",               "0",                    0,                  0,      qfalse },
	{ &g_fakePing,                  "g_fakePing",                   "0",                    0,                  0,      qfalse },
	{ &g_LTinfomsg,                 "g_LTinfomsg",                  "0",                    0,                  0,      qfalse },
	{ &g_allowGiveAmmo,             "g_allowGiveAmmo",              "0",                    0,                  0,      qfalse },
	// end sandmod

	// from etpub
	{ &g_privateMessages,           "g_privateMessages",            "0",                    0,                  0,      qtrue },
	{ &g_shortcuts,                 "g_shortcuts",                  "0",                    0,                  0,      qtrue },
	{ &g_mapConfigs,                "g_mapConfigs",                 "",                     0 },
	// end etpub

	//http downloads - if no server is given client will try s4ndmod.com/downloads
	{0,                             "g_httpdownhost",               "",                     CVAR_SERVERINFO,    0,      qfalse}
};

// bk001129 - made static to avoid aliasing
static int gameCvarTableSize = sizeof( gameCvarTable ) / sizeof( gameCvarTable[0] );

void G_InitGame( int levelTime, int randomSeed, int restart );
void G_RunFrame( int levelTime );
void G_ShutdownGame( int restart );
void CheckExitRules( void );

void G_HandleMessage( int _clientfrom, const char *_buffer, int _messagesize, int _commandtime );

// Ridah, Cast AI
qboolean AICast_VisibleFromPos( vec3_t srcpos, int srcnum,
							   vec3_t destpos, int destnum, qboolean updateVisPos );
qboolean AICast_CheckAttackAtPos( int entnum, int enemy, vec3_t pos, qboolean ducking, qboolean allowHitWorld );
void AICast_Init( void );
// done.

void G_RetrieveMoveSpeedsFromClient( int entnum, char *moveSpeedText );

/*
================
vmMain

This is the only way control passes into the module.
This must be the very first function compiled into the .q3vm file
================
*/
#if defined( __MACOS__ )
#pragma export on
#endif
int vmMain( int command, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6 ) {
#if defined( __MACOS__ )
#pragma export off
#endif
	switch ( command ) {
	case GAME_INIT:
		Bot_Interface_InitHandles();
		G_InitGame( arg0, arg1, arg2 );
		if ( !Bot_Interface_Init() ) {
			G_Printf( S_COLOR_RED "Unable to Initialize Omni-Bot.\n" );
		}
		return 0;
	case GAME_SHUTDOWN:
		if ( !Bot_Interface_Shutdown() ) {
			G_Printf( S_COLOR_RED "Error shutting down Omni-Bot.\n" );
		}
		G_ShutdownGame( arg0 );
		return 0;
	case GAME_CLIENT_CONNECT:
		return (int)ClientConnect( arg0, arg1, arg2 );
	case GAME_CLIENT_THINK:
		ClientThink( arg0 );
		return 0;
	case GAME_CLIENT_USERINFO_CHANGED:
		ClientUserinfoChanged( arg0 );
		return 0;
	case GAME_CLIENT_DISCONNECT:
		ClientDisconnect( arg0 );
		return 0;
	case GAME_CLIENT_BEGIN:
		ClientBegin( arg0 );
		return 0;
	case GAME_CLIENT_COMMAND:
		ClientCommand( arg0 );
		return 0;
	case GAME_MESSAGERECEIVED:
		{
			int iClientFrom = arg0;
			const char *pBuffer = (const char *)arg1;
			int iBufferLength = arg2;
			int iCommandTime = arg3;
			G_HandleMessage( iClientFrom, pBuffer, iBufferLength, iCommandTime );
			return -1;
		}
	case GAME_RUN_FRAME:
		G_RunFrame( arg0 );
		Bot_Interface_Update();
		return 0;
	case GAME_CONSOLE_COMMAND:
		return ConsoleCommand();
	case BOTAI_START_FRAME:
		return 0;
		// Ridah, Cast AI
	case AICAST_VISIBLEFROMPOS:
		return qfalse;
	case AICAST_CHECKATTACKATPOS:
		return qfalse;
	case GAME_RETRIEVE_MOVESPEEDS_FROM_CLIENT:
		G_RetrieveMoveSpeedsFromClient( arg0, (char *)arg1 );
		return 0;
	}

	return -1;
}

void QDECL G_Printf( const char *fmt, ... ) {
	va_list argptr;
	char text[1024];

	va_start( argptr, fmt );
	Q_vsnprintf( text, sizeof( text ), fmt, argptr );
	va_end( argptr );

#ifdef WITH_LUA
	G_LuaHook_Print( text );
#endif

	trap_Printf( text );
}

void QDECL G_DPrintf( const char *fmt, ... ) {
	va_list argptr;
	char text[1024];

	if ( !g_developer.integer ) {
		return;
	}

	va_start( argptr, fmt );
	Q_vsnprintf( text, sizeof( text ), fmt, argptr );
	va_end( argptr );

	trap_Printf( text );
}

void QDECL G_Error( const char *fmt, ... ) {
	va_list argptr;
	char text[1024];

	va_start( argptr, fmt );
	Q_vsnprintf( text, sizeof( text ), fmt, argptr );
	va_end( argptr );

	trap_Error( text );
}

#define CH_KNIFE_DIST       48  // from g_weapon.c
#define CH_LADDER_DIST      100
#define CH_WATER_DIST       100
#define CH_BREAKABLE_DIST   64
#define CH_DOOR_DIST        96
#define CH_ACTIVATE_DIST    96
#define CH_EXIT_DIST        256

#define CH_MAX_DIST         256     // use the largest value from above
#define CH_MAX_DIST_ZOOM    8192    // max dist for zooming hints

//----------------------------------------------------------//

/*
==============
G_CheckForCursorHints
non-AI's check for cursor hint contacts

server-side because there's info we want to show that the client
just doesn't know about.  (health or other info of an explosive,invisible_users,items,etc.)

traceEnt is the ent hit by the trace, checkEnt is the ent that is being
checked against (in case the traceent was an invisible_user or something)

==============
*/
void G_CheckForCursorHints( gentity_t *ent ) {
	vec3_t forward, right, up, offset, end;
	trace_t     *tr;
	float dist;
	gentity_t   *checkEnt, *traceEnt = 0;
	playerState_t *ps;
	int hintType, hintDist, hintVal;
	qboolean zooming;
	int trace_contents;                 // DHM - Nerve

	if ( !ent->client || ent->r.svFlags & SVF_BOT ) {
		return;
	}

	ps = &ent->client->ps;

	zooming = (qboolean)( ps->eFlags & EF_ZOOMING );

	AngleVectors( ps->viewangles, forward, right, up );

	VectorCopy( ps->origin, offset );
	offset[2] += ps->viewheight;

	// lean
	if ( ps->leanf ) {
		VectorMA( offset, ps->leanf, right, offset );
	}

	if ( zooming ) {
		VectorMA( offset, CH_MAX_DIST_ZOOM, forward, end );
	} else {
		VectorMA( offset, CH_MAX_DIST, forward, end );
	}

	tr = &ps->serverCursorHintTrace;

	trace_contents = ( CONTENTS_TRIGGER | CONTENTS_SOLID | CONTENTS_PLAYERCLIP | CONTENTS_BODY | CONTENTS_CORPSE );
	trap_Trace( tr, offset, NULL, NULL, end, ps->clientNum, trace_contents );
	// dhm - end

	// reset all
	hintType    = ps->serverCursorHint      = HINT_NONE;
	hintVal     = ps->serverCursorHintVal   = 0;

	if ( zooming ) {
		dist        = tr->fraction * CH_MAX_DIST_ZOOM;
		hintDist    = CH_MAX_DIST_ZOOM;
	} else {
		dist        = tr->fraction * CH_MAX_DIST;
		hintDist    = CH_MAX_DIST;
	}

	if ( tr->fraction == 1 ) {
		return;
	}

	traceEnt = &g_entities[tr->entityNum];

	// DHM - Nerve :: Ignore trigger_objective_info
	if ( ( ps->stats[ STAT_PLAYER_CLASS ] != PC_MEDIC && traceEnt->client )
		|| !( strcmp( traceEnt->classname, "trigger_objective_info" ) )
		|| !( strcmp( traceEnt->classname, "trigger_multiple" ) ) ) {
			trap_Trace( tr, offset, NULL, NULL, end, traceEnt->s.number, trace_contents );
			if ( zooming ) {
				dist        = tr->fraction * CH_MAX_DIST_ZOOM;
				hintDist    = CH_MAX_DIST_ZOOM;
			} else {
				dist        = tr->fraction * CH_MAX_DIST;
				hintDist    = CH_MAX_DIST;
			}
			if ( tr->fraction == 1 ) {
				return;
			}
			traceEnt = &g_entities[tr->entityNum];
	}

	//
	// WORLD
	//
	if ( tr->entityNum == ENTITYNUM_WORLD ) {
		if ( ( tr->contents & CONTENTS_WATER ) && !( ps->powerups[PW_BREATHER] ) ) {
			hintDist = CH_WATER_DIST;
			hintType = HINT_WATER;
		}
		// ladder
		else if ( ( tr->surfaceFlags & SURF_LADDER ) && !( ps->pm_flags & PMF_LADDER ) ) {
			hintDist = CH_LADDER_DIST;
			hintType = HINT_LADDER;
		}
	}
	//
	// PEOPLE
	//
	else if ( tr->entityNum < MAX_CLIENTS ) {
		// DHM - Nerve
		if ( g_gametype.integer >= GT_WOLF ) {
			// Show medics a syringe if they can revive someone
			if ( traceEnt->client && traceEnt->client->sess.sessionTeam == ent->client->sess.sessionTeam
				&& ps->stats[ STAT_PLAYER_CLASS ] == PC_MEDIC
				&& traceEnt->client->ps.pm_type == PM_DEAD
				&& !( traceEnt->client->ps.pm_flags & PMF_LIMBO ) ) {
					hintDist    = 48;     // JPW NERVE matches weapon_syringe in g_weapon.c
					hintType    = HINT_REVIVE;
			}
		}
		// dhm - Nerve
	}
	//
	// OTHER ENTITIES
	//
	else {
		checkEnt = traceEnt;

		// check invisible_users first since you don't want to draw a hint based
		// on that ent, but rather on what they are targeting.
		// so find the target and set checkEnt to that to show the proper hint.
		if ( traceEnt->s.eType == ET_GENERAL ) {
			// ignore trigger_aidoor.  can't just not trace for triggers, since I need invisible_users...
			// damn, I would like to ignore some of these triggers though.
			if ( !Q_stricmp( traceEnt->classname, "trigger_aidoor" ) ) {
				return;
			}

			if ( !Q_stricmp( traceEnt->classname, "func_invisible_user" ) ) {
				// DHM - Nerve :: Put this back in only in multiplayer
				if ( g_gametype.integer >= GT_WOLF && traceEnt->s.dmgFlags ) { // hint icon specified in entity
					hintType = traceEnt->s.dmgFlags;
					hintDist = CH_ACTIVATE_DIST;
					checkEnt = 0;
				} else {                      // use target for hint icon
					checkEnt = G_Find( NULL, FOFS( targetname ), traceEnt->target );
					if ( !checkEnt ) {     // no target found
						hintType = HINT_BAD_USER;
						hintDist = CH_MAX_DIST_ZOOM;    // show this one from super far for debugging
					}
				}
			}
		}

		if ( checkEnt ) {
			if ( checkEnt->s.eType == ET_GENERAL || checkEnt->s.eType == ET_MG42_BARREL ) {
				// this is effectively an 'exit' brush.  they should be created with:
				//
				// classname = 'ai_trigger'
				// ainame = 'player'
				// target = 'endmap'
				//
				if ( !Q_stricmp( traceEnt->classname, "ai_trigger" ) ) {
					if ( ( !Q_stricmp( traceEnt->aiName, "player" ) ) &&
						( !Q_stricmp( traceEnt->target, "endmap" ) ) ) {
							hintDist = CH_EXIT_DIST;
							hintType = HINT_EXIT;

							// show distance in the cursorhint bar
							hintVal = (int)dist;        // range for this hint is 256, so it happens to translate nicely
					}
				}

				if ( ( // general mg42 hint conditions
					!Q_stricmp( traceEnt->classname, "misc_mg42" ) ) &&
					( ps->weapon != WP_SNIPERRIFLE ) && // JPW NERVE no hint if you're scoped in sniperwise
					// ATVI #470
					// mount hint conditions only, if busted MG42, no check
					(
					( traceEnt->health < 255 ) ||
					(
					!( ent->client->ps.pm_flags & PMF_DUCKED ) &&
					( traceEnt->r.currentOrigin[2] - ent->r.currentOrigin[2] < 40 ) &&
					( traceEnt->r.currentOrigin[2] - ent->r.currentOrigin[2] > 0 ) &&
					!infront( traceEnt, ent )
					)
					)
					) {
						hintDist = CH_ACTIVATE_DIST;
						hintType = HINT_MG42;

						// JPW NERVE -- busted MG42 shouldn't show a use hint by default
						if ( traceEnt->health <= 0 ) {
							hintDist = 0;
							hintType = HINT_FORCENONE;
							ps->serverCursorHint = HINT_FORCENONE;
						}
						// jpw

						// DHM - Nerve :: Engineers can repair turrets
						if ( g_gametype.integer >= GT_WOLF ) {
							if ( ps->stats[ STAT_PLAYER_CLASS ] == PC_ENGINEER ) {
								if ( !traceEnt->takedamage ) {
									hintType = HINT_BUILD;
									hintDist = CH_BREAKABLE_DIST;
									hintVal = traceEnt->health;
									if ( hintVal > 255 ) {
										hintVal = 255;
									}
								}
							}
						}
						// dhm - end
				}
			} else if ( checkEnt->s.eType == ET_EXPLOSIVE )      {
				if ( ( checkEnt->health > 0 ) || ( checkEnt->spawnflags & 64 ) ) {    // JPW NERVE they are now if it's dynamite			// 0 health explosives are not breakable
					hintDist    = CH_BREAKABLE_DIST;
					hintType    = HINT_BREAKABLE;

					// DHM - Nerve :: Show different hint if it can only be destroyed with dynamite
					if ( g_gametype.integer >= GT_WOLF && ( checkEnt->spawnflags & 64 ) ) {
						// JPW NERVE only show hint for players who can blow it up
						vec3_t mins,maxs,range = { 40, 40, 52 };
						int i,num;
						int touch[MAX_GENTITIES];
						gentity_t *hit = NULL;

						VectorSubtract( ent->client->ps.origin, range, mins );
						VectorAdd( ent->client->ps.origin, range, maxs );
						num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );
						for ( i = 0 ; i < num ; i++ ) {
							hit = &g_entities[touch[i]];
							if ( !( hit->r.contents & CONTENTS_TRIGGER ) ) {
								continue;
							}
							if ( !strcmp( hit->classname,"trigger_objective_info" ) ) {
								if ( !( hit->spawnflags & ( AXIS_OBJECTIVE | ALLIED_OBJECTIVE ) ) ) {
									continue;
								}
								// we're in a trigger_objective_info field with at least one owner, so use this one and bail
								break;
							}
						}
						if ( ( hit ) &&
							( ( ( ent->client->sess.sessionTeam == TEAM_RED ) && ( hit->spawnflags & ALLIED_OBJECTIVE ) ) ||
							( ( ent->client->sess.sessionTeam == TEAM_BLUE ) && ( hit->spawnflags & AXIS_OBJECTIVE ) ) )
							) {
								hintDist = CH_BREAKABLE_DIST * 2;
								hintType = HINT_BREAKABLE_DYNAMITE;
						} else {
							hintDist = 0;
							hintType = ps->serverCursorHint     = HINT_FORCENONE;
							hintVal     = ps->serverCursorHintVal   = 0;
							return;
						}
						// jpw
					}
					// dhm - end

					hintVal     = checkEnt->health;     // also send health to client for visualization
				}
			} else if ( checkEnt->s.eType == ET_ALARMBOX )      {
				if ( checkEnt->health > 0 ) {
					hintType    = HINT_ACTIVATE;
				}
			} else if ( checkEnt->s.eType == ET_ITEM )      {
				gitem_t *it;
				it = &bg_itemlist[checkEnt->item - bg_itemlist];

				hintDist = CH_ACTIVATE_DIST;

				switch ( it->giType ) {
				case IT_HEALTH:
					hintType = HINT_HEALTH;
					break;
				case IT_TREASURE:
					hintType = HINT_TREASURE;
					break;
				case IT_CLIPBOARD:
					hintType = HINT_CLIPBOARD;
					break;
				case IT_WEAPON:
					// JPW NERVE changed this to be more specific
					if ( it->giTag != WP_LUGER && it->giTag != WP_COLT ) {
						if ( ps->stats[STAT_PLAYER_CLASS] == PC_SOLDIER ) {   // soldier can pick up all weapons
							hintType = HINT_WEAPON;
							break;
						}
						if ( ps->stats[STAT_PLAYER_CLASS] != PC_LT ) { // medics & engrs can't pick up squat
							break;
						}
						if ( it->giTag == WP_THOMPSON || it->giTag == WP_MP40 || it->giTag == WP_STEN ) {
							hintType = HINT_WEAPON;
						}
					}
					// jpw
					break;
				case IT_AMMO:
					hintType = HINT_AMMO;
					break;
				case IT_ARMOR:
					hintType = HINT_ARMOR;
					break;
				case IT_POWERUP:
					hintType = HINT_POWERUP;
					break;
				case IT_HOLDABLE:
					hintType = HINT_HOLDABLE;
					break;
				case IT_KEY:
					hintType = HINT_INVENTORY;
					break;
				case IT_TEAM:
					if ( !Q_stricmp( traceEnt->classname, "team_CTF_redflag" ) && ent->client->sess.sessionTeam == TEAM_BLUE ) {
						hintType = HINT_POWERUP;
					} else if ( !Q_stricmp( traceEnt->classname, "team_CTF_blueflag" ) && ent->client->sess.sessionTeam == TEAM_RED ) {
						hintType = HINT_POWERUP;
					}
					break;
				case IT_BAD:
				default:
					break;
				}
			} else if ( checkEnt->s.eType == ET_MOVER )     {
				if ( !Q_stricmp( checkEnt->classname, "func_door_rotating" ) ) {
					if ( checkEnt->moverState == MOVER_POS1ROTATE ) {  // stationary/closed
						hintDist = CH_DOOR_DIST;
						hintType = HINT_DOOR_ROTATING;
					}
				} else if ( !Q_stricmp( checkEnt->classname, "func_door" ) )        {
					if ( checkEnt->moverState == MOVER_POS1 ) {    // stationary/closed
						hintDist = CH_DOOR_DIST;
						hintType = HINT_DOOR;
					}
				} else if ( !Q_stricmp( checkEnt->classname, "func_button" ) )        {
					hintDist = CH_ACTIVATE_DIST;
					hintType = HINT_BUTTON;
				}
			}

			// DHM - Nerve :: Handle wolf multiplayer hints
			if ( g_gametype.integer >= GT_WOLF ) {
				if ( checkEnt->s.eType == ET_MISSILE ) {
					if ( ps->stats[ STAT_PLAYER_CLASS ] == PC_ENGINEER ) {
						hintDist    = CH_BREAKABLE_DIST;
						hintType    = HINT_DISARM;
						hintVal     = checkEnt->health;     // also send health to client for visualization
						if ( hintVal > 255 ) {
							hintVal = 255;
						}
					}
				}
			}
			// dhm - end

			// hint icon specified in entity (and proper contact was made, so hintType was set)
			// first try the checkent...
			if ( checkEnt->s.dmgFlags && hintType ) {
				hintType = checkEnt->s.dmgFlags;
			}
		}

		// then the traceent
		if ( traceEnt->s.dmgFlags && hintType ) {
			hintType = traceEnt->s.dmgFlags;
		}
	}

	if ( zooming ) {
		hintDist = CH_MAX_DIST_ZOOM;

		// zooming can eat a lot of potential hints
		switch ( hintType ) {
			// allow while zooming
		case HINT_PLAYER:
		case HINT_TREASURE:
		case HINT_LADDER:
		case HINT_EXIT:
		case HINT_PLYR_FRIEND:
		case HINT_PLYR_NEUTRAL:
		case HINT_PLYR_ENEMY:
		case HINT_PLYR_UNKNOWN:
			break;

		default:
			return;
		}
	}

	if ( dist <= hintDist ) {
		ps->serverCursorHint = hintType;
		ps->serverCursorHintVal = hintVal;
	}
}

/*
================
G_FindTeams

Chain together all entities with a matching team field.
Entity teams are used for item groups and multi-entity mover groups.

All but the first will have the FL_TEAMSLAVE flag set and teammaster field set
All but the last will have the teamchain field set to the next one
================
*/
void G_FindTeams( void ) {
	gentity_t   *e, *e2;
	int i, j;
	int c, c2;

	c = 0;
	c2 = 0;
	for ( i = 1, e = g_entities + i ; i < level.num_entities ; i++,e++ ) {
		if ( !e->inuse ) {
			continue;
		}

		if ( !e->team ) {
			continue;
		}

		if ( e->flags & FL_TEAMSLAVE ) {
			continue;
		}

		if ( !Q_stricmp( e->classname, "func_tramcar" ) ) {
			if ( e->spawnflags & 8 ) { // leader
				e->teammaster = e;
			} else
			{
				continue;
			}
		}

		c++;
		c2++;
		for ( j = i + 1, e2 = e + 1 ; j < level.num_entities ; j++,e2++ )
		{
			if ( !e2->inuse ) {
				continue;
			}
			if ( !e2->team ) {
				continue;
			}
			if ( e2->flags & FL_TEAMSLAVE ) {
				continue;
			}
			if ( !strcmp( e->team, e2->team ) ) {
				c2++;
				e2->teamchain = e->teamchain;
				e->teamchain = e2;
				e2->teammaster = e;
				e2->flags |= FL_TEAMSLAVE;

				if ( !Q_stricmp( e2->classname, "func_tramcar" ) ) {
					trap_UnlinkEntity( e2 );
				}

				// make sure that targets only point at the master
				if ( e2->targetname ) {
					e->targetname = e2->targetname;

					// Rafael
					// note to self: added this because of problems
					// pertaining to keys and double doors
					if ( Q_stricmp( e2->classname, "func_door_rotating" ) ) {
						e2->targetname = NULL;
					}
				}
			}
		}
	}

	G_Printf( "%i teams with %i entities\n", c, c2 );
}

/*
==============
G_RemapTeamShaders
==============
*/
void G_RemapTeamShaders() {
}

/*
=================
G_RegisterCvars
=================
*/
void G_RegisterCvars( void ) {
	int i;
	cvarTable_t *cv;
	qboolean remapped = qfalse;

	for ( i = 0, cv = gameCvarTable ; i < gameCvarTableSize ; i++, cv++ ) {
		trap_Cvar_Register( cv->vmCvar, cv->cvarName,
			cv->defaultString, cv->cvarFlags );
		if ( cv->vmCvar ) {
			cv->modificationCount = cv->vmCvar->modificationCount;
		}

		if ( cv->teamShader ) {
			remapped = qtrue;
		}
	}

	if ( remapped ) {
		G_RemapTeamShaders();
	}

	// check some things
	// DHM - Gametype is currently restricted to supported types only
	if ( g_gametype.integer < GT_WOLF || g_gametype.integer > GT_WOLF_CPH ) { // JPW NERVE WOLF_CPH now highest type
		G_Printf( "g_gametype %i is out of range, defaulting to GT_WOLF(5)\n", g_gametype.integer );
		trap_Cvar_Set( "g_gametype", "5" );
		trap_Cvar_Update( &g_gametype );
	}

	if ( g_deathmatch.integer ) {
		trap_SendConsoleCommand( EXEC_APPEND, va( "timelimit 0\n" ) );

		// set a default frag limit if one isn't set
		if ( g_fraglimit.integer == 0 ) {
			trap_SendConsoleCommand( EXEC_APPEND, va( "fraglimit 50\n" ) );
		}
	}

	// Rafael gameskill
	if ( g_gameskill.integer < GSKILL_EASY || g_gameskill.integer > GSKILL_VERYHARD ) {
		G_Printf( "g_gameskill %i is out of range, default to medium\n", g_gameskill.integer );
		trap_Cvar_Set( "g_gameskill", "3" ); // default to medium
	}

	bg_pmove_gameskill_integer = g_gameskill.integer;
	// done

	level.warmupModificationCount = g_warmup.modificationCount;
}

/*
=================
G_UpdateCvars
=================
*/
void G_UpdateCvars( void ) {
	int i;
	cvarTable_t *cv;
	qboolean remapped = qfalse;

	for ( i = 0, cv = gameCvarTable ; i < gameCvarTableSize ; i++, cv++ ) {
		if ( cv->vmCvar ) {
			trap_Cvar_Update( cv->vmCvar );

			if ( cv->modificationCount != cv->vmCvar->modificationCount ) {
				cv->modificationCount = cv->vmCvar->modificationCount;

				if ( cv->trackChange ) {
					trap_SendServerCommand( -1, va( "print \"Server:[lof] %s [lon]changed to[lof] %s\n\"",
						cv->cvarName, cv->vmCvar->string ) );
				}

				if ( cv->teamShader ) {
					remapped = qtrue;
				}

#ifdef WITH_LUA
				if ( cv->vmCvar == &lua_modules ||
					cv->vmCvar == &lua_allowedModules ) {
						G_LuaShutdown();
				}
#endif
			}
		}
	}

	if ( remapped ) {
		G_RemapTeamShaders();
	}
}

/*
==============
G_SpawnScriptCamera
create the game entity that's used for camera<->script communication and portal location for camera view
==============
*/
void G_SpawnScriptCamera( void ) {
	if ( g_camEnt ) {
		G_FreeEntity( g_camEnt );
	}

	g_camEnt = G_Spawn();

	g_camEnt->scriptName = "scriptcamera";

	g_camEnt->s.eType = ET_CAMERA;
	g_camEnt->s.apos.trType = TR_STATIONARY;
	g_camEnt->s.apos.trTime = 0;
	g_camEnt->s.apos.trDuration = 0;
	VectorCopy( g_camEnt->s.angles, g_camEnt->s.apos.trBase );
	VectorClear( g_camEnt->s.apos.trDelta );

	g_camEnt->s.frame = 0;

	g_camEnt->r.svFlags |= SVF_NOCLIENT;        // only broadcast when in use

	if ( g_camEnt->s.number >= MAX_CLIENTS && g_camEnt->scriptName ) {
		G_Script_ScriptParse( g_camEnt );
		G_Script_ScriptEvent( g_camEnt, "spawn", "" );
	}
}

/*
============
G_InitGame

============
*/
void G_InitGame( int levelTime, int randomSeed, int restart ) {
	int i;
	char cs[MAX_INFO_STRING];
	unsigned int dwBlueOffset, dwRedOffset;
	unsigned int aRandomValues[MAX_REINFSEEDS];
	char strReinfSeeds[MAX_STRING_CHARS];

	G_Printf( "------- Game Initialization -------\n" );
	G_Printf( "gamename: %s\n", GAMEVERSION );
	G_Printf( "gamedate: %s\n", __DATE__ );

	srand( randomSeed );

	G_RegisterCvars();

	ReadAwards();

	// Xian enforcemaxlives stuff
	/*
	we need to clear the list even if enforce maxlives is not active
	in cas ethe g_maxlives was changed, and a map_restart happened
	*/
	ClearMaxLivesGUID();

	// just for verbosity
	if ( g_enforcemaxlives.integer && ( g_maxlives.integer > 0 || g_axismaxlives.integer > 0 || g_alliedmaxlives.integer > 0 ) ) {
		G_Printf( "EnforceMaxLives-Cleared GUID List\n" );
	}

	G_ProcessIPBans();

	G_InitMemory();

	// NERVE - SMF - intialize gamestate
	if ( g_gamestate.integer == GS_INITIALIZE ) {
		if ( g_noTeamSwitching.integer ) {
			trap_Cvar_Set( "gamestate", va( "%i", GS_WAITING_FOR_PLAYERS ) );
		} else {
			trap_Cvar_Set( "gamestate", va( "%i", GS_WARMUP ) );
		}
	}

	// set some level globals
	memset( &level, 0, sizeof( level ) );
	level.time = levelTime;
	level.startTime = levelTime;

	level.snd_fry = G_SoundIndex( "sound/player/fry.wav" );   // FIXME standing in lava / slime
	level.bulletRicochetSound = G_SoundIndex( "bulletRicochet" );
	level.snipersound = G_SoundIndex( "sound/weapons/mauser/mauserf1.wav" );
	level.knifeSound[0] = G_SoundIndex( "sound/weapons/knife/knife_hitwall1.wav" );

	// RF, init the anim scripting
	level.animScriptData.soundIndex = G_SoundIndex;
	level.animScriptData.playSound = G_AnimScriptSound;

	if ( g_log.string[0] ) {
		if ( g_logSync.integer ) {
			trap_FS_FOpenFile( g_log.string, &level.logFile, FS_APPEND_SYNC );
		} else {
			trap_FS_FOpenFile( g_log.string, &level.logFile, FS_APPEND );
		}
		if ( !level.logFile ) {
			G_Printf( "WARNING: Couldn't open logfile: %s\n", g_log.string );
		} else {
			char serverinfo[MAX_INFO_STRING];

			trap_GetServerinfo( serverinfo, sizeof( serverinfo ) );
			Q_strncpyz( level.rawmapname, Info_ValueForKey( serverinfo, "mapname" ), sizeof( level.rawmapname ) );

			G_LogPrintf( "------------------------------------------------------------\n" );
			G_LogPrintf( "InitGame: %s\n", serverinfo );
		}
	} else {
		G_Printf( "Not logging to disk.\n" );
	}

	if(g_mapConfigs.string[0]) {
		char mapConfig[MAX_STRING_CHARS];

		Q_strncpyz(mapConfig, "exec ", sizeof(mapConfig));
		Q_strcat(mapConfig, sizeof(mapConfig), g_mapConfigs.string);
		Q_strcat(mapConfig, sizeof(mapConfig), "/default.cfg\n");
		trap_SendConsoleCommand(EXEC_APPEND, mapConfig);

		Q_strncpyz(mapConfig, "exec ", sizeof(mapConfig));
		Q_strcat(mapConfig, sizeof(mapConfig), g_mapConfigs.string);
		Q_strcat(mapConfig, sizeof(mapConfig), "/");
		Q_strcat(mapConfig, sizeof(mapConfig), level.rawmapname);
		Q_strcat(mapConfig, sizeof(mapConfig), ".cfg\n");
		trap_SendConsoleCommand(EXEC_APPEND, mapConfig);
	}

	G_InitWorldSession();

	// DHM - Nerve :: Clear out spawn target config strings
	if ( g_gametype.integer >= GT_WOLF ) {
		trap_GetConfigstring( CS_MULTI_INFO, cs, sizeof( cs ) );
		Info_SetValueForKey( cs, "numspawntargets", "0" );
		trap_SetConfigstring( CS_MULTI_INFO, cs );

		for ( i = CS_MULTI_SPAWNTARGETS; i < CS_MULTI_SPAWNTARGETS + MAX_MULTI_SPAWNTARGETS; i++ ) {
			trap_SetConfigstring( i, "" );
		}
	}

	// initialize all entities for this game
	memset( g_entities, 0, MAX_GENTITIES * sizeof( g_entities[0] ) );
	level.gentities = g_entities;

	// initialize all clients for this game
	level.maxclients = g_maxclients.integer;
	memset( g_clients, 0, MAX_CLIENTS * sizeof( g_clients[0] ) );
	level.clients = g_clients;

	// set client fields on player ents
	for ( i = 0 ; i < level.maxclients ; i++ ) {
		g_entities[i].client = level.clients + i;
	}

	// always leave room for the max number of clients,
	// even if they aren't all used, so numbers inside that
	// range are NEVER anything but clients
	level.num_entities = MAX_CLIENTS;

	// let the server system know where the entites are
	trap_LocateGameData( level.gentities, level.num_entities, sizeof( gentity_t ),
		&level.clients[0].ps, sizeof( level.clients[0] ) );

	// load level script
	G_Script_ScriptLoad();

	// reserve some spots for dead player bodies
	InitBodyQue();

	ClearRegisteredItems();

	// parse the key/value pairs and spawn gentities
	G_SpawnEntitiesFromString();

	// create the camera entity that will communicate with the scripts
	G_SpawnScriptCamera();

	// general initialization
	G_FindTeams();

	// make sure we have flags for CTF, etc
	if ( g_gametype.integer >= GT_TEAM ) {
		G_CheckTeamItems();
	}

	SaveRegisteredItems();

	G_Printf( "-----------------------------------\n" );

	// Set up the random reinforcement seeds for both teams and send to clients
	dwBlueOffset = rand() % MAX_REINFSEEDS;
	dwRedOffset = rand() % MAX_REINFSEEDS;
	strcpy( strReinfSeeds, va( "%d %d", ( dwBlueOffset << REINF_BLUEDELT ) + ( rand() % ( 1 << REINF_BLUEDELT ) ),
		( dwRedOffset << REINF_REDDELT )  + ( rand() % ( 1 << REINF_REDDELT ) ) ) );

	for ( i = 0; i < MAX_REINFSEEDS; i++ ) {
		aRandomValues[i] = ( rand() % REINF_RANGE ) * aReinfSeeds[i];
		strcat( strReinfSeeds, va( " %d", aRandomValues[i] ) );
	}

	level.dwBlueReinfOffset = 1000 * aRandomValues[dwBlueOffset] / aReinfSeeds[dwBlueOffset];
	level.dwRedReinfOffset  = 1000 * aRandomValues[dwRedOffset] / aReinfSeeds[dwRedOffset];
	trap_SetConfigstring( CS_REINFSEEDS, strReinfSeeds );

	CheckTotalMsgs();
	G_RemapTeamShaders();

#ifdef WITH_LUA
	G_LuaInit();
	G_LuaHook_InitGame( levelTime, randomSeed, restart );
#endif
}

/*
=================
G_ShutdownGame
=================
*/
void G_ShutdownGame( int restart ) {
#ifdef WITH_LUA
	G_LuaHook_ShutdownGame( restart );
	G_LuaShutdown();
#endif

	G_Printf( "==== ShutdownGame ====\n" );

	if ( level.logFile ) {
		G_LogPrintf( "ShutdownGame:\n" );
		G_LogPrintf( "------------------------------------------------------------\n" );
		trap_FS_FCloseFile( level.logFile );
		level.logFile = 0;
	}

	// write all the client session data so we can get it back
	G_WriteSessionData();
}

//===================================================================

#ifndef GAME_HARD_LINKED
// this is only here so the functions in q_shared.c and bg_*.c can link

void QDECL Com_Error( int errorLevel, const char *error, ... ) {
	va_list argptr;
	char text[1024];

	va_start( argptr, error );
	Q_vsnprintf( text, sizeof( text ), error, argptr );
	va_end( argptr );

	G_Error( "%s", text );
}

void QDECL Com_Printf( const char *msg, ... ) {
	va_list argptr;
	char text[1024];

	va_start( argptr, msg );
	Q_vsnprintf( text, sizeof( text ), msg, argptr );
	va_end( argptr );

	G_Printf( "%s", text );
}

#endif

/*
========================================================================

PLAYER COUNTING / SCORE SORTING

========================================================================
*/

/*
=============
SortRanks

=============
*/
int QDECL SortRanks( const void *a, const void *b ) {
	gclient_t   *ca, *cb;

	ca = &level.clients[*(int *)a];
	cb = &level.clients[*(int *)b];

	// sort special clients last
	if ( ca->sess.spectatorState == SPECTATOR_SCOREBOARD || ca->sess.spectatorClient < 0 ) {
		return 1;
	}
	if ( cb->sess.spectatorState == SPECTATOR_SCOREBOARD || cb->sess.spectatorClient < 0  ) {
		return -1;
	}

	// then connecting clients
	if ( ca->pers.connected == CON_CONNECTING ) {
		return 1;
	}
	if ( cb->pers.connected == CON_CONNECTING ) {
		return -1;
	}

	// then spectators
	if ( ca->sess.sessionTeam == TEAM_SPECTATOR && cb->sess.sessionTeam == TEAM_SPECTATOR ) {
		if ( ca->sess.spectatorTime < cb->sess.spectatorTime ) {
			return -1;
		}
		if ( ca->sess.spectatorTime > cb->sess.spectatorTime ) {
			return 1;
		}
		return 0;
	}
	if ( ca->sess.sessionTeam == TEAM_SPECTATOR ) {
		return 1;
	}
	if ( cb->sess.sessionTeam == TEAM_SPECTATOR ) {
		return -1;
	}

	// then sort by score
	if ( ca->ps.persistant[PERS_SCORE]
	> cb->ps.persistant[PERS_SCORE] ) {
		return -1;
	}
	if ( ca->ps.persistant[PERS_SCORE]
	< cb->ps.persistant[PERS_SCORE] ) {
		return 1;
	}
	return 0;
}

/*
============
CalculateRanks

Recalculates the score ranks of all players
This will be called on every client connect, begin, disconnect, death,
and team change.
============
*/
void CalculateRanks( void ) {
	int i;
	gclient_t   *cl;

	level.follow1 = -1;
	level.follow2 = -1;
	level.numConnectedClients = 0;
	level.numNonSpectatorClients = 0;
	level.numPlayingClients = 0;
	level.numVotingClients = 0;     // don't count bots

	level.numFinalDead[0] = 0;      // NERVE - SMF
	level.numFinalDead[1] = 0;      // NERVE - SMF

	for ( i = 0; i < 2; i++ ) {
		level.numteamVotingClients[i] = 0;
	}
	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[i].pers.connected != CON_DISCONNECTED ) {
			level.sortedClients[level.numConnectedClients] = i;
			level.numConnectedClients++;

			if ( level.clients[i].sess.sessionTeam != TEAM_SPECTATOR ) {
				level.numNonSpectatorClients++;

				// decide if this should be auto-followed
				if ( level.clients[i].pers.connected == CON_CONNECTED ) {
					level.numPlayingClients++;
					if ( !( g_entities[i].r.svFlags & SVF_BOT ) ) {
						level.numVotingClients++;
					}

					if ( level.clients[i].sess.sessionTeam == TEAM_RED ) {
						// NERVE - SMF
						if ( level.clients[i].ps.persistant[PERS_RESPAWNS_LEFT] == 0
							&& g_entities[i].health <= 0 ) {
								level.numFinalDead[0]++;
						}

						level.numteamVotingClients[0]++;
					} else if ( level.clients[i].sess.sessionTeam == TEAM_BLUE )   {
						// NERVE - SMF
						if ( level.clients[i].ps.persistant[PERS_RESPAWNS_LEFT] == 0
							&& g_entities[i].health <= 0 ) {
								level.numFinalDead[1]++;
						}

						level.numteamVotingClients[1]++;
					}

					if ( level.follow1 == -1 ) {
						level.follow1 = i;
					} else if ( level.follow2 == -1 ) {
						level.follow2 = i;
					}
				}
			}
		}
	}

	qsort( level.sortedClients, level.numConnectedClients,
		sizeof( level.sortedClients[0] ), SortRanks );

	// set the rank value for all clients that are connected and not spectators
	if ( g_gametype.integer >= GT_TEAM ) {
		// in team games, rank is just the order of the teams, 0=red, 1=blue, 2=tied
		for ( i = 0; i < level.numConnectedClients; i++ ) {
			cl = &level.clients[ level.sortedClients[i] ];
			if ( level.teamScores[TEAM_RED] == level.teamScores[TEAM_BLUE] ) {
				cl->ps.persistant[PERS_RANK] = 2;
			} else if ( level.teamScores[TEAM_RED] > level.teamScores[TEAM_BLUE] ) {
				cl->ps.persistant[PERS_RANK] = 0;
			} else {
				cl->ps.persistant[PERS_RANK] = 1;
			}
		}
	} else {
		int newScore, rank = -1,score = 0;

		for ( i = 0; i < level.numPlayingClients; i++ ) {
			cl = &level.clients[ level.sortedClients[i] ];
			newScore = cl->ps.persistant[PERS_SCORE];
			if ( i == 0 || newScore != score ) {
				rank = i;
				// assume we aren't tied until the next client is checked
				level.clients[ level.sortedClients[i] ].ps.persistant[PERS_RANK] = rank;
			} else {
				// we are tied with the previous client
				level.clients[ level.sortedClients[i - 1] ].ps.persistant[PERS_RANK] = rank | RANK_TIED_FLAG;
				level.clients[ level.sortedClients[i] ].ps.persistant[PERS_RANK] = rank | RANK_TIED_FLAG;
			}
			score = newScore;
		}
	}

	// set the CS_SCORES1/2 configstrings, which will be visible to everyone
	if ( g_gametype.integer >= GT_TEAM ) {
		trap_SetConfigstring( CS_SCORES1, va( "%i", level.teamScores[TEAM_RED] ) );
		trap_SetConfigstring( CS_SCORES2, va( "%i", level.teamScores[TEAM_BLUE] ) );
	} else {
		if ( level.numConnectedClients == 0 ) {
			trap_SetConfigstring( CS_SCORES1, va( "%i", SCORE_NOT_PRESENT ) );
			trap_SetConfigstring( CS_SCORES2, va( "%i", SCORE_NOT_PRESENT ) );
		} else if ( level.numConnectedClients == 1 ) {
			trap_SetConfigstring( CS_SCORES1, va( "%i", level.clients[ level.sortedClients[0] ].ps.persistant[PERS_SCORE] ) );
			trap_SetConfigstring( CS_SCORES2, va( "%i", SCORE_NOT_PRESENT ) );
		} else {
			trap_SetConfigstring( CS_SCORES1, va( "%i", level.clients[ level.sortedClients[0] ].ps.persistant[PERS_SCORE] ) );
			trap_SetConfigstring( CS_SCORES2, va( "%i", level.clients[ level.sortedClients[1] ].ps.persistant[PERS_SCORE] ) );
		}
	}

	// see if it is time to end the level
	CheckExitRules();

	// if we are at the intermission, send the new info to everyone
	if ( level.intermissiontime ) {
		SendScoreboardMessageToAllClients();
	}
}

/*
========================================================================

MAP CHANGING

========================================================================
*/

/*
========================
SendScoreboardMessageToAllClients

Do this at BeginIntermission time and whenever ranks are recalculated
due to enters/exits/forced team changes
========================
*/
void SendScoreboardMessageToAllClients( void ) {
	int i;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[ i ].pers.connected == CON_CONNECTED ) {
			DeathmatchScoreboardMessage( g_entities + i );
		}
	}
}

/*
========================
MoveClientToIntermission

When the intermission starts, this will be called for all players.
If a new client connects, this will be called after the spawn function.
========================
*/
void MoveClientToIntermission( gentity_t *ent ) {
	// take out of follow mode if needed
	if ( ent->client->sess.spectatorState == SPECTATOR_FOLLOW ) {
		StopFollowing( ent );
	}

	// move to the spot
	VectorCopy( level.intermission_origin, ent->s.origin );
	VectorCopy( level.intermission_origin, ent->client->ps.origin );
	VectorCopy( level.intermission_angle, ent->client->ps.viewangles );
	ent->client->ps.pm_type = PM_INTERMISSION;

	// clean up powerup info
	// memset( ent->client->ps.powerups, 0, sizeof(ent->client->ps.powerups) );

	ent->client->ps.eFlags = 0;
	ent->s.eFlags = 0;
	ent->s.eType = ET_GENERAL;
	ent->s.modelindex = 0;
	ent->s.loopSound = 0;
	ent->s.event = 0;
	ent->s.events[0] = ent->s.events[1] = ent->s.events[2] = ent->s.events[3] = 0;      // DHM - Nerve
	ent->r.contents = 0;
}

/*
==================
FindIntermissionPoint

This is also used for spectator spawns
==================
*/
void FindIntermissionPoint( void ) {
	gentity_t   *ent, *target;
	vec3_t dir;
	char cs[MAX_STRING_CHARS];              // DHM - Nerve
	char        *buf;                       // DHM - Nerve
	int winner;                             // DHM - Nerve

	if ( g_gametype.integer >= GT_WOLF ) {
		ent = NULL;

		// NERVE - SMF - if the match hasn't ended yet, and we're just a spectator
		if ( !level.intermissiontime ) {
			// try to find the intermission spawnpoint with no team flags set
			ent = G_Find( NULL, FOFS( classname ), "info_player_intermission" );

			for ( ; ent; ent = G_Find( ent, FOFS( classname ), "info_player_intermission" ) ) {
				if ( !ent->spawnflags ) {
					break;
				}
			}
		}

		trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof( cs ) );
		buf = Info_ValueForKey( cs, "winner" );
		winner = atoi( buf );

		// Change from scripting value for winner (0==AXIS, 1==ALLIES) to spawnflag value
		if ( winner == 0 ) {
			winner = 1;
		} else {
			winner = 2;
		}

		if ( !ent ) {
			ent = G_Find( NULL, FOFS( classname ), "info_player_intermission" );

			if ( ent && !( ent->spawnflags & winner ) ) {
				ent = G_Find( ent, FOFS( classname ), "info_player_intermission" );
			}
		}
	} else {
		// find the intermission spot
		ent = G_Find( NULL, FOFS( classname ), "info_player_intermission" );
	}

	if ( !ent ) {   // the map creator forgot to put in an intermission point...
		SelectSpawnPoint( vec3_origin, level.intermission_origin, level.intermission_angle );
	} else {
		VectorCopy( ent->s.origin, level.intermission_origin );
		VectorCopy( ent->s.angles, level.intermission_angle );
		// if it has a target, look towards it
		if ( ent->target ) {
			target = G_PickTarget( ent->target );
			if ( target ) {
				VectorSubtract( target->s.origin, level.intermission_origin, dir );
				vectoangles( dir, level.intermission_angle );
			}
		}
	}
}

/*
==================
BeginIntermission
==================
*/
void BeginIntermission( void ) {
	int i;
	gentity_t   *client;

	if ( level.intermissiontime ) {
		return;     // already active
	}

	level.intermissiontime = level.time;
	FindIntermissionPoint();

	// move all clients to the intermission point
	for ( i = 0 ; i < level.maxclients ; i++ ) {
		client = g_entities + i;
		if ( !client->inuse ) {
			continue;
		}
		// respawn if dead
		if ( g_gametype.integer < GT_WOLF && client->health <= 0 ) {
			respawn( client );
		}
		MoveClientToIntermission( client );
	}

	// send the current scoring to all clients
	SendScoreboardMessageToAllClients();
}

/*
=============
ExitLevel

When the intermission has been exited, the server is either killed
or moved to a new level based on the "nextmap" cvar

=============
*/
void ExitLevel( void ) {
	int i;
	gclient_t *cl;

	trap_SendConsoleCommand( EXEC_APPEND, "vstr nextmap\n" );
	level.changemap = NULL;
	level.intermissiontime = 0;

	// reset all the scores so we don't enter the intermission again
	// cs: also reset the spawn point selection.
	level.teamScores[TEAM_RED] = 0;
	level.teamScores[TEAM_BLUE] = 0;
	for ( i = 0 ; i < g_maxclients.integer ; i++ ) {
		cl = level.clients + i;
		if ( cl->pers.connected != CON_CONNECTED ) {
			continue;
		}
		cl->sess.spawnObjectiveIndex = 0;
		cl->ps.persistant[PERS_SCORE] = 0;
	}

	// we need to do this here before chaning to CON_CONNECTING
	G_WriteSessionData();

	// change all client states to connecting, so the early players into the
	// next level will know the others aren't done reconnecting
	for ( i = 0 ; i < g_maxclients.integer ; i++ ) {
		if ( level.clients[i].pers.connected == CON_CONNECTED ) {
			level.clients[i].pers.connected = CON_CONNECTING;
		}
	}

	G_LogPrintf( "ExitLevel: executed\n" );
}

/*
=================
G_LogPrintf

Print to the logfile with a time stamp if it is open
=================
*/
void QDECL G_LogPrintf( const char *fmt, ... ) {
	va_list argptr;
	char string[1024];
	int min, tens, sec;

	sec = level.time / 1000;

	min = sec / 60;
	sec -= min * 60;
	tens = sec / 10;
	sec -= tens * 10;

	Com_sprintf( string, sizeof( string ), "%3i:%i%i ", min, tens, sec );

	va_start( argptr, fmt );
	Q_vsnprintf( string + 7, sizeof( string ) - 7, fmt, argptr );
	va_end( argptr );

	if ( g_dedicated.integer ) {
		G_Printf( "%s", string + 7 );
	}

	if ( !level.logFile ) {
		return;
	}

	trap_FS_Write( string, strlen( string ), level.logFile );
}

//CS: print players stats to console at the end of the round
char *aTeams[TEAM_NUM_TEAMS] = { "FFA", "^1Axis^7", "^4Allies^7", "Spectators" };
void RoundStats( void ) {
	int i, j, cnt, eff;
	int tot_kills, tot_deaths, tot_gibs, tot_sui, tot_tk, tot_dg, tot_dr, tot_td, tot_gp;
	gclient_t *cl;
	char *ref;
	char n2[MAX_STRING_CHARS];

	cnt = 0;
	for ( i = TEAM_RED; i <= TEAM_BLUE; i++ ) {
		if ( !TeamCount( -1, i ) ) {
			continue;
		}

		tot_kills = 0;
		tot_deaths = 0;
		tot_sui = 0;
		tot_tk = 0;
		tot_dg = 0;
		tot_dr = 0;
		tot_td = 0;
		tot_gibs = 0;
		tot_gp = 0;

		trap_SendServerCommand( -1, "print \"\n^7TEAM   Player          Kll Dth Sui TK Eff  ^3GB^7    ^2DG    ^1DR   ^6TD  ^3Score\n"
			"^7---------------------------------------------------------------------\n\"" );

		for ( j = 0; j < level.numPlayingClients; j++ ) {
			cl = level.clients + level.sortedClients[j];

			if ( cl->pers.connected != CON_CONNECTED || cl->sess.sessionTeam != i ) {
				continue;
			}

			// see if this one qualifies for any awards
			SetAwards( cl );

			CleanseString( cl->pers.netname, n2, qfalse );
			n2[15] = 0;

			ref = "^7";

			tot_kills += cl->pers.kills;
			tot_deaths += cl->pers.deaths;
			tot_sui += cl->pers.suicides;
			tot_tk += cl->pers.teamKills;
			tot_dg += cl->pers.damage_given;
			tot_dr += cl->pers.damage_received;
			tot_td += cl->pers.team_damage;
			tot_gibs += cl->pers.gibs;
			tot_gp += cl->ps.persistant[PERS_SCORE];

			eff = ( cl->pers.deaths + cl->pers.kills == 0 ) ? 0 : 100 * cl->pers.kills / ( cl->pers.deaths + cl->pers.kills );
			if ( eff < 0 ) {
				eff = 0;
			}

			//TODO: remove this
			ref = "^7";

			cnt++;
			trap_SendServerCommand( -1, va( "print \"%-10s %s%-15s^3%4d%4d%4d%3d%s%4d^3%4d^2%6d^1%6d^6%5d^3%7d\n\"",
				aTeams[i],
				ref,
				n2,
				cl->pers.kills,
				cl->pers.deaths,
				cl->pers.suicides,
				cl->pers.teamKills,
				ref,
				eff,
				cl->pers.gibs,
				cl->pers.damage_given,
				cl->pers.damage_received,
				cl->pers.team_damage,
				cl->ps.persistant[PERS_SCORE] ) );
		}

		eff = ( tot_kills + tot_deaths == 0 ) ? 0 : 100 * tot_kills / ( tot_kills + tot_deaths );
		if ( eff < 0 ) {
			eff = 0;
		}

		trap_SendServerCommand( -1, va( "print \"^7---------------------------------------------------------------------\n"
			"%-10s ^5%-15s%4d%4d%4d%3d^5%4d^3%4d^2%6d^1%6d^6%5d^3%7d\n\"",
			aTeams[i],
			"Totals",
			tot_kills,
			tot_deaths,
			tot_sui,
			tot_tk,
			eff,
			tot_gibs,
			tot_dg,
			tot_dr,
			tot_td,
			tot_gp ) );
	}

	trap_SendServerCommand( -1, va( "print \"%s\n\n\" 0", ( ( !cnt ) ? "^3\nNo scores to report." : "" ) ) );
}

/*
================
LogExit

Append information about this game to the log file
================
*/
void LogExit( const char *string ) {
	int i, winner;
	gclient_t       *cl;
	char cs[MAX_STRING_CHARS];

	// NERVE - SMF - do not allow LogExit to be called in non-playing gamestate
	if ( g_gamestate.integer != GS_PLAYING ) {
		return;
	}

#ifdef WITH_LUA
	G_LuaHook_LogExit();
#endif

	G_LogPrintf( "Exit: %s\n", string );

	level.intermissionQueued = level.time;

	// this will keep the clients from playing any voice sounds
	// that will get cut off when the queued intermission starts
	trap_SetConfigstring( CS_INTERMISSION, "1" );

	if ( g_gametype.integer >= GT_TEAM ) {
		G_LogPrintf( "red:%i  blue:%i\n",
			level.teamScores[TEAM_RED], level.teamScores[TEAM_BLUE] );
	}

	// NERVE - SMF - send gameCompleteStatus message to master servers
	trap_SendConsoleCommand( EXEC_APPEND, "gameCompleteStatus\n" );

	for ( i = 0 ; i < level.numConnectedClients ; i++ ) {
		int ping;

		cl = &level.clients[level.sortedClients[i]];

		if ( cl->sess.sessionTeam == TEAM_SPECTATOR ) {
			continue;
		}
		if ( cl->pers.connected == CON_CONNECTING ) {
			continue;
		}

		BG_SetKillSpree( &cl->ps, 0 );

		// cs: default behavior of sending 32 scores, but we want to clear spree for all.
		if ( i > 31 ) { continue; }

		ping = cl->ps.ping < 999 ? cl->ps.ping : 999;

		G_LogPrintf( "score: %i  ping: %i  client: %i %s\n",
			cl->ps.persistant[PERS_SCORE], ping, level.sortedClients[i],
			cl->pers.netname );
	}

	trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof( cs ) );
	winner = atoi( Info_ValueForKey( cs, "winner" ) );

	// NERVE - SMF
	if ( g_gametype.integer == GT_WOLF_STOPWATCH ) {
		int defender;

		trap_GetConfigstring( CS_MULTI_INFO, cs, sizeof( cs ) );
		defender = atoi( Info_ValueForKey( cs, "defender" ) );

		// NERVE - SMF
		if ( !g_currentRound.integer ) {
			if ( winner == defender ) {
				// if the defenders won, use default timelimit
				trap_Cvar_Set( "g_nextTimeLimit", va( "%f", g_timelimit.value ) );
			} else {
				// use remaining time as next timer
				trap_Cvar_Set( "g_nextTimeLimit", va( "%f", ( level.time - level.startTime ) / 60000.f ) );
			}
		} else {
			// reset timer
			trap_Cvar_Set( "g_nextTimeLimit", "0" );
		}

		trap_Cvar_Set( "g_currentRound", va( "%i", !g_currentRound.integer ) );
	}
	// -NERVE - SMF
	winner += 1; // to match ob team constants
	Bot_Util_SendTrigger( NULL, NULL, "Round End.", va("%i", winner) );
	RoundStats();
	ExtendWarmup( qtrue );
	WriteAwards();
}

/*
=================
CheckIntermissionExit

The level will stay at the intermission for a minimum of 5 seconds
If all players wish to continue, the level will then exit.
If one or more players have not acknowledged the continue, the game will
wait 10 seconds before going on.
=================
*/
void CheckIntermissionExit( void ) {
	int ready, notReady;
	int i;
	gclient_t   *cl;
	int readyMask;

	// DHM - Nerve :: Flat 10 second timer until exit
	if ( g_gametype.integer >= GT_WOLF ) {
		if ( level.time < level.intermissiontime + 10000 ) {
			return;
		}

		ExitLevel();
		return;
	}
	// dhm - end

	// see which players are ready
	ready = 0;
	notReady = 0;
	readyMask = 0;
	for ( i = 0 ; i < g_maxclients.integer ; i++ ) {
		cl = level.clients + i;
		if ( cl->pers.connected != CON_CONNECTED ) {
			continue;
		}
		if ( g_entities[cl->ps.clientNum].r.svFlags & SVF_BOT ) {
			continue;
		}

		if ( cl->readyToExit ) {
			ready++;
			if ( i < 16 ) {
				readyMask |= 1 << i;
			}
		} else {
			notReady++;
		}
	}

	// copy the readyMask to each player's stats so
	// it can be displayed on the scoreboard
	for ( i = 0 ; i < g_maxclients.integer ; i++ ) {
		cl = level.clients + i;
		if ( cl->pers.connected != CON_CONNECTED ) {
			continue;
		}
		cl->ps.stats[STAT_CLIENTS_READY] = readyMask;
	}

	// never exit in less than five seconds
	if ( level.time < level.intermissiontime + 5000 ) {
		return;
	}

	// if nobody wants to go, clear timer
	if ( !ready ) {
		level.readyToExit = qfalse;
		return;
	}

	// if everyone wants to go, go now
	if ( !notReady ) {
		ExitLevel();
		return;
	}

	// the first person to ready starts the ten second timeout
	if ( !level.readyToExit ) {
		level.readyToExit = qtrue;
		level.exitTime = level.time;
	}

	// if we have waited ten seconds since at least one player
	// wanted to exit, go ahead
	if ( level.time < level.exitTime + 10000 ) {
		return;
	}

	ExitLevel();
}

/*
=============
ScoreIsTied
=============
*/
qboolean ScoreIsTied( void ) {
	int a, b;
	char cs[MAX_STRING_CHARS];
	char    *buf;

	// DHM - Nerve :: GT_WOLF checks the current value of
	if ( g_gametype.integer >= GT_WOLF ) {
		trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof( cs ) );

		buf = Info_ValueForKey( cs, "winner" );
		a = atoi( buf );

		return a == -1;
	}

	if ( level.numPlayingClients < 2 ) {
		return qfalse;
	}

	if ( g_gametype.integer >= GT_TEAM ) {
		return level.teamScores[TEAM_RED] == level.teamScores[TEAM_BLUE];
	}

	a = level.clients[level.sortedClients[0]].ps.persistant[PERS_SCORE];
	b = level.clients[level.sortedClients[1]].ps.persistant[PERS_SCORE];

	return a == b;
}

qboolean G_ScriptAction_SetWinner( gentity_t *ent, char *params );

/*
=================
CheckExitRules

There will be a delay between the time the exit is qualified for
and the time everyone is moved to the intermission spot, so you
can see the last frag.
=================
*/
void CheckExitRules( void ) {
	gclient_t   *cl;
	gentity_t   *gm;
	char cs[MAX_STRING_CHARS];
	char txt[5];

	// if at the intermission, wait for all non-bots to
	// signal ready, then go to next level
	if ( level.intermissiontime ) {
		CheckIntermissionExit();
		return;
	}

	if ( level.intermissionQueued ) {
		if ( level.time - level.intermissionQueued >= INTERMISSION_DELAY_TIME || g_gametype.integer >= GT_WOLF ) {
			level.intermissionQueued = 0;
			BeginIntermission();
		}
		return;
	}

	if ( g_timelimit.value && !level.warmupTime ) {
		if ( level.time - level.startTime >= g_timelimit.value * 60000 ) {
			// check for sudden death
			if ( g_gametype.integer != GT_CTF && ScoreIsTied() ) {
				// score is tied, so don't end the game
				return;
			}

			if ( g_gametype.integer >= GT_WOLF ) {
				gm = G_Find( NULL, FOFS( scriptName ), "game_manager" );
				// JPW NERVE -- in CPH, check final capture/hold times and adjust winner
				// 0 == axis, 1 == allied
				if ( g_gametype.integer == GT_WOLF_CPH ) {
					int num = -1;
					if ( level.capturetimes[TEAM_RED] > level.capturetimes[TEAM_BLUE] ) {
						num = 0;
					}
					if ( level.capturetimes[TEAM_RED] < level.capturetimes[TEAM_BLUE] ) {
						num = 1;
					}
					if ( num != -1 ) {
						sprintf( txt,"%d",num );
						G_ScriptAction_SetWinner( NULL, txt );
					}
				}
				// jpw
				if ( gm ) {
					G_Script_ScriptEvent( gm, "trigger", "timelimit_hit" );
				}
			}

			// NERVE - SMF - do not allow LogExit to be called in non-playing gamestate
			// - This already happens in LogExit, but we need it for the print command
			if ( g_gamestate.integer != GS_PLAYING ) {
				return;
			}

			trap_SendServerCommand( -1, "print \"Timelimit hit.\n\"" );
			LogExit( "Timelimit hit." );

			return;
		}
	}

	if ( level.numPlayingClients < 2 ) {
		return;
	}

	if ( g_gametype.integer >= GT_WOLF && ( g_maxlives.integer > 0 || g_axismaxlives.integer > 0 || g_alliedmaxlives.integer > 0 ) ) {
		if ( level.numFinalDead[0] >= level.numteamVotingClients[0] && level.numteamVotingClients[0] > 0 ) {
			trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof( cs ) );
			Info_SetValueForKey( cs, "winner", "1" );
			trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );
			LogExit( "Axis team eliminated." );
		} else if ( level.numFinalDead[1] >= level.numteamVotingClients[1] && level.numteamVotingClients[1] > 0 )   {
			trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof( cs ) );
			Info_SetValueForKey( cs, "winner", "0" );
			trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );
			LogExit( "Allied team eliminated." );
		}
	}

	if ( ( ( g_gametype.integer != GT_CTF && g_gametype.integer < GT_WOLF ) || g_deathmatch.integer ) && g_fraglimit.integer ) {
		int i;

		if ( level.teamScores[TEAM_RED] >= g_fraglimit.integer ) {
			trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof( cs ) );
			Info_SetValueForKey( cs, "winner", "0" );
			trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );
			trap_SendServerCommand( -1, "print \"Red hit the fraglimit.\n\"" );
			LogExit( "Fraglimit hit." );
			return;
		}

		if ( level.teamScores[TEAM_BLUE] >= g_fraglimit.integer ) {
			trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof( cs ) );
			Info_SetValueForKey( cs, "winner", "1" );
			trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );
			trap_SendServerCommand( -1, "print \"Blue hit the fraglimit.\n\"" );
			LogExit( "Fraglimit hit." );
			return;
		}

		for ( i = 0 ; i < g_maxclients.integer ; i++ ) {
			cl = level.clients + i;
			if ( cl->pers.connected != CON_CONNECTED ) {
				continue;
			}
			if ( cl->sess.sessionTeam != TEAM_FREE ) {
				continue;
			}

			if ( cl->ps.persistant[PERS_SCORE] >= g_fraglimit.integer ) {
				LogExit( "Fraglimit hit." );
				trap_SendServerCommand( -1, va( "print \"%s" S_COLOR_WHITE " hit the fraglimit.\n\"",
					cl->pers.netname ) );
				return;
			}
		}
	}

	if ( g_gametype.integer == GT_CTF && g_capturelimit.integer ) {
		if ( level.teamScores[TEAM_RED] >= g_capturelimit.integer ) {
			trap_SendServerCommand( -1, "print \"Red hit the capturelimit.\n\"" );
			LogExit( "Capturelimit hit." );
			return;
		}

		if ( level.teamScores[TEAM_BLUE] >= g_capturelimit.integer ) {
			trap_SendServerCommand( -1, "print \"Blue hit the capturelimit.\n\"" );
			LogExit( "Capturelimit hit." );
			return;
		}
	}
}

/*
========================================================================

FUNCTIONS CALLED EVERY FRAME

========================================================================
*/

/*
=============
CheckWolfMP

NERVE - SMF
=============
*/
void CheckGameState() {
	gamestate_t current_gs;

	current_gs = trap_Cvar_VariableIntegerValue( "gamestate" );

	if ( level.intermissiontime && current_gs != GS_INTERMISSION ) {
		trap_Cvar_Set( "gamestate", va( "%i", GS_INTERMISSION ) );
		return;
	}

	if ( g_noTeamSwitching.integer && !trap_Cvar_VariableIntegerValue( "sv_serverRestarting" ) ) {
		if ( current_gs != GS_WAITING_FOR_PLAYERS && level.numPlayingClients <= 1 && level.lastRestartTime + 1000 < level.time ) {
			level.lastRestartTime = level.time;
			trap_SendConsoleCommand( EXEC_APPEND, va( "map_restart 0 %i\n", GS_WAITING_FOR_PLAYERS ) );
		}
	}

	if ( current_gs == GS_WAITING_FOR_PLAYERS && g_minGameClients.integer > 1 &&
		level.numPlayingClients >= g_minGameClients.integer && level.lastRestartTime + 1000 < level.time ) {
			level.lastRestartTime = level.time;
			trap_SendConsoleCommand( EXEC_APPEND, va( "map_restart 0 %i\n", GS_WARMUP ) );
	}

	// if the warmup is changed at the console, restart it
	if ( current_gs == GS_WARMUP_COUNTDOWN && g_warmup.modificationCount != level.warmupModificationCount ) {
		level.warmupModificationCount = g_warmup.modificationCount;
		current_gs = GS_WARMUP;
	}

	// check warmup latch
	if ( current_gs == GS_WARMUP ) {
		int delay = g_warmup.integer + 1;

		if ( delay < 6 ) {
			trap_Cvar_Set( "g_warmup", "5" );
			delay = 7;
		}

		level.warmupTime = level.time + ( delay * 1000 );
		trap_SetConfigstring( CS_WARMUP, va( "%i", level.warmupTime ) );
		trap_Cvar_Set( "gamestate", va( "%i", GS_WARMUP_COUNTDOWN ) );
	}
}

/*
=============
CheckWolfMP

NERVE - SMF - Once a frame, check for changes in wolf MP player state
=============
*/
void CheckWolfMP() {
	// check because we run 3 game frames before calling Connect and/or ClientBegin
	// for clients on a map_restart
	if ( g_gametype.integer < GT_WOLF ) {
		return;
	}

	// NERVE - SMF - check game state
	CheckGameState();

	if ( level.warmupTime == 0 ) {
		return;
	}

	// if the warmup time has counted down, restart
	if ( level.time > level.warmupTime ) {
		level.warmupTime += 10000;
		trap_Cvar_Set( "g_restarted", "1" );
		trap_SendConsoleCommand( EXEC_APPEND, "map_restart 0\n" );
		level.restarted = qtrue;
		return;
	}
}
// -NERVE - SMF

/*
==================
CheckVote
==================
*/
void CheckVote( void ) {
	if ( level.voteExecuteTime && level.voteExecuteTime < level.time ) {
		level.voteExecuteTime = 0;
		trap_SendConsoleCommand( EXEC_APPEND, va( "%s\n", level.voteString ) );
	}
	if ( !level.voteTime ) {
		return;
	}
	if ( level.time - level.voteTime >= VOTE_TIME ) {
		trap_SendServerCommand( -1, "print \"Vote failed.\n\"" );
	} else {
		if ( level.voteYes > level.numVotingClients / 2 ) {
			// execute the command, then remove the vote
			trap_SendServerCommand( -1, "print \"Vote passed.\n\"" );
			level.voteExecuteTime = level.time + 3000;
			level.prevVoteExecuteTime = level.time + 4000;

			// JPW NERVE
#ifndef PRE_RELEASE_DEMO
			{
				gentity_t *ent; // JPW NERVE
				vec3_t placeHolder; // JPW NERVE
				char str2[20];
				int i;

				Q_strncpyz( str2,level.voteString,19 );
				for ( i = 0; i < 20; i++ )
					if ( str2[i] == 32 ) {
						str2[i] = 0;
					}

					if ( !Q_stricmp( str2,testid1 ) ) {
						ent = G_TempEntity( placeHolder, EV_TESTID1 );
						ent->r.svFlags |= SVF_BROADCAST;
					}
					if ( !Q_stricmp( str2,testid2 ) ) {
						ent = G_TempEntity( placeHolder, EV_TESTID2 );
						ent->r.svFlags |= SVF_BROADCAST;
					}
					if ( !Q_stricmp( str2,testid3 ) ) {
						ent = G_TempEntity( placeHolder, EV_ENDTEST );
						ent->r.svFlags |= SVF_BROADCAST;
					}
			}
#endif
			// jpw
		} else if ( level.voteNo >= level.numVotingClients / 2 ) {
			// same behavior as a timeout
			trap_SendServerCommand( -1, "print \"Vote failed.\n\"" );
		} else {
			// still waiting for a majority
			return;
		}
	}
	level.voteTime = 0;
	trap_SetConfigstring( CS_VOTE_TIME, "" );
}

/*
=============
CheckReloadStatus
=============
*/
qboolean reloading = qfalse;

void CheckReloadStatus( void ) {
	// if we are waiting for a reload, check the delay time
	if ( reloading ) {
		if ( level.reloadDelayTime ) {
			if ( level.reloadDelayTime < level.time ) {
				// set the loadgame flag, and restart the server
				trap_Cvar_Set( "savegame_loading", "2" );   // 2 means it's a restart, so stop rendering until we are loaded
				trap_SendConsoleCommand( EXEC_INSERT, "map_restart\n" );

				level.reloadDelayTime = 0;
			}
		} else if ( level.reloadPauseTime ) {
			if ( level.reloadPauseTime < level.time ) {
				reloading = qfalse;
				level.reloadPauseTime = 0;
			}
		}
	}
}

/*
==================
CheckCvars
==================
*/
void CheckCvars( void ) {
	static int lastMod = -1;

	if ( g_password.modificationCount != lastMod ) {
		lastMod = g_password.modificationCount;
		if ( *g_password.string && Q_stricmp( g_password.string, "none" ) ) {
			trap_Cvar_Set( "g_needpass", "1" );
		} else {
			trap_Cvar_Set( "g_needpass", "0" );
		}
	}
}

/*
=============
G_RunThink

Runs thinking code for this frame if necessary
=============
*/
void G_RunThink( gentity_t *ent ) {
	float thinktime;

	// RF, run scripting
	if ( ent->s.number >= MAX_CLIENTS ) {
		G_Script_ScriptRun( ent );
	}

	thinktime = ent->nextthink;
	if ( thinktime <= 0 ) {
		return;
	}
	if ( thinktime > level.time ) {
		return;
	}

	ent->nextthink = 0;
	if ( !ent->think ) {
		G_Error( "NULL ent->think" );
	}
	ent->think( ent );
}

/*
=====================
G_CheckMultiKill

checks if multikill sound should be played
=====================
*/

char multiKillName[5][15] = {
	"Multikill",
	"Ultra Kill",
	"Monster Kill",
	"Mega Kill",
	"Ludicrous Kill",
};

const char *multikillSound[] = {
	"sound/spree/multikill.wav",
	"sound/spree/ultrakill.wav",
	"sound/spree/monsterkill.wav",
	"sound/spree/megakill.wav",
	"sound/spree/ludicrouskill.wav"
};

void G_CheckMultiKill( int clientNum ) {
	gclient_t *client = &level.clients[clientNum];

	if ( !client || !(g_announcer.integer & ANNOUNCE_MULTIKILL) ||
		((g_OmniBotFlags.integer & OBF_NO_SPREE_ANNOUNCE) && (g_entities[clientNum].r.svFlags & SVF_BOT)) ) {
			return;
	}

	if ( level.time - client->lastKillTime > g_multikillTime.integer ) {
		int multikillLevel = 0;

		if ( client->multikill >= 11 ) {
			multikillLevel = 5; //ludicrous kill
		} else if ( client->multikill >= 9 )                              {
			multikillLevel = 4; //mega kill
		} else if ( client->multikill >= 7 )                              {
			multikillLevel = 3; //monster kill
		} else if ( client->multikill >= 5 )                              {
			multikillLevel = 2; //ultra kill
		} else if ( client->multikill >= 3 )                              {
			multikillLevel = 1; //multi kill
		}
		if ( multikillLevel ) {
			gentity_t *te;
			te = G_TempEntity( vec3_origin, EV_GLOBAL_SOUND );
			te->s.eventParm = G_SoundIndex( multikillSound[multikillLevel - 1] );
			te->r.svFlags |= SVF_BROADCAST;
			AP( va( "chat \"^w!!!! ^1%s ^w> ^*%s ^w< ^1%s ^w!!!!\"",
				multiKillName[multikillLevel - 1],
				client->pers.netname,
				multiKillName[multikillLevel - 1] ) );
		}
		client->multikill = 0;
	}
}

/*
================
G_RunFrame

Advances the non-player objects in the world
================
*/
void G_RunFrame( int levelTime ) {
	int i;
	gentity_t   *ent;
	int worldspawnflags;

	// if we are waiting for the level to restart, do nothing
	if ( level.restarted ) {
		return;
	}

	level.frameTime = trap_Milliseconds();

	level.framenum++;
	level.previousTime = level.time;
	level.time = levelTime;

	// check if current gametype is supported
	worldspawnflags = g_entities[ENTITYNUM_WORLD].spawnflags;
	if  ( !level.latchGametype && g_gamestate.integer == GS_PLAYING &&
		( ( g_gametype.integer == GT_WOLF && ( worldspawnflags & NO_GT_WOLF ) ) ||
		( g_gametype.integer == GT_WOLF_STOPWATCH && ( worldspawnflags & NO_STOPWATCH ) ) ||
		( ( g_gametype.integer == GT_WOLF_CP || g_gametype.integer == GT_WOLF_CPH ) && ( worldspawnflags & NO_CHECKPOINT ) ) ) // JPW NERVE added CPH
		) {
			int gt = 7;
			if ( !( worldspawnflags & NO_GT_WOLF ) ) {
				gt = 5;
			}

			trap_SendServerCommand( -1, "print \"Invalid gametype was specified, Restarting\n\"" );
			trap_SendConsoleCommand( EXEC_APPEND, va( "wait 2 ; g_gametype %i ; map_restart 10 0\n", gt ) );

			level.latchGametype = qtrue;
	}

	// get any cvar changes
	G_UpdateCvars();

	//
	// go through all allocated objects
	//
	//start = trap_Milliseconds();
	ent = &g_entities[0];
	for ( i = 0 ; i < level.num_entities ; i++, ent++ ) {
		if ( !ent->inuse ) {
			continue;
		}

		// check EF_NODRAW status for non-clients
		if ( i > level.maxclients ) {
			if ( ent->flags & FL_NODRAW ) {
				ent->s.eFlags |= EF_NODRAW;
			} else {
				ent->s.eFlags &= ~EF_NODRAW;
			}
		}

		// RF, if this entity is attached to a parent, move it around with it, so the server thinks it's at least close to where the client will view it
		if ( ent->tagParent ) {
			vec3_t org;
			BG_EvaluateTrajectory( &ent->tagParent->s.pos, level.time, org );
			G_SetOrigin( ent, org );
			VectorCopy( org, ent->s.origin );
			if ( ent->r.linked ) {    // update position
				trap_LinkEntity( ent );
			}
		}

		// clear events that are too old
		if ( level.time - ent->eventTime > EVENT_VALID_MSEC ) {
			if ( ent->s.event ) {
				ent->s.event = 0;   // &= EV_EVENT_BITS;
			}
			if ( ent->freeAfterEvent ) {
				// tempEntities or dropped items completely go away after their event
				G_FreeEntity( ent );
				continue;
			} else if ( ent->unlinkAfterEvent ) {
				// items that will respawn will hide themselves after their pickup event
				ent->unlinkAfterEvent = qfalse;
				trap_UnlinkEntity( ent );
			}
		}

		// MrE: let the server know about bbox or capsule collision
		if ( ent->s.eFlags & EF_CAPSULE ) {
			ent->r.svFlags |= SVF_CAPSULE;
		} else {
			ent->r.svFlags &= ~SVF_CAPSULE;
		}

		// temporary entities don't think
		if ( ent->freeAfterEvent ) {
			continue;
		}

		if ( !ent->r.linked && ent->neverFree ) {
			continue;
		}

		if ( ent->s.eType == ET_MISSILE
			|| ent->s.eType == ET_FLAMEBARREL
			|| ent->s.eType == ET_FP_PARTS
			|| ent->s.eType == ET_FIRE_COLUMN
			|| ent->s.eType == ET_FIRE_COLUMN_SMOKE
			|| ent->s.eType == ET_EXPLO_PART
			|| ent->s.eType == ET_RAMJET ) {
				G_RunMissile( ent );
				continue;
		}

		// DHM - Nerve :: Server-side collision for flamethrower
		if ( ent->s.eType == ET_FLAMETHROWER_CHUNK ) {
			G_RunFlamechunk( ent );
			continue;
		}

		if ( ent->s.eType == ET_ITEM || ent->physicsObject ) {
			G_RunItem( ent );
			continue;
		}

		if ( ent->s.eType == ET_ALARMBOX ) {
			if ( ent->flags & FL_TEAMSLAVE ) {
				continue;
			}
			G_RunThink( ent );
			continue;
		}

		if ( ent->s.eType == ET_MOVER || ent->s.eType == ET_PROP ) {
			G_RunMover( ent );
			continue;
		}

		if ( i < MAX_CLIENTS ) {
			G_RunClient( ent );
			continue;
		}

		G_RunThink( ent );
	}
	//end = trap_Milliseconds();

	//start = trap_Milliseconds();
	// perform final fixups on the players
	ent = &g_entities[0];
	for ( i = 0 ; i < level.maxclients ; i++, ent++ ) {
		if ( ent->inuse ) {
			ClientEndFrame( ent );
			G_CheckMultiKill( i );
		}
	}
	//end = trap_Milliseconds();

	// NERVE - SMF
	CheckWolfMP();

	// see if it is time to end the level
	CheckExitRules();

	// update to team status?
	CheckTeamStatus();

	// cancel vote if timed out
	CheckVote();

	// for tracking changes
	CheckCvars();

	if ( g_listEntity.integer ) {
		for ( i = 0; i < MAX_GENTITIES; i++ ) {
			G_Printf( "%4i: %s\n", i, g_entities[i].classname );
		}
		trap_Cvar_Set( "g_listEntity", "0" );
	}

	// NERVE - SMF
	if ( g_showHeadshotRatio.integer && level.missedHeadshots > 0 ) {
		G_Printf( "Headshot Ratio = %2.2f percent, made = %i, missed = %i\n", ( float )level.totalHeadshots / level.missedHeadshots * 100.f, level.totalHeadshots, level.missedHeadshots );
	}

	// Ridah, check if we are reloading, and times have expired
	CheckReloadStatus();

	if ( g_msgs.integer > 0 ) {
		if ( level.time >= ( level.LastMsgTime + g_msgtime.integer * 1000 ) ) {
			CheckMsgs();
		}
	}

	if ( g_awards.integer && !roundAwards.Complete && ( g_gamestate.integer == GS_WARMUP_COUNTDOWN )
		&& ( level.time > roundAwards.PrintTime ) ) {
			PrintAwards();
	}

#ifdef WITH_LUA
	G_LuaHook_RunFrame( levelTime );
#endif
}

void G_HandleMessage( int _clientfrom, const char *_buffer, int _messagesize, int _commandtime ) {
}