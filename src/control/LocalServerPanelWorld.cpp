/*
 * $Id: LocalServerPanelWorld.cpp,v 1.3 2004/02/03 03:39:53 dick Exp $
 *
 * XPilot.  Copyright (C) 2002 by
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
 * $Log: LocalServerPanelWorld.cpp,v $
 * Revision 1.3  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.2  2004/01/15 05:38:25  dick
 * Add a StartScoreServer button to the local panel
 *
 * Revision 1.1  2002/09/07 17:52:00  dick
 * Skeleton support for the Simple/Advanced Local server.
 *
 */
#include "StdAfx.h"

#include "FL/Fl_Group.H"
#include "FL/Fl_Box.H"
#include "FL/Fl_Output.H"

#include "cstring.h"

#include "LocalServerPanelWorld.h"

///////////////////////////////////////////////////////////////////////////////
LocalServerPanelWorld::LocalServerPanelWorld(int x, int y, int w,int h,const char *l)
        : LocalServerPanelWorldSUPERCLASS(x,y,w,h,l)
{
	myX = x;
	myY = y;
	myW = w;
	myH = h;

	begin();
	mapName = new Fl_Output(x, y, 200, h);
	end();
}
