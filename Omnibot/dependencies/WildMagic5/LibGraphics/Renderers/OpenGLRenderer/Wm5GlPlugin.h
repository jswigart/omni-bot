// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5GLPLUGIN_H
#define WM5GLPLUGIN_H

// The default behavior is to have no additional semantics per OpenGL call.
// Replace GTglPlugin.h and, if necessary, add a file GTglPlugin.c that
// implements additional semantics.  These can include report-log generation
// or profiling.

#define GT_ENTER_FUNCTION(glFunction)
#define GT_NONNULL_FUNCTION(glFunction)

//#define DEV_ENABLE_GL_ERROR_CHECKING
#ifdef DEV_ENABLE_GL_ERROR_CHECKING

// This function asserts when a null function is encountered.  The renderer
// is expecting the function to exist.
extern void ReportNullFunction(const char*);
#define GT_NULL_FUNCTION(glFunction) ReportNullFunction(#glFunction)

// This function checks glGetError after each OpenGL function call.
extern void ReportGLError(const char*);
#define GT_EXIT_FUNCTION(glFunction) ReportGLError(#glFunction)

#else

// Enable this block to disable error checking.
#define GT_NULL_FUNCTION(glFunction)
#define GT_EXIT_FUNCTION(glFunction)

#endif

#endif
