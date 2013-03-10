// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5DISTRECTANGLE3RECTANGLE3_H
#define WM5DISTRECTANGLE3RECTANGLE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Distance.h"
#include "Wm5Rectangle3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM DistRectangle3Rectangle3
    : public Distance<Real,Vector3<Real> >
{
public:
    DistRectangle3Rectangle3 (const Rectangle3<Real>& rectangle0,
        const Rectangle3<Real>& rectangle1);

    // Object access.
    const Rectangle3<Real>& GetRectangle0 () const;
    const Rectangle3<Real>& GetRectangle1 () const;

    // Static distance queries.
    virtual Real Get ();
    virtual Real GetSquared ();

    // Function calculations for dynamic distance queries.
    virtual Real Get (Real t, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);
    virtual Real GetSquared (Real t, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);

private:
    using Distance<Real,Vector3<Real> >::mClosestPoint0;
    using Distance<Real,Vector3<Real> >::mClosestPoint1;

    const Rectangle3<Real>* mRectangle0;
    const Rectangle3<Real>* mRectangle1;
};

typedef DistRectangle3Rectangle3<float> DistRectangle3Rectangle3f;
typedef DistRectangle3Rectangle3<double> DistRectangle3Rectangle3d;

}

#endif
