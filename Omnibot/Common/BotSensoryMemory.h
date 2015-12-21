////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __BOTSENSORYMEMORY_H__
#define __BOTSENSORYMEMORY_H__

#include "platformspecifics.h"

#include "Omni-Bot_Types.h"
#include "MemoryRecord.h"
#include "Client.h"
#include "IGame.h"

#include <boost/pool/pool_alloc.hpp>

class FilterSensory;

namespace AiState
{
	// class: SensoryMemory
	//		Contains information about what a bot 'knows' about any entity it
	//		has sensed
	class SensoryMemory : public StateChild
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		// typedef: pfnGetEntityOffset
		//		Callback function for getting an offset for an entity type
		typedef const float( *pfnGetEntityOffset )( const TargetInfo &_target );
		// typedef: pfnGetEntityOffset
		//		Callback function for getting an offset for an entity type
		typedef const void( *pfnGetEntityVisDistance )( float &_distance, const TargetInfo &_target, const Client *_client );
		// typedef: pfnCanSensoreEntity
		//		Callback function for checking whether an entity can be shot or watched
		typedef const bool( *pfnCanSensoreEntity )( const EntityInstance &_ent );
		//////////////////////////////////////////////////////////////////////////

		enum DebugFlags
		{
			Dbg_ShowPerception,
			Dbg_ShowEntities,
		};

		// enumerations: Type
		//		EntAny - Identifies ANY type of entity, friend or foe
		//		EntEnemy - Identifies enemy entities
		//		EntAlly - Identifies ally entities
		enum Type
		{
			EntAny, EntEnemy, EntAlly
		};

		// typedef: MemoryMap
		//	stdext::hash_map of entities to their memory record.
#ifdef WIN32
		typedef boost::fast_pool_allocator< std::pair< GameEntity, MemoryRecord >, boost::default_user_allocator_new_delete, boost::details::pool::default_mutex, 769 > MemoryMapAllocator;
		typedef std::hash_compare<GameEntity> HashMapCompare;
		typedef std::unordered_map<GameEntity, MemoryRecord, HashMapCompare, MemoryMapAllocator > MemoryMap;
#else
		typedef stdext::hash_map<GameEntity, MemoryRecord> MemoryMap;
#endif

		// function: UpdateWithSoundSource
		//		Updates memory record from sound input.
		void UpdateWithSoundSource( const EvSound *_sound );

		// function: UpdateWithTouchSource
		//		Updates memory record from touch input.
		void UpdateWithTouchSource( GameEntity _sourceent );

		// function: RemoveBotFromMemory
		//		Removes an entity from memory of this bot.
		//void RemoveEntityFromMemory(const GameEntity _ent);

		void UpdateEntities();

		// function: UpdateSight
		//		Allows the bot to update its vision from the game.
		void UpdateSight();

		// function: UpdateSound
		//		Allows the bot to update its hearing from the game.
		void UpdateSound();

		// function: UpdateSmell
		//		Allows the bot to update its smell from the game.
		void UpdateSmell();

		bool UpdateRecord( MemoryRecord &_record );

		// function: GetTargetInfo
		//		Gets the target info for an entity, this includes things such
		//		as position, velocity, facing, distance to, class, flags, category
		const TargetInfo *GetTargetInfo( const GameEntity _ent );

		// function: QueryMemory
		//		Performs a query on the sensory memory, taking a custom filter to use.
		//		The filter is responsible for determining the result of a query.
		//		See <FilterAllType>, <FilterClosest>, <FilterMostHurt>, for default examples
		void QueryMemory( FilterSensory &_filter );

		// function: SetEntityTraceOffsetCallback
		//		Sets a callback to be used as <.mpfnGetTraceOffset>
		static void SetEntityTraceOffsetCallback( pfnGetEntityOffset _pfnCallback );

		// function: SetEntityAimOffsetCallback
		//		Sets a callback to be used as <.mpfnGetAimOffset>
		static void SetEntityAimOffsetCallback( pfnGetEntityOffset _pfnCallback );
		
		// function: GetMemorySpan
		//		Accessor for the bots memory span
		inline int GetMemorySpan() const
		{
			return mMemorySpan;
		}

		// function: SetMemorySpan
		//		Modifier for the bots memory span
		inline void SetMemorySpan( int _memspan )
		{
			mMemorySpan = _memspan;
		}

		// function: GetMemoryRecord
		//		Accessor for an entities memory record.
		MemoryRecord *GetMemoryRecord( GameEntity _ent, bool _add = false, bool _update = false );

		MemoryRecord *GetMemoryRecord( const RecordHandle &_hndl );

		void GetRecordInfo( const MemoryRecords &_hndls, Vector3List *_pos, Vector3List *_vel );

		int FindEntityByCategoryInRadius( float radius, BitFlag32 category, RecordHandle hndls [], int maxHandles );
		int FindEntityByCategoryInRadius( float radius, BitFlag32 category, GameEntity ents [], int maxEnts );

		bool HasLineOfSightTo( const MemoryRecord &mr, int customTraceMask = 0 );

		// function: CheckTargetsInRadius
		//		Static helper function for counting the number of target types in a given
		//		radius from a given point.
		//
		// Parameters:
		//
		//		_pos - The <Vector3> positionto check.
		//		_radius - The radius to check for targets within.
		//		_type - The type of targets to check for. Enemy/Friendly/Any.
		//		_category - The category of entities to search.
		//
		// Returns:
		//		Vector3 - A point along the targets current velocity to aim towards.
		int CheckTargetsInRadius( const Vector3f &_pos, float _radius,
			SensoryMemory::Type _type, const BitFlag64 &_category );

		void GetDebugString( std::stringstream &out );
		void RenderDebug();

		void Enter();
		void Exit();

		StateStatus Update( float fDt );

		enum
		{
			MaxRecords = Constants::MAX_ENTITIES
		};

		int GetAllRecords( MemoryRecord *_records, int _max );

		BitFlag32	 mDebugFlags;

		SensoryMemory();
		virtual ~SensoryMemory();
	protected:
		MemoryRecord mRecords[ MaxRecords ];

		// int: mMemorySpan
		//		The amount of time this bot can 'remember' an entity, before
		//		the memory record is treated as invalid
		int			 mMemorySpan;

		// function: AddRecordIfNotPresent
		//		Adds a memory record for an entity if not already present.
		//MemoryRecord &AddRecordIfNotPresent(const GameEntity _ent);

		// callback: mpfnGetTraceOffset
		//		Method the Sensory memory uses to request a trace offset from the game
		static pfnGetEntityOffset mpfnGetTraceOffset;
		// callback: mpfnGetAimOffset
		//		Method the Sensory memory uses to request an aim offset from the game
		static pfnGetEntityOffset mpfnGetAimOffset;
		// callback: mpfnGetVisDistance
		//		Method the Sensory memory uses to request an entity visibility distance from the game.
		static pfnGetEntityVisDistance	 mpfnGetVisDistance;
	};
};
#endif
