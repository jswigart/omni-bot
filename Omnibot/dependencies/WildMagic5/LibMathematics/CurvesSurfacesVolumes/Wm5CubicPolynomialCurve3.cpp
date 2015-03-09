// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5CubicPolynomialCurve3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
CubicPolynomialCurve3<Real>::CubicPolynomialCurve3
    (Polynomial1<Real>* xPoly, Polynomial1<Real>* yPoly,
    Polynomial1<Real>* zPoly)
    :
    PolynomialCurve3<Real>(xPoly, yPoly, zPoly)
{
    assertion(xPoly && yPoly && zPoly, "Invalid input\n");
    assertion(xPoly->GetDegree() == 3, "Invalid input\n");
    assertion(yPoly->GetDegree() == 3, "Invalid input\n");
    assertion(xPoly->GetDegree() == 3, "Invalid input\n");

    mNumVertices = 0;
    mVertices = 0;
}
//----------------------------------------------------------------------------
template <typename Real>
CubicPolynomialCurve3<Real>::~CubicPolynomialCurve3 ()
{
    delete1(mVertices);
}
//----------------------------------------------------------------------------
template <typename Real>
int CubicPolynomialCurve3<Real>::GetNumVertices () const
{
    return mNumVertices;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>* CubicPolynomialCurve3<Real>::GetVertices () const
{
    return mVertices;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real>* CubicPolynomialCurve3<Real>::GetVertices ()
{
    return mVertices;
}
//----------------------------------------------------------------------------
template <typename Real>
void CubicPolynomialCurve3<Real>::Tessellate (int level)
{
    // Vertices V = (2^L+1).
    int twoPowL = (1 << level);
    mNumVertices = twoPowL + 1;
    delete1(mVertices);
    mVertices = new1<Vector3<Real> >(mNumVertices);

    // Indices of endpoints, I[t].
    IntervalParameters IP;
    IP.I0 = 0;
    IP.I1 = twoPowL;

    // Vertices for subdivision.
    Vector3<Real>* X = mVertices;
    X[IP.I0] = GetPosition(mTMin);
    X[IP.I1] = GetPosition(mTMax);

    // Recursive subdivision.
    if (level > 0)
    {
        IP.Xuu[0] = GetSecondDerivative(mTMin);
        IP.Xuu[1] = GetSecondDerivative(mTMax);
        Subdivide(--level, (Real)0.25, X, IP);
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void CubicPolynomialCurve3<Real>::Subdivide (int level, Real dsqr,
    Vector3<Real>* X, IntervalParameters& IP)
{
    // Subdivision index.
    int IMid = (IP.I0 + IP.I1) >> 1;

    // Vertices.
    Vector3<Real> XuuM = ((Real)0.5)*(IP.Xuu[0] + IP.Xuu[1]);
    X[IMid] = ((Real)0.5)*(X[IP.I0] + X[IP.I1] - dsqr*XuuM);

    // Recurse on two children.
    if (level > 0)
    {
        --level;
        dsqr *= (Real)0.25;

        IntervalParameters subIP;

        // Subinterval [t0,tM].
        subIP.I0 = IP.I0;
        subIP.I1 = IMid;
        subIP.Xuu[0] = IP.Xuu[0];
        subIP.Xuu[1] = XuuM;
        Subdivide(level, dsqr, X, subIP);

        // Subinterval [tM,t1].
        subIP.I0 = IMid;
        subIP.I1 = IP.I1;
        subIP.Xuu[0] = XuuM;
        subIP.Xuu[1] = IP.Xuu[1];
        Subdivide(level, dsqr, X, subIP);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class CubicPolynomialCurve3<float>;

template WM5_MATHEMATICS_ITEM
class CubicPolynomialCurve3<double>;
//----------------------------------------------------------------------------
}
