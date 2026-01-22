/* $Id: Fl_KbKbSelector.cpp,v 1.4 2004/02/03 03:39:53 dick Exp $
 *
 * Fl_KbKbSelector - Select from a list of available keyboards
 *
 *      Copyright (C) 2001 by
 *
 *      The XPilot Authors   <xpilot@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
 *      Jarrod Miller        <jarrod@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
 *      Bjørn Stabell        <bjoern@xpilot.org>
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
 * $Log: Fl_KbKbSelector.cpp,v $
 * Revision 1.4  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.3  2001/10/09 15:48:45  dick
 * Wrap debug printf statements with DEBUGEVENTS()
 *
 * Revision 1.2  2001/09/27 17:35:05  dick
 * Debug printf code (still DDS).
 *
 * Revision 1.1  2001/09/25 15:47:54  dick
 * A widget to select the current keyboard layout.
 *
 */
#include "StdAfx.h"

#include <string.h>
#include <stdio.h>

#include "FL/Fl.H"
//#include "FL/fl_draw.H"
//#include "FL/Fl_Window.H"

#include "Fl_KbConfig.h"
#include "Fl_KbKbSelector.h"


///////////////////////////////////////////////////////////////////////////////
Fl_KbKbSelector::Fl_KbKbSelector(int X, int Y, int W, int H, const char* L)
: Fl_KbKbSelectorSUPERCLASS(X, Y, W, H, L)
{
}

int	Fl_KbKbSelector::handle(int event)
{
	int ret;
	ret = Fl_KbKbSelectorSUPERCLASS::handle(event);
	if (ret)
		return(ret);
	DEBUGEVENTS(printf("Fl_KbKbSelector::handle(%s)\n", eventName[event]);)
	return(ret);
}
