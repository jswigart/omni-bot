#pragma once

#ifndef _CONCURRENT_QUEUE_H_
#define _CONCURRENT_QUEUE_H_

#include <queue>
#include <boost/thread/condition_variable.hpp>

template<typename T>
class ConcurrentQueue
{
public:
	void Push( const T & data )
	{
		boost::mutex::scoped_lock lock( mMutex );
		mQueue.push( data );
		lock.unlock();
		mConditionVar.notify_one();
	}

	bool TryPush( const T & data )
	{
		boost::mutex::scoped_lock lock( mMutex );
		if ( mQueue.size() < mMaxSize )
		{
			mQueue.push( data );
			lock.unlock();
			mConditionVar.notify_one();
			return true;
		}
		return false;
	}

	bool Empty() const
	{
		boost::mutex::scoped_lock lock( mMutex );
		return mQueue.empty();
	}

	size_t Size() const
	{
		boost::mutex::scoped_lock lock( mMutex );
		return mQueue.size();
	}

	bool TryAndPop( T & data )
	{
		boost::mutex::scoped_lock lock( mMutex );
		if ( mQueue.empty() )
			return false;
		
		data = mQueue.front();
		mQueue.pop();
		return true;
	}

	void WaitAndPop( T & data )
	{
		boost::mutex::scoped_lock lock( mMutex );
		while ( mQueue.empty() )
		{
			mConditionVar.wait( lock );
		}

		data = mQueue.front();
		mQueue.pop();
	}

	ConcurrentQueue( const size_t maxSize = std::numeric_limits<size_t>::max() ) : mMaxSize( maxSize ) {}
	~ConcurrentQueue() {}
private:
	typedef std::queue<T> Queue;

	const size_t				mMaxSize;
	Queue						mQueue;
	mutable boost::mutex		mMutex;
	boost::condition_variable	mConditionVar;
};

#endif
