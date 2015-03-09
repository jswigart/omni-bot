// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5DISTLINE2LINE2_H
#define WM5DISTLINE2LINE2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Distance.h"
#include "Wm5Line2.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM DistLine2Line2
    : public Distance<Real,Vector2<Real> >
{
public:
    DistLine2Line2 (const Line2<Real>& line0, const Line2<Real>& line1);

    // Object access.
    const Line2<Real>& GetLine0 () const;
    const Line2<Real>& GetLine1 () const;

    // Static distance queries.
    virtual Real Get ();
    virtual Real GetSquared ();

    // Function calculations for dynamic distance queries.
    virtual Real Get (Real t, const Vector2<Real>& velocity0,
        const Vector2<Real>& velocity1);
    virtual Real GetSquared (Real t, const Vector2<Real>& velocity0,
        const Vector2<Real>& velocity1);

private:
    using Distance<Real,Vector2<Real> >::mClosestPoint0;
    using Distance<Real,Vector2<Real> >::mClosestPoint1;

    const Line2<Real>* mLine0;
    const Line2<Real>* mLine1;
};

typedef DistLine2Line2<float> DistLine2Line2f;
typedef DistLine2Line2<double> DistLine2Line2d;

}

#endif
