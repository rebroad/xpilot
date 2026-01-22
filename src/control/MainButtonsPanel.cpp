/* $Id: MainButtonsPanel.cpp,v 1.10 2004/06/01 23:13:00 dick Exp $
 *
 * MainButtonsPanel - Draw the main panel on the left.
 *
 * XPilot5 - Manage all local XPilot activity.
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
 * $Log: MainButtonsPanel.cpp,v $
 * Revision 1.10  2004/06/01 23:13:00  dick
 * You can't really make a new button from an object and stick it in a static.
 * (Who owns it if you open 2 MapEditors?)  So now each window keeps track of
 * his own buttons.
 *
 * Revision 1.9  2004/05/30 16:03:55  dick
 * Use a destructor to delete the background image when we're done.
 *
 * Revision 1.8  2004/05/28 18:43:16  dick
 * Add a NameAndShipButton to the bottom of the panel.
 *
 * Whoa.  Busted up logs, eh?
 * Man 1.3  2004/02/03 03:39:53  dick
 * Add WininButtonsPanel.cpp,v $
 * Revisiodows precompiled header support.
 *
 * Revision 1.2  2003/12/22 05:59:50  dick
 * Use png(s) instead of xpm(s).
 *
 * Revision 1.1  2002/09/18 14:49:22  jlmiller
 * Changes necessary for Main window to be resizable, and tiled.
 *
 */
#include "StdAfx.h"

#ifndef	_WINDOWS
#include <unistd.h>
#endif

#include "FL/Fl_Group.H"
#include "FL/Fl_PNG_Image.H"
#include "FL/fl_draw.H"

#include "Fl_XPColors.h"

#include "config.h"
#include "xpprintf.h"

#include "XPilotControlWindow.h"
#include "XPClientConfig.h"
#include "MainButtonsPanel.h"
#include "controlconst.h"
#include "xpcTypes.h"
#include "proto.h"
#include "NameAndShipButton.h"

static void NameButtonCallback(Fl_Widget *w, void* v);

///////////////////////////////////////////////////////////////////////////////
#define	BUTTPIX(_base)	\
	(_base "U"),	\
	(_base "D"),	\
	(_base "X")

#define	BUTT_LOCAL		0
#define	BUTT_INET		1
#define	BUTT_LAN		2
#define	BUTT_PRIVATE	3
#define	BUTT_EDITORS	4

xpe_tbtblkbutton toggleMainButtonsDefs[numToggleMainButtons] =
{
	{"xpilots", BUTTPIX("mainbut0"),
		IDM_CONNECT_LOCAL,    new Rect(7,  12, 197, 84), "&Local Server", "Configure Local Server (Alt-L)"},
	{"xpwhere", BUTTPIX("mainbut1"),
		IDM_CONNECT_INTERNET, new Rect(7,  84, 197, 156), "Connect to\n&Internet Server...", "Join or Admin an XPilot Server\non the Internet (Alt-I)"},
	{"nethood", BUTTPIX("mainbut2"),
		IDM_CONNECT_LAN,      new Rect(7, 156, 197, 228), "Connect to\nL&an Server...", "Join or Admin an XPilot Server\non the local network (Alt-A)"},
	{"nethood", BUTTPIX("mainbut3"),
		IDM_CONNECT_PRIVATE,  new Rect(7, 228, 197, 300), "Connect to\nPri&vate Server...", "Join or Admin an XPilot Server\nthat you know about privately (Alt-V)"},
	{"xpilotpencil", BUTTPIX("mainbut4"),
		IDM_EDITORS,          new Rect(7, 367, 197, 439), "&Editors", "Lots of editors (Alt-E)"},
};

///////////////////////////////////////////////////////////////////////////////
MainButtonsPanel::MainButtonsPanel(int x, int y, int w,int h,const char *l)
        : MainButtonsPanelSUPERCLASS(x,y,w,h,l)
{
	int	i;
	iSelectionMainButtons = IDM_SPLASH;

	Fl_Box* bigbox = new Fl_Box(FL_DOWN_BOX, x, y, w, h, "");
	bigbox->color(xpBackColor);

	bgImage = new Fl_PNG_Image(GetButtonFile("mainpanelback"));

	Fl_Group *o = new Fl_Group(x, y, w,	h);
	for (i=0; i < numToggleMainButtons; i++)
	{
		toggleMainButtons[i] = new FunctionToggleButton(
			toggleMainButtonsDefs[i].rect->left+x,
			toggleMainButtonsDefs[i].rect->top+y,
			toggleMainButtonsDefs[i].rect->right - toggleMainButtonsDefs[i].rect->left,
			toggleMainButtonsDefs[i].rect->bottom - toggleMainButtonsDefs[i].rect->top,
			toggleMainButtonsDefs[i].lbl,
			toggleMainButtonsDefs[i].pm,
			toggleMainButtonsDefs[i].bgU, toggleMainButtonsDefs[i].bgD, toggleMainButtonsDefs[i].bgX
			);
		toggleMainButtons[i]->id = toggleMainButtonsDefs[i].id;
		toggleMainButtons[i]->togglevar = &iSelectionMainButtons;
		toggleMainButtons[i]->type(FL_RADIO_BUTTON);
		toggleMainButtons[i]->callback(MainButtonsPanel::CallbackFl);
		toggleMainButtons[i]->align(FL_ALIGN_RIGHT);
		toggleMainButtons[i]->tooltip(toggleMainButtonsDefs[i].ttip);
		toggleMainButtons[i]->user_data(this);
	}

	nameAndShipButton = new NameAndShipButton(
			w/2-NameAndShipButtonWidth/2,
			h-NameAndShipButtonHeight-2,
			NameAndShipButtonWidth,
			NameAndShipButtonHeight
			);
	nameAndShipButton->callback((Fl_Callback*)NameButtonCallback);
	nameAndShipButton->SetShip(iniClient.shipShape);

	o->end();
	o->resizable(NULL);

	// TEMP disable Lan list
	toggleMainButtons[BUTT_LAN]->deactivate();
}

///////////////////////////////////////////////////////////////////////////////
MainButtonsPanel::~MainButtonsPanel()
{
	if (bgImage)
		delete bgImage;
	bgImage = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// static
void MainButtonsPanel::CallbackFl(Fl_Widget *w, void* v)
{
	int	i;

	FunctionToggleButton* btn = (FunctionToggleButton *) w;
	MainButtonsPanel* mbp = (MainButtonsPanel *) v;
	Fl_Tile* tmp1 = (Fl_Tile *) mbp->parent();
	XPilotControlWindow* xcw = (XPilotControlWindow *) tmp1->parent();

	// D(xpprintf("toolbar_cb = %d\n", btn->id);)


	if (*btn->togglevar != btn->id)
	{
		*btn->togglevar = btn->id;
		xcw->UpdateMainWindow();
		//Fl::check();
		for (i=0; i < numToggleMainButtons; i++)
			mbp->toggleMainButtons[i]->redraw();
		Fl::check();
	}
}

///////////////////////////////////////////////////////////////////////////////
static void NameButtonCallback(Fl_Widget *w, void* v)
{
	XPClientConfig* cfg = XPClientConfig::Factory();
	cfg->user_data(w);

}

///////////////////////////////////////////////////////////////////////////////
void MainButtonsPanel::draw()
{
	if (bgImage)	// has bg image
	{
		int	i;

		fl_clip(x(), y(), w(), h());
		fl_color(FL_BLACK);
		fl_rectf(x(), y(), w(), h());
//		if (active())
		if (1)
		{
			bgImage->draw(x(), y());
			//pixmap->draw(myX+32, myY+18);
		}
		//draw_children();
		for (i=0; i < numToggleMainButtons; i++)
			draw_child(*toggleMainButtons[i]);
		draw_child(*nameAndShipButton);
		fl_pop_clip();
	}
	else
		MainButtonsPanelSUPERCLASS::draw();
}
