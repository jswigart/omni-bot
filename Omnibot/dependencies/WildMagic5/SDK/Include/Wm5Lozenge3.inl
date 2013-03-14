// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Real>
Lozenge3<Real>::Lozenge3 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Lozenge3<Real>::~Lozenge3 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Lozenge3<Real>::Lozenge3 (const Rectangle3<Real>& rectangle, Real radius)
    :
    Rectangle(rectangle),
    Radius(radius)
{
}
//----------------------------------------------------------------------------
