/* $Id: EditorsPanel.cpp,v 1.11 2007/02/17 06:16:45 dick Exp $
 *
 * EditorsPanel - The panel on the right that contains the different editor buttons
 *
 * XPilot5 - Manage all local XPilot activity.
 *
 *      Copyright (C) 2002 by
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
 * $Log: EditorsPanel.cpp,v $
 * Revision 1.11  2007/02/17 06:16:45  dick
 * Add the Sound Editor.
 *
 * Revision 1.10  2004/06/01 23:13:00  dick
 * You can't really make a new button from an object and stick it in a static.
 * (Who owns it if you open 2 MapEditors?)  So now each window keeps track of
 * his own buttons.
 *
 * Revision 1.9  2004/05/28 18:36:19  dick
 * Keep a pointer to the NameAndShipButton that's on the main panel, and give
 * it to the ClientConfig.
 *
 * Revision 1.8  2004/05/05 01:05:27  dick
 * "New Map Editor" becomes "Map Editor" (Ship Editor, too)
 *
 * Revision 1.7  2004/03/03 07:54:34  dick
 * Don't make pixmaps at load time; wait until runtime init, after Strings have
 * actually been inited.
 *
 * Revision 1.6  2004/02/29 00:12:37  dick
 * Draw background in xpBackColor and labels in xpLabelColor.
 *
 * Revision 1.5  2004/02/06 03:39:20  dick
 * Add some hotkeys to the editors panel
 *
 * Revision 1.4  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.3  2003/12/22 05:59:50  dick
 * Use png(s) instead of xpm(s).
 *
 * Revision 1.2  2002/09/18 14:49:22  jlmiller
 * Changes necessary for Main window to be resizable, and tiled.
 *
 * Revision 1.1  2002/08/28 15:30:38  jlmiller
 * Make EditorsPanel a separate class, similar to LocalServerPanel.
 *
 */
#include "StdAfx.h"

#ifndef	_WINDOWS
#include <unistd.h>
#endif

#include "FL/Fl_Group.H"
#include "FL/Fl_Box.H"
#include "FL/Fl_PNG_Image.H"

#include "Fl_XPColors.h"

#include "cstring.h"
#include "controlconst.h"
#include "proto.h"
#include "types.h"
#include "xpcTypes.h"
#include "EditorsPanel.h"
#include "XPClientConfig.h"
#include "ShipWindow.h"
#include "MapWindow.h"
#include "audio/SoundWindow.h"


const int	buttonTop = 50;
#define	ROW(_x)	((BUTTON_SIZE_LG+2)*_x+buttonTop)
#define	WID		(BUTTON_SIZE_LG)
#define	HGT		(BUTTON_SIZE_LG)

static void MapEditorCallback(Fl_Widget* w, void* v);
static void ShipEditorCallback(Fl_Widget* w, void* v);
static void ClientConfigureCallback(Fl_Widget* w, void* v);
static void SoundConfigureCallback(Fl_Widget* w, void* v);

xpe_stbbutton editorsActionButtonsDefs[numEditorsActionButtons] =
{
	{2, ROW(0), "&MapEditor", "New Map Editor",
		"mapeditor", MapEditorCallback},
	{2, ROW(1), "&ShipEditor", "Start a new Ship Editor",
		"shipeditor", ShipEditorCallback},
	{2, ROW(2), "Replay &Recording", "Replay Recording",
		"filenew", MapEditorCallback},
	{2, ROW(3), "&Client Settings", "Configure Client Settings",
		"clientoptions", ClientConfigureCallback},
	{2, ROW(4), "S&ound Settings", "Setup what sounds are played",
		"filenew", SoundConfigureCallback},
};

///////////////////////////////////////////////////////////////////////////////
EditorsPanel::EditorsPanel(int x, int y, int w,int h,const char *l)
        : EditorsPanelSUPERCLASS(x,y,w,h,l)
{
	int	i;
	begin();
	Fl_Box* bigbox = new Fl_Box(FL_DOWN_BOX, x, y,
								   w, h, "");
	bigbox->color(xpBackColor);

	//Group so the buttons don't resize.
	Fl_Group* o = new Fl_Group(x, y, w, h, "");
	Fl_Box* label = new Fl_Box(x, y, w, 30, "Editors");
	label->labelcolor(xpLabelColor);

	for (i = 0; i < numEditorsActionButtons; i++)
    {
		editorsActionButtons[i] = new FunctionButton(
			editorsActionButtonsDefs[i].x+x, editorsActionButtonsDefs[i].y+y,
			WID, HGT,
			editorsActionButtonsDefs[i].lbl, editorsActionButtonsDefs[i].pm);
		editorsActionButtons[i]->align(FL_ALIGN_RIGHT);
		editorsActionButtons[i]->callback(editorsActionButtonsDefs[i].cb);
		editorsActionButtons[i]->tooltip(editorsActionButtonsDefs[i].ttip);
		editorsActionButtons[i]->user_data(this);
		editorsActionButtons[i]->labelcolor(xpLabelColor);
    }
	o->end();
	o->resizable(NULL);
	end();

	// temp.  These do nothing anyway...
	editorsActionButtons[2]->deactivate();
}

///////////////////////////////////////////////////////////////////////////////
EditorsPanel::~EditorsPanel()
{
//	int	i;
//	for (i = 0; i < numEditorsActionButtons; i++)
//		delete editorsActionButtons[i];
}

///////////////////////////////////////////////////////////////////////////////
static void MapEditorCallback(Fl_Widget*, void*) {
	MapWindow::Factory();
}

///////////////////////////////////////////////////////////////////////////////
static void ShipEditorCallback(Fl_Widget*, void*) {
	ShipWindow::Factory();
}

///////////////////////////////////////////////////////////////////////////////
static void ClientConfigureCallback(Fl_Widget* w, void* v)
{
	EditorsPanel* ep = (EditorsPanel*)v;
	XPClientConfig* xcc = XPClientConfig::Factory();
	xcc->user_data(ep->nameAndShipButton);
}

///////////////////////////////////////////////////////////////////////////////
static void SoundConfigureCallback(Fl_Widget* w, void* v)
{
	//EditorsPanel* ep = (EditorsPanel*)v;
	SoundWindow::Factory();
	//xcc->user_data(ep->nameAndShipButton);
}
