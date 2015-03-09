// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.5.2 (2012/07/08)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrEllipsoid3Ellipsoid3.h"

#include <functional>

namespace Wm5
{
	//----------------------------------------------------------------------------
	template <typename Real>
	IntrEllipsoid3Ellipsoid3<Real>::IntrEllipsoid3Ellipsoid3 (
		const Ellipsoid3<Real>& ellipsoid0, const Ellipsoid3<Real>& ellipsoid1)
		:
	mEllipsoid0(&ellipsoid0),
		mEllipsoid1(&ellipsoid1)
	{
	}
	//----------------------------------------------------------------------------
	template <typename Real>
	const Ellipsoid3<Real>& IntrEllipsoid3Ellipsoid3<Real>::GetEllipsoid0 () const
	{
		return *mEllipsoid0;
	}
	//----------------------------------------------------------------------------
	template <typename Real>
	const Ellipsoid3<Real>& IntrEllipsoid3Ellipsoid3<Real>::GetEllipsoid1 () const
	{
		return *mEllipsoid1;
	}
	//----------------------------------------------------------------------------
	template <typename Real>
	typename IntrEllipsoid3Ellipsoid3<Real>::Classification
		IntrEllipsoid3Ellipsoid3<Real>::GetClassification () const
	{
		// Get the parameters of ellipsoid0.
		Vector3<Real> K0 = mEllipsoid0->Center;
		Matrix3<Real> R0(mEllipsoid0->Axis, true);
		Matrix3<Real> D0(
			((Real)1)/(mEllipsoid0->Extent[0]*mEllipsoid0->Extent[0]),
			((Real)1)/(mEllipsoid0->Extent[1]*mEllipsoid0->Extent[1]),
			((Real)1)/(mEllipsoid0->Extent[2]*mEllipsoid0->Extent[2]));

		// Get the parameters of ellipsoid1.
		Vector3<Real> K1 = mEllipsoid1->Center;
		Matrix3<Real> R1(mEllipsoid1->Axis, true);
		Matrix3<Real> D1(
			((Real)1)/(mEllipsoid1->Extent[0]*mEllipsoid1->Extent[0]),
			((Real)1)/(mEllipsoid1->Extent[1]*mEllipsoid1->Extent[1]),
			((Real)1)/(mEllipsoid1->Extent[2]*mEllipsoid1->Extent[2]));

		// Compute K2.
		Matrix3<Real> D0NegHalf(
			mEllipsoid0->Extent[0],
			mEllipsoid0->Extent[1],
			mEllipsoid0->Extent[2]);
		Matrix3<Real> D0Half(
			((Real)1)/mEllipsoid0->Extent[0],
			((Real)1)/mEllipsoid0->Extent[1],
			((Real)1)/mEllipsoid0->Extent[2]);
		Vector3<Real> K2 = D0Half*((K1 - K0)*R0);

		// Compute M2.
		Matrix3<Real> R1TR0D0NegHalf = R1.TransposeTimes(R0*D0NegHalf);
		Matrix3<Real> M2 = R1TR0D0NegHalf.TransposeTimes(D1)*R1TR0D0NegHalf;

		// Factor M2 = R*D*R^T.
		Matrix3<Real> R, D;
		M2.EigenDecomposition(R, D);

		// Compute K = R^T*K2.
		Vector3<Real> K = K2*R;

		// Transformed ellipsoid0 is Z^T*Z = 1 and transformed ellipsoid1 is
		// (Z-K)^T*D*(Z-K) = 0.

		// The minimum and maximum squared distances from the origin of points on
		// transformed ellipsoid1 are used to determine whether the ellipsoids
		// intersect, are separated, or one contains the other.
		Real minSqrDistance = Math<Real>::MAX_REAL;
		Real maxSqrDistance = (Real)0;
		int i;

		if (K == Vector3<Real>::ZERO)
		{
			// The special case of common centers must be handled separately.  It
			// is not possible for the ellipsoids to be separated.
			for (i = 0; i < 3; ++i)
			{
				Real invD = ((Real)1)/D[i][i];
				if (invD < minSqrDistance)
				{
					minSqrDistance = invD;
				}
				if (invD > maxSqrDistance)
				{
					maxSqrDistance = invD;
				}
			}

			if (maxSqrDistance < (Real)1)
			{
				return EC_ELLIPSOID0_CONTAINS_ELLIPSOID1;
			}
			else if (minSqrDistance > (Real)1)
			{
				return EC_ELLIPSOID1_CONTAINS_ELLIPSOID0;
			}
			else
			{
				return EC_ELLIPSOIDS_INTERSECTING;
			}
		}

		// The closest point P0 and farthest point P1 are solutions to
		// s0*D*(P0 - K) = P0 and s1*D*(P1 - K) = P1 for some scalars s0 and s1
		// that are roots to the function
		//   f(s) = d0*k0^2/(d0*s-1)^2+d1*k1^2/(d1*s-1)^2+d2*k2^2/(d2*s-1)^2-1
		// where D = diagonal(d0,d1,d2) and K = (k0,k1,k2).
		Real d0 = D[0][0], d1 = D[1][1], d2 = D[2][2];
		Real c0 = K[0]*K[0], c1 = K[1]*K[1], c2 = K[2]*K[2];

		// Sort the values so that d0 >= d1 >= d2.  This allows us to bound the
		// roots of f(s), of which there are at most 6.
		std::vector<std::pair<Real,Real> > param(3);
		param[0] = std::make_pair(d0, c0);
		param[1] = std::make_pair(d1, c1);
		param[2] = std::make_pair(d2, c2);
		std::sort(param.begin(), param.end(),
			std::greater<std::pair<Real,Real> >());

		std::vector<std::pair<Real,Real> > valid;
		valid.reserve(3);
		if (param[0].first > param[1].first)
		{
			if (param[1].first > param[2].first)
			{
				// d0 > d1 > d2
				for (i = 0; i < 3; ++i)
				{
					if (param[i].second > (Real)0)
					{
						valid.push_back(param[i]);
					}
				}
			}
			else
			{
				// d0 > d1 = d2
				if (param[0].second > (Real)0)
				{
					valid.push_back(param[0]);
				}
				param[1].second += param[0].second;
				if (param[1].second > (Real)0)
				{
					valid.push_back(param[1]);
				}
			}
		}
		else
		{
			if (param[1].first > param[2].first)
			{
				// d0 = d1 > d2
				param[0].second += param[1].second;
				if (param[0].second > (Real)0)
				{
					valid.push_back(param[0]);
				}
				if (param[2].second > (Real)0)
				{
					valid.push_back(param[2]);
				}
			}
			else
			{
				// d0 = d1 = d2
				param[0].second += param[1].second + param[2].second;
				if (param[0].second > (Real)0)
				{
					valid.push_back(param[0]);
				}
			}
		}

		size_t numValid = valid.size();
		int numRoots;
		Real roots[6];
		if (numValid == 3)
		{
			GetRoots(
				valid[0].first, valid[1].first, valid[2].first,
				valid[0].second, valid[1].second, valid[2].second,
				numRoots, roots);
		}
		else if (numValid == 2)
		{
			GetRoots(
				valid[0].first, valid[1].first,
				valid[0].second, valid[1].second,
				numRoots, roots);
		}
		else if (numValid == 1)
		{
			GetRoots(
				valid[0].first,
				valid[0].second,
				numRoots, roots);
		}
		else
		{
			// numValid cannot be zero because we already handled case K = 0
			assertion(false, "Unexpected condition.\n");
			return EC_ELLIPSOIDS_INTERSECTING;
		}

		for (i = 0; i < numRoots; ++i)
		{
			Real s = roots[i];
			Real p0 = d0*K[0]*s/(d0*s - (Real)1);
			Real p1 = d1*K[1]*s/(d1*s - (Real)1);
			Real p2 = d2*K[2]*s/(d2*s - (Real)1);
			Real sqrDistance = p0*p0 + p1*p1 + p2*p2;
			if (sqrDistance < minSqrDistance)
			{
				minSqrDistance = sqrDistance;
			}
			if (sqrDistance > maxSqrDistance)
			{
				maxSqrDistance = sqrDistance;
			}
		}

		if (maxSqrDistance < (Real)1)
		{
			return EC_ELLIPSOID0_CONTAINS_ELLIPSOID1;
		}

		if (minSqrDistance > (Real)1)
		{
			if (d0*c0 + d1*c1 + d2*c2 > (Real)1)
			{
				return EC_ELLIPSOIDS_SEPARATED;
			}
			else
			{
				return EC_ELLIPSOID1_CONTAINS_ELLIPSOID0;
			}
		}

		return EC_ELLIPSOIDS_INTERSECTING;
	}
	//----------------------------------------------------------------------------
	template <typename Real>
	void IntrEllipsoid3Ellipsoid3<Real>::BisectF (Real d0, Real d1, Real d2,
		Real d0c0, Real d1c1, Real d2c2, Real smin, Real fmin, Real smax,
		Real fmax, Real& s, Real& f)
	{
		bool increasing = (fmin < (Real)0);

		const int maxIterations = 256;
		for (int i = 0; i < maxIterations; ++i)
		{
			s = ((Real)0.5)*(smin + smax);
			if (smin < s)
			{
				if (s < smax)
				{
					Real invN0 = ((Real)1)/(d0*s - (Real)1);
					Real invN1 = ((Real)1)/(d1*s - (Real)1);
					Real invN2 = ((Real)1)/(d2*s - (Real)1);
					Real invN0Sqr = invN0*invN0;
					Real invN1Sqr = invN1*invN1;
					Real invN2Sqr = invN2*invN2;
					f = d0c0*invN0Sqr + d1c1*invN1Sqr + d2c2*invN2Sqr - (Real)1;
					if (f < (Real)0)
					{
						if (increasing)
						{
							smin = s;
							fmin = f;
						}
						else
						{
							smax = s;
							fmax = f;
						}
					}
					else if (f > (Real)0)
					{
						if (increasing)
						{
							smax = s;
							fmax = f;
						}
						else
						{
							smin = s;
							fmin = f;
						}
					}
					else
					{
						break;
					}
				}
				else
				{
					f = fmax;
					break;
				}
			}
			else
			{
				f = fmin;
				break;
			}
		}
	}
	//----------------------------------------------------------------------------
	template <typename Real>
	void IntrEllipsoid3Ellipsoid3<Real>::BisectDF (Real d0, Real d1, Real d2,
		Real d0c0, Real d1c1, Real d2c2, Real smin, Real dfmin, Real smax,
		Real dfmax, Real& s, Real& df)
	{
		const int maxIterations = 256;
		for (int i = 0; i < maxIterations; ++i)
		{
			s = ((Real)0.5)*(smin + smax);
			if (smin < s)
			{
				if (s < smax)
				{
					Real invN0 = ((Real)1)/(d0*s - (Real)1);
					Real invN1 = ((Real)1)/(d1*s - (Real)1);
					Real invN2 = ((Real)1)/(d2*s - (Real)1);
					Real invN0Cub = invN0*invN0*invN0;
					Real invN1Cub = invN1*invN1*invN1;
					Real invN2Cub = invN2*invN2*invN2;
					df = ((Real)-2)*(d0*d0c0*invN0Cub + d1*d1c1*invN1Cub +
						d2*d2c2*invN2Cub);
					if (df < (Real)0)
					{
						smin = s;
						dfmin = df;
					}
					else if (df > (Real)0)
					{
						smax = s;
						dfmax = df;
					}
					else
					{
						break;
					}
				}
				else
				{
					df = dfmax;
					break;
				}
			}
			else
			{
				df = dfmin;
				break;
			}
		}
	}
	//----------------------------------------------------------------------------
	template <typename Real>
	void IntrEllipsoid3Ellipsoid3<Real>::GetRoots (Real d0, Real d1, Real d2,
		Real c0, Real c1, Real c2, int& numRoots, Real* roots)
	{
		// f(s) = d0*c0/(d0*s-1)^2 + d1*c1/(d1*s-1)^2 + d2*c2/(d2*s-1)^2 - 1
		// with d0 > d1 > d2

		numRoots = 0;

		// f(s) = d0*c0/(d0*s-1)^2 + d1*c1/(d1*s-1)^2 - 1
		// with d0 > d1
		Real epsilon = (Real)0.001;
		Real multiplier0 = Math<Real>::Sqrt(((Real)3)/((Real)1 - epsilon));
		Real multiplier1 = Math<Real>::Sqrt(((Real)1)/((Real)1 + epsilon));
		Real d0c0 = d0*c0;
		Real d1c1 = d1*c1;
		Real d2c2 = d2*c2;
		Real sqrtd0c0 = Math<Real>::Sqrt(d0c0);
		Real sqrtd1c1 = Math<Real>::Sqrt(d1c1);
		Real sqrtd2c2 = Math<Real>::Sqrt(d2c2);
		Real invD0 = ((Real)1)/d0;
		Real invD1 = ((Real)1)/d1;
		Real invD2 = ((Real)1)/d2;
		Real temp0, temp1, temp2, smin, smax, s, fmin, fmax, f;
		Real invN0, invN1, invN2, invN0Sqr, invN1Sqr, invN2Sqr;

		// Compute root in (-infinity,1/d0).
		temp0 = ((Real)1 - multiplier0*sqrtd0c0)*invD0;
		temp1 = ((Real)1 - multiplier0*sqrtd1c1)*invD1;
		temp2 = ((Real)1 - multiplier0*sqrtd2c2)*invD2;
		smin = (temp0 < temp1 ? temp0 : temp1);
		smin = (temp2 < smin ? temp2 : smin);
		invN0 = ((Real)1)/(d0*smin - (Real)1);
		invN1 = ((Real)1)/(d1*smin - (Real)1);
		invN2 = ((Real)1)/(d2*smin - (Real)1);
		invN0Sqr = invN0*invN0;
		invN1Sqr = invN1*invN1;
		invN2Sqr = invN2*invN2;
		fmin = d0c0*invN0Sqr + d1c1*invN1Sqr + d2c2*invN2Sqr - (Real)1;
		assertion(fmin < (Real)0, "Unexpected condition.\n");
		smax = ((Real)1 - multiplier1*sqrtd0c0)*invD0;
		invN0 = ((Real)1)/(d0*smax - (Real)1);
		invN1 = ((Real)1)/(d1*smax - (Real)1);
		invN2 = ((Real)1)/(d2*smax - (Real)1);
		invN0Sqr = invN0*invN0;
		invN1Sqr = invN1*invN1;
		invN2Sqr = invN2*invN2;
		fmax = d0c0*invN0Sqr + d1c1*invN1Sqr + d2c2*invN2Sqr - (Real)1;
		assertion(fmax > (Real)0, "Unexpected condition.\n");
		BisectF(d0, d1, d2, d0c0, d1c1, d2c2, smin, fmin, smax, fmax, s, f);
		roots[numRoots++] = s;

		// Compute roots (if any) in (1/d0,1/d1).
		Real smid, fmid, df;
		BisectDF(d0, d1, d2, d0c0, d1c1, d2c2, invD0, -Math<Real>::MAX_REAL,
			invD1, Math<Real>::MAX_REAL, smid, df);
		invN0 = ((Real)1)/(d0*smid - (Real)1);
		invN1 = ((Real)1)/(d1*smid - (Real)1);
		invN2 = ((Real)1)/(d2*smid - (Real)1);
		invN0Sqr = invN0*invN0;
		invN1Sqr = invN1*invN1;
		invN2Sqr = invN2*invN2;
		fmid = d0c0*invN0Sqr + d1c1*invN1Sqr + d2c2*invN2Sqr - (Real)1;
		if (fmid < (Real)0)
		{
			BisectF(d0, d1, d2, d0c0, d1c1, d2c2, invD0, Math<Real>::MAX_REAL,
				smid, fmid, s, f);
			roots[numRoots++] = s;
			BisectF(d0, d1, d2, d0c0, d1c1, d2c2, smid, fmid, invD1,
				Math<Real>::MAX_REAL, s, f);
			roots[numRoots++] = s;
		}

		// Compute roots (if any) in (1/d1,1/d2).
		BisectDF(d0, d1, d2, d0c0, d1c1, d2c2, invD1, -Math<Real>::MAX_REAL,
			invD2, Math<Real>::MAX_REAL, smid, df);
		invN0 = ((Real)1)/(d0*smid - (Real)1);
		invN1 = ((Real)1)/(d1*smid - (Real)1);
		invN2 = ((Real)1)/(d2*smid - (Real)1);
		invN0Sqr = invN0*invN0;
		invN1Sqr = invN1*invN1;
		invN2Sqr = invN2*invN2;
		fmid = d0c0*invN0Sqr + d1c1*invN1Sqr + d2c2*invN2Sqr - (Real)1;
		if (fmid < (Real)0)
		{
			BisectF(d0, d1, d2, d0c0, d1c1, d2c2, invD1, Math<Real>::MAX_REAL,
				smid, fmid, s, f);
			roots[numRoots++] = s;
			BisectF(d0, d1, d2, d0c0, d1c1, d2c2, smid, fmid, invD2,
				Math<Real>::MAX_REAL, s, f);
			roots[numRoots++] = s;
		}

		// Compute root in (1/d2,+infinity).
		temp0 = ((Real)1 + multiplier0*sqrtd0c0)*invD0;
		temp1 = ((Real)1 + multiplier0*sqrtd1c1)*invD1;
		temp2 = ((Real)1 + multiplier0*sqrtd2c2)*invD2;
		smax = (temp0 > temp1 ? temp0 : temp1);
		smax = (temp2 > smax ? temp2 : smax);
		invN0 = ((Real)1)/(d0*smax - (Real)1);
		invN1 = ((Real)1)/(d1*smax - (Real)1);
		invN2 = ((Real)1)/(d2*smax - (Real)1);
		invN0Sqr = invN0*invN0;
		invN1Sqr = invN1*invN1;
		invN2Sqr = invN2*invN2;
		fmax = d0c0*invN0Sqr + d1c1*invN1Sqr + d2c2*invN2Sqr - (Real)1;
		assertion(fmax < (Real)0, "Unexpected condition.\n");
		smin = ((Real)1 + multiplier1*sqrtd2c2)*invD2;
		invN0 = ((Real)1)/(d0*smin - (Real)1);
		invN1 = ((Real)1)/(d1*smin - (Real)1);
		invN2 = ((Real)1)/(d2*smin - (Real)1);
		invN0Sqr = invN0*invN0;
		invN1Sqr = invN1*invN1;
		invN2Sqr = invN2*invN2;
		fmin = d0c0*invN0Sqr + d1c1*invN1Sqr + d2c2*invN2Sqr - (Real)1;
		assertion(fmin > (Real)0, "Unexpected condition.\n");
		BisectF(d0, d1, d2, d0c0, d1c1, d2c2, smin, fmin, smax, fmax, s, f);
		roots[numRoots++] = s;
	}
	//----------------------------------------------------------------------------
	template <typename Real>
	void IntrEllipsoid3Ellipsoid3<Real>::BisectF (Real d0, Real d1, Real d0c0,
		Real d1c1, Real smin, Real fmin, Real smax, Real fmax, Real& s, Real& f)
	{
		bool increasing = (fmin < (Real)0);

		const int maxIterations = 256;
		for (int i = 0; i < maxIterations; ++i)
		{
			s = ((Real)0.5)*(smin + smax);
			if (smin < s)
			{
				if (s < smax)
				{
					Real invN0 = ((Real)1)/(d0*s - (Real)1);
					Real invN1 = ((Real)1)/(d1*s - (Real)1);
					Real invN0Sqr = invN0*invN0;
					Real invN1Sqr = invN1*invN1;
					f = d0c0*invN0Sqr + d1c1*invN1Sqr - (Real)1;
					if (f < (Real)0)
					{
						if (increasing)
						{
							smin = s;
							fmin = f;
						}
						else
						{
							smax = s;
							fmax = f;
						}
					}
					else if (f > (Real)0)
					{
						if (increasing)
						{
							smax = s;
							fmax = f;
						}
						else
						{
							smin = s;
							fmin = f;
						}
					}
					else
					{
						break;
					}
				}
				else
				{
					f = fmax;
					break;
				}
			}
			else
			{
				f = fmin;
				break;
			}
		}
	}
	//----------------------------------------------------------------------------
	template <typename Real>
	void IntrEllipsoid3Ellipsoid3<Real>::BisectDF (Real d0, Real d1, Real d0c0,
		Real d1c1, Real smin, Real dfmin, Real smax, Real dfmax, Real& s,
		Real& df)
	{
		const int maxIterations = 256;
		for (int i = 0; i < maxIterations; ++i)
		{
			s = ((Real)0.5)*(smin + smax);
			if (smin < s)
			{
				if (s < smax)
				{
					Real invN0 = ((Real)1)/(d0*s - (Real)1);
					Real invN1 = ((Real)1)/(d1*s - (Real)1);
					Real invN0Cub = invN0*invN0*invN0;
					Real invN1Cub = invN1*invN1*invN1;
					df = ((Real)-2)*(d0*d0c0*invN0Cub + d1*d1c1*invN1Cub);
					if (df < (Real)0)
					{
						smin = s;
						dfmin = df;
					}
					else if (df > (Real)0)
					{
						smax = s;
						dfmax = df;
					}
					else
					{
						break;
					}
				}
				else
				{
					df = dfmax;
					break;
				}
			}
			else
			{
				df = dfmin;
				break;
			}
		}
	}
	//----------------------------------------------------------------------------
	template <typename Real>
	void IntrEllipsoid3Ellipsoid3<Real>::GetRoots (Real d0, Real d1, Real c0,
		Real c1, int& numRoots, Real* roots)
	{
		// f(s) = d0*c0/(d0*s-1)^2 + d1*c1/(d1*s-1)^2 - 1
		// with d0 > d1

		numRoots = 0;

		Real epsilon = (Real)0.001;
		Real multiplier0 = Math<Real>::Sqrt(((Real)2)/((Real)1 - epsilon));
		Real multiplier1 = Math<Real>::Sqrt(((Real)1)/((Real)1 + epsilon));
		Real d0c0 = d0*c0;
		Real d1c1 = d1*c1;
		Real sqrtd0c0 = Math<Real>::Sqrt(d0c0);
		Real sqrtd1c1 = Math<Real>::Sqrt(d1c1);
		Real invD0 = ((Real)1)/d0;
		Real invD1 = ((Real)1)/d1;
		Real temp0, temp1, smin, smax, s, fmin, fmax, f;
		Real invN0, invN1, invN0Sqr, invN1Sqr;

		// Compute root in (-infinity,1/d0).
		temp0 = ((Real)1 - multiplier0*sqrtd0c0)*invD0;
		temp1 = ((Real)1 - multiplier0*sqrtd1c1)*invD1;
		smin = (temp0 < temp1 ? temp0 : temp1);
		invN0 = ((Real)1)/(d0*smin - (Real)1);
		invN1 = ((Real)1)/(d1*smin - (Real)1);
		invN0Sqr = invN0*invN0;
		invN1Sqr = invN1*invN1;
		fmin = d0c0*invN0Sqr + d1c1*invN1Sqr - (Real)1;
		assertion(fmin < (Real)0, "Unexpected condition.\n");
		smax = ((Real)1 - multiplier1*sqrtd0c0)*invD0;
		invN0 = ((Real)1)/(d0*smax - (Real)1);
		invN1 = ((Real)1)/(d1*smax - (Real)1);
		invN0Sqr = invN0*invN0;
		invN1Sqr = invN1*invN1;
		fmax = d0c0*invN0Sqr + d1c1*invN1Sqr - (Real)1;
		assertion(fmax > (Real)0, "Unexpected condition.\n");
		BisectF(d0, d1, d0c0, d1c1, smin, fmin, smax, fmax, s, f);
		roots[numRoots++] = s;

		// Compute roots (if any) in (1/d0,1/d1).
		Real smid, fmid, df;
		BisectDF(d0, d1, d0c0, d1c1, invD0, -Math<Real>::MAX_REAL, invD1,
			Math<Real>::MAX_REAL, smid, df);
		invN0 = ((Real)1)/(d0*smid - (Real)1);
		invN1 = ((Real)1)/(d1*smid - (Real)1);
		invN0Sqr = invN0*invN0;
		invN1Sqr = invN1*invN1;
		fmid = d0c0*invN0Sqr + d1c1*invN1Sqr - (Real)1;
		if (fmid < (Real)0)
		{
			BisectF(d0, d1, d0c0, d1c1, invD0, Math<Real>::MAX_REAL, smid, fmid,
				s, f);
			roots[numRoots++] = s;
			BisectF(d0, d1, d0c0, d1c1, smid, fmid, invD1, Math<Real>::MAX_REAL,
				s, f);
			roots[numRoots++] = s;
		}

		// Compute root in (1/d1,+infinity).
		temp0 = ((Real)1 + multiplier0*sqrtd0c0)*invD0;
		temp1 = ((Real)1 + multiplier0*sqrtd1c1)*invD1;
		smax = (temp0 > temp1 ? temp0 : temp1);
		invN0 = ((Real)1)/(d0*smax - (Real)1);
		invN1 = ((Real)1)/(d1*smax - (Real)1);
		invN0Sqr = invN0*invN0;
		invN1Sqr = invN1*invN1;
		fmax = d0c0*invN0Sqr + d1c1*invN1Sqr - (Real)1;
		assertion(fmax < (Real)0, "Unexpected condition.\n");
		smin = ((Real)1 + multiplier1*sqrtd1c1)*invD1;
		invN0 = ((Real)1)/(d0*smin - (Real)1);
		invN1 = ((Real)1)/(d1*smin - (Real)1);
		invN0Sqr = invN0*invN0;
		invN1Sqr = invN1*invN1;
		fmin = d0c0*invN0Sqr + d1c1*invN1Sqr - (Real)1;
		assertion(fmin > (Real)0, "Unexpected condition.\n");
		BisectF(d0, d1, d0c0, d1c1, smin, fmin, smax, fmax, s, f);
		roots[numRoots++] = s;
	}
	//----------------------------------------------------------------------------
	template <typename Real>
	void IntrEllipsoid3Ellipsoid3<Real>::GetRoots (Real d0, Real c0,
		int& numRoots, Real* roots)
	{
		// f(s) = d0*c0/(d0*s-1)^2 - 1
		Real temp = Math<Real>::Sqrt(d0*c0);
		Real inv = ((Real)1)/d0;
		numRoots = 2;
		roots[0] = ((Real)1 - temp)*inv;
		roots[1] = ((Real)1 + temp)*inv;
	}
	//----------------------------------------------------------------------------

	//----------------------------------------------------------------------------
	// Explicit instantiation.
	//----------------------------------------------------------------------------
	template WM5_MATHEMATICS_ITEM
	class IntrEllipsoid3Ellipsoid3<float>;

	template WM5_MATHEMATICS_ITEM
	class IntrEllipsoid3Ellipsoid3<double>;
	//----------------------------------------------------------------------------
}