////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "BotTargetingSystem.h"
#include "Client.h"
#include "FilterClosest.h"
#include "RenderBuffer.h"

namespace AiState
{
	TargetingSystem::TargetingSystem() : StateChild( "TargetingSystem" )
	{
	}

	TargetingSystem::~TargetingSystem()
	{
	}

	void TargetingSystem::RenderDebug()
	{
		if ( mCurrentTarget.IsValid() )
		{
			Vector3f vPos;
			EngineFuncs::EntityPosition( mCurrentTarget, vPos );
			RenderBuffer::AddLine( GetClient()->GetEyePosition(), vPos, COLOR::RED );
		}
		if ( mLastTarget.IsValid() )
		{
			Vector3f vPos;
			EngineFuncs::EntityPosition( mLastTarget, vPos );
			RenderBuffer::AddLine( GetClient()->GetEyePosition(), vPos, COLOR::ORANGE );
		}
	}

	void TargetingSystem::ForceTarget( GameEntity _ent )
	{
		mForceTarget = _ent;
	}

	const MemoryRecord *TargetingSystem::GetCurrentTargetRecord() const
	{
		// todo: cache the record
		return HasTarget() ? GetClient()->GetSensoryMemory()->GetMemoryRecord( GetCurrentTarget(), false, true ) : 0;
	}

	//////////////////////////////////////////////////////////////////////////
	// State stuff

	void TargetingSystem::Initialize()
	{
		// Give the bot a default targeting filter.
		FilterPtr filter( new FilterClosest( GetClient(), SensoryMemory::EntEnemy ) );
		filter->AddCategory( ENT_CAT_SHOOTABLE );
		SetDefaultTargetingFilter( filter );
	}

	void TargetingSystem::Exit()
	{
		mCurrentTarget.Reset();
	}

	State::StateStatus TargetingSystem::Update( float fDt )
	{
		rmt_ScopedCPUSample( TargetingSystemUpdate );
		{
			GameEntity newtarget;

			if ( mForceTarget.IsValid() )
			{
				const MemoryRecord *pRec = GetClient()->GetSensoryMemory()->GetMemoryRecord( mForceTarget );
				if ( !pRec || pRec->mTargetInfo.mEntInfo.mFlags.CheckFlag( ENT_FLAG_DEAD ) )
					mForceTarget.Reset();
				else
					newtarget = mForceTarget;
			}

			if ( !newtarget.IsValid() )
			{
				mDefaultFilter->Reset();
				GetClient()->GetSensoryMemory()->QueryMemory( *mDefaultFilter.get() );
				newtarget = mDefaultFilter->GetBestEntity();
			}

			// Update the last target.
			if ( newtarget.IsValid() && mCurrentTarget.IsValid() )
			{
				if ( newtarget != mCurrentTarget )
					mLastTarget = mCurrentTarget;
			}

			mCurrentTarget = newtarget;
		}
		return State_Busy;
	}
};