////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __TRAJECTORY_H__
#define __TRAJECTORY_H__

#include "common.h"

class gmThread;
class gmTableObject;

// namespace: Trajectory
//		This class simulates a trajectory of a projectile
namespace Trajectory
{
	struct AimTrajectory
	{
		Vector3f		 mAimVector;	// vector to aim at to accomplish this trajectory.
		float			 mAngle;		// angle in degrees in the range [-180, 180]
		float			 mTime;			// time it takes before the projectile arrives
	};

	int Calculate( const Vector3f &start, const Vector3f &end, float speed, float gravity, AimTrajectory bal[2] );
	float MaxHeightForTrajectory( const Vector3f &start, float zVel, float gravity );
	float HeightForTrajectory( const Vector3f &start, float zVel, float gravity, float t );

	struct TrajectorySim
	{
		Vector3f	 mPosition;
		Vector3f	 mVelocity;

		float		 mInterval;
		float		 mDuration;

		float		 mBounceLoss;
		float		 mGravityMultiplier;

		bool		 mTraceBounce;
		bool		 mStopAtHit;

		//////////////////////////////////////////////////////////////////////////
		Vector3f	 mStopPos;

		int FromTable(gmThread *a_thread, gmTableObject *a_table);

		void Render(obColor _color, float _duration);

		TrajectorySim();
	};
};

#endif
