////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "ScriptManager.h"

#include "JA_Messages.h"
#include "JA_Client.h"
#include "JA_VoiceMacros.h"
#include "JA_FilterClosest.h"
#include "JA_BaseStates.h"

#include "BotTargetingSystem.h"
#include "BotWeaponSystem.h"

JA_Client::JA_Client() :
mBreakableTargetDistance( 0.f )
{
	mStepHeight = 8.0f; // subtract a small value as a buffer to jump
}

JA_Client::~JA_Client()
{
}

void JA_Client::Init( int _gameid )
{
	Client::Init( _gameid );

	// We want to use a custom targeting filter.
	FilterPtr filter( new JA_FilterClosest( this, AiState::SensoryMemory::EntEnemy ) );
	filter->AddCategory( ENT_CAT_SHOOTABLE );
	GetTargetingSystem()->SetDefaultTargetingFilter( filter );
}

void JA_Client::ProcessEvent( const Message &message, CallbackParameters & cb )
{
	switch ( message.Id() )
	{
		CASE_MSG(EvFeelForce)
		{
			cb.CallScript();
			cb.AddEntity( "inflictor", msg->mInflictor );
			cb.AddInt( "power", msg->mPower );

			if ( msg->mInflictor.IsValid() )
			{
				GetSensoryMemory()->UpdateWithTouchSource( msg->mInflictor );
			}
			break;
		}
	}
	Client::ProcessEvent( message, cb );
}

NavFlags JA_Client::GetTeamFlag( int _team ) const
{
	static const NavFlags defaultTeam = 0;
	switch ( _team )
	{
	case JA_TEAM_RED:
		return NAVFLAGS_TEAM1_ONLY;
	case JA_TEAM_BLUE:
		return NAVFLAGS_TEAM2_ONLY;
	default:
		return defaultTeam;
	}
}

void JA_Client::GetNavFlags( NavFlags & includeFlags, NavFlags & excludeFlags )
{
	includeFlags = NAVFLAGS_WALK | NAVFLAGS_CROUCH;

	switch ( GetTeam() )
	{
	case JA_TEAM_RED:
		excludeFlags = (NavFlags)(~NAVFLAGS_TEAM1_ONLY & NAVFLAGS_ALLTEAMS);
		break;
	case JA_TEAM_BLUE:
		excludeFlags = (NavFlags)(~NAVFLAGS_TEAM2_ONLY & NAVFLAGS_ALLTEAMS);
		break;
	}
}

void JA_Client::SendVoiceMacro( int _macroId )
{
	JA_VoiceMacros::SendVoiceMacro( this, _macroId );
}

int JA_Client::HandleVoiceMacroEvent( const EvVoiceMacro* msg )
{
	int voiceId = JA_VoiceMacros::GetVChatId( msg->mMessage );
	/*switch(iVoiceId)
	{
	}*/
	return voiceId;
}

void JA_Client::ProcessGotoNode( const Path &_path )
{
	/*Path::PathPoint pt;
	_path.GetCurrentPt(pt);

	if(pt..mNavFlags & F_JA_NAV_BUTTON1)
	{
	PressButton(BOT_BUTTON_USE);
	}*/
}

float JA_Client::GetGameVar( GameVar _var ) const
{
	switch ( _var )
	{
	case JumpGapOffset:
		return 0.32f;
	}
	return 0.0f;
}

bool JA_Client::DoesBotHaveFlag( MapGoalPtr _mapgoal )
{
	return gJediAcademyFuncs->HasFlag( GetGameEntity() );
}

bool JA_Client::CanBotSnipe()
{
	// Make sure we have the disuptor and ammo.
	if ( GetWeaponSystem()->HasAmmo( JA_WP_DISRUPTOR ) )
		return true;
	return false;
}

bool JA_Client::GetSniperWeapon( int &nonscoped, int &scoped )
{
	nonscoped = 0;
	scoped = 0;

	if ( GetWeaponSystem()->HasWeapon( JA_WP_DISRUPTOR ) )
	{
		nonscoped = JA_WP_DISRUPTOR;
		scoped = JA_WP_DISRUPTOR;
		return true;
	}
	return false;
}

//float JA_Client::NavCallback(const NavFlags &_flag, Waypoint *from, Waypoint *to)
//{
//	using namespace AiState;
//
//	if(_flag & F_JA_NAV_FORCEJUMP)
//	{
//		int iForceJumpLevel = gEngineFuncs->GetForceLevel(this, JA_FP_LEVITATION);
//		float fMaxForceJumpheight = 0;
//		int fMinForceNeeded = 0;
//		if(iForceJumpLevel == 3)
//		{
//			fMaxForceJumpheight = 384.f;
//			fMinForceNeeded = 30;
//		}
//		else if(iForceJumpLevel == 2)
//		{
//			fMaxForceJumpheight = 226.f;
//			fMinForceNeeded = 30;
//		}
//		else if(iForceJumpLevel == 1)
//		{
//			fMaxForceJumpheight = 130.f;
//			fMinForceNeeded = 30;
//		}
//		else
//			return 0.f;
//
//		int iForce = gEngineFuncs->GetForce(this);
//		if(iForce < fMinForceNeeded)
//			return false;
//
//		const float fHeightDiff = to->GetPosition().Z() - from->GetPosition().Z();
//		if(fHeightDiff < fMaxForceJumpheight)
//			return 1.f;
//	}
//	return 0.f;
//}

//////////////////////////////////////////////////////////////////////////

void JA_Client::SetupBehaviorTree()
{
	using namespace AiState;

	GetStateRoot()->AppendTo( "MotorControl", new ForceJump );
}