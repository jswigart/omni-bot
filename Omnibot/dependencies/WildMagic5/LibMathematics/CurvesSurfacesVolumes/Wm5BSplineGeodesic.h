// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5BSPLINEGEODESIC_H
#define WM5BSPLINEGEODESIC_H

#include "Wm5MathematicsLIB.h"
#include "Wm5RiemannianGeodesic.h"
#include "Wm5BSplineRectangle.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM BSplineGeodesic : public RiemannianGeodesic<Real>
{
public:
    BSplineGeodesic (const BSplineRectangle<Real>& spline);
    virtual ~BSplineGeodesic ();

    virtual void ComputeMetric (const GVector<Real>& point);
    virtual void ComputeChristoffel1 (const GVector<Real>& point);

private:
    using RiemannianGeodesic<Real>::mMetric;
    using RiemannianGeodesic<Real>::mChristoffel1;

    const BSplineRectangle<Real>* mSpline;
};

typedef BSplineGeodesic<float> BSplineGeodesicf;
typedef BSplineGeodesic<double> BSplineGeodesicd;

}

#endif
