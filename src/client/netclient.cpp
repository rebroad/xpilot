/* $Id: netclient.cpp,v 1.32 2004/06/03 06:14:27 dick Exp $
 *
 * netclient - receive stuff from the server and decode it.
 *
 * client - the user interface to the game.
 *
 * XPilot, a multiplayer gravity war game.	Copyright (C) 1991-2001 by
 *
 *		Bj�rn Stabell		 <bjoern@xpilot.org>
 *		Ken Ronny Schouten	 <ken@xpilot.org>
 *		Bert Gijsbers		 <bert@xpilot.org>
 *		Dick Balaska		 <dick@xpilot.org>
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
 * $Log: netclient.cpp,v $
 * Revision 1.32  2004/06/03 06:14:27  dick
 * winAudio.* is gone.
 *
 * Revision 1.31  2004/06/03 06:04:42  dick
 * struct other_t becomes class Other.
 * array Others becomes ObjList others.
 * shipobj becomes ShipObj.
 *
 * Revision 1.30  2004/05/28 18:02:29  dick
 * Send_shape() takes a PCSTR, not a char*.
 *
 * Revision 1.29  2004/05/23 23:52:06  dick
 * src/common/IniClient.Defaults.cpp
 *
 * Revision 1.28  2004/05/20 22:23:42  dick
 * Add AddMessage(PCSTR, ...).  Wrap some debug with a D().
 *
 * Revision 1.27  2004/05/07 04:45:04  dick
 * Display rank data in the ScoreTable.
 *
 * Revision 1.26  2004/05/02 03:08:00  dick
 * Audio support.  Separate client and server audio support.
 * Servers will probably always build with -DSOUND
 * Clients will build -DSOUND only if OpenAL is present.
 *
 * Revision 1.25  2004/05/01 20:37:36  dick
 * Use the new OpenAL audio object
 *
 * Revision 1.24  2004/04/26 07:03:47  dick
 * ReceiveScoreTable determines the various pages we can display
 *
 * Revision 1.23  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.22  2004/01/20 15:14:26  dick
 * Get rid of #define WINDOWSCALING.  Window scaling is always built, it's not an option.
 *
 * Revision 1.21  2004/01/16 01:35:07  dick
 * Turn off the "Eyes" indicator on the client when a player unpauses.
 *
 * Revision 1.20  2004/01/06 04:48:27  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 * Revision 1.19  2002/09/16 21:42:04  dick
 * Use Stringized Connectparam.
 *
 * Revision 1.18  2002/09/07 17:22:06  dick
 * Cluster (C R Johnson) found a bug where frame pointers were being compared,
 * not the loop counter data in the pointer.
 *
 * Revision 1.17  2002/09/02 15:42:40  dick
 * Singleton ini becomes iniClient.
 *
 * Revision 1.16  2002/09/01 02:47:46  dick
 * Get rid of xp++ version checking
 *
 * Revision 1.15  2002/08/01 15:01:27  dick
 * Send the cookie with the verify message to the server. Retab.
 *
 * Revision 1.14  2002/07/11 16:36:03  dick
 * Update to XPilot-4.5.4beta
 *
 * Revision 1.13  2002/06/26 06:47:03  dick
 * Replace errno with xperrno and errno=0 with seterrno(0).
 *
 * Revision 1.12  2002/05/18 20:55:33  dick
 * Update to XPilot-4.5.1
 *
 * Revision 1.11  2001/09/11 07:33:44  dick
 * Encapsulate all client resources into new class Ini.
 *
 * Revision 1.10  2001/08/31 20:47:28  dick
 * Whitespace
 *
 * Revision 1.9  2001/08/14 21:51:35  dick
 * New functionality !anonymousViewing.  When you are viewing someone, everyone
 * gets notified.  This appears in the score table as if you have declared war
 * on the player.  Older clients actually SendWar().  New clients reuse PKT_EYES
 * which was previously defined as a server SendEyes() and client HandleEyes(),
 * neither of which were actually ever used.
 *
 * Revision 1.8  2001/08/08 00:37:17  dick
 * usleep becomes microdelay.
 *
 * Revision 1.7  2001/08/07 22:14:22  dick
 * usleep becomes microdelay.
 *
 * Revision 1.6  2001/08/07 21:00:06  dick
 * error becomes sockError
 *
 * Revision 1.5  2001/08/06 05:41:49  dick
 * struct sock_t becomes class Sock
 *
 * Revision 1.4  2001/07/29 23:40:49  dick
 * Linux gcc tweaks.  The newer versions of g++ has stricter (and different)
 * requirements.
 *
 * Revision 1.3  2001/07/29 06:01:46  dick
 * net.cpp becomes a very nice class Sockbuf.
 *
 * Revision 1.2  2001/07/27 05:47:39  dick
 * Client builds and runs in C++
 *
 */
#include "StdAfx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>

#if defined(_UNIX) || defined(_CYGWIN)
# include <unistd.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netdb.h>
# include <sys/param.h>
# if !defined(__hpux) && !defined(_CYGWIN)
#  include <sys/time.h>
# endif
# include <X11/Xlib.h>
#else
# include "NT/winclient.h"
# include "NT/winNet.h"
# include "NT/winX.h"
# include "NT/winXThread.h"
# include "NT/winXXPilot.h"
#endif

#ifdef VMS
# include <socket.h>
# include <in.h>
#endif

#include "version.h"
#include "config.h"
#include "const.h"
#include "error.h"
#include "Sockbuf.h"
#include "Connectparam.h"
#include "netclient.h"
#include "setup.h"
#include "packet.h"
#include "bit.h"
#include "paint.h"
#include "xinit.h"
#include "pack.h"
#include "types.h"
#include "Sock.h"
#include "protoclient.h"
#include "portability.h"
#include "talk.h"
#include "commonproto.h"
#include "microdelay.h"
#include "Ini.h"
#include "password.h"
#include "ScoreTable.h"

#ifdef	SOUND
#include "Audio.h"
#endif

char netclient_version[] = VERSION;

#define TALK_RETRY		2
#define MAX_MAP_ACK_LEN 500
#define KEYBOARD_STORE	20

/*
 * Type definitions.
 */
typedef struct {
	long				loops;
	Sockbuf*	sbuf;
} frame_buf_t;

/*
 * Exported variables.
 */
setup_t 				*Setup;
//int					receive_window_size;
long					last_loops;
#ifdef _WINDOWS
int 					received_self = FALSE;
#endif
/*
 * Local variables.
 */
static Sockbuf	rbuf,
						cbuf,
						wbuf;
static frame_buf_t		*Frames;
static int				(*receive_tbl[256])(void),
						(*reliable_tbl[256])(void);
static int				keyboard_delta;
static unsigned 		magic;
static long 			last_keyboard_change,
						last_keyboard_ack,
						last_keyboard_update,
						last_send_anything,
						reliable_offset,
						talk_pending,
						talk_sequence_num,
						talk_last_send;
static long 			keyboard_change[KEYBOARD_STORE];
static long 			keyboard_update[KEYBOARD_STORE];
static long 			keyboard_acktime[KEYBOARD_STORE];
static char 			talk_str[MAX_CHARS];

Connectparam		myConpar;

/*
 * Initialize the function dispatch tables.
 * There are two tables.  One for the semi-important unreliable
 * data like frame updates.
 * The other one is for the reliable data stream, which is
 * received as part of the unreliable data packets.
 */
static void Receive_init(void)
{
	int i;

	for (i = 0; i < 256; i++) {
		receive_tbl[i] = NULL;
		reliable_tbl[i] = NULL;
	}

	receive_tbl[PKT_TIME_LEFT]	= Receive_time_left;
	receive_tbl[PKT_AUDIO]		= Receive_audio;
	receive_tbl[PKT_START]		= Receive_start;
	receive_tbl[PKT_END]		= Receive_end;
	receive_tbl[PKT_SELF]		= Receive_self;
	receive_tbl[PKT_DAMAGED]	= Receive_damaged;
	receive_tbl[PKT_CONNECTOR]	= Receive_connector;
	receive_tbl[PKT_LASER]		= Receive_laser;
	receive_tbl[PKT_REFUEL] 	= Receive_refuel;
	receive_tbl[PKT_SHIP]		= Receive_ship;
	receive_tbl[PKT_ECM]		= Receive_ecm;
	receive_tbl[PKT_TRANS]		= Receive_trans;
	receive_tbl[PKT_PAUSED] 	= Receive_paused;
	receive_tbl[PKT_ITEM]		= Receive_item;
	receive_tbl[PKT_MINE]		= Receive_mine;
	receive_tbl[PKT_BALL]		= Receive_ball;
	receive_tbl[PKT_MISSILE]	= Receive_missile;
	receive_tbl[PKT_SHUTDOWN]	= Receive_shutdown;
	receive_tbl[PKT_DESTRUCT]	= Receive_destruct;
	receive_tbl[PKT_SELF_ITEMS] = Receive_self_items;
	receive_tbl[PKT_FUEL]		= Receive_fuel;
	receive_tbl[PKT_CANNON] 	= Receive_cannon;
	receive_tbl[PKT_TARGET] 	= Receive_target;
	receive_tbl[PKT_RADAR]		= Receive_radar;
	receive_tbl[PKT_FASTRADAR]	= Receive_fastradar;
	receive_tbl[PKT_RELIABLE]	= Receive_reliable;
	receive_tbl[PKT_QUIT]		= Receive_quit;
	receive_tbl[PKT_MODIFIERS]	= Receive_modifiers;
	receive_tbl[PKT_FASTSHOT]	= Receive_fastshot;
	receive_tbl[PKT_THRUSTTIME] = Receive_thrusttime;
	receive_tbl[PKT_SHIELDTIME] = Receive_shieldtime;
	receive_tbl[PKT_PHASINGTIME]= Receive_phasingtime;
	receive_tbl[PKT_ROUNDDELAY] = Receive_rounddelay;
	receive_tbl[PKT_LOSEITEM]	= Receive_loseitem;
	receive_tbl[PKT_WRECKAGE]	= Receive_wreckage;
	receive_tbl[PKT_ASTEROID]	= Receive_asteroid;
	receive_tbl[PKT_WORMHOLE]	= Receive_wormhole;
	for (i = 0; i < DEBRIS_TYPES; i++) {
		receive_tbl[PKT_DEBRIS + i] = Receive_debris;
	}

	reliable_tbl[PKT_EYES]		= Receive_eyes;
	reliable_tbl[PKT_MOTD]		= Receive_motd;
	reliable_tbl[PKT_MESSAGE]	= Receive_message;
	reliable_tbl[PKT_TEAM_SCORE] = Receive_team_score;
	reliable_tbl[PKT_PLAYER]	= Receive_player;
	reliable_tbl[PKT_SCORE] 	= Receive_score;
	reliable_tbl[PKT_SCORE_TABLE] 	= ReceiveScoreTable;
	reliable_tbl[PKT_TIMING]	= Receive_timing;
	reliable_tbl[PKT_LEAVE] 	= Receive_leave;
	reliable_tbl[PKT_WAR]		= Receive_war;
	reliable_tbl[PKT_SEEK]		= Receive_seek;
	reliable_tbl[PKT_BASE]		= Receive_base;
	reliable_tbl[PKT_QUIT]		= Receive_quit;
	reliable_tbl[PKT_STRING]	= Receive_string;
	reliable_tbl[PKT_SCORE_OBJECT] = Receive_score_object;
	reliable_tbl[PKT_TALK_ACK]	= Receive_talk_ack;
	reliable_tbl[PKT_COOKIE]	= Receive_cookie;

}

/*
 * Uncompress the map which is compressed using a simple
 * Run-Length-Encoding algorithm.
 * The map object type is encoded in the lower seven bits
 * of a byte.
 * If the high bit of a byte is set then the next byte
 * means the number of contiguous map data bytes that
 * have the same type.	Otherwise only one map byte
 * has this type.
 * Because we uncompress the map backwards to save on
 * memory usage there is some complexity involved.
 */
static int Uncompress_map(void)
{
	u_byte* 	cmp;			/* compressed map pointer */
	u_byte* ump;			/* uncompressed map pointer */
	u_byte* p;						/* temporary search pointer */
	int 		i;
	int 			count;

	if (Setup->map_order != SETUP_MAP_ORDER_XY)
	{
		seterrno(0);
		error("Unknown map ordering in setup (%d)", Setup->map_order);
		return -1;
	}

	/* Point to last compressed map byte */
	cmp = Setup->map_data + Setup->map_data_len - 1;

	/* Point to last uncompressed map byte */
	ump = Setup->map_data + Setup->x * Setup->y - 1;

	while (cmp >= Setup->map_data)
	{
		for (p = cmp; p > Setup->map_data; p--)
		{
			if ((p[-1] & SETUP_COMPRESSED) == 0)
			{
				break;
			}
		}
		if (p == cmp)
		{
			*ump-- = *cmp--;
			continue;
		}
		if ((cmp - p) % 2 == 0)
		{
			*ump-- = *cmp--;
		}
		while (p < cmp)
		{
			count = *cmp--;
			if (count < 2)
			{
				seterrno(0);
				error("Map compress count error %d", count);
				return -1;
			}
			*cmp &= ~SETUP_COMPRESSED;
			for (i = 0; i < count; i++)
			{
				*ump-- = *cmp;
			}
			cmp--;
			if (ump < cmp)
			{
				seterrno(0);
				error("Map uncompression error (%d,%d)",
				cmp - Setup->map_data, ump - Setup->map_data);
				return -1;
			}
		}
	}
	if (ump != cmp)
	{
		seterrno(0);
		error("map uncompress error (%d,%d)",
		cmp - Setup->map_data, ump - Setup->map_data);
		return -1;
	}
	Setup->map_order = SETUP_MAP_UNCOMPRESSED;
	return 0;
}

/*
 * Receive the map data and some game parameters from
 * the server.	The map data may be in compressed form.
 */
int Net_setup(void)
{
	int 		n,
				len,
				size,
				done = 0,
				retries;
	long		todo = sizeof(setup_t);
	char		*ptr;

	if ((Setup = (setup_t *) malloc(sizeof(setup_t))) == NULL) {
		error("No memory for setup data");
		return -1;
	}
	ptr = (char *) Setup;
	while (todo > 0) {
		if (cbuf.ptr != cbuf.buf) {
			cbuf.Advance(cbuf.ptr - cbuf.buf);
		}
		len = cbuf.len;
		if (len > todo) {
			len = todo;
		}
		if (len > 0) {
			if (done == 0) {
				n = cbuf.scanf(
								 "%ld" "%ld%hd" "%hd%hd" "%hd%hd" "%s%s",
								 &Setup->map_data_len,
								 &Setup->mode, &Setup->lives,
								 &Setup->x, &Setup->y,
								 &Setup->frames_per_second, &Setup->map_order,
								 Setup->name, Setup->author);
				if (n <= 0) {
						seterrno(0);
					error("Can't read setup info from reliable data buffer");
					return -1;
				}
				/*
				 * Do some consistency checks on the server setup structure.
				 */
				if (Setup->map_data_len <= 0
					|| Setup->x <= 0
					|| Setup->y <= 0
					|| Setup->map_data_len > Setup->x * Setup->y)
				{
						seterrno(0);
					error("Got bad map specs from server (%d,%d,%d)",
						Setup->map_data_len, Setup->x, Setup->y);
					return -1;
				}
				Setup->width = Setup->x * BLOCK_SZ;
				Setup->height = Setup->y * BLOCK_SZ;
				if (Setup->map_order != SETUP_MAP_ORDER_XY
					&& Setup->map_order != SETUP_MAP_UNCOMPRESSED)
				{
						seterrno(0);
					error("Unknown map order type (%d)", Setup->map_order);
					return -1;
				}
				size = sizeof(setup_t) + Setup->x * Setup->y;
				if ((Setup = (setup_t *) realloc(ptr, size)) == NULL) {
					error("No memory for setup and map");
					return -1;
				}
				ptr = (char *) Setup;
				done = (char *) &Setup->map_data[0] - ptr;
				todo = Setup->map_data_len;
			} else {
				memcpy(&ptr[done], cbuf.ptr, len);
				cbuf.Advance(len + cbuf.ptr - cbuf.buf);
				done += len;
				todo -= len;
			}
		}
		if (todo > 0) {
			if (rbuf.ptr != rbuf.buf) {
				rbuf.Advance(rbuf.ptr - rbuf.buf);
			}
			if (rbuf.len > 0) {
				if (rbuf.ptr[0] != PKT_RELIABLE) {
					if (rbuf.ptr[0] == PKT_QUIT) {
								seterrno(0);
								error("Server closed connection");
								return -1;
					} else {
								seterrno(0);
								error("Not a reliable packet (%d) in setup",
										rbuf.ptr[0]);
								return -1;
					}
				}
				if (Receive_reliable() == -1) {
					return -1;
				}
				if (wbuf.Flush() == -1) {
					return -1;
				}
			}
			if (cbuf.ptr != cbuf.buf) {
				cbuf.Advance(cbuf.ptr - cbuf.buf);
			}
			if (cbuf.len > 0) {
				continue;
			}
			for (retries = 0;; retries++) {
				if (retries >= 10) {
						seterrno(0);
					error("Can't read setup after %d retries "
						  "(todo=%d, left=%d)",
						  retries, todo, cbuf.len - (cbuf.ptr - cbuf.buf));
					return -1;
				}
				rbuf.sock.SetTimeout(2, 0);
				while (rbuf.sock.Readable() > 0) {
					rbuf.Clear();
					if (rbuf.Read() == -1) {
						error("Can't read all setup data");
						return -1;
					}
					if (rbuf.len > 0) {
						break;
					}
					rbuf.sock.SetTimeout(0, 0);
				}
				if (rbuf.len > 0) {
					break;
				}
			}
		}
	}
	if (Setup->map_order != SETUP_MAP_UNCOMPRESSED) {
		if (Uncompress_map() == -1) {
			return -1;
		}
	}

	return 0;
}

/*
 * Send the first packet to the server with our name,
 * nick and display contained in it.
 * The server uses this data to verify that the packet
 * is from the right UDP connection, it already has
 * this info from the ENTER_GAME_pack.
 */
#define MAX_VERIFY_RETRIES		5
//int Net_verify(char *real, char *nick, char *disp, int my_team)
int Net_verify(Connectparam* conpar)
{
	int 		n,
				type,
				result,
				retries;
	time_t		last;

	iniClient.team = conpar->team;

	for (retries = 0;;) {
		if (retries == 0
			|| time(NULL) - last >= 3) {
			if (retries++ >= MAX_VERIFY_RETRIES) {
						seterrno(0);
						error("Can't connect to server after %d retries", retries);
						return -1;
			}
			wbuf.Clear();
/*				IFWINDOWS( Trace("Verifying to sock=%d\n", wbuf.sock); ) */
			n = wbuf.printf("%c%s%s%s", PKT_VERIFY,
				(PCSTR)conpar->realName, (PCSTR)conpar->nick, (PCSTR)conpar->dispName);
			if (conpar->serverVersion >= 0x5000)
				wbuf.printf("%d", conpar->cookie);
			if (n <= 0 || wbuf.Flush() <= 0)
			{
				error("Can't send verify packet");
				return -1;
			}
			time(&last);
#ifndef SILENT
			if (retries > 1) {
				printf("Waiting for verify response\n");
				IFWINDOWS( Progress("Waiting for verify response"); )
			}
#endif
		}
		rbuf.sock.SetTimeout(1, 0);
		if (rbuf.sock.Readable() == 0) {
			continue;
		}
		rbuf.Clear();
		if (rbuf.Read() == -1) {
			error("Can't read verify reply packet");
			return -1;
		}
		if (rbuf.len <= 0) {
			continue;
		}
		if (rbuf.ptr[0] != PKT_RELIABLE) {
			if (rbuf.ptr[0] == PKT_QUIT) {
				seterrno(0);
				error("Server closed connection");
				return -1;
			} else {
				seterrno(0);
				error("Bad packet type when verifying (%d)", rbuf.ptr[0]);
				return -1;
			}
		}
		if (Receive_reliable() == -1) {
			return -1;
		}
		if (wbuf.Flush() == -1) {
			return -1;
		}
		if (cbuf.len == 0) {
			continue;
		}
		if (Receive_reply(&type, &result) <= 0) {
			seterrno(0);
			error("Can't receive verify reply packet");
			return -1;
		}
		if (type != PKT_VERIFY) {
			seterrno(0);
			error("Verify wrong reply type (%d)", type);
			return -1;
		}
		if (result != PKT_SUCCESS) {
			seterrno(0);
			error("Verification failed (%d)", result);
			return -1;
		}
		if (Receive_magic() <= 0) {
			error("Can't receive magic packet after verify");
			return -1;
		}
		break;
	}
#ifndef SILENT
	if (retries > 1) {
		printf("Verified correctly\n");
		IFWINDOWS( Progress("Verified correctly"); )
	}
#endif
	return 0;
}

/*
 * Open the datagram socket and allocate the network data
 * structures like buffers.
 * Currently there are three different buffers used:
 * 1) wbuf is used only for sending packets (write/printf).
 * 2) rbuf is used for receiving packets in (read/scanf).
 * 3) cbuf is used to copy the reliable data stream
 *	  into from the raw and unreliable rbuf packets.
 *
 * server == NULL sets up for internal simulation
 */
int Net_init(PCSTR server, int port)
{
	int 				i;
	unsigned			size;
	Sock				sock;

#ifndef _WINDOWS
	signal(SIGPIPE, SIG_IGN);
#endif

	Receive_init();
	if (!iniClient.clientPortStart
		 || !iniClient.clientPortEnd
		 || (iniClient.clientPortStart > iniClient.clientPortEnd))
	{
		if (sock.OpenUdp(NULL, 0) == SOCK_IS_ERROR)
		{
			error("Cannot create datagram socket (%d)", sock.sockError.error);
			return -1;
		}
	}
	else
	{
		int found_socket = 0;
		for (i = iniClient.clientPortStart; i <= iniClient.clientPortEnd; i++)
		{
			if (sock.OpenUdp(NULL, i) != SOCK_IS_ERROR)
			{
				found_socket = 1;
				break;
			}
		}
		if (found_socket == 0)
		{
			error("Could not find a usable port in given port range");
			return -1;
		}
	}


	if (server && sock.Connect(server, port) == -1) {
		error("Can't connect to server %s on port %d", server, port);
		sock.Close();
		return -1;
	}
	wbuf.sock = sock;
	if (sock.SetNonBlocking(1) == -1) {
		error("Can't make socket non-blocking");
		return -1;
	}
	if (sock.SetSendBufferSize(CLIENT_SEND_SIZE + 256) == -1) {
		error("Can't set send buffer size to %d", CLIENT_SEND_SIZE + 256);
	}
	if (sock.SetReceiveBufferSize(CLIENT_RECV_SIZE + 256) == -1) {
		error("Can't set receive buffer size to %d", CLIENT_RECV_SIZE + 256);
	}

	size = iniClient.receiveWindowSize * sizeof(frame_buf_t);
	if ((Frames = (frame_buf_t *) malloc(size)) == NULL) {
		error("No memory (%u)", size);
		return -1;
	}
	for (i = 0; i < iniClient.receiveWindowSize; i++) {
		Frames[i].loops = 0;
		Frames[i].sbuf = new Sockbuf;
		if (Frames[i].sbuf->Init(&sock, CLIENT_RECV_SIZE,
						 SOCKBUF_READ | SOCKBUF_DGRAM) == -1) {
			error("No memory for read buffer (%u)", CLIENT_RECV_SIZE);
			return -1;
		}
	}

	/* reliable data buffer, not a valid socket filedescriptor needed */
	if (cbuf.Init(NULL, CLIENT_RECV_SIZE,
					 SOCKBUF_WRITE | SOCKBUF_READ | SOCKBUF_LOCK) == -1) {
		error("No memory for control buffer (%u)", CLIENT_RECV_SIZE);
		return -1;
	}

	/* write buffer */
	if (wbuf.Init(&sock, CLIENT_SEND_SIZE,
					 SOCKBUF_WRITE | SOCKBUF_DGRAM) == -1) {
		error("No memory for write buffer (%u)", CLIENT_SEND_SIZE);
		return -1;
	}

	/* read buffer */
	rbuf = *Frames[0].sbuf;

	/* reliable data byte stream offset */
	reliable_offset = 0;

	/* reset talk status */
	talk_sequence_num = 0;
	talk_pending = 0;

	return 0;
}

/*
 * Cleanup all the network buffers and close the datagram socket.
 * Also try to send the server a quit packet if possible.
 * Because this quit packet may get lost we send one at the
 * beginning and one at the end.
 */
void Net_cleanup(void)
{
	int 		i;
	Sock		sock = wbuf.sock;
	char		ch;

	if (sock.fd > 2) {
		ch = PKT_QUIT;
		if (sock.Write(&ch, 1) != 1) {
			sock.GetError();
			sock.Write(&ch, 1);
		}
		microdelay((unsigned)50*1000);
	}
	if (Frames != NULL) {
		for (i = 0; i < iniClient.receiveWindowSize; i++) {
			//if (Frames[i].sbuf.buf != NULL) {
				// delete Frames[i].sbuf;
			// } else {
				// break;
			// }
		}
		free(Frames);
		Frames = NULL;
	}
	cbuf.Cleanup();
	wbuf.Cleanup();
	if (Setup != NULL) {
		free(Setup);
		Setup = NULL;
	}
	if (sock.fd > 2) {
		ch = PKT_QUIT;
		if (sock.Write(&ch, 1) != 1) {
			sock.GetError();
			sock.Write(&ch, 1);
		}
		microdelay((unsigned)50*1000);
		if (sock.Write(&ch, 1) != 1) {
			sock.GetError();
			sock.Write(&ch, 1);
		}
		sock.Close();
	}
}

/*
 * Calculate a new `keyboard-changed-id' which the server has to ack.
 */
void Net_key_change(void)
{
	last_keyboard_change++;
	Key_update();
}

/*
 * Flush the network output buffer if it has some data in it.
 * Called by the main loop before blocking on a select(2) call.
 */
int Net_flush(void)
{
	if (wbuf.len == 0) {
		wbuf.ptr = wbuf.buf;
		return 0;
	}
	if (last_keyboard_ack != last_keyboard_change) {
		/*
		 * Since 3.2.10: just call Key_update to add our keyboard vector.
		 * Key_update will call Send_keyboard to flush our buffer.
		 */
		return Key_update();
	}
	Send_talk();
	if (wbuf.Flush() == -1) {
		return -1;
	}
	wbuf.Clear();
	last_send_anything = last_loops;
	return 1;
}

/*
 * Return the socket filedescriptor for use in a select(2) call.
 */
int Net_fd(void)
{
	return rbuf.sock.fd;
}

/*
 * Try to send a `start play' packet to the server and get an
 * acknowledgement from the server.  This is called after
 * we have initialized all our other stuff like the user interface
 * and we also have the map already.
 */
int Net_start(void)
{
	int 				retries,
						type,
						result;
	time_t				last;

	for (retries = 0;;) {
		if (retries == 0
			|| (time(NULL) - last) > 1) {
			if (retries++ >= 10) {
						seterrno(0);
						error("Can't start play after %d retries", retries);
						return -1;
			}
			wbuf.Clear();
			/*
			 * Some networks have trouble receiving big packets.
			 * Therefore we don't transmit the shipshape when
			 * we have had 5 unsuccesful attempts.
			 */
			if ((retries < 5 && Send_shape(iniClient.shipShape) == -1)
				|| wbuf.printf("%c", PKT_PLAY) <= 0
				|| Client_power() == -1
#ifdef SOUND
				|| Send_audio_request(1) == -1
#endif
				|| Client_fps_request() == -1
				|| wbuf.Flush() == -1) {
				error("Can't send start play packet");
				return -1;
			}
			time(&last);
		}
		if (cbuf.ptr > cbuf.buf) {
			cbuf.Advance(cbuf.ptr - cbuf.buf);
		}
		rbuf.sock.SetTimeout(2, 0);
		while (cbuf.len <= 0
			&& rbuf.sock.Readable() != 0) {
			rbuf.Clear();
			if (rbuf.Read() == -1) {
				error("Error reading play reply");
				return -1;
			}
			if (rbuf.len <= 0) {
				continue;
			}
			if (rbuf.ptr[0] != PKT_RELIABLE) {
				if (rbuf.ptr[0] == PKT_QUIT) {
						seterrno(0);
					error("Server closed connection");
					return -1;
				}
				else if (rbuf.ptr[0] == PKT_START) {
					/*
					 * Packet out of order, drop it or...
					 * Skip the frame and check for a Reliable Data Packet.
					 * (HACK)
					 * In a future version we may not want a reply to
					 * the PKT_PLAY request and accept frames immediately.
					 */
					while (++rbuf.ptr < rbuf.buf + rbuf.len) {
						if (rbuf.ptr[0] == PKT_END
							&& rbuf.ptr + 5 < rbuf.buf + rbuf.len
							&& rbuf.ptr[5] == PKT_RELIABLE) {
							rbuf.ptr += 5;
							break;
						}
					}
					if (rbuf.ptr + 11 >= rbuf.buf + rbuf.len) {
						printf("skipping unexpected frame while starting\n");
						continue;
					}
					printf("abusing unexpected frame while starting\n");
				} else {
					printf("strange packet type while starting (%d)\n",
						rbuf.ptr[0]);
					/*
					 * What the hack do we care when we wanna play.
					 * Just drop the packet for now.
					 */
					rbuf.Clear();
					continue;
				}
			}
			if (Receive_reliable() == -1) {
				return -1;
			}
			if (wbuf.Flush() == -1) {
				return -1;
			}
		}
		if (cbuf.ptr - cbuf.buf >= cbuf.len) {
			continue;
		}
		if (cbuf.ptr[0] != PKT_REPLY) {
				seterrno(0);
			error("Not a reply packet after play (%d,%d,%d)",
				cbuf.ptr[0], cbuf.ptr - cbuf.buf, cbuf.len);
			return -1;
		}
		if (Receive_reply(&type, &result) <= 0) {
				seterrno(0);
			error("Can't receive reply packet after play");
			return -1;
		}
		if (type != PKT_PLAY) {
				seterrno(0);
			error("Can't receive reply packet after play");
			return -1;
		}
		if (result != PKT_SUCCESS) {
				seterrno(0);
			error("Start play not allowed (%d)", result);
			return -1;
		}
		break;
	}
	packet_measure = NULL;
	Net_init_measurement();
		Net_init_lag_measurement();
		seterrno(0);
	return 0;
}

void Net_init_measurement(void)
{
	packet_loss = 0;
	packet_drop = 0;
	packet_loop = 0;
	if (BIT(iniClient.instruments, SHOW_PACKET_LOSS_METER|SHOW_PACKET_DROP_METER) != 0) {
		if (packet_measure == NULL) {
			if ((packet_measure = (char *) malloc(FPS)) == NULL) {
				error("No memory for packet measurement");
				CLR_BIT(iniClient.instruments,
						SHOW_PACKET_LOSS_METER|SHOW_PACKET_DROP_METER);
			} else {
				memset(packet_measure, PACKET_DRAW, FPS);
			}
		}
	}
	else if (packet_measure != NULL) {
		free(packet_measure);
		packet_measure = NULL;
	}
}

void Net_init_lag_measurement(void)
{
	int i;

	packet_lag = 0;
	keyboard_delta = 0;
	for (i = 0; i < KEYBOARD_STORE; i++) {
		keyboard_change[i] = -1;
		keyboard_update[i] = -1;
		keyboard_acktime[i] = -1;
	}
}

/*
 * Process a packet which most likely is a frame update,
 * perhaps with some reliable data in it.
 */
static int Net_packet(void)
{
	int 		type,
				prev_type = 0,
				result,
				replyto,
				status;

#if 0
	{
		static int dropper = 0;
		if (++dropper == 200)
			return(-1);
	}
#endif
	while (rbuf.buf + rbuf.len > rbuf.ptr) {
		type = (*rbuf.ptr & 0xFF);

		if (receive_tbl[type] == NULL) {
				seterrno(0);
			IFNWINDOWS(error("Received unknown packet type (%d, %d), dropping frame.",
						type, prev_type);)
			rbuf.Clear();
			break;
		}
		else if ((result = (*receive_tbl[type])()) <= 0) {
			if (result == -1) {
				if (type != PKT_QUIT) {
						seterrno(0);
					error("Processing packet type (%d, %d) failed", type, prev_type);
				}
				return -1;
			}
			/* Drop rest of incomplete packet */
			rbuf.Clear();
			break;
		}
		prev_type = type;
	}
	while (cbuf.buf + cbuf.len > cbuf.ptr) {
		type = (*cbuf.ptr & 0xFF);
		if (type == PKT_REPLY) {
			if ((result = Receive_reply(&replyto, &status)) <= 0) {
				if (result == 0) {
					break;
				}
				return -1;
			}
			/* should do something more appropriate than this with the reply */
			seterrno(0);
			error("Got reply packet (%d,%d)", replyto, status);
		}
		else if (reliable_tbl[type] == NULL) {
			int i;
				seterrno(0);
			IFNWINDOWS(error("Received unknown reliable data packet type (%d,%d,%d)",
										type, cbuf.ptr - cbuf.buf, cbuf.len);)
			printf("\tdumping buffer for debugging:\n");
			for (i = 0; i < cbuf.len; i++) {
				printf("%3d", cbuf.buf[i] & 0xFF);
				if (i % 20 == 0) {
					printf("\n");
				} else {
					printf(" ");
				}
			}
			printf("\n");
			return -1;
		}
		else if ((result = (*reliable_tbl[type])()) <= 0) {
			if (result == 0) {
				break;
			}
			return -1;
		}
	}

	return 0;
}

static void Net_keyboard_track() {
	int i, ind = -1;
	long maxtime = 0;

	/* look for a match for the keyboard change */
	for (i = 0; i < KEYBOARD_STORE; i++) {
		if (keyboard_change[i] == last_keyboard_change) {
			return;
		}
	}

	/* look for a keyboard_change of -1 or an acknowledged change */
	if (ind == -1) {
		for (i = 0; i < KEYBOARD_STORE; i++) {
			if (keyboard_change[i] == -1
				|| keyboard_acktime[i] != -1) {
				ind = i;
				break;
			}
		}
	}

	/* next, look for the oldest update */
	if (ind == -1) {
		for (i = 0; i < KEYBOARD_STORE; i++) {
			if (keyboard_update[i] > maxtime) {
				ind = i;
				maxtime = keyboard_update[i];
			}
		}
	}

	if (ind == -1) {
		return;
	}

	keyboard_change[ind] = last_keyboard_change;
	keyboard_update[ind] = last_keyboard_update;
	keyboard_acktime[ind] = -1;

#if 0
	printf("T;%d;%ld;%ld ", ind, last_keyboard_change, last_keyboard_update);
#endif
}

/*
 * Do some (simple) packet loss/drop measurement
 * the results of which can be drawn on the display.
 * This is mainly for debugging and analysis.
 */
static void Net_measurement(long loop, int status)
{
	int 		i;
	long		delta;

	if (packet_measure == NULL) {
		return;
	}
	if ((delta = loop - packet_loop) < 0) {
		/*
		 * Duplicate or out of order.
		 */
		return;
	}
	if (delta >= FPS) {
		if (packet_loop == 0) {
			packet_loop = loop - (loop % FPS);
			return;
		}
		packet_loop = loop - (loop % FPS);
		packet_loss = 0;
		packet_drop = 0;
		for (i = 0; i < FPS; i++) {
			switch (packet_measure[i]) {
			case PACKET_LOSS:
				packet_loss++;
				continue;
			case PACKET_DROP:
				packet_drop++;
				break;
			}
			packet_measure[i] = PACKET_LOSS;
		}
		delta = loop - packet_loop;
   }
   if (packet_measure[(int)delta] != PACKET_DRAW) {
	   packet_measure[(int)delta] = status;
   }
}

/* Do some lag measurement the results of which can
 * be drawn on the display.  This is mainly for
 * debugging and analysis.
 */
static void Net_lag_measurement(long key_ack)
{
	int 		i, num;
	long		sum;

	for (i = 0; i < KEYBOARD_STORE; i++) {
		if (keyboard_change[i] == key_ack
			&& keyboard_acktime[i] == -1) {
			keyboard_acktime[i] = loops;
#if 0
			printf("A;%d;%ld;%ld ", i, keyboard_change[i], keyboard_acktime[i]);
#endif
			break;
		}
	}

#if 0
	if (i == KEYBOARD_STORE) {
		printf("N ");
	}
#endif

	num = 0;
	sum = 0;
	for (i = 0; i < KEYBOARD_STORE; i++) {
		if (keyboard_acktime[i] != -1L) {
			num++;
			sum += keyboard_acktime[i] - keyboard_update[i];
		}
	}

	if (num != 0) {
		packet_lag = (int) (sum / num);
	}
}

/*
 * Read a packet into one of the input buffers.
 * If it is a frame update then we check to see
 * if it is an old or duplicate one.  If it isn't
 * a new frame then the packet is discarded and
 * we retry to read a packet once more.
 * It's a non-blocking read.
 */
static int Net_read(frame_buf_t *frame)
{
	int 		n;
	long		loop;
	u_byte		ch;

	frame->loops = 0;
	for (;;) {
		frame->sbuf->Clear();
		if (frame->sbuf->Read() == -1) {
			error("Net input error");
			return -1;
		}
		if (frame->sbuf->len <= 0) {
			frame->sbuf->Clear();
			return 0;
		}
		/*IFWINDOWS( Trace("Net_read: read %d bytes type=%d\n", frame->sbuf.len, frame->sbuf.ptr[0]); ) */
		if (frame->sbuf->ptr[0] != PKT_START) {
			/*
			 * Don't know which type of packet this is
			 * and if it contains a frame at all (not likely).
			 * It could be a quit packet.
			 */
			return 1;
		}
		/* Peek at the frame loop number. */
		n = frame->sbuf->scanf("%c%ld", &ch, &loop);
		/*IFWINDOWS( Trace("Net_read: frame # %d\n", loop); )*/
		frame->sbuf->ptr = frame->sbuf->buf;
		if (n <= 0) {
			if (n == -1) {
				frame->sbuf->Clear();
				return -1;
			}
			continue;
		}
		else if (loop > last_loops) {
			frame->loops = loop;
			return 2;
		} else {
			/*
			 * Packet out of order.  Drop it.
			 * We may have already drawn it if it is duplicate.
			 * Perhaps we should try to extract any reliable data
			 * from it before dropping it.
			 */
		}
	}
		/*IFWINDOWS( Trace("Net_read: wbuf->len=%d\n", wbuf.len); )*/
}

/*
 * Read frames from the net until there are no more available.
 * If the server has floaded us with frame updates then we should
 * discard everything except the most recent ones.	The X server
 * may be too slow to keep up with the rate of the XPilot server
 * or there may have been a network hickup if the net is overloaded.
 */
int Net_input(void)
{
	int 		i,
				j,
				n;
	frame_buf_t*		frame;
		frame_buf_t*	last_frame;
		frame_buf_t*	oldest_frame = &Frames[0];
		frame_buf_t*	tmpframe;

	for (i = 0; i < iniClient.receiveWindowSize; i++) {
		frame = &Frames[i];
		if (!frame)
			continue;
		if (frame->loops != 0) {
			/*
			 * Already contains a frame.
			 */
			if (frame->loops < oldest_frame->loops
				|| oldest_frame->loops == 0) {
				oldest_frame = frame;
			}
		}
		else if (frame->sbuf->len > 0
			&& frame->sbuf->ptr == frame->sbuf->buf) {
			/*
			 * Contains an unidentifiable packet.
			 * No more input until this one is processed.
			 */
			break;
		} else {
			/*
			 * Empty buffer.  Read a frame.
			 */
			if ((n = Net_read(frame)) <= 0) {
				if (n == 0) {
					/*
					 * No more new packets available.
					 */
					if (i == 0) {
						/*
						 * No frames to be processed.
						 */
						return 0;
					}
					break;
				} else {
					return n;
				}
			}
			else if (n == 1) {
				/*
				 * Contains an unidentifiable packet.
				 * No more input until this one is processed.
				 */
				break;
			} else {
				/*
				 * Check for duplicate packets.
				 */
				for (j = i - 1; j >= 0; j--) {
					if (frame->loops == Frames[j].loops) {
						break;
					}
				}
				if (j >= 0) {
					/*
					 * Duplicate.  Drop it.
					 */
					Net_measurement(frame->loops, PACKET_DROP);
					frame->sbuf->Clear();
					frame->loops = 0;
					i--;		/* correct for for loop increment. */
					continue;
				}
				if (frame->loops < oldest_frame->loops) {
					oldest_frame = frame;
				}
			}
		}
		if ((i == iniClient.receiveWindowSize - 1 && i > 0)
#if defined(_WINDOWS) && !defined(_CYGWIN)
				|| drawPending
				|| (iniClient.threadedDraw &&
								!WaitForSingleObject(dinfo.eventNotDrawing, 0) == WAIT_OBJECT_0)
#endif
				) {
			/*
			 * Drop oldest packet.
			 */
			if (oldest_frame->loops < frame->loops) {
				/*
				 * Switch buffers to prevent gaps.
				 */
				tmpframe = oldest_frame;
				oldest_frame = frame;
				frame = tmpframe;
			}
			Net_measurement(frame->loops, PACKET_DROP);
			frame->sbuf->Clear();
			frame->loops = 0;
			oldest_frame = &Frames[0];
			/*
			 * Reset loop index.
			 */
			i = -1; 	/* correct for for loop increment. */
			continue;
		}
	}

	/*
	 * Find oldest packet.
	 */
	last_frame = oldest_frame = &Frames[0];
	for (i = 1; i < iniClient.receiveWindowSize; i++, last_frame++) {
		frame = &Frames[i];
		if (frame->loops == 0) {
			if (frame->sbuf->len > 0) {
				/*
				 * This is an unidentifiable packet.
				 * Process it last, because it arrived last.
				 */
				continue;
			} else {
				/*
				 * Empty.  The rest should be empty too,
				 * because we have taken care not to have gaps.
				 */
				break;
			}
		}
		else if (frame->loops < oldest_frame->loops
			|| oldest_frame->loops == 0) {
			oldest_frame = frame;
		}
	}

	if (oldest_frame->sbuf->len <= 0) {
		/*
		 * Couldn't find a non-empty packet.
		 */
		if (oldest_frame->loops > 0) {
				seterrno(0);
			error("bug %s,%d", __FILE__, __LINE__);
			oldest_frame->loops = 0;
		}
		return 0;
	}

	/*
	 * Let the packet processing routines know which
	 * packet they should process.
	 */
	rbuf = *oldest_frame->sbuf;

	/*
	 * Process the packet.
	 */
	n = Net_packet();

	if (last_frame->loops > oldest_frame->loops) {
		/*
		 * Switch buffers to prevent gaps.
		 */
		tmpframe = oldest_frame;
		oldest_frame = last_frame;
		last_frame = tmpframe;
	}
	last_frame->sbuf->Clear();
	last_frame->loops = 0;
	rbuf = *last_frame->sbuf;

	if (n == -1) {
		return -1;
	}

	/*
	 * If the server hasn't yet acked our last keyboard change
	 * and we haven't updated it in the (previous) current time frame
	 * or if we haven't sent anything for a while (keepalive)
	 * then we send our current keyboard state.
	 */
	if ((last_keyboard_ack != last_keyboard_change
			&& last_keyboard_update /*+ 1*/ < last_loops)
		|| last_loops - last_send_anything > 5 * Setup->frames_per_second) {
		Key_update();
		last_send_anything = last_loops;
	} else {
		/*
		 * 4.5.4a2: flush if non-empty
		 * This should help speedup the map update speed
		 * for maps with large number of targets or cannons.
		 */
		Net_flush();
	}

	return 1 + (last_frame->loops > oldest_frame->loops);
}

/*
 * Receive the beginning of a new frame update packet,
 * which contains the loops number.
 */
int Receive_start(void)
{
	int 		n;
	long		loops;
	u_byte		ch;
	long		key_ack;

	if ((n = rbuf.scanf("%c%ld%ld",
												&ch, &loops, &key_ack)) <= 0) {
		return n;
	}
	if (last_loops >= loops) {
		/*
		 * Packet is duplicate or out of order.
		 */
		Net_measurement(loops, PACKET_DROP);
		printf("ignoring frame (%ld)\n", last_loops - loops);
		return 0;
	}
	last_loops = loops;
	if (key_ack > last_keyboard_ack) {
		if (key_ack > last_keyboard_change) {
			printf("Premature keyboard ack by server (%ld,%ld,%ld)\n",
				   last_keyboard_change, last_keyboard_ack, key_ack);
			/*
			 * Packet could be corrupt???
			 * Some blokes turn off checksumming.
			 */
			return 0;
		}
		else {
			last_keyboard_ack = key_ack;
		}
	}
		Net_lag_measurement(key_ack);
	if ((n = Handle_start(loops)) == -1) {
		return -1;
	}
	return 1;
}

/*
 * Receive the end of a new frame update packet,
 * which should contain the same loops number
 * as the frame head.  If this terminating packet
 * is missing then the packet is corrupt or incomplete.
 */
int Receive_end(void)
{
	int 		n;
	long		loops;
	u_byte		ch;

	if ((n = rbuf.scanf("%c%ld", &ch, &loops)) <= 0) {
		return n;
	}
	Net_measurement(loops, PACKET_DRAW);
	if ((n = Handle_end(loops)) == -1) {
		return -1;
	}
	return 1;
}

/*
 * Receive a message string.  This currently is rather
 * inefficiently encoded as an ascii string.
 */
int Receive_message(void)
{
	int 		n;
	u_byte		ch;
	char		msg[MSG_LEN];

	if ((n = cbuf.scanf("%c%S", &ch, msg)) <= 0) {
		return n;
	}
	if ((n = Handle_message(msg)) == -1) {
		return -1;
	}
	return 1;
}

/*
 * Receive the remaining playing time.
 */
int Receive_time_left(void)
{
	int 		n;
	u_byte		ch;
	long		sec;

	if ((n = rbuf.scanf("%c%ld", &ch, &sec)) <= 0) {
		return n;
	}
	if ((n = Handle_time_left(sec)) == -1) {
		return -1;
	}
	return 1;
}

/*
 * Receive the server MOTD.
 */
int Receive_motd(void)
{
	u_byte				ch;
	long				off,
						size;
	short				len;
	int 				n;
	char				*cbuf_ptr = cbuf.ptr;

	if ((n = cbuf.scanf("%c%ld%hd%ld", &ch, &off, &len, &size)) <= 0) {
		return n;
	}
	if (cbuf.ptr + len > &cbuf.buf[cbuf.len]) {
		cbuf.ptr = cbuf_ptr;
		return 0;
	}
	Handle_motd(off, cbuf.ptr, (int)len, size);
	cbuf.ptr += len;

	return 1;
}

/*
 * Ask the server to send us the server MOTD.
 */
int Net_ask_for_motd(long offset, long maxlen)
{
	if (offset < 0 || maxlen <= 0) {
				seterrno(0);
				error("Bad motd request (%ld, %ld)", offset, maxlen);
				return -1;
	}
	if (wbuf.printf("%c%ld%ld", PKT_MOTD, offset, maxlen) <= 0) {
				seterrno(0);
				error("Can't ask motd");
				return -1;
	}

	return 0;
}


static void Check_view_dimensions(void)
{
	int 				width_wanted = draw_width;
	int 				height_wanted = draw_height;
	int 				srv_width, srv_height;

	width_wanted = (int)(width_wanted * iniClient.scaleFactor + 0.5);
	height_wanted = (int)(height_wanted * iniClient.scaleFactor + 0.5);

	srv_width = width_wanted;
	srv_height = height_wanted;
	LIMIT(srv_height, MIN_VIEW_SIZE, MAX_VIEW_SIZE);
	LIMIT(srv_width, MIN_VIEW_SIZE, MAX_VIEW_SIZE);
	if (ext_view_width != srv_width ||
		ext_view_height != srv_height) {
		Send_display();
	}

	active_view_width = ext_view_width;
	active_view_height = ext_view_height;
	ext_view_x_offset = 0;
	ext_view_y_offset = 0;
	if (width_wanted > ext_view_width) {
		ext_view_width = width_wanted;
		ext_view_x_offset = (width_wanted - active_view_width) / 2;
	}
	if (height_wanted > ext_view_height) {
		ext_view_height = height_wanted;
		ext_view_y_offset = (height_wanted - active_view_height) / 2;
	}
}


/*
 * Receive the packet with counts for all the items.
 * New since pack version 4203.
 */
int Receive_self_items(void)
{
	unsigned			mask;
	int 				i, n;
	u_byte				ch;
	char				*rbuf_ptr_start = rbuf.ptr;
	u_byte				num_items[NUM_ITEMS];

	n = rbuf.scanf("%c%u", &ch, &mask);
	if (n <= 0) {
		return n;
	}
	memset(num_items, 0, sizeof num_items);
	for (i = 0; mask != 0; i++) {
		if (mask & (1 << i)) {
			mask ^= (1 << i);
			if (rbuf.ptr - rbuf.buf < rbuf.len) {
				if (i < NUM_ITEMS) {
					num_items[i] = *rbuf.ptr++;
				} else {
					rbuf.ptr++;
				}
			}
		}
	}
	Handle_self_items(num_items);
	return (rbuf.ptr - rbuf_ptr_start);
}

/*
 * Receive the packet with all player information for the HUD.
 * If this packet is missing from the frame update then the player
 * isn't actively playing, which means he's either damaged, dead,
 * paused or has game over.
 */
int Receive_self(void)
{
	int 		n;
	short		x, y, vx, vy, lockId, lockDist,
				fuelSum, fuelMax;
	u_byte		ch, heading, power, turnspeed, turnresistance,
				nextCheckPoint, lockDir, autopilotLight, currentTank, stat;
	u_byte		num_items[NUM_ITEMS];

	n = rbuf.scanf(
					 "%c"
					 "%hd%hd%hd%hd%c"
					 "%c%c%c"
					 "%hd%hd%c%c"
					 ,
					 &ch,
					 &x, &y, &vx, &vy, &heading,
					 &power, &turnspeed, &turnresistance,
					 &lockId, &lockDist, &lockDir, &nextCheckPoint);
	if (n <= 0) {
		return n;
	}

	if (version >= 0x4203) {
		memset(num_items, 0, sizeof num_items);
	}
	else {
		n = rbuf.scanf(
						 "%c%c%c%c%c"
						 "%c%c%c%c%c"
						 "%c%c%c%c"
						 ,
						 &(num_items[ITEM_CLOAK]),
						 &(num_items[ITEM_SENSOR]),
						 &(num_items[ITEM_MINE]),
						 &(num_items[ITEM_MISSILE]),
						 &(num_items[ITEM_ECM]),

						 &(num_items[ITEM_TRANSPORTER]),
						 &(num_items[ITEM_WIDEANGLE]),
						 &(num_items[ITEM_REARSHOT]),
						 &(num_items[ITEM_AFTERBURNER]),
						 &(num_items[ITEM_TANK]),

						 &(num_items[ITEM_LASER]),
						 &(num_items[ITEM_EMERGENCY_THRUST]),
						 &(num_items[ITEM_TRACTOR_BEAM]),
						 &(num_items[ITEM_AUTOPILOT]));
		if (n <= 0) {
			return n;
		}
	}
	n = rbuf.scanf(
					 "%c%hd%hd"
					 "%hd%hd%c"
					 "%c%c"
					 ,
					 &currentTank, &fuelSum, &fuelMax,
					 &ext_view_width, &ext_view_height, &debris_colors,
					 &stat, &autopilotLight
					 );
	if (n <= 0) {
		return n;
	}

	if (version < 0x4203) {
		if (version >= 0x3720) {
			n = rbuf.scanf("%c%c%c%c",
							 &(num_items[ITEM_EMERGENCY_SHIELD]),
							 &(num_items[ITEM_DEFLECTOR]),
							 &(num_items[ITEM_HYPERJUMP]),
							 &(num_items[ITEM_PHASING])
							 );
			if (n <= 0) {
				return n;
			}
			if (version >= 0x4100) {
				n = rbuf.scanf("%c", &(num_items[ITEM_MIRROR]));
				if (n <= 0) {
					return n;
				}
				if (version >= 0x4201) {
					n = rbuf.scanf("%c", &(num_items[ITEM_ARMOR]));
					if (n <= 0) {
						return n;
					}
				} else {
					num_items[ITEM_ARMOR] = 0;
				}
			}
			else {
				num_items[ITEM_MIRROR] = 0;
				num_items[ITEM_ARMOR] = 0;
			}
		}
		else {
			if (version >= 0x3200) {
				n = rbuf.scanf("%c",
								 &(num_items[ITEM_EMERGENCY_SHIELD]));
				if (n <= 0) {
					return n;
				}
			}
			else {
				num_items[ITEM_EMERGENCY_SHIELD] = 0;
			}
			num_items[ITEM_DEFLECTOR] = 0;
			num_items[ITEM_HYPERJUMP] = 0;
			num_items[ITEM_PHASING] = 0;
			num_items[ITEM_MIRROR] = 0;
			num_items[ITEM_ARMOR] = 0;
		}
	}

	if (debris_colors > num_spark_colors) {
		debris_colors = num_spark_colors;
	}

	Check_view_dimensions();

	Game_over_action(stat);
	Handle_self(x, y, vx, vy, heading,
				(float) power,
				(float) turnspeed,
				(float) turnresistance / 255.0F,
				lockId, lockDist, lockDir,
				nextCheckPoint, autopilotLight,
				num_items,
				currentTank, fuelSum, fuelMax, rbuf.len);

#ifdef _WINDOWS
		received_self = TRUE;
#endif
	return 1;
}

int Receive_modifiers(void)
{
	int 		n;
	char		mods[MAX_CHARS];
	u_byte		ch;

	if ((n = rbuf.scanf("%c%s", &ch, mods)) <= 0) {
		return n;
	}
	if ((n = Handle_modifiers(mods)) == -1)
		return -1;
	return 1;
}

int Receive_refuel(void)
{
	int 		n;
	short		x0, y0, x1, y1;
	u_byte		ch;

	if ((n = rbuf.scanf("%c%hd%hd%hd%hd",
						  &ch, &x0, &y0, &x1, &y1)) <= 0) {
		return n;
	}
	if ((n = Handle_refuel(x0, y0, x1, y1)) == -1) {
		return -1;
	}
	return 1;
}

int Receive_connector(void)
{
	int 		n;
	short		x0, y0, x1, y1;
	u_byte		ch, tractor;

	n = rbuf.scanf("%c%hd%hd%hd%hd%c",
					 &ch, &x0, &y0, &x1, &y1, &tractor);
	if (n <= 0)
		return n;
	if ((n = Handle_connector(x0, y0, x1, y1, tractor)) == -1) {
		return -1;
	}
	return 1;
}

int Receive_laser(void)
{
	int 		n;
	short		x, y, len;
	u_byte		ch, color, dir;

	if ((n = rbuf.scanf("%c%c%hd%hd%hd%c",
						  &ch, &color, &x, &y, &len, &dir)) <= 0) {
		return n;
	}
	if ((n = Handle_laser(color, x, y, len, dir)) == -1) {
		return -1;
	}
	return 1;
}

int Receive_missile(void)
{
	int 		n;
	short		x, y;
	u_byte		ch, dir, len;

	if ((n = rbuf.scanf("%c%hd%hd%c%c", &ch, &x, &y, &len, &dir)) <= 0) {
		return n;
	}
	if ((n = Handle_missile(x, y, len, dir)) == -1) {
		return -1;
	}
	return 1;
}

int Receive_ball(void)
{
	int 		n;
	short		x, y, id;
	u_byte		ch;

	if ((n = rbuf.scanf("%c%hd%hd%hd", &ch, &x, &y, &id)) <= 0) {
		return n;
	}
	if ((n = Handle_ball(x, y, id)) == -1) {
		return -1;
	}
	return 1;
}

int Receive_ship(void)
{
	int 		n, shield, cloak, eshield, phased, deflector;
	short		x, y, id;
	u_byte		ch, dir, flags;

	if ((n = rbuf.scanf(
						  "%c%hd%hd%hd" "%c%c",
						  &ch, &x, &y, &id,
						  &dir, &flags)) <= 0) {
		return n;
	}
	shield = ((flags & 1) != 0);
	cloak = ((flags & 2) != 0);
	eshield = ((flags & 4) != 0);
	phased = ((flags & 8) != 0);
	deflector = ((flags & 0x10) != 0);

	if ((n = Handle_ship(x, y, id, dir, shield, cloak, eshield, phased, deflector)) == -1) {
		return -1;
	}
	return 1;
}

int Receive_mine(void)
{
	int 		n;
	short		x, y, id;
	u_byte		ch, teammine;

	n = rbuf.scanf("%c%hd%hd%c%hd", &ch, &x, &y, &teammine, &id);
	if (n <= 0) {
		return n;
	}
	if ((n = Handle_mine(x, y, teammine, id)) == -1) {
		return -1;
	}
	return 1;
}

int Receive_item(void)
{
	int 		n;
	short		x, y;
	u_byte		ch, type;

	if ((n = rbuf.scanf("%c%hd%hd%c", &ch, &x, &y, &type)) <= 0) {
		return n;
	}
	if (type < NUM_ITEMS) {
		if ((n = Handle_item(x, y, type)) == -1) {
			return -1;
		}
	}
	return 1;
}

int Receive_destruct(void)
{
	int 		n;
	short		count;
	u_byte		ch;

	if ((n = rbuf.scanf("%c%hd", &ch, &count)) <= 0) {
		return n;
	}
	if ((n = Handle_destruct(count)) == -1) {
		return -1;
	}
	return 1;
}

int Receive_shutdown(void)
{
	int 		n;
	short		count, delay;
	u_byte		ch;

	if ((n = rbuf.scanf("%c%hd%hd", &ch, &count, &delay)) <= 0) {
		return n;
	}
	if ((n = Handle_shutdown(count, delay)) == -1) {
		return -1;
	}
	return 1;
}

int Receive_thrusttime(void)
{
	int 		n;
	short		count, max;
	u_byte		ch;

	if ((n = rbuf.scanf("%c%hd%hd", &ch, &count, &max)) <= 0) {
		return n;
	}
	if ((n = Handle_thrusttime(count, max)) == -1) {
		return -1;
	}
	return 1;
}

int Receive_shieldtime(void)
{
	int 		n;
	short		count, max;
	u_byte		ch;

	if ((n = rbuf.scanf("%c%hd%hd", &ch, &count, &max)) <= 0) {
		return n;
	}
	if ((n = Handle_shieldtime(count, max)) == -1) {
		return -1;
	}
	return 1;
}

int Receive_phasingtime(void)
{
	int 		n;
	short		count, max;
	u_byte		ch;

	if ((n = rbuf.scanf("%c%hd%hd", &ch, &count, &max)) <= 0) {
		return n;
	}
	if ((n = Handle_phasingtime(count, max)) == -1) {
		return -1;
	}
	return 1;
}

int Receive_rounddelay(void)
{
	int 		n;
	short		count, max;
	u_byte		ch;

	if ((n = rbuf.scanf("%c%hd%hd", &ch, &count, &max)) <= 0) {
		return n;
	}
	if ((n = Handle_rounddelay(count, max)) == -1) {
		return -1;
	}
	return 1;
}

int Receive_fastshot(void)
{
	int 				n, r, type;

	rbuf.ptr++; /* skip PKT_FASTSHOT packet id */

	if (rbuf.ptr - rbuf.buf + 2 >= rbuf.len) {
		return 0;
	}
	type = (*rbuf.ptr++ & 0xFF);
	n = (*rbuf.ptr++ & 0xFF);
	if (rbuf.ptr - rbuf.buf + (n * 2) > rbuf.len) {
		return 0;
	}
	r = Handle_fastshot(type, (u_byte*)rbuf.ptr, n);
	rbuf.ptr += n * 2;

	return (r == -1) ? -1 : 1;
}

int Receive_debris(void)
{
	int 				n, r, type;

	if (rbuf.ptr - rbuf.buf + 2 >= rbuf.len) {
		return 0;
	}
	type = (*rbuf.ptr++ & 0xFF);
	n = (*rbuf.ptr++ & 0xFF);
	if (rbuf.ptr - rbuf.buf + (n * 2) > rbuf.len) {
		return 0;
	}
	r = Handle_debris(type - PKT_DEBRIS, (u_byte*)rbuf.ptr, n);
	rbuf.ptr += n * 2;

	return (r == -1) ? -1 : 1;
}

int Receive_wreckage(void)		/* since 3.8.0 */
{
	int 				n;
	short				x, y;
	u_byte				ch, wrecktype, size, rot;

	if ((n = rbuf.scanf("%c%hd%hd%c%c%c", &ch, &x, &y,
						  &wrecktype, &size, &rot)) <= 0) {
		return n;
	}
	if (version < 0x4202) {
		/* always color red. */
		wrecktype &= 0x7F;
	}
	if ((n = Handle_wreckage(x, y, wrecktype, size, rot)) == -1) {
		return -1;
	}
	return 1;
}

int Receive_asteroid(void)		/* since 4.4.0 */
{
	int 				n;
	short				x, y;
	u_byte				ch, type_size, type, size, rot;

	if ((n = rbuf.scanf("%c%hd%hd%c%c", &ch, &x, &y,
						  &type_size, &rot)) <= 0) {
		return n;
	}

	type = ((type_size >> 4) & 0x0F);
	size = (type_size & 0x0F);

	if ((n = Handle_asteroid(x, y, type, size, rot)) == -1) {
		return -1;
	}
	return 1;
}

int Receive_wormhole(void)		/* since 4.5.0 */
{
	int 				n;
	short				x, y;
	u_byte				ch;

	if ((n = rbuf.scanf("%c%hd%hd", &ch, &x, &y)) <= 0) {
		return n;
	}

	if ((n = Handle_wormhole(x, y)) == -1) {
		return -1;
	}
	return 1;
}

int Receive_ecm(void)
{
	int 				n;
	short				x, y, size;
	u_byte				ch;

	if ((n = rbuf.scanf("%c%hd%hd%hd", &ch, &x, &y, &size)) <= 0) {
		return n;
	}
	if ((n = Handle_ecm(x, y, size)) == -1) {
		return -1;
	}
	return 1;
}

int Receive_trans(void)
{
	int 				n;
	short				x1, y1, x2, y2;
	u_byte				ch;

	if ((n = rbuf.scanf("%c%hd%hd%hd%hd",
						  &ch, &x1, &y1, &x2, &y2)) <= 0) {
		return n;
	}
	if ((n = Handle_trans(x1, y1, x2, y2)) == -1) {
		return -1;
	}
	return 1;
}

int Receive_paused(void)
{
	int 				n;
	short				x, y, count;
	u_byte				ch;

	if ((n = rbuf.scanf("%c%hd%hd%hd", &ch, &x, &y, &count)) <= 0) {
		return n;
	}
	if ((n = Handle_paused(x, y, count)) == -1) {
		return -1;
	}
	return 1;
}

int Receive_radar(void)
{
	int 				n;
	short				x, y;
	u_byte				ch, size;

	if ((n = rbuf.scanf("%c%hd%hd%c", &ch, &x, &y, &size)) <= 0) {
		return n;
	}

	x = (int)((double)(x * 256) / Setup->width + 0.5);
	y = (int)((double)(y * RadarHeight) / Setup->height + 0.5);

	if ((n = Handle_radar(x, y, size)) == -1) {
		return -1;
	}
	return 1;
}

int Receive_fastradar(void)
{
	int 				n, i, r = 1;
	int 				x, y, size;
	unsigned char		*ptr;

	rbuf.ptr++; /* skip PKT_FASTRADAR packet id */

	if (rbuf.ptr - rbuf.buf >= rbuf.len) {
		return 0;
	}
	n = (*rbuf.ptr++ & 0xFF);
	if (rbuf.ptr - rbuf.buf + (n * 3) > rbuf.len) {
		return 0;
	}
	ptr = (unsigned char *) rbuf.ptr;
	for (i = 0; i < n; i++) {
		x = *ptr++;
		y = *ptr++;
		y |= (*ptr & 0xC0) << 2;
		size = (*ptr & 0x07);
		if (*ptr & 0x20) {
			size |= 0x80;
		}
		ptr++;
		r = Handle_radar(x, y, size);
		if (r == -1) {
			break;
		}
	}
	rbuf.ptr += n * 3;

	return (r == -1) ? -1 : 1;
}

int Receive_damaged(void)
{
	int 				n;
	u_byte				ch, damaged;

	if ((n = rbuf.scanf("%c%c", &ch, &damaged)) <= 0) {
		return n;
	}
	if ((n = Handle_damaged(damaged)) == -1) {
		return -1;
	}
	return 1;
}

int Receive_leave(void)
{
	int 				n;
	short				id;
	u_byte				ch;

	if ((n = cbuf.scanf("%c%hd", &ch, &id)) <= 0) {
		return n;
	}
	if ((n = Handle_leave(id)) == -1) {
		return -1;
	}
	return 1;
}

int Receive_war(void)
{
	int 				n;
	short				robot_id, killer_id;
	u_byte				ch;

	if ((n = cbuf.scanf("%c%hd%hd",
						  &ch, &robot_id, &killer_id)) <= 0) {
		return n;
	}
	if ((n = Handle_war(robot_id, killer_id)) == -1) {
		return -1;
	}
	return 1;
}

/*
 * Receive the id of the player we get frame updates for (game over mode),
 * or perhaps just who this player is locked on.
 */
int Receive_eyes(void)
{
	int 			n;
	short 			watcherId;
	short 			watchedId;
	u_byte			ch;

	if ((n = cbuf.scanf("%c%hd%hd", &ch, &watcherId, &watchedId)) <= 0)
				return n;
	if ((n = Handle_eyes(watcherId, watchedId)) == -1)
				return -1;
	return 1;
}

int Receive_seek(void)
{
	int 				n;
	short				programmer_id, robot_id, sought_id;
	u_byte				ch;

	if ((n = cbuf.scanf("%c%hd%hd%hd", &ch,
						  &programmer_id, &robot_id, &sought_id)) <= 0) {
		return n;
	}
	if ((n = Handle_seek(programmer_id, robot_id, sought_id)) == -1) {
		return -1;
	}
	return 1;
}

int Receive_player(void)
{
	int 				n;
	short				id;
	u_byte				ch, myteam, mychar;
	char				name[MAX_CHARS],
						real[MAX_CHARS],
						host[MAX_CHARS],
						shape[2*MSG_LEN],
						*cbuf_ptr = cbuf.ptr;

	if ((n = cbuf.scanf(
						  "%c%hd%c%c" "%s%s%s" "%S",
						  &ch, &id, &myteam, &mychar,
						  name, real, host,
						  shape)) <= 0) {
		return n;
	}
	name[MAX_NAME_LEN - 1] = '\0';
	real[MAX_NAME_LEN - 1] = '\0';
	host[MAX_HOST_LEN - 1] = '\0';
	if (version > 0x3200) {
		if ((n = cbuf.scanf("%S", &shape[strlen(shape)])) <= 0) {
			cbuf.ptr = cbuf_ptr;
			return n;
		}
	}
	if ((n = Handle_player(id, myteam, mychar, name, real, host, shape)) == -1) {
		return -1;
	}
	return 1;
}

int Receive_score_object(void)
{
	int 				n;
	unsigned short		x, y;
	DFLOAT				score = 0;
	char				msg[MAX_CHARS];
	u_byte				ch;

	if (version < 0x4500) {
		short	rcv_score;
		n = cbuf.scanf("%c%hd%hu%hu%s",
						 &ch, &rcv_score, &x, &y, msg);
		score = rcv_score;
	} else {
		/* newer servers send scores with two decimals */
		int 	rcv_score;
		n = cbuf.scanf("%c%d%hu%hu%s",
						 &ch, &rcv_score, &x, &y, msg);
		score = (DFLOAT)rcv_score / 100;
	}
	if (n <= 0)
		return n;
	if ((n = Handle_score_object(score, x, y, msg)) == -1) {
		return -1;
	}

	return 1;
}

int Receive_score(void)
{
	int 		n;
	short		id, life;
	short		kills = 0;
	short		deaths = 0;
	DFLOAT		score = 0;
	u_byte		ch, mychar, alliance = ' ';

	if (version < 0x4500) {
		short	rcv_score;
		n = cbuf.scanf("%c%hd%hd%hd%c", &ch,
						 &id, &rcv_score, &life, &mychar);
		score = rcv_score;
		alliance = ' ';
	} else {
		/* newer servers send scores with two decimals */
		int 	rcv_score;
		n = cbuf.scanf("%c%hd%d%hd%c%c", &ch,
						 &id, &rcv_score, &life, &mychar, &alliance);
		score = (DFLOAT)rcv_score / 100;
	}
	if (n <= 0) {
		return n;
	}
	if (version >= 0x5000)
	{
		n = cbuf.scanf("%hd%hd", &kills, &deaths);
		if (n <= 0)
			return(n);
	}
	if ((n = Handle_score(id, score, life, mychar, alliance, kills, deaths)) == -1) {
		return -1;
	}
	return 1;
}

int ReceiveScoreTable()
{
	int		n;
	ubyte	ch;
	char	sub;
	ubyte	pages;
	short	id;
	int		rank;
	int		rate;
	char	s[MAX_CHARS];
	String	msg;

	if ((n = cbuf.scanf("%c%c", &ch, &sub)) <= 0)
		return(n);
	switch (sub) {
	case STPages:
		if ((n = cbuf.scanf("%hd", &pages)) <= 0)
			return(n);
		return(HandleScoreTablePages(pages));
		break;
	case STIPVer:
		if ((n = cbuf.scanf("%hd%s", &id, &s)) <= 0)
			return(n);
		return(HandleScoreTableIPVer(id, s));
	case STRank:
		if ((n = cbuf.scanf("%hd%d%d", &id, &rank, &rate)) <= 0)
			return(n);
		return(HandleScoreTableRank(id, rank, (DFLOAT)rate/100.0));
	default:
		msg.printf("Unhandled ScoreTable type %d", sub);
		AddMessage(msg);
	}
	return(1);
}

int Receive_team_score(void)
{
	int 				n;
	u_byte				ch;
	short				team;
	int 				rcv_score;
	DFLOAT				score;

	if ((n = cbuf.scanf("%c%hd%d", &ch, &team, &rcv_score)) <= 0) {
		return n;
	}
	score = (DFLOAT)rcv_score / 100;
	if ((n = Handle_team_score(team, score)) == -1) {
		return -1;
	}
	return 1;
}

int Receive_timing(void)
{
	int 				n,
						check,
						round;
	short				id;
	unsigned short		timing;
	u_byte				ch;

	n = cbuf.scanf("%c%hd%hu", &ch, &id, &timing);
	if (n <= 0) {
		return n;
	}
	check = timing % MAX_CHECKS;
	round = timing / MAX_CHECKS;
	if ((n = Handle_timing(id, check, round)) == -1) {
		return -1;
	}
	return 1;
}

int Receive_fuel(void)
{
	int 				n;
	unsigned short		num, fuel;
	u_byte				ch;

	if ((n = rbuf.scanf("%c%hu%hu", &ch, &num, &fuel)) <= 0) {
		return n;
	}
	if ((n = Handle_fuel(num, fuel << FUEL_SCALE_BITS)) == -1) {
		return -1;
	}
	if (wbuf.len < MAX_MAP_ACK_LEN) {
		wbuf.printf("%c%ld%hu", PKT_ACK_FUEL, last_loops, num);
	}
	return 1;
}

int Receive_cannon(void)
{
	int 				n;
	unsigned short		num, dead_time;
	u_byte				ch;

	if ((n = rbuf.scanf("%c%hu%hu", &ch, &num, &dead_time)) <= 0) {
		return n;
	}
	if ((n = Handle_cannon(num, dead_time)) == -1) {
		return -1;
	}
	if (wbuf.len < MAX_MAP_ACK_LEN) {
		wbuf.printf("%c%ld%hu", PKT_ACK_CANNON, last_loops, num);
	}
	return 1;
}

int Receive_target(void)
{
	int 				n;
	unsigned short		num,
						dead_time,
						damage;
	u_byte				ch;

	if ((n = rbuf.scanf("%c%hu%hu%hu", &ch,
						  &num, &dead_time, &damage)) <= 0) {
		return n;
	}
	if ((n = Handle_target(num, dead_time, damage)) == -1) {
		return -1;
	}
	if (wbuf.len < MAX_MAP_ACK_LEN) {
		wbuf.printf("%c%ld%hu", PKT_ACK_TARGET, last_loops, num);
	}
	return 1;
}

int Receive_base(void)
{
	int 				n;
	short				id;
	unsigned short		num;
	u_byte				ch;

	if ((n = cbuf.scanf("%c%hd%hu", &ch, &id, &num)) <= 0) {
		return n;
	}
	if ((n = Handle_base(id, num)) == -1) {
		return -1;
	}
	return 1;
}

int Receive_magic(void)
{
	int 				n;
	u_byte				ch;

	if ((n = cbuf.scanf("%c%u", &ch, &magic)) <= 0) {
		return n;
	}
	return 1;
}

int Receive_string(void)
{
	int 				n;
	u_byte				ch,
						type;
	unsigned short		arg1,
						arg2;

	if ((n = cbuf.scanf("%c%c%hu%hu",
						  &ch, &type, &arg1, &arg2)) <= 0) {
		return n;
	}
	/*
	 * Not implemented yet.
	 */
	return 1;
}

int Receive_loseitem(void)
{
	int 		n;
	u_byte		pkt;
								/* Most of the Receive_ funcs call a */
								/* Handle_ func but that seems */
								/* unecessary here */
	if ((n = rbuf.scanf("%c%c", &pkt, &lose_item)) <= 0) {
		return n;
	}
	return 1;
}

int Send_ack(long rel_loops)
{
	int 				n;

	if ((n = wbuf.printf("%c%ld%ld", PKT_ACK,
						   reliable_offset, rel_loops)) <= 0) {
		if (n == 0) {
			return 0;
		}
		error("Can't ack reliable data");
		return -1;
	}
	return 1;
}

int Receive_reliable(void)
{
	int 				n;
	short				len;
	u_byte				ch;
	long				rel,
						rel_loops;

	if ((n = rbuf.scanf("%c%hd%ld%ld",
						  &ch, &len, &rel, &rel_loops)) == -1) {
		return -1;
	}
	if (n == 0) {
				seterrno(0);
				error("Incomplete reliable data packet");
				return 0;
	}
#if DEBUG
	if (reliable_offset >= rel + len) {
		printf("Reliable my=%ld pkt=%ld len=%d loops=%ld\n",
			   reliable_offset, rel, len, rel_loops);
	}
#endif
	if (len <= 0) {
				seterrno(0);
				error("Bad reliable data length (%d)", len);
				return -1;
	}
	if (rbuf.ptr + len > rbuf.buf + rbuf.len) {
				seterrno(0);
				error("Not all reliable data in packet (%d,%d,%d)",
					  rbuf.ptr - rbuf.buf, len, rbuf.len);
				rbuf.ptr += len;
				rbuf.Advance(rbuf.ptr - rbuf.buf);
				return -1;
	}
	if (rel > reliable_offset) {
		/*
		 * We miss one or more packets.
		 * For now we drop this packet.
		 * We could have kept it until the missing packet(s) arrived.
		 */
		rbuf.ptr += len;
		rbuf.Advance(rbuf.ptr - rbuf.buf);
		if (Send_ack(rel_loops) == -1) {
			return -1;
		}
		return 1;
	}
	if (rel + len <= reliable_offset) {
		/*
		 * Duplicate data.	Probably an ack got lost.
		 * Send an ack for our current stream position.
		 */
		rbuf.ptr += len;
		rbuf.Advance(rbuf.ptr - rbuf.buf);
		if (Send_ack(rel_loops) == -1) {
			return -1;
		}
		return 1;
	}
	if (rel < reliable_offset) {
		len -= (short)(reliable_offset - rel);
		rbuf.ptr += reliable_offset - rel;
		rel = reliable_offset;
	}
	if (cbuf.ptr > cbuf.buf) {
		cbuf.Advance(cbuf.ptr - cbuf.buf);
	}
	if (cbuf.Write(rbuf.ptr, len) != len) {
				seterrno(0);
				error("Can't copy reliable data to buffer");
				rbuf.ptr += len;
				rbuf.Advance(rbuf.ptr - rbuf.buf);
				return -1;
	}
	reliable_offset += len;
	rbuf.ptr += len;
	rbuf.Advance(rbuf.ptr - rbuf.buf);
	if (Send_ack(rel_loops) == -1) {
		return -1;
	}
	return 1;
}

int Receive_reply(int *replyto, int *result)
{
	int 		n;
	u_byte		type, ch1, ch2;

	n = cbuf.scanf("%c%c%c", &type, &ch1, &ch2);
	if (n <= 0) {
		return n;
	}
	if (n != 3 || type != PKT_REPLY) {
		error("Can't receive reply packet");
		return -1;
	}
	*replyto = ch1;
	*result = ch2;
	return 1;
}

int Send_keyboard(u_byte *keyboard_vector)
{
	int 		size = KEYBOARD_SIZE;

	if (version < 0x3800) {
		/* older servers have a keyboard_size of 8 bytes instead of 9. */
		size--;
	}
	if (wbuf.size - wbuf.len < size + 1 + 4) {
		/* Not enough write buffer space for keyboard state */
		return 0;
	}
	wbuf.printf("%c%ld", PKT_KEYBOARD, last_keyboard_change);
	memcpy(&wbuf.buf[wbuf.len], keyboard_vector, size);
	wbuf.len += size;
	last_keyboard_update = last_loops;
		Net_keyboard_track();
	Send_talk();
	if (wbuf.Flush() == -1) {
		error("Can't send keyboard update");
		return -1;
	}

	return 0;
}

int Send_shape(PCSTR str)
{
	ShipObj*	w;
	char		buf[MSG_LEN];
	char		ext[MSG_LEN];

	w = Convert_shape_str(str);
	Convert_ship_2_string(w, buf, ext, (version < 0x3200) ? 0x3100 : 0x3200);
	Free_ship_shape(w);
	if (wbuf.printf("%c%S", PKT_SHAPE, buf) <= 0) {
		return -1;
	}
	if (version > 0x3200) {
		if (wbuf.printf("%S", ext) <= 0) {
			return -1;
		}
	}
	return 0;
}

int Send_power(DFLOAT power)
{
	if (wbuf.printf("%c%hd", PKT_POWER,
					  (int) (power * 256.0)) == -1) {
		return -1;
	}
	return 0;
}

int Send_power_s(DFLOAT power_s)
{
	if (wbuf.printf("%c%hd", PKT_POWER_S,
					  (int) (power_s * 256.0)) == -1) {
		return -1;
	}
	return 0;
}

int Send_turnspeed(DFLOAT turnspeed)
{
	if (wbuf.printf("%c%hd", PKT_TURNSPEED,
					  (int) (turnspeed * 256.0)) == -1) {
		return -1;
	}
	return 0;
}

int Send_turnspeed_s(DFLOAT turnspeed_s)
{
	if (wbuf.printf("%c%hd", PKT_TURNSPEED_S,
					  (int) (turnspeed_s * 256.0)) == -1) {
		return -1;
	}
	return 0;
}

int Send_turnresistance(DFLOAT turnresistance)
{
	if (wbuf.printf("%c%hd", PKT_TURNRESISTANCE,
					  (int) (turnresistance * 256.0)) == -1) {
		return -1;
	}
	return 0;
}

int Send_turnresistance_s(DFLOAT turnresistance_s)
{
	if (wbuf.printf("%c%hd", PKT_TURNRESISTANCE_S,
					  (int) (turnresistance_s * 256.0)) == -1) {
		return -1;
	}
	return 0;
}

int Receive_quit(void)
{
	unsigned char		pkt;
	Sockbuf*	sbuf;
	char				reason[MAX_CHARS];

	if (rbuf.ptr < rbuf.buf + rbuf.len) {
		sbuf = &rbuf;
	} else {
		sbuf = &cbuf;
	}
	if (sbuf->scanf("%c", &pkt) != 1) {
				seterrno(0);
				error("Can't read quit packet");
	} else {
				if (sbuf->scanf("%s", reason) <= 0) {
					strlcpy(reason, "unknown reason", MAX_CHARS);
		}
		seterrno(0);
		error("Got quit packet: \"%s\"", reason);
	}
	return -1;
}


int Receive_audio(void)
{
	int 				n;
	unsigned char		pkt, type, vol;

	if ((n = rbuf.scanf("%c%c%c", &pkt, &type, &vol)) <= 0) {
		return n;
	}
#ifdef SOUND
	if ((n = audio.Handle(type, vol)) == -1) {
		return -1;
	}
#endif /* SOUND */
	return 1;
}


int Receive_talk_ack(void)
{
	int 				n;
	unsigned char		pkt;
	long				talk_ack;

	if ((n = cbuf.scanf("%c%ld", &pkt, &talk_ack)) <= 0) {
		return n;
	}
	if (talk_ack >= talk_pending) {
		talk_pending = 0;
	}
	return 1;
}

int Receive_cookie(void)
{
	int 			n;
	unsigned char	pkt;
	uint			cookie;

	if ((n = cbuf.scanf("%c%u", &pkt, &cookie)) <= 0) {
		return n;
	}
	String	s;
	bool b = SetCookie(myConpar.serverName, myConpar.contactPort, cookie, s);
	return 1;
}


int Net_talk(char *str)
{
	strlcpy(talk_str, str, sizeof talk_str);
	talk_pending = ++talk_sequence_num;
	talk_last_send = last_loops - TALK_RETRY;
	return 0;
}


int Send_talk(void)
{
	if (talk_pending == 0) {
		return 0;
	}
	if (last_loops - talk_last_send < TALK_RETRY) {
		return 0;
	}
	if (wbuf.printf("%c%ld%s", PKT_TALK,
					  talk_pending, talk_str) == -1) {
		return -1;
	}
	talk_last_send = last_loops;
	return 0;
}


int Send_display(void)
{
	int 				width_wanted = draw_width;
	int 				height_wanted = draw_height;

	width_wanted = (int)(width_wanted * iniClient.scaleFactor + 0.5);
	height_wanted = (int)(height_wanted * iniClient.scaleFactor + 0.5);

	LIMIT(width_wanted, MIN_VIEW_SIZE, MAX_VIEW_SIZE);
	LIMIT(height_wanted, MIN_VIEW_SIZE, MAX_VIEW_SIZE);

	if (width_wanted == ext_view_width &&
		height_wanted == ext_view_height &&
		debris_colors == num_spark_colors &&
		iniClient.sparkRand == old_spark_rand &&
		last_loops != 0) {
		return 0;
	}

	if (simulating) {
		ext_view_width = width_wanted;
		ext_view_height = height_wanted;
		Check_view_dimensions();
	}
	else if (wbuf.printf("%c%hd%hd%c%c", PKT_DISPLAY,
						   width_wanted, height_wanted,
						   num_spark_colors, iniClient.sparkRand) == -1) {
		return -1;
	}

	old_spark_rand = iniClient.sparkRand;

	return 0;
}


int Send_modifier_bank(int bank)
{
	if (bank < 0 || bank >= NUM_MODBANKS)
		return -1;
	if (wbuf.printf("%c%c%s", PKT_MODIFIERBANK,
					  bank, iniClient.modBankStr[bank]) == -1)
		return -1;
	return 0;
}

int Send_pointer_move(int movement)
{
	if (version > 0x3201) {
		if (wbuf.printf("%c%hd", PKT_POINTER_MOVE,
						  movement) == -1) {
			return -1;
		}
	}
	return 0;
}

int Send_audio_request(int onoff)
{
#ifdef DEBUG_SOUND
	printf("Send_audio_request %d\n", onoff);
#endif

	if (version < 0x3250) {
		return 0;
	}
#ifndef SOUND
	onoff = 0;
#endif
	if (wbuf.printf("%c%c", PKT_REQUEST_AUDIO, (onoff != 0)) == -1) {
		return -1;
	}
	return 0;
}

int Send_fps_request(int fps)
{
	if (version < 0x3280) {
		return 0;
	}
	if (wbuf.printf("%c%c", PKT_ASYNC_FPS, fps) == -1) {
		return -1;
	}
	return 0;
}

