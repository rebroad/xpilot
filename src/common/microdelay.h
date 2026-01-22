/* $Id: microdelay.h,v 1.1 2001/08/07 22:12:32 dick Exp $
 *
 * usleep.c - OS dependant implementation of usleep().
 *
 * Copyright (c) 1991 by Patrick J. Naughton.
 *
 * Revision History:
 * 30-Aug-90: written.
 *
 * 07-Dec-94: Bert Gijsbers
 *	Changed "void usleep(unsigned long)" to "int usleep(unsigned)"
 *	as this is what it seems to be on systems which do have usleep(3) (AIX).
 *	Changed usleep into micro_delay to forego any possible prototype clashes.
 */
/*
 * $Log: microdelay.h,v $
 * Revision 1.1  2001/08/07 22:12:32  dick
 * usleep becomes microdelay
 *
 * Revision 1.1  2001/08/07 20:51:55  dick
 * move client usleep/micro_delay to common usleep/usleep.
 *
 */

#ifndef	_MICRODELAY_H_
#define	_MICRODELAY_H_

extern int microdelay(unsigned usec);

#endif		// _MICRODELAY_H_
