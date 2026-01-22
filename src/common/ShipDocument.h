/* $Id: ShipDocument.h,v 1.5 2004/07/22 16:58:45 dick Exp $
 *
 * ShipDocument - Represent a single shipshape
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
 * $Log: ShipDocument.h,v $
 * Revision 1.5  2004/07/22 16:58:45  dick
 * Remove default constructor.  Constructor(int TRUE/FALSE) tells whether
 * to init with the default ship (TRUE) or not (FALSE).
 *
 * Revision 1.4  2004/05/28 17:43:19  dick
 * Add member Draw(x,y,orient), which lives in xp-flwidgets.
 * Convert_ship_2_string returns a String, not a String*.
 * Convert_string_2_ship(PCSTR) overwrites this ship with a new parse,
 * (used in NameAndShipButton).
 *
 */
#ifndef	_SHIPDOCUMENT_H_
#define	_SHIPDOCUMENT_H_

#include "XpPoint.h"
#include "XpPointList.h"
#include "const.h"

class ShipDocument;
typedef	void (ShipDocument::*PumpFunc)(XpPoint *pt);
typedef	bool (ShipDocument::*PumpFuncBool)(XpPoint *pt, int d);

#define	SHIP_WIDTH		30		//(-15 to +15  x and y)

#define MIN_SHIP_PTS	3
#define MAX_SHIP_PTS	24
#define MAX_GUN_PTS	3
#define MAX_LIGHT_PTS	3
#define MAX_RACK_PTS	4

class ShipDocument : public Obj
{
protected:
	int num_points;
	XpPoint *start, *end; //Two temporary points for editing
	bool sel_start;

	friend class ShipEditWindow;
	friend class ShipWindow;
	friend class ShipListBox;

	XpPointList pts;

	//Manage all the items as point lists, even the ones that only allow 1 item.
	XpPointList m_gun;
	XpPointList engine;

	XpPointList m_racks;
	XpPointList l_guns;
	XpPointList r_guns;
	XpPointList l_rguns;
	XpPointList r_rguns;
	XpPointList l_lights;
	XpPointList r_lights;

	String author;

public: //Member Functions
	//ShipDocument(); //Default Constructor
	ShipDocument(int useDefault);		// true = include default ship
	ShipDocument(PCSTR ship_shape_str);
	ShipDocument* copy();

	void	Reset();		// empty the ShipDocument

	int		Get_shape_keyword(char *keyw);
	String	Convert_ship_2_string();
	void	Convert_string_2_ship(PCSTR s);

	void PumpShip(PumpFunc pf);
	void PumpShip(PumpFunc pf, XpPointList& ol);
	bool PumpShipBool(PumpFuncBool pf, XpPointList& ol, int d);

	bool VerifyMove(int dir); //Can we move?

	void ShiftLeft(XpPoint *pt); //Shift this point
	void ShiftRight(XpPoint *pt);
	void ShiftUp(XpPoint *pt);
	void ShiftDown(XpPoint *pt);

	void MirrorHoriz(XpPoint *pt); //Mirror this point about 0,0
	void MirrorVert(XpPoint *pt);
	void RotateLeft(XpPoint *pt); //Rotate this point about 0,0
	void RotateRight(XpPoint *pt);
	bool AtEdge(XpPoint *pt, int d);
	void SetMaximums(); //Set the max values for ship items.

	void Rotate_pointlist(XpPointList *ptl); //Rotate the ship through all its rotations, filling in the pointlist arrays.

	// In xp-flwidgets/ShipDocument.FL.cpp
	void	Draw(int baseX, int baseY, bool orientation);

};

extern int rs(int i);

#endif	// _SHIPDOCUMENT_H_
