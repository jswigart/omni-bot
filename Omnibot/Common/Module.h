#pragma once

#ifndef MODULE_H
#define MODULE_H

class Module
{
public:
	Module( const char * moduleName = 0 );

	bool Load( const char * moduleName );
	void Unload();
	bool IsLoaded() const;

	void * GetFunc( const char * funcName );
private:
	void *	mModule;

	Module( Module & other );
	Module&operator=( Module & other );
};

#endif
