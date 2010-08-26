////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: drevil $
// $LastChangedDate: 2008-07-05 03:18:03 -0700 (Sat, 05 Jul 2008) $
// $LastChangedRevision: 3023 $
//
// File: JA Navigation Flags
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __JA_NAVIGATIONFLAGS_H__
#define __JA_NAVIGATIONFLAGS_H__

#include "NavigationFlags.h"

static const NavFlags F_JA_NAV_HIDE			= F_NAV_NEXT;
static const NavFlags F_JA_NAV_CAMP			= (F_NAV_NEXT<<1);
static const NavFlags F_JA_NAV_CAPPOINT		= (F_NAV_NEXT<<2);
static const NavFlags F_JA_NAV_WALL			= (F_NAV_NEXT<<3);
static const NavFlags F_JA_NAV_BRIDGE		= (F_NAV_NEXT<<4);
static const NavFlags F_JA_NAV_BUTTON1		= (F_NAV_NEXT<<5);
static const NavFlags F_JA_NAV_FORCEJUMP		= (F_NAV_NEXT<<6);

#endif

