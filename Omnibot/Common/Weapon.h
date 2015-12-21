////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
// file: Weapon
//		This class represents a weapon as a whole, and a container for the
//		various fire modes that it has. Some mods may only use 1 fire mod, while others
//		may support many.
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WEAPON_H__
#define __WEAPON_H__

#include "Base_Messages.h"
#include "PIDController.h"
#include "gmGCRoot.h"
#include "ScriptManager.h"

class Client;
class TargetInfo;

class gmMachine;
class gmTableObject;

//////////////////////////////////////////////////////////////////////////

class WeaponScriptResource : public ScriptResource
{
public:
	virtual bool InitScriptSource( const filePath &_path );
};

//////////////////////////////////////////////////////////////////////////
// class: Weapon
class Weapon : public WeaponScriptResource
{
public:
	friend class gmWeapon;

	typedef enum
	{
		Run,
		Walk,
		Still
	} MoveMode;

	typedef enum
	{
		Stand,
		Crouch,
		Prone,

		Ignore,
		NumStances
	} Stance;

	typedef enum
	{
		None,
		Melee,
		InstantHit,
		Projectile,
		Grenade,
	} WeaponType;

	typedef enum
	{
		RequiresAmmo,
		Waterproof,
		HasClip,
		HasZoom,
		Stealth,
		InheritsVelocity,
		MustBeOnGround,
		FireOnRelease,
		ManageHeat,
		IgnoreReload,
		UseMortarTrajectory,
		RequireTargetOutside,
		RequireShooterOutside,
		ChargeToIntercept,
		MeleeWeapon,
		ManualDetonation, // TODO
		WalkWhileZoomed,
		StopWhileZoomed,
		CrouchToMoveWhenZoomed,

		// THIS MUST STAY LAST
		NumProperties
	} WeaponProperty;

	// class: FireMode
	class WeaponFireMode
	{
	public:
		friend class Weapon;
		friend class gmFireMode;

		//////////////////////////////////////////////////////////////////////////
		enum
		{
			MaxDesirabilities = 4
		};
		struct DesirabilityWindow
		{
			float mMinRange;
			float mMaxRange;
			float mDesirability;
		};
		//////////////////////////////////////////////////////////////////////////
		enum
		{
			MaxBurstWindows = 4
		};
		struct BurstWindow
		{
			int	 mBurstRounds;
			float mMinRange;
			float mMaxRange;
			float mMinBurstDelay;
			float mMaxBurstDelay;
		};
		//////////////////////////////////////////////////////////////////////////

		// function: CalculateDefaultDesirability
		//		Calculates the default desirability for this weapon. Default desirability
		//		is the desirability of a weapon when the bot has no target. Basically his 'idle' weapon
		//
		// Parameters:
		//
		//		None
		//
		// Returns:
		//		float - The default desirability for this weapon
		float CalculateDefaultDesirability( Client *_bot );

		// function: CalculateDesirability
		//		Calculates the desirability for this weapon, based on the current targets info.
		//
		// Parameters:
		//
		//		<TargetInfo> - Known information about the current target.
		//
		// Returns:
		//		float - The desirability for this weapon against the target.
		float CalculateDesirability( Client *_bot, const TargetInfo &_targetinfo );

		// function: UpdateBurstWindow
		//		Updates the burst window currently in use by the weapon burst fire.
		//
		// Parameters:
		//
		//		<TargetInfo> - Known information about the current target.
		//
		// Returns:
		//		None
		void UpdateBurstWindow( const TargetInfo *_targetinfo );

		// function: GetCurrentClip
		//		Number of rounds in the current clip
		//
		// Parameters:
		//
		//		None
		//
		// Returns:
		//		int - rounds in the current clip.
		inline int GetCurrentClip() const
		{
			return mClipCurrent;
		}

		// function: GetMaxClip
		//		Number of max rounds in the current clip
		//
		// Parameters:
		//
		//		None
		//
		// Returns:
		//		int - max rounds in the clip
		inline int GetMaxClip() const
		{
			return mClipMax;
		}

		// function: GetCurrentAmmo
		//		Current amount of ammo for a given ammo type
		//
		// Parameters:
		//
		//		None
		//
		// Returns:
		//		int - number of rounds
		inline int GetCurrentAmmo() const
		{
			return mAmmoCurrent;
		}

		// function: GetMaxAmmo
		//		Max amount of ammo for a given ammo type
		//
		// Parameters:
		//
		//		None
		//
		// Returns:
		//		int - max number of rounds
		inline int GetMaxAmmo() const
		{
			return mAmmoMax;
		}

		// function: GetLowAmmoThreshold
		//		Get the script defined value at which the bot should attempt to resupply ammo.
		//
		// Parameters:
		//
		//		None
		//
		// Returns:
		//		int - max number of total ammo
		inline int GetLowAmmoThreshold() const
		{
			return mLowAmmoThreshold;
		}

		// function: GetLowAmmoPriority
		//		If the weapon needs ammo, what's the priority for it.
		//
		// Parameters:
		//
		//		None
		//
		// Returns:
		//		float - priority if the weapon needs ammo
		inline float GetLowAmmoPriority() const
		{
			return mLowAmmoPriority;
		}

		inline int GetLowAmmoGetAmmoAmount() const
		{
			return mLowAmmoGetAmmoAmount;
		}
		// function: HasAmmo
		//		Checks if this weapon currently has enough ammo to fire.
		//
		// Parameters:
		//
		//		None
		//
		// Returns:
		//		bool - true if it has enough ammo to fire, false if not
		bool HasAmmo( int _amount = 0 ) const;

		// function: NeedsAmmo
		//		Checks if this weapon currently needs ammo to use.
		//
		// Parameters:
		//
		//		None
		//
		// Returns:
		//		bool - true if this weapon needs ammo to fire, false if not
		bool NeedsAmmo() const;

		// function: FullClip
		//		Checks if this weapon currently has a full clip.
		//
		// Parameters:
		//
		//		None
		//
		// Returns:
		//		bool - true if it has a full clip, false if not
		bool FullClip() const;

		// function: EmptyClip
		//		Checks if this weapon currently has an empty clip.
		//
		// Parameters:
		//
		//		None
		//
		// Returns:
		//		bool - true if it has an empty clip, false if not
		bool EmptyClip() const;

		bool UsesClip() const;

		// function: EnoughAmmoToReload
		//		Checks if there is enough ammo in the bots inventory to reload this weapon.
		//
		// Parameters:
		//
		//		None
		//
		// Returns:
		//		bool - true if there is enough ammo to reload, false if not.
		bool EnoughAmmoToReload() const;

		Vector3f GetAimPoint( Client *_bot, const GameEntity &_target, const TargetInfo &_targetinfo );
		void AddAimError( Client *_bot, Vector3f &_aimpoint, const TargetInfo &_info );

		bool SetDesirabilityWindow( float _minrange, float _maxrange, float _desir );
		bool SetBurstWindow( float _minrange, float _maxrange, int _burst, float _mindelay, float _maxdelay );

		float GetWeaponBias() const
		{
			return mWeaponBias;
		}

		float GetTargetBias( const EntityInfo& classInfo );

		void SetTargetBias( uint16_t classId, float bias );

		BitFlag64 & GetIgnoreEntFlags()
		{
			return mTargetEntFlagIgnore;
		}

		bool IsCharging() const;
		bool HasChargeTimes() const;
		bool IsBurstDelaying() const;

		//////////////////////////////////////////////////////////////////////////
		inline int GetChargeTime() const
		{
			return mChargeTime;
		}
		inline int GetBurstTime() const
		{
			return mBurstTime;
		}
		inline int GetBurstRound() const
		{
			return mBurstRound;
		}
		inline const BurstWindow &GetBurstWindow() const
		{
			return mBurstWindows[ mCurrentBurstWindow ];
		}

		//////////////////////////////////////////////////////////////////////////
		// Events the fire mode can respond to.
		void OnPreFire( Weapon *_weapon, Client *_client, const TargetInfo *_target );
		void OnStartShooting( Weapon *_weapon, Client *_client );
		void OnStopShooting( Weapon *_weapon, Client *_client );
		void OnReload( Weapon *_weapon, Client *_client );
		bool OnNeedToReload( Weapon *_weapon, Client *_client );
		void OnChargeWeapon( Weapon *_weapon, Client *_client );
		void OnZoomWeapon( Weapon *_weapon, Client *_client );
		void OnShotFired( Weapon *_weapon, Client *_client, GameEntity _projectile = GameEntity() );

		//////////////////////////////////////////////////////////////////////////
		// Utility
		bool IsDefined() const;
		bool CheckDefined() const;
		bool CheckFlag( int32_t _flag ) const
		{
			return BitFlag32( mWeaponFlags ).CheckFlag( _flag );
		}
		void SetFlag( int32_t _flag, bool _set )
		{
			BitFlag32 bf( mWeaponFlags ); bf.SetFlag( _flag, _set ); mWeaponFlags = bf.GetRawFlags();
		}

		gmGCRoot<gmUserObject> GetScriptObject( gmMachine *_machine ) const;

		WeaponFireMode & operator=( const WeaponFireMode &_rh );

		static void Bind( gmMachine *_m );

		WeaponFireMode();
		~WeaponFireMode();
	private:
		// var: mWeaponType
		//		The enumerated type of the weapon.
		WeaponType	 mWeaponType;

		// var: mWeaponFlags
		//		Bit flags representing weapon properties.
		int			 mWeaponFlags;

		int			 mShootButton;
		int			 mZoomButton;

		float		 mProjectileSpeed;
		float		 mProjectileGravity;

		float		 mMinChargeTime;
		float		 mMaxChargeTime;

		float		 mMinLeadError;
		float		 mMaxLeadError;

		float		 mDelayAfterFiring;

		float		 mFuseTime;
		float		 mSplashRadius;

		float		 mPitchOffset;

		int			 mLowAmmoThreshold;
		float		 mLowAmmoPriority;
		int			 mLowAmmoGetAmmoAmount;

		PIDController mHeatController;

		typedef std::map<uint16_t,float> TargetBiasMap;
		TargetBiasMap mTargetBias;

		//////////////////////////////////////////////////////////////////////////
		DesirabilityWindow mDesirabilities[ MaxDesirabilities ];

		BurstWindow		 mBurstWindows[ MaxBurstWindows ];
		int				 mCurrentBurstWindow;
		//////////////////////////////////////////////////////////////////////////

		// var: mAmmoCurrent
		//		The current amount of ammo this weapon has.
		int			 mAmmoCurrent;

		// var: mAmmoMax
		//		The max amount of ammo this weapon can use
		int			 mAmmoMax;

		// var: mClipCurrent
		//		The ammo count in the current clip.
		int			 mClipCurrent;

		// var: mClipMax
		//		The ammo count in the max clip.
		int			 mClipMax;

		// var: mAimOffset
		//		The <Vector3> aim offset that should be applied to weapon aim points to modify targeting.
		Vector3f	 mAimOffset;

		// var: mAimErrorMax
		//		The max <Vector2> aim error range to apply to aiming. horizontal(x), vertical(y)
		Vector2f	 mAimErrorMax;

		// var: mAimErrorCurrent
		//		The current <Vector3> aim error used to apply a error to the aim functions points.
		Vector3f	 mAimErrorCurrent;

		// var: mMinAimAdjustmentDelay;
		//		The min time between aim adjustments.
		float		 mMinAimAdjustmentSecs;

		// var: mMaxAimAdjustmentDelay;
		//		The max time between aim adjustments.
		float		 mMaxAimAdjustmentSecs;

		// var: mNextAimAdjustmentTime
		//		The time the next aim adjustment should take place.
		int			 mNextAimAdjustmentTime;

		// var: mLastDesirability
		//		The last desirability calculated for this weapon.
		float		 mLastDesirability;

		// var: mDefaultDesirability
		//		The last default desirability calculated for this weapon.
		float		 mDefaultDesirability;

		// var: mWeaponBias
		//		The mClient's current bias for this weapon.
		float		 mWeaponBias;

		// var: mTargetEntFlagIgnore
		//		If any entity flags exist on a target, this weapon should ignore it
		BitFlag64	 mTargetEntFlagIgnore;

		// For scripting
		mutable gmGCRoot<gmUserObject> mScriptObject;

		gmGCRoot<gmFunctionObject>	 mscrCalcDefDesir;
		gmGCRoot<gmFunctionObject>	 mscrCalcDesir;
		gmGCRoot<gmFunctionObject>	 mscrCalcAimPoint;
	private:

		Vector3f _GetAimPoint_Melee( Client *_bot, const GameEntity &_target, const TargetInfo &_targetinfo );
		Vector3f _GetAimPoint_InstantHit( Client *_bot, const GameEntity &_target, const TargetInfo &_targetinfo );
		Vector3f _GetAimPoint_Projectile( Client *_bot, const GameEntity &_target, const TargetInfo &_targetinfo );
		Vector3f _GetAimPoint_Grenade( Client *_bot, const GameEntity &_target, const TargetInfo &_targetinfo );

		// Internal variables
		int	 mChargeTime;
		int	 mDelayChooseTime;
		int	 mBurstTime;
		int	 mBurstRound;

		struct
		{
			bool Charging : 1;
		} fl;

		// script binding helpers
		static bool getType( Weapon::WeaponFireMode *a_native, gmThread *a_thread, gmVariable *a_operands );
		static bool setType( Weapon::WeaponFireMode *a_native, gmThread *a_thread, gmVariable *a_operands );
		static bool getMaxAimError( Weapon::WeaponFireMode *a_native, gmThread *a_thread, gmVariable *a_operands );
		static bool setMaxAimError( Weapon::WeaponFireMode *a_native, gmThread *a_thread, gmVariable *a_operands );
		static bool getAimOffset( Weapon::WeaponFireMode *a_native, gmThread *a_thread, gmVariable *a_operands );
		static bool setAimOffset( Weapon::WeaponFireMode *a_native, gmThread *a_thread, gmVariable *a_operands );
	};

	void Update( FireMode _mode );

	struct MoveOptions
	{
		bool	MustWalk;
		bool	MustStop;
		bool	CrouchToMove;

		MoveOptions()
			: MustWalk( false )
			, MustStop( false )
			, CrouchToMove( false )
		{
		}
	};
	MoveOptions UpdateMoveMode();

	// function: UpdateClipAmmo
	//		Updates the clip ammo stats for the weapon from the game.
	//
	// Parameters:
	//
	//		None
	//
	// Returns:
	//		None
	void UpdateClipAmmo( FireMode _mode = Primary );

	// function: UpdateAmmo
	//		Updates the ammo stats for the weapon from the game, using the
	//		currently configured weapon id and ammo id.
	//
	// Parameters:
	//
	//		None
	//
	// Returns:
	//		None
	void UpdateAmmo( FireMode _mode = Primary );

	// function: ReloadWeapon
	//		Initiates a reload action on the weapon.
	//
	// Parameters:
	//
	//		None
	//
	// Returns:
	//		None
	void ReloadWeapon( FireMode _mode = Primary );

	// function: ChargeWeapon
	//		Initiates a charge action on the weapon.
	//
	// Parameters:
	//
	//		None
	//
	// Returns:
	//		None
	void ChargeWeapon( FireMode _mode = Primary );

	// function: ZoomWeapon
	//		Initiates a zoom action on the weapon.
	//
	// Parameters:
	//
	//		None
	//
	// Returns:
	//		None
	void ZoomWeapon( FireMode _mode = Primary );

	// function: ShotFired
	//		Notifies the weapon that it fired a shot.
	//
	// Parameters:
	//
	//		None
	//
	// Returns:
	//		None
	void ShotFired( FireMode _mode, GameEntity _projectile = GameEntity() );

	// function: Select
	//		Called when the weapon is initially selected, giving the weapon a
	//		chance to do any internal initialization.
	//
	// Parameters:
	//
	//		None
	//
	// Returns:
	//		bool - true if weapon is charged, false if still charging.
	void Select();

	bool SelectionLocked();

	bool CanShoot( FireMode _mode, const TargetInfo &_targetinfo );

	// function: PreShoot
	//		Called during the time the weapon has been chosen and the bot
	//		is attempting to fire at the enemy. Target won't necessarily be lined up yet.
	//
	// Parameters:
	//
	//		<FireMode> - FireMode that is getting ready to shoot.
	//		<TargetInfo> - Optional target being fired at
	//		bool - If the shot is lined up yet.
	//
	// Returns:
	//		bool - true if weapon is charged, false if still charging.
	void PreShoot( FireMode _mode, bool _facingTarget = false, const TargetInfo *_target = 0 );

	// function: Shoot
	//		Initiates a shoot action on the weapon.
	//
	// Parameters:
	//
	//		<TargetInfo> - Optional target being fired at
	//
	// Returns:
	//		None
	void Shoot( FireMode _mode = Primary, const TargetInfo *_target = 0 );

	// function: StopShooting
	//		Cancels a shooting action for the weapon.
	//
	// Parameters:
	//
	//		None
	//
	// Returns:
	//		None
	void StopShooting( FireMode _mode = Primary );

	// function: GetAimPoint
	//		Calculates an aim point for this weapon based on a targets information.
	//
	// Parameters:
	//
	//		_ent - The <GameEntity> we're getting the aim point for.
	//		_targetinfo - The <TargetInfo> for the game entity.
	//
	// Returns:
	//		Vector3 - The point we should attempt to aim for this weapon.
	Vector3f GetAimPoint( FireMode _mode, const GameEntity &_target, const TargetInfo &_targetinfo );

	// function: AddAimError
	//		Adds an error offset to an aim point vector.
	//
	// Parameters:
	//
	//		_aimpoint - The <Vector3> aim point we should apply an error to.
	//
	// Returns:
	//		None
	void AddAimError( FireMode _mode, Vector3f &_aimpoint, const TargetInfo &_info );

	// function: CanReload
	//		Checks if either fire mode of this weapon can reload(not full clip), and we have ammo to do it.
	//
	// Parameters:
	//
	//		None
	//
	// Returns:
	//		FireMode - Which fire mode if any needs to reload.
	FireMode CanReload();

	// function: IsClipEmpty
	//		Checks if either fire mode of this weapon is currently empty.
	//
	// Parameters:
	//
	//		None
	//
	// Returns:
	//		FireMode - Which fire mode is empty.
	FireMode IsClipEmpty();
	FireMode OutOfAmmo();

	// function: GetWeaponID
	//		Gets the weapon Id for this weapon.
	//
	// Parameters:
	//
	//		None
	//
	// Returns:
	//		int - The weapon Id
	inline int GetWeaponID() const
	{
		return mWeaponID;
	}
	inline int GetWeaponAliasID() const
	{
		return mWeaponAliasID;
	}

	// function: GetWeaponName
	//		Gets the weapon name for this weapon.
	//
	// Parameters:
	//
	//		None
	//
	// Returns:
	//		std::string - The weapon name.
	uint32_t GetWeaponNameHash() const
	{
		return mWeaponNameHash;
	}
	std::string GetWeaponName() const;
	void SetWeaponName( const char *_name );
	gmGCRoot<gmUserObject> GetScriptObject( gmMachine *_machine ) const;

	WeaponFireMode &GetFireMode( FireMode _mode )
	{
		return mFireModes[ _mode ];
	}
	const WeaponFireMode &GetFireMode( FireMode _mode ) const
	{
		return mFireModes[ _mode ];
	}

	void GetSpectateMessage( std::stringstream &_outstring );

	float CalculateDefaultDesirability();
	float CalculateDesirability( const TargetInfo &_targetinfo );

	float LowOnAmmoPriority( FireMode _mode, int &_ammotype, int &_getammo );

	bool IsWeapon( int _id ) const
	{
		return GetWeaponID() == _id || GetWeaponAliasID() == _id;
	}

	static FireMode GetFireMode( int _mode );

	static void Bind( gmMachine *_m );

	Weapon( Client *_client = 0 );
	Weapon( Client *_client, const Weapon *_wpn );
	~Weapon();
protected:
	// var: mClient
	//		The client that owns this weapon evaluator.
	Client			*	mClient;

	// var: mWeaponID
	//		The weapon type Id that this weapon uses. See global WEAPON table.
	int					mWeaponID;
	int					mWeaponAliasID;

	int					mWeaponLockTime;

	//////////////////////////////////////////////////////////////////////////

	ParamsWeaponLimits	mWeaponLimits;

	// var: mWeaponName
	//		The name for this weapon.
	uint32_t			mWeaponNameHash;

	WeaponFireMode		mFireModes[ NumFireModes ];

	// var: mMinUseTime
	//		The minimum time to use a weapon when equipped.
	float				mMinUseTime;

	int					mLastAmmoUpdate;

	// For scripting
	mutable gmGCRoot<gmUserObject> mScriptObject;
private:
	bool _MeetsRequirements( FireMode mode );
	bool _MeetsRequirements( FireMode mode, const TargetInfo &_targetinfo );

	// script binding helpers
	static bool getName( Weapon *a_native, gmThread *a_thread, gmVariable *a_operands );
	static bool setName( Weapon *a_native, gmThread *a_thread, gmVariable *a_operands );
	static bool getPrimaryFire( Weapon *a_native, gmThread *a_thread, gmVariable *a_operands );
	static bool getSecondaryFire( Weapon *a_native, gmThread *a_thread, gmVariable *a_operands );
};

//////////////////////////////////////////////////////////////////////////
// typedef: WeaponPtr
//		Shorthand typedef for a smart weapon pointer.
typedef std::shared_ptr<Weapon> WeaponPtr;
//////////////////////////////////////////////////////////////////////////

#endif
