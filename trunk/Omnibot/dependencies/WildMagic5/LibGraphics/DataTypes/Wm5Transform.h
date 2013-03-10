// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2010/10/01)

#ifndef WM5TRANSFORM_H
#define WM5TRANSFORM_H

#include "Wm5GraphicsLIB.h"
#include "Wm5HMatrix.h"
#include "Wm5Math.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM Transform
{
public:
    // The transform is Y = M*X+T, where M is a 3-by-3 matrix and T is a
    // ranslation vector.  In most cases, M = R, a rotation matrix, or
    // M = R*S, where R is a rotation matrix and S is a diagonal matrix
    // whose diagonal entries are positive scales.  To support modeling
    // packages that allow general affine transforms, M can be any invertible
    // 3x3 matrix.  The vector X is transformed in the "forward" direction to
    // Y.  The "inverse" direction transforms Y to X, namely X = M^{-1}*(Y-T)
    // in the general case.  In the special case of M = R*S, the inverse
    // direction is X = S^{-1}*R^t*(Y-T), where S^{-1} is the diagonal matrix
    // whose diagonal entries are the reciprocoals of those of S and where
    // R^t is the transpose of R.  For SIMD support of matrix-vector and
    // matrix-matrix multiplications, a homogeneous matrix H = {{M,T},{0,1}}
    // is stored by this class.  The forward transform is {Y,1} = H*{X,1} and
    // the inverse transform is {X,1} = H^{-1}*{Y,1}.

    // Construction and destruction.  The default constructor produces the
    // identity transformation.  The default copy constructor is created by
    // the compiler, as is the default assignment operator.  The defaults are
    // consistent with the design of this class.
    Transform ();
    ~Transform ();

    // Set the transformation to the identity matrix.
    void MakeIdentity ();

    // Set the transformation to have scales of 1.
    void MakeUnitScale ();

    // Hints about the structure of the transformation.
    inline bool IsIdentity () const;      // I
    inline bool IsRSMatrix () const;      // R*S
    inline bool IsUniformScale () const;  // R*S, S = c*I

    // Member access.
    // (1) The Set* functions set the is-identity hint to false.
    // (2) The SetRotate function sets the is-rsmatrix hint to true.  If this
    //     hint is false,  GetRotate fires an "assert" in debug mode.
    // (3) The SetMatrix function sets the is-rsmatrix and is-uniform-scale
    //     hints to false.
    // (4) The SetScale function sets the is-uniform-scale hint to false.
    //     The SetUniformScale function sets the is-uniform-scale hint to
    //     true.  If this hint is false, GetUniformScale fires an "assert" in
    //     debug mode.
    // (5) All Set* functions set the inverse-needs-update to true.  When
    //     GetInverse is called, the inverse must be computed in this case and
    //     the inverse-needs-update is reset to false.
    void SetRotate (const HMatrix& rotate);
    void SetMatrix (const HMatrix& matrix);
    void SetTranslate (const APoint& translate);
    void SetScale (const APoint& scale);
    void SetUniformScale (float scale);
    inline const HMatrix& GetRotate () const;
    inline const HMatrix& GetMatrix () const;
    inline const APoint& GetTranslate () const;
    inline const APoint& GetScale () const;
    inline float GetUniformScale () const;

    // For M = R*S, the largest value of S in absolute value is returned.
    // For general M, the max-row-sum norm is returned, which is a reasonable
    // measure of maximum scale of the transformation.
    float GetNorm () const;

    // Matrix-point multiplication, M*p.
    inline APoint operator* (const APoint& pnt) const;

    // Matrix-vector multiplication, M*v.
    inline AVector operator* (const AVector& vec) const;

    // Matrix-matrix multiplication.
    Transform operator* (const Transform& transform) const;

    // Get the homogeneous matrix.
    inline const HMatrix& Matrix () const;

    // Get the inverse homogeneous matrix, recomputing it when necessary.  If
    // H = {{M,T},{0,1}}, then H^{-1} = {{M^{-1},-M^{-1}*T},{0,1}}.
    const HMatrix& Inverse () const;

    // Get the inversion transform.  No test is performed to determine whether
    // the caller transform is invertible.
    Transform InverseTransform () const;

    // The identity transformation.
    static const Transform IDENTITY;

    // Streaming support.
    inline static int GetStreamingSize ();

private:
    // Fill in the entries of mHMatrix whenever one of the components
    // mMatrix, mTranslate, or mScale changes.
    void UpdateHMatrix ();

    // Invert the 3x3 upper-left block of the input matrix.
    static void Invert3x3 (const HMatrix& mat, HMatrix& invMat);

    // The full 4x4 homogeneous matrix H = {{M,T},{0,1}} and its inverse
    // H^{-1} = {M^{-1},-M^{-1}*T},{0,1}}.  The inverse is computed only
    // on demand.
    HMatrix mHMatrix;
    mutable HMatrix mInvHMatrix;

    HMatrix mMatrix;     // M (general) or R (rotation)
    APoint mTranslate;   // T
    APoint mScale;       // S
    bool mIsIdentity, mIsRSMatrix, mIsUniformScale;
    mutable bool mInverseNeedsUpdate;

    // Support for streaming.
    friend class InStream;
    friend class OutStream;
};

#include "Wm5Transform.inl"

}

#endif
