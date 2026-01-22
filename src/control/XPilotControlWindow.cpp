/* $Id: XPilotControlWindow.cpp,v 1.45 2004/05/28 18:14:04 dick Exp $
 *
 * XPilotControlWindow - Handle the main window for XPilot5
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
 * $Log: XPilotControlWindow.cpp,v $
 * Revision 1.45  2004/05/28 18:14:04  dick
 * Pass around the pointer to the NameAndShipWindow so that when ClientConfig
 * changes it, it gets notified.
 *
 * Revision 1.44  2004/05/22 15:20:15  dick
 * Don't add() the panel when it exists; just expose it
 *
 * Revision 1.43  2004/05/13 09:18:39  dick
 * XPWhereWindow becomes InetServerPanel
 *
 * Revision 1.42  2004/05/11 06:47:09  dick
 * Flush the PrivateServerPanel's settings back to the ini when finished
 *
 * Revision 1.41  2004/05/10 17:06:55  dick
 * Get rid of "type", which was supposed to be a switch between different configs.
 * Instead use different subclasses for the config.
 *
 * Revision 1.40  2004/05/10 12:48:00  dick
 * Add a Private Server button to the main panel, which allows the user to
 * manually track servers that don't appear in meta.
 *
 * Revision 1.39  2004/03/08 00:28:45  dick
 * Fix deleting the unused splashpanel
 *
 * Revision 1.38  2004/03/04 20:20:32  dick
 * Temp don't delete unix splash panel
 *
 * Revision 1.37  2004/03/04 05:38:17  dick
 * Free up the splash panel's memory after it's no longer displayed.
 *
 * Revision 1.36  2004/02/29 00:12:37  dick
 * Draw background in xpBackColor and labels in xpLabelColor.
 *
 * Revision 1.35  2004/02/28 07:40:56  dick
 * Draw a cosmic background png
 *
 * Revision 1.34  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.33  2004/01/22 04:49:01  dick
 * Uh-oh, another reorg.  The product is XPilot5.  The name of the control
 * program is also XPilot5.  No more shalt thou speak of XPilot++.
 *
 * Revision 1.32  2004/01/12 05:24:40  dick
 * Put "XPilot Control" in the titlebar of the window, less ambiguous.
 *
 * Revision 1.31  2004/01/06 04:48:28  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 * Revision 1.30  2002/09/18 14:49:22  jlmiller
 * Changes necessary for Main window to be resizable, and tiled.
 *
 * Revision 1.29  2002/09/09 23:47:31  dick
 * Set the localServerPanel Error handler.
 *
 * Revision 1.28  2002/09/07 17:53:03  dick
 * Add kb shortcuts to the button labels.
 *
 * Revision 1.27  2002/08/28 15:30:38  jlmiller
 * Make EditorsPanel a separate class, similar to LocalServerPanel.
 *
 * Revision 1.26  2002/08/28 13:42:51  dick
 * Use ErrMsgHandler to output error messages.
 *
 * Revision 1.25  2002/08/22 12:28:40  dick
 * Get rid of flicker by using Fl_Double_Window
 *
 * Revision 1.24  2002/08/20 20:06:51  jlmiller
 * Fix non repainting window bug.
 *
 * Revision 1.23  2002/08/16 18:56:03  jlmiller
 * First pass as configurable XPWhereWindows. Very incomplete still.
 *
 * Revision 1.22  2002/08/02 13:59:21  jlmiller
 * Create InetServerLists instead of ServerList.
 *
 * Revision 1.21  2002/08/01 14:57:55  dick
 * Display the program name as "XPilot"
 *
 * Revision 1.20  2002/07/28 20:18:50  dick
 * Display a pretty graphic in the panel at startup
 *
 * Revision 1.19  2002/07/27 02:21:36  dick
 * Temp disable lan list
 *
 * Revision 1.18  2002/07/23 19:58:24  jlmiller
 * No, actually the #include "" was right the first time, so lets put it back.
 *
 * Revision 1.17  2002/07/23 13:24:12  jlmiller
 * #includes should be <> instead of ""
 *
 * Revision 1.16  2002/07/19 14:06:18  jlmiller
 * Apparently we have to add() the editors group.
 *
 * Revision 1.15  2002/07/19 07:23:59  dick
 * Add a pane for the Local Server button.
 *
 * Revision 1.14  2002/07/17 18:28:27  jlmiller
 * Refresh ServerList if it already exists when switching tabs.
 *
 * Revision 1.13  2002/07/15 19:13:23  jlmiller
 * Provide a (very) basic ClientConfig window.
 *
 * Revision 1.12  2002/07/14 23:04:36  jlmiller
 * Move status line to main window. Don't create ServerList's till
 * we need them.
 *
 * Revision 1.11  2002/07/12 20:02:43  jlmiller
 * Provide buttons for Map and Ship editors
 *
 * Revision 1.10  2002/07/12 15:00:06  jlmiller
 * Integrate tabbed main window, with clean up of globals.
 *
 * Revision 1.9  2002/07/12 02:38:04  dick
 * Clicking the "Local Server" button opens a ServerPrefsWindow pointing at 127.0.0.1
 *
 * Revision 1.8  2002/07/10 04:09:41  dick
 * Add an icon on the left side for "Editors".  Some icons for the other buttons.
 *
 * Revision 1.7  2002/07/08 06:29:35  dick
 * Add buttons for looking up a local server, and for calling the KbConfig tool.
 *
 * Revision 1.6  2002/06/25 20:20:10  jlmiller
 * Massive restructure to clean up includes, and objectify code more.
 *
 * Revision 1.5  2002/06/03 02:58:20  jlmiller
 * Make XPWhere (ServerList window) its own class
 *
 * Revision 1.4  2002/05/29 20:32:06  jlmiller
 * ServerList Widget can join or admin anywhere.
 *
 * Revision 1.3  2002/05/28 07:19:51  dick
 * Rename xpilotedit to XPilotControl
 */
#include "StdAfx.h"

#include "FL/Fl_Tile.H"
#include "FL/Fl_Box.H"
#include "FL/Fl_XPM_Image.H"
#include "FL/Fl_PNG_Image.H"

#include "Fl_XPColors.h"
#include "Fl_ServerList.h"

#include "InetServerList.h"
#include "PrivateServerList.h"
#include "IniXPilot.h"

#include "XPilotControlWindow.h"
#include "ServerPrefsWindow.h"
#include "controlconst.h"
#include "InetServerPanel.h"
#include "MainButtonsPanel.h"
#include "LocalServerPanel.h"
#include "PrivateServerPanel.h"
#include "EditorsPanel.h"
#include "SplashPanel.h"
#include "proto.h"
#include "xpcTypes.h"



#define BUTTONWIDTH 32

#define	WIDGET_INSET	5
#define	STATUS_TOP		(MAINWIN_HEIGHT-WIDGET_INSET-STATUS_HEIGHT)



///////////////////////////////////////////////////////////////////////////////
int SetupMainWin(int argc, char **argv)
{
/* Bungled attempt to make main window size persistent.
#if defined(_WINDOWS) && !defined(_CYGWIN)
	RECT	rect;
	int		x, y, cx, cy;
	if (!iniXPilot.GetWindowPositions(&rect))
	{
		x  = CW_USEDEFAULT;
		cx = MAINWIN_WIDTH;
		y  = CW_USEDEFAULT;
		cy = MAINWIN_HEIGHT;
	}
	else
	{
		x = rect.left;
		cx = rect.right - rect.left;
		y = rect.top;
		cy = rect.bottom - rect.top;
	}
#endif
*/
	InitFlXPColors();
	XPilotControlWindow window(MAINWIN_WIDTH,MAINWIN_HEIGHT, NULL);
//	XPilotControlWindow window(cx, cy, NULL);
	window.color(xpBackColor);

#if defined(_WINDOWS) && !defined(_CYGWIN)
	window.icon((char*)LoadIcon(fl_display, MAKEINTRESOURCE(ICON_CONTROLWINDOW)));
#else

#endif
	window.label("XPilot5 Control Panel");

	window.resizable(window.mainTile);
    window.end();
    window.show(argc, argv);
    int ret = Fl::run(); //Run the FLTK event loop!

	if (window.privateServerPanel)
		window.privateServerPanel->FlushToIni();
	return(ret);
}

///////////////////////////////////////////////////////////////////////////////
XPilotControlWindow::XPilotControlWindow(int w,int h,const char *l)
        : Fl_Double_Window(w,h,l)
{
//	lanPanel = NULL;
	inetPanel = NULL;
	localServerPanel = NULL;
	privateServerPanel = NULL;
	editorsPanel = NULL;
	splashPanel = NULL;

	{ Fl_Tile* o = mainTile = new Fl_Tile(WIDGET_INSET, TOPOFS, MAINWIN_WIDTH-(WIDGET_INSET*2), STATUS_TOP-TOPOFS-WIDGET_INSET);
		mainButtonsPanel = new MainButtonsPanel(XPCONTROLBUTTON0LEFT, TOPOFS, XPCONTROLBUTTON1LEFT,
			STATUS_TOP-TOPOFS-WIDGET_INSET, "");
		mainButtonsPanel->end();

		mainTile->end();
 	}



	status = new Fl_Status(WIDGET_INSET,
						   STATUS_TOP,
						   MAINWIN_WIDTH-(WIDGET_INSET*2),
						   STATUS_HEIGHT);
	status->color(xpBackColor);
	status->value("Ready");

	UpdateMainWindow();
}

///////////////////////////////////////////////////////////////////////////////
void XPilotControlWindow::UpdateMainWindow()
{
	int leftPos = mainButtonsPanel->w()+2;
	int height = status->y()-TOPOFS-WIDGET_INSET;
	int width = w()-leftPos;

	if (localServerPanel)
		localServerPanel->hide();
	if (editorsPanel)
		editorsPanel->hide();
//	if (lanPanel)
//		lanPanel->hide();
	if (inetPanel)
		inetPanel->hide();
	if (privateServerPanel)
		privateServerPanel->hide();
	if (splashPanel)
		splashPanel->hide();

	switch (mainButtonsPanel->iSelectionMainButtons)
	{
	case IDM_SPLASH:
		if (!splashPanel)
		{
			splashPanel = new SplashPanel(leftPos, TOPOFS, width, height);
			splashPanel->end();
			mainTile->add(splashPanel);
//			splashPanel->show();
		}
		else
		{
			splashPanel->show();
			splashPanel->resize(leftPos, TOPOFS, width, height);
		}
		break;
	case IDM_CONNECT_LOCAL:
		if (!localServerPanel)
		{
			localServerPanel = new LocalServerPanel(leftPos, TOPOFS, width, height);
			localServerPanel->end();
			localServerPanel->SetErrMsgHandler(&XPilotControlWindow::ErrHandler, this);
			mainTile->add(localServerPanel);
			Fl::redraw();
		}
		else
		{
			localServerPanel->show();
			localServerPanel->resize(leftPos, TOPOFS, width, height);
			//mainTile->add(localServerPanel);

		}
		DeleteSplash();
		break;
/*
	case IDM_CONNECT_LAN:
		if (!lanPanel)
		{
			SetStatus("Creating LAN Server List.");
			lanPanel = new XPWhereWindow(leftPos, TOPOFS,
										 width, height, "Lan List");
			lanPanel->end();
			//lanPanel->SetControlWindow(this);
			// whut? inetPanel->SetErrMsgHandler(&XPilotControlWindow::ErrHandler, this);
			mainTile->add(lanPanel);
			lanPanel->resizable(mainTile);
		}
		else
		{
			InetServerList*	sl = (InetServerList *) lanPanel->fsl->GetServerList();
			sl->Refresh();
		}
		if (lanPanel)
		{
			lanPanel->show();
			lanPanel->resize(leftPos, TOPOFS, width, height);
			mainTile->add(lanPanel);
		}
		else
			SetStatus("Failed to create Lan Server List!");
		DeleteSplash();
		break;
*/
	case IDM_CONNECT_INTERNET:
		if (!inetPanel)
		{
			SetStatus("Creating Internet Server List.");
			inetPanel = new InetServerPanel(leftPos, TOPOFS,
										 width, height, "Internet List");

			inetPanel->end();
			//inetPanel->SetControlWindow(this);
			inetPanel->SetErrMsgHandler(&XPilotControlWindow::ErrHandler, this);
			mainTile->add(inetPanel);
			inetPanel->resizable(mainTile);
		}
		else
		{
			InetServerList*	sl = (InetServerList *) inetPanel->fsl->GetServerList();
			sl->Refresh();
		}
		if (inetPanel)
		{
			inetPanel->show();
			inetPanel->resize(leftPos, TOPOFS, width, height);
		}
		else
			SetStatus("Failed to create Internet Server List!");
		DeleteSplash();
		break;
	case IDM_CONNECT_PRIVATE:
		if (!privateServerPanel)
		{
			SetStatus("Creating Private Server List.");
			privateServerPanel = new PrivateServerPanel(leftPos, TOPOFS,
										 width, height, "Private Server List");

			privateServerPanel->end();
			//privatePanel->SetControlWindow(this);
			privateServerPanel->SetErrMsgHandler(&XPilotControlWindow::ErrHandler, this);
			mainTile->add(privateServerPanel);
			privateServerPanel->resizable(mainTile);
		}
		else
		{
			PrivateServerList*	sl = (PrivateServerList *) privateServerPanel->fsl->GetServerList();
			sl->Refresh();
		}
		if (privateServerPanel)
		{
			privateServerPanel->show();
			privateServerPanel->resize(leftPos, TOPOFS, width, height);
		}
		else
			SetStatus("Failed to create Private Server List!");
		DeleteSplash();
		break;
	case IDM_EDITORS :
		if (!editorsPanel)
		{
			editorsPanel = new EditorsPanel(leftPos, TOPOFS, width, height);
			editorsPanel->end();
			mainTile->add(editorsPanel);
			editorsPanel->nameAndShipButton = mainButtonsPanel->nameAndShipButton;
		}
		else
		{
			editorsPanel->show();
			editorsPanel->resize(leftPos, TOPOFS, width, height);
		}
		DeleteSplash();
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////
void XPilotControlWindow::SetStatus(PCSTR fmt, ...)
{
    va_list	ap;
    char	s[512];

    va_start(ap, fmt);
    vsnprintf(s, 511, fmt, ap);
	status->value(s);
    va_end(ap);
	Fl::check();
};

///////////////////////////////////////////////////////////////////////////////
int XPilotControlWindow::handle(int event)
{

    switch (event)
	{
    case FL_FOCUS:
		redraw();
        return TRUE;
	case UPDATECONTROL:
		UpdateMainWindow();
		return TRUE;
    }
    return Fl_Window::handle(event);
}

///////////////////////////////////////////////////////////////////////////////
void XPilotControlWindow::ErrHandler(void* myThis, ErrMsgType emt, PCSTR ctl, ...)
{
	XPilotControlWindow* xcw = (XPilotControlWindow*)myThis;
    char szBuffer[512];
	va_list marker;

    // Figure through the extra arguments.
    va_start(marker, ctl);
    vsnprintf(szBuffer, 512, ctl, marker);
    va_end(marker);

	xcw->SetStatus(szBuffer);
}

///////////////////////////////////////////////////////////////////////////////
void XPilotControlWindow::DeleteSplash()
{
	if (splashPanel)
	{
		mainTile->remove(splashPanel);
		delete splashPanel;
	}
	splashPanel = NULL;
}
