/* $Id: XpPointList.h,v 1.3 2004/05/30 08:10:39 dick Exp $
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
 * $Log: XpPointList.h,v $
 * Revision 1.3  2004/05/30 08:10:39  dick
 * Better copy constructor which doesn't leak
 *
 */
#ifndef	_XPPOINTLIST_H_
#define	_XPPOINTLIST_H_

#include "XpPoint.h"

class XpPointList : public ObjList {
private:
	int maxItems; //The Max number of Xp_Points in this list

public:
	XpPointList();
	XpPointList& operator=(const XpPointList&);

	XpPoint*	CheckAdd(XpPoint* pt); //Add item to list if we have < MaxItems
	XpPoint*	CheckInsert(XpPoint* before, XpPoint *nw); //Add item to list if we can
	int			CheckConsecutive(XpPoint* fir, XpPoint* sec); //Are these points consectutive
	void		SetMax(int max); //Set the Max number of items this list can have
	int			GetMax(); //Return the value of MaxItems
	XpPoint*	IsPointAt(int x, int y); //Is there a point at x,y?
	XpPoint*	pCurrentPoint; //Pointer to current point
};

#endif	// _XPPOINTLIST_H_
