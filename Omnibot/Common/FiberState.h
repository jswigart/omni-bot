////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy: jswigart $
// $LastChangedDate: 2010-12-17 00:53:18 -0600 (Fri, 17 Dec 2010) $
// $LastChangedRevision: 276 $
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __FIBERSTATE_H__
#define __FIBERSTATE_H__

#include "FiberManager.h"
#include "StateMachine.h"
#include "gmbinder2/gmbinder2_functraits.h"

namespace AiState
{
	class ScriptState {
	public:
		virtual const char * GetName() const { return ""; }
		virtual float GetPriority() { return 0.0f; }
		virtual void Enter() {}
		virtual void Exit() {}
		virtual State::StateStatus Update(float fDt) { return State::State_Finished; }
	};

	class StateFiber : public Fiber {
	public:
		enum funcType_t {
			FIBER_GETPRIORITY,
			FIBER_ENTER,
			FIBER_EXIT,
			FIBER_UPDATE
		};
		virtual void Run() {
			switch ( funcType )
			{
			case FIBER_GETPRIORITY:
				statusPriority = scriptState->GetPriority();
				break;
			case FIBER_ENTER:
				scriptState->Enter();
				break;
			case FIBER_EXIT:
				scriptState->Exit();
				break;
			case FIBER_UPDATE:
				statusUpdate = scriptState->Update( dt );
				break;
			}
		}

		void				SetDt( float t ) { dt = t; }
		float				GetPriority() const { return statusPriority; }
		State::StateStatus	GetStatus() const { return statusUpdate; }

		StateFiber( funcType_t func, ScriptState * state );
	private:
		funcType_t		funcType;
		ScriptState *	scriptState;

		float			dt;

		// internal data
		float				statusPriority;
		State::StateStatus	statusUpdate;

		StateFiber() {}
	};
};

namespace AiState
{
	class FiberState : public State {
	public:
		void GetDebugString(std::stringstream &out);
		void RenderDebug();

		float GetPriority();
		void Enter();
		void Exit();
		StateStatus Update(float fDt);

		FiberState( ScriptState * state );
	private:
		ScriptState *	scriptState;

		StateFiber 		fiberGetPriority;
		StateFiber 		fiberEnter;
		StateFiber 		fiberExit;
		StateFiber 		fiberUpdate;
	};
};

#endif
