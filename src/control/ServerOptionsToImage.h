/* $Id: ServerOptionsToImage.h,v 1.1 2004/01/19 06:49:06 dick Exp $
 *
 * Given a ServerOptions, return an Fl_Image
 *
 *      Copyright (C) 2004 by
 *
 *      The XPilot Authors   <xpilot@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
 *      Jarrod Miller        <jarrod@xpilot.org>
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
 * $Log: ServerOptionsToImage.h,v $
 * Revision 1.1  2004/01/19 06:49:06  dick
 * Convert a ServerOptions to an Fl_Image.
 *
 */

#ifndef	_SERVEROPTIONSTOIMAGE_H_
#define	_SERVEROPTIONSTOIMAGE_H_

extern Fl_Image* ServerOptionsToImage(ServerOptions* sos);

#endif	// _SERVEROPTIONSTOIMAGE_H_
