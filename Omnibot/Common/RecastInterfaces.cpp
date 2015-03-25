#include "RecastInterfaces.h"

RecastBuildContext::RecastBuildContext()
{
}

RecastBuildContext::~RecastBuildContext()
{
}

// Virtual functions for custom implementations.
void RecastBuildContext::doResetLog()
{
	messages.resize( 0 );
}

void RecastBuildContext::doLog( const rcLogCategory category, const char* msg, const int len )
{
	if ( !len )
		return;

	messages.push_back( Msg() );
	messages.back().mCategory = category;
	messages.back().mMsg.assign( msg, len );
}

void RecastBuildContext::doResetTimers()
{
	for ( int i = 0; i < RC_MAX_TIMERS; ++i )
		maccTime[ i ] = -1;
}

void RecastBuildContext::doStartTimer( const rcTimerLabel label )
{
	mtimers[ label ].Reset();
}

void RecastBuildContext::doStopTimer( const rcTimerLabel label )
{
	const float deltaTime = (float)mtimers[ label ].GetElapsedSeconds();
	if ( maccTime[ label ] == -1 )
		maccTime[ label ] = deltaTime;
	else
		maccTime[ label ] += deltaTime;
}

int RecastBuildContext::doGetAccumulatedTime( const rcTimerLabel label ) const
{
	return (int)( maccTime[ label ] * 1000.0f );
}

void RecastBuildContext::dumpLog( const char* format, ... )
{
	// Print header.
	//va_list ap;
	//va_start(ap, format);
	//vprintf(format, ap);
	//va_end(ap);
	//printf("\n");

	//// Print messages
	//const int TAB_STOPS[4] = { 28, 36, 44, 52 };
	//for (int i = 0; i < mmessageCount; ++i)
	//{
	//	const char* msg = messages[i]+1;
	//	int n = 0;
	//	while (*msg)
	//	{
	//		if (*msg == '\t')
	//		{
	//			int count = 1;
	//			for (int j = 0; j < 4; ++j)
	//			{
	//				if (n < TAB_STOPS[j])
	//				{
	//					count = TAB_STOPS[j] - n;
	//					break;
	//				}
	//			}
	//			while (--count)
	//			{
	//				putchar(' ');
	//				n++;
	//			}
	//		}
	//		else
	//		{
	//			putchar(*msg);
	//			n++;
	//		}
	//		msg++;
	//	}
	//	putchar('\n');
	//}
}

int RecastBuildContext::getLogCount() const
{
	return messages.size();
}

const char* RecastBuildContext::getLogText( const int i ) const
{
	return messages[ i ].mMsg.c_str();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

rcFileIO::rcFileIO() :
	mmode( -1 )
{
}

rcFileIO::~rcFileIO()
{
	mfile.Close();
}

bool rcFileIO::openForWrite( const char* path )
{
	mmode = 1;
	return mfile.OpenForWrite( path, File::Text, false );
}

bool rcFileIO::openForRead( const char* path )
{
	mmode = 2;
	return mfile.OpenForRead( path, File::Text );
}

bool rcFileIO::isWriting() const
{
	return mmode == 1;
}

bool rcFileIO::isReading() const
{
	return mmode == 2;
}

bool rcFileIO::write( const void* ptr, const size_t size )
{
	if ( mfile.IsOpen() )
	{
		mfile.Write( ptr, size, 1 );
		return true;
	}
	return false;
}

bool rcFileIO::read( void* ptr, const size_t size )
{
	if ( mfile.IsOpen() )
	{
		mfile.Read( ptr, size, 1 );
		return true;
	}
	return false;
}