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

	omnibot_error CreateGame( IEngineInterface * engineFuncs, int version );

	void Shutdown();

	void UpdateGame();

	static boost::thread::id sMainThread;

	// Callback Implementations
	void Error( const char * str );
protected:
	System				mBotSystem;

	virtual void InitCommands();
	void cmdVersion( const StringVector & args );
	void cmdNavSystem( const StringVector & args );
	void cmdPrintAllFiles( const StringVector & args );
	void cmdUpdateNavFile( const StringVector & args );
	void cmdUpdateAllNavFiles( const StringVector & args );
	void cmdSaveHeatMapScript( const StringVector & args );
	void cmdSaveAllHeatMapScript( const StringVector & args );
private:
	Remotery*			mRemotery;

	IGameManager &operator=( const IGameManager& );
};

#endif
