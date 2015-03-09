// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2011/07/23)

#ifndef WM5QUERY3FILTERED_H
#define WM5QUERY3FILTERED_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Query3Rational.h"
#include "Wm5Vector3.h"

namespace Wm5
{

template <typename Real>
class Query3Filtered : public Query3<Real>
{
public:
    // The base class handles floating-point queries.  Each query involves
    // comparing a determinant to zero.  If the determinant is sufficiently
    // close to zero, numerical round-off errors may cause the determinant
    // sign to be misclassified.  To avoid this, the query is repeated with
    // exact rational arithmetic.  You specify the closeness to zero for the
    // switch to rational arithmetic via 'uncertainty', a value in the
    // interval [0,1].  The uncertainty of 0 causes the class to behave
    // as if it were Query3.  The uncertainty of 1 causes the class to
    // behave as if it were Query3Rational.
    Query3Filtered (int numVertices, const Vector3<Real>* vertices,
        Real uncertainty);
    virtual ~Query3Filtered ();

    // Run-time type information.
    virtual Query::Type GetType () const;

    // Queries about the relation of a point to various geometric objects.

    virtual int ToPlane (const Vector3<Real>& test, int v0, int v1, int v2)
        const;

    virtual int ToCircumsphere (const Vector3<Real>& test, int v0, int v1,
        int v2, int v3) const;

private:
    using Query3<Real>::mVertices;
    using Query3<Real>::Det3;
    using Query3<Real>::Det4;

    Query3Rational<Real> mRQuery;
    Real mUncertainty;
};

#include "Wm5Query3Filtered.inl"

typedef Query3Filtered<float> Query3Filteredf;
typedef Query3Filtered<double> Query3Filteredd;

}

#endif
