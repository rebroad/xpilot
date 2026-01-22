/* $Id: PrivateServerConfig.cpp,v 1.4 2004/05/28 18:45:49 dick Exp $
 *
 * PrivateServerConfig - Configure options for the PrivateServerPanel
 *
 * XPilot5 - Manage all local XPilot activity.
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
 * $Log: PrivateServerConfig.cpp,v $
 * Revision 1.4  2004/05/28 18:45:49  dick
 * xpcheckbutton_cb() becomes XPCheckButton::CallbackFl() .
 *
 * Revision 1.3  2004/05/13 09:29:59  dick
 * Whitespace
 *
 * Revision 1.2  2004/05/10 17:34:13  dick
 * type switch has gone away in favor of subclassing configs) (yet it still built on Windows, hmm).
 *
 * Revision 1.1  2004/05/10 12:48:00  dick
 * Add a Private Server button to the main panel, which allows the user to
 * manually track servers that don't appear in meta.
 *
 */
#include "StdAfx.h"

#include "Fl_ServerList.h"
#include "controlconst.h"
#include "ServerListCfg.h"
#include "PrivateServerConfig.h"
#include "PrivateServerPanel.h"
#include "PrivateServerList.h"
#include "guiconst.h"
#include "const.h"

xpc_xpcheckbutton XPWhereColumnChecks[] =
{
        {300,   5, 20, 25, "Server Name"},
        {300,  25, 20, 25, "Map Name"},
        {300,  45, 20, 25, "Map Author"},
        {300,  65, 20, 25, "Map Size"},
        {300,  85, 20, 25, "# of players"},
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
void xpwcok_cb(Fl_Widget*, void* v) {
  PrivateServerConfig *w = (PrivateServerConfig *) v;
  int i;

  for (i = 0; i < numServerListColumns; i++)
	  w->psp->fsl->cfg->columns[i] = w->localColumns[i];

//  w->metaHost->Store();
//  w->metaHostAlt->Store();

  w->psp->fsl->SetColumnSizesFromCfg();
  w->psp = NULL;
  w->hide();
}
///////////////////////////////////////////////////////////////////////////////
void xpwccancel_cb(Fl_Widget*, void* v) {
  PrivateServerConfig *w = (PrivateServerConfig *) v;

  w->psp = NULL;
  w->hide();
}
///////////////////////////////////////////////////////////////////////////////
PrivateServerConfig* PrivateServerConfig::Factory()
{
	PrivateServerConfig *window = new PrivateServerConfig(570, 390, "Private Server Config");
	window->end();

	window->show();
	window->make_current();
	return(window);
}
///////////////////////////////////////////////////////////////////////////////
void PrivateServerConfig::SetupLocalColumns()
{
	int i;

	for (i = 0; i < numServerListColumns; i++)
		localColumns[i] = psp->fsl->cfg->columns[i];
}

///////////////////////////////////////////////////////////////////////////////
void PrivateServerConfig::UpdateDisplay()
{
	int i;
	XPCheckButton *tchk = NULL;

	for (i = 0; i< NELEM(XPWhereColumnChecks); i++)
	{
		tchk = XPWhereColumnChecks[i].button;
		tchk->SetBool(&localColumns[i]);
		tchk->Read();
	}
}

///////////////////////////////////////////////////////////////////////////////
PrivateServerConfig::PrivateServerConfig(int w,int h,const char *l)
        : Fl_Window(w,h,l)
{
	int i;
/*    { Fl_Box* o = new Fl_Box(5, 5, 290, 70);
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
*/
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

