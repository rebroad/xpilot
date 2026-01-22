/* $Id: FirewallPortList.h,v 1.4 2004/01/09 21:56:58 dick Exp $
 *
 * FirewallPortList - Manage a list of ports that we can penetrate
 *                    a firewall with.
 *.
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
 * $Log: FirewallPortList.h,v $
 * Revision 1.4  2004/01/09 21:56:58  dick
 * Fix operator=(const FirewallPortList&)
 *
 * Revision 1.3  2004/01/09 17:45:22  dick
 * Add Rewind() to start the port walk over.  Fill out Set() and Get().
 *
 * Revision 1.2  2002/09/09 23:32:52  dick
 * Construct a default FirewallPortList using 1024-65535 as it's range.
 *
 * Revision 1.1  2001/08/12 10:59:15  dick
 * Manage a list of ports that we believe can punch through a firewall and dole
 * them out one at a time.
 *
 */

#ifndef	_FIREWALLPORTLIST_H_
#define	_FIREWALLPORTLIST_H_

#include "Obj.h"

class FirewallPortList : public ObjList
{
public:
	FirewallPortList();
	FirewallPortList(int lo, int hi);
	FirewallPortList(const FirewallPortList& fwpl);
	void	operator=(const FirewallPortList& fwpl);


	bool	GetNextPort(int& port);
	String	Get();
	bool	Set(PCSTR s);
	void	Rewind();			// reset the port scanner to the beginning

private:
	int		whichRange;
	int		rangeOffset;
};

extern	FirewallPortList	normalFirewall;
#endif		// _FIREWALLPORTLIST_H_
