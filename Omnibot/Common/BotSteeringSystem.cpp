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
#include "Base_Messages.h"
#include "RenderBuffer.h"

namespace AiState
{
	const float SteeringSystem::sDefaultRadius = 32.0f;

	SteeringSystem::SteeringSystem()
		: StateChild( "SteeringSystem" )
		, mTargetVector( Vector3f::ZERO )
		, mTargetPathDist( 0.0f )
		, mTargetRadius( 32.f )
		, mMoveMode( Run )
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
		RenderBuffer::AddLine( GetClient()->GetEyePosition(), GetTarget(), COLOR::GREEN );
	}

	bool SteeringSystem::InTargetRadius() const
	{
		return mTargetPathDist <= mTargetRadius;
	}

	Vector3f SteeringSystem::GetMoveVector( MoveType _movetype )
	{
		return mbMoveEnabled ? mMoveVec : Vector3f::ZERO;
	}

	bool SteeringSystem::SetTarget( const Vector3f & pos, float radius, float pathDist, MoveMode movemode, bool in3d )
	{
		mTargetVector = pos;
		mTargetPathDist = pathDist;
		mTargetRadius = radius;
		mTargetVector3d = in3d;
		mMoveMode = movemode;
		mMoveType = Arrive;
		return true;
	}

	void SteeringSystem::SetNoAvoidTime( int _time )
	{
		mNoAvoidTime = IGame::GetTime() + _time;
	}

	void SteeringSystem::UpdateSteering()
	{
		rmt_ScopedCPUSample( UpdateSteering );

		if ( !mbMoveEnabled )
		{
			mMoveVec = Vector3f::ZERO;
			return;
		}

		const Vector3f bottomBounds = GetClient()->GetWorldBounds().GetCenterBottom();
		switch ( mMoveType )
		{
			case Normal:
				mMoveVec = mTargetVector - bottomBounds;
				if ( !mTargetVector3d )
					mMoveVec = mMoveVec.Flatten();
				break;
			case Arrive:
				mMoveVec = mTargetVector - bottomBounds;
				if ( !mTargetVector3d )
					mMoveVec = mMoveVec.Flatten();
				break;
		}

		//////////////////////////////////////////////////////////////////////////
		if ( mTargetPathDist <= mTargetRadius )
		{
			mMoveVec = Vector3f::ZERO;
			return;
		}

		//////////////////////////////////////////////////////////////////////////

		if ( GetClient()->IsDebugEnabled( BOT_DEBUG_MOVEVEC ) )
		{
			RenderBuffer::AddLine( bottomBounds, bottomBounds + mMoveVec * 64.f, COLOR::GREEN );
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
		FINDSTATEIF( Navigator, GetRootState(), Stop() );
	}

	State::StateStatus SteeringSystem::Update( float fDt )
	{
		rmt_ScopedCPUSample( SteeringSystem );

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