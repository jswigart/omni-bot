#include "SystemLib.h"

#include <stdio.h>
#include <math.h>
#include <winsock2.h>
#include <MMSystem.h>

namespace RemoteLib
{
	namespace SystemLib
	{
		unsigned int getTime() {
			return timeGetTime();
		}

		// endian helpers
		int16 hostToNetByteOrder( int16  v ) {
			return ( int16 )htons( (u_short)v );
		}
		int32 hostToNetByteOrder( int32  v ) {
			return ( int32 )htonl( (u_long)v );
		}
		int16 netToHostByteOrder( int16  v ) {
			return ( int16 )ntohs( (u_short)v );
		}
		int32 netToHostByteOrder( int32  v ) {
			return ( int32 )ntohl( (u_long)v );
		}

		void formatByteString( uint32 _bytes, char * str, int maxSize )
		{
			static const char * byteUnits[] =
			{
				" bytes",
				" KB",
				" MB",
				" GB",
				" TB"
			};

			const int32 numUnits = sizeof(byteUnits) / sizeof(byteUnits[0]);
			int unit = 0;
			for( int32 i = 1; i < numUnits; ++i )
			{
				if(_bytes / pow( 1024.0, i ) >= 1)
					unit = i;
			}

			const float value = (unit > 0 ? (_bytes / pow(1024.f, unit)) : _bytes);
			const char * unitStr = byteUnits[unit];
			_snprintf( str, maxSize, "%.1f%s", value, unitStr );
		}
	}
}