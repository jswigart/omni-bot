// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrTetrahedron3Tetrahedron3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrTetrahedron3Tetrahedron3<Real>::IntrTetrahedron3Tetrahedron3 (
    const Tetrahedron3<Real>& tetrahedron0,
    const Tetrahedron3<Real>& tetrahedron1)
    :
    mTetrahedron0(&tetrahedron0),
    mTetrahedron1(&tetrahedron1)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Tetrahedron3<Real>&
IntrTetrahedron3Tetrahedron3<Real>::GetTetrahedron0 () const
{
    return *mTetrahedron0;
}
//----------------------------------------------------------------------------
template <typename Real>
const Tetrahedron3<Real>&
IntrTetrahedron3Tetrahedron3<Real>::GetTetrahedron1 () const
{
    return *mTetrahedron1;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrTetrahedron3Tetrahedron3<Real>::Find ()
{
    // Build planar faces of tetrahedron0.
    Plane3<Real> plane[4];
    mTetrahedron0->GetPlanes(plane);

    // Initial object to clip is tetrahedron1.
    mIntersection.clear();
    mIntersection.push_back(*mTetrahedron1);

    // Clip tetrahedron1 against planes of tetrahedron0.
    for (int i = 0; i < 4; ++i)
    {
        std::vector<Tetrahedron3<Real> > inside;
        for (int j = 0; j < (int)mIntersection.size(); ++j)
        {
            SplitAndDecompose(mIntersection[j], plane[i], inside);
        }
        mIntersection = inside;
    }

    return mIntersection.size() > 0;
}
//----------------------------------------------------------------------------
template <typename Real>
const std::vector<Tetrahedron3<Real> >&
IntrTetrahedron3Tetrahedron3<Real>::GetIntersection () const
{
    return mIntersection;
}
//----------------------------------------------------------------------------
template <typename Real>
void IntrTetrahedron3Tetrahedron3<Real>::SplitAndDecompose (
    Tetrahedron3<Real> tetra, const Plane3<Real>& plane,
    std::vector<Tetrahedron3<Real> >& inside)
{
    // Determine on which side of the plane the points of the tetrahedron lie.
    Real C[4];
    int i, pos[4], neg[4], zer[4];
    int positive = 0, negative = 0, zero = 0;

    for (i = 0; i < 4; ++i)
    {
        C[i] = plane.DistanceTo(tetra.V[i]);
        if (C[i] > (Real)0)
        {
            pos[positive++] = i;
        }
        else if (C[i] < (Real)0)
        {
            neg[negative++] = i;
        }
        else
        {
            zer[zero++] = i;
        }
    }

    // For a split to occur, one of the c_i must be positive and one must
    // be negative.

    if (negative == 0)
    {
        // Tetrahedron is completely on the positive side of plane, full clip.
        return;
    }

    if (positive == 0)
    {
        // Tetrahedron is completely on the negative side of plane.
        inside.push_back(tetra);
        return;
    }

    // Tetrahedron is split by plane.  Determine how it is split and how to
    // decompose the negative-side portion into tetrahedra (6 cases).
    Real w0, w1, invCDiff;
    Vector3<Real> intp[4];

    if (positive == 3)
    {
        // +++-
        for (i = 0; i < positive; ++i)
        {
            invCDiff = ((Real)1)/(C[pos[i]] - C[neg[0]]);
            w0 = -C[neg[0]]*invCDiff;
            w1 = +C[pos[i]]*invCDiff;
            tetra.V[pos[i]] = w0*tetra.V[pos[i]] +
                w1*tetra.V[neg[0]];
        }
        inside.push_back(tetra);
    }
    else if (positive == 2)
    {
        if (negative == 2)
        {
            // ++--
            for (i = 0; i < positive; ++i)
            {
                invCDiff = ((Real)1)/(C[pos[i]] - C[neg[0]]);
                w0 = -C[neg[0]]*invCDiff;
                w1 = +C[pos[i]]*invCDiff;
                intp[i] = w0*tetra.V[pos[i]] + w1*tetra.V[neg[0]];
            }
            for (i = 0; i < negative; ++i)
            {
                invCDiff = ((Real)1)/(C[pos[i]] - C[neg[1]]);
                w0 = -C[neg[1]]*invCDiff;
                w1 = +C[pos[i]]*invCDiff;
                intp[i+2] = w0*tetra.V[pos[i]] +
                    w1*tetra.V[neg[1]];
            }

            tetra.V[pos[0]] = intp[2];
            tetra.V[pos[1]] = intp[1];
            inside.push_back(tetra);

            inside.push_back(Tetrahedron3<Real>(tetra.V[neg[1]],
                intp[3], intp[2], intp[1]));

            inside.push_back(Tetrahedron3<Real>(tetra.V[neg[0]],
                intp[0], intp[1], intp[2]));
        }
        else
        {
            // ++-0
            for (i = 0; i < positive; ++i)
            {
                invCDiff = ((Real)1)/(C[pos[i]] - C[neg[0]]);
                w0 = -C[neg[0]]*invCDiff;
                w1 = +C[pos[i]]*invCDiff;
                tetra.V[pos[i]] = w0*tetra.V[pos[i]] +
                    w1*tetra.V[neg[0]];
            }
            inside.push_back(tetra);
        }
    }
    else if (positive == 1)
    {
        if (negative == 3)
        {
            // +---
            for (i = 0; i < negative; ++i)
            {
                invCDiff = ((Real)1)/(C[pos[0]] - C[neg[i]]);
                w0 = -C[neg[i]]*invCDiff;
                w1 = +C[pos[0]]*invCDiff;
                intp[i] = w0*tetra.V[pos[0]] + w1*tetra.V[neg[i]];
            }

            tetra.V[pos[0]] = intp[0];
            inside.push_back(tetra);

            inside.push_back(Tetrahedron3<Real>(intp[0],
                tetra.V[neg[1]], tetra.V[neg[2]], intp[1]));

            inside.push_back(Tetrahedron3<Real>(tetra.V[neg[2]],
                intp[1], intp[2], intp[0]));
        }
        else if (negative == 2)
        {
            // +--0
            for (i = 0; i < negative; ++i)
            {
                invCDiff = ((Real)1)/(C[pos[0]] - C[neg[i]]);
                w0 = -C[neg[i]]*invCDiff;
                w1 = +C[pos[0]]*invCDiff;
                intp[i] = w0*tetra.V[pos[0]] + w1*tetra.V[neg[i]];
            }

            tetra.V[pos[0]] = intp[0];
            inside.push_back(tetra);

            inside.push_back(Tetrahedron3<Real>(intp[1],
                tetra.V[zer[0]], tetra.V[neg[1]], intp[0]));
        }
        else
        {
            // +-00
            invCDiff = ((Real)1)/(C[pos[0]] - C[neg[0]]);
            w0 = -C[neg[0]]*invCDiff;
            w1 = +C[pos[0]]*invCDiff;
            tetra.V[pos[0]] = w0*tetra.V[pos[0]] +
                w1*tetra.V[neg[0]];
            inside.push_back(tetra);
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrTetrahedron3Tetrahedron3<float>;

template WM5_MATHEMATICS_ITEM
class IntrTetrahedron3Tetrahedron3<double>;
//----------------------------------------------------------------------------
}
