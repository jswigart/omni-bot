////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __BASE_MESSAGES_H__
#define __BASE_MESSAGES_H__

#include "Omni-Bot_Types.h"
#include "Omni-Bot_BitFlags.h"

#pragma pack(push)
#pragma pack(4)

//////////////////////////////////////////////////////////////////////////

struct Msg_Addbot
{
	int			mTeam;
	int			mClass;
	char		mName[64];
	char		mModel[64];
	char		mSkin[64];
	char		mSpawnPointName[64];
	char		mProfile[64];

	Msg_Addbot()
		: mTeam(RANDOM_TEAM_IF_NO_TEAM)
		, mClass(RANDOM_CLASS_IF_NO_CLASS)
	{
		mName[0] = mModel[0] = mSkin[0] = mSpawnPointName[0] = mProfile[0] = 0;
	}
};

struct Msg_Kickbot
{
	enum { BufferSize = 64, InvalidGameId = -1 };
	char		mName[BufferSize];
	int			mGameId;

	Msg_Kickbot()
		: mGameId(InvalidGameId)
	{
		mName[0] =  0;
	}
};

struct Msg_ChangeName
{
	char	mNewName[64];
};

struct Msg_PlayerChooseEquipment
{
	enum { NumItems = 16 };
	int			mWeaponChoice[NumItems];
	int			mItemChoice[NumItems];

	Msg_PlayerChooseEquipment()
	{
		for(int i = 0; i < NumItems; ++i)
		{
			mWeaponChoice[i] = 0;
			mItemChoice[i] = 0;
		}
	}
};

struct Msg_PlayerMaxSpeed
{
	float		mMaxSpeed;

	Msg_PlayerMaxSpeed() : mMaxSpeed(0.f) {}
};

struct Msg_IsAlive
{
	obBool		mIsAlive;

	Msg_IsAlive() : mIsAlive(False) {}
};

struct Msg_IsAllied
{
	GameEntity	mTargetEntity;
	obBool		mIsAllied;

	Msg_IsAllied(GameEntity e) : mTargetEntity(e), mIsAllied(True) {}
};

struct Msg_IsOutside
{
	float		mPosition[3];
	obBool		mIsOutside;

	Msg_IsOutside()
		: mIsOutside(False)
	{
		mPosition[0] = mPosition[1] = mPosition[2] = 0.f;
	}
};

struct Msg_PointContents
{
	int			mContents;
	float		x,y,z;

	Msg_PointContents()
		: mContents(0)
		, x(0.f)
		, y(0.f)
		, z(0.f)
	{
	}
};

struct Msg_ReadyToFire
{
	obBool		mReady;

	Msg_ReadyToFire() : mReady(False) {}
};

struct Msg_Reloading
{
	obBool		mReloading;

	Msg_Reloading() : mReloading(False) {}
};

struct Msg_FlagState
{
	FlagState	mFlagState;
	GameEntity	mOwner;

	Msg_FlagState()
		: mFlagState(S_FLAG_NOT_A_FLAG)
	{
	}
};

struct Msg_GameState
{
	GameState	mGameState;
	float		mTimeLeft;

	Msg_GameState()
		: mGameState(GAME_STATE_INVALID)
		, mTimeLeft(0.f)
	{
	}
};

struct Msg_EntityStat
{
	char		mStatName[64];
	obUserData	mResult;

	Msg_EntityStat()
	{
		mStatName[0] = 0;
	}
};

struct Msg_TeamStat
{
	int			mTeam;
	char		mStatName[64];
	obUserData	mResult;

	Msg_TeamStat()
		: mTeam(0)
	{
		mStatName[0] = 0;
	}
};

struct Msg_ServerCommand
{
	char		mCommand[256];

	Msg_ServerCommand()
	{
		mCommand[0] = 0;
	}
};

struct WeaponCharged
{
	int			mWeapon;
	FireMode	mFireMode;
	obBool		mIsCharged;
	obBool		mIsCharging;

	WeaponCharged(int w = 0, FireMode m = Primary)
		: mWeapon(w)
		, mFireMode(m)
		, mIsCharged(False)
		, mIsCharging(False)
	{
	}
};

struct WeaponHeatLevel
{
	FireMode	mFireMode;
	float		mCurrentHeat;
	float		mMaxHeat;

	WeaponHeatLevel(FireMode m = Primary)
		: mFireMode(m)
		, mCurrentHeat(0.f)
		, mMaxHeat(0.f)
	{
	}
};

struct VehicleInfo
{
	int			mType;
	GameEntity	mEntity;
	GameEntity	mWeapon;
	GameEntity	mDriver;
	int			mVehicleHealth;
	int			mVehicleMaxHealth;
	float		mArmor;

	VehicleInfo()
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

struct ControllingTeam
{
	int		mControllingTeam;

	ControllingTeam() : mControllingTeam(0) {}
};

struct WeaponStatus
{
	int			mWeaponId;
	//FireMode	mFireMode;

	WeaponStatus() : mWeaponId(0) {}

	bool operator==(const WeaponStatus &_w2)
	{
		return mWeaponId == _w2.mWeaponId;
	}
	bool operator!=(const WeaponStatus &_w2)
	{
		return !(*this == _w2);
	}
};

struct WeaponLimits
{
	float		mCenterFacing[3];
	float		mMinYaw;
	float		mMaxYaw;
	float		mMinPitch;
	float		mMaxPitch;
	int			mWeaponId;
	obBool		mLimited;

	WeaponLimits()
		: mMinYaw(-45.f)
		, mMaxYaw( 45.f)
		, mMinPitch(-20.f)
		, mMaxPitch( 20.f)
		, mWeaponId(0)
		, mLimited(False)
	{
		mCenterFacing[0] = 0.f;
		mCenterFacing[1] = 0.f;
		mCenterFacing[2] = 0.f;
	}
};

struct Msg_KillEntity
{
	GameEntity	mWhoToKill;
};

struct Event_PlaySound
{
	char		mSoundName[128];
};

struct Event_StopSound
{
	char		mSoundName[128];
};

struct Event_ScriptEvent
{
	char		mFunctionName[64];
	char		mEntityName[64];
	char		mParam1[64];
	char		mParam2[64];
	char		mParam3[64];
};

struct Msg_GotoWaypoint
{
	char		mWaypointName[64];
	float		mOrigin[3];

	Msg_GotoWaypoint()
	{
		mOrigin[0] = 0.f;
		mOrigin[1] = 0.f;
		mOrigin[2] = 0.f;
		mWaypointName[0] = 0;
	}
};

struct Msg_MoverAt
{
	float		mPosition[3];
	float		mUnder[3];

	GameEntity mEntity;

	Msg_MoverAt()
	{
		mPosition[0]=mPosition[1]=mPosition[2]=0.f;
		mUnder[0]=mUnder[1]=mUnder[2]=0.f;
	}
};

//////////////////////////////////////////////////////////////////////////
// Events

struct Event_SystemScriptUpdated
{
	int32_t		mScriptKey;
};

struct Event_SystemClientConnected
{
	int			mGameId;
	obBool		mIsBot;
	int			mDesiredClass;
	int			mDesiredTeam;

	Event_SystemClientConnected()
		: mGameId(-1)
		, mIsBot(False)
		, mDesiredClass(RANDOM_CLASS_IF_NO_CLASS)
		, mDesiredTeam(RANDOM_TEAM_IF_NO_TEAM)
	{
	}
};

struct Event_SystemClientDisConnected
{
	int			mGameId;
};

struct Event_SystemGravity
{
	float		mGravity;
};

struct Event_SystemCheats
{
	obBool	 mEnabled;
};

struct Event_EntityCreated
{
	GameEntity			mEntity;
	EntityInfo 	mEntityInfo;
};

struct Event_EntityDeleted
{
	GameEntity		mEntity;
};

//////////////////////////////////////////////////////////////////////////

struct Event_Death
{
	GameEntity	mWhoKilledMe;
	char		mMeansOfDeath[32];
};

struct Event_KilledSomeone
{
	GameEntity	mWhoIKilled;
	char		mMeansOfDeath[32];
};

struct Event_TakeDamage
{
	GameEntity	mInflictor;
	char		mDamageType[32];
};

struct Event_Healed
{
	GameEntity	mWhoHealedMe;
};

struct Event_Revived
{
	GameEntity	mWhoRevivedMe;
};

struct Event_ChangeTeam
{
	int			mNewTeam;
};

struct Event_WeaponChanged
{
	int			mWeaponId;
};

struct Event_ChangeClass
{
	int			mNewClass;
};

struct Event_Spectated
{
	int			mWhoSpectatingMe;
};

struct Event_AddWeapon
{
	int			mWeaponId;
};

struct Event_RemoveWeapon
{
	int			mWeaponId;
};

struct Event_RefreshWeapon
{
	int32_t		mWeaponId;
};

struct Event_WeaponFire
{
	int			mWeaponId;
	FireMode	mFireMode;
	GameEntity	mProjectile;
};

struct Event_WeaponChange
{
	int			mWeaponId;
};

struct Event_ChatMessage
{
	GameEntity	mWhoSaidIt;
	char		mMessage[512];
};

struct Event_VoiceMacro
{
	GameEntity	mWhoSaidIt;
	char		mMacroString[64];
};

struct Event_PlayerUsed
{
	GameEntity	mWhoDidIt;
};

struct Event_Sound
{
	char		mSoundName[64];
	float		mOrigin[3];
	GameEntity	mSource;
	int			mSoundType;
};

struct Event_EntitySensed
{
	int			mEntityClass;
	GameEntity	mEntity;
};

struct Event_DynamicPathsChanged
{
	int			mTeamMask;
	int			mNavId;

	Event_DynamicPathsChanged(int _team, int _navid = 0)
		: mTeamMask(_team)
		, mNavId(_navid)
	{
	}
};

struct Event_ScriptMessage
{
	char		mMessageName[64];
	char		mMessageData1[64];
	char		mMessageData2[64];
	char		mMessageData3[64];
};

struct Event_ScriptSignal
{
	char		mSignalName[64];
};

struct Event_EntityConnection
{
	GameEntity	mEntitySrc;
	GameEntity	mEntityDst;

	/*float		mSrcPos[3];
	float		mDstPos[3];*/

	BitFlag32	mTeam;
	float		mRadius;
	float		mCost;

	Event_EntityConnection()
		: mRadius(0.f)
		, mCost(0.f)
	{
		/*for ( int i = 0; i < 3; ++i )
			mSrcPos[ i ] = mDstPos[ i ] = 0.0f;*/
	}
};

struct Event_EntEnterRadius
{
	GameEntity	mEntity;
};

struct Event_EntLeaveRadius
{
	GameEntity	mEntity;
};

struct Event_GameType
{
	int			mGameType;
};

struct Event_CvarSet
{
	const char *		mCvar;
	const char *		mValue;
};

struct Event_CvarGet
{
	const char *		mCvar;
	int					mValue;
};

struct Event_Analytics
{
	const char *		mName;
	const char *		mArea;
	float				mValue;
	float				mPosition[3];
	bool				mHasValue;
	bool				mHasPosition;
};

#pragma pack(pop)

#endif
