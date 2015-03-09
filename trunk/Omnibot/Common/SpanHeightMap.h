#pragma once

#ifndef _SPANHEIGHTMAP_H_
#define _SPANHEIGHTMAP_H_

#include <queue>

#include "influencemap.pb.h"

// Protocol buffers
#include "google/protobuf/text_format.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"

template<typename Data>
class SpanHeightMap
{
public:
	friend class InfluenceMap;

	struct Span
	{
		Span *			mNext;
		float			mMax;
		float			mMin;

		unsigned int	mIndex;
		Data			mData;

		Span()
			: mMin( 0.0f )
			, mMax( 0.0f )
			, mNext( NULL )
			, mIndex( 0 )
			, mData()
		{
		}
	};

	struct SpanInfo
	{
		const Span *	mSpan;
		unsigned int	mSpanX;
		unsigned int	mSpanY;
	};

	class InfluenceMap
	{
	public:
		InfluenceMap( const SpanHeightMap<Data> * map )
			: mSpanMap( map )
		{
			mInfluences.resize( map->mNumSpans, std::numeric_limits<float>::max() );
			mInfluences.assign( mInfluences.size(), std::numeric_limits<float>::max() );
		}
		struct SpanWeight
		{
			SpanInfo	mSpanInfo;
			float	mSpanWeight;

			SpanWeight( const SpanInfo & info, float weight )
				: mSpanInfo( info )
				, mSpanWeight( weight )
			{
			}
		};
		void Reset()
		{
			SearchQueue empty;
			mSearchFrontier.swap( empty );

			mInfluences.resize( mSpanMap->mNumSpans, std::numeric_limits<float>::max() );
			mInfluences.assign( mInfluences.size(), std::numeric_limits<float>::max() );

			mMinValue = std::numeric_limits<float>::max();
			mMaxValue = std::numeric_limits<float>::lowest();
		}
		void UpdateWeight( float weight )
		{
			if ( weight < mMinValue )
				mMinValue = weight;
			if ( weight > mMaxValue )
				mMaxValue = weight;
		}
		void AddSeed( const Vector3f & pos, float weight )
		{
			SpanInfo s;
			if ( mSpanMap->GetSpanInfoForPos( pos, s ) )
			{
				mSearchFrontier.push( SpanWeight( s, weight ) );

				UpdateWeight( weight );
			}
		}
		bool UpdateInfluences( int maxiterations )
		{
			const float cs = mSpanMap->GetCellSize();

			int iterations = 0;
			while ( !mSearchFrontier.empty() && iterations++ < maxiterations )
			{
				SpanWeight s = mSearchFrontier.front();
				mSearchFrontier.pop();

				// returns true if the cell value changed
				enum { MaxNeighbors = 16 };
				SpanInfo neighbors[ MaxNeighbors ];
				const int numNeighbors = mSpanMap->GetSpanNeighbors(
					s.mSpanInfo,
					64.0f, 32.0f, 1024.0f,
					neighbors, MaxNeighbors);

				for ( int i = 0; i < numNeighbors; ++i )
				{
					float & weight = mInfluences[ neighbors[ i ].mSpan->mIndex ];
					if ( weight > (s.mSpanWeight+cs) )
					{
						weight = s.mSpanWeight+cs;
						mSearchFrontier.push( SpanWeight( neighbors[ i ], weight ) );
						UpdateWeight( weight );
					}
				}
			}
			return iterations < maxiterations;
		}
		void GetWeightRange( float & minWeight, float & maxWeight )
		{
			minWeight = mMinValue;
			maxWeight = mMaxValue;
		}
		float GetWeight( unsigned int index )
		{
			return mInfluences[ index ];
		}
		size_t CalculateMemUsage() const
		{
			size_t sz = sizeof(*this);

			sz += sizeof( float ) * mInfluences.capacity();

			return sz;
		}
	private:
		float						mMinValue;
		float						mMaxValue;

		const SpanHeightMap<Data> * mSpanMap;

		typedef std::queue<SpanWeight> SearchQueue;
		SearchQueue					mSearchFrontier;

		std::vector<float>			mInfluences;
	};

	inline int IDX( const int x, const int y ) const
	{
		return y * mNumCellsX + x;
	}

	void MarkDirtyBounds( const Box3f & obb )
	{
		// dirty the whole AABB
		Vector3f vertices[ 8 ];
		obb.ComputeVertices( vertices );

		AxisAlignedBox3f aabb;
		aabb.Clear();

		for ( size_t i = 0; i < 8; ++i )
			aabb.ExpandPt( vertices[ i ] );
		
		const int minx = ClampT<int>( (int)( ( aabb.Min.X() - mMin.X() ) / mCellSize ), 0, mNumCellsX-1 );
		const int miny = ClampT<int>( (int)( ( aabb.Min.Y() - mMin.Y() ) / mCellSize ), 0, mNumCellsY-1 );
		const int maxx = ClampT<int>( (int)( ( aabb.Max.X() - mMin.X() ) / mCellSize ), 0, mNumCellsX-1 );
		const int maxy = ClampT<int>( (int)( ( aabb.Max.Y() - mMin.Y() ) / mCellSize ), 0, mNumCellsY-1 );

		for ( int x = minx; x < maxx; ++x )
		{
			for ( int y = miny; y < maxy; ++y )
			{
				//mSpans[ IDX( x,y ) ] = NULL;
			}
		}
	}
	bool AddOpenSpan( const Vector3f & pos, const float height )
	{
		const unsigned int ix = (unsigned int)( ( pos.X() - mMin.X() ) / mCellSize );
		const unsigned int iy = (unsigned int)( ( pos.Y() - mMin.Y() ) / mCellSize );

		if ( ix >= mNumCellsX || iy >= mNumCellsY )
			return NULL;

		const float spanMin = pos.Z();
		const float spanMax = pos.Z() + height;

		Span * head = mSpans[ IDX( ix, iy ) ];
		if ( head == NULL || spanMax < head->mMin )
		{
			// should it go at the head of the list?
			Span * top = new Span;
			top->mMin = spanMin;
			top->mMax = spanMax;
			top->mNext = mSpans[ IDX( ix, iy ) ];
			mSpans[ IDX( ix, iy ) ] = top;
			return true;
		}

		bool spanAdded = false;

		Span * prev = NULL;
		Span * cur = mSpans[ IDX( ix, iy ) ];
		while( cur )
		{
			if (cur->mMin > spanMax)
			{
				// went past it, so stop searching
				break;
			}
			else if (cur->mMax < spanMin)
			{
				// search through the spans before
				prev = cur;
				cur = cur->mNext;
			}
			else
			{
				// Merge spans.
				if (cur->mMin < spanMin)
					cur->mMin = spanMin;
				if (cur->mMax > spanMax)
					cur->mMax = spanMax;

				// new span is already handled?
				if (cur->mMin >= spanMin && cur->mMax <= spanMax)
					return false;

				return true;
			}
		}

		if (prev)
		{
			Span * s = new Span;
			s->mMin = spanMin;
			s->mMax = spanMax;

			s->mNext = prev->mNext;
			prev->mNext = s;

			return true;
		}

		return spanAdded;
	}

	InfluenceMap * CreateInfluenceLayer()
	{
		return new InfluenceMap( this );
	}

	Span * GetSpanForPos( const Vector3f & p )
	{
		const unsigned int ix = ( p.X() - mMin.X() ) / mCellSize;
		const unsigned int iy = ( p.Y() - mMin.Y() ) / mCellSize;

		if ( ix >= mNumCellsX || iy >= mNumCellsY )
			return NULL;

		Span * s = mSpans[ IDX( ix, iy ) ];
		while ( s )
		{
			if ( p.Z() >= s->mMin && p.Z() <= s->mMax )
				return s;

			s = s->mNext;
		}
		return NULL;
	}

	bool GetSpanInfoForPos( const Vector3f & p, SpanInfo & spanInfo ) const
	{
		const unsigned int ix = (unsigned int)(( p.X() - mMin.X() ) / mCellSize);
		const unsigned int iy = (unsigned int)(( p.Y() - mMin.Y() ) / mCellSize);

		if ( ix >= mNumCellsX || iy >= mNumCellsY )
			return false;

		Span * s = mSpans[ IDX( ix, iy ) ];
		while ( s )
		{
			if ( p.Z() >= s->mMin && p.Z() <= s->mMax )
			{
				spanInfo.mSpan = s;
				spanInfo.mSpanX = ix;
				spanInfo.mSpanY = iy;
				return true;
			}
			s = s->mNext;
		}
		return false;
	}
	int GetSpanNeighbors( const SpanInfo & src, float height, float stepup, float stepdown, SpanInfo * spans, int maxSpans ) const
	{
		const int xoff[4] = { -1, 1,  0, 0 };
		const int yoff[4] = {  0, 0, -1, 1 };

		int numSpans = 0;
		for ( int n = 0; n < 4; ++n )
		{
			const unsigned int newX = src.mSpanX + xoff[n];
			const unsigned int newY = src.mSpanY + yoff[n];

			if ( newX < mNumCellsX && newY < mNumCellsY )
			{
				Span * s = mSpans[ IDX( newX, newY ) ];
				while ( s )
				{
					if ( s->mMin < src.mSpan->mMax )
					{
						const float floordiff = s->mMin - src.mSpan->mMin;
						if ( floordiff <= stepup &&
							floordiff >= -stepdown &&
							( s->mMax - src.mSpan->mMin ) > height )
						{
							spans[numSpans].mSpan = s;
							spans[numSpans].mSpanX = newX;
							spans[numSpans].mSpanY = newY;
							++numSpans;
						}
					}
					if ( s->mMin > src.mSpan->mMax )
						break;
					s = s->mNext;
				}
			}
		}
		return numSpans;
	}
	Data * GetSpanData( const Vector3f & p )
	{
		Span * s = GetSpanForPos( p );
		return s != NULL ? &s->mData : NULL;
	}

	void Init( const Vector3f & mins, const Vector3f & maxs, const float cellSize )
	{
		mMin = mins;
		mMax = maxs;
		mCellSize = cellSize;
		mNumCellsX = (unsigned int)ceil( ( maxs.X() - mins.X() ) / cellSize );
		mNumCellsY = (unsigned int)ceil( ( maxs.Y() - mins.Y() ) / cellSize );

		mSpans = new Span *[ mNumCellsX * mNumCellsY ];

		for ( unsigned int i = 0; i < mNumCellsX; ++i )
		{
			for ( unsigned int j = 0; j < mNumCellsY; ++j )
			{
				mSpans[ IDX( i,j ) ] = NULL;
			}
		}
	}

	void ClearSpans()
	{
		for ( unsigned int i = 0; i < mNumCellsX; ++i )
		{
			for ( unsigned int j = 0; j < mNumCellsY; ++j )
			{
				Span * s = mSpans[ IDX( i,j ) ];
				while ( s )
				{
					Span * d = s;
					s = s->mNext;
					delete d;
				}
				mSpans[ IDX( i,j ) ] = NULL;
			}
		}
	}

	void Clear()
	{
		ClearSpans();

		delete [] mSpans;
		mSpans = NULL;

		mMin = Vector3f::ZERO;
		mMax = Vector3f::ZERO;
		mCellSize = 0.0f;
		mNumCellsX = 0;
		mNumCellsY = 0;
	}

	Vector3f GetBoundsMin() const { return mMin; }
	Vector3f GetBoundsMax() const { return mMax; }
	float GetCellSize() const { return mCellSize; }
	int GetNumCellsX() const { return mNumCellsX; }
	int GetNumCellsY() const { return mNumCellsY; }

	Span * GetSpan( const unsigned int x, const unsigned int y )
	{
		if ( x >= mNumCellsX || y >= mNumCellsY )
			return NULL;
		return mSpans[ IDX( x, y ) ];
	}

	const Span * GetSpan( const unsigned int x, const unsigned int y ) const
	{
		if ( x >= mNumCellsX || y >= mNumCellsY )
			return NULL;
		return mSpans[ IDX( x, y ) ];
	}

	struct RenderFunctor
	{
		virtual ~RenderFunctor() {}
		virtual void RenderCell( const Vector3f & pos, float cellSize, float influenceRatio ) = 0;
	};
	void RenderWithCallback( RenderFunctor & renderCb, InfluenceMap * influenceMap ) const
	{
		float weightMin = 0.0f, weightMax = 1.0f;
		if ( influenceMap != NULL )
			influenceMap->GetWeightRange( weightMin, weightMax );
		const float influenceRange = weightMax - weightMin;

		for ( unsigned int i = 0; i < mNumCellsX; ++i )
		{
			for ( unsigned int j = 0; j < mNumCellsY; ++j )
			{
				Vector3f spanPos = mMin + Vector3f( i * mCellSize, j * mCellSize, 0.0f );

				Span * s = mSpans[ IDX( i,j ) ];
				while ( s )
				{
					spanPos.Z() = s->mMin;

					const float influence = influenceMap ?
						influenceMap->GetWeight( s->mIndex ) : 1.0f;
					
					if ( influence == std::numeric_limits<float>::max() )
						renderCb.RenderCell( spanPos, mCellSize, -1.0f );
					else
						renderCb.RenderCell( spanPos, mCellSize, (influence - weightMin) / influenceRange);

					s = s->mNext;
				}
			}
		}
	}
	unsigned int IndexSpanNodes()
	{
		mNumSpans = 0;
		for ( unsigned int i = 0; i < mNumCellsX; ++i )
		{
			for ( unsigned int j = 0; j < mNumCellsY; ++j )
			{
				Vector3f spanPos = mMin + Vector3f( i * mCellSize, j * mCellSize, 0.0f );

				Span * s = mSpans[ IDX( i,j ) ];
				while ( s )
				{
					s->mIndex = mNumSpans++;
					s = s->mNext;
				}
			}
		}
		return mNumSpans;
	}

	unsigned int GetNumSpans() const
	{
		return mNumSpans;
	}

	size_t CalculateMemUsage() const
	{
		size_t sz = sizeof(*this);

		sz += sizeof(Span *) * mNumCellsX * mNumCellsY;

		for ( unsigned int i = 0; i < mNumCellsX; ++i )
		{
			for ( unsigned int j = 0; j < mNumCellsY; ++j )
			{
				const Span * s = mSpans[ IDX( i,j ) ];
				while ( s )
				{
					sz += sizeof(Span);
					s = s->mNext;
				}
			}
		}
		return sz;
	}

	bool Serialize( std::string & dataOut )
	{
		bool result = false;

		try
		{
			InfluenceMapIO::InfluenceMap map;
			map.mutable_header()->mutable_boundsmin()->set_x( mMin.X() );
			map.mutable_header()->mutable_boundsmin()->set_y( mMin.Y() );
			map.mutable_header()->mutable_boundsmin()->set_z( mMin.Z() );
			map.mutable_header()->mutable_boundsmax()->set_x( mMax.X() );
			map.mutable_header()->mutable_boundsmax()->set_y( mMax.Y() );
			map.mutable_header()->mutable_boundsmax()->set_z( mMax.Z() );
			map.mutable_header()->set_cellsize( mCellSize );
			map.mutable_header()->set_numcellsx( mNumCellsX );
			map.mutable_header()->set_numcellsy( mNumCellsY );

			for ( unsigned int i = 0; i < mNumCellsX; ++i )
			{
				for ( unsigned int j = 0; j < mNumCellsY; ++j )
				{
					InfluenceMapIO::SpanColumn * column = map.add_sectors();

					const Span * s = mSpans[ IDX( i,j ) ];
					while ( s )
					{
						InfluenceMapIO::Span * span = column->add_spans();
						span->set_min( s->mMin );
						span->set_max( s->mMax );
						s = s->mNext;
					}
				}

				result = map.SerializeToString( &dataOut );
			}
		}
		catch (std::exception* e)
		{
			LOG("PathPlannerNavMesh::Save ERROR: " << e->what() );
		}

		return result;
	}

	bool DeSerialize( const std::string & dataIn )
	{
		bool result = false;

		try
		{
			Clear();

			InfluenceMapIO::InfluenceMap map;
			if ( map.ParseFromString( dataIn ) )
			{
				mMin.X() = map.header().boundsmin().x();
				mMin.Y() = map.header().boundsmin().y();
				mMin.Z() = map.header().boundsmin().z();
				mMax.X() = map.header().boundsmax().x();
				mMax.Y() = map.header().boundsmax().y();
				mMax.Z() = map.header().boundsmax().z();
				mCellSize = map.header().cellsize();
				mNumCellsX = map.header().numcellsx();
				mNumCellsY = map.header().numcellsy();

				mSpans = new Span *[ mNumCellsX * mNumCellsY ];

				for ( unsigned int i = 0; i < mNumCellsX; ++i )
				{
					for ( unsigned int j = 0; j < mNumCellsY; ++j )
					{
						mSpans[ IDX( i, j ) ] = NULL;

						const InfluenceMapIO::SpanColumn & column = map.sectors( IDX( i,j ) );

						Span * lastSpan = NULL;
						for ( int s = 0; s < column.spans_size(); ++s )
						{
							const InfluenceMapIO::Span & span = column.spans( s );

							Span * newSpan = new Span();
							newSpan->mMin = span.min();
							newSpan->mMax = span.max();

							if ( lastSpan == NULL )
								mSpans[ IDX( i,j ) ] = newSpan;
							else
								lastSpan->mNext = newSpan;

							lastSpan = newSpan;
						}
					}
				}

				IndexSpanNodes();
				result = true;
			}
		}
		catch (std::exception* e)
		{
			LOG("PathPlannerNavMesh::Save ERROR: " << e->what() );
			return false;
		}

		return true;
	}

	SpanHeightMap()
		: mSpans( NULL )
		, mMin( Vector3f::ZERO )
		, mMax( Vector3f::ZERO )
		, mCellSize( 0.0f )
		, mNumCellsX( 0 )
		, mNumCellsY( 0 )
		, mNumSpans( 0 )
	{
	}
private:
	Vector3f		mMin;
	Vector3f		mMax;
	float			mCellSize;
	unsigned int	mNumCellsX;
	unsigned int	mNumCellsY;
	unsigned int	mNumSpans;
	Span **			mSpans;


};

#endif