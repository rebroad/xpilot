/* $Id: MapWindow.cpp,v 1.54 2004/06/01 23:13:00 dick Exp $
 *
 * MapWindow - The MapEditor 'applet'
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
 * $Log: MapWindow.cpp,v $
 * Revision 1.54  2004/06/01 23:13:00  dick
 * You can't really make a new button from an object and stick it in a static.
 * (Who owns it if you open 2 MapEditors?)  So now each window keeps track of
 * his own buttons.
 *
 * Revision 1.53  2004/05/22 15:22:18  dick
 * Whitespace
 *
 * Revision 1.52  2004/03/03 07:54:35  dick
 * Don't make pixmaps at load time; wait until runtime init, after Strings have
 * actually been inited.
 *
 * Revision 1.51  2004/02/23 05:53:01  dick
 * SuSE wants unistd.h
 *
 * Revision 1.50  2004/02/21 06:57:32  dick
 * In the mapeditor, the prefs should have no knowledge of a ServerOptionsNetClient.
 *
 * Revision 1.49  2004/02/05 04:42:48  dick
 * gcc 3 cries about no newline at end of file
 *
 * Revision 1.48  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.47  2004/01/28 00:10:56  dick
 * PrefTab Server becomes TabServer
 *
 * Revision 1.46  2004/01/20 04:06:38  jlmiller
 * Prompt when opening a map, if existing map has changes not saved.
 *
 * Revision 1.45  2004/01/20 03:15:54  jlmiller
 * Lets use the nice XPFileChooser, since it lets us preview maps.
 *
 * Revision 1.44  2004/01/19 06:54:47  dick
 * rename a tmp var to mw.  tmp implies temp, which it's not.
 *
 * Revision 1.43  2004/01/17 19:37:17  dick
 * WildMap is now in the common library. Control and server use this,
 * instead of each having it's own copy.
 *
 * Revision 1.42  2004/01/13 21:30:14  jlmiller
 * Use the provided TestFileExists function when Save As... to an existing file.
 *
 * Revision 1.41  2004/01/13 21:02:43  jlmiller
 * Make mapeditor properly prompt on exit.
 *
 * Revision 1.40  2003/12/22 05:59:50  dick
 * Use png(s) instead of xpm(s).
 *
 * Revision 1.39  2003/07/18 20:15:57  jlmiller
 * Fix non-pressable rotate button. Position it with the other map movement
 * buttons.
 *
 * Revision 1.38  2002/09/18 18:41:26  jlmiller
 * Specify a min window size. Fix move buttons so that they don't resize badly.
 *
 * Revision 1.37  2002/09/10 01:22:45  jlmiller
 * Added 'Closing Confirm' dialog boxes to mapeditor and shipeditor.
 *
 * Revision 1.36  2002/09/06 20:32:20  dick
 * Fix missing Auto-cannon tooltip.
 * Whitespace.
 *
 * Revision 1.35  2002/08/22 16:06:30  jlmiller
 * Use Fl_Double_Window to eliminate tooltip flicker.
 *
 * Revision 1.34  2002/08/20 16:32:56  jlmiller
 * Use NELEM() macro
 *
 * Revision 1.33  2002/08/20 10:47:29  jlmiller
 * File & Class Renamings:
 * ViewWindow -> MapEditWindow
 * XpInput -> XPInput
 * xpeTypes -> xpcTypes
 *
 * Revision 1.32  2002/08/15 16:03:16  jlmiller
 * Move MapMoveTools back to left pallete to make mapview area larger.
 *
 * Revision 1.31  2002/07/31 13:18:13  jlmiller
 * We HAVE to call make_current after setting the coordDisp.
 *
 * Revision 1.30  2002/07/30 19:52:59  jlmiller
 * Clipboard buttons update automatically. Sort out a few "selection" bugs. Add a coordinate display.
 *
 * Revision 1.29  2002/07/30 17:23:48  jlmiller
 * Overlay functions become member functions of the drawing windows.
 * Clipboard is its own class.
 *
 * Revision 1.28  2002/07/24 18:31:35  jlmiller
 * Update button structs. Use large size buttons for map block toggles. Much
 * easier to see.
 *
 * Revision 1.27  2002/07/23 22:52:48  jlmiller
 *
 * Disable File_Chooser Icon support for now.
 *
 * Revision 1.26  2002/07/23 19:58:24  jlmiller
 * No, actually the #include "" was right the first time, so lets put it back.
 *
 * Revision 1.25  2002/07/23 19:16:00  jlmiller
 * Fix tooltip typo.
 *
 * Revision 1.24  2002/07/23 18:20:41  jlmiller
 * Arrange Shift Left, Right, Mirror, Rotate etc..controls around ViewWindow for easier access.
 * Similar to Ship Editor.
 *
 * Revision 1.23  2002/07/23 13:04:34  jlmiller
 * Handle user canceling fl_file_chooser.
 *
 * Revision 1.22  2002/07/23 12:27:37  jlmiller
 * Activate image saving routines, which were present but not available before.
 *
 * Revision 1.21  2002/07/12 15:00:06  jlmiller
 * Integrate tabbed main window, with clean up of globals.
 *
 * Revision 1.20  2002/07/10 04:06:47  dick
 * CreateMapWin() becomes MapWindow::Factory()
 *
 * Revision 1.19  2002/06/25 20:20:09  jlmiller
 * Massive restructure to clean up includes, and objectify code more.
 *
 * Revision 1.18  2002/06/25 15:02:19  jlmiller
 * Move iSelection fields to the window, instead of the individual document,
 * as they didn't make any sense at document level.
 *
 * Revision 1.17  2002/06/17 21:00:11  jlmiller
 * Clear undo buffer, if called for.
 *
 * Revision 1.16  2002/06/08 20:45:44  jlmiller
 * Copy, Paste & Drag use undoMan
 *
 * Revision 1.15  2002/06/07 23:10:08  jlmiller
 * Encapsulate almost all map functions into undoMan.
 *
 * Revision 1.14  2002/06/07 20:49:00  jlmiller
 * Begin handling undoMan properly...most functions still broken, expecially RotateMap.
 *
 * Revision 1.13  2002/06/02 22:44:05  jlmiller
 * Add undoMan to shipeditor.
 * Commentability.
 *
 * Revision 1.12  2002/05/30 14:19:26  jlmiller
 * Remove activeMapWindow global, which is unnecessay
 * MapWindow & ShipWindow store documents similarly now.
 *
 * Revision 1.11  2002/05/28 19:29:03  jlmiller
 * lastTeamBase shouldn't be global. Hide the server tab from the preferences
 * while in the mapeditor, since it doesnt work through there anyway.
 *
 * Revision 1.10  2002/05/28 07:19:50  dick
 * Rename xpilotedit to XPilotControl
 *
 */
#include "StdAfx.h"

#ifdef	_WINDOWS
#include <direct.h>
#endif

#ifdef	_UNIX
#include <unistd.h>
#endif

#include "FL/Fl_PNG_Image.H"
#include "FL/Fl_File_Chooser.H"

#include "MapWindow.h"
#include "ServerPrefsWindow.h"
#include "MapUndoableCommand.h"
#include "guiconst.h"
#include "controlconst.h"
#include "config.h"
#include "map.h"
#include "proto.h"
#include "Clipboard.h"
#include "portability.h"
#undef	index
#include "XPFileChooser.h"

#define SPACER 6


static xpe_tbrbutton actionButtonsDefs[numActionButtons] =
{
	{0, BUTTON_SIZE, "Random Map", "randommap", randommap_cb},
	{BUTTON_SIZE*2, BUTTON_SIZE, "Clear", "clear", clearmap_cb},
	// {BUTTON_SIZE*3, BUTTON_SIZE, "Crop", "crop", cropmap_cb},
	{BUTTON_SIZE*3, BUTTON_SIZE, "Round", "roundmap", roundmap_cb},
	{BUTTON_SIZE*4, BUTTON_SIZE, "Grow", "growmap", growmap_cb},
	{BUTTON_SIZE*5, BUTTON_SIZE, "Negative", "negativemap", negativemap_cb},
	{BUTTON_SIZE*7, BUTTON_SIZE, "Zoom In", "zoomin", zoomin_cb},
	{BUTTON_SIZE*8, BUTTON_SIZE, "Zoom Out", "zoomout", zoomout_cb},
	{BUTTON_SIZE*10, BUTTON_SIZE*2, "Map Prefs", "properties", prefs_cb}
};

xpe_tbtbutton toggleMapToolsDefs[numToggleMapTools] =
{
	{"toolspen", IDM_PEN, NULL, "Pen"},
	{"toolseraser", IDM_ERASE, NULL, "Eraser"},
	{"toolsline", IDM_LINE, NULL, "Line"},
	{"toolsshape", IDM_SHAPE, NULL, "Shape"},
	{"toolsselect", IDM_SELECT, NULL, "Select"},
	{"toolsfill", IDM_FILL, NULL, "Fill"},
};

xpe_tbtbutton toggleMapShapesDefs[numToggleMapShapes] =
{
	{"shapecircleempty", IDM_CIRCLEEMPTY, NULL, "Empty Circle"},
	{"shapecirclefilled", IDM_CIRCLEFILLED, NULL, "Filled Circle"},
	{"shaperectempty", IDM_RECTEMPTY, NULL, "Empty Rectangle"},
	{"shaperectfilled", IDM_RECTFILLED, NULL, "Filled Rectangle"},
};

xpe_tbtbutton toggleMapSymButtonsDefs[numToggleMapSymButtons] =
{
	{"solid_lrfilled", MAP_REC_RD, NULL, "Lower Right Solid"},
	{"solid_llfilled", MAP_REC_LD, NULL, "Lower Left Solid"},
	{"solid_filled", MAP_FILLED, NULL, "Full Solid"},
	{"solid_urfilled", MAP_REC_RU, NULL, "Upper Right Solid"},
	{"solid_ulfilled", MAP_REC_LU, NULL, "Lower Left Solid"},
	{"decor_lrfilled", MAP_DEC_RD, NULL, "Lower Right Decor"},
	{"decor_llfilled", MAP_DEC_LD, NULL, "Lower Left Decor"},
	{"decor_filled", MAP_DEC_FLD, NULL, "Full Decor"},
	{"decor_urfilled", MAP_DEC_RU, NULL, "Upper Right Decor"},
	{"decor_ulfilled", MAP_DEC_LU, NULL, "Upper Left Decor"},
	{"fuelcell", MAP_FUEL, NULL, "Fuel Cell"},
	{"cannon_left", MAP_CAN_LEFT, NULL, "Cannon Left"},
	{"cannon_up", MAP_CAN_UP, NULL, "Cannon Up"},
	{"cannon_down", MAP_CAN_DOWN, NULL, "Cannon Down"},
	{"cannon_right", MAP_CAN_RIGHT, NULL, "Cannon Right"},
	{"cannon_quick", IDM_MAP_CAN_UNSPEC, NULL, "Quick Cannon"},
	{"base", MAP_BASE, NULL, "Base"},
	{"base_orient", MAP_BASE_ORNT, NULL, "Base Orient"},
	{"target", MAP_TARGET, NULL, "Target"},
	{"treasure_empty", MAP_EMPTYTREASURE, NULL, "Empty Treasure"},
	{"treasure", MAP_TREASURE, NULL, "Treasure"},
	{"item_conc", MAP_ITEM_CONC, NULL, "Item Concentrator"},
	{"asteroid_conc", MAP_ASTEROID_CONC, NULL, "Asteroid Concentrator"},
	{"gravity_positive", MAP_GRAV_POS, NULL, "Positive Gravity"},
	{"gravity_negative", MAP_GRAV_NEG, NULL, "Negative Gravity"},
	{"gravity_anticlockwise", MAP_GRAV_ACWISE, NULL, "Counterclockwise Gravity"},
	{"gravity_clockwise", MAP_GRAV_CWISE, NULL, "Clockwise Gravity"},
	{"wormhole_normal", MAP_WORM_NORMAL, NULL, "Normal Wormhole"},
	{"wormhole_in", MAP_WORM_IN, NULL, "In Wormhole"},
	{"wormhole_out", MAP_WORM_OUT, NULL, "Out Wormhole"},
	{"current_up", MAP_CRNT_UP, NULL, "Current Up"},
	{"current_left", MAP_CRNT_LT, NULL, "Current Left"},
	{"current_right", MAP_CRNT_RT, NULL, "Current Right"},
	{"current_down", MAP_CRNT_DN, NULL, "Current Down"},
	{"frictionblock", MAP_FRICTION, NULL, "Friction Block"},
	{"empty_space", MAP_SPACE, NULL, "Empty Space"},
	{"teambase", IDM_MAP_TEAMBASE, NULL, "Team Base"},
	{"checkpoint", IDM_MAP_CHECKPOINT, NULL, "Checkpoint"},
};

//Note:
//Prototypes of callbacks are in MapWindow.h
///////////////////////////////////////////////////////////////////////////////
void mapundo_cb(Fl_Widget* w, void* v)
{
	MapWindow *tmp = (MapWindow *) v;
	tmp->undoMan.Undo();
	tmp->mapview->redraw();
}
///////////////////////////////////////////////////////////////////////////////
void mapredo_cb(Fl_Widget* w, void* v)
{
	MapWindow *tmp = (MapWindow *) v;
	tmp->undoMan.Redo();
	tmp->mapview->redraw();
}
///////////////////////////////////////////////////////////////////////////////
void openmap_cb(Fl_Widget*, void* v)
{
	MapWindow*	mw = (MapWindow *) v;
	XPFileChooser*	openmap;
	PCSTR p = NULL;
	PCSTR filt = NULL;
	bool bCont = false;
	int ret;

	Fl::scheme(NULL);

	if (mw->mapDoc.changed)
	{
		ret = fl_choice("Map has unsaved changes. Do you wish to save them?",
			"Save", "Continue", "Cancel");
		switch (ret)
		{
			case 0:
				if (mw->SaveMapFile())
				  bCont = true;
				else
				{
				  bCont = false;
				  return;
				}
				break;
			case 1:
				bCont = true;
				break;
			case 2:
				bCont = false;
				break;
		}
		if (!bCont)
			return;
	}


	chdir(Conf_mapdir());
	openmap = new XPFileChooser(".", "XPilot Map File (*.xp)\tX11 Bitmap (*.xbm)\tPBMPLUS Portable Bitmap (*.pbm)\t", XPFileChooser::SINGLE, "Open");
	openmap->show();
	while (openmap->visible())
		Fl::wait();

	if (openmap->count() == 0)
		return;

	p = openmap->value();
	filt = openmap->filter();

	if (filt != NULL) {
		if (strcmp(filt, "*.xp") == 0) {
			mw->mapDoc.ResetMap();
			mw->mapDoc.LoadMap(p);
		}
		if (strcmp(filt, "*.xbm") == 0) {
			mw->mapDoc.ResetMap();
			mw->mapDoc.LoadXbmFile(p);
		}
		if (strcmp(filt, "*.pbm") == 0) {
			mw->mapDoc.ResetMap();
			mw->mapDoc.LoadPbmFile(p);
		}
		//I Think we should store the file name we came from here.
		mw->mapDoc.optionsDoc->mapFileName->Set(p);

		mw->mapDoc.optionsDoc->OptionsChanged();
		mw->label(mw->mapDoc.optionsDoc->mapName->GetString());
		mw->undoMan.Clear();
		mw->mapview->redraw();
	}
	mw->UpdateClipboardButtons();
}
///////////////////////////////////////////////////////////////////////////////
void saveasmap_cb(Fl_Widget* w, void* v)
{
	MapWindow *tmp = (MapWindow *) v;
	tmp->SaveAsMapFile();
}
///////////////////////////////////////////////////////////////////////////////
void savemap_cb(Fl_Widget* w, void* v)
{
	MapWindow *tmp = (MapWindow *) v;
	tmp->SaveMapFile();
}
///////////////////////////////////////////////////////////////////////////////
void randommap_cb(Fl_Widget*, void* v)
{
	MapWindow *tmp = (MapWindow *) v;
	tmp->mapDoc.WildMap();
	tmp->undoMan.Clear();
	tmp->mapview->ClearOverlay();
	tmp->mapview->redraw();
}
///////////////////////////////////////////////////////////////////////////////
void clearmap_cb(Fl_Widget*, void* v)
{
	MapWindow *tmp = (MapWindow *) v;
	ObjList *objLst = tmp->mapDoc.ClearMapArea(CLEARMAP);

	MapUndoableCommand* uc = new MapUndoableCommand(tmp,
		CHANGEAREA, false, objLst);
	uc->myMapWindow->undoMan.Do(uc);
	tmp->mapview->overlay_rect_clear();
	tmp->mapview->redraw();
}
///////////////////////////////////////////////////////////////////////////////
void cutmap_cb(Fl_Widget*, void* v)
{
	MapWindow *tmp = (MapWindow *) v;
	ObjList *objLst = NULL;
	if (tmp->mapDoc.fSelected)
	{
		objLst = tmp->mapDoc.FillClipData(true);
		MapUndoableCommand* uc = new MapUndoableCommand(tmp,
			CHANGEAREA, false, objLst);
		uc->myMapWindow->undoMan.Do(uc);
		clipboard.fClipboarded = true;
	}
	tmp->mapview->overlay_rect_clear();
	tmp->mapview->redraw();
	tmp->UpdateClipboardButtons();
}
///////////////////////////////////////////////////////////////////////////////
void copymap_cb(Fl_Widget*, void* v)
{
	MapWindow *tmp = (MapWindow *) v;
	if (tmp->mapDoc.fSelected)
	{
		tmp->mapDoc.FillClipData(false);
		clipboard.fClipboarded = true;
	}
	tmp->mapview->overlay_rect_clear();
	tmp->mapview->redraw();
	tmp->UpdateClipboardButtons();
}
///////////////////////////////////////////////////////////////////////////////
void pastemap_cb(Fl_Widget*, void* v)
{
	MapWindow *tmp = (MapWindow *) v;
	tmp->mapDoc.fSelected = false;
	tmp->mapview->fPasting = true;
	tmp->mapview->ClearOverlay();
	tmp->mapview->overlay_rect_clear();
	tmp->mapview->redraw();
}
///////////////////////////////////////////////////////////////////////////////
void cropmap_cb(Fl_Widget*, void* v)
{
	MapWindow *tmp = (MapWindow *) v;
	ObjList *objLst = tmp->mapDoc.ClearMapArea(CROPMAP);

	MapUndoableCommand* uc = new MapUndoableCommand(tmp,
		CHANGEAREA, false, objLst);
	uc->myMapWindow->undoMan.Do(uc);
	tmp->mapview->overlay_rect_clear();
	tmp->mapview->redraw();
}
///////////////////////////////////////////////////////////////////////////////
void roundmap_cb(Fl_Widget*, void* v)
{
	MapWindow *tmp = (MapWindow *) v;

	ObjList *objLst = tmp->mapDoc.RoundMapArea(NULL);

	MapUndoableCommand* uc = new MapUndoableCommand(tmp,
		CHANGEAREA, false, objLst);
	uc->myMapWindow->undoMan.Do(uc);
	tmp->mapview->overlay_rect_clear();
	tmp->mapview->redraw();
}
///////////////////////////////////////////////////////////////////////////////
void growmap_cb(Fl_Widget*, void* v)
{
	MapWindow *tmp = (MapWindow *) v;
	ObjList *objLst = NULL;

	int i;
	for (i = 0; i < 60; i++)
	{
		objLst = tmp->mapDoc.GrowMapArea(objLst);
	}
	MapUndoableCommand* uc = new MapUndoableCommand(tmp,
		CHANGEAREA, false, objLst);
	uc->myMapWindow->undoMan.Do(uc);
	tmp->mapview->overlay_rect_clear();
	tmp->mapview->redraw();
}
///////////////////////////////////////////////////////////////////////////////
void rotatemap_cb(Fl_Widget*, void* v)
{
	MapWindow *tmp = (MapWindow *) v;
	ObjList *objLst = tmp->mapDoc.RotateMapArea();

	MapUndoableCommand* uc = new MapUndoableCommand(tmp,
		CHANGEAREA, tmp->mapDoc.fSelected, objLst);

	uc->myMapWindow->undoMan.Do(uc);
	tmp->mapview->overlay_rect_clear();
	tmp->mapview->redraw();
}
///////////////////////////////////////////////////////////////////////////////
void mirrormapv_cb(Fl_Widget*, void* v)
{
	MapWindow *tmp = (MapWindow *) v;
	ObjList *objLst = tmp->mapDoc.MirrorMap(true);

	MapUndoableCommand* uc = new MapUndoableCommand(tmp,
		CHANGEAREA, false, objLst);
	uc->myMapWindow->undoMan.Do(uc);
	tmp->mapview->overlay_rect_clear();
	tmp->mapview->redraw();
}
///////////////////////////////////////////////////////////////////////////////
void mirrormaph_cb(Fl_Widget*, void* v)
{
	MapWindow *tmp = (MapWindow *) v;
	ObjList *objLst = tmp->mapDoc.MirrorMap(false);

	MapUndoableCommand* uc = new MapUndoableCommand(tmp,
		CHANGEAREA, false, objLst);
	uc->myMapWindow->undoMan.Do(uc);
	tmp->mapview->overlay_rect_clear();
	tmp->mapview->redraw();
}
///////////////////////////////////////////////////////////////////////////////
void cyclemapl_cb(Fl_Widget*, void* v)
{
	MapWindow *tmp = (MapWindow *) v;
	ObjList *objLst = tmp->mapDoc.CycleMapHorizontal(true);

	MapUndoableCommand* uc = new MapUndoableCommand(tmp,
		CHANGEAREA, false, objLst);
	uc->myMapWindow->undoMan.Do(uc);
	tmp->mapview->overlay_rect_clear();
	tmp->mapview->redraw();
}
///////////////////////////////////////////////////////////////////////////////
void cyclemapr_cb(Fl_Widget*, void* v)
{
	MapWindow *tmp = (MapWindow *) v;
	ObjList *objLst = tmp->mapDoc.CycleMapHorizontal(false);

	MapUndoableCommand* uc = new MapUndoableCommand(tmp,
		CHANGEAREA, false, objLst);
	uc->myMapWindow->undoMan.Do(uc);
	tmp->mapview->overlay_rect_clear();
	tmp->mapview->redraw();
}
///////////////////////////////////////////////////////////////////////////////
void cyclemapu_cb(Fl_Widget*, void* v)
{
	MapWindow *tmp = (MapWindow *) v;
	ObjList *objLst = tmp->mapDoc.CycleMapVertical(true);

	MapUndoableCommand* uc = new MapUndoableCommand(tmp,
		CHANGEAREA, false, objLst);
	uc->myMapWindow->undoMan.Do(uc);
	tmp->mapview->overlay_rect_clear();
	tmp->mapview->redraw();
}
///////////////////////////////////////////////////////////////////////////////
void cyclemapd_cb(Fl_Widget*, void* v)
{
	MapWindow *tmp = (MapWindow *) v;
	ObjList *objLst = tmp->mapDoc.CycleMapVertical(false);

	MapUndoableCommand* uc = new MapUndoableCommand(tmp,
		CHANGEAREA, false, objLst);
	uc->myMapWindow->undoMan.Do(uc);
	tmp->mapview->overlay_rect_clear();
	tmp->mapview->redraw();
}
///////////////////////////////////////////////////////////////////////////////
void negativemap_cb(Fl_Widget*, void* v)
{
	MapWindow *tmp = (MapWindow *) v;
	ObjList *objLst = tmp->mapDoc.NegativeMap();

	MapUndoableCommand* uc = new MapUndoableCommand(tmp,
		CHANGEAREA, false, objLst);
	uc->myMapWindow->undoMan.Do(uc);
	tmp->mapview->overlay_rect_clear();
	tmp->mapview->redraw();
}
///////////////////////////////////////////////////////////////////////////////
void zoomin_cb(Fl_Widget*, void* v)
{
	MapWindow* mw = (MapWindow *) v;

	if (!mw->mapview->ZoomIn())
	{
		mw->mapDoc.ptTempBeg.x = mw->mapDoc.ptTempBeg.y =
			mw->mapDoc.ptTempEnd.x = mw->mapDoc.ptTempEnd.y = 0;
		mw->mapview->overlay_rect_clear();
		mw->mapview->redraw();
	}
}
///////////////////////////////////////////////////////////////////////////////
void zoomout_cb(Fl_Widget*, void* v)
{
	MapWindow* mw = (MapWindow *) v;

	if (!mw->mapview->ZoomOut())
	{
		mw->mapview->overlay_rect_clear();
		mw->mapview->redraw();
	}
}
///////////////////////////////////////////////////////////////////////////////
void prefs_cb(Fl_Widget*, void* v)
{
	MapWindow* mw = (MapWindow *) v;
	ServerPrefsWindow* spw = CreateServerPrefsWindow(mw->mapDoc.optionsDoc);
	mw->mapview->redraw();
	mw->mapDoc.optionsDoc->OptionsChanged();

	spw->RemoveTab(TabServer); //Don't show the Server Tab if we're editing a map from the editor

	mw->redraw();
}
///////////////////////////////////////////////////////////////////////////////
void msb_cb(Fl_Widget*, void* v)
{
	MapWindow* mw = (MapWindow *) v;
	mw->mapview->iVscrollPos = mw->sb_v->value();
	mw->mapview->iHscrollPos = mw->sb_h->value();
	mw->mapview->redraw();
}
///////////////////////////////////////////////////////////////////////////////
void maptogglebar_cb(Fl_Widget *w, void* v)
{
	MapWindow *tmp = (MapWindow *) v;
	FunctionToggleButton *btn = (FunctionToggleButton *) w;

	*btn->togglevar = btn->id;
	if (tmp->mapDoc.fSelected == true)
	{
		tmp->mapDoc.fSelected = false;
		tmp->mapDoc.ptTempBeg.x = tmp->mapDoc.ptTempBeg.y =
			tmp->mapDoc.ptTempEnd.x = tmp->mapDoc.ptTempEnd.y = 0;
		tmp->mapview->overlay_rect_clear();
		tmp->mapview->redraw();
		tmp->UpdateClipboardButtons();
	}
	if (btn->togglevar == &tmp->iSelectionTools)
		if (btn->id == IDM_SHAPE)
			tmp->gpShapes->show();
		else
			tmp->gpShapes->hide();
}

///////////////////////////////////////////////////////////////////////////////
void mapclose_cb(Fl_Widget* win, void* v)
{
	MapWindow* w = (MapWindow*)v;
	bool close = false;
	int ret;

	if (w->mapDoc.changed)
	{
		ret = fl_choice("Map has unsaved changes. Do you still wish to quit?",
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
				if (w->SaveMapFile())
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
MapWindow* MapWindow::Factory()
{
	MapWindow *window = new MapWindow(800, 700, "New Map Window");
	window->resizable(window->mapview);
	window->size_range(800, 700, 0, 0);
	window->end();
	window->callback((Fl_Callback *)mapclose_cb, window);

	//Comes Highly recommended from the FLTK guys, so why not?
	Fl::visual(FL_DOUBLE|FL_INDEX);

	window->show();
	window->make_current();
	return(window);
}

#define MAPTOOLSTOP BUTTON_SIZE*4
#define MAPSHAPESTOP BUTTON_SIZE*5
#define MAPSYMSTOP BUTTON_SIZE*8
///////////////////////////////////////////////////////////////////////////////
MapWindow::MapWindow(int w,int h,const char *l)
        : Fl_Double_Window(w,h,l)
{
	int i, x2, y2;
	int num_buttons = 0;

	//Initialize the selection holders
	//This should be done BEFORE the buttons are created, so the default
	//button gets selected.
	iSelectionTools = IDM_PEN;
	iSelectionMapSyms = MAP_FILLED;
	iSelectionShape = IDM_CIRCLEEMPTY;
	lastTeamBase = '0';

	mapview = new MapEditWindow(250, 0, w-SCROLLBAR_SIZE-250, h-SCROLLBAR_SIZE, NULL);
	mapview->end();

	mapview->pMap = &mapDoc;

	sb_v = new Fl_Scrollbar(mapview->x()+mapview->w(), mapview->y(), SCROLLBAR_SIZE, mapview->h(), NULL);
	sb_v->callback(msb_cb);
	sb_v->linesize(1);
	sb_v->user_data(this);

	sb_h = new Fl_Scrollbar(mapview->x(), mapview->y()+mapview->h(), mapview->w(), SCROLLBAR_SIZE, NULL);
	sb_h->type(FL_HORIZONTAL);
	sb_h->callback(msb_cb);
	sb_h->linesize(1);
	sb_h->user_data(this);

	Fl_Group* o = new Fl_Group(0, 0,  11*BUTTON_SIZE, h, "");

	openButton = new FunctionButton(0, 0, BUTTON_SIZE, BUTTON_SIZE,
        "", "fileopen");
	openButton->callback(openmap_cb);
	openButton->tooltip("Open Map");
	openButton->user_data(this);

	saveButton = new FunctionButton(BUTTON_SIZE, 0, BUTTON_SIZE, BUTTON_SIZE,
        "", "filesave");
	saveButton->callback(savemap_cb);
	saveButton->tooltip("Save");
	saveButton->user_data(this);

	FunctionButton *but = new FunctionButton(BUTTON_SIZE*2, 0, BUTTON_SIZE, BUTTON_SIZE,
        "", "filesaveas");
	but->callback(saveasmap_cb);
	but->tooltip("Save As..");
	but->user_data(this);

	cutButton = new FunctionButton(BUTTON_SIZE*4, 0, BUTTON_SIZE, BUTTON_SIZE,
        "", "editcut");
	cutButton->callback(cutmap_cb);
	cutButton->tooltip("Cut");
	cutButton->user_data(this);

	copyButton = new FunctionButton(BUTTON_SIZE*5, 0, BUTTON_SIZE, BUTTON_SIZE,
        "", "editcopy");
	copyButton->callback(copymap_cb);
	copyButton->tooltip("Copy");
	copyButton->user_data(this);

	pasteButton = new FunctionButton(BUTTON_SIZE*6, 0, BUTTON_SIZE, BUTTON_SIZE,
        "", "editpaste");
	pasteButton->callback(pastemap_cb);
	pasteButton->tooltip("Paste");
	pasteButton->user_data(this);

	cropButton = new FunctionButton(BUTTON_SIZE*7, 0, BUTTON_SIZE, BUTTON_SIZE,
        "", "crop");
	cropButton->callback(cropmap_cb);
	cropButton->tooltip("Crop");
	cropButton->user_data(this);

	undoButton = new FunctionRepeatButton(BUTTON_SIZE*9, 0, BUTTON_SIZE, BUTTON_SIZE,
        "", "undo");
	undoButton->callback(mapundo_cb);
	undoButton->tooltip("Undo");
	undoButton->user_data(this);

	redoButton = new FunctionRepeatButton(BUTTON_SIZE*10, 0, BUTTON_SIZE, BUTTON_SIZE,
        "", "redo");
	redoButton->callback(mapredo_cb);
	redoButton->tooltip("Redo");
	redoButton->user_data(this);

	undoMan.RegisterButtons(UndoButtonEnabler,
							RedoButtonEnabler,
							SaveButtonEnabler, this);

	for (i = 0; i < numActionButtons; i++)
	{
		actionButtons[i] = new FunctionRepeatButton(
					actionButtonsDefs[i].x, actionButtonsDefs[i].y,
					BUTTON_SIZE, BUTTON_SIZE, "", actionButtonsDefs[i].pm);
		actionButtons[i]->callback(actionButtonsDefs[i].cb);
		actionButtons[i]->tooltip(actionButtonsDefs[i].ttip);
		actionButtons[i]->user_data(this);
	}

	o = new Fl_Group(0, MAPTOOLSTOP,  numToggleMapTools * BUTTON_SIZE, BUTTON_SIZE, "");
	for (i=0; i < numToggleMapTools; i++)
	{
		toggleMapTools[i] = new FunctionToggleButton(i*BUTTON_SIZE, MAPTOOLSTOP,
			BUTTON_SIZE, BUTTON_SIZE, "", toggleMapToolsDefs[i].pm);
		toggleMapTools[i]->id = toggleMapToolsDefs[i].id;
		toggleMapTools[i]->togglevar = &iSelectionTools;
		toggleMapTools[i]->type(FL_RADIO_BUTTON);

		toggleMapTools[i]->callback(maptogglebar_cb);
		toggleMapTools[i]->tooltip(toggleMapToolsDefs[i].ttip);
		toggleMapTools[i]->user_data(this);
		if (toggleMapTools[i]->id == iSelectionTools)
			toggleMapTools[i]->value(true);
	}
	o->end();
    Fl_Group::current()->resizable(o);

	gpShapes = new Fl_Group(0, MAPSHAPESTOP,  numToggleMapShapes * BUTTON_SIZE, BUTTON_SIZE, "");
	for (i=0; i < numToggleMapShapes; i++)
	{
		toggleMapShapes[i] = new FunctionToggleButton(i*BUTTON_SIZE, MAPSHAPESTOP,
			BUTTON_SIZE, BUTTON_SIZE, "", toggleMapShapesDefs[i].pm);
		toggleMapShapes[i]->id = toggleMapShapesDefs[i].id;
		toggleMapShapes[i]->togglevar = &iSelectionShape;
		toggleMapShapes[i]->type(FL_RADIO_BUTTON);

		toggleMapShapes[i]->callback(maptogglebar_cb);
		toggleMapShapes[i]->tooltip(toggleMapShapesDefs[i].ttip);
		toggleMapShapes[i]->user_data(this);
		if (toggleMapShapes[i]->id == iSelectionShape)
			toggleMapShapes[i]->value(true);
	}
	gpShapes->end();
    Fl_Group::current()->resizable(gpShapes);

	//Hide Shapes toolbar by default
	gpShapes->hide();

	//We have 5 columns and 8 rows.
	o = new Fl_Group(0, MAPSYMSTOP, BUTTON_SIZE_LG * 5,
		8 * BUTTON_SIZE_LG, "");
	for (i=0, x2=0, y2=0; i < numToggleMapSymButtons; i++, x2++)
	{
		if (x2 > 4)
		{
			x2 = 0;
			y2++;
		}
		toggleMapSymButtons[i] = new FunctionToggleButton(
			x2*BUTTON_SIZE_LG,
			MAPSYMSTOP+y2*BUTTON_SIZE_LG,
			BUTTON_SIZE_LG, BUTTON_SIZE_LG, "", toggleMapSymButtonsDefs[i].pm);
		toggleMapSymButtons[i]->id = toggleMapSymButtonsDefs[i].id;
		toggleMapSymButtons[i]->togglevar = &iSelectionMapSyms;
		toggleMapSymButtons[i]->type(FL_RADIO_BUTTON);

		toggleMapSymButtons[i]->callback(maptogglebar_cb);
		toggleMapSymButtons[i]->tooltip(toggleMapSymButtonsDefs[i].ttip);
		toggleMapSymButtons[i]->user_data(this);
		if (toggleMapSymButtons[i]->id == iSelectionMapSyms)
			toggleMapSymButtons[i]->value(true);

	}
	o->end();
	Fl_Group::current()->resizable(o);
	CreateMapMoveControls();
	o->end();
	o->resizable(NULL);

	coordDisp = new Fl_Output(0, h-20, 220, 20, NULL);
	UpdateClipboardButtons();
};

///////////////////////////////////////////////////////////////////////////////
void MapWindow::UpdateClipboardButtons()
{
	cutButton->deactivate();
	copyButton->deactivate();
	pasteButton->deactivate();
	cropButton->deactivate();

	if (mapDoc.fSelected)
	{
		cutButton->activate();
		copyButton->activate();
		cropButton->activate();
	}

	if (clipboard.fClipboarded)
		pasteButton->activate();
}

///////////////////////////////////////////////////////////////////////////////
MapWindow::~MapWindow()
{
	delete mapview;
}
///////////////////////////////////////////////////////////////////////////////
int MapWindow::handle(int event)
{
	switch (event)
	{
	case FL_FOCUS:
		mapview->make_current();
		mapview->redraw();
		return TRUE;
	}
	return Fl_Window::handle(event);
}
///////////////////////////////////////////////////////////////////////////////
void MapWindow::draw()
{
	Fl_Window::draw();
}
///////////////////////////////////////////////////////////////////////////////
void MapWindow::UndoButtonEnabler(void* myThis, bool enable)
{
	MapWindow* cfg = (MapWindow*)myThis;
	if (enable)
		cfg->undoButton->activate();
	else
		cfg->undoButton->deactivate();
}

///////////////////////////////////////////////////////////////////////////////
void MapWindow::RedoButtonEnabler(void* myThis, bool enable)
{
	MapWindow* cfg = (MapWindow*)myThis;
	if (enable)
		cfg->redoButton->activate();
	else
		cfg->redoButton->deactivate();
}

///////////////////////////////////////////////////////////////////////////////
void MapWindow::SaveButtonEnabler(void* myThis, bool enable)
{
	MapWindow* cfg = (MapWindow*)myThis;
	if (enable)
		cfg->saveButton->activate();
	else
		cfg->saveButton->deactivate();
}
///////////////////////////////////////////////////////////////////////////////
void MapWindow::CreateMapMoveControls()
{
	FunctionRepeatButton *o = NULL;
	FunctionButton *p = NULL;

	Fl_Group *g = new Fl_Group(BUTTON_SIZE, BUTTON_SIZE*24, BUTTON_SIZE*10, BUTTON_SIZE*8, "");
	o = new FunctionRepeatButton(BUTTON_SIZE, BUTTON_SIZE*25, BUTTON_SIZE, BUTTON_SIZE,
		"", "cyclel");
	o->callback(cyclemapl_cb);
	o->user_data(this);
	o->tooltip("Shift Left");

	o = new FunctionRepeatButton(BUTTON_SIZE*3, BUTTON_SIZE*25, BUTTON_SIZE, BUTTON_SIZE,
		"", "cycler");
	o->callback(cyclemapr_cb);
	o->user_data(this);
	o->tooltip("Shift Right");

	o = new FunctionRepeatButton(BUTTON_SIZE*2, BUTTON_SIZE*24, BUTTON_SIZE, BUTTON_SIZE,
		"", "cycleu");
	o->callback(cyclemapu_cb);
	o->user_data(this);
	o->tooltip("Shift Up");

	o = new FunctionRepeatButton(BUTTON_SIZE*2, BUTTON_SIZE*26, BUTTON_SIZE, BUTTON_SIZE,
		"", "cycled");
	o->callback(cyclemapd_cb);
	o->user_data(this);
	o->tooltip("Shift Down");

	p = new FunctionButton(BUTTON_SIZE*5, BUTTON_SIZE*25, BUTTON_SIZE, BUTTON_SIZE,
		"", "mirrorh");
	p->callback(mirrormaph_cb);
	p->user_data(this);
	p->tooltip("Mirror Horizontally");

	p = new FunctionButton(BUTTON_SIZE*7, BUTTON_SIZE*25, BUTTON_SIZE, BUTTON_SIZE,
		"", "mirrorv");
	p->callback(mirrormapv_cb);
	p->user_data(this);
	p->tooltip("Mirror Vertically");

	o = new FunctionRepeatButton(BUTTON_SIZE*9,
		BUTTON_SIZE*25, BUTTON_SIZE, BUTTON_SIZE,
		"", "rotate90");
	o->callback(rotatemap_cb);
	o->user_data(this);
	o->tooltip("Rotate Right");


	g->end();
	g->resizable(NULL);
}
///////////////////////////////////////////////////////////////////////////////
void MapWindow::SetCoordDisp(PCSTR fmt, ...)
{
	va_list ap;
	char	s[512];

	va_start(ap, fmt);
	vsnprintf(s, 511, fmt, ap);
	coordDisp->value(s);
	va_end(ap);
	Fl::check();
};
///////////////////////////////////////////////////////////////////////////////
bool MapWindow::SaveAsMapFile()
{
	XPFileChooser*	saveas;
	PCSTR p = NULL;
	PCSTR filt = NULL;
	bool rValue = false;

	Fl::scheme(NULL);
	saveas = new XPFileChooser(".", "XPilot Map File (*.xp)\tX11 Pixmap (*.xpm)\tX11 Bitmap (*.xbm)\tPBMPLUS Portable Bitmap (*.pbm)\tPBMPLUS Portable Pixmap Binary (*.ppm)", XPFileChooser::CREATE, "Save As");
	saveas->show();
	while (saveas->visible())
		Fl::wait();

	if (saveas->count() == 0)
		return false;

	p = saveas->value();
	filt = saveas->filter();

	if(TestFileOverwrite(p))
	{
		if (filt != NULL) {
			if (strcmp(filt, "*.xp") == 0) {
				if (!(rValue = mapDoc.SaveMap(p)))
				{
					label(mapDoc.optionsDoc->mapName->GetString());
					mapDoc.changed = false;
					rValue = true;
				}
			}
			else if (strcmp(filt, "*.xbm") == 0) {
				rValue = mapDoc.SaveXbmFile(p);
			}
			else if (strcmp(filt, "*.xpm") == 0) {
				rValue = mapDoc.SaveXpmFile(p);
			}
			else if (strcmp(filt, "*.pbm") == 0) {
				rValue = mapDoc.SavePbmPlusFile(p, 0);
			}
			else if (strcmp(filt, "*.ppm") == 0) {
				rValue = mapDoc.SavePbmPlusFile(p, 2);
			}
		}
	}

	mapview->redraw();
	return rValue;
}
///////////////////////////////////////////////////////////////////////////////
bool MapWindow::SaveMapFile()
{
	char szTempFileName[255+2] ;
	strcpy(szTempFileName, mapDoc.optionsDoc->mapFileName->GetString());

	if (szTempFileName[0])
	{
		if (!mapDoc.SaveMap(szTempFileName))
		{
			label(mapDoc.optionsDoc->mapName->GetString());
			mapDoc.changed = false;
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
		return SaveAsMapFile();
	}
}
