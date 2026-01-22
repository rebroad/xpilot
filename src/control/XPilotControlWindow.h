/* $Id: XPilotControlWindow.h,v 1.18 2004/05/28 18:14:04 dick Exp $
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
 * $Log: XPilotControlWindow.h,v $
 * Revision 1.18  2004/05/28 18:14:04  dick
 * Pass around the pointer to the NameAndShipWindow so that when ClientConfig
 * changes it, it gets notified.
 *
 * Revision 1.17  2004/05/13 09:18:39  dick
 * XPWhereWindow becomes InetServerPanel
 *
 * Revision 1.16  2004/05/10 12:48:00  dick
 * Add a Private Server button to the main panel, which allows the user to
 * manually track servers that don't appear in meta.
 *
 * Revision 1.15  2004/03/08 00:28:45  dick
 * Fix deleting the unused splashpanel
 *
 */

#ifndef	_XPILOTCONTROLWINDOW_H_
#define	_XPILOTCONTROLWINDOW_H_

#include "FL/Fl_Double_Window.H"
#include "FL/Fl_Tile.H"
#include "InetServerPanel.h"
#include "Fl_Status.h"

class MainButtonsPanel;
class LocalServerPanel;
class PrivateServerPanel;
class EditorsPanel;
class SplashPanel;

class XPilotControlWindow : public Fl_Double_Window {
public:
	XPilotControlWindow(int w,int h,const char* l=0);
	XPilotControlWindow(int X, int Y, int W, int H, const char* l = 0);
	static void	ErrHandler(void* myThis, ErrMsgType emt, PCSTR ctl, ...);

public:
	void	UpdateMainWindow();
	void	SetStatus(PCSTR text, ...);
    int		handle(int);

	Fl_Tile*			mainTile;
	MainButtonsPanel*	mainButtonsPanel;
	LocalServerPanel*	localServerPanel;
	PrivateServerPanel*	privateServerPanel;
	EditorsPanel*		editorsPanel;
//	InetServerPanel*	lanPanel;
	InetServerPanel*	inetPanel;

private:
	void			DeleteSplash();		// once it's off screen, we don't need it around anymore.

	Fl_Status*		status;
	SplashPanel*	splashPanel;
};

#endif	// _XPILOTCONTROLWINDOW_H_
