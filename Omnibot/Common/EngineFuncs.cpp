////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include <mutex>
#include <thread>

#include "EngineFuncs.h"
#include "Utilities.h"
#include "RenderBuffer.h"
#include "IGameManager.h"

StringVector		gThreadMessages;
StringVector		gThreadErrors;
boost::mutex		gMessageMutex;

namespace EngineFuncs
{
	bool TraceLine( obTraceResult &_tr, const Vector3f &_start, const Vector3f &_end,
		const AABB *_aabb, int _mask, int _user, bool _usepvs )
	{
		_tr.mEndpos[ 0 ] = _end.X();
		_tr.mEndpos[ 1 ] = _end.Y();
		_tr.mEndpos[ 2 ] = _end.Z();
		return SUCCESS( gEngineFuncs->TraceLine( _tr, _start, _end, _aabb, _mask, _user, _usepvs ) );
	}
	std::string EntityName( const GameEntity _ent, const char* defaultName )
	{
		obStringBuffer nameBuffer;
		gEngineFuncs->GetEntityName( _ent, nameBuffer );
		return nameBuffer.mBuffer[ 0 ] ? nameBuffer.mBuffer : defaultName;
	}
	GameEntity EntityOwner( const GameEntity _ent )
	{
		return gEngineFuncs->GetEntityOwner( _ent );
	}
	bool EntityPosition( const GameEntity _ent, Vector3f &_pos )
	{
		return SUCCESS( gEngineFuncs->GetEntityPosition( _ent, _pos ) );
	}
	bool EntityEyePosition( const GameEntity _ent, Vector3f &_pos )
	{
		return SUCCESS( gEngineFuncs->GetEntityEyePosition( _ent, _pos ) );
	}
	bool EntityBonePosition( const GameEntity _ent, int _boneid, Vector3f &_pos )
	{
		return SUCCESS( gEngineFuncs->GetEntityBonePosition( _ent, _boneid, _pos ) );
	}
	bool EntityOrientation( const GameEntity _ent, float _fwd[ 3 ], float _right[ 3 ], float _up[ 3 ] )
	{
		return SUCCESS( gEngineFuncs->GetEntityOrientation( _ent, _fwd, _right, _up ) );
	}
	bool EntityOrientation( const GameEntity _ent, Matrix3f & mat )
	{
		Vector3f fwd, right, up;
		if ( SUCCESS( gEngineFuncs->GetEntityOrientation( _ent, fwd, right, up ) ) )
		{
			mat = Matrix3f( fwd, right, up, true );
			return true;
		}
		return false;
	}
	bool EntityVelocity( const GameEntity _ent, Vector3f &_vel )
	{
		return SUCCESS( gEngineFuncs->GetEntityVelocity( _ent, _vel ) );
	}
	bool EntityLocalAABB( const GameEntity _ent, AABB &_bounds )
	{
		return SUCCESS( gEngineFuncs->GetEntityLocalAABB( _ent, _bounds ) );
	}
	bool EntityWorldOBB( const GameEntity _ent, Box3f &_bounds )
	{
		if ( SUCCESS( gEngineFuncs->GetEntityWorldOBB( _ent,
			_bounds.Center,
			_bounds.Axis[ 0 ],
			_bounds.Axis[ 1 ],
			_bounds.Axis[ 2 ],
			_bounds.Extent ) ) )
		{
			return true;
		}
		return false;
	}
	bool EntityGroundEntity( const GameEntity _ent, GameEntity &moveent )
	{
		return SUCCESS( gEngineFuncs->GetEntityGroundEntity( _ent, moveent ) );
	}
	bool IsInPvs( const Vector3f &_source, const Vector3f &_target )
	{
		return gEngineFuncs->IsInPVS( _source, _target );
	}
	bool GroundPosition( const Vector3f &_pos, Vector3f &_outPosition, Vector3f &_outNormal, float _offset )
	{
		obTraceResult tr;
		EngineFuncs::TraceLine( tr,
			_pos + Vector3f( 0.f, 0.f, _offset ),
			_pos - Vector3f::UNIT_Z * 2048.f,
			NULL,
			TR_MASK_FLOODFILL, -1, false );
		if ( tr.mFraction < 1.f )
		{
			_outPosition = Vector3f( tr.mEndpos[ 0 ], tr.mEndpos[ 1 ], tr.mEndpos[ 2 ] );
			_outNormal = Vector3f( tr.mNormal[ 0 ], tr.mNormal[ 1 ], tr.mNormal[ 2 ] );
			return true;
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////

	void FlushAsyncMessages()
	{
		if ( gMessageMutex.try_lock() )
		{
			for ( size_t i = 0; i < gThreadErrors.size(); ++i )
				gEngineFuncs->PrintError( gThreadErrors[ i ].c_str() );

			for ( size_t i = 0; i < gThreadMessages.size(); ++i )
				gEngineFuncs->PrintMessage( gThreadMessages[ i ].c_str() );

			gThreadErrors.resize( 0 );
			gThreadMessages.resize( 0 );

			gMessageMutex.unlock();
		}
	}

	//////////////////////////////////////////////////////////////////////////

	void ConsoleMessage( const char* _msg )
	{
		rmt_LogText( va( "Message: %s", _msg ).c_str() );

		if ( IGameManager::sMainThread == boost::this_thread::get_id() )
			gEngineFuncs->PrintMessage( _msg );
		else
		{
			boost::lock_guard<boost::mutex> lk( gMessageMutex );
			gThreadMessages.push_back( _msg );
		}
	}

	void ConsoleError( const char* _msg )
	{
		rmt_LogText( va( "Error: %s", _msg ).c_str() );

		if ( IGameManager::sMainThread == boost::this_thread::get_id() )
			gEngineFuncs->PrintError( _msg );
		else
		{
			boost::lock_guard<boost::mutex> lk( gMessageMutex );
			gThreadErrors.push_back( _msg );
		}
	}
};