////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: DrEvil $
// $LastChangedDate: 2006-09-22 20:45:46 -0700 (Fri, 22 Sep 2006) $
// $LastChangedRevision: 1284 $
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __GM_TIMER_H__
#define __GM_TIMER_H__

#include "gmBind.h"

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
