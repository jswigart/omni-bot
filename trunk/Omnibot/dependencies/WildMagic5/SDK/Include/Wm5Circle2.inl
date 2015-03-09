// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Real>
Circle2<Real>::Circle2 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Circle2<Real>::~Circle2 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Circle2<Real>::Circle2 (const Vector2<Real>& center, Real radius)
    :
    Center(center),
    Radius(radius)
{
}
//----------------------------------------------------------------------------
