////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "FileSystem.h"

#include "IGame.h"
#include "IGameManager.h"
#include "Logger.h"
#include "physfs.h"

extern "C"
{
#include "7zCrc.h"
};

#if defined WIN32
#define PATHDELIMITER ";"
#elif defined __linux__ || ((defined __MACH__) && (defined __APPLE__))
#define PATHDELIMITER ":"
#endif

bool g_FileSystemInitialized = false;

namespace IOAssertions
{
	BOOST_STATIC_ASSERT(sizeof(float) == sizeof(obuint32));
}

struct FindInfo
{
	DirectoryList	&m_DirList;
	std::string			m_Expression;
	bool			m_Recursive;

	FindInfo(DirectoryList &_list, const std::string & _exp, bool recurse) :
		m_DirList(_list),
		m_Expression(_exp),
		m_Recursive(recurse)
	{
	}
};

void _FindAllCallback(void *data, const char *origdir, const char *filename)
{
	FindInfo *pInfo = static_cast<FindInfo*>(data);

	try
	{
		char fullname[512] = {};
		sprintf(fullname, "%s/%s", origdir, filename);

		if(PHYSFS_isDirectory(fullname))
		{
			if(pInfo->m_Recursive)
				PHYSFS_enumerateFilesCallback(filename, _FindAllCallback, pInfo);
			return;
		}

		if( Utils::RegexMatch( pInfo->m_Expression.c_str(),filename ) )
		{
			if(std::find(pInfo->m_DirList.begin(), pInfo->m_DirList.end(), fullname) == pInfo->m_DirList.end())
				pInfo->m_DirList.push_back(fullname);
		}
	}
	catch(const std::exception &)
	{
	}
}

void FileSystem::FindAllFiles(const std::string &_path, DirectoryList &_list, const std::string &_expression, bool recurse)
{
	FindInfo inf(_list, _expression, recurse);
	PHYSFS_enumerateFilesCallback(_path.c_str(), _FindAllCallback, &inf);
}

//////////////////////////////////////////////////////////////////////////

bool FileSystem::InitFileSystem()
{
	PHYSFS_Version compiled;
	PHYSFS_VERSION(&compiled);
	LOG("Initializing PhysFS: Version " <<
		(int)compiled.major << "." <<
		(int)compiled.minor << "." <<
		(int)compiled.patch);

	LOGFUNC;
	fs::path basePath = GetBaseFolder();
	LOG("Your base directory is: "<<basePath.string().c_str());
	if(!PHYSFS_init(basePath.string().c_str()))
	{
		OBASSERT(0, "PhysFS: Error Initializing: %s", PHYSFS_getLastError());
		return false;
	}

	PHYSFS_permitSymbolicLinks(1);

	//////////////////////////////////////////////////////////////////////////
	fs::path globalMapGoalPath = basePath / "global_scripts/mapgoals";
	PHYSFS_mount(globalMapGoalPath.string().c_str(), "scripts/mapgoals", 0);

	LOG("Your user directory is: "<<PHYSFS_getUserDir());
	fs::path modPath = GetModFolder();
	LOG("Your mod directory is: %s"<<modPath.string().c_str());
	if(!PHYSFS_mount(modPath.string().c_str(), NULL, 1))
	{
		LOGERR("Can't mount folder: " << modPath.string().c_str());
		OBASSERT(0, "PhysFS: Error Mounting Directory: %s", PHYSFS_getLastError());
		PHYSFS_deinit();
		return false;
	}

	fs::path globalGuiPath = basePath / "gui";
	PHYSFS_mount(globalGuiPath.string().c_str(), "gui", 0);

	fs::path globalScriptsPath = basePath / "global_scripts";
	PHYSFS_mount(globalScriptsPath.string().c_str(), "global_scripts", 0);

	fs::path configPath = basePath / "config";
	PHYSFS_mount(configPath.string().c_str(), "config", 0);

	LogAvailableArchives();

	CrcGenerateTable();
	g_FileSystemInitialized = true;
	return true;
}

//////////////////////////////////////////////////////////////////////////

bool FileSystem::InitRawFileSystem(const std::string &folder)
{
	PHYSFS_Version compiled;
	PHYSFS_VERSION(&compiled);
	LOG("Initializing PhysFS: Version " <<
		(int)compiled.major << "." <<
		(int)compiled.minor << "." <<
		(int)compiled.patch);

	LOGFUNC;
	fs::path basePath = fs::path(folder.c_str(),fs::native);
	LOG("Your base directory is: " << folder.c_str());
	if(!PHYSFS_init(basePath.string().c_str()))
	{
		OBASSERT(0, "PhysFS: Error Initializing: %s", PHYSFS_getLastError());
		return false;
	}

	PHYSFS_permitSymbolicLinks(1);

	//////////////////////////////////////////////////////////////////////////

	LogAvailableArchives();

	CrcGenerateTable();
	g_FileSystemInitialized = true;
	return true;
}

//////////////////////////////////////////////////////////////////////////

void FileSystem::LogAvailableArchives()
{
	const PHYSFS_ArchiveInfo **rc = PHYSFS_supportedArchiveTypes();
	LOG("Supported Archive Types");
	if (*rc == NULL)
	{
		LOG("None!");
	}
	else
	{
		for(const PHYSFS_ArchiveInfo **i = rc; *i != NULL; i++)
		{
			LOG(" * " << (*i)->extension << " : " << (*i)->description);
			LOG("Written by " << (*i)->author << " @ " << (*i)->url );
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void FileSystem::ShutdownFileSystem()
{
	if(PHYSFS_deinit())
	{
		LOG("PhysFS shut down successfully.");
	}
	else
	{
		OBASSERT(0, "Error Shutting Down PhysFS: %s", PHYSFS_getLastError());
		LOG("Error Shutting Down PhysFS: "<<PHYSFS_getLastError());
	}
}

//////////////////////////////////////////////////////////////////////////

bool FileSystem::IsInitialized()
{
	return g_FileSystemInitialized;
}

//////////////////////////////////////////////////////////////////////////

bool FileSystem::Mount(const fs::path &_path, const char *_mountpoint, MountOrder _order)
{
	if(!PHYSFS_mount(_path.string().c_str(), _mountpoint, _order==MountLast))
	{
		LOG("Error Mounting " << _path.string().c_str() << " : " << PHYSFS_getLastError());
		return false;
	}
	return true;
}

bool FileSystem::UnMount(const fs::path &_path)
{
	if(!PHYSFS_removeFromSearchPath(_path.string().c_str()))
	{
		LOG("Error UnMounting " << _path.string().c_str() << " : " << PHYSFS_getLastError());
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////

bool FileSystem::SetWriteDirectory(const fs::path &_dir)
{
	char buffer[ 1024 ] = {};
	const int len = wcstombs( buffer, _dir.native().c_str(), _dir.native().length() );

	if(len == 0 || !PHYSFS_setWriteDir( buffer ))
		//if(!PHYSFS_setWriteDir(_dir.native_file_string().c_str()))
	{
		LOG("PhysFS: Error Setting Write Directory: " << PHYSFS_getLastError());
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////

bool FileSystem::MakeDirectory(const char *_folder)
{
	if(!PHYSFS_mkdir(_folder))
	{
		LOG("Error Creating Directory " << _folder << " : " << PHYSFS_getLastError());
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////

bool FileSystem::FileExists(const filePath &_file)
{
	return PHYSFS_exists(_file) != 0;
}

//////////////////////////////////////////////////////////////////////////

obint64 FileSystem::FileModifiedTime(const filePath &_file)
{
	return PHYSFS_getLastModTime(_file);
}

//////////////////////////////////////////////////////////////////////////

bool FileSystem::FileDelete(const filePath &_file)
{
	return PHYSFS_delete(_file) != 0;
}

//////////////////////////////////////////////////////////////////////////

fs::path FileSystem::GetRealDir(const std::string &_file)
{
	const char *pPath = PHYSFS_getRealDir(_file.c_str());

	try
	{
		return fs::path(pPath ? pPath : "", fs::native);
	}
	catch(const std::exception & ex)
	{
		SOFTASSERTALWAYS(0, "Filesystem: %s", ex.what());
	}
	return fs::path("", fs::native);
}

//////////////////////////////////////////////////////////////////////////

fs::path FileSystem::GetRealPath(const std::string &_file)
{
	try
	{
		fs::path filepath(_file, fs::native);
		return GetRealDir(_file) / filepath.leaf();
	}
	catch(const std::exception & ex)
	{
		SOFTASSERTALWAYS(0, "Filesystem: %s", ex.what());
	}
	return fs::path("", fs::native);
}

//////////////////////////////////////////////////////////////////////////
obuint32 FileSystem::CalculateCrc(const void *_data, obuint32 _size)
{
	obuint32 crc = CRC_INIT_VAL;
	crc = CrcUpdate(crc, _data, (size_t)_size);
	crc = CRC_GET_DIGEST(crc);
	return crc;
}

obuint32 FileSystem::GetFileCrc(const std::string &_file)
{
	obuint32 crc = 0;

	File f;
	if(f.OpenForRead(_file.c_str(), File::Binary) && f.IsOpen())
	{
		const int iBufferSize = 4096;
		char buffer[iBufferSize] = {};

		crc = CRC_INIT_VAL;

		obint64 rd = 0;
		while((rd = f.Read(buffer, 1, iBufferSize)) > 0)
		{
			crc = CrcUpdate(crc, buffer, (size_t)rd);
		}
		crc = CRC_GET_DIGEST(crc);

		f.Close();
	}

	return crc;
}

//////////////////////////////////////////////////////////////////////////

bool _SupportsArchiveType(const std::string &_extension)
{
	const PHYSFS_ArchiveInfo **rc = PHYSFS_supportedArchiveTypes();
	if (*rc)
	{
		std::string strExt;
		for(const PHYSFS_ArchiveInfo **i = rc; *i != NULL; i++)
		{
			strExt = std::string(".") + (*i)->extension;
			if(!Utils::StringCompareNoCase(_extension, strExt))
				return true;
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
struct MntFile
{
	fs::path	FilePath;
	std::string		OrigDir;
};
typedef std::vector<MntFile> MountFiles;
//////////////////////////////////////////////////////////////////////////

void _MountAllCallback(void *data, const char *origdir, const char *str)
{
	try
	{
		MountFiles *FileList = (MountFiles*)data;

		char fullname[512] = {};
		sprintf(fullname, "%s/%s", origdir, str);
		const char *pDir = PHYSFS_getRealDir(fullname);
		if(pDir)
		{
			fs::path filepath(pDir, fs::native);
			filepath /= origdir;
			filepath /= str;

			if(!fs::is_directory(filepath) &&
				_SupportsArchiveType(fs::extension(filepath)))
			{
				MntFile fl;
				fl.FilePath = filepath;
				fl.OrigDir = origdir;
				FileList->push_back(fl);
			}
		}
	}
	catch(const std::exception & ex)
	{
		//ex;
		SOFTASSERTALWAYS(0, "Filesystem: %s", ex.what());
	}
}

bool _FileNameGT(const MntFile &_pt1, const MntFile &_pt2)
{
	return _pt1.FilePath.string() > _pt2.FilePath.string();
}

void FileSystem::MountArchives(const char *_folder, const char *_mountpoint)
{
	try
	{
		MountFiles files;
		PHYSFS_enumerateFilesCallback(_folder, _MountAllCallback, &files);
		std::sort(files.begin(), files.end(), _FileNameGT);

		MountFiles::const_iterator cIt = files.begin();
		for(; cIt != files.end(); ++cIt)
		{
			if(PHYSFS_mount(cIt->FilePath.string().c_str(), _mountpoint?_mountpoint:cIt->OrigDir.c_str(), 1))
			{
				LOG("Mounted: " << cIt->FilePath.string().c_str() << " to " << cIt->OrigDir.c_str());
			}
			else
			{
				const char *pError = PHYSFS_getLastError();
				SOFTASSERTALWAYS(0, "PhysFS: %s", pError ? pError : "Unknown Error");
			}
		}
	}
	catch(const std::exception & ex)
	{
		//ex;
		SOFTASSERTALWAYS(0, "Filesystem: %s", ex.what());
	}
}

//////////////////////////////////////////////////////////////////////////

void EchoFileCallback(void *data, const char *origdir, const char *filename)
{
	if(PHYSFS_isDirectory(filename))
		PHYSFS_enumerateFilesCallback(filename, EchoFileCallback, 0);
	else
	{
		char fullname[512] = {};
		sprintf(fullname, "%s/%s", origdir, filename);
		const char *pDir = PHYSFS_getRealDir(fullname);

		EngineFuncs::ConsoleMessage(va("%s/%s : %s", origdir, filename, pDir?pDir:"<->"));
		Utils::OutputDebug(kInfo,"%s/%s : %s", origdir, filename, pDir?pDir:"<->");
	}
}

void FileSystem::EnumerateFiles(const char *_folder)
{
	Utils::OutputDebug(kNormal, "--------------------\n");
	PHYSFS_enumerateFilesCallback(_folder, EchoFileCallback, 0);
	Utils::OutputDebug(kNormal, "--------------------\n");
}

fs::path FileSystem::GetBaseFolder()
{
	fs::path basePath;

	// First try to get a path from the game.
	const char *pPathOverride = g_EngineFuncs->GetBotPath();
	try
	{
		fs::path pathOverride(pPathOverride, fs::native);
		if(fs::exists(pathOverride) && !fs::is_directory(pathOverride))
		{
			basePath = fs::path(pPathOverride, fs::native);
			basePath = basePath.branch_path();
		}

		if(basePath.empty())
		{
			basePath = FindFile(pathOverride.leaf());
			basePath = basePath.branch_path();
		}
	}
	catch(const std::exception & ex)
	{
		LOG("Bad Override Path: " << ex.what());
	}
	return basePath;
}
fs::path FileSystem::FindFile(const fs::path &_file)
{
	try
	{
		// Look for JUST the file in the current folder first.
		if(fs::exists(_file.leaf()))
			return _file.leaf();

		// Look for the file using the full provided path, if it differs from just the filename
		if((_file.string() != _file.leaf()) && fs::exists(_file))
			return _file;

		// Look in the system path for the file.
		StringVector pathList;
		const char* pPathVariable = getenv("OMNIBOTFOLDER");
		if(pPathVariable)
			Utils::Tokenize(pPathVariable, PATHDELIMITER, pathList);
		pPathVariable = getenv("PATH");
		if(pPathVariable)
			Utils::Tokenize(pPathVariable, PATHDELIMITER, pathList);
		StringVector::const_iterator it = pathList.begin();
		for( ; it != pathList.end(); ++it)
		{
			try
			{
				// search for the just the file or the whole path
				fs::path checkPath = fs::path(*it, fs::native) / fs::path(_file.leaf());
				if(fs::exists(checkPath) && !fs::is_directory(checkPath))
					return checkPath;

				if (_file.string() != _file.leaf())
				{
					checkPath = fs::path(*it, fs::native) / fs::path(_file);
					if(fs::exists(checkPath) && !fs::is_directory(checkPath))
						return checkPath;
				}
			}
			catch(const std::exception & ex)
			{
				const char *pErr = ex.what();
				LOG("Filesystem Exception: " << pErr);
			}
		}
	}
	catch(const std::exception & ex)
	{
		const char *pErr = ex.what();
		LOG("Filesystem Exception: " << pErr);
	}

	// Not found, give back an empty path.
	return fs::path();
}

fs::path FileSystem::GetModFolder()
{
	fs::path navFolder = GetBaseFolder();

	if( System::mInstance->mGame )
	{
		// Append the script subfolder
		navFolder /= fs::path(System::mInstance->mGame->GetModSubFolder(), fs::native);
		return navFolder;
	}

	return fs::path();
}

fs::path FileSystem::GetNavFolder()
{
	fs::path navFolder = GetBaseFolder();

	if( System::mInstance->mGame )
	{
		// Append the script subfolder
		navFolder /= fs::path(System::mInstance->mGame->GetNavSubfolder(), fs::native);
		return navFolder;
	}

	return fs::path();
}

fs::path FileSystem::GetScriptFolder()
{
	fs::path scriptFolder = GetBaseFolder();

	if(System::mInstance->mGame)
	{
		// Append the script subfolder
		scriptFolder /= fs::path(System::mInstance->mGame->GetScriptSubfolder(), fs::native);
		return scriptFolder;
	}

	return fs::path();
}

//////////////////////////////////////////////////////////////////////////
class File_Private
{
public:
	File_Private() : m_pPrivate(0) {}
	PHYSFS_File *m_pPrivate;
};
//////////////////////////////////////////////////////////////////////////

File::File() :
	m_pFile(new File_Private)
{
	m_TextMode = false;
}

File::~File()
{
	Close();
	OB_DELETE(m_pFile->m_pPrivate);
	OB_DELETE(m_pFile);
}

bool File::OpenForWrite(const char *_name, FileMode _mode, bool _append /*= false*/)
{
	m_pFile->m_pPrivate = _append ? PHYSFS_openAppend(_name) : PHYSFS_openWrite(_name);
	m_TextMode = _mode == Text;
	return m_pFile->m_pPrivate != NULL;
}

bool File::OpenForRead(const char *_name, FileMode _mode)
{
	m_pFile->m_pPrivate = PHYSFS_openRead(_name);
	m_TextMode = _mode == Text;
	return m_pFile->m_pPrivate != NULL;
}

void File::Close()
{
	if(m_pFile->m_pPrivate)
	{
		PHYSFS_close(m_pFile->m_pPrivate);
		m_pFile->m_pPrivate = 0;
	}
}

bool File::IsOpen()
{
	return m_pFile->m_pPrivate != NULL;
}

bool File::WriteInt8(obuint8 i)
{
	if(m_pFile->m_pPrivate)
	{
		if(m_TextMode)
		{
			std::stringstream str;
			str << i;
			std::string st;
			str >> st;
			st += " ";
			return WriteString(st);
		}
		else
		{
			return Write(&i, sizeof(i), 1) != 0;
		}
	}
	return false;
}

bool File::WriteInt16(obuint16 i)
{
	if(m_pFile->m_pPrivate)
	{
		if(m_TextMode)
		{
			std::stringstream str;
			str << i;
			std::string st;
			str >> st;
			st += " ";
			return WriteString(st);
		}
		else
		{
			return PHYSFS_writeULE16(m_pFile->m_pPrivate, i) != 0;
		}
	}
	return false;
}

bool File::WriteInt32(obuint32 i, bool spaceatend)
{
	if(m_pFile->m_pPrivate)
	{
		if(m_TextMode)
		{
			std::stringstream str;
			str << i;
			std::string st;
			str >> st;
			if(spaceatend)
				st += " ";
			return WriteString(st);
		}
		else
		{
			return PHYSFS_writeULE32(m_pFile->m_pPrivate, i) != 0;
		}
	}
	return false;
}

bool File::WriteInt64(obuint64 i, bool spaceatend)
{
	if(m_pFile->m_pPrivate)
	{
		if(m_TextMode)
		{
			std::stringstream str;
			str << i;
			std::string st;
			str >> st;
			if(spaceatend)
				st += " ";
			return WriteString(st);
		}
		else
		{
			return PHYSFS_writeULE64(m_pFile->m_pPrivate, i) != 0;
		}
	}
	return false;
}

bool File::WriteFloat(float f)
{
	if(m_pFile->m_pPrivate)
	{
		if(m_TextMode)
		{
			std::stringstream str;
			str << f;
			std::string st;
			str >> st;
			st += " ";
			return WriteString(st);
		}
		else
		{
			obuint32 tmp;
			memcpy(&tmp, &f, sizeof(tmp));
			return WriteInt32(tmp);
		}
	}
	return false;
}

obuint64 File::Write(const void *_buffer, obuint32 _size, obuint32 _numitems /*= 1*/)
{
	return m_pFile->m_pPrivate && _size != 0 ? PHYSFS_write(m_pFile->m_pPrivate, _buffer, _size, _numitems) : 0;
}

bool File::WriteString(const std::string &_str)
{
	if(m_pFile->m_pPrivate)
	{
		if(m_TextMode)
		{
			obuint32 len = (obuint32)_str.length();
			if(!Write(_str.c_str(), len, 1)) return false;
		}
		else
		{
			obuint32 len = (obuint32)_str.length();
			if(!WriteInt32(len)) return false;
			if(len > 0)
			{
				if(!Write(_str.c_str(), len, 1)) return false;
			}
		}
		return true;
	}
	return false;
}

void File::Printf(const char* _msg, ...)
{
	static char buffer[8192] = {0};
	va_list list;
	va_start(list, _msg);
#ifdef WIN32
	_vsnprintf(buffer, 8192, _msg, list);
#else
	vsnprintf(buffer, 8192, _msg, list);
#endif
	va_end(list);

	WriteString(buffer);
}

bool File::WriteNewLine()
{
	const obuint8 cr = 0x0D;
	const obuint8 lf = 0x0A;
	return Write(&cr, sizeof(obuint8)) && Write(&lf, sizeof(obuint8));
}

bool File::ReadInt8(obuint8 &i)
{
	if(m_pFile->m_pPrivate)
	{
		if(m_TextMode)
		{
			std::string st;
			return ReadString(st) && Utils::ConvertString<obuint8>(st, i);
		}
		else
		{
			return Read(&i, sizeof(i), 1) != 0;
		}
	}
	return false;
}

bool File::ReadInt16(obuint16 &i)
{
	if(m_pFile->m_pPrivate)
	{
		if(m_TextMode)
		{
			std::string st;
			return ReadString(st) && Utils::ConvertString<obuint16>(st, i);
		}
		else if(PHYSFS_readULE16(m_pFile->m_pPrivate, &i))
		{
			i = PHYSFS_swapSLE16(i);
			return true;
		}
	}
	return false;
}

bool File::ReadInt32(obuint32 &i)
{
	if(m_pFile->m_pPrivate)
	{
		if(m_TextMode)
		{
			std::string st;
			return ReadString(st) && Utils::ConvertString<obuint32>(st, i);
		}
		else if(PHYSFS_readULE32(m_pFile->m_pPrivate, &i))
		{
			i = PHYSFS_swapSLE32(i);
			return true;
		}
	}
	return false;
}

bool File::ReadInt64(obuint64 &i)
{
	if(m_pFile->m_pPrivate)
	{
		if(m_TextMode)
		{
			std::string st;
			return ReadString(st) && Utils::ConvertString<obuint64>(st, i);
		}
		else if(PHYSFS_readULE64(m_pFile->m_pPrivate, &i))
		{
			i = PHYSFS_swapSLE64(i);
			return true;
		}
	}
	return false;
}

bool File::ReadFloat(float &f)
{
	if(m_pFile->m_pPrivate)
	{
		obuint32 tmp;
		if(m_TextMode)
		{
			std::string st;
			return ReadString(st) && Utils::ConvertString<float>(st, f);
		}
		else if(PHYSFS_readULE32(m_pFile->m_pPrivate, &tmp))
		{
			tmp = PHYSFS_swapSLE32(tmp);
			memcpy(&f, &tmp, sizeof(f));
			return true;
		}
	}
	return false;
}

obint64 File::Read(void *_buffer, obuint32 _size, obuint32 _numitems /*= 1*/)
{
	return m_pFile->m_pPrivate && _size != 0 ? PHYSFS_read(m_pFile->m_pPrivate, _buffer, _size, _numitems) : 0;
}

obuint64 File::ReadWholeFile(std::string &_readto)
{
	enum { BufferSize = 4096 };
	char buffer[BufferSize] = {};

	obint64  readBytes = 0, totalBytes = 0;
	while((readBytes = Read(buffer,1,BufferSize)) > 0)
	{
		_readto.append(buffer,(unsigned int)readBytes);
		totalBytes += readBytes;
	}
	return totalBytes;
}

bool File::ReadString(std::string &_str)
{
	if(m_pFile->m_pPrivate)
	{
		_str.clear();
		if(m_TextMode)
		{
			char ch;
			while(Read(&ch, sizeof(ch), 1)>0 && !EndOfFile() && !Utils::IsWhiteSpace(ch))
				_str.push_back(ch);

			// eat white space.
			while(Read(&ch, sizeof(ch), 1)>0 && !EndOfFile() && Utils::IsWhiteSpace(ch)) { }
			// go back by 1
			Seek(Tell()-1);
		}
		else
		{
			obuint32 len;
			if(!ReadInt32(len)) return false;
			if(len > 0)
			{
				boost::shared_array<char> pBuffer(new char[len+1]);
				if(!Read(pBuffer.get(), len, 1)) return false;
				pBuffer.get()[len] = 0;
				_str = pBuffer.get();
			}
		}
		return true;
	}
	return false;
}

bool File::ReadLine(std::string &_str)
{
	_str.resize(0);
	if(m_pFile->m_pPrivate)
	{
		OBASSERT(m_TextMode, "Function Only for Text Mode");
		if(m_TextMode)
		{
			const obuint8 cr = 0x0D;
			const obuint8 lf = 0x0A;
			lf;

			if(EndOfFile())
				return false;

			char ch;
			while(Read(&ch, sizeof(ch), 1)>0 && !EndOfFile() && ch != cr && ch != '\n')
				_str.push_back(ch);

			// eat white space.
			while(Read(&ch, sizeof(ch), 1)>0 && !EndOfFile() && Utils::IsWhiteSpace(ch)) { }

			// go back by 1
			Seek(Tell()-1);
		}
	}
	return !_str.empty();
}

bool File::Seek(obuint64 _pos)
{
	return m_pFile->m_pPrivate ? PHYSFS_seek(m_pFile->m_pPrivate, _pos) != 0 : 0;
}

obint64 File::Tell()
{
	return m_pFile->m_pPrivate ? PHYSFS_tell(m_pFile->m_pPrivate) : -1;
}

bool File::EndOfFile()
{
	return m_pFile->m_pPrivate ? (PHYSFS_eof(m_pFile->m_pPrivate)!=0) : true;
}

bool File::SetBuffer(obuint64 _size)
{
	return m_pFile->m_pPrivate ? (PHYSFS_setBuffer(m_pFile->m_pPrivate, _size)!=0) : false;
}

bool File::Flush()
{
	return m_pFile->m_pPrivate ? (PHYSFS_flush(m_pFile->m_pPrivate)!=0) : false;
}

std::string File::GetLastError()
{
	const char *pError = PHYSFS_getLastError();
	return pError ? pError : "Unknown";
}

obint64 File::FileLength()
{
	return m_pFile->m_pPrivate ? PHYSFS_fileLength(m_pFile->m_pPrivate) : -1;
}