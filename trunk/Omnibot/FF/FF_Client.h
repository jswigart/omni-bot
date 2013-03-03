////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: jswigart $
// $LastChangedDate: 2010-08-28 00:12:05 -0500 (Sat, 28 Aug 2010) $
// $LastChangedRevision: 32 $
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __FF_CLIENT_H__
#define __FF_CLIENT_H__

#include "TF_Client.h"
#include "FF_Config.h"

// class: FF_Client
//		OF Bot Class
class FF_Client : public TF_Client
{
public:
	void Init(int _gameid);

	float NavCallback(const NavFlags &_flag, Waypoint *from, Waypoint *to);
	

	NavFlags GetTeamFlag(int _team);

	FF_Client();
	virtual ~FF_Client();
};

#endif
