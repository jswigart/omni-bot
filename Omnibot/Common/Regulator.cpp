////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "Regulator.h"
#include "IGame.h"

bool Regulator::IsReady()
{
	const int iCurrentTime = IGame::GetTime();
	if ( iCurrentTime >= mNextUpdateTime )
	{
		mNextUpdateTime = iCurrentTime + mUpdateInterval;
		return true;
	}
	return false;
}