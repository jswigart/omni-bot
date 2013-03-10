// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2010/10/01)

#ifndef WM5VECTOR2_H
#define WM5VECTOR2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Tuple.h"
#include "Wm5Math.h"

namespace Wm5
{

template <typename Real>
class Vector2 : public Tuple<2,Real>
{
public:
    // Construction.
    Vector2 ();  // uninitialized
    Vector2 (const Vector2& vec);
    Vector2 (const Tuple<2,Real>& tuple);
    Vector2 (Real x, Real y);

    // Assignment.
    Vector2& operator= (const Vector2& vec);
    Vector2& operator= (const Tuple<2,Real>& tuple);

    // Coordinate access.
    inline Real X () const;
    inline Real& X ();
    inline Real Y () const;
    inline Real& Y ();

    // Arithmetic operations.
    inline Vector2 operator+ (const Vector2& vec) const;
    inline Vector2 operator- (const Vector2& vec) const;
    inline Vector2 operator* (Real scalar) const;
    inline Vector2 operator/ (Real scalar) const;
    inline Vector2 operator- () const;

    // Arithmetic updates.
    inline Vector2& operator+= (const Vector2& vec);
    inline Vector2& operator-= (const Vector2& vec);
    inline Vector2& operator*= (Real scalar);
    inline Vector2& operator/= (Real scalar);

    // Vector operations.
    inline Real Length () const;
    inline Real SquaredLength () const;
    inline Real Dot (const Vector2& vec) const;
    inline Real Normalize (const Real epsilon = Math<Real>::ZERO_TOLERANCE);

    // Returns (y,-x).
    inline Vector2 Perp () const;

    // Returns (y,-x)/sqrt(x*x+y*y).
    inline Vector2 UnitPerp () const;

    // Returns DotPerp((x,y),(V.X(),V.Y())) = x*V.Y() - y*V.X().
    inline Real DotPerp (const Vector2& vec) const;

    // Compute the axis-aligned bounding box of the points.
    static void ComputeExtremes (int numVectors, const Vector2* vectors,
        Vector2& vmin, Vector2& vmax);

    // Gram-Schmidt orthonormalization.  Take linearly independent vectors U
    // and V and compute an orthonormal set (unit length, mutually
    // perpendicular).
    static void Orthonormalize (Vector2& u, Vector2& v);

    // Input V must be a nonzero vector.  The output is an orthonormal basis
    // {U,V}.  The input V is normalized by this function.  If you know V is
    // already unit length, use U = V.Perp().
    static void GenerateOrthonormalBasis (Vector2& u, Vector2& v);

    // Compute the barycentric coordinates of the point V with respect to the
    // triangle <V0,V1,V2>, V = b0*V0 + b1*V1 + b2*V2, where b0 + b1 + b2 = 1.
    // The return value is 'true' iff {V0,V1,V2} is a linearly independent
    // set.  Numerically, this is measured by |det[V0 V1 V2]| <= epsilon.
    // The values bary[...] are valid only when the return value is 'true'
    // but set to zero when the return value is 'false'.
    bool GetBarycentrics (const Vector2& v0, const Vector2& v1,
        const Vector2& v2, Real bary[3], const Real epsilon = (Real)0) const;

    struct Information
    {
        // The intrinsic dimension of the input set.  The parameter 'epsilon'
        // to the GetInformation function is used to provide a tolerance when
        // determining the dimension.
        int mDimension;

        // Axis-aligned bounding box of the input set.  The maximum range is
        // the larger of max[0]-min[0] and max[1]-min[1].
        Real mMin[2], mMax[2];
        Real mMaxRange;

        // Coordinate system.  The origin is valid for any dimension d.  The
        // unit-length direction vector is valid only for 0 <= i < d.  The
        // extreme index is relative to the array of input points, and is also
        // valid only for 0 <= i < d.  If d = 0, all points are effectively
        // the same, but the use of an epsilon may lead to an extreme index
        // that is not zero.  If d = 1, all points effectively lie on a line
        // segment.  If d = 2, the points are not collinear.
        Vector2 mOrigin;
        Vector2 mDirection[2];

        // The indices that define the maximum dimensional extents.  The
        // values mExtreme[0] and mExtreme[1] are the indices for the points
        // that define the largest extent in one of the coordinate axis
        // directions.  If the dimension is 2, then mExtreme[2] is the index
        // for the point that generates the largest extent in the direction
        // perpendicular to the line through the points corresponding to
        // mExtreme[0] and mExtreme[1].  The triangle formed by the points
        // V[extreme0], V[extreme1], and V[extreme2] is clockwise or
        // counterclockwise, the condition stored in mExtremeCCW.
        int mExtreme[3];
        bool mExtremeCCW;
    };

    // The value of epsilon is used as a relative error when computing the
    // dimension of the point set.
    static void GetInformation (int numPoints, const Vector2* points,
        Real epsilon, Information& info);

    // Special vectors.
    WM5_MATHEMATICS_ITEM static const Vector2 ZERO;    // (0,0)
    WM5_MATHEMATICS_ITEM static const Vector2 UNIT_X;  // (1,0)
    WM5_MATHEMATICS_ITEM static const Vector2 UNIT_Y;  // (0,1)
    WM5_MATHEMATICS_ITEM static const Vector2 ONE;     // (1,1)

protected:
    using Tuple<2,Real>::mTuple;
};

// Arithmetic operations.
template <typename Real>
inline Vector2<Real> operator* (Real scalar, const Vector2<Real>& vec);

// Debugging output.
template <typename Real>
std::ostream& operator<< (std::ostream& outFile, const Vector2<Real>& vec);

#include "Wm5Vector2.inl"

typedef Vector2<float> Vector2f;
typedef Vector2<double> Vector2d;

}

#endif
