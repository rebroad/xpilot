/* $Id: ShipWindow.h,v 1.27 2004/06/01 23:13:00 dick Exp $
 *
 * ShipWindow - The ShipEditor 'applet'
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
 * $Log: ShipWindow.h,v $
 * Revision 1.27  2004/06/01 23:13:00  dick
 * You can't really make a new button from an object and stick it in a static.
 * (Who owns it if you open 2 MapEditors?)  So now each window keeps track of
 * his own buttons.
 *
 * Revision 1.26  2004/05/28 18:51:24  dick
 * Button callbacks are private to this file.  Don't declare them in the .h .
 *
 * Revision 1.25  2004/01/13 20:32:16  jlmiller
 * Provide proper prompting when closing a modified shipeditor window.
 *
 * Revision 1.24  2002/09/10 19:09:45  jlmiller
 * Add "Copy Ship" button.
 *
 * Revision 1.23  2002/08/22 16:06:30  jlmiller
 * Use Fl_Double_Window to eliminate tooltip flicker.
 *
 * Revision 1.22  2002/08/20 10:47:29  jlmiller
 * File & Class Renamings:
 * ViewWindow -> MapEditWindow
 * XpInput -> XPInput
 * xpeTypes -> xpcTypes
 *
 * Revision 1.21  2002/08/02 13:36:39  jlmiller
 * Add coorDisplay to the ShipEditor. Useful for debug but might be useful for users too.
 *
 * Revision 1.20  2002/07/12 15:00:06  jlmiller
 * Integrate tabbed main window, with clean up of globals.
 *
 * Revision 1.19  2002/07/10 04:06:05  dick
 * CreateShipWin() becomes ShipWindow::Factory()
 *
 */

#ifndef	_SHIPWINDOW_H_
#define	_SHIPWINDOW_H_

#include "ShipEditWindow.h"
#include "Fl_Status.h"
#include "ShipUndoableCommand.h"
#include "ShipListViewWindow.h"
#include "XPInput.h"
#include "xpcTypes.h"
#include "UndoMan.h"

const int numShipActionButtons = 3;
const int numToggleShipToolButtons = 4;
const int numToggleShipSymButtons = 10;

class ShipWindow : public Fl_Double_Window {
public:
    int				handle(int);
    ShipWindow(int w,int h,const char *l=0);
	static ShipWindow*	Factory();

	static void			UndoButtonEnabler(void* myThis, bool enable);
	static void			RedoButtonEnabler(void* myThis, bool enable);
	static void			SaveButtonEnabler(void* myThis, bool enable);
	void				UpdateInfoFields();
	void				Update();
public:
	void				SetStatus(PCSTR text, ...);
	void				SetItemCount(PCSTR text, ...);
	void				SetCoordDisp(PCSTR text, ...);
	bool				SaveShipFile();
	bool				SaveAsShipFile();

	int					iSelectionShipSyms;
	int					iSelectionShipTools;
	XPInput*			nameInp;
	XPInput*			authorInp;
	Fl_Output*			itemCount;

	FunctionButton*			openButton;
	FunctionButton*			saveButton;
	FunctionRepeatButton*	undoButton;
	FunctionRepeatButton*	redoButton;

	FunctionRepeatButton*	shiftLeft;
	FunctionRepeatButton*	shiftRight;
	FunctionRepeatButton*	shiftUp;
	FunctionRepeatButton*	shiftDown;
	FunctionButton*			mirrorVert;
	FunctionButton*			mirrorHoriz;
	FunctionRepeatButton*	rotateRight;
	FunctionRepeatButton*	rotateLeft;
	Fl_Output*				coordDisp;

	ShipListViewWindow*		shiplistview;
	ShipEditWindow*			shipedit;
	ShipList				Shplst;

	UndoMan					undoMan;
private:
	Fl_Status*				status;
	FunctionRepeatButton*	shipActionButtons[numShipActionButtons];
	FunctionToggleButton*	toggleShipToolButtons[numToggleShipToolButtons];
	FunctionToggleButton*	toggleShipSymButtons[numToggleShipSymButtons];

};

#endif	// _SHIPWINDOW_H_
