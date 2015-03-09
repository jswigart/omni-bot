// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2012/08/02)

#include "Wm5PhysicsPCH.h"
#include "Wm5LCPPolyDist.h"
#include "Wm5Memory.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <int Dimension, typename FVector, typename DVector>
LCPPolyDist<Dimension,FVector,DVector>::LCPPolyDist (int numPoints1,
    FVector* points1, int numFaces1, ITuple* faces1, int numPoints2,
    FVector* points2, int numFaces2, ITuple* faces2, int& statusCode,
    float& distance, FVector closest[2], double verifyMinDifference,
    double randomWidth)
    :
    mDimension(sizeof(DVector)/sizeof(double)),
    mNumEquations(numFaces1 + numFaces2 + 2*mDimension),
    mNumPoints1(numPoints1),
    mNumPoints2(numPoints2),
    mNumFaces1(numFaces1),
    mNumFaces2(numFaces2),
    mFaces1(faces1),
    mFaces2(faces2),
    mVerifyMinDifference(verifyMinDifference),
    mRandomWidth(randomWidth)
{
    WM5_LCPPOLYDIST_FUNCTION(OpenLog());
    WM5_LCPPOLYDIST_FUNCTION(LogVerticesAndFaces(numPoints1, points1,
        numFaces1, faces1));
    WM5_LCPPOLYDIST_FUNCTION(LogVerticesAndFaces(numPoints2, points2,
        numFaces2, faces2));

    int i, j;

    mPoints1 = new1<DVector>(mNumPoints1);
    for (i = 0; i < mNumPoints1; ++i)
    {
        for (j = 0; j < mDimension; ++j)
        {
            mPoints1[i][j] = (double)points1[i][j];
        }
    }

    mPoints2 = new1<DVector>(mNumPoints2);
    for (i = 0; i < mNumPoints2; ++i)
    {
        for (j = 0; j < mDimension; ++j)
        {
            mPoints2[i][j] = (double)points2[i][j];
        }
    }

    mA1 = new1<DVector>(mNumFaces1);
    mB1 = new1<double>(mNumEquations);
    GenerateHalfSpaceDescription(mNumPoints1, mPoints1, mNumFaces1, mFaces1,
        mA1, mB1);

    mA2 = new1<DVector>(mNumFaces2);
    mB2 = new1<double>(mNumEquations);
    GenerateHalfSpaceDescription(mNumPoints2, mPoints2, mNumFaces2, mFaces2,
        mA2, mB2);

    distance = (float)ProcessLoop(false, statusCode, closest);

    delete1(mA1);
    delete1(mB1);
    delete1(mA2);
    delete1(mB2);
    delete1(mPoints1);
    delete1(mPoints2);

    WM5_LCPPOLYDIST_FUNCTION(CloseLog());
}
//----------------------------------------------------------------------------
template <int Dimension, typename FVector, typename DVector>
LCPPolyDist<Dimension,FVector,DVector>::LCPPolyDist (int numFaces1,
    FVector* A1, float* B1, int numFaces2, FVector* A2, float* B2,
    int& statusCode, float& distance, FVector closest[2],
    double verifyMinDifference, double randomWidth)
    :
    mDimension(sizeof(DVector)/sizeof(double)),
    mNumEquations(numFaces1 + numFaces2 + 2*mDimension),
    mNumPoints1(0),  // unused
    mNumPoints2(0),  // unused
    mNumFaces1(numFaces1),
    mNumFaces2(numFaces2),
    mPoints1(0),  // unused
    mPoints2(0),  // unused
    mVerifyMinDifference(verifyMinDifference),
    mRandomWidth(randomWidth)
{
    int i, j;

    mA1 = new1<DVector>(mNumFaces1);
    mB1 = new1<double>(mNumFaces1);
    for (i = 0; i < mNumFaces1; ++i)
    {
        mB1[i] = (double)B1[i];
        for (j = 0; j < mDimension; ++j)
        {
            mA1[i][j] = (double)A1[i][j];
        }
    }

    mA2 = new1<DVector>(mNumFaces2);
    mB2 = new1<double>(mNumFaces2);
    for (i = 0; i < mNumFaces2; ++i)
    {
        mB2[i] = (double)B2[i];
        for (j = 0; j < mDimension; ++j)
        {
            mA2[i][j] = (double)A2[i][j];
        }
    }

    WM5_LCPPOLYDIST_FUNCTION(OpenLog());
    WM5_LCPPOLYDIST_FUNCTION(LogHalfspaces(mNumFaces1, mA1, mB1));
    WM5_LCPPOLYDIST_FUNCTION(LogHalfspaces(mNumFaces2, mA2, mB2));

    WM5_LCPPOLYDIST_FUNCTION(RandomizeHalfspaces());
    
    distance = (float)ProcessLoop(true, statusCode, closest);

    WM5_LCPPOLYDIST_FUNCTION(CloseLog());

    delete1(mA1);
    delete1(mB1);
    delete1(mA2);
    delete1(mB2);
}
//----------------------------------------------------------------------------
template <int Dimension, typename FVector, typename DVector>
double LCPPolyDist<Dimension,FVector,DVector>::ProcessLoop (
    bool halfspaceConstructor, int& statusCode, FVector closest[2])
{
    int i;
    double* Q = new1<double>(mNumEquations);
    double** M = new2<double>(mNumEquations, mNumEquations);

    statusCode = 0;

    bool processing = true;
    if (!BuildMatrices(M, Q))
    {
        WM5_LCPPOLYDIST_FUNCTION(CloseLog());
        closest[0] = FVector::ZERO;
        closest[1] = FVector::ZERO;
        processing = false;
    }

    WM5_LCPPOLYDIST_FUNCTION(PrintMatrices(M, Q));

    int tryNumber = 0;
    double distance = -Mathd::MAX_REAL;
    while (processing)
    {
        double* zResult = new1<double>(mNumEquations);
        double* wResult = new1<double>(mNumEquations);
        int lcpStatusCode;

        LCPSolver solver(mNumEquations, M, Q, zResult, wResult, lcpStatusCode);
        WM5_UNUSED(solver);

        switch (lcpStatusCode)
        {
        case SC_FOUND_TRIVIAL_SOLUTION:
        {
            statusCode = SC_FOUND_TRIVIAL_SOLUTION;
            break;
        }
        case SC_FOUND_SOLUTION:
        {
            DVector lcpClosest[2];
            for (i = 0; i < mDimension; ++i)
            {
                lcpClosest[0][i] = (float)zResult[i];
                lcpClosest[1][i] = (float)zResult[i + mDimension];
            }

            DVector diff = lcpClosest[0] - lcpClosest[1];
            distance = diff.Length();

            statusCode = VerifySolution(lcpClosest);

            if (!halfspaceConstructor)
            {
                WM5_LCPPOLYDIST_FUNCTION(VerifyWithTestPoints(lcpClosest,
                    statusCode));
            }

            for (i = 0; i < mDimension; ++i)
            {
                closest[0][i] = (float)lcpClosest[0][i];
                closest[1][i] = (float)lcpClosest[1][i];
            }
            processing = false;
            break;
        }
        case SC_CANNOT_REMOVE_COMPLEMENTARY:
        {
            WM5_LCPPOLYDIST_FUNCTION(LogRetries(tryNumber));
            if (tryNumber == 3)
            {
                statusCode = SC_CANNOT_REMOVE_COMPLEMENTARY;
                processing = false;
                break;
            }
            MoveHalfspaces(mNumFaces1, mA1, mB1);
            WM5_LCPPOLYDIST_FUNCTION(LogHalfspaces(mNumFaces1, mA1, mB1));
            MoveHalfspaces(mNumFaces2, mA2, mB2);
            WM5_LCPPOLYDIST_FUNCTION(LogHalfspaces(mNumFaces2, mA2, mB2));
            BuildMatrices(M, Q);
            ++tryNumber;
            break;
        }
        case SC_EXCEEDED_MAX_RETRIES:
        {
            WM5_LCPPOLYDIST_FUNCTION(LCPSolverLoopLimit());
            statusCode = SC_EXCEEDED_MAX_RETRIES;
            processing = false;
            break;
        }
        }
        delete1(wResult);
        delete1(zResult);
    }

    delete2(M);
    delete1(Q);

    return distance;
}
//----------------------------------------------------------------------------
template <int Dimension, typename FVector, typename DVector>
bool LCPPolyDist<Dimension,FVector,DVector>::BuildMatrices (double** M,
    double* Q)
{
    // Set the entries of M and Q to zero.  Note the layout of M[][] is such
    // that M[0] is a pointer to mNumEquations*mNumEquations elements.
    size_t numBytes = mNumEquations*sizeof(double);
    memset(Q, 0, numBytes);
    memset(M[0], 0, mNumEquations*numBytes);

    // Set the M-terms for the upper-left positive-definite matrix S.
    int i, twoDimension = 2*mDimension;
    for (i = 0; i < twoDimension; ++i)
    {
        M[i][i] = 2.0;
        if (i < mDimension)
        {
            M[i][i + mDimension] = -2.0;
            M[i + mDimension][i] = -2.0;
        }
    }

    // enter pdZCoef terms
    // first matrix S
    //int iTwoDimension = 2*mDimension;
    //for (i = 0; i < iTwoDimension; i++)
    //{
    //    aadM[i][i] = 2.0;
    //    if (i < mDimension)
    //    {
    //        aadM[i][i+mDimension] = -2.0;
    //        aadM[i+mDimension][i] = -2.0;
    //    }
    //}

    // Set the A1 diagonal term in the M-matrix's A matrix.
    double qmin = 1.0;
    int j, k;
    for (i = 0, j = twoDimension; i < mNumFaces1; ++i, ++j)
    {
        for (k = 0; k < mDimension; ++k)
        {
            M[j][k] = -mA1[i][k];  // -A1
            M[k][j] = mA1[i][k];   // A1 transpose
        }

        Q[j] = mB1[i];
        if (Q[j] < qmin)
        {
            qmin = Q[j];
        }
    }

    // Set the A2 diagonal term in the M-matrix's A matrix.
    for (i = 0, j = twoDimension + mNumFaces1; i < mNumFaces2; ++i, ++j)
    {
        int n;
        for (k = 0, n = mDimension; k < mDimension; ++k, ++n)
        {
            M[j][n] = -mA2[i][k];  // -A2
            M[n][j] = mA2[i][k];   // A2 transpose
        }

        Q[j] = mB2[i];
        if (Q[j] < qmin)
        {
            qmin = Q[j];
        }
    }

    return qmin < 0.0;
}
//----------------------------------------------------------------------------
template <int Dimension, typename FVector, typename DVector>
void LCPPolyDist<Dimension,FVector,DVector>::ComputeHalfspaces (int numPoints,
    const DVector* points, int numFaces, const ITuple* faces, DVector* A,
    double* B)
{
    int j, k;

    DVector average = DVector::ZERO;
    for (j = 0; j < numPoints; ++j)
    {
        for (k = 0; k < mDimension; ++k)
        {
            average[k] += points[j][k];
        }
    }
    average /= (double)numPoints;

    DVector edge1, edge2, normal;
    for (j = 0; j < numFaces; ++j)
    {
        // In 2D we need the perp of a single vector.  In 3D we need the
        // cross product of two vectors.
        const DVector& point0 = points[faces[j][0]];
        const DVector& point1 = points[faces[j][1]];
        edge1 = point1 - point0;
        if (mDimension == 3)
        {
            const DVector& point2 = points[faces[j][2]];
            edge2 = point2 - point0;
            normal[0] = edge1[1]*edge2[2] - edge1[2]*edge2[1];
            normal[1] = edge1[2]*edge2[0] - edge1[0]*edge2[2];
            normal[2] = edge1[0]*edge2[1] - edge1[1]*edge2[0];
        }
        else
        {
            normal[0] = edge1[1];
            normal[1] = -edge1[0];
        }

        double constant = normal.Dot(point1);
        double sign = (normal.Dot(average) <= constant ? 1.0f : -1.0f);
        B[j] = sign*constant;
        for (k = 0; k < mDimension; ++k)
        {
            A[j][k] = sign*normal[k];
        }
    }
}
//----------------------------------------------------------------------------
template <int Dimension, typename FVector, typename DVector>
void LCPPolyDist<Dimension,FVector,DVector>::GenerateHalfSpaceDescription (
    int numPoints, DVector* points, int numFaces, ITuple* faces, DVector* A,
    double* B)
{
    WM5_LCPPOLYDIST_FUNCTION(RandomizeArray(numPoints, points));
    WM5_LCPPOLYDIST_FUNCTION(LogVertices(numPoints, points));

    ComputeHalfspaces(numPoints, points, numFaces, faces, A, B);

    WM5_LCPPOLYDIST_FUNCTION(LogHalfspaces(numFaces, A, B));
}
//----------------------------------------------------------------------------
template <int Dimension, typename FVector, typename DVector>
void LCPPolyDist<Dimension,FVector,DVector>::ChangeFaceOrder (int numFaces,
    ITuple* faces)
{
    ITuple temp = faces[0];
    for (int j = 1; j < numFaces; ++j)
    {
        faces[j - 1] = faces[j];
    }
    faces[numFaces - 1] = temp;
}
//----------------------------------------------------------------------------
template <int Dimension, typename FVector, typename DVector>
void LCPPolyDist<Dimension,FVector,DVector>::MoveHalfspaces (int numFaces,
    DVector* A, double* B)
{
    DVector aTemp = A[0];
    double bTemp = B[0];
    for (int j = 1; j < numFaces; ++j)
    {
        A[j - 1] = A[j];
        B[j - 1] = B[j];
    }
    A[numFaces - 1] = aTemp;
    B[numFaces - 1] = bTemp;
}
//----------------------------------------------------------------------------
template <int Dimension, typename FVector, typename DVector>
void LCPPolyDist<Dimension,FVector,DVector>::MovePoints ()
{ 
    ChangeFaceOrder(mNumFaces1, mFaces1);
    ChangeFaceOrder(mNumFaces2, mFaces2);
    GenerateHalfSpaceDescription(mNumPoints1, mPoints1, mNumFaces1, mFaces1,
        mA1, mB1);
    GenerateHalfSpaceDescription(mNumPoints2, mPoints2, mNumFaces2, mFaces2,
        mA2, mB2);
}
//----------------------------------------------------------------------------
template <int Dimension, typename FVector, typename DVector>
int LCPPolyDist<Dimension,FVector,DVector>::VerifySolution (
    const DVector closest[2])
{
    // Check to see that the first closest point is in the first
    // polygon/polyhedron  and the second closest point is in the second
    // polygon/polyhedron.
    double diff;
    int i;

    for (i = 0; i < mNumFaces1; ++i)
    {
        diff = mA1[i].Dot(closest[0]) - mB1[i];
        if (diff > mVerifyMinDifference)
        {
            WM5_LCPPOLYDIST_FUNCTION(LogVerifyFailure(1, i, diff));
            return SC_VERIFY_FAILURE;
        }
    }

    for (i = 0; i < mNumFaces2; ++i)
    {
        diff = mA2[i].Dot(closest[1]) - mB2[i];
        if (diff > mVerifyMinDifference)
        {
            WM5_LCPPOLYDIST_FUNCTION(LogVerifyFailure(2, i, diff));
            return SC_VERIFY_FAILURE;
        }
    }
    
    return SC_FOUND_SOLUTION;
}
//----------------------------------------------------------------------------

#ifdef WM5_LCPPOLYDIST_LOG
//----------------------------------------------------------------------------
template <int Dimension, typename FVector, typename DVector>
void LCPPolyDist<Dimension,FVector,DVector>::OpenLog ()
{
    if (!mLog.is_open())
    {
        // Open the log file.
        mLog.open("LCPPolyDist.log");
        assertion(mLog != 0, "Failed to open LCPPolyDist log.\n");
        if (!mLog)
        {
            return;
        }
    }
    
    mLog << "LCP Polyhedron Distance Log" << std::endl;

    // Print the current date and time.
    time_t clockTime;
    time(&clockTime);
    mLog << "Time: " << asctime(localtime(&clockTime)) << std::endl
        << std::endl;

    // Use scientific notation for floating point output.
    mLog.setf(std::ios::scientific, std::ios::floatfield);
}
//----------------------------------------------------------------------------
template <int Dimension, typename FVector, typename DVector>
void LCPPolyDist<Dimension,FVector,DVector>::CloseLog ()
{
    mLog.flush();
    mLog.close();
}
//----------------------------------------------------------------------------
template <int Dimension, typename FVector, typename DVector>
void LCPPolyDist<Dimension,FVector,DVector>::PrintMatrices (double** M,
    double* Q)
{
    // Print the input to the solver.
    mLog << "There are " << mNumEquations
        << " rows and columns in this problem." << std::endl << std::endl;

    mLog << "The matrix M." << std::endl;
    int i;
    for (i = 0; i < mNumEquations; ++i)
    {
        for (int j = 0; j < mNumEquations; ++j)
        {
            mLog << std::showpos << M[i][j] << ' ';
        }
        mLog << std::endl;
    }

    mLog << "The vector Q." << std::endl;
    for (i = 0; i < mNumEquations; ++i)
    {
        mLog << std::showpos << Q[i] << ' ';
    }
    mLog << std::endl;
}
//----------------------------------------------------------------------------
template <int Dimension, typename FVector, typename DVector>
void LCPPolyDist<Dimension,FVector,DVector>::LogRetries (int tryNumber)
{
    mLog << "LCPPolyDist cannot remove complementary variable. ";
    mLog << " No solution, tryNumber = " << tryNumber << std::endl;
}
//----------------------------------------------------------------------------
template <int Dimension, typename FVector, typename DVector>
void LCPPolyDist<Dimension,FVector,DVector>::LCPSolverLoopLimit ()
{
    mLog << "No solution found. LCPSolver used the maximum";
    mLog << " parameterized number of loops." << std::endl;
}
//----------------------------------------------------------------------------
template <int Dimension, typename FVector, typename DVector>
void LCPPolyDist<Dimension,FVector,DVector>::LogVertices (int numPoints,
    const DVector* points)
{
    // Write triangle vertices to log.
    mLog << std::endl << "Triangle vertices." << std::endl;
    for (int i = 0; i < numPoints; ++i)
    {
        mLog << "(" << points[i][0];
        for (int j = 1; j < mDimension; ++j)
        {
            mLog << ", " << points[i][j];
        }
        mLog << ")" << std::endl;
    }
    mLog << std::endl;
}
//----------------------------------------------------------------------------
template <int Dimension, typename FVector, typename DVector>
void LCPPolyDist<Dimension,FVector,DVector>::LogVerticesAndFaces (
    int numPoints, const FVector* points, int numFaces, const ITuple* faces)
{
    mLog << std::endl << "This polyhedron has " << numPoints; 
    mLog << " vertices." << std::endl << "They are:" << std::endl;
    int i, j;
    for (i = 0; i < numPoints; ++i)
    {
        mLog << "(" << points[i][0];
        for (j = 1; j < mDimension; ++j)
        {
            mLog << ", " << points[i][j];
        }
        mLog << ")" << std::endl;
    }
    mLog << " The number of faces is " << numFaces << "." << std::endl; 
    mLog << " And they are (counting the vertices above from 0 to ";
    mLog << numPoints - 1 << ")" << std::endl;
    for (i = 0; i < numFaces; ++i)
    {
        mLog << "(" << faces[i][0];
        for (j = 1; j < mDimension; ++j)
        {
            mLog << ", " << faces[i][j];
        }
        mLog << ")" << std::endl;
    }
}
//----------------------------------------------------------------------------
template <int Dimension, typename FVector, typename DVector>
void LCPPolyDist<Dimension,FVector,DVector>::LogHalfspaces (int numFaces,
    const DVector* A, const double* B)
{
    // Write halfspaces generated by triangle vertices to log.
    mLog << std::endl << "Halfspaces:" << std::endl;
    std::string strYZ[] = { "*y ", "*z " };
    for (int i = 0; i < numFaces; ++i)
    {
        mLog << A[i][0] << "*x ";
        for (int j = 1; j < mDimension; ++j)
        {
            mLog << A[i][j] << strYZ[j-1];
        }
        mLog << "<= " << B[i] << std::endl;
    }
    mLog << std::endl;
}
//----------------------------------------------------------------------------
template <int Dimension, typename FVector, typename DVector>
void LCPPolyDist<Dimension,FVector,DVector>::SetupTestPoints (int index,
    int numFaces, int numPoints, const DVector& closest, const DVector* A,
    const double* B, const DVector* points, DVector* testPoints)
{
    DVector average = DVector::ZERO;
    int i;
    for (i = 0; i < numPoints; ++i)
    {
        average += points[i];
    }
    testPoints[0] = average/((double)numPoints);

    int j = (numPoints < 4 ? numPoints : 4);
    for (i = 0; i < 4; ++i)
    {
        if (i < j)
        {
            testPoints[i + 1] = 0.5*closest + 0.5*points[i];
            testPoints[i + 5] = 0.8*closest + 0.2*points[i];
        }
        else
        {
            testPoints[i + 1] = closest;
            testPoints[i + 5] = closest;
        }
    }

    double amountOut = 0.2;
    testPoints[9] = 0.5*(testPoints[1] + testPoints[5]);
    testPoints[10] = 0.5*closest + 0.5*testPoints[0];
    testPoints[11] = 0.8*closest + 0.2*testPoints[0];
    testPoints[12] = (1.0 + amountOut)*closest - amountOut*testPoints[0];

    LogSolutionWithTestPoints(index, closest, testPoints);

    // Eliminate generated points outside poly.
    for (i = 0; i < 13; ++i)
    {
        for (j = 0; j < numFaces; ++j)
        {
            // For each point and each constraint, calculate the constraint
            // value.
            double diff = A[j].Dot(testPoints[i]) - B[j];
            if (diff > mVerifyMinDifference)
            {
                // Replace with solution point.
                testPoints[i] = closest;
            }
        }
    }
    mLog << "Test points reduced for containment in polyhedron:" << std::endl;
    LogTestPoints(testPoints);
}
//----------------------------------------------------------------------------
template <int Dimension, typename FVector, typename DVector>
void LCPPolyDist<Dimension,FVector,DVector>::LogTestPoints (
    const DVector* testPoints)
{
    for (int i = 0; i < 13; ++i) 
    {
        mLog << "(" << testPoints[i][0];
        for (int j = 1; j < mDimension; ++j)
        {
            mLog << ", " << testPoints[i][j];
        }
        mLog << ")" << std::endl;
    }
    mLog << std::endl;
}
//----------------------------------------------------------------------------
template <int Dimension, typename FVector, typename DVector>
void LCPPolyDist<Dimension,FVector,DVector>::LogSolutionWithTestPoints (
    int index, const DVector& closest, const DVector* testPoints)
{
    // Write generated test points to log.
    if (index == 1)
    {
        mLog << std::endl << std::endl;
    }
    mLog << "Solution point from polyhedron " << index;
    mLog << " = (" << closest[0];
    for (int i = 1; i < mDimension; ++i)
    {
        mLog << ", " << closest[i];
    }
    mLog << ")" << std::endl;
    mLog << "The generated test points are :" << std::endl;
    LogTestPoints(testPoints);
}       
//----------------------------------------------------------------------------
template <int Dimension, typename FVector, typename DVector>
void LCPPolyDist<Dimension,FVector,DVector>::VerifyWithTestPoints (
    const DVector* closest, int& statusCode)
{
    DVector* testPoints1 = new1<DVector>(13);
    DVector* testPoints2 = new1<DVector>(13);

    SetupTestPoints(1, mNumFaces1, mNumPoints1, closest[0], mA1, mB1,
        mPoints1, testPoints1);
    SetupTestPoints(2, mNumFaces2, mNumPoints2, closest[1], mA2, mB2,
        mPoints2, testPoints2);

    double diffSq = 0.0;
    double minValue = 0.0;
    DVector diff;
    int iLine = 0;
    int jLine = 0;

    // The min distance between generated points.  Note that one of these
    // points is the "solution".
    int i;
    for (i = 0; i < 13; ++i)
    {
        for (int j = 0; j < 13; ++j)
        {
            diff = testPoints1[i] - testPoints2[j];
            diffSq = diff.Dot(diff);
            if (i == 0 && j == 0)
            {
                minValue = diffSq;
            }
            else
            {
                if (diffSq < minValue)
                {
                    minValue = diffSq;
                    iLine = i;
                    jLine = j;
                }
            }
        }
    }

    diff = closest[0] - closest[1];
    float distance = (float)diff.Dot(diff);

    statusCode = SC_FOUND_SOLUTION;
    if (distance > diffSq)
    {
        mLog << "Test points closer than solution points by ";
        mLog << distance - diffSq << " squared units.\n";
        if ((distance - diffSq > mVerifyMinDifference)
        &&  (iLine != 12 || jLine != 12))
        {
            statusCode = SC_TEST_POINTS_TEST_FAILED;
        }
    }
    mLog << std::endl << " Solution points are separated by "
        << Mathf::Sqrt(distance);
    mLog << " units." << std::endl;
    mLog << "The smallest distance between test points is "
        << Mathf::Sqrt((float)minValue);
    mLog << std::endl << "and occurs for (" << testPoints1[iLine][0];
    for (i = 1; i < mDimension; ++i)
    {
        mLog << ", " << testPoints1[iLine][i];
    }
    mLog << ") and (" << testPoints2[jLine][0];
    for (i = 1; i < mDimension; ++i)
    {
        mLog << ", " << testPoints2[jLine][i];
    }
    mLog << ")" << std::endl;

    delete1(testPoints1);
    delete1(testPoints2);
}
//----------------------------------------------------------------------------
template <int Dimension, typename FVector, typename DVector>
void LCPPolyDist<Dimension,FVector,DVector>::RandomizeArray (int numPoints,
    DVector* points)
{
    // Large values (0.9) of RANDOM_WIDTH tolerated with sample data files.
    for (int j = 0; j < numPoints; ++j)
    {
        for (int k = 0; k < mDimension; ++k)
        {
            points[j][k] *= 1.0 + mRandomWidth*Mathd::SymmetricRandom();
        }
    }
}     
//----------------------------------------------------------------------------
template <int Dimension, typename FVector, typename DVector>
void LCPPolyDist<Dimension,FVector,DVector>::RandomizeHalfspaces ()
{
    // Small values (0.05) of RANDOM_WIDTH tolerated with sample data files.
    int j, k;
    for (j = 0; j < mNumFaces1; ++j)
    {
        mB1[j] *= 1.0 + mRandomWidth*Mathd::SymmetricRandom();
        for (k = 0; k < mDimension; ++k)
        {
            mA1[j][k] *= 1.0 + mRandomWidth*Mathd::SymmetricRandom();
        }
    }
    for (j = 0; j < mNumFaces2; ++j)
    {
        mB2[j] *= 1.0 + mRandomWidth*Mathd::SymmetricRandom();
        for (k = 0; k < mDimension; ++k)
        {
            mA2[j][k] *= 1.0 + mRandomWidth*Mathd::SymmetricRandom();
        }
    }
}
//----------------------------------------------------------------------------
template <int Dimension, typename FVector, typename DVector>
void LCPPolyDist<Dimension,FVector,DVector>::LogVerifyFailure (
    int whichPolyhedron, int i, double diff)
{
    mLog << "Solution fails to meet constraint " << i;
    if (whichPolyhedron == 1)
    {
        mLog << " for the first polyhedron." << std::endl;
    }
    else
    {
        mLog << " for the second polyhedron." << std::endl;           
    }
    mLog << "The error is " << diff << std::endl;
}
//----------------------------------------------------------------------------
#endif

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM5_PHYSICS_ITEM
class LCPPolyDist<2, Vector2<float>, Vector2<double> >;

template WM5_PHYSICS_ITEM
class LCPPolyDist<3, Vector3<float>, Vector3<double> >;
//----------------------------------------------------------------------------
}
