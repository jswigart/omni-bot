////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __BOTWEAPONSYSTEM_H__
#define __BOTWEAPONSYSTEM_H__

#include "FilterSensory.h"
#include "BotBaseStates.h"
#include "Weapon.h"

class Client;
class MemoryRecord;

// typedef: WeaponList
//		Holds a list of weapons the bot currently has.
typedef std::vector<WeaponPtr> WeaponList;

namespace AiState
{
	// class: AttackTarget
	class AttackTarget : public StateChild, public AimerUser
	{
	public:

		void RenderDebug();
		void GetDebugString( std::stringstream &out );

		float GetPriority();
		void Enter();
		void Exit();
		StateStatus Update( float fDt );

		bool TargetExceedsWeaponLimits() const
		{
			return mTargetExceedsWeaponLimits;
		}

		// AimerUser functions.
		bool GetAimPosition( Vector3f &_aimpos );
		void OnTarget();

		AttackTarget();
	private:
		ParamsWeaponLimits	mWeaponLimits;
		Vector3f			mAimPosition;
		uint32_t			mCurrentWeaponHash;
		bool				mShootTheBastard : 1;
		bool				mTargetExceedsWeaponLimits : 1;
	};

	// class: WeaponSystem
	//		The weapon system is the centerpoint for weapon selection and
	//		firing on a target using the weapons available to the bot.
	class WeaponSystem : public StateFirstAvailable
	{
	public:
		struct WeaponRequest
		{
			Priority::ePriority	 mPriority;
			uint32_t 			 mOwner;
			int					 mWeaponId;

			void Reset();
			WeaponRequest();
		private:
		};

		bool AddWeaponRequest( Priority::ePriority _prio, uint32_t _owner, int _weaponId );
		void ReleaseWeaponRequest( const std::string &_owner );
		void ReleaseWeaponRequest( uint32_t _owner );
		bool UpdateWeaponRequest( uint32_t _owner, int _weaponId );
		int GetWeaponNeedingReload();

		BitFlag128 GetWeaponMask() const
		{
			return mWeaponMask;
		}

		// function: GrabAllWeapons
		//		Gets a copy of all weapons from the weapon database.
		void GrabAllWeapons();

		// function: RefreshWeapon
		//		Refreshes a copy of a particular weapon.
		void RefreshWeapon( int _weaponId );

		// function: RefreshAllWeapons
		//		Refreshes a copy of all weapons from the weapon database.
		void RefreshAllWeapons();

		// function: AddWeaponToInventory
		//		Adds a pointer to a weapon into the inventory weapon list.
		bool AddWeaponToInventory( int _weaponId );

		// function: AddWeapon
		//		Gives a weapon to the bot for it to use
		void AddWeapon( WeaponPtr _weapon );

		// function: RemoveWeapon
		//		Removes a weapon from the bots inventory by its id.
		void RemoveWeapon( int _weaponId );

		// function: ClearWeapons
		//		Strips the bot of all weapons
		void ClearWeapons();

		// function: SelectBestWeapon
		//		Evaluates all the bots current weapons and chooses the best one,
		//		given the current target
		int SelectBestWeapon( GameEntity _targetent = GameEntity() );

		// function: SelectRandomWeapon
		//		Returns weapon id for a random weapon
		int SelectRandomWeapon();

		// function: UpdateWeapons
		void UpdateWeapons();

		// function: UpdateAllWeaponAmmo
		//		Allows each weapon to update the current status of its ammo
		void UpdateAllWeaponAmmo();

		// function: ReadyToFire
		//		Asks the game if the bot is currently ready to fire the current weapon
		bool ReadyToFire();

		// function: IsReloading
		//		Asks the game if the bot is currently reloading
		bool IsReloading();

		// function: ChargeWeapon
		//		Attempts to start 'charging' the current weapon.
		void ChargeWeapon( FireMode _mode = Primary );

		// function: ZoomWeapon
		//		Attempts to zoom the weapon(if HasZoom)
		void ZoomWeapon( FireMode _mode = Primary );

		// function: FireWeapon
		//		Attempts to fire the current weapon
		void FireWeapon( FireMode _mode = Primary );

		// function: HasWeapon
		//		Checks if the bot has this weapon
		bool HasWeapon( int _weaponId ) const;

		// function: HasAmmo
		//		Checks is the current weapon has ammo
		bool HasAmmo( FireMode _mode ) const;

		// function: HasAmmo
		//		Checks if a specific weapon has ammo
		bool HasAmmo( int _weaponid, FireMode _mode = Primary, int _amount = 0 ) const;

		// function: GetCurrentWeaponID
		//		Gets the Id of the current weapon
		inline int GetCurrentWeaponID() const
		{
			return mCurrentWeapon ? mCurrentWeapon->GetWeaponID() : -1;
		}

		// function: GetDesiredWeaponID
		//		Gets the Id of the desired weapon
		inline int GetDesiredWeaponID() const
		{
			return mDesiredWeaponID;
		}

		inline int32_t GetDefaultWeaponID() const
		{
			return mDefaultWeapon;
		}
		inline int32_t GetOverrideWeaponID() const
		{
			return mOverrideWeapon;
		}

		inline void SetOverrideWeaponID( int32_t w )
		{
			mOverrideWeapon = w;
		}
		inline void ClearOverrideWeaponID()
		{
			mOverrideWeapon = 0;
		}

		// function: CurrentWeaponIs
		//		Accessor to check if the current weapon is a particular weapon
		inline bool CurrentWeaponIs( int _weaponid ) const
		{
			const int wId = mCurrentWeapon ? mCurrentWeapon->GetWeaponID() : 0;
			return wId && wId == _weaponid;
		}

		inline bool CurrentWeaponIsAttackReady() const
		{
			if ( mCurrentWeapon )
			{
				return
					mCurrentWeapon->IsWeapon( GetDefaultWeaponID() ) ||
					mCurrentWeapon->IsWeapon( GetDesiredWeaponID() );
			}
			return false;
			/*const int wId = .mCurrentWeapon ? .mCurrentWeapon->GetWeaponID() : 0;
			return wId && (
			wId==GetDefaultWeaponID() ||
			wId==GetOverrideWeaponID() ||
			wId==GetDesiredWeaponID()
			); */
		}

		// function: GetCurrentWeapon
		//		Gets the actual current weapon
		inline WeaponPtr GetCurrentWeapon()
		{
			return mCurrentWeapon;
		}

		// function: GetWeapon
		//		Gets the ptr to a specific weapon, if available.
		WeaponPtr GetWeapon( int _weaponId, bool _inventory = true ) const;

		WeaponPtr GetWeaponByIndex( int _index, bool _inventory = true );

		// function: GetMostDesiredAmmo
		//		Calculates the most desired ammo based on the bots current weapons
		//		fills the parameter with the ammo Id, and returns the desirability of it
		float GetMostDesiredAmmo( int &_weapon, int &_getammo );
		uint32_t GetCurrentRequestOwner() const
		{
			return mCurrentRequestOwner;
		}

		bool CanShoot( const MemoryRecord &_record );

		inline int GetAimPersistance() const
		{
			return mAimPersistance;
		}
		inline void SetAimPersistance( int _aimperms )
		{
			mAimPersistance = _aimperms;
		}
		inline int GetReactionTime() const
		{
			return mReactionTimeInMS;
		}
		inline void SetReactionTime( int _reactionms )
		{
			mReactionTimeInMS = _reactionms;
		}

		void GetSpectateMessage( std::stringstream &_outstring );

		WeaponList &GetWeaponList()
		{
			return mWeaponList;
		}

		// State Functions
		void Initialize();
		float GetPriority();
		void Enter();
		void Exit();
		StateStatus Update( float fDt );

		void GetDebugString( std::stringstream &out );

		const WeaponRequest &GetHighestWeaponRequest();

		WeaponSystem();
		virtual ~WeaponSystem();
	protected:
		// int: .mReactionTimeInMS
		//		Minimum time the bot has to see an opponent before reacting
		int			 mReactionTimeInMS;

		// int: .mAimPersistance
		//		Amount of time target will aim at the last enemy position,
		//		after the target leaves view.
		int			 mAimPersistance;

		BitFlag128	 mWeaponMask;

		// var: .mWeaponList
		//		The list of all the bots current weapons
		WeaponList	 mAllWeaponList;
		WeaponList	 mWeaponList;

		// int: .mDesiredWeaponID
		//		The Id of the currently desired weapon
		int			 mDesiredWeaponID;

		int32_t		 mDefaultWeapon;
		int32_t		 mOverrideWeapon;

		enum
		{
			MaxWeaponRequests = 8
		};
		WeaponRequest	mWeaponRequests[ MaxWeaponRequests ];
		int				mCurrentRequestOwner;

		// var: .mCurrentWeapon
		//		The actual current weapon
		WeaponPtr		mCurrentWeapon;
		
		// function: _UpdateCurrentWeapon
		//		Chooses the current weapon based on the result of <_UpdateWeaponFromGame>
		void _UpdateCurrentWeapon( FireMode _mode );
	};
};
#endif
