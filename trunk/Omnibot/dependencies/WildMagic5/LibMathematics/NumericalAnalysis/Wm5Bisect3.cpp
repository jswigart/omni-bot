// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5Bisect3.h"
#include "Wm5Math.h"
#include "Wm5Memory.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
Bisect3<Real>::Bisect3 (Function fFunction, Function gFunction,
    Function hFunction, int maxLevel, Real tolerance)
    :
    mFFunction(fFunction),
    mGFunction(gFunction),
    mHFunction(hFunction),
    mLevel(0),
    mMaxLevel(maxLevel),
    mTolerance(tolerance)
{
}
//----------------------------------------------------------------------------
template <typename Real>
bool Bisect3<Real>::ZeroTest (Real x, Real y, Real z, Real& f, Real& g,
    Real& h, Real& xRoot, Real& yRoot, Real& zRoot)
{
    f = mFFunction(x,y,z);
    g = mGFunction(x,y,z);
    h = mHFunction(x,y,z);

    if (Math<Real>::FAbs(f) <= mTolerance
    &&  Math<Real>::FAbs(g) <= mTolerance
    &&  Math<Real>::FAbs(h) <= mTolerance)
    {
        xRoot = x;
        yRoot = y;
        zRoot = z;
        --mLevel;
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
typename Bisect3<Real>::BisectNode* Bisect3<Real>::AddNode (Real x, Real y,
    Real z, Real f, Real g, Real h)
{
    BisectNode* node = new0 BisectNode();
    node->X = x;
    node->Y = y;
    node->Z = z;
    node->F = f;
    node->G = g;
    node->H = h;
    return node;
}
//----------------------------------------------------------------------------
template <typename Real>
bool Bisect3<Real>::Bisect (Real x0, Real y0, Real z0, Real x1, Real y1,
    Real z1, Real& xRoot, Real& yRoot, Real& zRoot)
{
    // Test the eight corner values.
    if (ZeroTest(x0, y0, z0, mF000, mG000, mH000, xRoot, yRoot, zRoot))
    {
        return true;
    }

    if (ZeroTest(x1, y0, z0, mF100, mG100, mH100, xRoot, yRoot, zRoot))
    {
        return true;
    }

    if (ZeroTest(x0, y1, z0, mF010, mG010, mH010, xRoot, yRoot, zRoot))
    {
        return true;
    }

    if (ZeroTest(x1, y1, z0, mF110, mG110, mH110, xRoot, yRoot, zRoot))
    {
        return true;
    }

    if (ZeroTest(x0, y0, z1, mF001, mG001, mH001, xRoot, yRoot, zRoot))
    {
        return true;
    }

    if (ZeroTest(x1, y0, z1, mF101, mG101, mH101, xRoot, yRoot, zRoot))
    {
        return true;
    }

    if (ZeroTest(x0, y1, z1, mF011, mG011, mH011, xRoot, yRoot, zRoot))
    {
        return true;
    }

    if (ZeroTest(x1, y1, z1, mF111, mG111, mH111, xRoot, yRoot, zRoot))
    {
        return true;
    }

    // Build the initial oct.

    // Add N000.
    mGraph = new0 BisectNode();
    mGraph->X = x0;
    mGraph->Y = y0;
    mGraph->Z = z0;
    mGraph->F = mF000;
    mGraph->G = mG000;
    mGraph->H = mH000;

    // Add N100.
    BisectNode* temp = AddNode(x1, y0, z0, mF100, mG100, mH100);
    temp->XNext = 0;
    mGraph->XNext = temp;

    // Add N010.
    temp = AddNode(x0, y1, z0, mF010, mG010, mH010);
    temp->YNext = 0;
    mGraph->YNext = temp;

    // Add N110.
    AddNode(x1, y1, z0, mF110, mG110, mH110);
    temp->XNext = 0;
    temp->YNext = 0;
    mGraph->XNext->YNext = temp;
    mGraph->YNext->XNext = temp;

    // Add N001.
    temp = AddNode(x0, y1, z1, mF001, mG001, mH001);
    temp->ZNext = 0;
    mGraph->ZNext = temp;

    // Add N101.
    temp = AddNode(x1, y0, z1, mF101, mG101, mH101);
    temp->XNext = 0;
    temp->ZNext = 0;
    mGraph->XNext->ZNext = temp;
    mGraph->ZNext->XNext = temp;

    // Add N011.
    temp = AddNode(x0, y1, z1, mF011, mG011, mH011);
    temp->YNext = 0;
    temp->ZNext = 0;
    mGraph->YNext->ZNext = temp;
    mGraph->ZNext->YNext = temp;

    // Add N111.
    temp = AddNode(x1, y1, z1, mF111, mG111, mH111);
    mGraph->XNext->YNext->ZNext = temp;
    mGraph->YNext->XNext->ZNext = temp;
    mGraph->XNext->ZNext->YNext = temp;

    bool result = BisectRecurse(mGraph);
    if (result)
    {
        xRoot = mXRoot;
        yRoot = mYRoot;
        zRoot = mZRoot;
    }

    // Remove the remaining oct from mGraph.
    delete0(mGraph->XNext->YNext->ZNext);
    delete0(mGraph->XNext->ZNext);
    delete0(mGraph->YNext->ZNext);
    delete0(mGraph->ZNext);
    delete0(mGraph->XNext->YNext);
    delete0(mGraph->XNext);
    delete0(mGraph->YNext);
    delete0(mGraph);

    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
bool Bisect3<Real>::BisectRecurse (BisectNode* n000)
{
    if (++mLevel == mMaxLevel)
    {
        --mLevel;
        return false;
    }

    BisectNode* n100 = n000->XNext;
    BisectNode* n010 = n000->YNext;
    BisectNode* n110 = n100->YNext;
    BisectNode* n001 = n000->ZNext;
    BisectNode* n101 = n001->XNext;
    BisectNode* n011 = n001->YNext;
    BisectNode* n111 = n101->YNext;

    mNetSign = (int)(
        Math<Real>::Sign(n000->F) +
        Math<Real>::Sign(n010->F) +
        Math<Real>::Sign(n100->F) +
        Math<Real>::Sign(n110->F) +
        Math<Real>::Sign(n001->F) +
        Math<Real>::Sign(n011->F) +
        Math<Real>::Sign(n101->F) +
        Math<Real>::Sign(n111->F));

    if (abs(mNetSign) == 8)
    {
        // F has the same sign at corners.
        --mLevel;
        return false;
    }

    mNetSign = (int)(
        Math<Real>::Sign(n000->G) +
        Math<Real>::Sign(n010->G) +
        Math<Real>::Sign(n100->G) +
        Math<Real>::Sign(n110->G) +
        Math<Real>::Sign(n001->G) +
        Math<Real>::Sign(n011->G) +
        Math<Real>::Sign(n101->G) +
        Math<Real>::Sign(n111->G));

    if (abs(mNetSign) == 8)
    {
        // G has the same sign at corners.
        --mLevel;
        return false;
    }

    mNetSign = (int)(
        Math<Real>::Sign(n000->H) +
        Math<Real>::Sign(n010->H) +
        Math<Real>::Sign(n100->H) +
        Math<Real>::Sign(n110->H) +
        Math<Real>::Sign(n001->H) +
        Math<Real>::Sign(n011->H) +
        Math<Real>::Sign(n101->H) +
        Math<Real>::Sign(n111->H));

    if (abs(mNetSign) == 8)
    {
        // H has the same sign at corners.
        --mLevel;
        return false;
    }

    // Bisect the oct.
    mX0 = n000->X;
    mY0 = n000->Y;
    mZ0 = n000->Z;
    mX1 = n111->X;
    mY1 = n111->Y;
    mZ1 = n111->Z;
    mXm = ((Real)0.5)*(mX0 + mX1);
    mYm = ((Real)0.5)*(mY0 + mY1);
    mZm = ((Real)0.5)*(mZ0 + mZ1);

    // edge 011,111
    if (ZeroTest(mXm, mY1, mZ1, mFm11, mGm11, mHm11, mXRoot, mYRoot, mZRoot))
    {
        return true;
    }

    // edge 101, 111
    if (ZeroTest(mX1, mYm, mZ1, mF1m1, mG1m1, mH1m1, mXRoot, mYRoot, mZRoot))
    {
        return true;
    }

    // edge 110, 111
    if (ZeroTest(mX1, mY1, mZm, mF11m, mG11m, mH11m, mXRoot, mYRoot, mZRoot))
    {
        return true;
    }

    // edge 010, 110
    if (ZeroTest(mXm, mY1, mZ0, mFm10, mGm10, mHm10, mXRoot, mYRoot, mZRoot))
    {
        return true;
    }

    // edge 100, 110
    if (ZeroTest(mX1, mYm, mZ0, mF1m0, mG1m0, mH1m0, mXRoot, mYRoot, mZRoot))
    {
        return true;
    }

    // edge 100, 101
    if (ZeroTest(mX1, mY0, mZm, mF10m, mG10m, mH10m, mXRoot, mYRoot, mZRoot))
    {
        return true;
    }

    // edge 001, 101
    if (ZeroTest(mXm, mY0, mZ1, mFm01, mGm01, mHm01, mXRoot, mYRoot, mZRoot))
    {
        return true;
    }

    // edge 001, 011
    if (ZeroTest(mX0, mYm, mZ1, mF0m1, mG0m1, mH0m1, mXRoot, mYRoot, mZRoot))
    {
        return true;
    }

    // edge 010, 011
    if (ZeroTest(mX0, mY1, mZm, mF01m, mG01m, mH01m, mXRoot, mYRoot, mZRoot))
    {
        return true;
    }

    // edge 000, 100
    if (ZeroTest(mXm, mY0, mZ0, mFm00, mGm00, mHm00, mXRoot, mYRoot, mZRoot))
    {
        return true;
    }

    // edge 000, 010
    if (ZeroTest(mX0, mYm, mZ0, mF0m0, mG0m0, mH0m0, mXRoot, mYRoot, mZRoot))
    {
        return true;
    }

    // edge 000, 001
    if (ZeroTest(mX0, mY0, mZm, mF00m, mG00m, mH00m, mXRoot, mYRoot, mZRoot))
    {
        return true;
    }

    // face 110, 100, 101, 111
    if (ZeroTest(mX1, mYm, mZm, mF1mm, mG1mm, mH1mm, mXRoot, mYRoot, mZRoot))
    {
        return true;
    }

    // face 010, 110, 111, 011
    if (ZeroTest(mXm, mY1, mZm, mFm1m, mGm1m, mHm1m, mXRoot, mYRoot, mZRoot))
    {
        return true;
    }

    // face 001, 101, 111, 011
    if (ZeroTest(mXm, mYm, mZ1, mFmm1, mGmm1, mHmm1, mXRoot, mYRoot, mZRoot))
    {
        return true;
    }

    // face 000, 010, 011, 001
    if (ZeroTest(mX0, mYm, mZm, mF0mm, mG0mm, mH0mm, mXRoot, mYRoot, mZRoot))
    {
        return true;
    }

    // face 000, 100, 001, 101
    if (ZeroTest(mXm, mY0, mZm, mFm0m, mGm0m, mHm0m, mXRoot, mYRoot, mZRoot))
    {
        return true;
    }

    // face 000, 100, 110, 010
    if (ZeroTest(mXm, mYm, mZ0, mFmm0, mGmm0, mHmm0, mXRoot, mYRoot, mZRoot))
    {
        return true;
    }

    // center
    if (ZeroTest(mXm, mYm, mZm, mFmmm, mGmmm, mHmmm, mXRoot, mYRoot, mZRoot))
    {
        return true;
    }

    // edge 011,111
    BisectNode* temp = AddNode(mXm, mY1, mZ1, mFm11, mGm11, mHm11);
    temp->XNext = n111;
    temp->YNext = 0;
    temp->ZNext = 0;
    n011->XNext = temp;

    // edge 101, 111
    temp = AddNode(mX1, mYm, mZ1, mF1m1, mG1m1, mH1m1);
    temp->XNext = 0;
    temp->YNext = n111;
    temp->ZNext = 0;
    n101->YNext = temp;

    // edge 110, 111
    temp = AddNode(mX1, mY1, mZm, mF11m, mG11m, mH11m);
    temp->XNext = 0;
    temp->YNext = 0;
    temp->ZNext = n111;
    n110->ZNext = temp;

    // edge 010, 110
    temp = AddNode(mXm, mY1, mZ0, mFm10, mGm10, mHm10);
    temp->XNext = n110;
    temp->YNext = 0;
    n010->XNext = temp;

    // edge 100, 110
    temp = AddNode(mX1, mYm, mZ1, mF1m0, mG1m0, mH1m0);
    temp->XNext = 0;
    temp->YNext = n110;
    n100->YNext = temp;

    // edge 100, 101
    temp = AddNode(mX1, mY0, mZm, mF10m, mG10m, mH10m);
    temp->XNext = 0;
    temp->ZNext = n101;
    n100->ZNext = temp;

    // edge 001, 101
    temp = AddNode(mXm, mY0, mZ1, mFm01, mGm01, mHm01);
    temp->XNext = n101;
    temp->ZNext = 0;
    n001->XNext = temp;

    // edge 001, 011
    temp = AddNode(mX0, mYm, mZ1, mF0m1, mG0m1, mH0m1);
    temp->YNext = n011;
    temp->ZNext = 0;
    n001->YNext = temp;

    // edge 010, 011
    temp = AddNode(mX0, mY1, mZm, mF01m, mG01m, mH01m);
    temp->YNext = 0;
    temp->ZNext = n011;
    n010->ZNext = temp;

    // edge 000, 100
    temp = AddNode(mXm, mY0, mZ0, mFm00, mGm00, mHm00);
    temp->XNext = n100;
    n000->XNext = temp;

    // edge 000, 010
    temp = AddNode(mX0, mYm, mZ0, mF0m0, mG0m0, mH0m0);
    temp->YNext = n010;
    n000->YNext = temp;

    // edge 000, 001
    temp = AddNode(mX0, mY0, mZm, mF00m, mG00m, mH00m);
    temp->ZNext = n001;
    n000->ZNext = temp;

    // face 110, 100, 101, 111
    temp = AddNode(mX1, mYm, mZm, mF11m, mG11m, mH11m);
    temp->XNext = 0;
    temp->YNext = n110->ZNext;
    temp->ZNext = n101->YNext;
    n100->YNext->ZNext = temp;
    n100->ZNext->YNext = temp;

    // face 010, 110, 111, 011
    temp = AddNode(mXm, mY1, mZm, mFm1m, mGm1m, mHm1m);
    temp->XNext = n110->ZNext;
    temp->YNext = 0;
    temp->ZNext = n011->XNext;
    n010->XNext->ZNext = temp;
    n010->ZNext->XNext = temp;

    // face 001, 101, 111, 011
    temp = AddNode(mXm, mYm, mZ1, mFmm1, mGmm1, mHmm1);
    temp->XNext = n101->YNext;
    temp->YNext = n011->XNext;
    temp->ZNext = 0;
    n001->XNext->YNext = temp;
    n001->YNext->XNext = temp;

    // face 000, 010, 011, 001
    temp = AddNode(mX0, mYm, mZm, mF0mm, mG0mm, mH0mm);
    temp->YNext = n010->ZNext;
    temp->ZNext = n001->YNext;
    n000->YNext->ZNext = temp;
    n000->ZNext->YNext = temp;

    // face 000, 100, 001, 101
    temp = AddNode(mXm, mY0, mZm, mFm0m, mGm0m, mHm0m);
    temp->XNext = n100->ZNext;
    temp->ZNext = n001->XNext;
    n000->XNext->ZNext = temp;
    n000->ZNext->XNext = temp;

    // face 000, 100, 110, 010
    temp = AddNode(mXm, mYm, mZ0, mFmm0, mGmm0, mHmm0);
    temp->XNext = n100->YNext;
    temp->YNext = n010->XNext;
    n000->XNext->YNext = temp;
    n000->YNext->XNext = temp;

    // center
    temp = AddNode(mXm, mYm, mZm, mFmmm, mGmmm, mHmmm);
    temp->XNext = n100->YNext->ZNext;
    temp->YNext = n010->XNext->ZNext;
    temp->ZNext = n001->XNext->YNext;
    n000->XNext->YNext->ZNext = temp;
    n000->XNext->ZNext->YNext = temp;
    n000->YNext->ZNext->XNext = temp;

    // Search the subocts for roots.
    bool result =
        BisectRecurse(n000) ||
        BisectRecurse(n000->XNext) ||
        BisectRecurse(n000->YNext) ||
        BisectRecurse(n000->XNext->YNext) ||
        BisectRecurse(n000->ZNext) ||
        BisectRecurse(n000->ZNext->XNext) ||
        BisectRecurse(n000->ZNext->YNext) ||
        BisectRecurse(n000->ZNext->XNext->YNext);

    // The entire suboct check failed, remove the nodes that were added.

    // center
    delete0(n000->XNext->YNext->ZNext);

    // faces
    delete0(n000->XNext->YNext);
    delete0(n000->YNext->ZNext);
    delete0(n000->XNext->ZNext);
    delete0(n001->XNext->YNext);
    delete0(n010->XNext->ZNext);
    delete0(n100->YNext->ZNext);

    // edges
    delete0(n000->XNext);
    n000->XNext = n100;
    delete0(n000->YNext);
    n000->YNext = n010;
    delete0(n000->ZNext);
    n000->ZNext = n001;
    delete0(n001->YNext);
    n001->YNext = n011;
    delete0(n001->XNext);
    n001->XNext = n101;
    delete0(n010->ZNext);
    n010->ZNext = n011;
    delete0(n100->ZNext);
    n100->ZNext = n101;
    delete0(n010->XNext);
    n010->XNext = n110;
    delete0(n100->YNext);
    n100->YNext = n110;
    delete0(n011->XNext);
    n011->XNext = n111;
    delete0(n101->YNext);
    n101->YNext = n111;
    delete0(n110->ZNext);
    n110->ZNext = n111;

    --mLevel;
    return result;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Bisect3::BisectNode
//----------------------------------------------------------------------------
template <typename Real>
Bisect3<Real>::BisectNode::BisectNode ()
    :
    X((Real)0),
    Y((Real)0),
    Z((Real)0),
    F((Real)0),
    G((Real)0),
    H((Real)0),
    XNext(0),
    YNext(0),
    ZNext(0)
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class Bisect3<float>;

template WM5_MATHEMATICS_ITEM
class Bisect3<double>;
//----------------------------------------------------------------------------
}
