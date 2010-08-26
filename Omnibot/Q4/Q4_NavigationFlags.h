////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: drevil $
// $LastChangedDate: 2008-02-11 20:03:50 -0800 (Mon, 11 Feb 2008) $
// $LastChangedRevision: 2438 $
//
// Title: Q4 Navigation Flags
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __Q4_NAVIGATIONFLAGS_H__
#define __Q4_NAVIGATIONFLAGS_H__

#include "NavigationFlags.h"

static const NavFlags F_Q4_NAV_HIDE			= F_NAV_NEXT;
static const NavFlags F_Q4_NAV_CAMP			= (F_NAV_NEXT<<1);
static const NavFlags F_Q4_NAV_CAPPOINT		= (F_NAV_NEXT<<2);
static const NavFlags F_Q4_NAV_DEADZONE		= (F_NAV_NEXT<<3);

#endif

