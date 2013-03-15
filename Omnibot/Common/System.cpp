#include "System.h"

#include <assert.h>
#include <stddef.h>

System * System::mInstance = NULL;

System::System()
	: mGame( NULL )
	, mScript( NULL )
	, mNavigation( NULL )
	, mGoalManager( NULL )
	, mTaskManager( NULL )
	, mTriggerManager( NULL )
	, mNameManager( NULL )
{
	assert( mInstance == NULL );
	mInstance = this;
}