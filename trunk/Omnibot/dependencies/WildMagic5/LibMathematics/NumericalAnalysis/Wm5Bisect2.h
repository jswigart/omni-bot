// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5BISECT2_H
#define WM5BISECT2_H

#include "Wm5MathematicsLIB.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM Bisect2
{
public:
    typedef Real (*Function)(Real,Real);

    Bisect2 (Function fFunction, Function gFunction, int maxLevel,
        Real tolerance);

    bool Bisect (Real x0, Real y0, Real x1, Real y1, Real& xRoot,
        Real& yRoot);

private:
    class BisectNode
    {
    public:
        BisectNode ();

        Real X, Y, F, G;
        BisectNode* XNext;
        BisectNode* YNext;
    };

    bool ZeroTest (Real x, Real y, Real& f, Real& g, Real& xRoot,
        Real& yRoot);

    static BisectNode* AddNode (Real x, Real y, Real f, Real g);

    // Input data and functions.
    Function mFFunction, mGFunction;
    int mLevel, mMaxLevel;
    Real mTolerance;
    Real mXRoot, mYRoot;

    // Fixed storage to avoid stack depletion during recursion.
    Real mX0, mXm, mX1, mY0, mYm, mY1;
    Real mF00, mF10, mF01, mF11, mF0m, mF1m, mFm0, mFm1;
    Real mFmm, mG00, mG10, mG01, mG11, mG0m, mG1m, mGm0;
    Real mGm1, mGmm;
    int mNetSign;

    // The graph and recursion routine for building it.
    BisectNode* mGraph;
    bool BisectRecurse (BisectNode* n00);
};

typedef Bisect2<float> Bisect2f;
typedef Bisect2<double> Bisect2d;

}

#endif
