
/*****************************************************************************
 * name:		ai_cmd.c
 *
 * desc:		Wolf bot AI
 *
 *
 *****************************************************************************/

#include "../game/g_local.h"
#include "../game/botlib.h"
#include "../game/be_aas.h"
#include "../game/be_ea.h"
#include "../game/be_ai_char.h"
#include "../game/be_ai_chat.h"
#include "../game/be_ai_gen.h"
#include "../game/be_ai_goal.h"
#include "../game/be_ai_move.h"
#include "../game/be_ai_weap.h"
#include "../botai/botai.h"
//
#include "ai_main.h"
#include "ai_dmq3.h"
#include "ai_cmd.h"
//
#include "chars.h"				//characteristics
#include "inv.h"				//indexes into the inventory
#include "syn.h"				//synonyms
#include "match.h"				//string matching types and vars


#ifdef DEBUG
/*
==================
BotPrintTeamGoal
==================
*/
void BotPrintTeamGoal(bot_state_t *bs) {
	char netname[MAX_NETNAME];
	float t;

	ClientName(bs->client, netname, sizeof(netname));
	t = bs->teamgoal_time - trap_AAS_Time();
	switch(bs->ltgtype) {
		case LTG_TEAMHELP:
		{
			BotAI_Print(PRT_MESSAGE, "%s: I'm gonna help a team mate for %1.0f secs\n", netname, t);
			break;
		}
		case LTG_TEAMACCOMPANY:
		{
			BotAI_Print(PRT_MESSAGE, "%s: I'm gonna accompany a team mate for %1.0f secs\n", netname, t);
			break;
		}
		case LTG_GETFLAG:
		{
			BotAI_Print(PRT_MESSAGE, "%s: I'm gonna get the flag for %1.0f secs\n", netname, t);
			break;
		}
		case LTG_RUSHBASE:
		{
			BotAI_Print(PRT_MESSAGE, "%s: I'm gonna rush to the base for %1.0f secs\n", netname, t);
			break;
		}
		case LTG_RETURNFLAG:
		{
			BotAI_Print(PRT_MESSAGE, "%s: I'm gonna try to return the flag for %1.0f secs\n", netname, t);
			break;
		}
		case LTG_DEFENDKEYAREA:
		{
			BotAI_Print(PRT_MESSAGE, "%s: I'm gonna defend a key area for %1.0f secs\n", netname, t);
			break;
		}
		case LTG_GETITEM:
		{
			BotAI_Print(PRT_MESSAGE, "%s: I'm gonna get an item for %1.0f secs\n", netname, t);
			break;
		}
		case LTG_KILL:
		{
			BotAI_Print(PRT_MESSAGE, "%s: I'm gonna kill someone for %1.0f secs\n", netname, t);
			break;
		}
		case LTG_CAMP:
		case LTG_CAMPORDER:
		{
			BotAI_Print(PRT_MESSAGE, "%s: I'm gonna camp for %1.0f secs\n", netname, t);
			break;
		}
		case LTG_PATROL:
		{
			BotAI_Print(PRT_MESSAGE, "%s: I'm gonna patrol for %1.0f secs\n", netname, t);
			break;
		}
		default:
		{
			if (bs->ctfroam_time > trap_AAS_Time()) {
				t = bs->ctfroam_time - trap_AAS_Time();
				BotAI_Print(PRT_MESSAGE, "%s: I'm gonna roam for %1.0f secs\n", netname, t);
			}
			else {
				BotAI_Print(PRT_MESSAGE, "%s: I've got a regular goal\n", netname);
			}
		}
	}
}
#endif //DEBUG

/*
==================
BotGetItemTeamGoal

FIXME: add stuff like "upper rocket launcher"
"the rl near the railgun", "lower grenade launcher" etc.
==================
*/
int BotGetItemTeamGoal(char *goalname, bot_goal_t *goal) {
	int i;

	if (!strlen(goalname)) return qfalse;
	i = -1;
	do {
		i = trap_BotGetLevelItemGoal(i, goalname, goal);
		if (i > 0) {// && !AvoidGoalTime(&bs->gs, goal.number))
			return qtrue;
		}
	} while(i > 0);
	return qfalse;
}

/*
==================
BotGetMessageTeamGoal
==================
*/
int BotGetMessageTeamGoal(bot_state_t *bs, char *goalname, bot_goal_t *goal) {
	bot_waypoint_t *cp;

	if (BotGetItemTeamGoal(goalname, goal)) return qtrue;

	cp = BotFindWayPoint(bs->checkpoints, goalname);
	if (cp) {
		memcpy(goal, &cp->goal, sizeof(bot_goal_t));
		return qtrue;
	}
	return qfalse;
}

/*
==================
BotGetTime
==================
*/
float BotGetTime(bot_match_t *match) {
	bot_match_t timematch;
	char timestring[MAX_MESSAGE_SIZE];
	float t;

	//if the matched string has a time
	if (match->subtype & ST_TIME) {
		//get the time string
		trap_BotMatchVariable(match, TIME, timestring, MAX_MESSAGE_SIZE);
		//match it to find out if the time is in seconds or minutes
		if (trap_BotFindMatch(timestring, &timematch, MTCONTEXT_TIME)) {
			if (timematch.type == MSG_FOREVER) {
				t = 99999999;
			}
			else {
				trap_BotMatchVariable(&timematch, TIME, timestring, MAX_MESSAGE_SIZE);
				if (timematch.type == MSG_MINUTES) t = atof(timestring) * 60;
				else if (timematch.type == MSG_SECONDS) t = atof(timestring);
				else t = 0;
			}
			//if there's a valid time
			if (t > 0) return trap_AAS_Time() + t;
		}
	}
	return 0;
}

/*
==================
FindClientByName
==================
*/
int FindClientByName(char *name) {
	int i, j;
	char buf[MAX_INFO_STRING];

	for(j = 0; j < level.numConnectedClients; j++) {
		i = level.sortedClients[j];
		ClientName(i, buf, sizeof(buf));
		if (!Q_stricmp(buf, name)) {
			return i;
		}
	}

	for(j = 0; j < level.numConnectedClients; j++) {
		i = level.sortedClients[j];
		ClientName(i, buf, sizeof(buf));
		if( stristr(buf, name) ) {
			return i;
		}
	}

	return -1;
}

/*
==================
FindEnemyByName
==================
*/
int FindEnemyByName(bot_state_t *bs, char *name) {
	int i, j;
	char buf[MAX_INFO_STRING];

	for(j = 0; j < level.numConnectedClients; j++) {
		i = level.sortedClients[j];
		if( BotSameTeam(bs, i) ) {
			continue;
		}
		ClientName(i, buf, sizeof(buf));
		if( !Q_stricmp(buf, name) ) {
			return i;
		}
	}
	for(j = 0; j < level.numConnectedClients; j++) {
		i = level.sortedClients[j];
		if( BotSameTeam(bs, i) ) {
			continue;
		}
		ClientName(i, buf, sizeof(buf));
		if( stristr(buf, name) ) {
			return i;
		}
	}
	return -1;
}

/*
==================
NumPlayersOnSameTeam
==================
*/
int NumPlayersOnSameTeam( bot_state_t *bs ) {
	int i, j, num = 0;

	for(j = 0; j < level.numConnectedClients; j++) {
		i = level.sortedClients[j];
		if( i == bs->client ) {
			continue;
		}

		if( BotSameTeam(bs, j) ) {
			num++;
		}
	}
	return num;
}

/*
==================
TeamPlayIsOn
==================
*/
int BotGetPatrolWaypoints(bot_state_t *bs, bot_match_t *match) {
	char keyarea[MAX_MESSAGE_SIZE];
	int patrolflags;
	bot_waypoint_t *wp, *newwp, *newpatrolpoints;
	bot_match_t keyareamatch;
	bot_goal_t goal;

	newpatrolpoints = NULL;
	patrolflags = 0;
	//
	trap_BotMatchVariable(match, KEYAREA, keyarea, MAX_MESSAGE_SIZE);
	//
	while(1) {
		if (!trap_BotFindMatch(keyarea, &keyareamatch, MTCONTEXT_PATROLKEYAREA)) {
			trap_EA_SayTeam(bs->client, "what do you say?");
			BotFreeWaypoints(newpatrolpoints);
			bs->patrolpoints = NULL;
			return qfalse;
		}
		trap_BotMatchVariable(&keyareamatch, KEYAREA, keyarea, MAX_MESSAGE_SIZE);
		if (!BotGetMessageTeamGoal(bs, keyarea, &goal)) {
			//BotAI_BotInitialChat(bs, "cannotfind", keyarea, NULL);
			//trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
			BotFreeWaypoints(newpatrolpoints);
			bs->patrolpoints = NULL;
			return qfalse;
		}
		//create a new waypoint
		newwp = BotCreateWayPoint(keyarea, goal.origin, goal.areanum);
		//add the waypoint to the patrol points
		newwp->next = NULL;
		for (wp = newpatrolpoints; wp && wp->next; wp = wp->next);
		if (!wp) {
			newpatrolpoints = newwp;
			newwp->prev = NULL;
		}
		else {
			wp->next = newwp;
			newwp->prev = wp;
		}
		//
		if (keyareamatch.subtype & ST_BACK) {
			patrolflags = PATROL_LOOP;
			break;
		}
		else if (keyareamatch.subtype & ST_REVERSE) {
			patrolflags = PATROL_REVERSE;
			break;
		}
		else if (keyareamatch.subtype & ST_MORE) {
			trap_BotMatchVariable(&keyareamatch, MORE, keyarea, MAX_MESSAGE_SIZE);
		}
		else {
			break;
		}
	}
	//
	if (!newpatrolpoints || !newpatrolpoints->next) {
		trap_EA_SayTeam(bs->client, "I need more key points to patrol\n");
		BotFreeWaypoints(newpatrolpoints);
		newpatrolpoints = NULL;
		return qfalse;
	}
	//
	BotFreeWaypoints(bs->patrolpoints);
	bs->patrolpoints = newpatrolpoints;
	//
	bs->curpatrolpoint = bs->patrolpoints;
	//
	return qtrue;
}

/*
==================
BotAddressedToBot
==================
*/
int BotAddressedToBot(bot_state_t *bs, bot_match_t *match) {
	char addressedto[MAX_MESSAGE_SIZE];
	char netname[MAX_MESSAGE_SIZE];
	char name[MAX_MESSAGE_SIZE];
	char botname[128];
	int client;
	bot_match_t addresseematch;

	trap_BotMatchVariable(match, NETNAME, netname, sizeof(netname));
	client = ClientFromName(netname);
	if (client < 0) return qfalse;
	if (!BotSameTeam(bs, client)) return qfalse;
	//if the message is addressed to someone
	if (match->subtype & ST_ADDRESSED) {
		trap_BotMatchVariable(match, ADDRESSEE, addressedto, sizeof(addressedto));
		//the name of this bot
		ClientName(bs->client, botname, 128);
		//
		while(trap_BotFindMatch(addressedto, &addresseematch, MTCONTEXT_ADDRESSEE)) {
			if (addresseematch.type == MSG_EVERYONE) {
				return qtrue;
			}
			else if (addresseematch.type == MSG_MULTIPLENAMES) {
				trap_BotMatchVariable(&addresseematch, TEAMMATE, name, sizeof(name));
				if (strlen(name)) {
					if (stristr(botname, name)) return qtrue;
					if (stristr(bs->subteam, name)) return qtrue;
				}
				trap_BotMatchVariable(&addresseematch, MORE, addressedto, MAX_MESSAGE_SIZE);
			}
			else {
				trap_BotMatchVariable(&addresseematch, TEAMMATE, name, MAX_MESSAGE_SIZE);
				if (strlen(name)) {
					if (stristr(botname, name)) return qtrue;
					if (stristr(bs->subteam, name)) return qtrue;
				}
				break;
			}
		}
		//Com_sprintf(buf, sizeof(buf), "not addressed to me but %s", addressedto);
		//trap_EA_Say(bs->client, buf);
		return qfalse;
	} else {
		//make sure not everyone reacts to this message
		if( random() > 1.f / (float)NumPlayersOnSameTeam(bs) ) {
			return qfalse;
		}
	}
	return qtrue;
}

/*
==================
BotGPSToPosition
==================
*/
int BotGPSToPosition(char *buf, vec3_t position) {
	int i, j = 0;
	int num, sign;

	for (i = 0; i < 3; i++) {
		num = 0;
		while(buf[j] == ' ') j++;
		if (buf[j] == '-') {
			j++;
			sign = -1;
		}
		else {
			sign = 1;
		}
		while (buf[j]) {
			if (buf[j] >= '0' && buf[j] <= '9') {
				num = num * 10 + buf[j] - '0';
				j++;
			}
			else {
				j++;
				break;
			}
		}
		BotAI_Print(PRT_MESSAGE, "%d\n", sign * num);
		position[i] = (float) sign * num;
	}
	return qtrue;
}

/*
==================
BotMatch_HelpAccompany
==================
*/
void BotMatch_HelpAccompany(bot_state_t *bs, bot_match_t *match) {
/*
	int client, other, areanum;
	char teammate[MAX_MESSAGE_SIZE], netname[MAX_MESSAGE_SIZE];
	char itemname[MAX_MESSAGE_SIZE];
	bot_match_t teammatematch;
	aas_entityinfo_t entinfo;

	if (!TeamPlayIsOn()) return;
	//if not addressed to this bot
	if (!BotAddressedToBot(bs, match)) return;
	//get the team mate name
	trap_BotMatchVariable(match, TEAMMATE, teammate, sizeof(teammate));
	//get the client to help
	if (trap_BotFindMatch(teammate, &teammatematch, MTCONTEXT_TEAMMATE) &&
			//if someone asks for him or herself
			teammatematch.type == MSG_ME) {
		//get the netname
		trap_BotMatchVariable(match, NETNAME, netname, sizeof(netname));
		client = ClientFromName(netname);
		other = qfalse;
	}
	else {
		//asked for someone else
		client = FindClientByName(teammate);
		//if this is the bot self
		if (client == bs->client) {
			other = qfalse;
		}
		else if (!BotSameTeam(bs, client)) {
			//FIXME: say "I don't help the enemy"
			return;
		}
		else {
			other = qtrue;
		}
	}
	//if the bot doesn't know who to help (FindClientByName returned -1)
	if (client < 0) {
		if (other) BotAI_BotInitialChat(bs, "whois", teammate, NULL);
		else BotAI_BotInitialChat(bs, "whois", netname, NULL);
		trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
		return;
	}
	//don't help or accompany yourself
	if (client == bs->client) {
		return;
	}
	//
	bs->teamgoal.entitynum = -1;
	BotEntityInfo(client, &entinfo);
	//if info is valid (in PVS)
	if (entinfo.valid) {
		areanum = BotPointAreaNum(entinfo.number, entinfo.origin);
		if (areanum && trap_AAS_AreaReachability(areanum)) {
			bs->teamgoal.entitynum = client;
			bs->teamgoal.areanum = areanum;
			VectorCopy(entinfo.origin, bs->teamgoal.origin);
			VectorSet(bs->teamgoal.mins, -8, -8, -8);
			VectorSet(bs->teamgoal.maxs, 8, 8, 8);
		}
	}
	//if no teamgoal yet
	if (bs->teamgoal.entitynum < 0) {
		//if near an item
		if (match->subtype & ST_NEARITEM) {
			//get the match variable
			trap_BotMatchVariable(match, ITEM, itemname, sizeof(itemname));
			//
			if (!BotGetMessageTeamGoal(bs, itemname, &bs->teamgoal)) {
				//BotAI_BotInitialChat(bs, "cannotfind", itemname, NULL);
				//trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
				return;
			}
		}
	}
	//
	if (bs->teamgoal.entitynum < 0) {
		if (other) BotAI_BotInitialChat(bs, "whereis", teammate, NULL);
		else BotAI_BotInitialChat(bs, "whereareyou", netname, NULL);
		trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
		return;
	}
	//the team mate
	bs->teammate = client;
	//last time the team mate was assumed visible
	bs->teammatevisible_time = trap_AAS_Time();
	//set the time to send a message to the team mates
	bs->teammessage_time = trap_AAS_Time() + 2 * random();
	//get the team goal time
	bs->teamgoal_time = BotGetTime(match);
	//set the ltg type
	if (match->type == MSG_HELP) {
		bs->ltgtype = LTG_TEAMHELP;
		if (!bs->teamgoal_time) bs->teamgoal_time = trap_AAS_Time() + TEAM_HELP_TIME;
	}
	else {
		bs->ltgtype = LTG_TEAMACCOMPANY;
		if (!bs->teamgoal_time) bs->teamgoal_time = trap_AAS_Time() + TEAM_ACCOMPANY_TIME;
		bs->arrive_time = 0;
	}
#ifdef DEBUG
	BotPrintTeamGoal(bs);
#endif //DEBUG
*/
}

/*
==================
BotMatch_DefendKeyArea
==================
*/
void BotMatch_DefendKeyArea(bot_state_t *bs, bot_match_t *match) {
	char itemname[MAX_MESSAGE_SIZE];

	if (!TeamPlayIsOn()) return;
	//if not addressed to this bot
	if (!BotAddressedToBot(bs, match)) return;
	//get the match variable
	trap_BotMatchVariable(match, KEYAREA, itemname, sizeof(itemname));
	//
	if (!BotGetMessageTeamGoal(bs, itemname, &bs->teamgoal)) {
		//BotAI_BotInitialChat(bs, "cannotfind", itemname, NULL);
		//trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
		return;
	}
	//set the time to send a message to the team mates
	bs->teammessage_time = trap_AAS_Time() + 2 * random();
	//set the ltg type
	bs->ltgtype = LTG_DEFENDKEYAREA;
	//get the team goal time
	bs->teamgoal_time = BotGetTime(match);
	//set the team goal time
	if (!bs->teamgoal_time) bs->teamgoal_time = trap_AAS_Time() + TEAM_DEFENDKEYAREA_TIME;
	//away from defending
#ifdef DEBUG
	BotPrintTeamGoal(bs);
#endif //DEBUG
}

/*
==================
BotMatch_GetItem
==================
*/
void BotMatch_GetItem(bot_state_t *bs, bot_match_t *match) {
	char itemname[MAX_MESSAGE_SIZE];

	if (!TeamPlayIsOn()) return;
	//if not addressed to this bot
	if (!BotAddressedToBot(bs, match)) return;
	//get the match variable
	trap_BotMatchVariable(match, ITEM, itemname, sizeof(itemname));
	//
	if (!BotGetMessageTeamGoal(bs, itemname, &bs->teamgoal)) {
		//BotAI_BotInitialChat(bs, "cannotfind", itemname, NULL);
		//trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
		return;
	}
	//set the time to send a message to the team mates
	bs->teammessage_time = trap_AAS_Time() + 2 * random();
	//set the ltg type
	bs->ltgtype = LTG_GETITEM;
	//set the team goal time
	bs->teamgoal_time = trap_AAS_Time() + TEAM_GETITEM_TIME;
#ifdef DEBUG
	BotPrintTeamGoal(bs);
#endif //DEBUG
}

/*
==================
BotMatch_Camp
==================
*/
void BotMatch_Camp(bot_state_t *bs, bot_match_t *match) {
/*
	int client, areanum;
	char netname[MAX_MESSAGE_SIZE];
	char itemname[MAX_MESSAGE_SIZE];
	aas_entityinfo_t entinfo;

	if (!TeamPlayIsOn()) return;
	//if not addressed to this bot
	if (!BotAddressedToBot(bs, match)) return;
	//
	trap_BotMatchVariable(match, NETNAME, netname, sizeof(netname));
	//asked for someone else
	client = FindClientByName(netname);
	//if there's no valid client with this name
	if (client < 0) {
		BotAI_BotInitialChat(bs, "whois", netname, NULL);
		trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
		return;
	}
	//get the match variable
	trap_BotMatchVariable(match, KEYAREA, itemname, sizeof(itemname));
	//in CTF it could be the base
	if (match->subtype & ST_THERE) {
		//camp at the spot the bot is currently standing
		bs->teamgoal.entitynum = bs->entitynum;
		bs->teamgoal.areanum = bs->areanum;
		VectorCopy(bs->origin, bs->teamgoal.origin);
		VectorSet(bs->teamgoal.mins, -8, -8, -8);
		VectorSet(bs->teamgoal.maxs, 8, 8, 8);
	}
	else if (match->subtype & ST_HERE) {
		//if this is the bot self
		if (client == bs->client) return;
		//
		bs->teamgoal.entitynum = -1;
		BotEntityInfo(client, &entinfo);
		//if info is valid (in PVS)
		if (entinfo.valid) {
			areanum = BotPointAreaNum(entinfo.number, entinfo.origin);
			if (areanum && trap_AAS_AreaReachability(areanum)) {
				//NOTE: just cheat and assume the bot knows where the person is
				//if (BotEntityVisible(bs->entitynum, bs->eye, bs->viewangles, 360, client)) {
					bs->teamgoal.entitynum = client;
					bs->teamgoal.areanum = areanum;
					VectorCopy(entinfo.origin, bs->teamgoal.origin);
					VectorSet(bs->teamgoal.mins, -8, -8, -8);
					VectorSet(bs->teamgoal.maxs, 8, 8, 8);
				//}
			}
		}
		//if the other is not visible
		if (bs->teamgoal.entitynum < 0) {
			BotAI_BotInitialChat(bs, "whereareyou", netname, NULL);
			trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
			return;
		}
	}
	else if (!BotGetMessageTeamGoal(bs, itemname, &bs->teamgoal)) {
		//BotAI_BotInitialChat(bs, "cannotfind", itemname, NULL);
		//trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
		return;
	}
	//set the time to send a message to the team mates
	bs->teammessage_time = trap_AAS_Time() + 2 * random();
	//set the ltg type
	bs->ltgtype = LTG_CAMPORDER;
	//get the team goal time
	bs->teamgoal_time = BotGetTime(match);
	//set the team goal time
	if (!bs->teamgoal_time) bs->teamgoal_time = trap_AAS_Time() + TEAM_CAMP_TIME;
	//the teammate that requested the camping
	bs->teammate = client;
	//not arrived yet
	bs->arrive_time = 0;
	//
#ifdef DEBUG
	BotPrintTeamGoal(bs);
#endif //DEBUG
*/
}

/*
==================
BotMatch_Patrol
==================
*/
void BotMatch_Patrol(bot_state_t *bs, bot_match_t *match) {
	if (!TeamPlayIsOn()) return;
	//if not addressed to this bot
	if (!BotAddressedToBot(bs, match)) return;
	//get the patrol waypoints
	if (!BotGetPatrolWaypoints(bs, match)) return;
	//set the time to send a message to the team mates
	bs->teammessage_time = trap_AAS_Time() + 2 * random();
	//set the ltg type
	bs->ltgtype = LTG_PATROL;
	//get the team goal time
	bs->teamgoal_time = BotGetTime(match);
	//set the team goal time if not set already
	if (!bs->teamgoal_time) bs->teamgoal_time = trap_AAS_Time() + TEAM_PATROL_TIME;
	//
#ifdef DEBUG
	BotPrintTeamGoal(bs);
#endif //DEBUG
}

/*
==================
BotMatch_GetFlag
==================
*/
void BotMatch_GetFlag(bot_state_t *bs, bot_match_t *match) {
	//if not in CTF mode
	return;
/*	//if not addressed to this bot
	if (!BotAddressedToBot(bs, match)) return;
	//set the time to send a message to the team mates
	bs->teammessage_time = trap_AAS_Time() + 2 * random();
	//set the ltg type
	bs->ltgtype = LTG_GETFLAG;
	//set the team goal time
	bs->teamgoal_time = trap_AAS_Time() + CTF_GETFLAG_TIME;
#ifdef DEBUG
	BotPrintTeamGoal(bs);
#endif //DEBUG*/
}

/*
==================
BotMatch_RushBase
==================
*/
void BotMatch_RushBase(bot_state_t *bs, bot_match_t *match) {
	//if not in CTF mode
	return;
/*	//if not addressed to this bot
	if (!BotAddressedToBot(bs, match)) return;
	//set the time to send a message to the team mates
	bs->teammessage_time = trap_AAS_Time() + 2 * random();
	//set the ltg type
	bs->ltgtype = LTG_RUSHBASE;
	//set the team goal time
	bs->teamgoal_time = trap_AAS_Time() + CTF_RUSHBASE_TIME;
	bs->rushbaseaway_time = 0;
#ifdef DEBUG
	BotPrintTeamGoal(bs);
#endif //DEBUG*/
}


/*
==================
BotMatch_ReturnFlag
==================
*/
void BotMatch_ReturnFlag(bot_state_t *bs, bot_match_t *match) {
	//if not in CTF mode
	return;
/*	//if not addressed to this bot
	if (!BotAddressedToBot(bs, match)) return;
	//set the time to send a message to the team mates
	bs->teammessage_time = trap_AAS_Time() + 2 * random();
	//set the ltg type
	bs->ltgtype = LTG_RETURNFLAG;
	//set the team goal time
	bs->teamgoal_time = trap_AAS_Time() + CTF_RETURNFLAG_TIME;
	bs->rushbaseaway_time = 0;
#ifdef DEBUG
	BotPrintTeamGoal(bs);
#endif //DEBUG*/
}

/*
==================
BotMatch_JoinSubteam
==================
*/
void BotMatch_JoinSubteam(bot_state_t *bs, bot_match_t *match) {
	char teammate[MAX_MESSAGE_SIZE];

	if (!TeamPlayIsOn()) return;
	//if not addressed to this bot
	if (!BotAddressedToBot(bs, match)) return;
	//get the sub team name
	trap_BotMatchVariable(match, TEAMNAME, teammate, MAX_MESSAGE_SIZE);
	//set the sub team name
	strncpy(bs->subteam, teammate, 32);
	bs->subteam[31] = '\0';
	//
	BotAI_BotInitialChat(bs, "joinedteam", teammate, NULL);
	trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
}

/*
==================
BotMatch_LeaveSubteam
==================
*/
void BotMatch_LeaveSubteam(bot_state_t *bs, bot_match_t *match) {
	if (!TeamPlayIsOn()) return;
	//if not addressed to this bot
	if (!BotAddressedToBot(bs, match)) return;
	//
	if (strlen(bs->subteam))
	{
		BotAI_BotInitialChat(bs, "leftteam", bs->subteam, NULL);
	} //end if
	trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
	strcpy(bs->subteam, "");
}

/*
==================
BotMatch_LeaveSubteam
==================
*/
void BotMatch_WhichTeam(bot_state_t *bs, bot_match_t *match) {
	if (!TeamPlayIsOn()) return;
	//if not addressed to this bot
	if (!BotAddressedToBot(bs, match)) return;
	//
	if (strlen(bs->subteam)) {
		BotAI_BotInitialChat(bs, "inteam", bs->subteam, NULL);
	}
	else {
		BotAI_BotInitialChat(bs, "noteam", NULL);
	}
	trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
}

/*
==================
BotMatch_CheckPoint
==================
*/
void BotMatch_CheckPoint(bot_state_t *bs, bot_match_t *match) {
	int areanum;
	char buf[MAX_MESSAGE_SIZE];
	vec3_t position;
	bot_waypoint_t *cp;

	if (!TeamPlayIsOn()) return;
	//
	trap_BotMatchVariable(match, POSITION, buf, MAX_MESSAGE_SIZE);
	VectorClear(position);
	//BotGPSToPosition(buf, position);
	sscanf(buf, "%f %f %f", &position[0], &position[1], &position[2]);
	position[2] += 0.5;
	areanum = BotPointAreaNum(bs->client, position);
	if (!areanum) {
		if (BotAddressedToBot(bs, match)) {
			BotAI_BotInitialChat(bs, "checkpoint_invalid", NULL);
			trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
		}
		return;
	}
	//
	trap_BotMatchVariable(match, NAME, buf, MAX_MESSAGE_SIZE);
	//check if there already exists a checkpoint with this name
	cp = BotFindWayPoint(bs->checkpoints, buf);
	if (cp) {
		if (cp->next) cp->next->prev = cp->prev;
		if (cp->prev) cp->prev->next = cp->next;
		else bs->checkpoints = cp->next;
		cp->inuse = qfalse;
	}
	//create a new check point
	cp = BotCreateWayPoint(buf, position, areanum);
	//add the check point to the bot's known chech points
	cp->next = bs->checkpoints;
	if (bs->checkpoints) bs->checkpoints->prev = cp;
	bs->checkpoints = cp;
	//
	if (BotAddressedToBot(bs, match)) {
		Com_sprintf(buf, sizeof(buf), "%1.0f %1.0f %1.0f", cp->goal.origin[0],
													cp->goal.origin[1],
													cp->goal.origin[2]);

		BotAI_BotInitialChat(bs, "checkpoint_confirm", cp->name, buf, NULL);
		trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
	}
}

/*
==================
BotMatch_FormationSpace
==================
*/
void BotMatch_FormationSpace(bot_state_t *bs, bot_match_t *match) {
	char buf[MAX_MESSAGE_SIZE];
	float space;

	if (!TeamPlayIsOn()) return;
	//if not addressed to this bot
	if (!BotAddressedToBot(bs, match)) return;
	//
	trap_BotMatchVariable(match, NUMBER, buf, MAX_MESSAGE_SIZE);
	//if it's the distance in feet
	if (match->subtype & ST_FEET) space = 0.3048 * 32 * atof(buf);
	//else it's in meters
	else space = 32 * atof(buf);
	//check if the formation intervening space is valid
	if (space < 48 || space > 500) space = 100;

}

/*
==================
BotMatch_Dismiss
==================
*/
void BotMatch_Dismiss(bot_state_t *bs, bot_match_t *match) {
	if (!TeamPlayIsOn()) return;
	//if not addressed to this bot
	if (!BotAddressedToBot(bs, match)) return;
	//
	bs->ltgtype = 0;
	bs->lead_time = 0;
	//
	BotAI_BotInitialChat(bs, "dismissed", NULL);
	trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
}

/*
==================
BotMatch_StartTeamLeaderShip
==================
*/
void BotMatch_StartTeamLeaderShip(bot_state_t *bs, bot_match_t *match) {
	int client;
	char teammate[MAX_MESSAGE_SIZE];

	if (!TeamPlayIsOn()) return;
	//if chats for him or herself
	if (match->subtype & ST_I) {
		//get the team mate that will be the team leader
		trap_BotMatchVariable(match, NETNAME, teammate, sizeof(teammate));
		strncpy(bs->teamleader, teammate, sizeof(bs->teamleader));
		bs->teamleader[sizeof(bs->teamleader)] = '\0';
	}
	//chats for someone else
	else {
		//get the team mate that will be the team leader
		trap_BotMatchVariable(match, TEAMMATE, teammate, sizeof(teammate));
		client = FindClientByName(teammate);
		if (client >= 0) ClientName(client, bs->teamleader, sizeof(bs->teamleader));
	}
}

/*
==================
BotMatch_StopTeamLeaderShip
==================
*/
void BotMatch_StopTeamLeaderShip(bot_state_t *bs, bot_match_t *match) {
	int client;
	char teammate[MAX_MESSAGE_SIZE];
	char netname[MAX_MESSAGE_SIZE];

	if (!TeamPlayIsOn()) return;
	//get the team mate that stops being the team leader
	trap_BotMatchVariable(match, TEAMMATE, teammate, sizeof(teammate));
	//if chats for him or herself
	if (match->subtype & ST_I) {
		trap_BotMatchVariable(match, NETNAME, netname, sizeof(netname));
		client = FindClientByName(netname);
	}
	//chats for someone else
	else {
		client = FindClientByName(teammate);
	} //end else
	if (client >= 0) {
		if (!Q_stricmp(bs->teamleader, ClientName(client, netname, sizeof(netname)))) {
			bs->teamleader[0] = '\0';
		}
	}
}

/*
==================
BotMatch_WhoIsTeamLeader
==================
*/
void BotMatch_WhoIsTeamLeader(bot_state_t *bs, bot_match_t *match) {
	char netname[MAX_MESSAGE_SIZE];

	if (!TeamPlayIsOn()) return;

	ClientName(bs->client, netname, sizeof(netname));
	//if this bot IS the team leader
	if (!Q_stricmp(netname, bs->teamleader)) {
		trap_EA_SayTeam(bs->client, "I'm the team leader\n");
	}
}

/*
==================
BotMatch_WhatAreYouDoing
==================
*/
void BotMatch_WhatAreYouDoing(bot_state_t *bs, bot_match_t *match) {
	char netname[MAX_MESSAGE_SIZE];
	char goalname[MAX_MESSAGE_SIZE];

	//if not addressed to this bot
	if (!BotAddressedToBot(bs, match)) return;
	//
	switch(bs->ltgtype) {
		case LTG_TEAMHELP:
		{
			trap_BotMatchVariable(match, NETNAME, netname, sizeof(netname));
			EasyClientName(bs->teammate, netname, MAX_MESSAGE_SIZE);
			BotAI_BotInitialChat(bs, "helping", netname, NULL);
			break;
		}
		case LTG_TEAMACCOMPANY:
		{
			trap_BotMatchVariable(match, NETNAME, netname, sizeof(netname));
			EasyClientName(bs->teammate, netname, MAX_MESSAGE_SIZE);
			BotAI_BotInitialChat(bs, "accompanying", netname, NULL);
			break;
		}
		case LTG_DEFENDKEYAREA:
		{
			trap_BotGoalName(bs->teamgoal.number, goalname, sizeof(goalname));
			BotAI_BotInitialChat(bs, "defending", goalname, NULL);
			break;
		}
		case LTG_GETITEM:
		{
			trap_BotGoalName(bs->teamgoal.number, goalname, sizeof(goalname));
			BotAI_BotInitialChat(bs, "gettingitem", goalname, NULL);
			break;
		}
		case LTG_KILL:
		{
			ClientName(bs->teamgoal.entitynum, netname, sizeof(netname));
			BotAI_BotInitialChat(bs, "killing", netname, NULL);
			break;
		}
		case LTG_CAMP:
		case LTG_CAMPORDER:
		{
			BotAI_BotInitialChat(bs, "camping", NULL);
			break;
		}
		case LTG_PATROL:
		{
			BotAI_BotInitialChat(bs, "patrolling", NULL);
			break;
		}
		case LTG_GETFLAG:
		{
			BotAI_BotInitialChat(bs, "capturingflag", NULL);
			break;
		}
		case LTG_RUSHBASE:
		{
			BotAI_BotInitialChat(bs, "rushingbase", NULL);
			break;
		}
		case LTG_RETURNFLAG:
		{
			BotAI_BotInitialChat(bs, "returningflag", NULL);
			break;
		}
		default:
		{
			BotAI_BotInitialChat(bs, "roaming", NULL);
			break;
		}
	}
	//chat what the bot is doing
	trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
}

/*
==================
BotMatch_WhatIsMyCommand
==================
*/
void BotMatch_WhatIsMyCommand(bot_state_t *bs, bot_match_t *match) {
	char netname[MAX_NETNAME];

	ClientName(bs->client, netname, sizeof(netname));
	if (Q_stricmp(netname, bs->teamleader) != 0) return;

}

/*
==================
BotNearestVisibleItem
==================
*/
float BotNearestVisibleItem(bot_state_t *bs, char *itemname, bot_goal_t *goal) {
	int i;
	char name[64];
	bot_goal_t tmpgoal;
	float dist, bestdist;
	vec3_t dir;
	bsp_trace_t trace;

	bestdist = 999999;
	i = -1;
	do {
		i = trap_BotGetLevelItemGoal(i, itemname, &tmpgoal);
		trap_BotGoalName(tmpgoal.number, name, sizeof(name));
		if (Q_stricmp(itemname, name) != 0) continue;
		VectorSubtract(tmpgoal.origin, bs->origin, dir);
		dist = VectorLength(dir);
		if (dist < bestdist) {
			//trace from start to end
			BotAI_Trace(&trace, bs->eye, NULL, NULL, tmpgoal.origin, bs->client, CONTENTS_SOLID|CONTENTS_PLAYERCLIP);
			if (trace.fraction >= 1.0) {
				bestdist = dist;
				memcpy(goal, &tmpgoal, sizeof(bot_goal_t));
			}
		}
	} while(i > 0);
	return bestdist;
}

/*
==================
BotMatch_WhereAreYou
==================
*/
void BotMatch_WhereAreYou(bot_state_t *bs, bot_match_t *match) {
	float dist, bestdist;
	int i, bestitem/*, redflagtt, blueflagtt, redtobluett*/;
	bot_goal_t goal;
	char *nearbyitems[] = {
		"Shotgun",
		"Grenade Launcher",
		"Rocket Launcher",
		"Plasmagun",
		"Railgun",
		"Lightning Gun",
		"BFG10K",
		"Quad Damage",
		"Regeneration",
		"Battle Suit",
		"Speed",
		"Invisibility",
		"Flight",
		"Armor",
		"Heavy Armor",
		"Red Flag",
		"Blue Flag",
		NULL
	};
	//
	if (!TeamPlayIsOn()) return;
	//if not addressed to this bot
	if (!BotAddressedToBot(bs, match)) return;

	bestitem = -1;
	bestdist = 999999;
	for (i = 0; nearbyitems[i]; i++) {
		dist = BotNearestVisibleItem(bs, nearbyitems[i], &goal);
		if (dist < bestdist) {
			bestdist = dist;
			bestitem = i;
		}
	}
	if (bestitem != -1) {
			BotAI_BotInitialChat(bs, "location", nearbyitems[bestitem], NULL);
		trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
	}
}

/*
==================
BotMatch_LeadTheWay
==================
*/
void BotMatch_LeadTheWay(bot_state_t *bs, bot_match_t *match) {
	aas_entityinfo_t entinfo;
	char netname[MAX_MESSAGE_SIZE], teammate[MAX_MESSAGE_SIZE];
	int client, areanum, other;

	if (!TeamPlayIsOn()) return;
	//if not addressed to this bot
	if (!BotAddressedToBot(bs, match)) return;
	//if someone asks for someone else
	if (match->subtype & ST_SOMEONE) {
		//get the team mate name
		trap_BotMatchVariable(match, TEAMMATE, teammate, sizeof(teammate));
		client = FindClientByName(teammate);
		//if this is the bot self
		if (client == bs->client) {
			other = qfalse;
		}
		else if (!BotSameTeam(bs, client)) {
			//FIXME: say "I don't help the enemy"
			return;
		}
		else {
			other = qtrue;
		}
	}
	else {
		//get the netname
		trap_BotMatchVariable(match, NETNAME, netname, sizeof(netname));
		client = ClientFromName(netname);
		other = qfalse;
	}
	//if the bot doesn't know who to help (FindClientByName returned -1)
	if (client < 0) {
		BotAI_BotInitialChat(bs, "whois", netname, NULL);
		trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
		return;
	}
	//
	bs->lead_teamgoal.entitynum = -1;
	BotEntityInfo(client, &entinfo);
	//if info is valid (in PVS)
	if (entinfo.valid) {
		areanum = BotPointAreaNum(entinfo.number, entinfo.origin);
		if (areanum && trap_AAS_AreaReachability(areanum)) {
			bs->lead_teamgoal.entitynum = client;
			bs->lead_teamgoal.areanum = areanum;
			VectorCopy(entinfo.origin, bs->lead_teamgoal.origin);
			VectorSet(bs->lead_teamgoal.mins, -8, -8, -8);
			VectorSet(bs->lead_teamgoal.maxs, 8, 8, 8);
		}
	}

	if (bs->teamgoal.entitynum < 0) {
		if (other) BotAI_BotInitialChat(bs, "whereis", teammate, NULL);
		else BotAI_BotInitialChat(bs, "whereareyou", netname, NULL);
		trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
		return;
	}

	bs->lead_time = trap_AAS_Time() + TEAM_LEAD_TIME;


}

/*
==================
BotMatch_Kill
==================
*/
void BotMatch_Kill(bot_state_t *bs, bot_match_t *match) {
	char enemy[MAX_MESSAGE_SIZE];
	int client;

	if (!TeamPlayIsOn()) return;
	//if not addressed to this bot
	if (!BotAddressedToBot(bs, match)) return;

	trap_BotMatchVariable(match, ENEMY, enemy, sizeof(enemy));
	//
	client = FindEnemyByName(bs, enemy);
	if (client < 0) {
		BotAI_BotInitialChat(bs, "whois", enemy, NULL);
		trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
		return;
	}
	bs->teamgoal.entitynum = client;
	//set the time to send a message to the team mates
	bs->teammessage_time = trap_AAS_Time() + 2 * random();
	//set the ltg type
	bs->ltgtype = LTG_KILL;
	//set the team goal time
	bs->teamgoal_time = trap_AAS_Time() + TEAM_KILL_SOMEONE;
#ifdef DEBUG
	BotPrintTeamGoal(bs);
#endif //DEBUG
}

/*
==================
BotMatchMessage
==================
*/
int BotMatchMessage(bot_state_t *bs, char *message) {
	bot_match_t match;

	match.type = 0;
	//if it is an unknown message
	if (!trap_BotFindMatch(message, &match, MTCONTEXT_ENTERGAME
											|MTCONTEXT_INITIALTEAMCHAT
											|MTCONTEXT_CTF)) {
		return qfalse;
	}
	//react to the found message
	switch(match.type) {
		case MSG_HELP:					//someone calling for help
		case MSG_ACCOMPANY:				//someone calling for company
		{
			BotMatch_HelpAccompany(bs, &match);
			break;
		}
		case MSG_DEFENDKEYAREA:			//teamplay defend a key area
		{
			BotMatch_DefendKeyArea(bs, &match);
			break;
		}
		case MSG_CAMP:					//camp somewhere
		{
			BotMatch_Camp(bs, &match);
			break;
		}
		case MSG_PATROL:				//patrol between several key areas
		{
			BotMatch_Patrol(bs, &match);
			break;
		}
		case MSG_GETITEM:
		{
			BotMatch_GetItem(bs, &match);
			break;
		}
		case MSG_JOINSUBTEAM:			//join a sub team
		{
			BotMatch_JoinSubteam(bs, &match);
			break;
		}
		case MSG_LEAVESUBTEAM:			//leave a sub team
		{
			BotMatch_LeaveSubteam(bs, &match);
			break;
		}
		case MSG_WHICHTEAM:
		{
			BotMatch_WhichTeam(bs, &match);
			break;
		}
		case MSG_CHECKPOINT:			//remember a check point
		{
			BotMatch_CheckPoint(bs, &match);
			break;
		}
		case MSG_CREATENEWFORMATION:	//start the creation of a new formation
		{
			trap_EA_SayTeam(bs->client, "the part of my brain to create formations has been damaged");
			break;
		}
		case MSG_FORMATIONPOSITION:		//tell someone his/her position in the formation
		{
			trap_EA_SayTeam(bs->client, "the part of my brain to create formations has been damaged");
			break;
		}
		case MSG_FORMATIONSPACE:		//set the formation space
		{
			BotMatch_FormationSpace(bs, &match);
			break;
		}
		case MSG_DOFORMATION:			//form a certain formation
		{
			break;
		}
		case MSG_DISMISS:				//dismiss someone
		{
			BotMatch_Dismiss(bs, &match);
			break;
		}
		case MSG_STARTTEAMLEADERSHIP:	//someone will become the team leader
		{
			BotMatch_StartTeamLeaderShip(bs, &match);
			break;
		}
		case MSG_STOPTEAMLEADERSHIP:	//someone will stop being the team leader
		{
			BotMatch_StopTeamLeaderShip(bs, &match);
			break;
		}
		case MSG_WHOISTEAMLAEDER:
		{
			BotMatch_WhoIsTeamLeader(bs, &match);
			break;
		}
		case MSG_WHATAREYOUDOING:		//ask a bot what he/she is doing
		{
			BotMatch_WhatAreYouDoing(bs, &match);
			break;
		}
		case MSG_WHATISMYCOMMAND:
		{
			BotMatch_WhatIsMyCommand(bs, &match);
			break;
		}
		case MSG_WHEREAREYOU:
		{
			BotMatch_WhereAreYou(bs, &match);
			break;
		}
		case MSG_LEADTHEWAY:
		{
			BotMatch_LeadTheWay(bs, &match);
			break;
		}
		case MSG_KILL:
		{
			BotMatch_Kill(bs, &match);
			break;
		}
		case MSG_ENTERGAME:				//someone entered the game
		{
			//NOTE: eliza chats will catch this
			//BotMatchVariable(&match, NETNAME, netname);
			//Com_sprintf(buf, sizeof(buf), "heya %s", netname);
			//EA_Say(bs->client, buf);
			break;
		}
		case MSG_WAIT:
		{
			break;
		}
		default:
		{
			BotAI_Print(PRT_MESSAGE, "unknown match type\n");
			break;
		}
	}
	return qtrue;
}
