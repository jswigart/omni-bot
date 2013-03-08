#pragma once

#include <boost/interprocess/ipc/message_queue.hpp>

using namespace boost::interprocess;

namespace Ipc {
	//////////////////////////////////////////////////////////////////////////
	struct StateTreeMsg {
		enum { PathSize = 64, NameSize = 32, DebugInfoSize = 128 };
		char		path[PathSize];
		char		name[NameSize];
		char		dbgInfo[DebugInfoSize];
		int			color;
	};
	struct GraphicsMsg {
		enum { PathSize = 32 };
		enum PrimType { PrimLine, PrimCircle, PrimBox };
		char		path[PathSize];
		char		img[PathSize];
		PrimType	type;
		float		x, y, z;
		float		sx, sy, sz;
		float		yaw;
		int			color;

		void Circle( const char * pth, const float * p, float radius, int clr ) {
			type = PrimCircle;
			strncpy( path, pth, sizeof( path ) );
			x = p[0]; y = p[1]; z = p[2];
			sx = sy = radius; sz = 0.0f;
			yaw = 0.0f;
			color = clr;
		}
		void Line( const char * pth, const float * p0, const float * p1, int clr ) {
			type = PrimLine;
			strncpy( path, pth, sizeof( path ) );
			x = p0[0]; y = p0[1]; z = p0[2];
			sx = p1[0]; sy = p1[1]; sz = p1[2];
			yaw = 0.0f;
			color = clr;
		}
	};
	//////////////////////////////////////////////////////////////////////////

	template < typename T, int MaxMessages = 32 >
	class MessageQueue {
	public:
		// blocks if full
		void send( const T & msg, unsigned int priority = 0 ) {
			queue.send(&msg, sizeof(T), priority);
		}
		bool trySend( const T & msg, unsigned int priority = 0 ) {
			return queue.try_send( &msg, sizeof(T), priority );
		}
		// blocks if full
		void recieve( T & msg, size_t & recSize, unsigned int & recPrio ) {
			queue.receive( &msg, sizeof( T ), recSize, recPrio );
		}
		bool tryRecieve( T & msg, size_t & recSize, unsigned int & recPrio ) {
			return queue.try_receive( &msg, sizeof( T ), recSize, recPrio );
		}

		unsigned int getMaxMsgs() const { return queue.get_max_msg(); }
		unsigned int getMaxMsgSize() const { return queue.get_max_msg_size(); }
		unsigned int getNumMsgs() const { return queue.get_num_msg(); }

		MessageQueue( const char * name, bool removeOnExit = false )
			: queueName( name )
			, queue( open_or_create, name, MaxMessages, sizeof( T ) )
			, delQueue( removeOnExit ) {
		}
		~MessageQueue() {
			if ( delQueue ) {
				message_queue::remove( queueName );
			}
		}
	private:
		const char *	queueName;
		message_queue	queue;
		bool			delQueue;
	};

	typedef MessageQueue<StateTreeMsg, 256>	StateNodeQueue;
	typedef MessageQueue<GraphicsMsg, 512>	GraphicsQueue;

	//////////////////////////////////////////////////////////////////////////
	// client should instantiate one of these structs
	// to initialize all necessary message queues
	struct RemoteQueues {
		StateNodeQueue		stateQueue;
		GraphicsQueue		gfxQueue;

		RemoteQueues( bool removeOnExit )
			: stateQueue( "statequeue", removeOnExit )
			, gfxQueue( "gfxqueue", removeOnExit )
		{
		}
	};
};
