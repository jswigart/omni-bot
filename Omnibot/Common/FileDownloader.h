////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: drevil $
// $LastChangedDate: 2008-09-16 17:19:47 -0700 (Tue, 16 Sep 2008) $
// $LastChangedRevision: 3432 $
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __FILEDOWNLOADER__
#define __FILEDOWNLOADER__

#ifdef ENABLE_FILE_DOWNLOADER

class FileDownloader
{
public:

	static bool Init();
	static void Shutdown();
	static void Poll();

	static bool NavigationMissing(const String &_name);

	static void UpdateWaypoints(const String &_mapname, bool _load = false);
	static void UpdateAllWaypoints(bool _getnew = false);

protected:
private:
};

#endif

#endif
