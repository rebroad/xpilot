/* $Id: ShipList.h,v 1.4 2004/05/28 17:38:03 dick Exp $
 *
 * ShipList - Extend an ObjList to manage ShipDocument(s).
 *
 * common - The common library for XPilot
 *
 *      Copyright (C) 2001 by
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
 * $Log: ShipList.h,v $
 * Revision 1.4  2004/05/28 17:38:03  dick
 * Convert_ship_2_string outputs a String instead of a String*,
 * (prevents memory leaks and overwrites).
 *
 */
#ifndef	_SHIPLIST_H_
#define	_SHIPLIST_H_

#include "ShipDocument.h"

class ShipList : public ObjList
{
public:
	ShipDocument*	pCurrentShip;
	String			ShipFileName;
	bool			changed;

public:
	ShipList();
	int		LoadShipFile(PCSTR filename);
	int		getoneshipdef(char* sship, FILE* fin);
	void	ClearShipList();
	bool	SaveShipFile(PCSTR filename);
	void	CloneShip(ShipDocument* shp);
};


#endif	// _SHIPLIST_H_
