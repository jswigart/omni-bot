// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5CorePCH.h"
#include "Wm5Environment.h"
#include "Wm5Memory.h"
using namespace Wm5;

WM5_IMPLEMENT_INITIALIZE(Environment);
WM5_IMPLEMENT_TERMINATE(Environment);

std::vector<std::string>* Environment::msDirectories = 0;

//----------------------------------------------------------------------------
// Initialization/termination support.
//----------------------------------------------------------------------------
void Environment::Initialize ()
{
    if (!msDirectories)
    {
        msDirectories = new0 std::vector<std::string>();
        return;
    }

    assertion(false, "Directory list already allocated.\n");
}
//----------------------------------------------------------------------------
void Environment::Terminate ()
{
    if (msDirectories)
    {
        delete0(msDirectories);
        return;
    }

    assertion(false, "No directory list to deallocate.\n");
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
std::string Environment::GetVariable (const std::string& varName)
{
    const char* variable = getenv(varName.c_str());
    return variable ? std::string(variable) : std::string("");
}
//----------------------------------------------------------------------------
int Environment::GetNumDirectories ()
{
    if (!msDirectories)
    {
        Initialize();
    }

    return (int)msDirectories->size();
}
//----------------------------------------------------------------------------
std::string Environment::GetDirectory (int i)
{
    if (!msDirectories)
    {
        Initialize();
    }

    if (0 <= i && i < (int)msDirectories->size())
    {
        return (*msDirectories)[i];
    }

    return std::string("");
}
//----------------------------------------------------------------------------
bool Environment::InsertDirectory (const std::string& directory)
{
    if (!msDirectories)
    {
        Initialize();
    }

    std::vector<std::string>::iterator iter = msDirectories->begin();
    std::vector<std::string>::iterator end = msDirectories->end();
    for (/**/; iter != end; ++iter)
    {
        if (directory == *iter)
        {
            return false;
        }
    }
    msDirectories->push_back(directory);
    return true;
}
//----------------------------------------------------------------------------
bool Environment::RemoveDirectory (const std::string& directory)
{
    if (!msDirectories)
    {
        Initialize();
    }

    std::vector<std::string>::iterator iter = msDirectories->begin();
    std::vector<std::string>::iterator end = msDirectories->end();
    for (/**/; iter != end; ++iter)
    {
        if (directory == *iter)
        {
            msDirectories->erase(iter);
            return true;
        }
    }
    return false;
}
//----------------------------------------------------------------------------
void Environment::RemoveAllDirectories ()
{
    if (!msDirectories)
    {
        Initialize();
    }

    msDirectories->clear();
}
//----------------------------------------------------------------------------
std::string Environment::GetPathR (const std::string& fileName)
{
    return GetPath(fileName, "r");
}
//----------------------------------------------------------------------------
std::string Environment::GetPathW (const std::string& fileName)
{
    return GetPath(fileName, "w");
}
//----------------------------------------------------------------------------
std::string Environment::GetPathRW (const std::string& fileName)
{
    return GetPath(fileName, "r+");
}
//----------------------------------------------------------------------------
std::string Environment::GetPath (const std::string& fileName,
    const char* attributes)
{
    if (!msDirectories)
    {
        Initialize();
    }

    std::vector<std::string>::iterator iter = msDirectories->begin();
    std::vector<std::string>::iterator end = msDirectories->end();
    for (/**/; iter != end; ++iter)
    {
        std::string decorated = *iter + fileName;
        FILE* testFile = fopen(decorated.c_str(), attributes);
        if (testFile)
        {
            fclose(testFile);
            return decorated;
        }
    }
    return std::string("");
}
//----------------------------------------------------------------------------
