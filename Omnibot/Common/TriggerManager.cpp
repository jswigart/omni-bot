////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "TriggerManager.h"
#include "gmCall.h"
#include "gmTriggerInfo.h"

#include "IGame.h"
#include "ScriptManager.h"
#include "FilterSensory.h"
#include "RenderBuffer.h"

//////////////////////////////////////////////////////////////////////////
class TriggerShape
{
public:
	struct InTrigger
	{
		GameEntity mEntity;
		int		 mTimeStamp;
	};
	enum
	{
		MaxEntCount = 8,
		MaxClassCount = 8,
		MaxInTrigger = 128,
	};

	void SetNameHash( uint32_t _name )
	{
		mNameHash = _name;
	}
	uint32_t GetNameHash() const
	{
		return mNameHash;
	}
	int GetSerial() const
	{
		return mSerialNum;
	}
	void SetDeleteMe()
	{
		mDeleteMe = true;
	} // cs: added so script function can 'schedule' a deletion
	bool DeleteMe() const
	{
		return mDeleteMe;
	}
	bool Expired() const
	{
		return DeleteMe() || ( mExpireTime && IGame::GetTime() >= mExpireTime );
	}

	virtual void UpdatePosition( const Vector3f &pos ) = 0;

	virtual bool Test( GameEntity e, const Box3f & worldObb ) = 0;

	void SetOnEnter( gmGCRoot<gmFunctionObject> &_onenter )
	{
		mOnEnterFunction = _onenter;
	}
	void SetOnExit( gmGCRoot<gmFunctionObject> &_onexit )
	{
		mOnExitFunction = _onexit;
	}

	bool TriggerableEntity( const EntityInstance &_ent )
	{
		// check category
		if ( mTriggerOnCategory.AnyFlagSet() )
		{
			if ( ( _ent.mEntInfo.mCategory & mTriggerOnCategory ).AnyFlagSet() )
				return true;
		}

		// check class
		for ( int i = 0; i < MaxClassCount; ++i )
		{
			if ( mTriggerOnClass[ i ] == 0 )
				break;

			if ( mTriggerOnClass[ i ] == FilterSensory::ANYPLAYERCLASS )
			{
				if ( _ent.mEntInfo.mClassId < FilterSensory::ANYPLAYERCLASS )
					return true;
			}

			if ( mTriggerOnClass[ i ] == _ent.mEntInfo.mClassId )
				return true;
		}

		// check specific entities.
		for ( int i = 0; i < MaxEntCount; ++i )
		{
			if ( !mTriggerOnEntity[ i ].IsValid() )
				break;

			if ( mTriggerOnEntity[ i ] == _ent.mEntity )
				return true;
		}
		return false;
	}

	void FireEnterEvent( GameEntity _ent )
	{
		if ( mOnEnterFunction )
		{
			gmMachine *m = ScriptManager::GetInstance()->GetMachine();

			gmCall call;
			if ( call.BeginFunction( m, mOnEnterFunction, mThisObject ) )
			{
				call.AddParamEntity( _ent.AsInt() );
				call.End();

				int iRet = 0;
				if ( call.GetReturnedInt( iRet ) && iRet == 1 )
					mDeleteMe = true;
			}
		}
	}
	void FireExitEvent( GameEntity _ent )
	{
		if ( mOnExitFunction )
		{
			gmMachine *m = ScriptManager::GetInstance()->GetMachine();

			gmCall call;
			if ( call.BeginFunction( m, mOnExitFunction, mThisObject ) )
			{
				call.AddParamEntity( _ent.AsInt() );
				call.End();

				int iRet = 0;
				if ( call.GetReturnedInt( iRet ) && iRet == 1 )
					mDeleteMe = true;
			}
		}
	}

	void FireTrigger( const EntityInstance &_ent )
	{
		InTrigger *it = NULL;

		// If it's already in the trigger, just update the timestamp.
		for ( int i = 0; i < MaxInTrigger; ++i )
		{
			if ( !it && !mInTrigger[ i ].mEntity.IsValid() )
				it = &mInTrigger[ i ];

			if ( mInTrigger[ i ].mEntity == _ent.mEntity )
			{
				mInTrigger[ i ].mTimeStamp = IGame::GetTime();
				return;
			}
		}

		if ( it )
		{
			// otherwise add it to the list of entities in trigger, and fire an enter event.
			it->mEntity = _ent.mEntity;
			it->mTimeStamp = IGame::GetTime();
			FireEnterEvent( _ent.mEntity );
		}
	}

	void Update()
	{
		if ( mNextUpdateTime > IGame::GetTime() )
			return;

		IGame::EntityIterator ent;
		while ( IGame::IterateEntity( ent ) )
		{
			if ( TriggerableEntity( ent.GetEnt() ) )
			{
				Box3f obb;
				if ( EngineFuncs::EntityWorldOBB( ent.GetEnt().mEntity, obb ) )
				{
					if ( Test( ent.GetEnt().mEntity, obb ) )
					{
						FireTrigger( ent.GetEnt() );

						if ( DeleteMe() )
							return;
					}
				}
			}
		}

		// fire exit events for any expired objects.
		for ( int i = 0; i < MaxInTrigger; ++i )
		{
			if ( mInTrigger[ i ].mEntity.IsValid() &&
				mInTrigger[ i ].mTimeStamp != IGame::GetTime() )
			{
				FireExitEvent( mInTrigger[ i ].mEntity );

				mInTrigger[ i ].mEntity.Reset();
				mInTrigger[ i ].mTimeStamp = 0;
				return;
			}
		}
	}

	virtual void RenderDebug()
	{
		for ( int i = 0; i < MaxInTrigger; ++i )
		{
			if ( mInTrigger[ i ].mEntity.IsValid() )
			{
				Box3f bounds;
				if ( EngineFuncs::EntityWorldOBB( mInTrigger[ i ].mEntity, bounds ) )
				{
					RenderBuffer::AddOBB( bounds, COLOR::ORANGE );

					std::string name = Utils::HashToString( GetNameHash() );
					RenderBuffer::AddString3d( bounds.Center, COLOR::ORANGE, name.c_str() );
				}
			}
		}
	}

	bool FromTable( gmMachine *_m, gmTableObject *tbl )
	{
		gmVariable vName = tbl->Get( _m, "Name" );
		gmVariable vOnEnter = tbl->Get( _m, "OnEnter" );
		gmVariable vOnExit = tbl->Get( _m, "OnExit" );
		gmVariable vUpdateDelay = tbl->Get( _m, "UpdateDelay" );
		mThisObject = tbl->Get( _m, "CallbackObject" );

		if ( vOnEnter.GetFunctionObjectSafe() )
			mOnEnterFunction.Set( vOnEnter.GetFunctionObjectSafe(), _m );
		if ( vOnExit.GetFunctionObjectSafe() )
			mOnExitFunction.Set( vOnExit.GetFunctionObjectSafe(), _m );

		// must have at least 1 callback
		if ( !mOnEnterFunction && !mOnExitFunction )
			return false;

		bool bHasTriggerCondition = false;

		{
			gmVariable vTriggerOnClass = tbl->Get( _m, "TriggerOnClass" );
			if ( vTriggerOnClass.IsInt() )
			{
				bHasTriggerCondition = true;
				mTriggerOnClass[ 0 ] = vTriggerOnClass.GetInt();
			}
			else
			{
				gmTableObject *tbl = vTriggerOnClass.GetTableObjectSafe();
				if ( tbl )
				{
					gmTableIterator tIt;
					gmTableNode *pNode = tbl->GetFirst( tIt );
					while ( pNode )
					{
						if ( pNode->m_value.IsInt() )
						{
							for ( int c = 0; c < MaxClassCount; ++c )
							{
								if ( mTriggerOnClass[ c ] == 0 )
								{
									bHasTriggerCondition = true;
									mTriggerOnClass[ c ] = pNode->m_value.GetInt();
									break;
								}
							}
						}
						pNode = tbl->GetNext( tIt );
					}
				}
			}
		}
		{
			gmVariable vTriggerOnCategory = tbl->Get( _m, "TriggerOnCategory" );
			if ( vTriggerOnCategory.IsInt() )
			{
				bHasTriggerCondition = true;
				mTriggerOnCategory.SetFlag( vTriggerOnCategory.GetInt(), true );
			}
			else
			{
				gmTableObject *tbl = vTriggerOnCategory.GetTableObjectSafe();
				if ( tbl )
				{
					gmTableIterator tIt;
					gmTableNode *pNode = tbl->GetFirst( tIt );
					while ( pNode )
					{
						if ( pNode->m_value.IsInt() )
						{
							mTriggerOnCategory.SetFlag( pNode->m_value.GetInt(), true );
						}
						pNode = tbl->GetNext( tIt );
					}
				}
			}
		}
		{
			gmVariable vTriggerOnEntity = tbl->Get( _m, "TriggerOnEntity" );
			if ( vTriggerOnEntity.IsEntity() )
			{
				bHasTriggerCondition = true;
				mTriggerOnEntity[ 0 ].FromInt( vTriggerOnEntity.GetEntity() );
			}
			else
			{
				gmTableObject *tbl = vTriggerOnEntity.GetTableObjectSafe();
				if ( tbl )
				{
					gmTableIterator tIt;
					gmTableNode *pNode = tbl->GetFirst( tIt );
					while ( pNode )
					{
						if ( pNode->m_value.IsEntity() )
						{
							for ( int i = 0; i < MaxClassCount; ++i )
								if ( !mTriggerOnEntity[ i ].IsValid() )
								{
									bHasTriggerCondition = true;
									mTriggerOnEntity[ i ].FromInt( pNode->m_value.GetEntity() );
								}
						}
						pNode = tbl->GetNext( tIt );
					}
				}
			}
		}

		if ( vName.GetCStringSafe( NULL ) )
			mNameHash = Utils::MakeHash32( vName.GetCStringSafe() );
		if ( vUpdateDelay.IsNumber() )
			mUpdateDelay = Utils::SecondsToMilliseconds( vUpdateDelay.GetFloatSafe() );

		return bHasTriggerCondition;
	}

	TriggerShape()
		: mNameHash( 0 )
		, mExpireTime( 0 )
		, mNextUpdateTime( 0 )
		, mUpdateDelay( 0 )
		, mSerialNum( 0 )
		, mThisObject( gmVariable::s_null )
		, mDeleteMe( false )
	{
		static int sNextSerial = 1;

		mSerialNum = sNextSerial++;

		for ( int i = 0; i < MaxEntCount; ++i )
			mTriggerOnEntity[ i ].Reset();
		for ( int i = 0; i < MaxClassCount; ++i )
			mTriggerOnClass[ i ] = 0;
		for ( int i = 0; i < MaxInTrigger; ++i )
		{
			mInTrigger[ i ].mEntity.Reset();
			mInTrigger[ i ].mTimeStamp = 0;
		}
	}
	virtual ~TriggerShape()
	{
	}
private:
	uint32_t mNameHash;
	int		 mExpireTime;

	int		 mNextUpdateTime;
	int		 mUpdateDelay;

	int		 mSerialNum;

	GameEntity mTriggerOnEntity[ MaxEntCount ];
	int		 mTriggerOnClass[ MaxClassCount ];
	BitFlag32 mTriggerOnCategory;

	InTrigger mInTrigger[ MaxInTrigger ];

	gmGCRoot<gmFunctionObject> mOnEnterFunction;
	gmGCRoot<gmFunctionObject> mOnExitFunction;

	gmVariable				 mThisObject;

	bool	 mDeleteMe;
};
//////////////////////////////////////////////////////////////////////////
class TriggerShapeSphere : public TriggerShape
{
public:
	void UpdatePosition( const Vector3f &pos )
	{
		mPosition = pos;
	}
	bool Test( GameEntity e, const Box3f & worldObb )
	{
		return SquaredLength( worldObb.Center, mPosition ) <= Mathf::Sqr( mRadius );
	}
	void RenderDebug()
	{
		TriggerShape::RenderDebug();
		RenderBuffer::AddCircle( mPosition, mRadius, COLOR::GREEN );

		std::string name = Utils::HashToString( GetNameHash() );
		RenderBuffer::AddString3d( mPosition, COLOR::GREEN, name.c_str() );
	}
	TriggerShapeSphere( const Vector3f &p, float r )
		: mPosition( p )
		, mRadius( r )
	{
	}
private:
	Vector3f mPosition;
	float	 mRadius;
};
//////////////////////////////////////////////////////////////////////////
class TriggerShapeOBB : public TriggerShape
{
public:
	void UpdatePosition( const Vector3f &pos )
	{
		mTriggerBounds.Center = pos;
	}
	bool Test( GameEntity e, const Box3f & worldObb )
	{
		IntrBox3Box3f intr( mTriggerBounds, worldObb );
		return intr.Test();
	}
	void RenderDebug()
	{
		TriggerShape::RenderDebug();

		RenderBuffer::AddOBB( mTriggerBounds, COLOR::GREEN );

		std::string name = Utils::HashToString( GetNameHash() );
		RenderBuffer::AddString3d( mTriggerBounds.Center, COLOR::GREEN, name.c_str() );
	}
	TriggerShapeOBB( const Box3f & obb )
		: mTriggerBounds( obb )
	{
	}
private:
	Box3f		mTriggerBounds;
};

int TriggerManager::AddTrigger( const Vector3f &_pos, float _radius, gmMachine *_m, gmTableObject *tbl )
{
	ShapePtr trig( new TriggerShapeSphere( _pos, _radius ) );
	if ( trig->FromTable( _m, tbl ) )
	{
		mTriggerShapes.push_back( trig );
		return trig->GetSerial();
	}
	return 0;
}

int TriggerManager::AddTrigger( const Box3f & obb, gmMachine *_m, gmTableObject *tbl )
{
	ShapePtr trig( new TriggerShapeOBB( obb ) );
	if ( trig->FromTable( _m, tbl ) )
	{
		mTriggerShapes.push_back( trig );
		return trig->GetSerial();
	}
	return 0;
}

void TriggerManager::DeleteTrigger( int _serial )
{
	ShapeList::iterator it = mTriggerShapes.begin();
	for ( ; it != mTriggerShapes.end(); )
	{
		if ( ( *it )->GetSerial() == _serial )
		{
			//it = mTriggerShapes.erase(it);
			( *it )->SetDeleteMe(); // update handles the delete now
			return;
		}
		++it;
	}
}

void TriggerManager::DeleteTrigger( const std::string &_name )
{
	const uint32_t uiName = Utils::Hash32( _name.c_str() );
	ShapeList::iterator it = mTriggerShapes.begin();
	for ( ; it != mTriggerShapes.end(); )
	{
		if ( ( *it )->GetNameHash() == uiName )
		{
			// cs: this crashes if trigger is active
			//it = mTriggerShapes.erase(it);
			//continue;

			( *it )->SetDeleteMe(); // update handles the delete now
			//return;
		}
		++it;
	}
}

//////////////////////////////////////////////////////////////////////////

TriggerManager *TriggerManager::mInstance = 0;

TriggerManager::TriggerManager()
	: mDebugTriggers( false )
	, mDrawTriggers( false )
{
	InitCommands();
}

TriggerManager *TriggerManager::GetInstance()
{
	if ( !mInstance )
		mInstance = new TriggerManager;
	return mInstance;
}

void TriggerManager::DeleteInstance()
{
	OB_DELETE( mInstance );
}

void TriggerManager::InitCommands()
{
	SetEx( "debugtriggers", "Prints triggers to console",
		this, &TriggerManager::cmdDebugTriggers );
	SetEx( "drawtriggers", "Renders any active trigger zones",
		this, &TriggerManager::cmdDrawTriggers );

	//////////////////////////////////////////////////////////////////////////
	Options::GetValue( "Debug Render", "DrawTriggers", mDrawTriggers );
	Options::GetValue( "Debug Render", "DebugTriggers", mDebugTriggers );
	//////////////////////////////////////////////////////////////////////////
}

void TriggerManager::cmdDrawTriggers( const StringVector & args )
{
	if ( args.size() >= 2 )
	{
		if ( !mDrawTriggers && Utils::StringToTrue( args[ 1 ] ) )
			mDrawTriggers = true;
		else if ( mDrawTriggers && Utils::StringToFalse( args[ 1 ] ) )
			mDrawTriggers = false;
	}
	else
		mDrawTriggers = !mDrawTriggers;

	if ( mDrawTriggers )
		EngineFuncs::ConsoleMessage( "Trigger Drawing on." );
	else
		EngineFuncs::ConsoleMessage( "Trigger Drawing off." );
}

void TriggerManager::cmdDebugTriggers( const StringVector & args )
{
	int numArgs = (int)args.size();
	mDebugTriggersExpr = ".*";

	if ( numArgs >= 2 )
	{
		if ( !mDebugTriggers && Utils::StringToTrue( args[ 1 ] ) )
			mDebugTriggers = true;
		else if ( mDebugTriggers && Utils::StringToFalse( args[ 1 ] ) )
			mDebugTriggers = false;

		if ( numArgs >= 3 )
			mDebugTriggersExpr = va( "%s", args[ 2 ].c_str() ).c_str();
	}
	else
		mDebugTriggers = !mDebugTriggers;

	if ( mDebugTriggers )
		EngineFuncs::ConsoleMessage( "Trigger Debug on." );
	else
		EngineFuncs::ConsoleMessage( "Trigger Debug off." );
}

void TriggerManager::SetScriptCallback( const std::string &_name, gmGCRoot<gmFunctionObject> _func )
{
	mScriptCallbacks.insert( std::make_pair( _name, _func ) );
}

void TriggerManager::HandleTrigger( const TriggerInfo &_triggerInfo )
{
	bool bScriptCallback = false;

	//////////////////////////////////////////////////////////////////////////
	// Call any script callbacks.
	if ( _triggerInfo.mTagName[ 0 ] )
	{
		ScriptCallback::iterator it = mScriptCallbacks.lower_bound( _triggerInfo.mTagName ),
			itEnd = mScriptCallbacks.upper_bound( _triggerInfo.mTagName );
		gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
		DisableGCInScope gcEn( pMachine );

		for ( ; it != itEnd; ++it )
		{
			gmCall call;
			if ( call.BeginFunction( pMachine, it->second, gmVariable::s_null, true, gmThread::Highest ) )
			{
				gmUserObject *pTi = gmTriggerInfo::WrapObject( pMachine, new TriggerInfo( _triggerInfo ) );
				gmTriggerInfo::SetObjectOwnershipGM( pMachine, pTi );
				call.AddParamUser( pTi );
				call.End();
				bScriptCallback = true;
			}
		}
	}

	if ( mDebugTriggers )
	{
		if ( _triggerInfo.mTagName[ 0 ] && Utils::RegexMatch( mDebugTriggersExpr.c_str(), va( "%s", _triggerInfo.mTagName ).c_str() ) )
		{
			std::stringstream msg;
			msg << "<" << ( bScriptCallback ? "+++" : "---" ) << ">" << ( _triggerInfo );
			EngineFuncs::ConsoleMessage( msg.str().c_str() );
			LOG( msg.str().c_str() );

			Utils::OutputDebug( kInfo, msg.str().c_str() );
		}
	}
}

bool operator<( const GameEntity& _v1, const GameEntity& _v2 )
{
	return _v1.AsInt() < _v2.AsInt();
}

//////////////////////////////////////////////////////////////////////////

void TriggerManager::Update( System & system )
{
	rmt_ScopedCPUSample( TriggerManagerUpdate );

	ShapeList::iterator it = mTriggerShapes.begin();

	unsigned int x = 0;
	for ( ; x < mTriggerShapes.size(); )
	{
		if ( mDrawTriggers )
			mTriggerShapes[ x ]->RenderDebug();

		mTriggerShapes[ x ]->Update();

		if ( mTriggerShapes[ x ]->Expired() )
		{
			it = mTriggerShapes.erase( mTriggerShapes.begin() + x );
			continue;
		}

		++x;
	}
}

//////////////////////////////////////////////////////////////////////////