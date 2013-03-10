// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntpVectorField2.h"
#include "Wm5Memory.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntpVectorField2<Real>::IntpVectorField2 (int quantity,
    Vector2<Real>* domain, Vector2<Real>* range, bool owner,
    Query::Type queryType)
{
    // Repackage the output vectors into individual components.  This is
    // required because of the format that the quadratic interpolator expects
    // for its input data.
    Real* xOutput = new1<Real>(quantity);
    Real* yOutput = new1<Real>(quantity);
    for (int i = 0; i < quantity; ++i)
    {
        xOutput[i] = range[i].X();
        yOutput[i] = range[i].Y();
    }

    if (owner)
    {
        delete1(range);
    }

    // Common triangulator for the interpolators.
    mDT = new0 Delaunay2<Real>(quantity, domain, (Real)0.001, owner,
        queryType);

    // Create interpolator for x-coordinate of vector field.
    mXInterp = new0 IntpQdrNonuniform2<Real>(*mDT, xOutput, true);

    // Create interpolator for y-coordinate of vector field, but share the
    // already created triangulation for the x-interpolator.
    mYInterp = new0 IntpQdrNonuniform2<Real>(*mDT, yOutput, true);
}
//----------------------------------------------------------------------------
template <typename Real>
IntpVectorField2<Real>::~IntpVectorField2 ()
{
    delete0(mDT);
    delete0(mXInterp);
    delete0(mYInterp);
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntpVectorField2<Real>::Evaluate (const Vector2<Real>& input,
    Vector2<Real>& output)
{
    Real xDeriv, yDeriv;
    return mXInterp->Evaluate(input, output.X(), xDeriv, yDeriv)
        && mYInterp->Evaluate(input, output.Y(), xDeriv, yDeriv);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntpVectorField2<float>;

template WM5_MATHEMATICS_ITEM
class IntpVectorField2<double>;
//----------------------------------------------------------------------------
}
