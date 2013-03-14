// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5QUERY2INTEGER_H
#define WM5QUERY2INTEGER_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Query2.h"
#include "Wm5Integer.h"

namespace Wm5
{

template <typename Real>
class Query2Integer : public Query2<Real>
{
public:
    // The components of the input vertices are truncated to 32-bit integer
    // values, so you should guarantee that the vertices are sufficiently
    // large to give a good distribution of numbers.  The value N in
    // Integer<N> is chosen large enough so that the exact arithmetic is
    // correct for the functions.
    Query2Integer (int numVertices, const Vector2<Real>* vertices);

    // Run-time type information.
    virtual Query::Type GetType () const;

    // Queries about the relation of a point to various geometric objects.

    virtual int ToLine (const Vector2<Real>& test, int v0, int v1) const;

    virtual int ToCircumcircle (const Vector2<Real>& test, int v0, int v1,
        int v2) const;

    // Helper functions.
    static Integer<2> Dot (Integer<2>& x0, Integer<2>& y0, Integer<2>& x1,
        Integer<2>& y1);

    static Integer<2> Det2 (Integer<2>& x0, Integer<2>& y0, Integer<2>& x1,
        Integer<2>& y1);

    static Integer<4> Det3 (Integer<4>& x0, Integer<4>& y0, Integer<4>& z0,
        Integer<4>& x1, Integer<4>& y1, Integer<4>& z1, Integer<4>& x2,
        Integer<4>& y2, Integer<4>& z2);

private:
    using Query2<Real>::mVertices;
};

#include "Wm5Query2Integer.inl"

typedef Query2Integer<float> Query2Integerf;
typedef Query2Integer<double> Query2Integerd;

}

#endif
