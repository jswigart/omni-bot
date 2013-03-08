////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy: ken.nickel $
// $LastChangedDate: 2011-08-15 23:47:44 -0500 (Mon, 15 Aug 2011) $
// $LastChangedRevision: 351 $
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __FIBERMANAGER_H__
#define __FIBERMANAGER_H__

#include <vector>

class Fiber {
public:
	enum FiberState {
		FIBER_RUNNING,
		FIBER_FINISHED,
	};

	FiberState Execute();
	void WaitFrame();
	void Wait( float timeSeconds );

	virtual void Run() = 0;

	Fiber();
	virtual ~Fiber();
private:
	FiberState		fiberState;
	void *			fiber;
	void *			lastFiber;

	static void __stdcall FiberRun( void * data );
};

//////////////////////////////////////////////////////////////////////////

//template<typename Fn, int>
//struct CallFunc {};
//
//template<typename Fn>
//struct CallFunc<Fn, 0>
//{
//	// member function, no return value
//	static void Call() {
//		(obj->*funcPtr)();
//	}
//};
//template<typename Fn>
//struct CallFunc<Fn, 1> {
//	typename FunctionTraits<Fn>::arg0_type arg0;
//
//	// member function, no return value
//	static int Call( typename FunctionTraits<Fn>::arg0_type arg0 ) {
//		(obj->*funcPtr)( _arg0 );
//	}
//	// member function, one return value
//	/*static int Call( typename FunctionTraits<Fn>::arg0_type arg0 ) {
//		typedef typename FunctionTraits<Fn>::return_type ret_type;
//		ret_type ret = (obj->*funcPtr)( _arg0 );
//	}*/
//};

//////////////////////////////////////////////////////////////////////////

//template <typename FiberClass, typename Fn>
//class FiberT : public Fiber {
//public:
//	FiberT( FiberClass * cls, Fn a_fn, typename FunctionTraits<Fn>::arg0_type arg0 )
//	{
//		this->arg0 = arg0;
//		Execute();
//	}
//	void Run() {
//		(obj->*funcPtr)(arg0);
//	}
//private:
//	FiberClass *	obj;
//	Fn				funcPtr;
//	typename FunctionTraits<Fn>::arg0_type arg0;
//};

//////////////////////////////////////////////////////////////////////////

class FiberManager {
public:

	void Init();
	void Shutdown();

	void RunFibers();

	FiberManager();
private:
	typedef std::vector<Fiber*> FiberList;
	FiberList	fibers;
};

//////////////////////////////////////////////////////////////////////////

#endif
