// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5QUERY3INT64_H
#define WM5QUERY3INT64_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Query3.h"

namespace Wm5
{

template <typename Real>
class Query3Int64 : public Query3<Real>
{
public:
    // The components of the input vertices are truncated to 64-bit integer
    // values, so you should guarantee that the vertices are sufficiently
    // large to give a good distribution of numbers.
    Query3Int64 (int numVertices, const Vector3<Real>* vertices);

    // Run-time type information.
    virtual Query::Type GetType () const;

    // Queries about the relation of a point to various geometric objects.

    virtual int ToPlane (const Vector3<Real>& test, int v0, int v1, int v2)
        const;

    virtual int ToCircumsphere (const Vector3<Real>& test, int v0, int v1,
        int v2, int v3) const;

    // Helper functions.
    static int64_t Dot (int64_t x0, int64_t y0, int64_t z0, int64_t x1,
        int64_t y1, int64_t z1);

    static int64_t Det3 (int64_t x0, int64_t y0, int64_t z0, int64_t x1,
        int64_t y1, int64_t z1, int64_t x2, int64_t y2, int64_t z2);

    static int64_t Det4 (int64_t x0, int64_t y0, int64_t z0, int64_t w0,
        int64_t x1, int64_t y1, int64_t z1, int64_t w1, int64_t x2,
        int64_t y2, int64_t z2, int64_t w2, int64_t x3, int64_t y3,
        int64_t z3, int64_t w3);

private:
    using Query3<Real>::mVertices;
};

#include "Wm5Query3Int64.inl"

typedef Query3Int64<float> Query3Int64f;
typedef Query3Int64<double> Query3Int64d;

}

#endif
