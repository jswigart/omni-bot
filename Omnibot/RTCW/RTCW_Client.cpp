////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "RTCW_Client.h"
#include "RTCW_VoiceMacros.h"
#include "RTCW_FilterClosest.h"
#include "RTCW_Messages.h"
#include "RTCW_BaseStates.h"

#include "System.h"
#include "BotTargetingSystem.h"
#include "BotWeaponSystem.h"

//////////////////////////////////////////////////////////////////////////
// MOVE THIS

class Limbo : public StateSimultaneous
{
public:

	float GetPriority()
	{
		return GetClient()->HasEntityFlag( RTCW_ENT_FLAG_INLIMBO ) ? 1.f : 0.f;
	}

	void Exit()
	{
		GetRootState()->OnSpawn();
	}

	State::StateStatus Update( float fDt )
	{
		// need to do something special here?
		return State_Busy;
	}

	Limbo() : StateSimultaneous( "Limbo" )
	{
	}
protected:
};

class Incapacitated : public StateSimultaneous
{
public:
	float GetPriority()
	{
		return !gEngineFuncs->IsAlive( GetClient()->GetGameEntity() ) ? 1.f : 0.f;
	}

	State::StateStatus Update( float fDt )
	{
		GetClient()->SetMovementVector( Vector3f::ZERO );
		return State_Busy;
	}

	Incapacitated() : StateSimultaneous( "Incapacitated" )
	{
	}
protected:
};

//////////////////////////////////////////////////////////////////////////

RTCW_Client::RTCW_Client() :
mBreakableTargetDistance( 0.f ),
mHealthEntityDistance( 1000.0f ),
mAmmoEntityDistance( 2000.0f ),
mWeaponEntityDistance( 1500.0f ),
mProjectileEntityDistance( 500.0f ),
mStrafeJump( 0 )
{
	mStepHeight = 8.0f; // subtract a small value as a buffer to jump
}

RTCW_Client::~RTCW_Client()
{
}

void RTCW_Client::Init( int _gameid )
{
	Client::Init( _gameid );

	FilterPtr filter( new RTCW_FilterClosest( this, AiState::SensoryMemory::EntEnemy ) );
	filter->AddCategory( ENT_CAT_SHOOTABLE );
	GetTargetingSystem()->SetDefaultTargetingFilter( filter );
}

void RTCW_Client::ProcessEvent( const Message &message, CallbackParameters &cb )
{
	Client::ProcessEvent( message, cb );
}

NavFlags RTCW_Client::GetTeamFlag( int _team ) const
{
	static const NavFlags defaultTeam = 0;
	switch ( _team )
	{
		case RTCW_TEAM_AXIS:
			return NAVFLAGS_TEAM1_ONLY;
		case RTCW_TEAM_ALLIES:
			return NAVFLAGS_TEAM2_ONLY;
		default:
			return defaultTeam;
	}
}

void RTCW_Client::GetNavFlags( NavFlags & includeFlags, NavFlags & excludeFlags )
{
	includeFlags = NAVFLAGS_WALK | NAVFLAGS_CROUCH;

	switch ( GetTeam() )
	{
		case RTCW_TEAM_AXIS:
			excludeFlags = (NavFlags)( ~NAVFLAGS_TEAM1_ONLY & NAVFLAGS_ALLTEAMS );
			break;
		case RTCW_TEAM_ALLIES:
			excludeFlags = (NavFlags)( ~NAVFLAGS_TEAM2_ONLY & NAVFLAGS_ALLTEAMS );
			break;
	}
}

void RTCW_Client::SendVoiceMacro( int _macroId )
{
	RTCW_VoiceMacros::SendVoiceMacro( this, _macroId );
}

int RTCW_Client::HandleVoiceMacroEvent( const EvVoiceMacro* msg )
{
	int voiceId = RTCW_VoiceMacros::GetVChatId( msg->mMessage );
	/*switch(iVoiceId)
	{
	case VCHAT_TEAM_PATHCLEARED:
	case VCHAT_TEAM_ENEMYWEAK:
	case VCHAT_TEAM_ALLCLEAR:
	case VCHAT_TEAM_INCOMING:
	case VCHAT_TEAM_FIREINTHEHOLE:
	case VCHAT_TEAM_ONDEFENSE:
	case VCHAT_TEAM_ONOFFENSE:
	case VCHAT_TEAM_TAKINGFIRE:
	case VCHAT_TEAM_MINESCLEARED:
	case VCHAT_TEAM_ENEMYDISGUISED:
	case VCHAT_TEAM_MEDIC:
	case VCHAT_TEAM_NEEDAMMO:
	case VCHAT_TEAM_NEEDBACKUP:
	case VCHAT_TEAM_NEEDENGINEER:
	case VCHAT_TEAM_COVERME:
	case VCHAT_TEAM_HOLDFIRE:
	case VCHAT_TEAM_WHERETO:
	case VCHAT_TEAM_NEEDOPS:
	case VCHAT_TEAM_FOLLOWME:
	case VCHAT_TEAM_LETGO:
	case VCHAT_TEAM_MOVE:
	case VCHAT_TEAM_CLEARPATH:
	case VCHAT_TEAM_DEFENDOBJECTIVE:
	case VCHAT_TEAM_DISARMDYNAMITE:
	case VCHAT_TEAM_CLEARMINES:
	case VCHAT_TEAM_REINFORCE_OFF:
	case VCHAT_TEAM_REINFORCE_DEF:
	case VCHAT_TEAM_AFFIRMATIVE:
	case VCHAT_TEAM_NEGATIVE:
	case VCHAT_TEAM_THANKS:
	case VCHAT_TEAM_WELCOME:
	case VCHAT_TEAM_SORRY:
	case VCHAT_TEAM_OOPS:

	// Command related
	case VCHAT_TEAM_COMMANDACKNOWLEDGED:
	case VCHAT_TEAM_COMMANDDECLINED:
	case VCHAT_TEAM_COMMANDCOMPLETED:
	case VCHAT_TEAM_DESTROYPRIMARY:
	case VCHAT_TEAM_DESTROYSECONDARY:
	case VCHAT_TEAM_DESTROYCONSTRUCTION:
	case VCHAT_TEAM_CONSTRUCTIONCOMMENCING:
	case VCHAT_TEAM_REPAIRVEHICLE:
	case VCHAT_TEAM_DESTROYVEHICLE:
	case VCHAT_TEAM_ESCORTVEHICLE:

	// Global messages
	case VCHAT_GLOBAL_AFFIRMATIVE:
	case VCHAT_GLOBAL_NEGATIVE:
	case VCHAT_GLOBAL_ENEMYWEAK:
	case VCHAT_GLOBAL_HI:
	case VCHAT_GLOBAL_BYE:
	case VCHAT_GLOBAL_GREATSHOT:
	case VCHAT_GLOBAL_CHEER:
	case VCHAT_GLOBAL_THANKS:
	case VCHAT_GLOBAL_WELCOME:
	case VCHAT_GLOBAL_OOPS:
	case VCHAT_GLOBAL_SORRY:
	case VCHAT_GLOBAL_HOLDFIRE:
	case VCHAT_GLOBAL_GOODGAME:
	}*/
	return voiceId;
}

void RTCW_Client::ProcessGotoNode( const PathInterface::PathCorner corners[ 2 ], const size_t numEdges )
{
	//if ( numEdges == 0 )
	//	return;

	//if(corners[ 0 ].mFlags & F_RTCW_NAV_SPRINT)
	//{
	//	PressButton(BOT_BUTTON_SPRINT);
	//}

	//// test for inwater / jump to move to surface
	//if(corners[ 0 ].mFlags & F_NAV_INWATER)
	//{
	//	PressButton(BOT_BUTTON_JUMP);
	//}

	//if(corners[ 0 ].mFlags & F_RTCW_NAV_STRAFE_L)
	//{
	//	PressButton(BOT_BUTTON_LSTRAFE);
	//}
	//else if(corners[ 0 ].mFlags & F_RTCW_NAV_STRAFE_R)
	//{
	//	PressButton(BOT_BUTTON_RSTRAFE);
	//}

	//if (.mStrafeJump)
	//{
	//	if(corners[ 0 ].mFlags & F_RTCW_NAV_STRAFE_JUMP_L)
	//	{
	//		if (IGame::GetFrameNumber() % 20 == 0)
	//		{
	//			GameEntity targetent;
	//			targetent = this->GetTargetingSystem()->GetCurrentTarget();
	//			if (!targetent.IsValid())
	//			{
	//				BitFlag64 b;
	//				b.SetFlag(BOT_BUTTON_LSTRAFE,true);
	//				//b.SetFlag(BOT_BUTTON_FWD,true);

	//				PressButton(BOT_BUTTON_JUMP);
	//				HoldButton(b, 750);
	//			}
	//		}
	//		PressButton(BOT_BUTTON_SPRINT);
	//	}
	//	else if(corners[ 0 ].mFlags & F_RTCW_NAV_STRAFE_JUMP_R)
	//	{
	//		if (IGame::GetFrameNumber() % 20 == 0)
	//		{
	//			GameEntity targetent;
	//			targetent = this->GetTargetingSystem()->GetCurrentTarget();
	//			if (!targetent.IsValid())
	//			{
	//				BitFlag64 b;
	//				b.SetFlag(BOT_BUTTON_LSTRAFE,true);
	//				//b.SetFlag(BOT_BUTTON_FWD,true);

	//				PressButton(BOT_BUTTON_JUMP);
	//				HoldButton(b, 750);
	//			}
	//		}
	//		PressButton(BOT_BUTTON_SPRINT);
	//	}
	//}
}

void RTCW_Client::ProcessGotoNode( const Path &_path )
{
	//Path::PathPoint pt;
	//_path.GetCurrentPt(pt);

	//if(pt..mNavFlags & F_RTCW_NAV_SPRINT)
	//{
	//	PressButton(BOT_BUTTON_SPRINT);
	//}

	//// test for inwater / jump to move to surface
	//if(pt..mNavFlags & F_NAV_INWATER)
	//{
	//	PressButton(BOT_BUTTON_JUMP);
	//}

	//if(pt..mNavFlags & F_RTCW_NAV_STRAFE_L)
	//{
	//	PressButton(BOT_BUTTON_LSTRAFE);
	//}
	//else if(pt..mNavFlags & F_RTCW_NAV_STRAFE_R)
	//{
	//	PressButton(BOT_BUTTON_RSTRAFE);
	//}

	//if (.mStrafeJump)
	//{
	//	if(pt..mNavFlags & F_RTCW_NAV_STRAFE_JUMP_L)
	//	{
	//		if (IGame::GetFrameNumber() % 20 == 0)
	//		{
	//			GameEntity targetent;
	//			targetent = this->GetTargetingSystem()->GetCurrentTarget();
	//			if (!targetent.IsValid())
	//			{
	//				BitFlag64 b;
	//				b.SetFlag(BOT_BUTTON_LSTRAFE,true);
	//				//b.SetFlag(BOT_BUTTON_FWD,true);

	//				PressButton(BOT_BUTTON_JUMP);
	//				HoldButton(b, 750);
	//			}
	//		}
	//		PressButton(BOT_BUTTON_SPRINT);
	//	}
	//	else if(pt..mNavFlags & F_RTCW_NAV_STRAFE_JUMP_R)
	//	{
	//		if (IGame::GetFrameNumber() % 20 == 0)
	//		{
	//			GameEntity targetent;
	//			targetent = this->GetTargetingSystem()->GetCurrentTarget();
	//			if (!targetent.IsValid())
	//			{
	//				BitFlag64 b;
	//				b.SetFlag(BOT_BUTTON_LSTRAFE,true);
	//				//b.SetFlag(BOT_BUTTON_FWD,true);

	//				PressButton(BOT_BUTTON_JUMP);
	//				HoldButton(b, 750);
	//			}
	//		}
	//		PressButton(BOT_BUTTON_SPRINT);
	//	}
	//}
}

float RTCW_Client::GetGameVar( GameVar _var ) const
{
	switch ( _var )
	{
		case JumpGapOffset:
			return 0.32f;
	}
	return 0.0f;
}

bool RTCW_Client::DoesBotHaveFlag( MapGoalPtr _mapgoal )
{
	return gRTCWFuncs->HasFlag( GetGameEntity() );
}

bool RTCW_Client::IsFlagGrabbable( MapGoalPtr _mapgoal )
{
	return gRTCWFuncs->ItemCanBeGrabbed( GetGameEntity(), _mapgoal->GetEntity() );
}

bool RTCW_Client::CanBotSnipe()
{
	if ( GetClass() == RTCW_CLASS_SOLDIER )
	{
		// Make sure we have a sniping weapon.
		if ( GetWeaponSystem()->HasAmmo( RTCW_WP_MAUSER ) ||
			GetWeaponSystem()->HasAmmo( RTCW_WP_SNIPERRIFLE ) )
			return true;
	}
	return false;
}

bool RTCW_Client::GetSniperWeapon( int &nonscoped, int &scoped )
{
	nonscoped = 0;
	scoped = 0;

	if ( GetClass() == RTCW_CLASS_SOLDIER )
	{
		if ( GetWeaponSystem()->HasWeapon( RTCW_WP_SNIPERRIFLE ) )
		{
			nonscoped = RTCW_WP_MAUSER;
			scoped = RTCW_WP_SNIPERRIFLE;
			return true;
		}
	}
	return false;
}

//float RTCW_Client::NavCallback(const NavFlags &_flag, Waypoint *from, Waypoint *to)
//{
//	using namespace AiState;
//	std::string gn;
//
//	if(_flag & F_RTCW_NAV_USEPATH)
//	{
//		const PropertyMap::ValueMap &pm = to->GetPropertyMap().GetProperties();
//		PropertyMap::ValueMap::const_iterator cIt = pm.begin();
//		FINDSTATE(hl,HighLevel,this->GetStateRoot());
//
//		if(hl && hl->GetActiveState())
//		{
//			gn = Utils::StringToLower(hl->GetActiveState()->GetName());
//
//			//EngineFuncs::ConsoleMessage("current goal: %s", gn.c_str());
//
//			for(; cIt != pm.end(); ++cIt)
//			{
//				//	EngineFuncs::ConsoleMessage("property: %s = %s",
//				//		(*cIt).first.c_str(), (*cIt).second.c_str());
//
//				if ( gn == (*cIt).first && (*cIt).second == "true" )
//					return 1.0f;
//			}
//		}
//	}
//
//	return 0.f;
//}

//////////////////////////////////////////////////////////////////////////

void RTCW_Client::SetupBehaviorTree()
{
	using namespace AiState;
	delete GetStateRoot()->ReplaceState( "Dead", new Limbo );
	GetStateRoot()->InsertAfter( "Limbo", new Incapacitated );

	GetStateRoot()->AppendTo( "HighLevel", new CallArtillery );
}