////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __MAPGOAL_H__
#define __MAPGOAL_H__

#include <algorithm>
#include <boost/dynamic_bitset.hpp>

#include "GoalManager.h"
#include "ScriptManager.h"
#include "PropertyBinding.h"
#include "TrackablePtr.h"
#include "gmGCRoot.h"
#include "gmbinder2/gmbinder2.h"

typedef boost::dynamic_bitset<uint32_t> DynBitSet32;

class Client;
class GoalQueue;
class gmMachine;
class gmUserObject;

// class: MapGoal
//		removed from the triggermanager slots.
class MapGoal : public Trackable, public PropertyBinding
{
public:
	//////////////////////////////////////////////////////////////////////////
#define PROPERTY_BOOL(name) \
public: \
	bool Get##name() const { return (m##name); } \
	void Set##name(bool _b) { (m##name) = _b; } \
private: \
	bool m##name; \
public:
	//////////////////////////////////////////////////////////////////////////
#define PROPERTY_INT(name) \
public: \
	int Get##name() const { return (m##name); } \
	void Set##name(int _b) { (m##name) = _b; } \
private: \
	int m##name; \
public:
	//////////////////////////////////////////////////////////////////////////
#define PROPERTY_FLOAT(name) \
public: \
	float Get##name() const { return (m##name); } \
	void Set##name(float _b) { (m##name) = _b; } \
private: \
	float m##name; \
public:
	//////////////////////////////////////////////////////////////////////////
#define PROPERTY_BITFLAG64(name) \
public: \
	BitFlag64 Get##name() const { return (m##name); } \
	void Set##name(BitFlag64 _b) { (m##name) = _b; } \
private: \
	BitFlag64 m##name; \
public:
	//////////////////////////////////////////////////////////////////////////
#define PROPERTY_INIT(name, def) m##name = def;
#define PROPERTY_PROPOGATE(name) m##name =  _other->m##name;
	//////////////////////////////////////////////////////////////////////////

	PROPERTY_BITFLAG64( DisableWithEntityFlag );
	PROPERTY_BITFLAG64( DeleteWithEntityFlag );
	PROPERTY_FLOAT( RenderHeight );
	PROPERTY_FLOAT( DefaultRenderRadius );
	PROPERTY_INT( DefaultDrawFlags );
	PROPERTY_BOOL( DeleteMe );
	PROPERTY_BOOL( DynamicPosition );
	PROPERTY_BOOL( DynamicOrientation );
	PROPERTY_BOOL( PropertiesBound );
	PROPERTY_BOOL( RemoveWithEntity );
	PROPERTY_BOOL( InterfaceGoal );
	PROPERTY_BOOL( Disabled );
	PROPERTY_BOOL( InUse );
	PROPERTY_BOOL( DisableForControllingTeam );
	PROPERTY_BOOL( DontSave );
	PROPERTY_BOOL( RenderGoal );
	PROPERTY_BOOL( RenderRoutes );
	PROPERTY_BOOL( CreateOnLoad );

	enum DefaultDrawFlags
	{
		DrawName,
		DrawGroup,
		DrawRole,
		DrawBounds,
		DrawRadius,
		DrawInitialAvail,
		DrawCurrentAvail,
		DrawCenterBounds,
		DrawDisabled,
		DrawSynced,
		DrawRandomUsePoint,
		DrawRangeLimit,
		DrawAll = -1,
	};

	typedef enum
	{
		TRACK_INPROGRESS,
		TRACK_INUSE,
		NUM_TRACK_CATS
	} TrackingCat;

	enum FunctionCallback
	{
		ON_INIT,
		ON_UPGRADE,
		ON_RENDER,
		ON_SERIALIZE,
		ON_SETPROPERTY,
		ON_HELP,
		ON_UPDATE,
		ON_HUDDISPLAY,
		ON_CALC_PRIORITY,

		NUM_CALLBACKS,
	};

	struct Route
	{
		MapGoalPtr	mStart;
		MapGoalPtr	mEnd;
		float		mWeight;
	};
	typedef std::vector<Route> Routes;

	// function: GenerateName
	//		Generates a name for this goal, based on a name from the game
	//		or an auto generated name from the goal type
	void GenerateName( int _instance = 0, bool _skipdupecheck = false );

	// function: Init
	//		Initializes the starting state of the goal
	void InternalInitEntityState();
	bool InternalInit( gmGCRoot<gmTableObject> &_propmap, bool _newgoal );

	// function: Update
	//		Performs any updates for the map goal
	void Update();

	// function: SetAvailabilityFlags
	//		Sets the availability flags of this goal
	inline void SetAvailabilityTeams( int _flags )
	{
		mAvailableTeams = BitFlag32( _flags );
	}

	inline void SetAvailabilityTeamsInit( int _flags )
	{
		mAvailableTeamsInit = BitFlag32( _flags );
	}

	// function: SetAvailable
	//		Set the availability status for a certain team
	void SetAvailable( int _team, bool _available );

	// function: IsAvailable
	//		Check the availability status for a certain team
	bool IsAvailable( int _team ) const;

	// function: SetAvailable
	//		Set the availability status for a certain team
	void SetAvailableInitial( int _team, bool _available );

	// function: IsAvailable
	//		Check the availability status for a certain team
	bool IsAvailableInitial( int _team ) const;

	BitFlag32 GetAvailableFlags() const
	{
		return mAvailableTeams;
	}

	// function: SetEntity
	//		Set the <GameEntity> that represents this goal
	inline void SetEntity( GameEntity _ent )
	{
		mEntity = _ent;
	}

	// function: GetEntity
	//		Get the <GameEntity> that represents this goal
	inline const GameEntity GetEntity() const
	{
		return mEntity;
	}

	// function: SetOwner
	//		Set the <GameEntity> that represents the owner
	inline void SetOwner( GameEntity _ent )
	{
		mCurrentOwner = _ent;
	}

	// function: GetOwner
	//		Get the <GameEntity> that represents the owner
	inline const GameEntity GetOwner() const
	{
		return mCurrentOwner;
	}

	// function: SetTagName
	//		Sets the tag name of this goal
	inline void SetTagName( const std::string &_name )
	{
		mTagName = _name;
	}

	// function: GetTagName
	//		Gets the tag name of this goal
	inline const std::string &GetTagName()
	{
		return mTagName;
	}

	// function: SetPosition
	//		Sets the position of this goal
	void SetPosition( const Vector3f &_pos );

	// function: GetPosition
	//		Gets the position of this goal
	const Vector3f &GetPosition();

	// function: SetFacing
	//		Sets the facing of this goal
	void SetFacing( const Vector3f &_facing );

	// function: GetFacing
	//		Gets the facing of this goal
	Vector3f GetFacing();

	// function: SetMatrix
	//		Sets the matrix of this goal
	void SetMatrix( const Matrix3f &_mat );

	// function: GetMatrix
	//		Gets the matrix of this goal
	Matrix3f GetMatrix();

	// function: SetGoalBounds
	//		Sets the bounds of the goal.
	void SetGoalBounds( const AABB &_bounds );

	// function: GetGoalBounds
	//		Gets the bounds of the goal in world space.
	Box3f GetWorldBounds();

	// function: GetGoalBounds
	//		Gets the bounds of the goal.
	const AABB &GetLocalBounds() const;

	// function: SetRadius
	//		Sets the radius of this goal
	inline void SetRadius( float _rad )
	{
		mRadius = _rad;
	}

	inline void SetMinRadius( float _rad )
	{
		mMinRadius = _rad;
	}

	// function: GetRadius
	//		Gets the radius of this goal
	inline float GetRadius() const;

	// function: GetName
	//		Gets the name of this goal
	inline const std::string &GetName() const
	{
		return mName;
	}

	void SetGroupName( const std::string &_name )
	{
		mGroupName = _name;
	}
	const std::string &GetGroupName() const
	{
		return mGroupName;
	}

	void SetRoleMask( BitFlag32 _i )
	{
		mRoleMask = _i;
	}
	BitFlag32 GetRoleMask() const
	{
		return mRoleMask;
	}

	void SetDefaultPriority( float _f )
	{
		mDefaultPriority = _f;
	}
	float GetDefaultPriority() const
	{
		return mDefaultPriority;
	}

	float GetPriorityForClient( Client *_client );
	float GetPriorityForClass( int _teamid, int _classId );
	void ResetGoalPriorities();
	void SetPriorityForClass( int _teamid, int _classId, float _priority );

	// function: GetGoalType
	//		Gets the type identifier for this goal
	inline const std::string GetGoalType() const
	{
		return mGoalType;
	}
	inline uint32_t GetGoalTypeHash() const
	{
		return mGoalTypeHash;
	}

	// function: SetNavFlags
	//		Sets the navigation flags for this goal
	inline void SetNavFlags( NavFlags _flags )
	{
		mNavFlags = _flags;
	}

	// function: GetFlags
	//		Gets the navigation flags for this goal
	inline NavFlags GetFlags() const
	{
		return mNavFlags;
	} // deprecated

	inline int32_t GetSlotsOpen( TrackingCat _cat )
	{
		int32_t curUsers = GetCurrentUsers( _cat );
		return GetMaxUsers( _cat ) - curUsers;
	}

	inline int32_t GetCurrentUsers( TrackingCat _cat )
	{
		return GetRefCount( _cat );
	}

	inline void SetMaxUsers( TrackingCat _cat, int32_t _val )
	{
		if ( _cat < NUM_TRACK_CATS )
		{
			mMaxUsers[ _cat ] = _val;
		}
	}
	inline int32_t GetMaxUsers( TrackingCat _cat )
	{
		if ( _cat < NUM_TRACK_CATS )
		{
			return mMaxUsers[ _cat ];
		}
		return 0;
	}

	int32_t GetNumUsePoints() const
	{
		return (int32_t)mLocalUsePoints.size();
	}
	void AddUsePoint( const Vector3f &_pos, bool _relative = false );
	Vector3f GetWorldUsePoint( int32_t _index = -1 );
	void GetAllUsePoints( Vector3List &_pv );

	bool AddRoute_Script( const std::string &_start, const std::string &_end, float _weight );
	bool AddRoute( const MapGoalPtr &_routeStart, const MapGoalPtr &_midpt, float _weight );

	bool RouteTo( Client *_bot, DestinationVector &_dest, float _minradius = 0.f );

	Vector3f CalculateFarthestFacing();

	static void SetPersistentPriorityForClass( const std::string &_exp, int _team, int _class, float _priority );
	void CheckForPersistentPriority();

	// function: GetSerialNum
	//		Unique id for this goal. Useful for blackboard target or other identification.
	inline int32_t GetSerialNum() const
	{
		return mSerialNum;
	}

	void TriggerHandler( const TriggerInfo & )
	{
	};

	// function: GetGoalState
	//		Allows users to query a numeric state value from the goal.
	int GetGoalState() const
	{
		return mGoalState;
	};

	// function: GetControllingTeam
	//		Gets the team currently controlling the goal.
	int GetControllingTeam( int ) const
	{
		return 0;
	}

	void CheckPropertiesBound();

	void BindProperties();

	gmGCRoot<gmUserObject> GetScriptObject( gmMachine *_machine ) const;

	const Routes &GetRoutes() const
	{
		return mRoutes;
	}

	void RenderDebug( bool _editing, bool _highlighted );
	void RenderDefault();

	void SetProperty( const std::string &_propname, const obUserData &_val );

	struct ClassPriority
	{
		enum
		{
			MaxTeams = 4, MaxClasses = 10
		};
		float	Priorities[ MaxTeams ][ MaxClasses ];

		void GetPriorityText( std::string &_txtout ) const;
	};

	void DrawRoute( const obColor _color, float _duration );

	const ClassPriority &GetClassPriorities() const
	{
		return mClassPriority;
	}

	static void Bind( gmMachine *_m );

	enum GoalStateFunction
	{
		GoalStateNone,
		GoalStateFlagState,
		//GoalStateFlagHoldState,
	};

	void FromScriptTable( gmMachine *_machine, gmTableObject *_tbl, bool _caseSense = true );

	gmVariable GetProperty( const char *_name );

	bool GetProperty( const char *_name, Vector3f &_var );
	bool GetProperty( const char *_name, float &_var );
	bool GetProperty( const char *_name, bool &_var );
	bool GetProperty( const char *_name, int &_var );
	bool GetProperty( const char *_name, std::string &_var );
	bool GetProperty( const char *_name, Seconds &_var );

	bool SaveToTable( gmMachine *_machine, gmGCRoot<gmTableObject> &_savetable, ErrorObj &_err );
	bool LoadFromTable( gmMachine *_machine, gmGCRoot<gmTableObject> &_loadtable, ErrorObj &_err );

	MapGoalPtr GetSmartPtr();
	void SetSmartPtr( MapGoalPtr ptr );

	LimitWeapons &GetLimitWeapons()
	{
		return mLimitWeapon;
	}

	void ShowHelp();

	void CopyFrom( MapGoal *_other );
	bool LoadFromFile( const filePath & _file );

	void CreateGuiFromBluePrint( gmMachine *a_machine, gmTableObject *a_schema );
	void HudDisplay();

	int GetRandomUsePoint() const
	{
		return mRandomUsePoint;
	};

	int GetRange() const
	{
		return mRange;
	};
	void SetRange( int _range )
	{
		mRange = _range;
	};

	void CreateGuiFromSchema( gmMachine *a_machine, gmTableObject *a_schema );

	MapGoal( const char *_goaltype );
	~MapGoal();
private:
	std::string		mGoalType;
	uint32_t		mGoalTypeHash;

	BitFlag32		mAvailableTeams;
	BitFlag32		mAvailableTeamsInit;
	int				mControllingTeam;

	BitFlag32		mRoleMask;

	GameEntity		mEntity;
	GameEntity		mCurrentOwner;

	Vector3f		mPosition;
	Vector3f		mInterfacePosition; //cache the auto detected position
	Matrix3f		mOrientation;
	AABB			mLocalBounds; // deprecated
	//BoundingBox	mBounds;

	LimitWeapons	mLimitWeapon;

	float			mRadius;
	float			mMinRadius;

	int32_t			mMaxUsers[ NUM_TRACK_CATS ];

	int32_t			mSerialNum;

	NavFlags		mNavFlags;

	Vector3List		mLocalUsePoints;
	DynBitSet32		mRelativeUsePoints;

	Routes			mRoutes;

	ClassPriority	mClassPriority;
	float			mDefaultPriority;
	float			mRolePriorityBonus;

	int				mGoalState;

	int				mVersion;

	bool			mNeedsSynced;

	EntityInfo		mEntInfo;

	// var: mScriptObject
	//		This objects script instance, so that the object can clear its script
	//		references when deleted.
	mutable gmGCRoot<gmUserObject> mScriptObject;

	gmGCRoot<gmFunctionObject>	mCallback_OnInit;
	gmGCRoot<gmFunctionObject>	mCallback_OnUpgrade;
	gmGCRoot<gmFunctionObject>	mCallback_OnRender;
	gmGCRoot<gmFunctionObject>	mCallback_OnSerialize;
	gmGCRoot<gmFunctionObject>	mCallback_OnSetProperty;
	gmGCRoot<gmFunctionObject>	mCallback_OnHelp;
	gmGCRoot<gmFunctionObject>	mCallback_OnUpdate;
	gmGCRoot<gmFunctionObject>	mCallback_OnHudDisplay;
	gmGCRoot<gmFunctionObject>	mCallback_OnCalcPriority;

	ThreadScoper				mActiveThread[ NUM_CALLBACKS ];

	gmGCRoot<gmStringObject>	mExtraDebugText;

	std::string		mTagName;
	std::string		mName;
	std::string		mGroupName;

	int			mRandomUsePoint; // randomly select a usepoint to use?
	int			mRange;  // distance limited

	//////////////////////////////////////////////////////////////////////////
	// don't allow default ctr
	MapGoal();

	void _Init();

	// internal state updates
	void _UpdateFlagState();
	//void _UpdateFlagHoldState();
	void _CheckControllingTeam();

	// remove when all vectors converted to Vec3
	void SetPosition_Script( const Vec3 &_pos );
	void SetFacing_Script( const Vec3 &_pos );
	Vec3 GetPosition_Script();
	Vec3 GetFacing_Script();
	void SetBounds_Script( const Vec3 &_mins, const Vec3 &_maxs );
	Vec3 GetBoundsCenter_Script();

	void SetRange_Script( const int &_range );
	int GetRange_Script();

	void DrawPathsToGoals();

	MapGoalWPtr	mWeakPtr;
};

typedef TrackablePtr<MapGoal, MapGoal::TRACK_INPROGRESS>	TrackInProgress;
typedef TrackablePtr<MapGoal, MapGoal::TRACK_INUSE>			TrackInUse;

struct Trackers
{
	TrackInProgress		InProgress;
	TrackInUse			InUse;

	void Reset()
	{
		InProgress.Reset();
		InUse.Reset();
	}
};

int32_t GetMapGoalSerial();

#endif
