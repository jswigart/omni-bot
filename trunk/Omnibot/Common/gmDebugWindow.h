////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __GM_DEBUGWINDOW_H__
#define __GM_DEBUGWINDOW_H__

#include "common.h"

#ifdef ENABLE_DEBUG_WINDOW

#include "gmbinder2.h"

class gmMachine;
void gmBindDebugWindowLibrary(gmMachine *_machine);
void gmBindDebugWindowLibraryDocs(gmMachine *_m, gmBind2::TableConstructor &_tc);

#endif

#endif
