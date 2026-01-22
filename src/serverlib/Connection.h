/* $Id: Connection.h,v 1.22 2007/01/18 21:11:08 dick Exp $
 *
 * Describe a server's connection to a client.
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
 *  $Log: Connection.h,v $
 *  Revision 1.22  2007/01/18 21:11:08  dick
 *  Move MAX_RELIABLE_DATA_PACKET_SIZE to packet.h
 *
 *  Revision 1.21  2004/05/31 18:17:25  dick
 *  virtual destructors so subclasses don't leak.
 *
 *  Revision 1.20  2004/04/23 17:41:57  dick
 *  StateToText() is public
 *
 *  Revision 1.19  2004/03/04 03:47:32  dick
 *  Save off the version that the client reported as origVersion.
 *  This is for stats only and doesn't affect gameplay.
 *
 *  Revision 1.18  2004/02/18 04:41:17  dick
 *  Make protected and private a bunch of attributes that should have been.
 *  new InitBuffers(Sock*) replaces the only place they were exposed.
 *
 *  Revision 1.17  2004/02/18 01:19:29  dick
 *  GetAddr and GetDpy move to Connection where the attributes live (not ConnectionPlayer)
 *
 *  Revision 1.16  2004/02/16 14:29:44  dick
 *  server/sched.cpp and friends become serverlib/NetServer
 *  which disconnects the udp listen and receive functionality from the game.
 *
 *  Revision 1.15  2002/09/11 16:42:03  dick
 *  Here endeth the server encapsulation task.
 *  theWorld is now an array of World(s) and get loaded dynamically.
 *
 *  Revision 1.14  2002/09/09 23:42:55  dick
 *  Handle connecting a LocalCtl to this server.
 *
 *  Revision 1.13  2002/08/01 14:39:19  dick
 *  Add cookie as a member variable.
 *
 *  Revision 1.12  2002/06/30 06:51:55  dick
 *  Bump the LISTEN_TIMEOUT from 4 to 10 because a Purified client would timeout
 *  during the Listen state.
 *
 *  Revision 1.11  2002/06/23 06:33:35  dick
 *  Tag a ConnectionControl with our type as a PCSTR
 *
 *  Revision 1.10  2002/06/01 06:06:47  dick
 *  Encapsulate (almost) everything.  Get rid of (almost) all refs to theWorld.
 *
 *  Revision 1.9  2002/05/28 19:47:25  dick
 *  White space
 *
 *  Revision 1.8  2002/05/18 20:55:38  dick
 *  Update to XPilot-4.5.1
 *
 *  Revision 1.7  2001/09/03 00:45:00  dick
 *  New packet type Nop, used for a keepalive ping.
 *
 *  Revision 1.6  2001/08/31 20:58:13  dick
 *  Server needs to be able to receive reliable packets as well as send them.
 *  Use the new emh error handling.
 *
 *  Revision 1.5  2001/08/26 10:34:26  dick
 *  Handle a connection from a control.
 *
 *  Revision 1.4  2001/08/13 03:03:43  dick
 *  SockbufClient becomes ControlClient
 *
 *  Revision 1.3  2001/08/11 09:38:20  dick
 *  new class ConnectionControl, a Connection for a control.
 *
 *  Revision 1.2  2001/08/10 08:11:37  dick
 *  New class ConnectionPlayer subclasses Connection to provide player
 *  functions.  New class Frame subclasses ConnectionPlayer which provides
 *  Frame update functionality.
 *
 *  Revision 1.1  2001/08/09 07:43:34  dick
 *  New class Connection
 *
 */

#ifndef	_CONNECTION_H_
#define	_CONNECTION_H_

#include "Sockbuf.h"

//class shipobj;
//class World;

/*
 * In order to not let the server be locked by a collection
 * of idle connections we timeout a client if it doesn't
 * continue with logging in in a reasonable tempo.
 * Sorry, our resources are limited.
 * But the timeout should be easily configurable.
 * The timeout specifies the number of seconds each connection
 * state may last.
 */
#define LISTEN_TIMEOUT		10
#define SETUP_TIMEOUT		15
#define LOGIN_TIMEOUT		40
#define READY_TIMEOUT		40
#define IDLE_TIMEOUT		30
#define	CTL_TIMEOUT			40


/*
 * Different states a connection can be in.
 */
#define CONN_FREE		0x00	/* free for use */
#define CONN_LISTENING	0x01	/* before connect() */
#define CONN_SETUP		0x02	/* after verification */
#define CONN_LOGIN		0x04	/* after setup info transferred */
#define CONN_PLAYING	0x08	/* when actively playing */
#define CONN_DRAIN		0x20	/* wait for all reliable data to be acked */
#define CONN_READY		0x40	/* draining after LOGIN and before PLAYING */
#define	CONN_CTL		0x80	/* is a control connection */

// MOTD
#define MAX_MOTD_CHUNK			512
#define MAX_MOTD_SIZE			(30*1024)
#define MAX_MOTD_LOOPS			10.0		// seconds

enum ConnectionType {
	ctNone,
	ctPlayer,
	ctScoreServer,
	ctLocalCtl,
};

/*
 * All the connection state info.
 * Some of it is hardly ever used, if at all.
 */
class Connection
{
	friend class NetServer;
public:
	Connection();
	virtual ~Connection();
	static void InitReceiveTables();

	bool		InitBuffers(Sock* sock);
	void		SetErrMsgHandler(ErrMsgHandler _emh, void* _emhThis);

	PCSTR		GetAddr();
	PCSTR		GetDpy();

	void		SetState(int state, int drain_state);
	int			SendReliable();
	int			SendReply(int replyto, int result);
	int			HandleListening();	// Handle a connection that is in the listening state

	static void	HandleInput(int fd, void *arg);
	void		HandleInput();

	int			SendShutdown(int count, int delay);


public:
	ConnectionType	connType;
	int 		myInd;				/* my index into the Conn table */

	int 		state;				/* state of connection */
	unsigned	magic;				/* magic cookie */
	PCSTR		ctl;				/* if a control, the control type */


	int 		my_port;			/* server port for this player */
	int 		his_port;			/* client port for this player */
	int 		id; 				/* index into GetInd[] or NO_ID */
	uint		origVersion;		/* Original client version (what he asked for) stat use only */
	unsigned	version;			/* XPilot negotiated version of client */
	String		real;				/* real login name of player */
	String		nick;				/* nickname of player */
	String		dpy;				/* display of player */
	String		addr;				/* address of players host */
	String		host;				/* hostname of players host */
	uint		cookie; 			/* authentication cookie */


public:
	virtual int		HandleSetup() {return 0; }
	virtual	void	DestroyConnection(const char *reason);

	// the receive functions.  Override the ones you care about
	virtual int		ReceiveNop();
	virtual int		ReceiveReliable();
	virtual	int		ReceiveAck();
	virtual	int		ReceiveDiscard();
	virtual	int		ReceiveUndefined();
	virtual	int		ReceiveQuit();
	virtual	int		ReceiveMotd() {return 0;}
	virtual	int		ReceiveShape() {return 0;}
	virtual	int		ReceiveTalk() {return 0;}

	virtual	int		ReceivePlay() {return 0;}
	virtual	int		ReceivePower() {return 0;}
	virtual	int		ReceiveDisplay() {return 0;}
	virtual	int		ReceiveModifierBank() {return 0;}
	virtual	int		ReceiveAudioRequest() {return 0;}
	virtual	int		ReceiveFpsRequest() {return 0;}
	virtual	int		ReceiveKeyboard() {return 0;}
	virtual	int		ReceiveAckCannon() {return 0;}
	virtual	int		ReceiveAckFuel() {return 0;}
	virtual	int		ReceiveAckTarget() {return 0;}
	virtual	int		ReceivePointerMove() {return 0;}

	virtual	int		ReceiveCtl() {return 0;};

private:
	int			SendAck(long rel_loops);

	int 		drainState; 		/* state after draining done */
	long		reliableOffset;
	long		timeout;			/* time when state timeouts */
	long		last_send_loops;	/* last update of reliable */
	long		reliable_offset;	/* amount of data acked */
	long		reliable_unsent;	/* next unsend reliable byte */
	long		retransmit_at_loop; /* next retransmission time */
	int 		rtt_smoothed;		/* smoothed roundtrip time */
	int 		rtt_dev;			/* roundtrip time deviation */
	int 		rtt_retransmit; 	/* retransmission time */
	int 		rtt_timeouts;		/* how many timeouts */
	int 		acks;				/* good acknowledgements */

protected:
	ErrMsgHandler	emh;			/* Error message output handler */
	void*			emhThis;		/* The "this" of the outputter.  Usually a window, maybe stdout */

	int 		setup;				/* amount of setup done */
	long		start;				/* time of last state change */
	Sockbuf 	r;					/* input buffer */
	Sockbuf 	w;					/* output buffer */
	Sockbuf 	cr; 				/* reliable data buffer */
	Sockbuf 	cw; 				/* reliable data buffer */
};

inline PCSTR Connection::GetAddr() {return(addr); }
inline PCSTR Connection::GetDpy() {return(dpy); }

extern	PCSTR	s_generic;
extern	PCSTR	s_ScoreServer;

extern	String	StateToText(int state);


#endif			// _CONNECTION_H_
