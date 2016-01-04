////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "gmConfig.h"
#include "gmThread.h"
#include "gmMachine.h"
#include "gmBot.h"
#include "gmBotLibrary.h"

#include "Q4_Config.h"
#include "Q4_Game.h"
#include "Q4_Client.h"

// Title: Q4 Script Bindings

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

static int GM_CDECL gmfIsBuyingAllowed( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 0 );
	a_thread->PushInt( gQuake4Funcs->IsBuyingAllowed() ? 1 : 0 );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

static int GM_CDECL gmfGetCash( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 0 );
	Client *native = gmBot::GetThisObject( a_thread );
	if ( !native )
	{
		GM_EXCEPTION_MSG( "Script Function on NULL object" );
		return GM_EXCEPTION;
	}

	a_thread->PushFloat( gQuake4Funcs->GetPlayerCash( native->GetGameEntity() ) );
	return GM_OK;
}

static int GM_CDECL gmfBuy( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 1 );
	GM_CHECK_INT_PARAM( itm, 0 );
	Client *native = gmBot::GetThisObject( a_thread );
	if ( !native )
	{
		GM_EXCEPTION_MSG( "Script Function on NULL object" );
		return GM_EXCEPTION;
	}

	a_thread->PushInt( gQuake4Funcs->BuySomething( native->GetGameEntity(), (Q4_BuyOption)itm ) ? 1 : 0 );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////
// package: Q4 Global Functions
static gmFunctionEntry s_ExtendedBotLib[] =
{
	// Function: IsBuyingAllowed
	//		Gets the location name for a point.
	{ "IsBuyingAllowed", gmfIsBuyingAllowed },
};

static gmFunctionEntry s_ExtendedBotTypeLib[] =
{
	// Function: GetCredits
	//		Gets the cash for the bot.
	{ "GetCredits", gmfGetCash },

	// Function: Buy
	//		Buys an item for the bot.
	{ "Buy", gmfBuy },
};

void Q4_Game::InitScriptBinds( gmMachine *_machine )
{
	// Register the bot functions.
	_machine->RegisterLibrary( s_ExtendedBotLib, sizeof( s_ExtendedBotLib ) / sizeof( s_ExtendedBotLib[ 0 ] ) );
	//////////////////////////////////////////////////////////////////////////
	_machine->RegisterTypeLibrary( gmBot::GetType(), s_ExtendedBotTypeLib, sizeof( s_ExtendedBotTypeLib ) / sizeof( s_ExtendedBotTypeLib[ 0 ] ) );
}