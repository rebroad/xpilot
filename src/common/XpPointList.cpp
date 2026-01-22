/* $Id: XpPointList.cpp,v 1.5 2004/05/30 08:10:39 dick Exp $
 *
 * XpPointList - An ObjList of XpPoint(s)
 *
 * common - The common library for XPilot
 *
 *      Copyright (C) 2002 by
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
/*
 * $Log: XpPointList.cpp,v $
 * Revision 1.5  2004/05/30 08:10:39  dick
 * Better copy constructor which doesn't leak
 *
 * Revision 1.4  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.3  2002/09/11 02:34:46  jlmiller
 * Always copy the Max when copying a list, even if there are no points in the list.
 *
 * Revision 1.2  2002/09/10 19:03:06  jlmiller
 * Add copy constructor.
 *
 * Revision 1.1  2002/06/25 20:21:06  jlmiller
 * Add to common.
 *
 * Revision 1.8  2002/06/02 22:44:05  jlmiller
 * Add undoMan to shipeditor.
 * Commentability.
 *
 * Revision 1.7  2002/06/01 17:14:26  jlmiller
 * Fix a few nasty little bugs that would crash only when the Debugger WASNT running.
 * Commentability
 *
 * Revision 1.6  2002/05/30 18:33:58  jlmiller
 * Splitting of consecutive vertices now available.
 *
 * Revision 1.5  2002/05/30 17:13:38  jlmiller
 * Be sure to create  new XpPoints within scope, to prevent heap errors.
 *
 * Revision 1.4  2002/05/28 07:19:51  dick
 * Rename xpilotedit to XPilotControl
 *
 */
#include "StdAfx.h"

#include "XpPointList.h"

///////////////////////////////////////////////////////////////////////////////
XpPointList::XpPointList()
{
}

///////////////////////////////////////////////////////////////////////////////
XpPointList& XpPointList::operator=(const XpPointList& xpl)
{
	XpPoint*	tmpPt = (XpPoint*)xpl.GetHead();
	XpPoint*	newPt;

	while(tmpPt)
	{
		newPt = new XpPoint();
		*newPt = *tmpPt;
		Add(newPt);
		tmpPt = (XpPoint*)tmpPt->GetNext();
	}
	//Always copy the maximums.
	maxItems = xpl.maxItems;
	return(*this);
}

///////////////////////////////////////////////////////////////////////////////
XpPoint* XpPointList::CheckAdd(XpPoint* pt)
{
	//Add a point as long as we have space for one.
	//Return a pointer to the added point.

	if (GetCount() < maxItems)
	{
		Add(pt);
		return pt;
	}
	else
		return NULL;
}

///////////////////////////////////////////////////////////////////////////////
XpPoint* XpPointList::CheckInsert(XpPoint* before, XpPoint* pt)
{
	//Insert a point, as long as we have space for one.
	//Return a pointer to the added point.
	if (GetCount() < maxItems)
	{
		InsertBefore(before, pt);
		return pt;
	}
	else
		return NULL;
}


///////////////////////////////////////////////////////////////////////////////
int XpPointList::CheckConsecutive(XpPoint* fir, XpPoint* sec)
{
//Return: 0 if not consecutive in either order
//        1 if consec
//		  2 if reversed
	if (fir->next == sec)
		return 1;
	else if (sec->next == fir)
		return 2;
	else
		return 0;
}

///////////////////////////////////////////////////////////////////////////////
void XpPointList::SetMax(int max)
{
	maxItems = max;
}

///////////////////////////////////////////////////////////////////////////////
int XpPointList::GetMax()
{
	return maxItems;
}

///////////////////////////////////////////////////////////////////////////////
XpPoint* XpPointList::IsPointAt(int xp, int yp)
{
	//Is there a point in this list at the specified coord's?
	//Only return the first one found there.
	//Otherwise return NULL
	int i, numpts = GetCount();
	XpPoint *tmppt = NULL;
	tmppt = (XpPoint *) GetHead();

	if (tmppt == NULL) //if the list is empty, dont go any further
		return NULL;

	//else loop throught the points, if we find one that matches, return it.
	for (i = 0; i < numpts; i++)
	{
		if ((tmppt->x == xp) && (tmppt->y == yp))
			return tmppt;
		else
			tmppt = (XpPoint *) tmppt->GetNext();
	}

	//if we made it this far, we didnt find a vertex.
	return NULL;
}

