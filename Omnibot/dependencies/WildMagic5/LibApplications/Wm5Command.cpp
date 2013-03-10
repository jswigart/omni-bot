// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5ApplicationPCH.h"
#include "Wm5Command.h"
#include "Wm5Memory.h"
using namespace Wm5;

char Command::msOptionNotFound[] = "option not found";
char Command::msArgumentRequired[] = "option requires an argument";
char Command::msArgumentOutOfRange[] = "argument out of range";
char Command::msFilenameNotFound[] = "filename not found";

//----------------------------------------------------------------------------
Command::Command (int numArguments, char** arguments)
{
    mNumArguments = numArguments;
    mCommandLine = 0;
    mUsed = 0;

    if (mNumArguments > 0)
    {
        mArguments = new1<char*>(mNumArguments);
        for (int i = 0; i < mNumArguments; ++i)
        {
            mArguments[i] = arguments[i];
        }
    }
    else
    {
        mArguments = 0;
    }

    Initialize();
}
//----------------------------------------------------------------------------
Command::Command (char* commandLine)
{
    mNumArguments = 0;
    mArguments = 0;
    mCommandLine = 0;
    mUsed = 0;

    if (commandLine == 0 || strlen(commandLine) == 0)
    {
        return;
    }
    mCommandLine = new1<char>(strlen(commandLine) + 1);
    strcpy(mCommandLine, commandLine);

    char* token = strtok(mCommandLine, " \t");
    Argument* argList = 0;

    while (token)
    {
        mNumArguments++;

        Argument* current = new0 Argument(token, argList);
        argList = current;

        token = strtok(0, " \t");
    }

    mNumArguments++;
    mArguments = new1<char*>(mNumArguments);
    mArguments[0] = mCommandLine;
    int i = mNumArguments - 1;
    while (argList)
    {
        mArguments[i--] = argList->Item;
        
        Argument* save = argList->Next;
        delete0(argList->Next);
        argList = save;
    }

    Initialize();
}
//----------------------------------------------------------------------------
Command::~Command ()
{
    delete1(mUsed);
    delete1(mArguments);
    delete1(mCommandLine);
}
//----------------------------------------------------------------------------
void Command::Initialize ()
{
    mUsed = new1<bool>(mNumArguments);
    memset(mUsed, false, mNumArguments*sizeof(bool));

    mSmall = 0.0;
    mLarge = 0.0;
    mMinSet = false;
    mMaxSet = false;
    mInfSet = false;
    mSupSet = false;

    mLastError = 0;
}
//----------------------------------------------------------------------------
int Command::ExcessArguments ()
{
    // Check to see whether any command line arguments were not processed.
    for (int i = 1; i < mNumArguments; ++i)
    {
        if (!mUsed[i])
        {
            return i;
        }
    }

    return 0;
}
//----------------------------------------------------------------------------
Command& Command::Min (double value)
{
    mSmall = value;
    mMinSet = true;
    return *this;
}
//----------------------------------------------------------------------------
Command& Command::Max (double value)
{
    mLarge = value;
    mMaxSet = true;
    return *this;
}
//----------------------------------------------------------------------------
Command& Command::Inf (double value)
{
    mSmall = value;
    mInfSet = true;
    return *this;
}
//----------------------------------------------------------------------------
Command& Command::Sup (double value)
{
    mLarge = value;
    mSupSet = true;
    return *this;
}
//----------------------------------------------------------------------------
int Command::GetBoolean (const char* name)
{
    bool value = false;
    return GetBoolean(name, value);
}
//----------------------------------------------------------------------------
int Command::GetBoolean (const char* name, bool& value)
{
    int matchFound = 0;
    value = false;
    for (int i = 1; i < mNumArguments; ++i)
    {
        char* tmp = mArguments[i];
        if (!mUsed[i] && tmp[0] == '-' && strcmp(name, ++tmp) == 0)
        {
            mUsed[i] = true;
            matchFound = i;
            value = true;
            break;
        }
    }

    if (matchFound == 0)
    {
        mLastError = msOptionNotFound;
    }

    return matchFound;
}
//----------------------------------------------------------------------------
int Command::GetInteger (const char* name, int& value)
{
    int matchFound = 0;
    for (int i = 1; i < mNumArguments; ++i)
    {
        char* tmp = mArguments[i];
        if (!mUsed[i] && tmp[0] == '-' && strcmp(name, ++tmp) == 0)
        {
            // Get argument.
            tmp = mArguments[i + 1];
            if (mUsed[i + 1] || (tmp[0] == '-' && !isdigit(tmp[1])))
            {
                mLastError = msArgumentRequired;
                return 0;
            }
            value = atoi(tmp);
            if ((mMinSet && value < mSmall)
            ||  (mMaxSet && value > mLarge)
            ||  (mInfSet && value <= mSmall)
            ||  (mSupSet && value >= mLarge))
            {
                mLastError = msArgumentOutOfRange;
                return 0;
            }
            mUsed[i] = true;
            mUsed[i + 1] = true;
            matchFound = i;
            break;
        }
    }

    mMinSet = false;
    mMaxSet = false;
    mInfSet = false;
    mSupSet = false;

    if (matchFound == 0)
    {
        mLastError = msOptionNotFound;
    }

    return matchFound;
}
//----------------------------------------------------------------------------
int Command::GetFloat (const char* name, float& value)
{
    int matchFound = 0;
    for (int i = 1; i < mNumArguments; ++i)
    {
        char* tmp = mArguments[i];
        if (!mUsed[i] && tmp[0] == '-' && strcmp(name, ++tmp) == 0)
        {
            // Get argument.
            tmp = mArguments[i + 1];
            if (mUsed[i + 1] || (tmp[0] == '-' && !isdigit(tmp[1])))
            {
                mLastError = msArgumentRequired;
                return 0;
            }
            value = (float)atof(tmp);
            if ((mMinSet && value < mSmall)
            ||  (mMaxSet && value > mLarge)
            ||  (mInfSet && value <= mSmall)
            ||  (mSupSet && value >= mLarge))
            {
                mLastError = msArgumentOutOfRange;
                return 0;
            }
            mUsed[i] = true;
            mUsed[i + 1] = true;
            matchFound = i;
            break;
        }
    }

    mMinSet = false;
    mMaxSet = false;
    mInfSet = false;
    mSupSet = false;

    if (matchFound == 0)
    {
        mLastError = msOptionNotFound;
    }

    return matchFound;
}
//----------------------------------------------------------------------------
int Command::GetDouble (const char* name, double& value)
{
    int matchFound = 0;
    for (int i = 1; i < mNumArguments; ++i)
    {
        char* tmp = mArguments[i];
        if (!mUsed[i] && tmp[0] == '-' && strcmp(name, ++tmp) == 0)
        {
            // Get argument.
            tmp = mArguments[i + 1];
            if (mUsed[i + 1] || (tmp[0] == '-' && !isdigit(tmp[1])))
            {
                mLastError = msArgumentRequired;
                return 0;
            }
            value = atof(tmp);
            if ((mMinSet && value < mSmall)
            ||  (mMaxSet && value > mLarge)
            ||  (mInfSet && value <= mSmall)
            ||  (mSupSet && value >= mLarge))
            {
                mLastError = msArgumentOutOfRange;
                return 0;
            }
            mUsed[i] = true;
            mUsed[i + 1] = true;
            matchFound = i;
            break;
        }
    }

    mMinSet = false;
    mMaxSet = false;
    mInfSet = false;
    mSupSet = false;

    if (matchFound == 0)
    {
        mLastError = msOptionNotFound;
    }

    return matchFound;
}
//----------------------------------------------------------------------------
int Command::GetString (const char* name, char*& value)
{
    int matchFound = 0;
    for (int i = 1; i < mNumArguments; ++i)
    {
        char* tmp = mArguments[i];
        if (!mUsed[i] && tmp[0] == '-' && strcmp(name, ++tmp) == 0)
        {
            // Get argument.
            tmp = mArguments[i + 1];
            if (mUsed[i + 1] || tmp[0] == '-')
            {
                mLastError = msArgumentRequired;
                return 0;
            }

            value = new1<char>(strlen(tmp) + 1);
            strcpy(value, tmp);
            mUsed[i] = true;
            mUsed[i + 1] = true;
            matchFound = i;
            break;
        }
    }

    if (matchFound == 0)
    {
        mLastError = msOptionNotFound;
    }

    return matchFound;
}
//----------------------------------------------------------------------------
int Command::GetFilename (char*& name)
{
    int matchFound = 0;
    for (int i = 1; i < mNumArguments; ++i)
    {
        char* tmp = mArguments[i];
        if (!mUsed[i] && tmp[0] != '-')
        {
            name = new1<char>(strlen(tmp) + 1);
            strcpy(name, tmp);
            mUsed[i] = true;
            matchFound = i;
            break;
        }
    }

    if (matchFound == 0)
    {
        mLastError = msFilenameNotFound;
    }

    return matchFound;
}
//----------------------------------------------------------------------------
const char* Command::GetLastError ()
{
    return mLastError;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Command::Argument
//----------------------------------------------------------------------------
Command::Argument::Argument (char* item, Argument* next)
    :
    Item(item),
    Next(next)
{
}
//----------------------------------------------------------------------------
Command::Argument::~Argument ()
{
}
//----------------------------------------------------------------------------
