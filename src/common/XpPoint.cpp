/*
* XPilotedit, the cross platform map editor for XPilot.  Copyright (C) 2001 by
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
#include "StdAfx.h"

#include "XpPoint.h"

///////////////////////////////////////////////////////////////////////////////
XpPoint::XpPoint()
{
}
///////////////////////////////////////////////////////////////////////////////
XpPoint::XpPoint(int x1, int y1)
{
	x = x1;
	y = y1;
}
///////////////////////////////////////////////////////////////////////////////
XpPoint::XpPoint(XpPoint *pt1, XpPoint *pt2)
{
	//Create a new point at the midpoint of two other points.
	x = (pt2->x + pt1->x)/2;
	y = (pt2->y + pt1->y)/2;
}
