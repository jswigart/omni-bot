////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "Path.h"
#include "RenderBuffer.h"
#include "Utilities.h"

Path::Path() :
mCurrentPt( 0 ),
mNumPts( 0 )
{
}

void Path::Clear()
{
	mCurrentPt = 0;
	mNumPts = 0;
}

Path::PathPoint &Path::AddPt( const Vector3f &_pt, float _radius )
{
	static PathPoint s_pp;

	if ( mNumPts < MAX_PATH_PTS - 1 )
	{
		mPts[ mNumPts ].mPt = _pt;
		mPts[ mNumPts ].mRadius = _radius;
		mPts[ mNumPts ].mNavFlags = 0;
		mPts[ mNumPts ].mOnPathThrough = 0;

		// If this isn't the first node in the path, update the cached distances.
		if ( mNumPts > 0 )
		{
			mLinks[ mNumPts ].mDistance =
				mLinks[ mNumPts - 1 ].mDistance + ( mPts[ mNumPts ].mPt - mPts[ mNumPts - 1 ].mPt ).Length();
		}
		else
		{
			mLinks[ 0 ].mDistance = 0.f;
		}
		return mPts[ mNumPts++ ];
	}

	return s_pp;
}

bool Path::NextPt()
{
	if ( mCurrentPt < mNumPts - 1 )
	{
		mCurrentPt++;
		return true;
	}
	else
	{
		return false;
	}
}

void Path::Append( const Path &_path )
{
	for ( int i = 0; i < MAX_PATH_PTS; ++i )
	{
		mPts[ i ] = _path.mPts[ i ];
		mLinks[ i ] = _path.mLinks[ i ];
	}
	mNumPts += _path.GetNumPts();
}

bool Path::GetCurrentPt( PathPoint &_pt ) const
{
	if ( mNumPts > 0 )
	{
		_pt = mPts[ mCurrentPt ];
		return true;
	}
	return false;
}

bool Path::GetNextPt( PathPoint &_pt ) const
{
	if ( mNumPts > 0 && mCurrentPt + 1 < mNumPts )
	{
		_pt = mPts[ mCurrentPt + 1 ];
		return true;
	}
	return false;
}

bool Path::GetPreviousPt( PathPoint &_pt ) const
{
	if ( mNumPts > 0 && mCurrentPt > 0 )
	{
		_pt = mPts[ mCurrentPt - 1 ];
		return true;
	}
	return false;
}

bool Path::GetFirstPt( PathPoint &_pt ) const
{
	if ( mNumPts > 0 )
	{
		_pt = mPts[ 0 ];
		return true;
	}
	return false;
}

bool Path::GetLastPt( PathPoint &_pt ) const
{
	if ( mNumPts > 0 )
	{
		_pt = mPts[ mNumPts - 1 ];
		return true;
	}
	return false;
}

bool Path::GetPt( int _index, PathPoint &_pt ) const
{
	if ( _index >= mNumPts || _index > MAX_PATH_PTS - 1 )
		return GetLastPt( _pt );
	if ( _index < 0 )
		return GetFirstPt( _pt );
	_pt = mPts[ _index ];
	return true;
}

float Path::GetTotalLength() const
{
	return mLinks[ mNumPts - 1 ].mDistance;
}

void Path::DebugRender( obColor _color )
{
	for ( int32_t i = 1; i < mNumPts; ++i )
	{
		RenderBuffer::AddLine( mPts[ i - 1 ].mPt, mPts[ i ].mPt, _color );
	}
}

Vector3f Path::FindNearestPtOnPath( const Vector3f &_position, Vector3f *_outLookAhead /*= NULL*/, float _lookAheadDistance/* = 0.f*/ )
{
	Vector3f vClosestPt = _position;
	if ( _outLookAhead )
		*_outLookAhead = _position;

	if (/*.mCurrentPt <= 0 || */mNumPts == 1 )
	{
		vClosestPt = mPts[ 0 ].mPt;
		if ( _outLookAhead )
			*_outLookAhead = vClosestPt;
	}
	else
	{
		float fClosestRatioOnSeg = 0.f;
		Vector3f vPtOnSeg;

		float fClosestDist = Utils::FloatMax;
		int iStartPt = 0, iEndPt = 0;
		int iStart = MaxT( mCurrentPt - 1, 0 );
		int iEnd = MinT( iStart + 2, mNumPts - 1 );
		for ( int i = iStart; i < iEnd; ++i )
		{
			float fRatioOnSeg;
			float fDist = PointToSegmentDistance( _position, mPts[ i ].mPt, mPts[ i + 1 ].mPt, vPtOnSeg, fRatioOnSeg );
			if ( fDist < fClosestDist )
			{
				fClosestDist = fDist;
				vClosestPt = vPtOnSeg;
				fClosestRatioOnSeg = fRatioOnSeg;
				iStartPt = i;
				iEndPt = i + 1;
			}
		}

		// calculate lookahead point
		if ( _outLookAhead && _lookAheadDistance > 0.f && ( iStartPt != 0 || iEndPt != 0 ) )
		{
			const float fCurrentDistanceOnPath = mLinks[ iStartPt ].mDistance +
				( mLinks[ iEndPt ].mDistance - mLinks[ iStartPt ].mDistance ) * fClosestRatioOnSeg;

			const float fDesiredDistanceOnPath = fCurrentDistanceOnPath + _lookAheadDistance;

			bool bFound = false;
			for ( int i = iStartPt; i < mNumPts - 1; ++i )
			{
				if ( mLinks[ i + 1 ].mDistance >= fDesiredDistanceOnPath )
				{
					float fDistanceLeft = fDesiredDistanceOnPath - mLinks[ i ].mDistance;

					const float fT = fDistanceLeft / ( mLinks[ i + 1 ].mDistance - mLinks[ i ].mDistance );
					*_outLookAhead = Interpolate(
						mPts[ i ].mPt,
						mPts[ i + 1 ].mPt,
						ClampT( fT, 0.f, 1.f ) );
					bFound = true;
					break;
				}
			}
			if ( !bFound )
				*_outLookAhead = mPts[ mNumPts - 1 ].mPt; // last point
		}
	}
	return vClosestPt;
}