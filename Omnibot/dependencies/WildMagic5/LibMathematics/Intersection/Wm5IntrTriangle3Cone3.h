// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRTRIANGLE3CONE3_H
#define WM5INTRTRIANGLE3CONE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Triangle3.h"
#include "Wm5Cone3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrTriangle3Cone3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrTriangle3Cone3 (const Triangle3<Real>& triangle,
        const Cone3<Real>& cone);

    // Object access.
    const Triangle3<Real>& GetTriangle () const;
    const Cone3<Real>& GetCone () const;

    // Static query.
    virtual bool Test ();

private:
    // The objects to intersect.
    const Triangle3<Real>* mTriangle;
    const Cone3<Real>* mCone;
};

typedef IntrTriangle3Cone3<float> IntrTriangle3Cone3f;
typedef IntrTriangle3Cone3<double> IntrTriangle3Cone3d;

}

#endif
