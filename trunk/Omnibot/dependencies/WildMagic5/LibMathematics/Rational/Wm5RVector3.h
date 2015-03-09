// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5RVECTOR3_H
#define WM5RVECTOR3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5RVector.h"

namespace Wm5
{

template <int ISIZE>
class RVector3 : public RVector<3,ISIZE>
{
public:
    // Construction.
    RVector3 ();
    RVector3 (const RVector3& vec);
    RVector3 (const RVector<3,ISIZE>& vec);
    RVector3 (const Rational<ISIZE>& x, const Rational<ISIZE>& y,
        const Rational<ISIZE>& z);

    // Member access.
    inline Rational<ISIZE> X () const;
    inline Rational<ISIZE>& X ();
    inline Rational<ISIZE> Y () const;
    inline Rational<ISIZE>& Y ();
    inline Rational<ISIZE> Z () const;
    inline Rational<ISIZE>& Z ();

    // Assignment.
    RVector3& operator= (const RVector3& vec);
    RVector3& operator= (const RVector<3,ISIZE>& vec);

    // Returns Dot(this,V).
    Rational<ISIZE> Dot (const RVector3& vec) const;

    // Returns Cross(this,V).
    RVector3 Cross (const RVector3& vec) const;

    // Returns Dot(this,Cross(U,V)).
    Rational<ISIZE> TripleScalar (const RVector3& U, const RVector3& V) const;

protected:
    using RVector<3,ISIZE>::mTuple;
};

#include "Wm5RVector3.inl"

}

#endif
