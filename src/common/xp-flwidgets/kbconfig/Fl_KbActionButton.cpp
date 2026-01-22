/* $Id: Fl_KbActionButton.cpp,v 1.3 2004/02/03 03:39:53 dick Exp $
 *
 * Fl_KbActionButton - One of the Red buttons that perform an action
 *
 *      Copyright (C) 2001 by
 *
 *      The XPilot Authors   <xpilot@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
 *      Jarrod Miller        <jarrod@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
 *      Bjørn Stabell        <bjoern@xpilot.org>
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
 * $Log: Fl_KbActionButton.cpp,v $
 * Revision 1.3  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.2  2001/09/27 17:44:57  dick
 * Handle callbacks to this button type.
 *
 * Revision 1.1  2001/09/25 15:48:38  dick
 * A widget to handle the "Action" commands, Load/Save/Undo/Redo.
 *
 */
#include "StdAfx.h"

#include <stdlib.h>
#include <stdio.h>

#include "FL/Fl.H"

#include "Fl_KbActionButton.h"
#include "Fl_KbConfig.h"

extern	Fl_Color xpred;

///////////////////////////////////////////////////////////////////////////////
Fl_KbActionButton::Fl_KbActionButton(int X, int Y, int W, int H, const char* label)
: Fl_KbActionButtonSUPERCLASS(X,Y,W,H,label)
{
	color(xpred);
	color2(xpred);
	callback(myCallback, this);
}

///////////////////////////////////////////////////////////////////////////////
#if 0
int	Fl_KbActionButton::handle(int event)
{
	int ret = Fl_KbActionButtonSUPERCLASS::handle(event);
	printf("Fl_KbActionButton::handle(%s)\n", eventName[event]);
	switch (event)
	{
	case FL_PUSH:
		return(1);
	case FL_RELEASE:
		if (Fl::belowmouse() == this)
			myKbConfig->ActionButtonHandler(this);
		return(1);
	}
	return(0);
}
#endif
void Fl_KbActionButton::myCallback(Fl_Widget* widget, void* myThis)
{
	Fl_KbActionButton* b = (Fl_KbActionButton*)widget;
	b->myKbConfig->ActionButtonHandler(b);
}
