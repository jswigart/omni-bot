////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __ETF_CLIENT_H__
#define __ETF_CLIENT_H__

#include "TF_Client.h"
#include "ETF_Config.h"

// class: ETF_Client
//		ETF Bot Class
class ETF_Client : public TF_Client
{
public:
	void Init(int _gameid);

	NavFlags GetTeamFlag(int _team);

	float NavCallback(const NavFlags &_flag, Waypoint *from, Waypoint *to);

	ETF_Client();
	virtual ~ETF_Client();
};

#endif
