// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline EdgeKey::EdgeKey (int v0, int v1)
{
    if (v0 < v1)
    {
        // v0 is minimum
        V[0] = v0;
        V[1] = v1;
    }
    else
    {
        // v1 is minimum
        V[0] = v1;
        V[1] = v0;
    }
}
//----------------------------------------------------------------------------
inline bool EdgeKey::operator< (const EdgeKey& key) const
{
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
inline EdgeKey::operator size_t () const
{
    return V[0] | (V[1] << 16);
}
//----------------------------------------------------------------------------
