////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __ENGINE_INTERFACE_H__
#define __ENGINE_INTERFACE_H__

#include <malloc.h>
#include <memory.h>

#include "Omni-Bot_Types.h"
#include "Omni-Bot_BitFlags.h"
#include "Omni-Bot_Color.h"

class Message;

// Title: Engine Interface

// struct: ClientInput
typedef struct
{
	// float: mFacing
	//		The direction the bot is facing/aiming
	float		mFacing[ 3 ];
	// float: mMoveDir
	//		The direction the bot is moving
	float		mMoveDir[ 3 ];
	// int: mButtonFlags
	//		64 bit int of bits representing bot keypresses, see <ButtonFlags>
	BitFlag64	mButtonFlags;
	// int: mCurrentWeapon
	//		The current weapon Id this bot wants to use.
	int			mCurrentWeapon;
} ClientInput;

// class: obTraceResult
//		This file defines all the common structures used by the game and bot alike.
class obTraceResult
{
public:
	// float: mFraction
	//		0.0 - 1.0 how far the trace went
	float		mFraction;
	// float: mNormal
	//		The plane normal that was struck
	float		mNormal[ 3 ];
	// float: mEndpos
	//		The end point the trace ended at
	float		mEndpos[ 3 ];
	// var: mHitEntity
	//		The entity that was hit by the trace
	GameEntity	mHitEntity;
	// int: mStartSolid
	//		Did the trace start inside a solid?
	int			mStartSolid;
	// int: mContents
	//		Content flags.
	int			mContents;
	// int: mSurface;
	//		Flags representing the surface struct by the trace.
	int			mSurface;

	obTraceResult() :
		mFraction( 0.f ),
		mStartSolid( 0 ),
		mContents( 0 ),
		mSurface( 0 )
	{
	}
};

class obPlayerInfo
{
public:
	enum
	{
		MaxPlayers = 64, MaxTeams = 6
	};
	enum Controller
	{
		Bot = 0, Human, Both
	};
	struct PInfo
	{
		EntityInfo			mEntInfo;
		int					mTeam;
		Controller			mController;

		PInfo()
		{
			mTeam = OB_TEAM_NONE;
			mController = Bot;
		}
	};

	PInfo		mPlayers[ MaxPlayers ];
	int			mMaxPlayers;
	int			mAvailableTeams;

	int GetNumPlayers( int t = OB_TEAM_ALL, Controller c = Both ) const
	{
		int n = 0;
		for ( int i = 0; i < MaxPlayers; ++i )
		{
			if ( mPlayers[ i ].mTeam == 0 )
				continue;
			if ( t != OB_TEAM_ALL&&mPlayers[ i ].mTeam != t )
				continue;
			if ( c != Both &&mPlayers[ i ].mController != c )
				continue;
			++n;
		}
		return n;
	}
	int GetMaxPlayers() const
	{
		return mMaxPlayers;
	}
	int GetAvailableTeams() const
	{
		return mAvailableTeams;
	}
	obPlayerInfo()
	{
		for ( int i = 0; i < MaxPlayers; ++i )
			mPlayers[ i ] = PInfo();
		mAvailableTeams = 0;
		mMaxPlayers = 0;
	}
};

class MemoryAllocator
{
public:
	virtual char * AllocateMemory( unsigned int numBytes ) = 0;
	virtual void FreeMemory( void * ptr ) = 0;
};

class DefaultMemoryAllocator : public MemoryAllocator
{
public:
	virtual char * AllocateMemory( unsigned int numBytes )
	{
		return (char *)malloc( numBytes );
	}
	virtual void FreeMemory( void * ptr )
	{
		free( ptr );
	}
};

struct GameModelInfo
{
	enum
	{
		TypeBuferSize = 16,
		NameBufferSize = 128,
	};
	char 			mModelType[ TypeBuferSize ];
	char 			mModelName[ NameBufferSize ];
	char*			mDataBuffer;
	int				mDataBufferSize;
	float			mScale[ 3 ];

	// oriented bounding box
	AABB			mAABB;

	GameModelInfo()
		: mDataBuffer( 0 )
		, mDataBufferSize( 0 )
	{
		mModelName[ 0 ] = 0;
		mModelType[ 0 ] = 0;

		for ( int i = 0; i < 3; ++i )
		{
			mScale[ i ] = 1.0f;
		}

		mAABB.Set( 0.f, 0.f, 0.f );
	}
};

struct obStringBuffer
{
	enum
	{
		BUFFER_LENGTH = 64
	};
	char	mBuffer[ BUFFER_LENGTH ];

	obStringBuffer()
	{
		memset( mBuffer, 0, sizeof( mBuffer ) );
	}
};

struct ParamsAddbot
{
	int			mTeam;
	int			mClass;
	char		mName[ 64 ];
	char		mModel[ 64 ];
	char		mSkin[ 64 ];
	char		mSpawnPointName[ 64 ];
	char		mProfile[ 64 ];

	ParamsAddbot()
		: mTeam( RANDOM_TEAM_IF_NO_TEAM )
		, mClass( RANDOM_CLASS_IF_NO_CLASS )
	{
		mName[ 0 ] = mModel[ 0 ] = mSkin[ 0 ] = mSpawnPointName[ 0 ] = mProfile[ 0 ] = 0;
	}
};

struct ParamsKickbot
{
	enum
	{
		BufferSize = 64, InvalidGameId = -1
	};
	char		mName[ BufferSize ];
	int			mGameId;

	ParamsKickbot()
		: mGameId( InvalidGameId )
	{
		mName[ 0 ] = 0;
	}
};

struct ParamsWeaponLimits
{
	float		mCenterFacing[ 3 ];
	float		mMinYaw;
	float		mMaxYaw;
	float		mMinPitch;
	float		mMaxPitch;
	int			mWeaponId;
	bool		mLimited;

	ParamsWeaponLimits()
		: mMinYaw( -45.f )
		, mMaxYaw( 45.f )
		, mMinPitch( -20.f )
		, mMaxPitch( 20.f )
		, mWeaponId( 0 )
		, mLimited( false )
	{
		mCenterFacing[ 0 ] = 0.f;
		mCenterFacing[ 1 ] = 0.f;
		mCenterFacing[ 2 ] = 0.f;
	}
};

struct ParamsVehicleInfo
{
	int			mType;
	GameEntity	mEntity;
	GameEntity	mWeapon;
	GameEntity	mDriver;
	int			mVehicleHealth;
	int			mVehicleMaxHealth;
	float		mArmor;

	ParamsVehicleInfo()
	{
		mType = 0;
		mEntity = GameEntity();
		mWeapon = GameEntity();
		mDriver = GameEntity();
		mVehicleHealth = 0;
		mVehicleMaxHealth = 0;
		mArmor = 0.f;
	}
};

// typedef: IEngineInterface
//		This struct defines all the function that the game will implement
//		and give to the bot so that the bot may perform generic
//		actions without caring about the underlying engine or game.
class IEngineInterface
{
public:
	enum DebugRenderFlags
	{
		DR_NODEPTHTEST = ( 1 << 0 ),
	};
	enum ConvertType
	{
		ConvertSurfaceFlags,
		ConvertContentsFlags,
	};
	enum ConvertDirection
	{
		ConvertBotToGame,
		ConvertGameToBot,
	};

	// Function: AddBot
	virtual int AddBot( const ParamsAddbot & data ) = 0;

	// Function: RemoveBot
	virtual void RemoveBot( const ParamsKickbot & data ) = 0;

	// Function: ChangeTeam
	virtual obResult ChangeTeam( int client, int newteam, const Message * data ) = 0;

	// Function: ChangeClass
	virtual obResult ChangeClass( int client, int newclass, const Message * data ) = 0;

	// Function: UpdateBotInput
	virtual void UpdateBotInput( int client, const ClientInput &_input ) = 0;

	// Function: BotCommand
	virtual void BotCommand( int client, const char *cmd ) = 0;

	// Function: IsInPVS
	virtual bool IsInPVS( const float pos[ 3 ], const float _target[ 3 ] ) = 0;

	// Function: TraceLine
	virtual obResult TraceLine( obTraceResult &_result, const float _start[ 3 ], const float _end[ 3 ], const AABB *_pBBox, int _mask, int _user, bool _bUsePVS ) = 0;

	// Function: GetPointContents
	virtual int GetPointContents( const float pos[ 3 ] ) = 0;

	// Function: GetSurfaceFromGame
	virtual int ConvertValue( int value, ConvertType ctype, ConvertDirection cdir ) = 0;

	// Function: GetLocalGameEntity
	virtual GameEntity GetLocalGameEntity() = 0;

	// Function: GetEntityInfo
	virtual obResult GetEntityInfo( const GameEntity ent, EntityInfo& classInfo ) = 0;

	// Function: GetEntityEyePosition
	virtual obResult GetEntityEyePosition( const GameEntity ent, float pos[ 3 ] ) = 0;

	// Function: GetEntityEyePosition
	virtual obResult GetEntityBonePosition( const GameEntity ent, int _boneid, float pos[ 3 ] ) = 0;

	// Function: GetEntityOrientation
	virtual obResult GetEntityOrientation( const GameEntity ent, float _fwd[ 3 ], float _right[ 3 ], float _up[ 3 ] ) = 0;

	// Function: GetEntityVelocity
	virtual obResult GetEntityVelocity( const GameEntity ent, float _velocity[ 3 ] ) = 0;

	// Function: GetEntityPosition
	virtual obResult GetEntityPosition( const GameEntity ent, float pos[ 3 ] ) = 0;

	// Function: GetEntityLocalAABB
	virtual obResult GetEntityLocalAABB( const GameEntity ent, AABB &_aabb ) = 0;

	// Function: GetEntityWorldOBB
	virtual obResult GetEntityWorldOBB( const GameEntity ent, float *_center, float *_axis0, float *_axis1, float *_axis2, float *_extents ) = 0;

	// Function: GetEntityGroundEntity
	virtual obResult GetEntityGroundEntity( const GameEntity ent, GameEntity &moveent ) = 0;

	// Function: GetEntityOwner
	virtual GameEntity GetEntityOwner( const GameEntity ent ) = 0;

	// Function: GetEntityTeam
	virtual int GetEntityTeam( const GameEntity ent ) = 0;
	
	// Function: GetEntityName
	virtual obResult GetEntityName( const GameEntity ent, obStringBuffer& nameOut ) = 0;

	// Function: GetEntityForName
	virtual obResult GetEntityForName( const char* entName, GameEntity& entityOut ) = 0;

	// Function: GetEntityForMapModel
	virtual obResult GetEntityForMapModel( int mapModelId, GameEntity& entityOut ) = 0;

	// Function: GetEntityModel
	virtual obResult GetEntityModel( const GameEntity ent, GameModelInfo & modelOut, MemoryAllocator & alloc ) = 0;

	// Function: GetWorldModel
	virtual obResult GetWorldModel( GameModelInfo & modelOut, MemoryAllocator & alloc ) = 0;

	// Function: GetModel
	virtual obResult GetModel( GameModelInfo & modelOut, MemoryAllocator & alloc ) = 0;

	// Function: GetCurrentWeapons
	virtual int GetCurrentWeapons( const GameEntity ent, int weaponIds [], int maxWeapons ) = 0;

	// Function: GetCurrentWeaponClip
	virtual obResult GetCurrentWeaponClip( const GameEntity ent, FireMode _mode, int &_curclip, int &_maxclip ) = 0;

	// Function: BotGetCurrentAmmo
	virtual obResult GetCurrentAmmo( const GameEntity ent, int _weaponId, FireMode _mode, int &_cur, int &_max ) = 0;

	// Function: GetGameTime
	virtual int GetGameTime() = 0;

	// Function: GetGoals
	virtual void GetGoals() = 0;

	// Function: GetPlayerInfo
	virtual void GetPlayerInfo( obPlayerInfo &info ) = 0;
	
	// Function: DebugLine
	virtual bool DebugLine( const float _start[ 3 ], const float _end[ 3 ], const obColor &_color, float _time )
	{
		_start; _end; _color; _time; return false;
	}

	// Function: DebugBox
	virtual bool DebugBox( const float _mins[ 3 ], const float _maxs[ 3 ], const obColor &_color, float _time )
	{
		_mins; _maxs; _color; _time; return false;
	}

	// Function: DebugArrow
	virtual bool DebugArrow( const float _start[ 3 ], const float _end[ 3 ], const obColor &_color, float _time )
	{
		return DebugLine( _start, _end, _color, _time );
	}

	// Function: DebugRadius
	virtual bool DebugRadius( const float pos[ 3 ], const float _radius, const obColor &_color, float _time )
	{
		pos; _radius; _color; _time; return false;
	}

	// Function: DebugPolygon
	virtual bool DebugPolygon( const obVec3 *_verts, const int _numverts, const obColor &_color, float _time, int _flags )
	{
		_verts; _numverts; _color; _time; _flags; return false;
	}

	// Function: PrintScreenMessage
	virtual bool PrintScreenText( const float pos[ 3 ], float _duration, const obColor &_color, const char *_msg )
	{
		pos; _duration; _color; _msg; return false;
	}

	// Function: PrintError
	virtual void PrintError( const char *_error ) = 0;

	// Function: PrintMessage
	virtual void PrintMessage( const char *_msg ) = 0;

	// Function: GetMapName
	virtual const char *GetMapName() = 0;

	// Function: GetMapExtents
	virtual void GetMapExtents( AABB &_aabb ) = 0;

	// Function: EntityFromID
	virtual GameEntity EntityFromID( const int _gameId ) = 0;

	// Function: EntityByName
	virtual GameEntity EntityByName( const char *_name ) = 0;

	// Function: IDFromEntity
	virtual int IDFromEntity( const GameEntity ent ) = 0;
	
	// Function: GetAutoNavFeatures
	virtual int GetAutoNavFeatures( AutoNavFeature *_feature, int _max ) = 0;

	// Function: GetGameName
	virtual const char *GetGameName() = 0;

	// Function: GetModName
	virtual const char *GetModName() = 0;

	// Function: GetModVers
	virtual const char *GetModVers() = 0;

	// Function: GetBotPath
	virtual const char *GetBotPath() = 0;

	// Function: GetLogPath
	virtual const char *GetLogPath() = 0;

	virtual bool AreCheatsEnabled() = 0;

	virtual bool GetGravity( float& outGravity ) = 0;

	virtual bool IsAlive( const GameEntity ent ) = 0;

	virtual bool IsAllied( const GameEntity ent1, const GameEntity ent2 ) = 0;

	virtual bool IsOutSide( const float pos[3] ) = 0;

	virtual bool IsReadyToFire( const GameEntity ent ) = 0;

	virtual bool IsReloading( const GameEntity ent ) = 0;

	virtual bool IsWeaponCharged( const GameEntity ent, int weaponId, FireMode _mode ) = 0;

	virtual bool GetMaxSpeed( const GameEntity ent, float & outSpeed ) = 0;

	virtual bool GetEquippedWeapon( const GameEntity ent, int& outWeaponId, FireMode& outFireMode ) = 0;

	virtual bool GetMountedWeapon( const GameEntity ent, int& outWeaponId, FireMode& outFireMode ) = 0;
	
	virtual bool GetWeaponLimits( const GameEntity ent, int weaponId, ParamsWeaponLimits& outLimits ) = 0;

	virtual bool GetWeaponHeat( const GameEntity ent, int weaponId, FireMode firemode, float& curheat, float& maxheat ) = 0;

	virtual bool GetFlagState( const GameEntity ent, FlagState& outState, GameEntity& outCarrier ) = 0;

	virtual int GetControllingTeam( const GameEntity ent ) = 0;
	
	virtual int GetGameType() const = 0;
	virtual int GetGameTimeLeft() const = 0;
	virtual GameState GetGameState() const = 0;

	virtual bool ChangeName( const GameEntity ent, const char* newName ) = 0;

	virtual bool Suicide( const GameEntity ent ) = 0;

	virtual bool PlaySound( const GameEntity ent, const char* name ) = 0;

	virtual bool StopSound( const GameEntity ent, const char* name ) = 0;

	virtual bool GetVehicleInfo( const GameEntity ent, ParamsVehicleInfo& outVehicleInfo ) = 0;
	
	virtual bool IsMoverAt( const float pos1[ 3 ], const float pos2[ 3 ], GameEntity& outMover ) = 0;

	virtual bool ServerScriptEvent( const char* func, const char* entName, const obUserData params [], size_t numParams ) = 0;

	virtual bool ServerCommand( const char* cmd ) = 0;

	virtual bool ServerSetVariable( const char* name, const char* value ) = 0;

	virtual bool ServerGetVariable( const char* name, char* value, size_t valueMaxLen ) = 0;

	virtual bool GetLocationName( const float pos[ 3 ], char buffer[], size_t bufferSize ) = 0;

	IEngineInterface() { }
	virtual ~IEngineInterface() { }
};

class OmnibotFunctionInterface
{
public:
	enum Version
	{
		VERSION_0,
		VERSION_1,
		VERSION_LATEST,
	};

	virtual omnibot_error Initialize( IEngineInterface *_pEngineFuncs, int _version ) = 0;

	virtual void Update() = 0;
	virtual void Shutdown() = 0;
	virtual void ConsoleCommand( const Arguments &args ) = 0;

	virtual void SendTrigger( const TriggerInfo &_triggerInfo ) = 0;
	virtual void AddBlackboardRecord( BlackBoardKey _type, int posterID, int _targetID, obUserData *data ) = 0;

	// events
	virtual void SendEvent( const Message & message, GameEntity dest ) = 0;

	virtual void EntityAdded( GameEntity ent, const EntityInfo& entInfo ) = 0;
	virtual void EntityDestroyed( GameEntity ent ) = 0;

	// goals
	virtual void AddGoal( const MapGoalDef &goaldef ) = 0;
	virtual void DeleteGoal( const char *goalname ) = 0;
	virtual void UpdateEntity( GameEntity oldent, GameEntity newent ) = 0;

	OmnibotFunctionInterface()
	{
	}
	virtual ~OmnibotFunctionInterface()
	{
	}
};

#endif
