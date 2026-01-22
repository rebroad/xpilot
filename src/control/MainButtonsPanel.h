/* $Id: MainButtonsPanel.h,v 1.5 2004/06/01 23:13:00 dick Exp $
 *
 * MainButtonsPanel - Draw the main panel on the left.
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
 * $Log: MainButtonsPanel.h,v $
 * Revision 1.5  2004/06/01 23:13:00  dick
 * You can't really make a new button from an object and stick it in a static.
 * (Who owns it if you open 2 MapEditors?)  So now each window keeps track of
 * his own buttons.
 *
 * Revision 1.4  2004/05/30 16:03:55  dick
 * Use a destructor to delete the background image when we're done.
 *
 * Revision 1.3  2004/05/28 18:43:16  dick
 * Add a NameAndShipButton to the bottom of the panel.
 *
 * Revision 1.2  2004/02/28 07:41:32  dick
 * Draw a cosmic background png
 *
 * Revision 1.1  2002/09/18 14:49:22  jlmiller
 * Changes necessary for Main window to be resizable, and tiled.
 *
 */

#ifndef	_MAINBUTTONSPANEL_H_
#define	_MAINBUTTONSPANEL_H_

#include "FL/Fl_Group.H"

class NameAndShipButton;

const int numToggleMainButtons = 5;

#define	MainButtonsPanelSUPERCLASS	Fl_Group
class MainButtonsPanel : public MainButtonsPanelSUPERCLASS {
public:
	MainButtonsPanel(int x, int y, int w,int h,const char *l=0);
	~MainButtonsPanel();
	static void CallbackFl(Fl_Widget *w, void* v);

	virtual void	draw();

	int				iSelectionMainButtons;
	NameAndShipButton*	nameAndShipButton;

private:
	Fl_PNG_Image*		bgImage;
	FunctionToggleButton*	toggleMainButtons[numToggleMainButtons];
};
#endif	// _MAINBUTTONSPANEL_H_
