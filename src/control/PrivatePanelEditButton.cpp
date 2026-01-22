/* $Id: PrivatePanelEditButton.cpp,v 1.2 2004/05/13 09:29:59 dick Exp $
 *
 * PrivatePanelEditButton - One of the Red buttons that perform an action
 *
 * XPilot5 - Manage all local XPilot activity.
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
 * $Log: PrivatePanelEditButton.cpp,v $
 * Revision 1.2  2004/05/13 09:29:59  dick
 * Whitespace
 *
 * Revision 1.1  2004/05/10 12:48:00  dick
 * Add a Private Server button to the main panel, which allows the user to
 * manually track servers that don't appear in meta.
 */
#include "StdAfx.h"

#include <stdlib.h>
#include <stdio.h>

#include "FL/Fl.H"

#include "PrivatePanelEditButton.h"
#include "PrivateServerPanel.h"

extern	Fl_Color xpred;

///////////////////////////////////////////////////////////////////////////////
PrivatePanelEditButton::PrivatePanelEditButton(int X, int Y, int W, int H, const char* label)
: PrivatePanelEditButtonSUPERCLASS(X,Y,W,H,label)
{
	color(xpred);
	color2(xpred);
	callback(myCallback, this);
}

///////////////////////////////////////////////////////////////////////////////
void PrivatePanelEditButton::myCallback(Fl_Widget* widget, void* myThis)
{
	PrivatePanelEditButton* b = (PrivatePanelEditButton*)widget;
	b->myPanel->ActionButtonHandler(b);
}
