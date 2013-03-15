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

// interface: IGameManager
//		Handles most of the common functionality of the game manager. Mods should subclass
//		this in order to create their own instance of IGame from within the CreateGameFromGameID
//		function
class IGameManager : public CommandReciever
#ifdef ENABLE_REMOTE_DEBUGGING
	, public RemoteLib::ConnectionCallbacks
#endif
{
public:
	IGameManager();
	virtual ~IGameManager() {};

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

#ifdef ENABLE_REMOTE_DEBUGGING
	void SyncRemoteDelete( int entityHandle );
	void SyncRemoteMessage( const RemoteLib::DataBuffer & db );
#endif
protected:
	System				mBotSystem;

#ifdef ENABLE_REMOTE_DEBUGGING
	RemoteLib::ConnectionManagerServer	m_Remote;

	RemoteLib::Connection *			CreateNewConnection( class RemoteLib::TcpSocket & socket );
#endif

	virtual void InitCommands();
	void cmdVersion(const StringVector &_args);
	void cmdNavSystem(const StringVector &_args);
	void cmdPrintAllFiles(const StringVector &_args);
#ifdef ENABLE_FILE_DOWNLOADER
	void cmdUpdateNavFile(const StringVector &_args);
	void cmdUpdateAllNavFiles(const StringVector &_args);
#endif

	IGameManager &operator=(const IGameManager&);
};

void ThreadAddMessage(const std::string &_msg);
void ThreadAddError(const std::string &_msg);

#endif
