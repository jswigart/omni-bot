#include "../game/q_shared.h"
#include "Omnibot_Interprocess.h"
#include "Omni-Bot_Types.h"

//////////////////////////////////////////////////////////////////////////
// Callbacks for debug rendering
extern "C" void DrawDebugLine( float *_start, float *_end, int _duration, int _color );
extern "C" void DrawDebugRadius( float *_start, float _radius, int _duration, int _color );
extern "C" void DrawDebugAABB( float *mins, float *_maxs, int _duration, int _color, int _side );
extern "C" void DrawDebugPolygon( vec3_t *verts, int _numverts, int _duration, int _color );
extern "C" void DrawDebugText( float *_start, const char *_msg, int _duration, int _color );
//////////////////////////////////////////////////////////////////////////

#include <boost/interprocess/ipc/message_queue.hpp>
using namespace boost::interprocess;

extern "C" void OmnibotDisableDrawing();

extern "C" void CG_Printf( const char *msg, ... );

int g_IsInitialized = 0;

extern "C" int InterProcessIsInitialized() {
	return g_IsInitialized;
}

extern "C" InterProcessError InterProcessInitialize() {
	g_IsInitialized = 1;
	return IP_NoError;
}

extern "C" void InterProcessShutdown() {
}

extern "C" void InterProcessUpdate() {
	if ( InterProcessIsInitialized() ) {
		try
		{
			//Open a message queue.
			message_queue mq( open_only,"debug_draw_queue" );
			unsigned int priority = 0;
			unsigned int recvd_size = 0;
			IPC_DebugDrawMsg msg;
			while ( mq.try_receive( &msg, sizeof( msg ), recvd_size, priority ) )
			{
				if ( recvd_size != sizeof( msg ) ) {
					continue;
				}

				switch ( msg.m_Debugtype )
				{
				case DRAW_LINE:
				{
					DrawDebugLine(
						(float*)&msg.data.m_Line.m_Start,
						(float*)&msg.data.m_Line.m_End,
						msg.m_Duration,
						msg.data.m_Line.m_Color );
					continue;
				}
				case DRAW_RADIUS:
				{
					DrawDebugRadius(
						(float*)&msg.data.m_Radius.m_Pos,
						msg.data.m_Radius.m_Radius,
						msg.m_Duration,
						msg.data.m_Radius.m_Color );
					continue;
				}
				case DRAW_BOUNDS:
				{
					DrawDebugAABB(
						(float*)&msg.data.m_AABB.m_Mins,
						(float*)&msg.data.m_AABB.m_Maxs,
						msg.m_Duration,
						msg.data.m_AABB.m_Color,
						msg.data.m_AABB.m_Sides );
					continue;
				}
				case DRAW_POLYGON:
				{
					DrawDebugPolygon(
						(vec3_t*)&msg.data.m_Polygon.m_Verts,
						msg.data.m_Polygon.m_NumVerts,
						msg.m_Duration,
						msg.data.m_Polygon.m_Color );
					continue;
				}
				case DRAW_TEXT:
				{
					DrawDebugText(
						(float*)&msg.data.m_Radius.m_Pos, 
						msg.data.m_Text.m_Buffer, 
						msg.m_Duration, 
						msg.data.m_Text.m_Color );
					continue;
				}
				default:
					goto msg_error;
				}
msg_error:
				CG_Printf( "Omni-bot: Invalid InterProcess Message\n" );
			}
		}
		catch ( interprocess_exception &ex )
		{
			CG_Printf( "Omni-bot: Interproces Exception: %s\n", ex.what() );
			// disable it, so it doesn't spam messages.
			OmnibotDisableDrawing();
		}
	}
}
