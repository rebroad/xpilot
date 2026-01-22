/* $Id: flUtil.h,v 1.1 2007/02/17 06:14:28 dick Exp $
 *
 * flUtil - some fltk general routines
 *
 * XPilot5 - Manage all local XPilot activity.
 *
 *      Copyright (C) 2002 by
 *
 *      The XPilot Authors           <xpilot@xpilot.org>
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
 * $Log: flUtil.h,v $
 * Revision 1.1  2007/02/17 06:14:28  dick
 * Some general FLTK utilities
 *
 */

#ifndef	_FLUTIL_H_
#define	_FLUTIL_H_


extern	String	GetButtonFile(PCSTR f);
extern	bool	TestFileOverwrite(PCSTR f);

#endif// _FLUTIL_H_
