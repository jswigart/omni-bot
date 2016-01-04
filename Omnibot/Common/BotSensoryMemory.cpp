////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "BotSensoryMemory.h"
#include "IGameManager.h"
#include "ScriptManager.h"
#include "FilterSensory.h"
#include "RenderBuffer.h"

namespace AiState
{
	SensoryMemory::pfnGetEntityOffset SensoryMemory::mpfnGetTraceOffset = NULL;
	SensoryMemory::pfnGetEntityOffset SensoryMemory::mpfnGetAimOffset = NULL;

	SensoryMemory::SensoryMemory()
		: StateChild( "SensoryMemory", UpdateDelay( Utils::HzToSeconds( 10 ) ) ),
		mMemorySpan( 5000 )
	{
		mDebugFlags.SetFlag( Dbg_ShowPerception, true );
		mDebugFlags.SetFlag( Dbg_ShowEntities, true );
	}

	SensoryMemory::~SensoryMemory()
	{
	}

	int SensoryMemory::GetAllRecords( MemoryRecord *_records, int _max )
	{
		int iNum = 0;
		for ( int i = 0; i < MaxRecords; ++i )
		{
			if ( !mRecords[ i ].GetEntity().IsValid() )
				continue;

			_records[ iNum++ ] = mRecords[ i ];

			if ( iNum >= _max - 1 )
				break;
		}
		return iNum;
	}

	void SensoryMemory::GetDebugString( std::stringstream &out )
	{
		int iNumRecords = 0;
		for ( int i = 0; i < MaxRecords; ++i )
		{
			if ( mRecords[ i ].GetEntity().IsValid() )
				++iNumRecords;
		}
		out << iNumRecords;
	}

	void SensoryMemory::RenderDebug()
	{
		for ( int i = 0; i < MaxRecords; ++i )
		{
			if ( mRecords[ i ].GetEntity().IsValid() )
			{
				const MemoryRecord &r = mRecords[ i ];

				if ( mDebugFlags.CheckFlag( Dbg_ShowEntities ) )
				{
					Box3f worldObb;
					EngineFuncs::EntityWorldOBB( r.GetEntity(), worldObb );
					if ( worldObb.GetArea() <= 5.f )
						worldObb.Expand( 10.f );

					obColor col = r.IsShootable() ? COLOR::GREEN : COLOR::RED;

					if ( r.mTargetInfo.mEntInfo.mFlags.CheckFlag( ENT_FLAG_DEAD ) ||
						r.mTargetInfo.mEntInfo.mFlags.CheckFlag( ENT_FLAG_DISABLED ) )
						col = COLOR::BLACK;

					RenderBuffer::AddOBB( worldObb, col );

					std::string groupName, className;
					System::mInstance->mGame->FindClassName( groupName, className, r.mTargetInfo.mEntInfo.mGroup, r.mTargetInfo.mEntInfo.mClassId );
					RenderBuffer::AddString3d(
						worldObb.Center,
						COLOR::WHITE,
						va( "%s:%s", groupName.c_str(), className.c_str() ).c_str() );
				}
			}
		}
	}

	void SensoryMemory::Enter()
	{
	}

	void SensoryMemory::Exit()
	{
		// clear all shootable ents
		for ( int i = 0; i < MaxRecords; ++i )
		{
			mRecords[ i ].mIsShootable = false;
			mRecords[ i ].mInFOV = false;
			mRecords[ i ].mTimeLastSensed = -1;
		}
	}

	void SensoryMemory::SetEntityTraceOffsetCallback( pfnGetEntityOffset _pfnCallback )
	{
		mpfnGetTraceOffset = _pfnCallback;
	}

	void SensoryMemory::SetEntityAimOffsetCallback( pfnGetEntityOffset _pfnCallback )
	{
		mpfnGetAimOffset = _pfnCallback;
	}

	void SensoryMemory::UpdateEntities()
	{
		rmt_ScopedCPUSample( UpdateEntities );

		bool bFoundEntity = false;

		GameEntity selfEntity = GetClient()->GetGameEntity();

		IGame::EntityIterator ent;
		while ( IGame::IterateEntity( ent ) )
		{
			// skip myself
			if ( selfEntity == ent.GetEnt().mEntity )
				continue;

			// skip internal entities
			if ( ent.GetEnt().mEntInfo.mCategory.CheckFlag( ENT_CAT_INTERNAL ) )
				continue;

			int32_t iFreeRecord = -1;
			bFoundEntity = false;

			const int iStartIndex =
				ent.GetEnt().mEntInfo.mClassId < FilterSensory::ANYPLAYERCLASS /*|| ent.GetEnt().mEntInfo.mClassId == ENT_CLASS_SPECTATOR*/ ? 0 : 64;

			for ( int i = iStartIndex; i < MaxRecords; ++i )
			{
				if ( mRecords[ i ].GetEntity().IsValid() )
				{
					if ( mRecords[ i ].GetEntity().GetIndex() == ent.GetEnt().mEntity.GetIndex() )
					{
						mRecords[ i ].mEntity = ent.GetEnt().mEntity; // update just in case
						bFoundEntity = true;
						break;
					}
				}
				else
				{
					if ( iFreeRecord == -1 )
						iFreeRecord = i;
				}
			}

			if ( !bFoundEntity )
			{
				assert( iFreeRecord != -1 );
				if ( iFreeRecord != -1 )
				{
					mRecords[ iFreeRecord ].mEntity = ent.GetEnt().mEntity;
					mRecords[ iFreeRecord ].mTargetInfo.mEntInfo = ent.GetEnt().mEntInfo;
					mRecords[ iFreeRecord ].mTimeLastUpdated = -1; // initial update
				}
			}
		}
	}

	void SensoryMemory::UpdateSight()
	{
		rmt_ScopedCPUSample( UpdateSight );

		int iUpdated = 0;
		for ( int i = 0; i < MaxRecords; ++i )
		{
			if ( mRecords[ i ].GetEntity().IsValid() )
			{
				if ( !IGame::IsEntityValid( mRecords[ i ].GetEntity() ) )
				{
					mRecords[ i ].Reset();
					continue;
				}
				UpdateRecord( mRecords[ i ] );
				++iUpdated;
			}
		}
	}

	void SensoryMemory::UpdateSound()
	{
		rmt_ScopedCPUSample(UpdateSound );
	}

	void SensoryMemory::UpdateSmell()
	{
		rmt_ScopedCPUSample( UpdateSmell );
	}

	void SensoryMemory::UpdateWithSoundSource( const EvSound *_sound )
	{
		// TODO: don't bother storing sound memory from myself.
		if ( _sound->mSource.IsValid() && ( GetClient()->GetGameEntity() != _sound->mSource ) )
		{
		}
	}

	void SensoryMemory::UpdateWithTouchSource( GameEntity _sourceent )
	{
		// Paranoid check that we aren't touching ourselves, har har
		if ( _sourceent.IsValid() && ( GetClient()->GetGameEntity() != _sourceent ) )
		{
			MemoryRecord *pRecord = GetMemoryRecord( _sourceent, true, false );
			if ( pRecord )
			{
				EntityInfo classInfo;
				if ( !IGame::GetEntityInfo( _sourceent, pRecord->mTargetInfo.mEntInfo ) )
					return;
				
				// Get the entity position.
				Vector3f vThreatPosition( Vector3f::ZERO );
				EngineFuncs::EntityPosition( _sourceent, vThreatPosition );

				// Add it to the list.
				pRecord->mInFOV = true;
				pRecord->mIsShootable = GetClient()->HasLineOfSightTo( vThreatPosition, _sourceent );
				pRecord->mTimeLastSensed = IGame::GetTime();
				pRecord->mIsAllied = GetClient()->IsAllied( _sourceent );

				// Update Target Info.
				pRecord->mTargetInfo.mLastPosition = vThreatPosition;

				if ( pRecord->mIsShootable )
				{
					pRecord->mTimeLastVisible = IGame::GetTime();
				}
			}
		}
	}

	bool SensoryMemory::UpdateRecord( MemoryRecord &_record )
	{
		if ( _record.GetAge() <= 0 )
			return true;

		_record.MarkUpdated();

		TargetInfo &ti = _record.mTargetInfo;

		const bool bNoLos = ti.mEntInfo.mCategory.CheckFlag( ENT_CAT_NOLOS );
		const bool bShootable = ti.mEntInfo.mCategory.CheckFlag( ENT_CAT_SHOOTABLE );

		GameEntity ent = _record.GetEntity();

		//////////////////////////////////////////////////////////////////////////
		// Some entity flags are persistent and won't be overwritten until the subject is perceived again.
		const BitFlag64 bfPersistantMask
			( ( (int64_t)1 << ENT_FLAG_DEAD )
			| ( (int64_t)1 << ENT_FLAG_DISABLED ) );

		BitFlag64 bfPersistantFlags = bfPersistantMask & ti.mEntInfo.mFlags;
		//////////////////////////////////////////////////////////////////////////

		// clear targeting info
		if ( ti.mEntInfo.mFlags.CheckFlag( ENT_FLAG_DEAD ) || ti.mEntInfo.mFlags.CheckFlag( ENT_FLAG_DISABLED ) )
		{
			_record.mIsShootable = false;
			_record.mInFOV = false;
			_record.mTimeLastSensed = -1;
		}

		// Update data that changes.
		if ( !IGame::GetEntityInfo( ent, ti.mEntInfo ) )
			return false;

		Vector3f vNewPosition;
		if ( !EngineFuncs::EntityPosition( ent, vNewPosition ) )
			return false;

		//////////////////////////////////////////////////////////////////////////
		Vector3f vTracePosition = vNewPosition;
		if ( mpfnGetTraceOffset )
			vTracePosition.Z() += mpfnGetTraceOffset( ti );

		if ( bNoLos ||
			( GetClient()->IsWithinViewDistance( vTracePosition ) &&
			GetClient()->InFieldOfView( vTracePosition ) ) )
		{
			const float DistanceToEntity = Length( vTracePosition, GetClient()->GetEyePosition() );
			float EntityViewDistance = Utils::FloatMax;
			/*if ( mpfnGetVisDistance )
				mpfnGetVisDistance( EntityViewDistance, ti, GetClient() );*/

			if ( DebugDrawingEnabled() && mDebugFlags.CheckFlag( Dbg_ShowPerception ) )
			{
				RenderBuffer::AddLine( GetClient()->GetEyePosition(), vTracePosition, COLOR::YELLOW );
			}

			if ( bNoLos ||
				( DistanceToEntity < EntityViewDistance &&
				( !ti.mEntInfo.mFlags.CheckFlag( ENT_FLAG_VISTEST ) ||
				GetClient()->HasLineOfSightTo( vTracePosition, ent ) ) ) )
			{
				//const bool bNewlySeen = (IGame::GetTime() - _record.GetTimeLastSensed()) > mMemorySpan;

				// Still do the raycast on shootable statics
				if ( bNoLos )
				{
					//const bool bWasShootable = _record.mIsShootable;
					//const bool bWasInFov = _record.mInFOV;

					_record.mIsShootable = false;
					if ( bShootable )
					{
						if ( !GetClient()->HasLineOfSightTo( vTracePosition, ent ) )
						{
							_record.mIsShootable = false;
							_record.mInFOV = false;
							return true;
						}
					}
				}

				_record.mIsShootable = true;
				_record.mTimeLastSensed = IGame::GetTime();
				_record.mTimeLastVisible = IGame::GetTime();
				_record.mIsAllied = GetClient()->IsAllied( ent );
				
				FireMode currentWeaponMode;
				gEngineFuncs->GetEquippedWeapon( ent, _record.mTargetInfo.mCurrentWeapon, currentWeaponMode );

				if ( !_record.IsShootable() && !bNoLos )
				{
					// restore the old flags
					ti.mEntInfo.mFlags &= ~bfPersistantMask;
					ti.mEntInfo.mFlags |= bfPersistantFlags;
				}

				//////////////////////////////////////////////////////////////////////////
				// Update Target Info.
				ti.mLastPosition = vNewPosition;
				if ( mpfnGetAimOffset )
					ti.mLastPosition.Z() += mpfnGetAimOffset( ti );

				if ( DebugDrawingEnabled() && mDebugFlags.CheckFlag( Dbg_ShowPerception ) )
				{
					RenderBuffer::AddLine( GetClient()->GetEyePosition(), ti.mLastPosition, COLOR::YELLOW );
				}

				_record.mTargetInfo.mDistanceTo = Length( ti.mLastPosition, GetClient()->GetEyePosition() );
				if ( !bNoLos )
				{
					EngineFuncs::EntityOrientation( ent, ti.mLastFacing, 0, 0 );
					EngineFuncs::EntityVelocity( ent, ti.mLastVelocity );
				}
				//////////////////////////////////////////////////////////////////////////

				if ( _record.mInFOV == false )
				{
					_record.mInFOV = true;
					_record.mTimeBecameVisible = IGame::GetTime();
				}

				/*if(bNewlySeen)
				{
				Event_EntitySensed d;
				d.mEntityClass = ti.mEntityClass;
				d.mEntity = ent;
				GetClient()->SendEvent(Message(PERCEPT_SENSE_ENTITY, &d, sizeof(d)));
				}*/
			}
			else
			{
				// restore the old flags
				ti.mEntInfo.mFlags &= ~bfPersistantMask;
				ti.mEntInfo.mFlags |= bfPersistantFlags;

				// Can't see him.
				_record.mIsShootable = false;
				_record.mInFOV = false;
			}
		}
		return true;
	}

	void SensoryMemory::QueryMemory( FilterSensory &_filter )
	{
		rmt_ScopedCPUSample( SensoryQueryMemory );
		for ( int i = 0; i < MaxRecords; ++i )
		{
			if ( mRecords[ i ].GetEntity().IsValid() )
				_filter.Check( i, mRecords[ i ] );
		}
		_filter.PostQuery();
	}

	const TargetInfo *SensoryMemory::GetTargetInfo( const GameEntity _ent )
	{
		for ( int i = 0; i < MaxRecords; ++i )
		{
			if ( mRecords[ i ].GetEntity().IsValid() && mRecords[ i ].GetEntity() == _ent )
			{
				return &mRecords[ i ].mTargetInfo;
			}
		}
		return NULL;
	}

	int SensoryMemory::CheckTargetsInRadius( const Vector3f &_pos, float _radius,
		SensoryMemory::Type _type, const BitFlag64 &_category )
	{
		int iNumInRadius = 0;
		/*float fRadSq = _radius * _radius;

		EntityList entList;

		FilterAllType all(entList, mClient, _type, _category, BitFlag64(0));
		all.SetSortType(FilterSensory::Sort_NearToFar);
		mClient->GetSensoryMemory()->QueryMemory(all);

		Vector3f vPos;
		for(uint32_t i = 0; i < entList.size(); ++i)
		{
		if(GetLastRecordedPosition(entList[i], vPos))
		{
		if((vPos - _pos).SquaredLength() <= fRadSq)
		{
		++iNumInRadius;
		}
		}
		}*/
		return iNumInRadius;
	}

	MemoryRecord *SensoryMemory::GetMemoryRecord( GameEntity _ent, bool _add, bool _update )
	{
		MemoryRecord *pRecord = 0;

		int iFreeSlot = -1;
		for ( int i = 0; i < MaxRecords; ++i )
		{
			if ( mRecords[ i ].GetEntity().IsValid() )
			{
				if ( mRecords[ i ].GetEntity() == _ent )
				{
					pRecord = &mRecords[ i ];
					break;
				}
			}
			else if ( iFreeSlot == -1 && i >= 64 )
			{
				iFreeSlot = i;
			}
		}

		if ( !pRecord && _add && iFreeSlot != -1 )
		{
			pRecord = &mRecords[ iFreeSlot ];
			pRecord->Reset( _ent );
		}

		if ( _update && pRecord )
			UpdateRecord( *pRecord );

		return pRecord;
	}

	MemoryRecord *SensoryMemory::GetMemoryRecord( const RecordHandle &_hndl )
	{
		MemoryRecord *pRecord = 0;

		if ( _hndl.IsValid() )
		{
			int ix = (int)_hndl.GetIndex();
			if ( InRangeT<int>( ix, 0, MaxRecords - 1 ) &&
				( mRecords[ _hndl.GetIndex() ].mSerial == _hndl.GetSerial() ) )
			{
				pRecord = &mRecords[ _hndl.GetIndex() ];
			}
		}
		return pRecord;
	}

	void SensoryMemory::GetRecordInfo( const MemoryRecords &_hndls, Vector3List *_pos, Vector3List *_vel )
	{
		for ( uint32_t i = 0; i < _hndls.size(); ++i )
		{
			const MemoryRecord *pRec = GetMemoryRecord( _hndls[ i ] );
			if ( pRec )
			{
				if ( _pos ) _pos->push_back( pRec->GetLastSensedPosition() );
				if ( _vel ) _vel->push_back( pRec->GetLastSensedVelocity() );
			}
		}
	}

	int SensoryMemory::FindEntityByCategoryInRadius( float radius, BitFlag32 category, RecordHandle hndls [], int maxHandles )
	{
		int numRecords = 0;
		for ( int i = 0; i < MaxRecords && numRecords < maxHandles; ++i )
		{
			if ( mRecords[ i ].GetEntity().IsValid() )
			{
				if ( !mRecords[ i ].mTargetInfo.mEntInfo.mFlags.CheckFlag( ENT_FLAG_DISABLED ) )
				{
					if ( ( mRecords[ i ].mTargetInfo.mEntInfo.mCategory & category ).AnyFlagSet() )
					{
						if ( mRecords[ i ].mTargetInfo.mDistanceTo <= radius && mRecords[ i ].GetTimeLastSensed() >= 0 )
						{
							hndls[ numRecords++ ] = RecordHandle( (int16_t)i, mRecords[ i ].GetSerial() );
						}
					}
				}
			}
		}
		return numRecords;
	}

	int SensoryMemory::FindEntityByCategoryInRadius( float radius, BitFlag32 category, GameEntity ents [], int maxEnts )
	{
		int numRecords = 0;
		for ( int i = 0; i < MaxRecords && numRecords < maxEnts; ++i )
		{
			if ( mRecords[ i ].GetEntity().IsValid() )
			{
				if ( !mRecords[ i ].mTargetInfo.mEntInfo.mFlags.CheckFlag( ENT_FLAG_DISABLED ) )
				{
					if ( ( mRecords[ i ].mTargetInfo.mEntInfo.mCategory & category ).AnyFlagSet() )
					{
						if ( mRecords[ i ].mTargetInfo.mDistanceTo <= radius && mRecords[ i ].GetTimeLastSensed() >= 0 )
						{
							ents[ numRecords++ ] = mRecords[ i ].GetEntity();
						}
					}
				}
			}
		}
		return numRecords;
	}

	bool SensoryMemory::HasLineOfSightTo( const MemoryRecord &mr, int customTraceMask )
	{
		Vector3f vTracePosition = mr.mTargetInfo.mLastPosition;
		if ( mpfnGetTraceOffset )
			vTracePosition.Z() += mpfnGetTraceOffset( mr.mTargetInfo );
		return GetClient()->HasLineOfSightTo( vTracePosition, mr.GetEntity(), customTraceMask );
	}

	//////////////////////////////////////////////////////////////////////////
	// State stuff

	State::StateStatus SensoryMemory::Update( float fDt )
	{
		rmt_ScopedCPUSample( SensoryMemory );

		UpdateEntities();
		UpdateSight();
		//UpdateSound();
		//UpdateSmell();

		return State_Busy;
	}
};