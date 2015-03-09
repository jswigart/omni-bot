// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5QUERY3INTEGER_H
#define WM5QUERY3INTEGER_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Query3.h"
#include "Wm5Integer.h"

namespace Wm5
{

template <typename Real>
class Query3Integer : public Query3<Real>
{
public:
    // The components of the input vertices are truncated to 32-bit integer
    // values, so you should guarantee that the vertices are sufficiently
    // large to give a good distribution of numbers.  The value N in
    // Integer<N> is chosen large enough so that the exact arithmetic is
    // correct for the functions.
    Query3Integer (int iVQuantity, const Vector3<Real>* akVertex);

    // Run-time type information.
    virtual Query::Type GetType () const;

    // Queries about the relation of a point to various geometric objects.

    virtual int ToPlane (const Vector3<Real>& test, int v0, int v1, int v2)
        const;

    virtual int ToCircumsphere (const Vector3<Real>& test, int v0, int v1,
        int v2, int v3) const;

    // Helper functions.
    static Integer<3> Dot (Integer<3>& x0, Integer<3>& y0, Integer<3>& z0,
        Integer<3>& x1, Integer<3>& y1, Integer<3>& z1);

    static Integer<4> Det3 (Integer<4>& x0, Integer<4>& y0, Integer<4>& z0,
        Integer<4>& x1, Integer<4>& y1, Integer<4>& z1, Integer<4>& x2,
        Integer<4>& y2, Integer<4>& z2);

    static Integer<6> Det4 (Integer<6>& x0, Integer<6>& y0, Integer<6>& z0,
        Integer<6>& w0, Integer<6>& x1, Integer<6>& y1, Integer<6>& z1,
        Integer<6>& w1, Integer<6>& x2, Integer<6>& y2, Integer<6>& z2,
        Integer<6>& w2, Integer<6>& x3, Integer<6>& y3, Integer<6>& z3,
        Integer<6>& w3);

private:
    using Query3<Real>::mVertices;
};

#include "Wm5Query3Integer.inl"

typedef Query3Integer<float> Query3Integerf;
typedef Query3Integer<double> Query3Integerd;

}

#endif
