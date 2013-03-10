// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline TriangleKey::TriangleKey (int v0, int v1, int v2)
{
    if (v0 < v1)
    {
        if (v0 < v2)
        {
            // v0 is minimum
            V[0] = v0;
            V[1] = v1;
            V[2] = v2;
        }
        else
        {
            // v2 is minimum
            V[0] = v2;
            V[1] = v0;
            V[2] = v1;
        }
    }
    else
    {
        if (v1 < v2)
        {
            // v1 is minimum
            V[0] = v1;
            V[1] = v2;
            V[2] = v0;
        }
        else
        {
            // v2 is minimum
            V[0] = v2;
            V[1] = v0;
            V[2] = v1;
        }
    }
}
//----------------------------------------------------------------------------
inline bool TriangleKey::operator< (const TriangleKey& key) const
{
    if (V[2] < key.V[2])
    {
        return true;
    }

    if (V[2] > key.V[2])
    {
        return false;
    }

    if (V[1] < key.V[1])
    {
        return true;
    }

    if (V[1] > key.V[1])
    {
        return false;
    }

    return V[0] < key.V[0];
}
//----------------------------------------------------------------------------
inline TriangleKey::operator size_t () const
{
    return V[0] | (V[1] << 10) | (V[2] << 20);
}
//----------------------------------------------------------------------------
