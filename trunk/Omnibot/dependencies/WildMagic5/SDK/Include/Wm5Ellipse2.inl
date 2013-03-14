// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Real>
Ellipse2<Real>::Ellipse2 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Ellipse2<Real>::~Ellipse2 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Ellipse2<Real>::Ellipse2 (const Vector2<Real>& center,
    const Vector2<Real> axis[2], const Real extent[2])
    :
    Center(center)
{
    Axis[0] = axis[0];
    Axis[1] = axis[1];
    Extent[0] = extent[0];
    Extent[1] = extent[1];
}
//----------------------------------------------------------------------------
template <typename Real>
Ellipse2<Real>::Ellipse2 (const Vector2<Real>& center,
    const Vector2<Real>& axis0, const Vector2<Real>& axis1,
    const Real extent0, const Real extent1)
    :
    Center(center)
{
    Axis[0] = axis0;
    Axis[1] = axis1;
    Extent[0] = extent0;
    Extent[1] = extent1;
}
//----------------------------------------------------------------------------
template <typename Real>
void Ellipse2<Real>::GetM (Matrix2<Real>& M) const
{
    Vector2<Real> ratio0 = Axis[0]/Extent[0];
    Vector2<Real> ratio1 = Axis[1]/Extent[1];
    M = Matrix2<Real>(ratio0, ratio0) + Matrix2<Real>(ratio1, ratio1);
}
//----------------------------------------------------------------------------
template <typename Real>
void Ellipse2<Real>::GetMInverse (Matrix2<Real>& MInverse) const
{
    Vector2<Real> ratio0 = Axis[0]*Extent[0];
    Vector2<Real> ratio1 = Axis[1]*Extent[1];
    MInverse = Matrix2<Real>(ratio0, ratio0) +
        Matrix2<Real>(ratio1, ratio1);
}
//----------------------------------------------------------------------------
template <typename Real>
void Ellipse2<Real>::ToCoefficients (Real coeff[6]) const
{
    Matrix2<Real> A;
    Vector2<Real> B;
    Real C;
    ToCoefficients(A, B, C);
    Convert(A, B, C, coeff);

    // Arrange for one of the x0^2 or x1^2 coefficients to be 1.
    Real maxValue = Math<Real>::FAbs(coeff[3]);
    int maxIndex = 3;
    Real absValue = Math<Real>::FAbs(coeff[5]);
    if (absValue > maxValue)
    {
        maxValue = absValue;
        maxIndex = 5;
    }

    Real invMaxValue = ((Real)1)/maxValue;
    for (int i = 0; i < 6; ++i)
    {
        if (i != maxIndex)
        {
            coeff[i] *= invMaxValue;
        }
        else
        {
            coeff[i] = (Real)1;
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void Ellipse2<Real>::ToCoefficients (Matrix2<Real>& A, Vector2<Real>& B,
    Real& C) const
{
    Vector2<Real> ratio0 = Axis[0]/Extent[0];
    Vector2<Real> ratio1 = Axis[1]/Extent[1];
    A = Matrix2<Real>(ratio0, ratio0) + Matrix2<Real>(ratio1, ratio1);
    B = ((Real)-2)*(A*Center);
    C = A.QForm(Center, Center) - (Real)1;
}
//----------------------------------------------------------------------------
template <typename Real>
bool Ellipse2<Real>::FromCoefficients (const Real coeff[6])
{
    Matrix2<Real> A;
    Vector2<Real> B;
    Real C;
    Convert(coeff, A, B, C);
    return FromCoefficients(A, B, C);
}
//----------------------------------------------------------------------------
template <typename Real>
bool Ellipse2<Real>::FromCoefficients (const Matrix2<Real>& A,
    const Vector2<Real>& B, Real C)
{
    // Compute the center K = -A^{-1}*B/2.
    Matrix2<Real> invA = A.Inverse();
    if (invA == Matrix2<Real>::ZERO)
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
    Matrix2<Real> M = invRightSide*A;

    // Factor into M = R*D*R^T.
    EigenDecomposition<Real> eigensystem(M);
    eigensystem.Solve(true);
    for (int i = 0; i < 2; ++i)
    {
        if (eigensystem.GetEigenvalue(i) <= (Real)0)
        {
            return false;
        }

        Extent[i] = Math<Real>::InvSqrt(eigensystem.GetEigenvalue(i));
        Axis[i] = eigensystem.GetEigenvector2(i);
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
Real Ellipse2<Real>::Evaluate (const Vector2<Real>& point) const
{
    Vector2<Real> diff = point - Center;
    Real ratio0 = Axis[0].Dot(diff)/Extent[0];
    Real ratio1 = Axis[1].Dot(diff)/Extent[1];
    Real value = ratio0*ratio0 + ratio1*ratio1 - (Real)1;
    return value;
}
//----------------------------------------------------------------------------
template <typename Real>
bool Ellipse2<Real>::Contains (const Vector2<Real>& point) const
{
    return (Evaluate(point) <= (Real)0);
}
//----------------------------------------------------------------------------
template <typename Real>
void Ellipse2<Real>::Convert (const Real coeff[6], Matrix2<Real>& A,
    Vector2<Real>& B, Real& C)
{
    C = coeff[0];
    B[0] = coeff[1];
    B[1] = coeff[2];
    A[0][0] = coeff[3];
    A[0][1] = ((Real)0.5)*coeff[4];
    A[1][0] = A[0][1];
    A[1][1] = coeff[5];
}
//----------------------------------------------------------------------------
template <typename Real>
void Ellipse2<Real>::Convert (const Matrix2<Real>& A,
    const Vector2<Real>& B, Real C, Real coeff[6])
{
    coeff[0] = C;
    coeff[1] = B[0];
    coeff[2] = B[1];
    coeff[3] = A[0][0];
    coeff[4] = ((Real)2)*A[0][1];
    coeff[5] = A[1][1];
}
//----------------------------------------------------------------------------
