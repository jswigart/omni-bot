////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "TF_Config.h"
#include "FilterClosestTF.h"
#include "BotTargetingSystem.h"

// class: FilterClosestTF
//		This filter is specific to team fortress, and should take into account additional sensory considerations,
//		such as spy disguises, spy feigning

FilterClosestTF::FilterClosestTF( Client *_client, AiState::SensoryMemory::Type _type ) 
	: FilterClosest( _client, _type )
{
}

bool FilterClosestTF::CheckEx( const GameEntity _ent, const MemoryRecord &_record )
{
	if ( _record.mTargetInfo.mEntInfo.mPowerUps.CheckFlag( TF_PWR_CLOAKED ) &&
		 mClient->GetTargetingSystem()->GetLastTarget() != _ent )
		 return false;

	if ( _record.mTargetInfo.mEntInfo.mPowerUps.CheckFlag( TF_ENT_FLAG_BUILDINPROGRESS ) )
		return false;

	int iTeam = 0, iClass = 0;
	gTeamFortressFuncs->GetDisguiseInfo( _record.mTargetInfo.mEntInfo, iTeam, iClass );
	if ( iTeam != TF_TEAM_NONE && iTeam < TF_NUM_CLASSES )
		return false;

	return true;
}