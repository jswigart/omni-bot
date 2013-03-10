// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5DISTPOINT3TETRAHEDRON3_H
#define WM5DISTPOINT3TETRAHEDRON3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Distance.h"
#include "Wm5Tetrahedron3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM DistPoint3Tetrahedron3
    : public Distance<Real,Vector3<Real> >
{
public:
    DistPoint3Tetrahedron3 (const Vector3<Real>& point,
        const Tetrahedron3<Real>& tetrahedron);

    // Object access.
    const Vector3<Real>& GetPoint () const;
    const Tetrahedron3<Real>& GetTetrahedron () const;

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

    const Vector3<Real>* mPoint;
    const Tetrahedron3<Real>* mTetrahedron;
};

typedef DistPoint3Tetrahedron3<float> DistPoint3Tetrahedron3f;
typedef DistPoint3Tetrahedron3<double> DistPoint3Tetrahedron3d;

}

#endif
