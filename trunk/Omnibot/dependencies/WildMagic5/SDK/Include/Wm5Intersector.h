// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTERSECTOR_H
#define WM5INTERSECTOR_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Vector2.h"
#include "Wm5Vector3.h"

namespace Wm5
{

template <typename Real, typename TVector>
class WM5_MATHEMATICS_ITEM Intersector
{
public:
    // Abstract base class.
    virtual ~Intersector ();

    // Static intersection queries.  The default implementations return
    // 'false'.  The Find query produces a set of intersection.  The derived
    // class is responsible for providing access to that set, since the nature
    // of the set is dependent on the object types.
    virtual bool Test ();
    virtual bool Find ();

    // Dynamic intersection queries.  The default implementations return
    // 'false'.  The Find query produces a set of first contact.  The derived
    // class is responsible for providing access to that set, since the nature
    // of the set is dependent on the object types.
    virtual bool Test (Real tmax, const TVector& velocity0,
        const TVector& velocity1);
    virtual bool Find (Real tmax, const TVector& velocity0,
        const TVector& velocity1);

    // The time at which two objects are in first contact for the dynamic
    // intersection queries.
    Real GetContactTime () const;

    // Information about the intersection set
    enum
    {
        IT_EMPTY,
        IT_POINT,
        IT_SEGMENT,
        IT_RAY,
        IT_LINE,
        IT_POLYGON,
        IT_PLANE,
        IT_POLYHEDRON,
        IT_OTHER
    };
    int GetIntersectionType () const;

protected:
    Intersector ();

    int mIntersectionType;
    Real mContactTime;
};

typedef Intersector<float, Vector2<float> > Intersector2f;
typedef Intersector<float, Vector3<float> > Intersector3f;
typedef Intersector<double, Vector2<double> > Intersector2d;
typedef Intersector<double, Vector3<double> > Intersector3d;

}

#endif
