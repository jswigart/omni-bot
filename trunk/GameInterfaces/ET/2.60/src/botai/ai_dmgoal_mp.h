
/*****************************************************************************
 * name:		ai_dmgoal_mp.h
 *
 * desc:		Wolf bot AI
 *
 *
 *****************************************************************************/

#ifndef __AI_DMGOAL_MP_H__
#define __AI_DMGOAL_MP_H__

//
// MULTIPLAYER GOAL AI
//

qboolean BotMP_CheckEmergencyGoals( bot_state_t *bs );
qboolean BotMP_FindGoal( bot_state_t *bs );
// Gordon: new version
qboolean BotMP_FindGoal_New( bot_state_t *bs );

#endif

