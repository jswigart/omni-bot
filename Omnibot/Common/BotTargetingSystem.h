////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __BOTTARGETINGSYSTEM_H__
#define __BOTTARGETINGSYSTEM_H__

#include "FilterSensory.h"

class Client;

namespace AiState
{
	// class: TargetingSystem
	class TargetingSystem : public StateChild
	{
	public:

		// function: HasTarget
		//		true/false whether the bot has a target or not.
		inline bool HasTarget() const { return mCurrentTarget.IsValid(); }

		// function: GetCurrentTarget
		//		Accessor for the bots current target.
		inline GameEntity GetCurrentTarget() const { return mCurrentTarget; }

		// function: GetLastTarget
		//		Accessor for the bots last target.
		inline const GameEntity &GetLastTarget() const { return mLastTarget; }

		void ForceTarget(GameEntity _ent);

		// function: GetCurrentTargetRecord
		//		Accessor for the bots current target data.
		const MemoryRecord *GetCurrentTargetRecord() const;

		// function: SetDefaultTargetingFilter
		//		Set the <FilterSensory> that will be used for targeting
		inline void SetDefaultTargetingFilter(FilterPtr _filter) { mDefaultFilter = _filter; }

		// function: GetTargetingFilter
		//		Get the current <FilterSensory> being used for targeting
		inline FilterPtr GetTargetingFilter() const { return mDefaultFilter; }

		// State stuff

		void RenderDebug();

		void Initialize();
		void Exit();
		StateStatus Update(float fDt);

		TargetingSystem();
		virtual ~TargetingSystem();
	protected:
		// ptr: mDefaultFilter
		//		Pointer to the filter to use for targeting.
		FilterPtr	 mDefaultFilter;

		// var: mCurrentTarget
		//		The currently targeted entity
		GameEntity	 mCurrentTarget;

		// var: mLastTarget
		//		The last targeted entity
		GameEntity	 mLastTarget;

		GameEntity	 mForceTarget;
	};
};
#endif
