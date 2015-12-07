////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "JA_BaseStates.h"
#include "JA_Client.h"

#include "RenderBuffer.h"

#include "BotPathing.h"
#include "BotSteeringSystem.h"

namespace AiState
{
	//////////////////////////////////////////////////////////////////////////
	ForceJump::ForceJump()
		: StateChild( "ForceJump" )
		, mIsJumping( false )
	{
	}

	void ForceJump::Enter()
	{
		mIsJumping = false;
		mIsOnGround = true;
		mDistanceLeft = mDistanceTotal = 0.f;
	}

	void ForceJump::RenderDebug()
	{
		if ( IsActive() )
		{
			RenderBuffer::AddLine( GetClient()->GetEyePosition(), mNextPt.mPt, COLOR::GREEN );
		}
	}

	float ForceJump::GetPriority()
	{
		if ( IsActive() )
			return 1.f;

		FINDSTATE( nav, Navigator, GetParent() );
		if ( nav )
		{
			/*if(fp->IsMoving() && fp->IsOnCustomLink(F_JA_NAV_FORCEJUMP))
			{
			return 1.f;
			}*/
		}
		return 0.f;
	}

	State::StateStatus ForceJump::Update( float fDt )
	{
		Vector3f vMyPos = GetClient()->GetPosition();

		const float fDistanceToPt = SquaredLength2d( vMyPos, mNextPt.mPt );
		bool bInRadius = fDistanceToPt <= mNextPt.mRadius;

		FINDSTATEIF( SteeringSystem, GetRootState(), SetTarget( mNextPt.mPt ) );

		/*Trajectory::AimTrajectory traj[2];
		int t = Trajectory::Calculate(
		vMyPos,
		mNextPt.mPt,
		GetClient()->GetMaxSpeed(),
		IGame::GetGravity(),
		traj);*/

		if ( !mIsJumping )
		{
			// Wait till we get in radius to jump
			if ( !bInRadius )
				return State_Busy;

			if ( GetClient()->HasEntityFlag( ENT_FLAG_ONGROUND ) )
			{
				mIsJumping = true;
				mIsOnGround = true;
			}
		}

		GetClient()->PressButton( BOT_BUTTON_JUMP );

		// Detect when we leave the ground
		if ( mIsOnGround && !GetClient()->HasEntityFlag( ENT_FLAG_ONGROUND ) )
		{
			mDistanceTotal = fDistanceToPt;
			mIsOnGround = false;
		}

		mDistanceLeft = fDistanceToPt;

		// When we touch the ground again after leaving it, we consider it done.
		if ( !mIsOnGround && GetClient()->HasEntityFlag( ENT_FLAG_ONGROUND ) )
			return State_Finished;

		// Finish when we exceed the targets height, because of this, it isn't usable for
		// long distance force jumps, only height based force jumps.
		if ( mIsJumping && vMyPos.Z() >= mNextPt.mPt.Z() && ( mDistanceLeft < mDistanceTotal*0.35f ) )
			return State_Finished;

		return State_Busy;
	}
	//////////////////////////////////////////////////////////////////////////
};