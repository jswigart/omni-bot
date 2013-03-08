#include <assert.h>

#include "Connection.h"
#include "SystemLib.h"

namespace RemoteLib
{
	static const int ConnectionBufferSize = 1024 * 1024 * 4;
	static const int ConnectionRetryInterval = 3000;

	enum {
		RcvBufferSize	= 1024*1024*2,
		SendBufferSize	= 1024 * 64,
	};

	Connection::Connection( TcpSocket & sock ) {
		socket.swap( sock );

		internalInit( "", port );

		const char * peerName = socket.getPeerName();
		strcpy_s( ipAddress, peerName ? peerName : "" );

		rcvBuffer.init( NULL, RcvBufferSize );
		sndBuffer.init( NULL, SendBufferSize );

		autoConnect = false;
	}

	Connection::Connection( const char * ip, unsigned short port ) {
		socket.init();
		internalInit( ip, port );
		autoConnect = true;
	}

	Connection::~Connection() {
		socket.disconnect();
	}

	void Connection::internalInit( const char * ip, unsigned short port ) {
		if ( ip ) {
			strcpy_s( ipAddress, ip );
		}
		this->port = port;

		nextConnectTime = 0;

		assert( socket.isValid() );
		socket.setBufferSizes( ConnectionBufferSize, ConnectionBufferSize );
		socket.setNonBlocking( true );

		stats.connectTime = 0;
		stats.totalBytesRcvd = 0;
		stats.totalBytesSent = 0;
	}

	void Connection::update( ConnectionCallbacks * cb ) {
		cb;
		if ( socket.isConnected() ) {
			sendAndRecieveData();

			if ( socket.getSocketError() == TcpSocket::SocketDisconnected ) {
				onDisconnect();
			}
		} else {
			if ( autoConnect ) {
				if ( tryToConnect() ) {
					onConnect();
				}
			}
		}
	}

	bool Connection::shouldDelete() {
		if ( !isConnected() && !isAutoConnectEnabled() ) {
			return true;
		}
		return false;
	}

	bool Connection::tryToConnect() {
		if ( SystemLib::getTime() > nextConnectTime ) {
			nextConnectTime = SystemLib::getTime() + ConnectionRetryInterval;
			if ( socket.connect( ipAddress, port ) ) {
				stats.connectTime = SystemLib::getTime();
				stats.totalBytesRcvd = 0;
				stats.totalBytesSent = 0;

				rcvBuffer.init( NULL, RcvBufferSize );
				sndBuffer.init( NULL, SendBufferSize );
				return true;
			}
		}
		return false;
	}

	void Connection::sendAndRecieveData() {
		if ( socket.isConnected() ) {
			stats.totalBytesRcvd +=	rcvBuffer.readFromSocket( socket );
			stats.totalBytesSent += sndBuffer.sendToSocket( socket );
		}
	}

	//////////////////////////////////////////////////////////////////////////

	ConnectionManager::ConnectionManager( unsigned short maxConn )
		: maxConnections( maxConn )
		, connections( 0 )
		, skipSocketShutdown( false ) {
			setMaxConnections( maxConn );
	}

	ConnectionManager::~ConnectionManager() {
		destroyConnections();
		delete [] connections;
		connections = NULL;
	}

	void ConnectionManager::setMaxConnections( unsigned short maxConn ) {
		destroyConnections();
		delete [] connections;
		connections = NULL;

		maxConnections = maxConn;
		if ( maxConnections > 0 ) {
			connections = new Connection *[ maxConnections ];
			for( unsigned short i = 0; i < maxConnections; ++i ) {
				connections[ i ] = NULL;
			}
		}
	}

	void ConnectionManager::addConnection( Connection * conn ) {
		for( int i = 0; i < maxConnections; ++i ) {
			if ( connections[ i ] == NULL ) {
				connections[ i ] = conn;
				break;
			}
		}
	}

	int ConnectionManager::getNumConnections() const {
		int numConn = 0;
		for( int i = 0; i < maxConnections; ++i ) {
			if ( connections[ i ] != NULL ) {
				++numConn;
			}
		}
		return numConn;
	}

	Connection * ConnectionManager::getConnection( int index ) const {
		int numConn = 0;
		for( int i = 0; i < maxConnections; ++i ) {
			if ( connections[ i ] != NULL ) {
				if ( numConn == index ) {
					return connections[ i ];
				}
				numConn++;
			}
		}
		return NULL;
	}

	bool ConnectionManager::init( bool dontInitSockets ) {
		if ( !dontInitSockets ) {
			RemoteLib::TcpSocket::initSockets();
		}
		skipSocketShutdown = dontInitSockets;
		return true;
	}

	void ConnectionManager::shutdown() {
		if ( !skipSocketShutdown ) {
			RemoteLib::TcpSocket::shutdownSockets();
		}
	}

	void ConnectionManager::destroyConnections() {
		for( int i = 0; i < maxConnections; ++i ) {
			if ( connections[ i ] != NULL ) {
				delete connections[ i ];
				connections[ i ] = NULL;
			}
		}
	}

	void ConnectionManager::updateConnections( ConnectionCallbacks * cb ) {
		for( int i = 0; i < maxConnections; ++i ) {
			if ( connections[ i ] != NULL ) {
				connections[ i ]->update( cb );

				if ( connections[ i ]->shouldDelete() ) {
					delete connections[ i ];
					connections[ i ] = NULL;
				}
			}
		}
	}

	void ConnectionManager::sendToAll( const DataBuffer & db ) {
		if( db.getReadBytesLeft() > 0 ) {
			for( int i = 0; i < maxConnections; ++i ) {
				if ( connections[ i ] != NULL && connections[ i ]->isConnected() ) {
					//connections[ i ]->getSendBuffer().append( db );
				}
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	ConnectionManagerServer::ConnectionManagerServer( unsigned short maxConn, unsigned short port )
		: ConnectionManager( maxConn )
		, listenPort ( port ) {
	}

	ConnectionManagerServer::~ConnectionManagerServer() {
		shutdown();
	}

	bool ConnectionManagerServer::init( int sendBufSize, int recvBufSize, bool dontInitSockets ) {
		ConnectionManager::init( dontInitSockets );
		server.init();
		server.setNonBlocking( true );
		server.setBufferSizes( sendBufSize, recvBufSize );
		return server.listen( listenPort );
	}

	void ConnectionManagerServer::shutdown() {
		server.destroy();
		ConnectionManager::shutdown();
	}

	void ConnectionManagerServer::updateConnections( ConnectionCallbacks * cb ) {
		for( int i = 0; i < maxConnections; ++i ) {
			if ( connections[ i ] == NULL ) {
				TcpSocket newClient;
				if ( server.accept( newClient ) ) {
					connections[ i ] = cb->CreateNewConnection( newClient );
					connections[ i ]->onConnect();
					break;
				}
			}
		}
		ConnectionManager::updateConnections( cb );
	}
};