// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5PhysicsPCH.h"
#include "Wm5LCPSolver.h"
#include "Wm5Memory.h"
using namespace Wm5;

//----------------------------------------------------------------------------
LCPSolver::LCPSolver (int numEquations, double** M, double* Q, double* Z,
    double* W, int& status, int maxRetries, double zeroTolerance,
    double ratioError)
    :
    mNumEquations(numEquations),
    mM(M),
    mQ(Q),
    mMaxRetries(maxRetries),
    mZeroTolerance(zeroTolerance),
    mRatioError(ratioError)
{
    WM5_LCPSOLVER_FUNCTION(OpenLog());

    AllocateEquations();

    if (InitializeEquations())
    {
        WM5_LCPSOLVER_FUNCTION(PrintEquations());

        int j;
        for (j = 0; j < mMaxRetries; ++j)
        {
            int equation;
            if (!SelectEquation(equation))
            {
                WM5_LCPSOLVER_FUNCTION(PrintCannotRemoveComplementary());
                status = SC_CANNOT_REMOVE_COMPLEMENTARY;
                break;
            }

            int eqM1 = equation - 1;
            Solve(mEquations[eqM1].Var, mEquations[eqM1].VarIndex);

            WM5_LCPSOLVER_FUNCTION(PrintEquations());

            // Determine whether z0 is a basic variable.
            bool z0Basic = false;
            int i;
            for (i = 0; i < mNumEquations; ++i)
            {
                if (mEquations[i].Var == 'z' && mEquations[i].VarIndex == 0)
                {
                    z0Basic = true;
                    break;
                }
            }

            if (!z0Basic)
            {
                // Solution found when z0 is removed from the basic set.
                size_t numBytes = mNumEquations*sizeof(double);
                memset(Z, 0, numBytes);
                memset(W, 0, numBytes);
                for (i = 0; i < mNumEquations; ++i)
                {
                    if (mEquations[i].Var == 'z')
                    {
                        Z[mEquations[i].VarIndex-1] = mEquations[i].C[0];
                    }
                    else
                    {
                        W[mEquations[i].VarIndex-1] = mEquations[i].C[0];
                    }
                }
                status = SC_FOUND_SOLUTION;
                WM5_LCPSOLVER_FUNCTION(PrintResults());
                break;
            }
        }

        if (j == mMaxRetries)
        {
            status = SC_EXCEEDED_MAX_RETRIES;
        }
    }
    else
    {
        WM5_LCPSOLVER_FUNCTION(PrintImmediateSolution());
        size_t numBytes = mNumEquations*sizeof(double);
        memset(Z, 0, numBytes);
        memcpy(W, Q, numBytes);
        status = SC_FOUND_TRIVIAL_SOLUTION;
    }

    DeallocateEquations();
    WM5_LCPSOLVER_FUNCTION(CloseLog());
}
//----------------------------------------------------------------------------
void LCPSolver::AllocateEquations ()
{
    mEquations = new1<Equation>(mNumEquations);
    int numEquationsP1 = mNumEquations + 1;
    for (int i = 0; i < mNumEquations; ++i)
    {
        mEquations[i].C = new1<double>(numEquationsP1);
        mEquations[i].W = new1<double>(numEquationsP1);
        mEquations[i].Z = new1<double>(numEquationsP1);
    }
}
//----------------------------------------------------------------------------
void LCPSolver::DeallocateEquations ()
{
    for (int i = 0; i < mNumEquations; ++i)
    {
        delete1(mEquations[i].C);
        delete1(mEquations[i].W);
        delete1(mEquations[i].Z);
    }
    delete1(mEquations);
}
//----------------------------------------------------------------------------
bool LCPSolver::InitializeEquations ()
{
    int numEquationsP1 = mNumEquations + 1;
    int numBytes = numEquationsP1*sizeof(double);
    int i;
    for (i = 0; i < mNumEquations; ++i)
    {
        // Initially w's are basic, z's are non-basic.
        mEquations[i].Var = 'w';

        // w indices run from 1 to numEquations.
        mEquations[i].VarIndex = i + 1;

        // The extra variable in the equations is z0.
        memset(mEquations[i].C, 0, numBytes);
        memset(mEquations[i].W, 0, numBytes);
        memset(mEquations[i].Z, 0, numBytes);
        mEquations[i].Z[0] = 1.0;
        mEquations[i].C[i + 1] = 1.0;
    }

    // Check whether all the constant terms are nonnegative.  If so, the
    // solution is z = 0 and w = constant_terms.  The caller will set the
    // values of z and w, so just return from here.
    double constTermMin = 0.0;
    for (i = 0; i < mNumEquations; ++i)
    {
        mEquations[i].C[0] = mQ[i];
        if (mQ[i] < constTermMin)
        {
            constTermMin = mQ[i];
        }
    }
    if (constTermMin >= 0.0)
    {
        return false;
    }

    // Enter Z terms.
    int j;
    for (i = 0; i < mNumEquations; ++i)
    {
        // Set equations Z[0] to 0.0 for any row in which all mM are 0.0.
        double rowOfZeros = 0.0;
        for (j = 0; j < mNumEquations; ++j)
        {
            double temp = mM[i][j];
            mEquations[i].Z[j + 1] = temp;
            if (temp != 0.0)
            {
                rowOfZeros = 1.0;
            }
        }
        mEquations[i].Z[0] *= rowOfZeros;
    }

    for (i = 0; i < mNumEquations; ++i)
    {
        // Find the max abs value of the coefficients on each row and divide
        // each row by that max abs value.
        double maxAbsValue = 0.0;
        for (j = 0; j < numEquationsP1; ++j)
        {
            double absValue = Mathd::FAbs(mEquations[i].C[j]);
            if (absValue > maxAbsValue)
            {
                maxAbsValue = absValue;
            }

            absValue = Mathd::FAbs(mEquations[i].W[j]);
            if (absValue > maxAbsValue)
            {
                maxAbsValue = absValue;
            }

            absValue = Mathd::FAbs(mEquations[i].Z[j]);
            if (absValue > maxAbsValue)
            {
                maxAbsValue = absValue;
            }
        }

        double invMaxAbsValue = 1.0/maxAbsValue;
        for (j = 0; j < numEquationsP1; ++j)
        {
            mEquations[i].C[j] *= invMaxAbsValue;
            mEquations[i].W[j] *= invMaxAbsValue;
            mEquations[i].Z[j] *= invMaxAbsValue;
        }       
    }
    return true;
}
//----------------------------------------------------------------------------
bool LCPSolver::SelectEquation (int& equation)
{
    // The algorithm for selecting the equation to be solved is:
    // 1. if z0 is not a basic variable, solve for z0
    //      choose the equation with smallest (negative) constant term.
    // 2. if a w, say wj, has just left the basic set, solve for zj.
    //      choose the equation to solve for zj by:
    //              coefficient, cj, of zj is negative
    //              the ratio constj/-cj is smallest.

    // Determine whether z0 is a basic variable.
    bool z0Basic = false;
    for (int i = 0; i < mNumEquations; ++i)
    {
        if (mEquations[i].Var == 'z' && mEquations[i].VarIndex == 0)
        {
            z0Basic = true;
        }
    }

    // If z0 is not basic, find the equation with the smallest (negative)
    // constant term and solve that equation for z0.
    if (!z0Basic)
    {
        mDepartingVariableIndex = 0;
        mNonBasicVariable = 'z';
        mNonBasicVariableIndex = 0;
    }
    else  // z0 is basic
    {
        // Since the departing variable left the dictionary, solve for the
        // complementary variable.
        mNonBasicVariable = (mDepartingVariable == 'w' ? 'z' : 'w');
    }

    bool found = FindEquation(equation);
    if (found)
    {
        int eqM1 = equation - 1;
        mNonBasicVariableIndex = mDepartingVariableIndex;
        mDepartingVariable = mEquations[eqM1].Var;
        mDepartingVariableIndex = mEquations[eqM1].VarIndex;

    }
    return found;
}
//----------------------------------------------------------------------------
bool LCPSolver::FindEquation (int& equation)
{
    if (mDepartingVariableIndex != 0)
    {
        // Find the limiting equation for variables other than z0.  The
        // coefficient of the variable must be negative.  The ratio of the
        // constant polynomial to the negative of the smallest coefficient
        // of the variable is sought.   The constant polynomial must be
        // evaluated to compute this ratio.  It must be evaluated at a value
        // of the variable, dEpsi, such that the ratio remains smallest for
        // all smaller dEpsi.
        return EquationAlgorithm(equation);
    }

    // Special case for nonbasic z0; the coefficients are 1.  Find the
    // limiting equation when solving for z0.  At least one C[0] must be
    // negative initially or we start with a solution.  If all of the
    // negative constant terms are different, pick the equation with the
    // smallest (negative) ratio of constant term to the coefficient of
    // z0.  If several equations contain the smallest negative constant
    // term, pick the one with the highest coefficient for that one
    // contains dEpsi to the largest exponent.  NOTE: This is equivalent
    // to using the constant term polynomial in dEpsi but avoids
    // evaluating it.
    double minValue = 0.0;
    for (int i = 0; i < mNumEquations; ++i)
    {
        if (mEquations[i].Z[0] != 0.0)
        {
            double quot = mEquations[i].C[0]/mEquations[i].Z[0];
            if (quot <= minValue || minValue == 0.0)
            {
                minValue = quot;
                equation = i + 1;
            }
        }
    }
    return minValue < 0.0;
}
//----------------------------------------------------------------------------
bool LCPSolver::EquationAlgorithm (int& equation)
{
    // This code loops through the rows of the z or w array to find all the
    // terms for which the coefficient of the chosen term is negative.  The
    // row search is reduced to these.  For the columns of the constants array
    // the rows (equations) for which the ratios of the constant terms to the
    // z or w coefficients of interest is smallest are found. If there are
    // several such rows, they are noted.  The row search is further reduced
    // to these.  Proceed to the next column until there is only one row left.

    int** found = new2<int>(2, mNumEquations + 1);

    // Find equations with negative coefficients for selected index.
    double temp;
    int i, j;
    for (i = 0, j = 0; i < mNumEquations; ++i)
    {                                    
        if (mNonBasicVariable == 'z')
        {
            temp = mEquations[i].Z[mDepartingVariableIndex];
        }
        else
        {
            temp = mEquations[i].W[mDepartingVariableIndex];
        }

        if (temp < 0.0)
        {
            found[j++][0] = i;
        }
    }

    if (j != 0)  // no terms with negative coefficients
    {
        found[j][0] = -1;

        // Find equation with smallest ratio of constTerm (polynomial) to 
        // selected (NonBasicVariable, DepartingVariableIndex) coefficient.
        int fai1 = 0, fai2 = 1;
        for (i = 0; i <= mNumEquations; ++i)
        {
            fai2 = (fai1 == 0 ? 1 : 0);

            int fi1 = 0, fi2 = 0;
            int j1 = found[fi1++][fai1];
            found[fi2++][fai2] = j1;
            int k = fi1;
            while (found[k][fai1] > -1)
            {
                int j2 = found[k][fai1];
                if (j2 < 0)
                {
                    break;
                }

                double denom1, denom2;
                if (mNonBasicVariable == 'z')
                {
                    denom1 = mEquations[j1].Z[mDepartingVariableIndex];
                    denom2 = mEquations[j2].Z[mDepartingVariableIndex];
                }
                else
                {
                    denom1 = mEquations[j1].W[mDepartingVariableIndex]; 
                    denom2 = mEquations[j2].W[mDepartingVariableIndex]; 
                }
                temp = mEquations[j2].C[i]/denom2 -
                    mEquations[j1].C[i]/denom1;
                if (temp < -mZeroTolerance)       
                {
                    // The first equation has the smallest ratio.  Do nothing;
                    // the first equation is the choice.
                }
                else if (temp > mZeroTolerance) 
                {
                    // The second equation has the smallest ratio.
                    fi1 = k;  // Make second equation comparison standard.
                    fi2 = 0;  // Restart the found array index.
                    j1 = found[fi1++][fai1];
                    found[fi2++][fai2] = j1;
                }
                else  // The ratios are the same.
                {
                    found[fi2++][fai2] = j2;
                }
                k++;
                found[fi2][fai2] = -1;
            }

            if (fi2 == 1)
            {
                // The "correct" exit.
                equation = found[0][fai2] + 1;
                delete2(found);
                return true;
            }

            fai1 = (fai1 == 0 ? 1 : 0);
        }
    }

    assertion(false, "We should not get here.\n");
    delete2(found);
    return false;
}
//----------------------------------------------------------------------------
void LCPSolver::Solve (char basicVariable, int basicVariableIndex)
{
    int found = -1, i ,j;
    for (i = 0; i < mNumEquations; ++i)
    {
        if (mEquations[i].Var == basicVariable)
        {
            if (mEquations[i].VarIndex == basicVariableIndex)
            {
                found = i;
            }
        }
    }
    if (found < 0 || found > mNumEquations-1)
    {
        WM5_LCPSOLVER_FUNCTION(PrintNoBasicVariable(basicVariable,
            basicVariableIndex));
        return;
    }

    // The equation for the replacement variable in this cycle.
    int numEquationsP1 = mNumEquations + 1;
    Equation replacement;
    replacement.Var = mNonBasicVariable;
    replacement.VarIndex = mNonBasicVariableIndex;
    replacement.C = new1<double>(numEquationsP1);
    replacement.W = new1<double>(numEquationsP1);
    replacement.Z = new1<double>(numEquationsP1);

    double denom;
    if (mNonBasicVariable == 'z')
    {
        denom = -mEquations[found].Z[mNonBasicVariableIndex];
    }
    else
    {
        denom = -mEquations[found].W[mNonBasicVariableIndex];
    }

    double invDenom = 1.0/denom;
    for (i = 0; i <= mNumEquations; ++i)
    {
        replacement.C[i] = mEquations[found].C[i]*invDenom;
        replacement.W[i] = mEquations[found].W[i]*invDenom;
        replacement.Z[i] = mEquations[found].Z[i]*invDenom;
    }

    if (mNonBasicVariable == 'z')
    {
        replacement.Z[mNonBasicVariableIndex] = 0.0;
    }
    else
    {
        replacement.W[mNonBasicVariableIndex] = 0.0;
    }

    if (basicVariable == 'z')
    {
        replacement.Z[basicVariableIndex] = -invDenom;
    }
    else
    {
        replacement.W[basicVariableIndex] = -invDenom;
    }

    for (i = 0; i < mNumEquations; ++i)
    {
        if (i != found)      
        {
            double coeff;
            if (replacement.Var == 'z')
            {
                coeff = mEquations[i].Z[mNonBasicVariableIndex];
            }
            else
            {
                coeff = mEquations[i].W[mNonBasicVariableIndex];
            }

            if (coeff != 0.0)
            {
                for (j = 0; j < numEquationsP1; ++j)
                {
                    mEquations[i].C[j] += coeff*replacement.C[j];
                    if (Mathd::FAbs(mEquations[i].C[j]) <
                        mRatioError*Mathd::FAbs(replacement.C[j]))
                    {
                        mEquations[i].C[j] = 0.0;
                    }

                    mEquations[i].W[j] += coeff*replacement.W[j];
                    if (Mathd::FAbs(mEquations[i].W[j]) <
                        mRatioError*Mathd::FAbs(replacement.W[j]))
                    {
                        mEquations[i].W[j] = 0.0;
                    }

                    mEquations[i].Z[j] += coeff*replacement.Z[j];
                    if (Mathd::FAbs(mEquations[i].Z[j]) <
                        mRatioError*Mathd::FAbs(replacement.Z[j]))
                    {
                        mEquations[i].Z[j] = 0.0;
                    }
                }

                if (replacement.Var == 'z')
                {
                    mEquations[i].Z[replacement.VarIndex] = 0.0;
                }
                else
                {
                    mEquations[i].W[replacement.VarIndex] = 0.0;
                }
            }
        }
    }

    // Replace the row corresponding to this equation.
    mEquations[found].Var = replacement.Var;
    mEquations[found].VarIndex = replacement.VarIndex;
    size_t numBytes = numEquationsP1*sizeof(double);
    memcpy(mEquations[found].C, replacement.C, numBytes);
    memcpy(mEquations[found].W, replacement.W, numBytes);
    memcpy(mEquations[found].Z, replacement.Z, numBytes);

    delete1(replacement.C);
    delete1(replacement.W);
    delete1(replacement.Z);
}
//----------------------------------------------------------------------------

#ifdef WM5_LCPSOLVER_LOG
//----------------------------------------------------------------------------
void LCPSolver::OpenLog ()
{
    // Open the log file.
    mLog.open("LCPSolver.log");
    assertion(mLog != 0, "Failed to open LCPSolver log.\n");
    if (!mLog)
    {
        return;
    }

    // Print a header.
    mLog << "LCPSolver" << std::endl;

    // Print the current date and time.
    time_t clockTime;
    time(&clockTime);
    mLog << "Time: " << asctime(localtime(&clockTime))
        << std::endl << std::endl;

    // Use scientific notation for floating point output.
    mLog.setf(std::ios::scientific, std::ios::floatfield);

    // Print the input to the solver.
    mLog << "There are " << mNumEquations
        << " rows and columns in this problem." << std::endl << std::endl;

    mLog << "The matrix M." << std::endl;
    int i;
    for (i = 0; i < mNumEquations; ++i)
    {
        for (int j = 0; j < mNumEquations; ++j)
        {
            mLog << std::showpos << mM[i][j] << ' ';
        }
        mLog << std::endl;
    }

    mLog << "The vector Q." << std::endl;
    for (i = 0; i < mNumEquations; ++i)
    {
        mLog << std::showpos << mQ[i] << ' ';
    }
    mLog << std::endl;

    // Counter for number of times LogPrintEquations is called.
    mPrintEquationsCalls = 0;
}
//----------------------------------------------------------------------------
void LCPSolver::CloseLog ()
{
    mLog.close();
}
//----------------------------------------------------------------------------
void LCPSolver::PrintImmediateSolution ()
{
    mLog << "Since the constants are all >= 0, the solution is " << std::endl;
    mLog << "Z = 0, W = Q." << std::endl;
}
//----------------------------------------------------------------------------
void LCPSolver::PrintCannotRemoveComplementary ()
{
    mLog << "LCPSolver cannot remove complementary variable." << std::endl;
}
//----------------------------------------------------------------------------
void LCPSolver::PrintNoBasicVariable (char basicVariable,
    int basicVariableIndex)
{
    mLog << "No equation found for basicVariable " << basicVariable;
    mLog << " with iBasicVariableIndex " << basicVariableIndex << std::endl;
}
//----------------------------------------------------------------------------
void LCPSolver::PrintEquations ()
{
    mLog << std::endl << std::endl
           << "This is call " << ++mPrintEquationsCalls
           << " to LogPrintEquations." << std::endl;

    for (int i = 0; i < mNumEquations; ++i)
    {
        mLog << std::endl << mEquations[i].Var << "("
            << mEquations[i].VarIndex << ") = " << mEquations[i].C[0];

        int j;
        for (j = 1; j <= mNumEquations; ++j)
        {
            if (mEquations[i].W[j] != 0.0)
            {
                mLog << std::showpos << mEquations[i].W[j] << "*w(" << j
                    << ")";
            }
        }
        for (j = 0; j <= mNumEquations; ++j)
        {
            if (mEquations[i].Z[j] != 0.0)
            {
                mLog << std::showpos << mEquations[i].Z[j] << "*z(" << j
                    << ")";
            }
        }
        mLog << std::endl;

        for (j = 1; j <= mNumEquations; ++j)
        {
            if (mEquations[i].C[j] != 0.0)
            {
                mLog << std::showpos << mEquations[i].C[j] << "*e^(" << j
                    << "}";
            }
        }
    }
}
//----------------------------------------------------------------------------
void LCPSolver::PrintResults ()
{
    int i;
    for (i = 0; i < mNumEquations; ++i)
    {
        mLog << std::endl;
        mLog << mEquations[i].Var << '(' << mEquations[i].VarIndex << ") = ";
        mLog << mEquations[i].C[0] << ' ';

        // Index j counts the position on a row.
        int j;

        // w terms
        for (j = 0; j <= mNumEquations; ++j)
        {   
            // skip zero terms
            if (mEquations[i].W[j] != 0)
            {
                mLog << mEquations[i].W[j];
                mLog << "*w (" << j << ')';
            }
        }

        // z terms
        for (j = 0; j <= mNumEquations; ++j)
        {   
            // skip zero terms
            if (mEquations[i].Z[j] != 0)
            {
                mLog << mEquations[i].Z[j];
                mLog << "*z (" << j << ')';
            }
        }
    }
    mLog << std::endl;
}
//----------------------------------------------------------------------------
#endif
