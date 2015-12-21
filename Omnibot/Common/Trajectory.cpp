////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "Trajectory.h"
#include "IGame.h"
#include "RenderBuffer.h"

namespace Trajectory
{
	int Calculate( const Vector3f &_start, const Vector3f &_end, float _speed, float _gravity, AimTrajectory _bal[ 2 ] )
	{
		int n, i;
		float x, y, a, b, c, d, sqrtd, inva, p[ 2 ];

		x = ( Vector2f( _end.X(), _end.Y() ) - Vector2f( _start.X(), _start.Y() ) ).Length();
		y = _end[ 2 ] - _start[ 2 ];

		a = 4.0f * y * y + 4.0f * x * x;
		b = -4.0f * _speed * _speed - 4.0f * y * _gravity;
		c = _gravity * _gravity;

		d = b * b - 4.0f * a * c;
		if ( d <= 0.0f || a == 0.0f )
		{
			return 0;
		}
		sqrtd = Mathf::Sqrt( d );
		inva = 0.5f / a;
		p[ 0 ] = ( -b + sqrtd ) * inva;
		p[ 1 ] = ( -b - sqrtd ) * inva;
		n = 0;
		for ( i = 0; i < 2; i++ )
		{
			if ( p[ i ] <= 0.0f )
				continue;

			d = Mathf::Sqrt( p[ i ] );
			_bal[ n ].mAngle = Mathf::ATan2( 0.5f * ( 2.0f * y * p[ i ] - _gravity ) / d, d * x );
			_bal[ n ].mTime = x / ( Mathf::Cos( _bal[ n ].mAngle ) * _speed );
			//bal[n].angle = Mathf::AngleNormalize180( Mathf::RadToDeg( bal[n].angle ) );

			// Calculate the Aim vector for this result.
			Vector3f vTargetPos = _end;
			Vector3f vAimDir = vTargetPos - _start;
			vTargetPos.Z() = _start.Z() + Mathf::Tan( _bal[ n ].mAngle ) * Vector2f( vAimDir.X(), vAimDir.Y() ).Length();
			_bal[ n ].mAimVector = vTargetPos - _start;
			_bal[ n ].mAimVector.Normalize();

			n++;
		}

		return n;
	}

	float MaxHeightForTrajectory( const Vector3f &start, float zVel, float gravity )
	{
		float t = zVel / gravity;
		return start.Z() - 0.5f * gravity * ( t * t );
	}

	float HeightForTrajectory( const Vector3f &start, float zVel, float gravity, float t )
	{
		return start.Z() - 0.5f * gravity * ( t * t );
	}

	//////////////////////////////////////////////////////////////////////////

	TrajectorySim::TrajectorySim()
		: mPosition( 0.f, 0.f, 0.f )
		, mVelocity( 0.f, 0.f, 0.f )
		, mInterval( 0.05f )
		, mDuration( 5.f )
		, mBounceLoss( 0.f )
		, mGravityMultiplier( 1.f )
		, mTraceBounce( true )
		, mStopAtHit( false )
		, mStopPos( 0.f, 0.f, 0.f )
	{
	}

	int TrajectorySim::FromTable( gmThread *a_thread, gmTableObject *a_table )
	{
		gmVariable v0 = a_table->Get( a_thread->GetMachine(), "Position" );
		if ( !v0.GetVector( mPosition ) )
		{
			GM_EXCEPTION_MSG( "expected Position field in table" );
			return GM_EXCEPTION;
		}
		gmVariable v1 = a_table->Get( a_thread->GetMachine(), "Velocity" );
		if ( !v1.GetVector( mVelocity ) )
		{
			GM_EXCEPTION_MSG( "expected Velocity field in table" );
			return GM_EXCEPTION;
		}
		gmVariable vInterval = a_table->Get( a_thread->GetMachine(), "Interval" );
		if ( !vInterval.IsNull() && !vInterval.GetFloatSafe( mInterval, mInterval ) )
		{
			GM_EXCEPTION_MSG( "expected Interval field as float or int" );
			return GM_EXCEPTION;
		}

		gmVariable vDuration = a_table->Get( a_thread->GetMachine(), "Duration" );
		if ( !vDuration.IsNull() && !vDuration.GetFloatSafe( mDuration, mDuration ) )
		{
			GM_EXCEPTION_MSG( "expected Duration field as float or int" );
			return GM_EXCEPTION;
		}

		gmVariable vBounceLoss = a_table->Get( a_thread->GetMachine(), "BounceLoss" );
		if ( !vBounceLoss.IsNull() && !vBounceLoss.GetFloatSafe( mBounceLoss, mBounceLoss ) )
		{
			GM_EXCEPTION_MSG( "expected BounceLoss field as float or int" );
			return GM_EXCEPTION;
		}

		gmVariable vGravMult = a_table->Get( a_thread->GetMachine(), "GravityMultiplier" );
		if ( !vGravMult.IsNull() && !vGravMult.GetFloatSafe( mGravityMultiplier, mGravityMultiplier ) )
		{
			GM_EXCEPTION_MSG( "expected BounceLoss field as float or int" );
			return GM_EXCEPTION;
		}

		gmVariable vStopAtHit = a_table->Get( a_thread->GetMachine(), "StopAtHit" );
		if ( !vStopAtHit.IsNull() && !vStopAtHit.GetBoolSafe( mStopAtHit, mStopAtHit ) )
		{
			GM_EXCEPTION_MSG( "expected StopAtHit field as true/false" );
			return GM_EXCEPTION;
		}

		gmVariable vTraceBounce = a_table->Get( a_thread->GetMachine(), "TraceBounce" );
		if ( !vTraceBounce.IsNull() && !vTraceBounce.GetBoolSafe( mTraceBounce, mTraceBounce ) )
		{
			GM_EXCEPTION_MSG( "expected StopAtHit field as true/false" );
			return GM_EXCEPTION;
		}

		return GM_OK;
	}

	void TrajectorySim::Render( obColor _col, float _time )
	{
		float interval = std::max( mInterval, 0.05f );

		obTraceResult tr;

		Vector3f vel = mVelocity;
		Vector3f pos = mPosition;
		Vector3f vlast = mPosition;
		for ( float t = interval; t <= mDuration; )
		{
			bool Hit = false;
			if ( mTraceBounce &&
				EngineFuncs::TraceLine( tr, vlast, pos, 0, TR_MASK_SOLID, Utils::GetLocalGameId(), false ) &&
				tr.mFraction < 1.f )
			{
				Hit = true;

				//Vector3f ray = pos - vlast;
				//const float length = ray.Normalize();

				// reflect the velocity
				vel = vel.Reflect( tr.mNormal );
				vel *= mBounceLoss;

				// adjust the position with the rest of the bounce
				float whatsleftT = ( 1.f - tr.mFraction );
				pos = Vector3f( tr.mEndpos ) + ( vel * whatsleftT ) * interval;

				//t += interval * (interval * tr.mFraction);
			}
			else
				pos += ( vel * interval ); // move the entire interval if it didnt hit anything

			t += interval;

			vel.Z() += ( IGame::GetGravity() * mGravityMultiplier ) * interval;

			//////////////////////////////////////////////////////////////////////////

			RenderBuffer::AddLine( vlast, pos, _col, _time );
			vlast = pos;

			mStopPos = pos;

			if ( mStopAtHit && Hit )
				break;
		}
	}
};