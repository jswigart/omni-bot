////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
// about: Exported function definitions
//		In order for the game to call functions from the bot, we must export
//		the functions to the game itself and allow it to call them.
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef _OMNIBOT_H__
#define _OMNIBOT_H__

#include "Omni-Bot_Types.h"
#include "Omni-Bot_Events.h"
#include "IEngineInterface.h"

class OmnibotFunctions : public OmnibotFunctionInterface
{
	virtual omnibot_error Initialize( IEngineInterface * engineFuncs, int version );

	virtual void Update();
	virtual void Shutdown();
	virtual void ConsoleCommand( const Arguments &args );

	virtual void SendTrigger( const TriggerInfo & triggerInfo );
	virtual void AddBlackboardRecord( BlackBoardKey  type, int posterID, int targetID, obUserData * data );

	// events
	virtual void SendEvent( const Message & message, GameEntity dest = GameEntity() );
	
	virtual void EntityAdded( GameEntity ent, const EntityInfo & entInfo );
	virtual void EntityDestroyed( GameEntity ent );

	// goals
	virtual void AddGoal( const MapGoalDef & goaldef );
	virtual void DeleteGoal( const char * goalname );
	virtual void UpdateEntity( GameEntity oldent, GameEntity newent );
};

#endif
