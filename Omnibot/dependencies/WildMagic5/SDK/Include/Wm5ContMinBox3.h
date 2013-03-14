// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5MINVOLUMEBOX3_H
#define WM5MINVOLUMEBOX3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Box3.h"
#include "Wm5Query.h"

namespace Wm5
{
// Compute a minimum volume oriented box containing the specified points.
//
// This is a function class.  Use it as follows:
//   Box3<Real> minBox = MinBox3(numPoints, points, epsilon, queryType);

template <typename Real>
class WM5_MATHEMATICS_ITEM MinBox3
{
public:
    MinBox3 (int numPoints, const Vector3<Real>* points, Real epsilon,
        Query::Type queryType);

    operator Box3<Real> () const;

private:
    Box3<Real> mMinBox;
};

}

#endif
