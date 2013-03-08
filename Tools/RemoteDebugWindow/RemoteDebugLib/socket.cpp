#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <assert.h>
#include <iostream>
#else

#endif

#include "Socket.h"

namespace RemoteLib
{
	//////////////////////////////////////////////////////////////////////////
	static const int DefaultSendBufferSize = 1024 * 4;
	static const int DefaultRecvBufferSize = 1024 * 4;
	//////////////////////////////////////////////////////////////////////////

	class TcpSocketImpl
	{
	public:
		// server functions
		virtual bool listen( unsigned short port ) = 0;
		virtual bool accept( TcpSocket & socketOut ) = 0;

		// client functions
		virtual bool connect( const char * ip, unsigned short port ) = 0;

		// shared
		virtual TcpSocket::SocketError getSocketError() const = 0;
		virtual bool isConnected() const = 0;
		virtual void disconnect( TcpSocket::SocketError setError ) = 0;

		virtual bool sendData( int & bytesSent, const char * buf, int bytes ) = 0;
		virtual bool rcvData( int & bytesRead, char * buf, const int bufSize ) = 0;

		virtual void setBufferSizes( int sendSize, int rcvSize ) = 0;
		virtual bool setNonBlocking( bool enableNonBlocking ) = 0;

		virtual const char * getPeerName() = 0;

		virtual ~TcpSocketImpl() {}
	};

	//////////////////////////////////////////////////////////////////////////
#ifdef _WIN32
	const char *inet_ntop(int af, const void *src, char *dst, socklen_t cnt)
	{
		if (af == AF_INET)
		{
			struct sockaddr_in in;
			memset(&in, 0, sizeof(in));
			in.sin_family = AF_INET;
			memcpy(&in.sin_addr, src, sizeof(struct in_addr));
			getnameinfo((struct sockaddr *)&in, sizeof(struct
				sockaddr_in), dst, cnt, NULL, 0, NI_NUMERICHOST);
			return dst;
		}
		else if (af == AF_INET6)
		{
			struct sockaddr_in6 in;
			memset(&in, 0, sizeof(in));
			in.sin6_family = AF_INET6;
			memcpy(&in.sin6_addr, src, sizeof(struct in_addr6));
			getnameinfo((struct sockaddr *)&in, sizeof(struct
				sockaddr_in6), dst, cnt, NULL, 0, NI_NUMERICHOST);
			return dst;
		}
		return NULL;
	}

	class TcpSocketWin32 : public TcpSocketImpl
	{
	public:
		// server functions
		bool listen( unsigned short port ) {
			if ( isConnected() ) {
				return false;
			}

			socket = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
			if ( socket != INVALID_SOCKET ) {
				setBufferSizes( bufferSizeSend, bufferSizeRcv );
				setNonBlocking( nonBlocking );

				bool reuse = true;
				setsockopt(socket,SOL_SOCKET,SO_REUSEADDR,(char*)&reuse,sizeof(reuse));

				int flag = 1;
				setsockopt(socket,IPPROTO_TCP,TCP_NODELAY,(char*)&flag,sizeof(int));

				sockaddr_in service;
				service.sin_family = AF_INET;
				service.sin_addr.s_addr = inet_addr("127.0.0.1");
				service.sin_port = htons(port);

				const int bindRes = ::bind( socket, (SOCKADDR*)&service, sizeof(service) );
				if ( bindRes != SOCKET_ERROR ) {
					const int listenRes = ::listen( socket, 1 );
					if ( listenRes != SOCKET_ERROR ) {
						return true;
					}
				}
				checkForDisconnectError();
			}
			return false;
		}

		bool accept( TcpSocket & socketOut ) {
			if ( socket != INVALID_SOCKET ) {
				SOCKET acceptSocket = ::accept( socket, NULL, NULL );
				if ( acceptSocket != INVALID_SOCKET ) {
					TcpSocketWin32 * w32Socket = new TcpSocketWin32( acceptSocket );
					w32Socket->socketError = TcpSocket::SocketOk;
					socketOut.impl = w32Socket;
					return true;
				}
			}
			checkForDisconnectError();
			return false;
		}

		// client functions
		bool connect( const char * ip, unsigned short port ) {
			if ( isConnected() ) {
				return false;
			}

			if ( socket == INVALID_SOCKET ) {
				socket = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
			}
			if ( socket != INVALID_SOCKET ) {
				setBufferSizes( bufferSizeSend, bufferSizeRcv );
				setNonBlocking( nonBlocking );

				bool reuse = true;
				setsockopt(socket,SOL_SOCKET,SO_REUSEADDR,(char*)&reuse,sizeof(reuse));

				int flag = 1;
				setsockopt(socket,IPPROTO_TCP,TCP_NODELAY,(char*)&flag,sizeof(int));

				sockaddr_in service;
				service.sin_family = AF_INET;
				service.sin_addr.s_addr = inet_addr( ip );
				service.sin_port = htons( port );

				const int res = ::connect( socket, (SOCKADDR*)&service, sizeof(service) );

				// already connected
				const int errCode = WSAGetLastError();
				if ( errCode == WSAEISCONN ) {
					socketError = TcpSocket::SocketOk;
					return true;
				}

				if ( res != SOCKET_ERROR ) {
					socketError = TcpSocket::SocketOk;
					return true;
				} else {
					checkForDisconnectError();
				}
			}
			return false;
		}

		// shared
		TcpSocket::SocketError getSocketError() const {
			return socketError;
		}

		bool isConnected() const {
			return socket != INVALID_SOCKET && socketError == TcpSocket::SocketOk;
		}

		void disconnect( TcpSocket::SocketError setError = TcpSocket::SocketDisconnected ) {
			socketError = setError;
			if ( socket != INVALID_SOCKET ) {
				::closesocket( socket );
				socket = INVALID_SOCKET;
			}
		}

		bool sendData( int & bytesSent, const char * buf, int bytes ) {
			if ( isConnected() ) {
				bytesSent = 0;
				const int sentBytes = ::send( socket, buf, bytes, 0 );
				if ( sentBytes > 0 ) {
					bytesSent = sentBytes;
					return true;
				}

				// connection error?
				if ( sentBytes == SOCKET_ERROR ) {
					return !checkForDisconnectError();
				}
			}
			return false;
		}

		bool rcvData( int & bytesRead, char * buf, const int bufSize ) {
			if ( isConnected() ) {
				bytesRead = ::recv( socket, buf, bufSize, 0 );

				if ( bytesRead > 0 ) {
					return true;
				}

				// connection error?
				if ( bytesRead == SOCKET_ERROR ) {
					bytesRead = 0;
					return !checkForDisconnectError();
				}
			}
			return false;
		}

		void setBufferSizes( int sendSize, int rcvSize ) {
			bufferSizeSend = sendSize;
			bufferSizeRcv = rcvSize;
			if ( socket ) {
				setsockopt(socket,SOL_SOCKET,SO_RCVBUF,(char*)&rcvSize,sizeof(rcvSize));
				setsockopt(socket,SOL_SOCKET,SO_SNDBUF,(char*)&sendSize,sizeof(sendSize));
			}
		}

		bool setNonBlocking( bool enableNonBlocking ) {
			nonBlocking = enableNonBlocking;
			u_long parm = enableNonBlocking ? 1 : 0;
			return ioctlsocket( socket, FIONBIO, &parm ) != SOCKET_ERROR;
		}

		const char * getPeerName() {
			if ( isConnected() ) {
				sockaddr_in peer;
				int peer_len = sizeof(peer);
				const int res = ::getpeername( socket, (sockaddr*)&peer, &peer_len );
				if ( res != SOCKET_ERROR ) {
					inet_ntop( AF_INET, &peer.sin_addr, peerIp, sizeof( peerIp ) );
					return peerIp;
				} else {
					checkForDisconnectError();
				}
			}
			return NULL;
		}

		TcpSocketWin32( SOCKET sock = INVALID_SOCKET )
			: socketError( TcpSocket::SocketDisconnected )
			, socket( sock )
			, nonBlocking( false )
			, bufferSizeSend( DefaultSendBufferSize )
			, bufferSizeRcv( DefaultRecvBufferSize ) {
				peerIp[0] = 0;
		}

		~TcpSocketWin32() {
			disconnect();
		}
	private:
		TcpSocket::SocketError		socketError;
		SOCKET						socket;

		bool						nonBlocking;
		int							bufferSizeSend;
		int							bufferSizeRcv;

		char						peerIp[32];

		bool checkForDisconnectError() {
			const int err = WSAGetLastError();
			switch( err )
			{
			case WSAEWOULDBLOCK:
			case WSAEISCONN:
			case WSAEALREADY:
				return false;
			}
			std::cout << err << std::endl;
			// everything else is fatal error, so disconnect
			disconnect();
			return true;
		}
	};
#else
#error "Sockets not yet supported on this platform!"
#endif

	//////////////////////////////////////////////////////////////////////////

	TcpSocket::TcpSocket() : impl( 0 ) {
	}

	TcpSocket::TcpSocket( TcpSocketImpl * impl ) : impl ( impl ) {
	}

	TcpSocket::~TcpSocket() {
		destroy();
	}

	void TcpSocket::init() {
		// instantiate different ones for different platforms
#ifdef _WIN32
		impl = new TcpSocketWin32;
#else
		// todo: linux
#endif
	}

	void TcpSocket::destroy() {
		delete impl;
		impl = NULL;
	}

	// server functions
	bool TcpSocket::listen( unsigned short port ) {
		return impl && impl->listen( port );
	}

	bool TcpSocket::accept( TcpSocket & socketOut ) {
		return impl && impl->accept( socketOut );
	}

	// client functions
	bool TcpSocket::connect( const char * ip, unsigned short port ) {
		return impl && impl->connect( ip, port );
	}

	// shared
	TcpSocket::SocketError TcpSocket::getSocketError() const {
		return impl ? impl->getSocketError() : SocketDisconnected;
	}

	bool TcpSocket::isConnected() const {
		return impl && impl->isConnected();
	}

	void TcpSocket::disconnect( SocketError setError ) {
		if ( impl ) {
			impl->disconnect( setError );
		}
	}

	bool TcpSocket::sendData( int & bytesSent, const char * buf, int bytes ) {
		return impl && impl->sendData( bytesSent, buf, bytes );
	}

	bool TcpSocket::rcvData( int & bytesRead, char * buf, const int bufSize ) {
		return impl && impl->rcvData( bytesRead, buf, bufSize );
	}

	void TcpSocket::setBufferSizes( int sendSize, int rcvSize ) {
		if ( impl ) {
			impl->setBufferSizes( sendSize, rcvSize );
		}
	}

	const char * TcpSocket::getPeerName() {
		return impl ? impl->getPeerName() : NULL;
	}

	bool TcpSocket::setNonBlocking( bool enableNonBlocking ) {
		return impl && impl->setNonBlocking( enableNonBlocking );
	}

	void TcpSocket::swap( TcpSocket & sock ) {
		TcpSocketImpl * tmp = impl;
		this->impl = sock.impl;
		sock.impl = tmp;
	}

	//////////////////////////////////////////////////////////////////////////
	// global init/shutdowns

	bool TcpSocket::initSockets() {
#ifdef WIN32
		WSADATA wsaData;
		if ( WSAStartup( MAKEWORD(2,2), &wsaData ) != NO_ERROR ) {
			return false;
		}
#else
#endif
		return true;
	}

	bool TcpSocket::shutdownSockets() {
#ifdef WIN32
		WSACleanup();
#endif
		return true;
	}

	//////////////////////////////////////////////////////////////////////////
};