// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5QUERY3RATIONAL_H
#define WM5QUERY3RATIONAL_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Query3.h"
#include "Wm5Memory.h"
#include "Wm5RVector3.h"

namespace Wm5
{

template <typename Real>
class Query3Rational : public Query3<Real>
{
public:
    // The components of the input vertices are represented exactly as
    // rational values.
    Query3Rational (int numVertices, const Vector3<Real>* vertices);
    virtual ~Query3Rational ();

    // run-time type information
    virtual Query::Type GetType () const;

    // Queries about the relation of a point to various geometric objects.

    virtual int ToPlane (int i, int v0, int v1, int v2) const;
    virtual int ToPlane (const Vector3<Real>& test, int v0, int v1, int v2)
        const;

    virtual int ToTetrahedron (int i, int v0, int v1, int v2, int v3) const;
    virtual int ToTetrahedron (const Vector3<Real>& test, int v0, int v1,
        int v2, int v3) const;

    virtual int ToCircumsphere (int i, int v0, int v1, int v2, int v3)
        const;
    virtual int ToCircumsphere (const Vector3<Real>& test, int v0, int v1,
        int v2, int v3) const;

    // Helper functions.
    typedef Rational<8*sizeof(Real)> QRational;
    typedef RVector3<8*sizeof(Real)> QRVector;

    static QRational Dot (QRational& x0, QRational& y0, QRational& z0,
        QRational& x1, QRational& y1, QRational& z1);

    static QRational Det3 (QRational& x0, QRational& y0, QRational& z0,
        QRational& x1, QRational& y1, QRational& z1, QRational& x2,
        QRational& y2, QRational& z2);

    static QRational Det4 (QRational& x0, QRational& y0, QRational& z0,
        QRational& w0, QRational& x1, QRational& y1, QRational& z1,
        QRational& w1, QRational& x2, QRational& y2, QRational& z2,
        QRational& w2, QRational& x3, QRational& y3, QRational& z3,
        QRational& w3);

private:
    using Query3<Real>::mNumVertices;
    using Query3<Real>::mVertices;

    void Convert (int numIndices, int* indices) const;
    int ToPlane (const QRVector& ratTest, int v0, int v1, int v2) const;
    int ToTetrahedron (const QRVector& ratTest, int v0, int v1, int v2,
        int v3) const;
    int ToCircumsphere (const QRVector& ratTest, int v0, int v1, int v2,
        int v3) const;

    // Caching for rational representations of the input.  The conversion of
    // floating-point numbers to Rational form is slow, so it is better to
    // keep track of which values have been converted.
    mutable QRVector* mRVertices;
    mutable bool* mEvaluated;
};

#include "Wm5Query3Rational.inl"

typedef Query3Rational<float> Query3Rationalf;
typedef Query3Rational<double> Query3Rationald;

}

#endif
