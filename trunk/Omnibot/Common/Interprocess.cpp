#include "PrecompCommon.h"
#include <iostream>
#include <fstream>
#include "Omni-Bot_Types.h"
#include "Omni-Bot_BitFlags.h"

#include "Interprocess.h"

//#define INTERPROCESS

#ifdef INTERPROCESS
#include <boost/interprocess/ipc/message_queue.hpp>
using namespace boost::interprocess;
#endif

//////////////////////////////////////////////////////////////////////////

Prof_Define(InterProcess);

//////////////////////////////////////////////////////////////////////////

#ifdef INTERPROCESS
template <typename MsgType>
class InterProcessMessageQueue
{
public:

	void Send(const MsgType &_message, obuint32 _priority = 0)
	{
		Prof(InterProcessMessageQueue_Send);
		m_MessageQueue.send(&_message, sizeof(MsgType), _priority);
	}

	bool TrySend(const MsgType &_message, obuint32 _priority = 0)
	{
		Prof(InterProcessMessageQueue_TrySend);
		return m_MessageQueue.try_send(&_message, sizeof(MsgType), _priority);
	}

	InterProcessMessageQueue(const char *_name, obuint32 _msgs) :
		m_MessageQueue(open_or_create, _name, _msgs, sizeof(MsgType)),
		m_Name(_name)
	{
	}
	~InterProcessMessageQueue()
	{
		message_queue::remove(m_Name.c_str());
	}
private:
	const String	m_Name;
	message_queue	m_MessageQueue;

	InterProcessMessageQueue();
};

typedef boost::shared_ptr< InterProcessMessageQueue<IPC_DebugDrawMsg> > MessageQueuePtr;
MessageQueuePtr g_MessageQueue;
#endif // INTERPROCESS
//////////////////////////////////////////////////////////////////////////
namespace InterProcess
{
	//////////////////////////////////////////////////////////////////////////
	void Init()
	{
		Prof_Scope(InterProcess);

		Vector3f v1(Vector3f::ZERO);

		if(!g_EngineFuncs->DebugLine(v1,v1,COLOR::GREEN,0.f) && 
			!g_EngineFuncs->DebugRadius(v1,0.f,COLOR::GREEN, 0.f))
		{
#ifdef INTERPROCESS
			try
			{
				message_queue::remove("debug_draw_queue");
				g_MessageQueue.reset(new InterProcessMessageQueue<IPC_DebugDrawMsg>("debug_draw_queue", 8192));
				LOG("InterProcess Initialized");
			}
			catch(interprocess_exception &ex)
			{
				g_MessageQueue.reset();
				LOGERR(ex.what());				
			}
#endif // INTERPROCESS
		}
		else
		{
			LOG("InterProcess Not Required, interface callbacks defined.");
		}
	}
	
	void Shutdown()
	{
#ifdef INTERPROCESS
		g_MessageQueue.reset();
#endif
	}

	void Update()
	{
		Prof_Scope(InterProcess);
		{
			Prof(Update);
#ifdef INTERPROCESS
			if(g_MessageQueue)
			{
				// todo: process incoming messages?
			}
#endif
		}
	}

	void Enable(bool _en)
	{
#ifdef INTERPROCESS
		if(!_en)
			g_MessageQueue.reset();
		if(_en && !g_MessageQueue)
			Init();
#endif
	}

	void DrawLine(const Vector3f &_a, const Vector3f &_b, obColor _color, float _time)
	{
		Prof_Scope(InterProcess);
		{
			Prof(DrawLine);
#ifdef INTERPROCESS
			if(g_MessageQueue)
			{
				IPC_DebugDrawMsg msg;
				msg.m_Debugtype = DRAW_LINE;
				msg.m_Duration = Utils::SecondsToMilliseconds(_time);

				msg.data.m_Line.m_Start.x = _a.x;
				msg.data.m_Line.m_Start.y = _a.y;
				msg.data.m_Line.m_Start.z = _a.z;

				msg.data.m_Line.m_End.x = _b.x;
				msg.data.m_Line.m_End.y = _b.y;
				msg.data.m_Line.m_End.z = _b.z;

				msg.data.m_Line.m_Color = _color.rgba();
				g_MessageQueue->TrySend(msg);
			}
#endif // INTERPROCESS
		}
	}

	void DrawRadius(const Vector3f &_a, float _radius, obColor _color, float _time)
	{
		Prof_Scope(InterProcess);
		{
			Prof(DrawRadius);
#ifdef INTERPROCESS
			if(g_MessageQueue)
			{
				IPC_DebugDrawMsg msg;
				msg.m_Debugtype = DRAW_RADIUS;
				msg.m_Duration = Utils::SecondsToMilliseconds(_time);

				msg.data.m_Radius.m_Pos.x = _a.x;
				msg.data.m_Radius.m_Pos.y = _a.y;
				msg.data.m_Radius.m_Pos.z = _a.z;
				
				msg.data.m_Radius.m_Radius = _radius;
				msg.data.m_Radius.m_Color = _color.rgba();
				g_MessageQueue->TrySend(msg);
			}
#endif // INTERPROCESS
		}
	}

	void DrawBounds(const AABB &_aabb, obColor _color, float _time, AABB::Direction _dir)
	{
		Prof_Scope(InterProcess);
		{
			Prof(DrawBounds);
#ifdef INTERPROCESS
			if(g_MessageQueue)
			{
				IPC_DebugDrawMsg msg;
				msg.m_Debugtype = DRAW_BOUNDS;
				msg.m_Duration = Utils::SecondsToMilliseconds(_time);

				msg.data.m_AABB.m_Mins.x = _aabb.m_Mins[0];
				msg.data.m_AABB.m_Mins.y = _aabb.m_Mins[1];
				msg.data.m_AABB.m_Mins.z = _aabb.m_Mins[2];

				msg.data.m_AABB.m_Maxs.x = _aabb.m_Maxs[0];
				msg.data.m_AABB.m_Maxs.y = _aabb.m_Maxs[1];
				msg.data.m_AABB.m_Maxs.z = _aabb.m_Maxs[2];

				msg.data.m_AABB.m_Color = _color.rgba();
				msg.data.m_AABB.m_Sides = _dir;
				g_MessageQueue->TrySend(msg);
			}
#endif // INTERPROCESS
		}
	}

	void DrawPolygon(const Vector3List &_vertices, obColor _color, float _time)
	{
		Prof_Scope(InterProcess);
		{
			Prof(DrawPolygon);
#ifdef INTERPROCESS
			if(g_MessageQueue)
			{
				IPC_DebugDrawMsg msg;
				msg.m_Debugtype = DRAW_POLYGON;
				msg.m_Duration = Utils::SecondsToMilliseconds(_time);

				msg.data.m_Polygon.m_NumVerts = _vertices.size();
				if(msg.data.m_Polygon.m_NumVerts>IPC_DebugPolygonMessage::MaxPolyVerts)
					msg.data.m_Polygon.m_NumVerts = IPC_DebugPolygonMessage::MaxPolyVerts;

				for(int v = 0; v < msg.data.m_Polygon.m_NumVerts; ++v)
				{
					msg.data.m_Polygon.m_Verts[v].x = _vertices[v].x;
					msg.data.m_Polygon.m_Verts[v].y = _vertices[v].y;
					msg.data.m_Polygon.m_Verts[v].z = _vertices[v].z;
				}

				msg.data.m_Polygon.m_Color = _color.rgba();
				g_MessageQueue->TrySend(msg);
			}
#endif // INTERPROCESS
		}
	}

	void DrawText(const Vector3f &_a, const char *_txt, obColor _color, float _time)
	{
		Prof_Scope(InterProcess);
		{
			Prof(DrawText);
#ifdef INTERPROCESS
			if(g_MessageQueue)
			{
				IPC_DebugDrawMsg msg;
				msg.m_Debugtype = DRAW_TEXT;
				msg.m_Duration = Utils::SecondsToMilliseconds(_time);

				msg.data.m_Text.m_Pos.x = _a.x;
				msg.data.m_Text.m_Pos.y = _a.y;
				msg.data.m_Text.m_Pos.z = _a.z;
				msg.data.m_Text.m_Color = _color.rgba();

				Utils::StringCopy(msg.data.m_Text.m_Buffer,_txt,IPC_DebugTextMessage::BufferSize);
				g_MessageQueue->TrySend(msg);
			}
#endif // INTERPROCESS
		}
	}
};
