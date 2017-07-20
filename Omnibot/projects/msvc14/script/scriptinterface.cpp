#include "scriptinterface.h"

ScriptModuleStatus RegisterScriptModule( const ScriptHost & host, int size ) {
	if ( sizeof( ScriptHost ) != size ) {
		return ScriptModuleWrongVersion;
	}



	return ScriptModuleOk;
}
