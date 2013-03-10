// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2012/07/29)

#ifndef WM5GLUTILITY_H
#define WM5GLUTILITY_H

#include "Wm5GlExtensions.h"
#include "Wm5GlPlugin.h"

#ifdef WIN32
// Microsoft Windows
#define STDCALL __stdcall
#define EXTERN __declspec(dllimport)
#else
// Macintosh, Linux
#define STDCALL
#define EXTERN extern
#endif

// If a function pointer is null, you can override the pointer with an ARB or
// EXT function pointer.  Set each to 1 to override, to 0 not to override.
#define GT_ARB_OVERRIDE  1
#define GT_EXT_OVERRIDE  1
#define GT_ATI_OVERRIDE  0
#define GT_HP_OVERRIDE   0
#define GT_IBM_OVERRIDE  0
#define GT_INGR_OVERRIDE 0
#define GT_MESA_OVERRIDE 0
#define GT_NV_OVERRIDE   0
#define GT_SGI_OVERRIDE  1
#define GT_SGIS_OVERRIDE 1
#define GT_SUN_OVERRIDE  0

//----------------------------------------------------------------------------
#define GT_GET_FUNCTION(glFunction)\
{ \
    const char* acString = #glFunction; \
    glFunction = \
        (P##glFunction)GTGetFunctionPointer(&acString[1]); \
}
//----------------------------------------------------------------------------
#define GT_GET_FUNCTION_C(glFunction,suffix,allowOverride)\
{ \
    const char* acString = #glFunction#suffix; \
    glFunction##suffix = \
        (P##glFunction)GTGetFunctionPointer(&acString[1]); \
    if (allowOverride) \
    { \
        if (!glFunction) \
        { \
            glFunction = glFunction##suffix; \
        } \
    } \
}
//----------------------------------------------------------------------------
#define GT_GET_FUNCTION_CA(glFunction,altglFunction,suffix,allowOverride)\
{ \
    const char* acString = #glFunction#suffix; \
    glFunction##suffix = \
        (P##glFunction)GTGetFunctionPointer(&acString[1]); \
    if (allowOverride) \
    { \
        if (!altglFunction) \
        { \
            altglFunction = glFunction##suffix; \
        } \
    } \
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Macros for bodies of preexisting functions with no return value.
//----------------------------------------------------------------------------
#define GT_PRE_BODY_A0(f)\
GT_ENTER_FUNCTION(f); \
GT_NONNULL_FUNCTION(f); \
f(); \
GT_EXIT_FUNCTION(f)
//----------------------------------------------------------------------------
#define GT_PRE_BODY_A1(f,a0)\
GT_ENTER_FUNCTION(f); \
GT_NONNULL_FUNCTION(f); \
f(a0); \
GT_EXIT_FUNCTION(f)
//----------------------------------------------------------------------------
#define GT_PRE_BODY_A2(f,a0,a1)\
GT_ENTER_FUNCTION(f); \
GT_NONNULL_FUNCTION(f); \
f(a0,a1); \
GT_EXIT_FUNCTION(f)
//----------------------------------------------------------------------------
#define GT_PRE_BODY_A3(f,a0,a1,a2)\
GT_ENTER_FUNCTION(f); \
GT_NONNULL_FUNCTION(f); \
f(a0,a1,a2); \
GT_EXIT_FUNCTION(f)
//----------------------------------------------------------------------------
#define GT_PRE_BODY_A4(f,a0,a1,a2,a3)\
GT_ENTER_FUNCTION(f); \
GT_NONNULL_FUNCTION(f); \
f(a0,a1,a2,a3); \
GT_EXIT_FUNCTION(f)
//----------------------------------------------------------------------------
#define GT_PRE_BODY_A5(f,a0,a1,a2,a3,a4)\
GT_ENTER_FUNCTION(f); \
GT_NONNULL_FUNCTION(f); \
f(a0,a1,a2,a3,a4); \
GT_EXIT_FUNCTION(f)
//----------------------------------------------------------------------------
#define GT_PRE_BODY_A6(f,a0,a1,a2,a3,a4,a5)\
GT_ENTER_FUNCTION(f); \
GT_NONNULL_FUNCTION(f); \
f(a0,a1,a2,a3,a4,a5); \
GT_EXIT_FUNCTION(f)
//----------------------------------------------------------------------------
#define GT_PRE_BODY_A7(f,a0,a1,a2,a3,a4,a5,a6)\
GT_ENTER_FUNCTION(f); \
GT_NONNULL_FUNCTION(f); \
f(a0,a1,a2,a3,a4,a5,a6); \
GT_EXIT_FUNCTION(f)
//----------------------------------------------------------------------------
#define GT_PRE_BODY_A8(f,a0,a1,a2,a3,a4,a5,a6,a7)\
GT_ENTER_FUNCTION(f); \
GT_NONNULL_FUNCTION(f); \
f(a0,a1,a2,a3,a4,a5,a6,a7); \
GT_EXIT_FUNCTION(f)
//----------------------------------------------------------------------------
#define GT_PRE_BODY_A9(f,a0,a1,a2,a3,a4,a5,a6,a7,a8)\
GT_ENTER_FUNCTION(f); \
GT_NONNULL_FUNCTION(f); \
f(a0,a1,a2,a3,a4,a5,a6,a7,a8); \
GT_EXIT_FUNCTION(f)
//----------------------------------------------------------------------------
#define GT_PRE_BODY_A10(f,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9)\
GT_ENTER_FUNCTION(f); \
GT_NONNULL_FUNCTION(f); \
f(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9); \
GT_EXIT_FUNCTION(f)
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Macros for bodies of preexisting functions with a return value.
//----------------------------------------------------------------------------
#define GT_PRE_BODY_A0_RET(f,type)\
type tResult; \
GT_ENTER_FUNCTION(f); \
GT_NONNULL_FUNCTION(f); \
tResult = f(); \
GT_EXIT_FUNCTION(f); \
return tResult
//----------------------------------------------------------------------------
#define GT_PRE_BODY_A1_RET(f,a0,type)\
type tResult; \
GT_ENTER_FUNCTION(f); \
GT_NONNULL_FUNCTION(f); \
tResult = f(a0); \
GT_EXIT_FUNCTION(f); \
return tResult
//----------------------------------------------------------------------------
#define GT_PRE_BODY_A2_RET(f,a0,a1,type)\
type tResult; \
GT_ENTER_FUNCTION(f); \
GT_NONNULL_FUNCTION(f); \
tResult = f(a0,a1); \
GT_EXIT_FUNCTION(f); \
return tResult
//----------------------------------------------------------------------------
#define GT_PRE_BODY_A3_RET(f,a0,a1,a2,type)\
type tResult; \
GT_ENTER_FUNCTION(f); \
GT_NONNULL_FUNCTION(f); \
tResult = f(a0,a1,a2); \
GT_EXIT_FUNCTION(f); \
return tResult
//----------------------------------------------------------------------------
#define GT_PRE_BODY_A4_RET(f,a0,a1,a2,a3,type)\
type tResult; \
GT_ENTER_FUNCTION(f); \
GT_NONNULL_FUNCTION(f); \
tResult = f(a0,a1,a2,a3); \
GT_EXIT_FUNCTION(f); \
return tResult
//----------------------------------------------------------------------------
#define GT_PRE_BODY_A5_RET(f,a0,a1,a2,a3,a4,type)\
type tResult; \
GT_ENTER_FUNCTION(f); \
GT_NONNULL_FUNCTION(f); \
tResult = f(a0,a1,a2,a3,a4); \
GT_EXIT_FUNCTION(f); \
return tResult
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Macros for bodies of functions with no return value.
//----------------------------------------------------------------------------
#define GT_BODY_A0(f)\
GT_ENTER_FUNCTION(f); \
if (f) \
{ \
    GT_NONNULL_FUNCTION(f); \
    f(); \
} \
else \
{ \
    GT_NULL_FUNCTION(f); \
} \
GT_EXIT_FUNCTION(f)
//----------------------------------------------------------------------------
#define GT_BODY_A1(f,a0)\
GT_ENTER_FUNCTION(f); \
if (f) \
{ \
    GT_NONNULL_FUNCTION(f); \
    f(a0); \
} \
else \
{ \
    GT_NULL_FUNCTION(f); \
} \
GT_EXIT_FUNCTION(f)
//----------------------------------------------------------------------------
#define GT_BODY_A2(f,a0,a1)\
GT_ENTER_FUNCTION(f); \
if (f) \
{ \
    GT_NONNULL_FUNCTION(f); \
    f(a0,a1); \
} \
else \
{ \
    GT_NULL_FUNCTION(f); \
} \
GT_EXIT_FUNCTION(f)
//----------------------------------------------------------------------------
#define GT_BODY_A3(f,a0,a1,a2)\
GT_ENTER_FUNCTION(f); \
if (f) \
{ \
    GT_NONNULL_FUNCTION(f); \
    f(a0,a1,a2); \
} \
else \
{ \
    GT_NULL_FUNCTION(f); \
} \
GT_EXIT_FUNCTION(f)
//----------------------------------------------------------------------------
#define GT_BODY_A4(f,a0,a1,a2,a3)\
GT_ENTER_FUNCTION(f); \
if (f) \
{ \
    GT_NONNULL_FUNCTION(f); \
    f(a0,a1,a2,a3); \
} \
else \
{ \
    GT_NULL_FUNCTION(f); \
} \
GT_EXIT_FUNCTION(f)
//----------------------------------------------------------------------------
#define GT_BODY_A5(f,a0,a1,a2,a3,a4)\
GT_ENTER_FUNCTION(f); \
if (f) \
{ \
    GT_NONNULL_FUNCTION(f); \
    f(a0,a1,a2,a3,a4); \
} \
else \
{ \
    GT_NULL_FUNCTION(f); \
} \
GT_EXIT_FUNCTION(f)
//----------------------------------------------------------------------------
#define GT_BODY_A6(f,a0,a1,a2,a3,a4,a5)\
GT_ENTER_FUNCTION(f); \
if (f) \
{ \
    GT_NONNULL_FUNCTION(f); \
    f(a0,a1,a2,a3,a4,a5); \
} \
else \
{ \
    GT_NULL_FUNCTION(f); \
} \
GT_EXIT_FUNCTION(f)
//----------------------------------------------------------------------------
#define GT_BODY_A7(f,a0,a1,a2,a3,a4,a5,a6)\
GT_ENTER_FUNCTION(f); \
if (f) \
{ \
    GT_NONNULL_FUNCTION(f); \
    f(a0,a1,a2,a3,a4,a5,a6); \
} \
else \
{ \
    GT_NULL_FUNCTION(f); \
} \
GT_EXIT_FUNCTION(f)
//----------------------------------------------------------------------------
#define GT_BODY_A8(f,a0,a1,a2,a3,a4,a5,a6,a7)\
GT_ENTER_FUNCTION(f); \
if (f) \
{ \
    GT_NONNULL_FUNCTION(f); \
    f(a0,a1,a2,a3,a4,a5,a6,a7); \
} \
else \
{ \
    GT_NULL_FUNCTION(f); \
} \
GT_EXIT_FUNCTION(f)
//----------------------------------------------------------------------------
#define GT_BODY_A9(f,a0,a1,a2,a3,a4,a5,a6,a7,a8)\
GT_ENTER_FUNCTION(f); \
if (f) \
{ \
    GT_NONNULL_FUNCTION(f); \
    f(a0,a1,a2,a3,a4,a5,a6,a7,a8); \
} \
else \
{ \
    GT_NULL_FUNCTION(f); \
} \
GT_EXIT_FUNCTION(f)
//----------------------------------------------------------------------------
#define GT_BODY_A10(f,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9)\
GT_ENTER_FUNCTION(f); \
if (f) \
{ \
    GT_NONNULL_FUNCTION(f); \
    f(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9); \
} \
else \
{ \
    GT_NULL_FUNCTION(f); \
} \
GT_EXIT_FUNCTION(f)
//----------------------------------------------------------------------------
#define GT_BODY_A11(f,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10)\
GT_ENTER_FUNCTION(f); \
if (f) \
{ \
    GT_NONNULL_FUNCTION(f); \
    f(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10); \
} \
else \
{ \
    GT_NULL_FUNCTION(f); \
} \
GT_EXIT_FUNCTION(f)
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Macros for bodies of functions with a return value.
//----------------------------------------------------------------------------
#define GT_BODY_A0_RET(f,type,nullretval)\
type tResult; \
GT_ENTER_FUNCTION(f); \
if (f) \
{ \
    GT_NONNULL_FUNCTION(f); \
    tResult = f(); \
} \
else \
{ \
    GT_NULL_FUNCTION(f); \
    tResult = nullretval; \
} \
GT_EXIT_FUNCTION(f); \
return tResult
//----------------------------------------------------------------------------
#define GT_BODY_A1_RET(f,a0,type,nullretval)\
type tResult; \
GT_ENTER_FUNCTION(f); \
if (f) \
{ \
    GT_NONNULL_FUNCTION(f); \
    tResult = f(a0); \
} \
else \
{ \
    GT_NULL_FUNCTION(f); \
    tResult = nullretval; \
} \
GT_EXIT_FUNCTION(f); \
return tResult
//----------------------------------------------------------------------------
#define GT_BODY_A2_RET(f,a0,a1,type,nullretval)\
type tResult; \
GT_ENTER_FUNCTION(f); \
if (f) \
{ \
    GT_NONNULL_FUNCTION(f); \
    tResult = f(a0,a1); \
} \
else \
{ \
    GT_NULL_FUNCTION(f); \
    tResult = nullretval; \
} \
GT_EXIT_FUNCTION(f); \
return tResult
//----------------------------------------------------------------------------
#define GT_BODY_A3_RET(f,a0,a1,a2,type,nullretval)\
type tResult; \
GT_ENTER_FUNCTION(f); \
if (f) \
{ \
    GT_NONNULL_FUNCTION(f); \
    tResult = f(a0,a1,a2); \
} \
else \
{ \
    GT_NULL_FUNCTION(f); \
    tResult = nullretval; \
} \
GT_EXIT_FUNCTION(f); \
return tResult
//----------------------------------------------------------------------------
#define GT_BODY_A4_RET(f,a0,a1,a2,a3,type,nullretval)\
type tResult; \
GT_ENTER_FUNCTION(f); \
if (f) \
{ \
    GT_NONNULL_FUNCTION(f); \
    tResult = f(a0,a1,a2,a3); \
} \
else \
{ \
    GT_NULL_FUNCTION(f); \
    tResult = nullretval; \
} \
GT_EXIT_FUNCTION(f); \
return tResult
//----------------------------------------------------------------------------
#define GT_BODY_A5_RET(f,a0,a1,a2,a3,a4,type,nullretval)\
type tResult; \
GT_ENTER_FUNCTION(f); \
if (f) \
{ \
    GT_NONNULL_FUNCTION(f); \
    tResult = f(a0,a1,a2,a3,a4); \
} \
else \
{ \
    GT_NULL_FUNCTION(f); \
    tResult = nullretval; \
} \
GT_EXIT_FUNCTION(f); \
return tResult
//----------------------------------------------------------------------------
#define GT_BODY_A6_RET(f,a0,a1,a2,a3,a4,a5,type,nullretval)\
type tResult; \
GT_ENTER_FUNCTION(f); \
if (f) \
{ \
    GT_NONNULL_FUNCTION(f); \
    tResult = f(a0,a1,a2,a3,a4,a5); \
} \
else \
{ \
    GT_NULL_FUNCTION(f); \
    tResult = nullretval; \
} \
GT_EXIT_FUNCTION(f); \
return tResult
//----------------------------------------------------------------------------
#define GT_BODY_A7_RET(f,a0,a1,a2,a3,a4,a5,a6,type,nullretval)\
type tResult; \
GT_ENTER_FUNCTION(f); \
if (f) \
{ \
    GT_NONNULL_FUNCTION(f); \
    tResult = f(a0,a1,a2,a3,a4,a5,a6); \
} \
else \
{ \
    GT_NULL_FUNCTION(f); \
    tResult = nullretval; \
} \
GT_EXIT_FUNCTION(f); \
return tResult
//----------------------------------------------------------------------------

#ifdef __cplusplus
#include <cassert>
extern "C" {
#else
#include <assert.h>
#endif

#define GT_VERSION_NONE  0
#define GT_VERSION_1_1  11
#define GT_VERSION_1_2  12
#define GT_VERSION_1_3  13
#define GT_VERSION_1_4  14
#define GT_VERSION_1_5  15
#define GT_VERSION_2_0  20
#define GT_VERSION_2_1  21
#define GT_VERSION_3_0  30
#define GT_VERSION_3_1  31
#define GT_VERSION_3_2  32
#define GT_VERSION_3_3  33
#define GT_VERSION_4_0  40
int GTGetVersion ();
int GTSupportsExtension (const char* acExtension);

// This function must be implemented for each platform.
void* GTGetFunctionPointer (const char*);

// For error checking.
const char* GTGetErrorString (GLenum code);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif
