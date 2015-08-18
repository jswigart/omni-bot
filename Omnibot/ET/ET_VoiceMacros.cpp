////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "ET_VoiceMacros.h"
#include "ET_Client.h"

#include <type_traits>

const char *strVoiceMacros[] =
{
	0,

	// Execute these using VoiceTeamChat
	"PathCleared",
	"EnemyWeak",
	"AllClear",
	"Incoming",
	"FireInTheHole",
	"OnDefense",
	"OnOffense",
	"TakingFire",
	"MinesCleared",
	"EnemyDisguised",

	"Medic",
	"NeedAmmo",
	"NeedBackup",
	"NeedEngineer",
	"CoverMe",
	"HoldFire",
	"WhereTo",
	"NeedOps",

	"FollowMe",
	"LetsGo",
	"Move",
	"ClearPath",
	"DefendObjective",
	"DisarmDynamite",
	"ClearMines",
	"ReinforceOffense",
	"ReinforceDefense",

	"Affirmative",
	"Negative",
	"Thanks",
	"Welcome",
	"Sorry",
	"Oops",

	"CommandAcknowledged",
	"CommandDeclined",
	"CommandCompleted",
	"DestroyPrimary",
	"DestroySecondary",
	"DestroyConstruction",
	"ConstructionCommencing",
	"RepairVehicle",
	"DestroyVehicle",
	"EscortVehicle",

	"IamSoldier",
	"IamMedic",
	"IamEngineer",
	"IamFieldOps",
	"IamCovertOps",

	0,

	// Execute with VoiceChat
	"Affirmative",
	"Negative",
	"EnemyWeak",
	"Hi",
	"Bye",
	"GreatShot",
	"Cheer",
	"Thanks",
	"Welcome",
	"Oops",
	"Sorry",
	"HoldFire",
	"GoodGame",

	0,

	"FTMortar",
	"FTHealSquad",
	"FTHealMe",
	"FTReviveTeamMate",
	"FTReviveMe",
	"FTDestroyObjective",
	"FTRepairObjective",
	"FTConstructObjective",
	"FTDeployLandmines",
	"FTDisarmLandmines",
	"FTCallAirStrike",
	"FTCallArtillery",
	"FTResupplySquad",
	"FTResupplyMe",
	"FTExploreArea",
	"FTCheckLandmines",
	"FTSatchelObjective",
	"FTInfiltrate",
	"FTGoUndercover",
	"FTProvideSniperCover",
	"FTAttack",
	"FTFallBack",

	0
	//"w.msayPlayerClass",
};

namespace ET_VoiceChatAssertions
{
	// Make sure our enum size at least matches the size of our array.
	static_assert( ( sizeof( strVoiceMacros ) / sizeof( strVoiceMacros[ 0 ] ) ) == NUM_ET_VCHATS, "Mismatched Voice Enumerations" );
}

int ET_VoiceMacros::GetVChatId(const char *_string)
{
	// Search for the string and return the index.
	for(int i = VCHAT_NONE; i < NUM_ET_VCHATS; ++i)
	{
		if(strVoiceMacros[i] && strcmp(strVoiceMacros[i], _string)==0)
			return i;
	}
	return VCHAT_NONE;
}

void ET_VoiceMacros::SendVoiceMacro(Client *_bot, int _msg)
{
	static char buffer[512];
	if((_msg < VCHAT_TEAM_NUMMESSAGES) && (_msg > VCHAT_NONE))
	{
		sprintf(buffer, "vsay_team %s", strVoiceMacros[_msg]);
		gEngineFuncs->BotCommand(_bot->GetGameID(), buffer);
	}
	else if((_msg < VCHAT_GLOBAL_NUMMESSAGES) && (_msg > VCHAT_TEAM_NUMMESSAGES))
	{
		sprintf(buffer, "vsay %s", strVoiceMacros[_msg]);
		gEngineFuncs->BotCommand(_bot->GetGameID(), buffer);
	}
}

//////////////////////////////////////////////////////////////////////////
//QM_MENU_START( "w.mquickstatements" )
//QM_MENU_ITEM_TEAM( "P. Path Cleared.", 	exec "VoiceTeamChat PathCleared"; 		close w.mquickstatements,	"p", 0 )
//QM_MENU_ITEM_TEAM( "W. Enemy Weak!", 		exec "VoiceTeamChat EnemyWeak"; 		close w.mquickstatements, 	"w", 1 )
//QM_MENU_ITEM_TEAM( "C. All Clear", 		exec "VoiceTeamChat AllClear"; 			close w.mquickstatements,	"c", 2 )
//QM_MENU_ITEM_TEAM( "I. Incoming", 		exec "VoiceTeamChat Incoming"; 			close w.mquickstatements,	"i", 3 )
//QM_MENU_ITEM_TEAM( "F. Fire In The Hole!",exec "VoiceTeamChat FireInTheHole";		 close w.mquickstatements,	"f", 4 )
//QM_MENU_ITEM_TEAM( "D. I'm Defending.", 	exec "VoiceTeamChat OnDefense"; 		close w.mquickstatements,	"d", 5 )
//QM_MENU_ITEM_TEAM( "A. I'm Attacking.", 	exec "VoiceTeamChat OnOffense"; 		close w.mquickstatements,	"a", 6 )
//QM_MENU_ITEM_TEAM( "T. Taking Fire!", 	exec "VoiceTeamChat TakingFire"; 		close w.mquickstatements,	"t", 7 )
//QM_MENU_ITEM_TEAM( "M. Mines Cleared", 	exec "VoiceTeamChat MinesCleared"; 		close w.mquickstatements,	"m", 8 )
//QM_MENU_ITEM_TEAM( "E. Enemy Disguised", 	exec "VoiceTeamChat EnemyDisguised";	close w.mquickstatements,	"e", 9 )
//QM_MENU_END
//QM_MENU_START( "w.mquickrequests" )
//QM_MENU_ITEM_TEAM( "M. Need Medic!", 		exec "VoiceTeamChat Medic"; 		close w.mquickrequests,	"m", 0 )
//QM_MENU_ITEM_TEAM( "A. Need Ammo!", 		exec "VoiceTeamChat NeedAmmo"; 		close w.mquickrequests,	"a", 1 )
//QM_MENU_ITEM_TEAM( "B. Need Backup!", 	exec "VoiceTeamChat NeedBackup"; 	close w.mquickrequests,	"b", 2 )
//QM_MENU_ITEM_TEAM( "E. Need Engineer!", 	exec "VoiceTeamChat NeedEngineer"; 	close w.mquickrequests,	"e", 3 )
//QM_MENU_ITEM_TEAM( "C. Cover Me!", 		exec "VoiceTeamChat CoverMe"; 		close w.mquickrequests,	"c", 4 )
//QM_MENU_ITEM_TEAM( "H. Hold Fire!",		exec "VoiceTeamChat HoldFire";	 	close w.mquickrequests, "h", 5 )
//QM_MENU_ITEM_TEAM( "W. Where To?", 		exec "VoiceTeamChat WhereTo"; 		close w.mquickrequests,	"w", 6 )
//QM_MENU_ITEM_TEAM( "O. Need Covert Ops!", exec "VoiceTeamChat NeedOps"; 		close w.mquickrequests,	"o", 7 )
//QM_MENU_END
//QM_MENU_START( "w.mquickcommand" )
//QM_MENU_ITEM_TEAM( "F. Follow Me!", 		exec "VoiceTeamChat FollowMe"; 			close w.mquickcommand, 	"f", 0 )
//QM_MENU_ITEM_TEAM( "G. Let's Go!", 		exec "VoiceTeamChat LetsGo"; 			close w.mquickcommand, 	"g", 1 )
//QM_MENU_ITEM_TEAM( "M. Move!", 			exec "VoiceTeamChat Move"; 				close w.mquickcommand, 	"m", 2 )
//QM_MENU_ITEM_TEAM( "C. Clear The Path!", 	exec "VoiceTeamChat ClearPath"; 		close w.mquickcommand, 	"c", 3 )
//QM_MENU_ITEM_TEAM( "O. Defend Objective!",exec "VoiceTeamChat DefendObjective"; 	close w.mquickcommand, 	"o", 4 )
//QM_MENU_ITEM_TEAM( "D. Disarm Dynamite!", exec "VoiceTeamChat DisarmDynamite"; 	close w.mquickcommand, 	"d", 5 )
//QM_MENU_ITEM_TEAM( "N. Clear Mines!", 	exec "VoiceTeamChat ClearMines"; 		close w.mquickcommand, 	"n", 6 )
//QM_MENU_ITEM_TEAM( "R. Reinforce Offense",exec "VoiceTeamChat ReinforceOffense"; 	close w.mquickcommand,	"r", 7 )
//QM_MENU_ITEM_TEAM( "E. Reinforce Defense",exec "VoiceTeamChat ReinforceDefense"; 	close w.mquickcommand,	"e", 8 )
//QM_MENU_END
//QM_MENU_START( "w.mquickmisc" )
//QM_MENU_ITEM_TEAM( "Y. Yes",			exec "VoiceTeamChat Affirmative"; 	close w.mquickmisc, "y", 0 )
//QM_MENU_ITEM_TEAM( "N. No",				exec "VoiceTeamChat Negative"; 		close w.mquickmisc, "n", 1 )
//QM_MENU_ITEM_TEAM( "T. Thanks",			exec "VoiceTeamChat Thanks"; 		close w.mquickmisc, "t", 2 )
//QM_MENU_ITEM_TEAM( "W. Welcome",		exec "VoiceTeamChat Welcome"; 		close w.mquickmisc, "w", 3 )
//QM_MENU_ITEM_TEAM( "S. Sorry",			exec "VoiceTeamChat Sorry"; 		close w.mquickmisc, "s", 4 )
//QM_MENU_ITEM_TEAM( "O. Oops", 			exec "VoiceTeamChat Oops"; 			close w.mquickmisc, "o", 5 )
//QM_MENU_END
//QM_MENU_START( "w.mquickglobal" )
//QM_MENU_ITEM( "Y. Yes",				exec "VoiceChat Affirmative"; 	close w.mquickglobal, 	"y", 0 )
//QM_MENU_ITEM( "N. No",				exec "VoiceChat Negative"; 		close w.mquickglobal, 	"n", 1 )
//QM_MENU_ITEM( "W. Enemy Weak",		exec "VoiceChat EnemyWeak";		close w.mquickglobal, 	"w", 2 )
//QM_MENU_ITEM( "H. Hi",				exec "VoiceChat Hi"; 			close w.mquickglobal, 	"h", 3 )
//QM_MENU_ITEM( "B. Bye",				exec "VoiceChat Bye"; 			close w.mquickglobal, 	"b", 4 )
//QM_MENU_ITEM( "S. Great Shot",		exec "VoiceChat GreatShot"; 	close w.mquickglobal, 	"s", 5 )
//QM_MENU_ITEM( "C. Cheer",				exec "VoiceChat Cheer"; 		close w.mquickglobal, 	"c", 6 )
//QM_MENU_ITEM( "G. More Globals",		close w.mquickglobal;			open w.mquickglobal2, 	"g", 7 )
//QM_MENU_END
//QM_MENU_START( "w.mquickglobal2" )
//QM_MENU_ITEM( "T. Thanks",		exec "VoiceChat Thanks";	close w.mquickglobal2, 	"t", 0 )
//QM_MENU_ITEM( "W. Welcome",		exec "VoiceChat Welcome"; 	close w.mquickglobal2, 	"w", 1 )
//QM_MENU_ITEM( "O. Oops",		exec "VoiceChat Oops"; 		close w.mquickglobal2, 	"o", 2 )
//QM_MENU_ITEM( "S. Sorry",		exec "VoiceChat Sorry"; 	close w.mquickglobal2, 	"s", 3 )
//QM_MENU_ITEM( "H. Hold Fire!",	exec "VoiceChat HoldFire";	close w.mquickglobal2, 	"h", 4 )
//QM_MENU_ITEM( "G. Good Game",	exec "VoiceChat GoodGame";	close w.mquickglobal2, 	"g", 5 )
//QM_MENU_END
//QM_MENU_START( "w.mquickobjectives" )
//QM_MENU_ITEM_TEAM( "A. Command Acknowledged",		exec "VoiceTeamChat CommandAcknowledged"; 		close w.mquickobjectives,	"a", 0 )
//QM_MENU_ITEM_TEAM( "D. Command Declined",			exec "VoiceTeamChat CommandDeclined";			close w.mquickobjectives,	"d", 1 )
//QM_MENU_ITEM_TEAM( "C. Command Completed",			exec "VoiceTeamChat CommandCompleted";			close w.mquickobjectives,	"c", 2 )
//QM_MENU_ITEM_TEAM( "P. Destroy Primary Objective",	exec "VoiceTeamChat DestroyPrimary"; 			close w.mquickobjectives,	"p", 3 )
//QM_MENU_ITEM_TEAM( "S. Destroy Secondary Objective",	exec "VoiceTeamChat DestroySecondary";		close w.mquickobjectives,	"s", 4 )
//QM_MENU_ITEM_TEAM( "X. Destroy Construction",		exec "VoiceTeamChat DestroyConstruction";		close w.mquickobjectives,	"x", 5 )
//QM_MENU_ITEM_TEAM( "M. Commencing Construction",	exec "VoiceTeamChat ConstructionCommencing";	close w.mquickobjectives, 	"m", 6 )
//QM_MENU_ITEM_TEAM( "R. Repair Vehicle",				exec "VoiceTeamChat RepairVehicle";				close w.mquickobjectives,	"r", 7 )
//QM_MENU_ITEM_TEAM( "V. Disable Vehicle",			exec "VoiceTeamChat DestroyVehicle";			close w.mquickobjectives,	"v", 8 )
//QM_MENU_ITEM_TEAM( "E. Escort Vehicle",				exec "VoiceTeamChat EscortVehicle";				close w.mquickobjectives,	"e", 9 )
//QM_MENU_END

//////////////////////////////////////////////////////////////////////////

//QM_MENU_START( "w.mnq_alt3" )
//QM_MENU_ITEM_TEAM( "1. Deploy Mortar",		exec "VoiceFireteamChat FTMortar";		close w.mnq_alt3, "1", 0)
//QM_MENU_ITEM_TEAM( "2. Heal The Squad",		exec "VoiceFireteamChat FTHealSquad";		close w.mnq_alt3, "2", 1)
//QM_MENU_ITEM_TEAM( "3. Heal Me",			exec "VoiceFireteamChat FTHealMe";		close w.mnq_alt3, "3", 2)
//QM_MENU_ITEM_TEAM( "4. Revive Team Mate",	exec "VoiceFireteamChat FTReviveTeamMate";	close w.mnq_alt3, "4", 3)
//QM_MENU_ITEM_TEAM( "5. Revive Me",			exec "VoiceFireteamChat FTReviveMe";		close w.mnq_alt3, "5", 4)
//QM_MENU_ITEM_TEAM( "6. Destroy The Objective",	exec "VoiceFireteamChat FTDestroyObjective";	close w.mnq_alt3, "6", 5)
//QM_MENU_ITEM_TEAM( "7. Repair Objective",	exec "VoiceFireteamChat FTRepairObjective";	close w.mnq_alt3, "7", 6)
//QM_MENU_ITEM_TEAM( "8. Construct Objective",exec "VoiceFireteamChat FTConstructObjective";	close w.mnq_alt3, "8", 7)
//QM_MENU_ITEM_TEAM( "9. Deploy Landmines",	exec "VoiceFireteamChat FTDeployLandmines";	close w.mnq_alt3, "9", 8)
//QM_MENU_ITEM_TEAM( "0. Disarm Landmines",	exec "VoiceFireteamChat FTDisarmLandmines";	close w.mnq_alt3, "0", 9)
//QM_MENU_END
//
//QM_MENU_START( "w.mnq_alt4" )
//QM_MENU_ITEM_TEAM( "1. Call an Airstrike",		exec "VoiceFireteamChat FTCallAirStrike";	close w.mnq_alt4, "1", 0)
//QM_MENU_ITEM_TEAM( "2. Call in Artillery",		exec "VoiceFireteamChat FTCallArtillery";	close w.mnq_alt4, "2", 1)
//QM_MENU_ITEM_TEAM( "3. Resupply The Squad",		exec "VoiceFireteamChat FTResupplySquad";	close w.mnq_alt4, "3", 2)
//QM_MENU_ITEM_TEAM( "4. Resupply Me",			exec "VoiceFireteamChat FTResupplyMe";		close w.mnq_alt4, "4", 3)
//QM_MENU_ITEM_TEAM( "5. Explore The Area",		exec "VoiceFireteamChat FTExploreArea";		close w.mnq_alt4, "5", 4)
//QM_MENU_ITEM_TEAM( "6. Check for Landmines",		exec "VoiceFireteamChat FTCheckLandmines";	close w.mnq_alt4, "6", 5)
//QM_MENU_ITEM_TEAM( "7. Destroy The Objective",		exec "VoiceFireteamChat FTSatchelObjective";	close w.mnq_alt4, "7", 6)
//QM_MENU_ITEM_TEAM( "8. Infiltrate",			exec "VoiceFireteamChat FTInfiltrate";		close w.mnq_alt4, "8", 7)
//QM_MENU_ITEM_TEAM( "9. Go Undercover",			exec "VoiceFireteamChat FTGoUndercover";	close w.mnq_alt4, "9", 8)
//QM_MENU_ITEM_TEAM( "0. Provide Sniper Cover",		exec "VoiceFireteamChat FTProvideSniperCover";	close w.mnq_alt4, "0", 9

//////////////////////////////////////////////////////////////////////////