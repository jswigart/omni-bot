// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/07/07)

//----------------------------------------------------------------------------
inline PickRecord::PickRecord ()
{
}
//----------------------------------------------------------------------------
inline PickRecord::~PickRecord ()
{
}
//----------------------------------------------------------------------------
inline bool PickRecord::operator== (const PickRecord& record) const
{
    return T == record.T;
}
//----------------------------------------------------------------------------
inline bool PickRecord::operator!= (const PickRecord& record) const
{
    return T != record.T;
}
//----------------------------------------------------------------------------
inline bool PickRecord::operator<  (const PickRecord& record) const
{
    return T < record.T;
}
//----------------------------------------------------------------------------
inline bool PickRecord::operator<= (const PickRecord& record) const
{
    return T <= record.T;
}
//----------------------------------------------------------------------------
inline bool PickRecord::operator>  (const PickRecord& record) const
{
    return T > record.T;
}
//----------------------------------------------------------------------------
inline bool PickRecord::operator>= (const PickRecord& record) const
{
    return T >= record.T;
}
//----------------------------------------------------------------------------
