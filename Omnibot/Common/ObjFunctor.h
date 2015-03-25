////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __OBJFUNCTOR_H__
#define __OBJFUNCTOR_H__

#include "Functor.h"

// class: ObjFunctor
template<class T>
class ObjFunctor : public Functor
{
public:
	typedef int (T::*funcType)();
	ObjFunctor(T *o, funcType f)
	{
		obj = o;
		mFunction = f;
	}
	int operator ()()
	{
		return (obj->*mFunction)();
	}
protected:
	T *obj;

	funcType mFunction;
};

#endif
