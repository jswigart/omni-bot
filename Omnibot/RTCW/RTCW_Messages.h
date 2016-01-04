////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
// Title: RTCW Message Structure Definitions
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __RTCW_MESSAGES_H__
#define __RTCW_MESSAGES_H__

#include "Base_Messages.h"
#include "RTCW_Config.h"

enum RTCW_Events
{
	RTCW_EVENT_BEGIN = MSG_BASE_LAST,

	RTCW_EVENT_END
};

#pragma pack(push)
#pragma pack(4)

//////////////////////////////////////////////////////////////////////////

#pragma pack(pop)

#endif
