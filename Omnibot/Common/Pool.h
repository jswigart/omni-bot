#pragma optimize

#ifndef POOL_H
#define POOL_H

template<typename T>
class Pool
{
public:
	T * Alloc()
	{
		T * obj = NULL;
		if ( mFreeObjs.size() > 0 )
		{
			obj = mFreeObjs.back();
			mFreeObjs.pop_back();
		}
		else
		{
			obj = new T();
		}
		return obj;
	}
	void Free( T * obj )
	{
		mFreeObjs.push_back( obj );
	}
	void Free( const std::vector<T*> & objs )
	{
		mFreeObjs.reserve( mFreeObjs.size() + objs.size() );
		mFreeObjs.insert( mFreeObjs.end(), objs.begin(), objs.end() );
	}
	void PreAlloc( size_t sz )
	{
		if ( mFreeObjs.size() != sz )
		{
			mFreeObjs.resize( mRuntimeSectors.size(), NULL );
			for ( size_t i = 0; i < mFreeObjs.size(); ++i )
				if ( mFreeObjs[ i ] == NULL )
					mFreeObjs[ i ] = new T();
		}
	}
private:
	std::vector<T*>	mFreeObjs;
};

#endif