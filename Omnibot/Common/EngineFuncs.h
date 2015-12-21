////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __ENGINEFUNCS_H__
#define __ENGINEFUNCS_H__

#include "common.h"
#include "IEngineInterface.h"

namespace EngineFuncs
{
	bool TraceLine( obTraceResult &_tr, const Vector3f &_start, const Vector3f &_end,
		const AABB *_aabb, int _mask, int _user, bool _usepvs );

	std::string EntityName( const GameEntity _ent, const char* defaultName = "" );
	GameEntity EntityOwner( const GameEntity _ent );
	bool EntityPosition( const GameEntity _ent, Vector3f &_pos );
	bool EntityEyePosition( const GameEntity _ent, Vector3f &_pos );
	bool EntityBonePosition( const GameEntity _ent, int _boneid, Vector3f &_pos );
	bool EntityOrientation( const GameEntity _ent, float _fwd[ 3 ], float _right[ 3 ], float _up[ 3 ] );
	bool EntityOrientation( const GameEntity _ent, Matrix3f & mat );
	bool EntityVelocity( const GameEntity _ent, Vector3f &_vel );
	bool EntityLocalAABB( const GameEntity _ent, AABB &_bounds );

	bool EntityWorldOBB( const GameEntity _ent, Box3f &_bounds );
	bool EntityGroundEntity( const GameEntity _ent, GameEntity &moveent );
	bool IsInPvs( const Vector3f &_source, const Vector3f &_target );

	bool GroundPosition( const Vector3f &_pos, Vector3f &_outPosition, Vector3f &_outNormal, float _offset = 0.f );

	//////////////////////////////////////////////////////////////////////////

	void FlushAsyncMessages();

	void ConsoleMessage( const char* _msg );
	void ConsoleError( const char* _msg );
};

namespace Constants
{
	enum Internal
	{
		MAX_PLAYERS = 64, MAX_ENTITIES = 4096
	};
}

#endif
