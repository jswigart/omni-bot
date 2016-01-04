////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "gmConfig.h"
#include "gmBot.h"
#include "gmThread.h"
#include "gmMachine.h"
#include "gmGameEntity.h"
#include "gmBotLibrary.h"

#include "TF_Game.h"
#include "TF_Client.h"
#include "TF_BaseStates.h"

#define CHECK_THIS_BOT() \
	Client *native = gmBot::GetThisObject( a_thread ); \
	if(!native) \
		{ \
	GM_EXCEPTION_MSG("Script Function on NULL object"); \
	return GM_EXCEPTION; \
		}

// Title: TF Script Bindings

//////////////////////////////////////////////////////////////////////////

// Function: GetSentry
//		Does the bot currently have a sentry?
//
// Parameters:
//
//		none
//
// Returns:
//		<GameEntity> - Sentry
//		- OR -
//		null
static int GM_CDECL gmfGetSentry( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 0 );
	using namespace AiState;

	FINDSTATE( sg, Sentry, native->GetStateRoot() );
	if ( sg && sg->SentryFullyBuilt() && sg->GetSentryStatus().mEntity.IsValid() )
		a_thread->PushEntity( sg->GetSentryStatus().mEntity.AsInt() );
	else
		a_thread->PushNull();
	return GM_OK;
}
static int GM_CDECL gmfGetSentryLevel( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 0 );
	using namespace AiState;

	FINDSTATE( sg, Sentry, native->GetStateRoot() );
	if ( sg && sg->SentryFullyBuilt() && sg->GetSentryStatus().mEntity.IsValid() )
		a_thread->PushInt( sg->GetSentryStatus().mLevel );
	else
		a_thread->PushInt( 0 );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// Function: GetDispenser
//		Does the bot currently have a dispenser?
//
// Parameters:
//
//		none
//
// Returns:
//		<GameEntity> - Dispenser
//		- OR -
//		null
static int GM_CDECL gmfGetDispenser( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 0 );
	using namespace AiState;

	FINDSTATE( disp, Dispenser, native->GetStateRoot() );
	if ( disp && disp->DispenserFullyBuilt() && disp->GetDispenserStatus().mEntity.IsValid() )
		a_thread->PushEntity( disp->GetDispenserStatus().mEntity.AsInt() );
	else
		a_thread->PushNull();
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// Function: GetDetpack
//		Does the bot currently have a detpack?
//
// Parameters:
//
//		none
//
// Returns:
//		<GameEntity> - Detpack
//		- OR -
//		null
static int GM_CDECL gmfGetDetpack( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 0 );
	using namespace AiState;

	FINDSTATE( detp, Detpack, native->GetStateRoot() );
	if ( detp && detp->DetpackFullyBuilt() && detp->GetDetpackStatus().mEntity.IsValid() )
		a_thread->PushEntity( detp->GetDetpackStatus().mEntity.AsInt() );
	else
		a_thread->PushNull();
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// Function: GetPipeCount
//		Get the current number of pipes out for this bot
//
// Parameters:
//
//		none
//
// Returns:
//		int - number of pipes deployed by this bot
static int GM_CDECL gmfGetPipeCount( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 0 );
	
	a_thread->PushInt( gTeamFortressFuncs->GetPlayerPipeCount( native->GetGameEntity() ) );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// Function: EnableDisguise
//		Enables or disables the bots automatic disguising.
//
// Parameters:
//
//		int - true to enable, false to disable
//
// Returns:
//		none
static int GM_CDECL gmfEnableDisguise( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 1 );
	GM_CHECK_INT_PARAM( enabled, 0 );

	native->SetUserFlag( TF_Client::TF_FL_DISGUISEDISABLED, !enabled );
	return GM_OK;
}
//////////////////////////////////////////////////////////////////////////

// Function: DisguiseOptions
//		Gets the available disguise team and class.
//
// Parameters:
//
//		none
//
// Returns:
//		table - contains team and class table with valid team and classes
static int GM_CDECL gmfDisguiseOptions( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 0 );

	gmMachine *pMachine = a_thread->GetMachine();
	DisableGCInScope gcEn( pMachine );

	ParamsDisguiseOptions_TF disguiseOptions = {};
	disguiseOptions.mCheckTeam = native->GetTeam();
	gTeamFortressFuncs->DisguiseOptions( native->GetGameEntity(), disguiseOptions );

	gmTableObject *pTbl = pMachine->AllocTableObject();
	gmTableObject *pTeamTbl = pMachine->AllocTableObject();
	gmTableObject *pClassTbl = pMachine->AllocTableObject();

	for ( int t = 0; t <= 4; ++t )
		pTeamTbl->Set( pMachine, t, gmVariable( disguiseOptions.mTeam[ t ] ? 1 : 0 ) );

	for ( int c = TF_CLASS_SCOUT; c <= TF_CLASS_CIVILIAN; ++c )
		pTeamTbl->Set( pMachine, c, gmVariable( disguiseOptions.mClass[ c ] ? 1 : 0 ) );

	pTbl->Set( pMachine, "team", gmVariable( pTeamTbl ) );
	pTbl->Set( pMachine, "class", gmVariable( pClassTbl ) );
	a_thread->PushTable( pTbl );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// Function: Disguise
//		Tell the bot to disguise.
//
// Parameters:
//
//		int - team
//		int - class
//
// Returns:
//		none
static int GM_CDECL gmfDisguise( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 2 );
	GM_CHECK_INT_PARAM( iTeam, 0 );
	GM_CHECK_INT_PARAM( iClass, 1 );

	gTeamFortressFuncs->Disguise( native->GetGameEntity(), iTeam, iClass );
	return GM_OK;
}

// Function: Feign
//		Tell the bot to cloak.
//
// Parameters:
//
//		int - OPTIONAL - true to silent cloak
//
// Returns:
//		none
static int GM_CDECL gmfCloak( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_INT_PARAM( silent, 0, 0 );

	gTeamFortressFuncs->Cloak( native->GetGameEntity(), silent != 0 ? 1 : 0 );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// Function: LockPlayerPosition
//		Freezes the player in place.
//
// Parameters:
//
//		<GameEntity> - The entity to freeze in place
//		int - true to freeze, false to unfreeze
//
// Returns:
//		none
static int GM_CDECL gmfLockPlayerPosition( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 2 );
	GM_CHECK_INT_PARAM( lock, 1 );

	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 0 );

	bool bSucceed = false;
	if ( gameEnt.IsValid() )
	{
		bSucceed = gTeamFortressFuncs->LockPlayerPosition( gameEnt, lock != 0 ? 1 : 0 );
	}
	a_thread->PushInt( bSucceed ? 1 : 0 );
	return GM_OK;
}

// Function: HudHint
//		Display a hud hint to a selected player.
//
// Parameters:
//
//		<GameEntity> - The entity to send the hint to
//		- OR -
//		<int> - The gameId to send the hint to
//		int - Message id
//		std::string - Text to display in the hud hint
//
// Returns:
//		none
static int GM_CDECL gmfHudHint( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 3 );
	GM_CHECK_INT_PARAM( id, 1 );
	GM_CHECK_STRING_PARAM( message, 2 );

	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 0 );

	gTeamFortressFuncs->ShowHudHint( gameEnt, id, message );
	return GM_OK;
}

// Function: HudTextMsg
//		Display a hud text message to a selected player.
//
// Parameters:
//
//		<GameEntity> - The entity to send the menu to
//		- OR -
//		<int> - The gameId to send the menu to
//		std::string - Text to display in the message
//
// Returns:
//		none
static int GM_CDECL gmfHudTextMsg( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 2 );

	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 0 );
	GM_CHECK_STRING_PARAM( message, 1 );

	ParamsHudText hudtxt;
	memset( &hudtxt, 0, sizeof( hudtxt ) );
	hudtxt.mMessageType = ParamsHudText::MsgHudCenter;
	hudtxt.mTargetPlayer = gameEnt;
	Utils::StringCopy( hudtxt.mMessage, message, sizeof( hudtxt.mMessage ) );

	gTeamFortressFuncs->ShowHudText( hudtxt );
	return GM_OK;
}

// Function: HudAlert
//		Display a hud alert to a selected player.
//
// Parameters:
//
//		<GameEntity> - The entity to send the menu to
//		- OR -
//		<int> - The gameId to send the menu to
//		std::string - Text to display in the title bar
//		int - level of the message
//		float - timeout
//		color - the color of the message
//
// Returns:
//		none
static int GM_CDECL gmfHudAlert( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 5 );

	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 0 );

	GM_CHECK_STRING_PARAM( title, 1 );
	GM_CHECK_INT_PARAM( level, 2 );
	GM_CHECK_FLOAT_OR_INT_PARAM( timeout, 3 );
	GM_CHECK_INT_PARAM( color, 4 );

	ParamsHudMenu menuData;
	memset( &menuData, 0, sizeof( menuData ) );

	menuData.mMenuType = ParamsHudMenu::GuiAlert;
	menuData.mTargetPlayer = gameEnt;
	Utils::StringCopy( menuData.mTitle, title, sizeof( menuData.mTitle ) );
	menuData.mLevel = level;
	menuData.mTimeOut = timeout;
	menuData.mColor = obColor( color );

	gTeamFortressFuncs->ShowHudMenu( menuData );
	return GM_OK;
}

// Function: HudMessage
//		Display a hud message to a selected player.
//
// Parameters:
//
//		<GameEntity> - The entity to send the menu to
//		- OR -
//		<int> - The gameId to send the menu to
//		std::string - Text to display in the text box
//		int - level of the message
//		float - timeout
//		color - the color of the message
//
// Returns:
//		none
static int GM_CDECL gmfHudTextBox( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 6 );

	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 0 );

	GM_CHECK_STRING_PARAM( title, 1 );
	GM_CHECK_STRING_PARAM( body, 2 );
	GM_CHECK_INT_PARAM( level, 3 );
	GM_CHECK_FLOAT_OR_INT_PARAM( timeout, 4 );
	GM_CHECK_INT_PARAM( color, 5 );

	ParamsHudMenu menuData;
	memset( &menuData, 0, sizeof( menuData ) );

	menuData.mMenuType = ParamsHudMenu::GuiTextBox;
	menuData.mTargetPlayer = gameEnt;
	Utils::StringCopy( menuData.mTitle, title, sizeof( menuData.mTitle ) );
	Utils::StringCopy( menuData.mMessage, body, sizeof( menuData.mMessage ) );
	menuData.mLevel = level;
	menuData.mTimeOut = timeout;
	menuData.mColor = obColor( color );

	gTeamFortressFuncs->ShowHudMenu( menuData );
	return GM_OK;
}
// Function: HudMenu
//		Display a hud menu to a selected player.
//
// Parameters:
//
//		<GameEntity> - The entity to send the menu to
//		- OR -
//		<int> - The gameId to send the menu to
//		std::string - Text to display in the title bar
//		std::string - Text to display in caption area
//		std::string - Text to display in menu body
//		int - level of the message
//		float - timeout
//		color - the color of the message
//		table - table of options & commands
//
// Returns:
//		none
static int GM_CDECL gmfHudMenu( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 8 );

	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 0 );

	GM_CHECK_STRING_PARAM( title, 1 );
	GM_CHECK_STRING_PARAM( caption, 2 );
	GM_CHECK_STRING_PARAM( body, 3 );
	GM_CHECK_INT_PARAM( level, 4 );
	GM_CHECK_FLOAT_OR_INT_PARAM( timeout, 5 );
	GM_CHECK_INT_PARAM( color, 6 );
	GM_CHECK_TABLE_PARAM( menuOptions, 7 );

	ParamsHudMenu menuData;
	memset( &menuData, 0, sizeof( menuData ) );

	menuData.mMenuType = ParamsHudMenu::GuiMenu;
	menuData.mTargetPlayer = gameEnt;
	Utils::StringCopy( menuData.mTitle, title, sizeof( menuData.mTitle ) );
	Utils::StringCopy( menuData.mCaption, caption, sizeof( menuData.mCaption ) );
	Utils::StringCopy( menuData.mMessage, body, sizeof( menuData.mMessage ) );
	menuData.mLevel = level;
	menuData.mTimeOut = timeout;
	menuData.mColor = obColor( color );

	int iOption = 0;
	for ( int i = 0; i < 10; ++i )
	{
		gmVariable val = menuOptions->Get( gmVariable( i ) );
		gmTableObject *pTbl = val.GetTableObjectSafe();
		if ( pTbl )
		{
			gmTableIterator tIt;
			gmTableNode *pNode = pTbl->GetFirst( tIt );
			if ( pNode )
			{
				gmStringObject *pOptionTxt = pNode->m_key.GetStringObjectSafe();
				gmStringObject *pCommandTxt = pNode->m_value.GetStringObjectSafe();

				if ( pOptionTxt && pCommandTxt )
				{
					Utils::StringCopy( menuData.mOption[ iOption ], pOptionTxt->GetString(), sizeof( menuData.mOption[ iOption ] ) );
					Utils::StringCopy( menuData.mCommand[ iOption ], pCommandTxt->GetString(), sizeof( menuData.mCommand[ iOption ] ) );
					++iOption;
				}
			}
		}
	}

	gTeamFortressFuncs->ShowHudMenu( menuData );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////
// package: Team Fortress Bot Library Functions
static gmFunctionEntry s_TFbotLib [] =
{
	{ "LockPlayerPosition", gmfLockPlayerPosition },
	{ "HudHint", gmfHudHint },
	{ "HudMessage", gmfHudTextBox },
	{ "HudAlert", gmfHudAlert },
	{ "HudMenu", gmfHudMenu },
	{ "HudTextMsg", gmfHudTextMsg },
};

//////////////////////////////////////////////////////////////////////////
// package: Team Fortress Bot Script Functions
static gmFunctionEntry s_TFbotTypeLib [] =
{
	{ "GetSentry", gmfGetSentry },
	{ "GetSentryLevel", gmfGetSentryLevel },
	{ "GetDispenser", gmfGetDispenser },
	{ "GetDetpack", gmfGetDetpack },
	{ "GetPipeCount", gmfGetPipeCount },
	{ "EnableDisguise", gmfEnableDisguise },
	{ "Disguise", gmfDisguise },
	{ "DisguiseOptions", gmfDisguiseOptions },
	{ "Cloak", gmfCloak },
};

void TF_Game::InitScriptBinds( gmMachine *_machine )
{
	// Register the bot functions.
	_machine->RegisterLibrary( s_TFbotLib, sizeof( s_TFbotLib ) / sizeof( s_TFbotLib[ 0 ] ) );
	//////////////////////////////////////////////////////////////////////////
	_machine->RegisterTypeLibrary( gmBot::GetType(), s_TFbotTypeLib, sizeof( s_TFbotTypeLib ) / sizeof( s_TFbotTypeLib[ 0 ] ) );
}