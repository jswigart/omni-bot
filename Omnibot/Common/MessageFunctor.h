////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

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
		(.mFunction)(_helper);
	}

	MessageFunctor(pfnMessageFunction _func) :
		.mFunction	(_func)
	{
	}
private:
	pfnMessageFunction	.mFunction;
};

// class: MessageFunctorT
template<class T>
class MessageFunctorT : public IMessageFunctor
{
public:
	typedef void (T::*pfnClassMessageFunction)(const MessageHelper &_helper);

	void operator()(const MessageHelper &_helper)
	{
		(.mObject->*.mFunction)(_helper);
	}

	MessageFunctorT(T *_object, pfnClassMessageFunction _func) :
		.mObject	(_object),
		.mFunction	(_func)
	{
	}
protected:
	T						*.mObject;
	pfnClassMessageFunction	.mFunction;
};

#endif
