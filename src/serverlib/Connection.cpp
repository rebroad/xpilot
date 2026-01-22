/* $Id: Connection.cpp,v 1.41 2004/05/31 18:17:25 dick Exp $
 *
 * Describe a server's connection to a client.
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2003 by
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
 *  $Log: Connection.cpp,v $
 *  Revision 1.41  2004/05/31 18:17:25  dick
 *  virtual destructors so subclasses don't leak.
 *
 *  Revision 1.40  2004/05/24 07:37:05  dick
 *  Add support for 2 serverMessage, which can displayed on a client when he starts up.
 *
 *  Revision 1.39  2004/05/22 14:49:50  dick
 *  Remove #ifndef SILENT
 *
 *  Revision 1.38  2004/04/23 17:41:57  dick
 *  StateToText() is public
 *
 *  Revision 1.37  2004/03/04 03:47:32  dick
 *  Save off the version that the client reported as origVersion.
 *  This is for stats only and doesn't affect gameplay.
 *
 *  Revision 1.36  2004/02/18 04:41:17  dick
 *  Make protected and private a bunch of attributes that should have been.
 *  new InitBuffers(Sock*) replaces the only place they were exposed.
 *
 *  Revision 1.35  2004/02/18 01:19:29  dick
 *  GetAddr and GetDpy move to Connection where the attributes live (not ConnectionPlayer)
 *
 *  Revision 1.34  2004/02/16 14:29:44  dick
 *  server/sched.cpp and friends become serverlib/NetServer
 *  which disconnects the udp listen and receive functionality from the game.
 *
 *  Revision 1.33  2004/02/03 03:39:53  dick
 *  Add Windows precompiled header support.
 *
 *  Revision 1.32  2004/01/09 05:28:25  dick
 *  When a Connection is destroyed, call World::NumLogoutsBump so meta will be
 *  notified during the loop.  Input becomes a member of World.
 *
 *  Revision 1.31  2004/01/06 04:48:28  dick
 *  Add support for building under Cygwin.  Still needs work and the output
 *  is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 *  Revision 1.30  2002/09/16 21:52:33  dick
 *  Use Stringized ServerT.
 *
 *  Revision 1.29  2002/09/11 16:42:03  dick
 *  Here endeth the server encapsulation task.
 *  theWorld is now an array of World(s) and get loaded dynamically.
 *
 *  Revision 1.28  2002/09/10 01:14:45  dick
 *  include stdlib.h
 *
 *  Revision 1.27  2002/09/01 00:13:45  dick
 *  Remove obsolete windows #include "winServer.h"
 *
 *  Revision 1.26  2002/08/31 23:31:48  dick
 *  User version 0x5000 instead of version++ 1
 *
 *  Revision 1.25  2002/08/01 14:40:41  dick
 *  Retab
 *
 *  Revision 1.24  2002/07/12 15:45:49  dick
 *  Track controlsConnected so we can run fast sched() when only a control
 *  is connected.
 *
 *  Revision 1.23  2002/07/09 21:43:44  dick
 *  Maintain the number of controls connected
 *
 *  Revision 1.22  2002/07/08 05:53:00  dick
 *  showtime() handles the space after itself
 *
 *  Revision 1.21  2002/06/30 06:52:58  dick
 *  SendMessage is a bad word.  It's a Windows function that is a macro and gets
 *  translated to SendMessageA or SendMessageW.  Although it works, renaming ours
 *  to SendPlayMessage makes browsing the source code easier.
 *
 *  Revision 1.20  2002/06/26 07:02:09  dick
 *  Use xperrno instead of errno and seterrno(0) instead of errno=0.
 *
 *  Revision 1.19  2002/06/23 06:32:43  dick
 *  cleanup if the ScoreServer goes away
 *
 *  Revision 1.18  2002/06/02 06:13:02  dick
 *  Fix bug calculating number of players.
 *
 *  Revision 1.17  2002/06/01 06:06:47  dick
 *  Encapsulate (almost) everything.  Get rid of (almost) all refs to theWorld.
 *
 *  Revision 1.16  2002/05/20 02:17:45  dick
 *  Windows needs errno and seterrno in portability.h.
 *  Windows also needs rint() defined if not using fltk.
 *
 *  Revision 1.15  2002/05/18 22:34:15  dick
 *  Move showtime to common.
 *
 *  Revision 1.14  2002/05/18 20:55:37  dick
 *  Update to XPilot-4.5.1
 *
 *  Revision 1.13  2002/02/18 05:47:50  dick
 *  Conf_logfile() now returns a const char*
 *
 *  Revision 1.12  2001/09/03 00:45:00  dick
 *  New packet type Nop, used for a keepalive ping.
 *
 *  Revision 1.11  2001/08/31 20:58:13  dick
 *  Server needs to be able to receive reliable packets as well as send them.
 *  Use the new emh error handling.
 *
 *  Revision 1.10  2001/08/30 07:16:44  dick
 *  Send and receive options between the server and xpilotedit.
 *
 *  Revision 1.9  2001/08/26 10:34:26  dick
 *  Handle a connection from a control.
 *
 *  Revision 1.8  2001/08/13 03:03:43  dick
 *  SockbufClient becomes ControlClient
 *
 *  Revision 1.7  2001/08/12 11:09:05  dick
 *  Zero char*'s during init
 *
 *  Revision 1.6  2001/08/11 09:38:20  dick
 *  new class ConnectionControl, a Connection for a control.
 *
 *  Revision 1.5  2001/08/11 00:44:49  dick
 *  Windows doesn't grok unistd.h
 *
 *  Revision 1.4  2001/08/11 00:36:52  dick
 *  frame.cpp becomes Frame.cpp.  object.h becomes Object.h
 *
 *  Revision 1.3  2001/08/10 08:11:36  dick
 *  New class ConnectionPlayer subclasses Connection to provide player
 *  functions.  New class Frame subclasses ConnectionPlayer which provides
 *  Frame update functionality.
 *
 *  Revision 1.2  2001/08/09 07:56:41  dick
 *  Linux cleanups
 *
 *  Revision 1.1  2001/08/09 07:43:34  dick
 *  New class Connection
 *
 */
#include "StdAfx.h"

#if defined(_UNIX) || defined(_CYGWIN)
# include <unistd.h>
# include <stdlib.h>
#else
# include <io.h>
#endif


#include <errno.h>
#include <string.h>
#include <sys/stat.h>


#include "config.h"
#include "NT/winNet.h"
#include "serverconst.h"
#include "draw.h"
#include "error.h"
#include "packet.h"
#include "pack.h"
#include "commonproto.h"
#include "showtime.h"
#include "Connection.h"
#include "NetServer.h"
//#include "sched.h"
//#include "setup.h"
#include "checknames.h"
#include "portability.h"

// Debug Reliable data
#if 0
#define	DR(__x)	D(__x)
#else
#define	DR(__x)
#endif

/*
 * Maximum roundtrip time taken as serious for rountrip time calculations.
 */
// DIK: HACK! Assume an FPS of 14 for now.
//#define MAX_RTT			(world->GetFPS() + 1)
#define	MAX_RTT		(14 + 1)

/*
 * The retransmission timeout bounds in number of frames.
 */
//#define MIN_RETRANSMIT		(theWorld.GetFPS() / 8 + 1)
//#define MAX_RETRANSMIT		(theWorld.GetFPS() + 1)
//#define DEFAULT_RETRANSMIT	(theWorld.GetFPS() / 2)
// DIK: HACK! Assume an FPS of 14 for now.
#define MIN_RETRANSMIT		(14 / 8 + 1)
#define MAX_RETRANSMIT		(14 + 1)
#define DEFAULT_RETRANSMIT	(14 / 2)


/* Currently, the server only knows one reliableReceive.
 * Is it a waste of 1K of memory? yes.
 * If we don't ever see more than PKT_CTL feeding through here
 * perhaps we should nuke it.
 */
ConnectionReceiveFunc	playingReceive[256];
ConnectionReceiveFunc	loginReceive[256];
ConnectionReceiveFunc	drainReceive[256];
ConnectionReceiveFunc	reliableReceive[256];


///////////////////////////////////////////////////////////////////////////////
Connection::Connection()
{
    id = NO_ID;
    timeout = LISTEN_TIMEOUT;
    reliable_offset = 0;
    reliable_unsent = 0;
    last_send_loops = 0;
    retransmit_at_loop = 0;
    rtt_retransmit = DEFAULT_RETRANSMIT;
    rtt_smoothed = 0;
    rtt_dev = 0;
    rtt_timeouts = 0;
    acks = 0;
    setup = 0;
	start = 0;
	state = 0;
	drainState = 0;
    SetState(CONN_LISTENING, CONN_FREE);
	//state = CONN_LISTENING;
	//drainState = CONN_FREE;

	ctl = NULL;

	reliableOffset = 0;
	emh = 0;
    start = netServer->mainLoops;
}

///////////////////////////////////////////////////////////////////////////////
Connection::~Connection()
{
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Initialize the function dispatch tables for the various client
 * connection states.  Some states use the same table.
 */
//static 
void Connection::InitReceiveTables()
{
    int			i;

//	InitConnectionControl();
    for (i = 0; i < 256; i++) 
	{
		loginReceive[i] = &Connection::ReceiveUndefined;
		playingReceive[i] = &Connection::ReceiveUndefined;
		drainReceive[i] = &Connection::ReceiveUndefined;
		// reliableReceive[i] = &Connection::ReceiveUndefined;
		reliableReceive[i] = NULL;
    }

    drainReceive[PKT_QUIT]				= &Connection::ReceiveQuit;
    drainReceive[PKT_ACK]				= &Connection::ReceiveAck;
    drainReceive[PKT_VERIFY]			= &Connection::ReceiveDiscard;
    drainReceive[PKT_PLAY]				= &Connection::ReceiveDiscard;
    drainReceive[PKT_SHAPE]				= &Connection::ReceiveDiscard;
    drainReceive[PKT_RELIABLE]			= &Connection::ReceiveReliable;

    loginReceive[PKT_PLAY]				= &Connection::ReceivePlay;
    loginReceive[PKT_QUIT]				= &Connection::ReceiveQuit;
    loginReceive[PKT_ACK]				= &Connection::ReceiveAck;
    loginReceive[PKT_VERIFY]			= &Connection::ReceiveDiscard;
    loginReceive[PKT_POWER]				= &Connection::ReceivePower;
    loginReceive[PKT_POWER_S]			= &Connection::ReceivePower;
    loginReceive[PKT_TURNSPEED]			= &Connection::ReceivePower;
    loginReceive[PKT_TURNSPEED_S]		= &Connection::ReceivePower;
    loginReceive[PKT_TURNRESISTANCE]	= &Connection::ReceivePower;
    loginReceive[PKT_TURNRESISTANCE_S]	= &Connection::ReceivePower;
    loginReceive[PKT_DISPLAY]			= &Connection::ReceiveDisplay;
    loginReceive[PKT_MODIFIERBANK]		= &Connection::ReceiveModifierBank;
    loginReceive[PKT_MOTD]				= &Connection::ReceiveMotd;
    loginReceive[PKT_SHAPE]				= &Connection::ReceiveShape;
    loginReceive[PKT_REQUEST_AUDIO]		= &Connection::ReceiveAudioRequest;
    loginReceive[PKT_ASYNC_FPS]			= &Connection::ReceiveFpsRequest;
    loginReceive[PKT_CTL]				= &Connection::ReceiveCtl;
    loginReceive[PKT_NOP]				= &Connection::ReceiveNop;
    loginReceive[PKT_RELIABLE]			= &Connection::ReceiveReliable;

    playingReceive[PKT_ACK]				= &Connection::ReceiveAck;
    playingReceive[PKT_VERIFY]			= &Connection::ReceiveDiscard;
    playingReceive[PKT_PLAY]			= &Connection::ReceivePlay;
    playingReceive[PKT_QUIT]			= &Connection::ReceiveQuit;
    playingReceive[PKT_KEYBOARD]		= &Connection::ReceiveKeyboard;
    playingReceive[PKT_POWER]			= &Connection::ReceivePower;
    playingReceive[PKT_POWER_S]			= &Connection::ReceivePower;
    playingReceive[PKT_TURNSPEED]		= &Connection::ReceivePower;
    playingReceive[PKT_TURNSPEED_S]		= &Connection::ReceivePower;
    playingReceive[PKT_TURNRESISTANCE]	= &Connection::ReceivePower;
    playingReceive[PKT_TURNRESISTANCE_S]= &Connection::ReceivePower;
    playingReceive[PKT_ACK_CANNON]		= &Connection::ReceiveAckCannon;
    playingReceive[PKT_ACK_FUEL]		= &Connection::ReceiveAckFuel;
    playingReceive[PKT_ACK_TARGET]		= &Connection::ReceiveAckTarget;
    playingReceive[PKT_TALK]			= &Connection::ReceiveTalk;
    playingReceive[PKT_DISPLAY]			= &Connection::ReceiveDisplay;
    playingReceive[PKT_MODIFIERBANK]	= &Connection::ReceiveModifierBank;
    playingReceive[PKT_MOTD]			= &Connection::ReceiveMotd;
    playingReceive[PKT_SHAPE]			= &Connection::ReceiveShape;
    playingReceive[PKT_POINTER_MOVE]	= &Connection::ReceivePointerMove;
    playingReceive[PKT_REQUEST_AUDIO]	= &Connection::ReceiveAudioRequest;
    playingReceive[PKT_ASYNC_FPS]		= &Connection::ReceiveFpsRequest;
    playingReceive[PKT_CTL]				= &Connection::ReceiveCtl;
    playingReceive[PKT_RELIABLE]		= &Connection::ReceiveReliable;
    playingReceive[PKT_NOP]				= &Connection::ReceiveNop;

    reliableReceive[PKT_CTL]			= &Connection::ReceiveCtl;

}

///////////////////////////////////////////////////////////////////////////////
void Connection::SetErrMsgHandler(ErrMsgHandler _emh, void* _emhThis)
{
	emh = _emh;
	emhThis = _emhThis;
	r.SetErrMsgHandler(emh, emhThis);
	w.SetErrMsgHandler(emh, emhThis);
	cr.SetErrMsgHandler(emh, emhThis);
	cw.SetErrMsgHandler(emh, emhThis);
}

///////////////////////////////////////////////////////////////////////////////
bool Connection::InitBuffers(Sock* sock)
{
    w.Init(sock, SERVER_SEND_SIZE,
		 SOCKBUF_WRITE | SOCKBUF_DGRAM);
    r.Init(sock, SERVER_RECV_SIZE,
		 SOCKBUF_READ | SOCKBUF_DGRAM);
    cw.Init((Sock *) NULL, MAX_SOCKBUF_SIZE,
		 SOCKBUF_WRITE | SOCKBUF_READ | SOCKBUF_LOCK);
    cr.Init((Sock *) NULL, MAX_SOCKBUF_SIZE,
		 SOCKBUF_WRITE | SOCKBUF_READ | SOCKBUF_LOCK);
	if (w.buf == NULL
	 || r.buf == NULL
	 || cw.buf == NULL
	 || cr.buf == NULL)
		return(false);
	return(true);
}
///////////////////////////////////////////////////////////////////////////////
int Connection::SendReply(int replyto, int result)
{
	int			n;

	n = cw.printf("%c%c%c", PKT_REPLY, replyto, result);
	if (n == -1) 
	{
		DestroyConnection("write error");
		return -1;
	}
	return n;
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Cleanup a connection.  The client may not know yet that
 * it is thrown out of the game so we send it a quit packet.
 * We send it twice because of UDP it could get lost.
 * Since 3.0.6 the client receives a short message
 * explaining why the connection was terminated.
 */
void Connection::DestroyConnection(const char *reason)
{
	int 		len;
	Sock*		sock;
	char		pkt[MAX_CHARS];

	if (state == CONN_FREE) {
		seterrno(0);
		error("Cannot destroy empty connection (\"%s\")", reason);
		return;
	}

	sock = &w.sock;
	netServer->RemoveInput(sock->fd);

	pkt[0] = PKT_QUIT;
	strlcpy(&pkt[1], reason, sizeof(pkt) - 1);
	len = strlen(pkt) + 1;
	if (sock->Write(pkt, len) != len) {
		sock->GetError();
		sock->Write(pkt, len);
	}
	xpprintf("%sGoodbye %s%s%s@%s|%s (\"%s\")\n",
			 showtime(),
			 nick.GetLength() ? (PCSTR)nick : "",
			 (nick.GetLength() && nick[0]) ? "=" : "",
			 real.GetLength() ? (PCSTR)real : "",
			 host.GetLength() ? (PCSTR)host : "",
			 dpy.GetLength() ? (PCSTR)dpy : "",
			 reason);

	SetState(CONN_FREE, CONN_FREE);

	w.Cleanup();
	r.Cleanup();
	cw.Cleanup();
	cr.Cleanup();

//	num_logouts++;

	if (sock->Write(pkt, len) != len) {
		sock->GetError();
		sock->Write(pkt, len);
	}
	sock->Close();

	netServer->conn[myInd] = NULL;
	delete this;
}

///////////////////////////////////////////////////////////////////////////////
String	StateToText(int state)
{
	String	s;
	if (state & CONN_FREE) {
		s += "FREE";
	}
	if (state & CONN_LISTENING) {
		if (!s.IsEmpty()) s += "|";
		s += "LISTENING";
	}
	if (state & CONN_SETUP) {
		if (!s.IsEmpty()) s += "|";
		s += "SETUP";
	}
	if (state & CONN_LOGIN) {
		if (!s.IsEmpty()) s += "|";
		s += "LOGIN";
	}
	if (state & CONN_PLAYING) {
		if (!s.IsEmpty()) s += "|";
		s += "PLAYING";
	}
	if (state & CONN_DRAIN) {
		if (!s.IsEmpty()) s += "|";
		s += "DRAIN";
	}
	if (state & CONN_READY) {
		if (!s.IsEmpty()) s += "|";
		s += "READY";
	}
	if (state & CONN_CTL) {
		if (!s.IsEmpty()) s += "|";
		s += "CTL";
	}
	return(s);
}

///////////////////////////////////////////////////////////////////////////////
void Connection::SetState(int _state, int _drainState)
{
	// D(xpprintf("SetState: %02X, %02X\n", _state, _drainState);)
	//D(Trace("SetState: %02X, %02X\n", _state, _drainState);)
	static int num_conn_busy = 0;
	static int num_conn_playing = 0;
	static int num_conn_ctl = 0;

	if ((state & (CONN_PLAYING | CONN_READY)) != 0)
	{
		num_conn_playing--;
	}
	else if (state == CONN_FREE)
	{
		num_conn_busy++;
	}
	else if (state == CONN_CTL)
	{
		num_conn_ctl--;
	}

	state = _state;
	drainState = _drainState;
//	if (world)
//		start = world->mainLoops;
	start = netServer->mainLoops;

	if (state == CONN_PLAYING)
	{
		num_conn_playing++;
		timeout = IDLE_TIMEOUT;
	}
	else if (state == CONN_READY)
	{
		num_conn_playing++;
		timeout = READY_TIMEOUT;
	}
	else if (state == CONN_LOGIN)
	{
		timeout = LOGIN_TIMEOUT;
	}
	else if (state == CONN_SETUP)
	{
		timeout = SETUP_TIMEOUT;
	}
	else if (state == CONN_LISTENING)
	{
		timeout = LISTEN_TIMEOUT;
	}
	else if (state == CONN_FREE)
	{
		num_conn_busy--;
		timeout = IDLE_TIMEOUT;
	}
	else if (state == CONN_CTL)
	{
		num_conn_ctl++;
		timeout = CTL_TIMEOUT;
	}

	netServer->loginInProgress = num_conn_busy - num_conn_playing - num_conn_ctl;
	netServer->controlsConnected = num_conn_ctl;
	D(Trace("SetState: %02X, %02X busy=%d playing=%d ctl=%d %s\n", _state, _drainState,
		num_conn_busy, num_conn_playing, num_conn_ctl, (PCSTR)StateToText(state));)

}

///////////////////////////////////////////////////////////////////////////////
/*
 * Handle a connection that is in the listening state.
 */
int Connection::HandleListening()
{
	unsigned char		type;
	int 		n;
	char		_nick[MAX_CHARS];
	char		_real[MAX_CHARS];
	char		_dpy[MAX_CHARS];

	if (state != CONN_LISTENING) {
		DestroyConnection("not listening");
		return -1;
	}
	r.Clear();
	seterrno(0);
	n = r.sock.ReceiveAny(r.buf, r.size);
	if (n <= 0) {
		if (n == 0
			|| xperrno == EWOULDBLOCK
			|| xperrno == EAGAIN) {
			n = 0;
		}
		else if (n != 0) {
			DestroyConnection("read first packet error");
		}
		return n;
	}
	r.len = n;
	his_port = r.sock.GetLastPort();
	if (w.sock.Connect(addr, his_port) == -1) {
		emh(emhThis, EmError, "Cannot connect datagram socket (%s,%d,%d,%d,%d)",
			  (PCSTR)addr, his_port,
			  w.sock.sockError.error,
			  w.sock.sockError.call,
			  w.sock.sockError.line);
		if (w.sock.GetError()) {
			emh(emhThis, EmError, "sock_get_error fails too, giving up");
			DestroyConnection("connect error");
			return -1;
		}
		seterrno(0);
		if (w.sock.Connect(addr, his_port) == -1) {
			emh(emhThis, EmError, "Still cannot connect datagram socket (%s,%d,%d,%d,%d)",
				  (PCSTR)addr, his_port,
				  w.sock.sockError.error,
				  w.sock.sockError.call,
				  w.sock.sockError.line);
			DestroyConnection("connect error");
			return -1;
		}
	}
	xpprintf("%sWelcome %s%s%s@%s|%s (%s/%d)", showtime(), (PCSTR)nick,
		   (nick && nick.GetLength()) ? "=" : "",
		   (PCSTR)real, (PCSTR)host, (PCSTR)dpy, (PCSTR)addr, his_port);
	if (version != MY_VERSION)
		xpprintf(" (version %04x)", version);
	if (version != origVersion)
		xpprintf(" (client %04x)", origVersion);

	if (r.ptr[0] != PKT_VERIFY) {
		SendReply(PKT_VERIFY, PKT_FAILURE);
		SendReliable();
		DestroyConnection("not connecting");
		return -1;
	}
	if ((n = r.scanf("%c%s%s%s",
						  &type, _real, _nick, _dpy)) <= 0) {
		SendReply(PKT_VERIFY, PKT_FAILURE);
		SendReliable();
		DestroyConnection("verify incomplete");
		return -1;
	}
	String	_r = _real;
	FixRealName(_r);
	String	_n = _nick;
	FixNickName(_n);
	if (strcmp(real, _r)) {
		xpprintf("\n%sClient verified incorrectly (%s,%s)(%s,%s)\n",
				 showtime(), (PCSTR)_r, (PCSTR)_n, (PCSTR)real, (PCSTR)nick);
		SendReply(PKT_VERIFY, PKT_FAILURE);
		SendReliable();
		DestroyConnection("verify incorrect");
		return -1;
	}
	if (version >= 0x5000)
	{
		if ((n = r.scanf("%d", &cookie)) <= 0) {
			SendReply(PKT_VERIFY, PKT_FAILURE);
			SendReliable();
			DestroyConnection("verify cookie incomplete");
			return -1;
		}
	}
	else
		cookie = COOKIENOSUP;
	if (version >= 0x5000)
		xpprintf(" (%08x)", cookie);

	xpprintf("\n");
	w.Clear();
	if (SendReply(PKT_VERIFY, PKT_SUCCESS) == -1
		|| cw.printf("%c%u", PKT_MAGIC, magic) <= 0
		|| SendReliable() <= 0) {
		DestroyConnection("confirm failed");
		return -1;
	}

	SetState(CONN_DRAIN, CONN_SETUP);

	return 1;	/* success! */
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Send the reliable data.
 * If the client is in the receive-frame-updates state then
 * all reliable data is piggybacked at the end of the
 * frame update packets.  (Except maybe for the MOTD data, which
 * could be transmitted in its own packets since MOTDs can be big.)
 * Otherwise if the client is not actively playing yet then
 * the reliable data is sent in its own packets since there
 * is no other data to combine it with.
 *
 * This thing still is not finished, but it works better than in 3.0.0 I hope.
 *
 * DIK-20020830: There is an annoying bug whereby when you disconnect an
 * established connection and reconnect (like with the server control), then
 * the reliable data moves very slowly in this second connection.
 */
int Connection::SendReliable()
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

	DR(xpprintf("SendReliable: ");)
	if (cw.len <= 0
		|| last_send_loops == netServer->mainLoops)
	{
		last_send_loops = netServer->mainLoops;
		DR(xpprintf("none\n");)
		return 0;
	}
	read_buf = cw.buf;
	max_todo = cw.len;
	rel_off = reliable_offset;
	if (w.len > 0) 
	{
		/* We are piggybacking on a frame update. */
		if (w.len >= max_packet_size - min_send_size)
		{
			/* Frame already too big */
			DR(xpprintf("too big\n");)
			return 0;
		}
		if (max_todo > max_packet_size - w.len)
		{
			/* Do not exceed minimum fragment size. */
			max_todo = max_packet_size - w.len;
		}
	}
	if (retransmit_at_loop > netServer->mainLoops) 
	{
	   /*
		* It is not time to retransmit yet.
		*/
		if (max_todo <= reliable_unsent - reliable_offset + min_send_size
			|| w.len == 0)
		{
		   /*
			* And we cannot send anything new either
			* and we do not want to introduce a new packet.
			*/
			DR(xpprintf("waiting for timeout\n");)
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
		DR(xpprintf("[timeout] ");)
	}

	todo = max_todo;
	for (i = 0; i <= acks && todo > 0; i++)
	{
		len = (todo > max_packet_size) ? max_packet_size : todo;
		if (w.printf("%c%hd%ld%ld", PKT_RELIABLE, len, rel_off, netServer->mainLoops) <= 0
			|| w.Write(read_buf, len) != len)
		{
			emh(emhThis, EmError, "Cannot write reliable data");
			DestroyConnection("write error");
			DR(xpprintf("died\n");)
			return -1;
		}
		if ((n = w.Flush()) < len)
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
				emh(emhThis, EmError, "Cannot flush reliable data (%d)", n);
				DestroyConnection("flush error");
				DR(xpprintf("died\n");)
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
	w.Clear();

	last_send_loops = netServer->mainLoops;

	if (max_todo - todo <= 0) 
	{
	   /*
		* We have not transmitted anything at all.
		*/
		DR(xpprintf("not xmit\n");)
		return 0;
	}

	/*
	* Retransmission timer with exponential backoff.
	*/
	if (rtt_retransmit > MAX_RETRANSMIT) 
	{
		rtt_retransmit = MAX_RETRANSMIT;
	}
	if (retransmit_at_loop <= netServer->mainLoops) 
	{
		retransmit_at_loop = netServer->mainLoops + rtt_retransmit;
		rtt_retransmit <<= 1;
		rtt_timeouts++;
	}
	else 
	{
		retransmit_at_loop = netServer->mainLoops + rtt_retransmit;
	}

	if (rel_off > reliable_unsent) 
	{
		reliable_unsent = rel_off;
	}
	DR(xpprintf("return %d\n", max_todo - todo);)
	return (max_todo - todo);
}

///////////////////////////////////////////////////////////////////////////////
/*
 * This is only used by the control connections.  Clients don't ever
 * need to SendReliable because they just continually pump their state.
 * This function could be in ConnectionControl, but it is here because
 * this is the layer that handles this part of the protocol.
 */
int Connection::ReceiveReliable()
{
	int			n;
	short		len;
	u_byte		ch;
	long		rel;
	long		rel_loops;

	if ((n = r.scanf("%c%hd%ld%ld",
						&ch, &len, &rel, &rel_loops)) == -1) 
	{
		return -1;
	}
	if (n == 0) 
	{
		seterrno(0);
		// emh(emhThis, EmError, "Incomplete reliable data packet");
		xpprintf("Incomplete reliable data packet\n");
		return 0;
	}
//	if (reliableOffset >= rel + len) 
	{
		DR(xpprintf("Reliable my=%ld pkt=%ld len=%d loops=%ld\n",
			reliableOffset, rel, len, rel_loops);)
	}
	if (len <= 0) 
	{
		seterrno(0);
		xpprintf("Bad reliable data length (%d)", len);
		return -1;
	}
	if (r.ptr + len > r.buf + r.len) 
	{
		seterrno(0);
		xpprintf("Not all reliable data in packet (%d,%d,%d)",
		r.ptr - r.buf, len, r.len);
		r.ptr += len;
		r.Advance(r.ptr - r.buf);
		return -1;
	}
	if (rel > reliableOffset) 
	{
		/*
		* We miss one or more packets.
		* For now we drop this packet.
		* We could have kept it until the missing packet(s) arrived.
		*/
		r.ptr += len;
		r.Advance(r.ptr - r.buf);
		if (SendAck(rel_loops) == -1) 
			return -1;
		w.Flush();
		return 1;
	}
	if (rel + len <= reliableOffset) 
	{
		/*
		* Duplicate data.  Probably an ack got lost.
		* Send an ack for our current stream position.
		*/
		r.ptr += len;
		r.Advance(r.ptr - r.buf);
		if (SendAck(rel_loops) == -1)
			return -1;
		w.Flush();
		return 1;
	}
	if (rel < reliableOffset) 
	{
		len -= (short)(reliableOffset - rel);
		r.ptr += reliableOffset - rel;
		rel = reliableOffset;
	}
	if (cr.ptr > cr.buf)
		cr.Advance(cr.ptr - cr.buf);

	if (cr.Write(r.ptr, len) != len) 
	{
		seterrno(0);
		xpprintf("Can't copy reliable data to buffer");
		r.ptr += len;
		r.Advance(r.ptr - r.buf);
		return -1;
	}
	DR(xpprintf("ReceiveReliable: "); 
		for (int i=0; i<cr.len; i++) 
			xpprintf("%02X ", (unsigned)(cr.buf[i]&255)); 
		xpprintf("\n"); )

	reliableOffset += len;
	r.ptr += len;
	r.Advance(r.ptr - r.buf);
	if (SendAck(rel_loops) == -1) 
		return -1;
	w.Flush();
	return 1;
}

///////////////////////////////////////////////////////////////////////////////
int Connection::SendShutdown(int count, int delay)
{
    return w.printf("%c%hd%hd", PKT_SHUTDOWN, count, delay);
}

///////////////////////////////////////////////////////////////////////////////
int Connection::ReceiveQuit()
{
    DestroyConnection("client quit");

    return -1;
}

///////////////////////////////////////////////////////////////////////////////
int Connection::ReceiveNop()
{
	int	n;
	unsigned char	ch;
	if ((n = r.scanf("%c", &ch)) <= 0)
	{
		xpprintf("%sBad Nop %d\n", showtime(), n);
		return n;
	}
	DR(xpprintf("Nop\n");)
	return(1);
}

///////////////////////////////////////////////////////////////////////////////
int Connection::SendAck(long rel_loops)
{
	int			n;
	// printf("SendAck: %d\n", rel_loops);
	if ((n = w.printf("%c%ld%ld", PKT_ACK, reliableOffset, rel_loops)) <= 0)
	{
		if (n == 0)
			return 0;
		xpprintf("Can't ack reliable data");
		return -1;
	}
	return 1;
}


///////////////////////////////////////////////////////////////////////////////
int Connection::ReceiveAck()
{
	int			n;
	unsigned char	ch;
	long		rel,
	rtt,	/* RoundTrip Time */
	diff,
	delta,
	rel_loops;

	if ((n = r.scanf("%c%ld%ld", &ch, &rel, &rel_loops)) <= 0)
	{
		seterrno(0);
		emh(emhThis, EmError, "Cannot read ack packet (%d)", n);
		DestroyConnection("read error");
		return -1;
	}
	DR(xpprintf("ReceiveAck: %d %d ", rel, rel_loops);)
	if (ch != PKT_ACK)
	{
		seterrno(0);
		DR(xpprintf("died\n");)
		emh(emhThis, EmError, "Not an ack packet (%d)", ch);
		DestroyConnection("not ack");
		return -1;
	}
	rtt = netServer->mainLoops - rel_loops;
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
			rtt_retransmit = MIN_RETRANSMIT;
		}
	}
	diff = rel - reliable_offset;
	if (diff > cw.len)
	{
		/* Impossible to ack data that has not been send */
		seterrno(0);
		emh(emhThis, EmError, "Bad ack (diff=%ld,cru=%ld,c=%ld,len=%d)",
			diff, rel, reliable_offset, cw.len);
		DestroyConnection("bad ack");
		return -1;
	}
	else if (diff <= 0) 
	{
		/* Late or duplicate ack of old data.  Discard. */
		DR(xpprintf("duplicate\n");)
		return 1;
	}
	cw.Advance((int) diff);
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
		if (state == CONN_DRAIN) 
		{
			SetState(drainState, drainState);
		}
	}
	if (state == CONN_READY
		&& (cw.len <= 0
		|| (cw.buf[0] != PKT_REPLY
		&& cw.buf[0] != PKT_PLAY
		&& cw.buf[0] != PKT_SUCCESS
		&& cw.buf[0] != PKT_FAILURE))) 
	{
		SetState(drainState, drainState);
	}
	rtt_timeouts = 0;
	DR(xpprintf("ok\n");)
	return 1;
}

///////////////////////////////////////////////////////////////////////////////
int Connection::ReceiveDiscard()
{

    seterrno(0);
    emh(emhThis, EmError, "Discarding packet %d while in state \"%s\"",
	  r.ptr[0], (PCSTR)StateToText(state));
    r.ptr = r.buf + r.len;

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
int Connection::ReceiveUndefined()
{

	seterrno(0);
    emh(emhThis, EmError, "Unknown packet type (%d,%02x)", r.ptr[0], state);
    DestroyConnection("undefined packet");
    return -1;
}

///////////////////////////////////////////////////////////////////////////////
//static 
void Connection::HandleInput(int fd, void *arg)
{
	Connection*	conn = (Connection*)arg;
	conn->HandleInput();
}

///////////////////////////////////////////////////////////////////////////////
void Connection::HandleInput()
{
	ConnectionReceiveFunc*	receive_tbl;
	int			type;
	int			result;


	if (state & (CONN_PLAYING | CONN_READY | CONN_CTL))
		receive_tbl = &playingReceive[0];
	else if (state == CONN_LOGIN)
		receive_tbl = &loginReceive[0];
	else if (state & (CONN_DRAIN | CONN_SETUP))
		receive_tbl = &drainReceive[0];
	else if (state == CONN_LISTENING)
	{
		HandleListening();
		return;
	}
	else 
	{
		if (state != CONN_FREE)
			DestroyConnection("not input");
		return;
	}
//	numKeyboardUpdates = 0;	// XXX:

	r.Clear();
	if (r.Read() == -1)
	{
		DestroyConnection("input error");
		return;
	}
	if (r.len <= 0)
	{
	   /*
		* No input.
		*/
		return;
	}
	while (r.ptr < r.buf + r.len)
	{
		type = (r.ptr[0] & 0xFF);
		//	result = (*receive_tbl[type])(ind);
		//ConnectionPlayer* connp = (ConnectionPlayer*)conn;

		result = (this->*(receive_tbl[type]))();
		if (result == -1) 
		{
		   /*
			* Unrecoverable error.
			* Connection has been destroyed.
			*/
			return;
		}
		if (result == 0)
		{
		   /*
			* Incomplete client packet.
			* Drop rest of packet.
			*/
			r.Clear();
			break;
		}
		if (state == CONN_PLAYING || state == CONN_CTL)
		{
			/*D(Trace("reset %s type=%d start=%d delay=%d\n", 
				nick ? nick : "nonick", 
				type,
				mainLoops, mainLoops - start);)*/
			start = netServer->mainLoops;

		}

	}
	while (cr.ptr < cr.buf + cr.len)
	{
		type = (cr.ptr[0] & 0xFF);
		//	result = (*receive_tbl[type])(ind);
		// ConnectionPlayer* connp = (ConnectionPlayer*)conn;
		if (reliableReceive[type] == 0)
		{
			xpprintf("%s%s@%s Unknown reliable packet type %d\n", 
				showtime(), (PCSTR)real, (PCSTR)dpy, cr.ptr[0]);
			cr.Clear();
			break;
		}
		result = (this->*(reliableReceive[type]))();
		if (result == -1) 
		{
		   /*
			* Unrecoverable error.
			* Connection has been destroyed.
			*/
			return;
		}
		if (result == 0)
		{
		   /*
			* Incomplete client packet.
			* Drop rest of packet.
			* Punt, flush.
			*/
			cr.Clear();
			break;
		}
	}
}

