/* $Id: ControlClient.cpp,v 1.34 2005/03/17 22:12:13 kps Exp $
 *
 * ControlClient - a network control to give us come client side commands
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2001 by
 *
 *      Dick Balaska         <dick@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
 *      Bj�rn Stabell        <bjoern@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
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
 * $Log: ControlClient.cpp,v $
 * Revision 1.34  2005/03/17 22:12:13  kps
 * Get rid of warnings from makedepend about "non-portable whitespace".
 *
 * Revision 1.33  2004/09/14 00:25:42  dick
 * SendPlayerMessage() needs to be a long message.
 *
 * Revision 1.32  2004/06/03 02:22:55  dick
 * Init reliable counters to 0
 *
 * Revision 1.31  2004/05/13 09:11:50  dick
 * Whitespace
 *
 * Revision 1.30  2004/05/07 04:36:33  dick
 * Get rank data from the scoreserver to the server to the client
 *
 * Revision 1.29  2004/02/05 04:47:44  dick
 * New command ServerInfo so the scoreserver can get our hostname
 *
 * Revision 1.28  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.27  2004/02/02 18:47:39  dick
 * XMLArchive.h
 *
 * Revision 1.26  2004/01/14 14:20:03  dick
 * Fix message (not really quitting, eh?)
 *
 * Revision 1.25  2004/01/09 21:56:39  dick
 * Init the fwpl during construction
 *
 * Revision 1.24  2004/01/06 04:48:27  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 * Revision 1.23  2002/09/16 22:07:30  dick
 * Linux wants stringized conpar too
 *
 * Revision 1.22  2002/09/16 21:27:50  dick
 * Use Stringized Connectparam
 *
 * Revision 1.21  2002/09/09 23:32:07  dick
 * Add a handler for the WorldInfo subcommand.
 *
 * Revision 1.20  2002/08/01 14:55:57  dick
 * real_name becomes realName, etc.
 *
 * Revision 1.19  2002/07/23 17:55:16  dick
 * virtual Connected(), a callback when a successful connection has been made.
 *
 * Revision 1.18  2002/07/17 23:54:30  dick
 * SendPlayerMessage() broadcasts to all players (player==NULL) or to a player.
 *
 * Revision 1.17  2002/07/12 15:44:36  dick
 * Comments
 *
 * Revision 1.16  2002/07/09 22:55:26  dick
 * xpprintf uses showtime()
 *
 * Revision 1.15  2002/07/08 06:27:19  dick
 * Receive and set the origin along with the option.
 *
 * Revision 1.14  2002/07/01 07:03:37  dick
 * Init retransmit_at_loop to 0 during constructor.
 *
 * Revision 1.13  2002/06/30 06:24:32  dick
 * SendWantPlayMessages(bool want), tell the server to send us Play messages.
 *
 * Revision 1.12  2002/06/26 06:56:36  dick
 * use xperrno instead of errno, seterrno(0) instead of errno = 0.
 *
 * Revision 1.11  2002/06/26 04:52:07  dick
 * ControlClient does not use Fltk
 *
 * Revision 1.10  2002/06/25 04:23:48  dick
 * Add two control receive events
 *
 * Revision 1.9  2002/06/23 06:16:05  dick
 * Move FLTK interfaces out; add ability to set the Windows window and message
 * that we need for 'select()'ing.
 *
 */
#include "StdAfx.h"

#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>

#ifdef _WINDOWS
# include <io.h>
#endif

#if defined(_WINDOWS) && !defined(_CYGWIN)
#include "NT/winNet.h"
#endif

#include "config.h"
#include "xpprintf.h"
#include "const.h"
#include "packet.h"
#include "microdelay.h"
#include "commonproto.h"
#include "portability.h"
#include "error.h"
#include "checknames.h"
#include "ControlClient.h"
#include "PacketCtl.h"
//#include "ServerOptions.h"

#define MAX_RELIABLE_DATA_PACKET_SIZE	1024

/////////////////////////////////////////
// These time values are all number of seconds
#define	KEEPALIVEDELAY	5.0
//#define	TIMER_INTERVAL	(1.0/12.0)

#define	MIN_RETRANSMIT	0.2
#define	MAX_RETRANSMIT	1.0
#define MAX_RTT			1.0


typedef	int	(ControlClient::*ControlClientReceiveFunc)();

static	ControlClientReceiveFunc	receiveTbl[256];
static	ControlClientReceiveFunc	reliableTbl[256];
static	ControlClientReceiveFunc	ctlTbl[256];

static	bool virgin = true;

#if 0					// Debug Reliable
#define	DR(__x)	D(__x)
#else
#define	DR(__x)
#endif

///////////////////////////////////////////////////////////////////////////////
ControlClient::ControlClient()
: fwpl(normalFirewall)
{
	if (virgin)
	{
		virgin = false;
		int		i;
		for (i=0; i<256; i++)
			receiveTbl[i] = NULL;
		for (i=0; i<256; i++)
			reliableTbl[i] = NULL;
		for (i=0; i<256; i++)
			ctlTbl[i] = NULL;

	    receiveTbl[PKT_QUIT]		= &ControlClient::ReceiveQuit;
	    receiveTbl[PKT_ACK]			= &ControlClient::ReceiveAck;
	    receiveTbl[PKT_RELIABLE]	= &ControlClient::ReceiveReliable;

	    reliableTbl[PKT_QUIT]		= &ControlClient::ReceiveQuit;
	    reliableTbl[PKT_MESSAGE]	= &ControlClient::ReceivePlayMessage;
	    reliableTbl[PKT_CTL]		= &ControlClient::ReceiveCtl;

		ctlTbl[SetOption]			= &ControlClient::ReceiveSetOption;
		ctlTbl[OptionCount]			= &ControlClient::ReceiveOptionCount;
		ctlTbl[GetScore]			= &ControlClient::ReceiveGetScore;
		ctlTbl[GetRanks]			= &ControlClient::ReceiveGetRanks;
		ctlTbl[SendScore]			= &ControlClient::ReceiveSetScore;
		ctlTbl[PlayerEvent]			= &ControlClient::ReceivePlayerEvent;
		ctlTbl[ScoreEvent]			= &ControlClient::ReceiveScoreEvent;
		ctlTbl[WorldInfo]			= &ControlClient::ReceiveWorldInfo;
		ctlTbl[ServerInfo]			= &ControlClient::ReceiveServerInfo;
	}
	time(&lastSpoken);
	sos = NULL;
	main_loops = 0;
	reliable_offset = 0;
    reliable_unsent = 0;
	last_send_loops = 0;
	rtt_timeouts = 0;
	acks = 0;
	connected = false;
	rtt_retransmit = MIN_RETRANSMIT;
	retransmit_at_loop = 0;

#if defined(_WINDOWS) && !defined(_CYGWIN)
	hWnd = NULL;
	wMsg = NULL;
#endif
}

///////////////////////////////////////////////////////////////////////////////
ControlClient::~ControlClient()
{
	Disconnect();
}


///////////////////////////////////////////////////////////////////////////////
void ControlClient::SetErrMsgHandler(ErrMsgHandler _emh, void* _emhThis)
{
	emh = _emh;
	emhThis = _emhThis;
	rbuf.SetErrMsgHandler(emh, emhThis);
	wbuf.SetErrMsgHandler(emh, emhThis);
	crbuf.SetErrMsgHandler(emh, emhThis);
	cwbuf.SetErrMsgHandler(emh, emhThis);
}

///////////////////////////////////////////////////////////////////////////////
void	ControlClient::SetFirewallPortList(FirewallPortList& _fwpl)
{
	fwpl = _fwpl;
};

///////////////////////////////////////////////////////////////////////////////
void ControlClient::SetControlType(PCSTR type)
{
	conpar.dispName = type;
}

///////////////////////////////////////////////////////////////////////////////
#if defined(_WINDOWS) && !defined(_CYGWIN)
void ControlClient::SetWindowMsg(HWND hw, UINT m)
{
	hWnd = hw;
	wMsg = m;
}
#endif

///////////////////////////////////////////////////////////////////////////////
bool ControlClient::Connect(PCSTR server, int port, PCSTR password)
{
	char*	cp;


	cp = getenv("XPILOTUSER");
	if (cp)
		conpar.realName = cp;
	else
		GetLoginName(conpar.realName);
	if (CheckRealName(conpar.realName) == NAME_ERROR)
	{
		emh(emhThis, EmInfo, "fixing name from \"%s\" ", (PCSTR)conpar.realName);
		FixRealName(conpar.realName);
		emh(emhThis, EmInfo, "to \"%s\"\n", (PCSTR)conpar.realName);
	}

	emh(emhThis, EmInfo, "Connecting to %s:%d", (PCSTR)server, port);
	if (!ContactServer(server, port))
	{
		NetCleanup();
		return(false);
	}
	if (!LoginToServer(password))
	{
		NetCleanup();
		return(false);
	}
	if (!NetInit())
	{
		NetCleanup();
		return(false);
	}
	if (!NetVerify())
	{
		emh(emhThis, EmError, "Failed to verify connection");
		NetCleanup();
		return(false);
	}

	connected = true;
	emh(emhThis, EmInfo, "Connected");
	Connected();

	return(true);
}

///////////////////////////////////////////////////////////////////////////////
bool ControlClient::Disconnect()
{
	NetCleanup();
	main_loops = 0;
	reliable_offset = 0;
	rtt_timeouts = 0;
	acks = 0;
	connected = false;
	rtt_retransmit = MIN_RETRANSMIT;
	return(true);
}

///////////////////////////////////////////////////////////////////////////////
bool ControlClient::ContactServer(PCSTR serverName, int serverPort)
{
    const int		max_retries = 2;
	int		retries	= 0;
    bool	contacted = false;
	Sockbuf	sbuf;
	Sock	sock;

	emh(emhThis, EmInfo, "ContactServer: %s:%d", serverName, serverPort);

	if (sock.OpenUdp(Sock::addrAny, fwpl) == SOCK_IS_ERROR)
	{
		emh(emhThis, EmError, "Can't create contact socket");
		return(false);
	}
    if (sbuf.Init(&sock, CLIENT_RECV_SIZE,
		     SOCKBUF_READ | SOCKBUF_WRITE | SOCKBUF_DGRAM) == -1)
	{
		emh(emhThis, EmError, "Can't create contact Sockbuf");
		return(false);
    }

	contactServer = serverName;
	conpar.serverName = contactServer;
	SockGetLocalHostname(myHostName, 0);
	conpar.contactPort = serverPort;
	do
	{
		sbuf.Clear();
		sbuf.printf("%u%s%hu%c", MAGIC,
		(PCSTR)conpar.realName, sock.GetPort(), CONTACT_pack);
		if (sock.SendDest(conpar.serverName,
						  conpar.contactPort,
						  sbuf.buf, sbuf.len) == -1)
		{
			if (sock.sockError.call == SOCK_CALL_GETHOSTBYNAME)
			{
				emh(emhThis, EmError, "Can't find %s\n", (PCSTR)conpar.serverName);
				break;
			}
			emh(emhThis, EmError, "Can't contact %s on port %d\n",
			(PCSTR)conpar.serverName, conpar.contactPort);
		}
		if (retries)
		{
			emh(emhThis, EmInfo, "Retrying %s...%d\n",
								 (PCSTR)conpar.serverName, retries);
		}
		if (GetContactMessage(sbuf))
		{
			contacted = true;

		}
	} while (!contacted && retries++ < max_retries);
	// Cleanup();
	// sock.Close();

	return contacted;
}

///////////////////////////////////////////////////////////////////////////////
bool ControlClient::GetContactMessage(Sockbuf& sbuf)
{
    int			_len;
    int			version, allow;
    unsigned		magic;
    unsigned char	reply_to, status;
    bool		readable = false;

    sbuf.sock.SetTimeout(2, 0);
    while (readable == false && sbuf.sock.Readable() > 0) {

	sbuf.Clear();
	_len = sbuf.sock.ReceiveAny(sbuf.buf, sbuf.size);
	if (_len <= 0) {
	    if (_len == 0) {
		continue;
	    }
	    emh(emhThis, EmError, "Error from sock_receive_any, contact message failed.");
	    /* exit(1);  no good since meta gui. */
	    return false;
	}
	sbuf.len = _len;

	/*
	 * Get server's host and port.
	 */
	conpar.serverAddr = sbuf.sock.GetLastAddr();
	conpar.serverPort = sbuf.sock.GetLastPort();
	/*
	 * If the name of the server to contact is the same as its
	 * IP address then we don't want to do a reverse lookup.
	 * Doing a reverse lookup may result in a long and annoying delay.
	 */
	if (!strcmp(conpar.serverAddr, contactServer)) {
	    conpar.serverName = conpar.serverAddr;
	} else {
	    conpar.serverName = sbuf.sock.GetLastName();
	}

	if (sbuf.scanf("%u%c%c", &magic, &reply_to, &status) <= 0) {
	    seterrno(0);
	    emh(emhThis, EmError, "Incomplete contact reply message (%d)", _len);
	}
	else if ((magic & 0xFFFF) != (MAGIC & 0xFFFF)) {
	    seterrno(0);
	    emh(emhThis, EmError, "Bad magic on contact message (0x%x).", magic);
	}
	else {
	    allow = version = MAGIC2VERSION(magic);
	    LIMIT(allow, MIN_SERVER_VERSION, MAX_SERVER_VERSION);
	    if (version != allow) {
		xpprintf("%sIncompatible version with server %s.\n",
			showtime(), (PCSTR)conpar.serverName);
		xpprintf("%sWe run version %04x, while server is running %04x.\n", showtime(),
		       MY_VERSION, MAGIC2VERSION(magic));
		if ((MY_VERSION >> 4) < (MAGIC2VERSION(magic) >> 4)) {
		    xpprintf("Time for us to upgrade?\n");
		}
	    }
	    else {
		/*
		 * Found one which we can talk to.
		 */
		conpar.serverVersion = version;
		readable = true;
	    }
	}
    }

    return readable;
}

///////////////////////////////////////////////////////////////////////////////
int ControlClient::GetReplyMessage(Sockbuf& sbuf)
{
    int			_len;
    unsigned		magic;


    if (sbuf.sock.Readable()) {
	sbuf.Clear();
	if ((_len = sbuf.sock.Read(sbuf.buf, sbuf.size)) == -1) {
	    emh(emhThis, EmError, "Can't read reply message from %s/%d",
		  (PCSTR)conpar.serverAddr, conpar.serverPort);
	    return(0);
	}

	sbuf.len = _len;
	if (sbuf.scanf("%u", &magic) <= 0) {
	    seterrno(0);
	    emh(emhThis, EmError, "Incomplete reply packet (%d)", _len);
	    return 0;
	}

	if ((magic & 0xFFFF) != (MAGIC & 0xFFFF)) {
	    seterrno(0);
	    emh(emhThis, EmError, "Wrong MAGIC in reply pack (0x%x).", magic);
	    return 0;
	}

	if (MAGIC2VERSION(magic) != conpar.serverVersion) {
	    xpprintf("%sIncompatible version with server on %s.\n",
			showtime(), (PCSTR)conpar.serverName);
	    xpprintf("%sWe run version %04x, while server is running %04x.\n", showtime(),
		   MY_VERSION, MAGIC2VERSION(magic));
	    return 0;
	}

	return sbuf.len;
    }

    return 0;
}


///////////////////////////////////////////////////////////////////////////////
/*
 * Setup a socket and a buffer for client-server messages.
 * We do this again for each server to prevent getting
 * old messages from past servers.
 */
bool ControlClient::LoginToServer(PCSTR password)
{
	Sockbuf		sbuf;

	key = 0;
    if (sbuf.Init(NULL, CLIENT_RECV_SIZE,
		     SOCKBUF_READ | SOCKBUF_WRITE | SOCKBUF_DGRAM) == -1)
	{
		emh(emhThis, EmError, "No memory for info buffer");
		return(false);
    }
//	if ((success = create_dgram_socket(&ibuf->sock, 0)) == SOCK_IS_ERROR)
	if (sbuf.sock.OpenUdp(conpar.serverAddr, fwpl) == SOCK_IS_ERROR)
	{
		emh(emhThis, EmError, "Could not create socket %s",
							  (PCSTR)sbuf.sock.GetErrorString());
		return(false);
	}
	if (sbuf.sock.Connect(conpar.serverAddr, conpar.serverPort) == SOCK_IS_ERROR)
	{
		emh(emhThis, EmError, "Can't connect to server %s on port %d\n",
							  (PCSTR)conpar.serverAddr, conpar.serverPort);
		return(false);
	}

	sbuf.Clear();
	sbuf.printf("%u%s%hu", VERSION2MAGIC(conpar.serverVersion),
				 (PCSTR)conpar.realName, sbuf.sock.GetPort());

	sbuf.printf("%c%s%s%s%s", CONNECT_CTL_pack,
		  (PCSTR)conpar.nick, (PCSTR)conpar.dispName,
		  (PCSTR)myHostName, password);


	int retries = 2;
	for (int i = 0; i <= retries; i++)
	{
		if (i > 0)
		{
			sbuf.sock.SetTimeout(1, 0);
			if (sbuf.sock.Readable())
				break;
		}
		if (sbuf.sock.Write(sbuf.buf, sbuf.len) != sbuf.len)
		{
			emh(emhThis, EmError, "Couldn't send request to server.");
			return(false);
		}
	}

	/*
	 * Get reply message(s).  If we failed, return false (next server).
	 */
	sbuf.sock.SetTimeout(2, 0);
	int max_replies = 1;
	do
	{
		char	reply_to;
		char	status;
	    char	linebuf[MSG_LEN];

	    sbuf.Clear();
	    if (GetReplyMessage(sbuf) <= 0)
		{
			seterrno(0);
			emh(emhThis, EmError, "No answer from server");
			return false;
	    }
	    if (sbuf.scanf("%c%c", &reply_to, &status) <= 0)
		{
			seterrno(0);
			emh(emhThis, EmError, "Incomplete reply from server");
			return false;
	    }

	    sbuf.sock.SetTimeout(0, 500*1000);

	    /*
	     * Now try and interpret the result.
	     */
	    seterrno(0);
	    switch (status)
		{
		case SUCCESS:
			/*
			 * Oh glorious success.
			 */
			switch (reply_to & 0xFF)
			{

			case OPTION_LIST_pack:
				while (sbuf.scanf("%S", linebuf) > 0)
				{
					xpprintf("%s\n", linebuf);
				}
				break;
			case CREDENTIALS_pack:
				if (sbuf.scanf("%ld", &key) <= 0)
				{
					seterrno(0);
					emh(emhThis, EmError, "Incomplete credentials reply from server");
				}
				else
				{
//					has_credentials++;
//					cmd_credentials = c;
					continue;
				}
				break;
			case CONNECT_CTL_pack:
				int port;
				port = 0;
				if (sbuf.scanf("%hu", &port) <= 0)
				{
					seterrno(0);
					emh(emhThis, EmError, "Incomplete login reply from server");
					conpar.loginPort = -1;
				}
				else
					conpar.loginPort = port;
				//printf("*** Login allowed.\n");
				break;
			default:
				//puts("*** Operation successful.");
				break;
			}			// end switch (reply_to)
			break;
		default:
			emh(emhThis, EmError, GetErrorString(status));
			return(false);

	    }

	    if (reply_to == CONNECT_CTL_pack)
		{
			if (status == SUCCESS && conpar.loginPort > 0)
				return true;
			return(false);
	    }

	} while (--max_replies > 0 && sbuf.sock.Readable());


    /*NOTREACHED*/

    if (sbuf.sock.fd != SOCK_FD_INVALID)
	{
		sbuf.sock.Close();
		sbuf.sock.fd = SOCK_FD_INVALID;
    }
    sbuf.Cleanup();
	return(false);
}

///////////////////////////////////////////////////////////////////////////////
String ControlClient::GetErrorString(int status)
{
	switch (status)
	{
    case E_NOT_OWNER:
		return("Permission denied, not owner");
    case E_GAME_FULL:
		return("Sorry, game full");
    case E_TEAM_FULL:
		return("Sorry, team is full");
    case E_TEAM_NOT_SET:
		return("Sorry, team play selected "
		      "and you haven't specified your team");
    case E_GAME_LOCKED:
		return("Sorry, game locked");
    case E_NOT_FOUND:
		return("That player is not logged on this server");
    case E_IN_USE:
		return("Your nick is already used");
    case E_SOCKET:
		return("Server can't setup socket");
    case E_INVAL:
		return("Invalid input parameters says the server");
    case E_VERSION:
		return("We have an incompatible version says the server");
    case E_NOENT:
		return("No such variable, says the server");
	case E_PASSWORD:
		return("Bad password says the server");
	case E_SERVER:
		return("Server made a boo-boo");
    case E_UNDEFINED:
		return("Requested operation is undefined, says the server");
    default:
		return("Server answers with unknown error status '%02x'", status);
	}
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Open the datagram socket and allocate the network data
 * structures like buffers.
 * Currently there are three different buffers used:
 * 1) wbuf is used only for sending packets (write/printf).
 * 2) rbuf is used for receiving packets in (read/scanf).
 * 3) cbuf is used to copy the reliable data stream
 *    into from the raw and unreliable rbuf packets.
 *
 * server == NULL sets up for internal simulation
 */
bool ControlClient::NetInit()
{

#if defined(_UNIX) || defined(_CYGWIN)
    signal(SIGPIPE, SIG_IGN);
#endif

	if (sock.OpenUdp(NULL, fwpl) == SOCK_IS_ERROR)
		return(false);

#if defined(_WINDOWS) && !defined(_CYGWIN)
	sock.SetWindowMsg(hWnd, wMsg);
#endif

    if (sock.Connect(conpar.serverAddr, conpar.loginPort) == -1) {
	emh(emhThis, EmError, "Can't connect to server %s on port %d\n",
		(PCSTR)conpar.serverName, conpar.loginPort);
	sock.Close();
	return false;
    }
    wbuf.sock = sock;
    if (sock.SetNonBlocking(1) == -1) {
	emh(emhThis, EmError, "Can't make socket non-blocking");
	return false;
    }
    if (sock.SetSendBufferSize(CLIENT_SEND_SIZE + 256) == -1) {
	emh(emhThis, EmError, "Can't set send buffer size to %d", CLIENT_SEND_SIZE + 256);
    }
    if (sock.SetReceiveBufferSize(CLIENT_RECV_SIZE + 256) == -1) {
	emh(emhThis, EmError, "Can't set receive buffer size to %d", CLIENT_RECV_SIZE + 256);
    }

	main_loops = 0;
	reliable_offset = 0;
	rtt_timeouts = 0;
	acks = 0;
	rtt_retransmit = MIN_RETRANSMIT;


    /* reliable data buffer, not a valid socket filedescriptor needed */
    if (cwbuf.Init(NULL, CLIENT_RECV_SIZE,
		     SOCKBUF_WRITE | SOCKBUF_READ | SOCKBUF_LOCK) == -1) {
	emh(emhThis, EmError, "No memory for control buffer (%u)", CLIENT_RECV_SIZE);
	return false;
    }

    if (crbuf.Init(NULL, CLIENT_RECV_SIZE,
		     SOCKBUF_WRITE | SOCKBUF_READ | SOCKBUF_LOCK) == -1) {
	emh(emhThis, EmError, "No memory for control buffer (%u)", CLIENT_RECV_SIZE);
	return false;
    }

    /* write buffer */
    if (wbuf.Init(&sock, CLIENT_SEND_SIZE,
		     SOCKBUF_WRITE | SOCKBUF_DGRAM) == -1) {
	emh(emhThis, EmError, "No memory for write buffer (%u)", CLIENT_SEND_SIZE);
	return false;
    }

    /* read buffer */
    if (rbuf.Init(&sock, CLIENT_SEND_SIZE,
		     SOCKBUF_READ | SOCKBUF_DGRAM) == -1) {
	emh(emhThis, EmError, "No memory for read buffer (%u)", CLIENT_RECV_SIZE);
	return false;
    }

    /* reliable data byte stream offset */
	reliableOffset = 0;

    return true;
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Cleanup all the network buffers and close the datagram socket.
 * Also try to send the server a quit packet if possible.
 * Because this quit packet may get lost we send one at the
 * beginning and one at the end.
 */
void ControlClient::NetCleanup()
{
    Sock	sock = wbuf.sock;
    char	ch;

    if (connected && sock.fd > 2)
	{
		ch = PKT_QUIT;
		if (sock.Write(&ch, 1) != 1)
		{
			sock.GetError();
			sock.Write(&ch, 1);
		}
		microdelay((unsigned)50*1000);
	}
	connected = false;
    crbuf.Cleanup();
    cwbuf.Cleanup();
	rbuf.Cleanup();
    wbuf.Cleanup();
	if (sock.fd > 2)
	{
		ch = PKT_QUIT;
		if (sock.Write(&ch, 1) != 1)
		{
			sock.GetError();
			sock.Write(&ch, 1);
		}
		microdelay((unsigned)50*1000);
		if (sock.Write(&ch, 1) != 1)
		{
			sock.GetError();
			sock.Write(&ch, 1);
		}
		sock.Close();
	}
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Send the first packet to the server with our name,
 * nick and display contained in it.
 * The server uses this data to verify that the packet
 * is from the right UDP connection, it already has
 * this info from the ENTER_GAME_pack.
 */
#define	MAX_VERIFY_RETRIES	5
bool ControlClient::NetVerify()
{
    int		n;
	int		type;
	int		result;
	int		retries;
    time_t	last = time(NULL);;

    for (retries = 0;;) {
	if (retries == 0
	    || time(NULL) - last >= 3) {
	    if (retries++ >= MAX_VERIFY_RETRIES) {
		seterrno(0);
		emh(emhThis, EmError, "Can't connect to server after %d retries", retries);
		return false;
	    }
	    wbuf.Clear();
	    n = wbuf.printf("%c%s%s%s%u", PKT_VERIFY,
			(PCSTR)conpar.realName, (PCSTR)conpar.nick, (PCSTR)conpar.dispName, 0);
	    if (n <= 0
		|| wbuf.Flush() <= 0) {
		emh(emhThis, EmError, "Can't send verify packet");
		return false;
	    }
	    time(&last);
#ifndef SILENT
	    if (retries > 1) {
		emh(emhThis, EmInfo, "Waiting for verify response");
	    }
#endif
	}
	rbuf.sock.SetTimeout(1, 0);
	if (rbuf.sock.Readable() == 0) {
	    continue;
	}
	rbuf.Clear();
	if (rbuf.Read() == -1) {
	    emh(emhThis, EmError, "Can't read verify reply packet");
	    return false;
	}
	if (rbuf.len <= 0) {
	    continue;
	}
	if (rbuf.ptr[0] != PKT_RELIABLE) {
	    if (rbuf.ptr[0] == PKT_QUIT) {
		seterrno(0);
		emh(emhThis, EmError, "Server closed connection");
		return false;
	    } else {
		seterrno(0);
		emh(emhThis, EmError, "Bad packet type when verifying (%d)", rbuf.ptr[0]);
		return false;
	    }
	}
	if (ReceiveReliable() == -1) {
	    return false;
	}
	if (wbuf.Flush() == -1) {
	    return false;
	}
	if (crbuf.len == 0) {
	    continue;
	}
	if (ReceiveReply(&type, &result) <= 0) {
		seterrno(0);
	    emh(emhThis, EmError, "Can't receive verify reply packet");
	    return false;
	}
	if (type != PKT_VERIFY) {
		seterrno(0);
	    emh(emhThis, EmError, "Verify wrong reply type (%d)", type);
	    return false;
	}
	if (result != PKT_SUCCESS) {
		seterrno(0);
	    emh(emhThis, EmError, "Verification failed (%d)", result);
	    return false;
	}
	if (ReceiveMagic() <= 0) {
	    emh(emhThis, EmError, "Can't receive magic packet after verify");
	    return false;
	}
	break;
    }
#ifndef SILENT
    if (retries > 1) {
	emh(emhThis, EmInfo, "Verified correctly\n");
    }
#endif
	IHaveSpoken();
    return true;
}

///////////////////////////////////////////////////////////////////////////////
int ControlClient::SendAck(long rel_loops)
{
	int			n;
	DR(Trace("SendAck: %d\n", rel_loops);)
	if ((n = wbuf.printf("%c%ld%ld", PKT_ACK, reliableOffset, rel_loops)) <= 0)
	{
		if (n == 0)
			return 0;
		emh(emhThis, EmError, "Can't ack reliable data");
		return -1;
	}
	return 1;
}

///////////////////////////////////////////////////////////////////////////////
int ControlClient::ReceiveAck()
{
	int			n;
	unsigned char	ch;
	long		rel,
	rtt,	/* RoundTrip Time */
	diff,
	delta,
	rel_loops;

	if ((n = rbuf.scanf("%c%ld%ld", &ch, &rel, &rel_loops)) <= 0)
	{
		seterrno(0);
		error("Cannot read ack packet (%d)", n);
		// DestroyConnection("read error");
		return -1;
	}
	DR(Trace("ReceiveAck: %d %d ", rel, rel_loops);)
	if (ch != PKT_ACK)
	{
		seterrno(0);
		DR(Trace("died\n");)
		error("Not an ack packet (%d)", ch);
		// DestroyConnection("not ack");
		return -1;
	}
	rtt = main_loops - rel_loops;
	if (rtt > 0 && rtt <= MAX_RTT)
	{
	   /*
		* These roundtrip estimation calculations are derived from Comer's
		* books "Internetworking with TCP/IP" parts I & II.
		*/
		if (rtt_smoothed == 0)
		{
		   /*
			* Initialize the rtt estimator by this first measurement.
			* The estimator is scaled by 3 bits.
			*/
			rtt_smoothed = rtt << 3;
		}
	   /*
		* Scale the estimator back by 3 bits before calculating the error.
		*/
		delta = rtt - (rtt_smoothed >> 3);
	   /*
		* Add one eigth of the error to the estimator.
		*/
		rtt_smoothed += delta;
	   /*
		* Now we need the absolute value of the error.
		*/
		if (delta < 0)
		{
			delta = -delta;
		}
	   /*
		* The rtt deviation is scaled by 2 bits.
		* Now we add one fourth of the difference between the
		* error and the previous deviation to the deviation.
		*/
		rtt_dev += delta - (rtt_dev >> 2);
	   /*
		* The calculation of the retransmission timeout is what this is
		* all about.  We take the smoothed rtt plus twice the deviation
		* as the next retransmission timeout to use.  Because of the
		* scaling used we get the following statement:
		*/
		rtt_retransmit = ((rtt_smoothed >> 2) + rtt_dev) >> 1;
		/*
		* Now keep it within reasonable bounds.
		*/
		if (rtt_retransmit < MIN_RETRANSMIT)
		{
			rtt_retransmit = (int)(TIMER_INTERVAL/MIN_RETRANSMIT);		// 0.1 / 0.2 = 0.5 = 0
		}
	}
	diff = rel - reliable_offset;
	if (diff > cwbuf.len)
	{
		/* Impossible to ack data that has not been send */
		seterrno(0);
		error("Bad ack (diff=%ld,cru=%ld,c=%ld,len=%d)",
			  diff, rel, reliable_offset, cwbuf.len);
		// DestroyConnection("bad ack");
		return -1;
	}
	else if (diff <= 0)
	{
		/* Late or duplicate ack of old data.  Discard. */
		DR(Trace("duplicate (@%d)\n", reliable_offset);)
		return 1;
	}
	cwbuf.Advance((int) diff);
	reliable_offset += diff;
	if ((n = ((diff + 512 - 1) / 512)) > acks)
	{
		acks = n;
	}
	else
	{
		acks++;
	}
	if (reliable_offset >= reliable_unsent)
	{
	   /*
		* All reliable data has been sent and acked.
		*/
		retransmit_at_loop = 0;
		//if (state == CONN_DRAIN)
		//	SetState(drainState, drainState);
	}
//	if (state == CONN_READY
//		&& (cw.len <= 0
//		|| (cw.buf[0] != PKT_REPLY
//		&& cw.buf[0] != PKT_PLAY
//		&& cw.buf[0] != PKT_SUCCESS
//		&& cw.buf[0] != PKT_FAILURE)))
//	{
//		SetState(drainState, drainState);
//	}
	rtt_timeouts = 0;
	DR(Trace("ok\n");)
	return 1;
}


///////////////////////////////////////////////////////////////////////////////
int ControlClient::ReceiveReliable(void)
{
	int			n;
	short		len;
	u_byte		ch;
	long		rel,
	rel_loops;

	if ((n = rbuf.scanf("%c%hd%ld%ld",
						&ch, &len, &rel, &rel_loops)) == -1)
	{
		return -1;
	}
	if (n == 0)
	{
		seterrno(0);
		emh(emhThis, EmError, "Incomplete reliable data packet");
		return 0;
	}
#	if DEBUG
	if (reliableOffset >= rel + len)
	{
		Trace("Reliable my=%ld pkt=%ld len=%d loops=%ld\n",
			  reliableOffset, rel, len, rel_loops);
	}
#	endif
	if (len <= 0)
	{
		seterrno(0);
		emh(emhThis, EmError, "Bad reliable data length (%d)", len);
		return -1;
	}
	if (rbuf.ptr + len > rbuf.buf + rbuf.len)
	{
		seterrno(0);
		emh(emhThis, EmWarning, "Not all reliable data in packet (%d,%d,%d)",
								rbuf.ptr - rbuf.buf, len, rbuf.len);
		rbuf.ptr += len;
		rbuf.Advance(rbuf.ptr - rbuf.buf);
		return -1;
	}
	if (rel > reliableOffset)
	{
		/*
		* We miss one or more packets.
		* For now we drop this packet.
		* We could have kept it until the missing packet(s) arrived.
		*/
		rbuf.ptr += len;
		rbuf.Advance(rbuf.ptr - rbuf.buf);
		if (SendAck(rel_loops) == -1)
			return -1;
		wbuf.Flush();
		return 1;
	}
	if (rel + len <= reliableOffset)
	{
		/*
		* Duplicate data.  Probably an ack got lost.
		* Send an ack for our current stream position.
		*/
		rbuf.ptr += len;
		rbuf.Advance(rbuf.ptr - rbuf.buf);
		if (SendAck(rel_loops) == -1)
			return -1;
		wbuf.Flush();
		return 1;
	}
	if (rel < reliableOffset)
	{
		len -= (short)(reliableOffset - rel);
		rbuf.ptr += reliableOffset - rel;
		rel = reliableOffset;
	}
	if (crbuf.ptr > crbuf.buf)
		crbuf.Advance(crbuf.ptr - crbuf.buf);

	if (crbuf.Write(rbuf.ptr, len) != len)
	{
		seterrno(0);
		emh(emhThis, EmWarning, "Can't copy reliable data to buffer");
		rbuf.ptr += len;
		rbuf.Advance(rbuf.ptr - rbuf.buf);
		return -1;
	}
	reliableOffset += len;
	rbuf.ptr += len;
	rbuf.Advance(rbuf.ptr - rbuf.buf);
	if (SendAck(rel_loops) == -1)
		return -1;
	wbuf.Flush();
	return 1;
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Send the reliable data.
 * Stolen from the server, so this comment is inaccurate...
 * If the client is in the receive-frame-updates state then
 * all reliable data is piggybacked at the end of the
 * frame update packets.  (Except maybe for the MOTD data, which
 * could be transmitted in its own packets since MOTDs can be big.)
 * Otherwise if the client is not actively playing yet then
 * the reliable data is sent in its own packets since there
 * is no other data to combine it with.
 *
 * This thing still is not finished, but it works better than in 3.0.0 I hope.
 */
int ControlClient::SendReliable()
{
	char		*read_buf;
	int			i;
	int			n;
	int			len;
	int			todo;
	int			max_todo;
	long		rel_off;
	const int	max_packet_size = MAX_RELIABLE_DATA_PACKET_SIZE;
	const int	min_send_size = 1;  /* was 4 in 3.0.7, 1 in 3.1.0 */

	if (cwbuf.len <= 0
		|| last_send_loops == main_loops)
	{
		last_send_loops = main_loops;
		return 0;
	}
	//DR(Trace("SendReliable: ");)
	read_buf = cwbuf.buf;
	max_todo = cwbuf.len;
	rel_off = reliable_offset;
	//DR(for (i=0; i<cwbuf.len; i++)
	//	Trace("%02X ", (unsigned)(cwbuf.buf[i]&255));
	//   Trace("\nSendReliable: ");
	//  )
	if (wbuf.len > 0)
	{
		/* We are piggybacking on a frame update. */
		if (wbuf.len >= max_packet_size - min_send_size)
		{
			/* Frame already too big */
			DR(Trace("too big\n");)
			return 0;
		}
		if (max_todo > max_packet_size - wbuf.len)
		{
			/* Do not exceed minimum fragment size. */
			max_todo = max_packet_size - wbuf.len;
		}
	}
	if (retransmit_at_loop > main_loops)
	{
	   /*
		* It is not time to retransmit yet.
		*/
		if (max_todo <= reliable_unsent - reliable_offset + min_send_size
			|| wbuf.len == 0)
		{
		   /*
			* And we cannot send anything new either
			* and we do not want to introduce a new packet.
			*/
			//DR(Trace("waiting for timeout\n");)
			return 0;
		}
	}
	else if (retransmit_at_loop != 0)
	{
	   /*
		* Timeout.
		* Either our packet or the acknowledgement got lost,
		* so retransmit.
		*/
		acks >>= 1;
		//DR(Trace("[timeout] ");)
	}

	todo = max_todo;
	for (i = 0; i <= acks && todo > 0; i++)
	{
		len = (todo > max_packet_size) ? max_packet_size : todo;
		DR(Trace("len=%d rel_off=%d, loops=%d\n", len, rel_off, main_loops);)
		if (wbuf.printf("%c%hd%ld%ld", PKT_RELIABLE, len, rel_off, main_loops) <= 0
			|| wbuf.Write(read_buf, len) != len)
		{
			error("Cannot write reliable data");
			// DestroyConnection("write error");
			Disconnect();
			DR(Trace("died\n");)
			return -1;
		}
		if ((n = wbuf.Flush()) < len)
		{
			if (n == 0
				&& (xperrno == EWOULDBLOCK
				|| xperrno == EAGAIN))
			{
				acks = 0;
				break;
			}
			else
			{
				error("Cannot flush reliable data (%d)", n);
				// DestroyConnection("flush error");
				Disconnect();
				DR(Trace("died\n");)
				return -1;
			}
		}
		todo -= len;
		rel_off += len;
		read_buf += len;
	}

	/*
	* Drop rest of outgoing data packet if something remains at all.
	*/
	wbuf.Clear();

	last_send_loops = main_loops;

	if (max_todo - todo <= 0)
	{
	   /*
		* We have not transmitted anything at all.
		*/
		DR(Trace("not xmit\n");)
		return 0;
	}

	/*
	* Retransmission timer with exponential backoff.
	*/
	if (rtt_retransmit > MAX_RETRANSMIT)
	{
		rtt_retransmit = MAX_RETRANSMIT;
	}
	if (retransmit_at_loop <= main_loops)
	{
		retransmit_at_loop = (long)(main_loops + (rtt_retransmit/TIMER_INTERVAL));
		rtt_retransmit *= 2.0;
		rtt_timeouts++;
	}
	else
	{
		retransmit_at_loop = (long)(main_loops + (rtt_retransmit/TIMER_INTERVAL));
	}

	if (rel_off > reliable_unsent)
	{
		reliable_unsent = rel_off;
	}
	DR(Trace("return %d\n", max_todo - todo);)
	return (max_todo - todo);
}


///////////////////////////////////////////////////////////////////////////////
int ControlClient::ReceiveReply(int *replyto, int *result)
{
    int		n;
    u_byte	type, ch1, ch2;

    n = crbuf.scanf("%c%c%c", &type, &ch1, &ch2);
    if (n <= 0) {
	return n;
    }
    if (n != 3 || type != PKT_REPLY) {
		emh(emhThis, EmWarning, "Can't receive reply packet");
		return -1;
    }
    *replyto = ch1;
    *result = ch2;
    return 1;
}

///////////////////////////////////////////////////////////////////////////////
int ControlClient::ReceiveMagic()
{
    int			n;
    u_byte		ch;
    unsigned		magic;

    if ((n = crbuf.scanf("%c%u", &ch, &magic)) <= 0) {
	return n;
    }
    return 1;
}

///////////////////////////////////////////////////////////////////////////////
int ControlClient::RbufSelected()
{
	if (!connected)
		return(-1);
	DR(	Trace("RbufSelected!\n"); )
	rbuf.Clear();
	if (rbuf.Read() == -1)
	{
		connected = false;
		//	DestroyConnection(ind, "input error");
		emh(emhThis, EmError, "Disconnect: %s", (PCSTR)rbuf.sock.GetErrorString());
		Disconnect();
		return(-1);
	}
	if (rbuf.len <= 0)
	{
		/*
		* No input.
		*/
		DR(Trace("RbufSelected: No input\n");)
		return(0);
	}
    int		type;
	int		prev_type = 0;
	int		result;
	int		replyto;
	int		status;

	while (rbuf.buf + rbuf.len > rbuf.ptr)
	{
		type = (*rbuf.ptr & 0xFF);

		DR(Trace("RbufSelected: handle rbuf type=%d\n", type);)
		if (receiveTbl[type] == 0)
		{
			seterrno(0);
			IFNWINDOWS(error("Received unknown packet type (%d, %d), dropping frame.",
				type, prev_type);)
			rbuf.Clear();
			break;
		}
		else if ((result = (this->*receiveTbl[type])()) <= 0)
		{
			if (result == -1)
			{
				if (type != PKT_QUIT)
				{
					seterrno(0);
					emh(emhThis, EmError, "Processing packet type (%d, %d) failed",
						type, prev_type);
				}
				return -1;
			}
			/* Drop rest of incomplete packet */
			rbuf.Clear();
			break;
		}
		prev_type = type;
	}
	while (crbuf.buf + crbuf.len > crbuf.ptr)
	{
		type = (*crbuf.ptr & 0xFF);
		DR(Trace("RbufSelected: handle crbuf type=%d\n", type);)
		if (type == PKT_REPLY)
		{
			if ((result = ReceiveReply(&replyto, &status)) <= 0)
			{
				if (result == 0)
					break;
				return -1;
			}
			/* should do something more appropriate than this with the reply */
			seterrno(0);
			emh(emhThis, EmError, "Got reply packet (%d,%d)", replyto, status);
		}
		else if (reliableTbl[type] == 0)
		{
			seterrno(0);
			emh(emhThis, EmError, "Received unknown reliable data packet type (%X,%d,%d)",
				type, crbuf.ptr - crbuf.buf, crbuf.len);
/*			int i;
			Trace("\tdumping buffer for debugging:\n");
			for (i = 0; i < cbuf.len; i++)
			{
				Trace("%3d", cbuf.buf[i] & 0xFF);
				if (i % 20 == 0)
					Trace("\n");
				else
					Trace(" ");
			}
			Trace("\n");
*/
			return -1;
		}
		else if ((result = (this->*reliableTbl[type])()) <= 0)
		{
			if (result == 0)
				break;
			return -1;
		}
	}

    return 0;
}


///////////////////////////////////////////////////////////////////////////////
int ControlClient::ReceiveCtl()
{
	uchar	ch;
	uchar	ctlType;
	int		result;

	if (crbuf.scanf("%c%c", &ch, &ctlType) <= 0)
	{
		emh(emhThis, EmWarning, "Error receiving Ctl type");
		return(0);
	}
	DR(Trace("ReceiveCtl: %d\n", ctlType);)
	if (ctlTbl[ctlType] == 0)
	{
		seterrno(0);
		emh(emhThis, EmWarning, "Received unknown ctl type (%d, %d), dropping packet.",
			ctlType);
		crbuf.Clear();
		return(0);
	}
	else if ((result = (this->*ctlTbl[ctlType])()) <= 0)
	{
		if (result == -1)
			return -1;
		/* Drop rest of incomplete packet */
		emh(emhThis, EmError, "ControlClient: Error processing control type %d", ctlType);
		crbuf.Clear();
		return(0);
	}
	return(1);
}

///////////////////////////////////////////////////////////////////////////////
int ControlClient::ReceiveSetOption()
{
	char	name[MAX_CHARS];
	uchar	source;
	char	value[MSG_LEN];

	// Trace("ReceiveSetOption buf=%p ptr=%p\n", cbuf.buf, cbuf.ptr);
	if (crbuf.scanf("%s%c%S", &name, &source, &value) <= 0)
	{
		emh(emhThis, EmError, "ReceiveSetOption: Can't parse option");
		return(-1);
	}
	if (sos)
	{
		if (!strlen(name))
		{				// The problem with requesting options by index instead of name
			wbuf.Flush();
			IHaveSpoken();
			return(1);	// is that we don't know which are the "bad" options, that won't be transferred.
		}				// So just ignore any nameless options
		D(Trace("Setting option <%s> to <%s>\n", name, value);)
		ServerOption* o = sos->FindOptionByName(name);
		if (!o)
		{
			emh(emhThis, EmError, "Can't find option <%s>", name);
			return(0);
		}
		o->optOrigin = (OptOrigin)source;
		o->Set(value);
	}
	wbuf.Flush();
	IHaveSpoken();
	return(1);
}

///////////////////////////////////////////////////////////////////////////////
int ControlClient::ReceiveQuit()
{
	unsigned char	pkt;
	Sockbuf*	sbuf;
	char		reason[MAX_CHARS];

	if (rbuf.ptr < rbuf.buf + rbuf.len)
		sbuf = &rbuf;
	else
		sbuf = &crbuf;
	if (sbuf->scanf("%c", &pkt) != 1)
	{
		seterrno(0);
		emh(emhThis, EmError, "Can't read quit packet");
	}
	else
	{
		if (sbuf->scanf("%s", reason) <= 0)
			strlcpy(reason, "unknown reason", MAX_CHARS);
		seterrno(0);
		emh(emhThis, EmError, "Got quit packet: \"%s\"", reason);
	}
	Disconnect();
	return -1;
}

///////////////////////////////////////////////////////////////////////////////
int ControlClient::TimerTick()
{
	if (!connected)
		return(0);

	time_t	now = time(NULL);
	if (lastSpoken + KEEPALIVEDELAY < now)
	{
		wbuf.printf("%c", PKT_NOP);
		wbuf.Flush();
		lastSpoken = now;
	}
	main_loops++;
	SendReliable();
	return(1);
}

///////////////////////////////////////////////////////////////////////////////
void ControlClient::IHaveSpoken()
{
	time(&lastSpoken);
}

///////////////////////////////////////////////////////////////////////////////
bool ControlClient::SendWantPlayMessages(bool want)
{
	if (!connected)
		return(false);
	D(xpprintf("SendWantPlayMessages: %s\n", want ? "true" : "false");)
	cwbuf.printf("%c%c%c", PKT_CTL, WantPlayMessages, want ? 1 : 0);
	if (wbuf.Flush() <= 0)
		return(false);
	IHaveSpoken();
	return(true);
}

///////////////////////////////////////////////////////////////////////////////
bool ControlClient::SendPlayerMessage(PCSTR message, PCSTR player, PlayerType pt)
{
	if (!connected)
		return(false);
	D(xpprintf(LOGMED, "SendPlayerMessage: \"%s\" to \"\"\n", message, player ? player : "");)
	cwbuf.printf("%c%c%s%S%c", PKT_CTL, PlayerMessage,
							 message,
							 player ? player : "",
							 pt);
	if (wbuf.Flush() <= 0)
		return(false);
	IHaveSpoken();
	return(true);

}

///////////////////////////////////////////////////////////////////////////////
bool ControlClient::SendPlayerRank(PCSTR player, PlayerType pt, int rank, DFLOAT rate)
{
	cwbuf.printf("%c%c%s%c%d%d", PKT_CTL, GetRanks,
								 player ? player : "", pt,
								 rank,
								(int)((DFLOAT)rate * 100.0 + ((DFLOAT)rate > 0.0 ? 0.5 : -0.5)));


	return(true);
}
