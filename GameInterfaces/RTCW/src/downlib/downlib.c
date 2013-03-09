/*

downlib
by Luigi Auriemma
e-mail: aluigi@autistici.org
web:    aluigi.org

Copyright 2006,2007,2008,2009,2010 Luigi Auriemma

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

http://www.gnu.org/licenses/gpl-2.0.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <time.h>
#include <ctype.h>

#include "downlib.h"

#ifdef WIN32
#include <winsock.h>

#define close       closesocket
#define in_addr_t   u32
#define TEMPOZ1
#define TEMPOZ2     GetTickCount()
#define ONESEC      1000
#pragma warning(disable : 4018)
#pragma warning(disable : 4244)
#pragma warning(disable : 4996)
#else
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/times.h>
#include <sys/timeb.h>

#define stristr     strcasestr
#define stricmp     strcasecmp
#define strnicmp    strncasecmp
#define TEMPOZ1     ftime(&timex)
#define TEMPOZ2     ((timex.time * 1000) + timex.millitm)
#define ONESEC      1
#endif

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;

#define VISDELAY    500
#define BUFFSZ      8192
#define MAXTIMEOUT  60          // one minute
#define MAXARGS     16          // modify it if you need more args in mydown_scanhead
#define MAXDNS      32
#define VERPRINTF   if(verbose >= 0) fprintf(stderr,
#define VERPRINTF2  if(verbose > 0)  fprintf(stderr,
#define TEMPOZ(x)   TEMPOZ1;    \
	x = TEMPOZ2

typedef struct {                // lame DNS caching implementation
	u8          *host;
	in_addr_t   ip;
} dns_db_t;

int         dns_db_max      = 0,
	dns_db_add      = 0;
dns_db_t    dns_db[MAXDNS];

void mydown_get_host(u8 *url, u8 **hostx, u16 *portx, u8 **urix, int verbose);
u8 *mydown_uri2hex(u8 *uri);
u8 *mydown_hex2uri(u8 *uri);
void mydown_scanhead(u8 *data, int datalen, ...);
u8 *mydown_tmpnam(void);
void mydown_free_sock(int *sock);
u8 *mydown_http_skip(u8 *buff, int len, int *needed, int *remain);
void mydown_free(u8 **buff);
int mydown_chunked_skip(u8 *buff, int chunkedsize);
int mydown_sscanf_hex(u8 *data, int datalen);
int mydown_timeout(int sock, int secs);
int mydown_recv(int sd, u8 *data, int len, int timeout);
u8 *mydown_showhttp80(u16 port);
void mydown_showstatus(u32 fsize, u32 ret, u32 oldret, int timediff);
u8 *mydown_base64_encode(u8 *data, int *length);
in_addr_t mydown_resolv(char *host);

u32 mydown(u8 *myurl, u8 *filename, mydown_options *opt) {
	FILE    *fd          = NULL;
	u32     from         = 0,
		tot          = 0,
		filesize     = MYDOWN_ERROR;
	int     showhead     = 0,
		resume       = 0,
		onlyifdiff   = 0,
		verbose      = 0,
		*keep_alive  = 0,
		*ret_code    = 0,
		timeout      = 0;
	u16     port         = 0;
	u8      *url         = NULL,
		*uri         = NULL,
		*host        = NULL,
		*useragent   = NULL,
		**filedata   = NULL;

#ifdef WIN32
	WSADATA wsadata;
	WSAStartup(MAKEWORD(1,0), &wsadata);
#endif

	setbuf(stdout, NULL);
	setbuf(stderr, NULL);

	if(opt) {
		from         = opt->from;
		tot          = opt->tot;
		showhead     = opt->showhead;
		resume       = opt->resume;
		onlyifdiff   = opt->onlyifdiff;
		useragent    = opt->useragent;
		verbose      = opt->verbose;
		filedata     = opt->filedata;
		keep_alive   = opt->keep_alive;
		timeout      = opt->timeout;
		ret_code     = opt->ret_code;
	}

	if(!myurl) return(MYDOWN_ERROR);
	url = strdup(myurl);
	if(!url) return(MYDOWN_ERROR);
	mydown_get_host(url, &host, &port, &uri, verbose);

	VERPRINTF2"  start download\n");

	filesize = mydown_http2file(
		keep_alive,         // int *sock
		timeout,            // int timeout
		host,               // u8 *host
		port,               // u16 port
		useragent,          // u8 *useragent
		verbose,            // int verbose
		uri,                // u8 *getstr
		fd,                 // FILE *fd
		filename,           // u8 *filename
		showhead,           // int showhead
		onlyifdiff,         // int onlyifdiff
		resume,             // int resume
		from,               // u32 from
		tot,                // u32 tot
		NULL,               // u32 *filesize
		filedata,           // u8 **filedata
		ret_code           // int *ret_code
		);

	if(fd && (fd != stdout)) fclose(fd);
	if(url) free(url);
	if(uri) free(uri);
	return(filesize);
}

void mydown_get_host(u8 *url, u8 **hostx, u16 *portx, u8 **urix, int verbose) {
	u16     port  = 80;
	u8      *host = NULL,
		*uri  = NULL,
		*p;

	if(!url) goto quit;
	host = url;

	p = strstr(host, "://");    // handle http://
	if(!p) p = strstr(host, ":\\\\");
	if(p) {
		for(p += 3; *p; p++) {  // in case of http:////
			if((*p != '/') && (*p != '\\')) break;
		}
		host = p;
	}

	for(p = host; *p; p++) {    // search the uri
		if((*p == '/') || (*p == '\\')) {
			uri = p;
			break;
		}
	}
	if(uri) {
		*uri++ = 0;
		uri = mydown_uri2hex(uri);
	}
	if(!uri) uri = strdup("");  // in case of no uri or mydown_uri2hex fails

	p = strchr(host, ':');
	if(p) {
		*p = 0;
		port = atoi(p + 1);
	}

	VERPRINTF"  %s\n", url);
	VERPRINTF2
		"  host   %s : %hu\n"
		"  uri    %s\n",
		host, port,
		uri);

quit:
	*hostx = host;
	*portx = port;
	*urix  = uri;
}

u8 *mydown_http_delimit(u8 *data, u8 *limit) {
	u8      *p;

	if(!data || !data[0]) return(NULL);

	for(p = data;; p++) {
		if(p >= limit) return(NULL);
		if(!*p || (*p == '\r') || (*p == '\n')) break;
	}
	*p = 0;
	for(p++;; p++) {
		if(p >= limit) return(NULL);
		if(!*p) break;
		if((*p != '\r') && (*p != '\n')) break;
	}
	return(p);
}

u8 *mydown_uri2hex(u8 *uri) {
	static const u8 hex[16] = "0123456789abcdef";
	u8      *ret,
		*p,
		c;

	if(!uri) return(NULL);
	ret = malloc((strlen(uri) * 3) + 1);
	if(!ret) return(NULL);

	for(p = ret; *uri; uri++) {
		c = *uri;
		if(isprint(c) && !strchr(" \"<>#" /*"%\\"*/ "{}|^~[]`", c)) {   // I have left the backslash and the percentage out
			*p++ = c;
		} else {
			*p++ = '%';
			*p++ = hex[c >> 4];
			*p++ = hex[c & 15];
		}
	}
	*p = 0;

	return(ret);
}

u8 *mydown_hex2uri(u8 *uri) {
	int     t,
		n;
	u8      *ret,
		*p;

	if(!uri) return(NULL);
	ret = strdup(uri);
	if(!ret) return(NULL);

	for(p = ret; *uri; uri++, p++) {
		t = *uri;
		if((*uri == '%') && (*(uri + 1) == 'u')) {
			if(sscanf(uri + 1, "u%04x", &t) == 1) uri += 5;
		} else if(*uri == '%') {
			if(sscanf(uri + 1, "%02x",  &t) == 1) uri += 2;
		} else if(*uri == '&') {
			if(*(uri + 1) == '#') {
				if((sscanf(uri + 1, "#%d;%n", &t, &n) == 1) && (n > 2)) {
					uri += n;
				} else {
					t = *uri;
				}
			} else if(!strnicmp(uri, "&quot;",   6)) {
				t = '\"';
				uri += 5;
			} else if(!strnicmp(uri, "&amp;",    5)) {
				t = '&';
				uri += 4;
			} else if(!strnicmp(uri, "&frasl;",  7)) {
				t = '/';
				uri += 6;
			} else if(!strnicmp(uri, "&lt;",     4)) {
				t = '<';
				uri += 3;
			} else if(!strnicmp(uri, "&gt;",     4)) {
				t = '>';
				uri += 3;
			} else if(!strnicmp(uri, "&nbsp;",   6)) {
				t = 160;
				uri += 5;
			} else if(!strnicmp(uri, "&middot;", 8)) {
				t = 183;
				uri += 7;
			}
		}
		*p = t;
	}
	*p = 0;

	return(ret);
}

void mydown_scanhead(u8 *data, int datalen, ...) {
	va_list ap;
	int     i,
		vals;
	u8      *par[MAXARGS],
		**val[MAXARGS],
		*l,
		*p,
		*limit;

	if(!data) return;
	if(datalen <= 0) return;

	va_start(ap, datalen);
	for(i = 0; i < MAXARGS; i++) {
		par[i] = va_arg(ap, u8 *);
		if(!par[i]) break;
		val[i] = va_arg(ap, u8 **);
		if(!val[i]) break;
		*val[i] = NULL;
	}
	vals = i;
	va_end(ap);

	for(limit = data + datalen; ; data = l) {
		l = mydown_http_delimit(data, limit);
		p = strchr(data, ':');
		if(!p) continue;
		*p++ = 0;
		for(i = 0; i < vals; i++) {
			if(stricmp(data, par[i])) continue;
			while(*p && ((*p == ' ') || (*p == '\t'))) p++;
			*val[i] = p;
			break;
		}
		if(!l) break;
	}
}

// fname is provided by the user or from the network, the cleaning here is intended only
// about the bad chars not supported by the file-system, it's not a security function
void mydown_filename_clean(u8 *fname) {
	u8      *p;

	if(!fname) return;
	p = strchr(fname, ':'); // the first ':' is allowed
	if(p) fname = p + 1;
	for(p = fname; *p && (*p != '\r') && (*p != '\n'); p++) {
		if(strchr("?%*:|\"<>", *p)) {   // invalid filename chars not supported by the most used file systems
			*p = '_';
		}
	}
	*p = 0;
	for(p--; (p >= fname) && ((*p == ' ') || (*p == '.')); p--) *p = 0;   // remove final spaces and dots
}

u32 mydown_http2file(int *sock, int timeout, u8 *host, u16 port, u8 *useragent, int verbose, u8 *getstr, FILE *fd, u8 *filename, int showhead, int onlyifdiff, int resume, u32 from, u32 tot, u32 *filesize, u8 **filedata, int *ret_code) {
#ifndef WIN32
	struct  timeb   timex;
#endif
	FILE    *oldfd          = fd;
	struct  sockaddr_in peer;
	struct  stat    xstat;
	time_t  oldtime         = 0,
		newtime;
	u32     ret             = 0,
		httpret         = 0,
		vishttpret      = 0,
		fsize           = 0;
	int     sd              = 0,
		t,
		err,
		len,
		code            = 0,
		filedatasz      = 0,
		chunked         = 0,
		chunkedsize     = 0,
		chunkedlen      = 0,
		zbufflen        = 0,
		httpskipbytes   = 0;
	u8      *buff           = NULL,
		*query          = NULL,
		*data           = NULL,
		*header_limit   = NULL,
		*s              = NULL,
		*b64            = NULL,
		*conttype       = NULL,
		*contlen        = NULL,
		*contdisp       = NULL,
		*icyname        = NULL,
		*transenc       = NULL,
		*contenc        = NULL,
		*connection     = NULL,
		*location       = NULL,
		*filedatatmp    = NULL,
		*zbuff          = NULL,
		*ztmp           = NULL,
		*chunkedbuff    = NULL,
		*chunkedtmp     = NULL,
		*filenamemalloc = NULL,
		httpgzip_flags  = 0;

#define GOTOQUIT    { ret = MYDOWN_ERROR; goto quit; }

	if(!sock || !*sock) {
		peer.sin_addr.s_addr = mydown_resolv(host);
		if(peer.sin_addr.s_addr == INADDR_NONE) GOTOQUIT;
		peer.sin_port        = htons(port);
		peer.sin_family      = AF_INET;

		sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(sd < 0) GOTOQUIT;
		if(sock) *sock = sd;

		VERPRINTF2"  connect to %s:%hu...", inet_ntoa(peer.sin_addr), port);
		if(connect(sd, (struct sockaddr *)&peer, sizeof(peer)) < 0) {
			fprintf(stderr, "\nError: connection refused\n");
			GOTOQUIT;
		}
		VERPRINTF2"done\n");
	} else {
		sd = *sock;
	}

	len =
		303                                       + // my format strings and HTTP parameters
		strlen(getstr)                            +
		strlen(host)                              +
		5                                         +
		((from || tot)   ? 20 + 20           : 0) +
		(b64             ? strlen(b64)       : 0) +
		(useragent       ? strlen(useragent) : 0) +
		1;

	query = malloc(len);
	if(!query) GOTOQUIT;

	len = sprintf(
		query,
		"GET /%s HTTP/1.1\r\n"
		"Host: %s%s\r\n"
		"Connection: %s\r\n",
		getstr,
		host, mydown_showhttp80(port),
		sock ? "keep-alive" : "close");

#define HTTP_APPEND  len += sprintf(query + len,

	if(from || tot) {
		HTTP_APPEND "Range: bytes=");
		if(from != -1) HTTP_APPEND "%u", from);
		HTTP_APPEND "-");
		if(tot >= 1)   HTTP_APPEND "%u", (tot - 1) + ((from == -1) ? 0 : from));
		HTTP_APPEND "\r\n");
	}
	if(b64) {
		HTTP_APPEND "Authorization: Basic %s\r\n", b64);
	}
	if(useragent) {
		HTTP_APPEND "User-Agent: %s\r\n", useragent);
	}

	HTTP_APPEND "\r\n");

#undef HTTP_APPEND

	err = send(sd, query, len, 0);
	mydown_free(&query);
	if(err != len) GOTOQUIT;

	buff = malloc(BUFFSZ + 1);
	if(!buff) GOTOQUIT;
	buff[0] = 0;
	data = buff;
	len  = BUFFSZ;
	while((t = mydown_recv(sd, data, len, timeout)) > 0) {
		data += t;
		len  -= t;
		*data = 0;

		header_limit = strstr(buff, "\r\n\r\n");
		if(header_limit) {
			*header_limit = 0;
			header_limit += 4;
			break;
		}
		header_limit = strstr(buff, "\n\n");
		if(header_limit) {
			*header_limit = 0;
			header_limit += 2;
			break;
		}
	}
	if(t < 0) GOTOQUIT;

	if(!header_limit) {    // no header received
		header_limit = buff;
	} else {
		if(showhead == 1) {
			if(filedata) *filedata = strdup(buff);
			VERPRINTF"\n%s", buff);
			goto quit;
		}
		if(verbose == 2) fprintf(stderr, "\n%s", buff);

		s = strchr(buff, ' ');
		if(s) {
			code = atoi(s + 1);

			if((code / 100) == 3) {
				mydown_scanhead(buff, header_limit - buff,
					"location",     &location,
					NULL,           NULL);
				if(!location) {
					fprintf(stderr, "\nError: remote file is temporary unavailable (%d)\n", code);
					GOTOQUIT;
				}
				//s = mydown_hex2uri(location); // not needed
				//if(s) {
				//free(location);
				//location = s;
				//}
				VERPRINTF2"\n- redirect: %s\n", location);
				mydown_get_host(location, &host, &port, &getstr, verbose);
				if(!host || !host[0] || !port) {
					fprintf(stderr, "\nError: the Location field is invalid (error code %d)\n", code);
					GOTOQUIT;
				}
				if(sd && !sock) { close(sd); sd = 0; }
				mydown_free_sock(sock);
				ret = mydown_http2file(sock, timeout, host, port, useragent, verbose, getstr, fd, filename, showhead, onlyifdiff, resume, from, tot, filesize, filedata, ret_code);
				goto quit;
			}

			// if((code != 200) && (code != 206)) {
			if((code / 100) != 2) {
				fprintf(stderr, "\nError: remote file is temporary unavailable (%d)\n", code);
				GOTOQUIT;
			}
		}

		mydown_scanhead(buff, header_limit - buff,
			"content-length",       &contlen,
			"content-type",         &conttype,
			"content-disposition",  &contdisp,
			"icy-name",             &icyname,
			"transfer-encoding",    &transenc,
			"content-encoding",     &contenc,
			"Connection",           &connection,
			NULL,                   NULL);

		if(connection) {
			if(!stricmp(connection, "close") && sock) *sock = 0;
		}

		if(contlen) {
			s = strchr(contlen, '/');
			if(s) contlen = s + 1;
			if(sscanf(contlen, "%u", &fsize) != 1) fsize = 0;   //break;
		}

		if(!contdisp && icyname) contdisp = icyname;

		if(transenc && strstr(transenc, "chunked")) chunked = 1;

		if(filesize) *filesize = fsize;
	}

	if(filename && ((resume == 2) || (resume == 3))) {
		fd = (void *)filename;
		if(!fd) {
			fprintf(stderr, "\nError: no FILE used with resume %d\n", resume);
			GOTOQUIT;
		}
		filename = "."; // this instruction is useless, I have added it only to skip the checks below
	}

	if(!filename || !filename[0]) {
		if(contdisp) {
			s = (u8 *)strstr(contdisp, "filename=");
			if(!s) s = (u8 *)strstr(contdisp, "filename*=");
			if(!s) s = (u8 *)strstr(contdisp, "file=");
			if(!s) s = (u8 *)strstr(contdisp, "file*=");
			if(s) {
				s = strchr(s, '=') + 1;
			} else {
				s = contdisp;
			}
			while(*s && ((*s == '\"') || (*s == ' ') || (*s == '\t'))) s++;
			filename = mydown_hex2uri(s);
			if(filename) filenamemalloc = filename; // needed for freeing it later!
			if(filename && filename[0]) {
				for(s = filename; *s; s++) {    // \r \n are don't exist, it's a line
					if((*s == '\\') || (*s == '/') || (*s == ';') || (*s == ':') || (*s == '\"') || (*s == '&') || (*s == '?')) break;
				}
				for(s--; (s >= filename) && *s && ((*s == ' ') || (*s == '\t')); s--);
				*(s + 1) = 0;
			}
		} else {
			filename = mydown_hex2uri(getstr);
			if(filename) filenamemalloc = filename; // needed for freeing it later!
			if(filename && filename[0]) {
				for(
					s = filename + strlen(filename) - 1;
					(s >= filename) && (*s != '/') && (*s != '\\') && (*s != ':') && (*s != '&') && (*s != '?') && (*s != '=');
				s--);
				filename = s + 1;
			}
		}

		if(filename) {                          // last useless check to avoid directory traversal
			s = strrchr(filename, ':');
			if(s) filename = s + 1;
			s = strrchr(filename, '\\');
			if(s) filename = s + 1;
			s = strrchr(filename, '/');
			if(s) filename = s + 1;

			s = strrchr(filename, '.');
		}
	}

	if(!filename || !filename[0]) {
		filename = mydown_tmpnam();
		if(filename) filenamemalloc = filename; // needed for freeing it later!
		//VERPRINTF"\nError: no filename retrieved, you must specify an output filename\n\n");
		//GOTOQUIT;
	}

	if(showhead == 2) {
		if(filedata) *filedata = strdup(filename);
		ret = fsize;
		goto quit;
	}

	if(!filedata && !fd) {
		if(!strcmp(filename, "-")) {
			fd = stdout;
			VERPRINTF"  file   %s\n", "stdout");
		} else {
			VERPRINTF"  file   %s\n", filename);
			err = stat(filename, &xstat);
			if(onlyifdiff && !err && (xstat.st_size == fsize)) {
				VERPRINTF"  the remote file has the same size of the local one, skip\n");
				GOTOQUIT;
			}
			mydown_filename_clean(filename);
			if((err < 0) || !resume) {      // file doesn't exist or must not resume
				fd = fopen(filename, "wb");
			} else {
				fd = fopen(filename, "ab");
				from = xstat.st_size;
				VERPRINTF2"  resume %u\n", from);
				if(sd && !sock) { close(sd); sd = 0; }
				mydown_free_sock(sock);
				ret = mydown_http2file(sock, timeout, host, port, useragent, verbose, getstr, fd, filename, showhead, onlyifdiff, resume, from, tot, filesize, filedata, ret_code);
				goto quit;
			}
			if(!fd) GOTOQUIT;
		}
	}

	len = data - header_limit;
	memmove(buff, header_limit, len);

	if(verbose > 0) {
		fprintf(stderr, "\n");
		if(fsize) fprintf(stderr, "    ");
		fprintf(stderr, " | downloaded   | kilobytes/second\n");
		if(fsize) fprintf(stderr, "----");
		fprintf(stderr, "-/--------------/-----------------\n");
	}

	if(verbose >= 0) {
		TEMPOZ(oldtime);
		oldtime -= VISDELAY;
	}

	if(filedata) {
		filedatasz  = fsize;
		filedatatmp = malloc(filedatasz);
		if(!filedatatmp) GOTOQUIT;
	}

	if(chunked) chunkedsize = len;

	do {
redo:
		httpret += len;

		if(chunked) {
			for(;;) {
				chunkedsize = mydown_chunked_skip(buff, chunkedsize);

				err = mydown_sscanf_hex(buff, chunkedsize);
				if(err > 0) break;
				if(!err) {
					chunkedsize = mydown_chunked_skip(buff, chunkedsize);
					break;
				}

				t = mydown_recv(sd, buff + chunkedsize, BUFFSZ - chunkedsize, timeout);
				if(t <= 0) GOTOQUIT;
				chunkedsize += t;
				if(chunkedsize >= BUFFSZ) GOTOQUIT;
			}

			chunkedlen = err;
			if(!chunkedlen) break;

			if(chunkedbuff) free(chunkedbuff);
			chunkedbuff = malloc(chunkedlen);
			if(!chunkedbuff) GOTOQUIT;

			s = (u8 *)strchr(buff, '\n');
			if(!s) GOTOQUIT;
			err = (s + 1) - buff;
			chunkedsize -= err;
			memmove(buff, buff + err, chunkedsize);

			if(chunkedlen < chunkedsize) {      // we have more data than how much we need
				memcpy(chunkedbuff, buff, chunkedlen);
				chunkedsize -= chunkedlen;
				memmove(buff, buff + chunkedlen, chunkedsize);
			} else {                            // we have only part of the needed data
				memcpy(chunkedbuff, buff, chunkedsize);
				for(len = chunkedsize; len < chunkedlen; len += t) {
					t = mydown_recv(sd, chunkedbuff + len, chunkedlen - len, timeout);
					if(t <= 0) GOTOQUIT;
				}
				chunkedsize = 0;
			}

			chunkedtmp  = buff;
			buff        = chunkedbuff;
			len         = chunkedlen;
		}

		ret += len;

		/* WRITE THE DATA INTO FILE OR MEMORY */

		if(tot && (ret > tot)) {
			len = tot - (ret - len);
			ret = tot;
		}

		if(filedata) {
			if(filedatasz < ret) {
				filedatasz  = ret;
				//filedatatmp = realloc(filedatatmp, filedatasz);
				filedatatmp = realloc(filedata, filedatasz);
				if(!filedatatmp) GOTOQUIT;
			}
			memcpy(filedatatmp + ret - len, buff, len);
		} else if(fd) {
			if(fwrite(buff, 1, len, fd) != len) {
				fprintf(stderr, "\nError: I/O error. Probably your disk is full or the file is write protected\n");
				GOTOQUIT;
			}
			//fflush(fd);
		}
		if(ret_code && (resume == 3)) *ret_code = ret;

		/* VISUALIZATION */

		if(verbose >= 0) {
			TEMPOZ(newtime);
			if((newtime - oldtime) >= VISDELAY) {
				mydown_showstatus(fsize, httpret, vishttpret, (int)(newtime - oldtime));
				oldtime = newtime;
				vishttpret = httpret;
			}
		}

		/* FREE, EXCHANGE OR OTHER STUFF */

		if(chunked) {
			chunkedbuff = buff;
			buff        = chunkedtmp;
			len         = 0;
			goto redo;
		}

		/* FSIZE CHECK */

		if(tot && (ret == tot)) break;
		if(fsize) {
			if(httpret >= fsize) break;
		}

		/* READ NEW DATA FROM THE STREAM */
	} while((len = mydown_recv(sd, buff, BUFFSZ, timeout)) > 0);

	if(verbose >= 0) {
		TEMPOZ(newtime);
		mydown_showstatus(fsize, httpret, vishttpret, (int)(newtime - oldtime));
	}

	if(fsize && (len < 0)) GOTOQUIT;

	if(filedata) {
		*filedata = filedatatmp;
	}

quit:
	if(chunkedbuff != buff) mydown_free(&chunkedbuff);
	mydown_free(&b64);
	mydown_free(&buff);
	mydown_free(&filenamemalloc);
	if(ret_code) {
		*ret_code = code;
		if(resume == 3) *ret_code = ret;
	}
	if(sd && !sock) { close(sd); sd = 0; }
	VERPRINTF"\n");
	if(ret == MYDOWN_ERROR) mydown_free_sock(sock);
	if(filename && ((resume == 2) || (resume == 3))) {
		// do nothing
	} else {
		if(!oldfd && fd && (fd != stdout)) fclose(fd);
	}
	return(ret);

#undef GOTOQUIT
}

u8 *mydown_tmpnam(void) {
	FILE    *fd;
	int     i;
	u8      *ret;

	ret = malloc(32);
	for(i = 1; ; i++) {
		sprintf(ret, "%u.myd", i);  // enough small for any OS (8.3 too)
		fd = fopen(ret, "rb");      // check real existence of the file
		if(!fd) break;
		fclose(fd);
	}
	return(ret);
}

void mydown_free_sock(int *sock) {
	if(sock && *sock) {
		close(*sock);
		*sock = 0;
	}
}

u8 *mydown_http_skip(u8 *buff, int len, int *needed, int *remain) {
	int     rest;

	if(!buff) return(NULL);
	rest = *needed;
	if(len < rest) {
		*needed = rest - len;
		*remain = 0;
		return(NULL);
	}
	*needed = 0;
	*remain = len - rest;
	return(buff + rest);
}

void mydown_free(u8 **buff) {
	if(!buff || !*buff) return;
	free(*buff);
	*buff = NULL;
}

int mydown_chunked_skip(u8 *buff, int chunkedsize) {
	int     t;

	if(!buff) return(0);
	for(t = 0; t < chunkedsize; t++) {
		if((buff[t] != '\r') && (buff[t] != '\n')) break;
	}
	if(t) {
		chunkedsize -= t;
		memmove(buff, buff + t, chunkedsize);
	}
	return(chunkedsize);
}

int mydown_sscanf_hex(u8 *data, int datalen) {
	int     i,
		ret;

	if(!data) return(MYDOWN_ERROR);
	for(i = 0; i < datalen; i++) {
		if((data[i] == '\r') || (data[i] == '\n')) break;
	}
	if(i == datalen) return(MYDOWN_ERROR);

	if(sscanf(data, "%x", &ret) != 1) return(MYDOWN_ERROR);
	return(ret);
}

int mydown_timeout(int sock, int secs) {
	struct  timeval tout;
	fd_set  fdr;
	int     err;

	tout.tv_sec  = secs;
	tout.tv_usec = 0;
	FD_ZERO(&fdr);
	FD_SET(sock, &fdr);
	err = select(sock + 1, &fdr, NULL, NULL, &tout);
	if(err < 0) return(MYDOWN_ERROR); //std_err();
	if(!err) return(MYDOWN_ERROR);
	return(0);
}

int mydown_recv(int sd, u8 *data, int len, int timeout) {
	if(!timeout) timeout = MAXTIMEOUT;
	if(mydown_timeout(sd, timeout) < 0) return(MYDOWN_ERROR);
	return(recv(sd, data, len, 0));
}

u8 *mydown_showhttp80(u16 port) {
	static u8  mini[7];

	*mini = 0;
	if(port != 80) sprintf(mini, ":%hu", port);
	return(mini);
}

void mydown_showstatus(u32 fsize, u32 ret, u32 oldret, int timediff) {
	u32     vis;

	if(fsize) {
		vis = ((u64)ret * (u64)100) / (u64)fsize;
		fprintf(stderr, "%3u%%", (vis < 100) ? vis : 100);
	}
	httpdown.byteDownloaded=ret;
	fprintf(stderr, "   %12u", ret);
	if(ret > 0) {
		if(timediff) fprintf(stderr, "   %-10u", (u32)(((((u64)ret - (u64)oldret) * (u64)1000) / (u64)timediff) / 1024));
		httpdown.speed=(u32)(((((u64)ret - (u64)oldret) * (u64)1000) / (u64)timediff) / 1024);
	}
	fprintf(stderr, "\r");
}

u8 *mydown_base64_encode(u8 *data, int *size) {
	int     len;
	u8      *buff,
		*p,
		a,
		b,
		c;
	static const u8 base[64] = {
		'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
		'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
		'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
		'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/'
	};

	if(!data) return(NULL);
	if(!size || (*size < 0)) {      // use size -1 for auto text size!
		len = strlen(data);
	} else {
		len = *size;
	}
	buff = malloc(((len / 3) << 2) + 6);
	if(!buff) return(NULL);

	p = buff;
	do {
		a     = data[0];
		b     = data[1];
		c     = data[2];
		*p++  = base[(a >> 2) & 63];
		*p++  = base[(((a &  3) << 4) | ((b >> 4) & 15)) & 63];
		*p++  = base[(((b & 15) << 2) | ((c >> 6) &  3)) & 63];
		*p++  = base[c & 63];
		data += 3;
		len  -= 3;
	} while(len > 0);
	*p = 0;

	for(; len < 0; len++) *(p + len) = '=';

	if(size) *size = p - buff;
	return(buff);
}

in_addr_t mydown_resolv(char *host) {
	struct      hostent *hp;
	in_addr_t   host_ip;
	int         i;
	dns_db_t    *dns;

	host_ip = inet_addr(host);
	if(host_ip == htonl(INADDR_NONE)) {
		for(i = 0; i < dns_db_max; i++) {           // search
			if(!stricmp(host, dns_db[i].host)) return(dns_db[i].ip);
		}

		hp = gethostbyname(host);
		if(!hp) {
			fprintf(stderr, "\nError: Unable to resolve hostname (%s)\n\n", host);
			return(INADDR_NONE);
		}
		host_ip = *(in_addr_t *)(hp->h_addr);

		if(!dns_db_max) memset(&dns_db, 0, sizeof(dns_db));
		if(dns_db_add == MAXDNS) dns_db_add = 0;    // add
		dns = &dns_db[dns_db_add];
		if(dns->host) free(dns->host);
		dns->host = strdup(host);
		dns->ip   = host_ip;
		dns_db_add++;
		if(dns_db_max < MAXDNS) dns_db_max++;
	}
	return(host_ip);
}