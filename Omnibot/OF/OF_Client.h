////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __OF_CLIENT_H__
#define __OF_CLIENT_H__

#include "TF_Client.h"
#include "OF_Config.h"

// class: OF_Client
//		OF Bot Class
class OF_Client : public TF_Client
{
public:
	void Init( int _gameid );

	NavFlags GetTeamFlag( int _team ) const;
	void GetNavFlags( NavFlags & includeFlags, NavFlags & excludeFlags );

	OF_Client();
	virtual ~OF_Client();
};

#endif
