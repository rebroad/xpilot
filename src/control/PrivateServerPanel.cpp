/* $Id: PrivateServerPanel.cpp,v 1.7 2004/05/28 18:48:09 dick Exp $
 *
 * PrivateServerPanel - Handle the private server panel.
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
 * $Log: PrivateServerPanel.cpp,v $
 * Revision 1.7  2004/05/28 18:48:09  dick
 * Clear ppEditor before returning from the callback.
 *
 * Revision 1.6  2004/05/22 15:22:18  dick
 * Whitespace
 *
 * Revision 1.5  2004/05/14 21:27:17  dick
 * Attribute renaming
 *
 * Revision 1.4  2004/05/13 09:22:34  dick
 * UpdateTeamButtons() becomes UpdateButtons()
 *
 * Revision 1.3  2004/05/11 06:49:31  dick
 * Edit/Add/Change/Delete the entries in the PrivateServerList.
 *
 * Revision 1.2  2004/05/10 17:06:55  dick
 * Get rid of "type", which was supposed to be a switch between different configs.
 * Instead use different subclasses for the config.
 *
 * Revision 1.1  2004/05/10 12:48:00  dick
 * Add a Private Server button to the main panel, which allows the user to
 * manually track servers that don't appear in meta.
 *
 */

#include "StdAfx.h"

#if defined(_UNIX) || defined(_CYGWIN)
# include <unistd.h>
# include <errno.h>
#else
# include <sys/types.h>
# include <sys/timeb.h>
# include "sockerrs.h"
#endif


#include "FL/Fl_Window.H"
#include "FL/Fl_PNG_Image.H"
#include "FL/Fl_Box.H"
#include "FL/fl_ask.H"

#include "Fl_XPColors.h"
#include "FlEventNames.h"
#include "Fl_ServerList.h"
#include "PrivatePanelEditButton.h"

#include "XPilotControl.h"
#include "controlconst.h"
#include "config.h"
#include "portability.h"
#include "pack.h"
#include "cstring.h"
#include "PrivateServerList.h"
#include "IniXPilot.h"

#include "ServerButtonsGroup.h"
#include "PrivateServerConfig.h"
#include "PrivateServerPanel.h"

#include "PrivatePanelEditor.h"

#define SERVERLISTWIN_TOP		40
#define	SERVERLISTWIN_HEIGHT	(XPWHEREWIN_HEIGHT-SERVERLISTWIN_TOP)

#define	EDITBUTTONMARGIN		3

#define	peHeight	privateEditButtonHeight
#define	peWidth		privateEditButtonWidth
#define	ebMargin	EDITBUTTONMARGIN

///////////////////////////////////////////////////////////////////////////////
PrivateServerPanel::PrivateServerPanel(int x, int y, int w,int h,const char *l)
        : Fl_Double_Window(x,y,w,h,l)
{
	Fl_Box* bigbox = new Fl_Box(FL_DOWN_BOX, 0, 0, w, h, "");
	bigbox->color(xpBackColor);
	color(xpBackColor);

	//Group everything so it all moves together right.
//	Fl_Group *o = new Fl_Group(0, 0, w, h, "");

	fsl = new Fl_ServerList(0, SERVERLISTWIN_TOP,
							w, h-SERVERLISTWIN_TOP-peHeight-ebMargin*2, l);
	PrivateServerList* psl = new PrivateServerList;
	psl->SetIniServerList(&iniXPilot.privateServerList);
	fsl->SetServerList(psl);
	fsl->SetCfg(&iniXPilot.privateSlCfg);
	fsl->ShowPlayers();
	fsl->MarkAllInactive();

	serverButtonsGroup = new ServerButtonsGroup(0, 0, w, h, &PrivateServerPanel::ServerButtonsCallback, this);
	addButton = new PrivatePanelEditButton(0+ebMargin,
									   h-peHeight-ebMargin,
									   peWidth,
									   peHeight,
									   "Add");
	addButton->myPanel = this;

	changeButton = new PrivatePanelEditButton(peWidth+ebMargin*2,
									   h-peHeight-ebMargin,
									   peWidth,
									   peHeight,
									   "Change");
	changeButton->myPanel = this;

	deleteButton = new PrivatePanelEditButton(peWidth*2+ebMargin*3,
									   h-peHeight-ebMargin,
									   peWidth,
									   peHeight,
									   "Delete");
	deleteButton->myPanel = this;

	pscConfig = NULL;
	ppEditor = NULL;

}


///////////////////////////////////////////////////////////////////////////////
void PrivateServerPanel::SetErrMsgHandler(ErrMsgHandler _emh, void* _emhThis)
{
	emh = _emh;
	emhThis = _emhThis;
	fsl->SetErrMsgHandler(emh, emhThis);

}

///////////////////////////////////////////////////////////////////////////////
void PrivateServerPanel::FlushToIni()
{
	PrivateServerList*	sl = (PrivateServerList*) fsl->GetServerList();
	sl->FlushToIni();
}

///////////////////////////////////////////////////////////////////////////////
//static
void PrivateServerPanel::ServerButtonsCallback(int id, void* v)
{
	PrivateServerPanel* psp = (PrivateServerPanel*)v;
	psp->ServerButtonsCallback(id);
}

///////////////////////////////////////////////////////////////////////////////
void PrivateServerPanel::ServerButtonsCallback(int id)
{
	ServerSite	si;

	switch (id)
	{
	case IDM_REFRESH_LIST:
	{
		PrivateServerList*	sl = (PrivateServerList*) fsl->GetServerList();
		serverButtonsGroup->actionButtons[ButtonRefresh]->deactivate();
		Fl::flush();
		sl->Refresh();
		serverButtonsGroup->actionButtons[ButtonRefresh]->activate();
		break;
	}
	case IDM_JOIN_GAME:
	case IDM_JOINTEAM0:
	case IDM_JOINTEAM1:
	case IDM_JOINTEAM2:
	case IDM_JOINTEAM3:
	case IDM_JOINTEAM4:
	case IDM_JOINTEAM5:
	case IDM_JOINTEAM6:
	case IDM_JOINTEAM7:
	case IDM_JOINTEAM8:
	case IDM_JOINTEAM9:
	{
	//		int row = fsl->select_start_row();
		int	team = -1;
		bool b = fsl->GetSelected(si);
		if (b)
		{
			if (id >= IDM_JOINTEAM0 && id <= IDM_JOINTEAM9)
				team = id - IDM_JOINTEAM0;
			String server;
			server = si.serverIP;
			if (server.IsEmpty())
				server = si.serverName;
			PlayGame(server, si.serverPort, team);
		}
		break;
	}
	case IDM_ADMIN_GAME:
	{
		int row = fsl->select_start_row();
		bool b = fsl->GetSelected(si);
		if (b)
		{
			AdminServer(si.serverName, si.serverIP, si.serverPort);
		}
		break;
	}
	case IDM_MEASURE_LAG:
	{
		serverButtonsGroup->actionButtons[ButtonMeasureLag]->deactivate();
		Fl::flush();
		PingServers();
		// SortByLag();
		redraw();
		serverButtonsGroup->actionButtons[ButtonMeasureLag]->activate();
		break;
	}
	case IDM_XPWHERE_CONFIG:
	{
		if (pscConfig == NULL)
			pscConfig = PrivateServerConfig::Factory();
		else
			pscConfig->show();

		pscConfig->psp = this;
		//pscConfig->type = type;
		pscConfig->SetupLocalColumns();
		pscConfig->UpdateDisplay();
		break;
	}
	}
}

///////////////////////////////////////////////////////////////////////////////
void PrivateServerPanel::UpdateButtons()
{
	ServerSite	si;
	bool	f = fsl->GetSelected(si);
	if (f)
		serverButtonsGroup->UpdateButtons(&si);
	else
		serverButtonsGroup->UpdateButtons(NULL);
	if (f)
	{
		changeButton->activate();
		deleteButton->activate();
	}
	else
	{
		changeButton->deactivate();
		deleteButton->deactivate();
	}
}

///////////////////////////////////////////////////////////////////////////////
int PrivateServerPanel::handle(int event)
{
	//D(PCSTR ev = GetFlEventName(event);
	//			printf("PrivateServerPanel::handle: ev=%s\n", ev);)

    switch (event)
	{
	//When the user clicks a server, we get notified here.
    case SELECTSERVER :
	case FL_SHOW:
		 UpdateButtons();
		 return(TRUE);
	case SERVERJOINGAME :
		ServerButtonsCallback(IDM_JOIN_GAME);
		return TRUE;
	case FL_MOVE:
		fsl->handle(event);		// maybe setup resize cursor, ignore return value
	}

    return PrivateServerPanelSUPERCLASS::handle(event);
}

///////////////////////////////////////////////////////////////////////////////
void PrivateServerPanel::ActionButtonHandler(PrivatePanelEditButton* b)
{
	if (b == addButton)
	{
		if (ppEditor)
		{
			ppEditor->show();
			return;
		}
		ppEditor = new PrivatePanelEditor();
		ppEditor->psp = this;
		ppEditor->command = b;
		ppEditor->portInput->value(SERVER_PORT_STR);
		ppEditor->show();
	}
	if (b == changeButton)
	{
		if (ppEditor)
		{
			ppEditor->show();
			return;
		}
		ServerSite	si;
		String		s;
		if (!fsl->GetSelected(si))
			return;
		ppEditor = new PrivatePanelEditor();
		ppEditor->psp = this;
		ppEditor->command = b;
		ppEditor->serverInput->value(si.serverName);
		s = si.serverPort;
		ppEditor->portInput->value(s);
		ppEditor->origName = si.serverName;
		ppEditor->origPort = si.serverPort;
		ppEditor->show();

	}
	if (b == deleteButton)
	{
		ServerSite	si;
		ObjSortable*	os;
		bool	f = fsl->GetSelected(si);
		if (!f)
		{
			emh(emhThis, EmError, "No server selected to delete");
			return;
		}
		ServerList*		sl;
		sl = fsl->GetServerList();
		os = sl->FindSortable(si.serverName, si.serverPort);
		if (!os)
		{
			emh(emhThis, EmError, "Internal Error: can't find selected server");
			return;
		}
		sl->Remove(os);
		delete os->o;
		delete os;
		fsl->ServerListStateChanged("OK");
		Fl::check();
		return;
	}
}

///////////////////////////////////////////////////////////////////////////////
void PrivateServerPanel::PanelEditorCallback(PrivatePanelEditButton* b)
{
	String			name;
	String			s;
	String			ip;
	int				port;
	ServerList*		sl;
	ServerSite*		ss;
	ObjSortable*	os;

	name = ppEditor->serverInput->value();
	s    = ppEditor->portInput->value();
	port = s.atoi();
	sl = fsl->GetServerList();
	ss = sl->Find(name, port);
	if (b == addButton)
	{
		if (!ss)
		{
			ss = sl->Find(ip, port);
			if (!ss)
			{
				if (name.IsEmpty() && ip.IsEmpty())
				{
					emh(emhThis, EmError, "You must select a name or an IP address");
					return;
				}
				if (name.IsEmpty())
					name = ip;
				ss = new ServerSite();
				ss->serverName = name;
				ss->serverIP = ip;
				ss->serverPort = port;
				os = new ObjSortable();
				os->o = ss;
				sl->Add(os);
				fsl->ServerListStateChanged("OK");
				Fl::check();
				ppEditor = NULL;
				return;
			}
		}
		emh(emhThis, EmError, "Duplicate server.  Not added.");
		ppEditor = NULL;
		return;
	}
	if (b == changeButton)
	{
		ss = sl->Find(ppEditor->origName, ppEditor->origPort);
		if (!ss)
		{
			emh(emhThis, EmError, "Error: Can't find original server in list \"%s:%d\"",
								  (PCSTR)ppEditor->origName, ppEditor->origPort);
			return;
		}
		ss->serverName = name;
		ss->serverIP = ip;
		ss->serverPort = port;
		fsl->ServerListStateChanged("OK");
		Fl::check();
		ppEditor = NULL;
		return;
	}
	ppEditor = NULL;
}

///////////////////////////////////////////////////////////////////////////////
void PrivateServerPanel::SortByLag()
{
	ServerList* sl = fsl->GetServerList();
	sl->Sort(COL_LAG);
}

///////////////////////////////////////////////////////////////////////////////
void PrivateServerPanel::PingServers()
{
}

