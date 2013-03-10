// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5POLYGON2_H
#define WM5POLYGON2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Vector2.h"
#include "Wm5SmartPointer.h"

namespace Wm5
{

template <typename Real>
class Polygon2
{
public:
    // Construction and destruction.  The caller is responsible for ensuring
    // that the array of vertices is counterclockwise ordered to represent a
    // simple polygon.
    //
    // Polygon2 objects typically share data from other sources.  The class
    // uses a smart pointer (Pointer1) to share the input vertices.  If you
    // do not want the class to delete the vertices, use your own smart
    // pointer for the vertices.

    Polygon2 (const int numVertices, Vector2<Real>* vertices);
    Polygon2 (const Polygon2& polygon);
    virtual ~Polygon2 ();

    // Assignment.
    Polygon2& operator= (const Polygon2& polygon);

    // Read-only member access.
    int GetNumVertices () const;
    const Vector2<Real>* GetVertices () const;
    const Vector2<Real>& GetVertex (int i) const;

    // Allow vertex modification.  The caller must ensure that the polygon
    // remains simple.
    virtual void SetVertex (int i, const Vector2<Real>& vertex);

    // Compute various information about the polygon.
    Vector2<Real> ComputeVertexAverage () const;
    Real ComputePerimeterLength () const;
    Real ComputeArea () const;

protected:
    int mNumVertices;
    Pointer1<Vector2<Real> > mVertices;
};

#include "Wm5Polygon2.inl"

typedef Polygon2<float> Polygon2f;
typedef Polygon2<double> Polygon2d;

}

#endif
