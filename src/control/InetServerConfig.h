/* $Id: InetServerConfig.h,v 1.2 2004/05/28 18:40:36 dick Exp $
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
 * $Log: InetServerConfig.h,v $
 * Revision 1.2  2004/05/28 18:40:36  dick
 * xpcheckbutton_cb() becomes XPCheckButton::CallbackFl() .
 *
 * Revision 1.1  2004/05/13 09:17:08  dick
 * XPWhereWindow becomes InetServerPanel, (which matches LocalServerPanel and PrivateServerPanel).
 * XPWhereConfig becomes InetServerConfig.
 *
 * Revision 1.8  2004/05/10 17:06:55  dick
 * Get rid of "type", which was supposed to be a switch between different configs.
 * Instead use different subclasses for the config.
 *
 * Revision 1.7  2004/05/10 12:44:37  dick
 * Callbacks are static here
 *
 * Revision 1.6  2004/02/09 18:22:48  dick
 * Lots of whitespace changes to support fixing resizable columns.
 *
 */

#ifndef _INETSERVERCONFIG_H_
#define _INETSERVERCONFIG_H_

#include "FL/Fl.H"
#include "FL/Fl_Window.H"
#include "FL/Fl_Box.H"
#include "FL/Fl_Group.H"
#include "FL/Fl_Check_Button.H"
#include "FL/Fl_Button.H"
#include "XPInput.h"
#include "XPCheckButton.h"


class InetServerPanel;

class InetServerConfig : public Fl_Window {
public:
	InetServerConfig(int w,int h,const char *l=0);
	static InetServerConfig*	Factory();		// Create and return an open XPWhereConfig

	void	UpdateDisplay();
	void	SetupLocalColumns();

public:
	bool				localColumns[14];
	InetServerPanel*	isPanel;
	XPInput*			metaHost;
	XPInput*			metaHostAlt;

private:
	Fl_Check_Button*	checkPrimary1;
	Fl_Check_Button*	checkPrimary2;
	Fl_Check_Button*	updateCheck;
	Fl_Input*			updateTime;
	Fl_Button*			okButton;
	Fl_Button*			cancelButton;
};
#endif //_INETSERVERCONFIG_H_
