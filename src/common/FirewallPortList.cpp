/* $Id: FirewallPortList.cpp,v 1.5 2004/02/03 03:39:52 dick Exp $
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
 * $Log: FirewallPortList.cpp,v $
 * Revision 1.5  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.4  2004/01/09 21:56:08  dick
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
#include "StdAfx.h"

#include <ctype.h>

#include "xpprintf.h"
#include "FirewallPortList.h"

// XPilot default game ports use this range.
// There's no reason for it (although it should be between 49152-65535
// see http://www.iana.org/assignments/port-numbers )
FirewallPortList normalFirewall(50000,50100);

///////////////////////////////////////////////////////////////////////////////
FirewallPortList::FirewallPortList()
{
	whichRange = 0;
	rangeOffset = 0;

	Range* r = new Range;
	r->min = 1024;
	r->max = 65535;
	Add(r);
}

///////////////////////////////////////////////////////////////////////////////
FirewallPortList::FirewallPortList(const FirewallPortList& fwpl)
{
	*this = fwpl;
}

///////////////////////////////////////////////////////////////////////////////
FirewallPortList::FirewallPortList(int lo, int hi)
{
	whichRange = 0;
	rangeOffset = 0;

	Range* r = new Range;
	r->min = lo;
	r->max = hi;
	Add(r);
}

///////////////////////////////////////////////////////////////////////////////
void FirewallPortList::operator=(const FirewallPortList& _fwpl)
{
	whichRange = _fwpl.whichRange;
	rangeOffset = _fwpl.rangeOffset;

	Empty();
	Obj* _o = _fwpl.GetHead();
	while (_o)
	{
		Range* _r = (Range*) _o;
		Range* r;
		r = new Range;
		r->min = _r->min;
		r->max = _r->max;
		Add(r);
		_o = _o->GetNext();
	}
}

///////////////////////////////////////////////////////////////////////////////
void FirewallPortList::Rewind()
{
	whichRange = 0;
	rangeOffset = 0;
}

///////////////////////////////////////////////////////////////////////////////
bool FirewallPortList::GetNextPort(int& port)
{
	Obj* o = GetHead();
	if (!o)
		return(false);

	int	 i = 0;
	while (i < whichRange)
	{
		o = o->GetNext();
		if (!o)
			return(false);
	}
	Range* r = (Range*)o;
	if (!rangeOffset)
		rangeOffset = r->min;
	port = rangeOffset++;
	if (rangeOffset > r->max)
	{
		rangeOffset = 0;
		whichRange++;
	}
	return(true);
}

///////////////////////////////////////////////////////////////////////////////
String	FirewallPortList::Get()
{
	String	s;
	Obj* o = GetHead();
	if (!o)
		return("0");
	while (o)
	{
		Range* r = (Range*)o;
		s += r->min;
		if (r->max)
		{
			s += '-';
			s += r->max;
		}
		o = o->GetNext();
		if (o)
			s += ',';
	}
	return(s);
}

///////////////////////////////////////////////////////////////////////////////
// 10000-10100,53
bool FirewallPortList::Set(PCSTR _s)
{
	String	s(_s);
	Empty();
	char*	t = s.s;
	Range*	r = NULL;
	bool	domin = true;
	while (*t != '\0')
	{
		if (isdigit(*t))
		{
			if (!r)
			{
				r = new Range();
				r->min = 0;
				r->max = 0;
				domin = true;
			}
			if (domin)
				r->min = r->min * 10 + (*t & 0x0F);
			else
				r->max = r->max * 10 + (*t & 0x0F);
			t++;
			continue;
		}
		if (isspace(*t))
		{
			t++;
			continue;
		}
		if (*t == ',')
		{
			if (r)
			{
				Add(r);
				r = NULL;
			}
			t++;
			continue;
		}
		if (*t == '-')
		{
			domin = false;
			t++;
			continue;
		}
		*t = '\0';
		xpprintf("%sFailed to parse FirewallPortList.\n", showtime());
		xpprintf("%sError was near: \"%s\"\n", showtime(), (PCSTR)s);
		if (r)
			delete r;
		return(false);
	}
	if (r)
		Add(r);
	return(true);
}
