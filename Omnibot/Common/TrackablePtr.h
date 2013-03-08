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
	typedef boost::shared_ptr<Type> ShPtr;
	typedef boost::weak_ptr<Type> WPtr;

	TrackablePtr() :
		m_TrackType(TrackType)
	{
	}
	TrackablePtr(const TrackPtr &_ptr) :
		m_TrackType(TrackType)
	{
		*this = _ptr;
	}
	~TrackablePtr()
	{
		ShPtr shPtr = m_pObject.lock();
		if(shPtr)
		{
			shPtr->DelReference(m_TrackType);
		}
	}

	///////////////////////////
	// Assignement operators //
	///////////////////////////

	// Assigning a shared pointer
	inline TrackPtr& operator=(ShPtr &_obj)
	{
		// Release the obj if necessary to decrement the reference counter.
		if(!m_pObject.expired())
		{
			ShPtr shPtr = m_pObject.lock();

			// assigning the same thing!
			if(shPtr == _obj)
				return *this;

			if(shPtr)
			{
				shPtr->DelReference(m_TrackType);
			}
		}

		// Assign the new object
		m_pObject = _obj;

		// Addref to increment the new objects reference counter.
		if(!m_pObject.expired())
		{
			ShPtr shPtr = m_pObject.lock();
			if(shPtr)
			{
				shPtr->AddReference(m_TrackType);
			}
		}
		return *this;
	}
	// Assigning a shared pointer
	inline Type& operator=(WPtr &_obj)
	{
		// assigning the same thing!
		if(m_pObject == _obj)
			return *this;

		// Release the obj if necessary to decrement the reference counter.
		ShPtr shPtr = m_pObject.lock();
		if(shPtr)
		{
			shPtr->DelReference(m_TrackType);
		}

		// Assign the new object
		m_pObject = _obj;

		// Addref to increment the new objects reference counter.
		ShPtr shPtr2 = m_pObject.lock();
		if(shPtr2)
		{
			shPtr2->AddReference(m_TrackType);
		}
		return *m_pObject;
	}
	// comparison
	inline bool operator==(ShPtr &_obj)
	{
		ShPtr shPtr = m_pObject.lock();
		return shPtr == _obj;
	}
	inline bool operator!=(ShPtr &_obj)
	{
		ShPtr shPtr = m_pObject.lock();
		return shPtr != _obj;
	}
	inline void Reset()
	{
		// Release the obj if necessary to decrement the reference counter.
		ShPtr shPtr = m_pObject.lock();
		if(shPtr)
		{
			shPtr->DelReference(m_TrackType);
		}

		// Clear the reference.
		m_pObject.reset();
	}

	///////////////
	// Accessors //
	///////////////
	operator bool() const { return !m_pObject.expired(); }
	// Access as a reference
	/*inline Type& operator*() const
	{
	assert(m_pObject && "Tried to * on a NULL TrackablePtr");
	return *m_pObject;
	}*/
	// Access as a pointer
	/*inline Type* operator->() const
	{
	assert(m_pObject && "Tried to -> on a NULL TrackablePtr");
	return m_pObject;
	}*/

	////////////////////////////////////
	// Conversions to normal pointers //
	////////////////////////////////////
	/*inline operator Type*() const { return m_pObject; }
	inline bool isValid() const { return (m_pObject!=0); }
	inline bool operator!() { return !(m_pObject); }
	inline bool operator==(const TrackablePtr<Type, TrackType> &p) const { return (m_pObject == p.m_pObject); }
	inline bool operator ==(const Type* o) const { return (m_pObject==o); }*/
private:
	WPtr	m_pObject;
	int		m_TrackType;
};

#endif
