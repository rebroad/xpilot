/* $Id: StdAfx.h,v 1.2 2004/02/03 04:06:53 dick Exp $
 *
 * StdAfx.cpp - Visual C++ precompiled header support
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2001 by
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
 * Revision 1.2  2004/02/03 04:06:53  dick
 * Unix fixes after adding the Windows precompiled header support.
 *
 * Revision 1.1  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 */

// Drasticly reduce the Windows compile time by using precompiled headers.
// On my (dual) Xeon 2Ghz, `time makedistribution -a` goes from
// 3:16 (196 seconds) to 1:32 (92 seconds).


#ifndef	_STDAFX_H_
#define _STDAFX_H_

#ifdef	_WINDOWS

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <windowsx.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
#endif // _STDAFX_H_
