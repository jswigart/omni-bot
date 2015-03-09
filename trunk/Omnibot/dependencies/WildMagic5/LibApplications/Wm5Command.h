// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5COMMAND_H
#define WM5COMMAND_H

namespace Wm5
{

class Command
{
public:
    // Construction and destruction.
    friend class Memory;
    Command (int numArguments, char** arguments);
    Command (char* commandLine);
    ~Command ();

    // The return value is the index of the first excess argument.
    int ExcessArguments ();

    // Set bounds for numerical arguments.  If bounds are required, they must
    // be set for each argument.
    Command& Min (double value);
    Command& Max (double value);
    Command& Inf (double value);
    Command& Sup (double value);

    // The return value of each of the following methods is the option index
    // within the argument array.

    // Use the bool methods for options which take no argument, for
    // example in
    //     myprogram -debug -x 10 -y 20 filename
    // the option -debug has no argument.

    int GetBoolean (const char* name);  // returns existence of option
    int GetBoolean (const char* name, bool& value);
    int GetInteger (const char* name, int& value);
    int GetFloat (const char* name, float& value);
    int GetDouble (const char* name, double& value);
    int GetString (const char* name, char*& value);
    int GetFilename (char*& name);

    // Last-error reporting.
    const char* GetLastError ();

protected:
    // Singly linked list for command-line arguments.
    class Argument
    {
    public:
        Argument (char* item, Argument* next);
        ~Argument ();

        char* Item;
        Argument* Next;
    };

    // Support for construction.
    void Initialize ();

    // Command line information.
    int mNumArguments;   // number of arguments
    char** mArguments;   // argument list (array of strings)
    char* mCommandLine;  // argument list (single string)
    bool* mUsed;      // keeps track of arguments already processed

    // Parameters for bounds checking.
    double mSmall;    // lower bound for numerical argument (min or inf)
    double mLarge;    // upper bound for numerical argument (max or sup)
    bool mMinSet;  // if true, compare:  small <= arg
    bool mMaxSet;  // if true, compare:  arg <= large
    bool mInfSet;  // if true, compare:  small < arg
    bool mSupSet;  // if true, compare:  arg < large

    // Last-error strings.
    const char* mLastError;
    static char msOptionNotFound[];
    static char msArgumentRequired[];
    static char msArgumentOutOfRange[];
    static char msFilenameNotFound[];
};

}

#endif
