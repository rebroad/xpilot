/*
 * $Id: LocalServerPanelWorld.h,v 1.3 2002/09/09 23:51:40 dick Exp $
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
 * $Log: LocalServerPanelWorld.h,v $
 * Revision 1.3  2002/09/09 23:51:40  dick
 * Define the height of one LSPW as 20 pixels
 *
 * Revision 1.2  2002/09/07 18:30:26  dick
 * Don't include Fl_Output.H, rather just have a forward class reference.
 *
 * Revision 1.1  2002/09/07 17:52:00  dick
 * Skeleton support for the Simple/Advanced Local server.
 *
 */

#ifndef	_LOCALSERVERPANELWORLD_H_
#define	_LOCALSERVERPANELWORLD_H_

#include "FL/Fl_Group.H"

class	Fl_Output;

#define	LSPW_HEIGHT	20

#define	LocalServerPanelWorldSUPERCLASS	Fl_Group
class LocalServerPanelWorld : public LocalServerPanelWorldSUPERCLASS {
public:
	LocalServerPanelWorld(int x, int y, int w,int h,const char *l=0);

	Fl_Output*	mapName;

//	virtual void draw();

private:

	int		myX;
	int		myY;
	int		myW;
	int		myH;
};

#endif	// _LOCALSERVERPANELWORLD_H_
