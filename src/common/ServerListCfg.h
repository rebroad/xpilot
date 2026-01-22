/* $Id: ServerListCfg.h,v 1.2 2004/05/10 06:27:09 dick Exp $
 *
 * XPwhere - Where in the world can i find people playing XPilot.
 *      Copyright (C) 2002 by
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
 * $Log: ServerListCfg.h,v $
 * Revision 1.2  2004/05/10 06:27:09  dick
 * Remove unused #includes
 *
 * Revision 1.1  2002/09/02 15:48:35  dick
 * New ServerListCfg contains user config info for Fl_ServerList.
 *
 */

#ifndef	_SERVERLISTCFG_H_
#define	_SERVERLISTCFG_H_

//#include "Obj.h"
//#include "types.h"

const int numServerListColumns = 15;

#define	COL_SERVERNAME	 0
#define	COL_MAPNAME		 1
#define	COL_MAPAUTHOR	 2
#define	COL_MAPSIZE		 3
#define	COL_PLAYERCOUNT	 4
#define	COL_LAG			 5
#define	COL_BASES		 6
#define	COL_TEAMS		 7
#define	COL_FPS			 8
#define	COL_VERSION		 9
#define	COL_SOUND		10
#define	COL_UPTIME		11
#define	COL_IP			12
#define	COL_RACE		13
#define	COL_PORT		14


class ServerListCfg
{
public:
//	ServerListCfg();
//	~ServerListCfg();

	bool		columns[numServerListColumns];
	int			colSizes[numServerListColumns];

};

#endif	// _SERVERLISTCFG_H_
