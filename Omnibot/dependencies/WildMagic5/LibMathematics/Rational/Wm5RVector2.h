// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5RVECTOR2_H
#define WM5RVECTOR2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5RVector.h"

namespace Wm5
{

template <int ISIZE>
class RVector2 : public RVector<2,ISIZE>
{
public:
    // Construction.
    RVector2 ();
    RVector2 (const RVector2& vec);
    RVector2 (const RVector<2,ISIZE>& vec);
    RVector2 (const Rational<ISIZE>& x, const Rational<ISIZE>& y);

    // Member access.
    inline Rational<ISIZE> X () const;
    inline Rational<ISIZE>& X ();
    inline Rational<ISIZE> Y () const;
    inline Rational<ISIZE>& Y ();

    // Assignment.
    RVector2& operator= (const RVector2& vec);
    RVector2& operator= (const RVector<2,ISIZE>& vec);

    // Returns Dot(this,V).
    Rational<ISIZE> Dot (const RVector2& vec) const;

    // Returns (y,-x).
    RVector2 Perp () const;

    // Returns Cross((x,y,0),(V.X(),V.Y(),0)) = x*V.Y() - y*V.X()
    Rational<ISIZE> DotPerp (const RVector2& vec) const;

protected:
    using RVector<2,ISIZE>::mTuple;
};

#include "Wm5RVector2.inl"

}

#endif
