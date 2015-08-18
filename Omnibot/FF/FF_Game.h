////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __FF_GAME_H__
#define __FF_GAME_H__

class gmMachine;
class gmTableObject;

#include "IGame.h"
#include "TF_Game.h"

// class: FF_Game
//		Game Type for Fortress Forever.
class FF_Game : public TF_Game
{
public:
	bool Init( System & system );

	virtual Client *CreateGameClient();

	void GetNavParms( NavParms & navParms ) const;

	void GetTeamEnumeration( const IntEnum *&_ptr, int &num );

	FF_Game();
	~FF_Game();
protected:
};

#endif
