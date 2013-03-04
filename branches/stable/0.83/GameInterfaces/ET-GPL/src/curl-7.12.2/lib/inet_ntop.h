#ifndef __INET_NTOP_H
#define __INET_NTOP_H
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
 * $Id: inet_ntop.h,v 1.1 2004/04/26 07:20:11 bagder Exp $
 ***************************************************************************/

#include "setup.h"

#ifdef HAVE_INET_NTOP
#define Curl_inet_ntop( af,addr,buf,size ) inet_ntop( af,addr,buf,size )
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#else
const char *Curl_inet_ntop( int af, const void *addr, char *buf, size_t size );
#endif

#endif /* __INET_NTOP_H */
