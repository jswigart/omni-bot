////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "EventReciever.h"
#include "Base_Messages.h"
#include "StateMachine.h"
#include "NameManager.h"
#include "BlackBoard.h"
#include "FileSystem.h"
#include "GoalManager.h"
#include "TacticalManager.h"
#include "PathPlannerBase.h"

class BotItemSystem;
namespace AiState
{
	class SensoryMemory;
	class TargetingSystem;
	class WeaponSystem;
	class SteeringSystem;
}

class Path;
class gmThread;
class gmTableObject;
class gmMachine;
class gmUserObject;
class gmFunctionObject;

//////////////////////////////////////////////////////////////////////////

// class: Client
class Client : public EventReciever
{
public:
	friend class gmBot;

	typedef enum
	{
		PROFILE_NONE,
		PROFILE_CUSTOM,
		PROFILE_CLASS,

		NUM_PROFILE_TYPES
	} ProfileType;

	typedef enum
	{
		FL_DISABLED = 0,
		FL_SHOOTINGDISABLED,
		FL_SELECTBESTWEAPON_OFF,
		FL_USINGMOUNTEDWEAPON,

		// THIS MUST BE LAST
		NUM_INTERNAL_FLAGS
	} InternalFlags;

	// struct: HoldButtons
	//		Tracks time values that buttons should be held for.
	class HoldButtons
	{
	public:
		enum
		{
			NumButtons = 64
		};
		uint32_t mStopHoldTime[ NumButtons ];

		HoldButtons();
	};

	// Function: GetName
	//		Get the current name of this bot
	const std::string& GetName( bool _clean = false ) const;

	// Function: GetTeam
	//		Get the current team this bot is on
	inline int GetTeam() const
	{
		return mTeam;
	}

	inline int GetClass() const
	{
		return mEntInfo.mClassId;
	}

	// Function: GetGameID
	//		Get the current <GameId> of this bot
	inline GameId GetGameID() const
	{
		return mGameID;
	}

	// Function: GetGameEntity
	//		Get the current <GameEntity> of this bot
	inline const GameEntity &GetGameEntity() const
	{
		return mGameEntity;
	}

	inline const Vector3f &GetPosition() const
	{
		return mPosition;
	}
	inline const Vector3f &GetFacingVector() const
	{
		return mFacingVector;
	}
	inline const Vector3f &GetUpVector() const
	{
		return mUpVector;
	}
	inline const Vector3f &GetRightVector() const
	{
		return mRightVector;
	}
	inline const Vector3f &GetVelocity() const
	{
		return mVelocity;
	}
	inline const Vector3f &GetMovementVector() const
	{
		return mMoveVector;
	}
	inline const AABB &GetLocalBounds() const
	{
		return mLocalBounds;
	}
	inline const Box3f &GetWorldBounds() const
	{
		return mWorldBounds;
	}
	const Matrix3f &GetOrientation() const
	{
		return mOrientation;
	}
	inline Vector3f GetCenterBounds() const
	{
		return mWorldBounds.Center;
	}

	//////////////////////////////////////////////////////////////////////////

	inline void SetMovementVector( const Vector3f &_vec )
	{
		mMoveVector = _vec;
	}

	inline bool InFieldOfView( const Vector3f &_pos );

	bool HasLineOfSightTo( const Vector3f &_pos, GameEntity entity = GameEntity(), int customTraceMask = 0 );
	static bool HasLineOfSightTo( const Vector3f &_pos1, const Vector3f &_pos2,
		GameEntity ent = GameEntity(), int _ignoreent = -1, int customTraceMask = 0 );

	bool IsAllied( const GameEntity ent ) const;

	const Vector3f& GetEyePosition();
	Vector3f GetEyeGroundOffset();

	virtual bool CanGetPowerUp( int32_t _powerup ) const;

	inline const BitFlag32 GetMovementCaps() const
	{
		return mMovementCaps;
	}

	inline const BitFlag64 &GetEntityFlags() const
	{
		return mEntInfo.mFlags;
	}
	inline const BitFlag32  &GetPowerUpFlags() const
	{
		return mEntInfo.mPowerUps;
	}
	inline const BitFlag32 &GetRoleMask() const
	{
		return mRoleMask;
	}
	inline void SetRoleMask( BitFlag32 &_bf )
	{
		mRoleMask = _bf;
	}

	inline bool HasEntityFlag( int32_t _flag ) const
	{
		return mEntInfo.mFlags.CheckFlag( _flag );
	}
	inline bool HasPowerup( int32_t _flag ) const
	{
		return mEntInfo.mPowerUps.CheckFlag( _flag );
	}

	inline void SetUserFlag( int _flag, bool _enable );
	inline bool CheckUserFlag( int _flag ) const;

	// Function: TurnTowardPosition
	//		Makes the bot turn toward a world space position.
	bool TurnTowardFacing( const Vector3f &_facing );

	// Function: TurnTowardPosition
	//		Makes the bot turn toward a facing.
	bool TurnTowardPosition( const Vector3f &_pos );

	Vector3f ToLocalSpace( const Vector3f &_worldpos );
	Vector3f ToWorldSpace( const Vector3f &_localpos );

	void OutputDebug( MessageType _type, const char * _str );

	// Property Accessors

	// todo: put error checking in here, not gmBot
	inline float GetFieldOfView() const
	{
		return mFieldOfView;
	}
	inline void SetFieldOfView( float _fov )
	{
		mFieldOfView = _fov;
	}
	inline float GetMaxViewDistance() const
	{
		return mMaxViewDistance;
	}
	inline void SetMaxViewDistance( float _dist )
	{
		mMaxViewDistance = _dist;
	}
	inline bool IsWithinViewDistance( const Vector3f &_pos );
	inline float GetMaxTurnSpeed() const
	{
		return mMaxTurnSpeed;
	}
	inline void SetMaxTurnSpeed( float _speed )
	{
		mMaxTurnSpeed = _speed;
	}
	inline float GetAimStiffness() const
	{
		return mAimStiffness;
	}
	inline void SetAimStiffness( float _stiffness )
	{
		mAimStiffness = _stiffness;
	}
	inline float GetAimDamping() const
	{
		return mAimDamping;
	}
	inline void SetAimDamping( float _damping )
	{
		mAimDamping = _damping;
	}
	inline float GetAimTolerance() const
	{
		return mAimTolerance;
	}
	inline void SetAimTolerance( float _tolerance )
	{
		mAimTolerance = _tolerance;
	}

	AiState::SensoryMemory		*GetSensoryMemory();
	AiState::SteeringSystem		*GetSteeringSystem();
	AiState::WeaponSystem		*GetWeaponSystem();
	AiState::TargetingSystem	*GetTargetingSystem();

	inline int GetCurrentHealth() const
	{
		return mEntInfo.mHealth.mNum;
	}
	inline int GetMaxHealth() const
	{
		return mEntInfo.mHealth.mMax;
	}
	inline float GetHealthPercent() const;
	inline int GetCurrentArmor() const
	{
		return mEntInfo.mArmor.mNum;
	}
	inline int GetMaxArmor() const
	{
		return mEntInfo.mArmor.mMax;
	}
	inline float GetArmorPercent() const;
	inline float GetStepHeight() const
	{
		return mStepHeight;
	}
	inline float GetMaxSpeed() const
	{
		return mMaxSpeed;
	}

	virtual void Init( int _gameid );
	virtual void Update();
	virtual void UpdateBotInput();
	virtual void Shutdown();

	gmUserObject *GetScriptObject();
	gmVariable GetScriptVariable();

	// Function: ChangeTeam
	//		Tells the bot to change to a different team.
	virtual void ChangeTeam( int _team );

	// Function: ChangeClass
	//		Tells the bot to change to a different class.
	virtual void ChangeClass( int _class );

	void LoadProfile( ProfileType _type );
	void ClearProfile();

	void PressButton( int button );
	void ReleaseButton( int button );
	bool IsButtonDown( int button ) const;

	void HoldButton( const BitFlag64 &buttons, uint32_t mstime );
	void ReleaseHeldButton( const BitFlag64 &buttons );
	void ReleaseAllHeldButtons();

	//AimRequestPtr GetAimRequest(const char *_owner);

	void EnableDebug( const int _flag, bool _enable );
	inline bool IsDebugEnabled( int32_t _flag ) const
	{
		return mDebugFlags.CheckFlag( _flag );
	}

	void CheckStuck();
	int GetStuckTime() const;
	void ResetStuckTime();

	virtual void GetNavFlags( NavFlags & includeFlags, NavFlags & excludeFlags ) = 0;
	virtual NavFlags GetTeamFlag() const
	{
		return GetTeamFlag( GetTeam() );
	}
	virtual NavFlags GetTeamFlag( int _team ) const = 0;
	inline BlackBoard &GetBB()
	{
		return mBlackboard;
	}

	void GameCommand( CHECK_PRINTF_ARGS const char* _msg, ... );

	// These helper functions allow the derived class to perform additional
	// actions as a result of mode specific flags on the waypoint. This allow handling
	// of custom nav flags without subclassing the actual goals.
	virtual void ProcessGotoNode( const PathInterface::PathCorner edges[ 2 ], const size_t numEdges )
	{
	}
	virtual void ProcessGotoNode( const Path &_path )
	{
	}
	
	virtual void ProcessStimulusBehavior( Behaviors& behaviors, const StimulusPtr& stim );

	// Game specific variables.
	typedef enum
	{
		JumpGapOffset,
	} GameVar;

	virtual float GetGameVar( GameVar _var ) const = 0;

	virtual void SendVoiceMacro( int _macroId ) = 0;
	virtual int HandleVoiceMacroEvent( const EvVoiceMacro* msg )
	{
		return 0;
	}

	virtual bool DoesBotHaveFlag( MapGoalPtr _mapgoal ) = 0;
	virtual bool IsFlagGrabbable( MapGoalPtr _mapgoal )
	{
		return true;
	}

	virtual bool CanBotSnipe()
	{
		return false;
	}
	virtual bool GetSniperWeapon( int &nonscoped, int &scoped )
	{
		nonscoped = 0; scoped = 0; return false;
	}
	virtual bool GetSkills( gmMachine *machine, gmTableObject *tbl )
	{
		machine; tbl; return false;
	}

	inline int GetProfileType() const
	{
		return mProfileType;
	}

	State *GetStateRoot()
	{
		return mStateRoot;
	}

	void InitBehaviorTree();
	bool AddScriptGoal( const std::string &_name );
	void InitScriptGoals();
	virtual void SetupBehaviorTree()
	{
	}

	int			 mDesiredTeam;
	int			 mDesiredClass;

	void CheckTeamEvent();
	void CheckClassEvent();

	void PropogateDeletedThreads( const int *_threadIds, int _numThreads );

	bool DistributeUnhandledCommand( const StringVector & args );

	Client();
	virtual ~Client();
protected:

	float			mStepHeight;
	float			mMaxSpeed;

	AABB			mStuckBounds;
	int				mStuckTime;

	State*			mStateRoot;

	ClientInput		mClientInput;

	void ProcessEvent( const Message & message, CallbackParameters & cb );
	void ProcessEventImpl( const Message & message, uint32_t targetState );
private:
	Vector3f		mPosition;
	Vector3f		mEyePosition;
	Vector3f		mMoveVector;
	Vector3f		mVelocity;
	Vector3f		mFacingVector;
	Vector3f		mUpVector;
	Vector3f		mRightVector;
	AABB			mLocalBounds;
	Box3f			mWorldBounds;
	Matrix3f		mOrientation;

	GameEntity		mMoveEntity;

	BitFlag64		mButtonFlags;

	float			mFieldOfView;
	float			mMaxViewDistance;

	// Bot Properties
	EntityInfo		mEntInfo;
	BitFlag32		mMovementCaps;
	BitFlag32		mRoleMask;
	BitFlag64		mInternalFlags;

	int				mTeam;
	GameId			mGameID;
	GameEntity		mGameEntity;

	gmUserObject *	mScriptObject;

	// Aiming properties
	float			mCurrentTurnSpeed;
	float			mMaxTurnSpeed;
	float			mAimStiffness;
	float			mAimDamping;
	float			mAimTolerance;

	HoldButtons		mHoldButtons;

	ProfileType		mProfileType;

	BlackBoard		mBlackboard;

	File			mDebugLog;
	BitFlag32		mDebugFlags;

	mutable std::string		mName;
	NamePtr					mNameReference;
};

inline float Client::GetHealthPercent() const
{
	return mEntInfo.mHealth.Percent();
}

inline float Client::GetArmorPercent() const
{
	return mEntInfo.mArmor.Percent();
}

inline bool Client::InFieldOfView( const Vector3f &_pos )
{
	Vector3f toTarget = _pos - GetEyePosition();
	toTarget.Normalize();
	return Utils::InFieldOfView2d( mFacingVector, toTarget, mFieldOfView );
}

inline bool Client::IsWithinViewDistance( const Vector3f &_pos )
{
	return ( ( _pos - mPosition ).SquaredLength() <= ( mMaxViewDistance* mMaxViewDistance ) );
}

inline void Client::SetUserFlag( int _flag, bool _enable )
{
	if ( _enable )
		mInternalFlags.SetFlag( _flag );
	else
		mInternalFlags.ClearFlag( _flag );
}

inline bool Client::CheckUserFlag( int _flag ) const
{
	return mInternalFlags.CheckFlag( _flag );
}

#endif
