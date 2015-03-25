////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "BotSteeringSystem.h"
#include "Client.h"
#include "IGame.h"
#include "IGameManager.h"
#include "BotPathing.h"
#include "RenderBuffer.h"

namespace AiState
{
	SteeringSystem::SteeringSystem()
		: StateChild( "SteeringSystem" )
		, mTargetVector( Vector3f::ZERO )
		, mTargetRadius( 32.f )
		, mMoveMode( Run )
		, mDistanceToTarget( 0.f )
		, mMoveVec( Vector3f::ZERO )
		, mNoAvoidTime( 0 )
		, mMoveType( Normal )
		, mbMoveEnabled( true )
		, mTargetVector3d( false )
	{
	}

	SteeringSystem::~SteeringSystem()
	{
	}

	void SteeringSystem::RenderDebug()
	{
		RenderBuffer::AddLine( GetClient()->GetEyePosition(), GetTarget(), COLOR::GREEN, IGame::GetDeltaTimeSecs()*2.f );
	}

	bool SteeringSystem::InTargetRadius() const
	{
		return mDistanceToTarget <= mTargetRadius;
	}

	Vector3f SteeringSystem::GetMoveVector( MoveType _movetype )
	{
		return mbMoveEnabled ? mMoveVec : Vector3f::ZERO;
	}

	bool SteeringSystem::SetTarget( const Vector3f &_pos, float _radius, MoveMode _movemode, bool _in3d )
	{
		mTargetVector = _pos;
		mTargetRadius = _radius;
		mTargetVector3d = _in3d;
		mMoveMode = _movemode;
		mMoveType = Arrive;
		return true;
	}

	void SteeringSystem::SetNoAvoidTime( int _time )
	{
		mNoAvoidTime = IGame::GetTime() + _time;
	}

	void SteeringSystem::UpdateSteering()
	{
		Prof( UpdateSteering );

		if ( !mbMoveEnabled )
		{
			mMoveVec = Vector3f::ZERO;
			return;
		}

		const Vector3f bottomBounds = GetClient()->GetWorldBounds().GetCenterBottom();

		mDistanceToTarget = 0.f;
		switch ( mMoveType )
		{
			case Normal:
				mMoveVec = mTargetVector - bottomBounds;
				if ( !mTargetVector3d )
					mMoveVec = mMoveVec.Flatten();
				mDistanceToTarget = mMoveVec.Normalize();
				break;
			case Arrive:
				mMoveVec = mTargetVector - bottomBounds;
				if ( !mTargetVector3d )
					mMoveVec = mMoveVec.Flatten();
				mDistanceToTarget = mMoveVec.Normalize();
				//.mvMoveVec *= _Arrive(.mvTargetVector, normal);
				break;
		}

		//////////////////////////////////////////////////////////////////////////
		if ( mDistanceToTarget <= mTargetRadius )
		{
			mMoveVec = Vector3f::ZERO;
			return;
		}

		//////////////////////////////////////////////////////////////////////////

		if ( GetClient()->IsDebugEnabled( BOT_DEBUG_MOVEVEC ) )
		{
			Vector3f vPos = GetClient()->GetWorldBounds().Center;
			RenderBuffer::AddLine( vPos, vPos + mMoveVec * 64.f, COLOR::GREEN, 0.1f );
		}

		if ( GetClient()->HasEntityFlag( ENT_FLAG_ON_ICE ) )
		{
			GetClient()->PressButton( BOT_BUTTON_WALK );
		}
	}

	//////////////////////////////////////////////////////////////////////////

	void SteeringSystem::Enter()
	{
		mTargetVector = GetClient()->GetWorldBounds().GetCenterBottom();
		GetClient()->SetMovementVector( Vector3f::ZERO );
		FINDSTATEIF( FollowPath, GetRootState(), Stop() );
	}

	State::StateStatus SteeringSystem::Update( float fDt )
	{
		Prof( SteeringSystem );
		UpdateSteering();
		GetClient()->SetMovementVector( GetMoveVector() );
		switch ( mMoveMode )
		{
			case Walk:
				GetClient()->PressButton( BOT_BUTTON_WALK );
				break;
			case Run:
			default:
				/*GetClient()->ReleaseButton(BOT_BUTTON_WALK);*/
				break;
		}
		return State_Busy;
	}
};