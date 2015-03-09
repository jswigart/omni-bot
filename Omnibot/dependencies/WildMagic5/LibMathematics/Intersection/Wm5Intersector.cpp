// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5Intersector.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
Intersector<Real,TVector>::Intersector ()
{
    mContactTime = (Real)0;
    mIntersectionType = IT_EMPTY;
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
Intersector<Real,TVector>::~Intersector ()
{
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
Real Intersector<Real,TVector>::GetContactTime () const
{
    return mContactTime;
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
int Intersector<Real,TVector>::GetIntersectionType () const
{
    return mIntersectionType;
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
bool Intersector<Real,TVector>::Test ()
{
    // Stub for derived class.
    assertion(false, "Function not yet implemented\n");
    return false;
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
bool Intersector<Real,TVector>::Find ()
{
    // Stub for derived class.
    assertion(false, "Function not yet implemented\n");
    return false;
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
bool Intersector<Real,TVector>::Test (Real, const TVector&, const TVector&)
{
    // Stub for derived class.
    assertion(false, "Function not yet implemented\n");
    return false;
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
bool Intersector<Real,TVector>::Find (Real, const TVector&, const TVector&)
{
    // Stub for derived class.
    assertion(false, "Function not yet implemented\n");
    return false;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class Intersector<float,Vector2f>;

template WM5_MATHEMATICS_ITEM
class Intersector<float,Vector3f>;

template WM5_MATHEMATICS_ITEM
class Intersector<double,Vector2d>;

template WM5_MATHEMATICS_ITEM
class Intersector<double,Vector3d>;
//----------------------------------------------------------------------------
}
