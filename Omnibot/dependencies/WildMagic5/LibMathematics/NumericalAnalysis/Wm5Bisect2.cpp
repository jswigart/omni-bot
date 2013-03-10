// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5Bisect2.h"
#include "Wm5Math.h"
#include "Wm5Memory.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
Bisect2<Real>::Bisect2 (Function fFunction, Function gFunction, int maxLevel,
    Real tolerance)
    :
    mFFunction(fFunction),
    mGFunction(gFunction),
    mLevel(0),
    mMaxLevel(maxLevel),
    mTolerance(tolerance)
{
}
//----------------------------------------------------------------------------
template <typename Real>
bool Bisect2<Real>::ZeroTest (Real x, Real y, Real& f, Real& g,
    Real& xRoot, Real& yRoot)
{
    f = mFFunction(x, y);
    g = mGFunction(x, y);

    if (Math<Real>::FAbs(f) <= mTolerance
    &&  Math<Real>::FAbs(g) <= mTolerance)
    {
        xRoot = x;
        yRoot = y;
        --mLevel;
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
typename Bisect2<Real>::BisectNode* Bisect2<Real>::AddNode (Real x, Real y,
    Real f, Real g)
{
    BisectNode* node = new0 BisectNode();
    node->X = x;
    node->Y = y;
    node->F = f;
    node->G = g;
    return node;
}
//----------------------------------------------------------------------------
template <typename Real>
bool Bisect2<Real>::Bisect (Real x0, Real y0, Real x1, Real y1,
    Real& xRoot, Real& yRoot)
{
    // Test the four corner values.
    if (ZeroTest(x0, y0, mF00, mG00, xRoot, yRoot))
    {
        return true;
    }

    if (ZeroTest(x1, y0, mF10, mG10, xRoot, yRoot))
    {
        return true;
    }

    if (ZeroTest(x0, y1, mF01, mG01, xRoot, yRoot))
    {
        return true;
    }

    if (ZeroTest(x1, y1, mF11, mG11, xRoot, yRoot))
    {
        return true;
    }

    // Build the initial quad.

    // Add N00.
    mGraph = new0 BisectNode();
    mGraph->X = x0;
    mGraph->Y = y0;
    mGraph->F = mF00;
    mGraph->G = mG00;

    // Add N10.
    BisectNode* temp = AddNode(x1, y0, mF10, mG10);
    temp->XNext = 0;
    mGraph->XNext = temp;

    // Add N01.
    temp = AddNode(x0, y1, mF01, mG01);
    temp->YNext = 0;
    mGraph->YNext = temp;

    // Add N11.
    temp = AddNode(x1, y1, mF11, mG11);
    temp->XNext = 0;
    temp->YNext = 0;
    mGraph->XNext->YNext = temp;
    mGraph->YNext->XNext = temp;

    mLevel = 0;
    bool result = BisectRecurse(mGraph);
    if (result)
    {
        xRoot = mXRoot;
        yRoot = mYRoot;
    }

    // Remove the remaining quad from mGraph.
    delete0(mGraph->XNext->YNext);
    delete0(mGraph->XNext);
    delete0(mGraph->YNext);
    delete0(mGraph);

    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
bool Bisect2<Real>::BisectRecurse (BisectNode* n00)
{
    if (++mLevel == mMaxLevel)
    {
        --mLevel;
        return false;
    }

    BisectNode* n10 = n00->XNext;
    BisectNode* n11 = n10->YNext;
    BisectNode* n01 = n00->YNext;

    mNetSign = (int)(
        Math<Real>::Sign(n00->F) +
        Math<Real>::Sign(n01->F) +
        Math<Real>::Sign(n10->F) +
        Math<Real>::Sign(n11->F));

    if (abs(mNetSign) == 4)
    {
        // F has the same sign at corners.
        --mLevel;
        return false;
    }

    mNetSign = (int)(
        Math<Real>::Sign(n00->G) +
        Math<Real>::Sign(n01->G) +
        Math<Real>::Sign(n10->G) +
        Math<Real>::Sign(n11->G));

    if ( abs(mNetSign) == 4 )
    {
        // G has the same sign at corners.
        --mLevel;
        return false;
    }

    // Bisect the quad.
    mX0 = n00->X;
    mY0 = n00->Y;
    mX1 = n11->X;
    mY1 = n11->Y;
    mXm = ((Real)0.5)*(mX0 + mX1);
    mYm = ((Real)0.5)*(mY0 + mY1);

    // right edge 10,11
    if (ZeroTest(mX1, mYm, mF1m, mG1m, mXRoot, mYRoot))
    {
        return true;
    }

    // bottom edge 01,11
    if (ZeroTest(mXm, mY1, mFm1, mGm1, mXRoot, mYRoot))
    {
        return true;
    }

    // top edge 00,10
    if (ZeroTest(mXm, mY0, mFm0, mGm0, mXRoot, mYRoot))
    {
        return true;
    }

    // left edge 00,01
    if (ZeroTest(mX0, mYm, mF0m, mG0m, mXRoot, mYRoot))
    {
        return true;
    }

    // center
    if (ZeroTest(mXm, mYm, mFmm, mGmm, mXRoot, mYRoot))
    {
        return true;
    }

    // right bisector
    BisectNode* temp = AddNode(mX1, mYm, mF1m, mG1m);
    temp->XNext = 0;
    temp->YNext = n11;
    n10->YNext = temp;

    // bottom bisector
    temp = AddNode(mXm, mY1, mFm1, mGm1);
    temp->XNext = n11;
    temp->YNext = 0;
    n01->XNext = temp;

    // top bisector
    temp = AddNode(mXm, mY0, mFm0, mGm0);
    temp->XNext = n10;
    n00->XNext = temp;

    // left bisector
    temp = AddNode(mX0, mYm, mF0m, mG0m);
    temp->YNext = n01;
    n00->YNext = temp;

    // middle bisector
    temp = AddNode(mXm, mYm, mFmm, mGmm);
    temp->XNext = n10->YNext;
    temp->YNext = n01->XNext;
    n00->XNext->YNext = temp;
    n00->YNext->XNext = temp;

    // Search the subquads for roots.
    bool result =
        BisectRecurse(n00) ||
        BisectRecurse(n00->XNext) ||
        BisectRecurse(n00->YNext) ||
        BisectRecurse(n00->XNext->YNext);

    // The entire subquad check failed, remove the nodes that were added.

    // center
    delete0(n00->XNext->YNext);

    // edges
    delete0(n00->XNext);
    n00->XNext = n10;
    delete0(n00->YNext);
    n00->YNext = n01;
    delete0(n01->XNext);
    n01->XNext = n11;
    delete0(n10->YNext);
    n10->YNext = n11;

    --mLevel;
    return result;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Bisect2::BisectNode
//----------------------------------------------------------------------------
template <typename Real>
Bisect2<Real>::BisectNode::BisectNode ()
    :
    X((Real)0),
    Y((Real)0),
    F((Real)0),
    G((Real)0),
    XNext(0),
    YNext(0)
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class Bisect2<float>;

template WM5_MATHEMATICS_ITEM
class Bisect2<double>;
//----------------------------------------------------------------------------
}
