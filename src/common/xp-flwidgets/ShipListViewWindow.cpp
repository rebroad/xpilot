/* $Id: ShipListViewWindow.cpp,v 1.9 2004/05/28 17:29:10 dick Exp $
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
 * $Log: ShipListViewWindow.cpp,v $
 * Revision 1.9  2004/05/28 17:29:10  dick
 * Move single ship drawing to its own function.
 *
 * Revision 1.8  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.7  2002/09/18 18:06:49  jlmiller
 * forgot to always calc iScrollMax
 *
 * Revision 1.6  2002/09/18 16:51:01  jlmiller
 * Can now set view to current selected ship.
 *
 * Revision 1.5  2002/08/23 14:18:09  jlmiller
 * Fix bug in scrollbar.
 *
 * Revision 1.4  2002/07/23 18:16:03  jlmiller
 * Draw ships the correct size.
 *
 * Revision 1.3  2002/07/15 19:12:12  jlmiller
 * UPDATESHIPWIN becomes SELECTSHIP for better readability.
 *
 * Revision 1.2  2002/07/09 15:22:31  jlmiller
 * Make the shiplistbox a separate class from the shiplistviewwindow
 * as Fl_Box doesnt handle a child scrollbar well at all. ShipListViewWindow
 * is now an Fl_Window, with two child widgets.
 *
 * Revision 1.1  2002/06/25 20:21:46  jlmiller
 * Add to xp-flwidgets.
 *
 * Revision 1.11  2002/06/25 15:02:19  jlmiller
 * Move iSelection fields to the window, instead of the individual document,
 * as they didn't make any sense at document level.
 *
 * Revision 1.10  2002/06/17 21:00:11  jlmiller
 * Clear undo buffer, if called for.
 *
 * Revision 1.9  2002/06/02 22:44:05  jlmiller
 * Add undoMan to shipeditor.
 * Commentability.
 *
 * Revision 1.8  2002/05/30 02:26:14  jlmiller
 * Eliminate unnecessary global pointer. Should help eliminate null pointer bugs.
 *
 * Revision 1.7  2002/05/28 07:19:50  dick
 * Rename xpilotedit to XPilotControl
 *
 */
#include "StdAfx.h"

#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#include "FL/Fl.H"
#include "FL/Fl_Window.H"
#include "FL/fl_draw.H"


#include "ShipListViewWindow.h"
#include "ShipDocument.h"
#include "guiconst.h"
#include "XpPoint.h"
#include "const.h"

///////////////////////////////////////////////////////////////////////////////
void slsb_cb(Fl_Widget* w, void* v)
{
	ShipListViewWindow *tmp = (ShipListViewWindow *) v;
	tmp->iScrollPos = tmp->sb->value();
	tmp->slb->iPos = tmp->iScrollPos;
	tmp->redraw();
}

///////////////////////////////////////////////////////////////////////////////
ShipListViewWindow::ShipListViewWindow(int x,int y,int w,int h, bool orient, const char *l, ShipList *sl)
        : Fl_Window(x,y,w,h,l)
{
	int wid, hei, scrx, scry, scrwid, scrhei;

	pShipList = sl;
	iScrollPos = 0;
	iScrollMax = 0;
	if (orient == true)//true == Vertical
	{
		wid = SHIPLIST_SIZE;
		hei = h;
		scrx = w-SCROLLBAR_SIZE;
		scry = 0;
		scrwid = SCROLLBAR_SIZE;
		scrhei = h;
	}
	else
	{
		sb = new Fl_Scrollbar(0, h-SCROLLBAR_SIZE, w, SCROLLBAR_SIZE, NULL);
		wid = w;
		hei = SHIPLIST_SIZE;
		scrx = 0;
		scry = h-SCROLLBAR_SIZE;
		scrwid = w;
		scrhei = SCROLLBAR_SIZE;
	}
	slb = new ShipListBox(0, 0, wid, hei, orient, NULL, sl);
	sb = new Fl_Scrollbar(scrx, scry, scrwid, scrhei, NULL);

	if (!orient)
		sb->type(FL_HORIZONTAL);

	drawVert = orient;
	sb->callback(slsb_cb);
	sb->linesize(1);
	sb->user_data(this);
}

///////////////////////////////////////////////////////////////////////////////
void ShipListViewWindow::ViewSelectedShip() {
	int num_view = 0;
	int current_is = pShipList->Index(pShipList->pCurrentShip);

	if (drawVert)
	{
		num_view = slb->h() / SPM_SIZE;
		iScrollMax = MAX (0, pShipList->GetCount() - h() / SPM_SIZE + 1) ;
	}
	else
	{
		num_view = slb->w() / SPM_SIZE;
		iScrollMax = MAX (0, pShipList->GetCount() - w() / SPM_SIZE + 1) ;
	}


	if (current_is < num_view)
	{
		if (sb->value() >= abs(current_is))
		{
			iScrollPos = 0;
			slb->iPos = 0;
		}
	}
	else
	{
		iScrollPos = MIN(current_is-num_view+1, iScrollMax);
		slb->iPos = iScrollPos;
	}

	sb->value(iScrollPos, 1, 0, iScrollMax);


	sb->redraw();
	slb->redraw();
}

///////////////////////////////////////////////////////////////////////////////
void ShipListViewWindow::draw()
{
	fl_color(FL_BLACK);

	if (drawVert)	//Are we a horizontal or vertical shiplistviewwindow?
	{
		fl_rectf(0,0, w()-SCROLLBAR_SIZE, h());
		iScrollMax = MAX (0, pShipList->GetCount() - h() / SPM_SIZE + 1) ;
	}
	else
	{
		fl_rectf(0,0, w(), h()-SCROLLBAR_SIZE);
		iScrollMax = MAX (0, pShipList->GetCount() - w() / SPM_SIZE + 1) ;
	}

	if (iScrollMax == 0)
	{
		sb->hide();
		iScrollPos = 0;
		slb->iPos = 0;
	}
	else
	{
		sb->show();
		iScrollPos = MIN (iScrollPos, iScrollMax) ;
		slb->iPos = iScrollPos;
	}

	sb->value(iScrollPos, 1, 0, iScrollMax);

	sb->redraw();
	slb->redraw();
	Fl_Window::draw();
}

///////////////////////////////////////////////////////////////////////////////
int ShipListViewWindow::handle(int event)
{
	Fl_Window *tmp = (Fl_Window *) parent();

	switch (event) {
	case SELECTSHIP :
		return tmp->handle(SELECTSHIP);
	}
	return Fl_Window::handle(event);
}
