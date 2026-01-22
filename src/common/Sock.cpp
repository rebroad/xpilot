/* $Id: Sock.cpp,v 1.23 2006/09/24 05:07:57 dick Exp $
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2001 by
 *
 *      Bjørn Stabell        <bjoern@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
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
 */
/*
 * $Log: Sock.cpp,v $
 * Revision 1.23  2006/09/24 05:07:57  dick
 * Diagnostics for failed DNS (sean was mis-configured)
 *
 * Revision 1.22  2004/05/14 20:43:34  dick
 * Use the correct error string function
 *
 * Revision 1.21  2004/05/13 09:58:53  dick
 * Missing semicolon on Linux code
 *
 * Revision 1.20  2004/05/13 09:08:27  dick
 * Retab
 *
 * Revision 1.19  2004/02/06 03:38:51  dick
 * Add bool IsValidSock()
 *
 * Revision 1.18  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.17  2004/01/09 17:43:32  dick
 * Define String localAddr = "127.0.0.1".  Rewind fwpl before use.
 * Deprecate OpenUDP(min port, max port) in favor of FirewallPortList.
 *
 * Revision 1.16  2004/01/06 04:48:28  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 * Revision 1.15  2003/07/13 05:06:18  dick
 * Copy the hostname, not a bad pointer to it
 *
 * Revision 1.14  2002/09/16 22:08:11  dick
 * Linux wants stringized conpar too
 *
 * Revision 1.13  2002/09/16 21:25:48  dick
 * Stringize SockGetLocalHostname().
 *
 * Revision 1.12  2002/08/28 13:30:53  dick
 * SendDest uses const
 *
 * Revision 1.11  2002/06/26 06:56:36  dick
 * use xperrno instead of errno, seterrno(0) instead of errno = 0.
 *
 * Revision 1.10  2002/06/26 05:03:55  dick
 * Force Windows to define errno as WSAGetLastError().
 * We should make an xperrno for portability purposes.
 *
 * Revision 1.9  2002/06/23 06:18:54  dick
 * Add the ability to set the W32 Window/message to select on.
 *
 * Revision 1.8  2001/08/26 09:46:30  dick
 * White space.
 *
 * Revision 1.7  2001/08/13 02:56:51  dick
 * Add PCSTR addrAny = "0.0.0.0";  NULL some values on contruct.
 *
 * Revision 1.6  2001/08/12 11:05:21  dick
 * Add support for opening a UDP port from a firewall port list.
 *
 * Revision 1.5  2001/08/11 13:18:42  dick
 * work on ServerOptionsNetClient
 *
 * Revision 1.4  2001/08/11 09:36:00  dick
 * New function OpenUDP(PCSTR dotaddr, int minPort, int maxPort) opens a port
 * within the specified range.
 *
 * Revision 1.3  2001/08/07 20:51:16  dick
 * Windows can't set errno=0, so wrap it in a macro.
 *
 * Revision 1.2  2001/08/06 06:00:46  dick
 * sock_t becomes Sock on Linux too...
 *
 * Revision 1.1  2001/08/06 05:41:50  dick
 * struct sock_t becomes class Sock
 *
 * Revision 1.5  2001/07/27 05:47:40  dick
 * Client builds and runs in C++
 *
 * Revision 1.4  2001/07/23 07:38:42  dick
 * Fold contact, metaserver, netserver, rules, and server to be members of World.
 *
 * Revision 1.3  2001/07/12 09:59:31  dick
 * Windows needs unsigned
 */
#include "StdAfx.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>

#if !defined(_WINDOWS) || defined(_CYGWIN)
# include <unistd.h>
# ifdef _AIX
#  include <sys/select.h> /* _BSD not defined in <sys/types.h>, so done by hand */
# endif
# include <sys/param.h>
# include <sys/ioctl.h>
# ifndef __hpux
#  include <sys/time.h>
# endif
# include <sys/socket.h>
# include <netinet/in.h>
# include <netinet/tcp.h>
# include <arpa/inet.h>
# include <netdb.h>
#endif

#ifdef SVR4
# include <sys/filio.h>
#endif

#ifdef _SEQUENT_
# include <sys/fcntl.h>
#else
# include <fcntl.h>
#endif

#ifdef __sun__
# include <arpa/nameser.h>
# include <resolv.h>
#endif

#if defined(_WINDOWS) && !defined(_CYGWIN)
# include "NT/winNet.h"
#undef	errno
#define	errno	WSAGetLastError()

// #include "../server/NT/winServer.h"
  /* Windows needs specific system calls for sockets: */
# undef close
# define close(x__) closesocket(x__)
# undef ioctl
# define ioctl(x__, y__, z__) ioctlsocket(x__, y__, (DWORD*)z__)
# undef read
# define read(x__, y__, z__) recv(x__, y__, z__,0)
# undef write
# define write(x__, y__, z__) send(x__, y__, z__,0)
#endif

#ifdef TERMNET
/* support for running clients over term, but not servers please. */
#include "termnet.h"
#endif

/* Socklib Includes And Definitions */
#include "version.h"
#include "portability.h"
#include "commonproto.h"
#include "error.h"
#include "Obj.h"
#include "Sock.h"
#include "sockerrs.h"

/* Debug macro */
#ifdef _DEBUG
# define DEB(x) x
#else
# define DEB(x)
#endif

#ifndef timerclear
# define timerclear(tvp)   ((tvp)->tv_sec = (tvp)->tv_usec = 0)
#endif



#define SOCK_GETHOST_TIMEOUT	6


char socklib_version[] = VERSION;

PCSTR	Sock::addrAny = "0.0.0.0";
String	localAddr = "127.0.0.1";

static jmp_buf		env;


static struct hostent *sock_get_host_by_name(const char *name);
static struct hostent *sock_get_host_by_addr(const char *addr, int len, int type);

///////////////////////////////////////////////////////////////////////////////
Sock::Sock()
{
	hostname = NULL;
	lastaddr = NULL;
	flags = 0;
	fd = SOCK_FD_INVALID;
#if defined(_WINDOWS) && !defined(_CYGWIN)
	hWnd = 0;
	wMsg = 0;
#endif
}

///////////////////////////////////////////////////////////////////////////////
#if defined(_WINDOWS) && !defined(_CYGWIN)
void Sock::SetWindowMsg(HWND hw, UINT m)
{
	hWnd = hw; 
	wMsg = m; 
}
#endif

///////////////////////////////////////////////////////////////////////////////
void Sock::FlagsAdd(unsigned bits)
{
	flags |= bits;
}

///////////////////////////////////////////////////////////////////////////////
void Sock::FlagsSet(unsigned bits)
{
	flags = bits;
}

///////////////////////////////////////////////////////////////////////////////
void Sock::FlagsRemove(unsigned bits)
{
	flags &= ~bits;
}

///////////////////////////////////////////////////////////////////////////////
int Sock::FlagsTestAll(unsigned bits)
{
	return (flags & bits) == (unsigned)bits;
}

///////////////////////////////////////////////////////////////////////////////
int Sock::FlagsTestAny(unsigned bits)
{
	return (flags & bits) != 0;
}

///////////////////////////////////////////////////////////////////////////////
int Sock::SetError(int _error, sock_call_t call, int line)
{
	DEB(printf("set error %d, %d, %d.  \"%s\"\n", _error, call, line, GetSockErrText(_error));)

	sockError.error = _error;
	sockError.call = call;
	sockError.line = line;

	return SOCK_IS_ERROR;
}

///////////////////////////////////////////////////////////////////////////////
PCSTR	sockCallType[] = {
		"any", "close", "socket", "fcntl", "io", "gethostbyname", "connect", "bind",
		"getsockname", "getsockopt", "setsockopt", "select" };

///////////////////////////////////////////////////////////////////////////////
String Sock::GetErrorString()
{
	String s;
	s.printf("Socket error %d (%s) on %s", 
			sockError.error, GetSockErrText(sockError.error),
			sockCallType[sockError.call < SOCK_CALL_MAX ? sockError.call : SOCK_CALL_MAX]);
	return(s);
}

///////////////////////////////////////////////////////////////////////////////
int Sock::Check()
{
	if (!FlagsTestAll(SOCK_FLAG_INIT))
	{
		return SetError(EINVAL, SOCK_CALL_ANY, __LINE__);
	}

	return SOCK_IS_OK;
}

///////////////////////////////////////////////////////////////////////////////
bool Sock::IsValidSock()
{
	return(fd != SOCK_FD_INVALID);
}

///////////////////////////////////////////////////////////////////////////////
int Sock::AllocHostname()
{
	if (!hostname) 
		{
				hostname = (char *) malloc(SOCK_HOSTNAME_LENGTH);
				if (!hostname)
						SetError(xperrno, SOCK_CALL_ANY, __LINE__);
				else
						hostname[0] = '\0';
	}
	return (hostname) ? SOCK_IS_OK : SOCK_IS_ERROR;
}

///////////////////////////////////////////////////////////////////////////////
void Sock::FreeHostname()
{
	if (hostname) {
		free(hostname);
		hostname = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
int Sock::AllocLastaddr()
{
	if (!lastaddr) {
		lastaddr = (void *) calloc(1, sizeof(struct sockaddr_in));
		if (!lastaddr) {
			SetError(xperrno, SOCK_CALL_ANY, __LINE__);
		}
	}

	return (lastaddr) ? SOCK_IS_OK : SOCK_IS_ERROR;
}

///////////////////////////////////////////////////////////////////////////////
void Sock::FreeLastaddr()
{
	if (lastaddr) {
		free(lastaddr);
		lastaddr = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
int Sock::Init()
{
	memset(this, 0, sizeof(Sock));

	FlagsSet(SOCK_FLAG_INIT);
	fd = SOCK_FD_INVALID;
	hostname = (char *) NULL;
	lastaddr = (void *) NULL;
	timeout.seconds = SOCK_TIMEOUT_SECONDS;

	return Check();
}

///////////////////////////////////////////////////////////////////////////////
int Sock::CloseTcp()
{
	int 				status = SOCK_IS_OK;

	if (close(fd) < 0) {
		SetError(xperrno, SOCK_CALL_CLOSE, __LINE__);
		status = SOCK_IS_ERROR;
	}
	FlagsRemove(SOCK_FLAG_TCP);
	fd = SOCK_FD_INVALID;

	return status;
}

///////////////////////////////////////////////////////////////////////////////
int Sock::CloseUdp()
{
	int 				status = SOCK_IS_OK;

	if (close(fd) < 0) {
		SetError(xperrno, SOCK_CALL_CLOSE, __LINE__);
		status = SOCK_IS_ERROR;
	}
	FlagsRemove(SOCK_FLAG_UDP);
	fd = SOCK_FD_INVALID;

	return status;
}

///////////////////////////////////////////////////////////////////////////////
int Sock::Close()
{
	FreeHostname();
	FreeLastaddr();
	if (FlagsTestAny(SOCK_FLAG_UDP)) {
		return CloseUdp();
	}
	if (FlagsTestAny(SOCK_FLAG_TCP)) {
		return CloseTcp();
	}
	return SetError(EINVAL, SOCK_CALL_ANY, __LINE__);
}

///////////////////////////////////////////////////////////////////////////////
int Sock::OpenTcp()
{
	if (Init()) {
		return SOCK_IS_ERROR;
	}

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		return SetError(xperrno, SOCK_CALL_SOCKET, __LINE__);
	}

	FlagsAdd(SOCK_FLAG_TCP);

	return SOCK_IS_OK;
}

///////////////////////////////////////////////////////////////////////////////
int Sock::SetNonBlocking(int _flag)
{
/*
 * There are some problems on some particular systems (suns) with
 * getting sockets to be non-blocking.  Just try all possible ways
 * until one of them succeeds.  Please keep us informed by e-mail
 * to xpilot@xpilot.org.
 */

#ifndef USE_FCNTL_O_NONBLOCK
# ifndef USE_FCNTL_O_NDELAY
#  ifndef USE_FCNTL_FNDELAY
#   ifndef USE_IOCTL_FIONBIO

#    if defined(_SEQUENT_) || defined(__svr4__) || defined(SVR4)
#     define USE_FCNTL_O_NDELAY
#    elif defined(__sun__) && defined(FNDELAY)
#     define USE_FCNTL_FNDELAY
#    elif defined(FIONBIO)
#     define USE_IOCTL_FIONBIO
#    elif defined(FNDELAY)
#     define USE_FCNTL_FNDELAY
#    elif defined(O_NONBLOCK)
#     define USE_FCNTL_O_NONBLOCK
#    else
#     define USE_FCNTL_O_NDELAY
#    endif

#    if 0
#     if defined(FNDELAY) && defined(F_SETFL)
#      define USE_FCNTL_FNDELAY
#     endif
#     if defined(O_NONBLOCK) && defined(F_SETFL)
#      define USE_FCNTL_O_NONBLOCK
#     endif
#     if defined(FIONBIO)
#      define USE_IOCTL_FIONBIO
#     endif
#     if defined(O_NDELAY) && defined(F_SETFL)
#      define USE_FCNTL_O_NDELAY
#     endif
#    endif

#   endif
#  endif
# endif
#endif

	char buf[128];

#if defined(_WINDOWS) && !defined(_CYGWIN)
	if (hWnd)
		return(WSAAsyncSelect(fd, hWnd, wMsg, FD_CLOSE|FD_READ));
#endif

#ifdef USE_FCNTL_FNDELAY
	if (fcntl(fd, F_SETFL, (_flag != 0) ? FNDELAY : 0) != -1) {
		return SOCK_IS_OK;
	}
	SetError(errno, SOCK_CALL_FCNTL, __LINE__);
	sprintf(buf, "fcntl FNDELAY failed in socklib.c line %d", __LINE__);
	perror(buf);
#endif

#ifdef USE_IOCTL_FIONBIO
	if (ioctl(fd, FIONBIO, &_flag) == 0) {
		return SOCK_IS_OK;
	}
	SetError(xperrno, SOCK_CALL_FCNTL, __LINE__);
	sprintf(buf, "ioctl FIONBIO failed in socklib.c line %d", __LINE__);
	perror(buf);
#endif

#ifdef USE_FCNTL_O_NONBLOCK
	if (fcntl(fd, F_SETFL, (_flag != 0) ? O_NONBLOCK : 0) != -1) {
		return SOCK_IS_OK;
	}
	SetError(xperrno, SOCK_CALL_FCNTL, __LINE__);
	sprintf(buf, "fcntl O_NONBLOCK failed in socklib.c line %d", __LINE__);
	perror(buf);
#endif

#ifdef USE_FCNTL_O_NDELAY
	if (fcntl(fd, F_SETFL, (_flag != 0) ? O_NDELAY : 0) != -1) {
		return SOCK_IS_OK;
	}
	SetError(xperrno, SOCK_CALL_FCNTL, __LINE__);
	sprintf(buf, "fcntl O_NDELAY failed in socklib.c line %d", __LINE__);
	perror(buf);
#endif

	return SOCK_IS_ERROR;
}

///////////////////////////////////////////////////////////////////////////////
int Sock::OpenTcpConnectedNonBlocking(char *host, int port)
{
	struct sockaddr_in	dest;
	struct hostent		*hp;

	if (OpenTcp()) {
		return SOCK_IS_ERROR;
	}

	/*
	 * On error a message will have been printed
	 * and we want to continue regardless.
	 */
	SetNonBlocking(1);

	memset(&dest, 0, sizeof(dest));
	dest.sin_family 	 = AF_INET;
	dest.sin_port		 = htons((unsigned short)port);
	dest.sin_addr.s_addr = inet_addr(host);
	if ((dest.sin_addr.s_addr & 0xFFFFFFFF) == 0xFFFFFFFF) {
		/*
		 * Cannot use h_errno because of portability problems.
		 * Let's hope errno is meaningful too.
		 */
		IFNWINDOWS(errno = 0;)
		if ((hp = sock_get_host_by_name(host)) == NULL) {
			SetError(xperrno, SOCK_CALL_GETHOSTBYNAME, __LINE__);
			Close();
			return SOCK_IS_ERROR;
		}

		dest.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr_list[0]))->s_addr;
	}

#if defined(_UNIX) || defined(_CYGWIN)
	if (connect(fd, (struct sockaddr *)&dest, sizeof(struct sockaddr_in)) < 0
		&& errno != EINPROGRESS)
	{
		SetError(xperrno, SOCK_CALL_CONNECT, __LINE__);
		Close();
		return SOCK_IS_ERROR;
	}
#endif

	FlagsAdd(SOCK_FLAG_CONNECT);

	return SOCK_IS_OK;
}

///////////////////////////////////////////////////////////////////////////////
int Sock::OpenUdp(PCSTR dotaddr, int port)
{
	struct sockaddr_in	addr;

	if (Init()) 
	{
		return SOCK_IS_ERROR;
	}

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
	{
		return SetError(xperrno, SOCK_CALL_SOCKET, __LINE__);
	}

	FlagsAdd(SOCK_FLAG_UDP);

	memset(&addr, 0, sizeof(addr));
	addr.sin_family 	 = AF_INET;
	addr.sin_port		 = htons((unsigned short)port);
	addr.sin_addr.s_addr = (dotaddr) ? inet_addr(dotaddr) : INADDR_ANY;
	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		SetError(xperrno, SOCK_CALL_BIND, __LINE__);
		Close();
		return SOCK_IS_ERROR;
	}

	return SOCK_IS_OK;
}

///////////////////////////////////////////////////////////////////////////////
int Sock::Connect(const char *host, int port)
{
	struct sockaddr_in			dest;
	struct hostent				*hp;

	memset(&dest, 0, sizeof(dest));
	dest.sin_family = AF_INET;
	dest.sin_port = htons((unsigned short) port);
	dest.sin_addr.s_addr = inet_addr(host);
	if ((dest.sin_addr.s_addr & 0xFFFFFFFF) == 0xFFFFFFFF)
	{
		IFNWINDOWS( errno = 0; )
		if ((hp = sock_get_host_by_name(host)) == NULL) 
		{
			SetError(xperrno, SOCK_CALL_GETHOSTBYNAME, __LINE__);
			return SOCK_IS_ERROR;
		}
		dest.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr_list[0]))->s_addr;
	}

	if (connect(fd, (struct sockaddr *)&dest, sizeof(dest)) < 0)
				return SetError(xperrno, SOCK_CALL_CONNECT, __LINE__);

	FlagsAdd(SOCK_FLAG_CONNECT);

	return SOCK_IS_OK;
}

///////////////////////////////////////////////////////////////////////////////
int Sock::GetLastPort()
{
	struct sockaddr_in* _lastaddr;

	if (lastaddr) {
		_lastaddr = (struct sockaddr_in *)(lastaddr);
		return ntohs(_lastaddr->sin_port);
	}

	SetError(EINVAL, SOCK_CALL_ANY, __LINE__);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
const char* Sock::GetLastAddr()
{
	static char 		error_addr[] = "255.255.255.255";
	char				*str;
	struct sockaddr_in* 		_lastaddr;

	if (lastaddr) {
		_lastaddr = (struct sockaddr_in *)(lastaddr);
		str = inet_ntoa(_lastaddr->sin_addr);
		if (AllocHostname()) {
			return str;
		}
		strlcpy(hostname, str, SOCK_HOSTNAME_LENGTH);
		return hostname;
	}

	SetError(EINVAL, SOCK_CALL_ANY, __LINE__);

	return error_addr;
}

///////////////////////////////////////////////////////////////////////////////
const char* Sock::GetLastName()
{
	static PCSTR		error_addr = "255.255.255.255";
	PCSTR		str;
	struct hostent		*hp;
	struct sockaddr_in* _lastaddr;

	if (lastaddr)
	{
		_lastaddr = (struct sockaddr_in *)(lastaddr);
		hp = sock_get_host_by_addr((char *)&(_lastaddr->sin_addr),
								   sizeof(_lastaddr->sin_addr), AF_INET);
		if (hp == NULL) {
			str = inet_ntoa(_lastaddr->sin_addr);
		} else {
			str = hp->h_name;
		}
		if (AllocHostname() == SOCK_IS_OK)
		{
			strlcpy(hostname, str, SOCK_HOSTNAME_LENGTH);
			return hostname;
		}
	}

	SetError(EINVAL, SOCK_CALL_ANY, __LINE__);

	return error_addr;
}

///////////////////////////////////////////////////////////////////////////////
int Sock::Read(char *buf, int len)
{
	int 				count;

	count = recv(fd, buf, len, 0);
	if (count < 0) {
		SetError(xperrno, SOCK_CALL_IO, __LINE__);
	}

	return count;
}

///////////////////////////////////////////////////////////////////////////////
int Sock::ReceiveAny(char *buf, int len)
{
	int 				count;
#if !defined(_WINDOWS) && !defined(_CYGWIN)
	uint	addrlen;
#else
	int 	addrlen;
#endif

	if (AllocLastaddr()) {
		return SOCK_IS_ERROR;
	}
	addrlen = sizeof(struct sockaddr_in);
	count = recvfrom(fd, buf, len, 0, (struct sockaddr *)(lastaddr), &addrlen);
	if (count < 0) {
		SetError(xperrno, SOCK_CALL_IO, __LINE__);
	}

	return count;
}

///////////////////////////////////////////////////////////////////////////////
int Sock::SendDest(PCSTR host, int port, PCSTR buf, int len)
{
	struct sockaddr_in			dest;
	struct hostent				*hp;
	int 						count;

	memset(&dest, 0, sizeof(dest));
	dest.sin_family = AF_INET;
	dest.sin_port = htons((unsigned short) port);
	dest.sin_addr.s_addr = inet_addr(host);
	if ((dest.sin_addr.s_addr & 0xFFFFFFFF) == 0xFFFFFFFF) {
		IFNWINDOWS(errno = 0;)
		if ((hp = sock_get_host_by_name(host)) == NULL) {
			return SetError(xperrno, SOCK_CALL_GETHOSTBYNAME, __LINE__);
		}

		dest.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr_list[0]))->s_addr;
	}

	count = sendto(fd, buf, len, 0, (struct sockaddr *) &dest, sizeof(dest));
	if (count < 0) {
		SetError(xperrno, SOCK_CALL_IO, __LINE__);
	}

	return count;
}

///////////////////////////////////////////////////////////////////////////////
int Sock::Write(char *buf, int len)
{
	int 				count;

	count = send(fd, buf, len, 0);
	if (count < 0)
				SetError(xperrno, SOCK_CALL_IO, __LINE__);
	return count;
}

///////////////////////////////////////////////////////////////////////////////
String sock_get_addr_by_name(PCSTR name)
{
	String	s;
	struct hostent		*hp;

	hp = sock_get_host_by_name(name);

	if (hp)
		s = inet_ntoa(*(struct in_addr *)(hp->h_addr_list[0]));
	DEB(printf("sock_get_addr_by_name: name = '%s' addr = '%s'\n", (PCSTR)name, (PCSTR)s);)
	return s;
}

///////////////////////////////////////////////////////////////////////////////
unsigned long sock_get_inet_by_addr(char *dotaddr)
{
	return inet_addr(dotaddr);
}

///////////////////////////////////////////////////////////////////////////////
void SockGetLocalHostname(String& hostname, int search_domain_for_xpilot)
{
	struct hostent* 	he = NULL;
	struct hostent* 	xpilot_he = NULL;
#if defined(_UNIX) || defined(_CYGWIN)
	int 		xpilot_len;
	char*		dot;
	char		xpilot_hostname[SOCK_HOSTNAME_LENGTH];
#endif
	static const char	xpilot[] = "xpilot";
	char	name[256];
	name[0] = '\0';

	gethostname(name, 256);
	hostname = name;
	DEB(printf("hostname.1 = '%s'\n", (PCSTR)name);)
	if ((he = sock_get_host_by_name(name)) == NULL) {
		return;
	}
	strlcpy(name, he->h_name, 256);
	hostname = name;
	DEB(printf("hostname.2 = '%s'\n", (PCSTR)name);)

	/*
	 * If there are no dots in the name then we don't have the FQDN,
	 * and if the address is of the normal Internet type
	 * then we try to get the FQDN via the backdoor of the IP address.
	 * Let's hope it works :)
	 */
	if (strchr(he->h_name, '.') == NULL
		&& he->h_addrtype == AF_INET) 
	{
		struct in_addr in;
		memcpy((void *)&in, he->h_addr_list[0], sizeof(in));
		if ((he = sock_get_host_by_addr((char *)&in, sizeof(in), AF_INET)) != NULL
			&& strchr(he->h_name, '.') != NULL) 
		{
			strlcpy(name, he->h_name, 256);
		}
		else {
			/* Let's try to find the domain from /etc/resolv.conf. */
			FILE* fp = fopen("/etc/resolv.conf", "r");
			if (fp) {
				char*	s;
				char	buf[256];
				while (fgets(buf, sizeof buf, fp)) 
				{
					if ((s = strtok(buf, " \t\r\n")) != NULL
						&& !strcmp(s, "domain")
						&& (s = strtok(NULL, " \t\r\n")) != NULL) 
					{
						strcat(name, ".");
						strcat(name, s);
						break;
					}
				}
				fclose(fp);
			}
		}
		/* make sure this is a valid FQDN. */
		if ((he = sock_get_host_by_name(name)) == NULL) {
			gethostname(name, 256);
			hostname = name;
			return;
		}
	}

	if (search_domain_for_xpilot != 1) {
		hostname = name;
		return;
	}

#if defined(_UNIX) || defined(_CYGWIN)

	/* if name starts with "xpilot" then we're done. */
	xpilot_len = strlen(xpilot);
	if (!strncmp(name, xpilot, xpilot_len)) {
		return;
	}

	/* Make a wild guess that a "xpilot" hostname or alias is in this domain */
	dot = name;
	while ((dot = strchr(dot, '.')) != NULL) {
		if (xpilot_len + strlen(dot) < sizeof(xpilot_hostname)) {
			strlcpy(xpilot_hostname, xpilot, SOCK_HOSTNAME_LENGTH);
			strlcat(xpilot_hostname, dot, SOCK_HOSTNAME_LENGTH);
			/*
			 * If there is a CNAME the h_name must be identical to the
			 * FQDN we guessed above.  It is hard to know our IP to know
			 * that an A record points to us.
			 */
			if ((xpilot_he = sock_get_host_by_name(xpilot_hostname)) != NULL 
				&& !strcmp(name, xpilot_he->h_name))
				break;
			xpilot_he = NULL;
		}
		++dot;
	}
	if (xpilot_he != NULL) {
		strncpy(name, xpilot_hostname, 256);
	}
	hostname = name;

#endif
}

///////////////////////////////////////////////////////////////////////////////
int Sock::GetPort()
{
	struct sockaddr_in	addr;
#if !defined(_WINDOWS) && !defined(_CYGWIN)
	unsigned
#endif
								int 	len = sizeof(addr);
	unsigned short		port;

	if (getsockname(fd, (struct sockaddr *)&addr, &len) < 0) {
		SetError(xperrno, SOCK_CALL_GETSOCKNAME, __LINE__);
		return SOCK_IS_ERROR;
	}

	port = ntohs(addr.sin_port);

	return port;
}

///////////////////////////////////////////////////////////////////////////////
int Sock::GetError()
{
	int 				_error;
#if !defined(_WINDOWS) && !defined(_CYGWIN)
	unsigned
#endif
								int 	size = sizeof(_error);

	if (getsockopt(fd, SOL_SOCKET, SO_ERROR, (char*)&_error, &size) < 0) {
		SetError(xperrno, SOCK_CALL_GETSOCKOPT, __LINE__);
		return SOCK_IS_ERROR;
	}
	IFNWINDOWS(errno = _error;)
	return SOCK_IS_OK;
}

///////////////////////////////////////////////////////////////////////////////
int Sock::SetBroadcast(int flag)
{
	if (setsockopt(fd, SOL_SOCKET, SO_BROADCAST,
				   (const char *)&flag, sizeof(flag)) < 0) {
		SetError(xperrno, SOCK_CALL_SETSOCKOPT, __LINE__);
		return SOCK_IS_ERROR;
	}
	return SOCK_IS_OK;
}

///////////////////////////////////////////////////////////////////////////////
int Sock::SetReceiveBufferSize(int size)
{
	if (setsockopt(fd, SOL_SOCKET, SO_RCVBUF,
				   (const char *)&size, sizeof(size)) < 0) {
		SetError(xperrno, SOCK_CALL_SETSOCKOPT, __LINE__);
		return SOCK_IS_ERROR;
	}
	return SOCK_IS_OK;
}

///////////////////////////////////////////////////////////////////////////////
int Sock::SetSendBufferSize(int size)
{
	if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF,
				   (const char *)&size, sizeof(size)) < 0) {
		SetError(xperrno, SOCK_CALL_SETSOCKOPT, __LINE__);
		return SOCK_IS_ERROR;
	}
	return SOCK_IS_OK;
}

///////////////////////////////////////////////////////////////////////////////
int Sock::SetTimeout(int seconds, int useconds)
{
	timeout.seconds = seconds;
	timeout.useconds = useconds;

	return SOCK_IS_OK;
}

///////////////////////////////////////////////////////////////////////////////
int Sock::Readable()
{
	int 				n;
	fd_set				readfds;
	struct timeval		_timeout;

	timerclear(&_timeout); /* macro */
	_timeout.tv_sec = timeout.seconds;
	_timeout.tv_usec = timeout.useconds;

	FD_ZERO(&readfds);
	FD_SET(fd, &readfds);

	n = select(fd + 1, &readfds, NULL, NULL, &_timeout);
	if (n == -1) {
		if (xperrno != EINTR) {
			SetError(xperrno, SOCK_CALL_SELECT, __LINE__);
			return SOCK_IS_ERROR;
		}
		return SOCK_IS_OK;
	}

	if ((n > 0) && FD_ISSET(fd, &readfds)) {
		return 1;
	}

	return SOCK_IS_OK;
}

///////////////////////////////////////////////////////////////////////////////
static void sock_catch_alarm(int signum)
{
	printf("DNS lookup cancelled\n");

	longjmp(env, 1);
}

///////////////////////////////////////////////////////////////////////////////
static struct hostent *sock_get_host_by_name(const char *name)
{
#ifndef _SOCKWINDOWS			// new fltk routines don't use notifyWnd
	struct hostent		*hp;

#if defined(_WINDOWS) || defined(_CYGWIN)
		hp = gethostbyname(name);
#else


	if (setjmp(env)) {
		alarm(0);
		signal(SIGALRM, SIG_DFL);
		return (struct hostent *) NULL;
	}

	signal(SIGALRM, sock_catch_alarm);
	alarm(SOCK_GETHOST_TIMEOUT);

	hp = gethostbyname(name);

	alarm(0);
	signal(SIGALRM, SIG_DFL);
#endif
	return hp;

#else
	
	/*
	 * If you aren't connected to the net, then gethostbyname()
	 * can take many minutes to time out.  WSACancelBlockingCall()
	 * doesn't affect it.
	 */
	
	static char 	chp[MAXGETHOSTSTRUCT+1];
	struct hostent* hp = (struct hostent*)&chp;
	HANDLE h;
	MSG msg;
	int i;
	
	h = WSAAsyncGetHostByName(notifyWnd, WM_GETHOSTNAME, name, 
		chp, MAXGETHOSTSTRUCT);
	
	for(i = 0; i < SOCK_GETHOST_TIMEOUT; i++) {
		if (PeekMessage(&msg, NULL, WM_GETHOSTNAME, WM_GETHOSTNAME, PM_REMOVE)) {
			return (WSAGETASYNCERROR(msg.lParam)) ? NULL : hp;
		}
		Sleep(1000);
	}
	WSACancelAsyncRequest(h);
	return NULL;

#endif
}

///////////////////////////////////////////////////////////////////////////////
static struct hostent *sock_get_host_by_addr(const char *addr, int len, int type)
{
#if defined(_UNIX) || defined(_CYGWIN)

	struct hostent		*hp;

	if (setjmp(env)) {
		alarm(0);
		signal(SIGALRM, SIG_DFL);
		return (struct hostent *) NULL;
	}

	signal(SIGALRM, sock_catch_alarm);
	alarm(SOCK_GETHOST_TIMEOUT);

	hp = gethostbyaddr(addr, len, type);

	alarm(0);
	signal(SIGALRM, SIG_DFL);

	return hp;

#else

	struct hostent		*hp;

	hp = gethostbyaddr(addr, len, type);

	return hp;

#endif
}

///////////////////////////////////////////////////////////////////////////////
int Sock::OpenUdp(PCSTR dotaddr, FirewallPortList& fwpl)
{
	int 	port;

	fwpl.Rewind();
	while (fwpl.GetNextPort(port))
	{
		if (OpenUdp(NULL, port) != SOCK_IS_ERROR)
		{
			return(SOCK_IS_OK);
		}
	}
	error("Could not find a usable port in given port range");
	return(SOCK_IS_ERROR);
}

/*
deprecated.  Use FirewallPortList version
int Sock::OpenUdp(PCSTR dotaddr, int minPort, int maxPort)
{
	if (!minPort || !maxPort || (minPort > maxPort))
	{
		if (OpenUdp(NULL, 0) == SOCK_IS_ERROR)
		{
			error("Cannot create datagram socket (%d)", sockError.error);
			return(SOCK_IS_ERROR);
		}
		return(SOCK_IS_OK);
	}
	else
	{
		for (int i = minPort; i <= maxPort; i++)
		{
			if (OpenUdp(NULL, i) != SOCK_IS_ERROR)
			{
				return(SOCK_IS_OK);
			}
		}
		error("Could not find a usable port in given port range");
		return(SOCK_IS_ERROR);
	}
}
*/
