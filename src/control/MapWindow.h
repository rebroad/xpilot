/* $Id: MapWindow.h,v 1.24 2004/06/01 23:13:00 dick Exp $
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
 * $Log: MapWindow.h,v $
 * Revision 1.24  2004/06/01 23:13:00  dick
 * You can't really make a new button from an object and stick it in a static.
 * (Who owns it if you open 2 MapEditors?)  So now each window keeps track of
 * his own buttons.
 *
 * Revision 1.23  2004/05/22 15:22:18  dick
 * Whitespace
 *
 * Revision 1.22  2004/02/21 06:57:32  dick
 * In the mapeditor, the prefs should have no knowledge of a ServerOptionsNetClient.
 *
 * Revision 1.21  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.20  2004/01/13 21:02:43  jlmiller
 * Make mapeditor properly prompt on exit.
 *
 * Revision 1.19  2002/08/22 16:06:30  jlmiller
 * Use Fl_Double_Window to eliminate tooltip flicker.
 *
 * Revision 1.18  2002/08/20 10:47:29  jlmiller
 * File & Class Renamings:
 * ViewWindow -> MapEditWindow
 * XpInput -> XPInput
 * xpeTypes -> xpcTypes
 *
 * Revision 1.17  2002/07/30 19:52:59  jlmiller
 * Clipboard buttons update automatically. Sort out a few "selection" bugs. Add a coordinate display.
 *
 * Revision 1.16  2002/07/24 18:31:35  jlmiller
 * Update button structs. Use large size buttons for map block toggles. Much
 * easier to see.
 *
 * Revision 1.15  2002/07/23 19:58:24  jlmiller
 * No, actually the #include "" was right the first time, so lets put it back.
 *
 * Revision 1.14  2002/07/23 18:20:41  jlmiller
 * Arrange Shift Left, Right, Mirror, Rotate etc..controls around ViewWindow for easier access.
 * Similar to Ship Editor.
 *
 * Revision 1.13  2002/07/23 13:24:12  jlmiller
 * #includes should be <> instead of ""
 *
 * Revision 1.12  2002/07/12 15:00:06  jlmiller
 * Integrate tabbed main window, with clean up of globals.
 *
 * Revision 1.11  2002/07/10 04:06:47  dick
 * CreateMapWin() becomes MapWindow::Factory()
 *
 */

#ifndef	_MAPWINDOW_H_
#define	_MAPWINDOW_H_

#include "FL/Fl_Scrollbar.H"
#include "FL/Fl_Output.H"
#include "MapEditWindow.h"
#include "xpcTypes.h"
#include "UndoMan.h"
#include "ServerPrefsWindow.h"

const int numActionButtons		=  8;
const int numToggleMapTools		=  6;
const int numToggleMapShapes	=  4;
const int numToggleMapSymButtons= 38;

class MapWindow : public Fl_Double_Window {
public:
    MapWindow(int w,int h,const char *l=0);
	virtual	~MapWindow();
	static MapWindow*	Factory();		// Create and return an open MapWindow

    void			activate();
    void			draw();
    int				handle(int);

	static void		UndoButtonEnabler(void* myThis, bool enable);
	static void		RedoButtonEnabler(void* myThis, bool enable);
	static void		SaveButtonEnabler(void* myThis, bool enable);

	void			CreateMapMoveControls();
	void			UpdateClipboardButtons();
	void			SetCoordDisp(PCSTR text, ...);

	bool			SaveMapFile();
	bool			SaveAsMapFile();

public:
	MapDocument		mapDoc;
    MapEditWindow*	mapview;

	int				iSelectionTools;	//The currently selected drawing tool
	int				iSelectionMapSyms;	//The currently selected map icon
	int				iSelectionShape;	//The currently selected shape.
	char			lastTeamBase;

    Fl_Scrollbar*	sb_v;
    Fl_Scrollbar*	sb_h;
	Fl_Group*		gpShapes;

	UndoMan			undoMan;

private:
	FunctionRepeatButton*	actionButtons[numActionButtons];
	FunctionToggleButton*	toggleMapTools[numToggleMapTools];
	FunctionToggleButton*	toggleMapShapes[numToggleMapShapes];
	FunctionToggleButton*	toggleMapSymButtons[numToggleMapSymButtons];

	FunctionButton*			openButton;
	FunctionButton*			saveButton;
	FunctionRepeatButton*	undoButton;
	FunctionRepeatButton*	redoButton;

	FunctionButton*			cutButton;
	FunctionButton*			copyButton;
	FunctionButton*			pasteButton;
	FunctionButton*			cropButton;

	Fl_Output*				coordDisp;
};

void openmap_cb(Fl_Widget* w, void*);
void saveasmap_cb(Fl_Widget*, void*);
void savemap_cb(Fl_Widget*, void*);
void randommap_cb(Fl_Widget*, void*);
void clearmap_cb(Fl_Widget*, void*);
void cutmap_cb(Fl_Widget*, void*);
void copymap_cb(Fl_Widget*, void*);
void pastemap_cb(Fl_Widget*, void*);
void cropmap_cb(Fl_Widget*, void*);
void roundmap_cb(Fl_Widget*, void*);
void growmap_cb(Fl_Widget*, void*);
void rotatemap_cb(Fl_Widget*, void*);
void mirrormapv_cb(Fl_Widget*, void*);
void mirrormaph_cb(Fl_Widget*, void*);
void cyclemapl_cb(Fl_Widget*, void*);
void cyclemapr_cb(Fl_Widget*, void*);
void cyclemapu_cb(Fl_Widget*, void*);
void cyclemapd_cb(Fl_Widget*, void*);
void negativemap_cb(Fl_Widget*, void*);
void zoomin_cb(Fl_Widget*, void*);
void zoomout_cb(Fl_Widget*, void*);
void prefs_cb(Fl_Widget*, void*);
void msb_cb(Fl_Widget*, void*);
void actvbtn_cb(Fl_Widget* w, void*);

#endif	// _MAPWINDOW_H_
