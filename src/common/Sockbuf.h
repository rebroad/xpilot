/* $Id: Sockbuf.h,v 1.6 2001/08/31 20:53:54 dick Exp $
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

#ifndef	_SOCKBUF_H_
#define	_SOCKBUF_H_

#include "Sock.h"

#define MIN_SOCKBUF_SIZE	1024
#define MAX_SOCKBUF_SIZE	(50*1024)

#define SERVER_RECV_SIZE	MIN_SOCKBUF_SIZE
#define SERVER_SEND_SIZE	(4*1024)

#define CLIENT_SEND_SIZE	SERVER_RECV_SIZE
#define CLIENT_RECV_SIZE	SERVER_SEND_SIZE

/*
 * Definitions for the states a socket buffer can be in.
 */
#define SOCKBUF_READ		0x01	/* if readable */
#define SOCKBUF_WRITE		0x02	/* if writeable */
#define SOCKBUF_LOCK		0x04	/* if locked against kernel i/o */
#define SOCKBUF_ERROR		0x08	/* if i/o error occurred */
#define SOCKBUF_DGRAM		0x10	/* if datagram socket */

/*
 * Hack: leave some spare room for the last terminating packet
 * of a frame update.
 */
#define SOCKBUF_WRITE_SPARE	8

/*
 * Maximum number of socket i/o retries if datagram socket.
 */
#define MAX_SOCKBUF_RETRIES	2

/*
 * A buffer to reduce the number of system calls made and to reduce
 * the number of network packets.
 */
class Sockbuf
{
public:
	Sockbuf();
	~Sockbuf();

	int Init(Sock* _sock, int _size, int _state);
	int Cleanup();
	int Clear();
	int Advance(int len);
	int Flush();
	int Write(char* buf, int len);
	int Read();
	int Copy(Sockbuf* src, int len);
    int printf(const char *fmt, ...);
    int scanf(const char *fmt, ...);

    Sock	sock;		/* socket descriptor */
    char	*buf;		/* i/o data buffer */
    int		size;		/* size of buffer */
    int		len;		/* amount of data in buffer (writing/reading) */
    char	*ptr;		/* current position in buffer (reading) */
    int		state;		/* read/write/locked/error status flags */

public:
	void	SetErrMsgHandler(ErrMsgHandler _emh, void* _emhThis);
private:
	ErrMsgHandler	emh;		/* Error message output handler */
	void*			emhThis;	/* The "this" of the outputter.  Usually a window */

};

extern int last_packet_of_frame;

#endif		// _SOCKBUF_H_
