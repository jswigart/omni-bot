////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __USERFLAGS_H__
#define __USERFLAGS_H__

#include <stdint.h>

class BitFlag32
{
public:
	bool AnyFlagSet() const
	{
		return mFlags != 0;
	}
	bool CheckFlag( int32_t _flag ) const
	{
		return ( mFlags & ( (int32_t)1 << _flag ) ) != 0;
	}
	void SetFlag( int32_t _flag )
	{
		mFlags |= ( (int32_t)1 << _flag );
	}
	void SetFlag( int32_t _flag, bool _set )
	{
		if ( _set )
			SetFlag( _flag );
		else
			ClearFlag( _flag );
	}
	void ClearFlag( int32_t _flag )
	{
		mFlags &= ~( (int32_t)1 << _flag );
	}
	void ClearAll()
	{
		mFlags = 0;
	}
	BitFlag32& operator&=( const BitFlag32& rhs )
	{
		mFlags &= rhs.mFlags;
		return *this;
	}
	BitFlag32 operator&( const BitFlag32& rhs ) const
	{
		BitFlag32 bf( *this );
		bf &= rhs;
		return bf;
	}
	BitFlag32& operator|=( const BitFlag32& rhs )
	{
		mFlags |= rhs.mFlags;
		return *this;
	}
	BitFlag32 operator|( const BitFlag32& rhs ) const
	{
		BitFlag32 bf( *this );
		bf |= rhs;
		return bf;
	}
	BitFlag32 operator~( ) const
	{
		BitFlag32 bf( *this );
		bf.mFlags = ~bf.mFlags;
		return bf;
	}
	bool operator<( int32_t _rhs ) const
	{
		return mFlags < _rhs;
	}
	bool operator==( const BitFlag32& r ) const
	{
		return mFlags == r.mFlags;
	}
	bool operator!=( const BitFlag32& r ) const
	{
		return mFlags != r.mFlags;
	}
	int32_t GetRawFlags() const
	{
		return mFlags;
	}
	explicit BitFlag32( int32_t flgs = 0 ) : mFlags( flgs )
	{
	}
private:
	int32_t mFlags;
};

class BitFlag64
{
public:
	bool AnyFlagSet() const
	{
		return mFlags != 0;
	}
	bool CheckFlag( int32_t _flag ) const
	{
		int64_t flg = (int64_t)1 << _flag;
		return ( mFlags & flg ) != 0;
	}
	void SetFlag( int32_t _flag )
	{
		mFlags |= ( (int64_t)1 << _flag );
	}
	void SetFlag( int32_t _flag, bool _set )
	{
		if ( _set )
			SetFlag( _flag );
		else
			ClearFlag( _flag );
	}
	void ClearFlag( int32_t _flag )
	{
		mFlags &= ~( (int64_t)1 << _flag );
	}
	void ClearAll()
	{
		mFlags = 0;
	}
	BitFlag64& operator&=( const BitFlag64& rhs )
	{
		mFlags &= rhs.mFlags;
		return *this;
	}
	BitFlag64 operator&( const BitFlag64& rhs ) const
	{
		BitFlag64 bf( *this );
		bf &= rhs;
		return bf;
	}
	BitFlag64& operator|=( const BitFlag64& rhs )
	{
		mFlags |= rhs.mFlags;
		return *this;
	}
	BitFlag64 operator|( const BitFlag64& rhs ) const
	{
		BitFlag64 bf( *this );
		bf |= rhs;
		return bf;
	}
	BitFlag64 operator~( ) const
	{
		BitFlag64 bf( *this );
		bf.mFlags = ~bf.mFlags;
		return bf;
	}
	bool operator<( int64_t _rhs ) const
	{
		return mFlags < _rhs;
	}
	bool operator==( const BitFlag64& r ) const
	{
		return mFlags == r.mFlags;
	}
	bool operator!=( const BitFlag64& r ) const
	{
		return mFlags != r.mFlags;
	}
	explicit BitFlag64( int64_t flgs = 0 ) : mFlags( flgs )
	{
	}
private:
	int64_t mFlags;
};

class BitFlag128
{
public:
	bool AnyFlagSet() const
	{
		return ( mLo | mHi ) != 0;
	}
	bool CheckFlag( int32_t _flag ) const
	{
		if ( _flag < 64 )
		{
			return ( mLo & ( (int64_t)1 << _flag ) ) != 0;
		}
		else
		{
			return ( mHi & ( (int64_t)1 << ( _flag - 64 ) ) ) != 0;
		}
	}
	void SetFlag( int32_t _flag )
	{
		if ( _flag < 64 )
		{
			mLo |= ( (int64_t)1 << _flag );
		}
		else
		{
			mHi |= ( (int64_t)1 << ( _flag - 64 ) );
		}
	}
	void SetFlag( int32_t _flag, bool _set )
	{
		if ( _set )
			SetFlag( _flag );
		else
			ClearFlag( _flag );
	}
	void ClearFlag( int32_t _flag )
	{
		if ( _flag < 64 )
		{
			mLo &= ~( (int64_t)1 << _flag );
		}
		else
		{
			mHi &= ~( (int64_t)1 << ( _flag - 64 ) );
		}
	}
	void ClearAll()
	{
		mLo = mHi = 0;
	}
	BitFlag128 operator&( const BitFlag128& rhs ) const
	{
		return BitFlag128( mLo & rhs.mLo, mHi & rhs.mHi );
	}
	explicit BitFlag128( int64_t lo = 0, int64_t hi = 0 ) : mLo( lo ), mHi( hi )
	{
	}
private:
	int64_t mLo, mHi;
};

#endif
