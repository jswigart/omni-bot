////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include "Utilities.h"

namespace fs = boost::filesystem;

typedef std::vector<fs::path> DirectoryList;

// class: FileSystem
//		Wraps filesystem functionality including searching archives and search paths for files.
class FileSystem
{
public:

	// function: FindAllFiles
	//		Searches for files matching an extension in the list of search paths, returns the path if found.
	static void FindAllFiles(const std::string &_path, DirectoryList &_list, const std::string &_expression = ".*", bool recurse = false);
	//////////////////////////////////////////////////////////////////////////

	static bool InitFileSystem();
	static bool InitRawFileSystem(const std::string &folder);
	static void ShutdownFileSystem();
	static bool SetWriteDirectory(const fs::path &_dir);

	static bool IsInitialized();

	static void EnumerateFiles(const char *_folder);

	static void MountArchives(const char *_folder, const char *_mountpoint = 0);

	static fs::path GetRealDir(const std::string &_file);
	static fs::path GetRealPath(const std::string &_file);

	static bool MakeDirectory(const char *_folder);
	static bool FileDelete(const filePath &_file);
	static bool FileExists(const filePath &_file);
	static obint64 FileModifiedTime(const filePath &_file);

	enum MountOrder { MountFirst,MountLast };
	static bool Mount(const fs::path &_path, const char *_mountpoint = 0, MountOrder _order = MountFirst);
	static bool UnMount(const fs::path &_path);

	static obuint32 GetFileCrc(const std::string &_file);
	static obuint32 CalculateCrc(const void *_data, obuint32 _size);

	static fs::path FindFile(const fs::path &_file);
	static fs::path GetModFolder();
	static fs::path GetNavFolder();
	static fs::path GetScriptFolder();
private:
	FileSystem() {}

	static fs::path GetBaseFolder();

	static void LogAvailableArchives();
};

class File_Private;

class File
{
public:
	enum FileMode
	{
		Binary,
		Text
	};

	FileMode GetFileMode() const { return m_TextMode ? Text : Binary; }

	bool OpenForWrite(const char *_name, FileMode _mode, bool _append = false);
	bool OpenForRead(const char *_name, FileMode _mode);
	void Close();
	bool IsOpen();

	bool WriteInt8(obuint8 i);
	bool WriteInt16(obuint16 i);
	bool WriteInt32(obuint32 i, bool spaceatend = true);
	bool WriteInt64(obuint64 i, bool spaceatend = true);
	bool WriteFloat(float f);
	obuint64 Write(const void *_buffer, obuint32 _size, obuint32 _numitems = 1);
	bool WriteString(const std::string &_str);
	void Printf(CHECK_PRINTF_ARGS const char* _msg, ...);
	bool WriteNewLine();

	bool ReadInt8(obuint8 &i);
	bool ReadInt16(obuint16 &i);
	bool ReadInt32(obuint32 &i);
	bool ReadInt64(obuint64 &i);
	bool ReadFloat(float &f);
	obint64 Read(void *_buffer, obuint32 _size, obuint32 _numitems = 1);
	bool ReadString(std::string &_str);
	bool ReadLine(std::string &_str);

	obuint64 ReadWholeFile(std::string &_readto);

	bool Seek(obuint64 _pos);
	obint64 Tell();
	bool EndOfFile();
	bool SetBuffer(obuint64 _size);
	bool Flush();
	obint64 FileLength();

	std::string GetLastError();

	File();
	~File();
private:
	File_Private *m_pFile;

	bool		m_TextMode : 1;
};

#endif
