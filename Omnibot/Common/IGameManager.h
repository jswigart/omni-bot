////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __IGAMEMANAGER_H__
#define __IGAMEMANAGER_H__

#include "System.h"
#include "CommandReciever.h"
#include "ObjFunctor.h"
#include "GameAnalytics.h"

#include <boost/thread.hpp>

// interface: IGameManager
//		Handles most of the common functionality of the game manager. Mods should subclass
//		this in order to create their own instance of IGame from within the CreateGameFromGameID
//		function
class IGameManager : public CommandReciever, public ErrorCallbacks
{
public:
	IGameManager();
	virtual ~IGameManager();
	
	/*
	Function: CreateGame
	Initializes the bot library based on the provided game type
	Parameters:
	_pEngineFuncs - The filled in struct of functions from the game
	_version - The version number we're attempting to load.
	Returns:
	omnibot_error BOT_ERROR_NONE if successful
	*/
	omnibot_error CreateGame(IEngineInterface *_pEngineFuncs, int _version);

	/*
	Function: Shutdown
	Shuts down the bot library and frees memory
	Returns:
	1 if successful, 0 if error.
	*/
	void Shutdown();

	/*
	Function: UpdateGame
	Updates the bot library and allows the bots to perform their AI.
	This should be called at regular intervals moderately often.
	*/
	void UpdateGame();

	static boost::thread::id sMainThread;

	// Callback Implementations
	void Error( const char * str );
protected:
	System				mBotSystem;
	
	virtual void InitCommands();
	void cmdVersion(const StringVector & args);
	void cmdNavSystem(const StringVector & args);
	void cmdPrintAllFiles(const StringVector & args);
	void cmdUpdateNavFile(const StringVector & args);
	void cmdUpdateAllNavFiles(const StringVector & args);
	void cmdSaveHeatMapScript(const StringVector & args);
	void cmdSaveAllHeatMapScript(const StringVector & args);
private:
	Remotery*			mRemotery;

	IGameManager &operator=(const IGameManager&);
};

#endif
