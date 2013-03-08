////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __GM_TIMER_H__
#define __GM_TIMER_H__

#include "gmBind.h"
#include "Timer.h"

class gmTimer : public gmBind<Timer, gmTimer>
{
public:
	GMBIND_DECLARE_FUNCTIONS( );

	//////////////////////////////////////////////////////////////////////////
	// Functions
	static int gmfResetTimer(gmThread *a_thread);
	static int gmfGetElapsedTime(gmThread *a_thread);
};

#endif
