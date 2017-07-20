//////////////////////////////////////////////////////////////////////////

class ScriptState {
public:
	enum StateStatus {
		State_Busy,
		State_Finished,
	};

	virtual float GetPriority() { return 0.0f; }
	virtual void Initialize() {}
	virtual void Enter() {}
	virtual void Exit() {}
	virtual StateStatus Update( float dt ) { return State_Busy; };
	//virtual void ProcessEvent(const Message &_message, CallbackParameters &_cb) {}
};



//////////////////////////////////////////////////////////////////////////

struct ScriptHost {
	virtual void RegisterState( const char * name ) = 0;
	virtual void RegisterMapScript( const char * mapname ) = 0;
};

//////////////////////////////////////////////////////////////////////////

enum ScriptModuleStatus {
	ScriptModuleOk,
	ScriptModuleWrongVersion,
};
extern "C" ScriptModuleStatus RegisterScriptModule( const ScriptHost * host, int size );
