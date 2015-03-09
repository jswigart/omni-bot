// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5QUERY2_H
#define WM5QUERY2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Query.h"
#include "Wm5Vector2.h"

namespace Wm5
{

template <typename Real>
class Query2 : public Query
{
public:
    // The base class handles floating-point queries.
    Query2 (int numVertices, const Vector2<Real>* vertices);
    virtual ~Query2 ();

    // Run-time type information.
    virtual Query::Type GetType () const;

    // Member access.
    inline int GetNumVertices () const;
    inline const Vector2<Real>* GetVertices () const;

    // Queries about the relation of a point to various geometric objects.

    // Returns:
    //   +1, on right of line
    //   -1, on left of line
    //    0, on the line
    virtual int ToLine (int i, int v0, int v1) const;
    virtual int ToLine (const Vector2<Real>& test, int v0, int v1) const;

    // Returns:
    //   +1, outside triangle
    //   -1, inside triangle
    //    0, on triangle
    virtual int ToTriangle (int i, int v0, int v1, int v2) const;
    virtual int ToTriangle (const Vector2<Real>& test, int v0, int v1,
        int v2) const;

    // Returns:
    //   +1, outside circumcircle of triangle
    //   -1, inside circumcircle of triangle
    //    0, on circumcircle of triangle
    virtual int ToCircumcircle (int i, int v0, int v1, int v2) const;
    virtual int ToCircumcircle (const Vector2<Real>& test, int v0, int v1,
        int v2) const;

    // Helper functions.
    static Real Dot (Real x0, Real y0, Real x1, Real y1);
    static Real Det2 (Real x0, Real y0, Real x1, Real y1);
    static Real Det3 (Real x0, Real y0, Real z0, Real x1, Real y1, Real z1,
        Real x2, Real y2, Real z2);

protected:
    // Input points.
    int mNumVertices;
    const Vector2<Real>* mVertices;

};

#include "Wm5Query2.inl"

typedef Query2<float> Query2f;
typedef Query2<double> Query2d;

}

#endif
