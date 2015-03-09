// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Real>
Triangle3<Real>::Triangle3 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Triangle3<Real>::~Triangle3 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Triangle3<Real>::Triangle3 (const Vector3<Real>& v0,
    const Vector3<Real>& v1, const Vector3<Real>& v2)
{
    V[0] = v0;
    V[1] = v1;
    V[2] = v2;
}
//----------------------------------------------------------------------------
template <typename Real>
Triangle3<Real>::Triangle3 (const Vector3<Real> vertex[3])
{
    for (int i = 0; i < 3; ++i)
    {
        V[i] = vertex[i];
    }
}
//----------------------------------------------------------------------------
template <typename Real>
Real Triangle3<Real>::DistanceTo (const Vector3<Real>& q) const
{
    Vector3<Real> diff = V[0] - q;
    Vector3<Real> edge0 = V[1] - V[0];
    Vector3<Real> edge1 = V[2] - V[0];
    Real a00 = edge0.SquaredLength();
    Real a01 = edge0.Dot(edge1);
    Real a11 = edge1.SquaredLength();
    Real b0 = diff.Dot(edge0);
    Real b1 = diff.Dot(edge1);
    Real c = diff.SquaredLength();
    Real det = Math<Real>::FAbs(a00*a11 - a01*a01);
    Real s = a01*b1 - a11*b0;
    Real t = a01*b0 - a00*b1;
    Real sqrDistance;

    if (s + t <= det)
    {
        if (s < (Real)0)
        {
            if (t < (Real)0)  // region 4
            {
                if (b0 < (Real)0)
                {
                    if (-b0 >= a00)
                    {
                        sqrDistance = a00 + ((Real)2)*b0 + c;
                    }
                    else
                    {
                        sqrDistance = c - b0*b0/a00;
                    }
                }
                else
                {
                    if (b1 >= (Real)0)
                    {
                        sqrDistance = c;
                    }
                    else if (-b1 >= a11)
                    {
                        sqrDistance = a11 + ((Real)2)*b1 + c;
                    }
                    else
                    {
                        sqrDistance = c - b1*b1/a11;
                    }
                }
            }
            else  // region 3
            {
                if (b1 >= (Real)0)
                {
                    sqrDistance = c;
                }
                else if (-b1 >= a11)
                {
                    sqrDistance = a11 + ((Real)2)*b1 + c;
                }
                else
                {
                    sqrDistance = c - b1*b1/a11;
                }
            }
        }
        else if (t < (Real)0)  // region 5
        {
            if (b0 >= (Real)0)
            {
                sqrDistance = c;
            }
            else if (-b0 >= a00)
            {
                sqrDistance = a00 + ((Real)2)*b0 + c;
            }
            else
            {
                sqrDistance = b0*s + c - b0*b0/a00;
            }
        }
        else  // region 0
        {
            // The minimum is at an interior point of the triangle.
            Real invDet = ((Real)1)/det;
            s *= invDet;
            t *= invDet;
            sqrDistance = s*(a00*s + a01*t + ((Real)2)*b0) +
                t*(a01*s + a11*t + ((Real)2)*b1) + c;
        }
    }
    else
    {
        Real tmp0, tmp1, numer, denom;

        if (s < (Real)0)  // region 2
        {
            tmp0 = a01 + b0;
            tmp1 = a11 + b1;
            if (tmp1 > tmp0)
            {
                numer = tmp1 - tmp0;
                denom = a00 - ((Real)2)*a01 + a11;
                if (numer >= denom)
                {
                    sqrDistance = a00 + ((Real)2)*b0 + c;
                }
                else
                {
                    s = numer/denom;
                    t = (Real)1 - s;
                    sqrDistance = s*(a00*s + a01*t + ((Real)2)*b0) +
                        t*(a01*s + a11*t + ((Real)2)*b1) + c;
                }
            }
            else
            {
                if (tmp1 <= (Real)0)
                {
                    sqrDistance = a11 + ((Real)2)*b1 + c;
                }
                else if (b1 >= (Real)0)
                {
                    sqrDistance = c;
                }
                else
                {
                    sqrDistance = c - b1*b1/a11;
                }
            }
        }
        else if (t < (Real)0)  // region 6
        {
            tmp0 = a01 + b1;
            tmp1 = a00 + b0;
            if (tmp1 > tmp0)
            {
                numer = tmp1 - tmp0;
                denom = a00 - ((Real)2)*a01 + a11;
                if (numer >= denom)
                {
                    t = (Real)1;
                    s = (Real)0;
                    sqrDistance = a11 + ((Real)2)*b1 + c;
                }
                else
                {
                    t = numer/denom;
                    s = (Real)1 - t;
                    sqrDistance = s*(a00*s + a01*t + ((Real)2)*b0) +
                        t*(a01*s + a11*t + ((Real)2)*b1) + c;
                }
            }
            else
            {
                if (tmp1 <= (Real)0)
                {
                    sqrDistance = a00 + ((Real)2)*b0 + c;
                }
                else if (b0 >= (Real)0)
                {
                    sqrDistance = c;
                }
                else
                {
                    sqrDistance = c - b0*b0/a00;
                }
            }
        }
        else  // region 1
        {
            numer = a11 + b1 - a01 - b0;
            if (numer <= (Real)0)
            {
                sqrDistance = a11 + ((Real)2)*b1 + c;
            }
            else
            {
                denom = a00 - ((Real)2)*a01 + a11;
                if (numer >= denom)
                {
                    sqrDistance = a00 + ((Real)2)*b0 + c;
                }
                else
                {
                    s = numer/denom;
                    t = (Real)1 - s;
                    sqrDistance = s*(a00*s + a01*t + ((Real)2)*b0) +
                        t*(a01*s + a11*t + ((Real)2)*b1) + c;
                }
            }
        }
    }

    return Math<Real>::Sqrt(Math<Real>::FAbs(sqrDistance));
}
//----------------------------------------------------------------------------
