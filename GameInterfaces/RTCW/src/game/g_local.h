// g_local.h -- local definitions for game module

#ifndef __G_LOCAL_H__
#define __G_LOCAL_H__

//#include "g_rtcwbot_interface.h"

#include "q_shared.h"
#include "bg_public.h"
#include "g_public.h"

//==================================================================

// the "gameversion" client command will print this plus compile date
//----(SA) Wolfenstein
#define GAMEVERSION "omnibot"
// done.

#define MAXBOTDIFFICULTY    7

#define WEAPONUNLOCK_WEAPONS  1 // any class can spawn with or pickup soldier weapons
#define WEAPONUNLOCK_NOSPAWN  2 // can pickup, but cannot spawn with the weapon
#define WEAPONUNLOCK_HANDICAP 4 // non-soldiers aren't as effective with soldier weapons

#define BODY_QUEUE_SIZE     8

#define INFINITE            1000000

#define FRAMETIME           100                 // msec
#define EVENT_VALID_MSEC    300

#define INTERMISSION_DELAY_TIME 1000

#define MAX_WP_HEAT         1500.f

#define MG42_MULTIPLAYER_HEALTH 350             // JPW NERVE

// gentity->flags
#define FL_GODMODE              0x00000010
#define FL_NOTARGET             0x00000020
#define FL_TEAMSLAVE            0x00000400  // not the first on the team
#define FL_NO_KNOCKBACK         0x00000800
#define FL_DROPPED_ITEM         0x00001000
#define FL_NO_BOTS              0x00002000  // spawn point not for bot use
#define FL_NO_HUMANS            0x00004000  // spawn point just for bots
#define FL_AI_GRENADE_KICK      0x00008000  // an AI has already decided to kick this grenade
// Rafael
#define FL_NOFATIGUE            0x00010000  // cheat flag no fatigue

#define FL_TOGGLE               0x00020000  //----(SA)	ent is toggling (doors use this for ex.)
#define FL_KICKACTIVATE         0x00040000  //----(SA)	ent has been activated by a kick (doors use this too for ex.)
#define FL_SOFTACTIVATE         0x00000040  //----(SA)	ent has been activated while 'walking' (doors use this too for ex.)
#define FL_DEFENSE_GUARD        0x00080000  // warzombie defense pose

#define FL_PARACHUTE            0x00100000
#define FL_WARZOMBIECHARGE      0x00200000
#define FL_NO_MONSTERSLICK      0x00400000
#define FL_NO_HEADCHECK         0x00800000

#define FL_NODRAW               0x01000000

// movers are things like doors, plats, buttons, etc
typedef enum {
	MOVER_POS1,
	MOVER_POS2,
	MOVER_POS3,
	MOVER_1TO2,
	MOVER_2TO1,
	// JOSEPH 1-26-00
	MOVER_2TO3,
	MOVER_3TO2,
	// END JOSEPH

	// Rafael
	MOVER_POS1ROTATE,
	MOVER_POS2ROTATE,
	MOVER_1TO2ROTATE,
	MOVER_2TO1ROTATE
} moverState_t;

// DHM - Nerve :: Worldspawn spawnflags to indicate if a gametype is not supported
#define NO_GT_WOLF      1
#define NO_STOPWATCH    2
#define NO_CHECKPOINT   4

//============================================================================

typedef struct gentity_s gentity_t;
typedef struct gclient_s gclient_t;

//====================================================================
//
// Scripting, these structure are not saved into savegames (parsed each start)
typedef struct
{
	char    *actionString;
	qboolean ( *actionFunc )( gentity_t *ent, char *params );
} g_script_stack_action_t;
//
typedef struct
{
	//
	// set during script parsing
	g_script_stack_action_t     *action;            // points to an action to perform
	char                        *params;
} g_script_stack_item_t;
//
#define G_MAX_SCRIPT_STACK_ITEMS    64
//
typedef struct
{
	g_script_stack_item_t items[G_MAX_SCRIPT_STACK_ITEMS];
	int numItems;
} g_script_stack_t;
//
typedef struct
{
	int eventNum;                           // index in scriptEvents[]
	char                *params;            // trigger targetname, etc
	g_script_stack_t stack;
} g_script_event_t;
//
typedef struct
{
	char        *eventStr;
	qboolean ( *eventMatch )( g_script_event_t *event, char *eventParm );
	int hash;
} g_script_event_define_t;
//
// Script Flags
#define SCFL_GOING_TO_MARKER    0x1
#define SCFL_ANIMATING          0x2
//
// Scripting Status (NOTE: this MUST NOT contain any pointer vars)
typedef struct
{
	int scriptStackHead, scriptStackChangeTime;
	int scriptEventIndex;       // current event containing stack of actions to perform
	// scripting system variables
	int scriptId;                   // incremented each time the script changes
	int scriptFlags;
	char    *animatingParams;
} g_script_status_t;
//
#define G_MAX_SCRIPT_ACCUM_BUFFERS  8
//
void G_Script_ScriptEvent( gentity_t *ent, char *eventStr, char *params );
//====================================================================

#define CFOFS( x ) ( (int)&( ( (gclient_t *)0 )->x ) )

struct gentity_s {
	entityState_t s;                // communicated by server to clients
	entityShared_t r;               // shared by both the server system and game

	// DO NOT MODIFY ANYTHING ABOVE THIS, THE SERVER
	// EXPECTS THE FIELDS IN THAT ORDER!
	//================================

	struct gclient_s    *client;            // NULL if not a client

	qboolean inuse;

	char        *classname;         // set in QuakeEd
	int spawnflags;                 // set in QuakeEd

	qboolean neverFree;             // if true, FreeEntity will only unlink
	// bodyque uses this

	int flags;                      // FL_* variables

	char        *model;
	char        *model2;
	int freetime;                   // level.time when the object was freed

	int eventTime;                  // events will be cleared EVENT_VALID_MSEC after set
	qboolean freeAfterEvent;
	qboolean unlinkAfterEvent;

	qboolean physicsObject;         // if true, it can be pushed by movers and fall off edges
	// all game items are physicsObjects,
	float physicsBounce;            // 1.0 = continuous bounce, 0.0 = no bounce
	int clipmask;                   // brushes with this content value will be collided against
	// when moving.  items and corpses do not collide against
	// players, for instance

	// movers
	moverState_t moverState;
	int soundPos1;
	int sound1to2;
	int sound2to1;
	int soundPos2;
	int soundLoop;
	// JOSEPH 1-26-00
	int sound2to3;
	int sound3to2;
	int soundPos3;
	// END JOSEPH

	int soundKicked;
	int soundKickedEnd;

	int soundSoftopen;
	int soundSoftendo;
	int soundSoftclose;
	int soundSoftendc;

	gentity_t   *parent;
	gentity_t   *nextTrain;
	gentity_t   *prevTrain;
	// JOSEPH 1-26-00
	vec3_t pos1, pos2, pos3;
	// END JOSEPH

	char        *message;

	int timestamp;              // body queue sinking, etc

	float angle;                // set in editor, -1 = up, -2 = down
	char        *target;
	char        *targetname;
	int targetnamehash;         // Gordon: adding a hash for this for faster lookups
	char        *team;
	char        *targetShaderName;
	char        *targetShaderNewName;
	gentity_t   *target_ent;

	float speed;
	float closespeed;           // for movers that close at a different speed than they open
	vec3_t movedir;

	int gDuration;
	int gDurationBack;
	vec3_t gDelta;
	vec3_t gDeltaBack;

	int nextthink;
	void ( *think )( gentity_t *self );
	void ( *reached )( gentity_t *self );       // movers call this when hitting endpoint
	void ( *blocked )( gentity_t *self, gentity_t *other );
	void ( *touch )( gentity_t *self, gentity_t *other, trace_t *trace );
	void ( *use )( gentity_t *self, gentity_t *other, gentity_t *activator );
	void ( *pain )( gentity_t *self, gentity_t *attacker, int damage, vec3_t point );
	void ( *die )( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod );

	int pain_debounce_time;
	int fly_sound_debounce_time;            // wind tunnel
	int last_move_time;

	int health;

	qboolean takedamage;

	int damage;
	int splashDamage;           // quad will increase this without increasing radius
	int splashRadius;
	int methodOfDeath;
	int splashMethodOfDeath;

	int count;

	gentity_t   *chain;
	gentity_t   *enemy;
	gentity_t   *activator;
	gentity_t   *teamchain;     // next entity in team
	gentity_t   *teammaster;    // master of the team

	int watertype;
	int waterlevel;

	int noise_index;

	// timing variables
	float wait;
	float random;

	// Rafael - sniper variable
	// sniper uses delay, random, radius
	int radius;
	float delay;

	// JOSEPH 10-11-99
	int TargetFlag;
	float duration;
	vec3_t rotate;
	vec3_t TargetAngles;
	// END JOSEPH

	gitem_t     *item;          // for bonus items

	// Ridah, AI fields
	char        *aiName;        // cs: used for movers / triggers too
	void ( *AIScript_AlertEntity )( gentity_t *ent );   // cs: airplanes, etc
	// done.

	vec3_t dl_color;
	char        *dl_stylestring;
	char        *dl_shader;
	int dl_atten;

	int key;                    // used by:  target_speaker->nopvs,

	qboolean active;

	// Rafael - mg42
	float harc;
	float varc;

	int props_frame_state;

	// Ridah
	int missionLevel;               // mission we are currently trying to complete
	// gets reset each new level
	// done.

	// Rafael
	qboolean is_dead;
	// done

	int start_size;
	int end_size;

	// Rafael props

	qboolean isProp;

	int mg42BaseEnt;

	gentity_t   *melee;

	char        *spawnitem;

	qboolean nopickup;

	int flameQuota, flameQuotaTime, flameBurnEnt;

	int count2;

	int grenadeExplodeTime;         // we've caught a grenade, which was due to explode at this time

	char        *track;

	// entity scripting system
	char                *scriptName;

	int numScriptEvents;
	g_script_event_t    *scriptEvents;  // contains a list of actions to perform for each event type
	g_script_status_t scriptStatus;     // current status of scripting
	// the accumulation buffer
	int scriptAccumBuffer[G_MAX_SCRIPT_ACCUM_BUFFERS];

	float accuracy;

	char        *tagName;       // name of the tag we are attached to
	gentity_t   *tagParent;

	float headshotDamageScale;

	int lastHintCheckTime;                  // DHM - Nerve

	entState_t entstate;
	// -------------------------------------------------------------------------------------------
	// if working on a post release patch, new variables should ONLY be inserted after this point
	// DHM - Nerve :: the above warning does not really apply to MP, but I'll follow it for good measure

	int voiceChatSquelch;                   // DHM - Nerve
	int voiceChatPreviousTime;              // DHM - Nerve
	int lastBurnedFrameNumber;              // JPW - Nerve   : to fix FT instant-kill exploit

	//for omni-bot dynamite goals
	int numPlanted;

	int thrownKnifeTime;
	qboolean poisoned;
	int poisonEnt;
	int lastPoisonTime;

	// no indoor arty from sandmod
	qboolean dmginloop;
	gentity_t   *dmgparent;
};

typedef enum {
	CON_DISCONNECTED,
	CON_CONNECTING,
	CON_CONNECTED
} clientConnected_t;

typedef enum {
	SPECTATOR_NOT,
	SPECTATOR_FREE,
	SPECTATOR_FOLLOW,
	SPECTATOR_SCOREBOARD
} spectatorState_t;

typedef enum {
	TEAM_BEGIN,     // Beginning a team game, spawn at base
	TEAM_ACTIVE     // Now actively playing
} playerTeamStateState_t;

typedef struct {
	playerTeamStateState_t state;
	int location;
} playerTeamState_t;

// the auto following clients don't follow a specific client
// number, but instead follow the first two active players
#define FOLLOW_ACTIVE1  -1
#define FOLLOW_ACTIVE2  -2

// client data that stays across multiple levels or tournament restarts
// this is achieved by writing all the data to cvar strings at game shutdown
// time and reading them back at connection time.  Anything added here
// MUST be dealt with in G_InitSessionData() / G_ReadSessionData() / G_WriteSessionData()
typedef struct {
	team_t sessionTeam;
	int spectatorTime;              // for determining next-in-line to play
	spectatorState_t spectatorState;
	int spectatorClient;            // for chasecam and follow mode
	int wins, losses;               // tournament stats
	int playerType;                 // DHM - Nerve :: for GT_WOLF
	int playerWeapon;               // DHM - Nerve :: for GT_WOLF
	int playerWeapon2;
	int playerItem;                 // DHM - Nerve :: for GT_WOLF
	int playerSkin;                 // DHM - Nerve :: for GT_WOLF
	int spawnObjectiveIndex;         // JPW NERVE index of objective to spawn nearest to (returned from UI)
	int latchPlayerType;            // DHM - Nerve :: for GT_WOLF not archived
	int latchPlayerWeapon;          // DHM - Nerve :: for GT_WOLF not archived
	int latchPlayerWeapon2;
	int latchPlayerItem;            // DHM - Nerve :: for GT_WOLF not archived
	int latchPlayerSkin;            // DHM - Nerve :: for GT_WOLF not archived
	qboolean botSuicide;            // CS: if true, bots will /kill 2 seconds before their next spawn
	qboolean botSuicidePersist;
	qboolean botPush;               // CS: in some cases we don't want bots pushing
	int credits;
	gentity_t *currentBetTarget;
	int currentBetAmount;
	int betTime;
	int buyTime;
} clientSession_t;

//
#define MAX_NETNAME         36
#define MAX_VOTE_COUNT      3

#define PICKUP_ACTIVATE 0   // pickup items only when using "+activate"
#define PICKUP_TOUCH    1   // pickup items when touched
#define PICKUP_FORCE    2   // pickup the next item when touched (and reset to PICKUP_ACTIVATE when done)

// zinx etpro antiwarp
#define LAG_MAX_COMMANDS 512
#define LAG_MAX_DELTA 75
#define LAG_MAX_DROP_THRESHOLD 800
#define LAG_MIN_DROP_THRESHOLD ( LAG_MAX_DROP_THRESHOLD - 200 )
#define LAG_DECAY 1.02f

// client data that stays across multiple respawns, but is cleared
// on each level change or team change at ClientBegin()
typedef struct {
	clientConnected_t connected;
	usercmd_t cmd;                  // we would lose angles if not persistant
	usercmd_t oldcmd;               // previous command processed by pmove()
	qboolean localClient;           // true if "ip" info key is "localhost"
	qboolean initialSpawn;          // the first spawn should be at a cool location
	qboolean predictItemPickup;     // based on cg_predictItems userinfo
	qboolean pmoveFixed;            //
	int pmoveMsec;
	char netname[MAX_NETNAME];

	int autoActivate;               // based on cg_autoactivate userinfo		(uses the PICKUP_ values above)
	int emptySwitch;                // based on cg_emptyswitch userinfo (means "switch my weapon for me when ammo reaches '0' rather than -1)

	int maxHealth;                  // for handicapping
	int enterTime;                  // level.time the client entered the game
	int connectTime;                // DHM - Nerve :: level.time the client first connected to the server
	playerTeamState_t teamState;    // status in teamplay games
	int voteCount;                  // to prevent people from constantly calling votes
	int teamVoteCount;              // to prevent people from constantly calling votes

	int complaints;                     // DHM - Nerve :: number of complaints lodged against this client
	int complaintClient;                // DHM - Nerve :: able to lodge complaint against this client
	int complaintEndTime;               // DHM - Nerve :: until this time has expired

	int lastReinforceTime;              // DHM - Nerve :: last reinforcement

	qboolean teamInfo;              // send team overlay updates?

	qboolean bAutoReloadAux; // TTimo - auxiliary storage for pmoveExt_t::bAutoReload, to achieve persistance

	int kills;                  //stats
	int deaths;
	int teamKills;
	int suicides;
	int gibs;
	int teamGibs;
	int revives;
	int medPacks;
	int ammoPacks;
	int acc_shots;
	int acc_hits;
	int headshots;
	int damage_given;
	int damage_received;
	int team_damage;
	int goomba;
	int poison;
	int knifeKill;
	int killSpreeMax;
	int deathSpreeMax;

	int seedPing;           //Martin - for fakeping

	// g_shortcuts
	int lastkilled_client;
	int lastrevive_client;
	int lastkiller_client;
	int lastammo_client;
	int lasthealth_client;
} clientPersistant_t;

typedef struct {
	vec3_t mins;
	vec3_t maxs;

	vec3_t origin;

	int time;
	int servertime;
} clientMarker_t;

#define MAX_CLIENT_MARKERS 10

#define LT_SPECIAL_PICKUP_MOD   3       // JPW NERVE # of times (minus one for modulo) LT must drop ammo before scoring a point
#define MEDIC_SPECIAL_PICKUP_MOD    4   // JPW NERVE same thing for medic

// this structure is cleared on each ClientSpawn(),
// except for 'client->pers' and 'client->sess'
struct gclient_s {
	// ps MUST be the first element, because the server expects it
	playerState_t ps;               // communicated by server to clients

	// the rest of the structure is private to game
	clientPersistant_t pers;
	clientSession_t sess;

	qboolean readyToExit;           // wishes to leave the intermission

	qboolean noclip;

	int lastCmdTime;                // level.time of last usercmd_t, for EF_CONNECTION
	// we can't just use pers.lastCommand.time, because
	// of the g_sycronousclients case
	int buttons;
	int oldbuttons;
	int latched_buttons;

	int wbuttons;
	int oldwbuttons;
	int latched_wbuttons;
	int flagParent;
	int lastJump;                   // for reverse FollowCycle
	vec3_t oldOrigin;

	// sum up damage over an entire frame, so
	// shotgun blasts give a single big kick
	int damage_armor;               // damage absorbed by armor
	int damage_blood;               // damage taken out of health
	int damage_knockback;           // impact damage
	vec3_t damage_from;             // origin for vector calculation
	qboolean damage_fromWorld;      // if true, don't use the damage_from vector

	int accurateCount;              // for "impressive" reward sound

	int accuracy_shots;             // total number of shots
	int accuracy_hits;              // total number of hits

	//
	int lastkilled_client;          // last client that this client killed
	int lasthurt_client;            // last client that damaged this client
	int lasthurt_mod;               // type of damage the client did

	// timers
	int respawnTime;                // can respawn when time > this, force after g_forcerespwan
	int inactivityTime;             // kick players when time > this
	qboolean inactivityWarning;     // qtrue if the five seoond warning has been given
	int rewardTime;                 // clear the EF_AWARD_IMPRESSIVE, etc when time > this

	int airOutTime;

	int lastKillTime;               // for multiple kill rewards
	int multikill;

	int switchTeamTime;             // time the player switched teams

	// timeResidual is used to handle events that happen every second
	// like health / armor countdowns and regeneration
	int timeResidual;

	float currentAimSpreadScale;

	int medicHealAmt;

	// RF, may be shared by multiple clients/characters
	animModelInfo_t *modelInfo;

	// -------------------------------------------------------------------------------------------
	// if working on a post release patch, new variables should ONLY be inserted after this point

	gentity_t   *persistantPowerup;
	int portalID;
	int ammoTimes[WP_NUM_WEAPONS];
	int invulnerabilityTime;

	gentity_t   *cameraPortal;              // grapple hook if out
	vec3_t cameraOrigin;

	int dropWeaponTime;         // JPW NERVE last time a weapon was dropped
	int limboDropWeapon;         // JPW NERVE weapon to drop in limbo
	int deployQueueNumber;         // JPW NERVE player order in reinforcement FIFO queue
	int sniperRifleFiredTime;         // JPW NERVE last time a sniper rifle was fired (for muzzle flip effects)
	float sniperRifleMuzzleYaw;       // JPW NERVE for time-dependent muzzle flip in multiplayer
	int lastBurnTime;         // JPW NERVE last time index for flamethrower burn
	int PCSpecialPickedUpCount;         // JPW NERVE used to count # of times somebody's picked up this LTs ammo (or medic health) (for scoring)
	int saved_persistant[MAX_PERSISTANT];           // DHM - Nerve :: Save ps->persistant here during Limbo

	// g_antilag.c
	int topMarker;
	clientMarker_t clientMarkers[MAX_CLIENT_MARKERS];
	clientMarker_t backupMarker;

	// lt info from sandmod
	int lastLTinfotime;

	// zinx etpro antiwarp
	int lastUpdateFrame;
	int frameOffset;
	qboolean warping;
	qboolean warped;
	int lastCmdRealTime;
	int cmdhead;                    // antiwarp command queue head
	int cmdcount;                   // antiwarp command queue # valid commands
	float cmddelta;                 // antiwarp command queue # valid commands

	usercmd_t cmds[LAG_MAX_COMMANDS];       // antiwarp command queue

	gentity_t       *tempHead;  // Gordon: storing a temporary head for bullet head shot detection

	pmoveExt_t pmext;
};

typedef struct
{
	int NumAwards;
	qboolean Complete;
	qboolean FirstAnnounce;
	int PrintTime;
	qboolean DelayPrint;

	// kills
	char highKillName[MAX_NETNAME];
	int highKillValue;
	qboolean highKillAnnounce;

	// deaths
	char highDeathName[MAX_NETNAME];
	int highDeathValue;
	qboolean highDeathAnnounce;

	// gibs
	char highGibsName[MAX_NETNAME];
	int highGibsValue;
	qboolean highGibsAnnounce;

	// accuracy
	char highAccName[MAX_NETNAME];
	float highAccValue;
	qboolean highAccAnnounce;

	// revives
	char highRevName[MAX_NETNAME];
	int highRevValue;
	qboolean highRevAnnounce;

	// med packs
	char highMedName[MAX_NETNAME];
	int highMedValue;
	qboolean highMedAnnounce;

	// ammo packs
	char highAmmoName[MAX_NETNAME];
	int highAmmoValue;
	qboolean highAmmoAnnounce;

	// headshots
	char highHSName[MAX_NETNAME];
	int highHSValue;
	qboolean highHSAnnounce;

	// goombas
	char highGoombaName[MAX_NETNAME];
	int highGoombaValue;
	qboolean highGoombaAnnounce;

	// poisons
	char highPoisonName[MAX_NETNAME];
	int highPoisonValue;
	qboolean highPoisonAnnounce;

	// knives
	char highKnifeName[MAX_NETNAME];
	int highKnifeValue;
	qboolean highKnifeAnnounce;

	// high kill spree
	char highKillSpreeName[MAX_NETNAME];
	int highKillSpreeValue;
	qboolean highKillSpreeAnnounce;

	// high death spree
	char highDeathSpreeName[MAX_NETNAME];
	int highDeathSpreeValue;
	qboolean highDeathSpreeAnnounce;
}round_awards_t;

//
// this structure is cleared as each map is entered
//
#define MAX_SPAWN_VARS          64
#define MAX_SPAWN_VARS_CHARS    2048

typedef struct {
	struct gclient_s    *clients;       // [maxclients]

	struct gentity_s    *gentities;
	int gentitySize;
	int num_entities;               // current number, <= MAX_GENTITIES

	int warmupTime;                 // restart match at this time

	char rawmapname[MAX_QPATH];

	fileHandle_t logFile;

	// store latched cvars here that we want to get at often
	int maxclients;

	int framenum;
	int time;                           // in msec
	int previousTime;                   // so movers can back up when blocked
	int frameTime;                      // Gordon: time the frame started, for antilag stuff

	int startTime;                      // level.time the map was started

	int teamScores[TEAM_NUM_TEAMS];
	int lastTeamLocationTime;               // last time of client team location update

	/* cs: these appear to be unused
	int soldierChargeTime[2];
	int medicChargeTime[2];
	int engineerChargeTime[2];
	int lieutenantChargeTime[2]; */

	qboolean newSession;                // don't use any old session data, because
	// we changed gametype

	qboolean restarted;                 // waiting for a map_restart to fire

	int numConnectedClients;
	int numNonSpectatorClients;         // includes connecting clients
	int numPlayingClients;              // connected, non-spectators
	int sortedClients[MAX_CLIENTS];             // sorted by score
	int follow1, follow2;               // clientNums for auto-follow spectators

	int snd_fry;                        // sound index for standing in lava

	int warmupModificationCount;            // for detecting if g_warmup is changed

	// voting state
	char voteString[MAX_STRING_CHARS];
	char voteDisplayString[MAX_STRING_CHARS];
	int voteTime;                       // level.time vote was called
	int voteExecuteTime;                // time the vote is executed
	int prevVoteExecuteTime;            // JPW NERVE last vote execute time
	int voteYes;
	int voteNo;
	int numVotingClients;               // set by CalculateRanks

	// team voting state
	char teamVoteString[2][MAX_STRING_CHARS];
	int teamVoteTime[2];                // level.time vote was called
	int teamVoteYes[2];
	int teamVoteNo[2];
	int numteamVotingClients[2];        // set by CalculateRanks

	// spawn variables
	qboolean spawning;                  // the G_Spawn*() functions are valid
	int numSpawnVars;
	char        *spawnVars[MAX_SPAWN_VARS][2];  // key / value pairs
	int numSpawnVarChars;
	char spawnVarChars[MAX_SPAWN_VARS_CHARS];

	// intermission state
	int intermissionQueued;             // intermission was qualified, but
	// wait INTERMISSION_DELAY_TIME before
	// actually going there so the last
	// frag can be watched.  Disable future
	// kills during this delay
	int intermissiontime;               // time the intermission was started
	char        *changemap;
	qboolean readyToExit;               // at least one client wants to exit
	int exitTime;
	vec3_t intermission_origin;         // also used for spectator spawns
	vec3_t intermission_angle;

	qboolean locationLinked;            // target_locations get linked
	gentity_t   *locationHead;          // head of the location list
	int bodyQueIndex;                   // dead bodies
	gentity_t   *bodyQue[BODY_QUEUE_SIZE];

	int portalSequence;
	// Ridah
	char        *scriptAI;
	int reloadPauseTime;                // don't think AI/client's until this time has elapsed
	int reloadDelayTime;                // don't start loading the savegame until this has expired

	int lastGrenadeKick;

	int loperZapSound;
	int stimSoldierFlySound;
	int bulletRicochetSound;

	int snipersound;

	//----(SA)	added
	int knifeSound[4];
	//----(SA)	end

	// JPW NERVE
	int capturetimes[4];         // red, blue, none, spectator for WOLF_MP_CPH
	int redReinforceTime, blueReinforceTime;         // last time reinforcements arrived in ms
	int redNumWaiting, blueNumWaiting;         // number of reinforcements in queue
	vec3_t spawntargets[MAX_MULTI_SPAWNTARGETS];      // coordinates of spawn targets
	int numspawntargets;         // # spawntargets in this map
	// jpw

	// RF, entity scripting
	char        *scriptEntity;

	// player/AI model scripting (server repository)
	animScriptData_t animScriptData;

	// NERVE - SMF - debugging/profiling info
	int totalHeadshots;
	int missedHeadshots;
	qboolean lastRestartTime;
	// -NERVE - SMF

	int numFinalDead[2];                // DHM - Nerve :: unable to respawn and in limbo (per team)
	int numOidTriggers;                 // DHM - Nerve

	qboolean latchGametype;             // DHM - Nerve

	qboolean mapcoordsValid, tracemapLoaded;
	vec2_t mapcoordsMins, mapcoordsMaxs;

	//Omni-Bot time triggers
	qboolean twoMinute;
	qboolean thirtySecond;

	//offsets for spawn times
	int dwBlueReinfOffset;
	int dwRedReinfOffset;

	int LastMsgTime;
	int MsgNum;
	int TotalMsgs;
} level_locals_t;

extern qboolean reloading;                  // loading up a savegame
// JPW NERVE
extern char testid1[];
extern char testid2[];
extern char testid3[];
// jpw

//
// g_spawn.c
//
qboolean    G_SpawnString( const char *key, const char *defaultString, char **out );
qboolean    G_SpawnStringExt( const char *key, const char *defaultString, char **out, const char* file, int line );
// spawn string returns a temporary reference, you must CopyString() if you want to keep it
qboolean    G_SpawnFloat( const char *key, const char *defaultString, float *out );
qboolean    G_SpawnInt( const char *key, const char *defaultString, int *out );
qboolean    G_SpawnVector( const char *key, const char *defaultString, float *out );
#define     G_SpawnVector2D(    key, def, out ) G_SpawnVector2DExt( key, def, out, __FILE__, __LINE__ )
qboolean    G_SpawnVector2DExt( const char *key, const char *defaultString, float *out, const char* file, int line );
void        G_SpawnEntitiesFromString( void );
char *G_NewString( const char *string );
// Ridah
qboolean G_CallSpawn( gentity_t *ent );
// done.

//
// g_cmds.c
//
void Cmd_Kill_f( gentity_t *ent );
void Cmd_Score_f( gentity_t *ent );
void StopFollowing( gentity_t *ent );
//void BroadcastTeamChange( gclient_t *client, int oldTeam );
void SetTeam( gentity_t *ent, char *s, qboolean ignoreRestrictions );
void SetWolfData( gentity_t *ent, char *ptype, char *weap, char *grenade, char *skinnum );  // DHM - Nerve
void Cmd_FollowCycle_f( gentity_t *ent, int dir );

#ifdef WITH_LUA
char *ConcatArgs( int start );
void G_Say( gentity_t *ent, gentity_t *target, int mode, const char *chatText );
qboolean G_FlingClient( gentity_t *vic, int flingType );
qboolean G_FlingClientA( int flingType );
//char *Q_SayConcatArgs(int start);
//char *Q_AddCR(char *s);
//int Q_SayArgc();
//qboolean Q_SayArgv(int n, char *buffer, int bufferLength);
#endif

//
// g_items.c
//
void G_CheckTeamItems( void );
void G_RunItem( gentity_t *ent );
void RespawnItem( gentity_t *ent );

void UseHoldableItem( gentity_t *ent, int item );
void PrecacheItem( gitem_t *it );
gentity_t *Drop_Item( gentity_t *ent, gitem_t *item, float angle, qboolean novelocity );
gentity_t *LaunchItem( gitem_t *item, vec3_t origin, vec3_t velocity, int ownerNum );
void SetRespawn( gentity_t *ent, float delay );
void G_SpawnItem( gentity_t *ent, gitem_t *item );
void FinishSpawningItem( gentity_t *ent );
void Think_Weapon( gentity_t *ent );
int ArmorIndex( gentity_t *ent );
void Fill_Clip( playerState_t *ps, int weapon );
void Add_Ammo( gentity_t *ent, int weapon, int count, qboolean fillClip );
void Touch_Item( gentity_t *ent, gentity_t *other, trace_t *trace );

// Touch_Item_Auto is bound by the rules of autoactivation (if cg_autoactivate is 0, only touch on "activate")
void Touch_Item_Auto( gentity_t *ent, gentity_t *other, trace_t *trace );

void ClearRegisteredItems( void );
void RegisterItem( gitem_t *item );
void SaveRegisteredItems( void );
void Prop_Break_Sound( gentity_t *ent );
void Spawn_Shard( gentity_t *ent, gentity_t *inflictor, int quantity, int type );
void limbo( gentity_t *ent, qboolean makeCorpse );

//
// g_utils.c
//
// Ridah
int G_FindConfigstringIndex( const char *name, int start, int max, qboolean create );
int G_FindConfigstringByIndex( int index, int start, int max, char * nameOut, int nameBufferSize );

// done.
int G_ModelIndex( char *name );
int     G_SoundIndex( const char *name );
void    G_TeamCommand( team_t team, char *cmd );
void    G_KillBox( gentity_t *ent );
gentity_t *G_Find( gentity_t *from, int fieldofs, const char *match );
gentity_t *G_PickTarget( char *targetname );
void    G_UseTargets( gentity_t *ent, gentity_t *activator );
void    G_SetMovedir( vec3_t angles, vec3_t movedir );

void    G_InitGentity( gentity_t *e );
gentity_t   *G_Spawn( void );
gentity_t *G_TempEntity( vec3_t origin, int event );
gentity_t* G_FindByTargetname( gentity_t *from, const char* match );
void    G_Sound( gentity_t *ent, int soundIndex );
void    G_AnimScriptSound( int soundIndex, vec3_t org, int client );
void    G_FreeEntity( gentity_t *e );
//qboolean	G_EntitiesFree( void );

void    G_TouchTriggers( gentity_t *ent );
void    G_TouchSolids( gentity_t *ent );

float   *tv( float x, float y, float z );
char    *vtos( const vec3_t v );

void G_AddPredictableEvent( gentity_t *ent, int event, int eventParm );
void G_AddEvent( gentity_t *ent, int event, int eventParm );
void G_SetOrigin( gentity_t *ent, vec3_t origin );
void AddRemap( const char *oldShader, const char *newShader, float timeOffset );
const char *BuildShaderStateConfig();
void G_SetAngle( gentity_t *ent, vec3_t angle );

qboolean infront( gentity_t *self, gentity_t *other );

void G_ProcessTagConnect( gentity_t *ent );

void CleanseString( char *in, char *out, qboolean fToLower );
gentity_t *AICast_FindEntityForName( char *name );
float AngleDifference( float ang1, float ang2 );

qboolean G_EmplacedGunIsMountable( gentity_t* ent, gentity_t* other );
qboolean G_EmplacedGunIsRepairable( gentity_t* ent, gentity_t* other );
void CheckTotalMsgs();
void CheckMsgs( void );
char * weaponToString( weapon_t weapon );

void G_ClearBet(gentity_t *ent);
int G_ExtraAmmo( int playerClass, int weapon );

//
// g_combat.c
//
qboolean CanDamage( gentity_t *targ, vec3_t origin );
void G_Damage( gentity_t *targ, gentity_t *inflictor, gentity_t *attacker, vec3_t dir, vec3_t point, int damage, int dflags, int mod );
qboolean G_RadiusDamage( vec3_t origin, gentity_t *attacker, float damage, float radius, gentity_t *ignore, int mod );
void body_die( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath );
void TossClientItems( gentity_t *self );
gentity_t* G_BuildHead( gentity_t *ent );
void G_RailBox( vec_t* origin, vec_t* mins, vec_t* maxs, vec_t* color, int index );

// for indoor arty disabling from sandmod
void G_AdjustedDamageVec( gentity_t *ent, vec3_t origin, vec3_t v );
qboolean G_ET_RadiusDamage( vec3_t origin, gentity_t *inflictor, gentity_t *attacker, float damage, float radius, gentity_t *ignore, int mod );
void BG_ET_EvaluateTrajectory( const trajectory_t *tr, int atTime, vec3_t result, qboolean isAngle, int splinePath );

// damage flags
#define DAMAGE_RADIUS           0x00000001  // damage was indirect
#define DAMAGE_NO_ARMOR         0x00000002  // armour does not protect from this damage
#define DAMAGE_NO_KNOCKBACK     0x00000008  // do not affect velocity, just view angles
#define DAMAGE_NO_PROTECTION    0x00000020  // armor, shields, invulnerability, and godmode have no effect
#define DAMAGE_NO_TEAM_PROTECTION   0x00000010  // armor, shields, invulnerability, and godmode have no effect

//
// g_missile.c
//
void G_RunMissile( gentity_t *ent );
int G_PredictMissile( gentity_t *ent, int duration, vec3_t endPos, qboolean allowBounce );

// Rafael zombiespit
void G_RunDebris( gentity_t *ent );

//DHM - Nerve :: server side flamethrower collision
void G_RunFlamechunk( gentity_t *ent );

//----(SA) removed unused q3a weapon firing
gentity_t *fire_flamechunk( gentity_t *self, vec3_t start, vec3_t dir );

gentity_t *fire_grenade( gentity_t *self, vec3_t start, vec3_t dir, int grenadeWPID );
gentity_t *fire_rocket( gentity_t *self, vec3_t start, vec3_t dir );
gentity_t *fire_speargun( gentity_t *self, vec3_t start, vec3_t dir );

//----(SA)	added from MP
gentity_t *fire_nail( gentity_t *self, vec3_t start, vec3_t forward, vec3_t right, vec3_t up );
gentity_t *fire_prox( gentity_t *self, vec3_t start, vec3_t aimdir );
//----(SA)	end

// Rafael sniper
void fire_lead( gentity_t *self,  vec3_t start, vec3_t dir, int damage );
qboolean visible( gentity_t *self, gentity_t *other );

gentity_t *fire_mortar( gentity_t *self, vec3_t start, vec3_t dir );
gentity_t *fire_flamebarrel( gentity_t *self, vec3_t start, vec3_t dir );
// done

//
// g_mover.c
//
gentity_t *G_TestEntityPosition( gentity_t *ent );
void G_RunMover( gentity_t *ent );
void Use_BinaryMover( gentity_t *ent, gentity_t *other, gentity_t *activator );
void G_Activate( gentity_t *ent, gentity_t *activator );

void G_TryDoor( gentity_t *ent, gentity_t *other, gentity_t *activator ); //----(SA)	added

void InitMoverRotate( gentity_t *ent );

void InitMover( gentity_t *ent );
void SetMoverState( gentity_t *ent, moverState_t moverState, int time );

void func_explosive_explode( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod );

//
// g_tramcar.c
//
void Reached_Tramcar( gentity_t *ent );

//
// g_misc.c
//
void TeleportPlayer( gentity_t *player, vec3_t origin, vec3_t angles );
void mg42_fire( gentity_t *other );

//
// g_weapon.c
//
qboolean LogAccuracyHit( gentity_t *target, gentity_t *attacker );
void CalcMuzzlePoint( gentity_t *ent, int weapon, vec3_t forward, vec3_t right, vec3_t up, vec3_t muzzlePoint );
void SnapVectorTowards( vec3_t v, vec3_t to );
gentity_t *weapon_grenadelauncher_fire( gentity_t *ent, int grenadeWPID );
// Rafael

void CalcMuzzlePoints( gentity_t *ent, int weapon );

// Rafael - for activate
void CalcMuzzlePointForActivate( gentity_t *ent, vec3_t forward, vec3_t right, vec3_t up, vec3_t muzzlePoint );
// done.

//
// g_client.c
//
team_t TeamCount( int ignoreClientNum, int team );          // NERVE - SMF - merge from team arena
team_t PickTeam( int ignoreClientNum );
void SetClientViewAngle( gentity_t *ent, vec3_t angle );
gentity_t *SelectSpawnPoint( vec3_t avoidPoint, vec3_t origin, vec3_t angles );
void respawn( gentity_t *ent );
void BeginIntermission( void );
void InitClientPersistant( gclient_t *client );
void InitClientResp( gclient_t *client );
void InitBodyQue( void );
void ClientSpawn( gentity_t *ent, qboolean revived );
void player_die( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod );
void AddScore( gentity_t *ent, int score );
void CalculateRanks( void );
qboolean SpotWouldTelefrag( gentity_t *spot );

//
// g_svcmds.c
//
qboolean    ConsoleCommand( void );
void G_ProcessIPBans( void );
qboolean G_FilterPacket( char *from );
qboolean G_FilterMaxLivesPacket( char *from );
void AddMaxLivesIP( char *str );
void AddMaxLivesGUID( char *str );
void ClearMaxLivesIP();
void ClearMaxLivesGUID();

#define AP( x ) trap_SendServerCommand( -1, x )                 // Print to all
#define CP( x ) trap_SendServerCommand( ent - g_entities, x )     // Print to an ent
#define CPx( x, y ) trap_SendServerCommand( x, y )              // Print to id = x

//
// g_weapon.c
//
void G_BurnMeGood( gentity_t *self, gentity_t *body );
void FireWeapon( gentity_t *ent );

//
// p_hud.c
//
void MoveClientToIntermission( gentity_t *client );
void G_SetStats( gentity_t *ent );
void DeathmatchScoreboardMessage( gentity_t *client );

//
// g_cmds.c
//
void G_SayTo( gentity_t *ent, gentity_t *other, int mode, int color, const char *name, const char *message, qboolean localize ); // JPW NERVE removed static declaration so it would link

//
// g_pweapon.c
//

//
// g_main.c
//
void FindIntermissionPoint( void );
void G_RunThink( gentity_t *ent );
void QDECL G_LogPrintf( const char *fmt, ... );
void SendScoreboardMessageToAllClients( void );
void QDECL G_Printf( const char *fmt, ... );
void QDECL G_DPrintf( const char *fmt, ... );
void QDECL G_Error( const char *fmt, ... );

//
// g_client.c
//
char *ClientConnect( int clientNum, qboolean firstTime, qboolean isBot );
void ClientUserinfoChanged( int clientNum );
void ClientDisconnect( int clientNum );
void ClientBegin( int clientNum );
void ClientCommand( int clientNum );

//
// g_active.c
//
void ClientThink( int clientNum );
void ClientEndFrame( gentity_t *ent );
void G_RunClient( gentity_t *ent );
void ClientThink_cmd( gentity_t *ent, usercmd_t *cmd );

//
// et-antiwarp.c
//
void etpro_AddUsercmd( int clientNum, usercmd_t *cmd );
void DoClientThinks( gentity_t *ent );
qboolean G_DoAntiwarp( gentity_t *ent );

//
// g_awards.c
//
void PrintAwards();
void SetAwards( gclient_t *cl );
void ExtendWarmup( qboolean adjust );
void ClearAwards();
void WriteAwards();
void ReadAwards();

//
// g_team.c
//
qboolean OnSameTeam( gentity_t *ent1, gentity_t *ent2 );

//
// g_mem.c
//
void *G_Alloc( int size );
void G_InitMemory( void );
void Svcmd_GameMem_f( void );

//
// g_session.c
//
void G_ReadSessionData( gclient_t *client );
void G_InitSessionData( gclient_t *client, char *userinfo );

void G_InitWorldSession( void );
void G_WriteSessionData( void );

// g_cmd.c
void Cmd_Activate_f( gentity_t *ent );
void Cmd_Activate2_f( gentity_t *ent );
void G_PrivateMessage( gentity_t *ent, char *botTargetName, char *botMessage, qboolean IsBot );
int ClientNumbersFromString( char *s, int *plist );
void Cmd_Stats_f( gentity_t *ent );
void Cmd_ThrowKnife_f( gentity_t* ent );
void Touch_Knife( gentity_t *ent, gentity_t *other, trace_t *trace );
void Cmd_BotTapOut_f( gentity_t *ent );
// Ridah

// g_script.c
void G_Script_ScriptParse( gentity_t *ent );
qboolean G_Script_ScriptRun( gentity_t *ent );
void G_Script_ScriptEvent( gentity_t *ent, char *eventStr, char *params );
void G_Script_ScriptLoad( void );

float AngleDifference( float ang1, float ang2 );

// g_props.c
void Props_Chair_Skyboxtouch( gentity_t *ent );

#ifdef WITH_LUA
// g_sha1.c
char *G_SHA1( char *string );
#endif

#include "g_team.h" // teamplay specific stuff

extern level_locals_t level;
extern round_awards_t roundAwards;
extern gentity_t g_entities[];          //DAJ was explicit set to MAX_ENTITIES
extern gentity_t       *g_camEnt;

#define FOFS( x ) ( (int)&( ( (gentity_t *)0 )->x ) )

extern vmCvar_t	g_OmniBotLibrary;
extern vmCvar_t g_OmniBotPath;
extern vmCvar_t g_OmniBotEnable;
extern vmCvar_t g_OmniBotFlags;
extern vmCvar_t g_OmniBotPlaying;
extern vmCvar_t g_dropItems;
extern vmCvar_t g_helmetProtection;
extern vmCvar_t g_waterRevive;
extern vmCvar_t g_warmupDamage;
extern vmCvar_t g_OmniBotGib;
extern vmCvar_t g_throwableKnives;
extern vmCvar_t g_maxKnives;
extern vmCvar_t g_knifeDamage;
extern vmCvar_t g_stopMovers;
extern vmCvar_t g_botTeam;
extern vmCvar_t g_centerView;
extern vmCvar_t g_instaGib;
extern vmCvar_t g_goomba;
extern vmCvar_t g_goombaDmg;
extern vmCvar_t g_reportGibs;
extern vmCvar_t g_reportRevives;
extern vmCvar_t g_reportTaps;
extern vmCvar_t g_msg1;
extern vmCvar_t g_msg2;
extern vmCvar_t g_msg3;
extern vmCvar_t g_msg4;
extern vmCvar_t g_msg5;
extern vmCvar_t g_msg6;
extern vmCvar_t g_msg7;
extern vmCvar_t g_msg8;
extern vmCvar_t g_msg9;
extern vmCvar_t g_msg10;
extern vmCvar_t g_msgs;
extern vmCvar_t g_msgtime;
extern vmCvar_t g_msgpos;
extern vmCvar_t g_debugPlayerHitboxes;
extern vmCvar_t g_awards;
extern vmCvar_t g_medExtraClips;
extern vmCvar_t g_ltExtraClips;
extern vmCvar_t g_engExtraClips;
extern vmCvar_t g_soldExtraClips;
extern vmCvar_t g_venomExtraClips;
extern vmCvar_t g_mauserExtraClips;
extern vmCvar_t g_pistolExtraClips;
extern vmCvar_t g_panzerExtraClips;
extern vmCvar_t g_deathmatch;
extern vmCvar_t g_poison;
extern vmCvar_t g_poisonDmg;
extern vmCvar_t g_LTNades;
extern vmCvar_t g_MedNades;
extern vmCvar_t g_smokeGrenades;
extern vmCvar_t g_nadePacks;
extern vmCvar_t g_betting;
extern vmCvar_t g_muteSpecs;
extern vmCvar_t g_kickItems;
extern vmCvar_t g_binocs;
extern vmCvar_t g_unlockWeapons;
extern vmCvar_t g_flagLock;
extern vmCvar_t g_venomSpread;
extern vmCvar_t g_venomDamage;
extern vmCvar_t g_dynoFuseTime;

// zinx etpro antiwarp
extern vmCvar_t g_antiwarp;
extern vmCvar_t g_maxWarp;

// from noquarter
extern vmCvar_t g_announcer;
extern vmCvar_t g_shove;
extern vmCvar_t g_shoveNoZ;
extern vmCvar_t g_dragCorpse;
// end

// from sandmod
extern vmCvar_t g_fixedphysics;
extern vmCvar_t g_fakePing;
extern vmCvar_t g_LTinfomsg;
extern vmCvar_t g_allowGiveAmmo;
//end

// from etpub
extern vmCvar_t g_privateMessages;
extern vmCvar_t g_shortcuts;
// end

#ifdef WITH_LUA
extern vmCvar_t lua_modules;
extern vmCvar_t lua_allowedModules;
#endif

extern vmCvar_t g_gametype;

// Rafael gameskill
extern vmCvar_t g_gameskill;
// done

extern vmCvar_t g_dedicated;
extern vmCvar_t g_cheats;
extern vmCvar_t g_maxclients;               // allow this many total, including spectators
extern vmCvar_t g_maxGameClients;           // allow this many active
extern vmCvar_t g_minGameClients;           // NERVE - SMF - we need at least this many before match actually starts
extern vmCvar_t g_restarted;

extern vmCvar_t g_dmflags;
extern vmCvar_t g_fraglimit;
extern vmCvar_t g_timelimit;
extern vmCvar_t g_capturelimit;
extern vmCvar_t g_friendlyFire;
extern vmCvar_t g_password;
extern vmCvar_t g_needpass;
extern vmCvar_t g_gravity;
extern vmCvar_t g_speed;
extern vmCvar_t g_knockback;
extern vmCvar_t g_forcerespawn;
extern vmCvar_t g_inactivity;
extern vmCvar_t g_debugMove;
extern vmCvar_t g_debugAlloc;
extern vmCvar_t g_debugDamage;
extern vmCvar_t g_debugBullets;     //----(SA)	added
extern vmCvar_t g_weaponRespawn;
extern vmCvar_t g_synchronousClients;
extern vmCvar_t g_motd;
extern vmCvar_t g_warmup;
extern vmCvar_t g_voteFlags;

// DHM - Nerve :: The number of complaints allowed before kick/ban
extern vmCvar_t g_complaintlimit;
extern vmCvar_t g_maxlives;                 // DHM - Nerve :: number of respawns allowed (0==infinite)
extern vmCvar_t g_voiceChatsAllowed;        // DHM - Nerve :: number before spam control
extern vmCvar_t g_alliedmaxlives;           // Xian
extern vmCvar_t g_axismaxlives;             // Xian
extern vmCvar_t g_fastres;                  // Xian - Fast medic res'ing
extern vmCvar_t g_fastResMsec;
extern vmCvar_t g_knifeonly;                // Xian - Wacky Knife-Only rounds
extern vmCvar_t g_enforcemaxlives;          // Xian - Temp ban with maxlives between rounds

extern vmCvar_t g_needpass;
extern vmCvar_t g_weaponTeamRespawn;
extern vmCvar_t g_doWarmup;
extern vmCvar_t g_teamAutoJoin;
extern vmCvar_t g_teamForceBalance;
extern vmCvar_t g_banIPs;
extern vmCvar_t g_filterBan;
extern vmCvar_t g_rankings;
extern vmCvar_t g_enableBreath;
extern vmCvar_t pmove_fixed;
extern vmCvar_t pmove_msec;

//Rafael
extern vmCvar_t g_autoactivate;

extern vmCvar_t g_testPain;

extern vmCvar_t g_missionStats;
extern vmCvar_t ai_scriptName;          // name of AI script file to run (instead of default for that map)
extern vmCvar_t g_scriptName;           // name of script file to run (instead of default for that map)

extern vmCvar_t g_scriptDebug;

extern vmCvar_t g_userAim;

extern vmCvar_t g_forceModel;

extern vmCvar_t g_mg42arc;

extern vmCvar_t g_footstepAudibleRange;
// JPW NERVE multiplayer
extern vmCvar_t g_redlimbotime;
extern vmCvar_t g_bluelimbotime;
extern vmCvar_t g_medicChargeTime;
extern vmCvar_t g_engineerChargeTime;
extern vmCvar_t g_LTChargeTime;
extern vmCvar_t g_soldierChargeTime;
extern vmCvar_t sv_screenshake;
// jpw

// NERVE - SMF
extern vmCvar_t g_warmupLatch;
extern vmCvar_t g_nextTimeLimit;
extern vmCvar_t g_showHeadshotRatio;
extern vmCvar_t g_userTimeLimit;
extern vmCvar_t g_userAlliedRespawnTime;
extern vmCvar_t g_userAxisRespawnTime;
extern vmCvar_t g_currentRound;
extern vmCvar_t g_noTeamSwitching;
extern vmCvar_t g_altStopwatchMode;
extern vmCvar_t g_gamestate;
extern vmCvar_t g_swapteams;
// -NERVE - SMF

//Gordon
extern vmCvar_t g_antilag;

extern vmCvar_t g_dbgRevive;

void    trap_Printf( const char *fmt );
void    trap_Error( const char *fmt );
int     trap_Milliseconds( void );
int     trap_Argc( void );
void    trap_Argv( int n, char *buffer, int bufferLength );
void    trap_Args( char *buffer, int bufferLength );
int     trap_FS_FOpenFile( const char *qpath, fileHandle_t *f, fsMode_t mode );
void    trap_FS_Read( void *buffer, int len, fileHandle_t f );
int     trap_FS_Write( const void *buffer, int len, fileHandle_t f );
int     trap_FS_Rename( const char *from, const char *to );
void    trap_FS_FCloseFile( fileHandle_t f );
int     trap_FS_GetFileList( const char *path, const char *extension, char *listbuf, int bufsize );
void    trap_SendConsoleCommand( int exec_when, const char *text );
void    trap_Cvar_Register( vmCvar_t *cvar, const char *var_name, const char *value, int flags );
void    trap_Cvar_Update( vmCvar_t *cvar );
void    trap_Cvar_Set( const char *var_name, const char *value );
int     trap_Cvar_VariableIntegerValue( const char *var_name );
float   trap_Cvar_VariableValue( const char *var_name );
void    trap_Cvar_VariableStringBuffer( const char *var_name, char *buffer, int bufsize );
void    trap_LocateGameData( gentity_t *gEnts, int numGEntities, int sizeofGEntity_t, playerState_t *gameClients, int sizeofGameClient );
void    trap_DropClient( int clientNum, const char *reason );
void    trap_SendServerCommand( int clientNum, const char *text );
void    trap_SetConfigstring( int num, const char *string );
void    trap_GetConfigstring( int num, char *buffer, int bufferSize );
void    trap_GetUserinfo( int num, char *buffer, int bufferSize );
void    trap_SetUserinfo( int num, const char *buffer );
void    trap_GetServerinfo( char *buffer, int bufferSize );
void    trap_SetBrushModel( gentity_t *ent, const char *name );
void    trap_Trace( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask );
void    trap_TraceCapsule( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask );
int     trap_PointContents( const vec3_t point, int passEntityNum );
qboolean trap_InPVS( const vec3_t p1, const vec3_t p2 );
qboolean trap_InPVSIgnorePortals( const vec3_t p1, const vec3_t p2 );
void    trap_AdjustAreaPortalState( gentity_t *ent, qboolean open );
qboolean trap_AreasConnected( int area1, int area2 );
void    trap_LinkEntity( gentity_t *ent );
void    trap_UnlinkEntity( gentity_t *ent );
int     trap_EntitiesInBox( const vec3_t mins, const vec3_t maxs, int *entityList, int maxcount );
qboolean trap_EntityContact( const vec3_t mins, const vec3_t maxs, const gentity_t *ent );
qboolean trap_EntityContactCapsule( const vec3_t mins, const vec3_t maxs, const gentity_t *ent );

void    trap_GetUsercmd( int clientNum, usercmd_t *cmd );
qboolean    trap_GetEntityToken( char *buffer, int bufferSize );
qboolean trap_LoadTag( const char* filename );
qboolean trap_GetTag( int clientNum, char *tagName, orientation_t *ori );

int     trap_DebugPolygonCreate( int color, int numPoints, vec3_t *points );
void    trap_DebugPolygonDelete( int id );
int     trap_RealTime( qtime_t *qtime );

// bot lib
void    trap_BotUserCommand( int client, usercmd_t *ucmd );
void    trap_EA_Command( int client, char *command );
int     trap_BotAllocateClient( void );
void    trap_BotFreeClient( int clientNum );
int     trap_BotGetServerCommand( int clientNum, char *message, int size );

int     trap_GeneticParentsAndChildSelection( int numranks, float *ranks, int *parent1, int *parent2, int *child );
qboolean G_EmplacedGunIsRepairable( gentity_t* ent, gentity_t* other );
qboolean G_EmplacedGunIsMountable( gentity_t* ent, gentity_t* other );

void    trap_SnapVector( float *v );

typedef void (*pfnDebugCallback)();
void	trap_SetDebugFunctionCallback( pfnDebugCallback callback );

typedef enum
{
	shard_glass = 0,
	shard_wood,
	shard_metal,
	shard_ceramic,
	shard_rubble
} shards_t;

#define BODY_TEAM( ENT ) ENT->s.modelindex

// g_antilag.c
void G_StoreClientPosition( gentity_t* ent );
void G_ResetMarkers( gentity_t* ent );
void G_HistoricalTrace( gentity_t* ent, trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask );
void G_HistoricalTraceBegin( gentity_t *ent );
void G_HistoricalTraceEnd( gentity_t *ent );
void G_Trace( gentity_t* ent, trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask );

// credits
#define CREDITS_KILLBONUS 1
#define CREDITS_OBJBONUS 10
#define CREDITS_PRICE 50 // price for buying ammo / health
#define CREDITS_ENABLE 0x0001
#define CREDITS_BUYING 0x0002

// kick items
#define KICK_AIRSTRIKE	0x0001
#define KICK_GRENADE	0x0002

// binoculars
#define BINOCS_SOLDIER	0x0001
#define	BINOCS_ENG		0x0002
#define	BINOCS_MEDIC	0x0004

#endif // __G_LOCAL_H__
