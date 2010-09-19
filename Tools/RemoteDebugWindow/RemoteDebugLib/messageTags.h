#include "primitives.h"

#define REMOTELIB_MAKE_ID32( a, b, c, d )  ( ((d)<<24) | ((c)<<16) | ((b)<<8) | (a))

namespace RemoteLib
{
	enum MsgTagId 
	{
		ID_ack = REMOTELIB_MAKE_ID32('A','C','K','0'),
		ID_treeNode = REMOTELIB_MAKE_ID32('T','R','E','N'),
		ID_circle = REMOTELIB_MAKE_ID32('C','I','R','C'),
		ID_line = REMOTELIB_MAKE_ID32('L','I','N','E'),
		ID_image = REMOTELIB_MAKE_ID32('I','M','A','G'),
	};
};
