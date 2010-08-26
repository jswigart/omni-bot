////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: drevil $
// $LastChangedDate: 2009-06-27 23:27:19 -0500 (Sat, 27 Jun 2009) $
// $LastChangedRevision: 4303 $
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __GM_DEBUGWINDOW_H__
#define __GM_DEBUGWINDOW_H__

#ifdef ENABLE_DEBUG_WINDOW

class gmMachine;
void gmBindDebugWindowLibrary(gmMachine *_machine);
void gmBindDebugWindowLibraryDocs(gmMachine *_m, gmBind2::TableConstructor &_tc);

#endif

#endif
