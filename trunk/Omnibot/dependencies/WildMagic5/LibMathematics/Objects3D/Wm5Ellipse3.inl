// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Real>
Ellipse3<Real>::Ellipse3 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Ellipse3<Real>::~Ellipse3 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Ellipse3<Real>::Ellipse3 (const Vector3<Real>& center,
    const Vector3<Real>& normal, const Vector3<Real>& major,
    const Vector3<Real>& minor, Real majorLength, Real minorLength)
    :
    Center(center),
    Normal(normal),
    Major(major),
    Minor(minor),
    MajorLength(majorLength),
    MinorLength(minorLength)
{
}
//----------------------------------------------------------------------------
