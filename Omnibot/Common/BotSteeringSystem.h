////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __BOTSTEERINGSYSTEM_H__
#define __BOTSTEERINGSYSTEM_H__

#include "StateMachine.h"
#include "MemoryRecord.h"

class Client;

typedef std::vector<GameEntity> EntityList;

namespace AiState
{
	// class: SteeringSystem
	//		Handles steering of the bot based on avoidance, seeking,...
	class SteeringSystem : public StateChild
	{
	public:
		enum MoveType
		{
			Normal,
			Arrive,
		};

		static const float sDefaultRadius;

		// function: Update
		//		Allows the steering system to process steering effectors
		//		and calculate the <.mvMoveVec>
		void UpdateSteering();

		// function: SetTarget
		//		Sets <.mTargetVector>, which is the goal point the bot
		//		should be heading for.
		bool SetTarget( const Vector3f & pos, float radius = sDefaultRadius, float pathDist = 0.0f, MoveMode movemode = Run, bool in3d = false );

		const Vector3f &GetTarget() const
		{
			return mTargetVector;
		}
		float GetTargetRadius() const
		{
			return mTargetRadius;
		}

		bool InTargetRadius() const;

		// function: GetMoveVector
		//		Returns the <.mvMoveVec> for the direction the bot wants to move
		//		This should be called after Update()
		Vector3f GetMoveVector( MoveType _movetype = Normal );

		// function: SetNoAvoidTime
		//		Set's a time for disabling avoid. Normally goals should just set it for
		//		the time in the current frame. But it may sometimes be useful to set it longer
		void SetNoAvoidTime( int _time );

		// function: SetEnableMovement
		//		Enables/Disables this bots movement.
		inline void SetEnableMovement( bool _b )
		{
			mbMoveEnabled = _b;
		}

		// State Functions
		void Enter();
		StateStatus Update( float fDt );

		void RenderDebug();

		SteeringSystem();
		virtual ~SteeringSystem();
	protected:
		// var: mTargetVector
		//		The current position we are suppose to be going for
		Vector3f		mTargetVector;
		float			mTargetPathDist;
		float			mTargetRadius;
		MoveMode		mMoveMode;
		
		// var: mvMoveVec
		//		The latest calculated movement vector
		Vector3f		mMoveVec;
		// var: mObstacles
		//		A list of entities we should try to avoid.
		MemoryRecords	mObstacles;
		EntityList		mNearbyPlayers;
		// var: mNoAvoidTime
		//		A time to not calculate avoidance. If > IGame::GetTime() avoidance is not processed.
		int				mNoAvoidTime;

		MoveType mMoveType;

		// var: mNoAvoidTime
		//		A time to not calculate avoidance. If > IGame::GetTime() avoidance is not processed.
		bool	 mbMoveEnabled;

		bool	 mTargetVector3d;
	};
};
#endif
