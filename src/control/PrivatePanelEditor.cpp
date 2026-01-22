/* $Id: PrivatePanelEditor.cpp,v 1.4 2004/05/14 21:20:10 dick Exp $
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
 * $Log: PrivatePanelEditor.cpp,v $
 * Revision 1.4  2004/05/14 21:20:10  dick
 * Abandon the IP field.  Use only needs name (which could be an IP) and port
 *
 * Revision 1.3  2004/05/11 06:49:31  dick
 * Edit/Add/Change/Delete the entries in the PrivateServerList.
 *
 * Revision 1.2  2004/05/10 17:03:31  dick
 * Linux needs stdlib.h
 *
 * Revision 1.1  2004/05/10 12:48:00  dick
 * Add a Private Server button to the main panel, which allows the user to
 * manually track servers that don't appear in meta.
 *
 */

#include "StdAfx.h"
#include <stdlib.h>

#include "FL/Fl_Window.H"
#include "FL/Fl_Input.H"

#include "FlEventNames.h"

#include "config.h"
#include "portability.h"
#include "cstring.h"

#include "PrivateServerPanel.h"
#include "PrivatePanelEditor.h"

///////////////////////////////////////////////////////////////////////////////
PrivatePanelEditor::PrivatePanelEditor()
: Fl_Window(351, 152, "Private Server Editor")
{
	user_data((void*)(this));
	serverInput = new Fl_Input(130, 25, 210, 25, "Server name");
	serverInput->tooltip("Enter the name or IP for the XPilot server");

	portInput = new Fl_Input(130, 50, 210, 25, "Server Port");
	portInput->tooltip("Enter the port that the server listens on");

	okButton = new Fl_Return_Button(160, 115, 80, 25, "OK");
	okButton->callback(Callback, this);

	cancelButton = new Fl_Button(255, 115, 85, 25, "Cancel");
	cancelButton->callback(Callback, this);
	end();
}


///////////////////////////////////////////////////////////////////////////////
int PrivatePanelEditor::handle(int event)
{
	D(PCSTR ev = GetFlEventName(event);
				printf("PrivatePanelEditor::handle: ev=%s\n", ev);)

	switch (event)
	{
		case FL_KEYDOWN:
			switch (Fl::event_key())
			{
			case FL_Enter:
				Callback(okButton);
				return(1);
			case FL_Escape:
				Callback(cancelButton);
				return(1);
			}

	}

	return(PrivatePanelEditorSUPERCLASS::handle(event));
}

///////////////////////////////////////////////////////////////////////////////
//static
void PrivatePanelEditor::Callback(Fl_Widget* button, void* v)
{
	PrivatePanelEditor* ppe = (PrivatePanelEditor*)v;
	ppe->Callback(button);
}

///////////////////////////////////////////////////////////////////////////////
void PrivatePanelEditor::Callback(Fl_Widget* button)
{
	if (button == okButton || button == cancelButton)
	{
		hide();
		psp->PanelEditorCallback(button == okButton ? command : NULL);
		delete this;
		return;
	}
/*	if (button == cancelButton)
	{
		hide();
		delete this;
		return;
	}
*/
}
