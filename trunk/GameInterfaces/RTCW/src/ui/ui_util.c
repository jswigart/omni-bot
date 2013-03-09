// ui_util.c
//
// origin: rad
// new ui support stuff
//
// memory, string alloc

//S4NDM4NN - putting http download code and functions here to keep it seperate from everything else
#ifdef _WIN32

#include "../downlib/downlib.h"
#include <process.h>
#include "ui_local.h"
downloader_t httpdown;

void Text_Paint( float x, float y, float scale, vec4_t color, const char *text, float adjust, int limit, int style );
void UI_ReadableSize( char *buf, int bufsize, int value );
void UI_PrintTime( char *buf, int bufsize, int time );
void Text_PaintCenter( float x, float y, float scale, vec4_t color, const char *text, float adjust );

char *get_http_info( char *host, char *filename, int *filesize, mydown_options *opt ) {
	int old_showhead,
		fsize;

	old_showhead = opt->showhead;
	if ( !filename ) {
		opt->filedata = (void *)&filename;
	}
	opt->showhead = 2;
	fsize = mydown( host, filename, opt );
	if ( fsize == MYDOWN_ERROR ) {
		return NULL;
	}
	opt->showhead = old_showhead;
	opt->filedata = NULL;

	trap_Print( va(
		"^7- ^3file name   ^1%s\n"
		"^7- ^3file size   ^1%u\n"
		"\n", filename, fsize ) );

	*filesize = fsize;
	return( filename );
}

//Basically just a systemcall to the downloads complete function in the engine
typedef void ( *g_DownloadsComplete )( void );
void trap_DownloadsComplete( void ) {
	g_DownloadsComplete DownloadsComplete;
	if ( !Q_stricmp( httpdown.version, "Wolf 1.4-MP win-x86 Oct 28 2002" ) || !Q_stricmp( httpdown.version, "Wolf 1.41-MP win-x86 Dec 4 2002" ) ) {
		DownloadsComplete = (g_DownloadsComplete)0x0040C580;
	} else {
		DownloadsComplete = (g_DownloadsComplete)0x0040F660;
	}
	(DownloadsComplete)( );
}

//this will be the actual thread
void downloadThread( void*dummy ) {
	httpdown.inUse = 1;
	mydown( httpdown.host, httpdown.filename, &httpdown.opt );
	httpdown.inUse = 0;

	trap_DownloadsComplete();
	trap_Cmd_ExecuteText( EXEC_APPEND, "wait 25;reconnect\n" ); //after testing seems this is needed to make clients reload map on a map switch,the wait time isnt that important
	_endthread();
}

//Hook to kick off the http downloader
void ( WINAPIV *orig_BeginDownload )( const char *localName, const char *remoteName );
void hook_BeginDownload( const char *localName, const char *remoteName ) {
	char*host;
	char info[1024];

	if ( !ui_useHttpDown.integer ) { //pass back to the default function and return
		orig_BeginDownload( localName,remoteName );
		return;
	}

	if ( httpdown.inUse ) {
		return;
	}

	trap_GetConfigString( CS_SERVERINFO, info, sizeof( info ) );
	host = Info_ValueForKey( info, "g_httpdownhost" );

	//if they dont have a host set lets atleast try something
	if ( !strlen( host ) ) {
		host = "s4ndmod.com/downloads/";
	}

	httpdown.opt.useragent = va( "%s Omni-bot",Q3_VERSION );
	trap_Cvar_Set( "cl_downloadName", remoteName );
	trap_Cvar_Set( "cl_downloadTime",va( "%i",uiInfo.uiDC.realTime ) );
	httpdown.filename = va( "%s",localName );

	httpdown.host = va( "http://%s%s",host,remoteName );
	strcpy( httpdown.display,httpdown.host );

	httpdown.filename = get_http_info( httpdown.host, httpdown.filename, &httpdown.filesize, &httpdown.opt ); //at some point should be in its own thread heh
	httpdown.useHttp = 0;

	if ( !httpdown.filename ) { //do the original download!
		httpdown.inUse = 0; //the game can track if its in use here...
		orig_BeginDownload( localName,remoteName );
		return;
	}

	httpdown.useHttp = 1;
	_beginthread( downloadThread, 0, NULL );
}

//S4NDM4NN - Display for the http downloader
#define ESTIMATES 80
void UI_DisplayMyDownloadInfo( const char *downloadName, float centerPoint, float yStart, float scale ) {
	static char dlText[]    = "Downloading:";
	static char etaText[]   = "Estimated time left:";
	static char xferText[]  = "Transfer rate:";
	static int tleEstimates[ESTIMATES] = { 60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,
		60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,
		60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,
		60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60 };
	static int tleIndex = 0;
	vec4_t color = { 0.3f, 0.3f, 0.3f, 0.8f };
	int downloadSize, downloadCount, downloadTime;
	char dlSizeBuf[64], totalSizeBuf[64], xferRateBuf[64], dlTimeBuf[64];
	int xferRate;
	const char *s;

	downloadSize = httpdown.filesize;
	downloadCount = httpdown.byteDownloaded;
	downloadTime = trap_Cvar_VariableValue( "cl_downloadTime" );

	// Background

	UI_FillRect( 0, yStart + 170, 640, 98, color );

	UI_SetColor( colorYellow );
	//we may aswell let them know they are using http download
	Text_PaintCenter( centerPoint, yStart + 185, scale * .5, colorYellow,va( "Downloading: %s",httpdown.display ), 0 );
	Text_Paint( 92, yStart + 210, scale, colorYellow, dlText, 0, 64, ITEM_TEXTSTYLE_SHADOWEDMORE );
	Text_Paint( 20, yStart + 235, scale, colorYellow, etaText, 0, 64, ITEM_TEXTSTYLE_SHADOWEDMORE );
	Text_Paint( 84, yStart + 260, scale, colorYellow, xferText, 0, 64, ITEM_TEXTSTYLE_SHADOWEDMORE );

	if ( downloadSize > 0 ) {
		s = va( "%s (%d%%)", downloadName, downloadCount * 100 / downloadSize );
	} else {
		s = downloadName;
	}

	Text_Paint( 260, yStart + 210, scale, colorYellow, s, 0, 0, ITEM_TEXTSTYLE_SHADOWEDMORE );

	UI_ReadableSize( dlSizeBuf,     sizeof dlSizeBuf,       downloadCount );
	UI_ReadableSize( totalSizeBuf,  sizeof totalSizeBuf,    downloadSize );

	if ( downloadCount < 4096 || !downloadTime ) {
		Text_PaintCenter( centerPoint, yStart + 235, scale, colorYellow, "estimating", 0 );
		Text_PaintCenter( centerPoint, yStart + 340, scale, colorYellow, va( "(%s of %s copied)", dlSizeBuf, totalSizeBuf ), 0 );
	} else {
		if ( ( uiInfo.uiDC.realTime - downloadTime ) / 1000 ) {
			xferRate = downloadCount / ( ( uiInfo.uiDC.realTime - downloadTime ) / 1000 );
		} else {
			xferRate = 0;
		}
		UI_ReadableSize( xferRateBuf, sizeof xferRateBuf, xferRate );

		// Extrapolate estimated completion time
		if ( downloadSize && xferRate ) {
			int n = downloadSize / xferRate; // estimated time for entire d/l in secs
			int timeleft = 0, i;

			// We do it in K (/1024) because we'd overflow around 4MB
			tleEstimates[ tleIndex ] = ( n - ( ( ( downloadCount / 1024 ) * n ) / ( downloadSize / 1024 ) ) );
			tleIndex++;
			if ( tleIndex >= ESTIMATES ) {
				tleIndex = 0;
			}

			for ( i = 0; i < ESTIMATES; i++ )
				timeleft += tleEstimates[ i ];

			timeleft /= ESTIMATES;

			UI_PrintTime( dlTimeBuf, sizeof dlTimeBuf, timeleft );

			Text_Paint( 260, yStart + 235, scale, colorYellow, dlTimeBuf, 0, 0, ITEM_TEXTSTYLE_SHADOWEDMORE );
			Text_PaintCenter( centerPoint, yStart + 340, scale, colorYellow, va( "(%s of %s copied)", dlSizeBuf, totalSizeBuf ), 0 );
		} else {
			Text_PaintCenter( centerPoint, yStart + 235, scale, colorYellow, "estimating", 0 );
			if ( downloadSize ) {
				Text_PaintCenter( centerPoint, yStart + 340, scale, colorYellow, va( "(%s of %s copied)", dlSizeBuf, totalSizeBuf ), 0 );
			} else {
				Text_PaintCenter( centerPoint, yStart + 340, scale, colorYellow, va( "(%s copied)", dlSizeBuf ), 0 );
			}
		}

		if ( xferRate ) {
			Text_Paint( 260, yStart + 260, scale, colorYellow, va( "%s/Sec", xferRateBuf ), 0, 0, ITEM_TEXTSTYLE_SHADOWEDMORE );
		}
	}
}
#endif