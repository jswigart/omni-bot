#ifndef __COMMAND_FUNCTOR_H__
#define __COMMAND_FUNCTOR_H__

#include "gmbinder2_functraits.h"

// class: ICommandFunctor
class ICommandFunctor
{
public:
	virtual void operator()(const StringVector &_args) = 0;
};

typedef boost::shared_ptr<ICommandFunctor> CommandFunctorPtr;

// class: CommandFunctorT
template<class T>
class CommandFunctorT : public ICommandFunctor
{
public:
	typedef void (T::*pfnClassMessageFunction)(const StringVector &_args);

	void operator()(const StringVector &_args)
	{
		(m_Object->*m_Function)(_args); 
	}

	CommandFunctorT(T *_object, pfnClassMessageFunction _func) :
		m_Object	(_object),
		m_Function	(_func)
	{
	}	
protected:
	T						*m_Object;
	pfnClassMessageFunction	m_Function;
};

//////////////////////////////////////////////////////////////////////////

template <typename T, typename Fn>
class Delegate : public ICommandFunctor
{
public:
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
