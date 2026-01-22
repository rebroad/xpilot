/* $Id: NetServerScoreServer.cpp,v 1.5 2004/05/07 04:38:25 dick Exp $
 *
 * ScoreServerNetContact - You talkin' to me?
 *
 * XPScoreServer - Who's on first?
 *
 *      Copyright (C) 2004 by
 *
 *      The XPilot Authors   <xpilot@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
 *      Jarrod Miller        <jarrod@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
 *      Bjørn Stabell        <bjoern@xpilot.org>
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
 * $Log: NetServerScoreServer.cpp,v $
 * Revision 1.5  2004/05/07 04:38:25  dick
 * Send rank data to the server.
 *
 * Revision 1.4  2004/05/04 21:07:38  dick
 * Need randommnt.h
 *
 * Revision 1.3  2004/04/22 15:42:25  dick
 * gcc3 needs string.h
 *
 * Revision 1.2  2004/04/21 23:28:44  dick
 * Startup a UDP server (via NetServer) that allows xp5 to connect to us,
 * monitor our health, and shut us down.
 *
 * Revision 1.1  2004/04/16 21:07:59  dick
 * NetServer becomes a little UDP engine that can be plugged into different apps.
 *
 */
#include "StdAfx.h"
#include <errno.h>
#include <string.h>

#include "version.h"
#include "config.h"
#include "types.h"
#include "serverconst.h"
#include "Sock.h"
#include "pack.h"
#include "bit.h"
#include "Sockbuf.h"
#include "commonproto.h"
#include "randommt.h"
#include "showtime.h"

#include "error.h"
#include "xpprintf.h"
#include "const.h"
#include "portability.h"
#include "sockerrs.h"

#include "ScoreServer.h"
#include "NetServer.h"
#include "ScoreServerCfg.h"
#include "NetServerScoreServer.h"
#include "ConnectionLocalCtl.h"

#if 0					// Debug Contact
#define DC(__d) D(__d)
#else
#define DC(__d)
#endif

FirewallPortList	firewallPortList;

///////////////////////////////////////////////////////////////////////////////
void NetServerScoreServer::ContactCleanup()
{
//	if (remoteContactSocket.IsValidSock())
//	{
//		netServer->RemoveInput(remoteContactSocket.fd);
//		remoteContactSocket.Close();
//	}
	if (localContactSocket.IsValidSock())
	{
		netServer->RemoveInput(localContactSocket.fd);
		localContactSocket.Close();
	}

}

///////////////////////////////////////////////////////////////////////////////
static int Reply(Sockbuf& ibuf, char *host_addr, int port)
{
	int 				i, result = -1;
	const int			max_send_retries = 3;

	for (i = 0; i < max_send_retries; i++) {
		if ((result = ibuf.sock.SendDest(host_addr, port, ibuf.buf, ibuf.len)) == -1) {
			ibuf.sock.GetError();
		} else {
			break;
		}
	}

	return result;
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Support some older clients, which don't know
 * that they can join the current version.
 *
 * IMPORTANT! Adjust the next code if you're changing version numbers.
 */
static unsigned Version_to_magic(unsigned version)
{
	if (version >= 0x3100 && version <= MY_VERSION) {
		return VERSION2MAGIC(version);
	}
	return MAGIC;
}

///////////////////////////////////////////////////////////////////////////////
bool NetServerScoreServer::ContactInit()
{
	if (ContactInitPort(localContactBuf, localContactSocket, localAddr, SCORE_SERVER_PORT))
	{
		netServer->InstallInput(&NetServerScoreServer::ContactFromLocal, localContactSocket.fd,
			(void *)this);
		return(true);
	}
	return(false);
}

///////////////////////////////////////////////////////////////////////////////
void NetServerScoreServer::ContactFromLocal(int fd, void* v)
{
	NetServerScoreServer* ns = (NetServerScoreServer*)v;
	DC(xpprintf("%s got localhost Contact\n", showtime());)
	ns->Contact(ns->localContactBuf);
}

///////////////////////////////////////////////////////////////////////////////
void NetServerScoreServer::Contact(Sockbuf& ibuf)
{
//	int 				i,
//						team,
	int					bytes;
//						delay,
	int					login_port;
//						qpos,
	int					status;
	char				reply_to;
	uint				magic;
	uint				version;
	uint				origVersion;
	uint				my_magic;
	unsigned short		port;
	char				ch;
	char				disp_name[MAX_CHARS];
	char				host_addr[24];
	char				str[MSG_LEN];
	String				nick_name;
	String				host_name;
	String				real_name;

	Sock& contactSocket = ibuf.sock;
	/*
	 * Someone connected to us, now try and decipher the message :)
	 */
	ibuf.Clear();
	if ((bytes = contactSocket.ReceiveAny(ibuf.buf, ibuf.size)) <= 8) {
		int myErrno = xperrno;
		D(xpprintf("%sContact: error = %s\n", showtime(), GetSockErrText(myErrno));)
		if (bytes < 0
			&& myErrno != EWOULDBLOCK
			&& myErrno != EAGAIN
			&& myErrno != EINTR) {
			/*
			 * Clear the error condition for the contact socket.
			 */
			contactSocket.GetError();
		}
		return;
	}
	ibuf.len = bytes;

	strlcpy(host_addr, contactSocket.GetLastAddr(), sizeof(host_addr));
//	if (Check_address(host_addr)) {
//		return;
//	}

	/*
	 * Determine if we can talk with this client.
	 */
	if (ibuf.scanf("%u", &magic) <= 0
		|| (magic & 0xFFFF) != (MAGIC & 0xFFFF)) {
		D(xpprintf("%sIncompatible packet from %s (0x%08x)\n", showtime(), host_addr, magic);)
		return;
	}
	version = MAGIC2VERSION(magic);
	origVersion = version;

	// Handle version forks between 4.5.1 and 5.0.0
	if (version > 0x4501 && version < 0x5000)
		version = 0x4501; 
	/*
	 * Read core of packet.
	 */
	if (ibuf.scanf("%s%hu%c", str, &port, &ch) <= 0) {
		D(xpprintf("%sIncomplete packet from %s\n", showtime(), host_addr);)
		return;
	}
	real_name = str;
//	FixRealName(real_name);
	reply_to = (ch & 0xFF); 	/* no sign extension. */

	/* ignore port for termified clients. */
	port = contactSocket.GetLastPort();

	/*
	 * Now see if we have the same (or a compatible) version.
	 * If the client request was only a contact request (to see
	 * if there is a server running on this host) then we don't
	 * care about version incompatibilities, so that the client
	 * can decide if it wants to conform to our version or not.
	 */
	if (version < MIN_CLIENT_VERSION
		|| (version > MAX_CLIENT_VERSION
			&& reply_to != CONTACT_pack)) {
		DC(xpprintf("%s Incompatible version with %s@%s (%04x,%04x)\n",
			  showtime(), (PCSTR)real_name, host_addr, MY_VERSION, version);)
		ibuf.Clear();
		ibuf.printf("%u%c%c", MAGIC, reply_to, E_VERSION);
		Reply(ibuf, host_addr, port);
		return;
	}

	my_magic = Version_to_magic(version);

	status = SUCCESS;

#if 0
	if (reply_to & PRIVILEGE_PACK_MASK) {
		long					key;
		static long 			credentials = 0;

		if (!credentials) {
			credentials = (time(NULL) * (time_t)GetProcessId());
			credentials ^= (long)(void*)&credentials;
			credentials += (long)key + (long)&key;
			credentials ^= (long)randomMT() << 1;
			credentials &= 0xFFFFFFFF;
		}
		if (ibuf.scanf("%ld", &key) <= 0) {
			return;
		}
		if (reply_to == PASSWORD_pack)
		{
						
			if (ibuf.scanf("%s", &str) <= 0)
				return;
//			if (options.password->GetString() == str)
			if (1)		// contact from only localhost is always success
			{
				ibuf.printf("%u%c%c%ld", my_magic, reply_to, SUCCESS, credentials);
				Reply(ibuf, host_addr, port);
				return;
			}
			else
			{
				ibuf.printf("%u%c%c%ld", my_magic, reply_to, E_PASSWORD);
				Reply(ibuf, host_addr, port);
				return;
			}

		}
		if (!IsOwner(reply_to, real_name, host_addr, port, key == credentials)) {
			ibuf.Clear();
			ibuf.printf("%u%c%c", my_magic, reply_to, E_NOT_OWNER);
			Reply(ibuf, host_addr, port);
			return;
		}
		if (reply_to == CREDENTIALS_pack) {
			ibuf.Clear();
			ibuf.printf("%u%c%c%ld", my_magic, reply_to, E_SUCCESS, credentials);
			Reply(ibuf, host_addr, port);
			return;
		}
	}
#endif
	/*
	 * Now decode the packet type field and do something witty.
	 */
	switch (reply_to) {

	case CONTACT_pack:	{
		/*
		 * Got contact message from client.
		 */

		D(xpprintf("%sGot CONTACT from %s.\n", showtime(), host_addr);)
		ibuf.Clear();
		ibuf.printf("%u%c%c", my_magic, reply_to, status);
		break;
	}

	case CONNECT_CTL_pack: {
		char	_n[MAX_CHARS];
		char	_h[MAX_CHARS];
		if (ibuf.scanf("%s%s%s%s", _n, disp_name, _h, str) <= 0)
		{
			D(xpprintf("Incomplete control login from %s@%s\n", (PCSTR)real_name, host_addr);)
			return;
		}
		nick_name = _n;
		host_name = _h;
		//if (localAddr != host_addr && options.password->GetString() != str)
		if (0) // contact is from only localhost and is always SUCCESS
		{
				ibuf.Clear();
				ibuf.printf("%u%c%c%ld", my_magic, reply_to, E_PASSWORD);
				Reply(ibuf, host_addr, port);
				xpprintf(LOGMED, "%s%s@%s failed the control password connecting a %s\n",
					showtime(), (PCSTR)real_name, host_addr, disp_name);
				return;
		}

		xpprintf(LOGMIN, "%s%s@%s is connecting a %s control\n",
				   showtime(), (PCSTR)real_name, host_addr, disp_name);
		status = SetupConnectionControl(real_name, nick_name, disp_name,
														  host_addr, host_name, &login_port,
														  version, origVersion);
		ibuf.Clear();
		ibuf.printf("%u%c%c%hu", my_magic, reply_to, status, login_port);

		break;
	}
	default:
		/*
		 * Incorrect packet type.
		 */
		D(xpprintf("%sUnknown packet type (0x%X) from %s@%s.\n",
			showtime(), reply_to, (PCSTR)real_name, host_addr);)

		ibuf.Clear();
		ibuf.printf("%u%c%c", my_magic, reply_to, E_VERSION);
	}

	Reply(ibuf, host_addr, port);
}

///////////////////////////////////////////////////////////////////////////////
int NetServerScoreServer::SetupConnectionControl(PCSTR real, PCSTR nick, PCSTR dpy,
												 PCSTR addr, PCSTR host, int* port,
												 uint version, uint origVersion)
{
	int			i;
	int			free_conn_index = netServer->maxConnections;
	int			my_port;
	Sock		sock;
	Connection*	connp;

	for (i = 0; i < netServer->maxConnections; i++)
	{
		connp = (Connection*)netServer->conn[i];
		if (!connp)	//  || connp->state == CONN_FREE) 	// DIK: checking _FREE is wrong, since we will leak this conn with the new below
		{
			if (free_conn_index == netServer->maxConnections) 
			{
				free_conn_index = i;
			}
			continue;
		}
	}

	if (free_conn_index >= netServer->maxConnections) {
#ifndef SILENT
		xpprintf("%sFull house for %s(%s)@%s\n", showtime(), real, nick, host);
#endif
		return E_GAME_FULL;
	}

	/////////////////////////////////////////////
	// Create the connection
	if (!strcmp(dpy, "localctl"))
	{
		if (localAddr != addr)
		{
			xpprintf("%s%s tried to connect a localctl\n", showtime(), addr);
			sock.Close();
			return(E_NOTLOCAL);
		}
		connp = new ConnectionLocalCtl;
	}
	else
		return(E_UNKNOWNCTL);

	netServer->conn[free_conn_index] = connp;

	if (sock.OpenUdp(localAddr, firewallPortList) == SOCK_IS_ERROR)
	{
		// error already reported
		return E_SERVER;
	}

	if ((my_port = sock.GetPort()) == -1) {
	error("Cannot get port from socket");
	sock.Close();
	return E_SERVER;
	}
	if (sock.SetNonBlocking(1) == -1) {
		error("Cannot make client socket non-blocking");
		sock.Close();
		return E_SERVER;
	}
	*port = my_port;

	if (sock.SetReceiveBufferSize(SERVER_RECV_SIZE + 256) == -1) {
		error("Cannot set receive buffer size to %d", SERVER_RECV_SIZE + 256);
	}
	if (sock.SetSendBufferSize(SERVER_SEND_SIZE + 256) == -1) {
		error("Cannot set send buffer size to %d", SERVER_SEND_SIZE + 256);
	}

	if (!connp->InitBuffers(&sock))
	{
		error("Not enough memory for connection");
		/* socket is not yet connected, but it doesn't matter much. */
		connp->DestroyConnection("no memory");
		return -1;
	}
	connp->myInd = free_conn_index;
	connp->my_port = my_port;
	connp->real = real;
	connp->nick = nick;
	connp->addr = addr;
	connp->host = host;
	connp->dpy  = dpy;
	connp->version = version;
	connp->origVersion = version;
	connp->magic = randomMT() + my_port + sock.fd + netServer->mainLoops;
	netServer->InstallInput(&Connection::HandleInput, sock.fd, connp);

	return E_SUCCESS;
}

