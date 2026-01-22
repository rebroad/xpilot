/* $Id: Sockbuf.cpp,v 1.23 2005/03/17 22:12:13 kps Exp $
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2001 by
 *
 *      Bj�rn Stabell        <bjoern@xpilot.org>
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
#include "StdAfx.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#if defined(_UNIX) || defined(_CYGWIN)
# include <unistd.h>
# ifndef __hpux
#  include <sys/time.h>
# endif
# include <sys/socket.h>
# include <netinet/in.h>
# include <netdb.h>
#endif

#ifdef __sgi
# include <bstring.h>
#endif

#if defined(_WINDOWS) && !defined(_CYGWIN)
# include "NT/winNet.h"
#	ifndef	errno
#	define	errno	h_errno
#	endif

#include <stdarg.h>
#endif

#include "version.h"
#include "config.h"
#include "const.h"
#include "error.h"
#include "Sockbuf.h"
#include "packet.h"
#include "bit.h"
#include "Sock.h"
#include "sockerrs.h"
#include "portability.h"

#if defined(_WINDOWS) && !defined(_CYGWIN)
#undef	errno
#define	errno	WSAGetLastError()
#endif

char net_version[] = VERSION;

int last_packet_of_frame;

///////////////////////////////////////////////////////////////////////////////
Sockbuf::Sockbuf()
{
	buf = NULL;
	ptr = NULL;
	size = 0;
	emh = 0;
}

///////////////////////////////////////////////////////////////////////////////
Sockbuf::~Sockbuf()
{
	sock.Close();
	Cleanup();
}

///////////////////////////////////////////////////////////////////////////////
int Sockbuf::Init(Sock* _sock, int _size, int _state)
{
	if ((buf = ptr = (char *) malloc(_size)) == NULL)
		return -1;

	if (_sock != (Sock *) NULL)
		sock = *_sock;
	else
		sock.Init();

	state = _state;
	len = 0;
	size = _size;
	ptr = buf;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
int Sockbuf::Cleanup()
{
	if (buf != NULL) {
		free(buf);
	}
	buf = ptr = NULL;
	size = len = 0;
	state = 0;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
void Sockbuf::SetErrMsgHandler(ErrMsgHandler _emh, void* _emhThis)
{
	emh = _emh;
	emhThis = _emhThis;
}


///////////////////////////////////////////////////////////////////////////////
int Sockbuf::Clear()
{
	len = 0;
	ptr = buf;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
int Sockbuf::Advance(int _len)
{
	/*
	 * First do a few buffer consistency checks.
	 */
	if (ptr > buf + len) {
		seterrno(0);
		emh(emhThis, EmError, "Sockbuf pointer too far");
		ptr = buf + len;
	}
	if (ptr < buf) {
		seterrno(0);
		emh(emhThis, EmError, "Sockbuf pointer bad");
		ptr = buf;
	}
	if (len > size) {
		seterrno(0);
		emh(emhThis, EmError, "Sockbuf len too far");
		len = size;
	}
	if (len < 0) {
		seterrno(0);
		emh(emhThis, EmError, "Sockbuf len bad");
		len = 0;
	}
	if (_len <= 0) {
		if (_len < 0) {
				seterrno(0);
			emh(emhThis, EmError, "Sockbuf advance negative (%d)", _len);
		}
	}
	else if (_len >= len) {
		if (_len > len) {
				seterrno(0);
			emh(emhThis, EmError, "Sockbuf advancing too far");
		}
		len = 0;
		ptr = buf;
	} else {
#if defined(__hpux) || defined(SVR4) || defined(_SEQUENT_) || defined(SYSV) || defined(_WINDOWS)
		memmove(buf, buf + _len, len - _len);
#else
		bcopy(buf + _len, buf, len - _len);
#endif
		len -= _len;
		if (ptr - buf <= _len) {
			ptr = buf;
		} else {
			ptr -= _len;
		}
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
int Sockbuf::Flush()
{
	int 		_len;
	int 			i;
	if (BIT(state, SOCKBUF_WRITE) == 0)
	{
		seterrno(0);
		emh(emhThis, EmError, "No flush on non-writable socket buffer");
		emh(emhThis, EmError, "(state=%02x,buf=%08x,ptr=%08x,size=%d,len=%d,sock=%d)",
				state, buf, ptr, size, len, sock.fd);
		return -1;
	}
	//D(xpprintf("Sockbuf_flush: state=%02x,buf=%08x,ptr=%08x,size=%d,len=%d,sock=%d\n",
	//						state, buf, ptr, size, len, 	sock); )
	if (BIT(state, SOCKBUF_LOCK) != 0)
	{
		seterrno(0);
		emh(emhThis, EmError, "No flush on locked socket buffer (0x%02x)", state);
		return -1;
	}
	if (len <= 0)
	{
		if (len < 0)
		{
			seterrno(0);
			emh(emhThis, EmError, "Write socket buffer length negative");
			len = 0;
			ptr = buf;
		}
		return 0;
	}

#if 0
	/* maintain a few statistics */
	{
		static int	max = 1024, avg, count;

		avg += len;
		count++;
		if (len > max) {
			max = len;
			xpprintf("Max packet = %d, avg = %d\n", max, avg / count);
		}
		else if (max > 1024 && (count & 0x03) == 0) {
			max--;
		}
	}
#endif

	if (BIT(state, SOCKBUF_DGRAM) != 0) {
		seterrno(0);
		i = 0;
#if 0
		if (randomMT() % 12 == 0)		/* artificial packet loss */
			_len = len;
		else
#endif
		while ((_len = sock.Write(buf, len)) <= 0)
		{
			if (_len == 0
				|| errno == EWOULDBLOCK
				|| errno == EAGAIN) {
				Clear();
				return 0;
			}
			if (errno == EINTR) {
				seterrno(0);
				continue;
			}
#if 0
			if (errno == ECONNREFUSED) {
				emh(emhThis, EmError, "Send refused");
				Clear();
				return -1;
			}
#endif
			if (++i > MAX_SOCKBUF_RETRIES) {
				emh(emhThis, EmError, "Can't send on socket (%d,%d)", sock.fd, len);
				Clear();
				return -1;
			}
			{
				static int send_err;
				if ((send_err++ & 0x3F) == 0) {
					emh(emhThis, EmError, "send (%d)", i);
				}
			}
			if (sock.GetError() == -1) {
				emh(emhThis, EmError, "sock_get_error send");
				return -1;
			}
				seterrno(0);
		}
		if (_len != len) {
				seterrno(0);
			emh(emhThis, EmError, "Can't write complete datagram (%d,%d)", _len, len);
		}
		Clear();
	} else
	{
		seterrno(0);
		while ((_len = sock.Write(buf, len)) <= 0) {
			if (errno == EINTR) {
				seterrno(0);
				continue;
			}
			if (errno != EWOULDBLOCK
				&& errno != EAGAIN) {
				emh(emhThis, EmError, "Can't write on socket");
				return -1;
			}
			return 0;
		}
		Advance(_len);
	}
	return _len;
}

///////////////////////////////////////////////////////////////////////////////
int Sockbuf::Write(char *_buf, int _len)
{
	if (BIT(state, SOCKBUF_WRITE) == 0) {
		seterrno(0);
		emh(emhThis, EmError, "No write to non-writable socket buffer");
		return -1;
	}
	if (size - len < _len) {
		if (BIT(state, SOCKBUF_LOCK | SOCKBUF_DGRAM) != 0) {
				seterrno(0);
			emh(emhThis, EmError, "No write to locked socket buffer (%d,%d,%d,%d)",
				state, size, len, _len);
			return -1;
		}
		if (Flush() == -1) {
			return -1;
		}
		if (size - len < _len) {
			return 0;
		}
	}
	memcpy(buf + len, _buf, _len);
	len += _len;

	return _len;
}

///////////////////////////////////////////////////////////////////////////////
int Sockbuf::Read()
{
	int 	max,
			i,
			_len;

	if (BIT(state, SOCKBUF_READ) == 0) {
		seterrno(0);
		emh(emhThis, EmError, "No read from non-readable socket buffer (%d)", state);
		return -1;
	}
	if (BIT(state, SOCKBUF_LOCK) != 0) {
		return 0;
	}
	if (ptr > buf) {
		Advance(ptr - buf);
	}
	if ((max = size - len) <= 0) {
		static int before;
		if (before++ == 0) {
				seterrno(0);
			emh(emhThis, EmError, "Read socket buffer not big enough (%d,%d)",
				  size, len);
		}
		return -1;
	}
	if (BIT(state, SOCKBUF_DGRAM) != 0) {
		seterrno(0);
		i = 0;
#if 0
		if (randomMT() % 12 == 0)				/* artificial packet loss */
			_len = len;
		else
#endif
		while ((_len = sock.Read(buf + len, max)) <= 0) {
			if (_len == 0) {
				return 0;
			}
#ifdef _SOCKWINDOWS
				seterrno(WSAGetLastError());
#endif
			if (errno == EINTR) {
				seterrno(0);
				continue;
			}
			if (errno == EWOULDBLOCK
				|| errno == EAGAIN) {
				return 0;
			}
#if 0
			if (errno == ECONNREFUSED) {
				emh(emhThis, EmError, "Receive refused");
				return -1;
			}
#endif
				if (errno == ECONNRESET)
				{
						// emh(emhThis, EmError, "Connection reset");
						xpprintf("Connection reset\n");
						return(-1);
				}
				xpprintf("Sockbuf: read error %s\n", (PCSTR)sock.GetErrorString());
//				xpprintf("errno=%d (%s) len = %d during sock_read\n",
//						errno, GetSockErrText(errno), _len);

			if (++i > MAX_SOCKBUF_RETRIES) {
				emh(emhThis, EmError, "Can't recv on socket");
				return -1;
			}
			{ static int recv_err = 0;
				if ((recv_err++ & 0x3F) == 0) {
					//emh(emhThis, EmError, "recv (%d)", i);
						xpprintf("recv (%d)\n", i);
				}
			}
			if (sock.GetError() == -1) {
				emh(emhThis, EmError, "GetSocketError recv");
				return -1;
			}
				seterrno(0);
		}
		len += _len;
	} else {
				seterrno(0);
		while ((_len = sock.Read(buf + len, max)) <= 0) {
			if (_len == 0) {
				return 0;
			}
			if (errno == EINTR) {
				seterrno(0);
				continue;
			}
			if (errno != EWOULDBLOCK
				&& errno != EAGAIN) {
				emh(emhThis, EmError, "Can't read on socket");
				return -1;
			}
			return 0;
		}
/*		IFWINDOWS( Trace("Read stream %d bytes from %d\n", _len, sock); )*/
		len += _len;
	}

	return len;
}

///////////////////////////////////////////////////////////////////////////////
int Sockbuf::Copy(Sockbuf* src, int _len)
{
	if (_len < size - len) {
		seterrno(0);
		emh(emhThis, EmError, "Not enough room in destination copy socket buffer");
		return -1;
	}
	if (_len < src->len) {
		seterrno(0);
		emh(emhThis, EmError, "Not enough data in source copy socket buffer");
		return -1;
	}
	memcpy(buf + len, src->buf, len);
	len += _len;

	return _len;
}


///////////////////////////////////////////////////////////////////////////////
int Sockbuf::printf(const char *fmt, ...)
{
#define PRINTF_FMT		1
#define PRINTF_IO		2
#define PRINTF_SIZE 	3

	int 				i,
						cval,
						ival,
						count,
						failure = 0,
						max_str_size;
	unsigned int		uval;
	short				sval;
	unsigned short		usval;
	long				lval;
	unsigned long		ulval;
	char				*str,
						*end,
						*_buf,
						*stop;
	va_list 			ap;
	va_start(ap, fmt);

	if (buf == NULL)	// trying to send down a closed pipe?
	{
		va_end(ap);
		return(0);
	}

	/*
	 * Stream socket buffers should flush the buffer if running
	 * out of write space.	This is currently not needed cause
	 * only datagram sockets are used or the buffer is locked.
	 */

	/*
	 * Mark the end of the available buffer space,
	 * but keep a little room for a terminating packet.
	 * This terminating packet will be from Send_end_of_frame()
	 * in netserver.c.	This is a hack, sorry.
	 * But we want to send frames even if they're bigger than
	 * our available buffer space.
	 */
	end = buf + size;
	if (last_packet_of_frame != 1) {
		end -= SOCKBUF_WRITE_SPARE;
	}
	_buf = buf + len;
	for (i = 0; failure == 0 && fmt[i] != '\0'; i++) {
		if (fmt[i] == '%') {
			switch (fmt[++i]) {
			case 'c':
				if (_buf + 1 >= end) {
					failure = PRINTF_SIZE;
					break;
				}
				cval = va_arg(ap, int);
				*_buf++ = cval;
				break;
			case 'd':
				if (_buf + 4 >= end) {
					failure = PRINTF_SIZE;
					break;
				}
				ival = va_arg(ap, int);
				*_buf++ = ival >> 24;
				*_buf++ = ival >> 16;
				*_buf++ = ival >> 8;
				*_buf++ = ival;
				break;
			case 'u':
				if (_buf + 4 >= end) {
					failure = PRINTF_SIZE;
					break;
				}
				uval = va_arg(ap, unsigned int);
				*_buf++ = uval >> 24;
				*_buf++ = uval >> 16;
				*_buf++ = uval >> 8;
				*_buf++ = uval;
				break;
			case 'h':
				if (_buf + 2 >= end) {
					failure = PRINTF_SIZE;
					break;
				}
				switch (fmt[++i]) {
				case 'd':
					sval = va_arg(ap, int);
					*_buf++ = sval >> 8;
					*_buf++ = (char)sval;
					break;
				case 'u':
					usval = va_arg(ap, unsigned);
					*_buf++ = usval >> 8;
					*_buf++ = (char)usval;
					break;
				default:
					failure = PRINTF_FMT;
					break;
				}
				break;
			case 'l':
				if (_buf + 4 >= end) {
					failure = PRINTF_SIZE;
					break;
				}
				switch (fmt[++i]) {
				case 'd':
					lval = va_arg(ap, long);
					*_buf++ = (char)(lval >> 24);
					*_buf++ = (char)(lval >> 16);
					*_buf++ = (char)(lval >> 8);
					*_buf++ = (char)lval;
					break;
				case 'u':
					ulval = va_arg(ap, unsigned long);
					*_buf++ = (char)(ulval >> 24);
					*_buf++ = (char)(ulval >> 16);
					*_buf++ = (char)(ulval >> 8);
					*_buf++ = (char)ulval;
					break;
				default:
					failure = PRINTF_FMT;
					break;
				}
				break;
			case 'S':	/* Big strings */
			case 's':	/* Small strings */
				max_str_size = (fmt[i] == 'S') ? MSG_LEN : MAX_CHARS;
				str = va_arg(ap, char *);
				if (_buf + max_str_size >= end) {
					stop = end;
				} else {
					stop = _buf + max_str_size;
				}
				/* Send the nul byte too */
				do {
					if (_buf >= stop) {
						break;
					}
				} while ((*_buf++ = *str++) != '\0');
				if (_buf > stop) {
					failure = PRINTF_SIZE;
				}
				break;
			default:
				failure = PRINTF_FMT;
				break;
			}
		} else {
			failure = PRINTF_FMT;
		}
	}
	if (failure != 0) {
		count = -1;
		if (failure == PRINTF_SIZE) {
#if 0
			static int before;
			if ((before++ & 0x0F) == 0) {
				printf("Write socket buffer not big enough (%d,%d,\"%s\")\n",
					size, len, fmt);
			}
#endif
			if (BIT(state, SOCKBUF_DGRAM) != 0) {
				count = 0;
				failure = 0;
			}
		}
		else if (failure == PRINTF_FMT) {
				seterrno(0);
			emh(emhThis, EmError, "Error in format string (\"%s\")", fmt);
		}
	} else {
		count = _buf - (buf + len);
		len += count;
	}

	va_end(ap);

	return count;
}

///////////////////////////////////////////////////////////////////////////////
int Sockbuf::scanf(const char *fmt, ...)
{
	int 				i,
						j,
						k,
						*iptr,
						count = 0,
						failure = 0,
						max_str_size;
	unsigned			*uptr;
	short				*sptr;
	unsigned short		*usptr;
	long				*lptr;
	unsigned long		*ulptr;
	char				*cptr,
						*str;
	va_list 			ap;
	va_start(ap, fmt);

	if (buf == NULL)	// trying to slurp off a closed pipe?
	{
		va_end(ap);
		return(-1);
	}

	for (i = j = 0; failure == 0 && fmt[i] != '\0'; i++) {
		if (fmt[i] == '%') {
			count++;
			switch (fmt[++i]) {
			case 'c':
				if (&buf[len] < &ptr[j + 1]) {
					if (BIT(state, SOCKBUF_DGRAM | SOCKBUF_LOCK) != 0) {
						failure = 3;
						break;
					}
					if (Read() == -1) {
						failure = 2;
						break;
					}
					if (&buf[len] < &ptr[j + 1]) {
						failure = 3;
						break;
					}
				}
				cptr = va_arg(ap, char *);
				*cptr = ptr[j++];
				break;
			case 'd':
				if (&buf[len] < &ptr[j + 4]) {
					if (BIT(state, SOCKBUF_DGRAM | SOCKBUF_LOCK) != 0) {
						failure = 3;
						break;
					}
					if (Read() == -1) {
						failure = 2;
						break;
					}
					if (&buf[len] < &ptr[j + 4]) {
						failure = 3;
						break;
					}
				}
				iptr = va_arg(ap, int *);
				*iptr = ptr[j++] << 24;
				*iptr |= (ptr[j++] & 0xFF) << 16;
				*iptr |= (ptr[j++] & 0xFF) << 8;
				*iptr |= (ptr[j++] & 0xFF);
				break;
			case 'u':
				if (&buf[len] < &ptr[j + 4]) {
					if (BIT(state, SOCKBUF_DGRAM | SOCKBUF_LOCK) != 0) {
						failure = 3;
						break;
					}
					if (Read() == -1) {
						failure = 2;
						break;
					}
					if (&buf[len] < &ptr[j + 4]) {
						failure = 3;
						break;
					}
				}
				uptr = va_arg(ap, unsigned *);
				*uptr = (ptr[j++] & 0xFF) << 24;
				*uptr |= (ptr[j++] & 0xFF) << 16;
				*uptr |= (ptr[j++] & 0xFF) << 8;
				*uptr |= (ptr[j++] & 0xFF);
				break;
			case 'h':
				if (&buf[len] < &ptr[j + 2]) {
					if (BIT(state, SOCKBUF_DGRAM | SOCKBUF_LOCK) != 0) {
						failure = 3;
						break;
					}
					if (Read() == -1) {
						failure = 2;
						break;
					}
					if (&buf[len] < &ptr[j + 2]) {
						failure = 3;
						break;
					}
				}
				switch (fmt[++i]) {
				case 'd':
					sptr = va_arg(ap, short *);
					*sptr = ptr[j++] << 8;
					*sptr |= (ptr[j++] & 0xFF);
					break;
				case 'u':
					usptr = va_arg(ap, unsigned short *);
					*usptr = (ptr[j++] & 0xFF) << 8;
					*usptr |= (ptr[j++] & 0xFF);
					break;
				default:
					failure = 1;
					break;
				}
				break;
			case 'l':
				if (&buf[len] < &ptr[j + 4]) {
					if (BIT(state, SOCKBUF_DGRAM | SOCKBUF_LOCK) != 0) {
						failure = 3;
						break;
					}
					if (Read() == -1) {
						failure = 2;
						break;
					}
					if (&buf[len] < &ptr[j + 4]) {
						failure = 3;
						break;
					}
				}
				switch (fmt[++i]) {
				case 'd':
					lptr = va_arg(ap, long *);
					*lptr = ptr[j++] << 24;
					*lptr |= (ptr[j++] & 0xFF) << 16;
					*lptr |= (ptr[j++] & 0xFF) << 8;
					*lptr |= (ptr[j++] & 0xFF);
					break;
				case 'u':
					ulptr = va_arg(ap, unsigned long *);
					*ulptr = (ptr[j++] & 0xFF) << 24;
					*ulptr |= (ptr[j++] & 0xFF) << 16;
					*ulptr |= (ptr[j++] & 0xFF) << 8;
					*ulptr |= (ptr[j++] & 0xFF);
					break;
				default:
					failure = 1;
					break;
				}
				break;
			case 'S':	/* Big strings */
			case 's':	/* Small strings */
				max_str_size = (fmt[i] == 'S') ? MSG_LEN : MAX_CHARS;
				str = va_arg(ap, char *);
				k = 0;
				for (;;) {
					if (&buf[len] < &ptr[j + 1]) {
						if (BIT(state, SOCKBUF_DGRAM | SOCKBUF_LOCK) != 0) {
							failure = 3;
							break;
						}
						if (Read() == -1) {
							failure = 2;
							break;
						}
						if (&buf[len] < &ptr[j + 1]) {
							failure = 3;
							break;
						}
					}
					if ((str[k++] = ptr[j++]) == '\0') {
						break;
					}
					else if (k >= max_str_size) {
						/*
						 * What to do now is unclear to me.
						 * The server should drop the packet, but
						 * the client has more difficulty with that
						 * if this is the reliable data buffer.
						 */
#ifndef SILENT
						seterrno(0);
						emh(emhThis, EmError, "String overflow while scanning (%d,%d)",
							  k, max_str_size);
#endif
						if (BIT(state, SOCKBUF_LOCK) != 0) {
							failure = 2;
						} else {
							failure = 3;
						}
						break;
					}
				}
				if (failure != 0) {
					strcpy(str, "ErRoR");
				}
				break;
			default:
				failure = 1;
				break;
			}
		} else {
			failure = 1;
		}
	}
	if (failure == 1) {
		seterrno(0);
		emh(emhThis, EmError, "Error in format string (%s)", fmt);
	}
	else if (failure == 3) {
		/* Not enough input for one complete packet */
		count = 0;
		failure = 0;
	}
	else if (failure == 0) {
		if (&buf[len] < &ptr[j]) {
				seterrno(0);
			emh(emhThis, EmError, "Input buffer exceeded (%s)", fmt);
			failure = 1;
		} else {
			ptr += j;
		}
	}

	va_end(ap);

	return (failure) ? -1 : count;
}
