#include "Module.h"

#ifdef __linux__
#include <dlfcn.h>
#else
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOWINRES
#define NOWINRES
#endif
#ifndef NOSERVICE
#define NOSERVICE
#endif
#ifndef NOMCX
#define NOMCX
#endif
#ifndef NOIME
#define NOIME
#endif
#include <stdio.h>
#include <windows.h>
#endif

Module::Module( const char * moduleName ) : mModule( NULL )
{
	Load( moduleName );
}

bool Module::Load( const char * moduleName )
{
	if ( moduleName )
	{
#ifdef __linux__
		mModule = dlopen( moduleName, RTLD_NOW );
#else
		mModule = LoadLibrary( moduleName );
#endif
	}
	return mModule != NULL;
}

void Module::Unload()
{
	if ( mModule )
	{
#ifdef __linux__
		mModule = dlclose( mModule );
#else
		FreeLibrary( (HMODULE)mModule );
#endif
		mModule = NULL;
	}
}

bool Module::IsLoaded() const
{
	return mModule != NULL;
}

void * Module::GetFunc( const char * funcName )
{
#ifdef __linux__
	return dlsym( mModule );
#else
	return GetProcAddress( (HMODULE)mModule, funcName );
#endif
}