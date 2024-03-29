#include "../game/q_shared.h"
#include "IEngineInterface.h"

// Debug rendering functions are in cg_omnibot.c
extern "C" void DrawDebugLine(float *_start, float *_end, int _duration, int _color);
extern "C" void DrawDebugRadius(float *_start, float _radius, int _duration, int _color);
extern "C" void DrawDebugAABB(float *mins, float *_maxs, int _duration, int _color, int _side);
extern "C" void DrawDebugPolygon(vec3_t *verts, int _numverts, int _duration, int _color);
extern "C" void DrawDebugText(float *_start, const char *_msg, int _duration, int _color);
extern "C" void ClearDebugDraw();
extern "C" void CG_DrawLine(float *start, float *end, int color);

class ETClientInterface : public IClientInterface
{
	void DrawLine(const float _start[3], const float _end[3], int _color, int _time)
	{
		DrawDebugLine((float*)_start, (float*)_end, _time, _color);
	}

	void DrawAABB(const float _mins[3], const float _maxs[3], int _color, int _time, AABB::Direction _dir)
	{
		DrawDebugAABB((float*)_mins, (float*)_maxs, _time, _color, _dir);
	}

	void DrawRadius(const float _pos[3], const float _radius, int _color, int _time)
	{
		DrawDebugRadius((float*)_pos, _radius, _time, _color); 
	}

	void DrawPolygon(const float *_verts, const int _numverts, int _color, int _time)
	{
		DrawDebugPolygon((vec3_t*)_verts, _numverts, _time, _color);
	}

	void PrintScreenText(const float _pos[3], int _duration, int _color, const char *_msg)
	{
		DrawDebugText((float*)_pos, _msg, _duration, _color); 
	}

	void ClearView()
	{
		ClearDebugDraw();
	}

	void DrawLine(const float _start[3], const float _end[3], int _color)
	{
		CG_DrawLine((float*)_start, (float*)_end, _color);
	}

} g_ClientInterface;

extern "C" void OmnibotDrawActiveFrame()
{
	if(g_ClientInterface.DrawActiveFrame) g_ClientInterface.DrawActiveFrame();
}

#ifdef WIN32
#define OMNIBOT_API __declspec(dllexport)
#else
#define OMNIBOT_API 
#endif

extern "C" OMNIBOT_API eomnibot_error ExportClientFunctionsFromDLL(IClientInterface **_pClientFuncs, int _version)
{
	if(_version==2){
		*_pClientFuncs = &g_ClientInterface;
		return BOT_ERROR_NONE;
	}
	return BOT_ERROR_BAD_INTERFACE;
}
