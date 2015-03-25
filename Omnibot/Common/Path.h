////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __PATH_H__
#define __PATH_H__

#include "common.h"

// class: Path
//		This class represents a traversable path through the environment.
class Path
{
public:
	struct PathPoint
	{
		Vector3f		 mPt;
		float			 mRadius;
		NavFlags		 mNavFlags;

		int				 mNavId;
		uint32_t		 mOnPathThrough;
		uint32_t		 mOnPathThroughParam;

		PathPoint &Position( const Vector3f &aPos )
		{
			mPt = aPos; return *this;
		}
		PathPoint &Radius( float aRadius )
		{
			mRadius = aRadius; return *this;
		}
		PathPoint &Flags( NavFlags aNavFlags )
		{
			mNavFlags = aNavFlags; return *this;
		}
		PathPoint &NavId( int aNavId )
		{
			mNavId = aNavId; return *this;
		}
		PathPoint &OnPathThrough( uint32_t aPathThrough )
		{
			mOnPathThrough = aPathThrough; return *this;
		}
		PathPoint &OnPathThroughParam( uint32_t aPathThroughParam )
		{
			mOnPathThroughParam = aPathThroughParam; return *this;
		}

		PathPoint()
			: mPt( Vector3f::ZERO )
			, mRadius( 0.f )
			, mNavFlags( 0 )
			, mNavId( 0 )
			, mOnPathThrough( 0 )
			, mOnPathThroughParam( 0 )
		{
		}
	};

	struct PathLink
	{
		float mDistance;
		PathLink() : mDistance( 0.f )
		{
		}
	};

	void Clear();

	Path::PathPoint &AddPt( const Vector3f &_pt, float _radius );
	bool NextPt();

	void Append( const Path &_path );

	Vector3f FindNearestPtOnPath( const Vector3f &_position, Vector3f *_outLookAhead = NULL, float _lookAheadDistance = 0.f );

	bool GetCurrentPt( PathPoint &_pt ) const;
	bool GetNextPt( PathPoint &_pt ) const;
	bool GetPreviousPt( PathPoint &_pt ) const;
	bool GetFirstPt( PathPoint &_pt ) const;
	bool GetLastPt( PathPoint &_pt ) const;
	bool GetPt( int _index, PathPoint &_pt ) const;

	inline int GetNumPts() const
	{
		return mNumPts;
	}
	inline int GetCurrentPtIndex() const
	{
		return mCurrentPt;
	}
	inline bool IsEndOfPath() const
	{
		return mCurrentPt >= mNumPts - 1;
	}

	float GetTotalLength() const;

	void DebugRender( obColor _color = COLOR::GREEN );

	Path();
private:
	static const int MAX_PATH_PTS = 512;
	PathPoint mPts[ MAX_PATH_PTS ];
	PathLink mLinks[ MAX_PATH_PTS ]; // store total distance at the end [MAX_PATH_PTS-1]

	// state info
	int	 mCurrentPt;
	int	 mNumPts;
};

#endif
