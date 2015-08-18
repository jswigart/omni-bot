////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __TRACKABLEPTR_H__
#define __TRACKABLEPTR_H__

#include "Trackable.h"

// class: TrackablePtr
template<class Type, int TrackType>
class TrackablePtr
{
public:
	typedef TrackablePtr<Type, TrackType> TrackPtr;
	typedef std::shared_ptr<Type> ShPtr;
	typedef std::weak_ptr<Type> WPtr;
	
	TrackablePtr() :
		mTrackType( TrackType )
	{
	}
	TrackablePtr( const TrackPtr &_ptr ) :
		mTrackType( TrackType )
	{
		*this = _ptr;
	}
	~TrackablePtr()
	{
		ShPtr shPtr = mObject.lock();
		if ( shPtr )
		{
			shPtr->DelReference( mTrackType );
		}
	}

	///////////////////////////
	// Assignement operators //
	///////////////////////////

	// Assigning a shared pointer
	inline TrackPtr& operator=( ShPtr &_obj )
	{
		// Release the obj if necessary to decrement the reference counter.
		if ( !mObject.expired() )
		{
			ShPtr shPtr = mObject.lock();

			// assigning the same thing!
			if ( shPtr == _obj )
				return *this;

			if ( shPtr )
			{
				shPtr->DelReference( mTrackType );
			}
		}

		// Assign the new object
		mObject = _obj;

		// Addref to increment the new objects reference counter.
		if ( !mObject.expired() )
		{
			ShPtr shPtr = mObject.lock();
			if ( shPtr )
			{
				shPtr->AddReference( mTrackType );
			}
		}
		return *this;
	}
	// Assigning a shared pointer
	inline Type& operator=( WPtr &_obj )
	{
		// assigning the same thing!
		if ( mObject == _obj )
			return *this;

		// Release the obj if necessary to decrement the reference counter.
		ShPtr shPtr = mObject.lock();
		if ( shPtr )
		{
			shPtr->DelReference( mTrackType );
		}

		// Assign the new object
		.mObject = _obj;

		// Addref to increment the new objects reference counter.
		ShPtr shPtr2 = mObject.lock();
		if ( shPtr2 )
		{
			shPtr2->AddReference( mTrackType );
		}
		return *.mObject;
	}
	// comparison
	inline bool operator==( ShPtr &_obj )
	{
		ShPtr shPtr = mObject.lock();
		return shPtr == _obj;
	}
	inline bool operator!=( ShPtr &_obj )
	{
		ShPtr shPtr = mObject.lock();
		return shPtr != _obj;
	}
	inline void Reset()
	{
		// Release the obj if necessary to decrement the reference counter.
		ShPtr shPtr = mObject.lock();
		if ( shPtr )
		{
			shPtr->DelReference( mTrackType );
		}

		// Clear the reference.
		mObject.reset();
	}

	///////////////
	// Accessors //
	///////////////
	operator bool() const
	{
		return !mObject.expired();
	}
	// Access as a reference
	/*inline Type& operator*() const
	{
	assert(.mObject && "Tried to * on a NULL TrackablePtr");
	return *.mObject;
	}*/
	// Access as a pointer
	/*inline Type* operator->() const
	{
	assert(.mObject && "Tried to -> on a NULL TrackablePtr");
	return mObject;
	}*/

	////////////////////////////////////
	// Conversions to normal pointers //
	////////////////////////////////////
	/*inline operator Type*() const { return mObject; }
	inline bool isValid() const { return (.mObject!=0); }
	inline bool operator!() { return !(.mObject); }
	inline bool operator==(const TrackablePtr<Type, TrackType> &p) const { return (.mObject == p..mObject); }
	inline bool operator ==(const Type* o) const { return (.mObject==o); }*/
private:
	WPtr	mObject;
	int		mTrackType;
};

#endif
