
/*****************************************************************************
 * name:		be_ai_move.h
 *
 * desc:		movement AI
 *
 *
 *****************************************************************************/

#ifndef __BE_AI_MOVE_H__
#define __BE_AI_MOVE_H__

//movement types
#define MOVE_WALK						1
#define MOVE_CROUCH						2
#define MOVE_JUMP						4
#define MOVE_GRAPPLE					8
#define MOVE_ROCKETJUMP					16
#define MOVE_BFGJUMP					32
//move flags
#define MFL_BARRIERJUMP					1		//bot is performing a barrier jump
#define MFL_ONGROUND					2		//bot is in the ground
#define MFL_SWIMMING					4		//bot is swimming
#define MFL_AGAINSTLADDER				8		//bot is against a ladder
#define MFL_WATERJUMP					16		//bot is waterjumping
#define MFL_TELEPORTED					32		//bot is being teleported
#define MFL_ACTIVEGRAPPLE				64		//bot is using the grapple hook
#define MFL_GRAPPLERESET				128		//bot has reset the grapple
#define MFL_WALK						256		//bot should walk slowly
//move result flags
#define MOVERESULT_MOVEMENTVIEW			1		//bot uses view for movement
#define MOVERESULT_SWIMVIEW				2		//bot uses view for swimming
#define MOVERESULT_WAITING				4		//bot is waiting for something
#define MOVERESULT_MOVEMENTVIEWSET		8		//bot has set the view in movement code
#define MOVERESULT_MOVEMENTWEAPON		16		//bot uses weapon for movement
#define MOVERESULT_ONTOPOFOBSTACLE		32		//bot is ontop of obstacle
#define MOVERESULT_ONTOPOF_FUNCBOB		64		//bot is ontop of a func_bobbing
#define MOVERESULT_ONTOPOF_ELEVATOR		128		//bot is ontop of an elevator (func_plat)
#define	MOVERESULT_FUTUREVIEW			256		// RF, if we want to look ahead of time, this is a good direction
#define	MOVERESULT_DIRECTMOVE			512		// direct movement

//
#define MAX_AVOIDREACH					1
//
#define RESULTTYPE_ELEVATORUP				1
#define RESULTTYPE_INVISIBLEGRAPPLE		2

//structure used to initialize the movement state
//the or_moveflags MFL_ONGROUND, MFL_TELEPORTED and MFL_WATERJUMP come from the playerstate
typedef struct bot_initmove_s
{
	vec3_t origin;				//origin of the bot
	vec3_t velocity;			//velocity of the bot
	vec3_t viewoffset;			//view offset
	int entitynum;				//entity number of the bot
	int client;					//client number of the bot
	float thinktime;			//time the bot thinks
	int presencetype;			//presencetype of the bot
	vec3_t viewangles;			//view angles of the bot
	int or_moveflags;			//values ored to the movement flags
	int areanum;
} bot_initmove_t;

//NOTE: the ideal_viewangles are only valid if MFL_MOVEMENTVIEW is set
typedef struct bot_moveresult_s
{
	int failure;				//true if movement failed all together
	int type;					//failure or blocked type
	int blocked;				//true if blocked by an entity
	int blockentity;			//entity blocking the bot
	int traveltype;				//last executed travel type
	int flags;					//result flags
	int weapon;					//weapon used for movement
	vec3_t movedir;				//movement direction
	vec3_t ideal_viewangles;	//ideal viewangles for the movement
} bot_moveresult_t;

//resets the whole movestate
void BotResetMoveState(int movestate);
//moves the bot to the given goal
void BotMoveToGoal(bot_moveresult_t *result, int movestate, bot_goal_t *goal, int travelflags);
//moves the bot in the specified direction
int BotMoveInDirection(int movestate, vec3_t dir, float speed, int type);
//reset avoid reachability
void BotResetAvoidReach(int movestate);
//resets the last avoid reachability
void BotResetLastAvoidReach(int movestate);
//returns a reachability area if the origin is in one
int BotReachabilityArea(vec3_t origin, int client);
//view target based on movement
int BotMovementViewTarget(int movestate, bot_goal_t *goal, int travelflags, float lookahead, vec3_t target);
//predict the position of a player
int BotPredictVisiblePosition(vec3_t origin, int areanum, bot_goal_t *goal, int travelflags, vec3_t target);
//returns the handle of a newly allocated movestate
int BotAllocMoveState(void);
//frees the movestate with the given handle
void BotFreeMoveState(int handle);
//initialize movement state
void BotInitMoveState(int handle, bot_initmove_t *initmove);
//must be called every map change
void BotSetBrushModelTypes(void);
//setup movement AI
int BotSetupMoveAI(void);
//shutdown movement AI
void BotShutdownMoveAI(void);

// Ridah
//initialize avoid reachabilities
void BotInitAvoidReach(int handle);
// done.

#endif
