/* $Id: ShipListViewWindow.h,v 1.4 2004/05/28 17:29:10 dick Exp $
 *
 * ShipListViewWindow - Draw the ShipListBox in a scrollable window
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
 * $Log: ShipListViewWindow.h,v $
 * Revision 1.4  2004/05/28 17:29:10  dick
 * Move single ship drawing to its own function.
 *
 */
#ifndef	_SHIPLISTVIEWWINDOW_H_
#define	_SHIPLISTVIEWWINDOW_H_

#include "FL/Fl_Window.H"
#include "FL/Fl_Scrollbar.H"
#include "ShipListBox.h"
#include "ShipList.h"

class ShipListViewWindow : public Fl_Window {
    virtual void	draw();
    virtual int		handle(int);

public:
	ShipList*		pShipList;
	ShipListBox*	slb;
	Fl_Scrollbar*	sb;
	int				iScrollPos;
	int				iScrollMax;
	bool			drawVert; //True = Vert, False = Horiz
public:
    ShipListViewWindow(int x,int y,int w,int h, bool orient, const char *l, ShipList *sl);
	void ViewSelectedShip();
};


#endif	// _SHIPLISTVIEWWINDOW_H_
