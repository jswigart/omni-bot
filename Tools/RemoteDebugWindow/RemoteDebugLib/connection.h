#pragma once

#include <list>

#include "DataBuffer.h"
#include "Socket.h"

namespace RemoteLib
{
	class Connection;
	class ConnectionCallbacks
	{
	public:
		virtual void OnConnect( Connection * conn ) = 0;
		virtual void OnDisConnect( Connection * conn ) = 0;
		virtual void OnAcceptConnection( Connection * conn ) = 0;
		virtual ~ConnectionCallbacks()  {}
	};
	
	//////////////////////////////////////////////////////////////////////////

	class Connection
	{
	public:
		struct Stats {
			int					connectTime;
			int					totalBytesRcvd;
			int					totalBytesSent;
		};

		bool isConnected() const { return socket.isConnected(); }
		bool isAutoConnectEnabled() const { return autoConnect; }

		virtual bool shouldDelete();

		virtual void update( ConnectionCallbacks * cb );
		
		const char * getIp() const { return ipAddress; }
		const Stats & getStats() const { return stats; }

		DataBuffer & getRecvBuffer() { return rcvBuffer; }
		DataBuffer & getSendBuffer() { return sndBuffer; }

		// Notifications for sub classes.
		virtual void onConnect() {}
		virtual void onDisconnect() {}

		bool isNewConnection() const { return newConnection; }
		void clearNewConnection() { newConnection = false; }

		void setUserData( int data ) { userData = data; }
		int getUserData() const { return userData; }

		explicit Connection( TcpSocket & sock );
		Connection( const char * ip, unsigned short port = TcpSocket::DefaultPort );
		virtual ~Connection();
	protected:
		TcpSocket			socket;
		char				ipAddress[16];		
		unsigned short		port;

		unsigned int		nextConnectTime;

		Stats				stats;

		DataBuffer			rcvBuffer;
		DataBuffer			sndBuffer;
		
		bool				autoConnect;
		bool				newConnection;

		int					userData; // meh

		bool				tryToConnect();
		void				sendAndRecieveData();
	private:
		void				internalInit( const char * ip, unsigned short port );
	};

	//////////////////////////////////////////////////////////////////////////

	class ConnectionManager
	{
	public:
		virtual bool init( bool dontInitSockets = false );
		virtual void shutdown();
		virtual void updateConnections( ConnectionCallbacks * cb );
		virtual void destroyConnections();

		void setMaxConnections( int maxConn );

		void sendToAll( DataBuffer & db );

		void addConnection( Connection * conn );

		int getNumConnections() const;
		Connection * getConnection( int index ) const;

		ConnectionManager( int maxConn );
		virtual ~ConnectionManager();
	public:
		unsigned short			maxConnections;
		Connection **			connections;

		bool					skipSocketShutdown;
	};

	//////////////////////////////////////////////////////////////////////////

	class ConnectionManagerServer : public ConnectionManager
	{
	public:
		virtual bool init( bool dontInitSockets = false );
		virtual void shutdown();
		virtual void updateConnections( ConnectionCallbacks * cb = NULL );

		uint16 getPort() const { return listenPort; }
		void setPort( uint16 p ) { listenPort = p; }

		ConnectionManagerServer( unsigned short maxConn, unsigned short port = TcpSocket::DefaultPort );
		virtual ~ConnectionManagerServer();
	protected:
		uint16				listenPort;
		TcpSocket			server;
	};
};
