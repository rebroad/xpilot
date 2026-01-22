/* $Id: SoundWindow.h,v 1.2 2007/02/17 19:31:14 dick Exp $
 *
 * SoundWindow - The ShipEditor 'applet'
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
 * $Log: SoundWindow.h,v $
 * Revision 1.2  2007/02/17 19:31:14  dick
 * Fix friend delaration for gcc
 *
 * Revision 1.1  2007/02/17 06:13:10  dick
 * A Sound Editor.  Connects events to sounds
 *
 */

#ifndef	_SOUNDWINDOW_H_
#define	_SOUNDWINDOW_H_

#include "FL/Fl_Double_Window.H"

//#include "ShipEditWindow.h"
#include "Fl_Status.h"
//#include "SoundUndoableCommand.h"
//#include "ShipListViewWindow.h"
//#include "XPInput.h"
#include "xpcTypes.h"
#include "UndoMan.h"

const int numSoundActionButtons = 3;
const int numToggleSoundToolButtons = 4;
const int numToggleSoundSymButtons = 10;

class	Fl_MainGainSlider;
class	Fl_SoundEventList;
class	Fl_SoundList;
class	Fl_EventSoundConnector;
class	SoundUndoableCommand;

class SoundWindow : public Fl_Double_Window {
	friend class SoundUndoableCommand;

  public:
    SoundWindow(int w,int h,const char *l=0);
	static SoundWindow*	Factory();

	static void		UndoButtonEnabler(void* myThis, bool enable);
	static void		RedoButtonEnabler(void* myThis, bool enable);
	static void		SaveButtonEnabler(void* myThis, bool enable);
	void			GainChanged(int value);
	void			DisplaySelectedEvent();
	void			DisplaySelectedSound();
	void			ScrollUpdate();

  public:
    // virtual int			handle(int);

	void				SetStatus(PCSTR text, ...);
	bool				SaveSoundFile();
	bool				SaveAsSoundFile();

	FunctionRepeatButton*	undoButton;
	FunctionRepeatButton*	redoButton;
	FunctionButton*			saveButton;
	FunctionButton*			saveAsButton;
/*	int					iSelectionShipSyms;
	int					iSelectionShipTools;
	XPInput*			nameInp;
	XPInput*			authorInp;
	Fl_Output*			itemCount;

	FunctionButton*			openButton;

	FunctionRepeatButton*	shiftLeft;
	FunctionRepeatButton*	shiftRight;
	FunctionRepeatButton*	shiftUp;
	FunctionRepeatButton*	shiftDown;
	FunctionButton*			mirrorVert;
	FunctionButton*			mirrorHoriz;
	FunctionRepeatButton*	rotateRight;
	FunctionRepeatButton*	rotateLeft;
	Fl_Output*				coordDisp;
*/
//	ShipListViewWindow*		shiplistview;
//	ShipEditWindow*			shipedit;
//	ShipList				Shplst;
//	SoundDocument*			doc;

	UndoMan					undoMan;
private:
	bool					iniClientChanged;
	Fl_SoundEventList*		flSoundEventList;
	Fl_SoundList*			flSoundList;
	Fl_EventSoundConnector*	flEventSoundConnector;
	Fl_Status*				status;
	FunctionRepeatButton*	soundActionButtons[numSoundActionButtons];
	FunctionToggleButton*	toggleSoundToolButtons[numToggleSoundToolButtons];
	FunctionToggleButton*	toggleSoundSymButtons[numToggleSoundSymButtons];
	Fl_MainGainSlider*		mainGain;
};

#endif	// _SHIPWINDOW_H_
