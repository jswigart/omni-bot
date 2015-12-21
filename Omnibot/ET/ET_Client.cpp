////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "ScriptManager.h"

#include "ET_Client.h"
#include "ET_VoiceMacros.h"
#include "ET_FilterClosest.h"
#include "ET_Messages.h"
#include "ET_BaseStates.h"
#include "ET_Game.h"

//////////////////////////////////////////////////////////////////////////
// MOVE THIS

class Limbo : public StateSimultaneous
{
public:
	float GetPriority()
	{
		return GetClient()->HasEntityFlag( ET_ENT_FLAG_INLIMBO ) ? 1.f : 0.f;
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

class Incapacitated : public StateChild
{
public:
	float GetPriority()
	{
		return !GetClient()->HasEntityFlag( ENT_FLAG_ALIVE ) ? 1.f : 0.f;
	}

	State::StateStatus Update( float fDt )
	{
		if ( gEnemyTerritoryFuncs->GetReinforceTime( GetClient()->GetGameEntity() ) < 1000 )
		{
			if ( !gEnemyTerritoryFuncs->IsMedicNear( GetClient()->GetGameEntity() ) )
			{
				gEnemyTerritoryFuncs->GoToLimbo( GetClient()->GetGameEntity() );
			}
		}
		GetClient()->SetMovementVector( Vector3f::ZERO );
		return State_Busy;
	}

	Incapacitated() : StateChild( "Incapacitated" )
	{
	}
protected:
};

//////////////////////////////////////////////////////////////////////////

ET_Client::ET_Client()
	: mBreakableTargetDistance( 0.f )
{
	mStepHeight = 8.0f; // subtract a small value as a buffer to jump
}

ET_Client::~ET_Client()
{
}

void ET_Client::Init( int _gameid )
{
	Client::Init( _gameid );

	FilterPtr filter( new ET_FilterClosest( this, AiState::SensoryMemory::EntEnemy ) );
	filter->AddCategory( ENT_CAT_SHOOTABLE );
	GetTargetingSystem()->SetDefaultTargetingFilter( filter );
}

void ET_Client::UpdateBotInput()
{
	//ETBlight and bastardmod sniper hack
	if ( mClientInput.mButtonFlags.CheckFlag( BOT_BUTTON_AIM ) )
	{
		if ( ET_Game::IsETBlight )
		{
			if ( mClientInput.mCurrentWeapon == 72 ) //MN PROTO
			{
				mClientInput.mCurrentWeapon = 75;
				mClientInput.mButtonFlags.ClearFlag( BOT_BUTTON_AIM );
			}
			if ( mClientInput.mCurrentWeapon == 79 ) //MP40SS
			{
				mClientInput.mCurrentWeapon = 80;
				mClientInput.mButtonFlags.ClearFlag( BOT_BUTTON_AIM );
			}
		}
		if ( ET_Game::IsBastardmod )
		{
			if ( mClientInput.mCurrentWeapon == 59 ) //BASTARD FG42
			{
				mClientInput.mCurrentWeapon = 60;
			}
		}
	}
	Client::UpdateBotInput();
}

void ET_Client::ProcessEvent( const Message & message, CallbackParameters & cb )
{
	switch ( message.Id() )
	{
		CASE_MSG( EvMineArmed )
		{
			BitFlag64 b;
			b.SetFlag( BOT_BUTTON_SPRINT, true );
			HoldButton( b, 3000 );
			break;
		}
		CASE_MSG( EvFireTeamCreated )
		{
			cb.CallScript();
			cb.AddEntity( "teamleader", msg->mTeamLeader );
			cb.AddInt( "fireteamnum", msg->mFireTeamNum );
			break;
		}
		CASE_MSG( EvFireTeamDisbanded )
		{
			cb.CallScript();
			cb.AddEntity( "teamleader", msg->mTeamLeader );
			cb.AddInt( "fireteamnum", msg->mFireTeamNum );
			break;
		}
		CASE_MSG( EvFireTeamJoined )
		{
			cb.CallScript();
			cb.AddEntity( "teamleader", msg->mTeamLeader );
			cb.AddInt( "fireteamnum", msg->mFireTeamNum );
			break;
		}
		CASE_MSG( EvFireTeamLeft )
		{
			cb.CallScript();
			cb.AddEntity( "teamleader", msg->mTeamLeader );
			cb.AddInt( "fireteamnum", msg->mFireTeamNum );
			break;
		}
		CASE_MSG( EvFireTeamInvited )
		{
			cb.CallScript();
			cb.AddEntity( "invitor", msg->mInviter );
			cb.AddInt( "fireteamnum", msg->mFireTeamNum );
			break;
		}
		CASE_MSG( EvFireTeamProposal )
		{
			cb.CallScript();
			cb.AddEntity( "teamleader", msg->mTeamLeader );
			cb.AddEntity( "invitee", msg->mInvitee );
			cb.AddInt( "fireteamnum", msg->mFireTeamNum );
			break;
		}
		CASE_MSG( EvFireTeamWarning )
		{
			cb.CallScript();
			cb.AddEntity( "warnedby", msg->mWarnedBy );
			cb.AddInt( "fireteamnum", msg->mFireTeamNum );
			break;
		}
	}
	Client::ProcessEvent( message, cb );
}

void ET_Client::GetNavFlags( NavFlags & includeFlags, NavFlags & excludeFlags )
{
	includeFlags = NAVFLAGS_WALK | NAVFLAGS_CROUCH;

	switch ( GetTeam() )
	{
		case ET_TEAM_AXIS:
			excludeFlags = (NavFlags)( ~NAVFLAGS_TEAM1_ONLY & NAVFLAGS_ALLTEAMS );
			break;
		case ET_TEAM_ALLIES:
			excludeFlags = (NavFlags)( ~NAVFLAGS_TEAM2_ONLY & NAVFLAGS_ALLTEAMS );
			break;
	}
}

NavFlags ET_Client::GetTeamFlag( int _team ) const
{
	static const NavFlags defaultTeam = 0;
	switch ( _team )
	{
		case ET_TEAM_AXIS:
			return NAVFLAGS_TEAM1_ONLY;
		case ET_TEAM_ALLIES:
			return NAVFLAGS_TEAM2_ONLY;
		default:
			return defaultTeam;
	}
}

void ET_Client::SendVoiceMacro( int _macroId )
{
	ET_VoiceMacros::SendVoiceMacro( this, _macroId );
}

int ET_Client::HandleVoiceMacroEvent( const EvVoiceMacro* msg )
{	
	int voiceId = ET_VoiceMacros::GetVChatId( msg->mMessage );
	switch ( voiceId )
	{
		/*case VCHAT_TEAM_PATHCLEARED:
		case VCHAT_TEAM_ENEMYWEAK:
		case VCHAT_TEAM_ALLCLEAR:
		case VCHAT_TEAM_INCOMING:*/
		case VCHAT_TEAM_FIREINTHEHOLE:
		{
			break;
		}
		/*case VCHAT_TEAM_ONDEFENSE:
		case VCHAT_TEAM_ONOFFENSE:
		case VCHAT_TEAM_TAKINGFIRE:
		case VCHAT_TEAM_MINESCLEARED:
		case VCHAT_TEAM_ENEMYDISGUISED:*/
		case VCHAT_TEAM_MEDIC:
		{
			if ( msg->mWhoSaidIt.IsValid() && ( GetClass() == ET_CLASS_MEDIC ) )
			{
				// FIXME
				/*BotBrain::EvaluatorPtr eval(new ET_Evaluator_RequestGiveHealth(this, m->mWhoSaidIt));
				if(GetBrain())
				GetBrain()->AddGoalEvaluator(eval);*/
			}
			break;
		}
		case VCHAT_TEAM_NEEDAMMO:
		{
			if ( msg->mWhoSaidIt.IsValid() && ( GetClass() == ET_CLASS_FIELDOPS ) )
			{
				// FIXME
				/*BotBrain::EvaluatorPtr eval(new ET_Evaluator_RequestGiveAmmo(this, m->mWhoSaidIt));
				if(GetBrain())
				GetBrain()->AddGoalEvaluator(eval);*/
			}
			break;
		}
		/*case VCHAT_TEAM_NEEDBACKUP:
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
		case VCHAT_TEAM_OOPS:*/

		// Command related
		/*case VCHAT_TEAM_COMMANDACKNOWLEDGED:
		case VCHAT_TEAM_COMMANDDECLINED:
		case VCHAT_TEAM_COMMANDCOMPLETED:
		case VCHAT_TEAM_DESTROYPRIMARY:
		case VCHAT_TEAM_DESTROYSECONDARY:
		case VCHAT_TEAM_DESTROYCONSTRUCTION:
		case VCHAT_TEAM_CONSTRUCTIONCOMMENCING:
		case VCHAT_TEAM_REPAIRVEHICLE:
		case VCHAT_TEAM_DESTROYVEHICLE:
		case VCHAT_TEAM_ESCORTVEHICLE:*/

		// Global messages
		/*case VCHAT_GLOBAL_AFFIRMATIVE:
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
		case VCHAT_GLOBAL_GOODGAME:*/
	}
	return voiceId;
}

void ET_Client::ProcessGotoNode( const PathInterface::PathCorner corners[ 2 ], const size_t numEdges )
{
	//if ( numEdges > 0 )
	//{
	//	if(corners[ 0 ].mFlags & F_ET_NAV_SPRINT)
	//	{
	//		PressButton(BOT_BUTTON_SPRINT);
	//	}

	//	// test for inwater / jump to move to surface
	//	if(corners[ 0 ].mFlags & F_NAV_INWATER)
	//	{
	//		PressButton(BOT_BUTTON_JUMP);
	//	}

	//	if(corners[ 0 ].mFlags & F_ET_NAV_STRAFE_L)
	//	{
	//		PressButton(BOT_BUTTON_LSTRAFE);
	//	}
	//	else if(corners[ 0 ].mFlags & F_ET_NAV_STRAFE_R)
	//	{
	//		PressButton(BOT_BUTTON_RSTRAFE);
	//	}
	//}
}

void ET_Client::ProcessGotoNode( const Path &_path )
{
	Path::PathPoint pt;
	_path.GetCurrentPt( pt );

	//if(pt.mNavFlags & F_ET_NAV_SPRINT)
	//{
	//	PressButton(BOT_BUTTON_SPRINT);
	//}

	//// test for inwater / jump to move to surface
	//if(pt.mNavFlags & F_NAV_INWATER)
	//{
	//	PressButton(BOT_BUTTON_JUMP);
	//}

	//if(pt.mNavFlags & F_ET_NAV_STRAFE_L)
	//{
	//	PressButton(BOT_BUTTON_LSTRAFE);
	//}
	//else if(pt.mNavFlags & F_ET_NAV_STRAFE_R)
	//{
	//	PressButton(BOT_BUTTON_RSTRAFE);
	//}
}

float ET_Client::GetGameVar( GameVar _var ) const
{
	switch ( _var )
	{
		case JumpGapOffset:
			return 0.32f;
	}
	return 0.0f;
}

bool ET_Client::DoesBotHaveFlag( MapGoalPtr _mapgoal )
{
	return gEnemyTerritoryFuncs->IsCarryingFlag( GetGameEntity() );
}

bool ET_Client::IsFlagGrabbable( MapGoalPtr _mapgoal )
{
	return gEnemyTerritoryFuncs->ItemCanBeGrabbed( GetGameEntity(), _mapgoal->GetEntity() );
}

bool ET_Client::CanBotSnipe()
{
	// Make sure we have a sniping weapon.
	const int SniperWeapons [] =
	{
		ET_WP_FG42_SCOPE,
		ET_WP_K43_SCOPE,
		ET_WP_GARAND_SCOPE,
	};
	for ( unsigned int i = 0; i < sizeof( SniperWeapons ) / sizeof( SniperWeapons[ 0 ] ); ++i )
	{
		WeaponPtr w = GetWeaponSystem()->GetWeapon( SniperWeapons[ i ] );
		if ( w && w->GetFireMode( Primary ).HasAmmo() )
			return true;
	}
	return false;
}

bool ET_Client::GetSniperWeapon( int &nonscoped, int &scoped )
{
	nonscoped = 0;
	scoped = 0;

	if ( GetClass() == ET_CLASS_COVERTOPS )
	{
		if ( GetWeaponSystem()->HasWeapon( ET_WP_FG42 ) )
		{
			nonscoped = ET_WP_FG42;
			scoped = ET_WP_FG42;
			return true;
		}
		if ( GetWeaponSystem()->HasWeapon( ET_WP_K43 ) )
		{
			nonscoped = ET_WP_K43;
			scoped = ET_WP_K43;
			return true;
		}
		if ( GetWeaponSystem()->HasWeapon( ET_WP_GARAND ) )
		{
			nonscoped = ET_WP_GARAND;
			scoped = ET_WP_GARAND;
			return true;
		}
	}
	return false;
}

bool ET_Client::GetSkills( gmMachine *machine, gmTableObject *tbl )
{
	ParamsSkills skills;
	if ( gEnemyTerritoryFuncs->GetSkills( GetGameEntity(), skills ) )
	{
		for ( int i = 0; i < ET_SKILLS_NUM_SKILLS; ++i )
			tbl->Set( machine, i, gmVariable( skills.mSkill[ i ] ) );
		return true;
	}
	return false;
}

//float ET_Client::NavCallback(const NavFlags &_flag, Waypoint *from, Waypoint *to)
//{
//	using namespace AiState;
//
//	if(_flag & F_ET_NAV_DISGUISE)
//	{
//		if(HasEntityFlag(ET_ENT_FLAG_DISGUISED))
//			return 1.f;
//		return 0.f;
//	}
//
//	if(_flag & F_ET_NAV_USEPATH)
//	{
//		const PropertyMap::ValueMap &pm = to->GetPropertyMap().GetProperties();
//		PropertyMap::ValueMap::const_iterator cIt = pm.begin();
//		FINDSTATE(hl,HighLevel,this->GetStateRoot());
//
//		if(hl != NULL && hl->GetActiveState())
//		{
//			std::string gn = Utils::StringToLower(hl->GetActiveState()->GetName());
//			for(; cIt != pm.end(); ++cIt)
//			{
//				if ( gn == (*cIt).first && (*cIt).second == "true" )
//					return 1.0f;
//			}
//		}
//	}
//
//	return 0.f;
//}

//////////////////////////////////////////////////////////////////////////

void ET_Client::SetupBehaviorTree()
{
	using namespace AiState;
	delete GetStateRoot()->ReplaceState( "Dead", new Limbo );
	GetStateRoot()->InsertAfter( "Limbo", new Incapacitated );

	//GetStateRoot()->AppendTo("HighLevel", new BuildConstruction);
	//GetStateRoot()->AppendTo("HighLevel", new PlantExplosive);
	//GetStateRoot()->AppendTo("HighLevel", new MountMg42);
	//GetStateRoot()->AppendTo("HighLevel", new RepairMg42);
	//GetStateRoot()->AppendTo("HighLevel", new TakeCheckPoint);
	//GetStateRoot()->AppendTo("HighLevel", new MobileMg42);
	GetStateRoot()->AppendTo( "HighLevel", new MobileMortar );
	//GetStateRoot()->AppendTo("HighLevel", new ReviveTeammate);
	//GetStateRoot()->AppendTo("HighLevel", new DefuseDynamite);
	GetStateRoot()->AppendTo( "HighLevel", new PlantMine );
	GetStateRoot()->AppendTo( "HighLevel", new CallArtillery );
	GetStateRoot()->AppendTo( "HighLevel", new UseCabinet );
	//GetStateRoot()->AppendTo("HighLevel", new Flamethrower);
	//GetStateRoot()->AppendTo("HighLevel", new Panzer);

	//FINDSTATEIF(Flamethrower,GetStateRoot(),LimitToClass().SetFlag(ET_CLASS_SOLDIER));
	//FINDSTATEIF(Panzer,GetStateRoot(),LimitToClass().SetFlag(ET_CLASS_SOLDIER));
}