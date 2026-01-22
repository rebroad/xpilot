/* $Id: PrivatePanelEditor.h,v 1.3 2004/05/14 21:20:10 dick Exp $
 *
 * PrivatePanelEditor - The window that edits an entry in the
 *                      private server list
 *
 *      Copyright (C) 2004 by
 *
 *      The XPilot Authors   <xpilot@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
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
 * $Log: PrivatePanelEditor.h,v $
 * Revision 1.3  2004/05/14 21:20:10  dick
 * Abandon the IP field.  Use only needs name (which could be an IP) and port
 *
 * Revision 1.2  2004/05/11 06:49:31  dick
 * Edit/Add/Change/Delete the entries in the PrivateServerList.
 *
 * Revision 1.1  2004/05/10 12:48:00  dick
 * Add a Private Server button to the main panel, which allows the user to
 * manually track servers that don't appear in meta.
 *
 */


#ifndef PrivatePanelEditor_h
#define PrivatePanelEditor_h
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Button.H>

class PrivateServerPanel;
class PrivatePanelEditButton;

#define PrivatePanelEditorSUPERCLASS	Fl_Window
class PrivatePanelEditor : public PrivatePanelEditorSUPERCLASS
{
public:
	PrivatePanelEditor();

	virtual	int handle(int event);

	PrivateServerPanel*	psp;

	Fl_Input*			serverInput;
	Fl_Input*			portInput;
	Fl_Return_Button*	okButton;
	Fl_Button*			cancelButton;

	PrivatePanelEditButton*	command;		//add,change or delete

	String				origName;		// original name and port so change can find
	int					origPort;		// the original matching ServerSite.
private:
	static void			Callback(Fl_Widget*, void*);
		   void			Callback(Fl_Widget*);

};
#endif
