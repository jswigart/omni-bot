// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5BISECT3_H
#define WM5BISECT3_H

#include "Wm5MathematicsLIB.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM Bisect3
{
public:
    typedef Real (*Function)(Real,Real,Real);

    Bisect3 (Function fFunction, Function gFunction, Function hFunction,
        int maxLevel, Real tolerance);

    bool Bisect (Real x0, Real y0, Real z0, Real x1, Real y1, Real z1,
        Real& xRoot, Real& yRoot, Real& zRoot);

private:
    class BisectNode
    {
    public:
        BisectNode ();

        Real X, Y, Z, F, G, H;
        BisectNode* XNext;
        BisectNode* YNext;
        BisectNode* ZNext;
    };

    bool ZeroTest (Real x, Real y, Real z, Real& f, Real& g, Real& h,
        Real& xRoot, Real& yRoot, Real& zRoot);

    static BisectNode* AddNode (Real x, Real y, Real z, Real f, Real g,
        Real h);

    // Input data and functions.
    Function mFFunction, mGFunction, mHFunction;
    int mLevel, mMaxLevel;
    Real mTolerance;
    Real mXRoot, mYRoot, mZRoot;

    // Vertex and midpoint locations.
    Real mX0, mXm, mX1, mY0, mYm, mY1, mZ0, mZm, mZ1;

    // vertices
    Real mF000, mF100, mF010, mF110;
    Real mF001, mF101, mF011, mF111;
    Real mG000, mG100, mG010, mG110;
    Real mG001, mG101, mG011, mG111;
    Real mH000, mH100, mH010, mH110;
    Real mH001, mH101, mH011, mH111;

    // edges
    Real mF00m, mF10m, mF01m, mF11m, mF0m0, mF1m0, mF0m1, mF1m1, mFm00, mFm10;
    Real mFm01, mFm11;
    Real mG00m, mG10m, mG01m, mG11m, mG0m0, mG1m0, mG0m1, mG1m1, mGm00, mGm10;
    Real mGm01, mGm11;
    Real mH00m, mH10m, mH01m, mH11m, mH0m0, mH1m0, mH0m1, mH1m1, mHm00, mHm10;
    Real mHm01, mHm11;

    // faces
    Real mF0mm, mFm0m, mFmm0, mF1mm, mFm1m, mFmm1;
    Real mG0mm, mGm0m, mGmm0, mG1mm, mGm1m, mGmm1;
    Real mH0mm, mHm0m, mHmm0, mH1mm, mHm1m, mHmm1;

    // center
    Real mFmmm, mGmmm, mHmmm;

    int mNetSign;

    // the graph and recursion routine for building it
    BisectNode* mGraph;
    bool BisectRecurse (BisectNode* n000);
};

typedef Bisect3<float> Bisect3f;
typedef Bisect3<double> Bisect3d;

}

#endif
