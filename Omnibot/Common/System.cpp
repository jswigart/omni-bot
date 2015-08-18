#include "System.h"

#include <assert.h>
#include <stddef.h>

System * System::mInstance = NULL;

System::System()
	: mGame( NULL )
	, mScript( NULL )
	, mNavigation( NULL )
	, mGoalManager( NULL )
	, mTacticalManager( NULL )
	, mTriggerManager( NULL )
	, mNameManager( NULL )
	, mAnalytics( NULL )
{
	assert( mInstance == NULL );
	mInstance = this;
}