////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy: ken.nickel $
// $LastChangedDate: 2011-09-13 14:24:15 -0500 (Tue, 13 Sep 2011) $
// $LastChangedRevision: 387 $
//
////////////////////////////////////////////////////////////////////////////////

#include "FiberState.h"

//////////////////////////////////////////////////////////////////////////
namespace AiState
{
	StateFiber::StateFiber( funcType_t func, ScriptState * state ) :
		funcType( func ),
		scriptState( state ),
		dt( 0.0f ),
		statusPriority( 0.0f ),
		statusUpdate( State::State_Finished ) {
	}

	//////////////////////////////////////////////////////////////////////////
	FiberState::FiberState( ScriptState * state ) :
		State( state->GetName() ),
		scriptState( state ),
		fiberGetPriority( StateFiber::FIBER_GETPRIORITY, scriptState ),
		fiberEnter( StateFiber::FIBER_ENTER, scriptState ),
		fiberExit( StateFiber::FIBER_EXIT, scriptState ),
		fiberUpdate( StateFiber::FIBER_UPDATE, scriptState ) {
	}
	float FiberState::GetPriority() {
		fiberGetPriority.Execute();
		return fiberGetPriority.GetPriority();
	}
	void FiberState::Enter() {
		fiberEnter.Execute();
	}
	void FiberState::Exit() {
		fiberExit.Execute();
	}
	State::StateStatus FiberState::Update(float fDt) {
		fiberUpdate.SetDt( fDt );
		fiberUpdate.Execute();
		return fiberUpdate.GetStatus();
	}
};