/*
 * $Id: LocalServerPanelSimple.cpp,v 1.12 2004/06/01 23:13:00 dick Exp $
 *
 * The "Simple" local server configuration panel
 * i.e. newbie mode = ON
 *
 * XPilot.  Copyright (C) 2002 by
 *
 *      The XPilot Authors   <xpilot@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
 *      Jarrod Miller        <jarrod@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
 *      Bj�rn Stabell        <bjoern@xpilot.org>
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
 * $Log: LocalServerPanelSimple.cpp,v $
 * Revision 1.12  2004/06/01 23:13:00  dick
 * You can't really make a new button from an object and stick it in a static.
 * (Who owns it if you open 2 MapEditors?)  So now each window keeps track of
 * his own buttons.
 *
 * Revision 1.11  2004/03/03 07:54:35  dick
 * Don't make pixmaps at load time; wait until runtime init, after Strings have
 * actually been inited.
 *
 * Revision 1.10  2004/02/29 00:12:37  dick
 * Draw background in xpBackColor and labels in xpLabelColor.
 *
 * Revision 1.9  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.8  2004/01/26 17:01:41  dick
 * LocalSimple panel only has Start Server, Play Game, and Change Map.
 * LocalAdvanced also has AdminServer, StartScoreServer, BrowseScoreServer
 * and dummy edit defaults and edit robots buttons.
 *
 * Revision 1.7  2004/01/15 06:05:37  dick
 * Enable/disable scoreserver button based on the server running
 *
 * Revision 1.6  2004/01/15 05:38:25  dick
 * Add a StartScoreServer button to the local panel
 *
 * Revision 1.5  2003/12/22 05:59:50  dick
 * Use png(s) instead of xpm(s).
 *
 * Revision 1.4  2002/09/16 01:19:58  dick
 * TEMP: Allow changing the map anytime.
 *
 * Revision 1.3  2002/09/12 20:02:03  dick
 * Handle selecting and changing the map that the server will use.
 *
 * Revision 1.2  2002/09/09 23:50:55  dick
 * Route commands (button presses) back to the LocalServerPanel for handling.
 *
 * Revision 1.1  2002/09/07 17:52:00  dick
 * Skeleton support for the Simple/Advanced Local server.
 *
 */
#include "StdAfx.h"

#include "FL/Fl_Group.H"
#include "FL/Fl_Box.H"
#include "FL/Fl_Output.H"
#include "FL/Fl_PNG_Image.H"
//#include "FL/fl_draw.H"

#include "Fl_XPColors.h"

#include "cstring.h"
#include "proto.h"
#include "xpcTypes.h"
#include "controlconst.h"

#include "LocalServerPanel.h"
#include "LocalServerPanelSimple.h"


#define	ROW(_x)	((BUTTON_SIZE_LG+2)*_x+buttonTop)
#define	WID		(BUTTON_SIZE_LG)
#define	HGT		(BUTTON_SIZE_LG)

const int	buttonTop = 50;

#define	BUTT_PLAYGAME	0
#define	BUTT_CHANGEMAP	1

static xpe_stbbutton localServerPanelSimpleButtonsDefs[numLocalServerPanelSimpleButtons] =
{
	{0, ROW(0), "&Play Game", "Connect a client to your local server",
		"xpilot",
		&LocalServerPanelSimple::PlayGameCallback},
	{0, ROW(1), "Change &Map", "Load a different world into your server",
		"map",
		&LocalServerPanelSimple::ChangeMapCallback},
};

///////////////////////////////////////////////////////////////////////////////
LocalServerPanelSimple::LocalServerPanelSimple(int x, int y, int w,int h,const char *l)
        : LocalServerPanelSimpleSUPERCLASS(x,y,w,h,l)
{
	int	i;
	begin();
	for (i = 0; i < numLocalServerPanelSimpleButtons; i++)
    {
		localServerPanelSimpleButtons[i] = new FunctionButton(
			localServerPanelSimpleButtonsDefs[i].x+x+4,
			localServerPanelSimpleButtonsDefs[i].y+y,
			WID, HGT,
			localServerPanelSimpleButtonsDefs[i].lbl,
			localServerPanelSimpleButtonsDefs[i].pm);
		localServerPanelSimpleButtons[i]->align(FL_ALIGN_RIGHT);
		localServerPanelSimpleButtons[i]->callback(localServerPanelSimpleButtonsDefs[i].cb);
		localServerPanelSimpleButtons[i]->tooltip(localServerPanelSimpleButtonsDefs[i].ttip);
		localServerPanelSimpleButtons[i]->user_data(this);
		localServerPanelSimpleButtons[i]->labelcolor(xpLabelColor);
    }
	mapName = new Fl_Output(x, y, 200, 20);
	end();
}

///////////////////////////////////////////////////////////////////////////////
void LocalServerPanelSimple::PlayGameCallback(Fl_Widget* w, void* v)
{
	LocalServerPanelSimple* lsps = (LocalServerPanelSimple*)w->user_data();
	lsps->PlayGameCallback();
}

///////////////////////////////////////////////////////////////////////////////
void LocalServerPanelSimple::PlayGameCallback()
{
	lsp->PlayGameCallback(0);
}

///////////////////////////////////////////////////////////////////////////////
void LocalServerPanelSimple::ChangeMapCallback(Fl_Widget* w, void* v)
{
	LocalServerPanelSimple* lsps = (LocalServerPanelSimple*)w->user_data();
	lsps->ChangeMapCallback();
}

///////////////////////////////////////////////////////////////////////////////
void LocalServerPanelSimple::ChangeMapCallback()
{
	lsp->ChangeMapCallback(0);
}

///////////////////////////////////////////////////////////////////////////////
void LocalServerPanelSimple::UpdateWidgets()
{
	if (lsp->IsServerRunning())
	{
		localServerPanelSimpleButtons[BUTT_PLAYGAME]->activate();
		mapName->activate();
	}
	else
	{
		localServerPanelSimpleButtons[BUTT_PLAYGAME]->deactivate();
		mapName->deactivate();
	}
}
