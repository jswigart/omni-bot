// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5QUERY3_H
#define WM5QUERY3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Query.h"
#include "Wm5Vector3.h"

namespace Wm5
{

template <typename Real>
class Query3 : public Query
{
public:
    // The base class handles floating-point queries.
    Query3 (int numVertices, const Vector3<Real>* vertices);
    virtual ~Query3 ();

    // Run-time type information.
    virtual Query::Type GetType () const;

    // Member access.
    inline int GetNumVertices () const;
    inline const Vector3<Real>* GetVertices () const;

    // Queries about the relation of a point to various geometric objects.

    // Returns:
    //   +1, on positive side of plane
    //   -1, on negative side of line
    //    0, on the plane
    virtual int ToPlane (int i, int v0, int v1, int v2) const;
    virtual int ToPlane (const Vector3<Real>& test, int v0, int v1, int v2)
        const;

    // Returns:
    //   +1, outside tetrahedron
    //   -1, inside tetrahedron
    //    0, on tetrahedron
    virtual int ToTetrahedron (int i, int v0, int v1, int v2, int iV3)
        const;
    virtual int ToTetrahedron (const Vector3<Real>& test, int v0, int v1,
        int v2, int iV3) const;

    // Returns:
    //   +1, outside circumsphere of tetrahedron
    //   -1, inside circumsphere of tetrahedron
    //    0, on circumsphere of tetrahedron
    virtual int ToCircumsphere (int i, int v0, int v1, int v2, int iV3)
        const;
    virtual int ToCircumsphere (const Vector3<Real>& test, int v0, int v1,
        int v2, int iV3) const;

    // Helper functions.
    static Real Dot (Real x0, Real y0, Real z0, Real x1, Real y1, Real z1);

    static Real Det3 (Real x0, Real y0, Real z0, Real x1, Real y1, Real z1,
        Real x2, Real y2, Real z2);

    static Real Det4 (Real x0, Real y0, Real z0, Real w0, Real x1, Real y1,
        Real z1, Real w1, Real x2, Real y2, Real z2, Real w2, Real x3,
        Real y3, Real z3, Real w3);

protected:
    // Input points.
    int mNumVertices;
    const Vector3<Real>* mVertices;

};

#include "Wm5Query3.inl"

typedef Query3<float> Query3f;
typedef Query3<double> Query3d;

}

#endif
