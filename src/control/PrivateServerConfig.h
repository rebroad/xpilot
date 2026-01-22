/* $Id: PrivateServerConfig.h,v 1.4 2004/05/28 18:45:49 dick Exp $
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
 * $Log: PrivateServerConfig.h,v $
 * Revision 1.4  2004/05/28 18:45:49  dick
 * xpcheckbutton_cb() becomes XPCheckButton::CallbackFl() .
 *
 * Revision 1.3  2004/05/13 09:29:59  dick
 * Whitespace
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

#ifndef _PRIVATESERVERCONFIG_H_
#define _PRIVATESERVERCONFIG_H_

#include "FL/Fl.H"
#include "FL/Fl_Window.H"
#include "FL/Fl_Box.H"
#include "FL/Fl_Group.H"
#include "FL/Fl_Check_Button.H"
#include "FL/Fl_Button.H"
#include "XPInput.h"
#include "XPCheckButton.h"


class PrivateServerPanel;

class PrivateServerConfig : public Fl_Window {
public:
	PrivateServerConfig(int w,int h,const char *l=0);
	static PrivateServerConfig*	Factory();		// Create and return an open PrivateServerConfig

	void	UpdateDisplay();
	void	SetupLocalColumns();

public:
	bool				localColumns[14];
	PrivateServerPanel*	psp;

private:
	Fl_Button*			okButton;
	Fl_Button*			cancelButton;
};
#endif //_PRIVATESERVERCONFIG_H_
