////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __MESSAGEHELPER_H__
#define __MESSAGEHELPER_H__

#include <stdint.h>
#include <assert.h>

//////////////////////////////////////////////////////////////////////////

struct SubscriberHandle
{
	union
	{
		struct
		{
			short mMessageId;
			short mSerialNum;
		} split;
		int mInt;
	} u;
};

//////////////////////////////////////////////////////////////////////////

class MessageHelper
{
public:
	friend class MessageRouter;

	template<class Type>
	Type *Get() const
	{
		assert( sizeof( Type ) == mBlockSize && "Memory Block Doesn't match!" );
		return static_cast<Type*>( mPtr );
	}

	template<class Type>
	void Get2( Type *&_p ) const
	{
		assert( sizeof( Type ) == mBlockSize && "Memory Block Doesn't match!" );
		_p = static_cast<Type*>( mPtr );
	}

	int GetMessageId() const
	{
		return mMessageId;
	}

	operator bool() const
	{
		return ( mMessageId != 0 );
	}

	MessageHelper( int _msgId, void *_void = 0, uint32_t _size = 0 )
		: mMessageId( _msgId )
		, mPtr( _void )
		, mBlockSize( _size )
	{
	}
	~MessageHelper()
	{
	};
private:
	mutable int mMessageId;
	void*		mPtr;
	uint32_t	mBlockSize;

	MessageHelper();
};

#define OB_GETMSG(msgtype) msgtype *pMsg = 0; _data.Get2(pMsg);

//////////////////////////////////////////////////////////////////////////

typedef void( *pfnMessageFunction )( const MessageHelper &_helper );

//////////////////////////////////////////////////////////////////////////

#endif
