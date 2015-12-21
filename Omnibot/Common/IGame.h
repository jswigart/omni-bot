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

typedef std::shared_ptr<Client> ClientPtr;

struct NavParms
{
	float	AgentHeightStand;
	float	AgentHeightCrouch;
	float	AgentRadius;
	float	AgentMaxClimb;
	float	WalkableSlopeAngle;
	
	NavParms();
};

struct EntityInstance
{
	GameEntity				mEntity;	
	EntityInfo				mEntInfo;
	int						mTimeStamp;

	EntityInstance()
		: mTimeStamp( -1 )
	{
	}
};

struct GameVars
{
	int						mClientBase;
	float					mPlayerHeight;

	int						mGameVersion;
	std::string				mGameAbbrev;
	std::string				mGameName;
	std::string				mVersionString;

	bool					mRendersToGame;

	const std::string GetDLLName() const;
	const std::string GetModSubFolder() const;
	const std::string GetNavSubfolder() const;
	const std::string GetScriptSubfolder() const;

	GameVars();
};

// class: IGame
//		This class provides common functionality for various game types.
//		Specific games will derive from this class to expand and implement
//		their required functionality.
class IGame : public CommandReciever, public EventReciever
{
public:
	virtual bool Init( System & system );
	virtual void Shutdown();
	virtual void UpdateGame( System & system );
	virtual void StartGame();
	virtual void EndGame();
	virtual void NewRound();
	virtual void StartTraining();
	virtual void InitMapScript();

	virtual void ClientJoined( const EvClientConnected *_msg );
	virtual void ClientLeft( const EvClientDisconnected *_msg );

	static void InitTime();
	static void UpdateTime();

	static inline int32_t GetTime()
	{
		return mGameMsec;
	}
	static inline float GetTimeSecs()
	{
		return ( float )mGameMsec / 1000.f;
	}
	static inline int32_t GetDeltaTime()
	{
		return mDeltaMsec;
	}
	static inline float GetDeltaTimeSecs()
	{
		return ( float )mDeltaMsec * 0.001f;
	}
	static inline int32_t GetTimeSinceStart()
	{
		return mGameMsec - mStartTimeMsec;
	}
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

	void DispatchEvent( const Message &message, GameEntity dest = GameEntity() );
	
	const GameVars &GetGameVars() const;

	virtual bool CheckVersion( int version );
			
	virtual void GetNavParms( NavParms & navParms ) const;
	
	ClientPtr GetClientByEntity( GameEntity ent );
	ClientPtr GetClientByIndex( int index );

	bool FindTeamName( std::string& teamName, int teamId );
	bool FindClassName( std::string& groupName, std::string& className, const EntityGroup group, int classId );

	// Mod specific subclasses.
	virtual GoalManager * AllocGoalManager();
	virtual Client *CreateGameClient() = 0;

	virtual void GetWeaponEnumeration( const IntEnum *&ptr, int &num ) = 0;
	virtual void GetTeamEnumeration( const IntEnum *&ptr, int &num ) = 0;
	virtual void GetRoleEnumeration( const IntEnum *&ptr, int &num );

	virtual void AddBot( ParamsAddbot & parms, bool createnow = true );

	virtual void CheckServerSettings( bool managePlayers = true );

	inline bool DrawBlockableTests()
	{
		return mbDrawBlockableTests;
	}

	void LoadGoalScripts( bool clearold );

	virtual ClientPtr &GetClientFromCorrectedGameId( int gameid );

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

		EntityInstance &GetEnt();
		const EntityInstance &GetEnt() const;
		const int GetIndex() const;
		EntityIterator();
		EntityIterator( GameEntity initialEntity );
	private:
		EntityInstance	mCurrent;
		int				mIndex;
	};

	static bool IsEntityValid( const GameEntity & hndl );
	static GameEntity GetEntityByIndex( size_t index );
	static bool GetEntityInfo( const GameEntity & hndl, EntityInfo& entInfo );
	static bool IterateEntity( IGame::EntityIterator &_it );
	static void UpdateEntity( EntityInstance &_ent );

	void AddDeletedThread( int threadId );
	void PropogateDeletedThreads();

	virtual void InitGlobalStates()
	{
	}

	virtual bool AddWeaponId( const char * weaponName, int weaponId )
	{
		return false;
	}
	
	bool AddUpdateFunction( const std::string &_name, FunctorPtr _func );
	bool RemoveUpdateFunction( const std::string &_name );

	virtual bool GetAnalyticsKeys( GameAnalytics::Keys & keys )
	{
		return false;
	}

	IGame();
	virtual ~IGame();
protected:
	ClientPtr					mClientList[ Constants::MAX_PLAYERS ];

	State *						mStateRoot;

	static int					mMaxEntity;
	static EntityInstance		mGameEntities[ Constants::MAX_ENTITIES ];

	static GameState			mGameState;
	static GameState			mLastGameState;

	static int32_t				mGameMsec;
	static int32_t				mDeltaMsec;
	static int32_t				mStartTimeMsec;
	static int32_t				mGameFrame;
	static float				mGravity;
	static bool					mCheatsEnabled;
	static bool					mBotJoining;
	GameVars					mGameVars;

	FunctorMap					mUpdateMap;

	static const int MaxDeletedThreads = 1024;
	int							mDeletedThreads[ MaxDeletedThreads ];
	int							mNumDeletedThreads;
	
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
	bool UnhandledCommand( const StringVector & args );
	void cmdRevision( const StringVector & args );
	void cmdAddbot( const StringVector & args );
	void cmdKickbot( const StringVector & args );
	void cmdDebugBot( const StringVector & args );
	void cmdKickAll( const StringVector & args );
	void cmdBotDontShoot( const StringVector & args );
	void cmdDumpBlackboard( const StringVector & args );
	void cmdReloadWeaponDatabase( const StringVector & args );
	void cmdDrawBlockableTests( const StringVector & args );
	void cmdPrintFileSystem( const StringVector & args );
	void cmdTraceBenchmark( const StringVector & args );
	void cmdShowProcesses( const StringVector & args );
	void cmdStopProcess( const StringVector & args );

	// Server settings
	RegulatorPtr mSettingLimiter;
	bool		 mPlayersChanged;

	// Misc
	bool		 mbDrawBlockableTests;

	void ProcessEvent( const Message &message, CallbackParameters &_cb );

	void UpdateProcesses();
};

#endif
