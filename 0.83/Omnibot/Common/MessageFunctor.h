#ifndef __MESSAGE_FUNCTOR_H__
#define __MESSAGE_FUNCTOR_H__

#include "MessageHelper.h"

// class: IMessageFunctor
class IMessageFunctor
{
public:
	virtual void operator()(const MessageHelper &_helper) = 0;
};

typedef boost::shared_ptr<IMessageFunctor> MessageFunctorPtr;

// class: MessageFunctor
class MessageFunctor : public IMessageFunctor
{
public:
	void operator()(const MessageHelper &_helper)
	{
		(m_Function)(_helper); 
	}

	MessageFunctor(pfnMessageFunction _func) :	 
	m_Function	(_func)
	{
	}
private:
	pfnMessageFunction	m_Function;
};

// class: MessageFunctorT
template<class T>
class MessageFunctorT : public IMessageFunctor
{
public:
	typedef void (T::*pfnClassMessageFunction)(const MessageHelper &_helper);

	void operator()(const MessageHelper &_helper)
	{
		(m_Object->*m_Function)(_helper); 
	}

	MessageFunctorT(T *_object, pfnClassMessageFunction _func) :
	m_Object	(_object),
		m_Function	(_func)
	{
	}	
protected:
	T						*m_Object;
	pfnClassMessageFunction	m_Function;
};

#endif
