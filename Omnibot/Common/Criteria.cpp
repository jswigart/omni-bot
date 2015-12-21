////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "Criteria.h"
#include "Client.h"
#include "IGame.h"
#include "BotWeaponSystem.h"
#include "System.h"

bool CheckCriteria::ParseOperator( uint32_t sHash )
{
	switch ( sHash )
	{
		case 0x862a54e3 /* lessthan */:
		case 0x390caefb /* < */:
			mOperator = Criteria::OP_LESSTHAN;
			return true;
		case 0x81bc04be /* greaterthan */:
		case 0x3b0cb221 /* > */:
			mOperator = Criteria::OP_GREATERTHAN;
			return true;
		case 0x513c8d94 /* equals */:
		case 0x90f4dccf /* == */:
			mOperator = Criteria::OP_EQUALS;
			return true;
	}
	return false;
}

bool CheckCriteria::Check( Client * bot )
{
	return mpfnCheckCriteria ? mpfnCheckCriteria( *this, bot ) : false;
}

bool Base_CheckCriteria( CheckCriteria &crit, Client * bot )
{
	using namespace Criteria;

	EntityInfo subInfo;
	const bool gotSubjectInfo = IGame::GetEntityInfo( crit.mSubject.GetEntity(), subInfo );

	switch ( crit.mCriteria )
	{
		case ON_ENTITY_DELETED:
		{
			if ( !IGame::IsEntityValid( crit.mSubject.GetEntity() ) )
				return true;
			break;
		}
		case ON_ENTITY_FLAG:
		{
			if ( gotSubjectInfo )
			{
				bool bResult = subInfo.mFlags.CheckFlag( crit.mOperand[ 0 ].GetInt() );
				if ( crit.mNegated )
					bResult = !bResult;
				return bResult;
			}
			break;
		}
		case ON_ENTITY_HEALTH:
		{
			if ( gotSubjectInfo )
			{
				bool bResult = false;
				HANDLE_OPERATORS( bResult, subInfo.mHealth.mNum, crit.mOperand[ 0 ].GetNumAsInt() );
				if ( crit.mNegated )
					bResult = !bResult;
				return bResult;
			}
			break;
		}
		case ON_ENTITY_WEAPON:
		{
			int weaponId = 0;
			FireMode weaponMode = Primary;
			gEngineFuncs->GetEquippedWeapon( crit.mSubject.GetEntity(), weaponId, weaponMode );
			bool bResult = weaponId == crit.mOperand[ 0 ].GetInt();
			if ( crit.mNegated )
				bResult = !bResult;
			return bResult;
		}
		case ON_ENTITY_VELOCITY:
		{
			Vector3f v;
			if ( EngineFuncs::EntityVelocity( crit.mSubject.GetEntity(), v ) )
			{
				bool bResult = false;
				switch ( crit.mOperator )
				{
					case OP_NONE:
					{
						break;
					}
					case OP_LESSTHAN:
					{
						if ( crit.mOperand[ 0 ].IsVector() )
							bResult = v < Vector3f( crit.mOperand[ 0 ].GetVector() );
						else if ( crit.mOperand[ 0 ].IsFloatOrInt() )
							bResult = v.Length() < Vector3f( crit.mOperand[ 0 ].GetVector() ).Length();
						break;
					}
					case OP_GREATERTHAN:
					{
						if ( crit.mOperand[ 0 ].IsVector() )
							bResult = v > Vector3f( crit.mOperand[ 0 ].GetVector() );
						else if ( crit.mOperand[ 0 ].IsFloatOrInt() )
							bResult = v.Length() > Vector3f( crit.mOperand[ 0 ].GetVector() ).Length();
						break;
					}
					case OP_EQUALS:
					{
						if ( crit.mOperand[ 0 ].IsVector() )
							bResult = v == Vector3f( crit.mOperand[ 0 ].GetVector() );
						else if ( crit.mOperand[ 0 ].IsFloatOrInt() )
							bResult = v.Length() == Vector3f( crit.mOperand[ 0 ].GetVector() ).Length();
						break;
					}
				}
				if ( crit.mNegated )
					bResult = !bResult;
				return bResult;
			}
		}
		case ON_ENTITY_WEAPONCHARGED:
		{
			bool bResult = gEngineFuncs->IsWeaponCharged( crit.mSubject.GetEntity(), crit.mOperand[ 0 ].GetInt(), Primary );
			if ( crit.mNegated )
				bResult = !bResult;
			return bResult;
		}
		case ON_ENTITY_WEAPONHASAMMO:
		{
			bool bResult = false;
			WeaponPtr wpn = bot->GetWeaponSystem()->GetWeapon( crit.mOperand[ 0 ].GetInt() );
			if ( wpn )
				bResult = wpn->GetFireMode( Primary ).HasAmmo( crit.mOperand[ 1 ].GetInt() );
			if ( crit.mNegated )
				bResult = !bResult;
			return bResult;
		}
		case ON_MAPGOAL_AVAILABLE:
		{
			bool bResult = false;
			MapGoalPtr mg = System::mInstance->mGoalManager->GetGoal( crit.mOperand[ 0 ].GetInt() );
			if ( mg && mg->IsAvailable( bot->GetTeam() ) )
				bResult = true;
			if ( crit.mNegated )
				bResult = !bResult;
			return bResult;
		}
		case NONE:
		case FIRST_USER_CRITERIA:
			break;
	}
	return false;
}

pfnCheckCriteria CheckCriteria::mpfnCheckCriteria = Base_CheckCriteria;
