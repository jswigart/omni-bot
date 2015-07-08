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

#ifndef __OMNIBOT_H__
#define __OMNIBOT_H__

#include "Omni-Bot_Types.h"
#include "Omni-Bot_Events.h"
#include "MessageHelper.h"
#include "IEngineInterface.h"

class OmnibotFunctions : public OmnibotFunctionInterface
{
	virtual omnibot_error Initialize( IEngineInterface *_pEngineFuncs, int _version );

	virtual void Update();
	virtual void Shutdown();
	virtual void ConsoleCommand( const Arguments &args );

	virtual void SendTrigger( const TriggerInfo &_triggerInfo );
	virtual void AddBlackboardRecord( BlackBoard_Key _type, int _posterID, int _targetID, obUserData *_data );

	// events
	virtual void SendEvent( int _dest, const MessageHelper &_message );
	virtual void SendGlobalEvent( const MessageHelper &_message );
	
	virtual void EntityAdded( GameEntity ent, const EntityInfo& entInfo );
	virtual void EntityDestroyed( GameEntity ent );

	// goals
	virtual void AddGoal( const MapGoalDef &goaldef );
	virtual void DeleteGoal( const char *goalname );
	virtual void UpdateEntity( GameEntity oldent, GameEntity newent );
};

#endif
