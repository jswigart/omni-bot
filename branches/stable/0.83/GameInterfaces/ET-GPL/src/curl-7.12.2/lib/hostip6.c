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
 * $Id: hostip6.c,v 1.10 2004/10/06 07:50:18 bagder Exp $
 ***************************************************************************/

#include "setup.h"

#include <string.h>
#include <errno.h>

#define _REENTRANT

#if defined( WIN32 ) && !defined( __GNUC__ ) || defined( __MINGW32__ )
#include <malloc.h>
#else
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>     /* required for free() prototypes */
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>     /* for the close() proto */
#endif
#ifdef  VMS
#include <in.h>
#include <inet.h>
#include <stdlib.h>
#endif
#endif

#ifdef HAVE_SETJMP_H
#include <setjmp.h>
#endif

#ifdef WIN32
#include <process.h>
#endif

#if ( defined( NETWARE ) && defined( __NOVELL_LIBC__ ) )
#undef in_addr_t
#define in_addr_t unsigned long
#endif

#include "urldata.h"
#include "sendf.h"
#include "hostip.h"
#include "hash.h"
#include "share.h"
#include "strerror.h"
#include "url.h"

#define _MPRINTF_REPLACE /* use our functions only */
#include <curl/mprintf.h>

#if defined( HAVE_INET_NTOA_R ) && !defined( HAVE_INET_NTOA_R_DECL )
#include "inet_ntoa_r.h"
#endif

#include "memory.h"
/* The last #include file should be: */
#include "memdebug.h"

/***********************************************************************
 * Only for ipv6-enabled builds
 **********************************************************************/
#ifdef CURLRES_IPV6
/*
 * This is a wrapper function for freeing name information in a protocol
 * independent way. This takes care of using the appropriate underlaying
 * function.
 */
void Curl_freeaddrinfo( Curl_addrinfo *p ) {
	freeaddrinfo( p );
}

#ifdef CURLRES_ASYNCH
/*
 * Curl_addrinfo_copy() is used by the asynch callback to copy a given
 * address. But this is an ipv6 build and then we don't copy the address, we
 * just return the same pointer!
 */
Curl_addrinfo *Curl_addrinfo_copy( void *source, int port ) {
	(void) port;
	return source;
}
#endif

#ifdef CURLDEBUG
/* These are strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I didn't wanna include in memdebug.c
 */
int curl_dogetaddrinfo( char *hostname, char *service,
						struct addrinfo *hints,
						struct addrinfo **result,
						int line, const char *source ) {
	int res = (getaddrinfo)( hostname, service, hints, result );
	if ( 0 == res ) {
		/* success */
		if ( logfile ) {
			fprintf( logfile, "ADDR %s:%d getaddrinfo() = %p\n",
					 source, line, (void *)*result );
		}
	} else {
		if ( logfile ) {
			fprintf( logfile, "ADDR %s:%d getaddrinfo() failed\n",
					 source, line );
		}
	}
	return res;
}

int curl_dogetnameinfo( const struct sockaddr *sa, socklen_t salen,
						char *host, size_t hostlen,
						char *serv, size_t servlen, int flags,
						int line, const char *source ) {
	int res = (getnameinfo)( sa, salen, host, hostlen, serv, servlen, flags );
	if ( 0 == res ) {
		/* success */
		if ( logfile ) {
			fprintf( logfile, "GETNAME %s:%d getnameinfo()\n",
					 source, line );
		}
	} else {
		if ( logfile ) {
			fprintf( logfile, "GETNAME %s:%d getnameinfo() failed = %d\n",
					 source, line, res );
		}
	}
	return res;
}

void curl_dofreeaddrinfo( struct addrinfo *freethis,
						  int line, const char *source ) {
	(freeaddrinfo)( freethis );
	if ( logfile ) {
		fprintf( logfile, "ADDR %s:%d freeaddrinfo(%p)\n",
				 source, line, (void *)freethis );
	}
}

#endif

/*
 * Curl_ipvalid() checks what CURL_IPRESOLVE_* requirements that might've
 * been set and returns TRUE if they are OK.
 */
bool Curl_ipvalid( struct SessionHandle *data ) {
	if ( data->set.ip_version == CURL_IPRESOLVE_V6 ) {
		/* see if we have an IPv6 stack */
		curl_socket_t s = socket( PF_INET6, SOCK_DGRAM, 0 );
		if ( s == CURL_SOCKET_BAD ) {
			/* an ipv6 address was requested and we can't get/use one */
			return FALSE;
		}
		sclose( s );
	}
	return TRUE;
}

#ifndef USE_THREADING_GETADDRINFO
/*
 * Curl_getaddrinfo() when built ipv6-enabled (non-threading version).
 *
 * Returns name information about the given hostname and port number. If
 * successful, the 'addrinfo' is returned and the forth argument will point to
 * memory we need to free after use. That memory *MUST* be freed with
 * Curl_freeaddrinfo(), nothing else.
 */
Curl_addrinfo *Curl_getaddrinfo( struct connectdata *conn,
								 char *hostname,
								 int port,
								 int *waitp ) {
	struct addrinfo hints, *res;
	int error;
	char sbuf[NI_MAXSERV];
	char addrbuf[128];
	curl_socket_t s;
	int pf;
	struct SessionHandle *data = conn->data;
	int ai_flags;

	*waitp = 0; /* don't wait, we have the response now */

	/* see if we have an IPv6 stack */
	s = socket( PF_INET6, SOCK_DGRAM, 0 );
	if ( s < 0 ) {
		/* Some non-IPv6 stacks have been found to make very slow name resolves
		 * when PF_UNSPEC is used, so thus we switch to a mere PF_INET lookup if
		 * the stack seems to be a non-ipv6 one. */

		pf = PF_INET;
	} else {
		/* This seems to be an IPv6-capable stack, use PF_UNSPEC for the widest
		 * possible checks. And close the socket again.
		 */
		sclose( s );

		/*
		 * Check if a more limited name resolve has been requested.
		 */
		switch ( data->set.ip_version ) {
		case CURL_IPRESOLVE_V4:
			pf = PF_INET;
			break;
		case CURL_IPRESOLVE_V6:
			pf = PF_INET6;
			break;
		default:
			pf = PF_UNSPEC;
			break;
		}
	}

	if ( ( 1 == inet_pton( AF_INET, hostname, addrbuf ) ) ||
		 ( 1 == inet_pton( AF_INET6, hostname, addrbuf ) ) ) {
		/* the given address is numerical only, prevent a reverse lookup */
		ai_flags = AI_NUMERICHOST;
	} else {
		ai_flags = AI_CANONNAME;
	}

	memset( &hints, 0, sizeof( hints ) );
	hints.ai_family = pf;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = ai_flags;
	snprintf( sbuf, sizeof( sbuf ), "%d", port );
	error = getaddrinfo( hostname, sbuf, &hints, &res );
	if ( error ) {
		infof( data, "getaddrinfo(3) failed for %s:%d\n", hostname, port );
		return NULL;
	}

	return res;
}
#endif /* USE_THREADING_GETADDRINFO */
#endif /* ipv6 */

