/* $Id: ShipListBox.h,v 1.2 2004/05/28 17:29:10 dick Exp $
 *
 * ShipListBox - Draw a list box with all of the ships in a shipfile
 *
 * common/xp-flwidgets - A library of FLTK widgets for XPilot5.
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
 * $Log: ShipListBox.h,v $
 * Revision 1.2  2004/05/28 17:29:10  dick
 * Move single ship drawing to its own function.
 *
 */
#ifndef	_SHIPLISTBOX_H_
#define	_SHIPLISTBOX_H_

#include "FL/Fl_Box.H"
#include "ShipList.h"

#define	ShipListSUPERCLASS	Fl_Box
class ShipListBox : public ShipListSUPERCLASS {
public:
        ShipListBox(int x,int y,int w,int h, bool orient, const char *l, ShipList *sl);

protected:
        virtual void	draw();
        virtual int		handle(int);
public:
		ShipList *pShipList;
		int iPos;
		bool orientation; //True = Vert, False = Horiz
};

#endif	// _SHIPLISTBOX_H_
