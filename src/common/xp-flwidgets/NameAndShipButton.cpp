/* $Id: NameAndShipButton.cpp,v 1.2 2004/07/22 17:02:38 dick Exp $
 *
 * NameAndShipButton - A button with the user's name and ship shape in it.
 *
 * common/xp-flwidgets - A library of FLTK widgets for XPilot5.
 *
 *      Copyright (C) 2004 by
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
 * $Log: NameAndShipButton.cpp,v $
 * Revision 1.2  2004/07/22 17:02:38  dick
 * Construct the ShipDocument with the default ship.
 *
 * Revision 1.1  2004/05/28 17:33:09  dick
 * A button that displays the player's selected ship and his nickname.
 *
 */

#include "StdAfx.h"
#include "FL/fl_draw.H"

#include "cstring.h"
#include "guiconst.h"
#include "IniClient.h"
#include "Fl_XPColors.h"

#include "NameAndShipButton.h"

///////////////////////////////////////////////////////////////////////////////
NameAndShipButton::NameAndShipButton(int x,int y,int w,int h,const char *l)
        : NameAndShipButtonSUPERCLASS(x,y,w,h,l),
		ship(true)
{
}

///////////////////////////////////////////////////////////////////////////////
NameAndShipButton::~NameAndShipButton()
{
}

///////////////////////////////////////////////////////////////////////////////
// virtual
void NameAndShipButton::draw()
{
	fl_clip(x(), y(), w(), h());
	String name = iniClient.name;
//	fl_rectf(x(), y(), w(), h(), xpBackColor);
	int nw = 0;
	int	nh = 0;
	fl_color(FL_WHITE);
	fl_font(FL_HELVETICA, 14);
	fl_measure(name, nw, nh, 0);
	fl_draw(name, w()/2-nw/2+x(), y()+h()-fl_descent()-1);
	ship.Draw(x()+w()/2-0, y()+h()-fl_descent()-nh-SHIP_WIDTH/2-2, true);
//	fl_color(FL_RED);
//	fl_line(x()+w()/2, y(), x()+w()/2, y()+h());
	fl_pop_clip();
}

///////////////////////////////////////////////////////////////////////////////
// virtual
int NameAndShipButton::handle(int event)
{
	switch (event) {
	case SELECTSHIP :
		SetShip(iniClient.shipShape);
		redraw();
		break;
	}
	return NameAndShipButtonSUPERCLASS::handle(event);
}

///////////////////////////////////////////////////////////////////////////////
void NameAndShipButton::SetShip(PCSTR shapeStr)
{
	ship.Convert_string_2_ship(shapeStr);
}
