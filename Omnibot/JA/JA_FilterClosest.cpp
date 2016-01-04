////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "JA_FilterClosest.h"
#include "JA_Messages.h"
#include "BotTargetingSystem.h"

JA_FilterClosest::JA_FilterClosest( Client *_client, AiState::SensoryMemory::Type _type ) 
	: FilterClosest( _client, _type )
{
}

bool JA_FilterClosest::CheckEx( const GameEntity _ent, const MemoryRecord &_record )
{
	if ( _record.mTargetInfo.mEntInfo.mClassId == JA_CLASSEX_CORPSE )
		return false;

	if ( _record.mTargetInfo.mEntInfo.mFlags.CheckFlag( JA_ENT_FLAG_CLOAKED ) && !_record.mTargetInfo.mEntInfo.mFlags.CheckFlag( JA_ENT_FLAG_MUTANT ) )
		return false;

	if ( gJediAcademyFuncs->HasMeMindTricked( mClient->GetGameEntity(), mClient->GetTargetingSystem()->GetLastTarget() ) )
		return false;

	return true;
}