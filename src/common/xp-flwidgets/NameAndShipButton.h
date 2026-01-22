/* $Id: NameAndShipButton.h,v 1.1 2004/05/28 17:33:09 dick Exp $
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
 * $Log: NameAndShipButton.h,v $
 * Revision 1.1  2004/05/28 17:33:09  dick
 * A button that displays the player's selected ship and his nickname.
 *
 */

#ifndef	_NAMEANDSHIPBUTTON_H_
#define	_NAMEANDSHIPBUTTON_H_

#include "FL/Fl_Button.H"

#include "ShipDocument.h"

const int NameAndShipButtonWidth	= 100;
const int NameAndShipButtonHeight	= 100;

#define	NameAndShipButtonSUPERCLASS Fl_Button
class NameAndShipButton : public NameAndShipButtonSUPERCLASS {
public:
	NameAndShipButton(int x,int y,int w,int h,const char *l=0);
	~NameAndShipButton();
	void	SetShip(PCSTR shapeStr);

	virtual void	draw();
	virtual int		handle(int);

private:
	ShipDocument	ship;
};

#endif	// _NAMEANDSHIPBUTTON_H_
