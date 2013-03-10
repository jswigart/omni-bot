// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/08/22)

#include "Wm5GlExtensions.h"

#if defined(_MSC_VER)
// Disable the warning about "conditional expression is constant".  Without
// this, the macros will generate about 250 warnings.
#pragma warning( disable : 4127 )
#endif

//----------------------------------------------------------------------------
// OpenGL 1.1
//----------------------------------------------------------------------------
int GTExistsOpenGL11 = 1;

#undef glAccum
#undef glAlphaFunc
#undef glAreTexturesResident
#undef glArrayElement
#undef glBegin
#undef glBindTexture
#undef glBitmap
#undef glBlendFunc
#undef glCallList
#undef glCallLists
#undef glClear
#undef glClearAccum
#undef glClearColor
#undef glClearDepth
#undef glClearIndex
#undef glClearStencil
#undef glClipPlane
#undef glColor3b
#undef glColor3bv
#undef glColor3d
#undef glColor3dv
#undef glColor3f
#undef glColor3fv
#undef glColor3i
#undef glColor3iv
#undef glColor3s
#undef glColor3sv
#undef glColor3ub
#undef glColor3ubv
#undef glColor3ui
#undef glColor3uiv
#undef glColor3us
#undef glColor3usv
#undef glColor4b
#undef glColor4bv
#undef glColor4d
#undef glColor4dv
#undef glColor4f
#undef glColor4fv
#undef glColor4i
#undef glColor4iv
#undef glColor4s
#undef glColor4sv
#undef glColor4ub
#undef glColor4ubv
#undef glColor4ui
#undef glColor4uiv
#undef glColor4us
#undef glColor4usv
#undef glColorMask
#undef glColorMaterial
#undef glColorPointer
#undef glCopyPixels
#undef glCopyTexImage1D
#undef glCopyTexImage2D
#undef glCopyTexSubImage1D
#undef glCopyTexSubImage2D
#undef glCullFace
#undef glDeleteLists
#undef glDeleteTextures
#undef glDepthFunc
#undef glDepthMask
#undef glDepthRange
#undef glDisable
#undef glDisableClientState
#undef glDrawArrays
#undef glDrawBuffer
#undef glDrawElements
#undef glDrawPixels
#undef glEdgeFlag
#undef glEdgeFlagPointer
#undef glEdgeFlagv
#undef glEnable
#undef glEnableClientState
#undef glEnd
#undef glEndList
#undef glEvalCoord1d
#undef glEvalCoord1dv
#undef glEvalCoord1f
#undef glEvalCoord1fv
#undef glEvalCoord2d
#undef glEvalCoord2dv
#undef glEvalCoord2f
#undef glEvalCoord2fv
#undef glEvalMesh1
#undef glEvalMesh2
#undef glEvalPoint1
#undef glEvalPoint2
#undef glFeedbackBuffer
#undef glFinish
#undef glFlush
#undef glFogf
#undef glFogfv
#undef glFogi
#undef glFogiv
#undef glFrontFace
#undef glFrustum
#undef glGenLists
#undef glGenTextures
#undef glGetBooleanv
#undef glGetClipPlane
#undef glGetDoublev
#undef glGetError
#undef glGetFloatv
#undef glGetIntegerv
#undef glGetLightfv
#undef glGetLightiv
#undef glGetMapdv
#undef glGetMapfv
#undef glGetMapiv
#undef glGetMaterialfv
#undef glGetMaterialiv
#undef glGetPixelMapfv
#undef glGetPixelMapuiv
#undef glGetPixelMapusv
#undef glGetPointerv
#undef glGetPolygonStipple
#undef glGetString
#undef glGetTexEnvfv
#undef glGetTexEnviv
#undef glGetTexGendv
#undef glGetTexGenfv
#undef glGetTexGeniv
#undef glGetTexImage
#undef glGetTexLevelParameterfv
#undef glGetTexLevelParameteriv
#undef glGetTexParameterfv
#undef glGetTexParameteriv
#undef glHint
#undef glIndexMask
#undef glIndexPointer
#undef glIndexd
#undef glIndexdv
#undef glIndexf
#undef glIndexfv
#undef glIndexi
#undef glIndexiv
#undef glIndexs
#undef glIndexsv
#undef glIndexub
#undef glIndexubv
#undef glInitNames
#undef glInterleavedArrays
#undef glIsEnabled
#undef glIsList
#undef glIsTexture
#undef glLightModelf
#undef glLightModelfv
#undef glLightModeli
#undef glLightModeliv
#undef glLightf
#undef glLightfv
#undef glLighti
#undef glLightiv
#undef glLineStipple
#undef glLineWidth
#undef glListBase
#undef glLoadIdentity
#undef glLoadMatrixd
#undef glLoadMatrixf
#undef glLoadName
#undef glLogicOp
#undef glMap1d
#undef glMap1f
#undef glMap2d
#undef glMap2f
#undef glMapGrid1d
#undef glMapGrid1f
#undef glMapGrid2d
#undef glMapGrid2f
#undef glMaterialf
#undef glMaterialfv
#undef glMateriali
#undef glMaterialiv
#undef glMatrixMode
#undef glMultMatrixd
#undef glMultMatrixf
#undef glNewList
#undef glNormal3b
#undef glNormal3bv
#undef glNormal3d
#undef glNormal3dv
#undef glNormal3f
#undef glNormal3fv
#undef glNormal3i
#undef glNormal3iv
#undef glNormal3s
#undef glNormal3sv
#undef glNormalPointer
#undef glOrtho
#undef glPassThrough
#undef glPixelMapfv
#undef glPixelMapuiv
#undef glPixelMapusv
#undef glPixelStoref
#undef glPixelStorei
#undef glPixelTransferf
#undef glPixelTransferi
#undef glPixelZoom
#undef glPointSize
#undef glPolygonMode
#undef glPolygonOffset
#undef glPolygonStipple
#undef glPopAttrib
#undef glPopClientAttrib
#undef glPopMatrix
#undef glPopName
#undef glPrioritizeTextures
#undef glPushAttrib
#undef glPushClientAttrib
#undef glPushMatrix
#undef glPushName
#undef glRasterPos2d
#undef glRasterPos2dv
#undef glRasterPos2f
#undef glRasterPos2fv
#undef glRasterPos2i
#undef glRasterPos2iv
#undef glRasterPos2s
#undef glRasterPos2sv
#undef glRasterPos3d
#undef glRasterPos3dv
#undef glRasterPos3f
#undef glRasterPos3fv
#undef glRasterPos3i
#undef glRasterPos3iv
#undef glRasterPos3s
#undef glRasterPos3sv
#undef glRasterPos4d
#undef glRasterPos4dv
#undef glRasterPos4f
#undef glRasterPos4fv
#undef glRasterPos4i
#undef glRasterPos4iv
#undef glRasterPos4s
#undef glRasterPos4sv
#undef glReadBuffer
#undef glReadPixels
#undef glRectd
#undef glRectdv
#undef glRectf
#undef glRectfv
#undef glRecti
#undef glRectiv
#undef glRects
#undef glRectsv
#undef glRenderMode
#undef glRotated
#undef glRotatef
#undef glScaled
#undef glScalef
#undef glScissor
#undef glSelectBuffer
#undef glShadeModel
#undef glStencilFunc
#undef glStencilMask
#undef glStencilOp
#undef glTexCoord1d
#undef glTexCoord1dv
#undef glTexCoord1f
#undef glTexCoord1fv
#undef glTexCoord1i
#undef glTexCoord1iv
#undef glTexCoord1s
#undef glTexCoord1sv
#undef glTexCoord2d
#undef glTexCoord2dv
#undef glTexCoord2f
#undef glTexCoord2fv
#undef glTexCoord2i
#undef glTexCoord2iv
#undef glTexCoord2s
#undef glTexCoord2sv
#undef glTexCoord3d
#undef glTexCoord3dv
#undef glTexCoord3f
#undef glTexCoord3fv
#undef glTexCoord3i
#undef glTexCoord3iv
#undef glTexCoord3s
#undef glTexCoord3sv
#undef glTexCoord4d
#undef glTexCoord4dv
#undef glTexCoord4f
#undef glTexCoord4fv
#undef glTexCoord4i
#undef glTexCoord4iv
#undef glTexCoord4s
#undef glTexCoord4sv
#undef glTexCoordPointer
#undef glTexEnvf
#undef glTexEnvfv
#undef glTexEnvi
#undef glTexEnviv
#undef glTexGend
#undef glTexGendv
#undef glTexGenf
#undef glTexGenfv
#undef glTexGeni
#undef glTexGeniv
#undef glTexImage1D
#undef glTexImage2D
#undef glTexParameterf
#undef glTexParameterfv
#undef glTexParameteri
#undef glTexParameteriv
#undef glTexSubImage1D
#undef glTexSubImage2D
#undef glTranslated
#undef glTranslatef
#undef glVertex2d
#undef glVertex2dv
#undef glVertex2f
#undef glVertex2fv
#undef glVertex2i
#undef glVertex2iv
#undef glVertex2s
#undef glVertex2sv
#undef glVertex3d
#undef glVertex3dv
#undef glVertex3f
#undef glVertex3fv
#undef glVertex3i
#undef glVertex3iv
#undef glVertex3s
#undef glVertex3sv
#undef glVertex4d
#undef glVertex4dv
#undef glVertex4f
#undef glVertex4fv
#undef glVertex4i
#undef glVertex4iv
#undef glVertex4s
#undef glVertex4sv
#undef glVertexPointer
#undef glViewport

extern "C" {

EXTERN void STDCALL glAccum (GLenum, GLfloat);
EXTERN void STDCALL glAlphaFunc (GLenum, GLclampf);
EXTERN GLboolean STDCALL glAreTexturesResident (GLsizei, const GLuint*,
    GLboolean*);
EXTERN void STDCALL glArrayElement (GLint);
EXTERN void STDCALL glBegin (GLenum);
EXTERN void STDCALL glBindTexture (GLenum, GLuint);
EXTERN void STDCALL glBitmap (GLsizei, GLsizei, GLfloat, GLfloat, GLfloat,
    GLfloat, const GLubyte*);
EXTERN void STDCALL glBlendFunc (GLenum, GLenum);
EXTERN void STDCALL glCallList (GLuint);
EXTERN void STDCALL glCallLists (GLsizei, GLenum, const GLvoid*);
EXTERN void STDCALL glClear (GLbitfield);
EXTERN void STDCALL glClearAccum (GLfloat, GLfloat, GLfloat, GLfloat);
EXTERN void STDCALL glClearColor (GLclampf, GLclampf, GLclampf, GLclampf);
EXTERN void STDCALL glClearDepth (GLclampd);
EXTERN void STDCALL glClearIndex (GLfloat);
EXTERN void STDCALL glClearStencil (GLint);
EXTERN void STDCALL glClipPlane (GLenum, const GLdouble*);
EXTERN void STDCALL glColor3b (GLbyte, GLbyte, GLbyte);
EXTERN void STDCALL glColor3bv (const GLbyte*);
EXTERN void STDCALL glColor3d (GLdouble, GLdouble, GLdouble);
EXTERN void STDCALL glColor3dv (const GLdouble*);
EXTERN void STDCALL glColor3f (GLfloat, GLfloat, GLfloat);
EXTERN void STDCALL glColor3fv (const GLfloat*);
EXTERN void STDCALL glColor3i (GLint, GLint, GLint);
EXTERN void STDCALL glColor3iv (const GLint*);
EXTERN void STDCALL glColor3s (GLshort, GLshort, GLshort);
EXTERN void STDCALL glColor3sv (const GLshort*);
EXTERN void STDCALL glColor3ub (GLubyte, GLubyte, GLubyte);
EXTERN void STDCALL glColor3ubv (const GLubyte*);
EXTERN void STDCALL glColor3ui (GLuint, GLuint, GLuint);
EXTERN void STDCALL glColor3uiv (const GLuint*);
EXTERN void STDCALL glColor3us (GLushort, GLushort, GLushort);
EXTERN void STDCALL glColor3usv (const GLushort*);
EXTERN void STDCALL glColor4b (GLbyte, GLbyte, GLbyte, GLbyte);
EXTERN void STDCALL glColor4bv (const GLbyte*);
EXTERN void STDCALL glColor4d (GLdouble, GLdouble, GLdouble, GLdouble);
EXTERN void STDCALL glColor4dv (const GLdouble*);
EXTERN void STDCALL glColor4f (GLfloat, GLfloat, GLfloat, GLfloat);
EXTERN void STDCALL glColor4fv (const GLfloat*);
EXTERN void STDCALL glColor4i (GLint, GLint, GLint, GLint);
EXTERN void STDCALL glColor4iv (const GLint*);
EXTERN void STDCALL glColor4s (GLshort, GLshort, GLshort, GLshort);
EXTERN void STDCALL glColor4sv (const GLshort*);
EXTERN void STDCALL glColor4ub (GLubyte, GLubyte, GLubyte, GLubyte);
EXTERN void STDCALL glColor4ubv (const GLubyte*);
EXTERN void STDCALL glColor4ui (GLuint, GLuint, GLuint, GLuint);
EXTERN void STDCALL glColor4uiv (const GLuint*);
EXTERN void STDCALL glColor4us (GLushort, GLushort, GLushort, GLushort);
EXTERN void STDCALL glColor4usv (const GLushort*);
EXTERN void STDCALL glColorMask (GLboolean, GLboolean, GLboolean, GLboolean);
EXTERN void STDCALL glColorMaterial (GLenum, GLenum);
EXTERN void STDCALL glColorPointer (GLint, GLenum, GLsizei, const GLvoid*);
EXTERN void STDCALL glCopyPixels (GLint, GLint, GLsizei, GLsizei, GLenum);
EXTERN void STDCALL glCopyTexImage1D (GLenum, GLint, GLenum, GLint, GLint,
    GLsizei, GLint);
EXTERN void STDCALL glCopyTexImage2D (GLenum, GLint, GLenum, GLint, GLint,
    GLsizei, GLsizei, GLint);
EXTERN void STDCALL glCopyTexSubImage1D (GLenum, GLint, GLint, GLint, GLint,
    GLsizei);
EXTERN void STDCALL glCopyTexSubImage2D (GLenum, GLint, GLint, GLint, GLint,
    GLint, GLsizei, GLsizei);
EXTERN void STDCALL glCullFace (GLenum);
EXTERN void STDCALL glDeleteLists (GLuint, GLsizei);
EXTERN void STDCALL glDeleteTextures (GLsizei, const GLuint*);
EXTERN void STDCALL glDepthFunc (GLenum);
EXTERN void STDCALL glDepthMask (GLboolean);
EXTERN void STDCALL glDepthRange (GLclampd, GLclampd);
EXTERN void STDCALL glDisable (GLenum);
EXTERN void STDCALL glDisableClientState (GLenum);
EXTERN void STDCALL glDrawArrays (GLenum, GLint, GLsizei);
EXTERN void STDCALL glDrawBuffer (GLenum);
EXTERN void STDCALL glDrawElements (GLenum, GLsizei, GLenum, const GLvoid*);
EXTERN void STDCALL glDrawPixels (GLsizei, GLsizei, GLenum, GLenum,
    const GLvoid*);
EXTERN void STDCALL glEdgeFlag (GLboolean);
EXTERN void STDCALL glEdgeFlagPointer (GLsizei, const GLvoid*);
EXTERN void STDCALL glEdgeFlagv (const GLboolean*);
EXTERN void STDCALL glEnable (GLenum);
EXTERN void STDCALL glEnableClientState (GLenum);
EXTERN void STDCALL glEnd ();
EXTERN void STDCALL glEndList ();
EXTERN void STDCALL glEvalCoord1d (GLdouble);
EXTERN void STDCALL glEvalCoord1dv (const GLdouble*);
EXTERN void STDCALL glEvalCoord1f (GLfloat);
EXTERN void STDCALL glEvalCoord1fv (const GLfloat*);
EXTERN void STDCALL glEvalCoord2d (GLdouble, GLdouble);
EXTERN void STDCALL glEvalCoord2dv (const GLdouble*);
EXTERN void STDCALL glEvalCoord2f (GLfloat, GLfloat);
EXTERN void STDCALL glEvalCoord2fv (const GLfloat*);
EXTERN void STDCALL glEvalMesh1 (GLenum, GLint, GLint);
EXTERN void STDCALL glEvalMesh2 (GLenum, GLint, GLint, GLint, GLint);
EXTERN void STDCALL glEvalPoint1 (GLint);
EXTERN void STDCALL glEvalPoint2 (GLint, GLint);
EXTERN void STDCALL glFeedbackBuffer (GLsizei, GLenum, GLfloat*);
EXTERN void STDCALL glFinish ();
EXTERN void STDCALL glFlush ();
EXTERN void STDCALL glFogf (GLenum, GLfloat);
EXTERN void STDCALL glFogfv (GLenum, const GLfloat*);
EXTERN void STDCALL glFogi (GLenum, GLint);
EXTERN void STDCALL glFogiv (GLenum, const GLint*);
EXTERN void STDCALL glFrontFace (GLenum);
EXTERN void STDCALL glFrustum (GLdouble, GLdouble, GLdouble, GLdouble,
    GLdouble, GLdouble);
EXTERN GLuint STDCALL glGenLists (GLsizei);
EXTERN void STDCALL glGenTextures (GLsizei, GLuint*);
EXTERN void STDCALL glGetBooleanv (GLenum, GLboolean*);
EXTERN void STDCALL glGetClipPlane (GLenum, GLdouble*);
EXTERN void STDCALL glGetDoublev (GLenum, GLdouble*);
EXTERN GLenum STDCALL glGetError ();
EXTERN void STDCALL glGetFloatv (GLenum, GLfloat*);
EXTERN void STDCALL glGetIntegerv (GLenum, GLint*);
EXTERN void STDCALL glGetLightfv (GLenum, GLenum, GLfloat*);
EXTERN void STDCALL glGetLightiv (GLenum, GLenum, GLint*);
EXTERN void STDCALL glGetMapdv (GLenum, GLenum, GLdouble*);
EXTERN void STDCALL glGetMapfv (GLenum, GLenum, GLfloat*);
EXTERN void STDCALL glGetMapiv (GLenum, GLenum, GLint*);
EXTERN void STDCALL glGetMaterialfv (GLenum, GLenum, GLfloat*);
EXTERN void STDCALL glGetMaterialiv (GLenum, GLenum, GLint*);
EXTERN void STDCALL glGetPixelMapfv (GLenum, GLfloat*);
EXTERN void STDCALL glGetPixelMapuiv (GLenum, GLuint*);
EXTERN void STDCALL glGetPixelMapusv (GLenum, GLushort*);
EXTERN void STDCALL glGetPointerv (GLenum, GLvoid**);
EXTERN void STDCALL glGetPolygonStipple (GLubyte*);
EXTERN const GLubyte* STDCALL glGetString (GLenum);
EXTERN void STDCALL glGetTexEnvfv (GLenum, GLenum, GLfloat*);
EXTERN void STDCALL glGetTexEnviv (GLenum, GLenum, GLint*);
EXTERN void STDCALL glGetTexGendv (GLenum, GLenum, GLdouble*);
EXTERN void STDCALL glGetTexGenfv (GLenum, GLenum, GLfloat*);
EXTERN void STDCALL glGetTexGeniv (GLenum, GLenum, GLint*);
EXTERN void STDCALL glGetTexImage (GLenum, GLint, GLenum, GLenum, GLvoid*);
EXTERN void STDCALL glGetTexLevelParameterfv (GLenum, GLint, GLenum,
    GLfloat*);
EXTERN void STDCALL glGetTexLevelParameteriv (GLenum, GLint, GLenum, GLint*);
EXTERN void STDCALL glGetTexParameterfv (GLenum, GLenum, GLfloat*);
EXTERN void STDCALL glGetTexParameteriv (GLenum, GLenum, GLint*);
EXTERN void STDCALL glHint (GLenum, GLenum);
EXTERN void STDCALL glIndexMask (GLuint);
EXTERN void STDCALL glIndexPointer (GLenum, GLsizei, const GLvoid*);
EXTERN void STDCALL glIndexd (GLdouble);
EXTERN void STDCALL glIndexdv (const GLdouble*);
EXTERN void STDCALL glIndexf (GLfloat);
EXTERN void STDCALL glIndexfv (const GLfloat*);
EXTERN void STDCALL glIndexi (GLint);
EXTERN void STDCALL glIndexiv (const GLint*);
EXTERN void STDCALL glIndexs (GLshort);
EXTERN void STDCALL glIndexsv (const GLshort*);
EXTERN void STDCALL glIndexub (GLubyte);
EXTERN void STDCALL glIndexubv (const GLubyte*);
EXTERN void STDCALL glInitNames ();
EXTERN void STDCALL glInterleavedArrays (GLenum, GLsizei, const GLvoid*);
EXTERN GLboolean STDCALL glIsEnabled (GLenum);
EXTERN GLboolean STDCALL glIsList (GLuint);
EXTERN GLboolean STDCALL glIsTexture (GLuint);
EXTERN void STDCALL glLightModelf (GLenum, GLfloat);
EXTERN void STDCALL glLightModelfv (GLenum, const GLfloat*);
EXTERN void STDCALL glLightModeli (GLenum, GLint);
EXTERN void STDCALL glLightModeliv (GLenum, const GLint*);
EXTERN void STDCALL glLightf (GLenum, GLenum, GLfloat);
EXTERN void STDCALL glLightfv (GLenum, GLenum, const GLfloat*);
EXTERN void STDCALL glLighti (GLenum, GLenum, GLint);
EXTERN void STDCALL glLightiv (GLenum, GLenum, const GLint*);
EXTERN void STDCALL glLineStipple (GLint, GLushort);
EXTERN void STDCALL glLineWidth (GLfloat);
EXTERN void STDCALL glListBase (GLuint);
EXTERN void STDCALL glLoadIdentity ();
EXTERN void STDCALL glLoadMatrixd (const GLdouble*);
EXTERN void STDCALL glLoadMatrixf (const GLfloat*);
EXTERN void STDCALL glLoadName (GLuint);
EXTERN void STDCALL glLogicOp (GLenum);
EXTERN void STDCALL glMap1d (GLenum, GLdouble, GLdouble, GLint, GLint,
    const GLdouble*);
EXTERN void STDCALL glMap1f (GLenum, GLfloat, GLfloat, GLint, GLint,
    const GLfloat*);
EXTERN void STDCALL glMap2d (GLenum, GLdouble, GLdouble, GLint, GLint,
    GLdouble, GLdouble, GLint, GLint, const GLdouble*);
EXTERN void STDCALL glMap2f (GLenum, GLfloat, GLfloat, GLint, GLint, GLfloat,
    GLfloat, GLint, GLint, const GLfloat*);
EXTERN void STDCALL glMapGrid1d (GLint, GLdouble, GLdouble);
EXTERN void STDCALL glMapGrid1f (GLint, GLfloat, GLfloat);
EXTERN void STDCALL glMapGrid2d (GLint, GLdouble, GLdouble, GLint, GLdouble,
    GLdouble);
EXTERN void STDCALL glMapGrid2f (GLint, GLfloat, GLfloat, GLint, GLfloat,
    GLfloat);
EXTERN void STDCALL glMaterialf (GLenum, GLenum, GLfloat);
EXTERN void STDCALL glMaterialfv (GLenum, GLenum, const GLfloat*);
EXTERN void STDCALL glMateriali (GLenum, GLenum, GLint);
EXTERN void STDCALL glMaterialiv (GLenum, GLenum, const GLint*);
EXTERN void STDCALL glMatrixMode (GLenum);
EXTERN void STDCALL glMultMatrixd (const GLdouble*);
EXTERN void STDCALL glMultMatrixf (const GLfloat*);
EXTERN void STDCALL glNewList (GLuint, GLenum);
EXTERN void STDCALL glNormal3b (GLbyte, GLbyte, GLbyte);
EXTERN void STDCALL glNormal3bv (const GLbyte*);
EXTERN void STDCALL glNormal3d (GLdouble, GLdouble, GLdouble);
EXTERN void STDCALL glNormal3dv (const GLdouble*);
EXTERN void STDCALL glNormal3f (GLfloat, GLfloat, GLfloat);
EXTERN void STDCALL glNormal3fv (const GLfloat*);
EXTERN void STDCALL glNormal3i (GLint, GLint, GLint);
EXTERN void STDCALL glNormal3iv (const GLint*);
EXTERN void STDCALL glNormal3s (GLshort, GLshort, GLshort);
EXTERN void STDCALL glNormal3sv (const GLshort*);
EXTERN void STDCALL glNormalPointer (GLenum, GLsizei, const GLvoid*);
EXTERN void STDCALL glOrtho (GLdouble, GLdouble, GLdouble, GLdouble,
    GLdouble, GLdouble);
EXTERN void STDCALL glPassThrough (GLfloat);
EXTERN void STDCALL glPixelMapfv (GLenum, GLsizei, const GLfloat*);
EXTERN void STDCALL glPixelMapuiv (GLenum, GLsizei, const GLuint*);
EXTERN void STDCALL glPixelMapusv (GLenum, GLsizei, const GLushort*);
EXTERN void STDCALL glPixelStoref (GLenum, GLfloat);
EXTERN void STDCALL glPixelStorei (GLenum, GLint);
EXTERN void STDCALL glPixelTransferf (GLenum, GLfloat);
EXTERN void STDCALL glPixelTransferi (GLenum, GLint);
EXTERN void STDCALL glPixelZoom (GLfloat, GLfloat);
EXTERN void STDCALL glPointSize (GLfloat);
EXTERN void STDCALL glPolygonMode (GLenum, GLenum);
EXTERN void STDCALL glPolygonOffset (GLfloat, GLfloat);
EXTERN void STDCALL glPolygonStipple (const GLubyte*);
EXTERN void STDCALL glPopAttrib ();
EXTERN void STDCALL glPopClientAttrib ();
EXTERN void STDCALL glPopMatrix ();
EXTERN void STDCALL glPopName ();
EXTERN void STDCALL glPrioritizeTextures (GLsizei, const GLuint*,
    const GLclampf*);
EXTERN void STDCALL glPushAttrib (GLbitfield);
EXTERN void STDCALL glPushClientAttrib (GLbitfield);
EXTERN void STDCALL glPushMatrix ();
EXTERN void STDCALL glPushName (GLuint);
EXTERN void STDCALL glRasterPos2d (GLdouble, GLdouble);
EXTERN void STDCALL glRasterPos2dv (const GLdouble*);
EXTERN void STDCALL glRasterPos2f (GLfloat, GLfloat);
EXTERN void STDCALL glRasterPos2fv (const GLfloat*);
EXTERN void STDCALL glRasterPos2i (GLint, GLint);
EXTERN void STDCALL glRasterPos2iv (const GLint*);
EXTERN void STDCALL glRasterPos2s (GLshort, GLshort);
EXTERN void STDCALL glRasterPos2sv (const GLshort *);
EXTERN void STDCALL glRasterPos3d (GLdouble, GLdouble, GLdouble);
EXTERN void STDCALL glRasterPos3dv (const GLdouble*);
EXTERN void STDCALL glRasterPos3f (GLfloat, GLfloat, GLfloat);
EXTERN void STDCALL glRasterPos3fv (const GLfloat*);
EXTERN void STDCALL glRasterPos3i (GLint, GLint, GLint);
EXTERN void STDCALL glRasterPos3iv (const GLint*);
EXTERN void STDCALL glRasterPos3s (GLshort, GLshort, GLshort);
EXTERN void STDCALL glRasterPos3sv (const GLshort*);
EXTERN void STDCALL glRasterPos4d (GLdouble, GLdouble, GLdouble, GLdouble);
EXTERN void STDCALL glRasterPos4dv (const GLdouble*);
EXTERN void STDCALL glRasterPos4f (GLfloat, GLfloat, GLfloat, GLfloat);
EXTERN void STDCALL glRasterPos4fv (const GLfloat*);
EXTERN void STDCALL glRasterPos4i (GLint, GLint, GLint, GLint);
EXTERN void STDCALL glRasterPos4iv (const GLint*);
EXTERN void STDCALL glRasterPos4s (GLshort, GLshort, GLshort, GLshort);
EXTERN void STDCALL glRasterPos4sv (const GLshort*);
EXTERN void STDCALL glReadBuffer (GLenum);
EXTERN void STDCALL glReadPixels (GLint, GLint, GLsizei, GLsizei, GLenum,
    GLenum, GLvoid*);
EXTERN void STDCALL glRectd (GLdouble, GLdouble, GLdouble, GLdouble);
EXTERN void STDCALL glRectdv (const GLdouble*, const GLdouble*);
EXTERN void STDCALL glRectf (GLfloat, GLfloat, GLfloat, GLfloat);
EXTERN void STDCALL glRectfv (const GLfloat*, const GLfloat*);
EXTERN void STDCALL glRecti (GLint, GLint, GLint, GLint);
EXTERN void STDCALL glRectiv (const GLint*, const GLint*);
EXTERN void STDCALL glRects (GLshort, GLshort, GLshort, GLshort);
EXTERN void STDCALL glRectsv (const GLshort*, const GLshort*);
EXTERN GLint STDCALL glRenderMode (GLenum);
EXTERN void STDCALL glRotated (GLdouble, GLdouble, GLdouble, GLdouble);
EXTERN void STDCALL glRotatef (GLfloat, GLfloat, GLfloat, GLfloat);
EXTERN void STDCALL glScaled (GLdouble, GLdouble, GLdouble);
EXTERN void STDCALL glScalef (GLfloat, GLfloat, GLfloat);
EXTERN void STDCALL glScissor (GLint, GLint, GLsizei, GLsizei);
EXTERN void STDCALL glSelectBuffer (GLsizei, GLuint*);
EXTERN void STDCALL glShadeModel (GLenum);
EXTERN void STDCALL glStencilFunc (GLenum, GLint, GLuint);
EXTERN void STDCALL glStencilMask (GLuint);
EXTERN void STDCALL glStencilOp (GLenum, GLenum, GLenum);
EXTERN void STDCALL glTexCoord1d (GLdouble);
EXTERN void STDCALL glTexCoord1dv (const GLdouble*);
EXTERN void STDCALL glTexCoord1f (GLfloat);
EXTERN void STDCALL glTexCoord1fv (const GLfloat*);
EXTERN void STDCALL glTexCoord1i (GLint);
EXTERN void STDCALL glTexCoord1iv (const GLint*);
EXTERN void STDCALL glTexCoord1s (GLshort);
EXTERN void STDCALL glTexCoord1sv (const GLshort*);
EXTERN void STDCALL glTexCoord2d (GLdouble, GLdouble);
EXTERN void STDCALL glTexCoord2dv (const GLdouble*);
EXTERN void STDCALL glTexCoord2f (GLfloat, GLfloat);
EXTERN void STDCALL glTexCoord2fv (const GLfloat*);
EXTERN void STDCALL glTexCoord2i (GLint, GLint);
EXTERN void STDCALL glTexCoord2iv (const GLint*);
EXTERN void STDCALL glTexCoord2s (GLshort, GLshort);
EXTERN void STDCALL glTexCoord2sv (const GLshort*);
EXTERN void STDCALL glTexCoord3d (GLdouble, GLdouble, GLdouble);
EXTERN void STDCALL glTexCoord3dv (const GLdouble*);
EXTERN void STDCALL glTexCoord3f (GLfloat, GLfloat, GLfloat);
EXTERN void STDCALL glTexCoord3fv (const GLfloat*);
EXTERN void STDCALL glTexCoord3i (GLint, GLint, GLint);
EXTERN void STDCALL glTexCoord3iv (const GLint*);
EXTERN void STDCALL glTexCoord3s (GLshort, GLshort, GLshort);
EXTERN void STDCALL glTexCoord3sv (const GLshort*);
EXTERN void STDCALL glTexCoord4d (GLdouble, GLdouble, GLdouble, GLdouble);
EXTERN void STDCALL glTexCoord4dv (const GLdouble*);
EXTERN void STDCALL glTexCoord4f (GLfloat, GLfloat, GLfloat, GLfloat);
EXTERN void STDCALL glTexCoord4fv (const GLfloat*);
EXTERN void STDCALL glTexCoord4i (GLint, GLint, GLint, GLint);
EXTERN void STDCALL glTexCoord4iv (const GLint*);
EXTERN void STDCALL glTexCoord4s (GLshort, GLshort, GLshort, GLshort);
EXTERN void STDCALL glTexCoord4sv (const GLshort*);
EXTERN void STDCALL glTexCoordPointer (GLint, GLenum, GLsizei, const GLvoid*);
EXTERN void STDCALL glTexEnvf (GLenum, GLenum, GLfloat);
EXTERN void STDCALL glTexEnvfv (GLenum, GLenum, const GLfloat*);
EXTERN void STDCALL glTexEnvi (GLenum, GLenum, GLint);
EXTERN void STDCALL glTexEnviv (GLenum, GLenum, const GLint*);
EXTERN void STDCALL glTexGend (GLenum, GLenum, GLdouble);
EXTERN void STDCALL glTexGendv (GLenum, GLenum, const GLdouble*);
EXTERN void STDCALL glTexGenf (GLenum, GLenum, GLfloat);
EXTERN void STDCALL glTexGenfv (GLenum, GLenum, const GLfloat*);
EXTERN void STDCALL glTexGeni (GLenum, GLenum, GLint);
EXTERN void STDCALL glTexGeniv (GLenum, GLenum, const GLint*);
EXTERN void STDCALL glTexImage1D (GLenum, GLint, GLint, GLsizei, GLint,
    GLenum, GLenum, const GLvoid*);
EXTERN void STDCALL glTexImage2D (GLenum, GLint, GLint, GLsizei, GLsizei,
    GLint, GLenum, GLenum, const GLvoid*);
EXTERN void STDCALL glTexParameterf (GLenum, GLenum, GLfloat);
EXTERN void STDCALL glTexParameterfv (GLenum, GLenum, const GLfloat*);
EXTERN void STDCALL glTexParameteri (GLenum, GLenum, GLint);
EXTERN void STDCALL glTexParameteriv (GLenum, GLenum, const GLint*);
EXTERN void STDCALL glTexSubImage1D (GLenum, GLint, GLint, GLsizei, GLenum,
    GLenum, const GLvoid*);
EXTERN void STDCALL glTexSubImage2D (GLenum, GLint, GLint, GLint, GLsizei,
    GLsizei, GLenum, GLenum, const GLvoid*);
EXTERN void STDCALL glTranslated (GLdouble, GLdouble, GLdouble);
EXTERN void STDCALL glTranslatef (GLfloat, GLfloat, GLfloat);
EXTERN void STDCALL glVertex2d (GLdouble, GLdouble);
EXTERN void STDCALL glVertex2dv (const GLdouble*);
EXTERN void STDCALL glVertex2f (GLfloat, GLfloat);
EXTERN void STDCALL glVertex2fv (const GLfloat*);
EXTERN void STDCALL glVertex2i (GLint, GLint);
EXTERN void STDCALL glVertex2iv (const GLint*);
EXTERN void STDCALL glVertex2s (GLshort, GLshort);
EXTERN void STDCALL glVertex2sv (const GLshort*);
EXTERN void STDCALL glVertex3d (GLdouble, GLdouble, GLdouble);
EXTERN void STDCALL glVertex3dv (const GLdouble*);
EXTERN void STDCALL glVertex3f (GLfloat, GLfloat, GLfloat);
EXTERN void STDCALL glVertex3fv (const GLfloat*);
EXTERN void STDCALL glVertex3i (GLint, GLint, GLint);
EXTERN void STDCALL glVertex3iv (const GLint*);
EXTERN void STDCALL glVertex3s (GLshort, GLshort, GLshort);
EXTERN void STDCALL glVertex3sv (const GLshort*);
EXTERN void STDCALL glVertex4d (GLdouble, GLdouble, GLdouble, GLdouble);
EXTERN void STDCALL glVertex4dv (const GLdouble*);
EXTERN void STDCALL glVertex4f (GLfloat, GLfloat, GLfloat, GLfloat);
EXTERN void STDCALL glVertex4fv (const GLfloat*);
EXTERN void STDCALL glVertex4i (GLint, GLint, GLint, GLint);
EXTERN void STDCALL glVertex4iv (const GLint*);
EXTERN void STDCALL glVertex4s (GLshort, GLshort, GLshort, GLshort);
EXTERN void STDCALL glVertex4sv (const GLshort*);
EXTERN void STDCALL glVertexPointer (GLint, GLenum, GLsizei, const GLvoid*);
EXTERN void STDCALL glViewport (GLint, GLint, GLsizei, GLsizei);

}  // extern "C"

//----------------------------------------------------------------------------
void GTglAccum (GLenum eOperation, GLfloat fValue)
{
    GT_PRE_BODY_A2(glAccum,eOperation,fValue);
}
//----------------------------------------------------------------------------
void GTglAlphaFunc (GLenum eFunction, GLclampf fReference)
{
    GT_PRE_BODY_A2(glAlphaFunc,eFunction,fReference);
}
//----------------------------------------------------------------------------
GLboolean GTglAreTexturesResident (GLsizei iNumTextures,
    const GLuint* auiTextures, GLboolean* aucResidences)
{
    GT_PRE_BODY_A3_RET(glAreTexturesResident,iNumTextures,auiTextures,
        aucResidences,GLboolean);
}
//----------------------------------------------------------------------------
void GTglArrayElement (GLint i)
{
    GT_PRE_BODY_A1(glArrayElement,i);
}
//----------------------------------------------------------------------------
void GTglBegin (GLenum eMode)
{
    GT_PRE_BODY_A1(glBegin,eMode);
}
//----------------------------------------------------------------------------
void GTglBindTexture (GLenum eTarget, GLuint uiTexture)
{
    GT_PRE_BODY_A2(glBindTexture,eTarget,uiTexture);
}
//----------------------------------------------------------------------------
void GTglBitmap (GLsizei iWidth, GLsizei iHeight, GLfloat fXOrigin,
    GLfloat fYOrigin, GLfloat fXMove, GLfloat fYMove,
    const GLubyte* aucBitmap)
{
    GT_PRE_BODY_A7(glBitmap,iWidth,iHeight,fXOrigin,fYOrigin,fXMove,fYMove,
        aucBitmap);
}
//----------------------------------------------------------------------------
void GTglBlendFunc (GLenum eSrcFunction, GLenum eDstFunction)
{
    GT_PRE_BODY_A2(glBlendFunc,eSrcFunction,eDstFunction);
}
//----------------------------------------------------------------------------
void GTglCallList (GLuint uiList)
{
    GT_PRE_BODY_A1(glCallList,uiList);
}
//----------------------------------------------------------------------------
void GTglCallLists (GLsizei iNumLists, GLenum eType, const GLvoid* pvLists)
{
    GT_PRE_BODY_A3(glCallLists,iNumLists,eType,pvLists);
}
//----------------------------------------------------------------------------
void GTglClear (GLbitfield uiMask)
{
    GT_PRE_BODY_A1(glClear,uiMask);
}
//----------------------------------------------------------------------------
void GTglClearAccum (GLfloat fRed, GLfloat fGreen, GLfloat fBlue,
    GLfloat fAlpha)
{
    GT_PRE_BODY_A4(glClearAccum,fRed,fGreen,fBlue,fAlpha);
}
//----------------------------------------------------------------------------
void GTglClearColor (GLclampf fRed, GLclampf fGreen, GLclampf fBlue,
    GLclampf fAlpha)
{
    GT_PRE_BODY_A4(glClearColor,fRed,fGreen,fBlue,fAlpha);
}
//----------------------------------------------------------------------------
void GTglClearDepth (GLclampd dDepth)
{
    GT_PRE_BODY_A1(glClearDepth,dDepth);
}
//----------------------------------------------------------------------------
void GTglClearIndex (GLfloat fIndex)
{
    GT_PRE_BODY_A1(glClearIndex,fIndex);
}
//----------------------------------------------------------------------------
void GTglClearStencil (GLint iStencil)
{
    GT_PRE_BODY_A1(glClearStencil,iStencil);
}
//----------------------------------------------------------------------------
void GTglClipPlane (GLenum ePlane, const GLdouble* adEquation)
{
    GT_PRE_BODY_A2(glClipPlane,ePlane,adEquation);
}
//----------------------------------------------------------------------------
void GTglColor3b (GLbyte cRed, GLbyte cGreen, GLbyte cBlue)
{
    GT_PRE_BODY_A3(glColor3b,cRed,cGreen,cBlue);
}
//----------------------------------------------------------------------------
void GTglColor3bv (const GLbyte* acRGB)
{
    GT_PRE_BODY_A1(glColor3bv,acRGB);
}
//----------------------------------------------------------------------------
void GTglColor3d (GLdouble dRed, GLdouble dGreen, GLdouble dBlue)
{
    GT_PRE_BODY_A3(glColor3d,dRed,dGreen,dBlue);
}
//----------------------------------------------------------------------------
void GTglColor3dv (const GLdouble* adRGB)
{
    GT_PRE_BODY_A1(glColor3dv,adRGB);
}
//----------------------------------------------------------------------------
void GTglColor3f (GLfloat fRed, GLfloat fGreen, GLfloat fBlue)
{
    GT_PRE_BODY_A3(glColor3f,fRed,fGreen,fBlue);
}
//----------------------------------------------------------------------------
void GTglColor3fv (const GLfloat* afRGB)
{
    GT_PRE_BODY_A1(glColor3fv,afRGB);
}
//----------------------------------------------------------------------------
void GTglColor3i (GLint iRed, GLint iGreen, GLint iBlue)
{
    GT_PRE_BODY_A3(glColor3i,iRed,iGreen,iBlue);
}
//----------------------------------------------------------------------------
void GTglColor3iv (const GLint* aiRGB)
{
    GT_PRE_BODY_A1(glColor3iv,aiRGB);
}
//----------------------------------------------------------------------------
void GTglColor3s (GLshort sRed, GLshort sGreen, GLshort sBlue)
{
    GT_PRE_BODY_A3(glColor3s,sRed,sGreen,sBlue);
}
//----------------------------------------------------------------------------
void GTglColor3sv (const GLshort* asColor)
{
    GT_PRE_BODY_A1(glColor3sv,asColor);
}
//----------------------------------------------------------------------------
void GTglColor3ub (GLubyte ucRed, GLubyte ucGreen, GLubyte ucBlue)
{
    GT_PRE_BODY_A3(glColor3ub,ucRed,ucGreen,ucBlue);
}
//----------------------------------------------------------------------------
void GTglColor3ubv (const GLubyte* aucRGB)
{
    GT_PRE_BODY_A1(glColor3ubv,aucRGB);
}
//----------------------------------------------------------------------------
void GTglColor3ui (GLuint uiRed, GLuint uiGreen, GLuint uiBlue)
{
    GT_PRE_BODY_A3(glColor3ui,uiRed,uiGreen,uiBlue);
}
//----------------------------------------------------------------------------
void GTglColor3uiv (const GLuint* auiRGB)
{
    GT_PRE_BODY_A1(glColor3uiv,auiRGB);
}
//----------------------------------------------------------------------------
void GTglColor3us (GLushort usRed, GLushort usGreen, GLushort usBlue)
{
    GT_PRE_BODY_A3(glColor3us,usRed,usGreen,usBlue);
}
//----------------------------------------------------------------------------
void GTglColor3usv (const GLushort* ausRGB)
{
    GT_PRE_BODY_A1(glColor3usv,ausRGB);
}
//----------------------------------------------------------------------------
void GTglColor4b (GLbyte cRed, GLbyte cGreen, GLbyte cBlue, GLbyte cAlpha)
{
    GT_PRE_BODY_A4(glColor4b,cRed,cGreen,cBlue,cAlpha);
}
//----------------------------------------------------------------------------
void GTglColor4bv (const GLbyte* acRGBA)
{
    GT_PRE_BODY_A1(glColor4bv,acRGBA);
}
//----------------------------------------------------------------------------
void GTglColor4d (GLdouble dRed, GLdouble dGreen, GLdouble dBlue,
    GLdouble dAlpha)
{
    GT_PRE_BODY_A4(glColor4d,dRed,dGreen,dBlue,dAlpha);
}
//----------------------------------------------------------------------------
void GTglColor4dv (const GLdouble* adRGBA)
{
    GT_PRE_BODY_A1(glColor4dv,adRGBA);
}
//----------------------------------------------------------------------------
void GTglColor4f (GLfloat fRed, GLfloat fGreen, GLfloat fBlue, GLfloat fAlpha)
{
    GT_PRE_BODY_A4(glColor4f,fRed,fGreen,fBlue,fAlpha);
}
//----------------------------------------------------------------------------
void GTglColor4fv (const GLfloat* afRGBA)
{
    GT_PRE_BODY_A1(glColor4fv,afRGBA);
}
//----------------------------------------------------------------------------
void GTglColor4i (GLint iRed, GLint iGreen, GLint iBlue, GLint iAlpha)
{
    GT_PRE_BODY_A4(glColor4i,iRed,iGreen,iBlue,iAlpha);
}
//----------------------------------------------------------------------------
void GTglColor4iv (const GLint* aiRGBA)
{
    GT_PRE_BODY_A1(glColor4iv,aiRGBA);
}
//----------------------------------------------------------------------------
void GTglColor4s (GLshort sRed, GLshort sGreen, GLshort sBlue, GLshort sAlpha)
{
    GT_PRE_BODY_A4(glColor4s,sRed,sGreen,sBlue,sAlpha);
}
//----------------------------------------------------------------------------
void GTglColor4sv (const GLshort* asRGBA)
{
    GT_PRE_BODY_A1(glColor4sv,asRGBA);
}
//----------------------------------------------------------------------------
void GTglColor4ub (GLubyte ucRed, GLubyte ucGreen, GLubyte ucBlue,
    GLubyte ucAlpha)
{
    GT_PRE_BODY_A4(glColor4ub,ucRed,ucGreen,ucBlue,ucAlpha);
}
//----------------------------------------------------------------------------
void GTglColor4ubv (const GLubyte* aucRGBA)
{
    GT_PRE_BODY_A1(glColor4ubv,aucRGBA);
}
//----------------------------------------------------------------------------
void GTglColor4ui (GLuint uiRed, GLuint uiGreen, GLuint uiBlue,
    GLuint uiAlpha)
{
    GT_PRE_BODY_A4(glColor4ui,uiRed,uiGreen,uiBlue,uiAlpha);
}
//----------------------------------------------------------------------------
void GTglColor4uiv (const GLuint* auiRGBA)
{
    GT_PRE_BODY_A1(glColor4uiv,auiRGBA);
}
//----------------------------------------------------------------------------
void GTglColor4us (GLushort usRed, GLushort usGreen, GLushort usBlue,
    GLushort usAlpha)
{
    GT_PRE_BODY_A4(glColor4us,usRed,usGreen,usBlue,usAlpha);
}
//----------------------------------------------------------------------------
void GTglColor4usv (const GLushort* ausRGBA)
{
    GT_PRE_BODY_A1(glColor4usv,ausRGBA);
}
//----------------------------------------------------------------------------
void GTglColorMask (GLboolean ucRed, GLboolean ucGreen, GLboolean ucBlue,
    GLboolean ucAlpha)
{
    GT_PRE_BODY_A4(glColorMask,ucRed,ucGreen,ucBlue,ucAlpha);
}
//----------------------------------------------------------------------------
void GTglColorMaterial (GLenum eFace, GLenum eMode)
{
    GT_PRE_BODY_A2(glColorMaterial,eFace,eMode);
}
//----------------------------------------------------------------------------
void GTglColorPointer (GLint iSize, GLenum eType, GLsizei iStride,
    const GLvoid* pvPointer)
{
    GT_PRE_BODY_A4(glColorPointer,iSize,eType,iStride,pvPointer);
}
//----------------------------------------------------------------------------
void GTglCopyPixels (GLint iX, GLint iY, GLsizei iWidth, GLsizei iHeight,
    GLenum eType)
{
    GT_PRE_BODY_A5(glCopyPixels,iX,iY,iWidth,iHeight,eType);
}
//----------------------------------------------------------------------------
void GTglCopyTexImage1D (GLenum eTarget, GLint iLevel, GLenum eInternalFormat,
    GLint iX, GLint iY, GLsizei iWidth, GLint iBorder)
{
    GT_PRE_BODY_A7(glCopyTexImage1D,eTarget,iLevel,eInternalFormat,iX,iY,
        iWidth,iBorder);
}
//----------------------------------------------------------------------------
void GTglCopyTexImage2D (GLenum eTarget, GLint iLevel, GLenum eInternalFormat,
    GLint iX, GLint iY, GLsizei iWidth, GLsizei iHeight, GLint iBorder)
{
    GT_PRE_BODY_A8(glCopyTexImage2D,eTarget,iLevel,eInternalFormat,iX,iY,
        iWidth,iHeight,iBorder);
}
//----------------------------------------------------------------------------
void GTglCopyTexSubImage1D (GLenum eTarget, GLint iLevel, GLint iXOffset,
    GLint iX, GLint iY, GLsizei iWidth)
{
    GT_PRE_BODY_A6(glCopyTexSubImage1D,eTarget,iLevel,iXOffset,iX,iY,iWidth);
}
//----------------------------------------------------------------------------
void GTglCopyTexSubImage2D (GLenum eTarget, GLint iLevel, GLint iXOffset,
    GLint iYOffset, GLint iX, GLint iY, GLsizei iWidth, GLsizei iHeight)
{
    GT_PRE_BODY_A8(glCopyTexSubImage2D,eTarget,iLevel,iXOffset,iYOffset,iX,iY,
        iWidth,iHeight);
}
//----------------------------------------------------------------------------
void GTglCullFace (GLenum eMode)
{
    GT_PRE_BODY_A1(glCullFace,eMode);
}
//----------------------------------------------------------------------------
void GTglDeleteLists (GLuint uiList, GLsizei iRange)
{
    GT_PRE_BODY_A2(glDeleteLists,uiList,iRange);
}
//----------------------------------------------------------------------------
void GTglDeleteTextures (GLsizei iNumTextures, const GLuint* auiTextures)
{
    GT_PRE_BODY_A2(glDeleteTextures,iNumTextures,auiTextures);
}
//----------------------------------------------------------------------------
void GTglDepthFunc (GLenum eFunction)
{
    GT_PRE_BODY_A1(glDepthFunc,eFunction);
}
//----------------------------------------------------------------------------
void GTglDepthMask (GLboolean ucMask)
{
    GT_PRE_BODY_A1(glDepthMask,ucMask);
}
//----------------------------------------------------------------------------
void GTglDepthRange (GLclampd dZNear, GLclampd dZFar)
{
    GT_PRE_BODY_A2(glDepthRange,dZNear,dZFar);
}
//----------------------------------------------------------------------------
void GTglDisable (GLenum eCapability)
{
    GT_PRE_BODY_A1(glDisable,eCapability);
}
//----------------------------------------------------------------------------
void GTglDisableClientState (GLenum eArray)
{
    GT_PRE_BODY_A1(glDisableClientState,eArray);
}
//----------------------------------------------------------------------------
void GTglDrawArrays (GLenum eMode, GLint iFirst, GLsizei iCount)
{
    GT_PRE_BODY_A3(glDrawArrays,eMode,iFirst,iCount);
}
//----------------------------------------------------------------------------
void GTglDrawBuffer (GLenum eMode)
{
    GT_PRE_BODY_A1(glDrawBuffer,eMode);
}
//----------------------------------------------------------------------------
void GTglDrawElements (GLenum eMode, GLsizei iCount, GLenum eType,
    const GLvoid* pvIndices)
{
    GT_PRE_BODY_A4(glDrawElements,eMode,iCount,eType,pvIndices);
}
//----------------------------------------------------------------------------
void GTglDrawPixels (GLsizei iWidth, GLsizei iHeight, GLenum eFormat,
    GLenum eType, const GLvoid* pvPixels)
{
    GT_PRE_BODY_A5(glDrawPixels,iWidth,iHeight,eFormat,eType,pvPixels);
}
//----------------------------------------------------------------------------
void GTglEdgeFlag (GLboolean ucFlag)
{
    GT_PRE_BODY_A1(glEdgeFlag,ucFlag);
}
//----------------------------------------------------------------------------
void GTglEdgeFlagv (const GLboolean* aucFlag)
{
    GT_PRE_BODY_A1(glEdgeFlagv,aucFlag);
}
//----------------------------------------------------------------------------
void GTglEdgeFlagPointer (GLsizei iStride, const GLvoid* pvPointer)
{
    GT_PRE_BODY_A2(glEdgeFlagPointer,iStride,pvPointer);
}
//----------------------------------------------------------------------------
void GTglEnable (GLenum eCapability)
{
    GT_PRE_BODY_A1(glEnable,eCapability);
}
//----------------------------------------------------------------------------
void GTglEnableClientState (GLenum eArray)
{
    GT_PRE_BODY_A1(glEnableClientState,eArray);
}
//----------------------------------------------------------------------------
void GTglEnd ()
{
    GT_PRE_BODY_A0(glEnd);
}
//----------------------------------------------------------------------------
void GTglEndList ()
{
    GT_PRE_BODY_A0(glEndList);
}
//----------------------------------------------------------------------------
void GTglEvalCoord1f (GLfloat fU)
{
    GT_PRE_BODY_A1(glEvalCoord1f,fU);
}
//----------------------------------------------------------------------------
void GTglEvalCoord1fv (const GLfloat* pfU)
{
    GT_PRE_BODY_A1(glEvalCoord1fv,pfU);
}
//----------------------------------------------------------------------------
void GTglEvalCoord1d (GLdouble dU)
{
    GT_PRE_BODY_A1(glEvalCoord1d,dU);
}
//----------------------------------------------------------------------------
void GTglEvalCoord1dv (const GLdouble* pdU)
{
    GT_PRE_BODY_A1(glEvalCoord1dv,pdU);
}
//----------------------------------------------------------------------------
void GTglEvalCoord2f (GLfloat fU, GLfloat fV)
{
    GT_PRE_BODY_A2(glEvalCoord2f,fU,fV);
}
//----------------------------------------------------------------------------
void GTglEvalCoord2fv (const GLfloat* afUV)
{
    GT_PRE_BODY_A1(glEvalCoord2fv,afUV);
}
//----------------------------------------------------------------------------
void GTglEvalCoord2d (GLdouble dU, GLdouble dV)
{
    GT_PRE_BODY_A2(glEvalCoord2d,dU,dV);
}
//----------------------------------------------------------------------------
void GTglEvalCoord2dv (const GLdouble* adUV)
{
    GT_PRE_BODY_A1(glEvalCoord2dv,adUV);
}
//----------------------------------------------------------------------------
void GTglEvalMesh1 (GLenum eMode, GLint i1, GLint i2)
{
    GT_PRE_BODY_A3(glEvalMesh1,eMode,i1,i2);
}
//----------------------------------------------------------------------------
void GTglEvalMesh2 (GLenum eMode, GLint i1, GLint i2, GLint j1, GLint j2)
{
    GT_PRE_BODY_A5(glEvalMesh2,eMode,i1,i2,j1,j2);
}
//----------------------------------------------------------------------------
void GTglEvalPoint1 (GLint i)
{
    GT_PRE_BODY_A1(glEvalPoint1,i);
}
//----------------------------------------------------------------------------
void GTglEvalPoint2 (GLint i, GLint j)
{
    GT_PRE_BODY_A2(glEvalPoint2,i,j);
}
//----------------------------------------------------------------------------
void GTglFeedbackBuffer (GLsizei iSize, GLenum eType, GLfloat* afBuffer)
{
    GT_PRE_BODY_A3(glFeedbackBuffer,iSize,eType,afBuffer);
}
//----------------------------------------------------------------------------
void GTglFinish ()
{
    GT_PRE_BODY_A0(glFinish);
}
//----------------------------------------------------------------------------
void GTglFlush ()
{
    GT_PRE_BODY_A0(glFlush);
}
//----------------------------------------------------------------------------
void GTglFogf (GLenum ePName, GLfloat fParam)
{
    GT_PRE_BODY_A2(glFogf,ePName,fParam);
}
//----------------------------------------------------------------------------
void GTglFogfv (GLenum ePName, const GLfloat* afParam)
{
    GT_PRE_BODY_A2(glFogfv,ePName,afParam);
}
//----------------------------------------------------------------------------
void GTglFogi (GLenum ePName, int iParam)
{
    GT_PRE_BODY_A2(glFogi,ePName,iParam);
}
//----------------------------------------------------------------------------
void GTglFogiv (GLenum ePName, const int* aiParam)
{
    GT_PRE_BODY_A2(glFogiv,ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTglFrontFace (GLenum eMode)
{
    GT_PRE_BODY_A1(glFrontFace,eMode);
}
//----------------------------------------------------------------------------
void GTglFrustum (GLdouble dLeft, GLdouble dRight, GLdouble dBottom,
    GLdouble dTop, GLdouble dZNear, GLdouble dZFar)
{
    GT_PRE_BODY_A6(glFrustum,dLeft,dRight,dBottom,dTop,dZNear,dZFar);
}
//----------------------------------------------------------------------------
GLuint GTglGenLists (GLsizei iRange)
{
    GT_PRE_BODY_A1_RET(glGenLists,iRange,GLuint);
}
//----------------------------------------------------------------------------
void GTglGenTextures (GLsizei iNumTextures, GLuint* auiTexture)
{
    GT_PRE_BODY_A2(glGenTextures,iNumTextures,auiTexture);
}
//----------------------------------------------------------------------------
void GTglGetBooleanv (GLenum ePName, GLboolean* aucParam)
{
    GT_PRE_BODY_A2(glGetBooleanv,ePName,aucParam);
}
//----------------------------------------------------------------------------
void GTglGetClipPlane (GLenum ePlane, GLdouble* adEquation)
{
    GT_PRE_BODY_A2(glGetClipPlane,ePlane,adEquation);
}
//----------------------------------------------------------------------------
void GTglGetDoublev (GLenum ePName, GLdouble* adParam)
{
    GT_PRE_BODY_A2(glGetDoublev,ePName,adParam);
}
//----------------------------------------------------------------------------
GLenum GTglGetError ()
{
    return glGetError();
}
//----------------------------------------------------------------------------
void GTglGetFloatv (GLenum ePName, GLfloat* afParam)
{
    GT_PRE_BODY_A2(glGetFloatv,ePName,afParam);
}
//----------------------------------------------------------------------------
void GTglGetIntegerv (GLenum ePName, int* aiParam)
{
    GT_PRE_BODY_A2(glGetIntegerv,ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTglGetLightfv (GLenum eLight, GLenum ePName, GLfloat* afParam)
{
    GT_PRE_BODY_A3(glGetLightfv,eLight,ePName,afParam);
}
//----------------------------------------------------------------------------
void GTglGetLightiv (GLenum eLight, GLenum ePName, int* aiParam)
{
    GT_PRE_BODY_A3(glGetLightiv,eLight,ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTglGetMapdv (GLenum eTarget, GLenum eQuery, GLdouble* adValue)
{
    GT_PRE_BODY_A3(glGetMapdv,eTarget,eQuery,adValue);
}
//----------------------------------------------------------------------------
void GTglGetMapfv (GLenum eTarget, GLenum eQuery, GLfloat* afValue)
{
    GT_PRE_BODY_A3(glGetMapfv,eTarget,eQuery,afValue);
}
//----------------------------------------------------------------------------
void GTglGetMapiv (GLenum eTarget, GLenum eQuery, GLint* aiValue)
{
    GT_PRE_BODY_A3(glGetMapiv,eTarget,eQuery,aiValue);
}
//----------------------------------------------------------------------------
void GTglGetMaterialfv (GLenum eFace, GLenum ePName, GLfloat* afParam)
{
    GT_PRE_BODY_A3(glGetMaterialfv,eFace,ePName,afParam);
}
//----------------------------------------------------------------------------
void GTglGetMaterialiv (GLenum eFace, GLenum ePName, GLint* aiParam)
{
    GT_PRE_BODY_A3(glGetMaterialiv,eFace,ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTglGetPixelMapfv (GLenum eMap, GLfloat* afValue)
{
    GT_PRE_BODY_A2(glGetPixelMapfv,eMap,afValue);
}
//----------------------------------------------------------------------------
void GTglGetPixelMapuiv (GLenum eMap, GLuint* auiValue)
{
    GT_PRE_BODY_A2(glGetPixelMapuiv,eMap,auiValue);
}
//----------------------------------------------------------------------------
void GTglGetPixelMapusv (GLenum eMap, GLushort* ausValue)
{
    GT_PRE_BODY_A2(glGetPixelMapusv,eMap,ausValue);
}
//----------------------------------------------------------------------------
void GTglGetPointerv (GLenum ePName, GLvoid** apvParam)
{
    GT_PRE_BODY_A2(glGetPointerv,ePName,apvParam);
}
//----------------------------------------------------------------------------
void GTglGetPolygonStipple (GLubyte* aucPattern)
{
    GT_PRE_BODY_A1(glGetPolygonStipple,aucPattern);
}
//----------------------------------------------------------------------------
const GLubyte* GTglGetString (GLenum eName)
{
    GT_PRE_BODY_A1_RET(glGetString,eName,const GLubyte*);
}
//----------------------------------------------------------------------------
void GTglGetTexEnvfv (GLenum eTarget, GLenum ePName, GLfloat* afParam)
{
    GT_PRE_BODY_A3(glGetTexEnvfv,eTarget,ePName,afParam);
}
//----------------------------------------------------------------------------
void GTglGetTexEnviv (GLenum eTarget, GLenum ePName, GLint* aiParam)
{
    GT_PRE_BODY_A3(glGetTexEnviv,eTarget,ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTglGetTexGendv (GLenum eCoord, GLenum ePName, GLdouble* adParam)
{
    GT_PRE_BODY_A3(glGetTexGendv,eCoord,ePName,adParam);
}
//----------------------------------------------------------------------------
void GTglGetTexGenfv (GLenum eCoord, GLenum ePName, GLfloat* afParam)
{
    GT_PRE_BODY_A3(glGetTexGenfv,eCoord,ePName,afParam);
}
//----------------------------------------------------------------------------
void GTglGetTexGeniv (GLenum eCoord, GLenum ePName, GLint* aiParam)
{
    GT_PRE_BODY_A3(glGetTexGeniv,eCoord,ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTglGetTexImage (GLenum eTarget, GLint iLevel, GLenum eFormat,
    GLenum eType, GLvoid* pvPixels)
{
    GT_PRE_BODY_A5(glGetTexImage,eTarget,iLevel,eFormat,eType,pvPixels);
}
//----------------------------------------------------------------------------
void GTglGetTexLevelParameterfv (GLenum eTarget, GLint iLevel, GLenum ePName,
    GLfloat* afParam)
{
    GT_PRE_BODY_A4(glGetTexLevelParameterfv,eTarget,iLevel,ePName,afParam);
}
//----------------------------------------------------------------------------
void GTglGetTexLevelParameteriv (GLenum eTarget, GLint iLevel, GLenum ePName,
    GLint* aiParam)
{
    GT_PRE_BODY_A4(glGetTexLevelParameteriv,eTarget,iLevel,ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTglGetTexParameterfv (GLenum eTarget, GLenum ePName, GLfloat* afParam)
{
    GT_PRE_BODY_A3(glGetTexParameterfv,eTarget,ePName,afParam);
}
//----------------------------------------------------------------------------
void GTglGetTexParameteriv (GLenum eTarget, GLenum ePName, GLint* aiParam)
{
    GT_PRE_BODY_A3(glGetTexParameteriv,eTarget,ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTglHint (GLenum eTarget, GLenum eMode)
{
    GT_PRE_BODY_A2(glHint,eTarget,eMode);
}
//----------------------------------------------------------------------------
void GTglIndexMask (GLuint uiMask)
{
    GT_PRE_BODY_A1(glIndexMask,uiMask);
}
//----------------------------------------------------------------------------
void GTglIndexPointer (GLenum eType, GLsizei iStride, const GLvoid* pvPointer)
{
    GT_PRE_BODY_A3(glIndexPointer,eType,iStride,pvPointer);
}
//----------------------------------------------------------------------------
void GTglIndexd (GLdouble dIndex)
{
    GT_PRE_BODY_A1(glIndexd,dIndex);
}
//----------------------------------------------------------------------------
void GTglIndexdv (const GLdouble* pdIndex)
{
    GT_PRE_BODY_A1(glIndexdv,pdIndex);
}
//----------------------------------------------------------------------------
void GTglIndexf (GLfloat fIndex)
{
    GT_PRE_BODY_A1(glIndexf,fIndex);
}
//----------------------------------------------------------------------------
void GTglIndexfv (const GLfloat* pfIndex)
{
    GT_PRE_BODY_A1(glIndexfv,pfIndex);
}
//----------------------------------------------------------------------------
void GTglIndexi (GLint iIndex)
{
    GT_PRE_BODY_A1(glIndexi,iIndex);
}
//----------------------------------------------------------------------------
void GTglIndexiv (const GLint* piIndex)
{
    GT_PRE_BODY_A1(glIndexiv,piIndex);
}
//----------------------------------------------------------------------------
void GTglIndexs (GLshort sIndex)
{
    GT_PRE_BODY_A1(glIndexs,sIndex);
}
//----------------------------------------------------------------------------
void GTglIndexsv (const GLshort* psIndex)
{
    GT_PRE_BODY_A1(glIndexsv,psIndex);
}
//----------------------------------------------------------------------------
void GTglIndexub (GLubyte ucIndex)
{
    GT_PRE_BODY_A1(glIndexub,ucIndex);
}
//----------------------------------------------------------------------------
void GTglIndexubv (const GLubyte* pucIndex)
{
    GT_PRE_BODY_A1(glIndexubv,pucIndex);
}
//----------------------------------------------------------------------------
void GTglInitNames ()
{
    GT_PRE_BODY_A0(glInitNames);
}
//----------------------------------------------------------------------------
void GTglInterleavedArrays (GLenum eFormat, GLsizei iStride,
    const GLvoid* pvPointer)
{
    GT_PRE_BODY_A3(glInterleavedArrays,eFormat,iStride,pvPointer);
}
//----------------------------------------------------------------------------
GLboolean GTglIsEnabled (GLenum eCapability)
{
    GT_PRE_BODY_A1_RET(glIsEnabled,eCapability,GLboolean);
}
//----------------------------------------------------------------------------
GLboolean GTglIsList (GLuint uiList)
{
    GT_PRE_BODY_A1_RET(glIsList,uiList,GLboolean);
}
//----------------------------------------------------------------------------
GLboolean GTglIsTexture (GLuint uiTexture)
{
    GT_PRE_BODY_A1_RET(glIsTexture,uiTexture,GLboolean);
}
//----------------------------------------------------------------------------
void GTglLightModelf (GLenum ePName, GLfloat fParam)
{
    GT_PRE_BODY_A2(glLightModelf,ePName,fParam);
}
//----------------------------------------------------------------------------
void GTglLightModelfv (GLenum ePName, const GLfloat* afParam)
{
    GT_PRE_BODY_A2(glLightModelfv,ePName,afParam);
}
//----------------------------------------------------------------------------
void GTglLightModeli (GLenum ePName, GLint iParam)
{
    GT_PRE_BODY_A2(glLightModeli,ePName,iParam);
}
//----------------------------------------------------------------------------
void GTglLightModeliv (GLenum ePName, const GLint* aiParam)
{
    GT_PRE_BODY_A2(glLightModeliv,ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTglLightf (GLenum eLight, GLenum ePName, GLfloat fParam)
{
    GT_PRE_BODY_A3(glLightf,eLight,ePName,fParam);
}
//----------------------------------------------------------------------------
void GTglLightfv (GLenum eLight, GLenum ePName, const GLfloat* afParam)
{
    GT_PRE_BODY_A3(glLightfv,eLight,ePName,afParam);
}
//----------------------------------------------------------------------------
void GTglLighti (GLenum eLight, GLenum ePName, GLint iParam)
{
    GT_PRE_BODY_A3(glLighti,eLight,ePName,iParam);
}
//----------------------------------------------------------------------------
void GTglLightiv (GLenum eLight, GLenum ePName, const GLint* aiParam)
{
    GT_PRE_BODY_A3(glLightiv,eLight,ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTglLineStipple (GLint iFactor, GLushort usPattern)
{
    GT_PRE_BODY_A2(glLineStipple,iFactor,usPattern);
}
//----------------------------------------------------------------------------
void GTglLineWidth (GLfloat fWidth)
{
    GT_PRE_BODY_A1(glLineWidth,fWidth);
}
//----------------------------------------------------------------------------
void GTglListBase (GLuint uiBase)
{
    GT_PRE_BODY_A1(glListBase,uiBase);
}
//----------------------------------------------------------------------------
void GTglLoadIdentity ()
{
    GT_PRE_BODY_A0(glLoadIdentity);
}
//----------------------------------------------------------------------------
void GTglLoadMatrixd (const GLdouble* adMatrix)
{
    GT_PRE_BODY_A1(glLoadMatrixd,adMatrix);
}
//----------------------------------------------------------------------------
void GTglLoadMatrixf (const GLfloat* afMatrix)
{
    GT_PRE_BODY_A1(glLoadMatrixf,afMatrix);
}
//----------------------------------------------------------------------------
void GTglLoadName (GLuint uiName)
{
    GT_PRE_BODY_A1(glLoadName,uiName);
}
//----------------------------------------------------------------------------
void GTglLogicOp (GLenum eOpCode)
{
    GT_PRE_BODY_A1(glLogicOp,eOpCode);
}
//----------------------------------------------------------------------------
void GTglMap1d (GLenum eTarget, GLdouble dU1, GLdouble dU2, GLint iStride,
    GLint iOrder, const GLdouble* adPoints)
{
    GT_PRE_BODY_A6(glMap1d,eTarget,dU1,dU2,iStride,iOrder,adPoints);
}
//----------------------------------------------------------------------------
void GTglMap1f (GLenum eTarget, GLfloat fU1, GLfloat fU2, GLint iStride,
    GLint iOrder, const GLfloat* afPoints)
{
    GT_PRE_BODY_A6(glMap1f,eTarget,fU1,fU2,iStride,iOrder,afPoints);
}
//----------------------------------------------------------------------------
void GTglMap2d (GLenum eTarget, GLdouble dU1, GLdouble dU2, GLint iUStride,
    GLint iUOrder, GLdouble dV1, GLdouble dV2, GLint iVStride, GLint iVOrder,
    const GLdouble* adPoints)
{
    GT_PRE_BODY_A10(glMap2d,eTarget,dU1,dU2,iUStride,iUOrder,dV1,dV2,
        iVStride,iVOrder,adPoints);
}
//----------------------------------------------------------------------------
void GTglMap2f (GLenum eTarget, GLfloat fU1, GLfloat fU2, GLint iUStride,
    GLint iUOrder, GLfloat fV1, GLfloat fV2, GLint iVStride, GLint iVOrder,
    const GLfloat* afPoints)
{
    GT_PRE_BODY_A10(glMap2f,eTarget,fU1,fU2,iUStride,iUOrder,fV1,fV2,
        iVStride,iVOrder,afPoints);
}
//----------------------------------------------------------------------------
void GTglMapGrid1d (GLint iNumUPartitions, GLdouble dU1, GLdouble dU2)
{
    GT_PRE_BODY_A3(glMapGrid1d,iNumUPartitions,dU1,dU2);
}
//----------------------------------------------------------------------------
void GTglMapGrid1f (GLint iNumUPartitions, GLfloat fU1, GLfloat fU2)
{
    GT_PRE_BODY_A3(glMapGrid1f,iNumUPartitions,fU1,fU2);
}
//----------------------------------------------------------------------------
void GTglMapGrid2d (GLint iNumUPartitions, GLdouble dU1, GLdouble dU2,
    GLint iNumVPartitions, GLdouble dV1, GLdouble dV2)
{
    GT_PRE_BODY_A6(glMapGrid2d,iNumUPartitions,dU1,dU2,iNumVPartitions,dV1,
        dV2);
}
//----------------------------------------------------------------------------
void GTglMapGrid2f (GLint iNumUPartitions, GLfloat fU1, GLfloat fU2,
    GLint iNumVPartitions, GLfloat fV1, GLfloat fV2)
{
    GT_PRE_BODY_A6(glMapGrid2f,iNumUPartitions,fU1,fU2,iNumVPartitions,fV1,
        fV2);
}
//----------------------------------------------------------------------------
void GTglMaterialf (GLenum eFace, GLenum ePName, GLfloat fParam)
{
    GT_PRE_BODY_A3(glMaterialf,eFace,ePName,fParam);
}
//----------------------------------------------------------------------------
void GTglMaterialfv (GLenum eFace, GLenum ePName, const GLfloat* afParam)
{
    GT_PRE_BODY_A3(glMaterialfv,eFace,ePName,afParam);
}
//----------------------------------------------------------------------------
void GTglMateriali (GLenum eFace, GLenum ePName, GLint iParam)
{
    GT_PRE_BODY_A3(glMateriali,eFace,ePName,iParam);
}
//----------------------------------------------------------------------------
void GTglMaterialiv (GLenum eFace, GLenum ePName, const GLint* aiParam)
{
    GT_PRE_BODY_A3(glMaterialiv,eFace,ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTglMatrixMode (GLenum eMode)
{
    GT_PRE_BODY_A1(glMatrixMode,eMode);
}
//----------------------------------------------------------------------------
void GTglMultMatrixd (const GLdouble* adMatrix)
{
    GT_PRE_BODY_A1(glMultMatrixd,adMatrix);
}
//----------------------------------------------------------------------------
void GTglMultMatrixf (const GLfloat* afMatrix)
{
    GT_PRE_BODY_A1(glMultMatrixf,afMatrix);
}
//----------------------------------------------------------------------------
void GTglNewList (GLuint uiList, GLenum eMode)
{
    GT_PRE_BODY_A2(glNewList,uiList,eMode);
}
//----------------------------------------------------------------------------
void GTglNormal3b (GLbyte bX, GLbyte bY, GLbyte bZ)
{
    GT_PRE_BODY_A3(glNormal3b,bX,bY,bZ);
}
//----------------------------------------------------------------------------
void GTglNormal3bv (const GLbyte* abXYZ)
{
    GT_PRE_BODY_A1(glNormal3bv,abXYZ);
}
//----------------------------------------------------------------------------
void GTglNormal3d (GLdouble dX, GLdouble dY, GLdouble dZ)
{
    GT_PRE_BODY_A3(glNormal3d,dX,dY,dZ);
}
//----------------------------------------------------------------------------
void GTglNormal3dv (const GLdouble* adXYZ)
{
    GT_PRE_BODY_A1(glNormal3dv,adXYZ);
}
//----------------------------------------------------------------------------
void GTglNormal3f (GLfloat fX, GLfloat fY, GLfloat fZ)
{
    GT_PRE_BODY_A3(glNormal3f,fX,fY,fZ);
}
//----------------------------------------------------------------------------
void GTglNormal3fv (const GLfloat* afXYZ)
{
    GT_PRE_BODY_A1(glNormal3fv,afXYZ);
}
//----------------------------------------------------------------------------
void GTglNormal3i (GLint iX, GLint iY, GLint iZ)
{
    GT_PRE_BODY_A3(glNormal3i,iX,iY,iZ);
}
//----------------------------------------------------------------------------
void GTglNormal3iv (const GLint* aiXYZ)
{
    GT_PRE_BODY_A1(glNormal3iv,aiXYZ);
}
//----------------------------------------------------------------------------
void GTglNormal3s (GLshort sX, GLshort sY, GLshort sZ)
{
    GT_PRE_BODY_A3(glNormal3s,sX,sY,sZ);
}
//----------------------------------------------------------------------------
void GTglNormal3sv (const GLshort* asXYZ)
{
    GT_PRE_BODY_A1(glNormal3sv,asXYZ);
}
//----------------------------------------------------------------------------
void GTglNormalPointer (GLenum eType, GLsizei iStride,
    const GLvoid* pvPointer)
{
    GT_PRE_BODY_A3(glNormalPointer,eType,iStride,pvPointer);
}
//----------------------------------------------------------------------------
void GTglOrtho (GLdouble dLeft, GLdouble dRight, GLdouble dBottom,
    GLdouble dTop, GLdouble dZNear, GLdouble dZFar)
{
    GT_PRE_BODY_A6(glOrtho,dLeft,dRight,dBottom,dTop,dZNear,dZFar);
}
//----------------------------------------------------------------------------
void GTglPassThrough (GLfloat fToken)
{
    GT_PRE_BODY_A1(glPassThrough,fToken);
}
//----------------------------------------------------------------------------
void GTglPixelMapfv (GLenum eMap, GLsizei iSize, const GLfloat* afValue)
{
    GT_PRE_BODY_A3(glPixelMapfv,eMap,iSize,afValue);
}
//----------------------------------------------------------------------------
void GTglPixelMapuiv (GLenum eMap, GLsizei iSize, const GLuint* auiValue)
{
    GT_PRE_BODY_A3(glPixelMapuiv,eMap,iSize,auiValue);
}
//----------------------------------------------------------------------------
void GTglPixelMapusv (GLenum eMap, GLsizei iSize, const GLushort* ausValue)
{
    GT_PRE_BODY_A3(glPixelMapusv,eMap,iSize,ausValue);
}
//----------------------------------------------------------------------------
void GTglPixelStoref (GLenum ePName, GLfloat fValue)
{
    GT_PRE_BODY_A2(glPixelStoref,ePName,fValue);
}
//----------------------------------------------------------------------------
void GTglPixelStorei (GLenum ePName, GLint iValue)
{
    GT_PRE_BODY_A2(glPixelStorei,ePName,iValue);
}
//----------------------------------------------------------------------------
void GTglPixelTransferf (GLenum ePName, GLfloat fValue)
{
    GT_PRE_BODY_A2(glPixelTransferf,ePName,fValue);
}
//----------------------------------------------------------------------------
void GTglPixelTransferi (GLenum ePName, GLint iValue)
{
    GT_PRE_BODY_A2(glPixelTransferi,ePName,iValue);
}
//----------------------------------------------------------------------------
void GTglPixelZoom (GLfloat fXFactor, GLfloat fYFactor)
{
    GT_PRE_BODY_A2(glPixelZoom,fXFactor,fYFactor);
}
//----------------------------------------------------------------------------
void GTglPointSize (GLfloat fSize)
{
    GT_PRE_BODY_A1(glPointSize,fSize);
}
//----------------------------------------------------------------------------
void GTglPolygonMode (GLenum eFace, GLenum eMode)
{
    GT_PRE_BODY_A2(glPolygonMode,eFace,eMode);
}
//----------------------------------------------------------------------------
void GTglPolygonOffset (GLfloat fScale, GLfloat fBias)
{
    GT_PRE_BODY_A2(glPolygonOffset,fScale,fBias);
}
//----------------------------------------------------------------------------
void GTglPolygonStipple (const GLubyte* aucMask)
{
    GT_PRE_BODY_A1(glPolygonStipple,aucMask);
}
//----------------------------------------------------------------------------
void GTglPopAttrib ()
{
    GT_PRE_BODY_A0(glPopAttrib);
}
//----------------------------------------------------------------------------
void GTglPopClientAttrib ()
{
    GT_PRE_BODY_A0(glPopClientAttrib);
}
//----------------------------------------------------------------------------
void GTglPopMatrix ()
{
    GT_PRE_BODY_A0(glPopMatrix);
}
//----------------------------------------------------------------------------
void GTglPopName ()
{
    GT_PRE_BODY_A0(glPopName);
}
//----------------------------------------------------------------------------
void GTglPrioritizeTextures (GLsizei iNumTextures, const GLuint* auiTextures,
    const GLclampf* afPriorities)
{
    GT_PRE_BODY_A3(glPrioritizeTextures,iNumTextures,auiTextures,
        afPriorities);
}
//----------------------------------------------------------------------------
void GTglPushAttrib (GLbitfield uiMask)
{
    GT_PRE_BODY_A1(glPushAttrib,uiMask);
}
//----------------------------------------------------------------------------
void GTglPushClientAttrib (GLbitfield uiMask)
{
    GT_PRE_BODY_A1(glPushClientAttrib,uiMask);
}
//----------------------------------------------------------------------------
void GTglPushMatrix ()
{
    GT_PRE_BODY_A0(glPushMatrix);
}
//----------------------------------------------------------------------------
void GTglPushName (GLuint uiName)
{
    GT_PRE_BODY_A1(glPushName,uiName);
}
//----------------------------------------------------------------------------
void GTglRasterPos2d (GLdouble dX, GLdouble dY)
{
    GT_PRE_BODY_A2(glRasterPos2d,dX,dY);
}
//----------------------------------------------------------------------------
void GTglRasterPos2dv (const GLdouble* adXY)
{
    GT_PRE_BODY_A1(glRasterPos2dv,adXY);
}
//----------------------------------------------------------------------------
void GTglRasterPos2f (GLfloat fX, GLfloat fY)
{
    GT_PRE_BODY_A2(glRasterPos2f,fX,fY);
}
//----------------------------------------------------------------------------
void GTglRasterPos2fv (const GLfloat* afXY)
{
    GT_PRE_BODY_A1(glRasterPos2fv,afXY);
}
//----------------------------------------------------------------------------
void GTglRasterPos2i (GLint iX, GLint iY)
{
    GT_PRE_BODY_A2(glRasterPos2i,iX,iY);
}
//----------------------------------------------------------------------------
void GTglRasterPos2iv (const GLint* aiXY)
{
    GT_PRE_BODY_A1(glRasterPos2iv,aiXY);
}
//----------------------------------------------------------------------------
void GTglRasterPos2s (GLshort sX, GLshort sY)
{
    GT_PRE_BODY_A2(glRasterPos2s,sX,sY);
}
//----------------------------------------------------------------------------
void GTglRasterPos2sv (const GLshort* asXY)
{
    GT_PRE_BODY_A1(glRasterPos2sv,asXY);
}
//----------------------------------------------------------------------------
void GTglRasterPos3d (GLdouble dX, GLdouble dY, GLdouble dZ)
{
    GT_PRE_BODY_A3(glRasterPos3d,dX,dY,dZ);
}
//----------------------------------------------------------------------------
void GTglRasterPos3dv (const GLdouble* adXYZ)
{
    GT_PRE_BODY_A1(glRasterPos3dv,adXYZ);
}
//----------------------------------------------------------------------------
void GTglRasterPos3f (GLfloat fX, GLfloat fY, GLfloat fZ)
{
    GT_PRE_BODY_A3(glRasterPos3f,fX,fY,fZ);
}
//----------------------------------------------------------------------------
void GTglRasterPos3fv (const GLfloat* afXYZ)
{
    GT_PRE_BODY_A1(glRasterPos3fv,afXYZ);
}
//----------------------------------------------------------------------------
void GTglRasterPos3i (GLint iX, GLint iY, GLint iZ)
{
    GT_PRE_BODY_A3(glRasterPos3i,iX,iY,iZ);
}
//----------------------------------------------------------------------------
void GTglRasterPos3iv (const GLint* aiXYZ)
{
    GT_PRE_BODY_A1(glRasterPos3iv,aiXYZ);
}
//----------------------------------------------------------------------------
void GTglRasterPos3s (GLshort sX, GLshort sY, GLshort sZ)
{
    GT_PRE_BODY_A3(glRasterPos3s,sX,sY,sZ);
}
//----------------------------------------------------------------------------
void GTglRasterPos3sv (const GLshort* asXYZ)
{
    GT_PRE_BODY_A1(glRasterPos3sv,asXYZ);
}
//----------------------------------------------------------------------------
void GTglRasterPos4d (GLdouble dX, GLdouble dY, GLdouble dZ, GLdouble dW)
{
    GT_PRE_BODY_A4(glRasterPos4d,dX,dY,dZ,dW);
}
//----------------------------------------------------------------------------
void GTglRasterPos4dv (const GLdouble* adXYZW)
{
    GT_PRE_BODY_A1(glRasterPos4dv,adXYZW);
}
//----------------------------------------------------------------------------
void GTglRasterPos4f (GLfloat fX, GLfloat fY, GLfloat fZ, GLfloat fW)
{
    GT_PRE_BODY_A4(glRasterPos4f,fX,fY,fZ,fW);
}
//----------------------------------------------------------------------------
void GTglRasterPos4fv (const GLfloat* afXYZW)
{
    GT_PRE_BODY_A1(glRasterPos4fv,afXYZW);
}
//----------------------------------------------------------------------------
void GTglRasterPos4i (GLint iX, GLint iY, GLint iZ, GLint iW)
{
    GT_PRE_BODY_A4(glRasterPos4i,iX,iY,iZ,iW);
}
//----------------------------------------------------------------------------
void GTglRasterPos4iv (const GLint* aiXYZW)
{
    GT_PRE_BODY_A1(glRasterPos4iv,aiXYZW);
}
//----------------------------------------------------------------------------
void GTglRasterPos4s (GLshort sX, GLshort sY, GLshort sZ, GLshort sW)
{
    GT_PRE_BODY_A4(glRasterPos4s,sX,sY,sZ,sW);
}
//----------------------------------------------------------------------------
void GTglRasterPos4sv (const GLshort* asXYZW)
{
    GT_PRE_BODY_A1(glRasterPos4sv,asXYZW);
}
//----------------------------------------------------------------------------
void GTglReadBuffer (GLenum eMode)
{
    GT_PRE_BODY_A1(glReadBuffer,eMode);
}
//----------------------------------------------------------------------------
void GTglReadPixels (GLint iX, GLint iY, GLsizei iWidth, GLsizei iHeight,
    GLenum eFormat, GLenum eType, GLvoid* pvPixels)
{
    GT_PRE_BODY_A7(glReadPixels,iX,iY,iWidth,iHeight,eFormat,eType,pvPixels);
}
//----------------------------------------------------------------------------
void GTglRectd (GLdouble dX1, GLdouble dY1, GLdouble dX2, GLdouble dY2)
{
    GT_PRE_BODY_A4(glRectd,dX1,dY1,dX2,dY2);
}
//----------------------------------------------------------------------------
void GTglRectdv (const GLdouble* adX1Y1, const GLdouble* adX2Y2)
{
    GT_PRE_BODY_A2(glRectdv,adX1Y1,adX2Y2);
}
//----------------------------------------------------------------------------
void GTglRectf (GLfloat fX1, GLfloat fY1, GLfloat fX2, GLfloat fY2)
{
    GT_PRE_BODY_A4(glRectf,fX1,fY1,fX2,fY2);
}
//----------------------------------------------------------------------------
void GTglRectfv (const GLfloat* afX1Y1, const GLfloat* afX2Y2)
{
    GT_PRE_BODY_A2(glRectfv,afX1Y1,afX2Y2);
}
//----------------------------------------------------------------------------
void GTglRecti (GLint iX1, GLint iY1, GLint iX2, GLint iY2)
{
    GT_PRE_BODY_A4(glRecti,iX1,iY1,iX2,iY2);
}
//----------------------------------------------------------------------------
void GTglRectiv (const GLint* aiX1Y1, const GLint* aiX2Y2)
{
    GT_PRE_BODY_A2(glRectiv,aiX1Y1,aiX2Y2);
}
//----------------------------------------------------------------------------
void GTglRects (GLshort sX1, GLshort sY1, GLshort sX2, GLshort sY2)
{
    GT_PRE_BODY_A4(glRects,sX1,sY1,sX2,sY2);
}
//----------------------------------------------------------------------------
void GTglRectsv (const GLshort* asX1Y1, const GLshort* asX2Y2)
{
    GT_PRE_BODY_A2(glRectsv,asX1Y1,asX2Y2);
}
//----------------------------------------------------------------------------
GLint GTglRenderMode (GLenum eMode)
{
    GT_PRE_BODY_A1_RET(glRenderMode,eMode,GLint);
}
//----------------------------------------------------------------------------
void GTglRotated (GLdouble dAngle, GLdouble dX, GLdouble dY, GLdouble dZ)
{
    GT_PRE_BODY_A4(glRotated,dAngle,dX,dY,dZ);
}
//----------------------------------------------------------------------------
void GTglRotatef (GLfloat fAngle, GLfloat fX, GLfloat fY, GLfloat fZ)
{
    GT_PRE_BODY_A4(glRotatef,fAngle,fX,fY,fZ);
}
//----------------------------------------------------------------------------
void GTglScaled (GLdouble dX, GLdouble dY, GLdouble dZ)
{
    GT_PRE_BODY_A3(glScaled,dX,dY,dZ);
}
//----------------------------------------------------------------------------
void GTglScalef (GLfloat fX, GLfloat fY, GLfloat fZ)
{
    GT_PRE_BODY_A3(glScalef,fX,fY,fZ);
}
//----------------------------------------------------------------------------
void GTglScissor (GLint iX, GLint iY, GLsizei iWidth, GLsizei iHeight)
{
    GT_PRE_BODY_A4(glScissor,iX,iY,iWidth,iHeight);
}
//----------------------------------------------------------------------------
void GTglSelectBuffer (GLsizei iSize, GLuint* auiBuffer)
{
    GT_PRE_BODY_A2(glSelectBuffer,iSize,auiBuffer);
}
//----------------------------------------------------------------------------
void GTglShadeModel (GLenum eMode)
{
    GT_PRE_BODY_A1(glShadeModel,eMode);
}
//----------------------------------------------------------------------------
void GTglStencilFunc (GLenum eFunction, GLint iReference, GLuint uiMask)
{
    GT_PRE_BODY_A3(glStencilFunc,eFunction,iReference,uiMask);
}
//----------------------------------------------------------------------------
void GTglStencilMask (GLuint uiMask)
{
    GT_PRE_BODY_A1(glStencilMask,uiMask);
}
//----------------------------------------------------------------------------
void GTglStencilOp (GLenum eFail, GLenum eZFail, GLenum eZPass)
{
    GT_PRE_BODY_A3(glStencilOp,eFail,eZFail,eZPass);
}
//----------------------------------------------------------------------------
void GTglTexCoord1d (GLdouble dS)
{
    GT_PRE_BODY_A1(glTexCoord1d,dS);
}
//----------------------------------------------------------------------------
void GTglTexCoord1dv (const GLdouble* pdS)
{
    GT_PRE_BODY_A1(glTexCoord1dv,pdS);
}
//----------------------------------------------------------------------------
void GTglTexCoord1f (GLfloat fS)
{
    GT_PRE_BODY_A1(glTexCoord1f,fS);
}
//----------------------------------------------------------------------------
void GTglTexCoord1fv (const GLfloat* pfS)
{
    GT_PRE_BODY_A1(glTexCoord1fv,pfS);
}
//----------------------------------------------------------------------------
void GTglTexCoord1i (GLint iS)
{
    GT_PRE_BODY_A1(glTexCoord1i,iS);
}
//----------------------------------------------------------------------------
void GTglTexCoord1iv (const GLint* piS)
{
    GT_PRE_BODY_A1(glTexCoord1iv,piS);
}
//----------------------------------------------------------------------------
void GTglTexCoord1s (GLshort sS)
{
    GT_PRE_BODY_A1(glTexCoord1s,sS);
}
//----------------------------------------------------------------------------
void GTglTexCoord1sv (const GLshort* psS)
{
    GT_PRE_BODY_A1(glTexCoord1sv,psS);
}
//----------------------------------------------------------------------------
void GTglTexCoord2d (GLdouble dS, GLdouble dT)
{
    GT_PRE_BODY_A2(glTexCoord2d,dS,dT);
}
//----------------------------------------------------------------------------
void GTglTexCoord2dv (const GLdouble* adST)
{
    GT_PRE_BODY_A1(glTexCoord2dv,adST);
}
//----------------------------------------------------------------------------
void GTglTexCoord2f (GLfloat fS, GLfloat fT)
{
    GT_PRE_BODY_A2(glTexCoord2f,fS,fT);
}
//----------------------------------------------------------------------------
void GTglTexCoord2fv (const GLfloat* afST)
{
    GT_PRE_BODY_A1(glTexCoord2fv,afST);
}
//----------------------------------------------------------------------------
void GTglTexCoord2i (GLint iS, GLint iT)
{
    GT_PRE_BODY_A2(glTexCoord2i,iS,iT);
}
//----------------------------------------------------------------------------
void GTglTexCoord2iv (const GLint* aiST)
{
    GT_PRE_BODY_A1(glTexCoord2iv,aiST);
}
//----------------------------------------------------------------------------
void GTglTexCoord2s (GLshort sS, GLshort sT)
{
    GT_PRE_BODY_A2(glTexCoord2s,sS,sT);
}
//----------------------------------------------------------------------------
void GTglTexCoord2sv (const GLshort* asST)
{
    GT_PRE_BODY_A1(glTexCoord2sv,asST);
}
//----------------------------------------------------------------------------
void GTglTexCoord3d (GLdouble dS, GLdouble dT, GLdouble dR)
{
    GT_PRE_BODY_A3(glTexCoord3d,dS,dT,dR);
}
//----------------------------------------------------------------------------
void GTglTexCoord3dv (const GLdouble* adSTR)
{
    GT_PRE_BODY_A1(glTexCoord3dv,adSTR);
}
//----------------------------------------------------------------------------
void GTglTexCoord3f (GLfloat fS, GLfloat fT, GLfloat fR)
{
    GT_PRE_BODY_A3(glTexCoord3f,fS,fT,fR);
}
//----------------------------------------------------------------------------
void GTglTexCoord3fv (const GLfloat* afSTR)
{
    GT_PRE_BODY_A1(glTexCoord3fv,afSTR);
}
//----------------------------------------------------------------------------
void GTglTexCoord3i (GLint iS, GLint iT, GLint iR)
{
    GT_PRE_BODY_A3(glTexCoord3i,iS,iT,iR);
}
//----------------------------------------------------------------------------
void GTglTexCoord3iv (const GLint* aiSTR)
{
    GT_PRE_BODY_A1(glTexCoord3iv,aiSTR);
}
//----------------------------------------------------------------------------
void GTglTexCoord3s (GLshort sS, GLshort sT, GLshort sR)
{
    GT_PRE_BODY_A3(glTexCoord3s,sS,sT,sR);
}
//----------------------------------------------------------------------------
void GTglTexCoord3sv (const GLshort* asSTR)
{
    GT_PRE_BODY_A1(glTexCoord3sv,asSTR);
}
//----------------------------------------------------------------------------
void GTglTexCoord4d (GLdouble dS, GLdouble dT, GLdouble dR, GLdouble dQ)
{
    GT_PRE_BODY_A4(glTexCoord4d,dS,dT,dR,dQ);
}
//----------------------------------------------------------------------------
void GTglTexCoord4dv (const GLdouble* adSTRQ)
{
    GT_PRE_BODY_A1(glTexCoord4dv,adSTRQ);
}
//----------------------------------------------------------------------------
void GTglTexCoord4f (GLfloat fS, GLfloat fT, GLfloat fR, GLfloat fQ)
{
    GT_PRE_BODY_A4(glTexCoord4f,fS,fT,fR,fQ);
}
//----------------------------------------------------------------------------
void GTglTexCoord4fv (const GLfloat* afSTRQ)
{
    GT_PRE_BODY_A1(glTexCoord4fv,afSTRQ);
}
//----------------------------------------------------------------------------
void GTglTexCoord4i (GLint iS, GLint iT, GLint iR, GLint iQ)
{
    GT_PRE_BODY_A4(glTexCoord4i,iS,iT,iR,iQ);
}
//----------------------------------------------------------------------------
void GTglTexCoord4iv (const GLint* aiSTRQ)
{
    GT_PRE_BODY_A1(glTexCoord4iv,aiSTRQ);
}
//----------------------------------------------------------------------------
void GTglTexCoord4s (GLshort sS, GLshort sT, GLshort sR, GLshort sQ)
{
    GT_PRE_BODY_A4(glTexCoord4s,sS,sT,sR,sQ);
}
//----------------------------------------------------------------------------
void GTglTexCoord4sv (const GLshort* asSTRQ)
{
    GT_PRE_BODY_A1(glTexCoord4sv,asSTRQ);
}
//----------------------------------------------------------------------------
void GTglTexCoordPointer (GLint iSize, GLenum eType, GLsizei iStride,
    const GLvoid* pvPointer)
{
    GT_PRE_BODY_A4(glTexCoordPointer,iSize,eType,iStride,pvPointer);
}
//----------------------------------------------------------------------------
void GTglTexEnvf (GLenum eTarget, GLenum ePName, GLfloat fParam)
{
    GT_PRE_BODY_A3(glTexEnvf,eTarget,ePName,fParam);
}
//----------------------------------------------------------------------------
void GTglTexEnvfv (GLenum eTarget, GLenum ePName, const GLfloat* afParam)
{
    GT_PRE_BODY_A3(glTexEnvfv,eTarget,ePName,afParam);
}
//----------------------------------------------------------------------------
void GTglTexEnvi (GLenum eTarget, GLenum ePName, GLint iParam)
{
    GT_PRE_BODY_A3(glTexEnvi,eTarget,ePName,iParam);
}
//----------------------------------------------------------------------------
void GTglTexEnviv (GLenum eTarget, GLenum ePName, const GLint* aiParam)
{
    GT_PRE_BODY_A3(glTexEnviv,eTarget,ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTglTexGend (GLenum eCoord, GLenum ePName, GLdouble dParam)
{
    GT_PRE_BODY_A3(glTexGend,eCoord,ePName,dParam);
}
//----------------------------------------------------------------------------
void GTglTexGendv (GLenum eCoord, GLenum ePName, const GLdouble* adParam)
{
    GT_PRE_BODY_A3(glTexGendv,eCoord,ePName,adParam);
}
//----------------------------------------------------------------------------
void GTglTexGenf (GLenum eCoord, GLenum ePName, GLfloat fParam)
{
    GT_PRE_BODY_A3(glTexGenf,eCoord,ePName,fParam);
}
//----------------------------------------------------------------------------
void GTglTexGenfv (GLenum eCoord, GLenum ePName, const GLfloat* afParam)
{
    GT_PRE_BODY_A3(glTexGenfv,eCoord,ePName,afParam);
}
//----------------------------------------------------------------------------
void GTglTexGeni (GLenum eCoord, GLenum ePName, GLint iParam)
{
    GT_PRE_BODY_A3(glTexGeni,eCoord,ePName,iParam);
}
//----------------------------------------------------------------------------
void GTglTexGeniv (GLenum eCoord, GLenum ePName, const GLint* aiParam)
{
    GT_PRE_BODY_A3(glTexGeniv,eCoord,ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTglTexImage1D (GLenum eTarget, GLint iLevel, GLint iInternalFormat,
    GLsizei iWidth, GLint iBorder, GLenum eFormat, GLenum eType,
    const GLvoid* pvPixels)
{
    GT_PRE_BODY_A8(glTexImage1D,eTarget,iLevel,iInternalFormat,iWidth,
        iBorder,eFormat,eType,pvPixels);
}
//----------------------------------------------------------------------------
void GTglTexImage2D (GLenum eTarget, GLint iLevel, GLint iInternalFormat,
    GLsizei iWidth, GLsizei iHeight, GLint iBorder, GLenum eFormat,
    GLenum eType, const GLvoid* pvPixels)
{
    GT_PRE_BODY_A9(glTexImage2D,eTarget,iLevel,iInternalFormat,iWidth,
        iHeight,iBorder,eFormat,eType,pvPixels);
}
//----------------------------------------------------------------------------
void GTglTexParameterf (GLenum eTarget, GLenum ePName, GLfloat fParam)
{
    GT_PRE_BODY_A3(glTexParameterf,eTarget,ePName,fParam);
}
//----------------------------------------------------------------------------
void GTglTexParameterfv (GLenum eTarget, GLenum ePName,
    const GLfloat* afParam)
{
    GT_PRE_BODY_A3(glTexParameterfv,eTarget,ePName,afParam);
}
//----------------------------------------------------------------------------
void GTglTexParameteri (GLenum eTarget, GLenum ePName, GLint iParam)
{
    GT_PRE_BODY_A3(glTexParameteri,eTarget,ePName,iParam);
}
//----------------------------------------------------------------------------
void GTglTexParameteriv (GLenum eTarget, GLenum ePName, const GLint* aiParam)
{
    GT_PRE_BODY_A3(glTexParameteriv,eTarget,ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTglTexSubImage1D (GLenum eTarget, GLint iLevel, GLint iXOffset,
    GLsizei iWidth, GLenum eFormat, GLenum eType, const GLvoid* pvPixels)
{
    GT_PRE_BODY_A7(glTexSubImage1D,eTarget,iLevel,iXOffset,iWidth,eFormat,
        eType,pvPixels);
}
//----------------------------------------------------------------------------
void GTglTexSubImage2D (GLenum eTarget, GLint iLevel, GLint iXOffset,
    GLint iYOffset, GLsizei iWidth, GLsizei iHeight, GLenum eFormat,
    GLenum eType, const GLvoid* pvPixels)
{
    GT_PRE_BODY_A9(glTexSubImage2D,eTarget,iLevel,iXOffset,iYOffset,iWidth,
        iHeight,eFormat,eType,pvPixels);
}
//----------------------------------------------------------------------------
void GTglTranslated (GLdouble dX, GLdouble dY, GLdouble dZ)
{
    GT_PRE_BODY_A3(glTranslated,dX,dY,dZ);
}
//----------------------------------------------------------------------------
void GTglTranslatef (GLfloat fX, GLfloat fY, GLfloat fZ)
{
    GT_PRE_BODY_A3(glTranslatef,fX,fY,fZ);
}
//----------------------------------------------------------------------------
void GTglVertex2d (GLdouble dX, GLdouble dY)
{
    GT_PRE_BODY_A2(glVertex2d,dX,dY);
}
//----------------------------------------------------------------------------
void GTglVertex2dv (const GLdouble* adXY)
{
    GT_PRE_BODY_A1(glVertex2dv,adXY);
}
//----------------------------------------------------------------------------
void GTglVertex2f (GLfloat fX, GLfloat fY)
{
    GT_PRE_BODY_A2(glVertex2f,fX,fY);
}
//----------------------------------------------------------------------------
void GTglVertex2fv (const GLfloat* afXY)
{
    GT_PRE_BODY_A1(glVertex2fv,afXY);
}
//----------------------------------------------------------------------------
void GTglVertex2i (GLint iX, GLint iY)
{
    GT_PRE_BODY_A2(glVertex2i,iX,iY);
}
//----------------------------------------------------------------------------
void GTglVertex2iv (const GLint* aiXY)
{
    GT_PRE_BODY_A1(glVertex2iv,aiXY);
}
//----------------------------------------------------------------------------
void GTglVertex2s (GLshort sX, GLshort sY)
{
    GT_PRE_BODY_A2(glVertex2s,sX,sY);
}
//----------------------------------------------------------------------------
void GTglVertex2sv (const GLshort* asXY)
{
    GT_PRE_BODY_A1(glVertex2sv,asXY);
}
//----------------------------------------------------------------------------
void GTglVertex3d (GLdouble dX, GLdouble dY, GLdouble dZ)
{
    GT_PRE_BODY_A3(glVertex3d,dX,dY,dZ);
}
//----------------------------------------------------------------------------
void GTglVertex3dv (const GLdouble* adXYZ)
{
    GT_PRE_BODY_A1(glVertex3dv,adXYZ);
}
//----------------------------------------------------------------------------
void GTglVertex3f (GLfloat fX, GLfloat fY, GLfloat fZ)
{
    GT_PRE_BODY_A3(glVertex3f,fX,fY,fZ);
}
//----------------------------------------------------------------------------
void GTglVertex3fv (const GLfloat* afXYZ)
{
    GT_PRE_BODY_A1(glVertex3fv,afXYZ);
}
//----------------------------------------------------------------------------
void GTglVertex3i (GLint iX, GLint iY, GLint iZ)
{
    GT_PRE_BODY_A3(glVertex3i,iX,iY,iZ);
}
//----------------------------------------------------------------------------
void GTglVertex3iv (const GLint* aiXYZ)
{
    GT_PRE_BODY_A1(glVertex3iv,aiXYZ);
}
//----------------------------------------------------------------------------
void GTglVertex3s (GLshort sX, GLshort sY, GLshort sZ)
{
    GT_PRE_BODY_A3(glVertex3s,sX,sY,sZ);
}
//----------------------------------------------------------------------------
void GTglVertex3sv (const GLshort* asXYZ)
{
    GT_PRE_BODY_A1(glVertex3sv,asXYZ);
}
//----------------------------------------------------------------------------
void GTglVertex4d (GLdouble dX, GLdouble dY, GLdouble dZ, GLdouble dW)
{
    GT_PRE_BODY_A4(glVertex4d,dX,dY,dZ,dW);
}
//----------------------------------------------------------------------------
void GTglVertex4dv (const GLdouble* adXYZW)
{
    GT_PRE_BODY_A1(glVertex4dv,adXYZW);
}
//----------------------------------------------------------------------------
void GTglVertex4f (GLfloat fX, GLfloat fY, GLfloat fZ, GLfloat fW)
{
    GT_PRE_BODY_A4(glVertex4f,fX,fY,fZ,fW);
}
//----------------------------------------------------------------------------
void GTglVertex4fv (const GLfloat* afXYZW)
{
    GT_PRE_BODY_A1(glVertex4fv,afXYZW);
}
//----------------------------------------------------------------------------
void GTglVertex4i (GLint iX, GLint iY, GLint iZ, GLint iW)
{
    GT_PRE_BODY_A4(glVertex4i,iX,iY,iZ,iW);
}
//----------------------------------------------------------------------------
void GTglVertex4iv (const GLint* aiXYZW)
{
    GT_PRE_BODY_A1(glVertex4iv,aiXYZW);
}
//----------------------------------------------------------------------------
void GTglVertex4s (GLshort sX, GLshort sY, GLshort sZ, GLshort sW)
{
    GT_PRE_BODY_A4(glVertex4s,sX,sY,sZ,sW);
}
//----------------------------------------------------------------------------
void GTglVertex4sv (const GLshort* asXYZW)
{
    GT_PRE_BODY_A1(glVertex4sv,asXYZW);
}
//----------------------------------------------------------------------------
void GTglVertexPointer (GLint iSize, GLenum eType, GLsizei iStride,
    const GLvoid* pvPointer)
{
    GT_PRE_BODY_A4(glVertexPointer,iSize,eType,iStride,pvPointer);
}
//----------------------------------------------------------------------------
void GTglViewport (GLint iX, GLint iY, GLsizei iWidth, GLsizei iHeight)
{
    GT_PRE_BODY_A4(glViewport,iX,iY,iWidth,iHeight);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// OpenGL 1.2
//----------------------------------------------------------------------------
int GTExistsOpenGL12 = 0;

typedef void (STDCALL* PsglCopyTexSubImage3D) (GLenum, GLint, GLint,
    GLint, GLint, GLint, GLint, GLsizei, GLsizei);
typedef void (STDCALL* PsglDrawRangeElements) (GLenum, GLuint, GLuint,
    GLsizei, GLenum, const GLvoid*);
typedef void (STDCALL* PsglTexImage3D) (GLenum, GLint, GLint, GLsizei,
    GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid*);
typedef void (STDCALL* PsglTexSubImage3D) (GLenum, GLint, GLint, GLint,
    GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const GLvoid*);

static PsglCopyTexSubImage3D sglCopyTexSubImage3D = 0;
static PsglDrawRangeElements sglDrawRangeElements = 0;
static PsglTexImage3D sglTexImage3D = 0;
static PsglTexSubImage3D sglTexSubImage3D = 0;
//----------------------------------------------------------------------------
void GTglDrawRangeElements (GLenum eMode, GLuint uiStart, GLuint uiEnd,
    GLsizei iCount, GLenum eType, const GLvoid* pvIndices)
{
    GT_BODY_A6(sglDrawRangeElements,eMode,uiStart,uiEnd,iCount,eType,
        pvIndices);
}
//----------------------------------------------------------------------------
void GTglTexImage3D (GLenum eTarget, GLint iLevel, GLint iInternalFormat,
    GLsizei iWidth, GLsizei iHeight, GLsizei iDepth, GLint iBorder,
    GLenum eFormat, GLenum eType, const GLvoid* pvData)
{
    GT_BODY_A10(sglTexImage3D,eTarget,iLevel,iInternalFormat,iWidth,iHeight,
        iDepth,iBorder,eFormat,eType,pvData);
}
//----------------------------------------------------------------------------
void GTglTexSubImage3D (GLenum eTarget, GLint iLevel, GLint iXOffset,
    GLint iYOffset, GLint iZOffset, GLsizei iWidth, GLsizei iHeight,
    GLsizei iDepth, GLenum eFormat, GLenum eType, const GLvoid* pvData)
{
    GT_BODY_A11(sglTexSubImage3D,eTarget,iLevel,iXOffset,iYOffset,iZOffset,
        iWidth,iHeight,iDepth,eFormat,eType,pvData);
}
//----------------------------------------------------------------------------
void GTglCopyTexSubImage3D (GLenum eTarget, GLint iLevel, GLint iXOffset,
    GLint iYOffset, GLint iZOffset, GLint iX, GLint iY, GLsizei iWidth,
    GLsizei iHeight)
{
    GT_BODY_A9(sglCopyTexSubImage3D,eTarget,iLevel,iXOffset,iYOffset,
        iZOffset,iX,iY,iWidth,iHeight);
}
//----------------------------------------------------------------------------
void GTInitOpenGL12 ()
{
    if (GTGetVersion() >= GT_VERSION_1_2)
    {
        GTExistsOpenGL12 = 1;

        // GL_ARB_imaging
        //     GL_EXT_blend_color
        //     GL_EXT_blend_minmax
        //     GL_EXT_color_subtable
        //     GL_EXT_convolution
        //     GL_EXT_histogram
        //     GL_EXT_paletted_texture
        //     GL_SGI_color_table

        // GL_EXT_copy_texture
        GT_GET_FUNCTION(sglCopyTexSubImage3D);

        // GL_EXT_draw_range_elements
        GT_GET_FUNCTION(sglDrawRangeElements);

        // GL_EXT_texture3D
        GT_GET_FUNCTION(sglTexImage3D);
        GT_GET_FUNCTION(sglTexSubImage3D);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Imaging Subset (GL_ARB_imaging)
//----------------------------------------------------------------------------
int GTExistsGlArbImaging = 0;

typedef void (STDCALL* PsglBlendColor) (GLclampf, GLclampf, GLclampf,
    GLclampf);
typedef void (STDCALL* PsglBlendEquation) (GLenum);
typedef void (STDCALL* PsglColorTable) (GLenum, GLenum, GLsizei, GLenum,
    GLenum, const GLvoid*);
typedef void (STDCALL* PsglColorTableParameterfv) (GLenum, GLenum,
    const GLfloat*);
typedef void (STDCALL* PsglColorTableParameteriv) (GLenum, GLenum,
    const GLint*);
typedef void (STDCALL* PsglCopyColorTable) (GLenum, GLenum, GLint, GLint,
    GLsizei);
typedef void (STDCALL* PsglGetColorTable) (GLenum, GLenum, GLenum,
    GLvoid*);
typedef void (STDCALL* PsglGetColorTableParameterfv) (GLenum, GLenum,
    GLfloat*);
typedef void (STDCALL* PsglGetColorTableParameteriv) (GLenum, GLenum,
    GLint*);
typedef void (STDCALL* PsglColorSubTable) (GLenum, GLsizei, GLsizei,
    GLenum, GLenum, const GLvoid*);
typedef void (STDCALL* PsglCopyColorSubTable) (GLenum, GLsizei, GLint,
    GLint, GLsizei);
typedef void (STDCALL* PsglConvolutionFilter1D) (GLenum, GLenum, GLsizei,
    GLenum, GLenum, const GLvoid*);
typedef void (STDCALL* PsglConvolutionFilter2D) (GLenum, GLenum, GLsizei,
    GLsizei, GLenum, GLenum, const GLvoid*);
typedef void (STDCALL* PsglConvolutionParameterf) (GLenum, GLenum,
    GLfloat);
typedef void (STDCALL* PsglConvolutionParameterfv) (GLenum, GLenum,
    const GLfloat*);
typedef void (STDCALL* PsglConvolutionParameteri) (GLenum, GLenum, GLint);
typedef void (STDCALL* PsglConvolutionParameteriv) (GLenum, GLenum,
    const GLint*);
typedef void (STDCALL* PsglCopyConvolutionFilter1D) (GLenum, GLenum, GLint,
    GLint, GLsizei);
typedef void (STDCALL* PsglCopyConvolutionFilter2D) (GLenum, GLenum, GLint,
    GLint, GLsizei, GLsizei);
typedef void (STDCALL* PsglGetConvolutionFilter) (GLenum, GLenum, GLenum,
    GLvoid*);
typedef void (STDCALL* PsglGetConvolutionParameterfv) (GLenum, GLenum,
    GLfloat*);
typedef void (STDCALL* PsglGetConvolutionParameteriv) (GLenum, GLenum,
    GLint*);
typedef void (STDCALL* PsglGetSeparableFilter) (GLenum, GLenum, GLenum,
    GLvoid*, GLvoid*, GLvoid*);
typedef void (STDCALL* PsglSeparableFilter2D) (GLenum, GLenum, GLsizei,
    GLsizei, GLenum, GLenum, const GLvoid*, const GLvoid*);
typedef void (STDCALL* PsglGetHistogram) (GLenum, GLboolean, GLenum,
    GLenum, GLvoid*);
typedef void (STDCALL* PsglGetHistogramParameterfv) (GLenum, GLenum,
    GLfloat*);
typedef void (STDCALL* PsglGetHistogramParameteriv) (GLenum, GLenum,
    GLint*);
typedef void (STDCALL* PsglGetMinmax) (GLenum, GLboolean, GLenum, GLenum,
    GLvoid*);
typedef void (STDCALL* PsglGetMinmaxParameterfv) (GLenum, GLenum,
    GLfloat*);
typedef void (STDCALL* PsglGetMinmaxParameteriv) (GLenum, GLenum, GLint*);
typedef void (STDCALL* PsglHistogram) (GLenum, GLsizei, GLenum, GLboolean);
typedef void (STDCALL* PsglMinmax) (GLenum, GLenum, GLboolean);
typedef void (STDCALL* PsglResetHistogram) (GLenum);
typedef void (STDCALL* PsglResetMinmax) (GLenum);

static PsglBlendColor sglBlendColor = 0;
static PsglBlendEquation sglBlendEquation = 0;
static PsglColorTable sglColorTable = 0;
static PsglColorTableParameterfv sglColorTableParameterfv = 0;
static PsglColorTableParameteriv sglColorTableParameteriv = 0;
static PsglCopyColorTable sglCopyColorTable = 0;
static PsglGetColorTable sglGetColorTable = 0;
static PsglGetColorTableParameterfv sglGetColorTableParameterfv = 0;
static PsglGetColorTableParameteriv sglGetColorTableParameteriv = 0;
static PsglColorSubTable sglColorSubTable = 0;
static PsglCopyColorSubTable sglCopyColorSubTable = 0;
static PsglConvolutionFilter1D sglConvolutionFilter1D = 0;
static PsglConvolutionFilter2D sglConvolutionFilter2D = 0;
static PsglConvolutionParameterf sglConvolutionParameterf = 0;
static PsglConvolutionParameterfv sglConvolutionParameterfv = 0;
static PsglConvolutionParameteri sglConvolutionParameteri = 0;
static PsglConvolutionParameteriv sglConvolutionParameteriv = 0;
static PsglCopyConvolutionFilter1D sglCopyConvolutionFilter1D = 0;
static PsglCopyConvolutionFilter2D sglCopyConvolutionFilter2D = 0;
static PsglGetConvolutionFilter sglGetConvolutionFilter = 0;
static PsglGetConvolutionParameterfv sglGetConvolutionParameterfv = 0;
static PsglGetConvolutionParameteriv sglGetConvolutionParameteriv = 0;
static PsglGetSeparableFilter sglGetSeparableFilter = 0;
static PsglSeparableFilter2D sglSeparableFilter2D = 0;
static PsglGetHistogram sglGetHistogram = 0;
static PsglGetHistogramParameterfv sglGetHistogramParameterfv = 0;
static PsglGetHistogramParameteriv sglGetHistogramParameteriv = 0;
static PsglGetMinmax sglGetMinmax = 0;
static PsglGetMinmaxParameterfv sglGetMinmaxParameterfv = 0;
static PsglGetMinmaxParameteriv sglGetMinmaxParameteriv = 0;
static PsglHistogram sglHistogram = 0;
static PsglMinmax sglMinmax = 0;
static PsglResetHistogram sglResetHistogram = 0;
static PsglResetMinmax sglResetMinmax = 0;
//----------------------------------------------------------------------------
void GTglBlendColor (GLclampf fRed, GLclampf fGreen, GLclampf fBlue,
    GLclampf fAlpha)
{
    GT_BODY_A4(sglBlendColor,fRed,fGreen,fBlue,fAlpha);
}
//----------------------------------------------------------------------------
void GTglBlendEquation (GLenum eMode)
{
    GT_BODY_A1(sglBlendEquation,eMode);
}
//----------------------------------------------------------------------------
void GTglColorTable (GLenum eTarget, GLenum eInternalFormat, GLsizei iWidth,
    GLenum eFormat, GLenum eType, const GLvoid* pvTable)
{
    GT_BODY_A6(sglColorTable,eTarget,eInternalFormat,iWidth,eFormat,eType,
        pvTable);
}
//----------------------------------------------------------------------------
void GTglColorTableParameterfv (GLenum eTarget, GLenum ePName,
    const GLfloat* afParam)
{
    GT_BODY_A3(sglColorTableParameterfv,eTarget,ePName,afParam);
}
//----------------------------------------------------------------------------
void GTglColorTableParameteriv (GLenum eTarget, GLenum ePName,
    const GLint* aiParam)
{
    GT_BODY_A3(sglColorTableParameteriv,eTarget,ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTglCopyColorTable (GLenum eTarget, GLenum eInternalFormat, GLint iX,
    GLint iY, GLsizei iWidth)
{
    GT_BODY_A5(sglCopyColorTable,eTarget,eInternalFormat,iX,iY,iWidth);
}
//----------------------------------------------------------------------------
void GTglGetColorTable (GLenum eTarget, GLenum eFormat, GLenum eType,
    GLvoid* pvTable)
{
    GT_BODY_A4(sglGetColorTable,eTarget,eFormat,eType,pvTable);
}
//----------------------------------------------------------------------------
void GTglGetColorTableParameterfv (GLenum eTarget, GLenum ePName,
    GLfloat* afParam)
{
    GT_BODY_A3(sglGetColorTableParameterfv,eTarget,ePName,afParam);
}
//----------------------------------------------------------------------------
void GTglGetColorTableParameteriv (GLenum eTarget, GLenum ePName,
    GLint* aiParam)
{
    GT_BODY_A3(sglGetColorTableParameteriv,eTarget,ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTglColorSubTable (GLenum eTarget, GLsizei iStart, GLsizei iCount,
    GLenum eFormat, GLenum eType, const GLvoid* pvData)
{
    GT_BODY_A6(sglColorSubTable,eTarget,iStart,iCount,eFormat,eType,pvData);
}
//----------------------------------------------------------------------------
void GTglCopyColorSubTable (GLenum eTarget, GLsizei iStart, GLint iX,
    GLint iY, GLsizei iWidth)
{
    GT_BODY_A5(sglCopyColorSubTable,eTarget,iStart,iX,iY,iWidth);
}
//----------------------------------------------------------------------------
void GTglConvolutionFilter1D (GLenum eTarget, GLenum eInternalFormat,
    GLsizei iWidth, GLenum eFormat, GLenum eType, const GLvoid* pvImage)
{
    GT_BODY_A6(sglConvolutionFilter1D,eTarget,eInternalFormat,iWidth,eFormat,
        eType,pvImage);
}
//----------------------------------------------------------------------------
void GTglConvolutionFilter2D (GLenum eTarget, GLenum eInternalFormat,
    GLsizei iWidth, GLsizei iHeight, GLenum eFormat, GLenum eType,
    const GLvoid* pvImage)
{
    GT_BODY_A7(sglConvolutionFilter2D,eTarget,eInternalFormat,iWidth,iHeight,
        eFormat,eType,pvImage);
}
//----------------------------------------------------------------------------
void GTglConvolutionParameterf (GLenum eTarget, GLenum ePName, GLfloat fParam)
{
    GT_BODY_A3(sglConvolutionParameterf,eTarget,ePName,fParam);
}
//----------------------------------------------------------------------------
void GTglConvolutionParameterfv (GLenum eTarget, GLenum ePName,
    const GLfloat* afParam)
{
    GT_BODY_A3(sglConvolutionParameterfv,eTarget,ePName,afParam);
}
//----------------------------------------------------------------------------
void GTglConvolutionParameteri (GLenum eTarget, GLenum ePName, GLint iParam)
{
    GT_BODY_A3(sglConvolutionParameteri,eTarget,ePName,iParam);
}
//----------------------------------------------------------------------------
void GTglConvolutionParameteriv (GLenum eTarget, GLenum ePName,
    const GLint* aiParam)
{
    GT_BODY_A3(sglConvolutionParameteriv,eTarget,ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTglCopyConvolutionFilter1D (GLenum eTarget, GLenum eInternalFormat,
    GLint iX, GLint iY, GLsizei iWidth)
{
    GT_BODY_A5(sglCopyConvolutionFilter1D,eTarget,eInternalFormat,iX,iY,
        iWidth);
}
//----------------------------------------------------------------------------
void GTglCopyConvolutionFilter2D (GLenum eTarget, GLenum eInternalFormat,
    GLint iX, GLint iY, GLsizei iWidth, GLsizei iHeight)
{
    GT_BODY_A6(sglCopyConvolutionFilter2D,eTarget,eInternalFormat,iX,iY,
        iWidth,iHeight);
}
//----------------------------------------------------------------------------
void GTglGetConvolutionFilter (GLenum eTarget, GLenum eFormat, GLenum eType,
    GLvoid* pvImage)
{
    GT_BODY_A4(sglGetConvolutionFilter,eTarget,eFormat,eType,pvImage);
}
//----------------------------------------------------------------------------
void GTglGetConvolutionParameterfv (GLenum eTarget, GLenum ePName,
    GLfloat* afParam)
{
    GT_BODY_A3(sglGetConvolutionParameterfv,eTarget,ePName,afParam);
}
//----------------------------------------------------------------------------
void GTglGetConvolutionParameteriv (GLenum eTarget, GLenum ePName,
    GLint* aiParam)
{
    GT_BODY_A3(sglGetConvolutionParameteriv,eTarget,ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTglGetSeparableFilter (GLenum eTarget, GLenum eFormat, GLenum eType,
    GLvoid* pvRow, GLvoid* pvColumn, GLvoid* pvSpan)
{
    GT_BODY_A6(sglGetSeparableFilter,eTarget,eFormat,eType,pvRow,pvColumn,
        pvSpan);
}
//----------------------------------------------------------------------------
void GTglSeparableFilter2D (GLenum eTarget, GLenum eInternalFormat,
    GLsizei iWidth, GLsizei iHeight, GLenum eFormat, GLenum eType,
    const GLvoid* pvRow, const GLvoid* pvColumn)
{
    GT_BODY_A8(sglSeparableFilter2D,eTarget,eInternalFormat,iWidth,iHeight,
        eFormat,eType,pvRow,pvColumn);
}
//----------------------------------------------------------------------------
void GTglGetHistogram (GLenum eTarget, GLboolean ucReset, GLenum eFormat,
    GLenum eType, GLvoid* pvValues)
{
    GT_BODY_A5(sglGetHistogram,eTarget,ucReset,eFormat,eType,pvValues);
}
//----------------------------------------------------------------------------
void GTglGetHistogramParameterfv (GLenum eTarget, GLenum ePName,
    GLfloat* afParam)
{
    GT_BODY_A3(sglGetHistogramParameterfv,eTarget,ePName,afParam);
}
//----------------------------------------------------------------------------
void GTglGetHistogramParameteriv (GLenum eTarget, GLenum ePName,
    GLint* aiParam)
{
    GT_BODY_A3(sglGetHistogramParameteriv,eTarget,ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTglGetMinmax (GLenum eTarget, GLboolean ucReset, GLenum eFormat,
    GLenum eType, GLvoid* pvValues)
{
    GT_BODY_A5(sglGetMinmax,eTarget,ucReset,eFormat,eType,pvValues);
}
//----------------------------------------------------------------------------
void GTglGetMinmaxParameterfv (GLenum eTarget, GLenum ePName,
    GLfloat* afParam)
{
    GT_BODY_A3(sglGetMinmaxParameterfv,eTarget,ePName,afParam);
}
//----------------------------------------------------------------------------
void GTglGetMinmaxParameteriv (GLenum eTarget, GLenum ePName, GLint* aiParam)
{
    GT_BODY_A3(sglGetMinmaxParameteriv,eTarget,ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTglHistogram (GLenum eTarget, GLsizei iWidth, GLenum eInternalFormat,
    GLboolean ucSink)
{
    GT_BODY_A4(sglHistogram,eTarget,iWidth,eInternalFormat,ucSink);
}
//----------------------------------------------------------------------------
void GTglMinmax (GLenum eTarget, GLenum eInternalFormat, GLboolean ucSink)
{
    GT_BODY_A3(sglMinmax,eTarget,eInternalFormat,ucSink);
}
//----------------------------------------------------------------------------
void GTglResetHistogram (GLenum eTarget)
{
    GT_BODY_A1(sglResetHistogram,eTarget);
}
//----------------------------------------------------------------------------
void GTglResetMinmax (GLenum eTarget)
{
    GT_BODY_A1(sglResetMinmax,eTarget);
}
//----------------------------------------------------------------------------
void GTInitGlArbImaging ()
{
    if (GTSupportsExtension("GL_ARB_imaging"))
    {
        GTExistsGlArbImaging = 1;

        // GL_SGI_color_table
        GT_GET_FUNCTION(sglColorTableParameterfv);
        GT_GET_FUNCTION(sglColorTableParameteriv);
        GT_GET_FUNCTION(sglCopyColorTable);

        // GL_SGI_color_table and GL_EXT_paletted_texture
        GT_GET_FUNCTION(sglColorTable);
        GT_GET_FUNCTION(sglGetColorTable);
        GT_GET_FUNCTION(sglGetColorTableParameterfv);
        GT_GET_FUNCTION(sglGetColorTableParameteriv);

        // GL_EXT_color_subtable
        GT_GET_FUNCTION(sglColorSubTable);
        GT_GET_FUNCTION(sglCopyColorSubTable);

        // GL_EXT_convolution
        GT_GET_FUNCTION(sglConvolutionFilter1D);
        GT_GET_FUNCTION(sglConvolutionFilter2D);
        GT_GET_FUNCTION(sglConvolutionParameterf);
        GT_GET_FUNCTION(sglConvolutionParameterfv);
        GT_GET_FUNCTION(sglConvolutionParameteri);
        GT_GET_FUNCTION(sglConvolutionParameteriv);
        GT_GET_FUNCTION(sglCopyConvolutionFilter1D);
        GT_GET_FUNCTION(sglCopyConvolutionFilter2D);
        GT_GET_FUNCTION(sglGetConvolutionFilter);
        GT_GET_FUNCTION(sglGetConvolutionParameterfv);
        GT_GET_FUNCTION(sglGetConvolutionParameteriv);
        GT_GET_FUNCTION(sglGetSeparableFilter);
        GT_GET_FUNCTION(sglSeparableFilter2D);

        // GL_EXT_histogram
        GT_GET_FUNCTION(sglGetHistogram);
        GT_GET_FUNCTION(sglGetHistogramParameterfv);
        GT_GET_FUNCTION(sglGetHistogramParameteriv);
        GT_GET_FUNCTION(sglGetMinmax);
        GT_GET_FUNCTION(sglGetMinmaxParameterfv);
        GT_GET_FUNCTION(sglGetMinmaxParameteriv);
        GT_GET_FUNCTION(sglHistogram);
        GT_GET_FUNCTION(sglMinmax);
        GT_GET_FUNCTION(sglResetHistogram);
        GT_GET_FUNCTION(sglResetMinmax);

        // GL_EXT_blend_color
        GT_GET_FUNCTION(sglBlendColor);

        // GL_EXT_blend_minmax
        GT_GET_FUNCTION(sglBlendEquation);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// OpenGL 1.3
//----------------------------------------------------------------------------
int GTExistsOpenGL13 = 0;

typedef void (STDCALL* PsglActiveTexture) (GLenum);
typedef void (STDCALL* PsglClientActiveTexture) (GLenum);
typedef void (STDCALL* PsglMultiTexCoord1d) (GLenum, GLdouble);
typedef void (STDCALL* PsglMultiTexCoord1dv) (GLenum, const GLdouble*);
typedef void (STDCALL* PsglMultiTexCoord1f) (GLenum, GLfloat);
typedef void (STDCALL* PsglMultiTexCoord1fv) (GLenum, const GLfloat*);
typedef void (STDCALL* PsglMultiTexCoord1i) (GLenum, int);
typedef void (STDCALL* PsglMultiTexCoord1iv) (GLenum, const GLint*);
typedef void (STDCALL* PsglMultiTexCoord1s) (GLenum, GLshort);
typedef void (STDCALL* PsglMultiTexCoord1sv) (GLenum, const GLshort*);
typedef void (STDCALL* PsglMultiTexCoord2d) (GLenum, GLdouble, GLdouble);
typedef void (STDCALL* PsglMultiTexCoord2dv) (GLenum, const GLdouble*);
typedef void (STDCALL* PsglMultiTexCoord2f) (GLenum, GLfloat, GLfloat);
typedef void (STDCALL* PsglMultiTexCoord2fv) (GLenum, const GLfloat*);
typedef void (STDCALL* PsglMultiTexCoord2i) (GLenum, GLint, GLint);
typedef void (STDCALL* PsglMultiTexCoord2iv) (GLenum, const GLint*);
typedef void (STDCALL* PsglMultiTexCoord2s) (GLenum, GLshort, GLshort);
typedef void (STDCALL* PsglMultiTexCoord2sv) (GLenum, const GLshort*);
typedef void (STDCALL* PsglMultiTexCoord3d) (GLenum, GLdouble, GLdouble,
    GLdouble);
typedef void (STDCALL* PsglMultiTexCoord3dv) (GLenum, const GLdouble*);
typedef void (STDCALL* PsglMultiTexCoord3f) (GLenum, GLfloat, GLfloat,
    GLfloat);
typedef void (STDCALL* PsglMultiTexCoord3fv) (GLenum, const GLfloat*);
typedef void (STDCALL* PsglMultiTexCoord3i) (GLenum, GLint, GLint, GLint);
typedef void (STDCALL* PsglMultiTexCoord3iv) (GLenum, const GLint*);
typedef void (STDCALL* PsglMultiTexCoord3s) (GLenum, GLshort, GLshort,
    GLshort);
typedef void (STDCALL* PsglMultiTexCoord3sv) (GLenum, const GLshort*);
typedef void (STDCALL* PsglMultiTexCoord4d) (GLenum, GLdouble, GLdouble,
    GLdouble, GLdouble);
typedef void (STDCALL* PsglMultiTexCoord4dv) (GLenum, const GLdouble*);
typedef void (STDCALL* PsglMultiTexCoord4f) (GLenum, GLfloat, GLfloat,
    GLfloat, GLfloat);
typedef void (STDCALL* PsglMultiTexCoord4fv) (GLenum, const GLfloat*);
typedef void (STDCALL* PsglMultiTexCoord4i) (GLenum, GLint, GLint, GLint,
    GLint);
typedef void (STDCALL* PsglMultiTexCoord4iv) (GLenum, const int*);
typedef void (STDCALL* PsglMultiTexCoord4s) (GLenum, GLshort, GLshort,
    GLshort, GLshort);
typedef void (STDCALL* PsglMultiTexCoord4sv) (GLenum, const GLshort*);
typedef void (STDCALL* PsglLoadTransposeMatrixd) (const GLdouble*);
typedef void (STDCALL* PsglLoadTransposeMatrixf) (const GLfloat*);
typedef void (STDCALL* PsglMultTransposeMatrixd) (const GLdouble*);
typedef void (STDCALL* PsglMultTransposeMatrixf) (const GLfloat*);
typedef void (STDCALL* PsglSampleCoverage) (GLclampf, GLboolean);
typedef void (STDCALL* PsglCompressedTexImage1D) (GLenum, GLint, GLenum,
    GLsizei, GLint, GLsizei, const GLvoid*);
typedef void (STDCALL* PsglCompressedTexImage2D) (GLenum, GLint, GLenum,
    GLsizei, GLsizei, GLint, GLsizei, const GLvoid*);
typedef void (STDCALL* PsglCompressedTexImage3D) (GLenum, GLint, GLenum,
    GLsizei, GLsizei, GLsizei, GLint, GLsizei, const GLvoid*);
typedef void (STDCALL* PsglCompressedTexSubImage1D) (GLenum, GLint, GLint,
    GLsizei, GLenum, GLsizei, const GLvoid*);
typedef void (STDCALL* PsglCompressedTexSubImage2D) (GLenum, GLint, GLint,
    GLint, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid*);
typedef void (STDCALL* PsglCompressedTexSubImage3D) (GLenum, GLint, GLint,
    GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid*);
typedef void (STDCALL* PsglGetCompressedTexImage) (GLenum, GLint, GLvoid*);

static PsglActiveTexture sglActiveTexture = 0;
static PsglClientActiveTexture sglClientActiveTexture = 0;
static PsglMultiTexCoord1d sglMultiTexCoord1d = 0;
static PsglMultiTexCoord1dv sglMultiTexCoord1dv = 0;
static PsglMultiTexCoord1f sglMultiTexCoord1f = 0;
static PsglMultiTexCoord1fv sglMultiTexCoord1fv = 0;
static PsglMultiTexCoord1i sglMultiTexCoord1i = 0;
static PsglMultiTexCoord1iv sglMultiTexCoord1iv = 0;
static PsglMultiTexCoord1s sglMultiTexCoord1s = 0;
static PsglMultiTexCoord1sv sglMultiTexCoord1sv = 0;
static PsglMultiTexCoord2d sglMultiTexCoord2d = 0;
static PsglMultiTexCoord2dv sglMultiTexCoord2dv = 0;
static PsglMultiTexCoord2f sglMultiTexCoord2f = 0;
static PsglMultiTexCoord2fv sglMultiTexCoord2fv = 0;
static PsglMultiTexCoord2i sglMultiTexCoord2i = 0;
static PsglMultiTexCoord2iv sglMultiTexCoord2iv = 0;
static PsglMultiTexCoord2s sglMultiTexCoord2s = 0;
static PsglMultiTexCoord2sv sglMultiTexCoord2sv = 0;
static PsglMultiTexCoord3d sglMultiTexCoord3d = 0;
static PsglMultiTexCoord3dv sglMultiTexCoord3dv = 0;
static PsglMultiTexCoord3f sglMultiTexCoord3f = 0;
static PsglMultiTexCoord3fv sglMultiTexCoord3fv = 0;
static PsglMultiTexCoord3i sglMultiTexCoord3i = 0;
static PsglMultiTexCoord3iv sglMultiTexCoord3iv = 0;
static PsglMultiTexCoord3s sglMultiTexCoord3s = 0;
static PsglMultiTexCoord3sv sglMultiTexCoord3sv = 0;
static PsglMultiTexCoord4d sglMultiTexCoord4d = 0;
static PsglMultiTexCoord4dv sglMultiTexCoord4dv = 0;
static PsglMultiTexCoord4f sglMultiTexCoord4f = 0;
static PsglMultiTexCoord4fv sglMultiTexCoord4fv = 0;
static PsglMultiTexCoord4i sglMultiTexCoord4i = 0;
static PsglMultiTexCoord4iv sglMultiTexCoord4iv = 0;
static PsglMultiTexCoord4s sglMultiTexCoord4s = 0;
static PsglMultiTexCoord4sv sglMultiTexCoord4sv = 0;
static PsglLoadTransposeMatrixf sglLoadTransposeMatrixf = 0;
static PsglLoadTransposeMatrixd sglLoadTransposeMatrixd = 0;
static PsglMultTransposeMatrixf sglMultTransposeMatrixf = 0;
static PsglMultTransposeMatrixd sglMultTransposeMatrixd = 0;
static PsglSampleCoverage sglSampleCoverage = 0;
static PsglCompressedTexImage3D sglCompressedTexImage3D = 0;
static PsglCompressedTexImage2D sglCompressedTexImage2D = 0;
static PsglCompressedTexImage1D sglCompressedTexImage1D = 0;
static PsglCompressedTexSubImage3D sglCompressedTexSubImage3D = 0;
static PsglCompressedTexSubImage2D sglCompressedTexSubImage2D = 0;
static PsglCompressedTexSubImage1D sglCompressedTexSubImage1D = 0;
static PsglGetCompressedTexImage sglGetCompressedTexImage = 0;
//----------------------------------------------------------------------------
void GTglActiveTexture (GLenum eTexture)
{
    GT_BODY_A1(sglActiveTexture,eTexture);
}
//----------------------------------------------------------------------------
void GTglClientActiveTexture (GLenum eTexture)
{
    GT_BODY_A1(sglClientActiveTexture,eTexture);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord1d (GLenum eTarget, GLdouble dS)
{
    GT_BODY_A2(sglMultiTexCoord1d,eTarget,dS);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord1dv (GLenum eTarget, const GLdouble* pdS)
{
    GT_BODY_A2(sglMultiTexCoord1dv,eTarget,pdS);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord1f (GLenum eTarget, GLfloat fS)
{
    GT_BODY_A2(sglMultiTexCoord1f,eTarget,fS);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord1fv (GLenum eTarget, const GLfloat* pfS)
{
    GT_BODY_A2(sglMultiTexCoord1fv,eTarget,pfS);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord1i (GLenum eTarget, GLint iS)
{
    GT_BODY_A2(sglMultiTexCoord1i,eTarget,iS);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord1iv (GLenum eTarget, const GLint* piS)
{
    GT_BODY_A2(sglMultiTexCoord1iv,eTarget,piS);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord1s (GLenum eTarget, GLshort sS)
{
    GT_BODY_A2(sglMultiTexCoord1s,eTarget,sS);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord1sv (GLenum eTarget, const GLshort* psS)
{
    GT_BODY_A2(sglMultiTexCoord1sv,eTarget,psS);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord2d (GLenum eTarget, GLdouble dS, GLdouble dT)
{
    GT_BODY_A3(sglMultiTexCoord2d,eTarget,dS,dT);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord2dv (GLenum eTarget, const GLdouble* adST)
{
    GT_BODY_A2(sglMultiTexCoord2dv,eTarget,adST);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord2f (GLenum eTarget, GLfloat fS, GLfloat fT)
{
    GT_BODY_A3(sglMultiTexCoord2f,eTarget,fS,fT);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord2fv (GLenum eTarget, const GLfloat* afST)
{
    GT_BODY_A2(sglMultiTexCoord2fv,eTarget,afST);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord2i (GLenum eTarget, GLint iS, GLint iT)
{
    GT_BODY_A3(sglMultiTexCoord2i,eTarget,iS,iT);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord2iv (GLenum eTarget, const GLint* aiST)
{
    GT_BODY_A2(sglMultiTexCoord2iv,eTarget,aiST);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord2s (GLenum eTarget, GLshort sS, GLshort sT)
{
    GT_BODY_A3(sglMultiTexCoord2s,eTarget,sS,sT);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord2sv (GLenum eTarget, const GLshort* asST)
{
    GT_BODY_A2(sglMultiTexCoord2sv,eTarget,asST);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord3d (GLenum eTarget, GLdouble dS, GLdouble dT,
    GLdouble dR)
{
    GT_BODY_A4(sglMultiTexCoord3d,eTarget,dS,dT,dR);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord3dv (GLenum eTarget, const GLdouble* adSTR)
{
    GT_BODY_A2(sglMultiTexCoord3dv,eTarget,adSTR);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord3f (GLenum eTarget, GLfloat fS, GLfloat fT, GLfloat fR)
{
    GT_BODY_A4(sglMultiTexCoord3f,eTarget,fS,fT,fR);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord3fv (GLenum eTarget, const GLfloat* afSTR)
{
    GT_BODY_A2(sglMultiTexCoord3fv,eTarget,afSTR);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord3i (GLenum eTarget, GLint iS, GLint iT, GLint iR)
{
    GT_BODY_A4(sglMultiTexCoord3i,eTarget,iS,iT,iR);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord3iv (GLenum eTarget, const GLint* aiSTR)
{
    GT_BODY_A2(sglMultiTexCoord3iv,eTarget,aiSTR);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord3s (GLenum eTarget, GLshort sS, GLshort sT, GLshort sR)
{
    GT_BODY_A4(sglMultiTexCoord3s,eTarget,sS,sT,sR);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord3sv (GLenum eTarget, const GLshort* asSTR)
{
    GT_BODY_A2(sglMultiTexCoord3sv,eTarget,asSTR);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord4d (GLenum eTarget, GLdouble dS, GLdouble dT,
    GLdouble dR, GLdouble dQ)
{
    GT_BODY_A5(sglMultiTexCoord4d,eTarget,dS,dT,dR,dQ);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord4dv (GLenum eTarget, const GLdouble* adSTRQ)
{
    GT_BODY_A2(sglMultiTexCoord4dv,eTarget,adSTRQ);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord4f (GLenum eTarget, GLfloat fS, GLfloat fT, GLfloat fR,
    GLfloat fQ)
{
    GT_BODY_A5(sglMultiTexCoord4f,eTarget,fS,fT,fR,fQ);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord4fv (GLenum eTarget, const GLfloat* afSTRQ)
{
    GT_BODY_A2(sglMultiTexCoord4fv,eTarget,afSTRQ);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord4i (GLenum eTarget, GLint iS, GLint iT, GLint iR,
    GLint iQ)
{
    GT_BODY_A5(sglMultiTexCoord4i,eTarget,iS,iT,iR,iQ);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord4iv (GLenum eTarget, const GLint* aiSTRQ)
{
    GT_BODY_A2(sglMultiTexCoord4iv,eTarget,aiSTRQ);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord4s (GLenum eTarget, GLshort sS, GLshort sT, GLshort sR,
    GLshort sQ)
{
    GT_BODY_A5(sglMultiTexCoord4s,eTarget,sS,sT,sR,sQ);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord4sv (GLenum eTarget, const GLshort* asSTRQ)
{
    GT_BODY_A2(sglMultiTexCoord4sv,eTarget,asSTRQ);
}
//----------------------------------------------------------------------------
void GTglLoadTransposeMatrixd (const GLdouble* adMatrix)
{
    GT_BODY_A1(sglLoadTransposeMatrixd,adMatrix);
}
//----------------------------------------------------------------------------
void GTglLoadTransposeMatrixf (const GLfloat* afMatrix)
{
    GT_BODY_A1(sglLoadTransposeMatrixf,afMatrix);
}
//----------------------------------------------------------------------------
void GTglMultTransposeMatrixd (const GLdouble* adMatrix)
{
    GT_BODY_A1(sglMultTransposeMatrixd,adMatrix);
}
//----------------------------------------------------------------------------
void GTglMultTransposeMatrixf (const GLfloat* afMatrix)
{
    GT_BODY_A1(sglMultTransposeMatrixf,afMatrix);
}
//----------------------------------------------------------------------------
void GTglSampleCoverage (GLclampf fValue, GLboolean ucInvert)
{
    GT_BODY_A2(sglSampleCoverage,fValue,ucInvert);
}
//----------------------------------------------------------------------------
void GTglCompressedTexImage1D (GLenum eTarget, GLint iLevel,
    GLenum eInternalFormat, GLsizei iWidth, GLint iBorder, GLsizei iImageSize,
    const GLvoid* pvData)
{
    GT_BODY_A7(sglCompressedTexImage1D,eTarget,iLevel,eInternalFormat,iWidth,
        iBorder,iImageSize,pvData);
}
//----------------------------------------------------------------------------
void GTglCompressedTexImage2D (GLenum eTarget, GLint iLevel,
    GLenum eInternalFormat, GLsizei iWidth, GLsizei iHeight, GLint iBorder,
    GLsizei iImageSize, const GLvoid* pvData)
{
    GT_BODY_A8(sglCompressedTexImage2D,eTarget,iLevel,eInternalFormat,iWidth,
        iHeight,iBorder,iImageSize,pvData);
}
//----------------------------------------------------------------------------
void GTglCompressedTexImage3D (GLenum eTarget, GLint iLevel,
    GLenum eInternalFormat, GLsizei iWidth, GLsizei iHeight, GLsizei iDepth,
    GLint iBorder, GLsizei iImageSize, const GLvoid* pvData)
{
    GT_BODY_A9(sglCompressedTexImage3D,eTarget,iLevel,eInternalFormat,iWidth,
        iHeight,iDepth,iBorder,iImageSize,pvData);
}
//----------------------------------------------------------------------------
void GTglCompressedTexSubImage1D (GLenum eTarget, GLint iLevel,
    GLint iXOffset, GLsizei iWidth, GLenum eFormat, GLsizei iImageSize,
    const GLvoid* pvData)
{
    GT_BODY_A7(sglCompressedTexSubImage1D,eTarget,iLevel,iXOffset,iWidth,
        eFormat,iImageSize,pvData);
}
//----------------------------------------------------------------------------
void GTglCompressedTexSubImage2D (GLenum eTarget, GLint iLevel,
    GLint iXOffset, GLint iYOffset, GLsizei iWidth, GLsizei iHeight,
    GLenum eFormat, GLsizei iImageSize, const GLvoid* pvData)
{
    GT_BODY_A9(sglCompressedTexSubImage2D,eTarget,iLevel,iXOffset,iYOffset,
        iWidth,iHeight,eFormat,iImageSize,pvData);
}
//----------------------------------------------------------------------------
void GTglCompressedTexSubImage3D (GLenum eTarget, GLint iLevel,
    GLint iXOffset, GLint iYOffset, GLint iZOffset, GLsizei iWidth,
    GLsizei iHeight, GLsizei iDepth, GLenum eFormat, GLsizei iImageSize,
    const GLvoid* pvData)
{
    GT_BODY_A11(sglCompressedTexSubImage3D,eTarget,iLevel,iXOffset,iYOffset,
        iZOffset,iWidth,iHeight,iDepth,eFormat,iImageSize,pvData);
}
//----------------------------------------------------------------------------
void GTglGetCompressedTexImage (GLenum eTarget, GLint iLevel, GLvoid* pvData)
{
    GT_BODY_A3(sglGetCompressedTexImage,eTarget,iLevel,pvData);
}
//----------------------------------------------------------------------------
void GTInitOpenGL13 ()
{
    if (GTGetVersion() >= GT_VERSION_1_3)
    {
        GTExistsOpenGL13 = 1;

        // GL_ARB_multisample
        GT_GET_FUNCTION(sglSampleCoverage);

        // GL_ARB_multitexture
        GT_GET_FUNCTION(sglActiveTexture);
        GT_GET_FUNCTION(sglClientActiveTexture);
        GT_GET_FUNCTION(sglMultiTexCoord1d);
        GT_GET_FUNCTION(sglMultiTexCoord1dv);
        GT_GET_FUNCTION(sglMultiTexCoord1f);
        GT_GET_FUNCTION(sglMultiTexCoord1fv);
        GT_GET_FUNCTION(sglMultiTexCoord1i);
        GT_GET_FUNCTION(sglMultiTexCoord1iv);
        GT_GET_FUNCTION(sglMultiTexCoord1s);
        GT_GET_FUNCTION(sglMultiTexCoord1sv);
        GT_GET_FUNCTION(sglMultiTexCoord2d);
        GT_GET_FUNCTION(sglMultiTexCoord2dv);
        GT_GET_FUNCTION(sglMultiTexCoord2f);
        GT_GET_FUNCTION(sglMultiTexCoord2fv);
        GT_GET_FUNCTION(sglMultiTexCoord2i);
        GT_GET_FUNCTION(sglMultiTexCoord2iv);
        GT_GET_FUNCTION(sglMultiTexCoord2s);
        GT_GET_FUNCTION(sglMultiTexCoord2sv);
        GT_GET_FUNCTION(sglMultiTexCoord3d);
        GT_GET_FUNCTION(sglMultiTexCoord3dv);
        GT_GET_FUNCTION(sglMultiTexCoord3f);
        GT_GET_FUNCTION(sglMultiTexCoord3fv);
        GT_GET_FUNCTION(sglMultiTexCoord3i);
        GT_GET_FUNCTION(sglMultiTexCoord3iv);
        GT_GET_FUNCTION(sglMultiTexCoord3s);
        GT_GET_FUNCTION(sglMultiTexCoord3sv);
        GT_GET_FUNCTION(sglMultiTexCoord4d);
        GT_GET_FUNCTION(sglMultiTexCoord4dv);
        GT_GET_FUNCTION(sglMultiTexCoord4f);
        GT_GET_FUNCTION(sglMultiTexCoord4fv);
        GT_GET_FUNCTION(sglMultiTexCoord4i);
        GT_GET_FUNCTION(sglMultiTexCoord4iv);
        GT_GET_FUNCTION(sglMultiTexCoord4s);
        GT_GET_FUNCTION(sglMultiTexCoord4sv);

        // GL_ARB_texture_compression
        GT_GET_FUNCTION(sglCompressedTexImage3D);
        GT_GET_FUNCTION(sglCompressedTexImage2D);
        GT_GET_FUNCTION(sglCompressedTexImage1D);
        GT_GET_FUNCTION(sglCompressedTexSubImage3D);
        GT_GET_FUNCTION(sglCompressedTexSubImage2D);
        GT_GET_FUNCTION(sglCompressedTexSubImage1D);
        GT_GET_FUNCTION(sglGetCompressedTexImage);

        // GL_ARB_transpose_matrix
        GT_GET_FUNCTION(sglLoadTransposeMatrixf);
        GT_GET_FUNCTION(sglLoadTransposeMatrixd);
        GT_GET_FUNCTION(sglMultTransposeMatrixf);
        GT_GET_FUNCTION(sglMultTransposeMatrixd);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// OpenGL 1.4
//----------------------------------------------------------------------------
int GTExistsOpenGL14 = 0;

typedef void (STDCALL* PsglBlendFuncSeparate) (GLenum, GLenum, GLenum,
    GLenum);
typedef void (STDCALL* PsglFogCoordd) (GLdouble);
typedef void (STDCALL* PsglFogCoorddv) (const GLdouble*);
typedef void (STDCALL* PsglFogCoordf) (GLfloat);
typedef void (STDCALL* PsglFogCoordfv) (const GLfloat*);
typedef void (STDCALL* PsglFogCoordPointer) (GLenum, GLsizei,
    const GLvoid*);
typedef void (STDCALL* PsglMultiDrawArrays) (GLenum, GLint*, GLsizei*,
    GLsizei);
typedef void (STDCALL* PsglMultiDrawElements) (GLenum, const GLsizei*,
    GLenum, const GLvoid**, GLsizei);
typedef void (STDCALL* PsglPointParameterf) (GLenum, GLfloat);
typedef void (STDCALL* PsglPointParameterfv) (GLenum, const GLfloat*);
typedef void (STDCALL* PsglPointParameteri) (GLenum, GLint);
typedef void (STDCALL* PsglPointParameteriv) (GLenum, const GLint*);
typedef void (STDCALL* PsglSecondaryColor3b) (GLbyte, GLbyte, GLbyte);
typedef void (STDCALL* PsglSecondaryColor3bv) (const GLbyte*);
typedef void (STDCALL* PsglSecondaryColor3d) (GLdouble, GLdouble, GLdouble);
typedef void (STDCALL* PsglSecondaryColor3dv) (const GLdouble*);
typedef void (STDCALL* PsglSecondaryColor3f) (GLfloat, GLfloat, GLfloat);
typedef void (STDCALL* PsglSecondaryColor3fv) (const GLfloat*);
typedef void (STDCALL* PsglSecondaryColor3i) (GLint, GLint, GLint);
typedef void (STDCALL* PsglSecondaryColor3iv) (const GLint*);
typedef void (STDCALL* PsglSecondaryColor3s) (GLshort, GLshort, GLshort);
typedef void (STDCALL* PsglSecondaryColor3sv) (const GLshort*);
typedef void (STDCALL* PsglSecondaryColor3ub) (GLubyte, GLubyte, GLubyte);
typedef void (STDCALL* PsglSecondaryColor3ubv) (const GLubyte*);
typedef void (STDCALL* PsglSecondaryColor3ui) (GLuint, GLuint, GLuint);
typedef void (STDCALL* PsglSecondaryColor3uiv) (const GLuint*);
typedef void (STDCALL* PsglSecondaryColor3us) (GLushort, GLushort,
    GLushort);
typedef void (STDCALL* PsglSecondaryColor3usv) (const GLushort*);
typedef void (STDCALL* PsglSecondaryColorPointer) (GLint, GLenum, GLsizei,
    const GLvoid*);
typedef void (STDCALL* PsglWindowPos2d) (GLdouble, GLdouble);
typedef void (STDCALL* PsglWindowPos2dv) (const GLdouble*);
typedef void (STDCALL* PsglWindowPos2f) (GLfloat, GLfloat);
typedef void (STDCALL* PsglWindowPos2fv) (const GLfloat*);
typedef void (STDCALL* PsglWindowPos2i) (GLint, GLint);
typedef void (STDCALL* PsglWindowPos2iv) (const GLint*);
typedef void (STDCALL* PsglWindowPos2s) (GLshort, GLshort);
typedef void (STDCALL* PsglWindowPos2sv) (const GLshort*);
typedef void (STDCALL* PsglWindowPos3d) (GLdouble, GLdouble, GLdouble);
typedef void (STDCALL* PsglWindowPos3dv) (const GLdouble*);
typedef void (STDCALL* PsglWindowPos3f) (GLfloat, GLfloat, GLfloat);
typedef void (STDCALL* PsglWindowPos3fv) (const GLfloat*);
typedef void (STDCALL* PsglWindowPos3i) (GLint, GLint, GLint);
typedef void (STDCALL* PsglWindowPos3iv) (const GLint*);
typedef void (STDCALL* PsglWindowPos3s) (GLshort, GLshort, GLshort);
typedef void (STDCALL* PsglWindowPos3sv) (const GLshort*);

static PsglBlendFuncSeparate sglBlendFuncSeparate = 0;
static PsglFogCoordf sglFogCoordf = 0;
static PsglFogCoordfv sglFogCoordfv = 0;
static PsglFogCoordd sglFogCoordd = 0;
static PsglFogCoorddv sglFogCoorddv = 0;
static PsglFogCoordPointer sglFogCoordPointer = 0;
static PsglMultiDrawArrays sglMultiDrawArrays = 0;
static PsglMultiDrawElements sglMultiDrawElements = 0;
static PsglPointParameterf sglPointParameterf = 0;
static PsglPointParameterfv sglPointParameterfv = 0;
static PsglPointParameteri sglPointParameteri = 0;
static PsglPointParameteriv sglPointParameteriv = 0;
static PsglSecondaryColor3b sglSecondaryColor3b = 0;
static PsglSecondaryColor3bv sglSecondaryColor3bv = 0;
static PsglSecondaryColor3d sglSecondaryColor3d = 0;
static PsglSecondaryColor3dv sglSecondaryColor3dv = 0;
static PsglSecondaryColor3f sglSecondaryColor3f = 0;
static PsglSecondaryColor3fv sglSecondaryColor3fv = 0;
static PsglSecondaryColor3i sglSecondaryColor3i = 0;
static PsglSecondaryColor3iv sglSecondaryColor3iv = 0;
static PsglSecondaryColor3s sglSecondaryColor3s = 0;
static PsglSecondaryColor3sv sglSecondaryColor3sv = 0;
static PsglSecondaryColor3ub sglSecondaryColor3ub = 0;
static PsglSecondaryColor3ubv sglSecondaryColor3ubv = 0;
static PsglSecondaryColor3ui sglSecondaryColor3ui = 0;
static PsglSecondaryColor3uiv sglSecondaryColor3uiv = 0;
static PsglSecondaryColor3us sglSecondaryColor3us = 0;
static PsglSecondaryColor3usv sglSecondaryColor3usv = 0;
static PsglSecondaryColorPointer sglSecondaryColorPointer = 0;
static PsglWindowPos2d sglWindowPos2d = 0;
static PsglWindowPos2dv sglWindowPos2dv = 0;
static PsglWindowPos2f sglWindowPos2f = 0;
static PsglWindowPos2fv sglWindowPos2fv = 0;
static PsglWindowPos2i sglWindowPos2i = 0;
static PsglWindowPos2iv sglWindowPos2iv = 0;
static PsglWindowPos2s sglWindowPos2s = 0;
static PsglWindowPos2sv sglWindowPos2sv = 0;
static PsglWindowPos3d sglWindowPos3d = 0;
static PsglWindowPos3dv sglWindowPos3dv = 0;
static PsglWindowPos3f sglWindowPos3f = 0;
static PsglWindowPos3fv sglWindowPos3fv = 0;
static PsglWindowPos3i sglWindowPos3i = 0;
static PsglWindowPos3iv sglWindowPos3iv = 0;
static PsglWindowPos3s sglWindowPos3s = 0;
static PsglWindowPos3sv sglWindowPos3sv = 0;
//----------------------------------------------------------------------------
void GTglBlendFuncSeparate (GLenum eSrcFactorRGB, GLenum eDstFactorRGB,
    GLenum eSrcFactorA, GLenum eDstFactorA)
{
    GT_BODY_A4(sglBlendFuncSeparate,eSrcFactorRGB,eDstFactorRGB,eSrcFactorA,
        eDstFactorA);
}
//----------------------------------------------------------------------------
void GTglFogCoordd (GLdouble dCoord)
{
    GT_BODY_A1(sglFogCoordd,dCoord);
}
//----------------------------------------------------------------------------
void GTglFogCoorddv (const GLdouble* pdCoord)
{
    GT_BODY_A1(sglFogCoorddv,pdCoord);
}
//----------------------------------------------------------------------------
void GTglFogCoordf (GLfloat fCoord)
{
    GT_BODY_A1(sglFogCoordf,fCoord);
}
//----------------------------------------------------------------------------
void GTglFogCoordfv (const GLfloat* pfCoord)
{
    GT_BODY_A1(sglFogCoordfv,pfCoord);
}
//----------------------------------------------------------------------------
void GTglFogCoordPointer (GLenum eType, GLsizei iStride,
    const GLvoid* pvPointer)
{
    GT_BODY_A3(sglFogCoordPointer,eType,iStride,pvPointer);
}
//----------------------------------------------------------------------------
void GTglMultiDrawArrays (GLenum eMode, GLint* aiFirst, GLsizei* aiCount,
    GLsizei iPrimitiveCount)
{
    GT_BODY_A4(sglMultiDrawArrays,eMode,aiFirst,aiCount,iPrimitiveCount);
}
//----------------------------------------------------------------------------
void GTglMultiDrawElements (GLenum eMode, const GLsizei* aiCount,
    GLenum eType, const GLvoid** apvIndices, GLsizei iPrimitiveCount)
{
    GT_BODY_A5(sglMultiDrawElements,eMode,aiCount,eType,apvIndices,
        iPrimitiveCount);
}
//----------------------------------------------------------------------------
void GTglPointParameterf (GLenum ePName, GLfloat fParam)
{
    GT_BODY_A2(sglPointParameterf,ePName,fParam);
}
//----------------------------------------------------------------------------
void GTglPointParameterfv (GLenum ePName, const GLfloat* afParam)
{
    GT_BODY_A2(sglPointParameterfv,ePName,afParam);
}
//----------------------------------------------------------------------------
void GTglPointParameteri (GLenum ePName, int iParam)
{
    GT_BODY_A2(sglPointParameteri,ePName,iParam);
}
//----------------------------------------------------------------------------
void GTglPointParameteriv (GLenum ePName, const int* aiParam)
{
    GT_BODY_A2(sglPointParameteriv,ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTglSecondaryColor3b (GLbyte cRed, GLbyte cGreen, GLbyte cBlue)
{
    GT_BODY_A3(sglSecondaryColor3b,cRed,cGreen,cBlue);
}
//----------------------------------------------------------------------------
void GTglSecondaryColor3bv (const GLbyte* acRGB)
{
    GT_BODY_A1(sglSecondaryColor3bv,acRGB);
}
//----------------------------------------------------------------------------
void GTglSecondaryColor3d (GLdouble dRed, GLdouble dGreen, GLdouble dBlue)
{
    GT_BODY_A3(sglSecondaryColor3d,dRed,dGreen,dBlue);
}
//----------------------------------------------------------------------------
void GTglSecondaryColor3dv (const GLdouble* adRGB)
{
    GT_BODY_A1(sglSecondaryColor3dv,adRGB);
}
//----------------------------------------------------------------------------
void GTglSecondaryColor3f (GLfloat fRed, GLfloat fGreen, GLfloat fBlue)
{
    GT_BODY_A3(sglSecondaryColor3f,fRed,fGreen,fBlue);
}
//----------------------------------------------------------------------------
void GTglSecondaryColor3fv (const GLfloat* afRGB)
{
    GT_BODY_A1(sglSecondaryColor3fv,afRGB);
}
//----------------------------------------------------------------------------
void GTglSecondaryColor3i (GLint iRed, GLint iGreen, GLint iBlue)
{
    GT_BODY_A3(sglSecondaryColor3i,iRed,iGreen,iBlue);
}
//----------------------------------------------------------------------------
void GTglSecondaryColor3iv (const GLint* aiRGB)
{
    GT_BODY_A1(sglSecondaryColor3iv,aiRGB);
}
//----------------------------------------------------------------------------
void GTglSecondaryColor3s (GLshort sRed, GLshort sGreen, GLshort sBlue)
{
    GT_BODY_A3(sglSecondaryColor3s,sRed,sGreen,sBlue);
}
//----------------------------------------------------------------------------
void GTglSecondaryColor3sv (const GLshort* asRGB)
{
    GT_BODY_A1(sglSecondaryColor3sv,asRGB);
}
//----------------------------------------------------------------------------
void GTglSecondaryColor3ub (GLubyte ucRed, GLubyte ucGreen, GLubyte ucBlue)
{
    GT_BODY_A3(sglSecondaryColor3ub,ucRed,ucGreen,ucBlue);
}
//----------------------------------------------------------------------------
void GTglSecondaryColor3ubv (const GLubyte* aucRGB)
{
    GT_BODY_A1(sglSecondaryColor3ubv,aucRGB);
}
//----------------------------------------------------------------------------
void GTglSecondaryColor3ui (GLuint uiRed, GLuint uiGreen, GLuint uiBlue)
{
    GT_BODY_A3(sglSecondaryColor3ui,uiRed,uiGreen,uiBlue);
}
//----------------------------------------------------------------------------
void GTglSecondaryColor3uiv (const GLuint* auiRGB)
{
    GT_BODY_A1(sglSecondaryColor3uiv,auiRGB);
}
//----------------------------------------------------------------------------
void GTglSecondaryColor3us (GLushort usRed, GLushort usGreen, GLushort usBlue)
{
    GT_BODY_A3(sglSecondaryColor3us,usRed,usGreen,usBlue);
}
//----------------------------------------------------------------------------
void GTglSecondaryColor3usv (const GLushort* ausRGB)
{
    GT_BODY_A1(sglSecondaryColor3usv,ausRGB);
}
//----------------------------------------------------------------------------
void GTglSecondaryColorPointer (GLint iSize, GLenum eType, GLsizei iStride,
    const GLvoid* pvPointer)
{
    GT_BODY_A4(sglSecondaryColorPointer,iSize,eType,iStride,pvPointer);
}
//----------------------------------------------------------------------------
void GTglWindowPos2d (GLdouble dX, GLdouble dY)
{
    GT_BODY_A2(sglWindowPos2d,dX,dY);
}
//----------------------------------------------------------------------------
void GTglWindowPos2dv (const GLdouble* adXY)
{
    GT_BODY_A1(sglWindowPos2dv,adXY);
}
//----------------------------------------------------------------------------
void GTglWindowPos2f (GLfloat fX, GLfloat fY)
{
    GT_BODY_A2(sglWindowPos2f,fX,fY);
}
//----------------------------------------------------------------------------
void GTglWindowPos2fv (const GLfloat* afXY)
{
    GT_BODY_A1(sglWindowPos2fv,afXY);
}
//----------------------------------------------------------------------------
void GTglWindowPos2i (GLint iX, GLint iY)
{
    GT_BODY_A2(sglWindowPos2i,iX,iY);
}
//----------------------------------------------------------------------------
void GTglWindowPos2iv (const GLint* aiXY)
{
    GT_BODY_A1(sglWindowPos2iv,aiXY);
}
//----------------------------------------------------------------------------
void GTglWindowPos2s (GLshort sX, GLshort sY)
{
    GT_BODY_A2(sglWindowPos2s,sX,sY);
}
//----------------------------------------------------------------------------
void GTglWindowPos2sv (const GLshort* asXY)
{
    GT_BODY_A1(sglWindowPos2sv,asXY);
}
//----------------------------------------------------------------------------
void GTglWindowPos3d (GLdouble dX, GLdouble dY, GLdouble dZ)
{
    GT_BODY_A3(sglWindowPos3d,dX,dY,dZ);
}
//----------------------------------------------------------------------------
void GTglWindowPos3dv (const GLdouble* adXYZ)
{
    GT_BODY_A1(sglWindowPos3dv,adXYZ);
}
//----------------------------------------------------------------------------
void GTglWindowPos3f (GLfloat fX, GLfloat fY, GLfloat fZ)
{
    GT_BODY_A3(sglWindowPos3f,fX,fY,fZ);
}
//----------------------------------------------------------------------------
void GTglWindowPos3fv (const GLfloat* afXYZ)
{
    GT_BODY_A1(sglWindowPos3fv,afXYZ);
}
//----------------------------------------------------------------------------
void GTglWindowPos3i (GLint iX, GLint iY, GLint iZ)
{
    GT_BODY_A3(sglWindowPos3i,iX,iY,iZ);
}
//----------------------------------------------------------------------------
void GTglWindowPos3iv (const GLint* aiXYZ)
{
    GT_BODY_A1(sglWindowPos3iv,aiXYZ);
}
//----------------------------------------------------------------------------
void GTglWindowPos3s (GLshort sX, GLshort sY, GLshort sZ)
{
    GT_BODY_A3(sglWindowPos3s,sX,sY,sZ);
}
//----------------------------------------------------------------------------
void GTglWindowPos3sv (const GLshort* asXYZ)
{
    GT_BODY_A1(sglWindowPos3sv,asXYZ);
}
//----------------------------------------------------------------------------
void GTInitOpenGL14 ()
{
    if (GTGetVersion() >= GT_VERSION_1_4)
    {
        GTExistsOpenGL14 = 1;

        // GL_{EXT,INGR}_blend_func_separate
        GT_GET_FUNCTION(sglBlendFuncSeparate);

        // GL_EXT_fog_coord
        GT_GET_FUNCTION(sglFogCoordf);
        GT_GET_FUNCTION(sglFogCoordfv);
        GT_GET_FUNCTION(sglFogCoordd);
        GT_GET_FUNCTION(sglFogCoorddv);
        GT_GET_FUNCTION(sglFogCoordPointer);

        // GL_EXT_multi_draw_arrays
        GT_GET_FUNCTION(sglMultiDrawArrays);
        GT_GET_FUNCTION(sglMultiDrawElements);

        // GL_{ARB,EXT,SGIS}_point_parameters
        GT_GET_FUNCTION(sglPointParameterf);
        GT_GET_FUNCTION(sglPointParameterfv);

        // GL_NV_point_sprite
        GT_GET_FUNCTION(sglPointParameteri);
        GT_GET_FUNCTION(sglPointParameteriv);

        // GL_EXT_secondary_color
        GT_GET_FUNCTION(sglSecondaryColor3b);
        GT_GET_FUNCTION(sglSecondaryColor3bv);
        GT_GET_FUNCTION(sglSecondaryColor3d);
        GT_GET_FUNCTION(sglSecondaryColor3dv);
        GT_GET_FUNCTION(sglSecondaryColor3f);
        GT_GET_FUNCTION(sglSecondaryColor3fv);
        GT_GET_FUNCTION(sglSecondaryColor3i);
        GT_GET_FUNCTION(sglSecondaryColor3iv);
        GT_GET_FUNCTION(sglSecondaryColor3s);
        GT_GET_FUNCTION(sglSecondaryColor3sv);
        GT_GET_FUNCTION(sglSecondaryColor3ub);
        GT_GET_FUNCTION(sglSecondaryColor3ubv);
        GT_GET_FUNCTION(sglSecondaryColor3ui);
        GT_GET_FUNCTION(sglSecondaryColor3uiv);
        GT_GET_FUNCTION(sglSecondaryColor3us);
        GT_GET_FUNCTION(sglSecondaryColor3usv);
        GT_GET_FUNCTION(sglSecondaryColorPointer);

        // GL_{ARB,MESA}_window_pos
        GT_GET_FUNCTION(sglWindowPos2d);
        GT_GET_FUNCTION(sglWindowPos2dv);
        GT_GET_FUNCTION(sglWindowPos2f);
        GT_GET_FUNCTION(sglWindowPos2fv);
        GT_GET_FUNCTION(sglWindowPos2i);
        GT_GET_FUNCTION(sglWindowPos2iv);
        GT_GET_FUNCTION(sglWindowPos2s);
        GT_GET_FUNCTION(sglWindowPos2sv);
        GT_GET_FUNCTION(sglWindowPos3d);
        GT_GET_FUNCTION(sglWindowPos3dv);
        GT_GET_FUNCTION(sglWindowPos3f);
        GT_GET_FUNCTION(sglWindowPos3fv);
        GT_GET_FUNCTION(sglWindowPos3i);
        GT_GET_FUNCTION(sglWindowPos3iv);
        GT_GET_FUNCTION(sglWindowPos3s);
        GT_GET_FUNCTION(sglWindowPos3sv);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// OpenGL 1.5
//----------------------------------------------------------------------------
int GTExistsOpenGL15 = 0;

typedef void (STDCALL* PsglGenQueries) (GLsizei, GLuint*);
typedef void (STDCALL* PsglDeleteQueries) (GLsizei, const GLuint*);
typedef unsigned char (STDCALL* PsglIsQuery) (GLuint);
typedef void (STDCALL* PsglBeginQuery) (GLenum, GLuint);
typedef void (STDCALL* PsglEndQuery) (GLenum);
typedef void (STDCALL* PsglGetQueryiv) (GLenum, GLenum, GLint*);
typedef void (STDCALL* PsglGetQueryObjectiv) (GLuint, GLenum, GLint*);
typedef void (STDCALL* PsglGetQueryObjectuiv) (GLuint, GLenum, GLuint*);
typedef void (STDCALL* PsglBindBuffer) (GLenum, GLuint);
typedef void (STDCALL* PsglDeleteBuffers) (GLsizei, const GLuint*);
typedef void (STDCALL* PsglGenBuffers) (GLsizei, GLuint*);
typedef unsigned char (STDCALL* PsglIsBuffer) (GLuint);
typedef void (STDCALL* PsglBufferData) (GLenum, GLsizeiptr, const GLvoid*,
    GLenum);
typedef void (STDCALL* PsglBufferSubData) (GLenum, GLintptr, GLsizeiptr,
    const GLvoid*);
typedef void (STDCALL* PsglGetBufferSubData) (GLenum, GLintptr, GLsizeiptr,
    GLvoid*);
typedef void* (STDCALL* PsglMapBuffer) (GLenum, GLenum);
typedef unsigned char (STDCALL* PsglUnmapBuffer) (GLenum);
typedef void (STDCALL* PsglGetBufferParameteriv) (GLenum, GLenum, GLint*);
typedef void (STDCALL* PsglGetBufferPointerv) (GLenum, GLenum, GLvoid**);

static PsglGenQueries sglGenQueries = 0;
static PsglDeleteQueries sglDeleteQueries = 0;
static PsglIsQuery sglIsQuery = 0;
static PsglBeginQuery sglBeginQuery = 0;
static PsglEndQuery sglEndQuery = 0;
static PsglGetQueryiv sglGetQueryiv = 0;
static PsglGetQueryObjectiv sglGetQueryObjectiv = 0;
static PsglGetQueryObjectuiv sglGetQueryObjectuiv = 0;
static PsglBindBuffer sglBindBuffer = 0;
static PsglDeleteBuffers sglDeleteBuffers = 0;
static PsglGenBuffers sglGenBuffers = 0;
static PsglIsBuffer sglIsBuffer = 0;
static PsglBufferData sglBufferData = 0;
static PsglBufferSubData sglBufferSubData = 0;
static PsglGetBufferSubData sglGetBufferSubData = 0;
static PsglMapBuffer sglMapBuffer = 0;
static PsglUnmapBuffer sglUnmapBuffer = 0;
static PsglGetBufferParameteriv sglGetBufferParameteriv = 0;
static PsglGetBufferPointerv sglGetBufferPointerv = 0;
//----------------------------------------------------------------------------
void GTglGenQueries (GLsizei iNumQueries, GLuint* auiID)
{
    GT_BODY_A2(sglGenQueries,iNumQueries,auiID);
}
//----------------------------------------------------------------------------
void GTglDeleteQueries (GLsizei iNumQueries, const GLuint* auiID)
{
    GT_BODY_A2(sglDeleteQueries,iNumQueries,auiID);
}
//----------------------------------------------------------------------------
GLboolean GTglIsQuery (GLuint uiID)
{
    GT_BODY_A1_RET(sglIsQuery,uiID,GLboolean,0);
}
//----------------------------------------------------------------------------
void GTglBeginQuery (GLenum eTarget, GLuint uiID)
{
    GT_BODY_A2(sglBeginQuery,eTarget,uiID);
}
//----------------------------------------------------------------------------
void GTglEndQuery (GLenum eTarget)
{
    GT_BODY_A1(sglEndQuery,eTarget);
}
//----------------------------------------------------------------------------
void GTglGetQueryiv (GLenum eTarget, GLenum ePName, GLint* aiParam)
{
    GT_BODY_A3(sglGetQueryiv,eTarget,ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTglGetQueryObjectiv (GLuint uiID, GLenum ePName, GLint* aiParam)
{
    GT_BODY_A3(sglGetQueryObjectiv,uiID,ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTglGetQueryObjectuiv (GLuint uiID, GLenum ePName, GLuint* auiParam)
{
    GT_BODY_A3(sglGetQueryObjectuiv,uiID,ePName,auiParam);
}
//----------------------------------------------------------------------------
void GTglBindBuffer (GLenum eTarget, GLuint uiBuffer)
{
    GT_BODY_A2(sglBindBuffer,eTarget,uiBuffer);
}
//----------------------------------------------------------------------------
void GTglDeleteBuffers (GLsizei iNumBuffers, const GLuint* auiBuffer)
{
    GT_BODY_A2(sglDeleteBuffers,iNumBuffers,auiBuffer);
}
//----------------------------------------------------------------------------
void GTglGenBuffers (GLsizei iNumBuffers, GLuint* auiBuffer)
{
    GT_BODY_A2(sglGenBuffers,iNumBuffers,auiBuffer);
}
//----------------------------------------------------------------------------
GLboolean GTglIsBuffer (GLuint uiBuffer)
{
    GT_BODY_A1_RET(sglIsBuffer,uiBuffer,GLboolean,0);
}
//----------------------------------------------------------------------------
void GTglBufferData (GLenum eTarget, GLsizeiptr piSize, const GLvoid* pvData,
    GLenum eUsage)
{
    GT_BODY_A4(sglBufferData,eTarget,piSize,pvData,eUsage);
}
//----------------------------------------------------------------------------
void GTglBufferSubData (GLenum eTarget, GLintptr piOffset, GLsizeiptr piSize,
    const GLvoid* pvData)
{
    GT_BODY_A4(sglBufferSubData,eTarget,piOffset,piSize,pvData);
}
//----------------------------------------------------------------------------
void GTglGetBufferSubData (GLenum eTarget, GLintptr piOffset,
    GLsizeiptr piSize, GLvoid* pvData)
{
    GT_BODY_A4(sglGetBufferSubData,eTarget,piOffset,piSize,pvData);
}
//----------------------------------------------------------------------------
GLvoid* GTglMapBuffer (GLenum eTarget, GLenum eAccess)
{
    GT_BODY_A2_RET(sglMapBuffer,eTarget,eAccess,GLvoid*,0);
}
//----------------------------------------------------------------------------
GLboolean GTglUnmapBuffer (GLenum eTarget)
{
    GT_BODY_A1_RET(sglUnmapBuffer,eTarget,GLboolean,0);
}
//----------------------------------------------------------------------------
void GTglGetBufferParameteriv (GLenum eTarget, GLenum ePName, GLint* aiParam)
{
    GT_BODY_A3(sglGetBufferParameteriv,eTarget,ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTglGetBufferPointerv (GLenum eTarget, GLenum ePName, GLvoid** apvParam)
{
    GT_BODY_A3(sglGetBufferPointerv,eTarget,ePName,apvParam);
}
//----------------------------------------------------------------------------
void GTInitOpenGL15 ()
{
    if (GTGetVersion() >= GT_VERSION_1_5)
    {
        GTExistsOpenGL15 = 1;

        // GL_ARB_occlusion_query
        GT_GET_FUNCTION(sglGenQueries);
        GT_GET_FUNCTION(sglDeleteQueries);
        GT_GET_FUNCTION(sglIsQuery);
        GT_GET_FUNCTION(sglBeginQuery);
        GT_GET_FUNCTION(sglEndQuery);
        GT_GET_FUNCTION(sglGetQueryiv);
        GT_GET_FUNCTION(sglGetQueryObjectiv);
        GT_GET_FUNCTION(sglGetQueryObjectuiv);

        // GL_ARB_vertex_buffer_object
        GT_GET_FUNCTION(sglBindBuffer);
        GT_GET_FUNCTION(sglDeleteBuffers);
        GT_GET_FUNCTION(sglGenBuffers);
        GT_GET_FUNCTION(sglIsBuffer);
        GT_GET_FUNCTION(sglBufferData);
        GT_GET_FUNCTION(sglBufferSubData);
        GT_GET_FUNCTION(sglGetBufferSubData);
        GT_GET_FUNCTION(sglMapBuffer);
        GT_GET_FUNCTION(sglUnmapBuffer);
        GT_GET_FUNCTION(sglGetBufferParameteriv);
        GT_GET_FUNCTION(sglGetBufferPointerv);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// OpenGL 2.0
//----------------------------------------------------------------------------
int GTExistsOpenGL20 = 0;

typedef void (STDCALL* PsglBlendEquationSeparate) (GLenum, GLenum);
typedef void (STDCALL* PsglDrawBuffers) (GLsizei, const GLenum*);
typedef void (STDCALL* PsglStencilOpSeparate) (GLenum, GLenum, GLenum,
    GLenum);
typedef void (STDCALL* PsglStencilFuncSeparate) (GLenum, GLenum, GLint,
    GLuint);
typedef void (STDCALL* PsglStencilMaskSeparate) (GLenum, GLuint);
typedef void (STDCALL* PsglAttachShader) (GLuint, GLuint);
typedef void (STDCALL* PsglBindAttribLocation) (GLuint, GLuint,
    const GLchar*);
typedef void (STDCALL* PsglCompileShader) (GLuint);
typedef GLuint (STDCALL* PsglCreateProgram) ();
typedef GLuint (STDCALL* PsglCreateShader) (GLenum);
typedef void (STDCALL* PsglDeleteProgram) (GLuint);
typedef void (STDCALL* PsglDeleteShader) (GLuint);
typedef void (STDCALL* PsglDetachShader) (GLuint, GLuint);
typedef void (STDCALL* PsglDisableVertexAttribArray) (GLuint);
typedef void (STDCALL* PsglEnableVertexAttribArray) (GLuint);
typedef void (STDCALL* PsglGetActiveAttrib) (GLuint, GLuint, GLsizei,
    GLsizei*, GLint*, GLenum*, GLchar*);
typedef void (STDCALL* PsglGetActiveUniform) (GLuint, GLuint, GLsizei,
    GLsizei*, GLint*, GLenum*, GLchar*);
typedef void (STDCALL* PsglGetAttachedShaders) (GLuint, GLsizei, GLsizei*,
    GLuint*);
typedef GLint (STDCALL* PsglGetAttribLocation) (GLuint, const GLchar*);
typedef void (STDCALL* PsglGetProgramiv) (GLuint, GLenum, GLint*);
typedef void (STDCALL* PsglGetProgramInfoLog) (GLuint, GLsizei, GLsizei*,
    GLchar*);
typedef void (STDCALL* PsglGetShaderiv) (GLuint, GLenum, GLint*);
typedef void (STDCALL* PsglGetShaderInfoLog) (GLuint, GLsizei, GLsizei*,
    GLchar*);
typedef void (STDCALL* PsglGetShaderSource) (GLuint, GLsizei, GLsizei*,
    GLchar*);
typedef GLint (STDCALL* PsglGetUniformLocation) (GLuint, const GLchar*);
typedef void (STDCALL* PsglGetUniformfv) (GLuint, GLint, GLfloat*);
typedef void (STDCALL* PsglGetUniformiv) (GLuint, GLint, GLint*);
typedef void (STDCALL* PsglGetVertexAttribdv) (GLuint, GLenum, GLdouble*);
typedef void (STDCALL* PsglGetVertexAttribfv) (GLuint, GLenum, GLfloat*);
typedef void (STDCALL* PsglGetVertexAttribiv) (GLuint, GLenum, GLint*);
typedef void (STDCALL* PsglGetVertexAttribPointerv) (GLuint, GLenum,
    GLvoid**);
typedef GLboolean (STDCALL* PsglIsProgram) (GLuint);
typedef GLboolean (STDCALL* PsglIsShader) (GLuint);
typedef void (STDCALL* PsglLinkProgram) (GLuint);
typedef void (STDCALL* PsglShaderSource) (GLuint, GLsizei, const GLchar**,
    const GLint*);
typedef void (STDCALL* PsglUseProgram) (GLuint);
typedef void (STDCALL* PsglUniform1f) (GLint, GLfloat);
typedef void (STDCALL* PsglUniform2f) (GLint, GLfloat, GLfloat);
typedef void (STDCALL* PsglUniform3f) (GLint, GLfloat, GLfloat, GLfloat);
typedef void (STDCALL* PsglUniform4f) (GLint, GLfloat, GLfloat, GLfloat,
    GLfloat);
typedef void (STDCALL* PsglUniform1i) (GLint, GLint);
typedef void (STDCALL* PsglUniform2i) (GLint, GLint, GLint);
typedef void (STDCALL* PsglUniform3i) (GLint, GLint, GLint, GLint);
typedef void (STDCALL* PsglUniform4i) (GLint, GLint, GLint, GLint, GLint);
typedef void (STDCALL* PsglUniform1fv) (GLint, GLsizei, const GLfloat*);
typedef void (STDCALL* PsglUniform2fv) (GLint, GLsizei, const GLfloat*);
typedef void (STDCALL* PsglUniform3fv) (GLint, GLsizei, const GLfloat*);
typedef void (STDCALL* PsglUniform4fv) (GLint, GLsizei, const GLfloat*);
typedef void (STDCALL* PsglUniform1iv) (GLint, GLsizei, const GLint*);
typedef void (STDCALL* PsglUniform2iv) (GLint, GLsizei, const GLint*);
typedef void (STDCALL* PsglUniform3iv) (GLint, GLsizei, const GLint*);
typedef void (STDCALL* PsglUniform4iv) (GLint, GLsizei, const GLint*);
typedef void (STDCALL* PsglUniformMatrix2fv) (GLint, GLsizei, GLboolean,
    const GLfloat*);
typedef void (STDCALL* PsglUniformMatrix3fv) (GLint, GLsizei, GLboolean,
    const GLfloat*);
typedef void (STDCALL* PsglUniformMatrix4fv) (GLint, GLsizei, GLboolean,
    const GLfloat*);
typedef void (STDCALL* PsglValidateProgram) (GLuint);
typedef void (STDCALL* PsglVertexAttrib1d) (GLuint, GLdouble);
typedef void (STDCALL* PsglVertexAttrib1dv) (GLuint, const GLdouble*);
typedef void (STDCALL* PsglVertexAttrib1f) (GLuint, GLfloat);
typedef void (STDCALL* PsglVertexAttrib1fv) (GLuint, const GLfloat*);
typedef void (STDCALL* PsglVertexAttrib1s) (GLuint, GLshort);
typedef void (STDCALL* PsglVertexAttrib1sv) (GLuint, const GLshort*);
typedef void (STDCALL* PsglVertexAttrib2d) (GLuint, GLdouble, GLdouble);
typedef void (STDCALL* PsglVertexAttrib2dv) (GLuint, const GLdouble*);
typedef void (STDCALL* PsglVertexAttrib2f) (GLuint, GLfloat, GLfloat);
typedef void (STDCALL* PsglVertexAttrib2fv) (GLuint, const GLfloat*);
typedef void (STDCALL* PsglVertexAttrib2s) (GLuint, GLshort, GLshort);
typedef void (STDCALL* PsglVertexAttrib2sv) (GLuint, const GLshort*);
typedef void (STDCALL* PsglVertexAttrib3d) (GLuint, GLdouble, GLdouble,
    GLdouble);
typedef void (STDCALL* PsglVertexAttrib3dv) (GLuint, const GLdouble*);
typedef void (STDCALL* PsglVertexAttrib3f) (GLuint, GLfloat, GLfloat,
    GLfloat);
typedef void (STDCALL* PsglVertexAttrib3fv) (GLuint, const GLfloat*);
typedef void (STDCALL* PsglVertexAttrib3s) (GLuint, GLshort, GLshort,
    GLshort);
typedef void (STDCALL* PsglVertexAttrib3sv) (GLuint, const GLshort*);
typedef void (STDCALL* PsglVertexAttrib4Nbv) (GLuint, const GLbyte*);
typedef void (STDCALL* PsglVertexAttrib4Niv) (GLuint, const GLint*);
typedef void (STDCALL* PsglVertexAttrib4Nsv) (GLuint, const GLshort*);
typedef void (STDCALL* PsglVertexAttrib4Nub) (GLuint, GLubyte, GLubyte,
    GLubyte, GLubyte);
typedef void (STDCALL* PsglVertexAttrib4Nubv) (GLuint, const GLubyte*);
typedef void (STDCALL* PsglVertexAttrib4Nuiv) (GLuint, const GLuint*);
typedef void (STDCALL* PsglVertexAttrib4Nusv) (GLuint, const GLushort*);
typedef void (STDCALL* PsglVertexAttrib4bv) (GLuint, const GLbyte*);
typedef void (STDCALL* PsglVertexAttrib4d) (GLuint, GLdouble, GLdouble,
    GLdouble, GLdouble);
typedef void (STDCALL* PsglVertexAttrib4dv) (GLuint, const GLdouble*);
typedef void (STDCALL* PsglVertexAttrib4f) (GLuint, GLfloat, GLfloat,
    GLfloat, GLfloat);
typedef void (STDCALL* PsglVertexAttrib4fv) (GLuint, const GLfloat*);
typedef void (STDCALL* PsglVertexAttrib4iv) (GLuint, const GLint*);
typedef void (STDCALL* PsglVertexAttrib4s) (GLuint, GLshort, GLshort,
    GLshort, GLshort);
typedef void (STDCALL* PsglVertexAttrib4sv) (GLuint, const GLshort*);
typedef void (STDCALL* PsglVertexAttrib4ubv) (GLuint, const GLubyte*);
typedef void (STDCALL* PsglVertexAttrib4uiv) (GLuint, const GLuint*);
typedef void (STDCALL* PsglVertexAttrib4usv) (GLuint, const GLushort*);
typedef void (STDCALL* PsglVertexAttribPointer) (GLuint, GLint, GLenum,
    GLboolean, GLsizei, const GLvoid*);

static PsglBlendEquationSeparate sglBlendEquationSeparate = 0;
static PsglDrawBuffers sglDrawBuffers = 0;
static PsglStencilOpSeparate sglStencilOpSeparate = 0;
static PsglStencilFuncSeparate sglStencilFuncSeparate = 0;
static PsglStencilMaskSeparate sglStencilMaskSeparate = 0;
static PsglAttachShader sglAttachShader = 0;
static PsglBindAttribLocation sglBindAttribLocation = 0;
static PsglCompileShader sglCompileShader = 0;
static PsglCreateProgram sglCreateProgram = 0;
static PsglCreateShader sglCreateShader = 0;
static PsglDeleteProgram sglDeleteProgram = 0;
static PsglDeleteShader sglDeleteShader = 0;
static PsglDetachShader sglDetachShader = 0;
static PsglDisableVertexAttribArray sglDisableVertexAttribArray = 0;
static PsglEnableVertexAttribArray sglEnableVertexAttribArray = 0;
static PsglGetActiveAttrib sglGetActiveAttrib = 0;
static PsglGetActiveUniform sglGetActiveUniform = 0;
static PsglGetAttachedShaders sglGetAttachedShaders = 0;
static PsglGetAttribLocation sglGetAttribLocation = 0;
static PsglGetProgramiv sglGetProgramiv = 0;
static PsglGetProgramInfoLog sglGetProgramInfoLog = 0;
static PsglGetShaderiv sglGetShaderiv = 0;
static PsglGetShaderInfoLog sglGetShaderInfoLog = 0;
static PsglGetShaderSource sglGetShaderSource = 0;
static PsglGetUniformLocation sglGetUniformLocation = 0;
static PsglGetUniformfv sglGetUniformfv = 0;
static PsglGetUniformiv sglGetUniformiv = 0;
static PsglGetVertexAttribdv sglGetVertexAttribdv = 0;
static PsglGetVertexAttribfv sglGetVertexAttribfv = 0;
static PsglGetVertexAttribiv sglGetVertexAttribiv = 0;
static PsglGetVertexAttribPointerv sglGetVertexAttribPointerv = 0;
static PsglIsProgram sglIsProgram = 0;
static PsglIsShader sglIsShader = 0;
static PsglLinkProgram sglLinkProgram = 0;
static PsglShaderSource sglShaderSource = 0;
static PsglUseProgram sglUseProgram = 0;
static PsglUniform1f sglUniform1f = 0;
static PsglUniform2f sglUniform2f = 0;
static PsglUniform3f sglUniform3f = 0;
static PsglUniform4f sglUniform4f = 0;
static PsglUniform1i sglUniform1i = 0;
static PsglUniform2i sglUniform2i = 0;
static PsglUniform3i sglUniform3i = 0;
static PsglUniform4i sglUniform4i = 0;
static PsglUniform1fv sglUniform1fv = 0;
static PsglUniform2fv sglUniform2fv = 0;
static PsglUniform3fv sglUniform3fv = 0;
static PsglUniform4fv sglUniform4fv = 0;
static PsglUniform1iv sglUniform1iv = 0;
static PsglUniform2iv sglUniform2iv = 0;
static PsglUniform3iv sglUniform3iv = 0;
static PsglUniform4iv sglUniform4iv = 0;
static PsglUniformMatrix2fv sglUniformMatrix2fv = 0;
static PsglUniformMatrix3fv sglUniformMatrix3fv = 0;
static PsglUniformMatrix4fv sglUniformMatrix4fv = 0;
static PsglValidateProgram sglValidateProgram = 0;
static PsglVertexAttrib1d sglVertexAttrib1d = 0;
static PsglVertexAttrib1dv sglVertexAttrib1dv = 0;
static PsglVertexAttrib1f sglVertexAttrib1f = 0;
static PsglVertexAttrib1fv sglVertexAttrib1fv = 0;
static PsglVertexAttrib1s sglVertexAttrib1s = 0;
static PsglVertexAttrib1sv sglVertexAttrib1sv = 0;
static PsglVertexAttrib2d sglVertexAttrib2d = 0;
static PsglVertexAttrib2dv sglVertexAttrib2dv = 0;
static PsglVertexAttrib2f sglVertexAttrib2f = 0;
static PsglVertexAttrib2fv sglVertexAttrib2fv = 0;
static PsglVertexAttrib2s sglVertexAttrib2s = 0;
static PsglVertexAttrib2sv sglVertexAttrib2sv = 0;
static PsglVertexAttrib3d sglVertexAttrib3d = 0;
static PsglVertexAttrib3dv sglVertexAttrib3dv = 0;
static PsglVertexAttrib3f sglVertexAttrib3f = 0;
static PsglVertexAttrib3fv sglVertexAttrib3fv = 0;
static PsglVertexAttrib3s sglVertexAttrib3s = 0;
static PsglVertexAttrib3sv sglVertexAttrib3sv = 0;
static PsglVertexAttrib4Nbv sglVertexAttrib4Nbv = 0;
static PsglVertexAttrib4Niv sglVertexAttrib4Niv = 0;
static PsglVertexAttrib4Nsv sglVertexAttrib4Nsv = 0;
static PsglVertexAttrib4Nub sglVertexAttrib4Nub = 0;
static PsglVertexAttrib4Nubv sglVertexAttrib4Nubv = 0;
static PsglVertexAttrib4Nuiv sglVertexAttrib4Nuiv = 0;
static PsglVertexAttrib4Nusv sglVertexAttrib4Nusv = 0;
static PsglVertexAttrib4bv sglVertexAttrib4bv = 0;
static PsglVertexAttrib4d sglVertexAttrib4d = 0;
static PsglVertexAttrib4dv sglVertexAttrib4dv = 0;
static PsglVertexAttrib4f sglVertexAttrib4f = 0;
static PsglVertexAttrib4fv sglVertexAttrib4fv = 0;
static PsglVertexAttrib4iv sglVertexAttrib4iv = 0;
static PsglVertexAttrib4s sglVertexAttrib4s = 0;
static PsglVertexAttrib4sv sglVertexAttrib4sv = 0;
static PsglVertexAttrib4ubv sglVertexAttrib4ubv = 0;
static PsglVertexAttrib4uiv sglVertexAttrib4uiv = 0;
static PsglVertexAttrib4usv sglVertexAttrib4usv = 0;
static PsglVertexAttribPointer sglVertexAttribPointer = 0;
//----------------------------------------------------------------------------
void GTglBlendEquationSeparate (GLenum eModeRGB, GLenum eModeAlpha)
{
    GT_BODY_A2(sglBlendEquationSeparate,eModeRGB,eModeAlpha);
}
//----------------------------------------------------------------------------
void GTglDrawBuffers (GLsizei iNumBuffers, const GLenum* aeBuffer)
{
    GT_BODY_A2(sglDrawBuffers,iNumBuffers,aeBuffer);
}
//----------------------------------------------------------------------------
void GTglStencilOpSeparate (GLenum eFace, GLenum eSFail, GLenum eDPFail,
    GLenum eDPPass)
{
    GT_BODY_A4(sglStencilOpSeparate,eFace,eSFail,eDPFail,eDPPass);
}
//----------------------------------------------------------------------------
void GTglStencilFuncSeparate (GLenum eFrontFunc, GLenum eBackFunc,
    GLint iReference, GLuint uiMask)
{
    GT_BODY_A4(sglStencilFuncSeparate,eFrontFunc,eBackFunc,iReference,uiMask);
}
//----------------------------------------------------------------------------
void GTglStencilMaskSeparate (GLenum eFace, GLuint uiMask)
{
    GT_BODY_A2(sglStencilMaskSeparate,eFace,uiMask);
}
//----------------------------------------------------------------------------
void GTglAttachShader (GLuint uiProgram, GLuint uiShader)
{
    GT_BODY_A2(sglAttachShader,uiProgram,uiShader);
}
//----------------------------------------------------------------------------
void GTglBindAttribLocation (GLuint uiProgram, GLuint uiIndex,
    const GLchar* acName)
{
    GT_BODY_A3(sglBindAttribLocation,uiProgram,uiIndex,acName);
}
//----------------------------------------------------------------------------
void GTglCompileShader (GLuint uiShader)
{
    GT_BODY_A1(sglCompileShader,uiShader);
}
//----------------------------------------------------------------------------
GLuint GTglCreateProgram ()
{
    // TO DO:  Check the default return value of 0 for correctness.
    GT_BODY_A0_RET(sglCreateProgram,GLuint,0);
}
//----------------------------------------------------------------------------
GLuint GTglCreateShader (GLenum eType)
{
    // TO DO:  Check the default return value of 0 for correctness.
    GT_BODY_A1_RET(sglCreateShader,eType,GLuint,0);
}
//----------------------------------------------------------------------------
void GTglDeleteProgram (GLuint uiProgram)
{
    GT_BODY_A1(sglDeleteProgram,uiProgram);
}
//----------------------------------------------------------------------------
void GTglDeleteShader (GLuint uiShader)
{
    GT_BODY_A1(sglDeleteShader,uiShader);
}
//----------------------------------------------------------------------------
void GTglDetachShader (GLuint uiProgram, GLuint uiShader)
{
    GT_BODY_A2(sglDetachShader,uiProgram,uiShader);
}
//----------------------------------------------------------------------------
void GTglDisableVertexAttribArray (GLuint uiIndex)
{
    GT_BODY_A1(sglDisableVertexAttribArray,uiIndex);
}
//----------------------------------------------------------------------------
void GTglEnableVertexAttribArray (GLuint uiIndex)
{
    GT_BODY_A1(sglEnableVertexAttribArray,uiIndex);
}
//----------------------------------------------------------------------------
void GTglGetActiveAttrib (GLuint uiProgram, GLuint uiIndex, GLsizei iBufSize,
    GLsizei* piLength, GLint* piSize, GLenum* peType, GLchar* acName)
{
    GT_BODY_A7(sglGetActiveAttrib,uiProgram,uiIndex,iBufSize,piLength,piSize,
        peType,acName);
}
//----------------------------------------------------------------------------
void GTglGetActiveUniform (GLuint uiProgram, GLuint uiIndex, GLsizei iBufSize,
    GLsizei* piLength, GLint* piSize, GLenum* peType, GLchar* acName)
{
    GT_BODY_A7(sglGetActiveUniform,uiProgram,uiIndex,iBufSize,piLength,piSize,
        peType,acName);
}
//----------------------------------------------------------------------------
void GTglGetAttachedShaders (GLuint uiProgram, GLsizei iMaxCount,
    GLsizei* aiCount, GLuint* aiObj)
{
    GT_BODY_A4(sglGetAttachedShaders,uiProgram,iMaxCount,aiCount,aiObj);
}
//----------------------------------------------------------------------------
GLint GTglGetAttribLocation (GLuint uiProgram, const GLchar* acName)
{
    // TO DO:  Check the default return value of 0 for correctness.
    GT_BODY_A2_RET(sglGetAttribLocation,uiProgram,acName,GLint,0);
}
//----------------------------------------------------------------------------
void GTglGetProgramiv (GLuint uiProgram, GLenum ePName, GLint* aiParam)
{
    GT_BODY_A3(sglGetProgramiv,uiProgram,ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTglGetProgramInfoLog (GLuint uiProgram, GLsizei iBufSize,
    GLsizei* piLength, GLchar* acInfoLog)
{
    GT_BODY_A4(sglGetProgramInfoLog,uiProgram,iBufSize,piLength,acInfoLog);
}
//----------------------------------------------------------------------------
void GTglGetShaderiv (GLuint uiShader, GLenum ePName, GLint* aiParam)
{
    GT_BODY_A3(sglGetShaderiv,uiShader,ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTglGetShaderInfoLog (GLuint uiShader, GLsizei iBufSize,
    GLsizei* piLength, GLchar* acInfoLog)
{
    GT_BODY_A4(sglGetShaderInfoLog,uiShader,iBufSize,piLength,acInfoLog);
}
//----------------------------------------------------------------------------
void GTglGetShaderSource (GLuint uiShader, GLsizei iBufSize,
    GLsizei* piLength, GLchar* acSource)
{
    GT_BODY_A4(sglGetShaderSource,uiShader,iBufSize,piLength,acSource);
}
//----------------------------------------------------------------------------
GLint GTglGetUniformLocation (GLuint uiProgram, const GLchar* acName)
{
    // TO DO:  Check the default return value of 0 for correctness.
    GT_BODY_A2_RET(sglGetUniformLocation,uiProgram,acName,GLint,0);
}
//----------------------------------------------------------------------------
void GTglGetUniformfv (GLuint uiProgram, GLint uiLocation, GLfloat* afParam)
{
    GT_BODY_A3(sglGetUniformfv,uiProgram,uiLocation,afParam);
}
//----------------------------------------------------------------------------
void GTglGetUniformiv (GLuint uiProgram, GLint uiLocation, GLint* aiParam)
{
    GT_BODY_A3(sglGetUniformiv,uiProgram,uiLocation,aiParam);
}
//----------------------------------------------------------------------------
void GTglGetVertexAttribdv (GLuint uiIndex, GLenum ePName, GLdouble* adParam)
{
    GT_BODY_A3(sglGetVertexAttribdv,uiIndex,ePName,adParam);
}
//----------------------------------------------------------------------------
void GTglGetVertexAttribfv (GLuint uiIndex, GLenum ePName, GLfloat* afParam)
{
    GT_BODY_A3(sglGetVertexAttribfv,uiIndex,ePName,afParam);
}
//----------------------------------------------------------------------------
void GTglGetVertexAttribiv (GLuint uiIndex, GLenum ePName, GLint* aiParam)
{
    GT_BODY_A3(sglGetVertexAttribiv,uiIndex,ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTglGetVertexAttribPointerv (GLuint uiIndex, GLenum ePName,
    GLvoid** apvParam)
{
    GT_BODY_A3(sglGetVertexAttribPointerv,uiIndex,ePName,apvParam);
}
//----------------------------------------------------------------------------
GLboolean GTglIsProgram (GLuint uiProgram)
{
    GT_BODY_A1_RET(sglIsProgram,uiProgram,GLboolean,0);
}
//----------------------------------------------------------------------------
GLboolean GTglIsShader (GLuint uiShader)
{
    GT_BODY_A1_RET(sglIsShader,uiShader,GLboolean,0);
}
//----------------------------------------------------------------------------
void GTglLinkProgram (GLuint uiProgram)
{
    GT_BODY_A1(sglLinkProgram,uiProgram);
}
//----------------------------------------------------------------------------
void GTglShaderSource (GLuint uiShader, GLsizei iCount,
    const GLchar** aacString, const GLint* piLength)
{
    GT_BODY_A4(sglShaderSource,uiShader,iCount,aacString,piLength);
}
//----------------------------------------------------------------------------
void GTglUseProgram (GLuint uiProgram)
{
    GT_BODY_A1(sglUseProgram,uiProgram);
}
//----------------------------------------------------------------------------
void GTglUniform1f (GLint uiLocation, GLfloat fV0)
{
    GT_BODY_A2(sglUniform1f,uiLocation,fV0);
}
//----------------------------------------------------------------------------
void GTglUniform2f (GLint uiLocation, GLfloat fV0, GLfloat fV1)
{
    GT_BODY_A3(sglUniform2f,uiLocation,fV0,fV1);
}
//----------------------------------------------------------------------------
void GTglUniform3f (GLint uiLocation, GLfloat fV0, GLfloat fV1, GLfloat fV2)
{
    GT_BODY_A4(sglUniform3f,uiLocation,fV0,fV1,fV2);
}
//----------------------------------------------------------------------------
void GTglUniform4f (GLint uiLocation, GLfloat fV0, GLfloat fV1, GLfloat fV2,
    GLfloat fV3)
{
    GT_BODY_A5(sglUniform4f,uiLocation,fV0,fV1,fV2,fV3);
}
//----------------------------------------------------------------------------
void GTglUniform1i (GLint uiLocation, GLint iV0)
{
    GT_BODY_A2(sglUniform1i,uiLocation,iV0);
}
//----------------------------------------------------------------------------
void GTglUniform2i (GLint uiLocation, GLint iV0, GLint iV1)
{
    GT_BODY_A3(sglUniform2i,uiLocation,iV0,iV1);
}
//----------------------------------------------------------------------------
void GTglUniform3i (GLint uiLocation, GLint iV0, GLint iV1, GLint iV2)
{
    GT_BODY_A4(sglUniform3i,uiLocation,iV0,iV1,iV2);
}
//----------------------------------------------------------------------------
void GTglUniform4i (GLint uiLocation, GLint iV0, GLint iV1, GLint iV2,
    GLint iV3)
{
    GT_BODY_A5(sglUniform4i,uiLocation,iV0,iV1,iV2,iV3);
}
//----------------------------------------------------------------------------
void GTglUniform1fv (GLint uiLocation, GLsizei iCount, const GLfloat* afValue)
{
    GT_BODY_A3(sglUniform1fv,uiLocation,iCount,afValue);
}
//----------------------------------------------------------------------------
void GTglUniform2fv (GLint uiLocation, GLsizei iCount, const GLfloat* afValue)
{
    GT_BODY_A3(sglUniform2fv,uiLocation,iCount,afValue);
}
//----------------------------------------------------------------------------
void GTglUniform3fv (GLint uiLocation, GLsizei iCount, const GLfloat* afValue)
{
    GT_BODY_A3(sglUniform3fv,uiLocation,iCount,afValue);
}
//----------------------------------------------------------------------------
void GTglUniform4fv (GLint uiLocation, GLsizei iCount, const GLfloat* afValue)
{
    GT_BODY_A3(sglUniform4fv,uiLocation,iCount,afValue);
}
//----------------------------------------------------------------------------
void GTglUniform1iv (GLint uiLocation, GLsizei iCount, const GLint* aiValue)
{
    GT_BODY_A3(sglUniform1iv,uiLocation,iCount,aiValue);
}
//----------------------------------------------------------------------------
void GTglUniform2iv (GLint uiLocation, GLsizei iCount, const GLint* aiValue)
{
    GT_BODY_A3(sglUniform2iv,uiLocation,iCount,aiValue);
}
//----------------------------------------------------------------------------
void GTglUniform3iv (GLint uiLocation, GLsizei iCount, const GLint* aiValue)
{
    GT_BODY_A3(sglUniform3iv,uiLocation,iCount,aiValue);
}
//----------------------------------------------------------------------------
void GTglUniform4iv (GLint uiLocation, GLsizei iCount, const GLint* aiValue)
{
    GT_BODY_A3(sglUniform4iv,uiLocation,iCount,aiValue);
}
//----------------------------------------------------------------------------
void GTglUniformMatrix2fv (GLint uiLocation, GLsizei iCount,
    GLboolean ucTranspose, const GLfloat* afValue)
{
    GT_BODY_A4(sglUniformMatrix2fv,uiLocation,iCount,ucTranspose,afValue);
}
//----------------------------------------------------------------------------
void GTglUniformMatrix3fv (GLint uiLocation, GLsizei iCount,
    GLboolean ucTranspose, const GLfloat* afValue)
{
    GT_BODY_A4(sglUniformMatrix3fv,uiLocation,iCount,ucTranspose,afValue);
}
//----------------------------------------------------------------------------
void GTglUniformMatrix4fv (GLint uiLocation, GLsizei iCount,
    GLboolean ucTranspose, const GLfloat* afValue)
{
    GT_BODY_A4(sglUniformMatrix4fv,uiLocation,iCount,ucTranspose,afValue);
}
//----------------------------------------------------------------------------
void GTglValidateProgram (GLuint uiProgram)
{
    GT_BODY_A1(sglValidateProgram,uiProgram);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib1d (GLuint uiIndex, GLdouble dX)
{
    GT_BODY_A2(sglVertexAttrib1d,uiIndex,dX);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib1dv (GLuint uiIndex, const GLdouble* pdX)
{
    GT_BODY_A2(sglVertexAttrib1dv,uiIndex,pdX);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib1f (GLuint uiIndex, GLfloat fX)
{
    GT_BODY_A2(sglVertexAttrib1f,uiIndex,fX);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib1fv (GLuint uiIndex, const GLfloat* pfX)
{
    GT_BODY_A2(sglVertexAttrib1fv,uiIndex,pfX);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib1s (GLuint uiIndex, GLshort sX)
{
    GT_BODY_A2(sglVertexAttrib1s,uiIndex,sX);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib1sv (GLuint uiIndex, const GLshort* psX)
{
    GT_BODY_A2(sglVertexAttrib1sv,uiIndex,psX);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib2d (GLuint uiIndex, GLdouble dX, GLdouble dY)
{
    GT_BODY_A3(sglVertexAttrib2d,uiIndex,dX,dY);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib2dv (GLuint uiIndex, const GLdouble* adXY)
{
    GT_BODY_A2(sglVertexAttrib2dv,uiIndex,adXY);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib2f (GLuint uiIndex, GLfloat fX, GLfloat fY)
{
    GT_BODY_A3(sglVertexAttrib2f,uiIndex,fX,fY);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib2fv (GLuint uiIndex, const GLfloat* afXY)
{
    GT_BODY_A2(sglVertexAttrib2fv,uiIndex,afXY);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib2s (GLuint uiIndex, GLshort sX, GLshort sY)
{
    GT_BODY_A3(sglVertexAttrib2s,uiIndex,sX,sY);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib2sv (GLuint uiIndex, const GLshort* asXY)
{
    GT_BODY_A2(sglVertexAttrib2sv,uiIndex,asXY);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib3d (GLuint uiIndex, GLdouble dX, GLdouble dY,
    GLdouble dZ)
{
    GT_BODY_A4(sglVertexAttrib3d,uiIndex,dX,dY,dZ);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib3dv (GLuint uiIndex, const GLdouble* adXYZ)
{
    GT_BODY_A2(sglVertexAttrib3dv,uiIndex,adXYZ);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib3f (GLuint uiIndex, GLfloat fX, GLfloat fY, GLfloat fZ)
{
    GT_BODY_A4(sglVertexAttrib3f,uiIndex,fX,fY,fZ);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib3fv (GLuint uiIndex, const GLfloat* afXYZ)
{
    GT_BODY_A2(sglVertexAttrib3fv,uiIndex,afXYZ);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib3s (GLuint uiIndex, GLshort sX, GLshort sY, GLshort sZ)
{
    GT_BODY_A4(sglVertexAttrib3s,uiIndex,sX,sY,sZ);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib3sv (GLuint uiIndex, const GLshort* asXYZ)
{
    GT_BODY_A2(sglVertexAttrib3sv,uiIndex,asXYZ);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib4Nbv (GLuint uiIndex, const GLbyte* acXYZW)
{
    GT_BODY_A2(sglVertexAttrib4Nbv,uiIndex,acXYZW);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib4Niv (GLuint uiIndex, const GLint* aiXYZW)
{
    GT_BODY_A2(sglVertexAttrib4Niv,uiIndex,aiXYZW);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib4Nsv (GLuint uiIndex, const GLshort* asXYZW)
{
    GT_BODY_A2(sglVertexAttrib4Nsv,uiIndex,asXYZW);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib4Nub (GLuint uiIndex, GLubyte ucX, GLubyte ucY,
    GLubyte ucZ, GLubyte ucW)
{
    GT_BODY_A5(sglVertexAttrib4Nub,uiIndex,ucX,ucY,ucZ,ucW);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib4Nubv (GLuint uiIndex, const GLubyte* aucXYZW)
{
    GT_BODY_A2(sglVertexAttrib4Nubv,uiIndex,aucXYZW);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib4Nuiv (GLuint uiIndex, const GLuint* auiXYZW)
{
    GT_BODY_A2(sglVertexAttrib4Nuiv,uiIndex,auiXYZW);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib4Nusv (GLuint uiIndex, const GLushort* ausXYZW)
{
    GT_BODY_A2(sglVertexAttrib4Nusv,uiIndex,ausXYZW);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib4bv (GLuint uiIndex, const GLbyte* acXYZW)
{
    GT_BODY_A2(sglVertexAttrib4bv,uiIndex,acXYZW);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib4d (GLuint uiIndex, GLdouble dX, GLdouble dY,
    GLdouble dZ, GLdouble dW)
{
    GT_BODY_A5(sglVertexAttrib4d,uiIndex,dX,dY,dZ,dW);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib4dv (GLuint uiIndex, const GLdouble* adXYZW)
{
    GT_BODY_A2(sglVertexAttrib4dv,uiIndex,adXYZW);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib4f (GLuint uiIndex, GLfloat fX, GLfloat fY, GLfloat fZ,
    GLfloat fW)
{
    GT_BODY_A5(sglVertexAttrib4f,uiIndex,fX,fY,fZ,fW);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib4fv (GLuint uiIndex, const GLfloat* afXYZW)
{
    GT_BODY_A2(sglVertexAttrib4fv,uiIndex,afXYZW);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib4iv (GLuint uiIndex, const GLint* aiXYZW)
{
    GT_BODY_A2(sglVertexAttrib4iv,uiIndex,aiXYZW);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib4s (GLuint uiIndex, GLshort sX, GLshort sY, GLshort sZ,
    GLshort sW)
{
    GT_BODY_A5(sglVertexAttrib4s,uiIndex,sX,sY,sZ,sW);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib4sv (GLuint uiIndex, const GLshort* asXYZW)
{
    GT_BODY_A2(sglVertexAttrib4sv,uiIndex,asXYZW);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib4ubv (GLuint uiIndex, const GLubyte* aucXYZW)
{
    GT_BODY_A2(sglVertexAttrib4ubv,uiIndex,aucXYZW);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib4uiv (GLuint uiIndex, const GLuint* auiXYZW)
{
    GT_BODY_A2(sglVertexAttrib4uiv,uiIndex,auiXYZW);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib4usv (GLuint uiIndex, const GLushort* ausXYZW)
{
    GT_BODY_A2(sglVertexAttrib4usv,uiIndex,ausXYZW);
}
//----------------------------------------------------------------------------
void GTglVertexAttribPointer (GLuint uiIndex, GLint iSize, GLenum eType,
     GLboolean ucNormalized, GLsizei iStride, const GLvoid* pvPointer)
{
    GT_BODY_A6(sglVertexAttribPointer,uiIndex,iSize,eType,ucNormalized,
        iStride,pvPointer);
}
//----------------------------------------------------------------------------
void GTInitOpenGL20 ()
{
    if (GTGetVersion() >= GT_VERSION_2_0)
    {
        GTExistsOpenGL20 = 1;

        GT_GET_FUNCTION(sglGetShaderiv);
        GT_GET_FUNCTION(sglIsShader);
        GT_GET_FUNCTION(sglStencilMaskSeparate);

        // GL_ARB_draw_buffers
        GT_GET_FUNCTION(sglDrawBuffers);

        // GL_ARB_shader_objects
        GT_GET_FUNCTION(sglAttachShader);       // glAttachObjectARB
        GT_GET_FUNCTION(sglCompileShader);
        GT_GET_FUNCTION(sglCreateProgram);      // glCreateProgramObjectARB
        GT_GET_FUNCTION(sglCreateShader);       // glCreateShaderObjectARB
        GT_GET_FUNCTION(sglDeleteProgram);      // glDeleteProgramObjectARB
        GT_GET_FUNCTION(sglDeleteShader);       // glDeleteProgramObjectARB
        GT_GET_FUNCTION(sglDetachShader);       // glDetachObjectARB
        GT_GET_FUNCTION(sglGetActiveUniform);
        GT_GET_FUNCTION(sglGetAttachedShaders); // glGetAttachObjectsARB
        GT_GET_FUNCTION(sglGetProgramInfoLog);  // glGetInfoLogARB
        GT_GET_FUNCTION(sglGetShaderInfoLog);   // glGetInfoLogARB
        GT_GET_FUNCTION(sglGetShaderSource);
        GT_GET_FUNCTION(sglGetUniformfv);
        GT_GET_FUNCTION(sglGetUniformiv);
        GT_GET_FUNCTION(sglGetUniformLocation);
        GT_GET_FUNCTION(sglLinkProgram);
        GT_GET_FUNCTION(sglShaderSource);
        GT_GET_FUNCTION(sglUniform1f);
        GT_GET_FUNCTION(sglUniform2f);
        GT_GET_FUNCTION(sglUniform3f);
        GT_GET_FUNCTION(sglUniform4f);
        GT_GET_FUNCTION(sglUniform1i);
        GT_GET_FUNCTION(sglUniform2i);
        GT_GET_FUNCTION(sglUniform3i);
        GT_GET_FUNCTION(sglUniform4i);
        GT_GET_FUNCTION(sglUniform1fv);
        GT_GET_FUNCTION(sglUniform2fv);
        GT_GET_FUNCTION(sglUniform3fv);
        GT_GET_FUNCTION(sglUniform4fv);
        GT_GET_FUNCTION(sglUniform1iv);
        GT_GET_FUNCTION(sglUniform2iv);
        GT_GET_FUNCTION(sglUniform3iv);
        GT_GET_FUNCTION(sglUniform4iv);
        GT_GET_FUNCTION(sglUniformMatrix2fv);
        GT_GET_FUNCTION(sglUniformMatrix3fv);
        GT_GET_FUNCTION(sglUniformMatrix4fv);
        GT_GET_FUNCTION(sglUseProgram);         // glUseProgramObjectARB
        GT_GET_FUNCTION(sglValidateProgram);

        // GL_ARB_fragment_program and GL_ARB_vertex_program
        GT_GET_FUNCTION(sglDisableVertexAttribArray);
        GT_GET_FUNCTION(sglEnableVertexAttribArray);
        GT_GET_FUNCTION(sglGetProgramiv);
        GT_GET_FUNCTION(sglGetVertexAttribdv);
        GT_GET_FUNCTION(sglGetVertexAttribfv);
        GT_GET_FUNCTION(sglGetVertexAttribiv);
        GT_GET_FUNCTION(sglGetVertexAttribPointerv);
        GT_GET_FUNCTION(sglIsProgram);
        GT_GET_FUNCTION(sglVertexAttrib1d);
        GT_GET_FUNCTION(sglVertexAttrib1dv);
        GT_GET_FUNCTION(sglVertexAttrib1f);
        GT_GET_FUNCTION(sglVertexAttrib1fv);
        GT_GET_FUNCTION(sglVertexAttrib1s);
        GT_GET_FUNCTION(sglVertexAttrib1sv);
        GT_GET_FUNCTION(sglVertexAttrib2d);
        GT_GET_FUNCTION(sglVertexAttrib2dv);
        GT_GET_FUNCTION(sglVertexAttrib2f);
        GT_GET_FUNCTION(sglVertexAttrib2fv);
        GT_GET_FUNCTION(sglVertexAttrib2s);
        GT_GET_FUNCTION(sglVertexAttrib2sv);
        GT_GET_FUNCTION(sglVertexAttrib3d);
        GT_GET_FUNCTION(sglVertexAttrib3dv);
        GT_GET_FUNCTION(sglVertexAttrib3f);
        GT_GET_FUNCTION(sglVertexAttrib3fv);
        GT_GET_FUNCTION(sglVertexAttrib3s);
        GT_GET_FUNCTION(sglVertexAttrib3sv);
        GT_GET_FUNCTION(sglVertexAttrib4Nbv);
        GT_GET_FUNCTION(sglVertexAttrib4Niv);
        GT_GET_FUNCTION(sglVertexAttrib4Nsv);
        GT_GET_FUNCTION(sglVertexAttrib4Nub);
        GT_GET_FUNCTION(sglVertexAttrib4Nubv);
        GT_GET_FUNCTION(sglVertexAttrib4Nuiv);
        GT_GET_FUNCTION(sglVertexAttrib4Nusv);
        GT_GET_FUNCTION(sglVertexAttrib4bv);
        GT_GET_FUNCTION(sglVertexAttrib4d);
        GT_GET_FUNCTION(sglVertexAttrib4dv);
        GT_GET_FUNCTION(sglVertexAttrib4f);
        GT_GET_FUNCTION(sglVertexAttrib4fv);
        GT_GET_FUNCTION(sglVertexAttrib4iv);
        GT_GET_FUNCTION(sglVertexAttrib4s);
        GT_GET_FUNCTION(sglVertexAttrib4sv);
        GT_GET_FUNCTION(sglVertexAttrib4ubv);
        GT_GET_FUNCTION(sglVertexAttrib4uiv);
        GT_GET_FUNCTION(sglVertexAttrib4usv);
        GT_GET_FUNCTION(sglVertexAttribPointer);

        // GL_ARB_vertex_shader
        GT_GET_FUNCTION(sglBindAttribLocation);
        GT_GET_FUNCTION(sglGetActiveAttrib);
        GT_GET_FUNCTION(sglGetAttribLocation);

        // GL_EXT_blend_equation_separate
        GT_GET_FUNCTION(sglBlendEquationSeparate);

        // GL_ATI_separate_stencil
        GT_GET_FUNCTION(sglStencilOpSeparate);
        GT_GET_FUNCTION(sglStencilFuncSeparate);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// OpenGL 2.1
//----------------------------------------------------------------------------
int GTExistsOpenGL21 = 0;

typedef void (STDCALL* PsglUniformMatrix2x3fv) (GLint, GLsizei, GLboolean,
    const GLfloat*);
typedef void (STDCALL* PsglUniformMatrix3x2fv) (GLint, GLsizei, GLboolean,
    const GLfloat*);
typedef void (STDCALL* PsglUniformMatrix2x4fv) (GLint, GLsizei, GLboolean,
    const GLfloat*);
typedef void (STDCALL* PsglUniformMatrix4x2fv) (GLint, GLsizei, GLboolean,
    const GLfloat*);
typedef void (STDCALL* PsglUniformMatrix3x4fv) (GLint, GLsizei, GLboolean,
    const GLfloat*);
typedef void (STDCALL* PsglUniformMatrix4x3fv) (GLint, GLsizei, GLboolean,
    const GLfloat*);

static PsglUniformMatrix2x3fv sglUniformMatrix2x3fv = 0;
static PsglUniformMatrix3x2fv sglUniformMatrix3x2fv = 0;
static PsglUniformMatrix2x4fv sglUniformMatrix2x4fv = 0;
static PsglUniformMatrix4x2fv sglUniformMatrix4x2fv = 0;
static PsglUniformMatrix3x4fv sglUniformMatrix3x4fv = 0;
static PsglUniformMatrix4x3fv sglUniformMatrix4x3fv = 0;
//----------------------------------------------------------------------------
void GTglUniformMatrix2x3fv (GLint location, GLsizei count,
    GLboolean transpose, const GLfloat* value)
{
    GT_BODY_A4(sglUniformMatrix2x3fv,location,count,transpose,value);
}
//----------------------------------------------------------------------------
void GTglUniformMatrix3x2fv (GLint location, GLsizei count,
    GLboolean transpose, const GLfloat* value)
{
    GT_BODY_A4(sglUniformMatrix3x2fv,location,count,transpose,value);
}
//----------------------------------------------------------------------------
void GTglUniformMatrix2x4fv (GLint location, GLsizei count,
    GLboolean transpose, const GLfloat* value)
{
    GT_BODY_A4(sglUniformMatrix2x4fv,location,count,transpose,value);
}
//----------------------------------------------------------------------------
void GTglUniformMatrix4x2fv (GLint location, GLsizei count,
    GLboolean transpose, const GLfloat* value)
{
    GT_BODY_A4(sglUniformMatrix4x2fv,location,count,transpose,value);
}
//----------------------------------------------------------------------------
void GTglUniformMatrix3x4fv (GLint location, GLsizei count,
    GLboolean transpose, const GLfloat* value)
{
    GT_BODY_A4(sglUniformMatrix3x4fv,location,count,transpose,value);
}
//----------------------------------------------------------------------------
void GTglUniformMatrix4x3fv (GLint location, GLsizei count,
    GLboolean transpose, const GLfloat* value)
{
    GT_BODY_A4(sglUniformMatrix4x3fv,location,count,transpose,value);
}
//----------------------------------------------------------------------------
void GTInitOpenGL21 ()
{
    if (GTGetVersion() >= GT_VERSION_2_1)
    {
        GTExistsOpenGL21 = 1;

        GT_GET_FUNCTION(sglUniformMatrix2x3fv);
        GT_GET_FUNCTION(sglUniformMatrix3x2fv);
        GT_GET_FUNCTION(sglUniformMatrix2x4fv);
        GT_GET_FUNCTION(sglUniformMatrix4x2fv);
        GT_GET_FUNCTION(sglUniformMatrix3x4fv);
        GT_GET_FUNCTION(sglUniformMatrix4x3fv);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// OpenGL 3.0
//----------------------------------------------------------------------------
int GTExistsOpenGL30 = 0;

typedef void (STDCALL* PsglColorMaski) (GLuint, GLboolean, GLboolean,
    GLboolean, GLboolean);
typedef void (STDCALL* PsglGetBooleani_v) (GLenum, GLuint, GLboolean*);
typedef void (STDCALL* PsglGetIntegeri_v) (GLenum, GLuint, GLint*);
typedef void (STDCALL* PsglEnablei) (GLenum, GLuint);
typedef void (STDCALL* PsglDisablei) (GLenum, GLuint);
typedef GLboolean (STDCALL* PsglIsEnabledi) (GLenum, GLuint);
typedef void (STDCALL* PsglBeginTransformFeedback) (GLenum);
typedef void (STDCALL* PsglEndTransformFeedback) (void);
typedef void (STDCALL* PsglBindBufferRange) (GLenum, GLuint, GLuint, GLintptr,
    GLsizeiptr);
typedef void (STDCALL* PsglBindBufferBase) (GLenum, GLuint, GLuint);
typedef void (STDCALL* PsglTransformFeedbackVaryings) (GLuint, GLsizei,
    const GLchar**, GLenum);
typedef void (STDCALL* PsglGetTransformFeedbackVarying) (GLuint, GLuint,
    GLsizei, GLsizei*, GLsizei*, GLenum*, GLchar*);
typedef void (STDCALL* PsglClampColor) (GLenum, GLenum);
typedef void (STDCALL* PsglBeginConditionalRender) (GLuint, GLenum);
typedef void (STDCALL* PsglEndConditionalRender) (void);
typedef void (STDCALL* PsglVertexAttribIPointer) (GLuint, GLint, GLenum,
    GLsizei, const GLvoid*);
typedef void (STDCALL* PsglGetVertexAttribIiv) (GLuint, GLenum, GLint*);
typedef void (STDCALL* PsglGetVertexAttribIuiv) (GLuint, GLenum, GLuint*);
typedef void (STDCALL* PsglVertexAttribI1i) (GLuint, GLint);
typedef void (STDCALL* PsglVertexAttribI2i) (GLuint, GLint, GLint);
typedef void (STDCALL* PsglVertexAttribI3i) (GLuint, GLint, GLint, GLint);
typedef void (STDCALL* PsglVertexAttribI4i) (GLuint, GLint, GLint, GLint,
    GLint);
typedef void (STDCALL* PsglVertexAttribI1ui) (GLuint, GLuint);
typedef void (STDCALL* PsglVertexAttribI2ui) (GLuint, GLuint, GLuint);
typedef void (STDCALL* PsglVertexAttribI3ui) (GLuint, GLuint, GLuint, GLuint);
typedef void (STDCALL* PsglVertexAttribI4ui) (GLuint, GLuint, GLuint, GLuint,
    GLuint);
typedef void (STDCALL* PsglVertexAttribI1iv) (GLuint, const GLint*);
typedef void (STDCALL* PsglVertexAttribI2iv) (GLuint, const GLint*);
typedef void (STDCALL* PsglVertexAttribI3iv) (GLuint, const GLint*);
typedef void (STDCALL* PsglVertexAttribI4iv) (GLuint, const GLint*);
typedef void (STDCALL* PsglVertexAttribI1uiv) (GLuint, const GLuint*);
typedef void (STDCALL* PsglVertexAttribI2uiv) (GLuint, const GLuint*);
typedef void (STDCALL* PsglVertexAttribI3uiv) (GLuint, const GLuint*);
typedef void (STDCALL* PsglVertexAttribI4uiv) (GLuint, const GLuint*);
typedef void (STDCALL* PsglVertexAttribI4bv) (GLuint, const GLbyte*);
typedef void (STDCALL* PsglVertexAttribI4sv) (GLuint, const GLshort*);
typedef void (STDCALL* PsglVertexAttribI4ubv) (GLuint, const GLubyte*);
typedef void (STDCALL* PsglVertexAttribI4usv) (GLuint, const GLushort*);
typedef void (STDCALL* PsglGetUniformuiv) (GLuint, GLint, GLuint*);
typedef void (STDCALL* PsglBindFragDataLocation) (GLuint, GLuint,
    const GLchar*);
typedef GLint (STDCALL* PsglGetFragDataLocation) (GLuint, const GLchar*);
typedef void (STDCALL* PsglUniform1ui) (GLint, GLuint);
typedef void (STDCALL* PsglUniform2ui) (GLint, GLuint, GLuint);
typedef void (STDCALL* PsglUniform3ui) (GLint, GLuint, GLuint, GLuint);
typedef void (STDCALL* PsglUniform4ui) (GLint, GLuint, GLuint, GLuint,
    GLuint);
typedef void (STDCALL* PsglUniform1uiv) (GLint, GLsizei, const GLuint*);
typedef void (STDCALL* PsglUniform2uiv) (GLint, GLsizei, const GLuint*);
typedef void (STDCALL* PsglUniform3uiv) (GLint, GLsizei, const GLuint*);
typedef void (STDCALL* PsglUniform4uiv) (GLint, GLsizei, const GLuint*);
typedef void (STDCALL* PsglTexParameterIiv) (GLenum, GLenum, const GLint*);
typedef void (STDCALL* PsglTexParameterIuiv) (GLenum, GLenum,
    const GLuint*);
typedef void (STDCALL* PsglGetTexParameterIiv) (GLenum, GLenum, GLint*);
typedef void (STDCALL* PsglGetTexParameterIuiv) (GLenum, GLenum, GLuint*);
typedef void (STDCALL* PsglClearBufferiv) (GLenum, GLint, const GLint*);
typedef void (STDCALL* PsglClearBufferuiv) (GLenum, GLint, const GLuint*);
typedef void (STDCALL* PsglClearBufferfv) (GLenum, GLint, const GLfloat*);
typedef void (STDCALL* PsglClearBufferfi) (GLenum, GLint, GLfloat, GLint);
typedef const GLubyte* (STDCALL* PsglGetStringi) (GLenum, GLuint);

static PsglColorMaski sglColorMaski = 0;
static PsglGetBooleani_v sglGetBooleani_v = 0;
static PsglGetIntegeri_v sglGetIntegeri_v = 0;
static PsglEnablei sglEnablei = 0;
static PsglDisablei sglDisablei = 0;
static PsglIsEnabledi sglIsEnabledi = 0;
static PsglBeginTransformFeedback sglBeginTransformFeedback = 0;
static PsglEndTransformFeedback sglEndTransformFeedback = 0;
static PsglBindBufferRange sglBindBufferRange = 0;
static PsglBindBufferBase sglBindBufferBase = 0;
static PsglTransformFeedbackVaryings sglTransformFeedbackVaryings = 0;
static PsglGetTransformFeedbackVarying sglGetTransformFeedbackVarying = 0;
static PsglClampColor sglClampColor = 0;
static PsglBeginConditionalRender sglBeginConditionalRender = 0;
static PsglEndConditionalRender sglEndConditionalRender = 0;
static PsglVertexAttribIPointer sglVertexAttribIPointer = 0;
static PsglGetVertexAttribIiv sglGetVertexAttribIiv = 0;
static PsglGetVertexAttribIuiv sglGetVertexAttribIuiv = 0;
static PsglVertexAttribI1i sglVertexAttribI1i = 0;
static PsglVertexAttribI2i sglVertexAttribI2i = 0;
static PsglVertexAttribI3i sglVertexAttribI3i = 0;
static PsglVertexAttribI4i sglVertexAttribI4i = 0;
static PsglVertexAttribI1ui sglVertexAttribI1ui = 0;
static PsglVertexAttribI2ui sglVertexAttribI2ui = 0;
static PsglVertexAttribI3ui sglVertexAttribI3ui = 0;
static PsglVertexAttribI4ui sglVertexAttribI4ui = 0;
static PsglVertexAttribI1iv sglVertexAttribI1iv = 0;
static PsglVertexAttribI2iv sglVertexAttribI2iv = 0;
static PsglVertexAttribI3iv sglVertexAttribI3iv = 0;
static PsglVertexAttribI4iv sglVertexAttribI4iv = 0;
static PsglVertexAttribI1uiv sglVertexAttribI1uiv = 0;
static PsglVertexAttribI2uiv sglVertexAttribI2uiv = 0;
static PsglVertexAttribI3uiv sglVertexAttribI3uiv = 0;
static PsglVertexAttribI4uiv sglVertexAttribI4uiv = 0;
static PsglVertexAttribI4bv sglVertexAttribI4bv = 0;
static PsglVertexAttribI4sv sglVertexAttribI4sv = 0;
static PsglVertexAttribI4ubv sglVertexAttribI4ubv = 0;
static PsglVertexAttribI4usv sglVertexAttribI4usv = 0;
static PsglGetUniformuiv sglGetUniformuiv = 0;
static PsglBindFragDataLocation sglBindFragDataLocation = 0;
static PsglGetFragDataLocation sglGetFragDataLocation = 0;
static PsglUniform1ui sglUniform1ui = 0;
static PsglUniform2ui sglUniform2ui = 0;
static PsglUniform3ui sglUniform3ui = 0;
static PsglUniform4ui sglUniform4ui = 0;
static PsglUniform1uiv sglUniform1uiv = 0;
static PsglUniform2uiv sglUniform2uiv = 0;
static PsglUniform3uiv sglUniform3uiv = 0;
static PsglUniform4uiv sglUniform4uiv = 0;
static PsglTexParameterIiv sglTexParameterIiv = 0;
static PsglTexParameterIuiv sglTexParameterIuiv = 0;
static PsglGetTexParameterIiv sglGetTexParameterIiv = 0;
static PsglGetTexParameterIuiv sglGetTexParameterIuiv = 0;
static PsglClearBufferiv sglClearBufferiv = 0;
static PsglClearBufferuiv sglClearBufferuiv = 0;
static PsglClearBufferfv sglClearBufferfv = 0;
static PsglClearBufferfi sglClearBufferfi = 0;
static PsglGetStringi sglGetStringi = 0;
//----------------------------------------------------------------------------
void GTglColorMaski (GLuint index, GLboolean r, GLboolean g,
    GLboolean b, GLboolean a)
{
    GT_BODY_A5(sglColorMaski,index,r,g,b,a);
}
//----------------------------------------------------------------------------
void GTglGetBooleani_v (GLenum target, GLuint index,
    GLboolean* data)
{
    GT_BODY_A3(sglGetBooleani_v,target,index,data);
}
//----------------------------------------------------------------------------
void GTglGetIntegeri_v (GLenum target, GLuint index, GLint* data)
{
    GT_BODY_A3(sglGetIntegeri_v,target,index,data);
}
//----------------------------------------------------------------------------
void GTglEnablei (GLenum target, GLuint index)
{
    GT_BODY_A2(sglEnablei,target,index);
}
//----------------------------------------------------------------------------
void GTglDisablei (GLenum target, GLuint index)
{
    GT_BODY_A2(sglDisablei,target,index);
}
//----------------------------------------------------------------------------
GLboolean GTglIsEnabledi (GLenum target, GLuint index)
{
    GT_BODY_A2_RET(sglIsEnabledi,target,index,GLboolean,0);
}
//----------------------------------------------------------------------------
void GTglBeginTransformFeedback (GLenum primitiveMode)
{
    GT_BODY_A1(sglBeginTransformFeedback,primitiveMode);
}
//----------------------------------------------------------------------------
void GTglEndTransformFeedback (void)
{
    GT_BODY_A0(sglEndTransformFeedback);
}
//----------------------------------------------------------------------------
void GTglBindBufferRange (GLenum target, GLuint index,
    GLuint buffer, GLintptr offset, GLsizeiptr size)
{
    GT_BODY_A5(sglBindBufferRange,target,index,buffer,offset,size);
}
//----------------------------------------------------------------------------
void GTglBindBufferBase (GLenum target, GLuint index,
    GLuint buffer)
{
    GT_BODY_A3(sglBindBufferBase,target,index,buffer);
}
//----------------------------------------------------------------------------
void GTglTransformFeedbackVaryings (GLuint program, GLsizei count,
    const GLchar** varyings, GLenum bufferMode)
{
    GT_BODY_A4(sglTransformFeedbackVaryings,program,count,varyings,
        bufferMode);
}
//----------------------------------------------------------------------------
void GTglGetTransformFeedbackVarying (GLuint program, GLuint index,
    GLsizei bufSize, GLsizei* length, GLsizei* size, GLenum* type,
    GLchar* name)
{
    GT_BODY_A7(sglGetTransformFeedbackVarying,program,index,bufSize,
        length,size,type,name);
}
//----------------------------------------------------------------------------
void GTglClampColor (GLenum target, GLenum clamp)
{
    GT_BODY_A2(sglClampColor,target,clamp);
}
//----------------------------------------------------------------------------
void GTglBeginConditionalRender (GLuint id, GLenum mode)
{
    GT_BODY_A2(sglBeginConditionalRender,id,mode);
}
//----------------------------------------------------------------------------
void GTglEndConditionalRender (void)
{
    GT_BODY_A0(sglEndConditionalRender);
}
//----------------------------------------------------------------------------
void GTglVertexAttribIPointer (GLuint index, GLint size,
    GLenum type, GLsizei stride, const GLvoid* pointer)
{
    GT_BODY_A5(sglVertexAttribIPointer,index,size,type,stride,pointer);
}
//----------------------------------------------------------------------------
void GTglGetVertexAttribIiv (GLuint index, GLenum pname,
    GLint* params)
{
    GT_BODY_A3(sglGetVertexAttribIiv,index,pname,params);
}
//----------------------------------------------------------------------------
void GTglGetVertexAttribIuiv (GLuint index, GLenum pname,
    GLuint* params)
{
    GT_BODY_A3(sglGetVertexAttribIuiv,index,pname,params);
}
//----------------------------------------------------------------------------
void GTglVertexAttribI1i (GLuint index, GLint x)
{
    GT_BODY_A2(sglVertexAttribI1i,index,x);
}
//----------------------------------------------------------------------------
void GTglVertexAttribI2i (GLuint index, GLint x, GLint y)
{
    GT_BODY_A3(sglVertexAttribI2i,index,x,y);
}
//----------------------------------------------------------------------------
void GTglVertexAttribI3i (GLuint index, GLint x, GLint y, GLint z)
{
    GT_BODY_A4(sglVertexAttribI3i,index,x,y,z);
}
//----------------------------------------------------------------------------
void GTglVertexAttribI4i (GLuint index, GLint x, GLint y, GLint z,
    GLint w)
{
    GT_BODY_A5(sglVertexAttribI4i,index,x,y,z,w);
}
//----------------------------------------------------------------------------
void GTglVertexAttribI1ui (GLuint index, GLuint x)
{
    GT_BODY_A2(sglVertexAttribI1ui,index,x);
}
//----------------------------------------------------------------------------
void GTglVertexAttribI2ui (GLuint index, GLuint x, GLuint y)
{
    GT_BODY_A3(sglVertexAttribI2ui,index,x,y);
}
//----------------------------------------------------------------------------
void GTglVertexAttribI3ui (GLuint index, GLuint x, GLuint y,
    GLuint z)
{
    GT_BODY_A4(sglVertexAttribI3ui,index,x,y,z);
}
//----------------------------------------------------------------------------
void GTglVertexAttribI4ui (GLuint index, GLuint x, GLuint y,
    GLuint z, GLuint w)
{
    GT_BODY_A5(sglVertexAttribI4ui,index,x,y,z,w);
}
//----------------------------------------------------------------------------
void GTglVertexAttribI1iv (GLuint index, const GLint* v)
{
    GT_BODY_A2(sglVertexAttribI1iv,index,v);
}
//----------------------------------------------------------------------------
void GTglVertexAttribI2iv (GLuint index, const GLint* v)
{
    GT_BODY_A2(sglVertexAttribI2iv,index,v);
}
//----------------------------------------------------------------------------
void GTglVertexAttribI3iv (GLuint index, const GLint* v)
{
    GT_BODY_A2(sglVertexAttribI3iv,index,v);
}
//----------------------------------------------------------------------------
void GTglVertexAttribI4iv (GLuint index, const GLint* v)
{
    GT_BODY_A2(sglVertexAttribI4iv,index,v);
}
//----------------------------------------------------------------------------
void GTglVertexAttribI1uiv (GLuint index, const GLuint* v)
{
    GT_BODY_A2(sglVertexAttribI1uiv,index,v);
}
//----------------------------------------------------------------------------
void GTglVertexAttribI2uiv (GLuint index, const GLuint* v)
{
    GT_BODY_A2(sglVertexAttribI2uiv,index,v);
}
//----------------------------------------------------------------------------
void GTglVertexAttribI3uiv (GLuint index, const GLuint* v)
{
    GT_BODY_A2(sglVertexAttribI3uiv,index,v);
}
//----------------------------------------------------------------------------
void GTglVertexAttribI4uiv (GLuint index, const GLuint* v)
{
    GT_BODY_A2(sglVertexAttribI4uiv,index,v);
}
//----------------------------------------------------------------------------
void GTglVertexAttribI4bv (GLuint index, const GLbyte* v)
{
    GT_BODY_A2(sglVertexAttribI4bv,index,v);
}
//----------------------------------------------------------------------------
void GTglVertexAttribI4sv (GLuint index, const GLshort* v)
{
    GT_BODY_A2(sglVertexAttribI4sv,index,v);
}
//----------------------------------------------------------------------------
void GTglVertexAttribI4ubv (GLuint index, const GLubyte* v)
{
    GT_BODY_A2(sglVertexAttribI4ubv,index,v);
}
//----------------------------------------------------------------------------
void GTglVertexAttribI4usv (GLuint index, const GLushort* v)
{
    GT_BODY_A2(sglVertexAttribI4usv,index,v);
}
//----------------------------------------------------------------------------
void GTglGetUniformuiv (GLuint program, GLint location,
    GLuint* params)
{
    GT_BODY_A3(sglGetUniformuiv,program,location,params);
}
//----------------------------------------------------------------------------
void GTglBindFragDataLocation (GLuint program, GLuint color,
    const GLchar* name)
{
    GT_BODY_A3(sglBindFragDataLocation,program,color,name);
}
//----------------------------------------------------------------------------
GLint GTglGetFragDataLocation (GLuint program, const GLchar* name)
{
    GT_BODY_A2_RET(sglGetFragDataLocation,program,name,GLint,0);
}
//----------------------------------------------------------------------------
void GTglUniform1ui (GLint location, GLuint v0)
{
    GT_BODY_A2(sglUniform1ui,location,v0);
}
//----------------------------------------------------------------------------
void GTglUniform2ui (GLint location, GLuint v0, GLuint v1)
{
    GT_BODY_A3(sglUniform2ui,location,v0,v1);
}
//----------------------------------------------------------------------------
void GTglUniform3ui (GLint location, GLuint v0, GLuint v1,  GLuint v2)
{
    GT_BODY_A4(sglUniform3ui,location,v0,v1,v2);
}
//----------------------------------------------------------------------------
void GTglUniform4ui (GLint location, GLuint v0, GLuint v1,  GLuint v2,
    GLuint v3)
{
    GT_BODY_A5(sglUniform4ui,location,v0,v1,v2,v3);
}
//----------------------------------------------------------------------------
void GTglUniform1uiv (GLint location, GLsizei count, const GLuint* value)
{
    GT_BODY_A3(sglUniform1uiv,location,count,value);
}
//----------------------------------------------------------------------------
void GTglUniform2uiv (GLint location, GLsizei count, const GLuint* value)
{
    GT_BODY_A3(sglUniform2uiv,location,count,value);
}
//----------------------------------------------------------------------------
void GTglUniform3uiv (GLint location, GLsizei count, const GLuint* value)
{
    GT_BODY_A3(sglUniform3uiv,location,count,value);
}
//----------------------------------------------------------------------------
void GTglUniform4uiv (GLint location, GLsizei count, const GLuint* value)
{
    GT_BODY_A3(sglUniform3uiv,location,count,value);
}
//----------------------------------------------------------------------------
void GTglTexParameterIiv (GLenum target, GLenum pname, const GLint* params)
{
    GT_BODY_A3(sglTexParameterIiv,target,pname,params);
}
//----------------------------------------------------------------------------
void GTglTexParameterIuiv (GLenum target, GLenum pname, const GLuint* params)
{
    GT_BODY_A3(sglTexParameterIuiv,target,pname,params);
}
//----------------------------------------------------------------------------
void GTglGetTexParameterIiv (GLenum target, GLenum pname, GLint* params)
{
    GT_BODY_A3(sglGetTexParameterIiv,target,pname,params);
}
//----------------------------------------------------------------------------
void GTglGetTexParameterIuiv (GLenum target, GLenum pname, GLuint* params)
{
    GT_BODY_A3(sglGetTexParameterIuiv,target,pname,params);
}
//----------------------------------------------------------------------------
void GTglClearBufferiv (GLenum buffer, GLint drawbuffer, const GLint* value)
{
    GT_BODY_A3(sglClearBufferiv,buffer,drawbuffer,value);
}
//----------------------------------------------------------------------------
void GTglClearBufferuiv (GLenum buffer, GLint drawbuffer, const GLuint* value)
{
    GT_BODY_A3(sglClearBufferuiv,buffer,drawbuffer,value);
}
//----------------------------------------------------------------------------
void GTglClearBufferfv (GLenum buffer, GLint drawbuffer, const GLfloat* value)
{
    GT_BODY_A3(sglClearBufferfv,buffer,drawbuffer,value);
}
//----------------------------------------------------------------------------
void GTglClearBufferfi (GLenum buffer, GLint drawbuffer, GLfloat depth,
    GLint stencil)
{
    GT_BODY_A4(sglClearBufferfi,buffer,drawbuffer,depth,stencil);
}
//----------------------------------------------------------------------------
const GLubyte* GTglGetStringi (GLenum name, GLuint index)
{
    GT_BODY_A2_RET(sglGetStringi,name,index,const GLubyte*,0);
}
//----------------------------------------------------------------------------
void GTInitOpenGL30 ()
{
    if (GTGetVersion() >= GT_VERSION_3_0)
    {
        GTExistsOpenGL30 = 1;

        GT_GET_FUNCTION(sglColorMaski);
        GT_GET_FUNCTION(sglGetBooleani_v);
        GT_GET_FUNCTION(sglGetIntegeri_v);
        GT_GET_FUNCTION(sglEnablei);
        GT_GET_FUNCTION(sglDisablei);
        GT_GET_FUNCTION(sglIsEnabledi);
        GT_GET_FUNCTION(sglBeginTransformFeedback);
        GT_GET_FUNCTION(sglEndTransformFeedback);
        GT_GET_FUNCTION(sglBindBufferRange);
        GT_GET_FUNCTION(sglBindBufferBase);
        GT_GET_FUNCTION(sglTransformFeedbackVaryings);
        GT_GET_FUNCTION(sglGetTransformFeedbackVarying);
        GT_GET_FUNCTION(sglClampColor);
        GT_GET_FUNCTION(sglBeginConditionalRender);
        GT_GET_FUNCTION(sglEndConditionalRender);
        GT_GET_FUNCTION(sglVertexAttribIPointer);
        GT_GET_FUNCTION(sglGetVertexAttribIiv);
        GT_GET_FUNCTION(sglGetVertexAttribIuiv);
        GT_GET_FUNCTION(sglVertexAttribI1i);
        GT_GET_FUNCTION(sglVertexAttribI2i);
        GT_GET_FUNCTION(sglVertexAttribI3i);
        GT_GET_FUNCTION(sglVertexAttribI4i);
        GT_GET_FUNCTION(sglVertexAttribI1ui);
        GT_GET_FUNCTION(sglVertexAttribI2ui);
        GT_GET_FUNCTION(sglVertexAttribI3ui);
        GT_GET_FUNCTION(sglVertexAttribI4ui);
        GT_GET_FUNCTION(sglVertexAttribI1iv);
        GT_GET_FUNCTION(sglVertexAttribI2iv);
        GT_GET_FUNCTION(sglVertexAttribI3iv);
        GT_GET_FUNCTION(sglVertexAttribI4iv);
        GT_GET_FUNCTION(sglVertexAttribI1uiv);
        GT_GET_FUNCTION(sglVertexAttribI2uiv);
        GT_GET_FUNCTION(sglVertexAttribI3uiv);
        GT_GET_FUNCTION(sglVertexAttribI4uiv);
        GT_GET_FUNCTION(sglVertexAttribI4bv);
        GT_GET_FUNCTION(sglVertexAttribI4sv);
        GT_GET_FUNCTION(sglVertexAttribI4ubv);
        GT_GET_FUNCTION(sglVertexAttribI4usv);
        GT_GET_FUNCTION(sglGetUniformuiv);
        GT_GET_FUNCTION(sglBindFragDataLocation);
        GT_GET_FUNCTION(sglGetFragDataLocation);
        GT_GET_FUNCTION(sglUniform1ui);
        GT_GET_FUNCTION(sglUniform2ui);
        GT_GET_FUNCTION(sglUniform3ui);
        GT_GET_FUNCTION(sglUniform4ui);
        GT_GET_FUNCTION(sglUniform1uiv);
        GT_GET_FUNCTION(sglUniform2uiv);
        GT_GET_FUNCTION(sglUniform3uiv);
        GT_GET_FUNCTION(sglUniform4uiv);
        GT_GET_FUNCTION(sglTexParameterIiv);
        GT_GET_FUNCTION(sglTexParameterIuiv);
        GT_GET_FUNCTION(sglGetTexParameterIiv);
        GT_GET_FUNCTION(sglGetTexParameterIuiv);
        GT_GET_FUNCTION(sglClearBufferiv);
        GT_GET_FUNCTION(sglClearBufferuiv);
        GT_GET_FUNCTION(sglClearBufferfv);
        GT_GET_FUNCTION(sglClearBufferfi);
        GT_GET_FUNCTION(sglGetStringi);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// OpenGL 3.1
//----------------------------------------------------------------------------
int GTExistsOpenGL31 = 0;

typedef void (STDCALL* PsglDrawArraysInstanced) (GLenum, GLint, GLsizei,
    GLsizei);
typedef void (STDCALL* PsglDrawElementsInstanced) (GLenum, GLsizei, GLenum,
    const GLvoid*, GLsizei);
typedef void (STDCALL* PsglTexBuffer) (GLenum, GLenum, GLuint);
typedef void (STDCALL* PsglPrimitiveRestartIndex) (GLuint);

static PsglDrawArraysInstanced sglDrawArraysInstanced = 0;
static PsglDrawElementsInstanced sglDrawElementsInstanced = 0;
static PsglTexBuffer sglTexBuffer = 0;
static PsglPrimitiveRestartIndex sglPrimitiveRestartIndex = 0;
//----------------------------------------------------------------------------
void GTglDrawArraysInstanced (GLenum mode, GLint first, GLsizei count,
    GLsizei primcount)
{
    GT_BODY_A4(sglDrawArraysInstanced,mode,first,count,primcount);
}
//----------------------------------------------------------------------------
void GTglDrawElementsInstanced (GLenum mode, GLsizei count, GLenum type,
    const GLvoid* indices, GLsizei primcount)
{
    GT_BODY_A5(sglDrawElementsInstanced,mode,count,type,indices,primcount);
}
//----------------------------------------------------------------------------
void GTglTexBuffer (GLenum target, GLenum internalformat, GLuint buffer)
{
    GT_BODY_A3(sglTexBuffer,target,internalformat,buffer);
}
//----------------------------------------------------------------------------
void GTglPrimitiveRestartIndex (GLuint index)
{
    GT_BODY_A1(sglPrimitiveRestartIndex,index);
}
//----------------------------------------------------------------------------
void GTInitOpenGL31 ()
{
    if (GTGetVersion() >= GT_VERSION_3_1)
    {
        GTExistsOpenGL31 = 1;

        GT_GET_FUNCTION(sglDrawArraysInstanced);
        GT_GET_FUNCTION(sglDrawElementsInstanced);
        GT_GET_FUNCTION(sglTexBuffer);
        GT_GET_FUNCTION(sglPrimitiveRestartIndex);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// OpenGL 3.2
//----------------------------------------------------------------------------
int GTExistsOpenGL32 = 0;

typedef void (STDCALL* PsglGetInteger64i_v) (GLenum, GLuint, GLint64*);
typedef void (STDCALL* PsglGetBufferParameteri64v) (GLenum, GLenum, GLint64*);
typedef void (STDCALL* PsglProgramParameteri) (GLuint, GLenum, GLint);
typedef void (STDCALL* PsglFramebufferTexture) (GLenum, GLenum, GLuint,
    GLint);

static PsglGetInteger64i_v sglGetInteger64i_v = 0;
static PsglGetBufferParameteri64v sglGetBufferParameteri64v = 0;
static PsglProgramParameteri sglProgramParameteri = 0;
static PsglFramebufferTexture sglFramebufferTexture = 0;
//----------------------------------------------------------------------------
void GTglGetInteger64i_v (GLenum target, GLuint index, GLint64* data)
{
    GT_BODY_A3(sglGetInteger64i_v,target,index,data);
}
//----------------------------------------------------------------------------
void GTglGetBufferParameteri64v (GLenum target, GLenum pname, GLint64* params)
{
    GT_BODY_A3(sglGetBufferParameteri64v,target,pname,params);
}
//----------------------------------------------------------------------------
void GTglProgramParameteri (GLuint program, GLenum pname, GLint value)
{
    GT_BODY_A3(sglProgramParameteri,program,pname,value);
}
//----------------------------------------------------------------------------
void GTglFramebufferTexture (GLenum target, GLenum attachment, GLuint texture,
    GLint level)
{
    GT_BODY_A4(sglFramebufferTexture,target,attachment,texture,level);
}
//----------------------------------------------------------------------------
void GTInitOpenGL32 ()
{
    if (GTGetVersion() >= GT_VERSION_3_2)
    {
        GTExistsOpenGL32 = 1;

        GT_GET_FUNCTION(sglGetInteger64i_v);
        GT_GET_FUNCTION(sglGetBufferParameteri64v);
        GT_GET_FUNCTION(sglProgramParameteri);
        GT_GET_FUNCTION(sglFramebufferTexture);
    }
}
//----------------------------------------------------------------------------
void GTInitOpenGL33 ()
{
    if (GTGetVersion() >= GT_VERSION_3_3)
    {
        GTExistsOpenGL33 = 1;
    }
}
//----------------------------------------------------------------------------
void GTInitOpenGL40 ()
{
    if (GTGetVersion() >= GT_VERSION_4_0)
    {
        GTExistsOpenGL40 = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// OpenGL 3.3
//----------------------------------------------------------------------------
int GTExistsOpenGL33 = 0;
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// OpenGL 4.0
//----------------------------------------------------------------------------
int GTExistsOpenGL40 = 0;
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// **** ARB Extensions *****
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 01) GL_ARB_multitexture
//----------------------------------------------------------------------------
int GTExistsGlArbMultitexture = 0;

static PsglActiveTexture sglActiveTextureARB = 0;
static PsglClientActiveTexture sglClientActiveTextureARB = 0;
static PsglMultiTexCoord1d sglMultiTexCoord1dARB = 0;
static PsglMultiTexCoord1dv sglMultiTexCoord1dvARB = 0;
static PsglMultiTexCoord1f sglMultiTexCoord1fARB = 0;
static PsglMultiTexCoord1fv sglMultiTexCoord1fvARB = 0;
static PsglMultiTexCoord1i sglMultiTexCoord1iARB = 0;
static PsglMultiTexCoord1iv sglMultiTexCoord1ivARB = 0;
static PsglMultiTexCoord1s sglMultiTexCoord1sARB = 0;
static PsglMultiTexCoord1sv sglMultiTexCoord1svARB = 0;
static PsglMultiTexCoord2d sglMultiTexCoord2dARB = 0;
static PsglMultiTexCoord2dv sglMultiTexCoord2dvARB = 0;
static PsglMultiTexCoord2f sglMultiTexCoord2fARB = 0;
static PsglMultiTexCoord2fv sglMultiTexCoord2fvARB = 0;
static PsglMultiTexCoord2i sglMultiTexCoord2iARB = 0;
static PsglMultiTexCoord2iv sglMultiTexCoord2ivARB = 0;
static PsglMultiTexCoord2s sglMultiTexCoord2sARB = 0;
static PsglMultiTexCoord2sv sglMultiTexCoord2svARB = 0;
static PsglMultiTexCoord3d sglMultiTexCoord3dARB = 0;
static PsglMultiTexCoord3dv sglMultiTexCoord3dvARB = 0;
static PsglMultiTexCoord3f sglMultiTexCoord3fARB = 0;
static PsglMultiTexCoord3fv sglMultiTexCoord3fvARB = 0;
static PsglMultiTexCoord3i sglMultiTexCoord3iARB = 0;
static PsglMultiTexCoord3iv sglMultiTexCoord3ivARB = 0;
static PsglMultiTexCoord3s sglMultiTexCoord3sARB = 0;
static PsglMultiTexCoord3sv sglMultiTexCoord3svARB = 0;
static PsglMultiTexCoord4d sglMultiTexCoord4dARB = 0;
static PsglMultiTexCoord4dv sglMultiTexCoord4dvARB = 0;
static PsglMultiTexCoord4f sglMultiTexCoord4fARB = 0;
static PsglMultiTexCoord4fv sglMultiTexCoord4fvARB = 0;
static PsglMultiTexCoord4i sglMultiTexCoord4iARB = 0;
static PsglMultiTexCoord4iv sglMultiTexCoord4ivARB = 0;
static PsglMultiTexCoord4s sglMultiTexCoord4sARB = 0;
static PsglMultiTexCoord4sv sglMultiTexCoord4svARB = 0;
//----------------------------------------------------------------------------
void GTglActiveTextureARB (GLenum eTexture)
{
    GT_BODY_A1(sglActiveTextureARB,eTexture);
}
//----------------------------------------------------------------------------
void GTglClientActiveTextureARB (GLenum eTexture)
{
    GT_BODY_A1(sglClientActiveTextureARB,eTexture);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord1dARB (GLenum eTarget, GLdouble dS)
{
    GT_BODY_A2(sglMultiTexCoord1dARB,eTarget,dS);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord1dvARB (GLenum eTarget, const GLdouble* pdS)
{
    GT_BODY_A2(sglMultiTexCoord1dvARB,eTarget,pdS);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord1fARB (GLenum eTarget, GLfloat fS)
{
    GT_BODY_A2(sglMultiTexCoord1fARB,eTarget,fS);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord1fvARB (GLenum eTarget, const GLfloat* pfS)
{
    GT_BODY_A2(sglMultiTexCoord1fvARB,eTarget,pfS);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord1iARB (GLenum eTarget, GLint iS)
{
    GT_BODY_A2(sglMultiTexCoord1iARB,eTarget,iS);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord1ivARB (GLenum eTarget, const GLint* piS)
{
    GT_BODY_A2(sglMultiTexCoord1ivARB,eTarget,piS);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord1sARB (GLenum eTarget, GLshort sS)
{
    GT_BODY_A2(sglMultiTexCoord1sARB,eTarget,sS);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord1svARB (GLenum eTarget, const GLshort* psS)
{
    GT_BODY_A2(sglMultiTexCoord1svARB,eTarget,psS);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord2dARB (GLenum eTarget, GLdouble dS, GLdouble dT)
{
    GT_BODY_A3(sglMultiTexCoord2dARB,eTarget,dS,dT);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord2dvARB (GLenum eTarget, const GLdouble* adST)
{
    GT_BODY_A2(sglMultiTexCoord2dvARB,eTarget,adST);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord2fARB (GLenum eTarget, GLfloat fS, GLfloat fT)
{
    GT_BODY_A3(sglMultiTexCoord2fARB,eTarget,fS,fT);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord2fvARB (GLenum eTarget, const GLfloat* afST)
{
    GT_BODY_A2(sglMultiTexCoord2fvARB,eTarget,afST);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord2iARB (GLenum eTarget, GLint iS, GLint iT)
{
    GT_BODY_A3(sglMultiTexCoord2iARB,eTarget,iS,iT);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord2ivARB (GLenum eTarget, const GLint* aiST)
{
    GT_BODY_A2(sglMultiTexCoord2ivARB,eTarget,aiST);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord2sARB (GLenum eTarget, GLshort sS, GLshort sT)
{
    GT_BODY_A3(sglMultiTexCoord2sARB,eTarget,sS,sT);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord2svARB (GLenum eTarget, const GLshort* asST)
{
    GT_BODY_A2(sglMultiTexCoord2svARB,eTarget,asST);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord3dARB (GLenum eTarget, GLdouble dS, GLdouble dT,
    GLdouble dR)
{
    GT_BODY_A4(sglMultiTexCoord3dARB,eTarget,dS,dT,dR);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord3dvARB (GLenum eTarget, const GLdouble* adSTR)
{
    GT_BODY_A2(sglMultiTexCoord3dvARB,eTarget,adSTR);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord3fARB (GLenum eTarget, GLfloat fS, GLfloat fT,
    GLfloat fR)
{
    GT_BODY_A4(sglMultiTexCoord3fARB,eTarget,fS,fT,fR);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord3fvARB (GLenum eTarget, const GLfloat* afSTR)
{
    GT_BODY_A2(sglMultiTexCoord3fvARB,eTarget,afSTR);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord3iARB (GLenum eTarget, GLint iS, GLint iT, GLint iR)
{
    GT_BODY_A4(sglMultiTexCoord3iARB,eTarget,iS,iT,iR);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord3ivARB (GLenum eTarget, const GLint* aiSTR)
{
    GT_BODY_A2(sglMultiTexCoord3ivARB,eTarget,aiSTR);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord3sARB (GLenum eTarget, GLshort sS, GLshort sT,
    GLshort sR)
{
    GT_BODY_A4(sglMultiTexCoord3sARB,eTarget,sS,sT,sR);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord3svARB (GLenum eTarget, const GLshort* asSTR)
{
    GT_BODY_A2(sglMultiTexCoord3svARB,eTarget,asSTR);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord4dARB (GLenum eTarget, GLdouble dS, GLdouble dT,
    GLdouble dR, GLdouble dQ)
{
    GT_BODY_A5(sglMultiTexCoord4dARB,eTarget,dS,dT,dR,dQ);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord4dvARB (GLenum eTarget, const GLdouble* adSTRQ)
{
    GT_BODY_A2(sglMultiTexCoord4dvARB,eTarget,adSTRQ);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord4fARB (GLenum eTarget, GLfloat fS, GLfloat fT,
    GLfloat fR, GLfloat fQ)
{
    GT_BODY_A5(sglMultiTexCoord4fARB,eTarget,fS,fT,fR,fQ);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord4fvARB (GLenum eTarget, const GLfloat* afSTRQ)
{
    GT_BODY_A2(sglMultiTexCoord4fvARB,eTarget,afSTRQ);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord4iARB (GLenum eTarget, GLint iS, GLint iT, GLint iR,
    GLint iQ)
{
    GT_BODY_A5(sglMultiTexCoord4iARB,eTarget,iS,iT,iR,iQ);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord4ivARB (GLenum eTarget, const GLint* aiSTRQ)
{
    GT_BODY_A2(sglMultiTexCoord4ivARB,eTarget,aiSTRQ);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord4sARB (GLenum eTarget, GLshort sS, GLshort sT,
    GLshort sR, GLshort sQ)
{
    GT_BODY_A5(sglMultiTexCoord4sARB,eTarget,sS,sT,sR,sQ);
}
//----------------------------------------------------------------------------
void GTglMultiTexCoord4svARB (GLenum eTarget, const GLshort* asSTRQ)
{
    GT_BODY_A2(sglMultiTexCoord4svARB,eTarget,asSTRQ);
}
//----------------------------------------------------------------------------
void GTInitGlArbMultitexture ()
{
    if (GTSupportsExtension("GL_ARB_multitexture"))
    {
        GTExistsGlArbMultitexture = 1;
        GT_GET_FUNCTION_C(sglActiveTexture,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglClientActiveTexture,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglMultiTexCoord1d,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglMultiTexCoord1dv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglMultiTexCoord1f,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglMultiTexCoord1fv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglMultiTexCoord1i,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglMultiTexCoord1iv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglMultiTexCoord1s,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglMultiTexCoord1sv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglMultiTexCoord2d,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglMultiTexCoord2dv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglMultiTexCoord2f,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglMultiTexCoord2fv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglMultiTexCoord2i,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglMultiTexCoord2iv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglMultiTexCoord2s,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglMultiTexCoord2sv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglMultiTexCoord3d,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglMultiTexCoord3dv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglMultiTexCoord3f,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglMultiTexCoord3fv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglMultiTexCoord3i,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglMultiTexCoord3iv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglMultiTexCoord3s,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglMultiTexCoord3sv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglMultiTexCoord4d,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglMultiTexCoord4dv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglMultiTexCoord4f,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglMultiTexCoord4fv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglMultiTexCoord4i,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglMultiTexCoord4iv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglMultiTexCoord4s,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglMultiTexCoord4sv,ARB,GT_ARB_OVERRIDE);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 03) GL_ARB_transpose_matrix
//----------------------------------------------------------------------------
int GTExistsGlArbTransposeMatrix = 0;

static PsglLoadTransposeMatrixf sglLoadTransposeMatrixfARB = 0;
static PsglLoadTransposeMatrixd sglLoadTransposeMatrixdARB = 0;
static PsglMultTransposeMatrixf sglMultTransposeMatrixfARB = 0;
static PsglMultTransposeMatrixd sglMultTransposeMatrixdARB = 0;
//----------------------------------------------------------------------------
void GTglLoadTransposeMatrixdARB (const GLdouble* adMatrix)
{
    GT_BODY_A1(sglLoadTransposeMatrixdARB,adMatrix);
}
//----------------------------------------------------------------------------
void GTglLoadTransposeMatrixfARB (const GLfloat* afMatrix)
{
    GT_BODY_A1(sglLoadTransposeMatrixfARB,afMatrix);
}
//----------------------------------------------------------------------------
void GTglMultTransposeMatrixdARB (const GLdouble* adMatrix)
{
    GT_BODY_A1(sglMultTransposeMatrixdARB,adMatrix);
}
//----------------------------------------------------------------------------
void GTglMultTransposeMatrixfARB (const GLfloat* afMatrix)
{
    GT_BODY_A1(sglMultTransposeMatrixfARB,afMatrix);
}
//----------------------------------------------------------------------------
void GTInitGlArbTransposeMatrix ()
{
    if (GTSupportsExtension("GL_ARB_transpose_matrix"))
    {
        GTExistsGlArbTransposeMatrix = 1;
        GT_GET_FUNCTION_C(sglLoadTransposeMatrixf,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglLoadTransposeMatrixd,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglMultTransposeMatrixf,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglMultTransposeMatrixd,ARB,GT_ARB_OVERRIDE);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 05) GL_ARB_multisample
//----------------------------------------------------------------------------
int GTExistsGlArbMultisample = 0;

static PsglSampleCoverage sglSampleCoverageARB = 0;
//----------------------------------------------------------------------------
void GTglSampleCoverageARB (GLclampf fValue, GLboolean ucInvert)
{
    GT_BODY_A2(sglSampleCoverageARB,fValue,ucInvert);
}
//----------------------------------------------------------------------------
void GTInitGlArbMultisample ()
{
    if (GTSupportsExtension("GL_ARB_multisample"))
    {
        GTExistsGlArbMultisample = 1;
        GT_GET_FUNCTION_C(sglSampleCoverage,ARB,GT_ARB_OVERRIDE);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 06) GL_ARB_texture_env_add
//----------------------------------------------------------------------------
int GTExistsGlArbTextureEnvAdd = 0;
//----------------------------------------------------------------------------
void GTInitGlArbTextureEnvAdd ()
{
    if (GTSupportsExtension("GL_ARB_texture_env_add"))
    {
        GTExistsGlArbTextureEnvAdd = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 07) GL_ARB_texture_cube_map
//----------------------------------------------------------------------------
int GTExistsGlArbTextureCubeMap = 0;
//----------------------------------------------------------------------------
void GTInitGlArbTextureCubeMap ()
{
    if (GTSupportsExtension("GL_ARB_texture_cube_map"))
    {
        GTExistsGlArbTextureCubeMap = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 12) GL_ARB_texture_compression
//----------------------------------------------------------------------------
int GTExistsGlArbTextureCompression = 0;

static PsglCompressedTexImage3D sglCompressedTexImage3DARB = 0;
static PsglCompressedTexImage2D sglCompressedTexImage2DARB = 0;
static PsglCompressedTexImage1D sglCompressedTexImage1DARB = 0;
static PsglCompressedTexSubImage3D sglCompressedTexSubImage3DARB = 0;
static PsglCompressedTexSubImage2D sglCompressedTexSubImage2DARB = 0;
static PsglCompressedTexSubImage1D sglCompressedTexSubImage1DARB = 0;
static PsglGetCompressedTexImage sglGetCompressedTexImageARB = 0;
//----------------------------------------------------------------------------
void GTglCompressedTexImage1DARB (GLenum eTarget, GLint iLevel,
    GLenum eInternalFormat, GLsizei iWidth, GLint iBorder, GLsizei iImageSize,
    const GLvoid* pvData)
{
    GT_BODY_A7(sglCompressedTexImage1DARB,eTarget,iLevel,eInternalFormat,
        iWidth,iBorder,iImageSize,pvData);
}
//----------------------------------------------------------------------------
void GTglCompressedTexImage2DARB (GLenum eTarget, GLint iLevel,
    GLenum eInternalFormat, GLsizei iWidth, GLsizei iHeight, GLint iBorder,
    GLsizei iImageSize, const GLvoid* pvData)
{
    GT_BODY_A8(sglCompressedTexImage2DARB,eTarget,iLevel,eInternalFormat,
        iWidth,iHeight,iBorder,iImageSize,pvData);
}
//----------------------------------------------------------------------------
void GTglCompressedTexImage3DARB (GLenum eTarget, GLint iLevel,
    GLenum eInternalFormat, GLsizei iWidth, GLsizei iHeight, GLsizei iDepth,
    GLint iBorder, GLsizei iImageSize, const GLvoid* pvData)
{
    GT_BODY_A9(sglCompressedTexImage3DARB,eTarget,iLevel,eInternalFormat,
        iWidth,iHeight,iDepth,iBorder,iImageSize,pvData);
}
//----------------------------------------------------------------------------
void GTglCompressedTexSubImage1DARB (GLenum eTarget, GLint iLevel,
    GLint iXOffset, GLsizei iWidth, GLenum eFormat, GLsizei iImageSize,
    const GLvoid* pvData)
{
    GT_BODY_A7(sglCompressedTexSubImage1DARB,eTarget,iLevel,iXOffset,iWidth,
        eFormat,iImageSize,pvData);
}
//----------------------------------------------------------------------------
void GTglCompressedTexSubImage2DARB (GLenum eTarget, GLint iLevel,
    GLint iXOffset, GLint iYOffset, GLsizei iWidth, GLsizei iHeight,
    GLenum eFormat, GLsizei iImageSize, const GLvoid* pvData)
{
    GT_BODY_A9(sglCompressedTexSubImage2DARB,eTarget,iLevel,iXOffset,iYOffset,
        iWidth,iHeight,eFormat,iImageSize,pvData);
}
//----------------------------------------------------------------------------
void GTglCompressedTexSubImage3DARB (GLenum eTarget, GLint iLevel,
    GLint iXOffset, GLint iYOffset, GLint iZOffset, GLsizei iWidth,
    GLsizei iHeight, GLsizei iDepth, GLenum eFormat, GLsizei iImageSize,
    const GLvoid* pvData)
{
    GT_BODY_A11(sglCompressedTexSubImage3DARB,eTarget,iLevel,iXOffset,
        iYOffset,iZOffset,iWidth,iHeight,iDepth,eFormat,iImageSize,pvData);
}
//----------------------------------------------------------------------------
void GTglGetCompressedTexImageARB (GLenum eTarget, GLint iLevel,
    GLvoid* pvData)
{
    GT_BODY_A3(sglGetCompressedTexImageARB,eTarget,iLevel,pvData);
}
//----------------------------------------------------------------------------
void GTInitGlArbTextureCompression ()
{
    if (GTSupportsExtension("GL_ARB_texture_compression"))
    {
        GTExistsGlArbTextureCompression = 1;
        GT_GET_FUNCTION_C(sglCompressedTexImage3D,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglCompressedTexImage2D,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglCompressedTexImage1D,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglCompressedTexSubImage3D,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglCompressedTexSubImage2D,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglCompressedTexSubImage1D,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglGetCompressedTexImage,ARB,GT_ARB_OVERRIDE);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 13) GL_ARB_texture_border_clamp
//----------------------------------------------------------------------------
int GTExistsGlArbTextureBorderClamp = 0;
//----------------------------------------------------------------------------
void GTInitGlArbTextureBorderClamp ()
{
    if (GTSupportsExtension("GL_ARB_texture_border_clamp"))
    {
        GTExistsGlArbTextureBorderClamp = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 14) GL_ARB_point_parameters
//----------------------------------------------------------------------------
int GTExistsGlArbPointParameters = 0;

static PsglPointParameterf sglPointParameterfARB = 0;
static PsglPointParameterfv sglPointParameterfvARB = 0;
//----------------------------------------------------------------------------
void GTglPointParameterfARB (GLenum ePName, GLfloat fParam)
{
    GT_BODY_A2(sglPointParameterfARB,ePName,fParam);
}
//----------------------------------------------------------------------------
void GTglPointParameterfvARB (GLenum ePName, const GLfloat* afParam)
{
    GT_BODY_A2(sglPointParameterfvARB,ePName,afParam);
}
//----------------------------------------------------------------------------
void GTInitGlArbPointParameters ()
{
    if (GTSupportsExtension("GL_ARB_point_parameters"))
    {
        GTExistsGlArbPointParameters = 1;
        GT_GET_FUNCTION_C(sglPointParameterf,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglPointParameterfv,ARB,GT_ARB_OVERRIDE);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 15) GL_ARB_vertex_blend
//----------------------------------------------------------------------------
int GTExistsGlArbVertexBlend = 0;

typedef void (STDCALL* PsglWeightbvARB) (GLint, const GLbyte*);
typedef void (STDCALL* PsglWeightsvARB) (GLint, const GLshort*);
typedef void (STDCALL* PsglWeightivARB) (GLint, const GLint*);
typedef void (STDCALL* PsglWeightfvARB) (GLint, const GLfloat*);
typedef void (STDCALL* PsglWeightdvARB) (GLint, const GLdouble*);
typedef void (STDCALL* PsglWeightubvARB) (GLint, const GLubyte*);
typedef void (STDCALL* PsglWeightusvARB) (GLint, const GLushort*);
typedef void (STDCALL* PsglWeightuivARB) (GLint, const GLuint*);
typedef void (STDCALL* PsglWeightPointerARB) (GLint, GLenum, GLsizei,
    const GLvoid*);
typedef void (STDCALL* PsglVertexBlendARB) (GLint);

static PsglWeightbvARB sglWeightbvARB = 0;
static PsglWeightsvARB sglWeightsvARB = 0;
static PsglWeightivARB sglWeightivARB = 0;
static PsglWeightfvARB sglWeightfvARB = 0;
static PsglWeightdvARB sglWeightdvARB = 0;
static PsglWeightubvARB sglWeightubvARB = 0;
static PsglWeightusvARB sglWeightusvARB = 0;
static PsglWeightuivARB sglWeightuivARB = 0;
static PsglWeightPointerARB sglWeightPointerARB = 0;
static PsglVertexBlendARB sglVertexBlendARB = 0;
//----------------------------------------------------------------------------
void GTglWeightbvARB (GLint iSize, const GLbyte* acWeight)
{
    GT_BODY_A2(sglWeightbvARB,iSize,acWeight);
}
//----------------------------------------------------------------------------
void GTglWeightsvARB (GLint iSize, const GLshort* asWeight)
{
    GT_BODY_A2(sglWeightsvARB,iSize,asWeight);
}
//----------------------------------------------------------------------------
void GTglWeightivARB (GLint iSize, const GLint* aiWeight)
{
    GT_BODY_A2(sglWeightivARB,iSize,aiWeight);
}
//----------------------------------------------------------------------------
void GTglWeightfvARB (GLint iSize, const GLfloat* afWeight)
{
    GT_BODY_A2(sglWeightfvARB,iSize,afWeight);
}
//----------------------------------------------------------------------------
void GTglWeightdvARB (GLint iSize, const GLdouble* adWeight)
{
    GT_BODY_A2(sglWeightdvARB,iSize,adWeight);
}
//----------------------------------------------------------------------------
void GTglWeightubvARB (GLint iSize, const GLubyte* aucWeight)
{
    GT_BODY_A2(sglWeightubvARB,iSize,aucWeight);
}
//----------------------------------------------------------------------------
void GTglWeightusvARB (GLint iSize, const GLushort* ausWeight)
{
    GT_BODY_A2(sglWeightusvARB,iSize,ausWeight);
}
//----------------------------------------------------------------------------
void GTglWeightuivARB (GLint iSize, const GLuint* auiWeight)
{
    GT_BODY_A2(sglWeightuivARB,iSize,auiWeight);
}
//----------------------------------------------------------------------------
void GTglWeightPointerARB (GLint iSize, GLenum eType, GLsizei iStride,
    const GLvoid* pvPointer)
{
    GT_BODY_A4(sglWeightPointerARB,iSize,eType,iStride,pvPointer);
}
//----------------------------------------------------------------------------
void GTglVertexBlendARB (GLint iCount)
{
    GT_BODY_A1(sglVertexBlendARB,iCount);
}
//----------------------------------------------------------------------------
void GTInitGlArbVertexBlend ()
{
    if (GTSupportsExtension("GL_ARB_vertex_blend"))
    {
        GTExistsGlArbVertexBlend = 1;
        GT_GET_FUNCTION(sglWeightbvARB);
        GT_GET_FUNCTION(sglWeightsvARB);
        GT_GET_FUNCTION(sglWeightivARB);
        GT_GET_FUNCTION(sglWeightfvARB);
        GT_GET_FUNCTION(sglWeightdvARB);
        GT_GET_FUNCTION(sglWeightubvARB);
        GT_GET_FUNCTION(sglWeightusvARB);
        GT_GET_FUNCTION(sglWeightuivARB);
        GT_GET_FUNCTION(sglWeightPointerARB);
        GT_GET_FUNCTION(sglVertexBlendARB);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 16) GL_ARB_matrix_palette
//----------------------------------------------------------------------------
int GTExistsGlArbMatrixPalette = 0;

typedef void (STDCALL* PsglCurrentPaletteMatrixARB) (GLint);
typedef void (STDCALL* PsglMatrixIndexubvARB) (GLint, const GLubyte*);
typedef void (STDCALL* PsglMatrixIndexusvARB) (GLint, const GLushort*);
typedef void (STDCALL* PsglMatrixIndexuivARB) (GLint, const GLuint*);
typedef void (STDCALL* PsglMatrixIndexPointerARB) (GLint, GLenum, GLsizei,
    const GLvoid*);

static PsglCurrentPaletteMatrixARB sglCurrentPaletteMatrixARB = 0;
static PsglMatrixIndexubvARB sglMatrixIndexubvARB = 0;
static PsglMatrixIndexusvARB sglMatrixIndexusvARB = 0;
static PsglMatrixIndexuivARB sglMatrixIndexuivARB = 0;
static PsglMatrixIndexPointerARB sglMatrixIndexPointerARB = 0;
//----------------------------------------------------------------------------
void GTglCurrentPaletteMatrixARB (GLint iIndex)
{
    GT_BODY_A1(sglCurrentPaletteMatrixARB,iIndex);
}
//----------------------------------------------------------------------------
void GTglMatrixIndexubvARB (GLint iSize, const GLubyte* aucIndex)
{
    GT_BODY_A2(sglMatrixIndexubvARB,iSize,aucIndex);
}
//----------------------------------------------------------------------------
void GTglMatrixIndexusvARB (GLint iSize, const GLushort* ausIndex)
{
    GT_BODY_A2(sglMatrixIndexusvARB,iSize,ausIndex);
}
//----------------------------------------------------------------------------
void GTglMatrixIndexuivARB (GLint iSize, const GLuint* auiIndex)
{
    GT_BODY_A2(sglMatrixIndexuivARB,iSize,auiIndex);
}
//----------------------------------------------------------------------------
void GTglMatrixIndexPointerARB (GLint iSize, GLenum eType, GLsizei iStride,
    const GLvoid* pvPointer)
{
    GT_BODY_A4(sglMatrixIndexPointerARB,iSize,eType,iStride,pvPointer);
}
//----------------------------------------------------------------------------
void GTInitGlArbMatrixPalette ()
{
    if (GTSupportsExtension("GL_ARB_matrix_palette"))
    {
        GTExistsGlArbMatrixPalette = 1;
        GT_GET_FUNCTION(sglCurrentPaletteMatrixARB);
        GT_GET_FUNCTION(sglMatrixIndexubvARB);
        GT_GET_FUNCTION(sglMatrixIndexusvARB);
        GT_GET_FUNCTION(sglMatrixIndexuivARB);
        GT_GET_FUNCTION(sglMatrixIndexPointerARB);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 17) GL_ARB_texture_env_combine
//----------------------------------------------------------------------------
int GTExistsGlArbTextureEnvCombine = 0;
//----------------------------------------------------------------------------
void GTInitGlArbTextureEnvCombine ()
{
    if (GTSupportsExtension("GL_ARB_texture_env_combine"))
    {
        GTExistsGlArbTextureEnvCombine = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 18) GL_ARB_texture_env_crossbar
//----------------------------------------------------------------------------
int GTExistsGlArbTextureEnvCrossbar = 0;
//----------------------------------------------------------------------------
void GTInitGlArbTextureEnvCrossbar ()
{
    if (GTSupportsExtension("GL_ARB_texture_env_crossbar"))
    {
        GTExistsGlArbTextureEnvCrossbar = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 19) GL_ARB_texture_env_dot3
//----------------------------------------------------------------------------
int GTExistsGlArbTextureEnvDot3 = 0;
//----------------------------------------------------------------------------
void GTInitGlArbTextureEnvDot3 ()
{
    if (GTSupportsExtension("GL_ARB_texture_env_dot3"))
    {
        GTExistsGlArbTextureEnvDot3 = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 21) GL_ARB_texture_mirrored_repeat
//----------------------------------------------------------------------------
int GTExistsGlArbTextureMirroredRepeat = 0;
//----------------------------------------------------------------------------
void GTInitGlArbTextureMirroredRepeat ()
{
    if (GTSupportsExtension("GL_ARB_texture_mirrored_repeat"))
    {
        GTExistsGlArbTextureMirroredRepeat = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 22) GL_ARB_depth_texture
//----------------------------------------------------------------------------
int GTExistsGlArbDepthTexture = 0;
//----------------------------------------------------------------------------
void GTInitGlArbDepthTexture ()
{
    if (GTSupportsExtension("GL_ARB_depth_texture"))
    {
        GTExistsGlArbDepthTexture = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 23) GL_ARB_shadow
//----------------------------------------------------------------------------
int GTExistsGlArbShadow = 0;
//----------------------------------------------------------------------------
void GTInitGlArbShadow ()
{
    if (GTSupportsExtension("GL_ARB_shadow"))
    {
        GTExistsGlArbShadow = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 24) GL_ARB_shadow_ambient
//----------------------------------------------------------------------------
int GTExistsGlArbShadowAmbient = 0;
//----------------------------------------------------------------------------
void GTInitGlArbShadowAmbient ()
{
    if (GTSupportsExtension("GL_ARB_shadow_ambient"))
    {
        GTExistsGlArbShadowAmbient = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 25) GL_ARB_window_pos
//----------------------------------------------------------------------------
int GTExistsGlArbWindowPos = 0;

static PsglWindowPos2d sglWindowPos2dARB = 0;
static PsglWindowPos2dv sglWindowPos2dvARB = 0;
static PsglWindowPos2f sglWindowPos2fARB = 0;
static PsglWindowPos2fv sglWindowPos2fvARB = 0;
static PsglWindowPos2i sglWindowPos2iARB = 0;
static PsglWindowPos2iv sglWindowPos2ivARB = 0;
static PsglWindowPos2s sglWindowPos2sARB = 0;
static PsglWindowPos2sv sglWindowPos2svARB = 0;
static PsglWindowPos3d sglWindowPos3dARB = 0;
static PsglWindowPos3dv sglWindowPos3dvARB = 0;
static PsglWindowPos3f sglWindowPos3fARB = 0;
static PsglWindowPos3fv sglWindowPos3fvARB = 0;
static PsglWindowPos3i sglWindowPos3iARB = 0;
static PsglWindowPos3iv sglWindowPos3ivARB = 0;
static PsglWindowPos3s sglWindowPos3sARB = 0;
static PsglWindowPos3sv sglWindowPos3svARB = 0;
//----------------------------------------------------------------------------
void GTglWindowPos2dARB (GLdouble dX, GLdouble dY)
{
    GT_BODY_A2(sglWindowPos2dARB,dX,dY);
}
//----------------------------------------------------------------------------
void GTglWindowPos2dvARB (const GLdouble* adXY)
{
    GT_BODY_A1(sglWindowPos2dvARB,adXY);
}
//----------------------------------------------------------------------------
void GTglWindowPos2fARB (GLfloat fX, GLfloat fY)
{
    GT_BODY_A2(sglWindowPos2fARB,fX,fY);
}
//----------------------------------------------------------------------------
void GTglWindowPos2fvARB (const GLfloat* afXY)
{
    GT_BODY_A1(sglWindowPos2fvARB,afXY);
}
//----------------------------------------------------------------------------
void GTglWindowPos2iARB (GLint iX, GLint iY)
{
    GT_BODY_A2(sglWindowPos2iARB,iX,iY);
}
//----------------------------------------------------------------------------
void GTglWindowPos2ivARB (const GLint* aiXY)
{
    GT_BODY_A1(sglWindowPos2ivARB,aiXY);
}
//----------------------------------------------------------------------------
void GTglWindowPos2sARB (GLshort sX, GLshort sY)
{
    GT_BODY_A2(sglWindowPos2sARB,sX,sY);
}
//----------------------------------------------------------------------------
void GTglWindowPos2svARB (const GLshort* asXY)
{
    GT_BODY_A1(sglWindowPos2svARB,asXY);
}
//----------------------------------------------------------------------------
void GTglWindowPos3dARB (GLdouble dX, GLdouble dY, GLdouble dZ)
{
    GT_BODY_A3(sglWindowPos3dARB,dX,dY,dZ);
}
//----------------------------------------------------------------------------
void GTglWindowPos3dvARB (const GLdouble* adXYZ)
{
    GT_BODY_A1(sglWindowPos3dvARB,adXYZ);
}
//----------------------------------------------------------------------------
void GTglWindowPos3fARB (GLfloat fX, GLfloat fY, GLfloat fZ)
{
    GT_BODY_A3(sglWindowPos3fARB,fX,fY,fZ);
}
//----------------------------------------------------------------------------
void GTglWindowPos3fvARB (const GLfloat* afXYZ)
{
    GT_BODY_A1(sglWindowPos3fvARB,afXYZ);
}
//----------------------------------------------------------------------------
void GTglWindowPos3iARB (GLint iX, GLint iY, GLint iZ)
{
    GT_BODY_A3(sglWindowPos3iARB,iX,iY,iZ);
}
//----------------------------------------------------------------------------
void GTglWindowPos3ivARB (const GLint* aiXYZ)
{
    GT_BODY_A1(sglWindowPos3ivARB,aiXYZ);
}
//----------------------------------------------------------------------------
void GTglWindowPos3sARB (GLshort sX, GLshort sY, GLshort sZ)
{
    GT_BODY_A3(sglWindowPos3sARB,sX,sY,sZ);
}
//----------------------------------------------------------------------------
void GTglWindowPos3svARB (const GLshort* asXYZ)
{
    GT_BODY_A1(sglWindowPos3svARB,asXYZ);
}
//----------------------------------------------------------------------------
void GTInitGlArbWindowPos ()
{
    if (GTSupportsExtension("GL_ARB_window_pos"))
    {
        GTExistsGlArbWindowPos = 1;
        GT_GET_FUNCTION_C(sglWindowPos2d,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglWindowPos2dv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglWindowPos2f,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglWindowPos2fv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglWindowPos2i,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglWindowPos2iv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglWindowPos2s,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglWindowPos2sv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglWindowPos3d,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglWindowPos3dv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglWindowPos3f,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglWindowPos3fv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglWindowPos3i,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglWindowPos3iv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglWindowPos3s,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglWindowPos3sv,ARB,GT_ARB_OVERRIDE);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 26) GL_ARB_vertex_program
//----------------------------------------------------------------------------
int GTExistsGlArbVertexProgram = 0;

typedef void (STDCALL* PsglBindProgramARB) (GLenum, GLuint);
typedef void (STDCALL* PsglDeleteProgramsARB) (GLsizei, const GLuint*);
typedef void (STDCALL* PsglGenProgramsARB) (GLsizei, GLuint*);
typedef void (STDCALL* PsglGetProgramEnvParameterdvARB) (GLenum, GLuint,
    GLdouble*);
typedef void (STDCALL* PsglGetProgramEnvParameterfvARB) (GLenum, GLuint,
    GLfloat*);
typedef void (STDCALL* PsglGetProgramLocalParameterdvARB) (GLenum, GLuint,
    GLdouble*);
typedef void (STDCALL* PsglGetProgramLocalParameterfvARB) (GLenum, GLuint,
    GLfloat*);
typedef void (STDCALL* PsglGetProgramStringARB) (GLenum, GLenum, GLvoid*);
typedef void (STDCALL* PsglProgramEnvParameter4dARB) (GLenum, GLuint,
    GLdouble, GLdouble, GLdouble, GLdouble);
typedef void (STDCALL* PsglProgramEnvParameter4dvARB) (GLenum, GLuint,
    const GLdouble*);
typedef void (STDCALL* PsglProgramEnvParameter4fARB) (GLenum, GLuint,
    GLfloat, GLfloat, GLfloat, GLfloat);
typedef void (STDCALL* PsglProgramEnvParameter4fvARB) (GLenum, GLuint,
    const GLfloat*);
typedef void (STDCALL* PsglProgramLocalParameter4dARB) (GLenum, GLuint,
    GLdouble, GLdouble, GLdouble, GLdouble);
typedef void (STDCALL* PsglProgramLocalParameter4dvARB) (GLenum, GLuint,
    const GLdouble*);
typedef void (STDCALL* PsglProgramLocalParameter4fARB) (GLenum, GLuint,
    GLfloat, GLfloat, GLfloat, GLfloat);
typedef void (STDCALL* PsglProgramLocalParameter4fvARB) (GLenum, GLuint,
    const GLfloat*);
typedef void (STDCALL* PsglProgramStringARB) (GLenum, GLenum, GLsizei,
    const GLvoid*);

static PsglBindProgramARB sglBindProgramARB = 0;
static PsglDeleteProgramsARB sglDeleteProgramsARB = 0;
static PsglGenProgramsARB sglGenProgramsARB = 0;
static PsglGetProgramEnvParameterdvARB sglGetProgramEnvParameterdvARB = 0;
static PsglGetProgramEnvParameterfvARB sglGetProgramEnvParameterfvARB = 0;
static PsglGetProgramLocalParameterdvARB sglGetProgramLocalParameterdvARB = 0;
static PsglGetProgramLocalParameterfvARB sglGetProgramLocalParameterfvARB = 0;
static PsglGetProgramStringARB sglGetProgramStringARB = 0;
static PsglProgramEnvParameter4dARB sglProgramEnvParameter4dARB = 0;
static PsglProgramEnvParameter4dvARB sglProgramEnvParameter4dvARB = 0;
static PsglProgramEnvParameter4fARB sglProgramEnvParameter4fARB = 0;
static PsglProgramEnvParameter4fvARB sglProgramEnvParameter4fvARB = 0;
static PsglProgramLocalParameter4dARB sglProgramLocalParameter4dARB = 0;
static PsglProgramLocalParameter4dvARB sglProgramLocalParameter4dvARB = 0;
static PsglProgramLocalParameter4fARB sglProgramLocalParameter4fARB = 0;
static PsglProgramLocalParameter4fvARB sglProgramLocalParameter4fvARB = 0;
static PsglProgramStringARB sglProgramStringARB = 0;

static PsglDisableVertexAttribArray sglDisableVertexAttribArrayARB = 0;
static PsglEnableVertexAttribArray sglEnableVertexAttribArrayARB = 0;
static PsglGetProgramiv sglGetProgramivARB = 0;
static PsglGetVertexAttribdv sglGetVertexAttribdvARB = 0;
static PsglGetVertexAttribfv sglGetVertexAttribfvARB = 0;
static PsglGetVertexAttribiv sglGetVertexAttribivARB = 0;
static PsglGetVertexAttribPointerv sglGetVertexAttribPointervARB = 0;
static PsglIsProgram sglIsProgramARB = 0;
static PsglVertexAttrib1d sglVertexAttrib1dARB = 0;
static PsglVertexAttrib1dv sglVertexAttrib1dvARB = 0;
static PsglVertexAttrib1f sglVertexAttrib1fARB = 0;
static PsglVertexAttrib1fv sglVertexAttrib1fvARB = 0;
static PsglVertexAttrib1s sglVertexAttrib1sARB = 0;
static PsglVertexAttrib1sv sglVertexAttrib1svARB = 0;
static PsglVertexAttrib2d sglVertexAttrib2dARB = 0;
static PsglVertexAttrib2dv sglVertexAttrib2dvARB = 0;
static PsglVertexAttrib2f sglVertexAttrib2fARB = 0;
static PsglVertexAttrib2fv sglVertexAttrib2fvARB = 0;
static PsglVertexAttrib2s sglVertexAttrib2sARB = 0;
static PsglVertexAttrib2sv sglVertexAttrib2svARB = 0;
static PsglVertexAttrib3d sglVertexAttrib3dARB = 0;
static PsglVertexAttrib3dv sglVertexAttrib3dvARB = 0;
static PsglVertexAttrib3f sglVertexAttrib3fARB = 0;
static PsglVertexAttrib3fv sglVertexAttrib3fvARB = 0;
static PsglVertexAttrib3s sglVertexAttrib3sARB = 0;
static PsglVertexAttrib3sv sglVertexAttrib3svARB = 0;
static PsglVertexAttrib4Nbv sglVertexAttrib4NbvARB = 0;
static PsglVertexAttrib4Niv sglVertexAttrib4NivARB = 0;
static PsglVertexAttrib4Nsv sglVertexAttrib4NsvARB = 0;
static PsglVertexAttrib4Nub sglVertexAttrib4NubARB = 0;
static PsglVertexAttrib4Nubv sglVertexAttrib4NubvARB = 0;
static PsglVertexAttrib4Nuiv sglVertexAttrib4NuivARB = 0;
static PsglVertexAttrib4Nusv sglVertexAttrib4NusvARB = 0;
static PsglVertexAttrib4bv sglVertexAttrib4bvARB = 0;
static PsglVertexAttrib4d sglVertexAttrib4dARB = 0;
static PsglVertexAttrib4dv sglVertexAttrib4dvARB = 0;
static PsglVertexAttrib4f sglVertexAttrib4fARB = 0;
static PsglVertexAttrib4fv sglVertexAttrib4fvARB = 0;
static PsglVertexAttrib4iv sglVertexAttrib4ivARB = 0;
static PsglVertexAttrib4s sglVertexAttrib4sARB = 0;
static PsglVertexAttrib4sv sglVertexAttrib4svARB = 0;
static PsglVertexAttrib4ubv sglVertexAttrib4ubvARB = 0;
static PsglVertexAttrib4uiv sglVertexAttrib4uivARB = 0;
static PsglVertexAttrib4usv sglVertexAttrib4usvARB = 0;
static PsglVertexAttribPointer sglVertexAttribPointerARB = 0;
//----------------------------------------------------------------------------
void GTglBindProgramARB (GLenum eTarget, GLuint uiProgram)
{
    GT_BODY_A2(sglBindProgramARB,eTarget,uiProgram);
}
//----------------------------------------------------------------------------
void GTglDeleteProgramsARB (GLsizei iNumPrograms, const GLuint* auiPrograms)
{
    GT_BODY_A2(sglDeleteProgramsARB,iNumPrograms,auiPrograms);
}
//----------------------------------------------------------------------------
void GTglDisableVertexAttribArrayARB (GLuint uiIndex)
{
    GT_BODY_A1(sglDisableVertexAttribArrayARB,uiIndex);
}
//----------------------------------------------------------------------------
void GTglEnableVertexAttribArrayARB (GLuint uiIndex)
{
    GT_BODY_A1(sglEnableVertexAttribArrayARB,uiIndex);
}
//----------------------------------------------------------------------------
void GTglGenProgramsARB (GLsizei iNumPrograms, GLuint* auiPrograms)
{
    GT_BODY_A2(sglGenProgramsARB,iNumPrograms,auiPrograms);
}
//----------------------------------------------------------------------------
void GTglGetProgramivARB (GLenum eTarget, GLenum ePName, GLint* aiParam)
{
    GT_BODY_A3(sglGetProgramivARB,eTarget,ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTglGetProgramEnvParameterdvARB (GLenum eTarget, GLuint uiIndex,
    GLdouble* adParam)
{
    GT_BODY_A3(sglGetProgramEnvParameterdvARB,eTarget,uiIndex,adParam);
}
//----------------------------------------------------------------------------
void GTglGetProgramEnvParameterfvARB (GLenum eTarget, GLuint uiIndex,
    GLfloat* afParam)
{
    GT_BODY_A3(sglGetProgramEnvParameterfvARB,eTarget,uiIndex,afParam);
}
//----------------------------------------------------------------------------
void GTglGetProgramLocalParameterdvARB (GLenum eTarget, GLuint uiIndex,
    GLdouble* adParam)
{
    GT_BODY_A3(sglGetProgramLocalParameterdvARB,eTarget,uiIndex,adParam);
}
//----------------------------------------------------------------------------
void GTglGetProgramLocalParameterfvARB (GLenum eTarget, GLuint uiIndex,
    GLfloat* afParam)
{
    GT_BODY_A3(sglGetProgramLocalParameterfvARB,eTarget,uiIndex,afParam);
}
//----------------------------------------------------------------------------
void GTglGetProgramStringARB (GLenum eTarget, GLenum ePName,
    GLvoid* pvString)
{
    GT_BODY_A3(sglGetProgramStringARB,eTarget,ePName,pvString);
}
//----------------------------------------------------------------------------
void GTglGetVertexAttribdvARB (GLuint uiIndex, GLenum ePName,
    GLdouble* adParam)
{
    GT_BODY_A3(sglGetVertexAttribdvARB,uiIndex,ePName,adParam);
}
//----------------------------------------------------------------------------
void GTglGetVertexAttribfvARB (GLuint uiIndex, GLenum ePName,
    GLfloat* afParam)
{
    GT_BODY_A3(sglGetVertexAttribfvARB,uiIndex,ePName,afParam);
}
//----------------------------------------------------------------------------
void GTglGetVertexAttribivARB (GLuint uiIndex, GLenum ePName, GLint* aiParam)
{
    GT_BODY_A3(sglGetVertexAttribivARB,uiIndex,ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTglGetVertexAttribPointervARB (GLuint uiIndex, GLenum ePName,
    GLvoid** apvPointer)
{
    GT_BODY_A3(sglGetVertexAttribPointervARB,uiIndex,ePName,apvPointer);
}
//----------------------------------------------------------------------------
GLboolean GTglIsProgramARB (GLuint uiProgram)
{
    GT_BODY_A1_RET(sglIsProgramARB,uiProgram,GLboolean,0);
}
//----------------------------------------------------------------------------
void GTglProgramEnvParameter4dARB (GLenum eTarget, GLuint uiIndex,
    GLdouble dX, GLdouble dY, GLdouble dZ, GLdouble dW)
{
    GT_BODY_A6(sglProgramEnvParameter4dARB,eTarget,uiIndex,dX,dY,dZ,dW);
}
//----------------------------------------------------------------------------
void GTglProgramEnvParameter4dvARB (GLenum eTarget, GLuint uiIndex,
    const GLdouble* adXYZW)
{
    GT_BODY_A3(sglProgramEnvParameter4dvARB,eTarget,uiIndex,adXYZW);
}
//----------------------------------------------------------------------------
void GTglProgramEnvParameter4fARB (GLenum eTarget, GLuint uiIndex,
    GLfloat fX, GLfloat fY, GLfloat fZ, GLfloat fW)
{
    GT_BODY_A6(sglProgramEnvParameter4fARB,eTarget,uiIndex,fX,fY,fZ,fW);
}
//----------------------------------------------------------------------------
void GTglProgramEnvParameter4fvARB (GLenum eTarget, GLuint uiIndex,
    const GLfloat* afXYZW)
{
    GT_BODY_A3(sglProgramEnvParameter4fvARB,eTarget,uiIndex,afXYZW);
}
//----------------------------------------------------------------------------
void GTglProgramLocalParameter4dARB (GLenum eTarget, GLuint uiIndex,
    GLdouble dX, GLdouble dY, GLdouble dZ, GLdouble dW)
{
    GT_BODY_A6(sglProgramLocalParameter4dARB,eTarget,uiIndex,dX,dY,dZ,dW);
}
//----------------------------------------------------------------------------
void GTglProgramLocalParameter4dvARB (GLenum eTarget, GLuint uiIndex,
    const GLdouble* adXYZW)
{
    GT_BODY_A3(sglProgramLocalParameter4dvARB,eTarget,uiIndex,adXYZW);
}
//----------------------------------------------------------------------------
void GTglProgramLocalParameter4fARB (GLenum eTarget, GLuint uiIndex,
    GLfloat fX, GLfloat fY, GLfloat fZ, GLfloat fW)
{
    GT_BODY_A6(sglProgramLocalParameter4fARB,eTarget,uiIndex,fX,fY,fZ,fW);
}
//----------------------------------------------------------------------------
void GTglProgramLocalParameter4fvARB (GLenum eTarget, GLuint uiIndex,
    const GLfloat* afXYZW)
{
    GT_BODY_A3(sglProgramLocalParameter4fvARB,eTarget,uiIndex,afXYZW);
}
//----------------------------------------------------------------------------
void GTglProgramStringARB (GLenum eTarget, GLenum eFormat, GLsizei iLength,
    const GLvoid* pvString)
{
    GT_BODY_A4(sglProgramStringARB,eTarget,eFormat,iLength,pvString);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib1dARB (GLuint uiIndex, GLdouble dX)
{
    GT_BODY_A2(sglVertexAttrib1dARB,uiIndex,dX);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib1dvARB (GLuint uiIndex, const GLdouble* pdX)
{
    GT_BODY_A2(sglVertexAttrib1dvARB,uiIndex,pdX);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib1fARB (GLuint uiIndex, GLfloat fX)
{
    GT_BODY_A2(sglVertexAttrib1fARB,uiIndex,fX);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib1fvARB (GLuint uiIndex, const GLfloat* pfX)
{
    GT_BODY_A2(sglVertexAttrib1fvARB,uiIndex,pfX);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib1sARB (GLuint uiIndex, GLshort sX)
{
    GT_BODY_A2(sglVertexAttrib1sARB,uiIndex,sX);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib1svARB (GLuint uiIndex, const GLshort* psX)
{
    GT_BODY_A2(sglVertexAttrib1svARB,uiIndex,psX);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib2dARB (GLuint uiIndex, GLdouble dX, GLdouble dY)
{
    GT_BODY_A3(sglVertexAttrib2dARB,uiIndex,dX,dY);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib2dvARB (GLuint uiIndex, const GLdouble* adXY)
{
    GT_BODY_A2(sglVertexAttrib2dvARB,uiIndex,adXY);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib2fARB (GLuint uiIndex, GLfloat fX, GLfloat fY)
{
    GT_BODY_A3(sglVertexAttrib2fARB,uiIndex,fX,fY);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib2fvARB (GLuint uiIndex, const GLfloat* afXY)
{
    GT_BODY_A2(sglVertexAttrib2fvARB,uiIndex,afXY);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib2sARB (GLuint uiIndex, GLshort sX, GLshort sY)
{
    GT_BODY_A3(sglVertexAttrib2sARB,uiIndex,sX,sY);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib2svARB (GLuint uiIndex, const GLshort* asXY)
{
    GT_BODY_A2(sglVertexAttrib2svARB,uiIndex,asXY);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib3dARB (GLuint uiIndex, GLdouble dX, GLdouble dY,
    GLdouble dZ)
{
    GT_BODY_A4(sglVertexAttrib3dARB,uiIndex,dX,dY,dZ);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib3dvARB (GLuint uiIndex, const GLdouble* adXYZ)
{
    GT_BODY_A2(sglVertexAttrib3dvARB,uiIndex,adXYZ);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib3fARB (GLuint uiIndex, GLfloat fX, GLfloat fY,
    GLfloat fZ)
{
    GT_BODY_A4(sglVertexAttrib3fARB,uiIndex,fX,fY,fZ);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib3fvARB (GLuint uiIndex, const GLfloat* afXYZ)
{
    GT_BODY_A2(sglVertexAttrib3fvARB,uiIndex,afXYZ);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib3sARB (GLuint uiIndex, GLshort sX, GLshort sY,
    GLshort sZ)
{
    GT_BODY_A4(sglVertexAttrib3sARB,uiIndex,sX,sY,sZ);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib3svARB (GLuint uiIndex, const GLshort* asXYZ)
{
    GT_BODY_A2(sglVertexAttrib3svARB,uiIndex,asXYZ);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib4NbvARB (GLuint uiIndex, const GLbyte* acXYZW)
{
    GT_BODY_A2(sglVertexAttrib4NbvARB,uiIndex,acXYZW);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib4NivARB (GLuint uiIndex, const GLint* aiXYZW)
{
    GT_BODY_A2(sglVertexAttrib4NivARB,uiIndex,aiXYZW);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib4NsvARB (GLuint uiIndex, const GLshort* asXYZW)
{
    GT_BODY_A2(sglVertexAttrib4NsvARB,uiIndex,asXYZW);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib4NubARB (GLuint uiIndex, GLubyte ucX, GLubyte ucY,
    GLubyte ucZ, GLubyte ucW)
{
    GT_BODY_A5(sglVertexAttrib4NubARB,uiIndex,ucX,ucY,ucZ,ucW);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib4NubvARB (GLuint uiIndex, const GLubyte* aucXYZW)
{
    GT_BODY_A2(sglVertexAttrib4NubvARB,uiIndex,aucXYZW);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib4NuivARB (GLuint uiIndex, const GLuint* auiXYZW)
{
    GT_BODY_A2(sglVertexAttrib4NuivARB,uiIndex,auiXYZW);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib4NusvARB (GLuint uiIndex, const GLushort* ausXYZW)
{
    GT_BODY_A2(sglVertexAttrib4NusvARB,uiIndex,ausXYZW);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib4bvARB (GLuint uiIndex, const GLbyte* acXYZW)
{
    GT_BODY_A2(sglVertexAttrib4bvARB,uiIndex,acXYZW);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib4dARB (GLuint uiIndex, GLdouble dX, GLdouble dY,
    GLdouble dZ, GLdouble dW)
{
    GT_BODY_A5(sglVertexAttrib4dARB,uiIndex,dX,dY,dZ,dW);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib4dvARB (GLuint uiIndex, const GLdouble* adXYZW)
{
    GT_BODY_A2(sglVertexAttrib4dvARB,uiIndex,adXYZW);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib4fARB (GLuint uiIndex, GLfloat fX, GLfloat fY,
    GLfloat fZ, GLfloat fW)
{
    GT_BODY_A5(sglVertexAttrib4fARB,uiIndex,fX,fY,fZ,fW);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib4fvARB (GLuint uiIndex, const GLfloat* afXYZW)
{
    GT_BODY_A2(sglVertexAttrib4fvARB,uiIndex,afXYZW);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib4ivARB (GLuint uiIndex, const GLint* aiXYZW)
{
    GT_BODY_A2(sglVertexAttrib4ivARB,uiIndex,aiXYZW);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib4sARB (GLuint uiIndex, GLshort sX, GLshort sY,
    GLshort sZ, GLshort sW)
{
    GT_BODY_A5(sglVertexAttrib4sARB,uiIndex,sX,sY,sZ,sW);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib4svARB (GLuint uiIndex, const GLshort* asXYZW)
{
    GT_BODY_A2(sglVertexAttrib4svARB,uiIndex,asXYZW);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib4ubvARB (GLuint uiIndex, const GLubyte* aucXYZW)
{
    GT_BODY_A2(sglVertexAttrib4ubvARB,uiIndex,aucXYZW);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib4uivARB (GLuint uiIndex, const GLuint* auiXYZW)
{
    GT_BODY_A2(sglVertexAttrib4uivARB,uiIndex,auiXYZW);
}
//----------------------------------------------------------------------------
void GTglVertexAttrib4usvARB (GLuint uiIndex, const GLushort* ausXYZW)
{
    GT_BODY_A2(sglVertexAttrib4usvARB,uiIndex,ausXYZW);
}
//----------------------------------------------------------------------------
void GTglVertexAttribPointerARB (GLuint uiIndex, GLint iSize, GLenum eType,
    GLboolean ucNormalized, GLsizei iStride, const GLvoid* pvPointer)
{
    GT_BODY_A6(sglVertexAttribPointerARB,uiIndex,iSize,eType,ucNormalized,
        iStride,pvPointer);
}
//----------------------------------------------------------------------------
void GTInitGlArbVertexProgram ()
{
    if (GTSupportsExtension("GL_ARB_vertex_program"))
    {
        GTExistsGlArbVertexProgram = 1;

        GT_GET_FUNCTION(sglBindProgramARB);
        GT_GET_FUNCTION(sglDeleteProgramsARB);
        GT_GET_FUNCTION(sglGenProgramsARB);
        GT_GET_FUNCTION(sglGetProgramEnvParameterdvARB);
        GT_GET_FUNCTION(sglGetProgramEnvParameterfvARB);
        GT_GET_FUNCTION(sglGetProgramLocalParameterdvARB);
        GT_GET_FUNCTION(sglGetProgramLocalParameterfvARB);
        GT_GET_FUNCTION(sglGetProgramStringARB);
        GT_GET_FUNCTION(sglProgramEnvParameter4dARB);
        GT_GET_FUNCTION(sglProgramEnvParameter4dvARB);
        GT_GET_FUNCTION(sglProgramEnvParameter4fARB);
        GT_GET_FUNCTION(sglProgramEnvParameter4fvARB);
        GT_GET_FUNCTION(sglProgramLocalParameter4dARB);
        GT_GET_FUNCTION(sglProgramLocalParameter4dvARB);
        GT_GET_FUNCTION(sglProgramLocalParameter4fARB);
        GT_GET_FUNCTION(sglProgramLocalParameter4fvARB);
        GT_GET_FUNCTION(sglProgramStringARB);

        GT_GET_FUNCTION_C(sglDisableVertexAttribArray,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglEnableVertexAttribArray,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglGetProgramiv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglGetVertexAttribdv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglGetVertexAttribfv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglGetVertexAttribiv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglGetVertexAttribPointerv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglIsProgram,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglVertexAttrib1d,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglVertexAttrib1dv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglVertexAttrib1f,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglVertexAttrib1fv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglVertexAttrib1s,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglVertexAttrib1sv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglVertexAttrib2d,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglVertexAttrib2dv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglVertexAttrib2f,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglVertexAttrib2fv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglVertexAttrib2s,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglVertexAttrib2sv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglVertexAttrib3d,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglVertexAttrib3dv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglVertexAttrib3f,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglVertexAttrib3fv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglVertexAttrib3s,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglVertexAttrib3sv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglVertexAttrib4Nbv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglVertexAttrib4Niv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglVertexAttrib4Nsv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglVertexAttrib4Nub,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglVertexAttrib4Nubv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglVertexAttrib4Nuiv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglVertexAttrib4Nusv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglVertexAttrib4bv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglVertexAttrib4d,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglVertexAttrib4dv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglVertexAttrib4f,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglVertexAttrib4fv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglVertexAttrib4iv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglVertexAttrib4s,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglVertexAttrib4sv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglVertexAttrib4ubv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglVertexAttrib4uiv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglVertexAttrib4usv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglVertexAttribPointer,ARB,GT_ARB_OVERRIDE);
    }
}//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 27) GL_ARB_fragment_program
//----------------------------------------------------------------------------
int GTExistsGlArbFragmentProgram = 0;
//----------------------------------------------------------------------------
void GTInitGlArbFragmentProgram ()
{
    if (GTSupportsExtension("GL_ARB_fragment_program"))
    {
        GTExistsGlArbFragmentProgram = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 28) GL_ARB_vertex_buffer_object
//----------------------------------------------------------------------------
int GTExistsGlArbVertexBufferObject = 0;

static PsglBindBuffer sglBindBufferARB = 0;
static PsglDeleteBuffers sglDeleteBuffersARB = 0;
static PsglGenBuffers sglGenBuffersARB = 0;
static PsglIsBuffer sglIsBufferARB = 0;
static PsglBufferData sglBufferDataARB = 0;
static PsglBufferSubData sglBufferSubDataARB = 0;
static PsglGetBufferSubData sglGetBufferSubDataARB = 0;
static PsglMapBuffer sglMapBufferARB = 0;
static PsglUnmapBuffer sglUnmapBufferARB = 0;
static PsglGetBufferParameteriv sglGetBufferParameterivARB = 0;
static PsglGetBufferPointerv sglGetBufferPointervARB = 0;
//----------------------------------------------------------------------------
void GTglBindBufferARB (GLenum eTarget, GLuint uiBuffer)
{
    GT_BODY_A2(sglBindBufferARB,eTarget,uiBuffer);
}
//----------------------------------------------------------------------------
void GTglDeleteBuffersARB (GLsizei iNumBuffers, const GLuint* auiBuffer)
{
    GT_BODY_A2(sglDeleteBuffersARB,iNumBuffers,auiBuffer);
}
//----------------------------------------------------------------------------
void GTglGenBuffersARB (GLsizei iNumBuffers, GLuint* auiBuffer)
{
    GT_BODY_A2(sglGenBuffersARB,iNumBuffers,auiBuffer);
}
//----------------------------------------------------------------------------
GLboolean GTglIsBufferARB (GLuint uiBuffer)
{
    GT_BODY_A1_RET(sglIsBufferARB,uiBuffer,GLboolean,0);
}
//----------------------------------------------------------------------------
void GTglBufferDataARB (GLenum eTarget, GLsizeiptrARB piSize,
    const GLvoid* pvData, GLenum eUsage)
{
    GT_BODY_A4(sglBufferDataARB,eTarget,piSize,pvData,eUsage);
}
//----------------------------------------------------------------------------
void GTglBufferSubDataARB (GLenum eTarget, GLintptrARB piOffset,
    GLsizeiptrARB piSize, const GLvoid* pvData)
{
    GT_BODY_A4(sglBufferSubDataARB,eTarget,piOffset,piSize,pvData);
}
//----------------------------------------------------------------------------
void GTglGetBufferSubDataARB (GLenum eTarget, GLintptrARB piOffset,
    GLsizeiptrARB piSize, GLvoid* pvData)
{
    GT_BODY_A4(sglGetBufferSubDataARB,eTarget,piOffset,piSize,pvData);
}
//----------------------------------------------------------------------------
GLvoid* GTglMapBufferARB (GLenum eTarget, GLenum eAccess)
{
    GT_BODY_A2_RET(sglMapBufferARB,eTarget,eAccess,GLvoid*,0);
}
//----------------------------------------------------------------------------
GLboolean GTglUnmapBufferARB (GLenum eTarget)
{
    GT_BODY_A1_RET(sglUnmapBufferARB,eTarget,GLboolean,0);
}
//----------------------------------------------------------------------------
void GTglGetBufferParameterivARB (GLenum eTarget, GLenum ePName,
    GLint* aiParam)
{
    GT_BODY_A3(sglGetBufferParameterivARB,eTarget,ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTglGetBufferPointervARB (GLenum eTarget, GLenum ePName,
    GLvoid** apvParam)
{
    GT_BODY_A3(sglGetBufferPointervARB,eTarget,ePName,apvParam);
}
//----------------------------------------------------------------------------
void GTInitGlArbVertexBufferObject ()
{
    if (GTSupportsExtension("GL_ARB_vertex_buffer_object"))
    {
        GTExistsGlArbVertexBufferObject = 1;
        GT_GET_FUNCTION_C(sglBindBuffer,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglDeleteBuffers,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglGenBuffers,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglIsBuffer,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglBufferData,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglBufferSubData,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglGetBufferSubData,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglMapBuffer,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglUnmapBuffer,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglGetBufferParameteriv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglGetBufferPointerv,ARB,GT_ARB_OVERRIDE);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 29) GL_ARB_occlusion_query
//----------------------------------------------------------------------------
int GTExistsGlArbOcclusionQuery = 0;

static PsglGenQueries sglGenQueriesARB = 0;
static PsglDeleteQueries sglDeleteQueriesARB = 0;
static PsglIsQuery sglIsQueryARB = 0;
static PsglBeginQuery sglBeginQueryARB = 0;
static PsglEndQuery sglEndQueryARB = 0;
static PsglGetQueryiv sglGetQueryivARB = 0;
static PsglGetQueryObjectiv sglGetQueryObjectivARB = 0;
static PsglGetQueryObjectuiv sglGetQueryObjectuivARB = 0;
//----------------------------------------------------------------------------
void GTglGenQueriesARB (GLsizei iNumQueries, GLuint* auiID)
{
    GT_BODY_A2(sglGenQueriesARB,iNumQueries,auiID);
}
//----------------------------------------------------------------------------
void GTglDeleteQueriesARB (GLsizei iNumQueries, const GLuint* auiID)
{
    GT_BODY_A2(sglDeleteQueriesARB,iNumQueries,auiID);
}
//----------------------------------------------------------------------------
GLboolean GTglIsQueryARB (GLuint uiID)
{
    GT_BODY_A1_RET(sglIsQueryARB,uiID,GLboolean,0);
}
//----------------------------------------------------------------------------
void GTglBeginQueryARB (GLenum eTarget, GLuint uiID)
{
    GT_BODY_A2(sglBeginQueryARB,eTarget,uiID);
}
//----------------------------------------------------------------------------
void GTglEndQueryARB (GLenum eTarget)
{
    GT_BODY_A1(sglEndQueryARB,eTarget);
}
//----------------------------------------------------------------------------
void GTglGetQueryivARB (GLenum eTarget, GLenum ePName, GLint* aiParam)
{
    GT_BODY_A3(sglGetQueryivARB,eTarget,ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTglGetQueryObjectivARB (GLuint uiID, GLenum ePName, GLint* aiParam)
{
    GT_BODY_A3(sglGetQueryObjectivARB,uiID,ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTglGetQueryObjectuivARB (GLuint uiID, GLenum ePName, GLuint* auiParam)
{
    GT_BODY_A3(sglGetQueryObjectuivARB,uiID,ePName,auiParam);
}
//----------------------------------------------------------------------------
void GTInitGlArbOcclusionQuery ()
{
    if (GTSupportsExtension("GL_ARB_occlusion_query"))
    {
        GTExistsGlArbOcclusionQuery = 1;
        GT_GET_FUNCTION_C(sglGenQueries,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglDeleteQueries,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglIsQuery,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglBeginQuery,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglEndQuery,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglGetQueryiv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglGetQueryObjectiv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglGetQueryObjectuiv,ARB,GT_ARB_OVERRIDE);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 29) GL_ARB_shader_objects
//----------------------------------------------------------------------------
int GTExistsGlArbShaderObjects = 0;

typedef GLhandleARB (STDCALL* PsglGetHandleARB) (GLenum);
typedef void (STDCALL* PsglGetObjectParameterfvARB) (GLhandleARB, GLenum,
    GLfloat*);
typedef void (STDCALL* PsglGetObjectParameterivARB) (GLhandleARB, GLenum,
    GLint*);

typedef void (STDCALL* PsglAttachObject) (GLhandleARB, GLhandleARB);
typedef GLhandleARB (STDCALL* PsglCreateProgramObject) ();
typedef GLhandleARB (STDCALL* PsglCreateShaderObject) (GLenum);
typedef void (STDCALL* PsglDeleteObject) (GLhandleARB);
typedef void (STDCALL* PsglDetachObject) (GLhandleARB, GLhandleARB);
typedef void (STDCALL* PsglGetInfoLog) (GLhandleARB, GLsizei, GLsizei*,
    GLcharARB*);
typedef void (STDCALL* PsglGetAttachedObjects) (GLhandleARB, GLsizei,
    GLsizei*, GLhandleARB*);
typedef void (STDCALL* PsglUseProgramObject) (GLhandleARB);

static PsglGetHandleARB sglGetHandleARB = 0;
static PsglGetObjectParameterfvARB sglGetObjectParameterfvARB = 0;
static PsglGetObjectParameterivARB sglGetObjectParameterivARB = 0;

static PsglAttachObject sglAttachObjectARB = 0;
static PsglCreateProgramObject sglCreateProgramObjectARB = 0;
static PsglCreateShaderObject sglCreateShaderObjectARB = 0;
static PsglDeleteObject sglDeleteObjectARB = 0;
static PsglDetachObject sglDetachObjectARB = 0;
static PsglGetInfoLog sglGetInfoLogARB = 0;
static PsglGetAttachedObjects sglGetAttachedObjectsARB = 0;
static PsglUseProgramObject sglUseProgramObjectARB = 0;

static PsglCompileShader sglCompileShaderARB = 0;
static PsglGetActiveUniform sglGetActiveUniformARB = 0;
static PsglGetUniformfv sglGetUniformfvARB = 0;
static PsglGetUniformiv sglGetUniformivARB = 0;
static PsglGetUniformLocation sglGetUniformLocationARB = 0;
static PsglGetShaderSource sglGetShaderSourceARB = 0;
static PsglLinkProgram sglLinkProgramARB = 0;
static PsglShaderSource sglShaderSourceARB = 0;
static PsglUniform1f sglUniform1fARB = 0;
static PsglUniform2f sglUniform2fARB = 0;
static PsglUniform3f sglUniform3fARB = 0;
static PsglUniform4f sglUniform4fARB = 0;
static PsglUniform1i sglUniform1iARB = 0;
static PsglUniform2i sglUniform2iARB = 0;
static PsglUniform3i sglUniform3iARB = 0;
static PsglUniform4i sglUniform4iARB = 0;
static PsglUniform1fv sglUniform1fvARB = 0;
static PsglUniform2fv sglUniform2fvARB = 0;
static PsglUniform3fv sglUniform3fvARB = 0;
static PsglUniform4fv sglUniform4fvARB = 0;
static PsglUniform1iv sglUniform1ivARB = 0;
static PsglUniform2iv sglUniform2ivARB = 0;
static PsglUniform3iv sglUniform3ivARB = 0;
static PsglUniform4iv sglUniform4ivARB = 0;
static PsglUniformMatrix2fv sglUniformMatrix2fvARB = 0;
static PsglUniformMatrix3fv sglUniformMatrix3fvARB = 0;
static PsglUniformMatrix4fv sglUniformMatrix4fvARB = 0;
static PsglValidateProgram sglValidateProgramARB = 0;
//----------------------------------------------------------------------------
void GTglAttachObjectARB (GLhandleARB uiContainer, GLhandleARB uiObject)
{
    GT_BODY_A2(sglAttachObjectARB,uiContainer,uiObject);
}
//----------------------------------------------------------------------------
void GTglCompileShaderARB (GLhandleARB uiShader)
{
    GT_BODY_A1(sglCompileShaderARB,uiShader);
}
//----------------------------------------------------------------------------
GLhandleARB GTglCreateProgramObjectARB ()
{
    // TO DO:  Check the default return value of 0 for correctness.
    GT_BODY_A0_RET(sglCreateProgramObjectARB,GLhandleARB,0);
}
//----------------------------------------------------------------------------
GLhandleARB GTglCreateShaderObjectARB (GLenum eType)
{
    // TO DO:  Check the default return value of 0 for correctness.
    GT_BODY_A1_RET(sglCreateShaderObjectARB,eType,GLhandleARB,0);
}
//----------------------------------------------------------------------------
void GTglDeleteObjectARB (GLhandleARB uiObject)
{
    GT_BODY_A1(sglDeleteObjectARB,uiObject);
}
//----------------------------------------------------------------------------
void GTglDetachObjectARB (GLhandleARB uiContainer, GLhandleARB uiObject)
{
    GT_BODY_A2(sglDetachObjectARB,uiContainer,uiObject);
}
//----------------------------------------------------------------------------
void GTglGetActiveUniformARB (GLhandleARB uiProgram, GLuint uiIndex,
    GLsizei iMaxLength, GLsizei* piLength, GLint* piSize, GLenum* peType,
    GLcharARB* acName)
{
    GT_BODY_A7(sglGetActiveUniformARB,uiProgram,uiIndex,iMaxLength,piLength,
        piSize,peType,acName);
}
//----------------------------------------------------------------------------
void GTglGetAttachedObjectsARB (GLhandleARB uiContainer, GLsizei iMaxCount,
    GLsizei* aiCount, GLhandleARB* auiObject)
{
    GT_BODY_A4(sglGetAttachedObjectsARB,uiContainer,iMaxCount,aiCount,
        auiObject);
}
//----------------------------------------------------------------------------
GLhandleARB GTglGetHandleARB (GLenum ePName)
{
    // TO DO:  Check the default return value of 0 for correctness.
    GT_BODY_A1_RET(sglGetHandleARB,ePName,GLhandleARB,0);
}
//----------------------------------------------------------------------------
void GTglGetInfoLogARB (GLhandleARB uiObject, GLsizei iMaxLength,
    GLsizei* piLength, GLcharARB* acInfoLog)
{
    GT_BODY_A4(sglGetInfoLogARB,uiObject,iMaxLength,piLength,acInfoLog);
}
//----------------------------------------------------------------------------
void GTglGetObjectParameterfvARB (GLhandleARB uiObject, GLenum ePName,
    GLfloat* afParam)
{
    GT_BODY_A3(sglGetObjectParameterfvARB,uiObject,ePName,afParam);
}
//----------------------------------------------------------------------------
void GTglGetObjectParameterivARB (GLhandleARB uiObject, GLenum ePName,
    GLint* aiParam)
{
    GT_BODY_A3(sglGetObjectParameterivARB,uiObject,ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTglGetShaderSourceARB (GLhandleARB uiObject, GLsizei iMaxLength,
    GLsizei* piLength, GLcharARB* acSource)
{
    GT_BODY_A4(sglGetShaderSourceARB,uiObject,iMaxLength,piLength,acSource);
}
//----------------------------------------------------------------------------
GLint GTglGetUniformLocationARB (GLhandleARB uiProgram,
    const GLcharARB* acName)
{
    // TO DO:  Check the default return value of 0 for correctness.
    GT_BODY_A2_RET(sglGetUniformLocationARB,uiProgram,acName,GLint,0);
}
//----------------------------------------------------------------------------
void GTglGetUniformfvARB (GLhandleARB uiProgram, GLint iLocation,
    GLfloat* afParam)
{
    GT_BODY_A3(sglGetUniformfvARB,uiProgram,iLocation,afParam);
}
//----------------------------------------------------------------------------
void GTglGetUniformivARB (GLhandleARB uiProgram, GLint iLocation,
    GLint* aiParam)
{
    GT_BODY_A3(sglGetUniformivARB,uiProgram,iLocation,aiParam);
}
//----------------------------------------------------------------------------
void GTglLinkProgramARB (GLhandleARB uiProgram)
{
    GT_BODY_A1(sglLinkProgramARB,uiProgram);
}
//----------------------------------------------------------------------------
void GTglShaderSourceARB (GLhandleARB uiShader, GLsizei iCount,
    const GLcharARB** aacString, const GLint* aiLength)
{
    GT_BODY_A4(sglShaderSourceARB,uiShader,iCount,aacString,aiLength);
}
//----------------------------------------------------------------------------
void GTglUniform1fARB (GLint iLocation, GLfloat fV0)
{
    GT_BODY_A2(sglUniform1fARB,iLocation,fV0);
}
//----------------------------------------------------------------------------
void GTglUniform2fARB (GLint iLocation, GLfloat fV0, GLfloat fV1)
{
    GT_BODY_A3(sglUniform2fARB,iLocation,fV0,fV1);
}
//----------------------------------------------------------------------------
void GTglUniform3fARB (GLint iLocation, GLfloat fV0, GLfloat fV1,
    GLfloat fV2)
{
    GT_BODY_A4(sglUniform3fARB,iLocation,fV0,fV1,fV2);
}
//----------------------------------------------------------------------------
void GTglUniform4fARB (GLint iLocation, GLfloat fV0, GLfloat fV1, GLfloat fV2,
    GLfloat fV3)
{
    GT_BODY_A5(sglUniform4fARB,iLocation,fV0,fV1,fV2,fV3);
}
//----------------------------------------------------------------------------
void GTglUniform1iARB (GLint iLocation, GLint iV0)
{
    GT_BODY_A2(sglUniform1iARB,iLocation,iV0);
}
//----------------------------------------------------------------------------
void GTglUniform2iARB (GLint iLocation, GLint iV0, GLint iV1)
{
    GT_BODY_A3(sglUniform2iARB,iLocation,iV0,iV1);
}
//----------------------------------------------------------------------------
void GTglUniform3iARB (GLint iLocation, GLint iV0, GLint iV1, GLint iV2)
{
    GT_BODY_A4(sglUniform3iARB,iLocation,iV0,iV1,iV2);
}
//----------------------------------------------------------------------------
void GTglUniform4iARB (GLint iLocation, GLint iV0, GLint iV1, GLint iV2,
    GLint iV3)
{
    GT_BODY_A5(sglUniform4iARB,iLocation,iV0,iV1,iV2,iV3);
}
//----------------------------------------------------------------------------
void GTglUniform1fvARB (GLint iLocation, GLsizei iCount,
    const GLfloat* afValue)
{
    GT_BODY_A3(sglUniform1fvARB,iLocation,iCount,afValue);
}
//----------------------------------------------------------------------------
void GTglUniform2fvARB (GLint iLocation, GLsizei iCount,
    const GLfloat* afValue)
{
    GT_BODY_A3(sglUniform2fvARB,iLocation,iCount,afValue);
}
//----------------------------------------------------------------------------
void GTglUniform3fvARB (GLint iLocation, GLsizei iCount,
    const GLfloat* afValue)
{
    GT_BODY_A3(sglUniform3fvARB,iLocation,iCount,afValue);
}
//----------------------------------------------------------------------------
void GTglUniform4fvARB (GLint iLocation, GLsizei iCount,
    const GLfloat* afValue)
{
    GT_BODY_A3(sglUniform4fvARB,iLocation,iCount,afValue);
}
//----------------------------------------------------------------------------
void GTglUniform1ivARB (GLint iLocation, GLsizei iCount,
    const GLint* aiValue)
{
    GT_BODY_A3(sglUniform1ivARB,iLocation,iCount,aiValue);
}
//----------------------------------------------------------------------------
void GTglUniform2ivARB (GLint iLocation, GLsizei iCount,
    const GLint* aiValue)
{
    GT_BODY_A3(sglUniform2ivARB,iLocation,iCount,aiValue);
}
//----------------------------------------------------------------------------
void GTglUniform3ivARB (GLint iLocation, GLsizei iCount,
    const GLint* aiValue)
{
    GT_BODY_A3(sglUniform3ivARB,iLocation,iCount,aiValue);
}
//----------------------------------------------------------------------------
void GTglUniform4ivARB (GLint iLocation, GLsizei iCount,
    const GLint* aiValue)
{
    GT_BODY_A3(sglUniform4ivARB,iLocation,iCount,aiValue);
}
//----------------------------------------------------------------------------
void GTglUniformMatrix2fvARB (GLint iLocation, GLsizei iCount,
    GLboolean ucTranspose, const GLfloat* afValue)
{
    GT_BODY_A4(sglUniformMatrix2fvARB,iLocation,iCount,ucTranspose,afValue);
}
//----------------------------------------------------------------------------
void GTglUniformMatrix3fvARB (GLint iLocation, GLsizei iCount,
    GLboolean ucTranspose, const GLfloat* afValue)
{
    GT_BODY_A4(sglUniformMatrix3fvARB,iLocation,iCount,ucTranspose,afValue);
}
//----------------------------------------------------------------------------
void GTglUniformMatrix4fvARB (GLint iLocation, GLsizei iCount,
    GLboolean ucTranspose, const GLfloat* afValue)
{
    GT_BODY_A4(sglUniformMatrix4fvARB,iLocation,iCount,ucTranspose,afValue);
}
//----------------------------------------------------------------------------
void GTglUseProgramObjectARB (GLhandleARB uiProgram)
{
    GT_BODY_A1(sglUseProgramObjectARB,uiProgram);
}
//----------------------------------------------------------------------------
void GTglValidateProgramARB (GLhandleARB uiProgram)
{
    GT_BODY_A1(sglValidateProgramARB,uiProgram);
}
//----------------------------------------------------------------------------
void GTInitGlArbShaderObjects ()
{
    if (GTSupportsExtension("GL_ARB_shader_objects"))
    {
        GTExistsGlArbShaderObjects = 1;

        GT_GET_FUNCTION(sglGetHandleARB);
        GT_GET_FUNCTION(sglGetObjectParameterfvARB);
        GT_GET_FUNCTION(sglGetObjectParameterivARB);

        GT_GET_FUNCTION_CA(sglAttachObject,sglAttachShader,ARB,
            GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_CA(sglCreateProgramObject,sglCreateProgram,ARB,
            GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_CA(sglCreateShaderObject,sglCreateShader,ARB,
            GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_CA(sglDeleteObject,sglDeleteProgram,ARB,
            GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_CA(sglDeleteObject,sglDeleteShader,ARB,
            GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_CA(sglDetachObject,sglDetachShader,ARB,
            GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_CA(sglGetAttachedObjects,sglGetAttachedShaders,ARB,
            GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_CA(sglGetInfoLog,sglGetProgramInfoLog,ARB,
            GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_CA(sglGetInfoLog,sglGetShaderInfoLog,ARB,
            GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_CA(sglUseProgramObject,sglUseProgram,ARB,
            GT_ARB_OVERRIDE);

        GT_GET_FUNCTION_C(sglCompileShader,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglGetActiveUniform,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglGetShaderSource,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglGetUniformfv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglGetUniformiv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglGetUniformLocation,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglLinkProgram,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglShaderSource,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglUniform1f,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglUniform2f,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglUniform3f,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglUniform4f,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglUniform1i,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglUniform2i,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglUniform3i,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglUniform4i,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglUniform1fv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglUniform2fv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglUniform3fv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglUniform4fv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglUniform1iv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglUniform2iv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglUniform3iv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglUniform4iv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglUniformMatrix2fv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglUniformMatrix3fv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglUniformMatrix4fv,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglValidateProgram,ARB,GT_ARB_OVERRIDE);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 31) GL_ARB_vertex_shader
//----------------------------------------------------------------------------
int GTExistsGlArbVertexShader = 0;

static PsglBindAttribLocation sglBindAttribLocationARB = 0;
static PsglGetActiveAttrib sglGetActiveAttribARB = 0;
static PsglGetAttribLocation sglGetAttribLocationARB = 0;
//----------------------------------------------------------------------------
void GTglBindAttribLocationARB (GLhandleARB uiProgram, GLuint uiIndex,
    const GLcharARB* acName)
{
    GT_BODY_A3(sglBindAttribLocationARB,uiProgram,uiIndex,acName);
}
//----------------------------------------------------------------------------
void GTglGetActiveAttribARB (GLhandleARB uiProgram, GLuint uiIndex,
    GLsizei iMaxLength, GLsizei* piLength, GLint* piSize, GLenum* peType,
    GLcharARB* acName)
{
    GT_BODY_A7(sglGetActiveAttribARB,uiProgram,uiIndex,iMaxLength,piLength,
        piSize,peType,acName);
}
//----------------------------------------------------------------------------
GLint GTglGetAttribLocationARB (GLhandleARB uiProgram,
    const GLcharARB* acName)
{
    // TO DO:  Check the default return value of 0 for correctness.
    GT_BODY_A2_RET(sglGetAttribLocationARB,uiProgram,acName,GLint,0);
}
//----------------------------------------------------------------------------
void GTInitGlArbVertexShader ()
{
    if (GTSupportsExtension("GL_ARB_vertex_shader"))
    {
        GTExistsGlArbVertexShader = 1;
        GT_GET_FUNCTION_C(sglBindAttribLocation,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglGetActiveAttrib,ARB,GT_ARB_OVERRIDE);
        GT_GET_FUNCTION_C(sglGetAttribLocation,ARB,GT_ARB_OVERRIDE);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 32) GL_ARB_fragment_shader
//----------------------------------------------------------------------------
int GTExistsGlArbFragmentShader = 0;
//----------------------------------------------------------------------------
void GTInitGlArbFragmentShader ()
{
    if (GTSupportsExtension("GL_ARB_fragment_shader"))
    {
        GTExistsGlArbFragmentShader = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 33) GL_ARB_shading_language_100
//----------------------------------------------------------------------------
int GTExistsGlArbShadingLanguage100 = 0;
//----------------------------------------------------------------------------
void GTInitGlArbShadingLanguage100 ()
{
    if (GTSupportsExtension("GL_ARB_shading_language_100"))
    {
        GTExistsGlArbShadingLanguage100 = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 34) GL_ARB_texture_non_power_of_two
//----------------------------------------------------------------------------
int GTExistsGlArbTextureNonpowerOfTwo = 0;
//----------------------------------------------------------------------------
void GTInitGlArbTextureNonpowerOfTwo ()
{
    if (GTSupportsExtension("GL_ARB_texture_non_power_of_two"))
    {
        GTExistsGlArbTextureNonpowerOfTwo = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 35) GL_ARB_point_sprite
//----------------------------------------------------------------------------
int GTExistsGlArbPointSprite = 0;
//----------------------------------------------------------------------------
void GTInitGlArbPointSprite ()
{
    if (GTSupportsExtension("GL_ARB_point_sprite"))
    {
        GTExistsGlArbPointSprite = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 36) GL_ARB_fragment_program_shadow
//----------------------------------------------------------------------------
int GTExistsGlArbFragmentProgramShadow = 0;
//----------------------------------------------------------------------------
void GTInitGlArbFragmentProgramShadow ()
{
    if (GTSupportsExtension("GL_ARB_fragment_program_shadow"))
    {
        GTExistsGlArbFragmentProgramShadow = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 37) GL_ARB_draw_buffers
//----------------------------------------------------------------------------
int GTExistsGlArbDrawBuffers = 0;

static PsglDrawBuffers sglDrawBuffersARB = 0;
//----------------------------------------------------------------------------
void GTglDrawBuffersARB (GLsizei iNumBuffers, const GLenum* aeBuffer)
{
    GT_BODY_A2(sglDrawBuffersARB,iNumBuffers,aeBuffer);
}
//----------------------------------------------------------------------------
void GTInitGlArbDrawBuffers ()
{
    if (GTSupportsExtension("GL_ARB_draw_buffers"))
    {
        GTExistsGlArbDrawBuffers = 1;
        GT_GET_FUNCTION_C(sglDrawBuffers,ARB,GT_ARB_OVERRIDE);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 38) GL_ARB_texture_rectangle
//----------------------------------------------------------------------------
int GTExistsGlArbTextureRectangle = 0;
//----------------------------------------------------------------------------
void GTInitGlArbTextureRectangle ()
{
    if (GTSupportsExtension("GL_ARB_texture_rectangle"))
    {
        GTExistsGlArbTextureRectangle = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 39) GL_ARB_color_buffer_float
//----------------------------------------------------------------------------
int GTExistsGlArbColorBufferFloat = 0;

typedef void (STDCALL* PsglClampColorARB) (GLenum, GLenum);

static PsglClampColorARB sglClampColorARB = 0;

//----------------------------------------------------------------------------
void GTglClampColorARB (GLenum eTarget, GLenum eClamp)
{
    GT_BODY_A2(sglClampColorARB,eTarget,eClamp);
}
//----------------------------------------------------------------------------
void GTInitGlArbColorBufferFloat ()
{
    if (GTSupportsExtension("GL_ARB_color_buffer_float"))
    {
        GTExistsGlArbColorBufferFloat = 1;
        GT_GET_FUNCTION(sglClampColorARB);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 40) GL_ARB_half_float_pixel
//----------------------------------------------------------------------------
int GTExistsGlArbHalfFloatPixel = 0;
//----------------------------------------------------------------------------
void GTInitGlArbHalfFloatPixel ()
{
    if (GTSupportsExtension("GL_ARB_half_float_pixel"))
    {
        GTExistsGlArbHalfFloatPixel = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 41) GL_ARB_texture_float
//----------------------------------------------------------------------------
int GTExistsGlArbTextureFloat = 0;
//----------------------------------------------------------------------------
void GTInitGlArbTextureFloat ()
{
    if (GTSupportsExtension("GL_ARB_texture_float"))
    {
        GTExistsGlArbTextureFloat = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 42) GL_ARB_pixel_buffer_object
//----------------------------------------------------------------------------
int GTExistsGlArbPixelBufferObject = 0;
//----------------------------------------------------------------------------
void GTInitGlArbPixelBufferObject ()
{
    if (GTSupportsExtension("GL_ARB_pixel_buffer_object"))
    {
        GTExistsGlArbPixelBufferObject = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (ARB 50) GL_ARB_map_buffer_range
//----------------------------------------------------------------------------
int GTExistsGlArbMapBufferRange = 0;

typedef void* (STDCALL* PsglMapBufferRange) (GLenum, GLintptr, GLsizeiptr,
    GLbitfield);
typedef void (STDCALL* PsglFlushMappedBufferRange) (GLenum, GLintptr,
    GLsizeiptr);

static PsglMapBufferRange sglMapBufferRange = 0;
static PsglFlushMappedBufferRange sglFlushMappedBufferRange = 0;

//----------------------------------------------------------------------------
GLvoid* GTglMapBufferRange (GLenum target, GLintptr offset, GLsizeiptr length,
    GLbitfield access)
{
    GT_BODY_A4_RET(sglMapBufferRange,target,offset,length,access,GLvoid*,0);
}
//----------------------------------------------------------------------------
void GTglFlushMappedBufferRange (GLenum target, GLintptr offset,
     GLsizeiptr length)
{
    GT_BODY_A3(sglFlushMappedBufferRange,target,offset,length);
}
//----------------------------------------------------------------------------
void GTInitGlArbMapBufferRange ()
{
    if (GTSupportsExtension("GL_ARB_map_buffer_range"))
    {
        GTExistsGlArbMapBufferRange = 1;
        GT_GET_FUNCTION(sglMapBufferRange);
        GT_GET_FUNCTION(sglFlushMappedBufferRange);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// ***** EXT Extensions *****
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 001) GL_EXT_abgr
//----------------------------------------------------------------------------
int GTExistsGlExtABGR = 0;
//----------------------------------------------------------------------------
void GTInitGlExtABGR ()
{
    if (GTSupportsExtension("GL_EXT_abgr"))
    {
        GTExistsGlExtABGR = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 002) GL_EXT_blend_color
//----------------------------------------------------------------------------
int GTExistsGlExtBlendColor = 0;

static PsglBlendColor sglBlendColorEXT = 0;
//----------------------------------------------------------------------------
void GTglBlendColorEXT (GLclampf fRed, GLclampf fGreen, GLclampf fBlue,
    GLclampf fAlpha)
{
    GT_BODY_A4(sglBlendColorEXT,fRed,fGreen,fBlue,fAlpha);
}
//----------------------------------------------------------------------------
void GTInitGlExtBlendColor ()
{
    if (GTSupportsExtension("GL_EXT_blend_color"))
    {
        GTExistsGlExtBlendColor = 1;
        GT_GET_FUNCTION_C(sglBlendColor,EXT,GT_EXT_OVERRIDE);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 003) GL_EXT_polygon_offset
//----------------------------------------------------------------------------
int GTExistsGlExtPolygonOffset = 0;

typedef void (STDCALL* PsglPolygonOffsetEXT) (GLfloat, GLfloat);

static PsglPolygonOffsetEXT sglPolygonOffsetEXT = 0;
//----------------------------------------------------------------------------
void GTglPolygonOffsetEXT (GLfloat fScale, GLfloat fBias)
{
    GT_BODY_A2(sglPolygonOffsetEXT,fScale,fBias);
}
//----------------------------------------------------------------------------
void GTInitGlExtPolygonOffset ()
{
    if (GTSupportsExtension("GL_EXT_polygon_offset"))
    {
        GTExistsGlExtPolygonOffset = 1;

        // This function is part of OpenGL 1.1, so it should not be
        // overridden.
        GT_GET_FUNCTION(sglPolygonOffsetEXT);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 004) GL_EXT_texture
//----------------------------------------------------------------------------
int GTExistsGlExtTexture = 0;
//----------------------------------------------------------------------------
void GTInitGlExtTexture ()
{
    if (GTSupportsExtension("GL_EXT_texture"))
    {
        GTExistsGlExtTexture = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 006) GL_EXT_texture3D
//----------------------------------------------------------------------------
int GTExistsGlExtTexture3D = 0;

static PsglTexImage3D sglTexImage3DEXT = 0;
static PsglTexSubImage3D sglTexSubImage3DEXT = 0;
//----------------------------------------------------------------------------
void GTglTexImage3DEXT (GLenum eTarget, GLint iLevel, GLint iInternalFormat,
    GLsizei iWidth, GLsizei iHeight, GLsizei iDepth, GLint iBorder,
    GLenum eFormat, GLenum eType, const GLvoid* pvData)
{
    GT_BODY_A10(sglTexImage3DEXT,eTarget,iLevel,iInternalFormat,iWidth,
        iHeight,iDepth,iBorder,eFormat,eType,pvData);
}
//----------------------------------------------------------------------------
void GTglTexSubImage3DEXT (GLenum eTarget, GLint iLevel, GLint iXOffset,
    GLint iYOffset, GLint iZOffset, GLsizei iWidth, GLsizei iHeight,
    GLsizei iDepth, GLenum eFormat, GLenum eType, const GLvoid* pvData)
{
    GT_BODY_A11(sglTexSubImage3DEXT,eTarget,iLevel,iXOffset,iYOffset,iZOffset,
        iWidth,iHeight,iDepth,eFormat,eType,pvData);
}
//----------------------------------------------------------------------------
void GTInitGlExtTexture3D ()
{
    if (GTSupportsExtension("GL_EXT_texture3D"))
    {
        GTExistsGlExtTexture3D = 1;
        GT_GET_FUNCTION_C(sglTexImage3D,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglTexSubImage3D,EXT,GT_EXT_OVERRIDE);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 009) GL_EXT_subtexture
//----------------------------------------------------------------------------
int GTExistsGlExtSubtexture = 0;

typedef void (STDCALL* PsglTexSubImage1DEXT) (GLenum, GLint, GLint, GLsizei,
    GLenum, GLenum, const GLvoid*);
typedef void (STDCALL* PsglTexSubImage2DEXT) (GLenum, GLint, GLint, GLint,
    GLsizei, GLsizei, GLenum, GLenum, const GLvoid*);

static PsglTexSubImage1DEXT sglTexSubImage1DEXT = 0;
static PsglTexSubImage2DEXT sglTexSubImage2DEXT = 0;

//----------------------------------------------------------------------------
void GTglTexSubImage1DEXT (GLenum eTarget, GLint iLevel, GLint iXOffset,
    GLsizei iWidth, GLenum eFormat, GLenum eType, const GLvoid* pvData)
{
    GT_BODY_A7(sglTexSubImage1DEXT,eTarget,iLevel,iXOffset,iWidth,eFormat,
        eType,pvData);
}
//----------------------------------------------------------------------------
void GTglTexSubImage2DEXT (GLenum eTarget, GLint iLevel, GLint iXOffset,
    GLint iYOffset, GLsizei iWidth, GLsizei iHeight, GLenum eFormat,
    GLenum eType, const GLvoid* pvData)
{
    GT_BODY_A9(sglTexSubImage2DEXT,eTarget,iLevel,iXOffset,iYOffset,iWidth,
        iHeight,eFormat,eType,pvData);
}
//----------------------------------------------------------------------------
void GTInitGlExtSubtexture ()
{
    if (GTSupportsExtension("GL_EXT_subtexture"))
    {
        GTExistsGlExtSubtexture = 1;

        // These functions are part of OpenGL 1.1, so they should not be
        // overridden.
        GT_GET_FUNCTION(sglTexSubImage1DEXT);
        GT_GET_FUNCTION(sglTexSubImage2DEXT);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 010) GL_EXT_copy_texture
//----------------------------------------------------------------------------
int GTExistsGlExtCopyTexture = 0;

typedef void (STDCALL* PsglCopyTexImage1DEXT) (GLenum, GLint, GLenum, GLint,
    GLint, GLsizei, GLint);
typedef void (STDCALL* PsglCopyTexImage2DEXT) (GLenum, GLint, GLenum, GLint,
    GLint, GLsizei, GLsizei, GLint);
typedef void (STDCALL* PsglCopyTexSubImage1DEXT) (GLenum, GLint, GLint, GLint,
    GLint, GLsizei);
typedef void (STDCALL* PsglCopyTexSubImage2DEXT) (GLenum, GLint, GLint, GLint,
    GLint, GLint, GLsizei, GLsizei);

static PsglCopyTexImage1DEXT sglCopyTexImage1DEXT = 0;
static PsglCopyTexImage2DEXT sglCopyTexImage2DEXT = 0;
static PsglCopyTexSubImage1DEXT sglCopyTexSubImage1DEXT = 0;
static PsglCopyTexSubImage2DEXT sglCopyTexSubImage2DEXT = 0;
static PsglCopyTexSubImage3D sglCopyTexSubImage3DEXT = 0;
//----------------------------------------------------------------------------
void GTglCopyTexImage1DEXT (GLenum eTarget, GLint iLevel,
    GLenum eInternalFormat, GLint iX, GLint iY, GLsizei iWidth, GLint iBorder)
{
    GT_BODY_A7(sglCopyTexImage1DEXT,eTarget,iLevel,eInternalFormat,iX,iY,
        iWidth,iBorder);
}
//----------------------------------------------------------------------------
void GTglCopyTexImage2DEXT (GLenum eTarget, GLint iLevel,
    GLenum eInternalFormat, GLint iX, GLint iY, GLsizei iWidth,
    GLsizei iHeight, GLint iBorder)
{
    GT_BODY_A8(sglCopyTexImage2DEXT,eTarget,iLevel,eInternalFormat,iX,iY,
        iWidth,iHeight,iBorder);
}
//----------------------------------------------------------------------------
void GTglCopyTexSubImage1DEXT (GLenum eTarget, GLint iLevel, GLint iXOffset,
    GLint iX, GLint iY, GLsizei iWidth)
{
    GT_BODY_A6(sglCopyTexSubImage1DEXT,eTarget,iLevel,iXOffset,iX,iY,iWidth);
}
//----------------------------------------------------------------------------
void GTglCopyTexSubImage2DEXT (GLenum eTarget, GLint iLevel, GLint iXOffset,
    GLint iYOffset, GLint iX, GLint iY, GLsizei iWidth, GLsizei iHeight)
{
    GT_BODY_A8(sglCopyTexSubImage2DEXT,eTarget,iLevel,iXOffset,iYOffset,iX,iY,
        iWidth,iHeight);
}
//----------------------------------------------------------------------------
void GTglCopyTexSubImage3DEXT (GLenum eTarget, GLint iLevel, GLint iXOffset,
    GLint iYOffset, GLint iZOffset, GLint iX, GLint iY, GLsizei iWidth,
    GLsizei iHeight)
{
    GT_BODY_A9(sglCopyTexSubImage3DEXT,eTarget,iLevel,iXOffset,iYOffset,
        iZOffset,iX,iY,iWidth,iHeight);
}
//----------------------------------------------------------------------------
void GTInitGlExtCopyTexture ()
{
    if (GTSupportsExtension("GL_EXT_copy_texture"))
    {
        GTExistsGlExtCopyTexture = 1;

        // These functions are part of OpenGL 1.1, so they should not be
        // overridden.
        GT_GET_FUNCTION(sglCopyTexImage1DEXT);
        GT_GET_FUNCTION(sglCopyTexImage2DEXT);
        GT_GET_FUNCTION(sglCopyTexSubImage1DEXT);
        GT_GET_FUNCTION(sglCopyTexSubImage2DEXT);

        GT_GET_FUNCTION_C(sglCopyTexSubImage3D,EXT,GT_EXT_OVERRIDE);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 011) GL_EXT_histogram
//----------------------------------------------------------------------------
int GTExistsGlExtHistogram = 0;

static PsglGetHistogram sglGetHistogramEXT = 0;
static PsglGetHistogramParameterfv sglGetHistogramParameterfvEXT = 0;
static PsglGetHistogramParameteriv sglGetHistogramParameterivEXT = 0;
static PsglGetMinmax sglGetMinmaxEXT = 0;
static PsglGetMinmaxParameterfv sglGetMinmaxParameterfvEXT = 0;
static PsglGetMinmaxParameteriv sglGetMinmaxParameterivEXT = 0;
static PsglHistogram sglHistogramEXT = 0;
static PsglMinmax sglMinmaxEXT = 0;
static PsglResetHistogram sglResetHistogramEXT = 0;
static PsglResetMinmax sglResetMinmaxEXT = 0;
//----------------------------------------------------------------------------
void GTglGetHistogramEXT (GLenum eTarget, GLboolean ucReset, GLenum eFormat,
    GLenum eType, GLvoid* pvValues)
{
    GT_BODY_A5(sglGetHistogramEXT,eTarget,ucReset,eFormat,eType,pvValues);
}
//----------------------------------------------------------------------------
void GTglGetHistogramParameterfvEXT (GLenum eTarget, GLenum ePName,
    GLfloat* afParam)
{
    GT_BODY_A3(sglGetHistogramParameterfvEXT,eTarget,ePName,afParam);
}
//----------------------------------------------------------------------------
void GTglGetHistogramParameterivEXT (GLenum eTarget, GLenum ePName,
    GLint* aiParam)
{
    GT_BODY_A3(sglGetHistogramParameterivEXT,eTarget,ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTglGetMinmaxEXT (GLenum eTarget, GLboolean ucReset, GLenum eFormat,
    GLenum eType, GLvoid* pvValues)
{
    GT_BODY_A5(sglGetMinmaxEXT,eTarget,ucReset,eFormat,eType,pvValues);
}
//----------------------------------------------------------------------------
void GTglGetMinmaxParameterfvEXT (GLenum eTarget, GLenum ePName,
    GLfloat* afParam)
{
    GT_BODY_A3(sglGetMinmaxParameterfvEXT,eTarget,ePName,afParam);
}
//----------------------------------------------------------------------------
void GTglGetMinmaxParameterivEXT (GLenum eTarget, GLenum ePName,
    GLint* aiParam)
{
    GT_BODY_A3(sglGetMinmaxParameterivEXT,eTarget,ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTglHistogramEXT (GLenum eTarget, GLsizei iWidth, GLenum eInternalFormat,
    GLboolean ucSink)
{
    GT_BODY_A4(sglHistogramEXT,eTarget,iWidth,eInternalFormat,ucSink);
}
//----------------------------------------------------------------------------
void GTglMinmaxEXT (GLenum eTarget, GLenum eInternalFormat, GLboolean ucSink)
{
    GT_BODY_A3(sglMinmaxEXT,eTarget,eInternalFormat,ucSink);
}
//----------------------------------------------------------------------------
void GTglResetHistogramEXT (GLenum eTarget)
{
    GT_BODY_A1(sglResetHistogramEXT,eTarget);
}
//----------------------------------------------------------------------------
void GTglResetMinmaxEXT (GLenum eTarget)
{
    GT_BODY_A1(sglResetMinmaxEXT,eTarget);
}
//----------------------------------------------------------------------------
void GTInitGlExtHistogram ()
{
    if (GTSupportsExtension("GL_EXT_histogram"))
    {
        GTExistsGlExtHistogram = 1;
        GT_GET_FUNCTION_C(sglGetHistogram,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglGetHistogramParameterfv,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglGetHistogramParameteriv,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglGetMinmax,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglGetMinmaxParameterfv,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglGetMinmaxParameteriv,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglHistogram,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglMinmax,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglResetHistogram,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglResetMinmax,EXT,GT_EXT_OVERRIDE);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 012) GL_EXT_convolution
//----------------------------------------------------------------------------
int GTExistsGlExtConvolution = 0;

static PsglConvolutionFilter1D sglConvolutionFilter1DEXT = 0;
static PsglConvolutionFilter2D sglConvolutionFilter2DEXT = 0;
static PsglConvolutionParameterf sglConvolutionParameterfEXT = 0;
static PsglConvolutionParameterfv sglConvolutionParameterfvEXT = 0;
static PsglConvolutionParameteri sglConvolutionParameteriEXT = 0;
static PsglConvolutionParameteriv sglConvolutionParameterivEXT = 0;
static PsglCopyConvolutionFilter1D sglCopyConvolutionFilter1DEXT = 0;
static PsglCopyConvolutionFilter2D sglCopyConvolutionFilter2DEXT = 0;
static PsglGetConvolutionFilter sglGetConvolutionFilterEXT = 0;
static PsglGetConvolutionParameterfv sglGetConvolutionParameterfvEXT = 0;
static PsglGetConvolutionParameteriv sglGetConvolutionParameterivEXT = 0;
static PsglGetSeparableFilter sglGetSeparableFilterEXT = 0;
static PsglSeparableFilter2D sglSeparableFilter2DEXT = 0;
//----------------------------------------------------------------------------
void GTglConvolutionFilter1DEXT (GLenum eTarget, GLenum eInternalFormat,
    GLsizei iWidth, GLenum eFormat, GLenum eType, const GLvoid* pvImage)
{
    GT_BODY_A6(sglConvolutionFilter1DEXT,eTarget,eInternalFormat,iWidth,
        eFormat,eType,pvImage);
}
//----------------------------------------------------------------------------
void GTglConvolutionFilter2DEXT (GLenum eTarget, GLenum eInternalFormat,
    GLsizei iWidth, GLsizei iHeight, GLenum eFormat, GLenum eType,
    const GLvoid* pvImage)
{
    GT_BODY_A7(sglConvolutionFilter2DEXT,eTarget,eInternalFormat,iWidth,
        iHeight,eFormat,eType,pvImage);
}
//----------------------------------------------------------------------------
void GTglConvolutionParameterfEXT (GLenum eTarget, GLenum ePName,
    GLfloat fParam)
{
    GT_BODY_A3(sglConvolutionParameterfEXT,eTarget,ePName,fParam);
}
//----------------------------------------------------------------------------
void GTglConvolutionParameterfvEXT (GLenum eTarget, GLenum ePName,
    const GLfloat* afParam)
{
    GT_BODY_A3(sglConvolutionParameterfvEXT,eTarget,ePName,afParam);
}
//----------------------------------------------------------------------------
void GTglConvolutionParameteriEXT (GLenum eTarget, GLenum ePName,
    GLint iParam)
{
    GT_BODY_A3(sglConvolutionParameteriEXT,eTarget,ePName,iParam);
}
//----------------------------------------------------------------------------
void GTglConvolutionParameterivEXT (GLenum eTarget, GLenum ePName,
    const GLint* aiParam)
{
    GT_BODY_A3(sglConvolutionParameterivEXT,eTarget,ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTglCopyConvolutionFilter1DEXT (GLenum eTarget, GLenum eInternalFormat,
    GLint iX, GLint iY, GLsizei iWidth)
{
    GT_BODY_A5(sglCopyConvolutionFilter1DEXT,eTarget,eInternalFormat,iX,iY,
        iWidth);
}
//----------------------------------------------------------------------------
void GTglCopyConvolutionFilter2DEXT (GLenum eTarget, GLenum eInternalFormat,
    GLint iX, GLint iY, GLsizei iWidth, GLsizei iHeight)
{
    GT_BODY_A6(sglCopyConvolutionFilter2DEXT,eTarget,eInternalFormat,iX,iY,
        iWidth,iHeight);
}
//----------------------------------------------------------------------------
void GTglGetConvolutionFilterEXT (GLenum eTarget, GLenum eFormat,
    GLenum eType, GLvoid* pvImage)
{
    GT_BODY_A4(sglGetConvolutionFilterEXT,eTarget,eFormat,eType,pvImage);
}
//----------------------------------------------------------------------------
void GTglGetConvolutionParameterfvEXT (GLenum eTarget, GLenum ePName,
    GLfloat* afParam)
{
    GT_BODY_A3(sglGetConvolutionParameterfvEXT,eTarget,ePName,afParam);
}
//----------------------------------------------------------------------------
void GTglGetConvolutionParameterivEXT (GLenum eTarget, GLenum ePName,
    GLint* aiParam)
{
    GT_BODY_A3(sglGetConvolutionParameterivEXT,eTarget,ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTglGetSeparableFilterEXT (GLenum eTarget, GLenum eFormat, GLenum eType,
    GLvoid* pvRow, GLvoid* pvColumn, GLvoid* pvSpan)
{
    GT_BODY_A6(sglGetSeparableFilterEXT,eTarget,eFormat,eType,pvRow,pvColumn,
        pvSpan);
}
//----------------------------------------------------------------------------
void GTglSeparableFilter2DEXT (GLenum eTarget, GLenum eInternalFormat,
    GLsizei iWidth, GLsizei iHeight, GLenum eFormat, GLenum eType,
    const GLvoid* pvRow, const GLvoid* pvColumn)
{
    GT_BODY_A8(sglSeparableFilter2DEXT,eTarget,eInternalFormat,iWidth,iHeight,
        eFormat,eType,pvRow,pvColumn);
}
//----------------------------------------------------------------------------
void GTInitGlExtConvolution ()
{
    if (GTSupportsExtension("GL_EXT_convolution"))
    {
        GTExistsGlExtConvolution = 1;
        GT_GET_FUNCTION_C(sglConvolutionFilter1D,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglConvolutionFilter2D,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglConvolutionParameterf,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglConvolutionParameterfv,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglConvolutionParameteri,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglConvolutionParameteriv,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglCopyConvolutionFilter1D,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglCopyConvolutionFilter2D,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglGetConvolutionFilter,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglGetConvolutionParameterfv,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglGetConvolutionParameteriv,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglGetSeparableFilter,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglSeparableFilter2D,EXT,GT_EXT_OVERRIDE);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 018) GL_EXT_cmyka
//----------------------------------------------------------------------------
int GTExistsGlExtCMYKA = 0;
//----------------------------------------------------------------------------
void GTInitGlExtCMYKA ()
{
    if (GTSupportsExtension("GL_EXT_cmyka"))
    {
        GTExistsGlExtCMYKA = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 020) GL_EXT_texture3D
//----------------------------------------------------------------------------
int GTExistsGlExtTextureObject = 0;

typedef GLboolean (STDCALL* PsglAreTexturesResidentEXT) (GLsizei,
    const GLuint*, GLboolean*);
typedef void (STDCALL* PsglBindTextureEXT) (GLenum, GLuint);
typedef void (STDCALL* PsglDeleteTexturesEXT) (GLsizei, const GLuint*);
typedef void (STDCALL* PsglGenTexturesEXT) (GLsizei, GLuint*);
typedef GLboolean (STDCALL* PsglIsTextureEXT) (GLuint);
typedef void (STDCALL* PsglPrioritizeTexturesEXT) (GLsizei, const GLuint*,
    const GLclampf*);

static PsglAreTexturesResidentEXT sglAreTexturesResidentEXT = 0;
static PsglBindTextureEXT sglBindTextureEXT = 0;
static PsglDeleteTexturesEXT sglDeleteTexturesEXT = 0;
static PsglGenTexturesEXT sglGenTexturesEXT = 0;
static PsglIsTextureEXT sglIsTextureEXT = 0;
static PsglPrioritizeTexturesEXT sglPrioritizeTexturesEXT = 0;

//----------------------------------------------------------------------------
GLboolean GTglAreTexturesResidentEXT (GLsizei iNumTextures,
    const GLuint* auiTextures, GLboolean* aucResidences)
{
    GT_BODY_A3_RET(sglAreTexturesResidentEXT,iNumTextures,auiTextures,
        aucResidences,GLboolean,0);
}
//----------------------------------------------------------------------------
void GTglBindTextureEXT (GLenum eTarget, GLuint uiTexture)
{
    GT_BODY_A2(sglBindTextureEXT,eTarget,uiTexture);
}
//----------------------------------------------------------------------------
void GTglDeleteTexturesEXT (GLsizei iNumTextures, const GLuint* auiTextures)
{
    GT_BODY_A2(sglDeleteTexturesEXT,iNumTextures,auiTextures);
}
//----------------------------------------------------------------------------
void GTglGenTexturesEXT (GLsizei iNumTextures, GLuint* auiTexture)
{
    GT_BODY_A2(sglGenTexturesEXT,iNumTextures,auiTexture);
}
//----------------------------------------------------------------------------
GLboolean GTglIsTextureEXT (GLuint uiTexture)
{
    GT_BODY_A1_RET(sglIsTextureEXT,uiTexture,GLboolean,0);
}
//----------------------------------------------------------------------------
void GTglPrioritizeTexturesEXT (GLsizei iNumTextures,
    const GLuint* auiTextures, const GLclampf* afPriorities)
{
    GT_BODY_A3(sglPrioritizeTexturesEXT,iNumTextures,auiTextures,
        afPriorities);
}
//----------------------------------------------------------------------------
void GTInitGlExtTextureObject ()
{
    if (GTSupportsExtension("GL_EXT_texture3D"))
    {
        GTExistsGlExtTextureObject = 1;

        // These functions are part of OpenGL 1.1, so they should not be
        // overridden.
        GT_GET_FUNCTION(sglAreTexturesResidentEXT);
        GT_GET_FUNCTION(sglBindTextureEXT);
        GT_GET_FUNCTION(sglDeleteTexturesEXT);
        GT_GET_FUNCTION(sglGenTexturesEXT);
        GT_GET_FUNCTION(sglIsTextureEXT);
        GT_GET_FUNCTION(sglPrioritizeTexturesEXT);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 023) GL_EXT_packed_pixels
//----------------------------------------------------------------------------
int GTExistsGlExtPackedPixels = 0;
//----------------------------------------------------------------------------
void GTInitGlExtPackedPixels ()
{
    if (GTSupportsExtension("GL_EXT_packed_pixels"))
    {
        GTExistsGlExtPackedPixels = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 027) GL_EXT_rescale_normal
//----------------------------------------------------------------------------
int GTExistsGlExtRescaleNormal = 0;
//----------------------------------------------------------------------------
void GTInitGlExtRescaleNormal ()
{
    if (GTSupportsExtension("GL_EXT_rescale_normal"))
    {
        GTExistsGlExtRescaleNormal = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 030) GL_EXT_vertex_array
//----------------------------------------------------------------------------
int GTExistsGlExtVertexArray = 0;

typedef void (STDCALL* PsglArrayElementEXT) (GLint);
typedef void (STDCALL* PsglDrawArraysEXT) (GLenum, GLint, GLsizei);
typedef void (STDCALL* PsglGetPointervEXT) (GLenum, GLvoid**);
typedef void (STDCALL* PsglColorPointerEXT) (GLint, GLenum, GLsizei,
    GLsizei, const GLvoid*);
typedef void (STDCALL* PsglEdgeFlagPointerEXT) (GLsizei, GLsizei,
    const GLboolean*);
typedef void (STDCALL* PsglIndexPointerEXT) (GLenum, GLsizei, GLsizei,
    const GLvoid*);
typedef void (STDCALL* PsglNormalPointerEXT) (GLenum, GLsizei, GLsizei,
    const GLvoid*);
typedef void (STDCALL* PsglTexCoordPointerEXT) (GLint, GLenum, GLsizei,
    GLsizei, const GLvoid*);
typedef void (STDCALL* PsglVertexPointerEXT) (GLint, GLenum, GLsizei,
    GLsizei, const GLvoid*);

static PsglArrayElementEXT sglArrayElementEXT = 0;
static PsglDrawArraysEXT sglDrawArraysEXT = 0;
static PsglGetPointervEXT sglGetPointervEXT = 0;
static PsglColorPointerEXT sglColorPointerEXT = 0;
static PsglEdgeFlagPointerEXT sglEdgeFlagPointerEXT = 0;
static PsglIndexPointerEXT sglIndexPointerEXT = 0;
static PsglNormalPointerEXT sglNormalPointerEXT = 0;
static PsglTexCoordPointerEXT sglTexCoordPointerEXT = 0;
static PsglVertexPointerEXT sglVertexPointerEXT = 0;
//----------------------------------------------------------------------------
void GTglArrayElementEXT (GLint i)
{
    GT_BODY_A1(sglArrayElementEXT,i);
}
//----------------------------------------------------------------------------
void GTglColorPointerEXT (GLint iSize, GLenum eType, GLsizei iStride,
    GLsizei iCount, const GLvoid* pvPointer)
{
    GT_BODY_A5(sglColorPointerEXT,iSize,eType,iStride,iCount,pvPointer);
}
//----------------------------------------------------------------------------
void GTglDrawArraysEXT (GLenum eMode, GLint iFirst, GLsizei iCount)
{
    GT_BODY_A3(sglDrawArraysEXT,eMode,iFirst,iCount);
}
//----------------------------------------------------------------------------
void GTglEdgeFlagPointerEXT (GLsizei iStride, GLsizei iCount,
    const GLboolean* pvPointer)
{
    GT_BODY_A3(sglEdgeFlagPointerEXT,iStride,iCount,pvPointer);
}
//----------------------------------------------------------------------------
void GTglGetPointervEXT (GLenum ePName, GLvoid** apvParam)
{
    GT_BODY_A2(sglGetPointervEXT,ePName,apvParam);
}
//----------------------------------------------------------------------------
void GTglIndexPointerEXT (GLenum eType, GLsizei iStride, GLsizei iCount,
    const GLvoid* pvPointer)
{
    GT_BODY_A4(sglIndexPointerEXT,eType,iStride,iCount,pvPointer);
}
//----------------------------------------------------------------------------
void GTglNormalPointerEXT (GLenum eType, GLsizei iStride, GLsizei iCount,
    const GLvoid* pvPointer)
{
    GT_BODY_A4(sglNormalPointerEXT,eType,iStride,iCount,pvPointer);
}
//----------------------------------------------------------------------------
void GTglTexCoordPointerEXT (GLint iSize, GLenum eType, GLsizei iStride,
    GLsizei iCount, const GLvoid* pvPointer)
{
    GT_BODY_A5(sglTexCoordPointerEXT,iSize,eType,iStride,iCount,pvPointer);
}
//----------------------------------------------------------------------------
void GTglVertexPointerEXT (GLint iSize, GLenum eType, GLsizei iStride,
    GLsizei iCount, const GLvoid* pvPointer)
{
    GT_BODY_A5(sglVertexPointerEXT,iSize,eType,iStride,iCount,pvPointer);
}
//----------------------------------------------------------------------------
void GTInitGlExtVertexArray ()
{
    if (GTSupportsExtension("GL_EXT_vertex_array"))
    {
        GTExistsGlExtVertexArray = 1;

        // These functions are part of OpenGL 1.1, so they should not be
        // overridden.
        GT_GET_FUNCTION(sglArrayElementEXT);
        GT_GET_FUNCTION(sglDrawArraysEXT);
        GT_GET_FUNCTION(sglGetPointervEXT);

        GT_GET_FUNCTION(sglColorPointerEXT);
        GT_GET_FUNCTION(sglEdgeFlagPointerEXT);
        GT_GET_FUNCTION(sglIndexPointerEXT);
        GT_GET_FUNCTION(sglNormalPointerEXT);
        GT_GET_FUNCTION(sglTexCoordPointerEXT);
        GT_GET_FUNCTION(sglVertexPointerEXT);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 031) GL_EXT_misc_attribute
//----------------------------------------------------------------------------
int GTExistsGlExtMiscAttribute = 0;
//----------------------------------------------------------------------------
void GTInitGlExtMiscAttribute ()
{
    if (GTSupportsExtension("GL_EXT_misc_attribute"))
    {
        GTExistsGlExtMiscAttribute = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 037) GL_EXT_blend_minmax
//----------------------------------------------------------------------------
int GTExistsGlExtBlendMinMax = 0;

static PsglBlendEquation sglBlendEquationEXT = 0;
//----------------------------------------------------------------------------
void GTglBlendEquationEXT (GLenum eMode)
{
    GT_BODY_A1(sglBlendEquationEXT,eMode);
}
//----------------------------------------------------------------------------
void GTInitGlExtBlendMinMax ()
{
    if (GTSupportsExtension("GL_EXT_blend_minmax"))
    {
        GTExistsGlExtBlendMinMax = 1;
        GT_GET_FUNCTION_C(sglBlendEquation,EXT,GT_EXT_OVERRIDE);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 038) GL_EXT_blend_subtract
//----------------------------------------------------------------------------
int GTExistsGlExtBlendSubtract = 0;
//----------------------------------------------------------------------------
void GTInitGlExtBlendSubtract ()
{
    if (GTSupportsExtension("GL_EXT_blend_subtract"))
    {
        GTExistsGlExtBlendSubtract = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 039) GL_EXT_blend_logic_op
//----------------------------------------------------------------------------
int GTExistsGlExtBlendLogicOp = 0;
//----------------------------------------------------------------------------
void GTInitGlExtBlendLogicOp ()
{
    if (GTSupportsExtension("GL_EXT_blend_logic_op"))
    {
        GTExistsGlExtBlendLogicOp = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 054) GL_EXT_point_parameters
//----------------------------------------------------------------------------
int GTExistsGlExtPointParameters = 0;

static PsglPointParameterf sglPointParameterfEXT = 0;
static PsglPointParameterfv sglPointParameterfvEXT = 0;
//----------------------------------------------------------------------------
void GTglPointParameterfEXT (GLenum ePName, GLfloat fParam)
{
    GT_BODY_A2(sglPointParameterfEXT,ePName,fParam);
}
//----------------------------------------------------------------------------
void GTglPointParameterfvEXT (GLenum ePName, const GLfloat* afParam)
{
    GT_BODY_A2(sglPointParameterfvEXT,ePName,afParam);
}
//----------------------------------------------------------------------------
void GTInitGlExtPointParameters ()
{
    if (GTSupportsExtension("GL_EXT_point_parameters"))
    {
        GTExistsGlExtPointParameters = 1;
        GT_GET_FUNCTION_C(sglPointParameterf,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglPointParameterfv,EXT,GT_EXT_OVERRIDE);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 074) GL_EXT_color_subtable
//----------------------------------------------------------------------------
int GTExistsGlExtColorSubtable = 0;

static PsglColorSubTable sglColorSubTableEXT = 0;
static PsglCopyColorSubTable sglCopyColorSubTableEXT = 0;
//----------------------------------------------------------------------------
void GTglColorSubTableEXT (GLenum eTarget, GLsizei iStart, GLsizei iCount,
    GLenum eFormat, GLenum eType, const GLvoid* pvData)
{
    GT_BODY_A6(sglColorSubTableEXT,eTarget,iStart,iCount,eFormat,eType,
        pvData);
}
//----------------------------------------------------------------------------
void GTglCopyColorSubTableEXT (GLenum eTarget, GLsizei iStart, GLint iX,
    GLint iY, GLsizei iWidth)
{
    GT_BODY_A5(sglCopyColorSubTableEXT,eTarget,iStart,iX,iY,iWidth);
}
//----------------------------------------------------------------------------
void GTInitGlExtColorSubtable ()
{
    if (GTSupportsExtension("GL_EXT_color_subtable"))
    {
        GTExistsGlExtColorSubtable = 1;
        GT_GET_FUNCTION_C(sglColorSubTable,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglCopyColorSubTable,EXT,GT_EXT_OVERRIDE);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 078) GL_EXT_paletted_texture
//----------------------------------------------------------------------------
int GTExistsGlExtPalettedTexture = 0;

static PsglColorTable sglColorTableEXT = 0;
static PsglGetColorTable sglGetColorTableEXT = 0;
static PsglGetColorTableParameterfv sglGetColorTableParameterfvEXT = 0;
static PsglGetColorTableParameteriv sglGetColorTableParameterivEXT = 0;
//----------------------------------------------------------------------------
void GTglColorTableEXT (GLenum eTarget, GLenum eInternalFormat,
    GLsizei iWidth, GLenum eFormat, GLenum eType, const GLvoid* pvTable)
{
    GT_BODY_A6(sglColorTableEXT,eTarget,eInternalFormat,iWidth,eFormat,eType,
        pvTable);
}
//----------------------------------------------------------------------------
void GTglGetColorTableEXT (GLenum eTarget, GLenum eFormat, GLenum eType,
    GLvoid* pvTable)
{
    GT_BODY_A4(sglGetColorTableEXT,eTarget,eFormat,eType,pvTable);
}
//----------------------------------------------------------------------------
void GTglGetColorTableParameterfvEXT (GLenum eTarget, GLenum ePName,
    GLfloat* afParam)
{
    GT_BODY_A3(sglGetColorTableParameterfvEXT,eTarget,ePName,afParam);
}
//----------------------------------------------------------------------------
void GTglGetColorTableParameterivEXT (GLenum eTarget, GLenum ePName,
    GLint* aiParam)
{
    GT_BODY_A3(sglGetColorTableParameterivEXT,eTarget,ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTInitGlExtPalettedTexture ()
{
    if (GTSupportsExtension("GL_EXT_paletted_texture"))
    {
        GTExistsGlExtPalettedTexture = 1;
        GT_GET_FUNCTION_C(sglColorTable,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglGetColorTable,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglGetColorTableParameterfv,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglGetColorTableParameteriv,EXT,GT_EXT_OVERRIDE);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 079) GL_EXT_clip_volume_hint
//----------------------------------------------------------------------------
int GTExistsGlExtClipVolumeHint = 0;
//----------------------------------------------------------------------------
void GTInitGlExtClipVolumeHint ()
{
    if (GTSupportsExtension("GL_EXT_clip_volume_hint"))
    {
        GTExistsGlExtClipVolumeHint = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 093) GL_EXT_index_texture
//----------------------------------------------------------------------------
int GTExistsGlExtIndexTexture = 0;
//----------------------------------------------------------------------------
void GTInitGlExtIndexTexture ()
{
    if (GTSupportsExtension("GL_EXT_index_texture"))
    {
        GTExistsGlExtIndexTexture = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 094) GL_EXT_index_material
//----------------------------------------------------------------------------
int GTExistsGlExtIndexMaterial = 0;

typedef void (STDCALL* PsglIndexMaterialEXT) (GLenum, GLenum);

static PsglIndexMaterialEXT sglIndexMaterialEXT = 0;
//----------------------------------------------------------------------------
void GTglIndexMaterialEXT (GLenum eFace, GLenum eMode)
{
    GT_BODY_A2(sglIndexMaterialEXT,eFace,eMode);
}
//----------------------------------------------------------------------------
void GTInitGlExtIndexMaterial ()
{
    if (GTSupportsExtension("GL_EXT_index_material"))
    {
        GTExistsGlExtIndexMaterial = 1;
        GT_GET_FUNCTION(sglIndexMaterialEXT);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 095) GL_EXT_index_func
//----------------------------------------------------------------------------
int GTExistsGlExtIndexFunc = 0;

typedef void (STDCALL* PsglIndexFuncEXT) (GLenum, GLclampf);

static PsglIndexFuncEXT sglIndexFuncEXT = 0;
//----------------------------------------------------------------------------
void GTglIndexFuncEXT (GLenum eFunction, GLclampf fReference)
{
    GT_BODY_A2(sglIndexFuncEXT,eFunction,fReference);
}
//----------------------------------------------------------------------------
void GTInitGlExtIndexFunc ()
{
    if (GTSupportsExtension("GL_EXT_index_func"))
    {
        GTExistsGlExtIndexFunc = 1;
        GT_GET_FUNCTION(sglIndexFuncEXT);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 096) GL_EXT_index_array_formats
//----------------------------------------------------------------------------
int GTExistsGlExtIndexArrayFormats = 0;
//----------------------------------------------------------------------------
void GTInitGlExtIndexArrayFormats ()
{
    if (GTSupportsExtension("GL_EXT_index_array_formats"))
    {
        GTExistsGlExtIndexArrayFormats = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 097) GL_EXT_compiled_vertex_array
//----------------------------------------------------------------------------
int GTExistsGlExtCompiledVertexArray = 0;

typedef void (STDCALL* PsglLockArraysEXT) (GLint, GLsizei);
typedef void (STDCALL* PsglUnlockArraysEXT) ();

static PsglLockArraysEXT sglLockArraysEXT = 0;
static PsglUnlockArraysEXT sglUnlockArraysEXT = 0;
//----------------------------------------------------------------------------
void GTglLockArraysEXT (GLint iFirst, GLsizei iCount)
{
    GT_BODY_A2(sglLockArraysEXT,iFirst,iCount);
}
//----------------------------------------------------------------------------
void GTglUnlockArraysEXT ()
{
    GT_BODY_A0(sglUnlockArraysEXT);
}
//----------------------------------------------------------------------------
void GTInitGlExtCompiledVertexArray ()
{
    if (GTSupportsExtension("GL_EXT_compiled_vertex_array"))
    {
        GTExistsGlExtCompiledVertexArray = 1;
        GT_GET_FUNCTION(sglLockArraysEXT);
        GT_GET_FUNCTION(sglUnlockArraysEXT);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 098) GL_EXT_cull_vertex
//----------------------------------------------------------------------------
int GTExistsGlExtCullVertex = 0;

typedef void (STDCALL* PsglCullParameterdvEXT) (GLenum, GLdouble*);
typedef void (STDCALL* PsglCullParameterfvEXT) (GLenum, GLfloat*);

static PsglCullParameterdvEXT sglCullParameterdvEXT = 0;
static PsglCullParameterfvEXT sglCullParameterfvEXT = 0;
//----------------------------------------------------------------------------
void GTglCullParameterdvEXT (GLenum ePName, GLdouble* adParam)
{
    GT_BODY_A2(sglCullParameterdvEXT,ePName,adParam);
}
//----------------------------------------------------------------------------
void GTglCullParameterfvEXT (GLenum ePName, GLfloat* afParam)
{
    GT_BODY_A2(sglCullParameterfvEXT,ePName,afParam);
}
//----------------------------------------------------------------------------
void GTInitGlExtCullVertex ()
{
    if (GTSupportsExtension("GL_EXT_cull_vertex"))
    {
        GTExistsGlExtCullVertex = 1;
        GT_GET_FUNCTION(sglCullParameterdvEXT);
        GT_GET_FUNCTION(sglCullParameterfvEXT);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 112) GL_EXT_draw_range_elements
//----------------------------------------------------------------------------
int GTExistsGlExtDrawRangeElements = 0;

static PsglDrawRangeElements sglDrawRangeElementsEXT = 0;
//----------------------------------------------------------------------------
void GTglDrawRangeElementsEXT (GLenum eMode, GLuint uiStart, GLuint uiEnd,
    GLsizei iCount, GLenum eType, const GLvoid* pvIndices)
{
    GT_BODY_A6(sglDrawRangeElementsEXT,eMode,uiStart,uiEnd,iCount,eType,
        pvIndices);
}
//----------------------------------------------------------------------------
void GTInitGlExtDrawRangeElements ()
{
    if (GTSupportsExtension("GL_EXT_draw_range_elements"))
    {
        GTExistsGlExtDrawRangeElements = 1;
        GT_GET_FUNCTION_C(sglDrawRangeElements,EXT,GT_EXT_OVERRIDE);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 117) GL_EXT_light_texture
//----------------------------------------------------------------------------
int GTExistsGlExtLightTexture = 0;

typedef void (STDCALL* PsglApplyTextureEXT) (GLenum);
typedef void (STDCALL* PsglTextureLightEXT) (GLenum);
typedef void (STDCALL* PsglTextureMaterialEXT) (GLenum, GLenum);

static PsglApplyTextureEXT sglApplyTextureEXT = 0;
static PsglTextureLightEXT sglTextureLightEXT = 0;
static PsglTextureMaterialEXT sglTextureMaterialEXT = 0;
//----------------------------------------------------------------------------
void GTglApplyTextureEXT (GLenum eMode)
{
    GT_BODY_A1(sglApplyTextureEXT,eMode);
}
//----------------------------------------------------------------------------
void GTglTextureLightEXT (GLenum ePName)
{
    GT_BODY_A1(sglTextureLightEXT,ePName);
}
//----------------------------------------------------------------------------
void GTglTextureMaterialEXT (GLenum eFace, GLenum eMode)
{
    GT_BODY_A2(sglTextureMaterialEXT,eFace,eMode);
}
//----------------------------------------------------------------------------
void GTInitGlExtLightTexture ()
{
    if (GTSupportsExtension("GL_EXT_light_texture"))
    {
        GTExistsGlExtLightTexture = 1;
        GT_GET_FUNCTION(sglApplyTextureEXT);
        GT_GET_FUNCTION(sglTextureLightEXT);
        GT_GET_FUNCTION(sglTextureMaterialEXT);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 129) GL_EXT_bgra
//----------------------------------------------------------------------------
int GTExistsGlExtBGRA = 0;
//----------------------------------------------------------------------------
void GTInitGlExtBGRA ()
{
    if (GTSupportsExtension("GL_EXT_bgra"))
    {
        GTExistsGlExtBGRA = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 138) GL_EXT_pixel_transform
//----------------------------------------------------------------------------
int GTExistsGlExtPixelTransform = 0;

typedef void (STDCALL* PsglPixelTransformParameteriEXT) (GLenum, GLenum,
    GLint);
typedef void (STDCALL* PsglPixelTransformParameterfEXT) (GLenum, GLenum,
    GLfloat);
typedef void (STDCALL* PsglPixelTransformParameterivEXT) (GLenum, GLenum,
    const GLint*);
typedef void (STDCALL* PsglPixelTransformParameterfvEXT) (GLenum, GLenum,
    const GLfloat*);

static PsglPixelTransformParameteriEXT sglPixelTransformParameteriEXT = 0;
static PsglPixelTransformParameterfEXT sglPixelTransformParameterfEXT = 0;
static PsglPixelTransformParameterivEXT sglPixelTransformParameterivEXT = 0;
static PsglPixelTransformParameterfvEXT sglPixelTransformParameterfvEXT = 0;
//----------------------------------------------------------------------------
void GTglPixelTransformParameteriEXT (GLenum eTarget, GLenum ePName,
    GLint iParam)
{
    GT_BODY_A3(sglPixelTransformParameteriEXT,eTarget,ePName,iParam);
}
//----------------------------------------------------------------------------
void GTglPixelTransformParameterfEXT (GLenum eTarget, GLenum ePName,
    GLfloat fParam)
{
    GT_BODY_A3(sglPixelTransformParameterfEXT,eTarget,ePName,fParam);
}
//----------------------------------------------------------------------------
void GTglPixelTransformParameterivEXT (GLenum eTarget, GLenum ePName,
    const GLint* aiParam)
{
    GT_BODY_A3(sglPixelTransformParameterivEXT,eTarget,ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTglPixelTransformParameterfvEXT (GLenum eTarget, GLenum ePName,
    const GLfloat* afParam)
{
    GT_BODY_A3(sglPixelTransformParameterfvEXT,eTarget,ePName,afParam);
}
//----------------------------------------------------------------------------
void GTInitGlExtPixelTransform ()
{
    if (GTSupportsExtension("GL_EXT_pixel_transform"))
    {
        GTExistsGlExtPixelTransform = 1;
        GT_GET_FUNCTION(sglPixelTransformParameteriEXT);
        GT_GET_FUNCTION(sglPixelTransformParameterfEXT);
        GT_GET_FUNCTION(sglPixelTransformParameterivEXT);
        GT_GET_FUNCTION(sglPixelTransformParameterfvEXT);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 139) GL_EXT_pixel_transform_color_table
//----------------------------------------------------------------------------
int GTExistsGlExtPixelTransformColorTable = 0;
//----------------------------------------------------------------------------
void GTInitGlExtPixelTransformColorTable ()
{
    if (GTSupportsExtension("GL_EXT_pixel_transform_color_table"))
    {
        GTExistsGlExtPixelTransformColorTable = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 141) GL_EXT_shared_texture_palette
//----------------------------------------------------------------------------
int GTExistsGlExtSharedTexturePalette = 0;
//----------------------------------------------------------------------------
void GTInitGlExtSharedTexturePalette ()
{
    if (GTSupportsExtension("GL_EXT_shared_texture_palette"))
    {
        GTExistsGlExtSharedTexturePalette = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 144) GL_EXT_separate_specular_color
//----------------------------------------------------------------------------
int GTExistsGlExtSeparateSpecularColor = 0;
//----------------------------------------------------------------------------
void GTInitGlExtSeparateSpecularColor ()
{
    if (GTSupportsExtension("GL_EXT_separate_specular_color"))
    {
        GTExistsGlExtSeparateSpecularColor = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 145) GL_EXT_secondary_color
//----------------------------------------------------------------------------
int GTExistsGlExtSecondaryColor = 0;

static PsglSecondaryColor3b sglSecondaryColor3bEXT = 0;
static PsglSecondaryColor3bv sglSecondaryColor3bvEXT = 0;
static PsglSecondaryColor3d sglSecondaryColor3dEXT = 0;
static PsglSecondaryColor3dv sglSecondaryColor3dvEXT = 0;
static PsglSecondaryColor3f sglSecondaryColor3fEXT = 0;
static PsglSecondaryColor3fv sglSecondaryColor3fvEXT = 0;
static PsglSecondaryColor3i sglSecondaryColor3iEXT = 0;
static PsglSecondaryColor3iv sglSecondaryColor3ivEXT = 0;
static PsglSecondaryColor3s sglSecondaryColor3sEXT = 0;
static PsglSecondaryColor3sv sglSecondaryColor3svEXT = 0;
static PsglSecondaryColor3ub sglSecondaryColor3ubEXT = 0;
static PsglSecondaryColor3ubv sglSecondaryColor3ubvEXT = 0;
static PsglSecondaryColor3ui sglSecondaryColor3uiEXT = 0;
static PsglSecondaryColor3uiv sglSecondaryColor3uivEXT = 0;
static PsglSecondaryColor3us sglSecondaryColor3usEXT = 0;
static PsglSecondaryColor3usv sglSecondaryColor3usvEXT = 0;
static PsglSecondaryColorPointer sglSecondaryColorPointerEXT = 0;
//----------------------------------------------------------------------------
void GTglSecondaryColor3bEXT (GLbyte cRed, GLbyte cGreen, GLbyte cBlue)
{
    GT_BODY_A3(sglSecondaryColor3bEXT,cRed,cGreen,cBlue);
}
//----------------------------------------------------------------------------
void GTglSecondaryColor3bvEXT (const GLbyte* acRGB)
{
    GT_BODY_A1(sglSecondaryColor3bvEXT,acRGB);
}
//----------------------------------------------------------------------------
void GTglSecondaryColor3dEXT (GLdouble dRed, GLdouble dGreen, GLdouble dBlue)
{
    GT_BODY_A3(sglSecondaryColor3dEXT,dRed,dGreen,dBlue);
}
//----------------------------------------------------------------------------
void GTglSecondaryColor3dvEXT (const GLdouble* adRGB)
{
    GT_BODY_A1(sglSecondaryColor3dvEXT,adRGB);
}
//----------------------------------------------------------------------------
void GTglSecondaryColor3fEXT (GLfloat fRed, GLfloat fGreen, GLfloat fBlue)
{
    GT_BODY_A3(sglSecondaryColor3fEXT,fRed,fGreen,fBlue);
}
//----------------------------------------------------------------------------
void GTglSecondaryColor3fvEXT (const GLfloat* afRGB)
{
    GT_BODY_A1(sglSecondaryColor3fvEXT,afRGB);
}
//----------------------------------------------------------------------------
void GTglSecondaryColor3iEXT (GLint iRed, GLint iGreen, GLint iBlue)
{
    GT_BODY_A3(sglSecondaryColor3iEXT,iRed,iGreen,iBlue);
}
//----------------------------------------------------------------------------
void GTglSecondaryColor3ivEXT (const GLint* aiRGB)
{
    GT_BODY_A1(sglSecondaryColor3ivEXT,aiRGB);
}
//----------------------------------------------------------------------------
void GTglSecondaryColor3sEXT (GLshort sRed, GLshort sGreen, GLshort sBlue)
{
    GT_BODY_A3(sglSecondaryColor3sEXT,sRed,sGreen,sBlue);
}
//----------------------------------------------------------------------------
void GTglSecondaryColor3svEXT (const GLshort* asRGB)
{
    GT_BODY_A1(sglSecondaryColor3svEXT,asRGB);
}
//----------------------------------------------------------------------------
void GTglSecondaryColor3ubEXT (GLubyte ucRed, GLubyte ucGreen, GLubyte ucBlue)
{
    GT_BODY_A3(sglSecondaryColor3ubEXT,ucRed,ucGreen,ucBlue);
}
//----------------------------------------------------------------------------
void GTglSecondaryColor3ubvEXT (const GLubyte* aucRGB)
{
    GT_BODY_A1(sglSecondaryColor3ubvEXT,aucRGB);
}
//----------------------------------------------------------------------------
void GTglSecondaryColor3uiEXT (GLuint uiRed, GLuint uiGreen, GLuint uiBlue)
{
    GT_BODY_A3(sglSecondaryColor3uiEXT,uiRed,uiGreen,uiBlue);
}
//----------------------------------------------------------------------------
void GTglSecondaryColor3uivEXT (const GLuint* auiRGB)
{
    GT_BODY_A1(sglSecondaryColor3uivEXT,auiRGB);
}
//----------------------------------------------------------------------------
void GTglSecondaryColor3usEXT (GLushort usRed, GLushort usGreen,
    GLushort usBlue)
{
    GT_BODY_A3(sglSecondaryColor3usEXT,usRed,usGreen,usBlue);
}
//----------------------------------------------------------------------------
void GTglSecondaryColor3usvEXT (const GLushort* ausRGB)
{
    GT_BODY_A1(sglSecondaryColor3usvEXT,ausRGB);
}
//----------------------------------------------------------------------------
void GTglSecondaryColorPointerEXT (GLint iSize, GLenum eType, GLsizei iStride,
    const GLvoid* pvPointer)
{
    GT_BODY_A4(sglSecondaryColorPointerEXT,iSize,eType,iStride,pvPointer);
}
//----------------------------------------------------------------------------
void GTInitGlExtSecondaryColor ()
{
    if (GTSupportsExtension("GL_EXT_secondary_color"))
    {
        GTExistsGlExtSecondaryColor = 1;
        GT_GET_FUNCTION_C(sglSecondaryColor3b,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglSecondaryColor3bv,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglSecondaryColor3d,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglSecondaryColor3dv,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglSecondaryColor3f,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglSecondaryColor3fv,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglSecondaryColor3i,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglSecondaryColor3iv,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglSecondaryColor3s,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglSecondaryColor3sv,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglSecondaryColor3ub,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglSecondaryColor3ubv,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglSecondaryColor3ui,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglSecondaryColor3uiv,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglSecondaryColor3us,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglSecondaryColor3usv,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglSecondaryColorPointer,EXT,GT_EXT_OVERRIDE);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 147) GL_EXT_texture_perturb_normal
//----------------------------------------------------------------------------
int GTExistsGlExtTexturePerturbNormal = 0;

typedef void (STDCALL* PsglTextureNormalEXT) (GLenum);

static PsglTextureNormalEXT sglTextureNormalEXT = 0;
//----------------------------------------------------------------------------
void GTglTextureNormalEXT (GLenum eMode)
{
    GT_BODY_A1(sglTextureNormalEXT,eMode);
}
//----------------------------------------------------------------------------
void GTInitGlExtTexturePerturbNormal ()
{
    if (GTSupportsExtension("GL_EXT_texture_perturb_normal"))
    {
        GTExistsGlExtTexturePerturbNormal = 1;
        GT_GET_FUNCTION(sglTextureNormalEXT);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 148) GL_EXT_multi_draw_arrays
//----------------------------------------------------------------------------
int GTExistsGlExtMultidrawArrays = 0;

static PsglMultiDrawArrays sglMultiDrawArraysEXT = 0;
static PsglMultiDrawElements sglMultiDrawElementsEXT = 0;
//----------------------------------------------------------------------------
void GTglMultiDrawArraysEXT (GLenum eMode, GLint* aiFirst, GLsizei* aiCount,
    GLsizei iPrimitiveCount)
{
    GT_BODY_A4(sglMultiDrawArraysEXT,eMode,aiFirst,aiCount,iPrimitiveCount);
}
//----------------------------------------------------------------------------
void GTglMultiDrawElementsEXT (GLenum eMode, const GLsizei* aiCount,
    GLenum eType, const GLvoid** apvIndices, GLsizei iPrimitiveCount)
{
    GT_BODY_A5(sglMultiDrawElementsEXT,eMode,aiCount,eType,apvIndices,
        iPrimitiveCount);
}
//----------------------------------------------------------------------------
void GTInitGlExtMultidrawArrays ()
{
    if (GTSupportsExtension("GL_EXT_multi_draw_arrays"))
    {
        GTExistsGlExtMultidrawArrays = 1;
        GT_GET_FUNCTION_C(sglMultiDrawArrays,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglMultiDrawElements,EXT,GT_EXT_OVERRIDE);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 149) GL_EXT_fog_coord
//----------------------------------------------------------------------------
int GTExistsGlExtFogCoord = 0;

static PsglFogCoordf sglFogCoordfEXT = 0;
static PsglFogCoordfv sglFogCoordfvEXT = 0;
static PsglFogCoordd sglFogCoorddEXT = 0;
static PsglFogCoorddv sglFogCoorddvEXT = 0;
static PsglFogCoordPointer sglFogCoordPointerEXT = 0;
//----------------------------------------------------------------------------
void GTglFogCoorddEXT (GLdouble dCoord)
{
    GT_BODY_A1(sglFogCoorddEXT,dCoord);
}
//----------------------------------------------------------------------------
void GTglFogCoorddvEXT (const GLdouble* pdCoord)
{
    GT_BODY_A1(sglFogCoorddvEXT,pdCoord);
}
//----------------------------------------------------------------------------
void GTglFogCoordfEXT (GLfloat fCoord)
{
    GT_BODY_A1(sglFogCoordfEXT,fCoord);
}
//----------------------------------------------------------------------------
void GTglFogCoordfvEXT (const GLfloat* pfCoord)
{
    GT_BODY_A1(sglFogCoordfvEXT,pfCoord);
}
//----------------------------------------------------------------------------
void GTglFogCoordPointerEXT (GLenum eType, GLsizei iStride,
    const GLvoid* pvPointer)
{
    GT_BODY_A3(sglFogCoordPointerEXT,eType,iStride,pvPointer);
}
//----------------------------------------------------------------------------
void GTInitGlExtFogCoord ()
{
    if (GTSupportsExtension("GL_EXT_fog_coord"))
    {
        GTExistsGlExtFogCoord = 1;
        GT_GET_FUNCTION_C(sglFogCoordf,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglFogCoordfv,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglFogCoordd,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglFogCoorddv,EXT,GT_EXT_OVERRIDE);
        GT_GET_FUNCTION_C(sglFogCoordPointer,EXT,GT_EXT_OVERRIDE);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 156) GL_EXT_coordinate_frame
//----------------------------------------------------------------------------
int GTExistsGlExtCoordinateFrame = 0;

typedef void (STDCALL* PsglTangent3bEXT) (GLbyte, GLbyte, GLbyte);
typedef void (STDCALL* PsglTangent3bvEXT) (const GLbyte*);
typedef void (STDCALL* PsglTangent3dEXT) (GLdouble, GLdouble, GLdouble);
typedef void (STDCALL* PsglTangent3dvEXT) (const GLdouble*);
typedef void (STDCALL* PsglTangent3fEXT) (GLfloat, GLfloat, GLfloat);
typedef void (STDCALL* PsglTangent3fvEXT) (const GLfloat*);
typedef void (STDCALL* PsglTangent3iEXT) (GLint, GLint, GLint);
typedef void (STDCALL* PsglTangent3ivEXT) (const GLint*);
typedef void (STDCALL* PsglTangent3sEXT) (GLshort, GLshort, GLshort);
typedef void (STDCALL* PsglTangent3svEXT) (const GLshort*);
typedef void (STDCALL* PsglBinormal3bEXT) (GLbyte, GLbyte, GLbyte);
typedef void (STDCALL* PsglBinormal3bvEXT) (const GLbyte*);
typedef void (STDCALL* PsglBinormal3dEXT) (GLdouble, GLdouble, GLdouble);
typedef void (STDCALL* PsglBinormal3dvEXT) (const GLdouble*);
typedef void (STDCALL* PsglBinormal3fEXT) (GLfloat, GLfloat, GLfloat);
typedef void (STDCALL* PsglBinormal3fvEXT) (const GLfloat*);
typedef void (STDCALL* PsglBinormal3iEXT) (GLint, GLint, GLint);
typedef void (STDCALL* PsglBinormal3ivEXT) (const GLint*);
typedef void (STDCALL* PsglBinormal3sEXT) (GLshort, GLshort, GLshort);
typedef void (STDCALL* PsglBinormal3svEXT) (const GLshort*);
typedef void (STDCALL* PsglTangentPointerEXT) (GLenum, GLsizei,
    const GLvoid*);
typedef void (STDCALL* PsglBinormalPointerEXT) (GLenum, GLsizei,
    const GLvoid*);

static PsglTangent3bEXT sglTangent3bEXT = 0;
static PsglTangent3bvEXT sglTangent3bvEXT = 0;
static PsglTangent3dEXT sglTangent3dEXT = 0;
static PsglTangent3dvEXT sglTangent3dvEXT = 0;
static PsglTangent3fEXT sglTangent3fEXT = 0;
static PsglTangent3fvEXT sglTangent3fvEXT = 0;
static PsglTangent3iEXT sglTangent3iEXT = 0;
static PsglTangent3ivEXT sglTangent3ivEXT = 0;
static PsglTangent3sEXT sglTangent3sEXT = 0;
static PsglTangent3svEXT sglTangent3svEXT = 0;
static PsglBinormal3bEXT sglBinormal3bEXT = 0;
static PsglBinormal3bvEXT sglBinormal3bvEXT = 0;
static PsglBinormal3dEXT sglBinormal3dEXT = 0;
static PsglBinormal3dvEXT sglBinormal3dvEXT = 0;
static PsglBinormal3fEXT sglBinormal3fEXT = 0;
static PsglBinormal3fvEXT sglBinormal3fvEXT = 0;
static PsglBinormal3iEXT sglBinormal3iEXT = 0;
static PsglBinormal3ivEXT sglBinormal3ivEXT = 0;
static PsglBinormal3sEXT sglBinormal3sEXT = 0;
static PsglBinormal3svEXT sglBinormal3svEXT = 0;
static PsglTangentPointerEXT sglTangentPointerEXT = 0;
static PsglBinormalPointerEXT sglBinormalPointerEXT = 0;
//----------------------------------------------------------------------------
void GTglTangent3bEXT (GLbyte cX, GLbyte cY, GLbyte cZ)
{
    GT_BODY_A3(sglTangent3bEXT,cX,cY,cZ);
}
//----------------------------------------------------------------------------
void GTglTangent3bvEXT (const GLbyte* acXYZ)
{
    GT_BODY_A1(sglTangent3bvEXT,acXYZ);
}
//----------------------------------------------------------------------------
void GTglTangent3dEXT (GLdouble dX, GLdouble dY, GLdouble dZ)
{
    GT_BODY_A3(sglTangent3dEXT,dX,dY,dZ);
}
//----------------------------------------------------------------------------
void GTglTangent3dvEXT (const GLdouble* adXYZ)
{
    GT_BODY_A1(sglTangent3dvEXT,adXYZ);
}
//----------------------------------------------------------------------------
void GTglTangent3fEXT (GLfloat fX, GLfloat fY, GLfloat fZ)
{
    GT_BODY_A3(sglTangent3fEXT,fX,fY,fZ);
}
//----------------------------------------------------------------------------
void GTglTangent3fvEXT (const GLfloat* afXYZ)
{
    GT_BODY_A1(sglTangent3fvEXT,afXYZ);
}
//----------------------------------------------------------------------------
void GTglTangent3iEXT (GLint iX, GLint iY, GLint iZ)
{
    GT_BODY_A3(sglTangent3iEXT,iX,iY,iZ);
}
//----------------------------------------------------------------------------
void GTglTangent3ivEXT (const GLint* aiXYZ)
{
    GT_BODY_A1(sglTangent3ivEXT,aiXYZ);
}
//----------------------------------------------------------------------------
void GTglTangent3sEXT (GLshort sX, GLshort sY, GLshort sZ)
{
    GT_BODY_A3(sglTangent3sEXT,sX,sY,sZ);
}
//----------------------------------------------------------------------------
void GTglTangent3svEXT (const GLshort* asXYZ)
{
    GT_BODY_A1(sglTangent3svEXT,asXYZ);
}
//----------------------------------------------------------------------------
void GTglBinormal3bEXT (GLbyte cX, GLbyte cY, GLbyte cZ)
{
    GT_BODY_A3(sglBinormal3bEXT,cX,cY,cZ);
}
//----------------------------------------------------------------------------
void GTglBinormal3bvEXT (const GLbyte* acXYZ)
{
    GT_BODY_A1(sglBinormal3bvEXT,acXYZ);
}
//----------------------------------------------------------------------------
void GTglBinormal3dEXT (GLdouble dX, GLdouble dY, GLdouble dZ)
{
    GT_BODY_A3(sglBinormal3dEXT,dX,dY,dZ);
}
//----------------------------------------------------------------------------
void GTglBinormal3dvEXT (const GLdouble* adXYZ)
{
    GT_BODY_A1(sglBinormal3dvEXT,adXYZ);
}
//----------------------------------------------------------------------------
void GTglBinormal3fEXT (GLfloat fX, GLfloat fY, GLfloat fZ)
{
    GT_BODY_A3(sglBinormal3fEXT,fX,fY,fZ);
}
//----------------------------------------------------------------------------
void GTglBinormal3fvEXT (const GLfloat* afXYZ)
{
    GT_BODY_A1(sglBinormal3fvEXT,afXYZ);
}
//----------------------------------------------------------------------------
void GTglBinormal3iEXT (GLint iX, GLint iY, GLint iZ)
{
    GT_BODY_A3(sglBinormal3iEXT,iX,iY,iZ);
}
//----------------------------------------------------------------------------
void GTglBinormal3ivEXT (const GLint* aiXYZ)
{
    GT_BODY_A1(sglBinormal3ivEXT,aiXYZ);
}
//----------------------------------------------------------------------------
void GTglBinormal3sEXT (GLshort sX, GLshort sY, GLshort sZ)
{
    GT_BODY_A3(sglBinormal3sEXT,sX,sY,sZ);
}
//----------------------------------------------------------------------------
void GTglBinormal3svEXT (const GLshort* asXYZ)
{
    GT_BODY_A1(sglBinormal3svEXT,asXYZ);
}
//----------------------------------------------------------------------------
void GTglTangentPointerEXT (GLenum eType, GLsizei iStride,
    const GLvoid* pvPointer)
{
    GT_BODY_A3(sglTangentPointerEXT,eType,iStride,pvPointer);
}
//----------------------------------------------------------------------------
void GTglBinormalPointerEXT (GLenum eType, GLsizei iStride,
    const GLvoid* pvPointer)
{
    GT_BODY_A3(sglBinormalPointerEXT,eType,iStride,pvPointer);
}
//----------------------------------------------------------------------------
void GTInitGlExtCoordinateFrame ()
{
    if (GTSupportsExtension("GL_EXT_coordinate_frame"))
    {
        GTExistsGlExtCoordinateFrame = 1;
        GT_GET_FUNCTION(sglTangent3bEXT);
        GT_GET_FUNCTION(sglTangent3bvEXT);
        GT_GET_FUNCTION(sglTangent3dEXT);
        GT_GET_FUNCTION(sglTangent3dvEXT);
        GT_GET_FUNCTION(sglTangent3fEXT);
        GT_GET_FUNCTION(sglTangent3fvEXT);
        GT_GET_FUNCTION(sglTangent3iEXT);
        GT_GET_FUNCTION(sglTangent3ivEXT);
        GT_GET_FUNCTION(sglTangent3sEXT);
        GT_GET_FUNCTION(sglTangent3svEXT);
        GT_GET_FUNCTION(sglBinormal3bEXT);
        GT_GET_FUNCTION(sglBinormal3bvEXT);
        GT_GET_FUNCTION(sglBinormal3dEXT);
        GT_GET_FUNCTION(sglBinormal3dvEXT);
        GT_GET_FUNCTION(sglBinormal3fEXT);
        GT_GET_FUNCTION(sglBinormal3fvEXT);
        GT_GET_FUNCTION(sglBinormal3iEXT);
        GT_GET_FUNCTION(sglBinormal3ivEXT);
        GT_GET_FUNCTION(sglBinormal3sEXT);
        GT_GET_FUNCTION(sglBinormal3svEXT);
        GT_GET_FUNCTION(sglTangentPointerEXT);
        GT_GET_FUNCTION(sglBinormalPointerEXT);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 158) GL_EXT_texture_env_combine
//----------------------------------------------------------------------------
int GTExistsGlExtTextureEnvCombine = 0;
//----------------------------------------------------------------------------
void GTInitGlExtTextureEnvCombine ()
{
    if (GTSupportsExtension("GL_EXT_texture_env_combine"))
    {
        GTExistsGlExtTextureEnvCombine = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 173) GL_EXT_blend_func_separate
//----------------------------------------------------------------------------
int GTExistsGlExtBlendFuncSeparate = 0;

static PsglBlendFuncSeparate sglBlendFuncSeparateEXT = 0;
//----------------------------------------------------------------------------
void GTglBlendFuncSeparateEXT (GLenum eSrcFactorRGB, GLenum eDstFactorRGB,
    GLenum eSrcFactorA, GLenum eDstFactorA)
{
    GT_BODY_A4(sglBlendFuncSeparateEXT,eSrcFactorRGB,eDstFactorRGB,
        eSrcFactorA,eDstFactorA);
}
//----------------------------------------------------------------------------
void GTInitGlExtBlendFuncSeparate ()
{
    if (GTSupportsExtension("GL_EXT_blend_func_separate"))
    {
        GTExistsGlExtBlendFuncSeparate = 1;
        GT_GET_FUNCTION_C(sglBlendFuncSeparate,EXT,GT_EXT_OVERRIDE);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 176) GL_EXT_stencil_wrap
//----------------------------------------------------------------------------
int GTExistsGlExtStencilWrap = 0;
//----------------------------------------------------------------------------
void GTInitGlExtStencilWrap ()
{
    if (GTSupportsExtension("GL_EXT_stencil_wrap"))
    {
        GTExistsGlExtStencilWrap = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 178) GL_EXT_422_pixels
//----------------------------------------------------------------------------
int GTExistsGlExt422Pixels = 0;
//----------------------------------------------------------------------------
void GTInitGlExt422Pixels ()
{
    if (GTSupportsExtension("GL_EXT_422_pixels"))
    {
        GTExistsGlExt422Pixels = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 185) GL_EXT_texture_env_add
//----------------------------------------------------------------------------
int GTExistsGlExtTextureEnvAdd = 0;
//----------------------------------------------------------------------------
void GTInitGlExtTextureEnvAdd ()
{
    if (GTSupportsExtension("GL_EXT_texture_env_add"))
    {
        GTExistsGlExtTextureEnvAdd = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 186) GL_EXT_texture_lod_bias
//----------------------------------------------------------------------------
int GTExistsGlExtTextureLodBias = 0;
//----------------------------------------------------------------------------
void GTInitGlExtTextureLodBias ()
{
    if (GTSupportsExtension("GL_EXT_texture_lod_bias"))
    {
        GTExistsGlExtTextureLodBias = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 187) GL_EXT_texture_filter_anisotropic
//----------------------------------------------------------------------------
int GTExistsGlExtTextureFilterAnisotropic = 0;
//----------------------------------------------------------------------------
void GTInitGlExtTextureFilterAnisotropic ()
{
    if (GTSupportsExtension("GL_EXT_texture_filter_anisotropic"))
    {
        GTExistsGlExtTextureFilterAnisotropic = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 188) GL_EXT_vertex_weighting
//----------------------------------------------------------------------------
int GTExistsGlExtVertexWeighting = 0;

typedef void (STDCALL* PsglVertexWeightfEXT) (GLfloat);
typedef void (STDCALL* PsglVertexWeightfvEXT) (const GLfloat*);
typedef void (STDCALL* PsglVertexWeightPointerEXT) (GLsizei, GLenum,
    GLsizei, const GLvoid*);

static PsglVertexWeightfEXT sglVertexWeightfEXT = 0;
static PsglVertexWeightfvEXT sglVertexWeightfvEXT = 0;
static PsglVertexWeightPointerEXT sglVertexWeightPointerEXT = 0;
//----------------------------------------------------------------------------
void GTglVertexWeightfEXT (GLfloat fWeight)
{
    GT_BODY_A1(sglVertexWeightfEXT,fWeight);
}
//----------------------------------------------------------------------------
void GTglVertexWeightfvEXT (const GLfloat* afWeight)
{
    GT_BODY_A1(sglVertexWeightfvEXT,afWeight);
}
//----------------------------------------------------------------------------
void GTglVertexWeightPointerEXT (GLsizei iSize, GLenum eType,
    GLsizei iStride, const GLvoid* pvPointer)
{
    GT_BODY_A4(sglVertexWeightPointerEXT,iSize,eType,iStride,pvPointer);
}
//----------------------------------------------------------------------------
void GTInitGlExtVertexWeighting ()
{
    if (GTSupportsExtension("GL_EXT_vertex_weighting"))
    {
        GTExistsGlExtVertexWeighting = 1;
        GT_GET_FUNCTION(sglVertexWeightfEXT);
        GT_GET_FUNCTION(sglVertexWeightfvEXT);
        GT_GET_FUNCTION(sglVertexWeightPointerEXT);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 198) GL_EXT_texture_compression_s3tc
//----------------------------------------------------------------------------
int GTExistsGlExtTextureCompressionS3TC = 0;
//----------------------------------------------------------------------------
void GTInitGlExtTextureCompressionS3TC ()
{
    if (GTSupportsExtension("GL_EXT_texture_compression_s3tc"))
    {
        GTExistsGlExtTextureCompressionS3TC = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 209) GL_EXT_multisample
//----------------------------------------------------------------------------
int GTExistsGlExtMultisample = 0;

typedef void (STDCALL* PsglSampleMaskEXT) (GLclampf, GLboolean);
typedef void (STDCALL* PsglSamplePatternEXT) (GLenum);

static PsglSampleMaskEXT sglSampleMaskEXT = 0;
static PsglSamplePatternEXT sglSamplePatternEXT = 0;
//----------------------------------------------------------------------------
void GTglSampleMaskEXT (GLclampf fValue, GLboolean ucInvert)
{
    GT_BODY_A2(sglSampleMaskEXT,fValue,ucInvert);
}
//----------------------------------------------------------------------------
void GTglSamplePatternEXT (GLenum ePattern)
{
    GT_BODY_A1(sglSamplePatternEXT,ePattern);
}
//----------------------------------------------------------------------------
void GTInitGlExtMultisample ()
{
    if (GTSupportsExtension("GL_EXT_multisample"))
    {
        GTExistsGlExtMultisample = 1;
        GT_GET_FUNCTION(sglSampleMaskEXT);
        GT_GET_FUNCTION(sglSamplePatternEXT);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 220) GL_EXT_texture_env_dot3
//----------------------------------------------------------------------------
int GTExistsGlExtTextureEnvDot3 = 0;
//----------------------------------------------------------------------------
void GTInitGlExtTextureEnvDot3 ()
{
    if (GTSupportsExtension("GL_EXT_texture_env_dot3"))
    {
        GTExistsGlExtTextureEnvDot3 = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 267) GL_EXT_shadow_funcs
//----------------------------------------------------------------------------
int GTExistsGlExtShadowFuncs = 0;
//----------------------------------------------------------------------------
void GTInitGlExtShadowFuncs ()
{
    if (GTSupportsExtension("GL_EXT_shadow_funcs"))
    {
        GTExistsGlExtShadowFuncs = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 268) GL_EXT_stencil_two_side
//----------------------------------------------------------------------------
int GTExistsGlExtStencilTwoSide = 0;

typedef void (STDCALL* PsglActiveStencilFaceEXT) (GLenum);

static PsglActiveStencilFaceEXT sglActiveStencilFaceEXT = 0;
//----------------------------------------------------------------------------
void GTglActiveStencilFaceEXT (GLenum eFace)
{
    GT_BODY_A1(sglActiveStencilFaceEXT,eFace);
}
//----------------------------------------------------------------------------
void GTInitGlExtStencilTwoSide ()
{
    if (GTSupportsExtension("GL_EXT_stencil_two_side"))
    {
        GTExistsGlExtStencilTwoSide = 1;
        GT_GET_FUNCTION(sglActiveStencilFaceEXT);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 297) GL_EXT_depth_bounds_test
//----------------------------------------------------------------------------
int GTExistsGlExtDepthBoundsTest = 0;

typedef void (STDCALL* PsglDepthBoundsEXT) (GLclampd, GLclampd);

static PsglDepthBoundsEXT sglDepthBoundsEXT = 0;
//----------------------------------------------------------------------------
void GTglDepthBoundsEXT (GLclampd dMin, GLclampd dMax)
{
    GT_BODY_A2(sglDepthBoundsEXT,dMin,dMax);
}
//----------------------------------------------------------------------------
void GTInitGlExtDepthBoundsTest ()
{
    if (GTSupportsExtension("GL_EXT_depth_bounds_test"))
    {
        GTExistsGlExtDepthBoundsTest = 1;
        GT_GET_FUNCTION(sglDepthBoundsEXT);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 298) GL_EXT_texture_mirror_clamp
//----------------------------------------------------------------------------
int GTExistsGlExtTextureMirrorClamp = 0;
//----------------------------------------------------------------------------
void GTInitGlExtTextureMirrorClamp ()
{
    if (GTSupportsExtension("GL_EXT_texture_mirror_clamp"))
    {
        GTExistsGlExtTextureMirrorClamp = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 299) GL_EXT_blend_equation_separate
//----------------------------------------------------------------------------
int GTExistsGlExtBlendEquationSeparate = 0;

static PsglBlendEquationSeparate sglBlendEquationSeparateEXT = 0;
//----------------------------------------------------------------------------
void GTglBlendEquationSeparateEXT (GLenum eModeRGB, GLenum eModeAlpha)
{
    GT_BODY_A2(sglBlendEquationSeparateEXT,eModeRGB,eModeAlpha);
}
//----------------------------------------------------------------------------
void GTInitGlExtBlendEquationSeparate ()
{
    if (GTSupportsExtension("GL_EXT_blend_equation_separate"))
    {
        GTExistsGlExtBlendEquationSeparate = 1;
        GT_GET_FUNCTION_C(sglBlendEquationSeparate,EXT,GT_EXT_OVERRIDE);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 302) GL_EXT_pixel_buffer_object
//----------------------------------------------------------------------------
int GTExistsGlExtPixelBufferObject = 0;
//----------------------------------------------------------------------------
void GTInitGlExtPixelBufferObject ()
{
    if (GTSupportsExtension("GL_EXT_pixel_buffer_object"))
    {
        GTExistsGlExtPixelBufferObject = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 309) GL_EXT_texture_compression_dxt1
//----------------------------------------------------------------------------
int GTExistsGlExtTextureCompressionDXT1 = 0;
//----------------------------------------------------------------------------
void GTInitGlExtTextureCompressionDXT1 ()
{
    if (GTSupportsExtension("GL_EXT_texture_compression_dxt1"))
    {
        GTExistsGlExtTextureCompressionDXT1 = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 310) GL_EXT_framebuffer_object
//----------------------------------------------------------------------------
int GTExistsGlExtFrameBufferObject = 0;

typedef GLboolean (STDCALL* PsglIsRenderbufferEXT) (GLuint);
typedef void (STDCALL* PsglBindRenderbufferEXT) (GLenum, GLuint);
typedef void (STDCALL* PsglDeleteRenderbuffersEXT) (GLsizei, const GLuint*);
typedef void (STDCALL* PsglGenRenderbuffersEXT) (GLsizei, GLuint*);
typedef void (STDCALL* PsglRenderbufferStorageEXT) (GLenum, GLenum,
    GLsizei, GLsizei);
typedef void (STDCALL* PsglGetRenderbufferParameterivEXT) (GLenum, GLenum,
    GLint*);
typedef GLboolean (STDCALL* PsglIsFramebufferEXT) (GLuint);
typedef void (STDCALL* PsglBindFramebufferEXT) (GLenum, GLuint);
typedef void (STDCALL* PsglDeleteFramebuffersEXT) (GLsizei, const GLuint*);
typedef void (STDCALL* PsglGenFramebuffersEXT) (GLsizei, GLuint*);
typedef GLenum (STDCALL* PsglCheckFramebufferStatusEXT) (GLenum);
typedef void (STDCALL* PsglFramebufferTexture1DEXT) (GLenum, GLenum, GLenum,
    GLuint, GLint);
typedef void (STDCALL* PsglFramebufferTexture2DEXT) (GLenum, GLenum, GLenum,
    GLuint, GLint);
typedef void (STDCALL* PsglFramebufferTexture3DEXT) (GLenum, GLenum, GLenum,
    GLuint, GLint, GLint);
typedef void (STDCALL* PsglFramebufferRenderbufferEXT) (GLenum, GLenum,
    GLenum, GLuint);
typedef void (STDCALL* PsglGetFramebufferAttachmentParameterivEXT) (GLenum,
    GLenum, GLenum, GLint*);
typedef void (STDCALL* PsglGenerateMipmapEXT) (GLenum);

static PsglIsRenderbufferEXT sglIsRenderbufferEXT = 0;
static PsglBindRenderbufferEXT sglBindRenderbufferEXT = 0;
static PsglDeleteRenderbuffersEXT sglDeleteRenderbuffersEXT = 0;
static PsglGenRenderbuffersEXT sglGenRenderbuffersEXT = 0;
static PsglRenderbufferStorageEXT sglRenderbufferStorageEXT = 0;
static PsglGetRenderbufferParameterivEXT sglGetRenderbufferParameterivEXT = 0;
static PsglIsFramebufferEXT sglIsFramebufferEXT = 0;
static PsglBindFramebufferEXT sglBindFramebufferEXT = 0;
static PsglDeleteFramebuffersEXT sglDeleteFramebuffersEXT = 0;
static PsglGenFramebuffersEXT sglGenFramebuffersEXT = 0;
static PsglCheckFramebufferStatusEXT sglCheckFramebufferStatusEXT = 0;
static PsglFramebufferTexture1DEXT sglFramebufferTexture1DEXT = 0;
static PsglFramebufferTexture2DEXT sglFramebufferTexture2DEXT = 0;
static PsglFramebufferTexture3DEXT sglFramebufferTexture3DEXT = 0;
static PsglFramebufferRenderbufferEXT sglFramebufferRenderbufferEXT = 0;
static PsglGetFramebufferAttachmentParameterivEXT
    sglGetFramebufferAttachmentParameterivEXT = 0;
static PsglGenerateMipmapEXT sglGenerateMipmapEXT = 0;
//----------------------------------------------------------------------------
GLboolean GTglIsRenderbufferEXT (GLuint uiRenderBuffer)
{
    GT_BODY_A1_RET(sglIsRenderbufferEXT,uiRenderBuffer,GLboolean,0);
}
//----------------------------------------------------------------------------
void GTglBindRenderbufferEXT (GLenum eTarget, GLuint uiRenderBuffer)
{
    GT_BODY_A2(sglBindRenderbufferEXT,eTarget,uiRenderBuffer);
}
//----------------------------------------------------------------------------
void GTglDeleteRenderbuffersEXT (GLsizei iNumBuffers,
    const GLuint* auiRenderBuffer)
{
    GT_BODY_A2(sglDeleteRenderbuffersEXT,iNumBuffers,auiRenderBuffer);
}
//----------------------------------------------------------------------------
void GTglGenRenderbuffersEXT (GLsizei iNumBuffers, GLuint* auiRenderBuffer)
{
    GT_BODY_A2(sglGenRenderbuffersEXT,iNumBuffers,auiRenderBuffer);
}
//----------------------------------------------------------------------------
void GTglRenderbufferStorageEXT (GLenum eTarget, GLenum eInternalFormat,
    GLsizei iWidth, GLsizei iHeight)
{
    GT_BODY_A4(sglRenderbufferStorageEXT,eTarget,eInternalFormat,iWidth,
        iHeight);
}
//----------------------------------------------------------------------------
void GTglGetRenderbufferParameterivEXT (GLenum eTarget, GLenum ePName,
    GLint* aiParam)
{
    GT_BODY_A3(sglGetRenderbufferParameterivEXT,eTarget,ePName,aiParam);
}
//----------------------------------------------------------------------------
GLboolean GTglIsFramebufferEXT (GLuint uiFrameBuffer)
{
    GT_BODY_A1_RET(sglIsFramebufferEXT,uiFrameBuffer,GLboolean,0);
}
//----------------------------------------------------------------------------
void GTglBindFramebufferEXT (GLenum eTarget, GLuint uiFrameBuffer)
{
    GT_BODY_A2(sglBindFramebufferEXT,eTarget,uiFrameBuffer);
}
//----------------------------------------------------------------------------
void GTglDeleteFramebuffersEXT (GLsizei iNumBuffers,
    const GLuint* auiFrameBuffer)
{
    GT_BODY_A2(sglDeleteFramebuffersEXT,iNumBuffers,auiFrameBuffer);
}
//----------------------------------------------------------------------------
void GTglGenFramebuffersEXT (GLsizei iNumBuffers, GLuint* auiFrameBuffer)
{
    GT_BODY_A2(sglGenFramebuffersEXT,iNumBuffers,auiFrameBuffer);
}
//----------------------------------------------------------------------------
GLenum GTglCheckFramebufferStatusEXT (GLenum eTarget)
{
    GT_BODY_A1_RET(sglCheckFramebufferStatusEXT,eTarget,GLenum,
        GL_FRAMEBUFFER_UNSUPPORTED_EXT);
}
//----------------------------------------------------------------------------
void GTglFramebufferTexture1DEXT (GLenum eTarget, GLenum eAttachment,
    GLenum eTextureTarget, GLuint uiTexture, GLint iLevel)
{
    GT_BODY_A5(sglFramebufferTexture1DEXT,eTarget,eAttachment,eTextureTarget,
        uiTexture,iLevel);
}
//----------------------------------------------------------------------------
void GTglFramebufferTexture2DEXT (GLenum eTarget, GLenum eAttachment,
    GLenum eTextureTarget, GLuint uiTexture, GLint iLevel)
{
    GT_BODY_A5(sglFramebufferTexture2DEXT,eTarget,eAttachment,eTextureTarget,
        uiTexture,iLevel);
}
//----------------------------------------------------------------------------
void GTglFramebufferTexture3DEXT (GLenum eTarget, GLenum eAttachment,
    GLenum eTextureTarget, GLuint uiTexture, GLint iLevel, GLint iZOffset)
{
    GT_BODY_A6(sglFramebufferTexture3DEXT,eTarget,eAttachment,eTextureTarget,
        uiTexture,iLevel,iZOffset);
}
//----------------------------------------------------------------------------
void GTglFramebufferRenderbufferEXT (GLenum eTarget, GLenum eAttachment,
    GLenum eRenderBufferTarget, GLuint uiRenderBuffer)
{
    GT_BODY_A4(sglFramebufferRenderbufferEXT,eTarget,eAttachment,
        eRenderBufferTarget,uiRenderBuffer);
}
//----------------------------------------------------------------------------
void GTglGetFramebufferAttachmentParameterivEXT (GLenum eTarget,
    GLenum eAttachment, GLenum ePName, GLint* aiParam)
{
    GT_BODY_A4(sglGetFramebufferAttachmentParameterivEXT,eTarget,eAttachment,
        ePName,aiParam);
}
//----------------------------------------------------------------------------
void GTglGenerateMipmapEXT (GLenum eTarget)
{
    GT_BODY_A1(sglGenerateMipmapEXT,eTarget);
}
//----------------------------------------------------------------------------
void GTInitGlExtFrameBufferObject ()
{
    if (GTSupportsExtension("GL_EXT_framebuffer_object"))
    {
        GTExistsGlExtFrameBufferObject = 1;
        GT_GET_FUNCTION(sglIsRenderbufferEXT);
        GT_GET_FUNCTION(sglBindRenderbufferEXT);
        GT_GET_FUNCTION(sglDeleteRenderbuffersEXT);
        GT_GET_FUNCTION(sglGenRenderbuffersEXT);
        GT_GET_FUNCTION(sglRenderbufferStorageEXT);
        GT_GET_FUNCTION(sglGetRenderbufferParameterivEXT);
        GT_GET_FUNCTION(sglIsFramebufferEXT);
        GT_GET_FUNCTION(sglBindFramebufferEXT);
        GT_GET_FUNCTION(sglDeleteFramebuffersEXT);
        GT_GET_FUNCTION(sglGenFramebuffersEXT);
        GT_GET_FUNCTION(sglCheckFramebufferStatusEXT);
        GT_GET_FUNCTION(sglFramebufferTexture1DEXT);
        GT_GET_FUNCTION(sglFramebufferTexture2DEXT);
        GT_GET_FUNCTION(sglFramebufferTexture3DEXT);
        GT_GET_FUNCTION(sglFramebufferRenderbufferEXT);
        GT_GET_FUNCTION(sglGetFramebufferAttachmentParameterivEXT);
        GT_GET_FUNCTION(sglGenerateMipmapEXT);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 312) GL_EXT_packed_depth_stencil
//----------------------------------------------------------------------------
int GTExistsGlExtPackedDepthStencil = 0;
//----------------------------------------------------------------------------
void GTInitGlExtPackedDepthStencil ()
{
    if (GTSupportsExtension("GL_EXT_packed_depth_stencil"))
    {
        GTExistsGlExtPackedDepthStencil = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 314) GL_EXT_stencil_clear_tag
//----------------------------------------------------------------------------
int GTExistsGlExtStencilClearTag = 0;

typedef void (STDCALL* PsglStencilClearTagEXT) (GLsizei, GLuint);

static PsglStencilClearTagEXT sglStencilClearTagEXT = 0;
//----------------------------------------------------------------------------
void GTglStencilClearTagEXT (GLsizei iStencilTagBits,
    GLuint uiStencilClearTag)
{
    GT_BODY_A2(sglStencilClearTagEXT,iStencilTagBits,uiStencilClearTag);
}
//----------------------------------------------------------------------------
void GTInitGlExtStencilClearTag ()
{
    if (GTSupportsExtension("GL_EXT_stencil_clear_tag"))
    {
        GTExistsGlExtStencilClearTag = 1;
        GT_GET_FUNCTION(sglStencilClearTagEXT);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 315) GL_EXT_texture_sRGB
//----------------------------------------------------------------------------
int GTExistsGlExtTextureSRGB = 0;
//----------------------------------------------------------------------------
void GTInitGlExtTextureSRGB ()
{
    if (GTSupportsExtension("GL_EXT_texture_sRGB"))
    {
        GTExistsGlExtTextureSRGB = 1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 316) GL_EXT_framebuffer_blit
//----------------------------------------------------------------------------
int GTExistsGlExtFrameBufferBlit = 0;

typedef void (STDCALL* PsglBlitFramebufferEXT) (GLint, GLint, GLint, GLint,
    GLint, GLint, GLint, GLint, GLbitfield, GLenum);

static PsglBlitFramebufferEXT sglBlitFramebufferEXT = 0;
//----------------------------------------------------------------------------
void GTglBlitFramebufferEXT (GLint iSrcX0, GLint iSrcY0, GLint iSrcX1,
    GLint iSrcY1, GLint iDstX0, GLint iDstY0, GLint iDstX1, GLint iDstY1,
    GLbitfield uiMask, GLenum eFilter)
{
    GT_BODY_A10(sglBlitFramebufferEXT,iSrcX0,iSrcY0,iSrcX1,iSrcY1,iDstX0,
        iDstY0,iDstX1,iDstY1,uiMask,eFilter);
}
//----------------------------------------------------------------------------
void GTInitGlExtFrameBufferBlit ()
{
    if (GTSupportsExtension("GL_EXT_framebuffer_blit"))
    {
        GTExistsGlExtFrameBufferBlit = 1;
        GT_GET_FUNCTION(sglBlitFramebufferEXT);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// (EXT 317) GL_EXT_framebuffer_multisample
//----------------------------------------------------------------------------
int GTExistsGlExtFrameBufferMultisample = 0;

typedef void (STDCALL* PsglRenderbufferStorageMultisampleEXT) (GLenum,
    GLsizei, GLenum, GLsizei, GLsizei);

static PsglRenderbufferStorageMultisampleEXT
    sglRenderbufferStorageMultisampleEXT = 0;

//----------------------------------------------------------------------------
void GTglRenderbufferStorageMultisampleEXT (GLenum eTarget, GLsizei iSamples,
    GLenum eInternalFormat, GLsizei iWidth, GLsizei iHeight)
{
    GT_BODY_A5(sglRenderbufferStorageMultisampleEXT,eTarget,iSamples,
        eInternalFormat,iWidth,iHeight);
}
//----------------------------------------------------------------------------
void GTInitGlExtFrameBufferMultisample ()
{
    if (GTSupportsExtension("GL_EXT_framebuffer_multisample"))
    {
        GTExistsGlExtFrameBufferMultisample = 1;
        GT_GET_FUNCTION(sglRenderbufferStorageMultisampleEXT);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Access to OpenGL driver information.
//----------------------------------------------------------------------------
#ifdef __cplusplus
#include <cstring>
#else
#include <string.h>
#endif
//----------------------------------------------------------------------------
const char* GTGetVendorString ()
{
    return (const char*)glGetString(GL_VENDOR);
}
//----------------------------------------------------------------------------
const char* GTGetRendererString ()
{
    return (const char*)glGetString(GL_RENDERER);
}
//----------------------------------------------------------------------------
const char* GTGetVersionString ()
{
    return (const char*)glGetString(GL_VERSION);
}
//----------------------------------------------------------------------------
const char* GTGetExtensionString ()
{
    return (const char*)glGetString(GL_EXTENSIONS);
}
//----------------------------------------------------------------------------
int GTSupportsExtension (const char* acExtension)
{
    const char* acExtensionString;
    char* acBegin;
    char cEnd;

    if (!acExtension)
    {
        return 0;
    }

    acExtensionString = GTGetExtensionString();
    if (!acExtensionString)
    {
        return 0;
    }

    acBegin = (char*)strstr(acExtensionString,acExtension);
    if (!acBegin)
    {
        return 0;
    }

    // The extension was found, but make sure it is not a proper substring
    // of another extension.
    cEnd = *(acBegin + strlen(acExtension));
    return cEnd == ' ' || cEnd == 0 || cEnd == '\t' || cEnd == '\n';
}
//----------------------------------------------------------------------------
int GTGetVersion ()
{
    const char* pcToken = GTGetVersionString();
    if (pcToken)
    {
        if (*pcToken == '1')
        {
            pcToken++;
            if (pcToken && *pcToken == '.')
            {
                pcToken++;
                if (pcToken)
                {
                    switch (*pcToken)
                    {
                    case '1': return GT_VERSION_1_1;
                    case '2': return GT_VERSION_1_2;
                    case '3': return GT_VERSION_1_3;
                    case '4': return GT_VERSION_1_4;
                    case '5': return GT_VERSION_1_5;
                    }
                }
            }
        }
        else if (*pcToken == '2')
        {
            pcToken++;
            if (pcToken && *pcToken == '.')
            {
                pcToken++;
                if (pcToken)
                {
                    switch (*pcToken)
                    {
                    case '0': return GT_VERSION_2_0;
                    case '1': return GT_VERSION_2_1;
                    }
                }
            }
        }
        else if (*pcToken == '3')
        {
            pcToken++;
            if (pcToken && *pcToken == '.')
            {
                pcToken++;
                if (pcToken)
                {
                    switch (*pcToken)
                    {
                    case '0': return GT_VERSION_3_0;
                    case '1': return GT_VERSION_3_1;
                    case '2': return GT_VERSION_3_2;
                    case '3': return GT_VERSION_3_3;
                    }
                }
            }
        }
        else if (*pcToken == '4')
        {
            pcToken++;
            if (pcToken && *pcToken == '.')
            {
                pcToken++;
                if (pcToken)
                {
                    switch (*pcToken)
                    {
                    case '0': return GT_VERSION_4_0;
                    }
                }
            }
        }
    }

    return GT_VERSION_NONE;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Initialization of the OpenGL system
//----------------------------------------------------------------------------
#include <cstdio>
#include <cstdlib>

// Disable the MSVC8 warnings about deprecated functions.
#pragma warning( push )
#pragma warning( disable : 4996 )

void GTInitGL (const char* acInfoFile)
{
    if (acInfoFile)
    {
        FILE* pkInfo = fopen(acInfoFile,"wt");
        if (pkInfo)
        {
            const char* acVendor;
            const char* acRenderer;
            const char* acVersion;
            const char* acExtensions;

            acVendor = GTGetVendorString();
            if (acVendor)
            {
                fprintf(pkInfo,"vendor = %s\n",acVendor);
            }
            else
            {
                fprintf(pkInfo,"vendor = <null>\n");
            }

            acRenderer = GTGetRendererString();
            if (acRenderer)
            {
                fprintf(pkInfo,"renderer = %s\n",acRenderer);
            }
            else
            {
                fprintf(pkInfo,"renderer = <null>\n");
            }

            acVersion = GTGetVersionString();
            if (acVersion)
            {
                fprintf(pkInfo,"version = %s\n",acVersion);
            }
            else
            {
                fprintf(pkInfo,"version = <null>\n");
            }

            acExtensions = GTGetExtensionString();
            if (acExtensions)
            {
                char* acExt;
                char* pcToken;

                fprintf(pkInfo,"extensions =\n");
                acExt = (char*)malloc((int)strlen(acExtensions)+1);
                strcpy(acExt,acExtensions);
                pcToken = strtok(acExt," \t\n");
                while (pcToken)
                {
                    fprintf(pkInfo,"    %s\n",pcToken);
                    pcToken = strtok(0," \t\n");
                }
                free(acExt);
            }
            else
            {
                fprintf(pkInfo,"extensions = <null>\n");
            }

            fclose(pkInfo);
        }
    }

    GTInitOpenGL12();
    GTInitGlArbImaging();
    GTInitOpenGL13();
    GTInitOpenGL14();
    GTInitOpenGL15();
    GTInitOpenGL20();
    GTInitOpenGL21();
    GTInitOpenGL30();
    GTInitOpenGL31();
    GTInitOpenGL32();
    GTInitOpenGL33();
    GTInitOpenGL40();

    GTInitGlArbMultitexture();
    GTInitGlArbTransposeMatrix();
    GTInitGlArbMultisample();
    GTInitGlArbTextureEnvAdd();
    GTInitGlArbTextureCubeMap();
    GTInitGlArbTextureCompression();
    GTInitGlArbTextureBorderClamp();
    GTInitGlArbPointParameters();
    GTInitGlArbVertexBlend();
    GTInitGlArbMatrixPalette();
    GTInitGlArbTextureEnvCombine();
    GTInitGlArbTextureEnvCrossbar();
    GTInitGlArbTextureEnvDot3();
    GTInitGlArbTextureMirroredRepeat();
    GTInitGlArbDepthTexture();
    GTInitGlArbShadow();
    GTInitGlArbShadowAmbient();
    GTInitGlArbWindowPos();
    GTInitGlArbVertexProgram();
    GTInitGlArbFragmentProgram();
    GTInitGlArbVertexBufferObject();
    GTInitGlArbOcclusionQuery();
    GTInitGlArbShaderObjects();
    GTInitGlArbVertexShader();
    GTInitGlArbFragmentShader();
    GTInitGlArbShadingLanguage100();
    GTInitGlArbTextureNonpowerOfTwo();
    GTInitGlArbPointSprite();
    GTInitGlArbFragmentProgramShadow();
    GTInitGlArbDrawBuffers();
    GTInitGlArbTextureRectangle();
    GTInitGlArbColorBufferFloat();
    GTInitGlArbHalfFloatPixel();
    GTInitGlArbTextureFloat();
    GTInitGlArbPixelBufferObject();
    GTInitGlArbMapBufferRange();

    GTInitGlExtABGR();
    GTInitGlExtBlendColor();
    GTInitGlExtPolygonOffset();
    GTInitGlExtTexture();
    GTInitGlExtTexture3D();
    GTInitGlExtSubtexture();
    GTInitGlExtCopyTexture();
    GTInitGlExtHistogram();
    GTInitGlExtConvolution();
    GTInitGlExtCMYKA();
    GTInitGlExtTextureObject();
    GTInitGlExtPackedPixels();
    GTInitGlExtRescaleNormal();
    GTInitGlExtVertexArray();
    GTInitGlExtMiscAttribute();
    GTInitGlExtBlendMinMax();
    GTInitGlExtBlendSubtract();
    GTInitGlExtBlendLogicOp();
    GTInitGlExtPointParameters();
    GTInitGlExtColorSubtable();
    GTInitGlExtPalettedTexture();
    GTInitGlExtClipVolumeHint();
    GTInitGlExtIndexTexture();
    GTInitGlExtIndexMaterial();
    GTInitGlExtIndexFunc();
    GTInitGlExtIndexArrayFormats();
    GTInitGlExtCompiledVertexArray();
    GTInitGlExtCullVertex();
    GTInitGlExtDrawRangeElements();
    GTInitGlExtLightTexture();
    GTInitGlExtBGRA();
    GTInitGlExtPixelTransform();
    GTInitGlExtPixelTransformColorTable();
    GTInitGlExtSharedTexturePalette();
    GTInitGlExtSeparateSpecularColor();
    GTInitGlExtSecondaryColor();
    GTInitGlExtTexturePerturbNormal();
    GTInitGlExtMultidrawArrays();
    GTInitGlExtFogCoord();
    GTInitGlExtCoordinateFrame();
    GTInitGlExtTextureEnvCombine();
    GTInitGlExtBlendFuncSeparate();
    GTInitGlExtStencilWrap();
    GTInitGlExt422Pixels();
    GTInitGlExtTextureEnvAdd();
    GTInitGlExtTextureLodBias();
    GTInitGlExtTextureFilterAnisotropic();
    GTInitGlExtVertexWeighting();
    GTInitGlExtTextureCompressionS3TC();
    GTInitGlExtMultisample();
    GTInitGlExtTextureEnvDot3();
    GTInitGlExtShadowFuncs();
    GTInitGlExtStencilTwoSide();
    GTInitGlExtDepthBoundsTest();
    GTInitGlExtTextureMirrorClamp();
    GTInitGlExtBlendEquationSeparate();
    GTInitGlExtPixelBufferObject();
    GTInitGlExtTextureCompressionDXT1();
    GTInitGlExtFrameBufferObject();
    GTInitGlExtPackedDepthStencil();
    GTInitGlExtStencilClearTag();
    GTInitGlExtTextureSRGB();
    GTInitGlExtFrameBufferBlit();
    GTInitGlExtFrameBufferMultisample();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Support for error checking.
//----------------------------------------------------------------------------
static const char* gsErrorString[7] =
{
    "GL_INVALID_ENUM",
    "GL_INVALID_VALUE",
    "GL_INVALID_OPERATION",
    "GL_STACK_OVERFLOW",
    "GL_STACK_UNDERFLOW",
    "GL_OUT_OF_MEMORY",
    "GL_UNKNOWN_ERROR_CODE"
};
//----------------------------------------------------------------------------
const char* GTGetErrorString (GLenum code)
{
    int index = (int)code - (int)GL_INVALID_ENUM;
    if (0 <= index && index < 6)
    {
        return gsErrorString[index];
    }
    return gsErrorString[6];
}
//----------------------------------------------------------------------------

#pragma warning( pop )
//----------------------------------------------------------------------------
