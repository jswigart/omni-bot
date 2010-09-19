
#ifdef WIN32
#define REMOTEGUI_API __declspec(dllexport)
#else
#define REMOTEGUI_API 
#endif

namespace RemoteGui {
	class StateTreeInterface {
	public:
		virtual void SetNumColumns( int numColumns ) = 0;
		virtual void SetColumnName( int column, const char * name ) = 0;

		virtual void SetNodeEnum( const char * path, const char * names[], int current ) = 0;
	private:
		StateTreeInterface() {}
		virtual ~StateTreeInterface() = 0;
	};

	extern "C" REMOTEGUI_API StateTreeInterface * AllocStateTree();
	extern "C" REMOTEGUI_API void FreeStateTree( StateTreeInterface * tree );
};
