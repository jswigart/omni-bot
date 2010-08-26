////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: DrEvil $
// $LastChangedDate: 2007-04-19 22:31:44 -0700 (Thu, 19 Apr 2007) $
// $LastChangedRevision: 1819 $
//
////////////////////////////////////////////////////////////////////////////////

#include "PrecompCommon.h"
#include "Regulator.h"
#include "IGame.h"



bool Regulator::IsReady()
{
	int iCurrentTime = IGame::GetTime();
	if(iCurrentTime >= m_NextUpdateTime)
	{
		m_NextUpdateTime = iCurrentTime + m_UpdateInterval;
		return true;
	}
	return false;
}

