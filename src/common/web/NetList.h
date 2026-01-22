/* $Id: NetList.h,v 1.1 2002/06/25 04:42:49 dick Exp $
 *
 * The Object world is a list of objects (most of which are lists of objects)
 *
 *      Copyright (C) 2002 by
 *
 *      The XPilot Authors   <xpilot@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
 *      Jarrod Miller        <jarrod@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
 *      Bj�rn Stabell        <bjoern@xpilot.org>
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
 * $Log: NetList.h,v $
 * Revision 1.1  2002/06/25 04:42:49  dick
 * An ObjList of network objects.  Useful when we have a selected socket
 * and need to find out who owns it.
 *
 */


///////////////////////////////////////////////////////////////////////////////
//
class NetList : public ObjList {
  public:
	NetList() { type = NULL; }
	~NetList();
	void TimerTick();
	void Shutdown();
};

extern NetList	netList;			// the world of objects

//extern	Obj*		FindWorldObjectByName(PCSTR s);
