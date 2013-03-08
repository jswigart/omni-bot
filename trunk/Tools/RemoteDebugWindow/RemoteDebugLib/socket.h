#pragma once

namespace RemoteLib
{
	class TcpSocketImpl;

	class TcpSocket
	{
	public:
		friend class TcpSocketWin32;

		enum SocketError {
			SocketOk,
			SocketDisconnected,
		};

		enum { DefaultPort = 53000 };

		// global init/shutdowns
		static bool initSockets();
		static bool shutdownSockets();

		// socket init/shutdown
		void init();
		void destroy();

		bool isValid() const { return impl != 0; }

		// server functions
		bool listen( unsigned short port = DefaultPort );
		bool accept( TcpSocket & acceptSocket );

		// client functions
		bool connect( const char * ip = "127.0.0.1", unsigned short port = DefaultPort );

		// shared
		SocketError getSocketError() const;
		bool isConnected() const;
		void disconnect( SocketError setError = TcpSocket::SocketDisconnected );

		bool sendData( int & bytesSent, const char * buf, int bytes );
		bool rcvData( int & bytesRead, char * buf, const int bufSize );

		void setBufferSizes( int sendSize, int rcvSize );
		bool setNonBlocking( bool enableNonBlocking );

		const char * getPeerName();

		void swap( TcpSocket & sock );

		TcpSocket & operator=( const TcpSocket & socket ); // not allowed

		TcpSocket();
		TcpSocket( TcpSocketImpl * impl );
		virtual ~TcpSocket();
	protected:
		TcpSocketImpl * impl;
	};
};
