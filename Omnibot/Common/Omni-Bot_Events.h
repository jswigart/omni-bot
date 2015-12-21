////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
// about: Generic Bot Events
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __OMNIBOT_EVENTS_H__
#define __OMNIBOT_EVENTS_H__

enum BlackBoardKey
{
	bbk_All = 0,
	bbk_DelayGoal,
	bbk_IsTaken,
	bbk_RunAway,

	// This must stay last.
	bbk_LastKey,
	bbk_FirstScript,
};

#endif
