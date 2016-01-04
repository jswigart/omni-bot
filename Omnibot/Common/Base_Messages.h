////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __BASE_MESSAGES_H__
#define __BASE_MESSAGES_H__

#include "Omni-Bot_Events.h"
#include "Omni-Bot_Types.h"
#include "Omni-Bot_BitFlags.h"

// typedef: EventId
//		Readable identifier for various events that can be sent to the bot
//		and considered for state changes or behavioral modifications.
enum MessageId
{
	MSG_NONE = 0,

	MSG_GETCONTROLLINGTEAM,
	MSG_GAMESTATE,

	MSG_ENTITYSTAT,
	MSG_TEAMSTAT,

	MSG_WEAPON_CHARGED,
	MSG_WEAPON_HEATLEVEL,

	MSG_SERVERCOMMAND,
	MSG_PLAYSOUND,
	MSG_STOPSOUND,
	MSG_SCRIPTEVENT,

	MSG_GOTOWAYPOINT,
	MSG_MOVERAT,

	MSG_SETLOADOUT,

	MSG_SCRIPT_CHANGED,

	MSG_STARTGAME,
	MSG_ENDGAME,
	MSG_NEWROUND,
	MSG_ENDROUND,
	MSG_CLIENTCONNECTED,
	MSG_CLIENTDISCONNECTED,
	MSG_ENTITYCREATED,
	MSG_ENTITYDELETED,
	MSG_START_TRAINING,
	MSG_SCRIPTSIGNAL,
	MSG_SOUND,
	MSG_ADD_ENTITY_CONNECTION,
	MSG_ANALYTIC_EVENT,

	MSG_WEAPON_FIRE,
	MSG_WEAPON_CHANGE,

	MSG_PICKEDUP_ITEM,

	MSG_GOAL_SUCCESS,
	MSG_GOAL_FAILED,
	MSG_GOAL_ABORTED,

	MSG_PATH_SUCCESS,
	MSG_PATH_FAILED,

	MSG_SPAWN,
	MSG_CHANGETEAM,
	MSG_INVALIDTEAM,
	MSG_INVALIDCLASS,
	MSG_CHANGECLASS,
	MSG_DEATH,
	MSG_HEALED,
	MSG_REVIVED,
	MSG_RECIEVED_AMMO,
	MSG_KILLEDSOMEONE,
	MSG_ADDWEAPON,
	MSG_REMOVEWEAPON,
	MSG_REFRESHWEAPON,
	MSG_REFRESHALLWEAPONS,
	MSG_SPECTATED,
	MSG_AIMCOMPLETED,
	MSG_SCRIPTMSG,
	MSG_PROXIMITY_TRIGGER,
	MSG_DYNAMIC_PATHS_CHANGED,
	MSG_MINE_ARMED,
	MSG_MINE_DETONATE,
	MSG_MORTAR_IMPACT,
	MSG_ENT_ENTER_RADIUS,
	MSG_ENT_LEAVE_RADIUS,
	MSG_MG_ENTER_RADIUS,
	MSG_MG_LEAVE_RADIUS,

	MSG_FEEL_PLAYER_USE,
	MSG_TOOK_DAMAGE,

	MSG_HEAR_CHATMSG_BASE,
	MSG_HEAR_CHATMSG_GLOBAL,
	MSG_HEAR_CHATMSG_TEAM,
	MSG_HEAR_CHATMSG_PRIVATE,
	MSG_HEAR_CHATMSG_GROUP,
	MSG_HEAR_VOICEMACRO_BASE,
	MSG_HEAR_VOICEMACRO_GLOBAL,
	MSG_HEAR_VOICEMACRO_TEAM,
	MSG_HEAR_VOICEMACRO_PRIVATE,
	MSG_HEAR_VOICEMACRO_GROUP,

	MSG_HEAR_SOUND,
	MSG_SENSE_ENTITY,
	MSG_DROWNING,

	// Must be last
	MSG_BASE_LAST = 1000
};

#pragma pack(push)
#pragma pack(4)

class Message
{
public:
	int Id() const
	{
		return mMessageId;
	}
	size_t Size() const
	{
		return mMessageSize;
	}
protected:
	const int	mMessageId;
	void*		mMessagePtr;
	size_t		mMessageSize;

	Message( int id, void * ptr = 0, uint32_t size = 0 )
		: mMessageId( id )
		, mMessagePtr( ptr )
		, mMessageSize( size )
	{
	}
	~Message()
	{
	}
	Message&operator=( const Message& other );
};

template<typename T, int MsgId>
class MessageT : public Message
{
public:
	enum
	{
		MessageId = MsgId
	};
	T	mData;

	static const T* Cast( const Message& msg, int allowId = -1 )
	{
		typedef MessageT<T, T::Msg::MessageId> TypedMsg;
		if ( ( msg.Id() == T::Msg::MessageId || msg.Id() == allowId ) && sizeof( T ) == msg.Size() )
		{
			const TypedMsg* typedPtr = static_cast<const TypedMsg*>( &msg );
			return &typedPtr->mData;
		}
		return 0;
	}

	static T* Cast( Message& msg, int allowId = 0 )
	{
		typedef MessageT<T, T::Msg::MessageId> TypedMsg;
		if ( ( msg.Id() == T::Msg::MessageId || msg.Id() == allowId ) && sizeof( T ) == msg.mMessageSize )
		{
			TypedMsg* typedPtr = static_cast<TypedMsg*>( &msg );
			return &typedPtr->mData;
		}
		return 0;
	}

	MessageT() : Message( MsgId, &mData, sizeof( T ) )
	{
	}
	~MessageT()
	{
	}
};

//////////////////////////////////////////////////////////////////////////

#define CASE_MSG(MSGCLASS) \
	case MSGCLASS::Msg::MessageId:\
		if ( const MSGCLASS* msg = MSGCLASS::Msg::Cast( message ) ) \

//////////////////////////////////////////////////////////////////////////
// Events

struct EvGameStart
{
	typedef MessageT<EvGameStart, MSG_STARTGAME> Msg;
};

struct EvGameEnd
{
	typedef MessageT<EvGameEnd, MSG_ENDGAME> Msg;
};

struct EvScriptChanged
{
	typedef MessageT<EvScriptChanged, MSG_SCRIPT_CHANGED> Msg;

	int32_t		mScriptKey;
};

struct EvClientConnected
{
	typedef MessageT<EvClientConnected, MSG_CLIENTCONNECTED> Msg;

	int			mGameId;
	bool		mIsBot;
	int			mDesiredClass;
	int			mDesiredTeam;

	EvClientConnected()
		: mGameId(-1)
		, mIsBot(false)
		, mDesiredClass(RANDOM_CLASS_IF_NO_CLASS)
		, mDesiredTeam(RANDOM_TEAM_IF_NO_TEAM)
	{
	}
};

struct EvClientDisconnected
{
	typedef MessageT<EvClientDisconnected, MSG_CLIENTDISCONNECTED> Msg;

	int			mGameId;
};

struct EvSpawn
{
	typedef MessageT<EvSpawn, MSG_SPAWN> Msg;	
};

struct EvEntityCreated
{
	typedef MessageT<EvEntityCreated, MSG_ENTITYCREATED> Msg;
	
	GameEntity		mEntity;
	EntityInfo 		mEntityInfo;
};

struct EvEntityDeleted
{
	typedef MessageT<EvEntityDeleted, MSG_ENTITYDELETED> Msg;

	GameEntity		mEntity;
};

struct EvDeath
{
	typedef MessageT<EvDeath, MSG_DEATH> Msg;

	GameEntity	mWhoKilledMe;
	char		mMeansOfDeath[32];
};

struct EvKilledSomeone
{
	typedef MessageT<EvKilledSomeone, MSG_KILLEDSOMEONE> Msg;

	GameEntity	mWhoIKilled;
	char		mMeansOfDeath[32];
};

struct EvTakeDamage
{
	typedef MessageT<EvTakeDamage, MSG_TOOK_DAMAGE> Msg;

	GameEntity	mInflictor;
	char		mDamageType[32];
};

struct EvHealed
{
	typedef MessageT<EvHealed, MSG_HEALED> Msg;

	GameEntity	mWhoHealedMe;
};

struct EvRevived
{
	typedef MessageT<EvRevived, MSG_REVIVED> Msg;

	GameEntity	mWhoRevivedMe;
};

struct EvRecievedAmmo
{
	typedef MessageT<EvRecievedAmmo, MSG_RECIEVED_AMMO> Msg;

	GameEntity	mFromWho;
};

struct EvMineArmed
{
	typedef MessageT<EvMineArmed, MSG_MINE_ARMED> Msg;

	GameEntity	mMine;
};

struct EvMineDetonate
{
	typedef MessageT<EvMineDetonate, MSG_MINE_DETONATE> Msg;

	GameEntity	mMine;
};

struct EvMortarImpact
{
	typedef MessageT<EvMortarImpact, MSG_MORTAR_IMPACT> Msg;

	GameEntity	mOwner;
	float		mPosition[ 3 ];
	float		mRadius;
};

struct EvChangeTeam
{
	typedef MessageT<EvChangeTeam, MSG_CHANGETEAM> Msg;

	int			mNewTeam;
};

struct EvWeaponChanged
{
	typedef MessageT<EvWeaponChanged, MSG_WEAPON_CHANGE> Msg;

	int			mWeaponId;
};

struct EvChangeClass
{
	typedef MessageT<EvChangeClass, MSG_CHANGECLASS> Msg;

	int			mNewClass;
};

struct EvSpectated
{
	typedef MessageT<EvSpectated, MSG_SPECTATED> Msg;

	int			mWhoSpectatingMe;
};

struct EvAddWeapon
{
	typedef MessageT<EvAddWeapon, MSG_ADDWEAPON> Msg;

	int			mWeaponId;
};

struct EvRemoveWeapon
{
	typedef MessageT<EvRemoveWeapon, MSG_REMOVEWEAPON> Msg;

	int			mWeaponId;
};

struct EvRefreshWeapon
{
	typedef MessageT<EvRefreshWeapon, MSG_REFRESHWEAPON> Msg;
	
	int32_t		mWeaponId;
};

struct EvRefreshAllWeapons
{
	typedef MessageT<EvRefreshAllWeapons, MSG_REFRESHALLWEAPONS> Msg;
	
	int32_t		mPad;
};

struct EvWeaponFire
{
	typedef MessageT<EvWeaponFire, MSG_WEAPON_FIRE> Msg;

	int				mWeaponId;
	int				mTeamNum;
	FireMode		mFireMode;
	GameEntity		mProjectile;
};

struct EvWeaponChange
{
	typedef MessageT<EvWeaponChange, MSG_WEAPON_CHANGE> Msg;

	int			mWeaponId;
};

struct EvPickedUpItem
{
	typedef MessageT<EvPickedUpItem, MSG_PICKEDUP_ITEM> Msg;

	EntityInfo		mInfo;
};

struct EvChatMessage
{
	typedef MessageT<EvChatMessage, MSG_HEAR_CHATMSG_BASE> Msg;

	GameEntity	mWhoSaidIt;
	char		mMessage[512];

	EvChatMessage()
	{
		mMessage[ 0 ] = 0;
	}
};

struct EvChatMessageGlobal : public EvChatMessage
{
	typedef MessageT<EvChatMessageGlobal, MSG_HEAR_CHATMSG_GLOBAL> Msg;
};

struct EvChatMessageTeam : public EvChatMessage
{
	typedef MessageT<EvChatMessageTeam, MSG_HEAR_CHATMSG_TEAM> Msg;
};

struct EvChatMessagePrivate : public EvChatMessage
{
	typedef MessageT<EvChatMessageTeam, MSG_HEAR_CHATMSG_PRIVATE> Msg;
};

struct EvChatMessageGroup : public EvChatMessage
{
	typedef MessageT<EvChatMessageTeam, MSG_HEAR_CHATMSG_GROUP> Msg;
};

struct EvVoiceMacro
{
	typedef MessageT<EvVoiceMacro, MSG_HEAR_VOICEMACRO_BASE> Msg;

	GameEntity	mWhoSaidIt;
	char		mMessage[512];

	EvVoiceMacro()
	{
		mMessage[ 0 ] = 0;
	}
};

struct EvVoiceMacroGlobal : public EvVoiceMacro
{
	typedef MessageT<EvVoiceMacroGlobal, MSG_HEAR_VOICEMACRO_GLOBAL> Msg;
};

struct EvVoiceMacroTeam : public EvVoiceMacro
{
	typedef MessageT<EvVoiceMacroTeam, MSG_HEAR_VOICEMACRO_TEAM> Msg;
};

struct EvVoiceMacroPrivate : public EvVoiceMacro
{
	typedef MessageT<EvVoiceMacroTeam, MSG_HEAR_VOICEMACRO_PRIVATE> Msg;
};

struct EvVoiceMacroGroup : public EvVoiceMacro
{
	typedef MessageT<EvVoiceMacroTeam, MSG_HEAR_VOICEMACRO_GROUP> Msg;
};

struct EvPlayerUsed
{
	typedef MessageT<EvPlayerUsed, MSG_FEEL_PLAYER_USE> Msg;

	GameEntity	mWhoDidIt;
};

struct EvSound
{
	typedef MessageT<EvSound, MSG_HEAR_SOUND> Msg;

	char		mSoundName[64];
	float		mOrigin[3];
	GameEntity	mSource;
	int			mSoundType;
};

struct EvEntitySensed
{
	typedef MessageT<EvEntitySensed, MSG_SENSE_ENTITY> Msg;

	int			mEntityClass;
	GameEntity	mEntity;
};

struct EvEntityDrowning
{
	typedef MessageT<EvEntityDrowning, MSG_DROWNING> Msg;
};

struct EvScriptMessage
{
	typedef MessageT<EvScriptMessage, MSG_SCRIPTMSG> Msg;

	char		mMessageName[64];
	char		mMessageData1[64];
	char		mMessageData2[64];
	char		mMessageData3[64];
};

struct EvScriptSignal
{
	typedef MessageT<EvScriptSignal, MSG_SCRIPTSIGNAL> Msg;

	char		mSignalName[64];
};

struct EvEntityConnection
{
	typedef MessageT<EvEntityConnection, MSG_ADD_ENTITY_CONNECTION> Msg;

	GameEntity	mEntitySrc;
	GameEntity	mEntityDst;

	/*float		mSrcPos[3];
	float		mDstPos[3];*/

	BitFlag32	mTeam;
	float		mRadius;
	float		mCost;

	EvEntityConnection()
		: mRadius(0.f)
		, mCost(0.f)
	{
		/*for ( int i = 0; i < 3; ++i )
			mSrcPos[ i ] = mDstPos[ i ] = 0.0f;*/
	}
};

struct EvEntEnterRadius
{
	typedef MessageT<EvEntEnterRadius, MSG_ENT_ENTER_RADIUS> Msg;

	GameEntity	mEntity;
};

struct EvEntLeaveRadius
{
	typedef MessageT<EvEntLeaveRadius, MSG_ENT_LEAVE_RADIUS> Msg;

	GameEntity	mEntity;
};

struct EvAnalyticEvent
{
	typedef MessageT<EvAnalyticEvent, MSG_ANALYTIC_EVENT> Msg;

	const char *		mName;
	const char *		mArea;
	float				mValue;
	float				mPosition[3];
	bool				mHasValue;
	bool				mHasPosition;
};

#pragma pack(pop)

#endif
