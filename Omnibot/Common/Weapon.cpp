////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "Weapon.h"
#include "Client.h"
#include "ScriptManager.h"
#include "MemoryRecord.h"
#include "TargetInfo.h"
#include "BotTargetingSystem.h"

#include "gmbinder2/gmbinder2.h"
#include "gmbinder2/gmbinder2_class.h"
#include "gmCall.h"
#include "gmWeapon.h"

//////////////////////////////////////////////////////////////////////////

Weapon::WeaponFireMode::WeaponFireMode()
	: mWeaponType( None )
	, mWeaponFlags( 0 )
	, mShootButton( BOT_BUTTON_ATTACK1 )
	, mZoomButton( BOT_BUTTON_AIM )
	, mProjectileSpeed( 0.f )
	, mProjectileGravity( 0.f )
	, mMinChargeTime( 0 )
	, mMaxChargeTime( 0 )
	, mMinLeadError( 0.f )
	, mMaxLeadError( 0.f )
	, mDelayAfterFiring( 0 )
	, mFuseTime( 0.f )
	, mSplashRadius( 0.f )
	, mPitchOffset( 0.f )
	, mLowAmmoThreshold( 0 )
	, mLowAmmoPriority( 0.f )
	, mLowAmmoGetAmmoAmount( 1 )
	, mHeatController( 1.f, 0.4f, 0.4f )
	, mCurrentBurstWindow( 0 )
	, mAmmoCurrent( 0 )
	, mAmmoMax( 0 )
	, mClipCurrent( 0 )
	, mClipMax( 0 )
	, mAimOffset( Vector3f::ZERO )
	, mAimErrorMax( Vector2f::ZERO )
	, mAimErrorCurrent( Vector3f::ZERO )
	, mMinAimAdjustmentSecs( 0.0f )
	, mMaxAimAdjustmentSecs( 2.0f )
	, mNextAimAdjustmentTime( 0 )
	, mLastDesirability( -1.0f )
	, mDefaultDesirability( 0.0f )
	, mWeaponBias( 1.0f )
	, mChargeTime( 0 )
	, mDelayChooseTime( 0 )
	, mBurstTime( 0 )
	, mBurstRound( 0 )
{
	// Initialize Default Properties
	SetFlag( Waterproof, true );
	SetFlag( RequiresAmmo, true );

	for ( int i = 0; i < MaxDesirabilities; ++i )
	{
		mDesirabilities[ i ].mMinRange = 0.f;
		mDesirabilities[ i ].mMaxRange = 0.f;
		mDesirabilities[ i ].mDesirability = 0.f;
	}
	for ( int i = 0; i < MaxBurstWindows; ++i )
	{
		mBurstWindows[ i ].mBurstRounds = 0;
		mBurstWindows[ i ].mMinBurstDelay = 0.f;
		mBurstWindows[ i ].mMaxBurstDelay = 0.f;
		mBurstWindows[ i ].mMinRange = 0.f;
		mBurstWindows[ i ].mMaxRange = 0.f;
	}

	fl.Charging = false;
}

Weapon::WeaponFireMode::~WeaponFireMode()
{
	if ( mScriptObject )
	{
		gmBind2::Class<WeaponFireMode>::NullifyUserObject( mScriptObject );
		mScriptObject = 0; //NULL;
	}
}

Weapon::WeaponFireMode &Weapon::WeaponFireMode::operator=( const WeaponFireMode &_rh )
{
	mWeaponType = _rh.mWeaponType;
	mWeaponFlags = _rh.mWeaponFlags;
	mShootButton = _rh.mShootButton;
	mZoomButton = _rh.mZoomButton;
	mProjectileSpeed = _rh.mProjectileSpeed;
	mProjectileGravity = _rh.mProjectileGravity;
	mMinChargeTime = _rh.mMinChargeTime;
	mMaxChargeTime = _rh.mMaxChargeTime;
	mDelayAfterFiring = _rh.mDelayAfterFiring;
	mFuseTime = _rh.mFuseTime;
	mSplashRadius = _rh.mSplashRadius;

	mLowAmmoThreshold = _rh.mLowAmmoThreshold;
	mLowAmmoPriority = _rh.mLowAmmoPriority;
	mLowAmmoGetAmmoAmount = _rh.mLowAmmoGetAmmoAmount;

	for ( int i = 0; i < MaxBurstWindows; ++i )
		mBurstWindows[ i ] = _rh.mBurstWindows[ i ];

	/*for(int i = 0; i < NumStances; ++i)
	 mStancePreference[i] = _rh. mStancePreference[i];*/

	mPitchOffset = _rh.mPitchOffset;

	mMinLeadError = _rh.mMinLeadError;
	mMaxLeadError = _rh.mMaxLeadError;

	mHeatController = _rh.mHeatController;
	mTargetBias = _rh.mTargetBias;
	mTargetEntFlagIgnore = _rh.mTargetEntFlagIgnore;

	for ( int i = 0; i < MaxDesirabilities; ++i )
		mDesirabilities[ i ] = _rh.mDesirabilities[ i ];

	mAmmoCurrent = _rh.mAmmoCurrent;
	mAmmoMax = _rh.mAmmoMax;
	mClipCurrent = _rh.mClipCurrent;
	mClipMax = _rh.mClipMax;
	mAimOffset = _rh.mAimOffset;
	mAimErrorMax = _rh.mAimErrorMax;
	mAimErrorCurrent = _rh.mAimErrorCurrent;
	mMinAimAdjustmentSecs = _rh.mMinAimAdjustmentSecs;
	mMaxAimAdjustmentSecs = _rh.mMaxAimAdjustmentSecs;
	mNextAimAdjustmentTime = _rh.mNextAimAdjustmentTime;
	mLastDesirability = _rh.mLastDesirability;
	mDefaultDesirability = _rh.mDefaultDesirability;
	mWeaponBias = _rh.mWeaponBias;

	mscrCalcDefDesir = _rh.mscrCalcDefDesir;
	mscrCalcDesir = _rh.mscrCalcDesir;
	mscrCalcAimPoint = _rh.mscrCalcAimPoint;

	// duplicate the script table
	gmMachine *pM = ScriptManager::GetInstance()->GetMachine();
	gmBind2::Class<WeaponFireMode>::CloneTable( pM, _rh.GetScriptObject( pM ), GetScriptObject( pM ) );

	return *this;
}

bool Weapon::WeaponFireMode::HasAmmo( int _amount ) const
{
	if ( CheckFlag( RequiresAmmo ) )
	{
		if ( _amount > 0 )
			return ( mAmmoCurrent >= _amount ) || ( mClipCurrent >= _amount );
		return ( mAmmoCurrent > 0 ) || ( mClipCurrent > 0 );
	}
	return true;
}

bool Weapon::WeaponFireMode::NeedsAmmo() const
{
	return CheckFlag( RequiresAmmo ) ? ( mAmmoMax > 0 ) : false;
}

bool Weapon::WeaponFireMode::FullClip() const
{
	// Only if this is a clipped weapon
	return ( mClipMax > 0 ) ? ( mClipCurrent >= mClipMax ) : true;
}

bool Weapon::WeaponFireMode::EmptyClip() const
{
	return UsesClip() ? ( mClipCurrent == 0 ) : ( mAmmoCurrent == 0 );
}

bool Weapon::WeaponFireMode::UsesClip() const
{
	return CheckFlag( HasClip );
}

bool Weapon::WeaponFireMode::EnoughAmmoToReload() const
{
	return ( mAmmoCurrent >= mClipMax );//( mAmmoCurrent > 0);
}

float Weapon::WeaponFireMode::CalculateDefaultDesirability( Client *_bot )
{
	//////////////////////////////////////////////////////////////////////////
	// Script callback if necessary.
	if ( mscrCalcDefDesir )
	{
		gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
		gmCall call;
		if ( call.BeginFunction( pMachine, mscrCalcDefDesir, gmVariable( GetScriptObject( pMachine ) ) ) )
		{
			call.AddParamUser( GetScriptObject( pMachine ) );
			call.AddParamUser( _bot->GetScriptObject() );
			call.End();

			if ( !call.GetReturnedFloat( mDefaultDesirability ) )
			{
				Utils::OutputDebug( kError, "Invalid Return Value From CalculateDefaultDesirability" );
			}
		}
		else
		{
			Utils::OutputDebug( kError, "Invalid Weapon CalculateDefaultDesirability Callback!" );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	return  mDefaultDesirability *  mWeaponBias;
}

float Weapon::WeaponFireMode::CalculateDesirability( Client *_bot, const TargetInfo &_targetinfo )
{
	mLastDesirability = mDefaultDesirability;
	//////////////////////////////////////////////////////////////////////////
	// Script callback if necessary.
	if ( mscrCalcDesir )
	{
		gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();

		gmCall call;
		if ( call.BeginFunction( pMachine, mscrCalcDesir, gmVariable( GetScriptObject( pMachine ) ) ) )
		{
			//call.AddParamUser(GetScriptObject(pMachine));
			call.AddParamUser( _bot->GetScriptObject() );
			call.AddParamUser( _targetinfo.GetScriptObject( pMachine ) );
			call.End();

			if ( !call.GetReturnedFloat( mLastDesirability ) )
			{
				Utils::OutputDebug( kError, "Invalid Return Value From CalculateDesirability" );
			}
			mLastDesirability *= mWeaponBias;
			return  mLastDesirability;
		}
		else
		{
			Utils::OutputDebug( kError, "Invalid Weapon CalculateDesirability Callback!" );
		}
	}
	//////////////////////////////////////////////////////////////////////////

	float fTargetBias = GetTargetBias( _targetinfo.mEntInfo );
	if ( fTargetBias == 0.f )
	{
		mLastDesirability = 0.0;
		return  mLastDesirability;
	}

	for ( int i = 0; i < MaxDesirabilities; ++i )
	{
		if ( mDesirabilities[ i ].mMaxRange != 0.f )
		{
			if ( InRangeT( _targetinfo.mDistanceTo, mDesirabilities[ i ].mMinRange, mDesirabilities[ i ].mMaxRange ) )
			{
				if ( mDesirabilities[ i ].mDesirability >  mLastDesirability )
					mLastDesirability = mDesirabilities[ i ].mDesirability * fTargetBias;
			}
		}
	}
	mLastDesirability *= mWeaponBias;
	return  mLastDesirability;
}

void Weapon::WeaponFireMode::UpdateBurstWindow( const TargetInfo *_targetinfo )
{
	if ( _targetinfo )
	{
		for ( int i = 0; i < MaxBurstWindows; ++i )
		{
			if ( mBurstWindows[ i ].mBurstRounds > 0 )
			{
				if ( InRangeT( _targetinfo->mDistanceTo, mBurstWindows[ i ].mMinRange, mBurstWindows[ i ].mMaxRange ) )
				{
					mCurrentBurstWindow = i;
					break;
				}
			}
		}
	}
}

//float Weapon::WeaponFireMode::CalculateAmmoDesirability(int &_ammotype)
//{
//	// TODO? OR DELETE
//	return 0.0;
//}

bool Weapon::WeaponFireMode::SetDesirabilityWindow( float _minrange, float _maxrange, float _desir )
{
	for ( int i = 0; i < MaxDesirabilities; ++i )
	{
		if ( mDesirabilities[ i ].mMinRange == _minrange &&
			mDesirabilities[ i ].mMaxRange == _maxrange )
		{
			mDesirabilities[ i ].mMinRange = _minrange;
			mDesirabilities[ i ].mMaxRange = _maxrange;
			mDesirabilities[ i ].mDesirability = _desir;
			return true;
		}
	}

	for ( int i = 0; i < MaxDesirabilities; ++i )
	{
		if ( mDesirabilities[ i ].mMaxRange == 0.f ) // maxrange 0 determines un-used slots
		{
			mDesirabilities[ i ].mMinRange = _minrange;
			mDesirabilities[ i ].mMaxRange = _maxrange;
			mDesirabilities[ i ].mDesirability = _desir;
			return true;
		}
	}
	return false;
}

bool Weapon::WeaponFireMode::SetBurstWindow( float _minrange, float _maxrange, int _burst, float _mindelay, float _maxdelay )
{
	for ( int i = 0; i < MaxBurstWindows; ++i )
	{
		if ( mBurstWindows[ i ].mMinRange == _minrange &&
			mBurstWindows[ i ].mMaxRange == _maxrange )
		{
			mBurstWindows[ i ].mMinRange = _minrange;
			mBurstWindows[ i ].mMaxRange = _maxrange;
			mBurstWindows[ i ].mBurstRounds = _burst;
			mBurstWindows[ i ].mMinBurstDelay = _mindelay;
			mBurstWindows[ i ].mMaxBurstDelay = _maxdelay;
			return true;
		}
	}

	for ( int i = 0; i < MaxBurstWindows; ++i )
	{
		if ( mBurstWindows[ i ].mBurstRounds == 0 ) //  mBurstRounds 0 determines un-used slots
		{
			mBurstWindows[ i ].mMinRange = _minrange;
			mBurstWindows[ i ].mMaxRange = _maxrange;
			mBurstWindows[ i ].mBurstRounds = _burst;
			mBurstWindows[ i ].mMinBurstDelay = _mindelay;
			mBurstWindows[ i ].mMaxBurstDelay = _maxdelay;
			return true;
		}
	}
	return false;
}

Vector3f Weapon::WeaponFireMode::GetAimPoint( Client *_bot, const GameEntity &_target, const TargetInfo &_targetinfo )
{
	Vector3f vAimPoint;

	//////////////////////////////////////////////////////////////////////////
	// Script callback if necessary.
	if ( mscrCalcAimPoint )
	{
		gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();

		gmCall call;
		if ( call.BeginFunction( pMachine, mscrCalcAimPoint, gmVariable( GetScriptObject( pMachine ) ) ) )
		{
			call.AddParamUser( GetScriptObject( pMachine ) );
			call.AddParamUser( _bot->GetScriptObject() );
			call.AddParamUser( _targetinfo.GetScriptObject( pMachine ) );
			call.End();

			const gmVariable &ret = call.GetReturnedVariable();
			if ( ret.m_type == GM_VEC3 )
			{
				ret.GetVector( vAimPoint.X(), vAimPoint.Y(), vAimPoint.Z() );
				vAimPoint += mAimOffset;
				return vAimPoint;
			}
			else
			{
				Utils::OutputDebug( kError, "Invalid Return Value From CalculateAimPoint" );
			}
		}
		else
		{
			Utils::OutputDebug( kError, "Invalid Weapon CalculateAimPoint Callback!" );
		}
	}
	//////////////////////////////////////////////////////////////////////////

	switch ( mWeaponType )
	{
		case Melee:
			vAimPoint = _GetAimPoint_Melee( _bot, _target, _targetinfo );
			break;
		case InstantHit:
			vAimPoint = _GetAimPoint_InstantHit( _bot, _target, _targetinfo );
			break;
		case Projectile:
			vAimPoint = _GetAimPoint_Projectile( _bot, _target, _targetinfo );
			break;
		case Grenade:
			vAimPoint = _GetAimPoint_Grenade( _bot, _target, _targetinfo );
			break;
		default:
			Utils::OutputDebug( kError, "Invalid Weapon Type!" );
			vAimPoint = _GetAimPoint_InstantHit( _bot, _target, _targetinfo );
			break;
	}

	vAimPoint += mAimOffset;
	return vAimPoint;
}

Vector3f Weapon::WeaponFireMode::_GetAimPoint_Melee( Client *_bot, const GameEntity &_target, const TargetInfo &_targetinfo )
{
	return _targetinfo.mLastPosition + ( _targetinfo.mLastVelocity * IGame::GetDeltaTimeSecs() );
}

Vector3f Weapon::WeaponFireMode::_GetAimPoint_InstantHit( Client *_bot, const GameEntity &_target, const TargetInfo &_targetinfo )
{
	return _targetinfo.mLastPosition + ( _targetinfo.mLastVelocity * IGame::GetDeltaTimeSecs() );
}

Vector3f Weapon::WeaponFireMode::_GetAimPoint_Projectile( Client *_bot, const GameEntity &_target, const TargetInfo &_targetinfo )
{
	const Vector3f vEyePos = _bot->GetPosition();
	const Vector3f vMyVelocity = CheckFlag( InheritsVelocity ) ? _bot->GetVelocity() : Vector3f::ZERO;
	Vector3f vAimPt = Utils::PredictFuturePositionOfTarget(
		vEyePos,
		mProjectileSpeed,
		_targetinfo,
		vMyVelocity,
		mMinLeadError,
		mMaxLeadError );

	if ( mProjectileGravity != 0 )
	{
		///////////////////////////////////////////////////////////////
		//float fProjSpeedAngleMod = 0.f;
		//static Vector3f s_ProjSpeed(3000*1.1f, 3000*1.1f, 1500*1.1f);
		//Vector3f vProjVel = _bot->GetFacingVector();
		//vProjVel.X() *= s_ProjSpeed.X();
		//vProjVel.Y() *= s_ProjSpeed.Y();
		//vProjVel.Z() *= s_ProjSpeed.Z();
		////vProjVel.Length();
		//float fProjectileSpeed = vProjVel.Normalize();
		//Quaternionf q;
		//q.Align(_bot->GetFacingVector(), vProjVel);

		//Vector3f vAxis;
		//q.ToAxisAngle(vAxis, fProjSpeedAngleMod);

		//fProjSpeedAngleMod = Mathf::ATan(vProjVel.Z() - _bot->GetFacingVector().Z());

		//EngineFuncs::ConsoleMessage("Angle Diff: %f", Mathf::RadToDeg(fProjSpeedAngleMod));
		///////////////////////////////////////////////////////////////

		Trajectory::AimTrajectory traj[ 2 ];
		int t = Trajectory::Calculate(
			vEyePos,
			vAimPt,
			mProjectileSpeed,
			IGame::GetGravity() *  mProjectileGravity,
			traj );

		// TODO: test each trajectory
		//bool bTrajectoryGood = true;

		int iTrajectoryNum = CheckFlag( UseMortarTrajectory ) ? 1 : 0;

		Vector3f vAimDir = vAimPt - vEyePos;
		const float fAimLength2d = Vector2f( vAimDir.X(), vAimDir.Y() ).Length();
		if ( t > iTrajectoryNum )
		{
			vAimPt.Z() = vEyePos.Z() + Mathf::Tan( traj[ iTrajectoryNum ].mAngle - mPitchOffset ) * fAimLength2d;

			/*if(_bot->IsDebugEnabled(BOT_DEBUG_AIMPOINT))
			{
			}*/
		}
		else
			vAimPt.Z() = vEyePos.Z() + Mathf::Tan( Mathf::DegToRad( 45.0f ) ) * fAimLength2d;
	}
	return vAimPt;
}

Vector3f Weapon::WeaponFireMode::_GetAimPoint_Grenade( Client *_bot, const GameEntity &_target, const TargetInfo &_targetinfo )
{
	// TODO: account for fusetime
	return _GetAimPoint_Projectile( _bot, _target, _targetinfo );
}

void Weapon::WeaponFireMode::AddAimError( Client *_bot, Vector3f &_aimpoint, const TargetInfo &_info )
{
	// Check if we need to get a new aim error.
	if ( IGame::GetTime() > mNextAimAdjustmentTime )
	{
		mAimErrorCurrent.X() = ( Mathf::SymmetricRandom() *  mAimErrorMax.X() );
		mAimErrorCurrent.Y() = ( Mathf::SymmetricRandom() *  mAimErrorMax.X() );
		mAimErrorCurrent.Z() = ( Mathf::SymmetricRandom() *  mAimErrorMax.Y() );

		const float fDelaySecs = Mathf::IntervalRandom( mMinAimAdjustmentSecs, mMaxAimAdjustmentSecs );
		mNextAimAdjustmentTime = IGame::GetTime() + Utils::SecondsToMilliseconds( fDelaySecs );
	}

	// Add the aim offset.
	_aimpoint += mAimErrorCurrent;
}

void Weapon::WeaponFireMode::SetTargetBias( uint16_t classId, float bias )
{
	mTargetBias[ classId ] = bias;
}

float Weapon::WeaponFireMode::GetTargetBias( const EntityInfo& classInfo )
{
	if ( ( classInfo.mFlags &  mTargetEntFlagIgnore ).AnyFlagSet() )
		return 0.0f;

	TargetBiasMap::const_iterator it = mTargetBias.find( classInfo.mClassId );
	if ( it != mTargetBias.end() )
		return it->second;
	return 1.f;
}

static int GM_CDECL gmfSetIgnoreEntFlags( gmThread *a_thread )
{
	Weapon::WeaponFireMode *NativePtr = 0;
	if ( !gmBind2::Class<Weapon::WeaponFireMode>::FromThis( a_thread, NativePtr ) || !NativePtr )
	{
		GM_EXCEPTION_MSG( "Script Function on NULL MapGoal" );
		return GM_EXCEPTION;
	}

	NativePtr->GetIgnoreEntFlags().ClearAll();
	for ( int i = 0; i < a_thread->GetNumParams(); ++i )
	{
		GM_CHECK_INT_PARAM( flg, i );
		NativePtr->GetIgnoreEntFlags().SetFlag( flg, true );
	}
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////
// Utilities
bool Weapon::WeaponFireMode::IsDefined() const
{
	return  mWeaponType != 0;
}

bool Weapon::WeaponFireMode::CheckDefined() const
{
	return IsDefined();
}

gmGCRoot<gmUserObject> Weapon::WeaponFireMode::GetScriptObject( gmMachine *_machine ) const
{
	if ( !mScriptObject )
		mScriptObject = gmBind2::Class<WeaponFireMode>::WrapObject( _machine, const_cast<WeaponFireMode*>( this ), true );
	return  mScriptObject;
}

//////////////////////////////////////////////////////////////////////////
// Events

void Weapon::WeaponFireMode::OnPreFire( Weapon *_weapon, Client *_client, const TargetInfo *_target )
{
	UpdateBurstWindow( _target );
}

void Weapon::WeaponFireMode::OnChargeWeapon( Weapon *_weapon, Client *_client )
{
	if ( CheckFlag( FireOnRelease ) )
	{
		_client->PressButton( mShootButton );
		if ( !IsCharging() )
		{
			mChargeTime = IGame::GetTime() +
				Utils::SecondsToMilliseconds( Mathf::IntervalRandom( mMinChargeTime, mMaxChargeTime ) );
		}
		else
		{
			// keep the charge time from expiring while charging.
			if ( mChargeTime <= IGame::GetTime() )
				mChargeTime = IGame::GetTime() + 1;
		}
	}
}

void Weapon::WeaponFireMode::OnZoomWeapon( Weapon *_weapon, Client *_client )
{
	if ( CheckFlag( HasZoom ) )
	{
		_client->PressButton( mZoomButton );
	}
}

void Weapon::WeaponFireMode::OnStartShooting( Weapon *_weapon, Client *_client )
{
	CheckDefined();

	if ( CheckFlag( MustBeOnGround ) && !_client->HasEntityFlag( ENT_FLAG_ONGROUND ) )
		return;

	if ( IsBurstDelaying() )
		return;

	if ( CheckFlag( FireOnRelease ) )
	{
		if ( IsCharging() )
		{
			if ( CheckFlag( ChargeToIntercept ) )
			{
				const MemoryRecord *pRecord = _client->GetTargetingSystem()->GetCurrentTargetRecord();
				if ( pRecord )
				{
					int32_t primeTime = mChargeTime - Utils::SecondsToMilliseconds( mFuseTime );
					float fTimeToDet = mFuseTime - Utils::MillisecondsToSeconds( IGame::GetTime() - primeTime );

					bool bThrow = fTimeToDet < 1.0;
					Vector3f vFirePos = _client->GetEyePosition();
					Vector3f vFacing = _client->GetFacingVector();
					Vector3f v2dFacing = vFacing.Flatten();

					Vector3f vDetPos = _client->GetEyePosition() + ( v2dFacing* mProjectileSpeed ) * fTimeToDet;
					float fExplodeDistToTarget = ( vDetPos - vFirePos ).Length();

					float fGrenadeRange = fTimeToDet *  mProjectileSpeed;

					/*Utils::OutputDebug(kDebug,"fTimeToDet: %f, fExplodeDistToTarget: %f / %f",
					fTimeToDet, fExplodeDistToTarget, fGrenadeRange);*/

					if ( fExplodeDistToTarget >= fGrenadeRange )
						bThrow = true;

					if ( bThrow )
					{
						mChargeTime = 0;
						_client->ReleaseButton( mShootButton );
					}
				}
			}
			else
			{
				if ( IGame::GetTime() > mChargeTime )
				{
					mChargeTime = 0;
					_client->ReleaseButton( mShootButton );
					return;
				}
			}
		}
		else
		{
			// Not currently charging, begin charging.
			if ( CheckFlag( ChargeToIntercept ) )
			{
				mChargeTime = IGame::GetTime() + Utils::SecondsToMilliseconds( mFuseTime );
			}
			else
			{
				mChargeTime = IGame::GetTime() +
					Utils::SecondsToMilliseconds( Mathf::IntervalRandom( mMinChargeTime, mMaxChargeTime ) );
			}
		}
		_client->PressButton( mShootButton );
	}
	else
	{
		_client->PressButton( mShootButton );
	}
}

void Weapon::WeaponFireMode::OnStopShooting( Weapon *_weapon, Client *_client )
{
	_client->ReleaseButton( mShootButton );
}

void Weapon::WeaponFireMode::OnReload( Weapon *_weapon, Client *_client )
{
	_client->PressButton( BOT_BUTTON_RELOAD );
	_client->ReleaseButton( mShootButton );
}

bool Weapon::WeaponFireMode::OnNeedToReload( Weapon *_weapon, Client *_client )
{
	if ( CheckDefined() )
	{
		if ( !CheckFlag( IgnoreReload ) && NeedsAmmo() && !FullClip() && EnoughAmmoToReload() )
			return true;
	}
	return false;
}

void Weapon::WeaponFireMode::OnShotFired( Weapon *_weapon, Client *_client, GameEntity _projectile /*= NULL*/ )
{
	const BurstWindow &w = mBurstWindows[ mCurrentBurstWindow ];
	if ( w.mBurstRounds > 0 )
	{
		if ( ++mBurstRound >= w.mBurstRounds )
		{
			mBurstTime = IGame::GetTime() +
				Utils::SecondsToMilliseconds( Mathf::IntervalRandom( w.mMinBurstDelay, w.mMaxBurstDelay ) );
			mBurstRound = 0;
		}
	}
	mDelayChooseTime = IGame::GetTime() + Utils::SecondsToMilliseconds( mDelayAfterFiring );
}

bool Weapon::WeaponFireMode::IsCharging() const
{
	return  mChargeTime != 0;
}

bool Weapon::WeaponFireMode::HasChargeTimes() const
{
	return  mMinChargeTime &&  mMaxChargeTime;
}

bool Weapon::WeaponFireMode::IsBurstDelaying() const
{
	return  mBurstTime > IGame::GetTime();
}

//////////////////////////////////////////////////////////////////////////

Weapon::Weapon( Client *_client/* = 0*/ )
	: mClient( _client )
	, mWeaponID( 0 )
	, mWeaponAliasID( 0 )
	, mWeaponLockTime( 0 )
	, mWeaponNameHash( 0 )
	, mMinUseTime( 0 )
	, mLastAmmoUpdate( 0 )
	, mScriptObject( 0 )
{
	memset( &mWeaponLimits, 0, sizeof( mWeaponLimits ) );
}

Weapon::Weapon( Client *_client, const Weapon *_wpn )
	: mClient( _client )
	, mWeaponID( 0 )
	, mWeaponLockTime( 0 )
	, mWeaponNameHash( 0 )
	, mMinUseTime( 0 )
	, mLastAmmoUpdate( 0 )
	, mScriptObject( 0 )
{
	mWeaponID = _wpn->mWeaponID;
	mWeaponAliasID = _wpn->mWeaponAliasID;

	mWeaponNameHash = _wpn->mWeaponNameHash;
	mWeaponLockTime = _wpn->mWeaponLockTime;
	mMinUseTime = _wpn->mMinUseTime;

	ScriptResource::operator=( *_wpn );

	memset( &mWeaponLimits, 0, sizeof( mWeaponLimits ) );

	// duplicate the script table
	gmMachine *pM = ScriptManager::GetInstance()->GetMachine();
	gmBind2::Class<Weapon>::CloneTable( pM, _wpn->GetScriptObject( pM ), GetScriptObject( pM ) );

	for ( int i = 0; i < NumFireModes; ++i )
	{
		mFireModes[ i ] = _wpn->mFireModes[ i ];
	}
}

Weapon::~Weapon()
{
	if ( mScriptObject )
	{
		gmBind2::Class<Weapon>::NullifyUserObject( mScriptObject );
		mScriptObject = 0; //NULL;
	}
}

std::string Weapon::GetWeaponName() const
{
	return Utils::HashToString( mWeaponNameHash );
}

void Weapon::SetWeaponName( const char *_name )
{
	mWeaponNameHash = _name ? Utils::MakeHash32( _name ) : 0;
}

gmGCRoot<gmUserObject> Weapon::GetScriptObject( gmMachine *_machine ) const
{
	if ( !mScriptObject )
		mScriptObject = gmBind2::Class<Weapon>::WrapObject( _machine, const_cast<Weapon*>( this ), true );
	return  mScriptObject;
}

void Weapon::GetSpectateMessage( std::stringstream &_outstring )
{
	_outstring << "Weapon: " << GetWeaponName() << " ";
	for ( int i = 0; i < NumFireModes; ++i )
	{
		FireMode m = GetFireMode( i );
		if ( GetFireMode( m ).IsDefined() )
		{
			if ( m == Primary )
				_outstring << "P(";
			else
				_outstring << "S(";

			if ( GetFireMode( m ).IsCharging() && GetFireMode( m ).HasChargeTimes() )
				_outstring << "Charging, ";
			if ( GetFireMode( m ).IsBurstDelaying() )
				_outstring << "BurstDelay, ";
			_outstring << ")";
		}
	}
}

float Weapon::LowOnAmmoPriority( FireMode _mode, int &_ammotype, int &_getammo )
{
	float fLowAmmoPriority = 0.f;
	if ( GetFireMode( _mode ).IsDefined() )
	{
		_ammotype = GetWeaponID();//GetFireMode(_mode). mAmmoType;

		// _getammo must be greater than threshold, otherwise UseCabinet goal would be
		// immediately aborted after path to cabinet is found
		int threshold = GetFireMode( _mode ).GetLowAmmoThreshold();
		_getammo = MaxT( threshold + 1, GetFireMode( _mode ).GetLowAmmoGetAmmoAmount() );
		if ( GetFireMode( _mode ).mAmmoCurrent <= threshold )
		{
			fLowAmmoPriority = GetFireMode( _mode ).GetLowAmmoPriority();
		}
	}
	return fLowAmmoPriority;
}

void Weapon::ReloadWeapon( FireMode _mode )
{
	if ( GetFireMode( _mode ).CheckDefined() )
	{
		GetFireMode( _mode ).OnReload( this, mClient );
	}
}

void Weapon::Select()
{
	if ( mMinUseTime != 0.f )
		mWeaponLockTime = IGame::GetTime() + Utils::SecondsToMilliseconds( mMinUseTime );
}

bool Weapon::SelectionLocked()
{
	return  mWeaponLockTime > IGame::GetTime();
}

bool Weapon::CanShoot( FireMode _mode, const TargetInfo &_targetinfo )
{
	if ( !_MeetsRequirements( _mode, _targetinfo ) )
		return false;

	WeaponFireMode &fireMode = GetFireMode( _mode );
	float d = fireMode.CalculateDesirability( mClient, _targetinfo );
	if ( d == 0.f ) return false;
	if ( fireMode.mWeaponType == Melee && d == fireMode.mDefaultDesirability * fireMode.mWeaponBias )
		return false;

	return true;
}

void Weapon::PreShoot( FireMode _mode, bool _facingTarget, const TargetInfo *_target )
{
	GetFireMode( _mode ).OnPreFire( this, mClient, _target );
}

void Weapon::ChargeWeapon( FireMode _mode )
{
	GetFireMode( _mode ).OnChargeWeapon( this, mClient );
}

void Weapon::ZoomWeapon( FireMode _mode )
{
	GetFireMode( _mode ).OnZoomWeapon( this, mClient );
}

void Weapon::ShotFired( FireMode _mode, GameEntity _projectile )
{
	GetFireMode( _mode ).OnShotFired( this, mClient, _projectile );
}

void Weapon::Shoot( FireMode _mode, const TargetInfo *_target )
{
	if ( GetFireMode( _mode ).EmptyClip() &&
		GetFireMode( _mode ).NeedsAmmo() )
	{
		if ( GetFireMode( _mode ).EnoughAmmoToReload() )
		{
			ReloadWeapon( _mode );
		}
	}
	else
	{
		bool bFire = true;
		if ( GetFireMode( _mode ).CheckFlag( ManageHeat ) )
		{
			float heatCur = 0.f, heatMax = 0.f;
			gEngineFuncs->GetWeaponHeat( mClient->GetGameEntity(), GetWeaponID(), _mode, heatCur, heatMax );
			const float heatRatio = heatCur / ( heatMax > 0.0f ? heatMax : 1.0f );
			GetFireMode( _mode ).mHeatController.Update( 0.7f, heatRatio, IGame::GetDeltaTimeSecs() );
			bFire = Mathf::Sign( GetFireMode( _mode ).mHeatController.GetControlValue() ) < 0.f ? false : true;
		}

		if ( bFire )
		{
			GetFireMode( _mode ).OnStartShooting( this, mClient );
		}
	}
}

void Weapon::StopShooting( FireMode _mode )
{
	GetFireMode( _mode ).OnStopShooting( this, mClient );
	if ( GetFireMode( _mode ).EmptyClip() &&
		GetFireMode( _mode ).NeedsAmmo() &&
		GetFireMode( _mode ).EnoughAmmoToReload() )
	{
		ReloadWeapon( _mode );
	}
}

void Weapon::UpdateClipAmmo( FireMode _mode )
{
	// This function is meant to be called when the weapon is currently equipped.
	if ( GetFireMode( _mode ).CheckFlag( RequiresAmmo ) )
	{
		gEngineFuncs->GetCurrentWeaponClip(
			mClient->GetGameEntity(),
			_mode,
			GetFireMode( _mode ).mClipCurrent,
			GetFireMode( _mode ).mClipMax );
	}
}

void Weapon::UpdateAmmo( FireMode _mode )
{
	if ( mLastAmmoUpdate == IGame::GetTime() )
		return;

	mLastAmmoUpdate = IGame::GetTime();
	if ( GetFireMode( _mode ).CheckFlag( RequiresAmmo ) )
	{
		gEngineFuncs->GetCurrentAmmo(
			mClient->GetGameEntity(),
			GetWeaponID(),
			_mode,
			GetFireMode( _mode ).mAmmoCurrent,
			GetFireMode( _mode ).mAmmoMax );
	}
}

void Weapon::Update( FireMode _mode )
{
	if ( GetFireMode( _mode ).IsBurstDelaying() )
	{
		if ( GetFireMode( _mode ).mBurstTime > IGame::GetTime() )
			return;
		GetFireMode( _mode ).mBurstTime = 0;
	}

	/*if(GetFireMode(_mode).IsCharging())
	{
	GetFireMode(_mode).OnStartShooting(this, mClient);
	}*/
}

Weapon::MoveOptions Weapon::UpdateMoveMode()
{
	MoveOptions _options;
	if ( GetFireMode( Primary ).IsDefined() )
	{
		if ( mClient->GetEntityFlags().CheckFlag( ENT_FLAG_AIMING ) )
		{
			if ( GetFireMode( Primary ).CheckFlag( StopWhileZoomed ) )
			{
				_options.MustStop = true;
			}
			else if ( GetFireMode( Primary ).CheckFlag( WalkWhileZoomed ) )
			{
				_options.MustWalk = true;
			}
			else if ( GetFireMode( Primary ).CheckFlag( CrouchToMoveWhenZoomed ) )
			{
				_options.CrouchToMove = true;
			}
		}
	}
	return _options;
}

Vector3f Weapon::GetAimPoint( FireMode _mode, const GameEntity &_target, const TargetInfo &_targetinfo )
{
	return GetFireMode( _mode ).GetAimPoint( mClient, _target, _targetinfo );
}

void Weapon::AddAimError( FireMode _mode, Vector3f &_aimpoint, const TargetInfo &_info )
{
	GetFireMode( _mode ).AddAimError( mClient, _aimpoint, _info );
}

FireMode Weapon::CanReload()
{
	FireMode ret = InvalidFireMode;
	for ( int mode = Primary; mode < NumFireModes; ++mode )
	{
		FireMode m = Weapon::GetFireMode( mode );
		if ( GetFireMode( m ).IsDefined() && GetFireMode( m ).OnNeedToReload( this, mClient ) )
		{
			ret = m;
			break;
		}
	}
	return ret;
}

FireMode Weapon::IsClipEmpty()
{
	FireMode ret = InvalidFireMode;
	for ( int mode = Primary; mode < NumFireModes; ++mode )
	{
		FireMode m = Weapon::GetFireMode( mode );
		if ( GetFireMode( m ).IsDefined() && GetFireMode( m ).EmptyClip() )
		{
			ret = m;
			break;
		}
	}
	return ret;
}

FireMode Weapon::OutOfAmmo()
{
	FireMode ret = InvalidFireMode;
	for ( int mode = Primary; mode < NumFireModes; ++mode )
	{
		FireMode m = Weapon::GetFireMode( mode );
		if ( GetFireMode( m ).IsDefined() && !GetFireMode( m ).HasAmmo() )
		{
			ret = m;
			break;
		}
	}
	return ret;
}

FireMode Weapon::GetFireMode( int _mode )
{
	switch ( _mode )
	{
		case Primary:
			return Primary;
		case Secondary:
			return Secondary;
		default:
			return InvalidFireMode;
	}
}

//////////////////////////////////////////////////////////////////////////

float Weapon::CalculateDefaultDesirability()
{
	float fDesir = 0.f, fBestDesir = 0.f;
	for ( int i = Primary; i < NumFireModes; ++i )
	{
		FireMode m = GetFireMode( i );

		if ( !_MeetsRequirements( m ) )
			continue;

		if ( GetFireMode( m ).IsDefined() ) // hmmm?
		{
			fDesir = GetFireMode( m ).CalculateDefaultDesirability( mClient );
			if ( fDesir > fBestDesir )
			{
				fBestDesir = fDesir;
			}
		}
	}
	return fBestDesir;
}

float Weapon::CalculateDesirability( const TargetInfo &_targetinfo )
{
	float fDesir = 0.f, fBestDesir = 0.f;
	for ( int i = Primary; i < NumFireModes; ++i )
	{
		FireMode m = GetFireMode( i );
		if ( GetFireMode( m ).IsDefined() )
		{
			if ( !_MeetsRequirements( m, _targetinfo ) )
				continue;

			fDesir = GetFireMode( m ).CalculateDesirability( mClient, _targetinfo );
			if ( fDesir > fBestDesir )
			{
				fBestDesir = fDesir;
			}
		}
	}
	return fBestDesir;
}

//////////////////////////////////////////////////////////////////////////

bool Weapon::_MeetsRequirements( FireMode mode )
{
	if ( !GetFireMode( mode ).CheckFlag( Waterproof ) && mClient->HasEntityFlag( ENT_FLAG_UNDERWATER ) )
		return false;

	UpdateAmmo( mode );
	if ( GetFireMode( mode ).CheckFlag( RequiresAmmo ) && !GetFireMode( mode ).HasAmmo() )
		return false;

	if ( GetFireMode( mode ).mDelayChooseTime > IGame::GetTime() )
		return false;

	if ( !gEngineFuncs->IsWeaponCharged( mClient->GetGameEntity(), GetWeaponID(), mode ) )
		return false;

	return true;
}

bool Weapon::_MeetsRequirements( FireMode mode, const TargetInfo &_targetinfo )
{
	if ( !_MeetsRequirements( mode ) )
		return false;
	if ( GetFireMode( mode ).CheckFlag( RequireShooterOutside ) && !gEngineFuncs->IsOutSide( mClient->GetPosition() ) )
		return false;
	if ( GetFireMode( mode ).CheckFlag( RequireTargetOutside ) && !gEngineFuncs->IsOutSide( _targetinfo.mLastPosition ) )
		return false;
	return true;
}

//////////////////////////////////////////////////////////////////////////

bool Weapon::WeaponFireMode::getType( Weapon::WeaponFireMode *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	switch ( a_native->mWeaponType )
	{
		case Weapon::Melee:
			a_operands[ 0 ].SetString( a_thread->GetMachine()->AllocStringObject( "melee" ) );
			break;
		case Weapon::InstantHit:
			a_operands[ 0 ].SetString( a_thread->GetMachine()->AllocStringObject( "instant" ) );
			break;
		case Weapon::Projectile:
			a_operands[ 0 ].SetString( a_thread->GetMachine()->AllocStringObject( "projectile" ) );
			break;
		case Weapon::Grenade:
			a_operands[ 0 ].SetString( a_thread->GetMachine()->AllocStringObject( "grenade" ) );
			break;
		default:
			a_operands[ 0 ].Nullify();
	}
	return true;
}

bool Weapon::WeaponFireMode::setType( Weapon::WeaponFireMode *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	gmStringObject *pObj = a_operands[ 1 ].GetStringObjectSafe();
	if ( pObj != NULL && pObj->GetString() )
	{
		if ( !_gmstricmp( pObj->GetString(), "melee" ) )
			a_native->mWeaponType = Weapon::Melee;
		else if ( !_gmstricmp( pObj->GetString(), "instant" ) )
			a_native->mWeaponType = Weapon::InstantHit;
		else if ( !_gmstricmp( pObj->GetString(), "projectile" ) )
			a_native->mWeaponType = Weapon::Projectile;
		else if ( !_gmstricmp( pObj->GetString(), "grenade" ) )
			a_native->mWeaponType = Weapon::Grenade;
		else
			Utils::OutputDebug( kError, "Invalid Weapon Type specified: %s", pObj->GetString() );
	}
	return true;
}

bool Weapon::WeaponFireMode::getMaxAimError( Weapon::WeaponFireMode *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	a_operands[ 0 ].SetVector( a_native->mAimErrorMax.X(), a_native->mAimErrorMax.Y(), 0 );
	return true;
}

bool Weapon::WeaponFireMode::setMaxAimError( Weapon::WeaponFireMode *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	if ( a_operands[ 1 ].IsVector() )
	{
		a_native->mAimErrorMax = Vector2f(
			a_operands[ 1 ].m_value.m_vec3.x,
			a_operands[ 1 ].m_value.m_vec3.y );
	}
	return true;
}

bool Weapon::WeaponFireMode::getAimOffset( Weapon::WeaponFireMode *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	a_operands[ 0 ].SetVector( a_native->mAimOffset.X(), a_native->mAimOffset.Y(), a_native->mAimOffset.Z() );
	return true;
}

bool Weapon::WeaponFireMode::setAimOffset( Weapon::WeaponFireMode *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	if ( a_operands[ 1 ].IsVector() )
	{
		a_native->mAimOffset = Vector3f(
			a_operands[ 1 ].m_value.m_vec3.x,
			a_operands[ 1 ].m_value.m_vec3.y,
			a_operands[ 1 ].m_value.m_vec3.z );
	}
	return true;
}

void Weapon::WeaponFireMode::Bind( gmMachine *_m )
{
	gmBind2::Class<WeaponFireMode>( "FireMode", _m )
		.func( &WeaponFireMode::SetDesirabilityWindow, "SetDesirabilityRange", "Set the desirability for a target within a certain min/max range." )
		.func( &WeaponFireMode::SetBurstWindow, "SetBurstRange", "Set a burst shot behavior for a target within a certain min/max range." )
		.func( &WeaponFireMode::SetTargetBias, "SetTargetBias", "Set a desirability multiplier versus a target class." )
		.func( gmfSetIgnoreEntFlags, "SetIgnoreEntFlags", "Sets one or more entity flags that should be ignored for this weapon." )

		.var( getType, setType, "WeaponType", "std::string", "melee, instant, projectile, or grenade" )

		.var( &WeaponFireMode::mShootButton, "ShootButton", 0, "The button to press to fire the weapon. Default ATTACK1." )
		.var( &WeaponFireMode::mZoomButton, "ZoomButton", 0, "The button to press to zoom the weapon. Default AIM." )

		// resupply
		.var( &WeaponFireMode::mLowAmmoThreshold, "LowAmmoThreshold", 0, "Bot will desire ammo if <= to this value." )
		.var( &WeaponFireMode::mLowAmmoPriority, "LowAmmoPriority", 0, "The priority to get ammo when the threshold is met." )
		.var( &WeaponFireMode::mLowAmmoGetAmmoAmount, "LowAmmoGetAmmoAmount", 0, "How much ammo to get to satisfy." )

		// projectile data
		.var( &WeaponFireMode::mFuseTime, "FuseTime", 0, "Fuse time to predict when ChargeToIntercept is set." )
		.var( &WeaponFireMode::mProjectileSpeed, "ProjectileSpeed", 0, "How fast the projectile moves. Used for prediction." )
		.var( &WeaponFireMode::mProjectileGravity, "ProjectileGravity", 0, "Gravity multiplier for how projectile is effected by gravity." )
		.var( &WeaponFireMode::mSplashRadius, "SplashRadius", 0, "The radius of the hits splash damage." )

		.var( &WeaponFireMode::mMinChargeTime, "MinChargeTime", 0, "Minimum time to charge FireOnRelease Shots" )
		.var( &WeaponFireMode::mMaxChargeTime, "MaxChargeTime", 0, "Maximum time to charge FireOnRelease Shots" )

		// misc
		.var( &WeaponFireMode::mDelayAfterFiring, "DelayAfterFiring", 0, "Time after shooting to delay choosing this weapon again." )

		.var( &WeaponFireMode::mDefaultDesirability, "DefaultDesirability", 0, "Desirability vs no target." )

		.var( &WeaponFireMode::mWeaponBias, "Bias", 0, "Multiplier to final desirability." )

		.var( &WeaponFireMode::mMinAimAdjustmentSecs, "MinAimAdjustmentTime", 0, "Minimum time between aim adjustments." )
		.var( &WeaponFireMode::mMaxAimAdjustmentSecs, "MaxAimAdjustmentTime", 0, "Maximum time between aim adjustments." )

		// aiming
		.var( getMaxAimError, setMaxAimError, "MaxAimError", "vec3", "Horizontal and vertical aim error." )
		.var( getAimOffset, setAimOffset, "AimOffset", "vec3", "Offset added to targeting aim point." )

		.var( &WeaponFireMode::mPitchOffset, "PitchOffset", 0, "Pitch offset to projectile spawn point." )

		.var( &WeaponFireMode::mMinLeadError, "MinLeadError", 0, "Minimum lead time error when firing weapon." )
		.var( &WeaponFireMode::mMaxLeadError, "MaxLeadError", 0, "Maximum lead time error when firing weapon." )

		// functions
		.var( &WeaponFireMode::mscrCalcDefDesir, "CalculateDefaultDesirability", "Callback", "Allows weapon to calculate default desirability." )
		.var( &WeaponFireMode::mscrCalcDesir, "CalculateDesirability", "Callback", "Allows weapon to calculate desirability." )
		.var( &WeaponFireMode::mscrCalcAimPoint, "CalculateAimPoint", "Callback", "Allows weapon to calculate aim point." )

		.var_bitfield( &WeaponFireMode::mWeaponFlags, RequiresAmmo, "RequiresAmmo", 0, "Weapon requires ammo to use. False means ammo is always assumed." )
		.var_bitfield( &WeaponFireMode::mWeaponFlags, Waterproof, "WaterProof", 0, "Weapon may be used the user is underwater." )
		.var_bitfield( &WeaponFireMode::mWeaponFlags, HasClip, "HasClip", 0, "Weapon has a clip. False means it simply has an ammo repository and doesn't need to reload." )
		/*replace with zoomfov?*/
		.var_bitfield( &WeaponFireMode::mWeaponFlags, HasZoom, "HasZoom", 0, "Weapon has zoom functionality." )
		.var_bitfield( &WeaponFireMode::mWeaponFlags, InheritsVelocity, "InheritsVelocity", 0, "Weapon projectile inherits user velocity." )
		.var_bitfield( &WeaponFireMode::mWeaponFlags, ManualDetonation, "ManualDetonation", 0, "Weapon projectiles must be manually detonated." )
		.var_bitfield( &WeaponFireMode::mWeaponFlags, MustBeOnGround, "MustBeOnGround", 0, "Weapon can only fire if user is on ground." )
		.var_bitfield( &WeaponFireMode::mWeaponFlags, FireOnRelease, "FireOnRelease", 0, "Weapon fires when the ShootButton is released, as opposed to when pressed." )
		.var_bitfield( &WeaponFireMode::mWeaponFlags, ManageHeat, "ManageHeat", 0, "Weapon may overheat, so user should fan the fire button to prevent." )
		.var_bitfield( &WeaponFireMode::mWeaponFlags, IgnoreReload, "IgnoreReload", 0, "Weapon should not be checked for reload." )
		.var_bitfield( &WeaponFireMode::mWeaponFlags, UseMortarTrajectory, "UseMortarTrajectory", 0, "Weapon should use mortar trajectory in prediction checks." )
		.var_bitfield( &WeaponFireMode::mWeaponFlags, RequireTargetOutside, "RequiresTargetOutside", 0, "Weapon cannot fire on target unless it is outside." )
		.var_bitfield( &WeaponFireMode::mWeaponFlags, RequireShooterOutside, "RequiresShooterOutside", 0, "Weapon cannot fire unless user is outside." )
		.var_bitfield( &WeaponFireMode::mWeaponFlags, ChargeToIntercept, "ChargeToIntercept", 0, "Weapon should be 'primed' with MinChargeTime/MaxChargeTime before firing." )
		.var_bitfield( &WeaponFireMode::mWeaponFlags, MeleeWeapon, "MeleeWeapon", 0, "Weapon is a melee weapon, user should use melee attack behavior." )
		.var_bitfield( &WeaponFireMode::mWeaponFlags, WalkWhileZoomed, "WalkWhileZoomed", 0, "Weapon requires user to hold walk key when zoomed." )
		.var_bitfield( &WeaponFireMode::mWeaponFlags, StopWhileZoomed, "StopWhileZoomed", 0, "Weapon requires user to stop moving when zoomed." )
		.var_bitfield( &WeaponFireMode::mWeaponFlags, CrouchToMoveWhenZoomed, "CrouchToMoveWhenZoomed", 0, "Weapon requires user to crouch to move when zoomed." )
		;
}

bool Weapon::getName( Weapon *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	a_operands[ 0 ].SetString( a_thread->GetMachine(), a_native->GetWeaponName().c_str() );
	return true;
}

bool Weapon::setName( Weapon *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	gmStringObject *pStr = a_operands[ 1 ].GetStringObjectSafe();
	if ( pStr != NULL && pStr->GetString() )
		a_native->SetWeaponName( pStr->GetString() );
	return true;
}

bool Weapon::getPrimaryFire( Weapon *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	gmUserObject *pObj = a_native->mFireModes[ Primary ].GetScriptObject( a_thread->GetMachine() );
	a_operands[ 0 ].SetUser( pObj );
	return true;
}

bool Weapon::getSecondaryFire( Weapon *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	gmUserObject *pObj = a_native->mFireModes[ Secondary ].GetScriptObject( a_thread->GetMachine() );
	a_operands[ 0 ].SetUser( pObj );
	return true;
}

void Weapon::Bind( gmMachine *_m )
{
	WeaponFireMode::Bind( _m );

	gmBind2::Class<Weapon>( "Weapon", _m )
		//.constructor()
		.var( getName, setName, "Name", "std::string", "Name of the weapon." )
		.var( &Weapon::mWeaponID, "WeaponId", "int", "Numeric Id of the weapon." )
		.var( &Weapon::mWeaponAliasID, "WeaponAliasId", "Numeric Id that will also match to this weapon." )

		.var( &Weapon::mMinUseTime, "MinUseTime", "float", "Weapon must be used for a minimum amount of time when chosen." )
		.var( getPrimaryFire, NULL, "PrimaryFire", "firemode", "Access to primary fire mode." )
		.var( getSecondaryFire, NULL, "SecondaryFire", "firemode", "Access to secondary fire mode." )
		;
}