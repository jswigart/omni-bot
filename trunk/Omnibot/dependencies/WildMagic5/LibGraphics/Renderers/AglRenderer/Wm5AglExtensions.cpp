// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include <mach-o/dyld.h>
#include <cstdlib>
#include <cstring>

extern "C"
{
//----------------------------------------------------------------------------
void* GTGetFunctionPointer (const char* function)
{
    NSSymbol symbol;
    char* symbolName;

    // Prepend a '_' for the Unix C symbol mangling convention.
    symbolName = (char*)malloc(strlen((const char*)function) + 2);
    strcpy(symbolName + 1,(const char*)function);
    symbolName[0] = '_';

    symbol = 0;
    if (NSIsSymbolNameDefined(symbolName))
	{
        symbol = NSLookupAndBindSymbol(symbolName);
	}

    free(symbolName);
    return symbol ? NSAddressOfSymbol(symbol) : 0;
}
//----------------------------------------------------------------------------
}
