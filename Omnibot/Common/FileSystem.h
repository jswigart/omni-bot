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

#ifdef _WIN32
#pragma warning( push )
// stfu boost
#pragma warning( disable: 4244 )
#pragma warning( disable: 4265 )
#pragma warning( disable: 6334 )
#pragma warning( disable: 6011 )
#pragma warning( disable: 4913 )
#endif //_WIN32

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#ifdef _WIN32
#pragma warning( pop )
#endif // _WIN32


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
	static int64_t FileModifiedTime(const filePath &_file);

	enum MountOrder { MountFirst,MountLast };
	static bool Mount(const fs::path &_path, const char *_mountpoint = 0, MountOrder _order = MountFirst);
	static bool UnMount(const fs::path &_path);

	static uint32_t GetFileCrc(const std::string &_file);
	static uint32_t CalculateCrc(const void *_data, uint32_t _size);

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

	FileMode GetFileMode() const { return mTextMode ? Text : Binary; }

	bool OpenForWrite(const char *_name, FileMode _mode, bool _append = false);
	bool OpenForRead(const char *_name, FileMode _mode);
	void Close();
	bool IsOpen();

	bool WriteInt8(uint8_t i);
	bool WriteInt16(uint16_t i);
	bool WriteInt32(uint32_t i, bool spaceatend = true);
	bool WriteInt64(uint64_t i, bool spaceatend = true);
	bool WriteFloat(float f);
	uint64_t Write(const void *_buffer, uint32_t _size, uint32_t _numitems = 1);
	bool WriteString(const std::string &_str);
	void Printf(CHECK_PRINTF_ARGS const char* _msg, ...);
	bool WriteNewLine();

	bool ReadInt8(uint8_t &i);
	bool ReadInt16(uint16_t &i);
	bool ReadInt32(uint32_t &i);
	bool ReadInt64(uint64_t &i);
	bool ReadFloat(float &f);
	int64_t Read(void *_buffer, uint32_t _size, uint32_t _numitems = 1);
	bool ReadString(std::string &_str);
	bool ReadLine(std::string &_str);

	uint64_t ReadWholeFile(std::string &_readto);

	bool Seek(uint64_t _pos);
	int64_t Tell();
	bool EndOfFile();
	bool SetBuffer(uint64_t _size);
	bool Flush();
	int64_t FileLength();

	std::string GetLastError();

	File();
	~File();
private:
	File_Private * mpFile;

	bool		mTextMode : 1;
};

#endif
