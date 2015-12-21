////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "BotTargetingSystem.h"
#include "BotWeaponSystem.h"
#include "Client.h"
#include "WeaponDatabase.h"
#include "ScriptManager.h"
#include "BotBaseStates.h"
#include "RenderBuffer.h"
#include "System.h"

namespace AiState
{
	//////////////////////////////////////////////////////////////////////////
	class ReloadOther : public StateChild
	{
	public:
		void GetDebugString( std::stringstream &out )
		{
			out << gWeaponDatabase.GetWeaponName( mWeaponNeedsReloading );
		}
		float GetPriority()
		{
			FINDSTATE( tsys, TargetingSystem, GetParent()->GetParent() );
			if ( !tsys || tsys->HasTarget() || GetClient()->CheckUserFlag( Client::FL_USINGMOUNTEDWEAPON ) )
				return 0.f;

			FINDSTATE( wsys, WeaponSystem, GetParent() );
			if ( wsys )
			{
				int wpn = wsys->GetWeaponNeedingReload();
				if ( wpn != mWeaponNeedsReloading )
				{
					if ( wpn && mWeaponNeedsReloading )
					{
						wsys->UpdateWeaponRequest( GetNameHash(), wpn );
					}
					mWeaponNeedsReloading = wpn;
				}
			}
			return mWeaponNeedsReloading ? 1.f : 0.f;
		}
		void Enter()
		{
			FINDSTATEIF( WeaponSystem, GetParent(), AddWeaponRequest( Priority::Low, GetNameHash(), mWeaponNeedsReloading ) );
		}
		void Exit()
		{
			mWeaponNeedsReloading = 0;
			FINDSTATEIF( WeaponSystem, GetParent(), ReleaseWeaponRequest( GetNameHash() ) );
		}
		StateStatus Update( float fDt )
		{
			{
				FINDSTATE( wsys, WeaponSystem, GetParent() );
				if ( wsys != NULL && wsys->GetCurrentRequestOwner() == GetNameHash() )
				{
					if ( wsys && wsys->CurrentWeaponIs( mWeaponNeedsReloading ) )
						wsys->GetCurrentWeapon()->ReloadWeapon();
				}
				return State_Busy;
			}
		}
		ReloadOther() : StateChild( "ReloadOther" ), mWeaponNeedsReloading( 0 )
		{
		}
	private:
		int mWeaponNeedsReloading;
	};

	//////////////////////////////////////////////////////////////////////////

	AttackTarget::AttackTarget()
		: StateChild( "AttackTarget" )
		, mAimPosition( Vector3f::ZERO )
		, mCurrentWeaponHash( 0 )
		, mShootTheBastard( 0 )
		, mTargetExceedsWeaponLimits( 0 )
	{
	}
	void AttackTarget::RenderDebug()
	{
		if ( mWeaponLimits.mLimited )
		{
			Vector3f vGunCenter( mWeaponLimits.mCenterFacing );
			Vector3f vAimVector( Normalize( mAimPosition - GetClient()->GetEyePosition() ) );

			const float fGunHeading = vGunCenter.XYHeading();
			const float fAimHeading = vAimVector.XYHeading();

			const float fGunPitch = vGunCenter.GetPitch();
			const float fAimPitch = vAimVector.GetPitch();

			float fHeadingDiff = Mathf::RadToDeg( Mathf::UnitCircleNormalize( fAimHeading - fGunHeading ) );
			float fPitchDiff = Mathf::RadToDeg( fAimPitch - fGunPitch );
			if ( fHeadingDiff < mWeaponLimits.mMinYaw || fHeadingDiff > mWeaponLimits.mMaxYaw )
				mTargetExceedsWeaponLimits = true;
			if ( fPitchDiff < mWeaponLimits.mMinPitch || fPitchDiff > mWeaponLimits.mMaxPitch )
				mTargetExceedsWeaponLimits = true;

			Quaternionf ql, qr;
			ql.FromAxisAngle( Vector3f::UNIT_Z, Mathf::DegToRad( mWeaponLimits.mMinYaw ) );
			qr.FromAxisAngle( Vector3f::UNIT_Z, Mathf::DegToRad( mWeaponLimits.mMaxYaw ) );
			Vector3f vLeft = ql.Rotate( vGunCenter );
			Vector3f vRght = qr.Rotate( vGunCenter );

			Vector3f vEye = GetClient()->GetEyePosition();
			RenderBuffer::AddLine( vEye, vEye + vGunCenter*64.f, COLOR::ORANGE );
			RenderBuffer::AddLine( vEye, vEye + vLeft*64.f, COLOR::RED );
			RenderBuffer::AddLine( vEye, vEye + vRght*64.f, COLOR::RED );
		}
	}
	void AttackTarget::GetDebugString( std::stringstream &out )
	{
		out << Utils::HashToString( mCurrentWeaponHash );
	}
	float AttackTarget::GetPriority()
	{
		return GetClient()->GetTargetingSystem()->HasTarget() ? 1.f : 0.f;
	}
	void AttackTarget::Enter()
	{
		//FINDSTATEIF(Aimer, GetRootState(), AddAimRequest(Priority::LowMed, this, GetNameHash()));
	}
	void AttackTarget::Exit()
	{
		mShootTheBastard = false;
		mCurrentWeaponHash = 0;
		FINDSTATEIF( Aimer, GetRootState(), ReleaseAimRequest( GetNameHash() ) );
		FINDSTATEIF( WeaponSystem, GetParent(), ReleaseWeaponRequest( GetNameHash() ) );
	}
	State::StateStatus AttackTarget::Update( float fDt )
	{
		{
			const MemoryRecord *pRecord = GetClient()->GetTargetingSystem()->GetCurrentTargetRecord();
			if ( !pRecord )
				return State_Finished;

			// Add the aim request when reaction time has been met
			FINDSTATE( wsys, WeaponSystem, GetParent() );
			if ( wsys != NULL &&
				( pRecord->GetTimeTargetHasBeenVisible() >= wsys->GetReactionTime() ) &&
				( pRecord->IsShootable() || ( pRecord->GetTimeHasBeenOutOfView() < wsys->GetAimPersistance() ) ) )
			{
				FINDSTATEIF( Aimer, GetRootState(), AddAimRequest( Priority::LowMed, this, GetNameHash() ) );
			}

			return State_Busy;
		}
	}
	bool AttackTarget::GetAimPosition( Vector3f &_aimpos )
	{
		const MemoryRecord *pRecord = GetClient()->GetTargetingSystem()->GetCurrentTargetRecord();
		if ( !pRecord )
		{
			mShootTheBastard = false;
			return false;
		}

		mShootTheBastard = pRecord->IsShootable();

		const GameEntity vTargetEnt = pRecord->GetEntity();
		const TargetInfo &targetInfo = pRecord->mTargetInfo;

		FINDSTATE( wsys, WeaponSystem, GetParent() );
		if ( wsys )
		{
			WeaponPtr wpn = wsys->GetCurrentWeapon();
			if ( wpn )
			{
				mCurrentWeaponHash = wpn->GetWeaponNameHash();

				// Calculate the position the bot will aim at, the weapon itself should account
				// for any leading that may need to take place
				mAimPosition = wpn->GetAimPoint( Primary, vTargetEnt, targetInfo );
				wpn->AddAimError( Primary, mAimPosition, targetInfo );
				_aimpos = mAimPosition;
				
				// Check limits
				mTargetExceedsWeaponLimits = false;

				if ( gEngineFuncs->GetWeaponLimits( GetClient()->GetGameEntity(), wpn->GetWeaponID(), mWeaponLimits ) && mWeaponLimits.mLimited )
				{
					Vector3f vGunCenter( mWeaponLimits.mCenterFacing );
					Vector3f vAimVector( Normalize( _aimpos - GetClient()->GetEyePosition() ) );

					const float fGunHeading = vGunCenter.XYHeading();
					const float fAimHeading = vAimVector.XYHeading();

					const float fGunPitch = vGunCenter.GetPitch();
					const float fAimPitch = vAimVector.GetPitch();

					float fHeadingDiff = Mathf::RadToDeg( Mathf::UnitCircleNormalize( fAimHeading - fGunHeading ) );
					float fPitchDiff = Mathf::RadToDeg( fAimPitch - fGunPitch );
					if ( fHeadingDiff < mWeaponLimits.mMinYaw || fHeadingDiff > mWeaponLimits.mMaxYaw )
						mTargetExceedsWeaponLimits = true;
					if ( fPitchDiff < mWeaponLimits.mMinPitch || fPitchDiff > mWeaponLimits.mMaxPitch )
						mTargetExceedsWeaponLimits = true;
				}
				return true;
			}
		}
		_aimpos = mAimPosition = pRecord->GetLastSensedPosition();
		return false;
	}
	void AttackTarget::OnTarget()
	{
		if ( !mShootTheBastard )
			return;

		FINDSTATE( wsys, WeaponSystem, GetParent() );
		if ( wsys != NULL && wsys->CurrentWeaponIsAttackReady() )
		{
			WeaponPtr wpn = wsys->GetCurrentWeapon();
			if ( wpn && !GetClient()->CheckUserFlag( Client::FL_SHOOTINGDISABLED ) )
			{
				if ( wsys->ReadyToFire() )
				{
					wpn->PreShoot( Primary );
					wpn->Shoot();
				}
				else
				{
					wpn->StopShooting( Primary );
				}
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	WeaponSystem::WeaponRequest::WeaponRequest()
	{
		Reset();
	}

	void WeaponSystem::WeaponRequest::Reset()
	{
		mPriority = Priority::Zero;
		mOwner = 0;
		mWeaponId = 0;
	}

	WeaponSystem::WeaponSystem()
		: StateFirstAvailable( "WeaponSystem" )
		, mReactionTimeInMS( 0 )
		, mAimPersistance( 2000 )
		, mDesiredWeaponID( 0 )
		, mDefaultWeapon( 0 )
		, mOverrideWeapon( 0 )
		, mCurrentRequestOwner( 0 )
	{
		//AppendState(new AttackTargetOffhand);
		AppendState( new AttackTarget );
		//AppendState(new ChooseMountedWeapon);
		AppendState( new ReloadOther );
	}

	WeaponSystem::~WeaponSystem()
	{
	}

	void WeaponSystem::GrabAllWeapons()
	{
		gWeaponDatabase.CopyAllWeapons( GetClient(), mAllWeaponList );
	}

	void WeaponSystem::RefreshWeapon( int _weaponId )
	{
		const bool HasWeapon = mWeaponMask.CheckFlag( _weaponId );

		RemoveWeapon( _weaponId );

		// update our inventory
		for ( WeaponList::iterator it = mAllWeaponList.begin(); it != mAllWeaponList.end(); ++it )
		{
			if ( ( *it )->GetWeaponID() == _weaponId )
			{
				( *it ) = gWeaponDatabase.CopyWeapon( GetClient(), _weaponId );
			}
		}

		if ( HasWeapon )
			AddWeaponToInventory( _weaponId );
	}

	void WeaponSystem::RefreshAllWeapons()
	{
		const BitFlag128 HasWeapons = mWeaponMask;

		ClearWeapons();

		mAllWeaponList.clear();
		gWeaponDatabase.CopyAllWeapons( GetClient(), mAllWeaponList );

		for ( int i = 0; i < 128; ++i )
		{
			if ( HasWeapons.CheckFlag( i ) )
				AddWeaponToInventory( i );
		}
	}

	bool WeaponSystem::AddWeaponToInventory( int _weaponId )
	{
		WeaponList::iterator it = mAllWeaponList.begin();
		for ( ; it != mAllWeaponList.end(); ++it )
		{
			if ( ( *it )->GetWeaponID() == _weaponId )
			{
				AddWeapon( *it );
				return true;
			}
		}
		return false;
	}

	void WeaponSystem::AddWeapon( WeaponPtr _weapon )
	{
		if ( !HasWeapon( _weapon->GetWeaponID() ) )
		{
			mWeaponList.push_back( _weapon );
			mWeaponMask.SetFlag( _weapon->GetWeaponID(), true );
		}
	}

	void WeaponSystem::RemoveWeapon( int _weaponId )
	{
		// clear the current weapon if we're removing it.
		if ( mCurrentWeapon && mCurrentWeapon->GetWeaponID() == _weaponId )
			mCurrentWeapon.reset();

		WeaponList::iterator it = mWeaponList.begin();
		for ( ; it != mWeaponList.end(); )
		{
			if ( ( *it )->GetWeaponID() == _weaponId )
			{
				it = mWeaponList.erase( it );
				mWeaponMask.SetFlag( _weaponId, false );
			}
			else
				++it;
		}
	}

	void WeaponSystem::ClearWeapons()
	{
		// Clear all the weapons
		mCurrentWeapon.reset();
		mWeaponList.clear();
		mWeaponMask.ClearAll();
	}

	void WeaponSystem::ChargeWeapon( FireMode _mode )
	{
		if ( mCurrentWeapon )
			mCurrentWeapon->ChargeWeapon( _mode );
	}

	void WeaponSystem::ZoomWeapon( FireMode _mode )
	{
		if ( mCurrentWeapon )
			mCurrentWeapon->ZoomWeapon( _mode );
	}

	void WeaponSystem::FireWeapon( FireMode _mode )
	{
		if ( mCurrentWeapon )
			mCurrentWeapon->Shoot( _mode );
	}

	WeaponPtr WeaponSystem::GetWeapon( int _weaponId, bool _inventory ) const
	{
		WeaponPtr ret;

		WeaponList::const_iterator it, itEnd;
		if ( _inventory )
		{
			it = mWeaponList.begin();
			itEnd = mWeaponList.end();
		}
		else
		{
			it = mAllWeaponList.begin();
			itEnd = mAllWeaponList.end();
		}

		for ( ; it != itEnd; ++it )
		{
			if ( ( *it )->GetWeaponID() == _weaponId )
			{
				ret = ( *it );
				break;
			}
		}
		return ret;
	}

	WeaponPtr WeaponSystem::GetWeaponByIndex( int _index, bool _inventory )
	{
		WeaponPtr ret;

		WeaponList::const_iterator it, itEnd;
		if ( _inventory )
		{
			it = mWeaponList.begin();
			itEnd = mWeaponList.end();
		}
		else
		{
			it = mAllWeaponList.begin();
			itEnd = mAllWeaponList.end();
		}

		std::advance( it, _index );
		return *it;
	}

	bool WeaponSystem::HasWeapon( int _weaponId ) const
	{
		return GetWeapon( _weaponId ) != NULL;
	}

	bool WeaponSystem::HasAmmo( FireMode _mode ) const
	{
		return mCurrentWeapon && mCurrentWeapon->GetFireMode( _mode ).IsDefined() ?
			mCurrentWeapon->GetFireMode( _mode ).HasAmmo() : false;
	}

	bool WeaponSystem::HasAmmo( int _weaponid, FireMode _mode, int _amount ) const
	{
		WeaponList::const_iterator it = mWeaponList.begin(), itEnd = mWeaponList.end();
		for ( ; it != itEnd; ++it )
		{
			if ( ( *it )->GetWeaponID() == _weaponid )
			{
				return ( *it )->GetFireMode( _mode ).IsDefined() ? ( *it )->GetFireMode( _mode ).HasAmmo( _amount ) : false;
			}
		}
		return false;
	}

	bool WeaponSystem::CanShoot( const MemoryRecord &_record )
	{
		// Target filters use this, so if we're mounted only check current weapon(which should be the mounted weapon)
		if ( mCurrentWeapon && GetClient()->CheckUserFlag( Client::FL_USINGMOUNTEDWEAPON ) )
			return mCurrentWeapon->CanShoot( Primary, _record.mTargetInfo );

		WeaponList::const_iterator it = mWeaponList.begin(), itEnd = mWeaponList.end();
		for ( ; it != itEnd; ++it )
		{
			if ( ( *it )->CanShoot( Primary, _record.mTargetInfo ) )
				return true;
		}
		return false;
	}
	
	void WeaponSystem::_UpdateCurrentWeapon( FireMode _mode )
	{
		// Get the weapon from the game.
		int weaponId = 0;
		FireMode weaponMode = Primary;

		gEngineFuncs->GetMountedWeapon( GetClient()->GetGameEntity(), weaponId, weaponMode );
		if ( weaponId != 0 )
		{
			GetClient()->SetUserFlag( Client::FL_USINGMOUNTEDWEAPON, true );
			mCurrentWeapon = GetWeapon( weaponId, false );
			mDesiredWeaponID = weaponId;
			mCurrentRequestOwner = GetNameHash();
		}
		else
		{
			GetClient()->SetUserFlag( Client::FL_USINGMOUNTEDWEAPON, false );
			
			gEngineFuncs->GetEquippedWeapon( GetClient()->GetGameEntity(), weaponId, weaponMode );

			// If it has changed, set our new weapon.
			if ( !mCurrentWeapon || !mCurrentWeapon->IsWeapon( weaponId ) )
			{
				//bool bFound = false;
				WeaponList::const_iterator it = mWeaponList.begin(), itEnd = mWeaponList.end();
				for ( ; it != itEnd; ++it )
				{
					if ( ( *it )->IsWeapon( weaponId ) )
					{
						mCurrentWeapon = ( *it );
						mCurrentWeapon->Select();
						//bFound = true;

						EvWeaponChanged::Msg event;
						event.mData.mWeaponId = weaponId;
						GetClient()->SendEvent( event );
						break;
					}
				}
			}
		}

		if ( mCurrentWeapon )
		{
			mCurrentWeapon->Update( _mode );
			mCurrentWeapon->UpdateClipAmmo( _mode );
		}
	}

	void WeaponSystem::UpdateWeapons()
	{
		const int MAX_WEAPONS = 128;
		int currentWeapons[ MAX_WEAPONS ] = {};
		const int numCurrentWeapons = gEngineFuncs->GetCurrentWeapons( GetClient()->GetGameEntity(), currentWeapons, MAX_WEAPONS );

		// keep track of what we have so we can add/remove whatever we need to meet the current list
		for ( size_t i = 0; i < mWeaponList.size(); ++i )
		{
			const int weaponId = mWeaponList[ i ]->GetWeaponID();

			bool hasWeapon = false;
			for ( int w = 0; w < numCurrentWeapons; ++w )
			{
				if ( weaponId == currentWeapons[ w ] )
				{
					hasWeapon = true;
					currentWeapons[ w ] = -1;
				}
			}

			if ( !hasWeapon )
			{
				EvRemoveWeapon::Msg event;
				event.mData.mWeaponId = weaponId;
				System::mInstance->mGame->DispatchEvent( event, GetClient()->GetGameEntity() );
			}
		}

		// anything not handled yet we need to add
		for ( int w = 0; w < numCurrentWeapons; ++w )
		{
			if ( currentWeapons[ w ] != -1 )
			{
				EvAddWeapon::Msg event;
				event.mData.mWeaponId = currentWeapons[ w ];
				System::mInstance->mGame->DispatchEvent( event, GetClient()->GetGameEntity() );
			}
		}
	}

	void WeaponSystem::UpdateAllWeaponAmmo()
	{
		// Update the primary weapons.
		WeaponList::iterator it = mWeaponList.begin();
		WeaponList::iterator itEnd = mWeaponList.end();
		for ( ; it != itEnd; ++it )
		{
			( *it )->UpdateAmmo( Primary );
		}
	}

	bool WeaponSystem::ReadyToFire()
	{
		return gEngineFuncs->IsReadyToFire( GetClient()->GetGameEntity() );
	}

	bool WeaponSystem::IsReloading()
	{
		return gEngineFuncs->IsReloading( GetClient()->GetGameEntity() );
	}

	float WeaponSystem::GetMostDesiredAmmo( int &_weapon, int &_getammo )
	{
		float fMostDesirable = 0.0f;
		int iMostDesirableAmmo = 0;
		int iGetAmmoAmount = 1;

		// Get the most desirable ammo currently.
		WeaponList::iterator it = mWeaponList.begin();
		WeaponList::iterator itEnd = mWeaponList.end();
		for ( ; it != itEnd; ++it )
		{
			( *it )->UpdateAmmo( Primary );

			int iThisWeaponType = 0;
			int iThisGetAmmo = 1;
			float dDesirability = ( *it )->LowOnAmmoPriority( Primary, iThisWeaponType, iThisGetAmmo );
			if ( dDesirability > fMostDesirable )
			{
				fMostDesirable = dDesirability;
				iMostDesirableAmmo = iThisWeaponType;
				iGetAmmoAmount = iThisGetAmmo;
			}
		}

		// Store the best results.
		_weapon = iMostDesirableAmmo;
		_getammo = iGetAmmoAmount;
		return fMostDesirable;
	}

	void WeaponSystem::GetSpectateMessage( std::stringstream &_outstring )
	{
		if ( mCurrentWeapon )
			mCurrentWeapon->GetSpectateMessage( _outstring );

		std::string desired = gWeaponDatabase.GetWeaponName( mDesiredWeaponID );
		_outstring << " Desired: " << desired.c_str() << " ";
	}

	//////////////////////////////////////////////////////////////////////////

	void WeaponSystem::GetDebugString( std::stringstream &out )
	{
		out <<
			Utils::HashToString( mCurrentRequestOwner ) <<
			" : " <<
			gWeaponDatabase.GetWeaponName( mDesiredWeaponID );
	}

	float WeaponSystem::GetPriority()
	{
		return 1.f;
	}

	void WeaponSystem::Enter()
	{
		mDefaultWeapon = SelectBestWeapon();
		AddWeaponRequest( Priority::Idle, GetNameHash(), mDefaultWeapon );
	}

	void WeaponSystem::Exit()
	{
		ReleaseWeaponRequest( GetNameHash() );
	}

	void WeaponSystem::Initialize()
	{
		GrabAllWeapons();
	}

	State::StateStatus WeaponSystem::Update( float fDt )
	{
		rmt_ScopedCPUSample( WeaponSystemUpdate );
		{
			//////////////////////////////////////////////////////////////////////////
			mDefaultWeapon = SelectBestWeapon();
			UpdateWeaponRequest( GetNameHash(), mDefaultWeapon );
			//////////////////////////////////////////////////////////////////////////

			// Update the preferred weapon.
			const WeaponRequest &bestWpn = GetHighestWeaponRequest();
			mDesiredWeaponID = bestWpn.mWeaponId;
			mCurrentRequestOwner = bestWpn.mOwner;

			_UpdateCurrentWeapon( Primary );
		}
		return State_Busy;
	}

	bool WeaponSystem::AddWeaponRequest( Priority::ePriority _prio, uint32_t _owner, int _weaponId )
	{
		int iOpen = -1;
		for ( int32_t i = 0; i < MaxWeaponRequests; ++i )
		{
			if ( mWeaponRequests[ i ].mOwner == _owner )
			{
				iOpen = i;
				break;
			}
			if ( mWeaponRequests[ i ].mPriority == Priority::Zero )
			{
				if ( iOpen == -1 )
					iOpen = i;
			}
		}

		if ( iOpen != -1 )
		{
			mWeaponRequests[ iOpen ].mPriority = _prio;
			mWeaponRequests[ iOpen ].mOwner = _owner;
			mWeaponRequests[ iOpen ].mWeaponId = _weaponId;
			return true;
		}
		return false;
	}

	void WeaponSystem::ReleaseWeaponRequest( uint32_t _owner )
	{
		for ( int32_t i = 0; i < MaxWeaponRequests; ++i )
		{
			if ( mWeaponRequests[ i ].mOwner == _owner )
			{
				mWeaponRequests[ i ].Reset();
				break;
			}
		}
	}

	bool WeaponSystem::UpdateWeaponRequest( uint32_t _owner, int _weaponId )
	{
		for ( int32_t i = 0; i < MaxWeaponRequests; ++i )
		{
			if ( mWeaponRequests[ i ].mOwner == _owner )
			{
				mWeaponRequests[ i ].mWeaponId = _weaponId;
				return true;
			}
		}
		return false;
	}

	const WeaponSystem::WeaponRequest &WeaponSystem::GetHighestWeaponRequest()
	{
		int iBestWpn = 0;
		for ( int32_t i = 1; i < MaxWeaponRequests; ++i )
		{
			if ( mWeaponRequests[ i ].mPriority > mWeaponRequests[ iBestWpn ].mPriority )
			{
				iBestWpn = i;
			}
		}
		return mWeaponRequests[ iBestWpn ];
	}

	int WeaponSystem::GetWeaponNeedingReload()
	{
		// Look for other weapons that need reloading.
		WeaponList::const_iterator it = mWeaponList.begin(), itEnd = mWeaponList.end();
		for ( ; it != itEnd; ++it )
		{
			// Magik: Added enough ammo check
			FireMode m = ( *it )->CanReload();
			if ( m != InvalidFireMode )
				return ( *it )->GetWeaponID();
		}
		return 0;
	}

	int WeaponSystem::SelectBestWeapon( GameEntity _targetent /*= NULL*/ )
	{
		rmt_ScopedCPUSample( SelectBestWeapon );

		UpdateWeapons();
		UpdateAllWeaponAmmo();

		int iBestWeaponID = 0;

		// If an override isn't provided, use the current target.
		if ( !_targetent.IsValid() )
			_targetent = GetClient()->GetTargetingSystem()->GetCurrentTarget();

		// Evaluate weapons with target data.
		if ( _targetent.IsValid() )
		{
			const TargetInfo *pTargetInfo = GetClient()->GetSensoryMemory()->GetTargetInfo( _targetent );

			if ( !pTargetInfo )
				return iBestWeaponID;

			float fBestDesirability = 0.0;

			// Evaluate my primary weapons.
			WeaponList::const_iterator it = mWeaponList.begin(), itEnd = mWeaponList.end();
			for ( ; it != itEnd; ++it )
			{
				float fDesirability = ( *it )->CalculateDesirability( *pTargetInfo );
				if ( fDesirability > fBestDesirability )
				{
					fBestDesirability = fDesirability;
					iBestWeaponID = ( *it )->GetWeaponID();
				}
			}
		}
		else
		{
			// Are we idly holding a weapon?
			float fBestIdleDesirability = 0.0;

			// Does the current weapon need reloading?
			/*FireMode m = mCurrentWeapon ? mCurrentWeapon->CanReload() : InvalidFireMode;
			if(m != InvalidFireMode)
			{
			mCurrentWeapon->ReloadWeapon(m);
			}
			else*/
			{
				// Look for other weapons that need reloading.
				WeaponList::const_iterator it = mWeaponList.begin(), itEnd = mWeaponList.end();
				for ( ; it != itEnd; ++it )
				{
					// Get the best idle weapon.
					float fBestDesirability = ( *it )->CalculateDefaultDesirability();
					if ( fBestDesirability > fBestIdleDesirability )
					{
						fBestIdleDesirability = fBestDesirability;
						iBestWeaponID = ( *it )->GetWeaponID();
					}
				}
			}
		}
		return iBestWeaponID;
	}

	int WeaponSystem::SelectRandomWeapon()
	{
		int WeaponIds[ 64 ] = {};
		int NumWeaponIds = 0;

		WeaponList::const_iterator it = mWeaponList.begin(), itEnd = mWeaponList.end();
		for ( ; it != itEnd; ++it )
		{
			WeaponIds[ NumWeaponIds++ ] = ( *it )->GetWeaponID();
		}
		return NumWeaponIds ? ( WeaponIds[ rand() % NumWeaponIds ] ) : 0;
	}
};