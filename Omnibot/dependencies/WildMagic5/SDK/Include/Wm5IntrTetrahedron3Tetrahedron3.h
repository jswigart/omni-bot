// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRTETRAHEDRON3TETRAHEDRON3_H
#define WM5INTRTETRAHEDRON3TETRAHEDRON3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Tetrahedron3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrTetrahedron3Tetrahedron3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrTetrahedron3Tetrahedron3 (const Tetrahedron3<Real>& tetrahedron0,
        const Tetrahedron3<Real>& tetrahedron1);

    // Object access.
    const Tetrahedron3<Real>& GetTetrahedron0 () const;
    const Tetrahedron3<Real>& GetTetrahedron1 () const;

    // Static query.
    virtual bool Find ();

    // Information about the intersection set.
    const std::vector<Tetrahedron3<Real> >& GetIntersection () const;

private:
    static void SplitAndDecompose (Tetrahedron3<Real> tetra,
        const Plane3<Real>& plane, std::vector<Tetrahedron3<Real> >& Inside);

    // The objects to intersect.
    const Tetrahedron3<Real>* mTetrahedron0;
    const Tetrahedron3<Real>* mTetrahedron1;

    std::vector<Tetrahedron3<Real> > mIntersection;
};

typedef IntrTetrahedron3Tetrahedron3<float> IntrTetrahedron3Tetrahedron3f;
typedef IntrTetrahedron3Tetrahedron3<double> IntrTetrahedron3Tetrahedron3d;

}

#endif
