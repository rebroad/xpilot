/* $Id: EditorsPanel.h,v 1.3 2004/06/01 23:13:00 dick Exp $
 *
 * EditorsPanel - The panel on the right that contains the different editor buttons
 *
 * XPilot5 - Manage all local XPilot activity.
 *
 *      Copyright (C) 2002 by
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
 * $Log: EditorsPanel.h,v $
 * Revision 1.3  2004/06/01 23:13:00  dick
 * You can't really make a new button from an object and stick it in a static.
 * (Who owns it if you open 2 MapEditors?)  So now each window keeps track of
 * his own buttons.
 *
 * Revision 1.2  2004/05/28 18:36:19  dick
 * Keep a pointer to the NameAndShipButton that's on the main panel, and give
 * it to the ClientConfig.
 *
 * Revision 1.1  2002/08/28 15:30:38  jlmiller
 * Make EditorsPanel a separate class, similar to LocalServerPanel.
 *
 */

#ifndef	_EDITORSPANEL_H_
#define	_EDITORSPANEL_H_

#include "FL/Fl_Group.H"

class NameAndShipButton;

const int numEditorsActionButtons = 4;

#define	EditorsPanelSUPERCLASS	Fl_Group
class EditorsPanel : public EditorsPanelSUPERCLASS {
public:
	EditorsPanel(int x, int y, int w,int h,const char *l=0);
	~EditorsPanel();

	NameAndShipButton*	nameAndShipButton;

private:
	FunctionButton*	editorsActionButtons[numEditorsActionButtons];
};
#endif	// _EDITORSPANEL_H_
