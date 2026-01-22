/* $Id: StdAfx.h,v 1.1 2004/02/03 04:06:53 dick Exp $
 *
 * StdAfx.h - Visual C++ precompiled header support
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 2004 by
 *
 *      XPilot Authors       <xpilot@xpilot.org>
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
 * $Log: StdAfx.h,v $
 * Revision 1.1  2004/02/03 04:06:53  dick
 * Unix fixes after adding the Windows precompiled header support.
 *
 */

/*
 * In the grand scheme of precompiled things, the Windows compiler will pick up
 * StdAfx.h from the project's root source directory and ignore this file;
 * Unix needs this so that it can resolve an empty include.
 */
#ifndef	_STDAFX_H_
#define _STDAFX_H_

#ifdef	_WINDOWS
#endif

#endif // _STDAFX_H_
