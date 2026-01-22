/*
 * $Id: LocalServerPanelAdvanced.cpp,v 1.11 2004/06/01 23:13:00 dick Exp $
 *
 * XPilotControl, .  Copyright (C) 2002 by
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
 * $Log: LocalServerPanelAdvanced.cpp,v $
 * Revision 1.11  2004/06/01 23:13:00  dick
 * You can't really make a new button from an object and stick it in a static.
 * (Who owns it if you open 2 MapEditors?)  So now each window keeps track of
 * his own buttons.
 *
 * Revision 1.10  2004/05/22 15:22:18  dick
 * Whitespace
 *
 * Revision 1.9  2004/04/21 23:27:16  dick
 * Handle starting up and shutting down the ScoreServer from the Advanced panel.
 *
 * Revision 1.8  2004/03/03 07:54:35  dick
 * Don't make pixmaps at load time; wait until runtime init, after Strings have
 * actually been inited.
 *
 * Revision 1.7  2004/02/29 00:12:37  dick
 * Draw background in xpBackColor and labels in xpLabelColor.
 *
 * Revision 1.6  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.5  2004/01/27 04:38:43  dick
 * Enable changing map from the Advanced panel
 *
 * Revision 1.4  2004/01/26 17:01:41  dick
 * LocalSimple panel only has Start Server, Play Game, and Change Map.
 * LocalAdvanced also has AdminServer, StartScoreServer, BrowseScoreServer
 * and dummy edit defaults and edit robots buttons.
 *
 * Revision 1.3  2003/12/22 05:59:50  dick
 * Use png(s) instead of xpm(s).
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
#include "FL/Fl_Check_Button.H"

#include "Fl_XPColors.h"

#include "cstring.h"
#include "proto.h"
#include "xpcTypes.h"
#include "controlconst.h"
#include "password.h"
#include "XPilotControl.h"
#include "ScoreServerControlClient.h"
#include "LocalServerPanel.h"
#include "LocalServerPanelWorld.h"

#include "LocalServerPanelAdvanced.h"

#define	ROW(_x)	((BUTTON_SIZE_LG+2)*_x+buttonTop)
#define	WID		(BUTTON_SIZE_LG)
#define	HGT		(BUTTON_SIZE_LG)

#define	BUTT_PLAY				0
#define	BUTT_CHANGEMAP			1
#define	BUTT_ADMIN_SERVER		2
#define	BUTT_START_SCORESERVER	3
#define	BUTT_BROWSE_SCORESERVER	4
#define	BUTT_EDIT_DEFAULTS		5
#define	BUTT_EDIT_ROBOTS		6


const int	buttonTop = 50;
static xpe_stbbutton localServerPanelAdvancedButtonsDefs[numLocalServerPanelAdvancedButtons] =
{
	{0, ROW(0), "&Play Game", "Connect a client to your local server", 
		"xpilot",
		&LocalServerPanelAdvanced::PlayGameCallback},
	{0, ROW(1), "Change &Map", "Load a different world into your server", 
		"map",
		&LocalServerPanelAdvanced::ChangeMapCallback},
	{0, ROW(2), "A&dmin Server", "Change settings of your server", 
		"serveroptions",
		&LocalServerPanelAdvanced::AdminServerCallback},
	{0, ROW(3), "Start S&coreServer", "Start or Stop the ScoreServer",
		"scoreserver",
		&LocalServerPanelAdvanced::StartScoreServerCallback},
	{0, ROW(4), "&Browse ScoreServer", "Launch your browser to configure the ScoreServer\nand check out the scores", 
		"scoreadmin",
		&LocalServerPanelAdvanced::BrowseScoreServerCallback},
	{0, ROW(6), "Edit Server De&faults", "Edit the defaults file", 
		"serveroptions", 
		&LocalServerPanelAdvanced::EditDefaultsCallback},
	{0, ROW(7), "Edit &Robots", "Edit the robots file", 
		"shipeditor",
		&LocalServerPanelAdvanced::EditRobotsCallback},
};

///////////////////////////////////////////////////////////////////////////////
LocalServerPanelAdvanced::LocalServerPanelAdvanced(int x, int y, int w,int h,const char *l)
        : LocalServerPanelAdvancedSUPERCLASS(x,y,w,h,l),
		  sscc(NULL)
{
	int	i;

	begin();
	//Fl_Box* bigbox = new Fl_Box(FL_EMBOSSED_FRAME, x, y, w, h, "");
	//Fl_Box* label = new Fl_Box(x, y, w, 30, "Local Server");
	for (i = 0; i < numLocalServerPanelAdvancedButtons; i++)
    {
		localServerPanelAdvancedButtons[i] = new FunctionButton(
			localServerPanelAdvancedButtonsDefs[i].x+x+4,
			localServerPanelAdvancedButtonsDefs[i].y+y,
			WID, HGT,
			localServerPanelAdvancedButtonsDefs[i].lbl,
			localServerPanelAdvancedButtonsDefs[i].pm);
		localServerPanelAdvancedButtons[i]->align(FL_ALIGN_RIGHT);
		localServerPanelAdvancedButtons[i]->callback(localServerPanelAdvancedButtonsDefs[i].cb);
		localServerPanelAdvancedButtons[i]->tooltip(localServerPanelAdvancedButtonsDefs[i].ttip);
		localServerPanelAdvancedButtons[i]->user_data(this);
		localServerPanelAdvancedButtons[i]->labelcolor(xpLabelColor);
    }

	for (i=0; i<MAX_ADVANCED_WORLDS; i++)
		lspWorld[i] = new LocalServerPanelWorld(x, ROW(2)+(i*LSPW_HEIGHT),
												w, LSPW_HEIGHT);

	end();
	Fl::add_timeout(0.5, &LocalServerPanelAdvanced::FlTimeoutHandler, this);
}

///////////////////////////////////////////////////////////////////////////////
LocalServerPanelAdvanced::~LocalServerPanelAdvanced()
{
	if (sscc)
		delete sscc;
	sscc = NULL;
}

///////////////////////////////////////////////////////////////////////////////
void LocalServerPanelAdvanced::FlTimeoutHandler(void* v)
{
	LocalServerPanelAdvanced* me = (LocalServerPanelAdvanced*)v;
	me->FlTimeoutHandler();
}

///////////////////////////////////////////////////////////////////////////////
void LocalServerPanelAdvanced::FlTimeoutHandler()
{
	DetermineScoreServerRunning();
	UpdateWidgets();
	Fl::repeat_timeout(5.0, FlTimeoutHandler, this);
}


///////////////////////////////////////////////////////////////////////////////
void LocalServerPanelAdvanced::PlayGameCallback(Fl_Widget* w, void* v)
{
	LocalServerPanelAdvanced* lspa = (LocalServerPanelAdvanced*)w->user_data();
	lspa->PlayGameCallback();
}

///////////////////////////////////////////////////////////////////////////////
void LocalServerPanelAdvanced::PlayGameCallback()
{
	lsp->PlayGameCallback(0);
}

///////////////////////////////////////////////////////////////////////////////
void LocalServerPanelAdvanced::ChangeMapCallback(Fl_Widget* w, void* v)
{
	LocalServerPanelAdvanced* lspa = (LocalServerPanelAdvanced*)w->user_data();
	lspa->ChangeMapCallback();
}

///////////////////////////////////////////////////////////////////////////////
void LocalServerPanelAdvanced::ChangeMapCallback()
{
	lsp->ChangeMapCallback(0);
}

///////////////////////////////////////////////////////////////////////////////
void LocalServerPanelAdvanced::AdminServerCallback(Fl_Widget* w, void* v)
{
	LocalServerPanelAdvanced* lspa = (LocalServerPanelAdvanced*)w->user_data();
	lspa->AdminServerCallback();
}

///////////////////////////////////////////////////////////////////////////////
void LocalServerPanelAdvanced::AdminServerCallback()
{
	lsp->AdminServerCallback(0);
}

///////////////////////////////////////////////////////////////////////////////
void LocalServerPanelAdvanced::StartScoreServerCallback(Fl_Widget* w, void* v)
{
	LocalServerPanelAdvanced* lspa = (LocalServerPanelAdvanced*)w->user_data();
	lspa->StartScoreServerCallback();
}


///////////////////////////////////////////////////////////////////////////////
void LocalServerPanelAdvanced::StartScoreServerCallback()
{
	StartScoreServerCallback(0);
}


///////////////////////////////////////////////////////////////////////////////
void LocalServerPanelAdvanced::StartScoreServerCallback(int /*world*/)
{
	if (IsScoreServerRunning())
	{
		sscc->SendShutdown();
		Fl::add_timeout(3.0, &LocalServerPanelAdvanced::ScoreServerExitedTimeout, this);
	}
	else
	{
		StartScoreServer();
	}
}

///////////////////////////////////////////////////////////////////////////////
void LocalServerPanelAdvanced::ScoreServerExitedTimeout(void* v)
{
	LocalServerPanelAdvanced* lspa = (LocalServerPanelAdvanced*)v;
	lspa->ScoreServerExitedTimeout();
}

///////////////////////////////////////////////////////////////////////////////
void LocalServerPanelAdvanced::ScoreServerExitedTimeout()
{
	if (sscc)
	{
		delete sscc;
		sscc = NULL;
	}
	DetermineScoreServerRunning();
	UpdateWidgets();
}


///////////////////////////////////////////////////////////////////////////////
void LocalServerPanelAdvanced::BrowseScoreServerCallback(Fl_Widget* w, void* v)
{
	LocalServerPanelAdvanced* lspa = (LocalServerPanelAdvanced*)w->user_data();
	lspa->BrowseScoreServerCallback();
}

///////////////////////////////////////////////////////////////////////////////
void LocalServerPanelAdvanced::BrowseScoreServerCallback()
{
	lsp->BrowseScoreServerCallback(0);
}

///////////////////////////////////////////////////////////////////////////////
void LocalServerPanelAdvanced::EditDefaultsCallback(Fl_Widget* w, void* v)
{
}

///////////////////////////////////////////////////////////////////////////////
void LocalServerPanelAdvanced::EditRobotsCallback(Fl_Widget* w, void* v)
{
}

///////////////////////////////////////////////////////////////////////////////
bool LocalServerPanelAdvanced::IsScoreServerRunning()
{
	if (!sscc)
		return(false);
	return(sscc->IsConnected());
}

///////////////////////////////////////////////////////////////////////////////
void LocalServerPanelAdvanced::DetermineScoreServerRunning()
{
	if (!sscc)
	{
		Sock	s;
		// if we can open a port on 15346, then the server must not be there
		if (s.OpenUdp(localAddr, SCORE_SERVER_PORT) == SOCK_IS_OK)
		{
			s.Close();
			return;
		}
		sscc = new ScoreServerControlClient;
		sscc->SetPanel(this);
		sscc->SetFirewallPortList(normalFirewall);
		sscc->SetErrMsgHandler(emh, emhThis);
		if (!sscc->Connect(localAddr, SCORE_SERVER_PORT, GetPassword(localAddr, SCORE_SERVER_PORT)))
		{
			delete sscc;
			sscc = NULL;
		}
		else
		{
			sscc->FlSetup();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
void LocalServerPanelAdvanced::UpdateWidgets(bool forceUpdate)
{
	if (lsp->IsServerRunning())
	{
		localServerPanelAdvancedButtons[BUTT_PLAY]->activate();
		localServerPanelAdvancedButtons[BUTT_CHANGEMAP]->activate();
		localServerPanelAdvancedButtons[BUTT_ADMIN_SERVER]->activate();
//		for (int i=0; i<MAX_WORLDS; i++)
//			if (lspWorld[i])
//				lspWorld[i]->activate();
	}
	else
	{
		localServerPanelAdvancedButtons[BUTT_PLAY]->deactivate();
//		localPanelActionButtons[BUTT_CHANGEMAP]->deactivate();
		localServerPanelAdvancedButtons[BUTT_ADMIN_SERVER]->deactivate();
//		for (int i=0; i<MAX_WORLDS; i++)
//			if (lspWorld[i])
//				lspWorld[i]->deactivate();
	}

	// Not available yet
	localServerPanelAdvancedButtons[BUTT_EDIT_DEFAULTS]->deactivate();
	localServerPanelAdvancedButtons[BUTT_EDIT_ROBOTS]->deactivate();


	if (IsScoreServerRunning() && (!scoreServerWasRunning || forceUpdate))
	{
		forceUpdate = !scoreServerWasRunning ? true : forceUpdate;
		scoreServerWasRunning = true;
		localServerPanelAdvancedButtons[BUTT_START_SCORESERVER]->label("&Stop ScoreServer");
		// The label text doesn't change from "Stop Server" to "Start Server"
		// until after an event, like changing focus.
		// This tries to help along redrawing the text (it seems to work)
		localServerPanelAdvancedButtons[BUTT_START_SCORESERVER]->deactivate();
		localServerPanelAdvancedButtons[BUTT_START_SCORESERVER]->activate();
		Fl::flush();
//		ShowSelectedPanel(forceUpdate);
	}
	if (!IsScoreServerRunning() && (scoreServerWasRunning || forceUpdate))
	{
		forceUpdate = scoreServerWasRunning ? true : forceUpdate;
		scoreServerWasRunning = false;
		localServerPanelAdvancedButtons[BUTT_START_SCORESERVER]->label("&Start ScoreServer");
//		EmptyWorldAttributes(false);
		// The label text doesn't change from "Stop Server" to "Start Server"
		// until after an event, like changing focus.
		// This tries to help along redrawing the text (it seems to work)
		localServerPanelAdvancedButtons[BUTT_START_SCORESERVER]->deactivate();
		localServerPanelAdvancedButtons[BUTT_START_SCORESERVER]->activate();
		Fl::flush();
//		ShowSelectedPanel(forceUpdate);
	}
}




