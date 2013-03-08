////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __FILEDOWNLOADER__
#define __FILEDOWNLOADER__

#ifdef ENABLE_FILE_DOWNLOADER

class FileDownloader
{
public:

	static bool Init();
	static void Shutdown();
	static void Poll();

	static bool NavigationMissing(const std::string &_name);

	static void UpdateWaypoints(const std::string &_mapname, bool _load = false);
	static void UpdateAllWaypoints(bool _getnew = false);

protected:
private:
};

#endif

#endif
