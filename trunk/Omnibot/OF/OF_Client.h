////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: drevil $
// $LastChangedDate: 2008-12-31 10:36:40 -0800 (Wed, 31 Dec 2008) $
// $LastChangedRevision: 3830 $
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __OF_CLIENT_H__
#define __OF_CLIENT_H__

#include "TF_Client.h"
#include "OF_Config.h"

// class: OF_Client
//		OF Bot Class
class OF_Client : public TF_Client
{
public:
	void Init(int _gameid);

	float NavCallback(const NavFlags &_flag, Waypoint *from, Waypoint *to);

	OF_Client();
	virtual ~OF_Client();
};

#endif
