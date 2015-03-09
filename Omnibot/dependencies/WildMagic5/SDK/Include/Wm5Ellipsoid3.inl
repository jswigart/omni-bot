// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Real>
Ellipsoid3<Real>::Ellipsoid3 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Ellipsoid3<Real>::~Ellipsoid3 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Ellipsoid3<Real>::Ellipsoid3 (const Vector3<Real>& center,
    const Vector3<Real> axis[3], const Real extent[3])
    :
    Center(center)
{
    Axis[0] = axis[0];
    Axis[1] = axis[1];
    Axis[2] = axis[2];
    Extent[0] = extent[0];
    Extent[1] = extent[1];
    Extent[2] = extent[2];
}
//----------------------------------------------------------------------------
template <typename Real>
Ellipsoid3<Real>::Ellipsoid3 (const Vector3<Real>& center,
    const Vector3<Real>& axis0, const Vector3<Real>& axis1,
    const Vector3<Real>& axis2, const Real extent0, const Real extent1,
    const Real extent2)
    :
    Center(center)
{
    Axis[0] = axis0;
    Axis[1] = axis1;
    Axis[2] = axis2;
    Extent[0] = extent0;
    Extent[1] = extent1;
    Extent[2] = extent2;
}
//----------------------------------------------------------------------------
template <typename Real>
void Ellipsoid3<Real>::GetM (Matrix3<Real>& M) const
{
    Vector3<Real> ratio0 = Axis[0]/Extent[0];
    Vector3<Real> ratio1 = Axis[1]/Extent[1];
    Vector3<Real> ratio2 = Axis[2]/Extent[2];
    M = Matrix3<Real>(ratio0, ratio0) + Matrix3<Real>(ratio1, ratio1)
        + Matrix3<Real>(ratio2, ratio2);
}
//----------------------------------------------------------------------------
template <typename Real>
void Ellipsoid3<Real>::GetMInverse (Matrix3<Real>& MInverse) const
{
    Vector3<Real> ratio0 = Axis[0]*Extent[0];
    Vector3<Real> ratio1 = Axis[1]*Extent[1];
    Vector3<Real> ratio2 = Axis[2]*Extent[2];
    MInverse = Matrix3<Real>(ratio0, ratio0) +
        Matrix3<Real>(ratio1, ratio1) + Matrix3<Real>(ratio2, ratio2);
}
//----------------------------------------------------------------------------
template <typename Real>
void Ellipsoid3<Real>::ToCoefficients (Real coeff[10]) const
{
    Matrix3<Real> A;
    Vector3<Real> B;
    Real C;
    ToCoefficients(A, B, C);
    Convert(A, B, C, coeff);

    // Arrange for one of the x0^2, x1^2, or x2^2 coefficients to be 1.
    Real maxValue = Math<Real>::FAbs(coeff[4]);
    int maxIndex = 4;
    Real absValue = Math<Real>::FAbs(coeff[7]);
    if (absValue > maxValue)
    {
        maxValue = absValue;
        maxIndex = 7;
    }
    absValue = Math<Real>::FAbs(coeff[9]);
    if (absValue > maxValue)
    {
        maxValue = absValue;
        maxIndex = 9;
    }

    Real invMaxValue = ((Real)1)/maxValue;
    for (int i = 0; i < 10; ++i)
    {
        if (i != maxIndex)
        {
            coeff[i] *= invMaxValue;
        }
        else
        {
            coeff[i] = (Real)1.0;
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void Ellipsoid3<Real>::ToCoefficients (Matrix3<Real>& A,
    Vector3<Real>& B, Real& C) const
{
    Vector3<Real> ratio0 = Axis[0]/Extent[0];
    Vector3<Real> ratio1 = Axis[1]/Extent[1];
    Vector3<Real> ratio2 = Axis[2]/Extent[2];
    A = Matrix3<Real>(ratio0, ratio0) + Matrix3<Real>(ratio1, ratio1)
        + Matrix3<Real>(ratio2, ratio2);

    B = ((Real)-2)*(A*Center);
    C = A.QForm(Center, Center) - (Real)1;
}
//----------------------------------------------------------------------------
template <typename Real>
bool Ellipsoid3<Real>::FromCoefficients (const Real coeff[10])
{
    Matrix3<Real> A;
    Vector3<Real> B;
    Real C;
    Convert(coeff, A, B, C);
    return FromCoefficients(A, B, C);
}
//----------------------------------------------------------------------------
template <typename Real>
bool Ellipsoid3<Real>::FromCoefficients (const Matrix3<Real>& A,
    const Vector3<Real>& B, Real C)
{
    // Compute the center K = -A^{-1}*B/2.
    Matrix3<Real> invA = A.Inverse();
    if (invA == Matrix3<Real>::ZERO)
    {
        return false;
    }

    Center = ((Real)-0.5)*(invA*B);

    // Compute B^T*A^{-1}*B/4 - C = K^T*A*K - C = -K^T*B/2 - C.
    Real rightSide = -((Real)0.5)*(Center.Dot(B)) - C;
    if (Math<Real>::FAbs(rightSide) < Math<Real>::ZERO_TOLERANCE)
    {
        return false;
    }

    // Compute M = A/(K^T*A*K - C).
    Real invRightSide = ((Real)1)/rightSide;
    Matrix3<Real> M = invRightSide*A;

    // Factor into M = R*D*R^T.
    EigenDecomposition<Real> eigensystem(M);
    eigensystem.Solve(true);
    for (int i = 0; i < 3; ++i)
    {
        if (eigensystem.GetEigenvalue(i) <= (Real)0)
        {
            return false;
        }

        Extent[i] = Math<Real>::InvSqrt(eigensystem.GetEigenvalue(i));
        Axis[i] = eigensystem.GetEigenvector3(i);
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
Real Ellipsoid3<Real>::Evaluate (const Vector3<Real>& point) const
{
    Vector3<Real> diff = point - Center;
    Real ratio0 = Axis[0].Dot(diff)/Extent[0];
    Real ratio1 = Axis[1].Dot(diff)/Extent[1];
    Real ratio2 = Axis[2].Dot(diff)/Extent[2];
    Real value = ratio0*ratio0 + ratio1*ratio1 + ratio2*ratio2 - (Real)1;
    return value;
}
//----------------------------------------------------------------------------
template <typename Real>
bool Ellipsoid3<Real>::Contains (const Vector3<Real>& point) const
{
    return (Evaluate(point) <= (Real)0);
}
//----------------------------------------------------------------------------
template <typename Real>
void Ellipsoid3<Real>::Convert (const Real coeff[10], Matrix3<Real>& A,
    Vector3<Real>& B, Real& C)
{
    C = coeff[0];
    B[0] = coeff[1];
    B[1] = coeff[2];
    B[2] = coeff[3];
    A[0][0] = coeff[4];
    A[0][1] = ((Real)0.5)*coeff[5];
    A[0][2] = ((Real)0.5)*coeff[6];
    A[1][0] = A[0][1];
    A[1][1] = coeff[7];
    A[1][2] = ((Real)0.5)*coeff[8];
    A[2][0] = A[0][2];
    A[2][1] = A[1][2];
    A[2][2] = coeff[9];
}
//----------------------------------------------------------------------------
template <typename Real>
void Ellipsoid3<Real>::Convert (const Matrix3<Real>& A,
    const Vector3<Real>& B, Real C, Real coeff[10])
{
    coeff[0] = C;
    coeff[1] = B[0];
    coeff[2] = B[1];
    coeff[3] = B[2];
    coeff[4] = A[0][0];
    coeff[5] = ((Real)2)*A[0][1];
    coeff[6] = ((Real)2)*A[0][2];
    coeff[7] = A[1][1];
    coeff[8] = ((Real)2)*A[1][2];
    coeff[9] = A[2][2];
}
//----------------------------------------------------------------------------
