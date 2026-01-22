/* $Id: ShipWindow.cpp,v 1.63 2004/07/22 17:04:36 dick Exp $
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
 * $Log: ShipWindow.cpp,v $
 * Revision 1.63  2004/07/22 17:04:36  dick
 * When the user presses the New Ship button, create an empty ship.
 *
 * Revision 1.62  2004/06/01 23:13:00  dick
 * You can't really make a new button from an object and stick it in a static.
 * (Who owns it if you open 2 MapEditors?)  So now each window keeps track of
 * his own buttons.
 *
 * Revision 1.61  2004/06/01 18:18:48  dick
 * xpinput_cb becomes XPInput::CallbackFl
 *
 * Revision 1.60  2004/05/28 18:51:24  dick
 * Button callbacks are private to this file.  Don't declare them in the .h .
 *
 * Revision 1.59  2004/03/03 07:54:35  dick
 * Don't make pixmaps at load time; wait until runtime init, after Strings have
 * actually been inited.
 *
 * Revision 1.58  2004/02/05 04:42:48  dick
 * gcc 3 cries about no newline at end of file
 *
 * Revision 1.57  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.56  2004/01/13 21:30:14  jlmiller
 * Use the provided TestFileExists function when Save As... to an existing file.
 *
 * Revision 1.55  2004/01/13 21:02:08  jlmiller
 * Function order cleanup
 *
 * Revision 1.54  2004/01/13 20:32:16  jlmiller
 * Provide proper prompting when closing a modified shipeditor window.
 *
 * Revision 1.53  2003/12/22 05:59:51  dick
 * Use png(s) instead of xpm(s).
 *
 * Revision 1.52  2002/09/18 16:51:59  jlmiller
 * Set view to current selected ship when cloning, or when removing tail ship.
 *
 * Revision 1.51  2002/09/10 19:09:45  jlmiller
 * Add "Copy Ship" button.
 *
 * Revision 1.50  2002/09/10 01:22:45  jlmiller
 * Added 'Closing Confirm' dialog boxes to mapeditor and shipeditor.
 *
 * Revision 1.49  2002/09/02 16:27:32  dick
 * Singleton ini becomes iniClient.
 *
 * Revision 1.48  2002/08/24 03:46:42  dick
 * Update to fltk1.1.0rc5 - fl_file_chooser.H has gone away.
 *
 * Revision 1.47  2002/08/23 15:58:04  dick
 * The filename is a const for ShipList.
 *
 * Revision 1.46  2002/08/23 14:18:25  jlmiller
 * ShipShapeFile is a String
 *
 * Revision 1.45  2002/08/22 16:06:30  jlmiller
 * Use Fl_Double_Window to eliminate tooltip flicker.
 *
 * Revision 1.44  2002/08/20 16:32:56  jlmiller
 * Use NELEM() macro
 *
 * Revision 1.43  2002/08/20 10:47:29  jlmiller
 * File & Class Renamings:
 * ViewWindow -> MapEditWindow
 * XpInput -> XPInput
 * xpeTypes -> xpcTypes
 *
 * Revision 1.42  2002/08/02 13:36:39  jlmiller
 * Add coorDisplay to the ShipEditor. Useful for debug but might be useful for users too.
 *
 * Revision 1.41  2002/07/29 19:16:49  jlmiller
 * Move itemCount out of group.
 *
 * Revision 1.40  2002/07/24 18:32:06  jlmiller
 * Remove a few unneeded fields from the toggle button struct.
 *
 * Revision 1.39  2002/07/23 19:58:24  jlmiller
 * No, actually the #include "" was right the first time, so lets put it back.
 *
 * Revision 1.38  2002/07/23 18:21:04  jlmiller
 * Move ShipList buttons above the list.
 *
 * Revision 1.37  2002/07/23 13:24:12  jlmiller
 * #includes should be <> instead of ""
 *
 * Revision 1.36  2002/07/23 12:27:37  jlmiller
 * Activate image saving routines, which were present but not available before.
 *
 * Revision 1.35  2002/07/19 14:49:26  jlmiller
 * use constant SHIPLIST_SIZE
 *
 * Revision 1.34  2002/07/19 14:05:36  jlmiller
 * Use correct pixmaps for cycle/rotate buttons.
 *
 * Revision 1.33  2002/07/15 19:12:38  jlmiller
 * UPDATESHIPWIN becomes SELECTSHIP for better readability.
 *
 * Revision 1.32  2002/07/12 15:00:06  jlmiller
 * Integrate tabbed main window, with clean up of globals.
 *
 * Revision 1.31  2002/07/10 04:06:05  dick
 * CreateShipWin() becomes ShipWindow::Factory()
 *
 * Revision 1.30  2002/07/09 15:23:45  jlmiller
 * Fl_Windows require a ->end() call when we're done creating them.
 *
 * Revision 1.29  2002/06/25 20:20:09  jlmiller
 * Massive restructure to clean up includes, and objectify code more.
 *
 * Revision 1.28  2002/06/25 15:02:19  jlmiller
 * Move iSelection fields to the window, instead of the individual document,
 * as they didn't make any sense at document level.
 *
 * Revision 1.27  2002/06/17 21:00:12  jlmiller
 * Clear undo buffer, if called for.
 *
 * Revision 1.26  2002/06/14 20:15:08  jlmiller
 * First pass at properly showing disabled buttons.
 *
 * Revision 1.25  2002/06/11 03:14:10  jlmiller
 * Better handling of itemCount text field. Similar to status, as we're just
 * outputing info.
 *
 * Revision 1.24  2002/06/03 02:58:20  jlmiller
 * Make XPWhere (ServerList window) its own class
 *
 * Revision 1.23  2002/06/02 22:44:05  jlmiller
 * Add undoMan to shipeditor.
 * Commentability.
 *
 * Revision 1.22  2002/06/01 17:14:25  jlmiller
 * Fix a few nasty little bugs that would crash only when the Debugger WASNT running.
 * Commentability
 *
 * Revision 1.21  2002/05/30 18:33:58  jlmiller
 * Splitting of consecutive vertices now available.
 *
 * Revision 1.20  2002/05/30 17:14:21  jlmiller
 * Moving of items now available.
 *
 * Revision 1.19  2002/05/30 14:19:26  jlmiller
 * Remove activeMapWindow global, which is unnecessay
 * MapWindow & ShipWindow store documents similarly now.
 *
 * Revision 1.18  2002/05/30 02:26:14  jlmiller
 * Eliminate unnecessary global pointer. Should help eliminate null pointer bugs.
 *
 * Revision 1.17  2002/05/29 20:31:47  jlmiller
 * Add a status widget
 *
 * Revision 1.16  2002/05/28 07:19:51  dick
 * Rename xpilotedit to XPilotControl
 *
 */
#include "StdAfx.h"

#include "FL/Fl_PNG_Image.H"
#include "FL/Fl_File_Chooser.H"

#include "ShipWindow.h"
#include "controlconst.h"
#include "proto.h"

#include "guiconst.h"
#include "portability.h"
#include "IniClient.h"

static void addship_cb(Fl_Widget*, void*);
static void remship_cb(Fl_Widget*, void*);
static void copyship_cb(Fl_Widget*, void*);

#define SPACER 6

xpe_tbrbutton shipActionButtonsDefs[numShipActionButtons] =
{
	{0, 0, "Add New Ship", "addship", addship_cb},
	{BUTTON_SIZE,0, "Remove Current Ship", "remship", remship_cb},
	{BUTTON_SIZE*2,0, "Copy Ship", "copyship", copyship_cb}
};


xpe_tbtbutton toggleShipToolButtonsDefs[numToggleShipToolButtons] =
{
	{"toolspen", IDM_INSERT, "Insert"},
	{"toolseraser", IDM_DELETE, "Delete"},
	{"moveitem", IDM_MOVE, "Move"},
	{"splitvertex", IDM_SPLIT, "Split Vertexes"},
};

xpe_tbtbutton toggleShipSymButtonsDefs[numToggleShipSymButtons] =
{
	{"vertex", IDM_SHIP_VERTEX, "Vertex"},
	{"m_gun", IDM_SHIP_MAINGUN, "Main Gun"},
	{"engine", IDM_SHIP_ENGINE, "Engine"},
	{"missile_rack", IDM_SHIP_MISSLERACK, "Missle Rack"},
	{"l_gun", IDM_SHIP_LGUN, "Forward Left Gun"},
	{"r_gun", IDM_SHIP_RGUN, "Forward Right Gun"},
	{"l_rgun", IDM_SHIP_LRGUN, "Rear Left Gun"},
	{"r_rgun", IDM_SHIP_RRGUN, "Rear Right Gun"},
	{"l_light", IDM_SHIP_LLIGHT, "Left Light"},
	{"r_light", IDM_SHIP_RLIGHT, "Right Light"},
};

//Note:
//Prototypes of callbacks are in ShipWindow.h
///////////////////////////////////////////////////////////////////////////////
void shipundo_cb(Fl_Widget* w, void* v)
{
	ShipWindow *tmp = (ShipWindow *) v;
	tmp->undoMan.Undo();
	tmp->shipedit->redraw();
	tmp->shiplistview->redraw();
}

///////////////////////////////////////////////////////////////////////////////
void shipredo_cb(Fl_Widget* w, void* v)
{
	ShipWindow *tmp = (ShipWindow *) v;
	tmp->undoMan.Redo();
	tmp->shipedit->redraw();
	tmp->shiplistview->redraw();
}

///////////////////////////////////////////////////////////////////////////////
void dummy_cb(Fl_Widget*, void* v)
{
	ShipWindow *tmp = (ShipWindow *) v;
	tmp->redraw();
}

///////////////////////////////////////////////////////////////////////////////
void itemtogglebar_cb(Fl_Widget *w, void* v)
{
	FunctionToggleButton *btn = (FunctionToggleButton *) w;
	ShipWindow *tmp = (ShipWindow *) v;

	*btn->togglevar = btn->id;
	if (tmp->Shplst.pCurrentShip != NULL)
		tmp->UpdateInfoFields();
}

///////////////////////////////////////////////////////////////////////////////
static void addship_cb(Fl_Widget*, void* v)
{
	ShipDocument *tmpnew = new ShipDocument(false);
	ShipWindow *tmp = (ShipWindow *) v;

	tmp->Shplst.Add(tmpnew);
	tmp->Shplst.pCurrentShip = tmpnew;
	tmp->Shplst.changed = true;
	tmp->undoMan.Clear();
	tmp->shiplistview->redraw();
	tmp->shipedit->redraw();
	tmp->UpdateInfoFields();
}
///////////////////////////////////////////////////////////////////////////////
static void copyship_cb(Fl_Widget*, void* v)
{
	ShipWindow *tmp = (ShipWindow *) v;
	tmp->Shplst.CloneShip(tmp->Shplst.pCurrentShip);

	tmp->Shplst.changed = true;
	tmp->undoMan.Clear();
	tmp->shiplistview->ViewSelectedShip();
	tmp->shiplistview->redraw();
	tmp->shipedit->redraw();
	tmp->UpdateInfoFields();
}

///////////////////////////////////////////////////////////////////////////////
//If Saved successfully, return 'true'
void saveasshipfile_cb(Fl_Widget* w, void* v)
{
	ShipWindow *tmp = (ShipWindow *) v;
	tmp->SaveAsShipFile();
}

///////////////////////////////////////////////////////////////////////////////
//If Saved successfully, return 'true'
void saveshipfile_cb(Fl_Widget* w, void* v)
{
	ShipWindow *tmp = (ShipWindow *) v;
	tmp->SaveShipFile();
}
///////////////////////////////////////////////////////////////////////////////
void openshipfile_cb(Fl_Widget*, void* v)
{
	ShipWindow *tmp = (ShipWindow *) v;
	const char *p = NULL;
	p = fl_file_chooser("Open","XPilot Shipfile (*.shp)",NULL);
	if (p)
	{
		tmp->Shplst.ClearShipList();
		tmp->Shplst.LoadShipFile((char *) p);
		tmp->shipedit->pShip = (ShipDocument *) tmp->Shplst.pCurrentShip;
	}
	tmp->undoMan.Clear();

	tmp->shiplistview->redraw();
	tmp->shipedit->redraw();
	tmp->UpdateInfoFields();
}

///////////////////////////////////////////////////////////////////////////////
static void remship_cb(Fl_Widget*, void* v)
{
	ShipWindow *tmp = (ShipWindow *) v;
	ShipDocument *tmpnew = NULL;
	if (!tmp->Shplst.pCurrentShip)
		return;


	if (tmp->Shplst.pCurrentShip ==
		tmp->Shplst.GetTail())
	{
		if (tmp->Shplst.GetCount() >= 2)
			tmpnew = (ShipDocument *) tmp->Shplst.GetHead();
		tmp->Shplst.RemoveTail();
		tmp->shiplistview->ViewSelectedShip();
	}
	else if (tmp->Shplst.GetCount() == 1)
	{
		tmp->Shplst.Empty();
	}
	else
	{
		tmpnew = (ShipDocument *) tmp->Shplst.pCurrentShip->GetNext();
		tmp->Shplst.Remove(tmp->Shplst.pCurrentShip);
	}



	delete tmp->Shplst.pCurrentShip;

	if (tmpnew)
		tmp->Shplst.pCurrentShip = tmpnew;
	else
		tmp->Shplst.pCurrentShip = NULL;

	tmp->undoMan.Clear();
	tmp->shiplistview->redraw();
	tmp->shipedit->redraw();
	tmp->UpdateInfoFields();
	tmp->Shplst.changed = true;
}

///////////////////////////////////////////////////////////////////////////////
void shiftship_cb(Fl_Widget* w, void* v)
{
	ShipWindow *tmp = (ShipWindow *) v;
	int dir;
	if (w == tmp->shiftLeft)
		dir = SHIFTLEFT;
	else if (w == tmp->shiftRight)
		dir = SHIFTRIGHT;
	else if (w == tmp->shiftUp)
		dir = SHIFTUP;
	else if (w == tmp->shiftDown)
		dir = SHIFTDOWN;

	//First check that we CAN move in the specified direction
	if (!tmp->Shplst.pCurrentShip->VerifyMove(dir))
		return;
	else
	{
		ShipUndoableCommand* uc = new ShipUndoableCommand(tmp,
			SHIFT, dir);
		uc->myShipWindow->undoMan.Do(uc);
	}

	tmp->shipedit->redraw();
	tmp->shiplistview->redraw();
}

///////////////////////////////////////////////////////////////////////////////
void mirrorship_cb(Fl_Widget* w, void* v)
{
	ShipWindow *tmp = (ShipWindow *) v;
	int dir;
	if (w == tmp->mirrorHoriz)
		dir = MIRRORH;
	else if (w == tmp->mirrorVert)
		dir = MIRRORV;

	ShipUndoableCommand* uc = new ShipUndoableCommand(tmp,
		MIRROR, dir);
	uc->myShipWindow->undoMan.Do(uc);

	tmp->shipedit->redraw();
	tmp->shiplistview->redraw();
}

///////////////////////////////////////////////////////////////////////////////
void rotateship_cb(Fl_Widget* w, void* v)
{
	ShipWindow *tmp = (ShipWindow *) v;
	int dir;
	if (w == tmp->rotateRight)
		dir = CLOCKWISE;
	else if (w == tmp->rotateLeft)
		dir = COUNTERCLOCKWISE;

	ShipUndoableCommand* uc = new ShipUndoableCommand(tmp,
		ROTATE, dir);
	uc->myShipWindow->undoMan.Do(uc);

	tmp->shipedit->redraw();
	tmp->shiplistview->redraw();
}
///////////////////////////////////////////////////////////////////////////////
void shipclose_cb(Fl_Widget* win, void* v)
{
	ShipWindow* w = (ShipWindow*)v;
	bool close = false;
	int ret;

	if (w->Shplst.changed)
	{
		ret = fl_choice("Ship Editor has unsaved changes. Do you still wish to quit?",
			"Close", "Cancel", "Save and Close");
		switch (ret)
		{
			case 0:
				close = true;
				break;
			case 1:
				close = false;
				break;
			case 2:
				if (w->SaveShipFile())
				  close = true;
				else
				  close = false;
				break;
		}
	}
	else
		close = true;

	if (close)
	{
		w->hide();
		delete w;
	}
}
///////////////////////////////////////////////////////////////////////////////
ShipWindow* ShipWindow::Factory()
{
	ShipWindow *window = new ShipWindow(780, 700, "XPilot ShipEditor");
	window->end();
	window->callback((Fl_Callback *)shipclose_cb, window);
	window->show();
	return(window);
}

#define SHIPTOOLSTOP BUTTON_SIZE*3
#define SHIPITEMSTOP BUTTON_SIZE*10
///////////////////////////////////////////////////////////////////////////////
ShipWindow::ShipWindow(int w,int h,const char *l)
        : Fl_Double_Window(w,h,l)
{
	ShipDocument *tmpshp = NULL;
	int i;

	iSelectionShipSyms = IDM_SHIP_VERTEX;
	iSelectionShipTools = IDM_INSERT;

	//Build the Ship system Buttons: Add ship, Del Ship, etc..
	for (i = 0; i < numShipActionButtons; i++)
    {
		shipActionButtons[i] = new FunctionRepeatButton(
			shipActionButtonsDefs[i].x, shipActionButtonsDefs[i].y,
			BUTTON_SIZE, BUTTON_SIZE, "", shipActionButtonsDefs[i].pm);
		shipActionButtons[i]->callback(shipActionButtonsDefs[i].cb);
		shipActionButtons[i]->tooltip(shipActionButtonsDefs[i].ttip);
		shipActionButtons[i]->user_data(this);
    }

	shiplistview = new ShipListViewWindow(0, BUTTON_SIZE, SHIPLIST_SIZE, h-BUTTON_SIZE, TRUE, "No label", &Shplst);
	shiplistview->end();

	//The following is a test only line to show the shiplistviewwindow oriented horizontally.
	//shiplistview = new ShipListViewWindow(0, 0, w, SHIPLIST_SIZE, FALSE, "No label", &Shplst);

	tmpshp = (ShipDocument *) Shplst.GetHead();

	shipedit = new ShipEditWindow(250, 65, 465, 465, "No label", tmpshp);
	shipedit->end();

	nameInp = new XPInput(shipedit->x()+40, 2, 120, 20, "Ship Name:");
	nameInp->callback((Fl_Callback *)XPInput::CallbackFl);
	authorInp = new XPInput(shipedit->x()+40, 26, 120, 20, "Ship Author:");
	authorInp->callback((Fl_Callback *)XPInput::CallbackFl);

	openButton = new FunctionButton(SHIPLIST_SIZE, 0, BUTTON_SIZE, BUTTON_SIZE,
        "", "fileopen");
	openButton->callback(openshipfile_cb);
	openButton->tooltip("Open Shipfile");
	openButton->user_data(this);


	saveButton = new FunctionButton(SHIPLIST_SIZE+BUTTON_SIZE, 0, BUTTON_SIZE, BUTTON_SIZE,
		"", "filesave");
	saveButton->callback(saveshipfile_cb);
	saveButton->tooltip("Save Shipfile");
	saveButton->user_data(this);

	FunctionButton *but = new FunctionButton(SHIPLIST_SIZE+BUTTON_SIZE*2, 0, BUTTON_SIZE, BUTTON_SIZE,
		"", "filesaveas");
	but->callback(saveasshipfile_cb);
	but->tooltip("Save Shipfile As..");
	but->user_data(this);

	undoButton = new FunctionRepeatButton(SHIPLIST_SIZE+BUTTON_SIZE*4, 0, BUTTON_SIZE, BUTTON_SIZE,
		"", "undo");
	undoButton->callback(shipundo_cb);
	undoButton->tooltip("Undo");
	undoButton->user_data(this);

	redoButton = new FunctionRepeatButton(SHIPLIST_SIZE+BUTTON_SIZE*5, 0, BUTTON_SIZE, BUTTON_SIZE,
		"", "redo");
	redoButton->callback(shipredo_cb);
	redoButton->tooltip("Redo");
	redoButton->user_data(this);

	undoMan.RegisterButtons(UndoButtonEnabler,
		RedoButtonEnabler,
		SaveButtonEnabler, this);

	//Build the Ship tool Buttons: Insert, Delete, Move, etc..
	Fl_Group *o = new Fl_Group(SHIPLIST_SIZE, SHIPTOOLSTOP, BUTTON_SIZE * 6,
		numToggleShipToolButtons * BUTTON_SIZE, "");
	for (i=0; i < numToggleShipToolButtons; i++)
	{
		toggleShipToolButtons[i] = new FunctionToggleButton(
			SHIPLIST_SIZE, SHIPTOOLSTOP+i*BUTTON_SIZE,
			BUTTON_SIZE, BUTTON_SIZE,
			toggleShipToolButtonsDefs[i].lbl, toggleShipToolButtonsDefs[i].pm);
		toggleShipToolButtons[i]->id = toggleShipToolButtonsDefs[i].id;
		toggleShipToolButtons[i]->togglevar = &iSelectionShipTools;
		toggleShipToolButtons[i]->type(FL_RADIO_BUTTON);
		toggleShipToolButtons[i]->callback(togglebar_cb);
		toggleShipToolButtons[i]->align(FL_ALIGN_RIGHT);
		toggleShipToolButtons[i]->user_data(this);
		if (toggleShipToolButtons[i]->id == iSelectionShipTools)
			toggleShipToolButtons[i]->value(true);

	}
	o->end();

	//Build the Ship item type Buttons: Vertex, Engine, Maingun etc..
	o = new Fl_Group(SHIPLIST_SIZE, SHIPITEMSTOP, BUTTON_SIZE * 6,
		numToggleShipSymButtons * BUTTON_SIZE, "");
	for (i=0; i < numToggleShipSymButtons; i++)
	{
		toggleShipSymButtons[i] = new FunctionToggleButton(
			SHIPLIST_SIZE, SHIPITEMSTOP+i*BUTTON_SIZE,
			BUTTON_SIZE, BUTTON_SIZE,
			toggleShipSymButtonsDefs[i].lbl, toggleShipSymButtonsDefs[i].pm);
		toggleShipSymButtons[i]->id = toggleShipSymButtonsDefs[i].id;
		toggleShipSymButtons[i]->togglevar = &iSelectionShipSyms;
		toggleShipSymButtons[i]->type(FL_RADIO_BUTTON);

		toggleShipSymButtons[i]->callback(itemtogglebar_cb);
		toggleShipSymButtons[i]->align(FL_ALIGN_RIGHT);
		toggleShipSymButtons[i]->user_data(this);
		if (toggleShipSymButtons[i]->id == iSelectionShipSyms)
			toggleShipSymButtons[i]->value(true);

	}
	o->end();

	shiftLeft = new FunctionRepeatButton(shipedit->x()-20, shipedit->y(), 20, shipedit->h(),
		"", "cyclel");
	shiftLeft->callback(shiftship_cb);
	shiftLeft->user_data(this);
	shiftLeft->tooltip("Shift Left");

	shiftRight = new FunctionRepeatButton(shipedit->x()+shipedit->w(), shipedit->y(), 20, shipedit->h(),
		"", "cycler");
	shiftRight->callback(shiftship_cb);
	shiftRight->user_data(this);
	shiftRight->tooltip("Shift Right");

	shiftUp = new FunctionRepeatButton(shipedit->x(), shipedit->y()-20, shipedit->w(), 20,
		"", "cycleu");
	shiftUp->callback(shiftship_cb);
	shiftUp->user_data(this);
	shiftUp->tooltip("Shift Up");

	shiftDown = new FunctionRepeatButton(shipedit->x(), shipedit->y()+shipedit->h(), shipedit->w(), 20,
		"", "cycled");
	shiftDown->callback(shiftship_cb);
	shiftDown->user_data(this);
	shiftDown->tooltip("Shift Down");

	mirrorHoriz = new FunctionButton(shipedit->x()+(shipedit->w()/2)-(BUTTON_SIZE/2),
		shipedit->y()+shipedit->h()+40, BUTTON_SIZE, BUTTON_SIZE,
		"", "mirrorh");
	mirrorHoriz->callback(mirrorship_cb);
	mirrorHoriz->user_data(this);
	mirrorHoriz->tooltip("Mirror Horizontally");

	mirrorVert = new FunctionButton(shipedit->x()+shipedit->w()+40,
		shipedit->y()+(shipedit->h()/2)-(BUTTON_SIZE/2), BUTTON_SIZE, BUTTON_SIZE,
		"", "mirrorv");
	mirrorVert->callback(mirrorship_cb);
	mirrorVert->user_data(this);
	mirrorVert->tooltip("Mirror Vertically");

	rotateRight = new FunctionRepeatButton(shipedit->x()+shipedit->w(),
		shipedit->y()+shipedit->h(), BUTTON_SIZE, BUTTON_SIZE,
		"", "rotate90");
	rotateRight->callback(rotateship_cb);
	rotateRight->user_data(this);
	rotateRight->tooltip("Rotate Right");

	rotateLeft = new FunctionRepeatButton(shipedit->x()-20,
		shipedit->y()-20, BUTTON_SIZE, BUTTON_SIZE,
		"", "rotate270");
	rotateLeft->callback(rotateship_cb);
	rotateLeft->user_data(this);
	rotateLeft->tooltip("Rotate Left");

	itemCount = new Fl_Output(SHIPLIST_SIZE, BUTTON_SIZE*20, 150, 20, NULL);
	itemCount->textcolor(FL_BLUE);

	coordDisp = new Fl_Output(shipedit->x(), shiftDown->y()+22, 100, 20, NULL);
	coordDisp->textcolor(FL_RED);

	status = new Fl_Status(shiplistview->x()+shiplistview->w(), h-70, w-shiplistview->x()-shiplistview->w(), 60);
	status->color(FL_GRAY);
	status->value("Ready");

	shiplistview->pShipList->LoadShipFile(iniClient.shipShapeFile);
	shiplistview->pShipList->pCurrentShip = (ShipDocument *) shiplistview->pShipList->GetHead();
	UpdateInfoFields();
};

///////////////////////////////////////////////////////////////////////////////
void ShipWindow::UpdateInfoFields()
{

	if (Shplst.pCurrentShip)
	{
		nameInp->SetStrng(&Shplst.pCurrentShip->name);
		authorInp->SetStrng(&Shplst.pCurrentShip->author);
		nameInp->Read();
		authorInp->Read();

		nameInp->activate();
		authorInp->activate();
		shiftUp->activate();
		shiftDown->activate();
		shiftLeft->activate();
		shiftRight->activate();
		rotateLeft->activate();
		rotateRight->activate();
		mirrorHoriz->activate();
		mirrorVert->activate();

		switch(iSelectionShipSyms)
		{
		case IDM_SHIP_VERTEX:
			SetItemCount("Vertex: %d of %d used.", Shplst.pCurrentShip->pts.GetCount(), Shplst.pCurrentShip->pts.GetMax());
			break;
		case IDM_SHIP_MAINGUN:
			SetItemCount("Main Gun: %d of %d used.", Shplst.pCurrentShip->m_gun.GetCount(), Shplst.pCurrentShip->m_gun.GetMax());
			break;
		case IDM_SHIP_ENGINE:
			SetItemCount("Engine: %d of %d used.", Shplst.pCurrentShip->engine.GetCount(), Shplst.pCurrentShip->engine.GetMax());
			break;
		case IDM_SHIP_MISSLERACK:
			SetItemCount("MRack: %d of %d used.", Shplst.pCurrentShip->m_racks.GetCount(), Shplst.pCurrentShip->m_racks.GetMax());
			break;
		case IDM_SHIP_LGUN:
			SetItemCount("L Gun: %d of %d used.", Shplst.pCurrentShip->l_guns.GetCount(), Shplst.pCurrentShip->l_guns.GetMax());
			break;
		case IDM_SHIP_RGUN:
			SetItemCount("R Gun: %d of %d used.", Shplst.pCurrentShip->r_guns.GetCount(), Shplst.pCurrentShip->r_guns.GetMax());
			break;
		case IDM_SHIP_LRGUN:
			SetItemCount("LR Gun: %d of %d used.", Shplst.pCurrentShip->l_rguns.GetCount(), Shplst.pCurrentShip->l_rguns.GetMax());
			break;
		case IDM_SHIP_RRGUN:
			SetItemCount("RR Gun: %d of %d used.", Shplst.pCurrentShip->r_rguns.GetCount(), Shplst.pCurrentShip->r_rguns.GetMax());
			break;
		case IDM_SHIP_LLIGHT:
			SetItemCount("L Light: %d of %d used.", Shplst.pCurrentShip->l_lights.GetCount(), Shplst.pCurrentShip->l_lights.GetMax());
			break;
		case IDM_SHIP_RLIGHT:
			SetItemCount("R Light: %d of %d used.", Shplst.pCurrentShip->r_lights.GetCount(), Shplst.pCurrentShip->r_lights.GetMax());
			break;
		}
	}
	else
	{
		String *tmpstrng = new String("");

		nameInp->SetStrng(tmpstrng);
		authorInp->SetStrng(tmpstrng);
		nameInp->deactivate();
		authorInp->deactivate();
		shiftUp->deactivate();
		shiftDown->deactivate();
		shiftLeft->deactivate();
		shiftRight->deactivate();
		rotateLeft->deactivate();
		rotateRight->deactivate();
		mirrorHoriz->deactivate();
		mirrorVert->deactivate();
	}
}

///////////////////////////////////////////////////////////////////////////////
void ShipWindow::SetStatus(PCSTR fmt, ...)
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
void ShipWindow::SetItemCount(PCSTR fmt, ...)
{
	va_list	ap;
	char	s[512];

	va_start(ap, fmt);
	vsnprintf(s, 511, fmt, ap);
	itemCount->value(s);
	va_end(ap);
//	Fl::check();
};
///////////////////////////////////////////////////////////////////////////////
void ShipWindow::SetCoordDisp(PCSTR fmt, ...)
{
	va_list	ap;
	char	s[512];

	va_start(ap, fmt);
	vsnprintf(s, 511, fmt, ap);
	coordDisp->value(s);
	va_end(ap);
	Fl::check();
}

///////////////////////////////////////////////////////////////////////////////
void ShipWindow::UndoButtonEnabler(void* myThis, bool enable)
{
	ShipWindow* cfg = (ShipWindow*)myThis;
	if (enable)
		cfg->undoButton->activate();
	else
		cfg->undoButton->deactivate();
}

///////////////////////////////////////////////////////////////////////////////
void ShipWindow::RedoButtonEnabler(void* myThis, bool enable)
{
	ShipWindow* cfg = (ShipWindow*)myThis;
	if (enable)
		cfg->redoButton->activate();
	else
		cfg->redoButton->deactivate();
}

///////////////////////////////////////////////////////////////////////////////
void ShipWindow::SaveButtonEnabler(void* myThis, bool enable)
{
	ShipWindow* cfg = (ShipWindow*)myThis;
	if (enable || cfg->Shplst.changed)
		cfg->saveButton->activate();
	else
		cfg->saveButton->deactivate();
}

///////////////////////////////////////////////////////////////////////////////
void ShipWindow::Update()
{
	shipedit->pShip = Shplst.pCurrentShip;
	undoMan.Clear();
	shipedit->redraw();
	UpdateInfoFields();
}

///////////////////////////////////////////////////////////////////////////////
// virtual
int ShipWindow::handle(int event)
{

	switch (event) {
	case SELECTSHIP :
		if (Shplst.changed)
		{
			Update();
			Shplst.changed = false;
			return TRUE;
		}
	}
	return Fl_Window::handle(event);
}

///////////////////////////////////////////////////////////////////////////////
bool ShipWindow::SaveAsShipFile(){
	char *p = NULL;
	bool rValue;
	p = fl_file_chooser("Save As", "XPilot Shipfile (*.shp)", NULL);

	if (TestFileOverwrite(p))
	{
		if (p)
		{
			if (rValue = Shplst.SaveShipFile(p))
			{
				Shplst.changed = false ;
				return true;
			}

		}
	}
	redraw();
	return false;
}
///////////////////////////////////////////////////////////////////////////////
bool ShipWindow::SaveShipFile() {
	char szTempFileName[255+2] ;
	strcpy(szTempFileName, Shplst.ShipFileName);

	if (szTempFileName[0])
	{
		if (Shplst.SaveShipFile(szTempFileName))
		{
			Shplst.changed = false ;
			return true;
		}
		else
		{
			ErrorHandler("Could not write file %s!", szTempFileName);
			return false;
		}
	}
	else
	{
		return SaveAsShipFile();
	}

}
