////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __IGAME_H__
#define __IGAME_H__

#include "CommandReciever.h"
#include "EventReciever.h"
#include "Weapon.h"
#include "Base_Messages.h"
#include "PropertyBinding.h"
#include "Regulator.h"
#include "GameAnalytics.h"

class System;
class State;
class Client;
class CheckCriteria;
class Regulator;
class GoalManager;
class PathPlannerBase;
class Network;
class gmMachine;
class gmTableObject;
class gmFunctionObject;

typedef boost::shared_ptr<Client> ClientPtr;
typedef boost::weak_ptr<Client> ClientWPtr;

//typedef MessageDepot<Event_Sound,1024> SoundDepot;
//extern SoundDepot g_SoundDepot;

struct EntityInstance
{
	GameEntity					 mEntity;	
	EntityInfo				 mEntInfo;
	int							 mTimeStamp;
};

// class: IGame
//		This class provides common functionality for various game types.
//		Specific games will derive from this class to expand and implement
//		their required functionality.
class IGame : public CommandReciever, public EventReciever
{
public:
	struct GameVars
	{
		float	mPlayerHeight;
		GameVars();
	};
	static const GameVars &GetGameVars()
	{
		return mGameVars;
	}

	virtual bool Init( System & system );
	virtual void Shutdown();
	virtual void UpdateGame( System & system );
	virtual void StartGame();
	virtual void EndGame();
	virtual void NewRound();
	virtual void StartTraining();
	virtual void InitMapScript();

	virtual void ClientJoined( const Event_SystemClientConnected *_msg );
	virtual void ClientLeft( const Event_SystemClientDisConnected *_msg );

	void UpdateTime();
	static inline int32_t GetTime()
	{
		return mGameMsec;
	};
	static inline float GetTimeSecs()
	{
		return ( float )mGameMsec / 1000.f;
	};
	static inline int32_t GetDeltaTime()
	{
		return mDeltaMsec;
	};
	static inline float GetDeltaTimeSecs()
	{
		return ( float )mDeltaMsec * 0.001f;
	};
	static inline int32_t GetTimeSinceStart()
	{
		return mGameMsec - mStartTimeMsec;
	};
	static inline int32_t GetFrameNumber()
	{
		return mGameFrame;
	}
	static inline void SetTime( int32_t _newtime )
	{
		mGameMsec = _newtime;
	}
	static inline float GetGravity()
	{
		return mGravity;
	}
	static inline bool GetCheatsEnabled()
	{
		return mCheatsEnabled;
	}

	void DispatchEvent( int _dest, const MessageHelper &_message );
	void DispatchGlobalEvent( const MessageHelper &_message );

	virtual const char *GetVersion() const;
	virtual int GetVersionNum() const = 0;
	virtual bool CheckVersion( int _version );
	virtual const char *GetGameName() const = 0;
	virtual const char *GetDLLName() const = 0;
	virtual const char *GetModSubFolder() const = 0;
	virtual const char *GetNavSubfolder() const = 0;
	virtual const char *GetScriptSubfolder() const = 0;
	virtual const char *GetGameDatabaseAbbrev() const = 0;
	virtual NavigatorID GetDefaultNavigator() const
	{
		return NAVID_RECAST;
	}

	virtual bool RendersToGame() const
	{
		return false;
	}

	virtual bool ReadyForDebugWindow() const
	{
		return true;
	}

	ClientPtr GetClientByGameId( int _gameId );
	ClientPtr GetClientByIndex( int _index );

	void FindClassName( std::string& groupName, std::string& className, const EntityInfo& classInfo );

	// Mod specific subclasses.
	virtual GoalManager * AllocGoalManager();
	virtual Client *CreateGameClient() = 0;

	virtual void GetWeaponEnumeration( const IntEnum *&_ptr, int &num ) = 0;
	virtual void GetTeamEnumeration( const IntEnum *&_ptr, int &num ) = 0;
	virtual void GetRoleEnumeration( const IntEnum *&_ptr, int &num );

	virtual void AddBot( Msg_Addbot &_addbot, bool _createnow = true );

	virtual void CheckServerSettings( bool managePlayers = true );

	inline bool DrawBlockableTests()
	{
		return mbDrawBlockableTests;
	}

	void LoadGoalScripts( bool _clearold );

	virtual ClientPtr &GetClientFromCorrectedGameId( int _gameid );

	virtual bool CreateCriteria( gmThread *_thread, CheckCriteria &_criteria, std::stringstream &err );

	static GameState GetGameState()
	{
		return mGameState;
	}
	static GameState GetLastGameState()
	{
		return mLastGameState;
	}
	static bool GameStarted()
	{
		return mGameState != GAME_STATE_INVALID;
	}

	// Game Entity Stuff
	class EntityIterator
	{
	public:
		friend class IGame;
		operator bool() const;
		void Clear();
		EntityInstance &GetEnt()
		{
			return mCurrent;
		}
		const EntityInstance &GetEnt() const
		{
			return mCurrent;
		}
		const int GetIndex() const
		{
			return mIndex;
		}
		EntityIterator()
		{
		}
	private:
		EntityInstance	mCurrent;
		int				mIndex;
	};

	static bool IsEntityValid( const GameEntity &_hnl );
	static bool GetEntityInfo( const GameEntity &_hnl, EntityInfo& entInfo );
	static bool IterateEntity( IGame::EntityIterator &_it );
	static void UpdateEntity( EntityInstance &_ent );

	void AddDeletedThread( int threadId );
	void PropogateDeletedThreads();

	// Debug Window
	int GetDebugWindowNumClients() const;
	ClientPtr GetDebugWindowClient( int index ) const;

	virtual void InitGlobalStates()
	{
	}

	virtual bool AddWeaponId( const char * weaponName, int weaponId )
	{
		return false;
	}

	virtual const char * RemoteConfigName() const
	{
		return "Omnibot";
	}

#ifdef ENABLE_REMOTE_DEBUGGING
	void UpdateSync( RemoteLib::DebugConnection * connection, Remote::Game & cached, Remote::Game & update );
	void SyncEntity( const EntityInstance & ent, RemoteLib::DebugConnection * connection, Remote::Entity & cachedEntity, Remote::Entity & entityUpdate );
	virtual void InternalSyncEntity( const EntityInstance & ent, RemoteLib::DebugConnection * connection );
#endif

	bool AddUpdateFunction( const std::string &_name, FunctorPtr _func );
	bool RemoveUpdateFunction( const std::string &_name );

	virtual bool GetAnalyticsKeys( GameAnalyticsKeys & keys )
	{
		return false;
	}

	IGame();
	virtual ~IGame();
protected:
	ClientPtr		 mClientList[ Constants::MAX_PLAYERS ];

	State				* mStateRoot;

	static int				 mMaxEntity;
	static EntityInstance	 mGameEntities[ Constants::MAX_ENTITIES ];

	static GameState mGameState;
	static GameState mLastGameState;

	static int32_t	 mGameMsec;
	static int32_t	 mDeltaMsec;
	static int32_t	 mStartTimeMsec;
	static int32_t	 mGameFrame;
	static float	 mGravity;
	static bool		 mCheatsEnabled;
	static bool		 mBotJoining;

	static GameVars	 mGameVars;

	FunctorMap		 mUpdateMap;

	enum
	{
		MaxDeletedThreads = 1024
	};
	int		 mDeletedThreads[ MaxDeletedThreads ];
	int		 mNumDeletedThreads;

	virtual void GetGameVars( GameVars &_gamevars ) = 0;

	void CheckGameState();

	// Script support.
	void InitScriptSupport();
	virtual void InitScriptBinds( gmMachine *_machine )
	{
	}
	virtual void InitScriptTeams( gmMachine *_machine, gmTableObject *_table );
	virtual void InitScriptWeapons( gmMachine *_machine, gmTableObject *_table );
	virtual void InitScriptRoles( gmMachine *_machine, gmTableObject *_table );
	virtual void InitScriptGroups( gmMachine *_machine, gmTableObject *_table );
	virtual void InitScriptClasses( gmMachine *_machine, gmTableObject *_table );
	virtual void InitScriptWeaponClasses( gmMachine *_machine, gmTableObject *_table );
	virtual void InitScriptSkills( gmMachine *_machine, gmTableObject *_table );
	virtual void InitScriptItems( gmMachine *_machine, gmTableObject *_table )
	{
	};
	virtual void InitScriptEvents( gmMachine *_machine, gmTableObject *_table );
	virtual void InitScriptEntityFlags( gmMachine *_machine, gmTableObject *_table );
	virtual void InitScriptPowerups( gmMachine *_machine, gmTableObject *_table );
	virtual void InitScriptCategories( gmMachine *_machine, gmTableObject *_table );
	virtual void InitScriptBotButtons( gmMachine *_machine, gmTableObject *_table );
	virtual void InitScriptTraceMasks( gmMachine *_machine, gmTableObject *_table );
	virtual void InitScriptContentFlags( gmMachine *_machine, gmTableObject *_table );
	virtual void InitScriptBlackboardKeys( gmMachine *_machine, gmTableObject *_table );
	virtual void InitScriptBuyMenu( gmMachine *_machine, gmTableObject *_table )
	{
	};
	virtual void InitDebugFlags( gmMachine *_machine, gmTableObject *_table );
	virtual void InitBoneIds( gmMachine *_machine, gmTableObject *_table );

	virtual void InitVoiceMacros( gmMachine *_machine, gmTableObject *_table )
	{
	};

	// Commands
	virtual void InitCommands();
	bool UnhandledCommand( const StringVector &_args );
	void cmdRevision( const StringVector &_args );
	void cmdAddbot( const StringVector &_args );
	void cmdKickbot( const StringVector &_args );
	void cmdDebugBot( const StringVector &_args );
	void cmdKickAll( const StringVector &_args );
	void cmdBotDontShoot( const StringVector &_args );
	void cmdDumpBlackboard( const StringVector &_args );
	void cmdReloadWeaponDatabase( const StringVector &_args );
	void cmdDrawBlockableTests( const StringVector &_args );
	void cmdPrintFileSystem( const StringVector &_args );
	void cmdTraceBenchmark( const StringVector &_args );
	void cmdShowProcesses( const StringVector &_args );
	void cmdStopProcess( const StringVector &_args );

	// Server settings
	RegulatorPtr mSettingLimiter;
	bool		 mPlayersChanged;

	// Misc
	bool		 mbDrawBlockableTests;

	void ProcessEvent( const MessageHelper &_message, CallbackParameters &_cb );

	void UpdateProcesses();
};

#endif
