// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ContLozenge3.h"
#include "Wm5ApprGaussPointsFit3.h"
#include "Wm5DistPoint3Line3.h"
#include "Wm5DistPoint3Rectangle3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
Lozenge3<Real> ContLozenge (int numPoints, const Vector3<Real>* points)
{
    // Fit with Gaussian.  Axis(0) corresponds to the smallest eigenvalue.
    Box3<Real> box = GaussPointsFit3<Real>(numPoints, points);

    Vector3<Real> diff = points[0] - box.Center;
    Real wMin = box.Axis[0].Dot(diff);
    Real wMax = wMin;
    Real w;
    int i;
    for (i = 1; i < numPoints; ++i)
    {
        diff = points[i] - box.Center;
        w = box.Axis[0].Dot(diff);
        if (w < wMin)
        {
            wMin = w;
        }
        else if (w > wMax)
        {
            wMax = w;
        }
    }

    Real radius = ((Real)0.5)*(wMax - wMin);
    Real rSqr = radius*radius;
    box.Center += (((Real)0.5)*(wMax + wMin))*box.Axis[0];

    Real aMin = Math<Real>::MAX_REAL;
    Real aMax = -aMin;
    Real bMin = Math<Real>::MAX_REAL;
    Real bMax = -bMin;
    Real discr, radical, u, v, test;
    for (i = 0; i < numPoints; ++i)
    {
        diff = points[i] - box.Center;
        u = box.Axis[2].Dot(diff);
        v = box.Axis[1].Dot(diff);
        w = box.Axis[0].Dot(diff);
        discr = rSqr - w*w;
        radical = Math<Real>::Sqrt(Math<Real>::FAbs(discr));

        test = u + radical;
        if (test < aMin)
        {
            aMin = test;
        }

        test = u - radical;
        if (test > aMax)
        {
            aMax = test;
        }

        test = v + radical;
        if (test < bMin)
        {
            bMin = test;
        }

        test = v - radical;
        if (test > bMax)
        {
            bMax = test;
        }
    }

    // The enclosing region might be a capsule or a sphere.
    if (aMin >= aMax)
    {
        test = ((Real)0.5)*(aMin + aMax);
        aMin = test;
        aMax = test;
    }
    if (bMin >= bMax)
    {
        test = ((Real)0.5)*(bMin + bMax);
        bMin = test;
        bMax = test;
    }

    // Make correction for points inside mitered corner but outside quarter
    // sphere.
    for (i = 0; i < numPoints; ++i)
    {
        diff = points[i] - box.Center;
        u = box.Axis[2].Dot(diff);
        v = box.Axis[1].Dot(diff);

        Real* aExtreme = 0;
        Real* bExtreme = 0;

        if (u > aMax)
        {
            if (v > bMax)
            {
                aExtreme = &aMax;
                bExtreme = &bMax;
            }
            else if (v < bMin)
            {
                aExtreme = &aMax;
                bExtreme = &bMin;
            }
        }
        else if (u < aMin)
        {
            if (v > bMax)
            {
                aExtreme = &aMin;
                bExtreme = &bMax;
            }
            else if (v < bMin)
            {
                aExtreme = &aMin;
                bExtreme = &bMin;
            }
        }

        if (aExtreme)
        {
            Real deltaU = u - *aExtreme;
            Real deltaV = v - *bExtreme;
            Real deltaSumSqr = deltaU*deltaU + deltaV*deltaV;
            w = box.Axis[0].Dot(diff);
            Real wSqr = w*w;
            test = deltaSumSqr + wSqr;
            if (test > rSqr)
            {
                discr = (rSqr - wSqr)/deltaSumSqr;
                Real t = -Math<Real>::Sqrt(Math<Real>::FAbs(discr));
                *aExtreme = u + t*deltaU;
                *bExtreme = v + t*deltaV;
            }
        }
    }

    Lozenge3<Real> lozenge;
    lozenge.Radius = radius;
    lozenge.Rectangle.Axis[0] = box.Axis[2];
    lozenge.Rectangle.Axis[1] = box.Axis[1];

    if (aMin < aMax)
    {
        if (bMin < bMax)
        {
            // Container is a lozenge.
            lozenge.Rectangle.Center = box.Center + aMin*box.Axis[2] +
                bMin*box.Axis[1];
            lozenge.Rectangle.Extent[0] = ((Real)0.5)*(aMax - aMin);
            lozenge.Rectangle.Extent[1] = ((Real)0.5)*(bMax - bMin);
        }
        else
        {
            // Container is a capsule.
            lozenge.Rectangle.Center = box.Center + aMin*box.Axis[2] +
                (((Real)0.5)*(bMin + bMax))*box.Axis[1];
            lozenge.Rectangle.Extent[0] = ((Real)0.5)*(aMax - aMin);
            lozenge.Rectangle.Extent[1] = (Real)0;
        }
    }
    else
    {
        if (bMin < bMax)
        {
            // Container is a capsule.
            lozenge.Rectangle.Center = box.Center + bMin*box.Axis[1] +
                (((Real)0.5)*(aMin + aMax))*box.Axis[2];
            lozenge.Rectangle.Extent[0] = (Real)0;
            lozenge.Rectangle.Extent[1] = ((Real)0.5)*(bMax - bMin);
        }
        else
        {
            // Container is a sphere.
            lozenge.Rectangle.Center = box.Center +
                (((Real)0.5)*(aMin + aMax))*box.Axis[2] +
                (((Real)0.5)*(bMin + bMax))*box.Axis[1];
            lozenge.Rectangle.Extent[0] = (Real)0;
            lozenge.Rectangle.Extent[1] = (Real)0;
        }
    }

    return lozenge;
}
//----------------------------------------------------------------------------
template <typename Real>
bool InLozenge (const Vector3<Real>& point, const Lozenge3<Real>& lozenge)
{
    Real dist = DistPoint3Rectangle3<Real>(point, lozenge.Rectangle).Get();
    return dist <= lozenge.Radius;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
Lozenge3<float> ContLozenge<float> (int, const Vector3<float>*);

template WM5_MATHEMATICS_ITEM
bool InLozenge<float> (const Vector3<float>&, const Lozenge3<float>&);


template WM5_MATHEMATICS_ITEM
Lozenge3<double> ContLozenge<double> (int, const Vector3<double>*);

template WM5_MATHEMATICS_ITEM
bool InLozenge<double> (const Vector3<double>&, const Lozenge3<double>&);
//----------------------------------------------------------------------------
}
