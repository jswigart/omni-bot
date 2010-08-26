////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: DrEvil $
// $LastChangedDate: 2007-05-16 16:53:29 -0700 (Wed, 16 May 2007) $
// $LastChangedRevision: 1924 $
//
////////////////////////////////////////////////////////////////////////////////

#include "PrecompCommon.h"

// Global variables.
IEngineInterface *g_EngineFuncs = 0;

#ifdef BOOST_NO_EXCEPTIONS
void throw_exception(std::exception const & e)
{
	OBASSERT(0, e.what());
}
#endif
