/* $Id: Http.cpp,v 1.15 2005/03/17 22:12:13 kps Exp $
 *
 * Our interface to a web browser
 *
 *      Copyright (C) 2002 by
 *
 *      The XPilot Authors   <xpilot@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
 *      Jarrod Miller        <jarrod@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
 *      Bj�rn Stabell        <bjoern@xpilot.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * See the file COPYRIGHT.TXT for current copyright information.
 *
 */
/*
 * $Log: Http.cpp,v $
 * Revision 1.15  2005/03/17 22:12:13  kps
 * Get rid of warnings from makedepend about "non-portable whitespace".
 *
 * Revision 1.14  2004/05/10 06:25:00  dick
 * Find the Http before a read
 *
 * Revision 1.13  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.12  2004/01/30 15:53:42  dick
 * Don't do the reverse name lookup that takes too long and freaks out the web server
 * (because it's taking too long).
 *
 * Revision 1.11  2004/01/28 00:06:59  dick
 * Prefer the htmlDirectory over the bootDirectory
 *
 * Revision 1.10  2004/01/24 00:57:25  dick
 * Socket cleanups for Linux.
 *
 * Revision 1.9  2004/01/24 00:43:04  dick
 * Note the IP address of the other end of our connection.
 *
 * Revision 1.8  2004/01/13 02:04:08  dick
 * Don't try to tokenize a blank "GET" line.
 *
 * Revision 1.7  2004/01/06 04:48:28  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 * Revision 1.6  2002/07/23 17:52:47  dick
 * Move ParseHeaders() to Obj.cpp and make it more general purpose.
 *
 * Revision 1.5  2002/07/09 22:53:32  dick
 * Add showtime to xpprintf()
 *
 * Revision 1.4  2002/07/08 18:19:33  dick
 * remove some old debug
 *
 * Revision 1.3  2002/06/26 04:44:25  dick
 * MessageBox is a Windows only call
 *
 * Revision 1.2  2002/06/25 06:58:37  dick
 * Windows doesn't use MFC for new xpilot++ work.
 *
 * Revision 1.1  2002/06/25 04:41:03  dick
 * An http server.
 *
 */
#include "StdAfx.h"

#if defined(_WINDOWS) && !defined(_CYGWIN)
#	include <io.h>
#	include <fcntl.h>
#else
// UNIX
#	include <unistd.h>
#	include <stdlib.h>
#	include <string.h>
#	include <errno.h>
#	include <sys/types.h>
#	include <sys/stat.h>
#	include <sys/socket.h>
#	include <netinet/in.h>
#	include <arpa/inet.h>
#	include <fcntl.h>
#	include <stdarg.h>
#endif

#include "Http.h"
#include "config.h"
#include "xpprintf.h"
#include "sockerrs.h"
#include "CfgBase.h"
#include "portability.h"
#include "xpprintf.h"
#include "NetList.h"

char     szHostname[100];

PCSTR	s_HttpListen	= "HttpListen";
PCSTR	s_Http			= "Http";
PCSTR	s_true			= "true";
PCSTR	s_false			= "false";
PCSTR	s_127_0_0_1		= "127.0.0.1";

char	tokseps[] = " ,\t\n";

///////////////////////////////////////////////////////////////////////////////
bool HttpListen::InitListen()
{
	HOSTENT *pHent;
	int ret;

	time_t	now = time(NULL);

	if (now < retryAt)
		return(false);
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
#		ifdef	_WINDOWS
			MessageBox(NULL, "Failed to create http listen socket", "Error", MB_OK);
#		else
			xpprintf("Failed to create http listen socket\n");
#		endif
		return(false);
	}
	gethostname( szHostname, sizeof( szHostname ));
	pHent = gethostbyname( szHostname );

	memset(&s_in, 0, sizeof(s_in));
//	s_in.sin_addr.s_addr = *(u_long *)pHent->h_addr;
	s_in.sin_addr.s_addr	= INADDR_ANY;
	s_in.sin_family = AF_INET;
	s_in.sin_port = htons(port);

	ret = bind(sock, (SOCKADDR*)&s_in, sizeof(SOCKADDR_IN));
	if (ret)
	{
		String	s;
		char	s1[256];
		s.printf("%sbind on http listen socket failed", showtime());
		if (errno)
		{
			sprintf(s1, " errno=%d %s", errno, GetSockErrText(errno));
			s += s1;
		}
		if (h_errno)
		{
			sprintf(s1, " h_errno=%d %s", h_errno, GetSockErrText(h_errno));
			s += s1;
		}
		s += "\n";
		xpprintf(s);
		closesocket(sock);
		retryCount++;
		retryAt = now + 4;
		if (retryCount > 4)
			retryAt = now + 10;
		if (retryCount > 8)
			retryAt = now + 60;
		if (retryCount > 20)
			retryAt = now + 300;
		return(false);
	}
	ret = listen(sock, SOMAXCONN);
	listensock = sock;
	if (SetNonBlockingMode())
		state = ready;
	else
		state = error;

	xpprintf("%sWeb: network ready to receive on port %d\n", showtime(), GetCfg()->webPort);
	return(true);
}

///////////////////////////////////////////////////////////////////////////////
HttpListen::HttpListen()
{
	int	i;
	type = s_HttpListen;
#ifdef	_WINDOWS
	wMsg = WMB_HTTP;
#endif
	port = GetCfg()->webPort;
	state = init;
	listensock = 0;
	for (i=0; i<SOMAXCONN; i++)
	{
		connectsock[i] = 0;
		myhttps[i] = 0;
	}
	time(&retryAt);
	InitListen();
}


///////////////////////////////////////////////////////////////////////////////
HttpListen::~HttpListen()
{
	int ret;
	int	i;

	for (i=0; i<SOMAXCONN; i++)
	{
		if (connectsock[i])
		{
			ret = closesocket(connectsock[i]);
		}
	}
	if (listensock)
	{
#ifdef	_WINDOWS
		BOOL	opts = FALSE;
		int ret;
		ret = setsockopt(listensock, SOL_SOCKET, SO_DONTLINGER, (PCSTR)&opts, sizeof(BOOL));
		if (ret == SOCKET_ERROR)
			ret = h_errno;
#endif
		ret = closesocket(listensock);
	}
}

///////////////////////////////////////////////////////////////////////////////
Http::Http()
{
//	u = NULL;
	type = s_Http;
#ifdef	_WINDOWS
	wMsg = WMB_HTTP;
#endif
}

///////////////////////////////////////////////////////////////////////////////
Http::~Http()
{
}

///////////////////////////////////////////////////////////////////////////////
HttpListen*	FindHttpListener()
{
	Obj* o = netList.GetHead();
	HttpListen* n;
	while (o)
	{
		n = (HttpListen*)o;
		if (o->type == s_HttpListen)
		{
			return(n);
		}
		o = o->GetNext();
	}
	return (NULL);
}

///////////////////////////////////////////////////////////////////////////////
Http*	FindHttpBySock(SOCKET s)
{
	Obj* o = netList.GetHead();
	HttpListen* n;
	while (o)
	{
		n = (HttpListen*)o;
		if (o->type == s_HttpListen)
		{
			if (n->listensock == s)
				return(NULL);			// bogus?
			for (int i=0; i<SOMAXCONN; i++)
				if (n->connectsock[i] == s)
					return(n->myhttps[i]);
		}
		o = o->GetNext();
	}
//	bprintf("Can't match network to socket %d\n", s);
	return(NULL);			// bogus dude.

}

///////////////////////////////////////////////////////////////////////////////
void HttpClose(SOCKET s)
{
	D(SOTRACE("http: close %d\n", s);)
	HttpListen* n = FindHttpListener();

//	struct linger ling;
//	ling.l_onoff  = TRUE;
//	ling.l_linger =5;
	int ret = shutdown(s, SD_SEND);
	if (ret)
	{
		D(SOTRACE("http_close: shutdown() socket %d error %d (%s)\n",
							s, h_errno, GetSockErrText(h_errno));)
	}
	if (closesocket(s))
	{
		D(SOTRACE("http_close: closesocket() socket %d error %d (%s)\n",
						s, h_errno, GetSockErrText(h_errno));)
	}
	for (int i=0; i<SOMAXCONN; i++)
	{
		if (n->connectsock[i] == s)
		{
			delete n->myhttps[i];
			n->connectsock[i] = 0;
			return;
		}
	}
	D(SOTRACE("http_close can't match socket %d\n", s);)
}

///////////////////////////////////////////////////////////////////////////////
void HttpAccept(SOCKET s)
{
	HttpListen* n = FindHttpListener();
	if (!n)
		return;			// bogus dude.

	SOCKADDR_IN	s_in;	// who we talking to?
#ifdef	_UNIX
	unsigned
#endif
			int			ssi = sizeof(SOCKADDR_IN);
//	HOSTENT*	he;

	for (int i=0; i<SOMAXCONN; i++)
	{
		if (!n->connectsock[i])
		{
			n->connectsock[i] = accept(s, (SOCKADDR*)&s_in, &ssi);
			n->myhttps[i] = GetCfg()->HttpFactory();
			n->myhttps[i]->sock = n->connectsock[i];
			n->myhttps[i]->SetNonBlockingMode();
			n->myhttps[i]->connectAddr = s_in.sin_addr;
			D(SOTRACE("_accept: sock %d @ %d sin_port %d\n",
							n->connectsock[i], i, (int)s_in.sin_port);)
/*			he = gethostbyaddr((const char*)&s_in.sin_addr,
							   sizeof(struct in_addr), AF_INET);
			if (!he)
			{
				char*	s = inet_ntoa(s_in.sin_addr);
				if (GetCfg()->logLevel > LOGMIN)
					xpprintf("%sgethostbyaddr error for %s\n", showtime(), s);
				n->myhttps[i]->connectHost = s;
			}
			else
				n->myhttps[i]->connectHost = he->h_name;
*/
			n->myhttps[i]->connectHost = inet_ntoa(s_in.sin_addr);
			return;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
void HttpRead(SOCKET s)
{
	int ret;
	char	buf[1000];
	char	buf1[1000];
	Http* n = FindHttpBySock(s);
	if (!n)
	{
		xpprintf("HttpRead: Can't match socket %d\n", s);
		return;
	}
//	n->SetNonBlockingMode();
	memset(buf, 0, 1000);
	ret = recv(s, buf, 1000, 0);
//	n->SetBlockingMode();
	D(SOTRACE("http_read: sock %d @ %p ret = %d\n", s, n, ret);)
	if (!ret)
		return;			// "gracefully closed"
	if (ret == SOCKET_ERROR)
	{
		int err = h_errno;
		if (err != EWOULDBLOCK)
			xpprintf(LOGMIN, "http_read: Error reading socket %d error %d (%s)\n", s, err, GetSockErrText(err));
	}
	if (!n)
	{
		xpprintf("http_read: can't match socket %d to http\n", s);
		return;
	}
	char* p = buf;
	char* e = buf+ret;
	LString*	l = (LString*)n->stringsin.GetHead();

	if (!l)
	{
		l = new LString;
		n->stringsin.Add(l);
	}

	// loop to unload each line of data
	while (p < e)
	{
		// determine where to put the data
		while(1)
		{
			int length = strlen(l->s);
			if (!strlen(l->s))
				break;
			if (l->s[length-1] != '\n')
				break;
			l = (LString*)l->GetNext();
			if (!l)
			{
				l = new LString;
				n->stringsin.Add(l);
				break;
			}
		}
		// find a \n or end of data
		char* st;
		for (st = p; st<e; st++)
		{
			if (*st == '\n')
			{
				st++;
				break;
			}
		}
		int count = st-p;
		strncpy(buf1, p, count);
		if (buf1[count-2] == '\r')
			buf1[--count-1] = '\n';
		buf1[count] = '\0';
		l->s += buf1;
		p = st;
		if (!strcmp(l->s, "\n"))		// blank line?
		{
			n->stringsin.ParseHeaders(':');	// fix the headers up
			time(&n->now);
			n->ProcessReceive();
			n->SetNonBlockingMode();
			int ret = shutdown(s, SD_SEND);
			if (ret)
			{
				D(SOTRACE("http_read: shutdown() socket %d error %d (%s)\n",
								s, h_errno, GetSockErrText(h_errno));)
			}
			//closesocket(s);
#ifdef	_UNIX
			HttpClose(s);
#endif
			return;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
void Http::LogHeaders(int level)
{
	if (GetCfg()->logLevel < level)
		return;
	LString* l = (LString*)stringsin.GetHead();
	while(l)
	{
		const char* data = l->s;
		if (!strcmp(l->name, "Authorization"))
			data = "(shhh...)";
		xpprintf("%s%s: %s%s%s%s%s\n", showtime(), type, l->name.GetLength() ? (PCSTR)l->name : "",
								  l->name.GetLength() ? ": " : "",
								  data,
								  connectHost.GetLength() ? " " : "",
								  (PCSTR)connectHost);

		if (GetCfg()->logLevel == LOGMAX)
			l = (LString*)l->GetNext();
		else
			l = NULL;
		if (!l || !l->name.GetLength())
			return;
	}
}

///////////////////////////////////////////////////////////////////////////////
void Http::ProcessReceive()
{
	//lstring* l = (lstring*)stringsin.GetHead();

	SetBlockingMode();
	ParseCommand();
//	u = FindUser();

	switch (httpCommand)
	{
	case GET:
		ProcessGET();
		break;
	case HEAD:
		ProcessHEAD();
		break;
	case POST:
		ProcessPOST();
		break;
	default:
		const char* s = "Bad command (no \"GET\")";
		Print404("Bad command", s);
		xpprintf("%shttp: %s %s\n", showtime(), s, (PCSTR)connectHost);
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////
void Http::ParseCommand()
{
	LString* l = (LString*)stringsin.GetHead();		// first line should be request/command
	char*	s = (char*)malloc(strlen(l->s)+1);
	char*	token;
	char*	t;

	strcpy(s, l->s);

	// Get the command
	token = strtok(s, tokseps);
	if (!token)								// blank line
		return;
	if (!strcasecmp(token, "GET"))
		httpCommand = GET;
	else if (!strcasecmp(token, "POST"))
		httpCommand = POST;
	else if (!strcasecmp(token, "HEAD"))
		httpCommand = HEAD;

	// Get the filename (and args)
	token = strtok(NULL, tokseps);
	if (!token)
	{
		xpprintf("%shttp: bogus command: %s\n", showtime(), (PCSTR)l->s);
		LogHeaders(LOGMIN);
		free(s);
		return;
	}
	t = strchr(token, '?');
	if (t)
		*t++ = '\0';;
	filename = token;
	if (t)
		ParseGetOpts(args, t);
	free(s);
}

///////////////////////////////////////////////////////////////////////////////
void Http::ParseGetOpts(ObjList& args, PCSTR s)
{
	char*	orig = (char*)malloc(strlen(s)+1);
	char*	t = orig;
	strcpy(t, s);
	char*	e;

	// Process any args
	args.Empty();
	LString* ns;
	// Get the argument name
	for (e = t; t && e && *e != '\0'; e++)
	{
		if (*e == '=')
		{
			*e = '\0';
			ns = new LString((PCSTR)t);
			args.Add(ns);
			e++;
			for (t = e; *e != '\0' && *e != '&' && *e != '\n'; e++)
				;
			char c = *e;
			*e = '\0';
			ns->s = t;
			if (c)
				e++;
			t = e;
			//break;
		}

		if (*e == '\n' || *e == '\0')
		{
			//t = NULL;
			break;
		}
	}
	free(orig);
}

///////////////////////////////////////////////////////////////////////////////
void Http::ProcessGET()
{
	headersOuted = false;
	if (!strncmp(filename, "/g/", 3))
		ProcessGETgraphic(filename.s);
	else if (filename[1] == '_')
	{
		command = &filename.s[2];
		DoCommand();
	}
	else
	{
		ProcessGEThtml(filename.s);
	}
/*
	else if (filename[1] == '#')
	{
		command = &filename.s[2];
		DoAgentCommand();
	}
	else
		ProcessGEThtml(filename.s);
*/

}

#define	MAXREAD	2048

///////////////////////////////////////////////////////////////////////////////
void Http::ProcessGETgraphic(const char* fname)
{
	int		fd;
	String fn;
	fn = GetCfg()->bootDirectory;
	if (GetCfg()->htmlDirectory.GetLength())
		fn = GetCfg()->htmlDirectory;
	else
		fn += "html";
	fn += fname;
	fd = open(fn, BINARYFILEFLAGS);
	if (fd == -1)
	{
		printf("HTTP/1.0 404 Not Found\n");
		PrintServer();
		printf("Connection: close\n\n");
		xpprintf("%shttp: 404 %s\n", showtime(), fname);
	}
	else
	{
		printf("HTTP/1.0 200 OK\n");
		PrintServer();
		printf("Connection: close\n");
		printf("Content-Type: image\n\n");

		char	buf[MAXREAD+1];
		int		ret;
		while (1)
		{
			ret = read(fd, buf, MAXREAD);
			if (ret <= 0)
				break;
			send(buf, ret);
		}
		close(fd);
	}
}

///////////////////////////////////////////////////////////////////////////////
void Http::ProcessGEThtml(PCSTR cfname)
{
	String	fname;
	fname = cfname;
	if (GetCfg()->logLevel > LOGNONE)
	{
		if (!strcmp(cfname, "/"))
			LogHeaders(GetCfg()->logLevel);
		else
			LogHeaders(LOGMIN);
	}
#if 0
	if (!AuthenticateUser(authReadOnly))	// any access at all?
	{
		AuthenticateFail();
		return;
	}

	if (cfname[0] == '/' && !strcmp(&cfname[1], s_login))
	{
		if (AuthenticateUser(authAdmin))	// any access at all?
		{
			Print200html();
			printf("You are logged in as the administrator.</BODY></HTML>\n");
			return;
		}
		/*
		if (AuthenticateUser(authReadWrite))	// any access at all?
		{
			//AuthenticateFail(sock);
			//return;
			Print200html();
			printf("You are logged in with read/write access (but not administrator).\n");
			printf("</BODY></HTML>\n");
			return;
		}
		*/
		bsuser* u;
		if (u = FindUser())
		{
			Print200html();
			printf("You are logged in as %s.\n", (PCSTR)u->name);
			printf("</BODY></HTML>\n");
			return;
		}
		AuthenticateFail();
		return;
	}
	if (cfname[0] == '/' && !strcmp(&cfname[1], s_welcomeHtml))
	{
		if (GetCfg()->welcomeHtml.GetLength())
		{
			fname = "/";
			fname += GetCfg()->welcomeHtml;
		}
	}
#endif
	ProcessFile(cfname);
}

///////////////////////////////////////////////////////////////////////////////
void Http::ProcessPOST()
{
}

///////////////////////////////////////////////////////////////////////////////
void Http::ProcessHEAD()
{
	printf("HTTP/1.0 200 OK\n");
	PrintServer();
	printf("Connection: close\n");
	printf("Content-Type: text/html\n\n");
}

///////////////////////////////////////////////////////////////////////////////
void Http::ProcessFile(PCSTR cfname)
{
	String	fname;
	fname = cfname;
	int		fd;
	String fn;

	fn = GetCfg()->bootDirectory;
	if (!strncmp(cfname, "/help/", 6))
	{
		if (GetCfg()->helpDirectory.GetLength())
		{
			String temp;
			temp = fname;
			fname = GetCfg()->helpDirectory;
			fname += temp;
		}
	}
	else if (GetCfg()->htmlDirectory.GetLength())
		fn = GetCfg()->htmlDirectory;
	else
		fn += "html";
	fn += fname;
	if (!strcmp(fname, "/"))
		fn += "index.html";
	fd = open(fn, BINARYFILEFLAGS);
	if (fd == -1)
	{
		Print404(fn);
	}
	else
	{
		if (!headersOuted)
		{
			printf("HTTP/1.0 200 OK\n");
			PrintServer();
			printf("Connection: close\n");
			printf("Content-Type: text/html\n\n");
			headersOuted = true;
		}

		char	buf[MAXREAD+1];
		int		ret;
		while (1)
		{
			ret = read(fd, buf, MAXREAD);
			if (ret <= 0)
				break;
			int	j;
			int i = 0;
			for (j=i; i<ret; i++)
			{
				if (buf[i] == '@' && buf[i+1] == '@')
				{
					if (i-j)
						send(&buf[j], i-j);
					for (i+=2, j=i; i<ret; i++)
					{
						if (buf[i] == '@' && buf[i+1] == '@')
						{
							String	s(&buf[j], i-j);

//							ParseHTMLout(sock, s);
							TokenSubst(s);
							i += 2;
							j = i;
							break;
						}
					}
				}
			}
			if (j<ret)
				send(&buf[j], i-j);
		}
		close(fd);
	}
}

///////////////////////////////////////////////////////////////////////////////
void Http::Print404(PCSTR name)
{
	printf("HTTP/1.0 404 %s\n", name);
	PrintServer();
	printf("Connection: close\n\n");
	printf("<HTML><HEAD><TITLE>404</TITLE></HEAD><BODY>404 %s</BODY></HTML>\n", name);
	xpprintf("%shttp: 404 %s\n", showtime(), name);
}

///////////////////////////////////////////////////////////////////////////////
void Http::Print404(PCSTR error, PCSTR longerror)
{
	printf("HTTP/1.0 404 %s\n", error);
	PrintServer();
	printf("Connection: close\n");
	printf("Content-Type: text/plain\n");
	printf("\n%s", longerror);
}

///////////////////////////////////////////////////////////////////////////////
void Http::Print404(PCSTR error, PCSTR longerror, ObjList& headers)
{
	printf("HTTP/1.0 404 %s\n", error);
	PrintServer();
	printf("Connection: close\n");
	printf("Content-Type: text/plain\n");
	LString* ls = (LString*)headers.GetHead();
	while (ls)
	{
		printf("%s: %s\n", (PCSTR)ls->name, (PCSTR)ls->s);
		ls = (LString*)ls->GetNext();
	}
	printf("\n%s", longerror);
}
///////////////////////////////////////////////////////////////////////////////
void Http::PrintError(int e, PCSTR error, PCSTR longerror)
{
	printf("HTTP/1.0 %d %s\n", e, error);
	PrintServer();
	printf("Connection: close\n");
	printf("Content-Type: text/plain\n");
	printf("\n%s", longerror);
}

///////////////////////////////////////////////////////////////////////////////
void Http::PrintExtendedError(int e, int exerror, PCSTR error, PCSTR longerror)
{
	printf("HTTP/1.0 %d %s\n", e, error);
	PrintServer();
	printf("Connection: close\n");
	printf("Content-Type: text/plain\n");
	printf("Error: %d\n", exerror);
	printf("\n%s", longerror);
}

///////////////////////////////////////////////////////////////////////////////
void Http::Print200html()
{
	printf("HTTP/1.0 200 OK\n");
	PrintServer();
	printf("Connection: close\n");
	printf("Content-Type: text/html\n\n");
};

///////////////////////////////////////////////////////////////////////////////
void Http::Print200text()
{
	printf("HTTP/1.0 200 OK\n");
	PrintServer();
	printf("Connection: close\n");
	printf("Content-Type: text/plain\n");
}

///////////////////////////////////////////////////////////////////////////////
void Http::Print200file()
{
	printf("HTTP/1.0 200 OK\n");
	PrintServer();
	printf("Connection: close\n");
	printf("Content-Type: application/bsac\n");
}

///////////////////////////////////////////////////////////////////////////////
void Http::PrintHead()
{
	printf("<HTML><HEAD>\n");
}

///////////////////////////////////////////////////////////////////////////////
void Http::PrintBodyChoo()
{
	printf("<BODY BGCOLOR=#C0C0C0 background=/g/backchoo.gif>\n");
}

///////////////////////////////////////////////////////////////////////////////
void Http::PrintTail()
{
	printf("</BODY></HTML>\n");
}

///////////////////////////////////////////////////////////////////////////////
void Http::PrintHiddenField(PCSTR key, PCSTR value)
{
	printf("<INPUT TYPE=hidden NAME=%s value=\"%s\">\n", key, value);
}

///////////////////////////////////////////////////////////////////////////////
// Use this one *after* a Print200 to let him know he's a no good.
bool Http::AuthOK(AUTH auth)
{
	if (auth == authAdmin
	  //&& this->connectAddr.S_un.S_addr == inet_addr(s_127_0_0_1))
	  && this->connectAddr.s_addr == inet_addr(s_127_0_0_1))
		return(true);
	if (auth == authAdmin)
		return(false);
	return(true);
}

///////////////////////////////////////////////////////////////////////////////
void Http::TokenSubst(String s)
{
	printf("*** Unhandled token '%s' ***", (PCSTR)s);
	xpprintf(LOGERR, "Http: Unhandled token '%s'\n", (PCSTR)s);
}

///////////////////////////////////////////////////////////////////////////////
char* Http::ShowTimeRelative(time_t then)
{
	char*	ago = " ago";
	time_t	rightnow;
	time(&rightnow);
	long	diff = rightnow - then;
	if (diff <= 0)
	{
		diff = -diff;
		ago = "";
	}
	if (diff < 60)
		sprintf(showtimebuf, "%ld second%s%s", diff, diff == 1 ? "" : "s", ago);
	else if (diff < (60*90))	// 1.5 hours
		sprintf(showtimebuf, "%ld minute%s%s", diff/60, diff/60 == 1 ? "" : "s", ago);
	else if (diff < (60*60*50))
		sprintf(showtimebuf, "%ld hour%s%s", diff/(60*60), diff/(60*60) == 1 ? "" : "s", ago);
	else if (diff < (60*60*24*21))
		sprintf(showtimebuf, "%ld day%s%s", diff/(60*60*24), diff/(60*60*24)== 1 ? "" : "s", ago);
	else if (diff < (60*60*24*7*5))
		sprintf(showtimebuf, "%ld week%s%s", diff/(60*60*24*7), diff/(60*60*24*7) == 1 ? "" : "s", ago);
	else
		sprintf(showtimebuf, "%ld month%s%s", diff/(60*60*24*30), diff/(60*60*24*30) == 1 ? "" : "s", ago);
	return(showtimebuf);
}

///////////////////////////////////////////////////////////////////////////////
char* Http::ShowTime(time_t then)
{
    struct tm*		tmp;
    static char		month_names[13][4] = {
			    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
			    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
			    "Bug"
			};
	const time_t	sixmonths = 60*60*24*30*6;

	if (!then)
		return("<CENTER>---</CENTER>");
    tmp = gmtime(&then);
	if (tmp)
	{
		time_t	rightnow;
		char	year[32] = "";
		time(&rightnow);
/*		if (!u || u->relativeTime)
		{
			showtimeRelative(then);
		}
		else
*/
		{
			if (then < rightnow-sixmonths)
				sprintf(year, "    %d", tmp->tm_year+1900);
			else
				sprintf(year, "%02d:%02d:%02d",
					tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
			sprintf(showtimebuf, "%02d %s %s",
				tmp->tm_mday, month_names[tmp->tm_mon], year);
		}
	}
	else
	{
		strcpy(showtimebuf, "??:??");
	}
    return showtimebuf;
}

