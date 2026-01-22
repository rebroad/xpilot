/* $Id: ControlClient.h,v 1.19 2004/05/14 21:15:35 dick Exp $
 *
 * ControlClient - a network control to give us come client side commands
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2002 by
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
 * $Log: ControlClient.h,v $
 * Revision 1.19  2004/05/14 21:15:35  dick
 * Handle PrivateServerList updates
 *
 * Revision 1.18  2004/05/07 04:36:33  dick
 * Get rank data from the scoreserver to the server to the client
 *
 * Revision 1.17  2004/02/16 14:30:16  dick
 * server/sched.cpp and friends become serverlib/NetServer
 * which disconnects the udp listen and receive functionality from the game.
 *
 */

#ifndef	_ControlClient_H_
#define	_ControlClient_H_

#include "Sockbuf.h"
#include "Connectparam.h"
#include "PacketCtl.h"
#include "FirewallPortList.h"
#include "ServerOptions.h"

#define	TIMER_INTERVAL	0.1

class ControlClient : public Obj
{
public:
	ControlClient();
	virtual ~ControlClient();
	void		SetErrMsgHandler(ErrMsgHandler _emh, void* _emhThis);
	void		SetFirewallPortList(FirewallPortList& _fwpl);
	void		SetControlType(PCSTR type);

	bool		Connect(PCSTR server, int port, PCSTR password);
	virtual bool	Disconnect();
	virtual void	Connected() {};		// callback when a connection is made
	bool		IsConnected();
	int			GetRBufSockFd();

	bool		SendWantPlayMessages(bool want);
	bool		SendPlayerMessage(PCSTR message, PCSTR player = NULL, PlayerType = PlayerAny);
	bool		SendPlayerRank(PCSTR player, PlayerType pt, int rank, DFLOAT rate);

private:
	bool		NetInit();
	void		NetCleanup();
	bool		NetVerify();
	int			ReceiveMagic();

	bool		ProcessReceived(uchar type);

	bool		ContactServer(PCSTR serverName, int serverPort);
	bool		LoginToServer(PCSTR password);
	bool		GetContactMessage(Sockbuf& sbuf);
	int			GetReplyMessage(Sockbuf& sbuf);
	String		GetErrorString(int status);
protected:
	int			SendReliable();
	int			SendAck(long rel_loops);
	void		IHaveSpoken();	// perform this after a flush().

	int			ReceiveReply(int *replyto, int *result);

// Fltk (and other) interface routines
// ControlClient takes two external stimulii; a socket selected, and a timer tick
public:
	int			RbufSelected();
	virtual int	TimerTick();


// Overridable
protected:
			int		ReceiveUnhandled();
	// UDP receive
			int		ReceiveReliable();
			int		ReceiveAck();
	// Reliable receive
	virtual int		ReceiveQuit();
			int		ReceiveCtl();
	// Control (PKT_CTL) receive
	virtual int		ReceiveOptionCount()	{ return(0); };
	virtual int		ReceiveSetOption();
	virtual int		ReceiveGetScore()		{ return(0); }
	virtual int		ReceiveGetRanks()		{ return(0); }
	virtual int		ReceiveSetScore()		{ return(0); }
	virtual int		ReceivePlayerEvent()	{ return(0); }
	virtual int		ReceiveScoreEvent()		{ return(0); }
	virtual int		ReceivePlayMessage()	{ return(0); }
	virtual int		ReceiveWorldInfo()		{ return(0); }
	virtual int		ReceiveServerInfo()		{ return(0); }

public:
	String			serverName;
	String			serverIP;
	int				serverPort;
	String			serverVersion;

protected:
	Connectparam	conpar;				// disp_name is the control type
	String			contactServer;
	String			myHostName;
    long			key;

	ErrMsgHandler	emh;
	void*			emhThis;
	ServerOptions*	sos;

    Sock			sock;
	Sockbuf			rbuf;
	Sockbuf			wbuf;
	Sockbuf			crbuf;
	Sockbuf			cwbuf;
//	Framebuf*		Frames;
	long			reliableOffset;
	time_t			lastSpoken;		// last time we spoke to the server
	FirewallPortList	fwpl;
	bool			connected;		/* are we connected to a server? */

private:
    long		last_send_loops;	/* last update of reliable */
	long		main_loops;			/* needed in events.c */
    long		reliable_offset;	/* amount of data acked */
    long		reliable_unsent;	/* next unsend reliable byte */
    long		retransmit_at_loop;	/* next retransmission time */
    int			acks;				/* good acknowledgements */
    double		rtt_retransmit;		/* retransmission time */
    int			rtt_timeouts;		/* how many timeouts */
    int			rtt_smoothed;		/* smoothed roundtrip time */
    int			rtt_dev;			/* roundtrip time deviation */

public:
#if defined(_WINDOWS) && !defined(_CYGWIN)
	void	SetWindowMsg(HWND hw, UINT m);
	HWND	hWnd;			// handle to window to receive selects
	UINT	wMsg;			// msg of interest
#endif

};

//inline void ControlClient::SetServerOptions(ServerOptions* _sos) { sos = _sos; };
inline int ControlClient::GetRBufSockFd() { return(rbuf.sock.fd); };
inline bool ControlClient::IsConnected() { return connected; };

#endif
