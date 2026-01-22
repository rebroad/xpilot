/* $Id: Sock.h,v 1.13 2004/02/06 03:38:51 dick Exp $
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

#ifndef _SOCK_H_
#define _SOCK_H_

#if defined(_WINDOWS) && !defined(_CYGWIN)
#include <windows.h>
#endif

#include "types.h"
#include "cstring.h"
#include "Obj.h"
#include "FirewallPortList.h"



#define SOCK_HOSTNAME_LENGTH	256
#define SOCK_FD_INVALID		(-1)
#define SOCK_IS_ERROR		(-1)
#define SOCK_IS_OK		(0)
#define SOCK_TIMEOUT_SECONDS	3

enum sock_flags_e {
    SOCK_FLAG_INIT	= 1,
    SOCK_FLAG_UDP	= 2,
    SOCK_FLAG_TCP	= 4,
    SOCK_FLAG_CONNECT	= 8
};

typedef enum sock_call_e {
    SOCK_CALL_ANY,
    SOCK_CALL_CLOSE,
    SOCK_CALL_SOCKET,
    SOCK_CALL_FCNTL,
    SOCK_CALL_IO,
    SOCK_CALL_GETHOSTBYNAME,
    SOCK_CALL_CONNECT,
    SOCK_CALL_BIND,
    SOCK_CALL_GETSOCKNAME,
    SOCK_CALL_GETSOCKOPT,
    SOCK_CALL_SETSOCKOPT,
    SOCK_CALL_SELECT,
	SOCK_CALL_MAX
} sock_call_t;

typedef struct sock_timeout_s {
    long		seconds;
    unsigned long	useconds;
} sock_timeout_t;

typedef struct sock_error_s {
    int			error;
    int			call;
    int			line;
} sock_error_t;

class Sock
{
public:
	Sock();
	int		Init();
	int		Close();
	int		SetNonBlocking(int flag);
	int		OpenTcpConnectedNonBlocking(char *host, int port);
	int		OpenUdp(PCSTR dotaddr, int port);
	//int		OpenUdp(PCSTR dotaddr, int minPort, int maxPort); // deprecated
	int		OpenUdp(PCSTR dotaddr, FirewallPortList& fwpl);
	int		Connect(const char *host, int port);
	int		GetLastPort();
	PCSTR	GetLastAddr();
	PCSTR	GetLastName();
	int		Read(char *buf, int len);
	int		ReceiveAny(char *buf, int len);
	int		SendDest(PCSTR host, int port, PCSTR buf, int len);
	int		Write(char *buf, int len);
	int		GetPort();
	int		GetError();
	String	GetErrorString();
	int		SetBroadcast(int flag);
	int		SetReceiveBufferSize(int size);
	int		SetSendBufferSize(int size);
	int		SetTimeout(int seconds, int useconds);
	int		Readable();
	bool	IsValidSock();

    int				fd;
    sock_timeout_t	timeout;
    unsigned		flags;
    sock_error_t	sockError;
    void*			lastaddr;
    char*			hostname;
#if defined(_WINDOWS) && !defined(_CYGWIN)
	void	SetWindowMsg(HWND hw, UINT m);
	HWND	hWnd;			// handle to window to receive selects
	UINT	wMsg;			// msg of interest
#endif
	static	PCSTR	addrAny;	/* = "0.0.0.0"; */


private:
	void	FlagsAdd(unsigned bits);
	void	FlagsSet(unsigned bits);
	void	FlagsRemove(unsigned bits);
	int		FlagsTestAll(unsigned bits);
	int		FlagsTestAny(unsigned bits);
	int		SetError(int _error, sock_call_t call, int line);
	int		Check();
	int		AllocHostname();
	void	FreeHostname();
	int		AllocLastaddr();
	void	FreeLastaddr();
	int		OpenTcp();
	int		CloseTcp();
	int		CloseUdp();

};

String	sock_get_addr_by_name(const char *name);
ulong	sock_get_inet_by_addr(char *dotaddr);
void	SockGetLocalHostname(String& host, int search_domain_for_xpilot);

extern	String	localAddr;

#endif	// _SOCK_H_
