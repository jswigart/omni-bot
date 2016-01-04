////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __OMNIBOT_TYPES_H__
#define __OMNIBOT_TYPES_H__

#include <stdint.h>
#include "Omni-Bot_BitFlags.h"

// constants: Omni-bot Errors
enum omnibot_error
{
	BOT_ERROR_NONE,
	BOT_ERROR_CANTLOADDLL,
	BOT_ERROR_CANTGETBOTFUNCTIONS,
	BOT_ERROR_CANTINITBOT,
	BOT_ERROR_BAD_INTERFACE,
	BOT_ERROR_WRONGVERSION,
	BOT_ERROR_FILESYSTEM,

	// THIS MUST STAY LAST
	BOT_NUM_ERRORS
};

enum MessageType
{
	kNormal,
	kInfo,
	kWarning,
	kError,
	kDebug,
	kScript
};

// typedef: GameEntity
class GameEntity
{
public:
	int16_t GetIndex() const
	{
		return udata.mShort[ 0 ];
	}
	int16_t GetSerial() const
	{
		return udata.mShort[ 1 ];
	}

	int32_t AsInt() const
	{
		return udata.mInt;
	}
	void FromInt( int32_t _n )
	{
		udata.mInt = _n;
	}

	void Reset()
	{
		*this = GameEntity();
	}

	bool IsValid() const
	{
		return udata.mShort[ 0 ] >= 0;
	}

	bool operator!=( const GameEntity& other ) const
	{
		return udata.mInt != other.udata.mInt;
	}
	bool operator==( const GameEntity& other ) const
	{
		return udata.mInt == other.udata.mInt;
	}

	explicit GameEntity( int16_t _index, int16_t _serial )
	{
		udata.mShort[ 0 ] = _index;
		udata.mShort[ 1 ] = _serial;
	}
	GameEntity()
	{
		udata.mShort[ 0 ] = -1;
		udata.mShort[ 1 ] = 0;
	}
private:
	union udatatype
	{
		int32_t			mInt;
		int16_t			mShort[ 2 ];
	} udata;
};

// typedef: GameId
typedef int GameId;

// typedef: NavFlags
typedef uint64_t NavFlags;

// enum: obFunctionStatus
//		Represents the status of some function.
enum obFunctionStatus
{
	Function_Finished,
	Function_InProgress,
};

// enumerations: obResult
enum obResult
{
	Success = 0,
	OutOfPVS,
	UnableToAddBot,
	InvalidEntity,
	InvalidParameter,
	UnknownMessageType,
};

inline bool SUCCESS( obResult _res )
{
	return ( _res == Success ) ? true : false;
}
inline int MAKE_KEY( char _v1, char _v2, char _v3, char _v4 )
{
	return ( ( ( _v1 ) << 24 ) | ( ( _v2 ) << 16 ) | ( ( _v3 ) << 8 ) | ( _v4 ) );
}

enum FireMode
{
	Primary,
	Secondary,

	// THIS MUST STAY LAST
	NumFireModes,

	// Cept for this
	InvalidFireMode
};

enum ConnDir
{
	CON_SOURCE,
	CON_DEST,
	CON_TWO_WAY,
};

enum Stance
{
	StanceStand,
	StanceCrouch,
	StanceProne,

	// last
	StanceNum,
};

// enumerations: WeaponType
enum WeaponType
{
	INVALID_WEAPON = 0,
};

// enumerations: BotDebugFlag
enum BotDebugFlag
{
	BOT_DEBUG_LOG = 0,
	BOT_DEBUG_MOVEVEC,
	BOT_DEBUG_SCRIPT,
	BOT_DEBUG_FPINFO,
	BOT_DEBUG_PLANNER,
	BOT_DEBUG_EVENTS,
	BOT_DEBUG_LOG_FAILED_PATHS,

	// THIS MUST STAY LAST
	NUM_BOT_DEBUG_FLAGS = 16,
};

enum TeamBase
{
	OB_TEAM_ALL = -2,
	OB_TEAM_SPECTATOR = -1,
	OB_TEAM_NONE,
	OB_TEAM_1,
	OB_TEAM_2,
	OB_TEAM_3,
	OB_TEAM_4,
};

// enumerations: Helpers
enum Helpers
{
	RANDOM_CLASS = -1,
	RANDOM_CLASS_IF_NO_CLASS = -2,
	RANDOM_TEAM = -1,
	RANDOM_TEAM_IF_NO_TEAM = -2,
};

// typedef: AABB
struct AABB
{
	float	mMins[ 3 ];
	float	mMaxs[ 3 ];
#ifdef __cplusplus

	enum Direction
	{
		DIR_NORTH,
		DIR_EAST,
		DIR_SOUTH,
		DIR_WEST,
		DIR_TOP,
		DIR_BOTTOM,

		DIR_ALL,
	};

	bool IsZero() const
	{
		for ( int i = 0; i < 3; ++i )
		{
			if ( mMins[ i ] != 0.f ||
				mMaxs[ i ] != 0.f )
				return false;
		}
		return true;
	}
	void Set( const float _x, const float _y, const float _z )
	{
		mMins[ 0 ] = _x;
		mMins[ 1 ] = _y;
		mMins[ 2 ] = _z;

		mMaxs[ 0 ] = _x;
		mMaxs[ 1 ] = _y;
		mMaxs[ 2 ] = _z;
	}
	void Set( const float _pt[ 3 ] )
	{
		for ( int i = 0; i < 3; ++i )
		{
			mMins[ i ] = _pt[ i ];
			mMaxs[ i ] = _pt[ i ];
		}
	}
	void Set( const float _min[ 3 ], const float _max[ 3 ] )
	{
		for ( int i = 0; i < 3; ++i )
		{
			mMins[ i ] = _min[ i ];
			mMaxs[ i ] = _max[ i ];
		}
	}
	void FixMinMax()
	{
		for ( int i = 0; i < 3; ++i )
		{
			mMins[ i ] = mMins[ i ] < mMaxs[ i ] ? mMins[ i ] : mMaxs[ i ];
			mMaxs[ i ] = mMins[ i ] > mMaxs[ i ] ? mMins[ i ] : mMaxs[ i ];
		}
	}
	void CenterPoint( float _out[ 3 ] ) const
	{
		_out[ 0 ] = ( mMins[ 0 ] + mMaxs[ 0 ] ) * 0.5f;
		_out[ 1 ] = ( mMins[ 1 ] + mMaxs[ 1 ] ) * 0.5f;
		_out[ 2 ] = ( mMins[ 2 ] + mMaxs[ 2 ] ) * 0.5f;
	}
	void CenterTop( float _out[ 3 ] ) const
	{
		_out[ 0 ] = ( mMins[ 0 ] + mMaxs[ 0 ] ) * 0.5f;
		_out[ 1 ] = ( mMins[ 1 ] + mMaxs[ 1 ] ) * 0.5f;
		_out[ 2 ] = mMaxs[ 2 ];
	}
	void CenterBottom( float _out[ 3 ] ) const
	{
		_out[ 0 ] = ( mMins[ 0 ] + mMaxs[ 0 ] ) * 0.5f;
		_out[ 1 ] = ( mMins[ 1 ] + mMaxs[ 1 ] ) * 0.5f;
		_out[ 2 ] = mMins[ 2 ];
	}
	void SetCenter( const float _out[ 3 ] )
	{
		for ( int i = 0; i < 3; ++i )
		{
			mMins[ i ] += _out[ i ];
			mMaxs[ i ] += _out[ i ];
		}
	}
	bool Intersects( const AABB &_bbox ) const
	{
		for ( int i = 0; i < 3; i++ )
		{
			if ( mMaxs[ i ] < _bbox.mMins[ i ] || mMins[ i ] > _bbox.mMaxs[ i ] )
				return false;
		}
		return true;
	}
	bool Contains( const float _pt[ 3 ] ) const
	{
		for ( int i = 0; i < 3; i++ )
		{
			if ( mMaxs[ i ] < _pt[ i ] || mMins[ i ] > _pt[ i ] )
				return false;
		}
		return true;
	}
	bool FindIntersection( const AABB &_bbox, AABB& _overlap ) const
	{
		if ( Intersects( _bbox ) )
		{
			for ( int i = 0; i < 3; i++ )
			{
				if ( mMaxs[ i ] <= _bbox.mMaxs[ i ] )
					_overlap.mMaxs[ i ] = mMaxs[ i ];
				else
					_overlap.mMaxs[ i ] = _bbox.mMaxs[ i ];

				if ( mMins[ i ] <= _bbox.mMins[ i ] )
					_overlap.mMins[ i ] = _bbox.mMins[ i ];
				else
					_overlap.mMins[ i ] = mMins[ i ];
			}
			return true;
		}
		return false;
	}

	float GetAxisLength( int _axis ) const
	{
		return mMaxs[ _axis ] - mMins[ _axis ];
	}
	void GetSize( float size[ 3 ] )
	{
		size[ 0 ] = GetAxisLength( 0 );
		size[ 1 ] = GetAxisLength( 1 );
		size[ 2 ] = GetAxisLength( 2 );
	}
	float GetArea() const
	{
		return GetAxisLength( 0 ) * GetAxisLength( 1 ) * GetAxisLength( 2 );
	}
	float DistanceFromBottom( const float _pt[ 3 ] ) const
	{
		return -( mMins[ 2 ] - _pt[ 2 ] );
	}
	float DistanceFromTop( const float _pt[ 3 ] ) const
	{
		return ( mMaxs[ 2 ] - _pt[ 2 ] );
	}
	void Scale( float _scale )
	{
		for ( int i = 0; i < 3; ++i )
		{
			mMins[ i ] *= _scale;
			mMaxs[ i ] *= _scale;
		}
	}
	AABB ScaleCopy( float _scale )
	{
		AABB out = *this; // cs: was AABB, but gcc said NO
		for ( int i = 0; i < 3; ++i )
		{
			out.mMins[ i ] *= _scale;
			out.mMaxs[ i ] *= _scale;
		}
		return out;
	}
	void Expand( float _expand )
	{
		for ( int i = 0; i < 3; ++i )
		{
			mMins[ i ] -= _expand;
			mMaxs[ i ] += _expand;
		}
	}
	void Expand( float _expandX, float _expandY, float _expandZ )
	{
		mMins[ 0 ] -= _expandX;
		mMaxs[ 0 ] += _expandX;
		mMins[ 1 ] -= _expandY;
		mMaxs[ 1 ] += _expandY;
		mMins[ 2 ] -= _expandZ;
		mMaxs[ 2 ] += _expandZ;
	}
	void Expand( const float _pt[ 3 ] )
	{
		for ( int i = 0; i < 3; ++i )
		{
			if ( _pt[ i ] < mMins[ i ] )
				mMins[ i ] = _pt[ i ];

			if ( _pt[ i ] > mMaxs[ i ] )
				mMaxs[ i ] = _pt[ i ];
		}
	}
	void Expand( const AABB &_bbox )
	{
		Expand( _bbox.mMins );
		Expand( _bbox.mMaxs );
	}
	void ExpandAxis( int _axis, float _expand )
	{
		mMins[ _axis ] -= _expand;
		mMaxs[ _axis ] += _expand;
	}
	void FlipHorizontalAxis()
	{
		for ( int i = 0; i < 2; ++i )
		{
			float tmp = mMins[ i ];
			mMins[ i ] = mMaxs[ i ];
			mMaxs[ i ] = tmp;
		}
	}
	void GetBottomCorners( float _bl[ 3 ], float _tl[ 3 ], float _tr[ 3 ], float _br[ 3 ] )
	{
		_bl[ 0 ] = mMins[ 0 ];
		_bl[ 1 ] = mMins[ 1 ];
		_bl[ 2 ] = mMins[ 2 ];

		_tl[ 0 ] = mMins[ 0 ];
		_tl[ 1 ] = mMaxs[ 1 ];
		_tl[ 2 ] = mMins[ 2 ];

		_tr[ 0 ] = mMaxs[ 0 ];
		_tr[ 1 ] = mMaxs[ 1 ];
		_tr[ 2 ] = mMins[ 2 ];

		_br[ 0 ] = mMaxs[ 0 ];
		_br[ 1 ] = mMins[ 1 ];
		_br[ 2 ] = mMins[ 2 ];
	}
	void GetTopCorners( float _bl[ 3 ], float _tl[ 3 ], float _tr[ 3 ], float _br[ 3 ] )
	{
		GetBottomCorners( _bl, _tl, _tr, _br );
		_bl[ 2 ] = mMaxs[ 2 ];
		_tl[ 2 ] = mMaxs[ 2 ];
		_tr[ 2 ] = mMaxs[ 2 ];
		_br[ 2 ] = mMaxs[ 2 ];
	}
	void Translate( const float _pos[ 3 ] )
	{
		for ( int i = 0; i < 3; ++i )
		{
			mMins[ i ] += _pos[ i ];
			mMaxs[ i ] += _pos[ i ];
		}
	}
	void UnTranslate( const float _pos[ 3 ] )
	{
		for ( int i = 0; i < 3; ++i )
		{
			mMins[ i ] -= _pos[ i ];
			mMaxs[ i ] -= _pos[ i ];
		}
	}
	AABB TranslateCopy( const float _pos[ 3 ] ) const
	{
		AABB aabb = *this;
		for ( int i = 0; i < 3; ++i )
		{
			aabb.mMins[ i ] += _pos[ i ];
			aabb.mMaxs[ i ] += _pos[ i ];
		}
		return aabb;
	}
	AABB( const float _mins[ 3 ], const float _maxs[ 3 ] )
	{
		Set( _mins, _maxs );
	}
	AABB( const float _center[ 3 ] )
	{
		Set( _center );
	}
	AABB()
	{
		for ( int i = 0; i < 3; ++i )
		{
			mMins[ i ] = 0.f;
			mMaxs[ i ] = 0.f;
		}
	}
#endif
};

// enumerations: ButtonFlags
enum ButtonFlags
{
	BOT_BUTTON_ATTACK1 = 0,
	BOT_BUTTON_ATTACK2,
	BOT_BUTTON_JUMP,
	BOT_BUTTON_CROUCH,
	BOT_BUTTON_PRONE,
	BOT_BUTTON_WALK,
	BOT_BUTTON_USE,
	BOT_BUTTON_FWD,
	BOT_BUTTON_BACK,
	BOT_BUTTON_RSTRAFE,
	BOT_BUTTON_LSTRAFE,
	BOT_BUTTON_MOVEUP,
	BOT_BUTTON_MOVEDN,
	BOT_BUTTON_RELOAD,
	BOT_BUTTON_SPRINT,
	BOT_BUTTON_DROP,
	BOT_BUTTON_LEANLEFT,
	BOT_BUTTON_LEANRIGHT,
	BOT_BUTTON_AIM,
	BOT_BUTTON_RESPAWN,
	BOT_BUTTON_TAUNT,

	// THIS MUST BE LAST
	BOT_BUTTON_FIRSTUSER
};

// enumerations: EntityFlags
enum EntityFlag
{
	ENT_FLAG_TEAM1,
	ENT_FLAG_TEAM2,
	ENT_FLAG_TEAM3,
	ENT_FLAG_TEAM4,
	ENT_FLAG_VISTEST,
	ENT_FLAG_DISABLED,
	ENT_FLAG_PRONED,
	ENT_FLAG_CROUCHED,
	ENT_FLAG_CARRYABLE,
	ENT_FLAG_ALIVE,
	ENT_FLAG_DEAD,
	ENT_FLAG_INWATER,
	ENT_FLAG_UNDERWATER,
	ENT_FLAG_ZOOMING,
	ENT_FLAG_ONLADDER,
	ENT_FLAG_ONGROUND,
	ENT_FLAG_RELOADING,
	ENT_FLAG_ON_ICE,
	ENT_FLAG_HUMANCONTROLLED,
	ENT_FLAG_IRONSIGHT,
	ENT_FLAG_INVEHICLE,
	ENT_FLAG_FROZEN,
	ENT_FLAG_TAUNTING,
	ENT_FLAG_AIMING,
	ENT_FLAG_COLLIDABLE,
	ENT_FLAG_HELDBYPLAYER,
	ENT_FLAG_USEBOUNDS,
	ENT_FLAG_SAVENAV,

	// THIS MUST BE LAST
	ENT_FLAG_FIRST_USER = 32
};

// enumerations: Powerups
enum Powerups
{
	PWR_NONE = 0,
	PWR_INVINCIBLE,

	// THIS MUST BE LAST
	PWR_FIRST_USER = 8,
};

// enumerations: EntityCategory
enum EntityCategory
{
	ENT_CAT_PLAYER,
	ENT_CAT_VEHICLE,
	ENT_CAT_PROJECTILE,
	ENT_CAT_SHOOTABLE,	
	ENT_CAT_PICKUP_PHYS,
	ENT_CAT_TRIGGER,
	ENT_CAT_MOVER,
	ENT_CAT_MOUNTEDWEAPON,
	ENT_CAT_MISC,
	ENT_CAT_NOLOS,
	ENT_CAT_PROP,
	ENT_CAT_PROP_PUSHABLE,
	ENT_CAT_AUTODEFENSE,
	ENT_CAT_BUILDABLE,
	ENT_CAT_OBSTACLE,
	ENT_CAT_DYNAMIC_NAV,
	ENT_CAT_INTERNAL,

	// THIS MUST BE LAST
	ENT_CAT_MAX,
};

// package: EntityGroup
enum EntityGroup
{
	ENT_GRP_UNKNOWN,
	ENT_GRP_PLAYER,
	ENT_GRP_SPECTATOR,
	ENT_GRP_PLAYERSTART,
	ENT_GRP_BUTTON,
	ENT_GRP_RESUPPLY,
	ENT_GRP_LADDER,
	ENT_GRP_FLAG,
	ENT_GRP_FLAGCAPPOINT,
	ENT_GRP_CONTROLPOINT,
	ENT_GRP_TELEPORTER,
	ENT_GRP_LIFT,
	ENT_GRP_MOVER,
	ENT_GRP_JUMPPAD,
	ENT_GRP_JUMPPAD_TARGET,
	ENT_GRP_GOAL,
	ENT_GRP_WEAPON,
	ENT_GRP_MAP,
	ENT_GRP_PROP,
	ENT_GRP_PROP_EXPLODE,
	ENT_GRP_PROP_BREAKABLE,
	ENT_GRP_PROP_STATIC,
	ENT_GRP_MONSTER,
	ENT_GRP_PROJECTILE,
	ENT_GRP_POWERUP,
	ENT_GRP_DISPENSER,
	ENT_GRP_BUILDABLE,
	ENT_GRP_MOUNTABLE,
};

enum EntityClass
{
	ENT_CLASS_NONE,
};

// Flags that denote the navigation area types for the navmesh
enum NavAreaFlags
{
	NAVFLAGS_NONE			= 0,
	NAVFLAGS_WALK			= ( 1 << 0 ),
	NAVFLAGS_CROUCH			= ( 1 << 1 ),
	NAVFLAGS_PRONE			= ( 1 << 2 ),
	NAVFLAGS_TEAM1_ONLY		= ( 1 << 3 ),
	NAVFLAGS_TEAM2_ONLY		= ( 1 << 4 ),
	NAVFLAGS_TEAM3_ONLY		= ( 1 << 5 ),
	NAVFLAGS_TEAM4_ONLY		= ( 1 << 6 ),
	NAVFLAGS_WATER			= ( 1 << 7 ),
	NAVFLAGS_JUMP			= ( 1 << 8 ),
	NAVFLAGS_LADDER			= ( 1 << 9 ),
	NAVFLAGS_TELEPORT		= ( 1 << 10 ),
	NAVFLAGS_DOOR			= ( 1 << 11 ),
	NAVFLAGS_ROCKETJUMP		= ( 1 << 12 ),
	NAVFLAGS_PUSHABLE		= ( 1 << 13 ),
	NAVFLAGS_MOVER			= ( 1 << 14 ),	
	NAVFLAGS_JUMPPAD		= ( 1 << 15 ),
	NAVFLAGS_JETPACK		= ( 1 << 16 ),
	NAVFLAGS_LONGJUMP		= ( 1 << 17 ),

	NAVFLAGS_DESTRUCTIBLE	= ( 1 << 29 ),
	NAVFLAGS_DISABLED		= ( 1 << 30 ),
	
	// compound flags
	NAVFLAGS_ALLTEAMS		= NAVFLAGS_TEAM1_ONLY | NAVFLAGS_TEAM2_ONLY | NAVFLAGS_TEAM3_ONLY | NAVFLAGS_TEAM4_ONLY,
};

static const int64_t NAVFLAGS_ENTITYREF = ( (int64_t)1 << 32 );
static const int64_t NAVFLAGS_ENTITYREF_MASK = ( (int64_t)-1 << 32 );

// struct: EntityInfo
struct EntityInfo
{
	struct Range
	{
		uint16_t		mNum;
		uint16_t		mMax;

		void Set( uint16_t v )
		{
			mNum = mMax = v;
		}
		void Set( uint16_t v, uint16_t vm )
		{
			mNum = v;
			mMax = vm;
		}
		float Percent( float def = 0.0f ) const
		{
			return mMax > 0 ? (float)mNum / (float)mMax : def;
		}
		Range( uint16_t v = 0)
		{
			Set( v );
		}

		Range( uint16_t v, uint16_t vm )
		{
			Set( v, vm );
		}
	};
	
	struct Ammo
	{
		uint16_t		mNum;
		uint16_t		mWeaponId;

		void Set( uint16_t num, uint16_t wpn )
		{
			mNum = num;
			mWeaponId = wpn;
		}

		Ammo() : mNum( 0 ), mWeaponId( 0 )
		{
		}
	};

	static const int NUM_AMMO_TYPES = 5;

	EntityGroup 	mGroup;
	uint16_t		mClassId;
	uint16_t		mLevel;
	Range			mHealth;
	Range			mArmor;
	Range			mEnergy;
	Ammo			mAmmo[ NUM_AMMO_TYPES ];
	BitFlag32		mCategory;
	BitFlag64		mFlags;
	BitFlag32		mPowerUps;
	NavAreaFlags	mNavFlags;
	GameEntity		mOwner;
	float			mNavTeamCost[ 4 ];
	
	EntityInfo()
		: mGroup( ENT_GRP_UNKNOWN )
		, mClassId( ENT_CLASS_NONE )
		, mLevel( 0 )
		, mNavFlags( NAVFLAGS_NONE )
	{
		for ( int i = 0; i < 4; ++i )
			mNavTeamCost[ i ] = 0.0f;
	}
};

// enumerations: SoundType
enum SoundType
{
	SND_NONE,
	SND_JUMP,
	SND_FOOTSTEP,
	SND_TAKEDAMAGE,
	SND_POWERUP_SPAWN,
	SND_POWERUP_PICKUP,
	SND_WEAPON_FIRE_OMNIBOT,
	SND_WEAPON_RELOAD_OMNIBOT,
	SND_WEAPON_EMPTY,
	SND_WEAPON_STARTFIRE,
	SND_VOICE_TAUNT,
	SND_VOICE_TEAM,
	SND_VOICE_ENEMY,

	// THIS MUST BE LAST
	SND_MAX_SOUNDS
};

// enumerations: Contents
enum ContentFlags
{
	CONT_NONE = 0,
	CONT_SOLID = ( 1 << 0 ),
	CONT_WATER = ( 1 << 1 ),
	CONT_SLIME = ( 1 << 2 ),
	CONT_FOG = ( 1 << 3 ),
	CONT_MOVER = ( 1 << 4 ),
	CONT_TRIGGER = ( 1 << 5 ),
	CONT_LAVA = ( 1 << 6 ),
	CONT_LADDER = ( 1 << 7 ),
	CONT_TELEPORTER = ( 1 << 8 ),
	CONT_PLYRCLIP = ( 1 << 9 ),
	CONT_NONSOLID = ( 1 << 10 ),
	CONT_JUMPPAD = ( 1 << 11 ),
	CONT_HITBOX = ( 1 << 12 ),

	// THIS MUST BE LAST
	CONT_START_USER = ( 1 << 24 )
};

// enumerations: SurfaceFlags
enum SurfaceFlags
{
	SURFACE_NONE = 0,
	SURFACE_NONSOLID = ( 1 << 0 ),
	SURFACE_SLICK = ( 1 << 1 ),
	SURFACE_LADDER = ( 1 << 2 ),
	SURFACE_NOFALLDAMAGE = ( 1 << 3 ),
	SURFACE_SKY = ( 1 << 4 ),
	SURFACE_IGNORE = ( 1 << 5 ),
	SURFACE_NOFOOTSTEP = ( 1 << 6 ),
	SURFACE_NODRAW = ( 1 << 7 ),
	SURFACE_HITBOX = ( 1 << 8 ),
	SURFACE_MINE = ( 1 << 9 ),

	// THIS MUST BE LAST
	SURFACE_START_USER = ( 1 << 24 )
};

// enumerations: SkeletonBone
enum SkeletonBone
{
	BONE_TORSO,
	BONE_PELVIS,
	BONE_HEAD,
	BONE_RIGHTARM,
	BONE_LEFTARM,
	BONE_RIGHTHAND,
	BONE_LEFTHAND,
	BONE_RIGHTLEG,
	BONE_LEFTLEG,
	BONE_RIGHTFOOT,
	BONE_LEFTFOOT,

	// THIS MUST BE LAST
	BONE_LAST_BONE = 1000
};

// enumerations: NavigationID
enum NavigatorID
{
	NAVID_NONE,
	NAVID_WAYPOINT,
	NAVID_FLOODFILL,
	NAVID_RECAST,

	// THIS MUST BE LAST
	NAVID_MAX
};

// enumerations: TraceMasks
enum TraceMasks
{
	TR_MASK_ALL = ( 1 << 0 ), // hit everything
	TR_MASK_SOLID = ( 1 << 1 ), // blocked by solids/world
	TR_MASK_PLAYER = ( 1 << 2 ), // blocked by players
	TR_MASK_SHOT = ( 1 << 3 ), // blocked by same mask that game uses for projectile/shot traces
	TR_MASK_OPAQUE = ( 1 << 4 ), // blocked by opaque surfaces(even if they can be shot through)
	TR_MASK_WATER = ( 1 << 5 ), // blocked by see-through water
	TR_MASK_SLIME = ( 1 << 6 ), // blocked by opaque water
	TR_MASK_GRATE = ( 1 << 7 ), // blocked by grates, normally projectiles go through grates, solids dont
	TR_MASK_PLAYERCLIP = ( 1 << 8 ), // blocked by player clips
	TR_MASK_SMOKEBOMB = ( 1 << 9 ), // blocked by smoke bombs
	TR_MASK_FLOODFILL = ( 1 << 10 ), // flood fill filter
	TR_MASK_FLOODFILLENT = ( 1 << 11 ), // flood fill filter with entities

	// THIS MUST BE LAST
	TR_MASK_START_USER = ( 1 << 24 ),

	// combo masks can be defined separately
	TR_MASK_VISIBLE = TR_MASK_SOLID | TR_MASK_OPAQUE | TR_MASK_SLIME, // can see but not necessarily shoot
};

// struct: BotUserData
//		Generic data structure that uses a union to hold various types
//		of information.
struct obUserData
{
	// enum: DataType
	//		This allows a small level of type safety with the messages
	//		that send BotUserData parameters. It is a good idea to use the
	//		.mDataType parameter so signal which element of the union
	//		is currently being used.
	enum
	{
		dtNone = 0, dtVector, dtString, dtInt, /*dtInt64,*/ dtFloat, dtEntity,
		dt3_4byteFlags, dt3_Strings, dt6_2byteFlags, dt12_1byteFlags
	} DataType;
	union udatatype
	{
		float			mVector[ 3 ];
		void *			mVoidPtrs[ 3 ];
		char *			mCharPtrs[ 3 ];
		const char *	mString;
		int				mInt;
		//int64_t			.mInt64;
		float			mFloat;
		int				mEntity;
		int				m4ByteFlags[ 3 ];
		short			m2ByteFlags[ 6 ];
		char			m1ByteFlags[ 12 ];
	} udata;
	// Easy Constructors for C++
#ifdef __cplusplus
	obUserData() : DataType( dtNone )
	{
	};
	obUserData( const char * _str ) : DataType( dtString )
	{
		udata.mString = _str;
	};
	obUserData( int _int ) : DataType( dtInt )
	{
		udata.mInt = _int;
	};
	//obUserData(int64_t _int) : DataType(dtInt64) { udata.mInt64 = _int; };
	obUserData( float _float ) : DataType( dtFloat )
	{
		udata.mFloat = _float;
	};
	obUserData( const GameEntity &_ent ) : DataType( dtEntity )
	{
		udata.mEntity = _ent.AsInt();
	};
	obUserData( float _x, float _y, float _z ) :
		DataType( dtVector )
	{
		udata.mVector[ 0 ] = _x;
		udata.mVector[ 1 ] = _y;
		udata.mVector[ 2 ] = _z;
	};
	obUserData( float *_v ) :
		DataType( dtVector )
	{
		udata.mVector[ 0 ] = _v[ 0 ];
		udata.mVector[ 1 ] = _v[ 1 ];
		udata.mVector[ 2 ] = _v[ 2 ];
	};
	obUserData( int _v0, int _v1, int _v2 ) : DataType( dt3_4byteFlags )
	{
		udata.m4ByteFlags[ 0 ] = _v0;
		udata.m4ByteFlags[ 1 ] = _v1;
		udata.m4ByteFlags[ 2 ] = _v2;
	};
	obUserData( char *_v0, char *_v1, char *_v2 ) : DataType( dt3_Strings )
	{
		udata.mCharPtrs[ 0 ] = _v0;
		udata.mCharPtrs[ 1 ] = _v1;
		udata.mCharPtrs[ 2 ] = _v2;
	};
	obUserData( short _v0, short _v1, short _v2, short _v3, short _v4, short _v5 ) :
		DataType( dt6_2byteFlags )
	{
		udata.m2ByteFlags[ 0 ] = _v0;
		udata.m2ByteFlags[ 1 ] = _v1;
		udata.m2ByteFlags[ 2 ] = _v2;
		udata.m2ByteFlags[ 3 ] = _v3;
		udata.m2ByteFlags[ 4 ] = _v4;
		udata.m2ByteFlags[ 5 ] = _v5;
	};
	obUserData( char _v0, char _v1, char _v2, char _v3, char _v4, char _v5, char _v6, char _v7, char _v8, char _v9, char _v10, char _v11 ) :
		DataType( dt12_1byteFlags )
	{
		udata.m1ByteFlags[ 0 ] = _v0;
		udata.m1ByteFlags[ 1 ] = _v1;
		udata.m1ByteFlags[ 2 ] = _v2;
		udata.m1ByteFlags[ 3 ] = _v3;
		udata.m1ByteFlags[ 4 ] = _v4;
		udata.m1ByteFlags[ 5 ] = _v5;
		udata.m1ByteFlags[ 6 ] = _v6;
		udata.m1ByteFlags[ 7 ] = _v7;
		udata.m1ByteFlags[ 8 ] = _v8;
		udata.m1ByteFlags[ 9 ] = _v9;
		udata.m1ByteFlags[ 10 ] = _v10;
		udata.m1ByteFlags[ 11 ] = _v11;
	};
	// Function: IsNone
	// This <BotUserData> has no type specified
	inline bool IsNone() const
	{
		return ( DataType == dtNone );
	};
	// Function: IsString
	// This <BotUserData> is a char * type
	inline bool IsString() const
	{
		return ( DataType == dtString );
	};
	// Function: Is3String
	// This <BotUserData> is a array of 3 strings
	inline bool Is3String() const
	{
		return ( DataType == dt3_Strings );
	};
	// Function: IsInt
	// This <BotUserData> is an int type
	inline bool IsInt() const
	{
		return ( DataType == dtInt );
	};
	// Function: IsInt64
	// This <BotUserData> is an int type
	//inline bool IsInt64() const { return (DataType == dtInt64); };
	// Function: IsFloat
	// This <BotUserData> is an float type
	inline bool IsFloat() const
	{
		return ( DataType == dtFloat );
	};
	// Function: IsFloatOrInt
	// This <BotUserData> is an float type or an int type
	inline bool IsFloatOrInt() const
	{
		return ( DataType == dtFloat ) || ( DataType == dtInt );
	};
	// Function: IsEntity
	// This <BotUserData> is an <GameEntity> type
	inline bool IsEntity() const
	{
		return ( DataType == dtEntity );
	};
	// Function: IsVector
	// This <BotUserData> is a 3d Vector type
	inline bool IsVector() const
	{
		return ( DataType == dtVector );
	};
	// Function: Is3_4ByteFlags
	// This <BotUserData> is an array of 3 4-byte values
	inline bool Is3_4ByteFlags() const
	{
		return ( DataType == dt3_4byteFlags );
	};
	// Function: Is6_otherByteFlags
	// This <BotUserData> is an array of 6 2-byte values
	inline bool Is6_otherByteFlags() const
	{
		return ( DataType == dt6_2byteFlags );
	};
	// Function: Is12_1ByteFlags
	// This <BotUserData> is an array of 12 1-byte values
	inline bool Is12_1ByteFlags() const
	{
		return ( DataType == dt12_1byteFlags );
	};

	inline const char *GetString() const
	{
		return udata.mString;
	};
	inline int GetInt() const
	{
		return udata.mInt;
	};
	//inline int64_t GetInt64() const { return udata.mInt64; };
	inline float GetFloat() const
	{
		return udata.mFloat;
	};
	inline GameEntity GetEntity() const
	{
		GameEntity e; e.FromInt( udata.mEntity ); return e;
	};
	inline void SetEntity( GameEntity e )
	{
		udata.mEntity = e.AsInt();
	};
	inline const char *GetStrings( int _index ) const
	{
		return udata.mCharPtrs[ _index ];
	};
	inline const float *GetVector() const
	{
		return udata.mVector;
	};
	inline const int *Get4ByteFlags() const
	{
		return udata.m4ByteFlags;
	};
	inline const short *Get2ByteFlags() const
	{
		return udata.m2ByteFlags;
	};
	inline const char *Get1ByteFlags() const
	{
		return udata.m1ByteFlags;
	};
	inline float GetNumAsFloat() const
	{
		if ( IsFloat() )
			return GetFloat();
		if ( IsInt() )
			return (float)GetInt();
		return 0.0f;
	};
	inline int GetNumAsInt() const
	{
		if ( IsFloat() )
			return (int)GetFloat();
		if ( IsInt() )
			return GetInt();
		return 0;
	};

	//////////////////////////////////////////////////////////////////////////
	bool Get( float &_val )
	{
		if ( IsFloat() )
		{
			_val = GetFloat();
			return true;
		}
		return false;
	}
	bool Get( int &_val )
	{
		if ( IsInt() )
		{
			_val = GetInt();
			return true;
		}
		return false;
	}
	bool Get( float *_val )
	{
		if ( IsVector() )
		{
			_val[ 0 ] = GetVector()[ 0 ];
			_val[ 1 ] = GetVector()[ 1 ];
			_val[ 2 ] = GetVector()[ 2 ];
			return true;
		}
		return false;
	}
	bool Get( bool &_val )
	{
		if ( IsInt() )
		{
			_val = GetInt() != 0;
			return true;
		}
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
#endif
};

class KeyVals
{
public:
	enum
	{
		MaxArgs = 32, MaxArgLength = 32, MaxStringLength = 64
	};

	bool SetInt( const char *_key, int _val );
	bool SetFloat( const char *_key, float _val );
	bool SetEntity( const char *_key, GameEntity _val );
	bool SetVector( const char *_key, float _x, float _y, float _z );
	bool SetVector( const char *_key, const float *_v );
	bool SetString( const char *_key, const char *_value );
	bool Set( const char *_key, const obUserData &_value );

	bool GetInt( const char *_key, int &_val ) const;
	bool GetFloat( const char *_key, float &_val ) const;
	bool GetEntity( const char *_key, GameEntity &_val ) const;
	bool GetVector( const char *_key, float &_x, float &_y, float &_z ) const;
	bool GetVector( const char *_key, float *_v ) const;
	bool GetString( const char *_key, const char *&_value ) const;

	void Reset();

	void GetKV( int _index, const char *&_key, obUserData &ud ) const;

	KeyVals();
private:
	char		mKey[ MaxArgs ][ MaxArgLength ];
	char		mString[ MaxArgs ][ MaxStringLength ];
	obUserData	mValue[ MaxArgs ];

	bool SetKeyVal( const char *_key, const obUserData &_ud );
	bool GetKeyVal( const char *_key, obUserData &_ud ) const;
};

class Seconds
{
public:
	int GetMs() const
	{
		return mMs;
	}
	Seconds( float _seconds = 0.f ) : mMs( (int)( _seconds*1000.f ) )
	{
	}
private:
	int		mMs;
};

// struct: TriggerInfo
enum
{
	TriggerBufferSize = 72
};
struct TriggerInfo
{
	// ptr: mTagName
	//		The tagname of this trigger, usually a name given by the mapper.
	char mTagName[ TriggerBufferSize ];
	// ptr: mAction
	//		The name of the action this trigger is performing, mod specific
	char mAction[ TriggerBufferSize ];
	// ptr: mEntity
	//		The entity of this trigger, if available
	GameEntity mEntity;
	// ptr: mActivator
	//		The entity that activated this trigger
	GameEntity mActivator;
#ifdef __cplusplus
	TriggerInfo()
	{
		for ( int i = 0; i < TriggerBufferSize; ++i )
			mTagName[ i ] = mAction[ i ] = 0;
	}
	TriggerInfo( const TriggerInfo &_ti )
	{
		mEntity = _ti.mEntity;
		mActivator = _ti.mActivator;
		for ( int i = 0; i < TriggerBufferSize; ++i )
		{
			mTagName[ i ] = _ti.mTagName[ i ];
			mAction[ i ] = _ti.mAction[ i ];
		}
	}
	TriggerInfo( GameEntity _ent, GameEntity _activator ) :
		mEntity( _ent ), mActivator( _activator )
	{
		mTagName[ 0 ] = mAction[ 0 ] = 0;
	}
#endif
};

// struct: MapGoalDef
struct MapGoalDef
{
	KeyVals			Props;
#ifdef __cplusplus

	void Reset()
	{
		Props.Reset();
	}
	MapGoalDef()
	{
		Reset();
	}
#endif
};

// struct: AutoNavFeature
struct AutoNavFeature
{
	EntityInfo		mEntityInfo;
	int				mTeam;
	float			mPosition[ 3 ];
	float			mFacing[ 3 ];
	float			mTargetPosition[ 3 ];
	AABB			mTargetBounds;
	float			mTravelTime;
	AABB			mBounds;
	bool			mObstacleEntity;
	bool			mBiDirectional;
};

// enumerations: FlagState
enum FlagState
{
	S_FLAG_NOT_A_FLAG = 0,
	S_FLAG_AT_BASE,
	S_FLAG_DROPPED,
	S_FLAG_CARRIED,
	S_FLAG_UNAVAILABLE,
	S_FLAG_UNKNOWN
};

// enumerations: GameState
enum GameState
{
	GAME_STATE_INVALID = 0,
	GAME_STATE_INTERMISSION,
	GAME_STATE_WAITINGFORPLAYERS,
	GAME_STATE_WARMUP,
	GAME_STATE_WARMUP_COUNTDOWN,
	GAME_STATE_PLAYING,
	GAME_STATE_SUDDENDEATH,
	GAME_STATE_SCOREBOARD,
	GAME_STATE_PAUSED,
};

//////////////////////////////////////////////////////////////////////////

class Arguments
{
public:
	enum
	{
		MaxArgs = 64, MaxArgLength = 128,
	};

	Arguments() : mNumArgs( 0 )
	{
		for ( int i = 0; i < MaxArgs; ++i )
			mArgs[ i ][ 0 ] = 0;
	}

	char	mArgs[ MaxArgs ][ MaxArgLength ];
	int		mNumArgs;
};

//////////////////////////////////////////////////////////////////////////

struct obVec3
{
	float x, y, z;
};

//////////////////////////////////////////////////////////////////////////

#endif
