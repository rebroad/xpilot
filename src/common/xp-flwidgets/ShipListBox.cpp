/* $Id: ShipListBox.cpp,v 1.6 2004/05/28 17:29:10 dick Exp $
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
 * $Log: ShipListBox.cpp,v $
 * Revision 1.6  2004/05/28 17:29:10  dick
 * Move single ship drawing to its own function.
 *
 */
#include "StdAfx.h"

#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#include "FL/Fl.H"
#include "FL/Fl_Window.H"
#include "FL/fl_draw.H"


#include "ShipListBox.h"
#include "ShipDocument.h"
#include "guiconst.h"
#include "XpPoint.h"
#include "const.h"

///////////////////////////////////////////////////////////////////////////////
ShipListBox::ShipListBox(int x,int y,int w,int h, bool orient, const char *l, ShipList *sl)
        : Fl_Box(x,y,w,h,l)
{
	box(FL_DOWN_BOX);
	color(FL_BLACK);
	pShipList = sl;
	iPos = 0;
	orientation = orient;
}

///////////////////////////////////////////////////////////////////////////////
void ShipListBox::draw() {
	int i = 0,j = 0;
	int iPaintBeg, iPaintEnd;

	Fl_Box::draw();

	ShipDocument*	shp;

	fl_color(FL_BLACK);

	if (orientation)//Are we a horizontal or vertical shiplistviewwindow?
	{
		//Be sure to not draw over the surrounding box. 2 pixels should be enough.
		fl_rectf(1,1, w()-SCROLLBAR_SIZE, h()-2);
	}
	else
	{
		fl_rectf(1,1, w()-2, h()-SCROLLBAR_SIZE-2);
	}

	fl_color(FL_WHITE);

	iPaintBeg = MAX (0, iPos);

	if (orientation)//Are we a horizontal or vertical shiplistviewwindow?
		iPaintEnd = MIN (pShipList->GetCount(), iPos + h() / SPM_SIZE+1);
	else
		iPaintEnd = MIN (pShipList->GetCount(), iPos + w() / SPM_SIZE+1);

	fl_clip(0, 0, w(), h());

	shp = (ShipDocument *) pShipList->GetHead();

	if (shp)
	{
		for (j = 0; j < iPaintBeg; j++)
			shp = (ShipDocument *) shp->GetNext();
		for (j = iPaintBeg; j < iPaintEnd || j == pShipList->GetCount()-1; j++)
		{
			if (shp == pShipList->pCurrentShip)
			{
				fl_color(FL_RED);
				if (orientation)//Are we a horizontal or vertical shiplistviewwindow?
					fl_rect(rs(2), rs(2)+(j-iPaintBeg)*SPM_SIZE, 44, 44);
				else
					fl_rect(rs(-21)+(j-iPaintBeg)*SPM_SIZE, rs(-21), 44, 44);
			}
			if (orientation)
				// vertical box, horiz facing ships
				shp->Draw(24, 23+(j-iPaintBeg)*SPM_SIZE, !orientation);
			else
				// horiz box, vertical ships
				shp->Draw(rs(-21)+(j-iPaintBeg)*SPM_SIZE, rs(-21), !orientation);

			shp = (ShipDocument *) shp->GetNext();
		}
	}
	fl_pop_clip();
}

///////////////////////////////////////////////////////////////////////////////
int ShipListBox::handle(int event)
{
	int w_pos; //The position in the current window
	int l_pos; //The position in the current shiplist
	int j;
	Fl_Window *tmp = (Fl_Window *) parent();

	switch (event) {
	case FL_PUSH :
		if (Fl::event_button() == FL_LEFT_MOUSE)
		{
			if (orientation) //Are we a horizontal or vertical shiplistviewwindow?
				w_pos = Fl::event_y();
			else
				w_pos = Fl::event_x();

			l_pos = w_pos / SPM_SIZE + iPos;

			if ((l_pos < 0) || (l_pos > pShipList->GetCount()-1))
				return TRUE;

			pShipList->pCurrentShip = (ShipDocument *) pShipList->GetHead();
			for (j = 0; j < l_pos; j++)
				pShipList->pCurrentShip = (ShipDocument *) pShipList->pCurrentShip->GetNext();
			redraw();

			//Be sure to flag the list as changed.
			//As a way of notifying the parent window that we've changed,
			//tell the parent to handle the SELECTSHIP event,
			pShipList->changed = true;
			return tmp->handle(SELECTSHIP);
		}
	case FL_RELEASE :
		if (Fl::event_button() == FL_LEFT_MOUSE)
		{
			return TRUE;
		}
	case FL_MOVE :
		return TRUE;
	}
	return Fl_Box::handle(event);
}
