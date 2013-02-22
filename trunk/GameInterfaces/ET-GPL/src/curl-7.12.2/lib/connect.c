/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2004, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at http://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 * $Id: connect.c,v 1.118 2004/10/11 17:23:41 bagder Exp $
 ***************************************************************************/

#include "setup.h"

#ifndef WIN32
/* headers for non-win32 */
#include <sys/time.h>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h> /* <netinet/tcp.h> may need it */
#endif
#ifdef HAVE_NETINET_TCP_H
#include <netinet/tcp.h> /* for TCP_NODELAY */
#endif
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h> /* required for free() prototype, without it, this crashes
					   on macos 68K */
#endif
#if ( defined( HAVE_FIONBIO ) && defined( __NOVELL_LIBC__ ) )
#include <sys/filio.h>
#endif
#if ( defined( NETWARE ) && defined( __NOVELL_LIBC__ ) )
#undef in_addr_t
#define in_addr_t unsigned long
#endif
#ifdef VMS
#include <in.h>
#include <inet.h>
#endif

#endif
#include <stdio.h>
#include <errno.h>
#include <string.h>

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#ifdef WIN32
#include <windows.h>
#define EINPROGRESS WSAEINPROGRESS
#define EWOULDBLOCK WSAEWOULDBLOCK
#define EISCONN     WSAEISCONN
#define ENOTSOCK    WSAENOTSOCK
#define ECONNREFUSED WSAECONNREFUSED
#endif

#include "urldata.h"
#include "sendf.h"
#include "if2ip.h"
#include "strerror.h"
#include "connect.h"
#include "memory.h"

/* The last #include file should be: */
#include "memdebug.h"

static bool verifyconnect( curl_socket_t sockfd, int *error );

static curl_socket_t
singleipconnect( struct connectdata *conn,
				 Curl_addrinfo *ai, /* start connecting to this */
				 long timeout_ms,
				 bool *connected );

/*
 * Curl_ourerrno() returns the errno (or equivalent) on this platform to
 * hide platform specific for the function that calls this.
 */
int Curl_ourerrno( void ) {
#ifdef WIN32
	return (int)GetLastError();
#else
	return errno;
#endif
}

/*
 * Curl_nonblock() set the given socket to either blocking or non-blocking
 * mode based on the 'nonblock' boolean argument. This function is highly
 * portable.
 */
int Curl_nonblock( curl_socket_t sockfd,    /* operate on this */
				   int nonblock /* TRUE or FALSE */ ) {
#undef SETBLOCK
#ifdef HAVE_O_NONBLOCK
	/* most recent unix versions */
	int flags;

	flags = fcntl( sockfd, F_GETFL, 0 );
	if ( TRUE == nonblock ) {
		return fcntl( sockfd, F_SETFL, flags | O_NONBLOCK );
	} else {
		return fcntl( sockfd, F_SETFL, flags & ( ~O_NONBLOCK ) );
	}
#define SETBLOCK 1
#endif

#ifdef HAVE_FIONBIO
	/* older unix versions */
	int flags;

	flags = nonblock;
	return ioctl( sockfd, FIONBIO, &flags );
#define SETBLOCK 2
#endif

#ifdef HAVE_IOCTLSOCKET
	/* Windows? */
	unsigned long flags;
	flags = nonblock;
	return ioctlsocket( sockfd, FIONBIO, &flags );
#define SETBLOCK 3
#endif

#ifdef HAVE_IOCTLSOCKET_CASE
	/* presumably for Amiga */
	return IoctlSocket( sockfd, FIONBIO, (long)nonblock );
#define SETBLOCK 4
#endif

#ifdef HAVE_SO_NONBLOCK
	/* BeOS */
	long b = nonblock ? 1 : 0;
	return setsockopt( sockfd, SOL_SOCKET, SO_NONBLOCK, &b, sizeof( b ) );
#define SETBLOCK 5
#endif

#ifdef HAVE_DISABLED_NONBLOCKING
	return 0; /* returns success */
#define SETBLOCK 6
#endif

#ifndef SETBLOCK
#error "no non-blocking method was found/used/set"
#endif
}

/*
 * waitconnect() waits for a TCP connect on the given socket for the specified
 * number if milliseconds. It returns:
 * 0    fine connect
 * -1   select() error
 * 1    select() timeout
 * 2    select() returned with an error condition fd_set
 */

#define WAITCONN_CONNECTED     0
#define WAITCONN_SELECT_ERROR -1
#define WAITCONN_TIMEOUT       1
#define WAITCONN_FDSET_ERROR   2

static
int waitconnect( curl_socket_t sockfd, /* socket */
				 long timeout_msec ) {
	fd_set fd;
	fd_set errfd;
	struct timeval interval;
	int rc;
#ifdef mpeix
	/* Call this function once now, and ignore the results. We do this to
	   "clear" the error state on the socket so that we can later read it
	   reliably. This is reported necessary on the MPE/iX operating system. */
	verifyconnect( sockfd, NULL );
#endif

	/* now select() until we get connect or timeout */
	FD_ZERO( &fd );
	FD_SET( sockfd, &fd );

	FD_ZERO( &errfd );
	FD_SET( sockfd, &errfd );

	interval.tv_sec = (int)( timeout_msec / 1000 );
	timeout_msec -= interval.tv_sec * 1000;

	interval.tv_usec = timeout_msec * 1000;

	rc = select( sockfd + 1, NULL, &fd, &errfd, &interval );
	if ( -1 == rc ) {
		/* error, no connect here, try next */
		return WAITCONN_SELECT_ERROR;
	} else if ( 0 == rc )   {
		/* timeout, no connect today */
		return WAITCONN_TIMEOUT;
	}

	if ( FD_ISSET( sockfd, &errfd ) ) {
		/* error condition caught */
		return WAITCONN_FDSET_ERROR;
	}

	/* we have a connect! */
	return WAITCONN_CONNECTED;
}

static CURLcode bindlocal( struct connectdata *conn,
						   curl_socket_t sockfd ) {
#ifdef HAVE_INET_NTOA
	bool bindworked = FALSE;
	struct SessionHandle *data = conn->data;

	/*************************************************************
	 * Select device to bind socket to
	 *************************************************************/
	if ( strlen( data->set.device ) < 255 ) {
		struct Curl_dns_entry *h = NULL;
		size_t size;
		char myhost[256] = "";
		in_addr_t in;
		int rc;
		bool was_iface = FALSE;

		/* First check if the given name is an IP address */
		in = inet_addr( data->set.device );

		if ( ( in == CURL_INADDR_NONE ) &&
			 Curl_if2ip( data->set.device, myhost, sizeof( myhost ) ) ) {
			/*
			 * We now have the numerical IPv4-style x.y.z.w in the 'myhost' buffer
			 */
			rc = Curl_resolv( conn, myhost, 0, &h );
			if ( rc == CURLRESOLV_PENDING ) {
				(void)Curl_wait_for_resolv( conn, &h );
			}

			if ( h ) {
				was_iface = TRUE;
			}
		}

		if ( !was_iface ) {
			/*
			 * This was not an interface, resolve the name as a host name
			 * or IP number
			 */
			rc = Curl_resolv( conn, data->set.device, 0, &h );
			if ( rc == CURLRESOLV_PENDING ) {
				(void)Curl_wait_for_resolv( conn, &h );
			}

			if ( h ) {
				/* we know data->set.device is shorter than the myhost array */
				strcpy( myhost, data->set.device );
			}
		}

		if ( !*myhost ) {
			/* need to fix this
			   h=Curl_gethost(data,
			   getmyhost(*myhost,sizeof(myhost)),
			   hostent_buf,
			   sizeof(hostent_buf));
			*/
			failf( data, "Couldn't bind to '%s'", data->set.device );
			return CURLE_HTTP_PORT_FAILED;
		}

		infof( data, "We bind local end to %s\n", myhost );

#ifdef SO_BINDTODEVICE
		/* I am not sure any other OSs than Linux that provide this feature, and
		 * at the least I cannot test. --Ben
		 *
		 * This feature allows one to tightly bind the local socket to a
		 * particular interface.  This will force even requests to other local
		 * interfaces to go out the external interface.
		 *
		 */
		if ( was_iface ) {
			/* Only bind to the interface when specified as interface, not just as a
			 * hostname or ip address.
			 */
			if ( setsockopt( sockfd, SOL_SOCKET, SO_BINDTODEVICE,
							 data->set.device, strlen( data->set.device ) + 1 ) != 0 ) {
				/* printf("Failed to BINDTODEVICE, socket: %d  device: %s error: %s\n",
				   sockfd, data->set.device, Curl_strerror(Curl_ourerrno())); */
				infof( data, "SO_BINDTODEVICE %s failed\n",
					   data->set.device );
				/* This is typically "errno 1, error: Operation not permitted" if
				   you're not running as root or another suitable privileged user */
			}
		}
#endif

		in = inet_addr( myhost );
		if ( CURL_INADDR_NONE != in ) {

			if ( h ) {
				Curl_addrinfo *addr = h->addr;

				Curl_resolv_unlock( data, h );
				/* we don't need it anymore after this function has returned */

				if ( bind( sockfd, addr->ai_addr, (socklen_t)addr->ai_addrlen ) >= 0 ) {
					/* we succeeded to bind */
#ifdef ENABLE_IPV6
					struct sockaddr_in6 add;
#else
					struct sockaddr_in add;
#endif

					bindworked = TRUE;

					size = sizeof( add );
					if ( getsockname( sockfd, (struct sockaddr *) &add,
									  (socklen_t *)&size ) < 0 ) {
						failf( data, "getsockname() failed" );
						return CURLE_HTTP_PORT_FAILED;
					}
				}

				if ( !bindworked ) {
					data->state.os_errno = Curl_ourerrno();
					failf( data, "%s", Curl_strerror( conn, data->state.os_errno ) );
					return CURLE_HTTP_PORT_FAILED;
				}

			} /* end of if  h */
			else {
				failf( data,"could't find my own IP address (%s)", myhost );
				return CURLE_HTTP_PORT_FAILED;
			}
		} /* end of inet_addr */
		else {
			failf( data, "could't find my own IP address (%s)", myhost );
			return CURLE_HTTP_PORT_FAILED;
		}

		return CURLE_OK;

	} /* end of device selection support */
#endif /* end of HAVE_INET_NTOA */

	return CURLE_HTTP_PORT_FAILED;
}

/*
 * verifyconnect() returns TRUE if the connect really has happened.
 */
static bool verifyconnect( curl_socket_t sockfd, int *error ) {
	bool rc = TRUE;
#ifdef SO_ERROR
	int err = 0;
	socklen_t errSize = sizeof( err );

#ifdef WIN32
	/*
	 * In October 2003 we effectively nullified this function on Windows due to
	 * problems with it using all CPU in multi-threaded cases.
	 *
	 * In May 2004, we bring it back to offer more info back on connect failures.
	 * Gisle Vanem could reproduce the former problems with this function, but
	 * could avoid them by adding this SleepEx() call below:
	 *
	 *    "I don't have Rational Quantify, but the hint from his post was
	 *    ntdll::NtRemoveIoCompletion(). So I'd assume the SleepEx (or maybe
	 *    just Sleep(0) would be enough?) would release whatever
	 *    mutex/critical-section the ntdll call is waiting on.
	 *
	 *    Someone got to verify this on Win-NT 4.0, 2000."
	 */
	SleepEx( 0, FALSE );
#endif

	if ( -1 == getsockopt( sockfd, SOL_SOCKET, SO_ERROR,
						   (void *)&err, &errSize ) ) {
		err = Curl_ourerrno();
	}

	if ( ( 0 == err ) || ( EISCONN == err ) ) {
		/* we are connected, awesome! */
		rc = TRUE;
	} else {
		/* This wasn't a successful connect */
		rc = FALSE;
	}
	if ( error ) {
		*error = err;
	}
#else
	(void)sockfd;
	if ( error ) {
		*error = Curl_ourerrno();
	}
#endif
	return rc;
}

/* Used within the multi interface. Try next IP address, return TRUE if no
   more address exists */
static bool trynextip( struct connectdata *conn,
					   int sockindex,
					   bool *connected ) {
	curl_socket_t sockfd;
	Curl_addrinfo *ai;

	if ( sockindex != FIRSTSOCKET ) {
		return TRUE; /* no next */

	}
	/* try the next address */
	ai = conn->ip_addr->ai_next;

	while ( ai ) {
		sockfd = singleipconnect( conn, ai, 0L, connected );
		if ( sockfd != CURL_SOCKET_BAD ) {
			/* store the new socket descriptor */
			conn->sock[sockindex] = sockfd;
			conn->ip_addr = ai;
			return FALSE;
		}
		ai = ai->ai_next;
	}
	return TRUE;
}

/*
 * Curl_is_connected() is used from the multi interface to check if the
 * firstsocket has connected.
 */

CURLcode Curl_is_connected( struct connectdata *conn,
							int sockindex,
							bool *connected ) {
	int rc;
	struct SessionHandle *data = conn->data;
	CURLcode code = CURLE_OK;
	curl_socket_t sockfd = conn->sock[sockindex];
	long allow = DEFAULT_CONNECT_TIMEOUT;
	long has_passed;

	curlassert( sockindex >= FIRSTSOCKET && sockindex <= SECONDARYSOCKET );

	*connected = FALSE; /* a very negative world view is best */

	/* Evaluate in milliseconds how much time that has passed */
	has_passed = Curl_tvdiff( Curl_tvnow(), data->progress.start );

	/* subtract the most strict timeout of the ones */
	if ( data->set.timeout && data->set.connecttimeout ) {
		if ( data->set.timeout < data->set.connecttimeout ) {
			allow = data->set.timeout * 1000;
		} else {
			allow = data->set.connecttimeout * 1000;
		}
	} else if ( data->set.timeout )      {
		allow = data->set.timeout * 1000;
	} else if ( data->set.connecttimeout )      {
		allow = data->set.connecttimeout * 1000;
	}

	if ( has_passed > allow ) {
		/* time-out, bail out, go home */
		failf( data, "Connection time-out after %ld ms", has_passed );
		return CURLE_OPERATION_TIMEOUTED;
	}
	if ( conn->bits.tcpconnect ) {
		/* we are connected already! */
		*connected = TRUE;
		return CURLE_OK;
	}

	/* check for connect without timeout as we want to return immediately */
	rc = waitconnect( sockfd, 0 );

	if ( WAITCONN_CONNECTED == rc ) {
		int error;
		if ( verifyconnect( sockfd, &error ) ) {
			/* we are connected, awesome! */
			*connected = TRUE;
			return CURLE_OK;
		}
		/* nope, not connected for real */
		data->state.os_errno = error;
		infof( data, "Connection failed\n" );
		if ( trynextip( conn, sockindex, connected ) ) {
			code = CURLE_COULDNT_CONNECT;
		}
	} else if ( WAITCONN_TIMEOUT != rc )      {
		/* nope, not connected  */
		infof( data, "Connection failed\n" );
		if ( trynextip( conn, sockindex, connected ) ) {
			int error = Curl_ourerrno();
			failf( data, "Failed connect to %s:%d; %s",
				   conn->host.name, conn->port, Curl_strerror( conn,error ) );
			code = CURLE_COULDNT_CONNECT;
		}
	}
	/*
	 * If the connection failed here, we should attempt to connect to the "next
	 * address" for the given host.
	 */

	return code;
}

static void tcpnodelay( struct connectdata *conn,
						curl_socket_t sockfd ) {
#ifdef TCP_NODELAY
	struct SessionHandle *data = conn->data;
	socklen_t onoff = (socklen_t) data->set.tcp_nodelay;
	if ( setsockopt( sockfd, IPPROTO_TCP, TCP_NODELAY, (void *)&onoff,
					 sizeof( onoff ) ) < 0 ) {
		infof( data, "Could not set TCP_NODELAY: %s\n",
			   Curl_strerror( conn, Curl_ourerrno() ) );
	} else {
		infof( data,"TCP_NODELAY set\n" );
	}
#else
	(void)conn;
	(void)sockfd;
#endif
}

#ifdef SO_NOSIGPIPE
/* The preferred method on Mac OS X (10.2 and later) to prevent SIGPIPEs when
   sending data to a dead peer (instead of relying on the 4th argument to send
   being MSG_NOSIGNAL). Possibly also existing and in use on other BSD
   systems? */
static void nosigpipe( struct connectdata *conn,
					   curl_socket_t sockfd ) {
	struct SessionHandle *data = conn->data;
	int onoff = 1;
	if ( setsockopt( sockfd, SOL_SOCKET, SO_NOSIGPIPE, (void *)&onoff,
					 sizeof( onoff ) ) < 0 ) {
		infof( data, "Could not set SO_NOSIGPIPE: %s\n",
			   Curl_strerror( conn, Curl_ourerrno() ) );
	}
}
#endif

/* singleipconnect() connects to the given IP only, and it may return without
   having connected if used from the multi interface. */
static curl_socket_t
singleipconnect( struct connectdata *conn,
				 Curl_addrinfo *ai,
				 long timeout_ms,
				 bool *connected ) {
	char addr_buf[128];
	int rc;
	int error;
	bool conected;
	struct SessionHandle *data = conn->data;
	curl_socket_t sockfd = socket( ai->ai_family, ai->ai_socktype,
								   ai->ai_protocol );
	if ( sockfd == CURL_SOCKET_BAD ) {
		return CURL_SOCKET_BAD;
	}

	*connected = FALSE; /* default is not connected */

	Curl_printable_address( ai, addr_buf, sizeof( addr_buf ) );
	infof( data, "  Trying %s... ", addr_buf );

	if ( data->set.tcp_nodelay ) {
		tcpnodelay( conn, sockfd );
	}

#ifdef SO_NOSIGPIPE
	nosigpipe( conn, sockfd );
#endif
	if ( conn->data->set.device ) {
		/* user selected to bind the outgoing socket to a specified "device"
		   before doing connect */
		CURLcode res = bindlocal( conn, sockfd );
		if ( res ) {
			return res;
		}
	}

	/* set socket non-blocking */
	Curl_nonblock( sockfd, TRUE );

	rc = connect( sockfd, ai->ai_addr, ai->ai_addrlen );

	if ( -1 == rc ) {
		error = Curl_ourerrno();

		switch ( error ) {
		case EINPROGRESS:
		case EWOULDBLOCK:
#if defined( EAGAIN ) && EAGAIN != EWOULDBLOCK
			/* On some platforms EAGAIN and EWOULDBLOCK are the
			 * same value, and on others they are different, hence
			 * the odd #if
			 */
		case EAGAIN:
#endif
			rc = waitconnect( sockfd, timeout_ms );
			break;
		default:
			/* unknown error, fallthrough and try another address! */
			failf( data, "Failed to connect to %s: %s",
				   addr_buf, Curl_strerror( conn,error ) );
			break;
		}
	}

	/* The 'WAITCONN_TIMEOUT == rc' comes from the waitconnect(), and not from
	   connect(). We can be sure of this since connect() cannot return 1. */
	if ( ( WAITCONN_TIMEOUT == rc ) &&
		 ( data->state.used_interface == Curl_if_multi ) ) {
		/* Timeout when running the multi interface */
		return sockfd;
	}

	conected = verifyconnect( sockfd, &error );

	if ( !rc && conected ) {
		/* we are connected, awesome! */
		*connected = TRUE; /* this is a true connect */
		infof( data, "connected\n" );
		return sockfd;
	} else if ( WAITCONN_TIMEOUT == rc )      {
		infof( data, "Timeout\n" );
	} else {
		data->state.os_errno = error;
		infof( data, "%s\n", Curl_strerror( conn, error ) );
	}

	/* connect failed or timed out */
	sclose( sockfd );

	return CURL_SOCKET_BAD;
}

/*
 * TCP connect to the given host with timeout, proxy or remote doesn't matter.
 * There might be more than one IP address to try out. Fill in the passed
 * pointer with the connected socket.
 */

CURLcode Curl_connecthost( struct connectdata *conn,  /* context */
						   struct Curl_dns_entry *remotehost, /* use this one */
						   curl_socket_t *sockconn,  /* the connected socket */
						   Curl_addrinfo **addr,     /* the one we used */
						   bool *connected ) {       /* really connected? */
	struct SessionHandle *data = conn->data;
	curl_socket_t sockfd = CURL_SOCKET_BAD;
	int aliasindex;
	int num_addr;
	Curl_addrinfo *ai;
	Curl_addrinfo *curr_addr;

	struct timeval after;
	struct timeval before = Curl_tvnow();

	/*************************************************************
	 * Figure out what maximum time we have left
	 *************************************************************/
	long timeout_ms = DEFAULT_CONNECT_TIMEOUT;
	long timeout_per_addr;

	*connected = FALSE; /* default to not connected */

	if ( data->set.timeout || data->set.connecttimeout ) {
		long has_passed;

		/* Evaluate in milliseconds how much time that has passed */
		has_passed = Curl_tvdiff( Curl_tvnow(), data->progress.start );

#ifndef min
#define min( a, b )   ( ( a ) < ( b ) ? ( a ) : ( b ) )
#endif

		/* get the most strict timeout of the ones converted to milliseconds */
		if ( data->set.timeout && data->set.connecttimeout ) {
			if ( data->set.timeout < data->set.connecttimeout ) {
				timeout_ms = data->set.timeout * 1000;
			} else {
				timeout_ms = data->set.connecttimeout * 1000;
			}
		} else if ( data->set.timeout )      {
			timeout_ms = data->set.timeout * 1000;
		} else {
			timeout_ms = data->set.connecttimeout * 1000;
		}

		/* subtract the passed time */
		timeout_ms -= has_passed;

		if ( timeout_ms < 0 ) {
			/* a precaution, no need to continue if time already is up */
			failf( data, "Connection time-out" );
			return CURLE_OPERATION_TIMEOUTED;
		}
	}

	/* Max time for each address */
	num_addr = Curl_num_addresses( remotehost->addr );
	timeout_per_addr = timeout_ms / num_addr;

	ai = remotehost->addr;

	/* Below is the loop that attempts to connect to all IP-addresses we
	 * know for the given host. One by one until one IP succeeds.
	 */

	if ( data->state.used_interface == Curl_if_multi ) {
		/* don't hang when doing multi */
		timeout_per_addr = timeout_ms = 0;
	}

	/*
	 * Connecting with a Curl_addrinfo chain
	 */
	for ( curr_addr = ai, aliasindex = 0; curr_addr;
		  curr_addr = curr_addr->ai_next, aliasindex++ ) {

		/* start connecting to the IP curr_addr points to */
		sockfd = singleipconnect( conn, curr_addr, timeout_per_addr, connected );

		if ( sockfd != CURL_SOCKET_BAD ) {
			break;
		}

		/* get a new timeout for next attempt */
		after = Curl_tvnow();
		timeout_ms -= Curl_tvdiff( after, before );
		if ( timeout_ms < 0 ) {
			failf( data, "connect() timed out!" );
			return CURLE_OPERATION_TIMEOUTED;
		}
		before = after;
	} /* end of connect-to-each-address loop */

	if ( sockfd == CURL_SOCKET_BAD ) {
		/* no good connect was made */
		*sockconn = CURL_SOCKET_BAD;
		failf( data, "couldn't connect to host" );
		return CURLE_COULDNT_CONNECT;
	}

	/* leave the socket in non-blocking mode */

	/* store the address we use */
	if ( addr ) {
		*addr = curr_addr;
	}

	/* allow NULL-pointers to get passed in */
	if ( sockconn ) {
		*sockconn = sockfd; /* the socket descriptor we've connected */

	}
	return CURLE_OK;
}
