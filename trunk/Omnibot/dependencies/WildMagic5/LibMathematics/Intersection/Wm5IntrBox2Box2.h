// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRBOX2BOX2_H
#define WM5INTRBOX2BOX2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Box2.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrBox2Box2
    : public Intersector<Real,Vector2<Real> >
{
public:
    IntrBox2Box2 (const Box2<Real>& box0, const Box2<Real>& box1);

    // Object access.
    const Box2<Real>& GetBox0 () const;
    const Box2<Real>& GetBox1 () const;

    // Static test-intersection query.
    virtual bool Test ();

private:
    // the objects to intersect
    const Box2<Real>* mBox0;
    const Box2<Real>* mBox1;
};

typedef IntrBox2Box2<float> IntrBox2Box2f;
typedef IntrBox2Box2<double> IntrBox2Box2d;

}

#endif
