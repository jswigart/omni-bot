// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.3 (2011/08/25)

#ifndef WM5CAMERA_H
#define WM5CAMERA_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Object.h"
#include "Wm5HMatrix.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM Camera : public Object
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(Camera);

public:
    // Construction and destruction.
    Camera (bool isPerspective = true);
    virtual ~Camera ();

    // The camera frame is always in world coordinates.
    //   default position  P = (0, 0,  0; 1)
    //   default direction D = (0, 0, -1; 0)
    //   default up        U = (0, 1,  0; 0)
    //   default right     R = (1, 0,  0; 0)
    void SetFrame (const APoint& position, const AVector& dVector,
        const AVector& uVector, const AVector& rVector);

    void SetPosition (const APoint& position);

    void SetAxes (const AVector& dVector, const AVector& uVector,
        const AVector& rVector);

    inline APoint GetPosition () const;
    inline AVector GetDVector () const;
    inline AVector GetUVector () const;
    inline AVector GetRVector () const;

    // Access the view matrix of the camera.  If D = (d0,d1,d2),
    // U = (u0,u1,u2), and R = (r0,r1,r2), then the view matrix is
    //   +-                     -+
    //   | r0  r1  r2  -Dot(R,P) |
    //   | u0  u1  u2  -Dot(U,P) |
    //   | d0  d1  d2  -Dot(D,P) |
    //   |  0   0   0          1 |
    //   +-                     -+
    // The view matrix multiplies vectors on its right, viewMat*vector4.
    inline const HMatrix& GetViewMatrix () const;

    // The view frustum has parameters [rmin,rmax], [umin,umax], and
    // [dmin,dmax].  The interval [rmin,rmax] is measured in the right
    // direction R.  These are the "left" and "right" frustum values.  The
    // interval [umin,umax] is measured in the up direction U.  These are
    // the "bottom" and "top" values.  The interval [dmin,dmax] is measured
    // in the view direction D.  These are the "near" and "far" values.
    // The frustum values are stored in an array with the following mappings:
    enum
    {
        VF_DMIN     = 0,  // near
        VF_DMAX     = 1,  // far
        VF_UMIN     = 2,  // bottom
        VF_UMAX     = 3,  // top
        VF_RMIN     = 4,  // left
        VF_RMAX     = 5,  // right
        VF_QUANTITY = 6
    };

    // The default view frustum has an up field-of-view of 90 degrees, an
    // aspect ratio of 1, near value 1, and far value 10000.

    inline bool IsPerspective () const;

    // Set the view frustum.  The interval [rmin,rmax] is measured in the
    // right direction R.  These are the "left" and "right" frustum values.
    // The interval [umin,umax] is measured in the up direction U.  These are
    // the "bottom" and "top" values.  The interval [dmin,dmax] is measured
    // in the view direction D.  These are the "near" and "far" values.
    void SetFrustum (float dMin, float dMax, float uMin, float uMax,
        float rMin, float rMax);

    // Set all the view frustum values simultaneously.  The input array must
    // have the order:  dmin, dmax, umin, umax, rmin, rmax.
    void SetFrustum (const float* frustum);

    // Set a symmetric view frustum (umin = -umax, rmin = -rmax) using a field
    // of view in the "up" direction and an aspect ratio "width/height".  This
    // call is the equivalent of gluPerspective in OpenGL.  As such, the field
    // of view in this function must be specified in degrees and be in the
    // interval (0,180).
    void SetFrustum (float upFovDegrees, float aspectRatio, float dMin,
        float dMax);

    // Get the view frustum.
    void GetFrustum (float& dMin, float& dMax, float& uMin, float& uMax,
        float& rMin, float& rMax) const;

    // Get all the view frustum values simultaneously.
    inline const float* GetFrustum () const;

    // Get the parameters for a symmetric view frustum.  The return value is
    // 'true' iff the current frustum is symmetric, in which case the output
    // parameters are valid.
    bool GetFrustum (float& upFovDegrees, float& aspectRatio, float& dMin,
        float& dMax) const;

    // Get the individual frustum values.
    inline float GetDMin () const;
    inline float GetDMax () const;
    inline float GetUMin () const;
    inline float GetUMax () const;
    inline float GetRMin () const;
    inline float GetRMax () const;

    // Access the projection matrices of the camera.  The projection matrix
    // that maps to depths [0,1] is what Direct3D uses.  The view matrix that
    // maps to depths [-1,1] is what OpenGL uses.
    enum WM5_GRAPHICS_ITEM DepthType
    {
        PM_DEPTH_ZERO_TO_ONE,       // [0,1]
        PM_DEPTH_MINUS_ONE_TO_ONE,  // [-1,1]
        PM_DEPTH_QUANTITY
    };

    inline DepthType GetDepthType () const;

    // The frustum values are N (near), F (far), B (bottom), T (top),
    // L (left), and R (right).  The various matrices are as follows.
    //
    // perspective, depth [0,1]
    //   +-                                               -+
    //   | 2*N/(R-L)  0           -(R+L)/(R-L)  0          |
    //   | 0          2*N/(T-B)   -(T+B)/(T-B)  0          |
    //   | 0          0           F/(F-N)       -N*F/(F-N) |
    //   | 0          0           1             0          |
    //   +-                                               -+
    //
    // perspective, depth [-1,1]
    //   +-                                                 -+
    //   | 2*N/(R-L)  0           -(R+L)/(R-L)  0            |
    //   | 0          2*N/(T-B)   -(T+B)/(T-B)  0            |
    //   | 0          0           (F+N)/(F-N)   -2*F*N/(F-N) |
    //   | 0          0           1             0
    //   +-                                                 -+
    //
    // orthographic, depth [0,1]
    //   +-                                       -+
    //   | 2/(R-L)  0  0              -(R+L)/(R-L) |
    //   | 0        2/(T-B)  0        -(T+B)/(T-B) |
    //   | 0        0        1/(F-N)  -N/(F-N)  0  |
    //   | 0        0        0        1            |
    //   +-                                       -+
    //
    // orthographic, depth [-1,1]
    //   +-                                       -+
    //   | 2/(R-L)  0        0        -(R+L)/(R-L) |
    //   | 0        2/(T-B)  0        -(T+B)/(T-B) |
    //   | 0        0        2/(F-N)  -(F+N)/(F-N) |
    //   | 0        0        0        1            |
    //   +-                                       -+
    //
    // The projection matrix multiplies vectors on its right, projMat*vector4.

    // The returned matrix depends on the values of msDepthType and
    // mIsPerspective.
    inline const HMatrix& GetProjectionMatrix () const;

    // Support for advanced effects.  The first function allows you to set the
    // projection matrix anyway you like.  The second function specifies a
    // convex quadrilateral viewport.  The points must be in camera
    // coordinates and are counterclockwise ordered as viewed from the
    // eyepoint.  The plane of the quadrilateral is the view plane and has an
    // "extrude" value of 1.  The nearExtrude value is in (0,infinity); this
    // specifies the fraction from the eyepoint to the view plane at which to
    // place the near-face of the cuboidal view volume.  The farExtrude value
    // is in (nearExtrude,infinity); this specifies the fraction from the
    // eyepoint at which to place the far-face of the cuboidal view volume.
    inline void SetProjectionMatrix (const HMatrix& projMatrix);
    void SetProjectionMatrix (const APoint& p00, const APoint& p10,
        const APoint& p11, const APoint& p01, float nearExtrude,
        float farExtrude);

    // The projection-view-world matrix is commonly used in the shader
    // programs to transform model-space data to clip-space data.  To avoid
    // repeatedly computing the projection-view matrix for each geometric
    // object, the product is stored and maintained in this class.
    inline const HMatrix& GetProjectionViewMatrix () const;

    // The preview matrix is applied after the model-to-world but before the
    // view matrix.  It is used for transformations such as reflections of
    // world objects.  The default value is the identity matrix.
    void SetPreViewMatrix (const HMatrix& preViewMatrix);
    inline const HMatrix& GetPreViewMatrix () const;
    inline bool PreViewIsIdentity () const;

    // The postprojection matrix is used for screen-space transformations such
    // as reflection of the rendered image.  The default value is the identity
    // matrix.
    void SetPostProjectionMatrix (const HMatrix& postProjMatrix);
    inline const HMatrix& GetPostProjectionMatrix () const;
    inline bool PostProjectionIsIdentity () const;

    // Compute the axis-aligned bounding rectangle in normalized display
    // space, [-1,1]x[-1,1], for a set of vertices.  The input 'vertices' is a
    // pointer to triples of 'float' values, each triple representing a vertex.
    // The stride is the number of bytes from the first position channel of one
    // vertex to the first position channel of the next vertex.  If the
    // vertices are packed contiguously, the stride is 3*sizeof(float).  The
    // vertices are in model space.  The input 'worldMatrix' transforms the
    // model space to world space.
    void ComputeBoundingAABB (int numVertices, const char* vertices,
        int stride, const HMatrix& worldMatrix, float& xmin, float& xmax,
        float& ymin, float& ymax);

protected:
    // Compute the view matrix after the frame changes.
    void OnFrameChange ();

    // Compute the projection matrices after the frustum changes.
    void OnFrustumChange ();

    // Compute the product postproj-proj-view-preview.
    void UpdatePVMatrix ();

    // The world coordinate frame.
    APoint mPosition;
    AVector mDVector, mUVector, mRVector;

    // The view matrix for the camera.  The matrix is stored so that you
    // apply it to vectors on the right:  viewMatrix*someVector4.
    HMatrix mViewMatrix;

    // The view frustum, stored in order as dmin (near), dmax (far),
    // umin (bottom), umax (top), rmin (left), and rmax (right).
    float mFrustum[VF_QUANTITY];

    // The projection matrices for the camera.  The matrices are stored so
    // that you apply it to vectors on the right:  projMatrix*someVector4.
    HMatrix mProjectionMatrix[PM_DEPTH_QUANTITY];

    // The product of the projection and view matrix.  This includes the
    // post-projection and/or pre-view whenever those are not the identity
    // matrix.
    HMatrix mProjectionViewMatrix[PM_DEPTH_QUANTITY];

    // The preview matrix for the camera.
    HMatrix mPreViewMatrix;
    bool mPreViewIsIdentity;

    // The postprojection matrix for the camera.
    HMatrix mPostProjectionMatrix;
    bool mPostProjectionIsIdentity;

    // This member is 'true' for a perspective camera or 'false' for an
    // orthographic camera.
    bool mIsPerspective;

    // This object's depth type.  The value is initialized to
    // msDefaultDepthType during the constructor call.  Camera does not
    // stream this value.  The Projector derived class overrides the depth
    // type and does stream it, because you might want a projector of a
    // type that does not match the graphics API type.
    DepthType mDepthType;

    // Specify the type of projection matrix (different between DirectX and
    // OpenGL).  This value is used to initialize mDepthType during the
    // constructor call.
    static DepthType msDefaultDepthType;

public_internal:
    // Set this to one of the previously mentioned enumerations.  You should
    // do so in the platform-dependent application layer.  This guarantees
    // that you access the correct projection matrix in GetProjection().
    inline static void SetDefaultDepthType (DepthType type);
    inline static DepthType GetDefaultDepthType ();

// Read the comments in Wm5GraphicsLIB.h about this define.
private_internal:
#ifdef WM5_VALIDATE_CAMERA_FRAME_ONCE
    bool mValidateCameraFrame;
#endif
};

WM5_REGISTER_STREAM(Camera);
typedef Pointer0<Camera> CameraPtr;
#include "Wm5Camera.inl"

}

#endif
