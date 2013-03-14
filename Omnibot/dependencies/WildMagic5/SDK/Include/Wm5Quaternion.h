// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/03/27)

#ifndef WM5QUATERNION_H
#define WM5QUATERNION_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Matrix3.h"

namespace Wm5
{

template <typename Real>
class Quaternion
{
public:
    // A quaternion is q = w + x*i + y*j + z*k where (w,x,y,z) is not
    // necessarily a unit-length vector in 4D.

    // Construction.
    Quaternion ();  // uninitialized
    Quaternion (Real w, Real x, Real y, Real z);
    Quaternion (const Quaternion& q);

    // Quaternion for the input rotation matrix.
    Quaternion (const Matrix3<Real>& rot);

    // Quaternion for the rotation of the axis-angle pair.
    Quaternion (const Vector3<Real>& axis, Real angle);

    // Quaternion for the rotation matrix with specified columns.
    Quaternion (const Vector3<Real> rotColumn[3]);

    // Coordinate access as an array:  0 = w, 1 = x, 2 = y, 3 = z.
    inline operator const Real* () const;
    inline operator Real* ();
    inline const Real& operator[] (int i) const;
    inline Real& operator[] (int i);
    inline Real W () const;
    inline Real& W ();
    inline Real X () const;
    inline Real& X ();
    inline Real Y () const;
    inline Real& Y ();
    inline Real Z () const;
    inline Real& Z ();

    // Assignment.
    inline Quaternion& operator= (const Quaternion& q);

    // Comparison (for use by STL containers).
    inline bool operator== (const Quaternion& q) const;
    inline bool operator!= (const Quaternion& q) const;
    inline bool operator<  (const Quaternion& q) const;
    inline bool operator<= (const Quaternion& q) const;
    inline bool operator>  (const Quaternion& q) const;
    inline bool operator>= (const Quaternion& q) const;

    // Arithmetic operations.
    inline Quaternion operator+ (const Quaternion& q) const;
    inline Quaternion operator- (const Quaternion& q) const;
    inline Quaternion operator* (const Quaternion& q) const;
    inline Quaternion operator* (Real scalar) const;
    inline Quaternion operator/ (Real scalar) const;
    inline Quaternion operator- () const;

    friend Quaternion<Real> operator* (Real scalar,
        const Quaternion<Real>& q)
    {
        return q*scalar;
    }

    // Arithmetic updates.
    inline Quaternion& operator+= (const Quaternion& q);
    inline Quaternion& operator-= (const Quaternion& q);
    inline Quaternion& operator*= (Real scalar);
    inline Quaternion& operator/= (Real scalar);

    // Conversion between quaternions, matrices, and axis-angle.
    void FromRotationMatrix (const Matrix3<Real>& rot);
    void ToRotationMatrix (Matrix3<Real>& rot) const;
    void FromRotationMatrix (const Vector3<Real> rotColumn[3]);
    void ToRotationMatrix (Vector3<Real> rotColumn[3]) const;
    void FromAxisAngle (const Vector3<Real>& axis, Real angle);
    void ToAxisAngle (Vector3<Real>& axis, Real& angle) const;

    // Functions of a quaternion.
    inline Real Length () const;  // length of 4-tuple
    inline Real SquaredLength () const;  // squared length of 4-tuple
    inline Real Dot (const Quaternion& q) const;  // dot product of 4-tuples
    inline Real Normalize (Real epsilon = Math<Real>::ZERO_TOLERANCE);
    Quaternion Inverse () const;  // apply to non-zero quaternion
    Quaternion Conjugate () const;  // negate x, y, and z terms
    Quaternion Exp () const;  // apply to quaternion with w = 0
    Quaternion Log () const;  // apply to unit-length quaternion

    // Rotation of a vector by a quaternion.
    Vector3<Real> Rotate (const Vector3<Real>& vec) const;

    // Spherical linear interpolation.
    Quaternion& Slerp (Real t, const Quaternion& p, const Quaternion& q);
    Quaternion& SlerpExtraSpins (Real t, const Quaternion& p,
        const Quaternion& q, int extraSpins);

    // Intermediate terms for spherical quadratic interpolation.
    Quaternion& Intermediate (const Quaternion& q0, const Quaternion& q1,
        const Quaternion& q2);

    // Spherical quadratic interpolation.
    Quaternion& Squad (Real t, const Quaternion& q0, const Quaternion& a0,
        const Quaternion& a1, const Quaternion& q1);

    // Compute a quaternion that rotates unit-length vector V1 to unit-length
    // vector V2.  The rotation is about the axis perpendicular to both V1 and
    // V2, with angle of that between V1 and V2.  If V1 and V2 are parallel,
    // any axis of rotation will do, such as the permutation (z2,x2,y2), where
    // V2 = (x2,y2,z2).
    Quaternion& Align (const Vector3<Real>& v1, const Vector3<Real>& v2);

    // Decompose a quaternion into q = q_twist * q_swing, where q is 'this'
    // quaternion.  If V1 is the input axis and V2 is the rotation of V1 by
    // q, q_swing represents the rotation about the axis perpendicular to
    // V1 and V2 (see Quaternion::Align), and q_twist is a rotation about V1.
    void DecomposeTwistTimesSwing (const Vector3<Real>& v1,
        Quaternion& twist, Quaternion& swing);

    // Decompose a quaternion into q = q_swing * q_twist, where q is 'this'
    // quaternion.  If V1 is the input axis and V2 is the rotation of V1 by
    // q, q_swing represents the rotation about the axis perpendicular to
    // V1 and V2 (see Quaternion::Align), and q_twist is a rotation about V1.
    void DecomposeSwingTimesTwist (const Vector3<Real>& v1,
        Quaternion& swing, Quaternion& twist);

    // *** Find closest quaternions with unconstrained angles.

    // Closest quaternion of the form (cx + sx*i).
    Quaternion GetClosestX () const;

    // Closest quaternion of the form (cy + sy*j).
    Quaternion GetClosestY () const;

    // Closest quaternion of the form (cz + sz*k).
    Quaternion GetClosestZ () const;

    // Closest quaternion of the form (cx + sx*i)*(cy + sy*j).
    Quaternion GetClosestXY () const;

    // Closest quaternion of the form (cy + sy*j)*(cx + sx*i).
    Quaternion GetClosestYX () const;

    // Closest quaternion of the form (cz + sz*k)*(cx + sx*i).
    Quaternion GetClosestZX () const;

    // Closest quaternion of the form (cx + sx*i)*(cz + sz*k).
    Quaternion GetClosestXZ () const;

    // Closest quaternion of the form (cy + sy*j)*(cz + sz*k).
    Quaternion GetClosestYZ () const;

    // Closest quaternion of the form (cz + sz*k)*(cy + sy*j).
    Quaternion GetClosestZY () const;

    // Factor to (cx + sx*i)*(cy + sy*j)*(cz + sz*k).
    void FactorXYZ (Real& cx, Real& sx, Real& cy, Real& sy, Real& cz,
        Real& sz);

    // Factor to (cx + sx*i)*(cz + sz*k)*(cy + sy*j).
    void FactorXZY (Real& cx, Real& sx, Real& cz, Real& sz, Real& cy,
        Real& sy);

    // Factor to (cy + sy*j)*(cz + sz*k)*(cx + sx*i).
    void FactorYZX (Real& cy, Real& sy, Real& cz, Real& sz, Real& cx,
        Real& sx);

    // Factor to (cy + sy*j)*(cx + sx*i)*(cz + sz*k).
    void FactorYXZ (Real& cy, Real& sy, Real& cx, Real& sx, Real& cz,
        Real& sz);

    // Factor to (cz + sz*k)*(cx + sx*i)*(cy + sy*j).
    void FactorZXY (Real& cz, Real& sz, Real& cx, Real& sx, Real& cy,
        Real& sy);

    // Factor to (cz + sz*k)*(cy + sy*j)*(cx + sx*i).
    void FactorZYX (Real& cz, Real& sz, Real& cy, Real& sy, Real& cx,
        Real& sx);

    // *** Find closest quaternions with constrained angles.
    class Constraints
    {
    public:
        Constraints ();  // uninitialized
        Constraints (Real minAngle, Real maxAngle);
        void SetAngles (Real minAngle, Real maxAngle);
        bool IsValid (Real x, Real y) const;

        Real MinAngle;       // in [-PI/2,PI/2]
        Real MaxAngle;       // in [m_fMinAngle/2,PI/2]
        Real CosMinAngle;    // = cos(m_fMinAngle)
        Real SinMinAngle;    // = sin(m_fMinAngle)
        Real CosMaxAngle;    // = cos(m_fMaxAngle)
        Real SinMaxAngle;    // = sin(m_fMaxAngle)
        Real DiffCosMaxMin;  // = cos(m_fMaxAngle) - cos(m_fMinAngle)
        Real DiffSinMaxMin;  // = sin(m_fMaxAngle) - sin(m_fMinAngle)
        Real CosAvrAngle;    // = cos((m_fMinAngle + m_fMaxAngle)/2)
        Real SinAvrAngle;    // = sin((m_fMinAngle + mM_faxAngle)/2)
    };

    // Closest constrained quaternion of the form (cx + sx*i).
    Quaternion GetClosestX (const Constraints& xCon) const;

    // Closest constrained quaternion of the form (cy + sy*j).
    Quaternion GetClosestY (const Constraints& yCon) const;

    // Closest constrained quaternion of the form (cz + sz*k).
    Quaternion GetClosestZ (const Constraints& zCon) const;

    // Closest constrained quaternion of the form (cx + sx*i)*(cy + sy*j).
    Quaternion GetClosestXY (const Constraints& xCon,
        const Constraints& yCon) const;

    // Closest constrained quaternion of the form (cy + sy*j)*(cx + sx*i).
    Quaternion GetClosestYX (const Constraints& yCon,
        const Constraints& xCon) const;

    // Closest constrained quaternion of the form (cz + sz*k)*(cx + sx*i).
    Quaternion GetClosestZX (const Constraints& zCon,
        const Constraints& xCon) const;

    // Closest constrained quaternion of the form (cx + sx*i)*(cz + sz*k).
    Quaternion GetClosestXZ (const Constraints& xCon,
        const Constraints& zCon) const;

    // Closest constrained quaternion of the form (cz + sz*k)*(cy + sy*j).
    Quaternion GetClosestZY (const Constraints& zCon,
        const Constraints& yCon) const;

    // Closest constrained quaternion of the form (cy + sy*j)*(cz + sz*k).
    Quaternion GetClosestYZ (const Constraints& yCon,
        const Constraints& zCon) const;

    // Special quaternions.
    WM5_MATHEMATICS_ITEM static const Quaternion ZERO;
    WM5_MATHEMATICS_ITEM static const Quaternion IDENTITY;

private:
    // Closest unconstrained quaternion of the form:
    //   (cx + sx*i) when axis = 1,
    //   (cy + sy*j) when axis = 2,
    //   (cz + sz*k) when axis = 3
    Quaternion GetClosest (int axis) const;

    // Closest constrained quaternion of the form:
    //   (cx + sx*i) when axis = 1,
    //   (cy + sy*j) when axis = 2,
    //   (cz + sz*k) when axis = 3
    Quaternion GetClosest (int axis, const Constraints& con) const;

    // Order of storage is (w,x,y,z).
    Real mTuple[4];
};

#include "Wm5Quaternion.inl"

typedef Quaternion<float> Quaternionf;
typedef Quaternion<double> Quaterniond;

}

#endif
