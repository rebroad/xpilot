/* $Id: PrivatePanelEditButton.h,v 1.1 2004/05/10 12:48:00 dick Exp $
 *
 * PrivatePanelEditButton - One of the Red buttons that perform an action
 *
 *      Copyright (C) 2004 by
 *
 *      The XPilot Authors   <xpilot@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
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
 * $Log: PrivatePanelEditButton.h,v $
 * Revision 1.1  2004/05/10 12:48:00  dick
 * Add a Private Server button to the main panel, which allows the user to
 * manually track servers that don't appear in meta.
 *
 */

#ifndef	_PRIVATEPANELEDITBUTTON_H_
#define	_PRIVATEPANELEDITBUTTON_H_

#include "FL/Fl_Button.H"


class PrivateServerPanel;
const int	privateEditButtonWidth	= 60;
const int	privateEditButtonHeight	= 30;

#define	PrivatePanelEditButtonSUPERCLASS	Fl_Button
class PrivatePanelEditButton : public PrivatePanelEditButtonSUPERCLASS
{
public:
	PrivatePanelEditButton(int X, int Y, int W, int H, const char* label = 0);

//	virtual	int		handle(int event);
	static Fl_Callback	myCallback;
	PrivateServerPanel*	myPanel;
};


#endif		// _PRIVATEPANELEDITBUTTON_H_
