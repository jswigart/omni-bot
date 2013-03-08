#pragma once

#include "primitives.h"

namespace RemoteLib
{
	namespace SystemLib
	{
		unsigned int getTime();

		int16 hostToNetByteOrder( int16 v );
		int32 hostToNetByteOrder( int32 v );

		int16 netToHostByteOrder( int16 v );
		int32 netToHostByteOrder( int32 v );

		void formatByteString( uint32 _bytes, char * str, int maxSize );
	};
}
