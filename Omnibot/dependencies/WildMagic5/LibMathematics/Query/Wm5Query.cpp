// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5Query.h"
using namespace Wm5;

//----------------------------------------------------------------------------
Query::Query ()
{
}
//----------------------------------------------------------------------------
Query::~Query ()
{
}
//----------------------------------------------------------------------------
bool Query::Sort (int& v0, int& v1)
{
    int j0, j1;
    bool positive;

    if (v0 < v1)
    {
        j0 = 0; j1 = 1; positive = true;
    }
    else
    {
        j0 = 1; j1 = 0; positive = false;
    }

    int value[2] = { v0, v1 };
    v0 = value[j0];
    v1 = value[j1];
    return positive;
}
//----------------------------------------------------------------------------
bool Query::Sort (int& v0, int& v1, int& v2)
{
    int j0, j1, j2;
    bool positive;

    if (v0 < v1)
    {
        if (v2 < v0)
        {
            j0 = 2; j1 = 0; j2 = 1; positive = true;
        }
        else if (v2 < v1)
        {
            j0 = 0; j1 = 2; j2 = 1; positive = false;
        }
        else
        {
            j0 = 0; j1 = 1; j2 = 2; positive = true;
        }
    }
    else
    {
        if (v2 < v1)
        {
            j0 = 2; j1 = 1; j2 = 0; positive = false;
        }
        else if (v2 < v0)
        {
            j0 = 1; j1 = 2; j2 = 0; positive = true;
        }
        else
        {
            j0 = 1; j1 = 0; j2 = 2; positive = false;
        }
    }

    int value[3] = { v0, v1, v2 };
    v0 = value[j0];
    v1 = value[j1];
    v2 = value[j2];
    return positive;
}
//----------------------------------------------------------------------------
bool Query::Sort (int& v0, int& v1, int& v2, int& v3)
{
    int j0, j1, j2, j3;
    bool positive;

    if (v0 < v1)
    {
        if (v2 < v3)
        {
            if (v1 < v2)
            {
                j0 = 0; j1 = 1; j2 = 2; j3 = 3; positive = true;
            }
            else if (v3 < v0)
            {
                j0 = 2; j1 = 3; j2 = 0; j3 = 1; positive = true;
            }
            else if (v2 < v0)
            {
                if (v3 < v1)
                {
                    j0 = 2; j1 = 0; j2 = 3; j3 = 1; positive = false;
                }
                else
                {
                    j0 = 2; j1 = 0; j2 = 1; j3 = 3; positive = true;
                }
            }
            else
            {
                if (v3 < v1)
                {
                    j0 = 0; j1 = 2; j2 = 3; j3 = 1; positive = true;
                }
                else
                {
                    j0 = 0; j1 = 2; j2 = 1; j3 = 3; positive = false;
                }
            }
        }
        else
        {
            if (v1 < v3)
            {
                j0 = 0; j1 = 1; j2 = 3; j3 = 2; positive = false;
            }
            else if (v2 < v0)
            {
                j0 = 3; j1 = 2; j2 = 0; j3 = 1; positive = false;
            }
            else if (v3 < v0)
            {
                if (v2 < v1)
                {
                    j0 = 3; j1 = 0; j2 = 2; j3 = 1; positive = true;
                }
                else
                {
                    j0 = 3; j1 = 0; j2 = 1; j3 = 2; positive = false;
                }
            }
            else
            {
                if (v2 < v1)
                {
                    j0 = 0; j1 = 3; j2 = 2; j3 = 1; positive = false;
                }
                else
                {
                    j0 = 0; j1 = 3; j2 = 1; j3 = 2; positive = true;
                }
            }
        }
    }
    else
    {
        if (v2 < v3)
        {
            if (v0 < v2)
            {
                j0 = 1; j1 = 0; j2 = 2; j3 = 3; positive = false;
            }
            else if (v3 < v1)
            {
                j0 = 2; j1 = 3; j2 = 1; j3 = 0; positive = false;
            }
            else if (v2 < v1)
            {
                if (v3 < v0)
                {
                    j0 = 2; j1 = 1; j2 = 3; j3 = 0; positive = true;
                }
                else
                {
                    j0 = 2; j1 = 1; j2 = 0; j3 = 3; positive = false;
                }
            }
            else
            {
                if (v3 < v0)
                {
                    j0 = 1; j1 = 2; j2 = 3; j3 = 0; positive = false;
                }
                else
                {
                    j0 = 1; j1 = 2; j2 = 0; j3 = 3; positive = true;
                }
            }
        }
        else
        {
            if (v0 < v3)
            {
                j0 = 1; j1 = 0; j2 = 3; j3 = 2; positive = true;
            }
            else if (v2 < v1)
            {
                j0 = 3; j1 = 2; j2 = 1; j3 = 0; positive = true;
            }
            else if (v3 < v1)
            {
                if (v2 < v0)
                {
                    j0 = 3; j1 = 1; j2 = 2; j3 = 0; positive = false;
                }
                else
                {
                    j0 = 3; j1 = 1; j2 = 0; j3 = 2; positive = true;
                }
            }
            else
            {
                if (v2 < v0)
                {
                    j0 = 1; j1 = 3; j2 = 2; j3 = 0; positive = true;
                }
                else
                {
                    j0 = 1; j1 = 3; j2 = 0; j3 = 2; positive = false;
                }
            }
        }
    }

    int value[4] = { v0, v1, v2, v3 };
    v0 = value[j0];
    v1 = value[j1];
    v2 = value[j2];
    v3 = value[j3];
    return positive;
}
//----------------------------------------------------------------------------
