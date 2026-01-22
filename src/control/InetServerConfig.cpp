/* $Id: InetServerConfig.cpp,v 1.2 2004/05/28 18:40:36 dick Exp $
 *
 * InetServerConfig - Window that configures the InetServerPanel
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
 * $Log: InetServerConfig.cpp,v $
 * Revision 1.2  2004/05/28 18:40:36  dick
 * xpcheckbutton_cb() becomes XPCheckButton::CallbackFl() .
 *
 * Revision 1.1  2004/05/13 09:17:08  dick
 * XPWhereWindow becomes InetServerPanel, (which matches LocalServerPanel and PrivateServerPanel).
 * XPWhereConfig becomes InetServerConfig.
 *
 * Revision 1.9  2004/05/10 17:34:13  dick
 * type switch has gone away in favor of subclassing configs) (yet it still built on Windows, hmm).
 *
 * Revision 1.8  2004/05/10 12:44:37  dick
 * Callbacks are static here
 *
 * Revision 1.7  2004/02/09 18:22:48  dick
 * Lots of whitespace changes to support fixing resizable columns.
 *
 * Revision 1.6  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.5  2002/09/02 16:29:41  dick
 * Get our user config from the user's config passed in.
 *
 * Revision 1.4  2002/08/20 20:07:09  jlmiller
 * Deadwood delete.
 */
#include "StdAfx.h"

#include "controlconst.h"
#include "InetServerList.h"
#include "InetServerConfig.h"
#include "InetServerPanel.h"
#include "guiconst.h"
#include "const.h"

extern MetaHost metaHosts[MAXMETAHOSTS];

static xpc_xpcheckbutton XPWhereColumnChecks[] =
{
        {300, 5, 20, 25, "Server Name"},
        {300, 25, 20, 25, "Map Name"},
        {300, 45, 20, 25, "Map Author"},
        {300, 65, 20, 25, "Map Size"},
        {300, 85, 20, 25, "# of players"},
        {300, 105, 20, 25, "Lag"},
        {300, 125, 20, 25, "# of bases"},
        {300, 145, 20, 25, "# of teams"},
        {300, 165, 20, 25, "Frames per Second"},
        {300, 185, 20, 25, "Server Version"},
        {300, 205, 20, 25, "Sound"},
        {300, 225, 20, 25, "Uptime"},
        {300, 245, 20, 25, "IP address"},
        {300, 265, 20, 25, "Race mode"},
        {300, 285, 20, 25, "Port"},
};

///////////////////////////////////////////////////////////////////////////////
static void xpwcok_cb(Fl_Widget*, void* v) {
	InetServerConfig *w = (InetServerConfig *) v;
	int i;

	for (i = 0; i < numServerListColumns; i++)
		w->isPanel->fsl->cfg->columns[i] = w->localColumns[i];

	w->metaHost->Store();
	w->metaHostAlt->Store();

	w->isPanel->fsl->SetColumnSizesFromCfg();
	w->isPanel = NULL;
	w->hide();
}
///////////////////////////////////////////////////////////////////////////////
static void xpwccancel_cb(Fl_Widget*, void* v) {
	InetServerConfig *w = (InetServerConfig *) v;

	w->isPanel = NULL;
	w->hide();
}
///////////////////////////////////////////////////////////////////////////////
InetServerConfig* InetServerConfig::Factory()
{
	InetServerConfig *window = new InetServerConfig(570, 390, "Internet List Config");
	window->end();

	window->show();
	window->make_current();
	return(window);
}
///////////////////////////////////////////////////////////////////////////////
void InetServerConfig::SetupLocalColumns()
{
	int i;

	for (i = 0; i < numServerListColumns; i++)
		localColumns[i] = isPanel->fsl->cfg->columns[i];
}

///////////////////////////////////////////////////////////////////////////////
void InetServerConfig::UpdateDisplay()
{
	int i;
	XPCheckButton *tchk = NULL;

	//TODO: Fix the following items so they arent just disabled.
	checkPrimary1->deactivate();
	checkPrimary2->deactivate();
	updateCheck->deactivate();
	updateTime->deactivate();

	metaHost->activate();
	metaHostAlt->activate();
//		checkPrimary1->activate();
//		checkPrimary2->activate();
	for (i = 0; i< NELEM(XPWhereColumnChecks); i++)
	{
		tchk = XPWhereColumnChecks[i].button;
		tchk->SetBool(&localColumns[i]);
		tchk->Read();
	}

	metaHost->SetStrng(&metaHosts[0].name);
	metaHost->Read();
	metaHostAlt->SetStrng(&metaHosts[1].name);
	metaHostAlt->Read();
}


///////////////////////////////////////////////////////////////////////////////
InetServerConfig::InetServerConfig(int w,int h,const char *l)
        : Fl_Window(w,h,l)
{
	int i;
    { Fl_Box* o = new Fl_Box(5, 5, 290, 70);
      o->box(FL_ENGRAVED_BOX);
    }
    metaHost = new XPInput(110, 15, 110, 25, "meta host:");
    metaHostAlt = new XPInput(110, 45, 110, 25, "meta host (alt):");
    { Fl_Group* o = new Fl_Group(225, 10, 100, 75);
      { Fl_Check_Button* o = checkPrimary1 = new Fl_Check_Button(250, 20, 20, 20, "Primary");
        o->down_box(FL_ROUND_DOWN_BOX);
        o->align(FL_ALIGN_TOP);
		o->type(FL_RADIO_BUTTON);
      }
      { Fl_Check_Button* o = checkPrimary2 = new Fl_Check_Button(250, 50, 20, 20);
        o->down_box(FL_ROUND_DOWN_BOX);
        o->align(FL_ALIGN_TOP);
		o->type(FL_RADIO_BUTTON);
      }
      o->end();
    }

    { Fl_Check_Button* o = updateCheck = new Fl_Check_Button(20, 80, 125, 25, "Update list every");
      o->down_box(FL_DOWN_BOX);
      o->align(FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
    }
    { Fl_Input* o = updateTime = new Fl_Input(160, 80, 45, 25, "minutes");
      o->align(FL_ALIGN_RIGHT);
    }

	for (i = 0; i < NELEM(XPWhereColumnChecks); i++)
	{
		XPWhereColumnChecks[i].button = new XPCheckButton(XPWhereColumnChecks[i].x, XPWhereColumnChecks[i].y,
			XPWhereColumnChecks[i].w, XPWhereColumnChecks[i].h, XPWhereColumnChecks[i].lbl);
		XPWhereColumnChecks[i].button->callback((Fl_Callback*)XPCheckButton::CallbackFl);
		XPWhereColumnChecks[i].button->align(FL_ALIGN_RIGHT);
	}

	okButton = new Fl_Button(410, 5, 95, 25, "OK");
	okButton->callback((Fl_Callback *)xpwcok_cb, this);
	cancelButton = new Fl_Button(410, 32, 95, 25, "Cancel");
	cancelButton->callback((Fl_Callback *)xpwccancel_cb, this);
	set_modal();
}
