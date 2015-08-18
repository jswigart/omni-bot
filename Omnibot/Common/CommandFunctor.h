////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __COMMAND_FUNCTOR_H__
#define __COMMAND_FUNCTOR_H__

#include "common.h"
#include "Utilities.h"
#include "gmbinder2/gmbinder2_functraits.h"

// class: ICommandFunctor
class ICommandFunctor
{
public:
	virtual bool IsObj( void * ptr ) const
	{
		return false;
	}
	virtual void operator()( const StringVector & args ) = 0;
};

typedef std::shared_ptr<ICommandFunctor> CommandFunctorPtr;

//////////////////////////////////////////////////////////////////////////

template <typename T, typename Fn>
class Delegate : public ICommandFunctor
{
public:
	virtual bool IsObj( void * ptr ) const
	{
		return mObject == ptr;
	}
	const T * GetObj() const
	{
		return mObject;
	}
	void operator()( const StringVector & args )
	{
		( mObject->*mFunction )( args );
	}
	Delegate( T *obj, Fn fnc )
		: mObject( obj )
		, mFunction( fnc )
	{
	}
private:
	T*			mObject;
	Fn			mFunction;
};

//////////////////////////////////////////////////////////////////////////

template <typename T, typename Fn>
class Delegate0 : public ICommandFunctor
{
public:
	void operator()( const StringVector & args )
	{
		( mObject->*mFunction )( );
	}
	Delegate0( T *obj, Fn fnc )
		: mObject( obj )
		, mFunction( fnc )
	{
	}
private:
	T		* mObject;
	Fn	 mFunction;
};

//////////////////////////////////////////////////////////////////////////

#endif
