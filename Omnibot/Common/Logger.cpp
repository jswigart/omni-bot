// ---------------------------------------------------------------------------------------------------------------------------------
//  _
// | |
// | | ___   __ _  __ _  ___ _ __      ___ _ __  _ __
// | |/ _ \ / _` |/ _` |/ _ \ '__|    / __| '_ \| '_ \
// | | (_) | (_| | (_| |  __/ |    _ | (__| |_) | |_) |
// |_|\___/ \__, |\__, |\___|_|   (_) \___| .__/| .__/
//           __/ | __/ |                  | |   | |
//          |___/ |___/                   |_|   |_|
//
// Generic informational logging class
//
// ---------------------------------------------------------------------------------------------------------------------------------
//
// Restrictions & freedoms pertaining to usage and redistribution of this software:
//
//  * This software is 100% free
//  * If you use this software (in part or in whole) you must credit the author.
//  * This software may not be re-distributed (in part or in whole) in a modified
//    form without clear documentation on how to obtain a copy of the original work.
//  * You may not use this software to directly or indirectly cause harm to others.
//  * This software is provided as-is and without warrantee. Use at your own risk.
//
// For more information, visit HTTP://www.FluidStudios.com
//
// ---------------------------------------------------------------------------------------------------------------------------------
// Originally created on 07/06/2000 by Paul Nettle
// Modified Heavily by Jeremy Swigart
//
// Copyright 2000, Fluid Studios, Inc., all rights reserved.
// ---------------------------------------------------------------------------------------------------------------------------------

#include <fstream>
#include <iostream>
#include <iomanip>
//#include <strstream>
#include <sstream>
#include <sys/types.h>
//#include <sys/stat.h>

//#include <stdio.h>
#include <cstdio>
#include <cstring>
#include <stdarg.h>

#ifdef __GNUC__
#include <unistd.h>
#endif

#include "Logger.h"

// ---------------------------------------------------------------------------------------------------------------------------------
// The global logger object
// ---------------------------------------------------------------------------------------------------------------------------------

Logger g_Logger;

// ---------------------------------------------------------------------------------------------------------------------------------

std::string g_FileName = "omnibot.log";

Logger::Logger()
	: mSourceLine( 0 )
	, mIndentCount( 0 )
	, mIndentChars( 4 )
	, mFileSizeLimit( -1 )
	, mLogMask( LOG_ALL )
	, mOutMask( WRITE_TIMESTAMP )
	, mLineCharsFlag( false )
{
}

Logger::~Logger()
{
	Stop();
}

bool Logger::LogStarted()
{
	return mLogFile.is_open();
}

void Logger::LimitFileSize()
{
	if ( FileSizeLimit() < 0 || !mLogFile.is_open() )
		return;

	if ( mLogFile.tellp() > FileSizeLimit() )
	{
		mLogFile.close();
		mLogFile.open( g_FileName.c_str(), std::ios::out | std::ios::trunc );
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

void Logger::Start( const std::string &_filename, const bool reset )
{
	if ( LogStarted() )
		return;

	g_FileName = _filename;

	// Get the time
	time_t t = time( NULL );
	std::string ts = asctime( localtime( &t ) );

	// Start the log
	LimitFileSize();
	mLogFile.open( g_FileName.c_str(), std::ios::out | ( reset ? std::ios::trunc : std::ios::app ) );
	if ( !mLogFile.is_open() )
		return;

	mLogFile << "---------------- Log begins on " << ts << " ---------------" << std::endl;
}

// ---------------------------------------------------------------------------------------------------------------------------------

void Logger::Stop()
{
	if ( !LogStarted() )
		return;

	// Get the time
	time_t	t = time( NULL );
	std::string	ts = asctime( localtime( &t ) );
	ts.erase( ts.length() - 1 );

	// Stop the log
	LimitFileSize();
	mLogFile << "---------------- Log ends on " << ts << " -----------------" << std::endl;
	mLogFile.close();
}

// ---------------------------------------------------------------------------------------------------------------------------------

void Logger::LogTex( const LogFlags logBits, const char *s )
{
	if ( !LogStarted() )
		return;

	// If the bits don't match the mask, then bail
	if ( !( logBits & LogMask() ) )
		return;

	LimitFileSize();
	mLogFile << HeaderString( logBits ) << s << std::endl;
}

// ---------------------------------------------------------------------------------------------------------------------------------

void Logger::LogRaw( const std::string &s )
{
	if ( !LogStarted() )
		return;

	LimitFileSize();
	mLogFile << s << std::endl;
}

// ---------------------------------------------------------------------------------------------------------------------------------

void Logger::LogHex( const char *buffer, const unsigned int count, const LogFlags logBits )
{
	if ( !LogStarted() )
		return;

	// No input? No output
	if ( !buffer )
		return;

	// If the bits don't match the mask, then bail
	if ( !( logBits & LogMask() ) )
		return;

	LimitFileSize();

	// Log the output
	unsigned int logged = 0;
	while ( logged < count )
	{
		// One line at a time...
		std::string		line;

		// The number of characters per line
		unsigned int	hexLength = 20;

		// Default the buffer
		unsigned int i;
		for ( i = 0; i < hexLength; i++ )
		{
			line += "-- ";
		}

		for ( i = 0; i < hexLength; i++ )
		{
			line += ".";
		}

		// Fill it in with real data
		for ( i = 0; i < hexLength && logged < count; i++, logged++ )
		{
			unsigned char	byte = buffer[ logged ];
			unsigned int	index = i * 3;

			// The hex characters
			const std::string	hexlist( "0123456789ABCDEF" );
			line[ index + 0 ] = hexlist[ byte >> 4 ];
			line[ index + 1 ] = hexlist[ byte & 0xf ];

			// The ascii characters
			if ( byte < 0x20 || byte > 0x7f ) byte = '.';
			line[ ( hexLength * 3 ) + i + 0 ] = byte;
		}

		mLogFile << HeaderString( logBits ) << line << std::endl;
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

void Logger::Indent( const std::string &s, const LogFlags logBits )
{
	if ( !LogStarted() )
		return;

	// If the bits don't match the mask, then bail
	if ( !( logBits & LogMask() ) )
		return;

	// Log the output
	if ( LineCharsFlag() )
	{
		mLogFile << HeaderString( logBits ) << "\xDA\xC4\xC4" << s << std::endl;
		mLogFile << HeaderString( logBits ) << "\xDA\xC4\xC4" << s << std::endl;
	}
	else
	{
		mLogFile << HeaderString( logBits ) << "+-  " << s << std::endl;
	}

	// Indent...
	mIndentCount += mIndentChars;
}

// ---------------------------------------------------------------------------------------------------------------------------------

void Logger::Undent( const std::string &s, const LogFlags logBits )
{
	if ( !LogStarted() )
		return;

	// If the bits don't match the mask, then bail
	if ( !( logBits & LogMask() ) )
		return;

	// Undo the indentation

	mIndentCount -= mIndentCount;
	if ( mIndentCount < 0 )
		mIndentCount = 0;

	// Log the output
	if ( LineCharsFlag() )
	{
		mLogFile << HeaderString( logBits ) << "\xC0\xC4\xC4" << s << std::endl;
	}
	else
	{
		mLogFile << HeaderString( logBits ) << "+-  " << s << std::endl;
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

const std::string &Logger::HeaderString( const LogFlags logBits ) const
{
	static std::string HeaderString;
	HeaderString.resize( 0 );

	// Get the std::string that represents the bits
	switch ( logBits )
	{
		case LOG_INDENT: HeaderString += "> "; break;
		case LOG_UNDENT: HeaderString += "< "; break;
		case LOG_ALL: HeaderString += "A "; break;
		case LOG_CRIT: HeaderString += "! "; break;
		case LOG_DATA: HeaderString += "D "; break;
		case LOG_ERR: HeaderString += "E "; break;
		case LOG_FLOW: HeaderString += "F "; break;
		case LOG_INFO: HeaderString += "I "; break;
		case LOG_WARN: HeaderString += "W "; break;
		default:          HeaderString += "  "; break;
	}

	// File std::string (strip out the path)
	char temp[ 1024 ] = { 0 };

	if ( mOutMask & WRITE_FILE )
	{
		int	ix = (int)SourceFile().rfind( '\\' );
		ix = ( ix == (int)std::string::npos ? 0 : ix + 1 );
		sprintf( temp, "%15s", SourceFile().substr( ix ).c_str() );
		HeaderString += temp;
	}
	if ( mOutMask & WRITE_LINE )
	{
		sprintf( temp, "[%04d]", SourceLine() );
		HeaderString += temp;
	}

	time_t	t = time( NULL );
	struct	tm *tme = localtime( &t );
	// Time std::string (specially formatted to save room)
	if ( mOutMask & WRITE_DATE )
	{
		sprintf( temp, "%02d/%02d ", tme->tm_mon + 1, tme->tm_mday );
		HeaderString += temp;
	}

	if ( mOutMask & WRITE_TIME )
	{
		sprintf( temp, "%02d:%02d:%02d ", tme->tm_hour, tme->tm_min, tme->tm_sec );
		HeaderString += temp;
	}

	// Spaces for indentation
	memset( temp, ' ', sizeof( temp ) );
	temp[ mIndentCount ] = '\0';

	// Add the indentation markers

	int	count = 0;
	while ( count < mIndentCount )
	{
		if ( LineCharsFlag() )
			temp[ count ] = '\xB3';
		else
			temp[ count ] = '|';
		count += mIndentChars;
	}
	HeaderString += temp;

	return HeaderString;
}
