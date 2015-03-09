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
#include "gmbinder2/gmbinder2_functraits.h"

// class: ICommandFunctor
class ICommandFunctor
{
public:
	virtual bool IsObj( void * ptr ) const { return false; }
	virtual void operator()(const StringVector &_args) = 0;
};

typedef boost::shared_ptr<ICommandFunctor> CommandFunctorPtr;

//////////////////////////////////////////////////////////////////////////

template <typename T, typename Fn>
class Delegate : public ICommandFunctor
{
public:
	virtual bool IsObj( void * ptr ) const { return m_Object == ptr; }
	const T * GetObj() const { return m_Object; }
	void operator()(const StringVector &_args)
	{
		(m_Object->*m_Function)(_args);
	}
	Delegate(T *obj, Fn fnc)
		: m_Object(obj)
		, m_Function(fnc)
	{
	}
private:
	T		*m_Object;
	Fn		m_Function;
};

//////////////////////////////////////////////////////////////////////////

template <typename T, typename Fn>
class Delegate0 : public ICommandFunctor
{
public:
	void operator()(const StringVector &_args)
	{
		(m_Object->*m_Function)();
	}
	Delegate0(T *obj, Fn fnc)
		: m_Object(obj)
		, m_Function(fnc)
	{
	}
private:
	T		*m_Object;
	Fn		m_Function;
};

//////////////////////////////////////////////////////////////////////////

#endif
