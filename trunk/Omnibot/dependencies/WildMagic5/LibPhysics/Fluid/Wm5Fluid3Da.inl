// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Real>
inline Real Fluid3Da<Real>::GetX0 () const
{
    return mX0;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Fluid3Da<Real>::GetY0 () const
{
    return mY0;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Fluid3Da<Real>::GetZ0 () const
{
    return mZ0;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Fluid3Da<Real>::GetX1 () const
{
    return mX1;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Fluid3Da<Real>::GetY1 () const
{
    return mY1;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Fluid3Da<Real>::GetZ1 () const
{
    return mZ1;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Fluid3Da<Real>::GetDt () const
{
    return mDt;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Fluid3Da<Real>::GetDx () const
{
    return mDx;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Fluid3Da<Real>::GetDy () const
{
    return mDy;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Fluid3Da<Real>::GetDz () const
{
    return mDz;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Fluid3Da<Real>::GetTime () const
{
    return mTime;
}
//----------------------------------------------------------------------------
template <typename Real>
inline int Fluid3Da<Real>::GetIMax () const
{
    return mIMax;
}
//----------------------------------------------------------------------------
template <typename Real>
inline int Fluid3Da<Real>::GetJMax () const
{
    return mJMax;
}
//----------------------------------------------------------------------------
template <typename Real>
inline int Fluid3Da<Real>::GetKMax () const
{
    return mKMax;
}
//----------------------------------------------------------------------------
template <typename Real>
inline const Real* Fluid3Da<Real>::GetX () const
{
    return mX;
}
//----------------------------------------------------------------------------
template <typename Real>
inline const Real* Fluid3Da<Real>::GetY () const
{
    return mY;
}
//----------------------------------------------------------------------------
template <typename Real>
inline const Real* Fluid3Da<Real>::GetZ () const
{
    return mZ;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real*** Fluid3Da<Real>::GetDensity () const
{
    return mDensity0;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Vector3<Real>*** Fluid3Da<Real>::GetVelocity () const
{
    return mVelocity0;
}
//----------------------------------------------------------------------------
