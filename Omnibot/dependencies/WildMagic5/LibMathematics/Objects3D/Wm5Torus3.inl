// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Real>
Torus3<Real>::Torus3 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Torus3<Real>::~Torus3 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Torus3<Real>::Torus3 (Real outerRadius, Real innerRadius)
{
    // assert: outerRadius > 0 && innerRadius > 0
    OuterRadius = outerRadius;
    InnerRadius = innerRadius;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> Torus3<Real>::GetPosition (Real s, Real t) const
{
    Real twoPiS = Math<Real>::TWO_PI*s;
    Real twoPiT = Math<Real>::TWO_PI*t;
    Real cosTwoPiS = Math<Real>::Cos(twoPiS);
    Real sinTwoPiS = Math<Real>::Sin(twoPiS);
    Real cosTwoPiT = Math<Real>::Cos(twoPiT);
    Real sinTwoPiT = Math<Real>::Sin(twoPiT);
    Real maxRadius = OuterRadius + InnerRadius*cosTwoPiT;

    Vector3<Real> position(
        maxRadius*cosTwoPiS,
        maxRadius*sinTwoPiS,
        InnerRadius*sinTwoPiT);

    return position;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> Torus3<Real>::GetNormal (Real s, Real t) const
{
    Real twoPiS = Math<Real>::TWO_PI*s;
    Real cosTwoPiS = Math<Real>::Cos(twoPiS);
    Real sinTwoPiS = Math<Real>::Sin(twoPiS);
    Vector3<Real> position = GetPosition(s, t);

    Vector3<Real> normal(
        position.X() - OuterRadius*cosTwoPiS,
        position.Y() - OuterRadius*sinTwoPiS,
        position.Z());

    normal.Normalize();
    return normal;
}
//----------------------------------------------------------------------------
template <typename Real>
void Torus3<Real>::GetParameters (const Vector3<Real>& position, Real& s,
    Real& t) const
{
    Real radius = Math<Real>::Sqrt(position.X()*position.X() +
        position.Y()*position.Y());

    Real angle;

    if (radius < Math<Real>::ZERO_TOLERANCE)
    {
        s = (Real)0;
    }
    else
    {
        angle = Math<Real>::ATan2(position.Y(), position.X());
        if (angle >= (Real)0)
        {
            s = angle*Math<Real>::INV_TWO_PI;
        }
        else
        {
            s = (Real)1 + angle*Math<Real>::INV_TWO_PI;
        }
    }

    Real diff = radius - OuterRadius;

    if (Math<Real>::FAbs(diff) < Math<Real>::ZERO_TOLERANCE
    &&  Math<Real>::FAbs(position.Z()) < Math<Real>::ZERO_TOLERANCE)
    {
        t = (Real)0;
    }
    else
    {
        angle = Math<Real>::ATan2(position.Z(), diff);
        if (angle >= (Real)0)
        {
            t = angle*Math<Real>::INV_TWO_PI;
        }
        else
        {
            t = (Real)1 + angle*Math<Real>::INV_TWO_PI;
        }
    }
}
//----------------------------------------------------------------------------
