
#ifndef _FUNCTRAITS_H_
#define _FUNCTRAITS_H_

//////////////////////////////////////////////////////////////////////////
// Function Traits
template<typename Function> struct FunctionTraits;

template<typename R>
struct FunctionTraits<R (*)(void)>
{
	static const int Arity = 0;
	typedef R return_type;

	static const char *PrintDeclaration()
	{
		return gmBind2::FuncInfo("%s()", 
			typeid(return_type).name());
	}
};
template<typename R, typename T0>
struct FunctionTraits<R (*)(T0)>
{
	static const int Arity = 1;
	typedef R return_type;
	typedef T0 arg0_type;

	static const char *PrintDeclaration()
	{
		return gmBind2::FuncInfo("%s(%s)", 
			typeid(return_type).name(),
			typeid(arg0_type).name());
	}
};
template<typename R, typename T0, typename T1>
struct FunctionTraits<R (*)(T0, T1)>
{
	static const int Arity = 2;
	typedef R return_type;
	typedef T0 arg0_type;
	typedef T1 arg1_type;

	static const char *PrintDeclaration()
	{
		return gmBind2::FuncInfo("%s(%s,%s)", 
			typeid(return_type).name(),
			typeid(arg0_type).name(),
			typeid(arg1_type).name());
	}
};
template<typename R, typename T0, typename T1, typename T2>
struct FunctionTraits<R (*)(T0, T1, T2)>
{
	static const int Arity = 3;
	typedef R return_type;
	typedef T0 arg0_type;
	typedef T1 arg1_type;
	typedef T2 arg2_type;

	static const char *PrintDeclaration()
	{
		return gmBind2::FuncInfo("%s(%s,%s,%s)", 
			typeid(return_type).name(),
			typeid(arg0_type).name(),
			typeid(arg1_type).name(),
			typeid(arg2_type).name());
	}
};
template<typename R, typename T0, typename T1, typename T2, typename T3>
struct FunctionTraits<R (*)(T0, T1, T2, T3)>
{
	static const int Arity = 4;
	typedef R return_type;
	typedef T0 arg0_type;
	typedef T1 arg1_type;
	typedef T2 arg2_type;
	typedef T3 arg3_type;

	static const char *PrintDeclaration()
	{
		return gmBind2::FuncInfo("%s(%s,%s,%s,%s)", 
			typeid(return_type).name(),
			typeid(arg0_type).name(),
			typeid(arg1_type).name(),
			typeid(arg2_type).name(),
			typeid(arg3_type).name());
	}
};
template<typename R, typename T0, typename T1, typename T2, typename T3, typename T4>
struct FunctionTraits<R (*)(T0, T1, T2, T3, T4)>
{
	static const int Arity = 5;
	typedef R return_type;
	typedef T0 arg0_type;
	typedef T1 arg1_type;
	typedef T2 arg2_type;
	typedef T3 arg3_type;
	typedef T4 arg4_type;

	static const char *PrintDeclaration()
	{
		return gmBind2::FuncInfo("%s(%s,%s,%s,%s,%s)", 
			typeid(return_type).name(),
			typeid(arg0_type).name(),
			typeid(arg1_type).name(),
			typeid(arg2_type).name(),
			typeid(arg3_type).name(),
			typeid(arg4_type).name());
	}
};
template<typename R, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
struct FunctionTraits<R (*)(T0, T1, T2, T3, T4, T5)>
{
	static const int Arity = 6;
	typedef R return_type;
	typedef T0 arg0_type;
	typedef T1 arg1_type;
	typedef T2 arg2_type;
	typedef T3 arg3_type;
	typedef T4 arg4_type;
	typedef T5 arg5_type;

	static const char *PrintDeclaration()
	{
		return gmBind2::FuncInfo("%s(%s,%s,%s,%s,%s,%s)", 
			typeid(return_type).name(),
			typeid(arg0_type).name(),
			typeid(arg1_type).name(),
			typeid(arg2_type).name(),
			typeid(arg3_type).name(),
			typeid(arg4_type).name(),
			typeid(arg5_type).name());
	}
};
template<typename R, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
struct FunctionTraits<R (*)(T0, T1, T2, T3, T4, T5, T6)>
{
	static const int Arity = 7;
	typedef R return_type;
	typedef T0 arg0_type;
	typedef T1 arg1_type;
	typedef T2 arg2_type;
	typedef T3 arg3_type;
	typedef T4 arg4_type;
	typedef T5 arg5_type;
	typedef T6 arg6_type;

	static const char *PrintDeclaration()
	{
		return gmBind2::FuncInfo("%s(%s,%s,%s,%s,%s,%s,%s)", 
			typeid(return_type).name(),
			typeid(arg0_type).name(),
			typeid(arg1_type).name(),
			typeid(arg2_type).name(),
			typeid(arg3_type).name(),
			typeid(arg4_type).name(),
			typeid(arg5_type).name(),
			typeid(arg6_type).name());
	}
};
template<typename R, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
struct FunctionTraits<R (*)(T0, T1, T2, T3, T4, T5, T6, T7)>
{
	static const int Arity = 8;
	typedef R return_type;
	typedef T0 arg0_type;
	typedef T1 arg1_type;
	typedef T2 arg2_type;
	typedef T3 arg3_type;
	typedef T4 arg4_type;
	typedef T5 arg5_type;
	typedef T6 arg6_type;
	typedef T7 arg7_type;

	static const char *PrintDeclaration()
	{
		return gmBind2::FuncInfo("%s(%s,%s,%s,%s,%s,%s,%s,%s)", 
			typeid(return_type).name(),
			typeid(arg0_type).name(),
			typeid(arg1_type).name(),
			typeid(arg2_type).name(),
			typeid(arg3_type).name(),
			typeid(arg4_type).name(),
			typeid(arg5_type).name(),
			typeid(arg6_type).name(),
			typeid(arg7_type).name());
	}
};
template<typename R, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
struct FunctionTraits<R (*)(T0, T1, T2, T3, T4, T5, T6, T7, T8)>
{
	static const int Arity = 9;
	typedef R return_type;
	typedef T0 arg0_type;
	typedef T1 arg1_type;
	typedef T2 arg2_type;
	typedef T3 arg3_type;
	typedef T4 arg4_type;
	typedef T5 arg5_type;
	typedef T6 arg6_type;
	typedef T7 arg7_type;
	typedef T8 arg8_type;

	static const char *PrintDeclaration()
	{
		return gmBind2::FuncInfo("%s(%s,%s,%s,%s,%s,%s,%s,%s,%s)", 
			typeid(return_type).name(),
			typeid(arg0_type).name(),
			typeid(arg1_type).name(),
			typeid(arg2_type).name(),
			typeid(arg3_type).name(),
			typeid(arg4_type).name(),
			typeid(arg5_type).name(),
			typeid(arg6_type).name(),
			typeid(arg7_type).name(),
			typeid(arg8_type).name());
	}
};
template<typename R, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
struct FunctionTraits<R (*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9)>
{
	static const int Arity = 10;
	typedef R return_type;
	typedef T0 arg0_type;
	typedef T1 arg1_type;
	typedef T2 arg2_type;
	typedef T3 arg3_type;
	typedef T4 arg4_type;
	typedef T5 arg5_type;
	typedef T6 arg6_type;
	typedef T7 arg7_type;
	typedef T8 arg8_type;
	typedef T9 arg9_type;

	static const char *PrintDeclaration()
	{
		return gmBind2::FuncInfo("%s(%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", 
			typeid(return_type).name(),
			typeid(arg0_type).name(),
			typeid(arg1_type).name(),
			typeid(arg2_type).name(),
			typeid(arg3_type).name(),
			typeid(arg4_type).name(),
			typeid(arg5_type).name(),
			typeid(arg6_type).name(),
			typeid(arg7_type).name(),
			typeid(arg8_type).name(),
			typeid(arg9_type).name());
	}
};
template<typename R, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
struct FunctionTraits<R (*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10)>
{
	static const int Arity = 11;
	typedef R return_type;
	typedef T0 arg0_type;
	typedef T1 arg1_type;
	typedef T2 arg2_type;
	typedef T3 arg3_type;
	typedef T4 arg4_type;
	typedef T5 arg5_type;
	typedef T6 arg6_type;
	typedef T7 arg7_type;
	typedef T8 arg8_type;
	typedef T9 arg9_type;
	typedef T10 arg10_type;

	static const char *PrintDeclaration()
	{
		return gmBind2::FuncInfo("%s(%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", 
			typeid(return_type).name(),
			typeid(arg0_type).name(),
			typeid(arg1_type).name(),
			typeid(arg2_type).name(),
			typeid(arg3_type).name(),
			typeid(arg4_type).name(),
			typeid(arg5_type).name(),
			typeid(arg6_type).name(),
			typeid(arg7_type).name(),
			typeid(arg8_type).name(),
			typeid(arg9_type).name(),
			typeid(arg10_type).name());
	}
};

// -------------------------------------------------------------------------------------
// Member Function Traits Helpers
template<typename T, typename R>
struct FunctionTraits<R (T::*)(void)>
{
	static const int Arity = 0;
	typedef T Class_Type;
	typedef R return_type;
	typedef void arg0_type;

	static const char *PrintDeclaration()
	{
		return gmBind2::FuncInfo("%s()", 
			typeid(return_type).name());
	}
};

template<typename T, typename R, typename T0>
struct FunctionTraits<R (T::*)(T0)>
{
	static const int Arity = 1;
	typedef T Class_Type;
	typedef R return_type;
	typedef T0 arg0_type;

	static const char *PrintDeclaration()
	{
		return gmBind2::FuncInfo("%s(%s)", 
			typeid(return_type).name(),
			typeid(arg0_type).name());
	}
};

template<typename T, typename R, typename T0, typename T1>
struct FunctionTraits<R (T::*)(T0, T1)>
{
	static const int Arity = 2;
	typedef T Class_Type;
	typedef R return_type;
	typedef T0 arg0_type;
	typedef T1 arg1_type;

	static const char *PrintDeclaration()
	{
		return gmBind2::FuncInfo("%s(%s,%s)", 
			typeid(return_type).name(),
			typeid(arg0_type).name(),
			typeid(arg1_type).name());
	}
};

template<typename T, typename R, typename T0, typename T1, typename T2>
struct FunctionTraits<R (T::*)(T0, T1, T2)>
{
	static const int Arity = 3;
	typedef T Class_Type;
	typedef R return_type;
	typedef T0 arg0_type;
	typedef T1 arg1_type;
	typedef T2 arg2_type;

	static const char *PrintDeclaration()
	{
		return gmBind2::FuncInfo("%s(%s,%s,%s)", 
			typeid(return_type).name(),
			typeid(arg0_type).name(),
			typeid(arg1_type).name(),
			typeid(arg2_type).name());
	}
};

template<typename T, typename R, typename T0, typename T1, typename T2, typename T3>
struct FunctionTraits<R (T::*)(T0, T1, T2, T3)>
{
	static const int Arity = 4;
	typedef T Class_Type;
	typedef R return_type;
	typedef T0 arg0_type;
	typedef T1 arg1_type;
	typedef T2 arg2_type;
	typedef T3 arg3_type;

	static const char *PrintDeclaration()
	{
		return gmBind2::FuncInfo("%s(%s,%s,%s,%s)", 
			typeid(return_type).name(),
			typeid(arg0_type).name(),
			typeid(arg1_type).name(),
			typeid(arg2_type).name(),
			typeid(arg3_type).name());
	}
};

template<typename T, typename R, typename T0, typename T1, typename T2, typename T3, typename T4>
struct FunctionTraits<R (T::*)(T0, T1, T2, T3, T4)>
{
	static const int Arity = 5;
	typedef T Class_Type;
	typedef R return_type;
	typedef T0 arg0_type;
	typedef T1 arg1_type;
	typedef T2 arg2_type;
	typedef T3 arg3_type;
	typedef T4 arg4_type;

	static const char *PrintDeclaration()
	{
		return gmBind2::FuncInfo("%s(%s,%s,%s,%s,%s)", 
			typeid(return_type).name(),
			typeid(arg0_type).name(),
			typeid(arg1_type).name(),
			typeid(arg2_type).name(),
			typeid(arg3_type).name(),
			typeid(arg4_type).name());
	}
};

template<typename T, typename R, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
struct FunctionTraits<R (T::*)(T0, T1, T2, T3, T4, T5)>
{
	static const int Arity = 6;
	typedef T Class_Type;
	typedef R return_type;
	typedef T0 arg0_type;
	typedef T1 arg1_type;
	typedef T2 arg2_type;
	typedef T3 arg3_type;
	typedef T4 arg4_type;
	typedef T5 arg5_type;

	static const char *PrintDeclaration()
	{
		return gmBind2::FuncInfo("%s(%s,%s,%s,%s,%s,%s)", 
			typeid(return_type).name(),
			typeid(arg0_type).name(),
			typeid(arg1_type).name(),
			typeid(arg2_type).name(),
			typeid(arg3_type).name(),
			typeid(arg4_type).name(),
			typeid(arg5_type).name());
	}
};

// -------------------------------------------------------------------------------------
// const Member Function Traits Helpers
template<typename T, typename R>
struct FunctionTraits<R (T::*)(void) const>
{
	static const int Arity = 0;
	typedef T Class_Type;
	typedef R return_type;
	typedef void arg0_type;

	static const char *PrintDeclaration()
	{
		return gmBind2::FuncInfo("%s()", 
			typeid(return_type).name());
	}
};

template<typename T, typename R, typename T0>
struct FunctionTraits<R (T::*)(T0) const>
{
	static const int Arity = 1;
	typedef T Class_Type;
	typedef R return_type;
	typedef T0 arg0_type;

	static const char *PrintDeclaration()
	{
		return gmBind2::FuncInfo("%s(%s)", 
			typeid(return_type).name(),
			typeid(arg0_type).name());
	}
};

template<typename T, typename R, typename T0, typename T1>
struct FunctionTraits<R (T::*)(T0, T1) const>
{
	static const int Arity = 2;
	typedef T Class_Type;
	typedef R return_type;
	typedef T0 arg0_type;
	typedef T1 arg1_type;

	static const char *PrintDeclaration()
	{
		return gmBind2::FuncInfo("%s(%s,%s)", 
			typeid(return_type).name(),
			typeid(arg0_type).name(),
			typeid(arg1_type).name());
	}
};

template<typename T, typename R, typename T0, typename T1, typename T2>
struct FunctionTraits<R (T::*)(T0, T1, T2) const>
{
	static const int Arity = 3;
	typedef T Class_Type;
	typedef R return_type;
	typedef T0 arg0_type;
	typedef T1 arg1_type;
	typedef T2 arg2_type;

	static const char *PrintDeclaration()
	{
		return gmBind2::FuncInfo("%s(%s,%s,%s)", 
			typeid(return_type).name(),
			typeid(arg0_type).name(),
			typeid(arg1_type).name(),
			typeid(arg2_type).name());
	}
};

template<typename T, typename R, typename T0, typename T1, typename T2, typename T3>
struct FunctionTraits<R (T::*)(T0, T1, T2, T3) const>
{
	static const int Arity = 4;
	typedef T Class_Type;
	typedef R return_type;
	typedef T0 arg0_type;
	typedef T1 arg1_type;
	typedef T2 arg2_type;
	typedef T3 arg3_type;

	static const char *PrintDeclaration()
	{
		return gmBind2::FuncInfo("%s(%s,%s,%s,%s)", 
			typeid(return_type).name(),
			typeid(arg0_type).name(),
			typeid(arg1_type).name(),
			typeid(arg2_type).name(),
			typeid(arg3_type).name());
	}
};

template<typename T, typename R, typename T0, typename T1, typename T2, typename T3, typename T4>
struct FunctionTraits<R (T::*)(T0, T1, T2, T3, T4) const>
{
	static const int Arity = 5;
	typedef T Class_Type;
	typedef R return_type;
	typedef T0 arg0_type;
	typedef T1 arg1_type;
	typedef T2 arg2_type;
	typedef T3 arg3_type;
	typedef T4 arg4_type;

	static const char *PrintDeclaration()
	{
		return gmBind2::FuncInfo("%s(%s,%s,%s,%s,%s)", 
			typeid(return_type).name(),
			typeid(arg0_type).name(),
			typeid(arg1_type).name(),
			typeid(arg2_type).name(),
			typeid(arg3_type).name(),
			typeid(arg4_type).name());
	}
};

template<typename T, typename R, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
struct FunctionTraits<R (T::*)(T0, T1, T2, T3, T4, T5) const>
{
	static const int Arity = 6;
	typedef T Class_Type;
	typedef R return_type;
	typedef T0 arg0_type;
	typedef T1 arg1_type;
	typedef T2 arg2_type;
	typedef T3 arg3_type;
	typedef T4 arg4_type;
	typedef T5 arg5_type;

	static const char *PrintDeclaration()
	{
		return gmBind2::FuncInfo("%s(%s,%s,%s,%s,%s,%s)", 
			typeid(return_type).name(),
			typeid(arg0_type).name(),
			typeid(arg1_type).name(),
			typeid(arg2_type).name(),
			typeid(arg3_type).name(),
			typeid(arg4_type).name(),
			typeid(arg5_type).name());
	}
};

// Extra Utility stuff
namespace Meta
{
	template< bool B_ > 
	struct BoolToType
	{
		static const bool value = B_;
		typedef BoolToType type;
		typedef bool value_type;
		operator bool() const { return this->value; }
	};

	template <class A, class B> struct IsSame { enum { value = false }; };
	template <class A> struct IsSame< A, A > { enum { value = true }; };

	template <class T> struct IsReference { enum { value = false }; };
	template <class T> struct IsReference<T&> { enum { value = true }; };

	template <class T> struct Strip { typedef T type; };
	template <class T> struct Strip<T&> { typedef T type; };
	template <class T> struct Strip<const T&> { typedef T type; };
	template <class T> struct Strip<T*> { typedef T type; };
	template <class T> struct Strip<const T*> { typedef T type; };

	template <class T> struct Argify { typedef T type; };
	template <class T> struct Argify<T&> { typedef T type; };
	template <class T> struct Argify<const T&> { typedef T type; };
	template <class T> struct Argify<T*> { typedef T* type; };
	template <class T> struct Argify<const T*> { typedef T* type; };

	// -------------------------------------------------------------------------------------
	// IsMemberFunction stuff
	template <class T> struct IsMemberFunction { enum { value = false }; };
	template <class T, class R																		> struct IsMemberFunction <R(T::*)(							)	> { enum { value = true }; };
	template <class T, class R, class P0															> struct IsMemberFunction <R(T::*)(P0						)	> { enum { value = true }; };
	template <class T, class R, class P0, class P1													> struct IsMemberFunction <R(T::*)(P0, P1					)	> { enum { value = true }; };
	template <class T, class R, class P0, class P1, class P2										> struct IsMemberFunction <R(T::*)(P0, P1, P2				)	> { enum { value = true }; };
	template <class T, class R, class P0, class P1, class P2, class P3								> struct IsMemberFunction <R(T::*)(P0, P1, P2, P3			)	> { enum { value = true }; };
	template <class T, class R, class P0, class P1, class P2, class P3, class P4					> struct IsMemberFunction <R(T::*)(P0, P1, P2, P3, P4		)	> { enum { value = true }; };
	template <class T, class R, class P0, class P1, class P2, class P3, class P4, class P5			> struct IsMemberFunction <R(T::*)(P0, P1, P2, P3, P4, P5	)	> { enum { value = true }; };
	template <class T, class R, class P0, class P1, class P2, class P3, class P4, class P5, class P6> struct IsMemberFunction <R(T::*)(P0, P1, P2, P3, P4, P5, P6)	> { enum { value = true }; };

	// const member functions
	template <class T, class R																		> struct IsMemberFunction <R(T::*)(							)	const > { enum { value = true }; };
	template <class T, class R, class P0															> struct IsMemberFunction <R(T::*)(P0						)	const > { enum { value = true }; };
	template <class T, class R, class P0, class P1													> struct IsMemberFunction <R(T::*)(P0, P1					)	const > { enum { value = true }; };
	template <class T, class R, class P0, class P1, class P2										> struct IsMemberFunction <R(T::*)(P0, P1, P2				)	const > { enum { value = true }; };
	template <class T, class R, class P0, class P1, class P2, class P3								> struct IsMemberFunction <R(T::*)(P0, P1, P2, P3			)	const > { enum { value = true }; };
	template <class T, class R, class P0, class P1, class P2, class P3, class P4					> struct IsMemberFunction <R(T::*)(P0, P1, P2, P3, P4		)	const > { enum { value = true }; };
	template <class T, class R, class P0, class P1, class P2, class P3, class P4, class P5			> struct IsMemberFunction <R(T::*)(P0, P1, P2, P3, P4, P5	)	const > { enum { value = true }; };
	template <class T, class R, class P0, class P1, class P2, class P3, class P4, class P5, class P6> struct IsMemberFunction <R(T::*)(P0, P1, P2, P3, P4, P5, P6)	const > { enum { value = true }; };

	//template<typename Fn, int>
	//struct GMExportStruct {};

	//template<typename Fn>
	//struct GMExportStruct<Fn, 0>
	//{
	//	//////////////////////////////////////////////////////////////////////////
	//	static int Call(gmThread *a_thread, Fn fn, Meta::BoolToType<true>, Meta::BoolToType<false>)
	//	{
	//		GM_CHECK_NUM_PARAMS(FunctionTraits<Fn>::Arity);
	//		fn();
	//		return GM_OK;
	//	}
	//	//////////////////////////////////////////////////////////////////////////
	//	static int Call(gmThread *a_thread, Fn fn, Meta::BoolToType<false>, Meta::BoolToType<false>)
	//	{
	//		GM_CHECK_NUM_PARAMS(FunctionTraits<Fn>::Arity);
	//		typedef typename FunctionTraits<Fn>::return_type ret_type;
	//		ret_type ret = fn();
	//		return PushReturnToGM(a_thread, ret);
	//	}
	//	//////////////////////////////////////////////////////////////////////////
	//	// member function, no return value
	//	static int Call(gmThread *a_thread, Fn fn, Meta::BoolToType<true>, Meta::BoolToType<true>)
	//	{
	//		GM_CHECK_NUM_PARAMS(FunctionTraits<Fn>::Arity);
	//		CHECKTHIS;
	//		(obj->*fn)();
	//		return GM_OK;
	//	}
	//	//////////////////////////////////////////////////////////////////////////
	//	// member function, one return value
	//	static int Call(gmThread *a_thread, Fn fn, Meta::BoolToType<false>, Meta::BoolToType<true>)
	//	{
	//		GM_CHECK_NUM_PARAMS(FunctionTraits<Fn>::Arity);
	//		CHECKTHIS;
	//		typedef typename FunctionTraits<Fn>::return_type ret_type;
	//		ret_type ret = (obj->*fn)();
	//		return PushReturnToGM(a_thread, ret);
	//	}
	//};
	//template<typename Fn>
	//struct GMExportStruct<Fn, 1>
	//{
	//	//////////////////////////////////////////////////////////////////////////
	//	static int Call(gmThread *a_thread, Fn fn, Meta::BoolToType<true>, Meta::BoolToType<false>)
	//	{
	//		GM_CHECK_NUM_PARAMS(FunctionTraits<Fn>::Arity);
	//		CHECKTYPE_ARG(0);
	//		fn(arg0);
	//		return GM_OK;
	//	}
	//	//////////////////////////////////////////////////////////////////////////
	//	static int Call(gmThread *a_thread, Fn fn, Meta::BoolToType<false>, Meta::BoolToType<false>)
	//	{
	//		GM_CHECK_NUM_PARAMS(FunctionTraits<Fn>::Arity);
	//		CHECKTYPE_ARG(0);
	//		typedef typename FunctionTraits<Fn>::return_type ret_type;
	//		ret_type ret = fn(arg0);
	//		return PushReturnToGM(a_thread, ret);
	//	}
	//	//////////////////////////////////////////////////////////////////////////
	//	// member function, no return value
	//	static int Call(gmThread *a_thread, Fn fn, Meta::BoolToType<true>, Meta::BoolToType<true>)
	//	{
	//		GM_CHECK_NUM_PARAMS(FunctionTraits<Fn>::Arity);
	//		CHECKTHIS;
	//		CHECKTYPE_ARG(0);
	//		(obj->*fn)(arg0);
	//		return GM_OK;
	//	}
	//	//////////////////////////////////////////////////////////////////////////
	//	// member function, one return value
	//	static int Call(gmThread *a_thread, Fn fn, Meta::BoolToType<false>, Meta::BoolToType<true>)
	//	{
	//		GM_CHECK_NUM_PARAMS(FunctionTraits<Fn>::Arity);
	//		CHECKTHIS;
	//		CHECKTYPE_ARG(0);
	//		typedef typename FunctionTraits<Fn>::return_type ret_type;
	//		ret_type ret = (obj->*fn)(arg0);
	//		return PushReturnToGM(a_thread, ret);
	//	}
	//};
	//template<typename Fn>
	//struct GMExportStruct<Fn, 2>
	//{
	//	//////////////////////////////////////////////////////////////////////////
	//	static int Call(gmThread *a_thread, Fn fn, Meta::BoolToType<true>, Meta::BoolToType<false>)
	//	{
	//		GM_CHECK_NUM_PARAMS(FunctionTraits<Fn>::Arity);
	//		CHECKTYPE_ARG(0);
	//		CHECKTYPE_ARG(1);
	//		fn(arg0, arg1);
	//		return GM_OK;
	//	}
	//	//////////////////////////////////////////////////////////////////////////
	//	static int Call(gmThread *a_thread, Fn fn, Meta::BoolToType<false>, Meta::BoolToType<false>)
	//	{
	//		GM_CHECK_NUM_PARAMS(FunctionTraits<Fn>::Arity);
	//		CHECKTYPE_ARG(0);
	//		CHECKTYPE_ARG(1);
	//		typedef typename FunctionTraits<Fn>::return_type ret_type;
	//		ret_type ret = fn(arg0, arg1);
	//		return PushReturnToGM(a_thread, ret);
	//	}
	//	//////////////////////////////////////////////////////////////////////////
	//	// member function, no return value
	//	static int Call(gmThread *a_thread, Fn fn, Meta::BoolToType<true>, Meta::BoolToType<true>)
	//	{
	//		GM_CHECK_NUM_PARAMS(FunctionTraits<Fn>::Arity);
	//		CHECKTHIS;
	//		CHECKTYPE_ARG(0);
	//		CHECKTYPE_ARG(1);
	//		(obj->*fn)(arg0, arg1);
	//		return GM_OK;
	//	}
	//	//////////////////////////////////////////////////////////////////////////
	//	// member function, one return value
	//	static int Call(gmThread *a_thread, Fn fn, Meta::BoolToType<false>, Meta::BoolToType<true>)
	//	{
	//		GM_CHECK_NUM_PARAMS(FunctionTraits<Fn>::Arity);
	//		CHECKTHIS;
	//		CHECKTYPE_ARG(0);
	//		CHECKTYPE_ARG(1);
	//		typedef typename FunctionTraits<Fn>::return_type ret_type;
	//		ret_type ret = (obj->*fn)(arg0, arg1);
	//		return PushReturnToGM(a_thread, ret);
	//	}
	//};
	//template<typename Fn>
	//struct GMExportStruct<Fn, 3>
	//{
	//	//////////////////////////////////////////////////////////////////////////
	//	static int Call(gmThread *a_thread, Fn fn, Meta::BoolToType<true>, Meta::BoolToType<false>)
	//	{
	//		GM_CHECK_NUM_PARAMS(FunctionTraits<Fn>::Arity);
	//		CHECKTYPE_ARG(0);
	//		CHECKTYPE_ARG(1);
	//		CHECKTYPE_ARG(2);
	//		fn(arg0, arg1, arg2);
	//		return GM_OK;
	//	}
	//	//////////////////////////////////////////////////////////////////////////
	//	static int Call(gmThread *a_thread, Fn fn, Meta::BoolToType<false>, Meta::BoolToType<false>)
	//	{
	//		GM_CHECK_NUM_PARAMS(FunctionTraits<Fn>::Arity);
	//		CHECKTYPE_ARG(0);
	//		CHECKTYPE_ARG(1);
	//		CHECKTYPE_ARG(2);
	//		typedef typename FunctionTraits<Fn>::return_type ret_type;
	//		ret_type ret = fn(arg0, arg1, arg2);
	//		return PushReturnToGM(a_thread, ret);
	//	}
	//	//////////////////////////////////////////////////////////////////////////
	//	// member function, no return value
	//	static int Call(gmThread *a_thread, Fn fn, Meta::BoolToType<true>, Meta::BoolToType<true>)
	//	{
	//		GM_CHECK_NUM_PARAMS(FunctionTraits<Fn>::Arity);
	//		CHECKTHIS;
	//		CHECKTYPE_ARG(0);
	//		CHECKTYPE_ARG(1);
	//		CHECKTYPE_ARG(2);
	//		(obj->*fn)(arg0, arg1, arg2);
	//		return GM_OK;
	//	}
	//	//////////////////////////////////////////////////////////////////////////
	//	// member function, one return value
	//	static int Call(gmThread *a_thread, Fn fn, Meta::BoolToType<false>, Meta::BoolToType<true>)
	//	{
	//		GM_CHECK_NUM_PARAMS(FunctionTraits<Fn>::Arity);
	//		CHECKTHIS;
	//		CHECKTYPE_ARG(0);
	//		CHECKTYPE_ARG(1);
	//		CHECKTYPE_ARG(2);
	//		typedef typename FunctionTraits<Fn>::return_type ret_type;
	//		ret_type ret = (obj->*fn)(arg0, arg1, arg2);
	//		return PushReturnToGM(a_thread, ret);
	//	}
	//};
	//template<typename Fn>
	//struct GMExportStruct<Fn, 4>
	//{
	//	//////////////////////////////////////////////////////////////////////////
	//	static int Call(gmThread *a_thread, Fn fn, Meta::BoolToType<true>, Meta::BoolToType<false>)
	//	{
	//		GM_CHECK_NUM_PARAMS(FunctionTraits<Fn>::Arity);
	//		CHECKTYPE_ARG(0);
	//		CHECKTYPE_ARG(1);
	//		CHECKTYPE_ARG(2);
	//		CHECKTYPE_ARG(3);
	//		fn(arg0, arg1, arg2, arg3);
	//		return GM_OK;
	//	}
	//	//////////////////////////////////////////////////////////////////////////
	//	static int Call(gmThread *a_thread, Fn fn, Meta::BoolToType<false>, Meta::BoolToType<false>)
	//	{
	//		GM_CHECK_NUM_PARAMS(FunctionTraits<Fn>::Arity);
	//		CHECKTYPE_ARG(0);
	//		CHECKTYPE_ARG(1);
	//		CHECKTYPE_ARG(2);
	//		CHECKTYPE_ARG(3);
	//		typedef typename FunctionTraits<Fn>::return_type ret_type;
	//		ret_type ret = fn(arg0, arg1, arg2, arg3);
	//		return PushReturnToGM(a_thread, ret);
	//	}
	//	//////////////////////////////////////////////////////////////////////////
	//	// member function, no return value
	//	static int Call(gmThread *a_thread, Fn fn, Meta::BoolToType<true>, Meta::BoolToType<true>)
	//	{
	//		GM_CHECK_NUM_PARAMS(FunctionTraits<Fn>::Arity);
	//		CHECKTHIS;
	//		CHECKTYPE_ARG(0);
	//		CHECKTYPE_ARG(1);
	//		CHECKTYPE_ARG(2);
	//		CHECKTYPE_ARG(3);
	//		(obj->*fn)(arg0, arg1, arg2, arg3);
	//		return GM_OK;
	//	}
	//	//////////////////////////////////////////////////////////////////////////
	//	// member function, one return value
	//	static int Call(gmThread *a_thread, Fn fn, Meta::BoolToType<false>, Meta::BoolToType<true>)
	//	{
	//		GM_CHECK_NUM_PARAMS(FunctionTraits<Fn>::Arity);
	//		CHECKTHIS;
	//		CHECKTYPE_ARG(0);
	//		CHECKTYPE_ARG(1);
	//		CHECKTYPE_ARG(2);
	//		CHECKTYPE_ARG(3);
	//		typedef typename FunctionTraits<Fn>::return_type ret_type;
	//		ret_type ret = (obj->*fn)(arg0, arg1, arg2, arg3);
	//		return PushReturnToGM(a_thread, ret);
	//	}
	//};
	//template<typename Fn>
	//struct GMExportStruct<Fn, 5>
	//{
	//	//////////////////////////////////////////////////////////////////////////
	//	static int Call(gmThread *a_thread, Fn fn, Meta::BoolToType<true>, Meta::BoolToType<false>)
	//	{
	//		GM_CHECK_NUM_PARAMS(FunctionTraits<Fn>::Arity);
	//		CHECKTYPE_ARG(0);
	//		CHECKTYPE_ARG(1);
	//		CHECKTYPE_ARG(2);
	//		CHECKTYPE_ARG(3);
	//		CHECKTYPE_ARG(4);
	//		fn(arg0, arg1, arg2, arg3, arg4);
	//		return GM_OK;
	//	}
	//	//////////////////////////////////////////////////////////////////////////
	//	static int Call(gmThread *a_thread, Fn fn, Meta::BoolToType<false>, Meta::BoolToType<false>)
	//	{
	//		GM_CHECK_NUM_PARAMS(FunctionTraits<Fn>::Arity);
	//		CHECKTYPE_ARG(0);
	//		CHECKTYPE_ARG(1);
	//		CHECKTYPE_ARG(2);
	//		CHECKTYPE_ARG(3);
	//		CHECKTYPE_ARG(4);
	//		typedef typename FunctionTraits<Fn>::return_type ret_type;
	//		ret_type ret = fn(arg0, arg1, arg2, arg3, arg4);
	//		return PushReturnToGM(a_thread, ret);
	//	}
	//	//////////////////////////////////////////////////////////////////////////
	//	// member function, no return value
	//	static int Call(gmThread *a_thread, Fn fn, Meta::BoolToType<true>, Meta::BoolToType<true>)
	//	{
	//		GM_CHECK_NUM_PARAMS(FunctionTraits<Fn>::Arity);
	//		CHECKTHIS;
	//		CHECKTYPE_ARG(0);
	//		CHECKTYPE_ARG(1);
	//		CHECKTYPE_ARG(2);
	//		CHECKTYPE_ARG(3);
	//		CHECKTYPE_ARG(4);
	//		(obj->*fn)(arg0, arg1, arg2, arg3, arg4);
	//		return GM_OK;
	//	}
	//	//////////////////////////////////////////////////////////////////////////
	//	// member function, one return value
	//	static int Call(gmThread *a_thread, Fn fn, Meta::BoolToType<false>, Meta::BoolToType<true>)
	//	{
	//		GM_CHECK_NUM_PARAMS(FunctionTraits<Fn>::Arity);
	//		CHECKTHIS;
	//		CHECKTYPE_ARG(0);
	//		CHECKTYPE_ARG(1);
	//		CHECKTYPE_ARG(2);
	//		CHECKTYPE_ARG(3);
	//		CHECKTYPE_ARG(4);
	//		typedef typename FunctionTraits<Fn>::return_type ret_type;
	//		ret_type ret = (obj->*fn)(arg0, arg1, arg2, arg3, arg4);
	//		return PushReturnToGM(a_thread, ret);
	//	}
	//};
};

#endif
