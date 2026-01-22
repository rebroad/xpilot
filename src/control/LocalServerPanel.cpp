/*
 * $Id: LocalServerPanel.cpp,v 1.29 2004/06/01 23:13:00 dick Exp $
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
 * $Log: LocalServerPanel.cpp,v $
 * Revision 1.29  2004/06/01 23:13:00  dick
 * You can't really make a new button from an object and stick it in a static.
 * (Who owns it if you open 2 MapEditors?)  So now each window keeps track of
 * his own buttons.
 *
 * Revision 1.28  2004/05/22 15:21:44  dick
 * Only display the 'start and stop' message if the server is running
 *
 * Revision 1.27  2004/04/21 23:27:16  dick
 * Handle starting up and shutting down the ScoreServer from the Advanced panel.
 *
 * Revision 1.26  2004/03/03 07:54:35  dick
 * Don't make pixmaps at load time; wait until runtime init, after Strings have
 * actually been inited.
 *
 * Revision 1.25  2004/02/29 00:12:37  dick
 * Draw background in xpBackColor and labels in xpLabelColor.
 *
 * Revision 1.24  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.23  2004/02/01 15:51:48  dick
 * Fix tooltips.  LocalServerPanel caused the widgets to be updated every
 * two seconds (even when this window is hidden), which resets the tooltips timer.
 * Use forceUpdate in combination with current settings to determine whether
 * to actually alter the widgets.  (forceUpdate is needed for initial settings)
 * I don't really like the forceUpdate technique, it's a runaway stack waiting
 * to happen, but it works.
 *
 * Revision 1.22  2004/01/28 00:10:56  dick
 * PrefTab Server becomes TabServer
 *
 * Revision 1.21  2004/01/26 17:01:41  dick
 * LocalSimple panel only has Start Server, Play Game, and Change Map.
 * LocalAdvanced also has AdminServer, StartScoreServer, BrowseScoreServer
 * and dummy edit defaults and edit robots buttons.
 *
 * Revision 1.20  2004/01/19 07:37:07  dick
 * Linux fixes
 *
 * Revision 1.19  2004/01/19 06:52:57  dick
 * Use the new XPFileChooser
 *
 * Revision 1.18  2004/01/15 05:38:25  dick
 * Add a StartScoreServer button to the local panel
 *
 * Revision 1.17  2003/12/22 05:59:50  dick
 * Use png(s) instead of xpm(s).
 *
 * Revision 1.16  2002/09/18 14:49:22  jlmiller
 * Changes necessary for Main window to be resizable, and tiled.
 *
 * Revision 1.15  2002/09/12 20:02:03  dick
 * Handle selecting and changing the map that the server will use.
 *
 * Revision 1.14  2002/09/10 06:01:59  dick
 * Only show one panel during construction
 *
 * Revision 1.13  2002/09/10 05:26:18  dick
 * Use address of func
 *
 * Revision 1.12  2002/09/10 05:16:50  dick
 * Use a timer to heartbeat the connection to the Local Server.
 *
 * Revision 1.11  2002/09/09 23:52:51  dick
 * Create and use a LocalServerControlClient to talk to the server (and to determine
 * if the server is running).
 *
 * Revision 1.10  2002/09/07 18:31:03  dick
 * #undef index because it confuses Fl_Input.
 *
 * Revision 1.9  2002/09/07 17:52:00  dick
 * Skeleton support for the Simple/Advanced Local server.
 *
 * Revision 1.8  2002/08/28 15:29:59  jlmiller
 * make a simple version of the xpe_tbbutton struct, for panel usages.
 * These different button types should probably be consolidated at some point.
 *
 * Revision 1.7  2002/08/20 10:47:29  jlmiller
 * File & Class Renamings:
 * ViewWindow -> MapEditWindow
 * XpInput -> XPInput
 * xpeTypes -> xpcTypes
 *
 * Revision 1.6  2002/07/28 20:18:50  dick
 * Display a pretty graphic in the panel at startup
 *
 * Revision 1.5  2002/07/23 19:58:24  jlmiller
 * No, actually the #include "" was right the first time, so lets put it back.
 *
 * Revision 1.4  2002/07/23 18:23:28  dick
 * Use password.h not Password.h
 *
 * Revision 1.3  2002/07/23 18:06:16  dick
 * The button callbacks become member functions.  Disable unused server buttons
 * for now.
 *
 * Revision 1.2  2002/07/23 13:24:12  jlmiller
 * #includes should be <> instead of ""
 *
 * Revision 1.1  2002/07/19 07:18:33  dick
 * The panel for the Local Server on the main window.
 *
 */
#include "StdAfx.h"

#ifndef	_WINDOWS
//#include <unistd.h>
#endif

#include "FL/Fl_Group.H"
#include "FL/Fl_Box.H"
#include "FL/Fl_PNG_Image.H"

#include "Fl_XPColors.h"

#include "cstring.h"
#include "controlconst.h"
#include "proto.h"
#include "types.h"
#include "password.h"
#include "xpcTypes.h"
#include "IniXPilot.h"
#include "IniServer.h"
#include "LocalServerControlClient.h"
#include "LocalServerPanelWorld.h"
#include "LocalServerPanelSimple.h"
#include "LocalServerPanelAdvanced.h"
#include "LocalServerPanel.h"
#include "ServerOptionsNetClientFl.h"

// On Linux, 2.95.3 (glibc 2.2.3), somewhere, string.h defines index as a macro:
// #define index(s,c) (strchr((s),(c)))
// why this confuses Fl_Input from only this module i don't know
#undef	index

#include "XPFileChooser.h"
#include "ServerPrefsWindow.h"
#include "ServerOptionsNetClientFl.h"
#include "XPilotControl.h"


const int	buttonTop = 50;
#define	ROW(_x)	((BUTTON_SIZE_LG+2)*_x+buttonTop)
#define	WID		(BUTTON_SIZE_LG)
#define	HGT		(BUTTON_SIZE_LG)

#define	BUTT_STOPSERVER	0
xpe_stbbutton localServerPanelButtonsDefs[numLocalServerPanelButtons] =
{
	{0, ROW(0), "&Stop Server", "Start or stop the server",
		"xpilots",
		&LocalServerPanel::StartServerCallback},
//	{0, ROW(3), "Play Game", "Connect a client to your local server",
//		new Fl_XPM_Image(GetButtonFile("xpilot")), StartServerCallback},
//	{0, ROW(4), "Change Map", "Load a different world into your server",
//		new Fl_XPM_Image(GetButtonFile("map")), StartServerCallback},
//	{0, ROW(5), "Admin Server", "Change settings of your server",
//		new Fl_XPM_Image(GetButtonFile("serveroptions")),
//		&LocalServerPanel::AdminServerCallback},
//	{0, ROW(2), "Start ScoreServer", "Join Game",
//		new Fl_XPM_Image(GetButtonFile("scoreserver")), StartScoreCallback},
//	{0, ROW(3), "Admin ScoreServer", "Change the settings of the ScoreServer",
//		new Fl_XPM_Image(GetButtonFile("scoreadmin")), AdminScoreCallback},
};

///////////////////////////////////////////////////////////////////////////////
LocalServerPanel::LocalServerPanel(int x, int y, int w,int h,const char *l)
        : LocalServerPanelSUPERCLASS(x,y,w,h,l),
		  lscc(NULL)
{
	int	i;
	begin();
	Fl_Box* bigbox = new Fl_Box(FL_DOWN_BOX, x, y,
								   w, h, "");
	bigbox->color(xpBackColor);

	//Group so the buttons don't resize.
	Fl_Group *o = new Fl_Group(x, y, w, h, "");
	Fl_Box* label = new Fl_Box(x, y, w, 30, "Local Server");
	label->labelcolor(xpLabelColor);

	for (i = 0; i < numLocalServerPanelButtons; i++)
    {
		localServerPanelButtons[i] = new FunctionButton(
			localServerPanelButtonsDefs[i].x+x+9, localServerPanelButtonsDefs[i].y+y,
			WID, HGT,
			localServerPanelButtonsDefs[i].lbl, localServerPanelButtonsDefs[i].pm);
		localServerPanelButtons[i]->align(FL_ALIGN_RIGHT);
		localServerPanelButtons[i]->callback(localServerPanelButtonsDefs[i].cb);
		localServerPanelButtons[i]->tooltip(localServerPanelButtonsDefs[i].ttip);
		localServerPanelButtons[i]->user_data(this);
		localServerPanelButtons[i]->labelcolor(xpLabelColor);
    }

	advancedCheck = new Fl_Check_Button(x+10, h-20, 250, 20, "&Advanced Server Configuration");
	advancedCheck->user_data(this);
	advancedCheck->callback(AdvancedCheckCallback);
	advancedCheck->value(iniXPilot.localServerAdvanced);
	advancedCheck->labelcolor(xpLabelColor);

	const int subpanelX = x+5;
	const int subpanelY = ROW(2);
	const int subpanelW = w-10;;
	const int subpanelH = h-ROW(2)-20;

	lspSimple   = new LocalServerPanelSimple(  subpanelX, subpanelY, subpanelW, subpanelH);
	lspSimple->SetLocalServerPanel(this);

	lspAdvanced = new LocalServerPanelAdvanced(subpanelX, subpanelY, subpanelW, subpanelH);
	lspAdvanced->SetLocalServerPanel(this);

	o->end();
	o->resizable(NULL);
	end();

	serverWasRunning = true;
	advancedShown = false;

	ShowSelectedPanel(true);
	Fl::add_timeout(0.5, &LocalServerPanel::FlTimeoutHandler, this);
}

///////////////////////////////////////////////////////////////////////////////
LocalServerPanel::~LocalServerPanel()
{
	if (lscc)
		delete lscc;
	lscc = NULL;
}

///////////////////////////////////////////////////////////////////////////////
void LocalServerPanel::SetErrMsgHandler(ErrMsgHandler _emh, void* _emhThis)
{
	emh = _emh; emhThis = _emhThis;
	lspAdvanced->SetErrMsgHandler(emh, emhThis);
}

///////////////////////////////////////////////////////////////////////////////
void LocalServerPanel::FlTimeoutHandler(void* v)
{
	LocalServerPanel* me = (LocalServerPanel*)v;
	me->FlTimeoutHandler();
}

///////////////////////////////////////////////////////////////////////////////
void LocalServerPanel::FlTimeoutHandler()
{
	DetermineServerRunning();
	UpdateWidgets();
	Fl::repeat_timeout(2.0, FlTimeoutHandler, this);
}

///////////////////////////////////////////////////////////////////////////////
void LocalServerPanel::UpdateWidgets(bool forceUpdate)
{
	if (IsServerRunning() && (!serverWasRunning || forceUpdate))
	{
		forceUpdate = !serverWasRunning ? true : forceUpdate;
		serverWasRunning = true;
		localServerPanelButtons[BUTT_STOPSERVER]->label("&Stop Server");
		// The label text doesn't change from "Stop Server" to "Start Server"
		// until after an event, like changing focus.
		// This tries to help along redrawing the text (it seems to work)
		localServerPanelButtons[BUTT_STOPSERVER]->deactivate();
		localServerPanelButtons[BUTT_STOPSERVER]->activate();
		Fl::flush();
		ShowSelectedPanel(forceUpdate);
	}
	if (!IsServerRunning() && (serverWasRunning || forceUpdate))
	{
		forceUpdate = serverWasRunning ? true : forceUpdate;
		serverWasRunning = false;
		localServerPanelButtons[BUTT_STOPSERVER]->label("&Start Server");
		EmptyWorldAttributes(false);
		// The label text doesn't change from "Stop Server" to "Start Server"
		// until after an event, like changing focus.
		// This tries to help along redrawing the text (it seems to work)
		localServerPanelButtons[BUTT_STOPSERVER]->deactivate();
		localServerPanelButtons[BUTT_STOPSERVER]->activate();
		Fl::flush();
		ShowSelectedPanel(forceUpdate);
	}

}

///////////////////////////////////////////////////////////////////////////////
void LocalServerPanel::ShowSelectedPanel(bool forceUpdate)
{
	if (iniXPilot.localServerAdvanced && (!advancedShown || forceUpdate))
	{
		lspSimple->hide();
		lspAdvanced->show();
		lspAdvanced->UpdateWidgets();
		advancedShown = true;
	}
	if (!iniXPilot.localServerAdvanced && (advancedShown || forceUpdate))
	{
		lspSimple->show();
		lspAdvanced->hide();
		lspSimple->UpdateWidgets();
		advancedShown = false;
	}
}

///////////////////////////////////////////////////////////////////////////////
void LocalServerPanel::EmptyWorldAttributes(bool forceUpdate)
{
	int		i;

	for (i=0; i<MAX_ADVANCED_WORLDS; i++)
	{
		UpdateWorldInfo(i, "", forceUpdate);
	}
}

///////////////////////////////////////////////////////////////////////////////
void LocalServerPanel::UpdateWorldInfo(int worldIndex, PCSTR mapName, bool forceUpdate)
{
	if (worldIndex == 0)
	{
		lspSimple->mapName->value(mapName);
	}
	lspAdvanced->lspWorld[worldIndex]->mapName->value(mapName);
	UpdateWidgets(forceUpdate);
}

///////////////////////////////////////////////////////////////////////////////
void LocalServerPanel::StartServerCallback(Fl_Widget* w, void* v)
{
	LocalServerPanel* lsp = (LocalServerPanel*)w->user_data();
	lsp->StartServerCallback();
}

///////////////////////////////////////////////////////////////////////////////
void LocalServerPanel::StartServerCallback()
{
	if (IsServerRunning())
	{
		lscc->SendShutdown();
		Fl::add_timeout(3.0, &LocalServerPanel::ServerExitedTimeout, this);
	}
	else
	{
		StartLocalServer();
	}
}

///////////////////////////////////////////////////////////////////////////////
void LocalServerPanel::ServerExitedTimeout(void* v)
{
	LocalServerPanel* lsp = (LocalServerPanel*)v;
	lsp->ServerExitedTimeout();
}

///////////////////////////////////////////////////////////////////////////////
void LocalServerPanel::ServerExitedTimeout()
{
	if (lscc)
	{
		delete lscc;
		lscc = NULL;
	}
	DetermineServerRunning();
	UpdateWidgets();
}

///////////////////////////////////////////////////////////////////////////////
void LocalServerPanel::AdvancedCheckCallback(Fl_Widget* w, void* v)
{
	LocalServerPanel* lsp = (LocalServerPanel*)w->user_data();
	lsp->AdvancedCheckCallback();
}

///////////////////////////////////////////////////////////////////////////////
void LocalServerPanel::AdvancedCheckCallback()
{
	iniXPilot.localServerAdvanced = advancedCheck->value() ? true : false;
	ShowSelectedPanel();
}

///////////////////////////////////////////////////////////////////////////////
void LocalServerPanel::AdminServerCallback(int world)
{
	// DIK: need to look up port from ini
	AdminServer(s_localhost, localAddr, SERVER_PORT+world);

}

///////////////////////////////////////////////////////////////////////////////
void LocalServerPanel::BrowseScoreServerCallback(int /*world*/)
{
	BrowseScoreServer();
}

///////////////////////////////////////////////////////////////////////////////
void LocalServerPanel::PlayGameCallback(int world)
{
	// DIK: need to look up port from ini
	PlayGame(localAddr, SERVER_PORT+world);

}

///////////////////////////////////////////////////////////////////////////////
void LocalServerPanel::ChangeMapCallback(int world)
{
	XPFileChooser*	openmap;
	PCSTR		p = NULL;
	PCSTR		filt = NULL;

	Fl::scheme(NULL);
	openmap = new XPFileChooser(Conf_mapdir(), "XPilot Map File (*.xp)\t", XPFileChooser::SINGLE, "Open");
	openmap->show();
	while (openmap->visible())
		Fl::wait();

	if (openmap->count() == 0)
		return;

	p = openmap->value();
	if (!p)
		return;
	filt = openmap->filter();
	String s(p);
	s = s.GetFileName();
	iniServer.world0 = s;
	iniServer.SaveAll();

	if (IsServerRunning())
		emh(emhThis, EmInfo, "You must stop and start the server for the changes to take effect");

}

///////////////////////////////////////////////////////////////////////////////
bool LocalServerPanel::IsServerRunning()
{
	if (!lscc)
		return(false);
	return(lscc->IsConnected());
}

///////////////////////////////////////////////////////////////////////////////
void LocalServerPanel::DetermineServerRunning()
{
	if (!lscc)
	{
		Sock	s;
		// if we can open a port on 15345, then the server must not be there
		if (s.OpenUdp(localAddr, SERVER_PORT) == SOCK_IS_OK)
		{
			s.Close();
			return;
		}
		lscc = new LocalServerControlClient;
		lscc->SetPanel(this);
		lscc->SetFirewallPortList(normalFirewall);
		lscc->SetErrMsgHandler(emh, emhThis);
		if (!lscc->Connect(s_localhost, 15345, GetPassword(s_localhost, 15345)))
		{
			delete lscc;
			lscc = NULL;
		}
		else
		{
			lscc->FlSetup();
		}
	}
}
