/* $Id: Fl_KbConfig.h,v 1.8 2004/05/23 23:52:07 dick Exp $
 *
 * Fl_KbConfig - Display a picture of a keyboard with all of the keys defined
 *
 *      Copyright (C) 2001 by
 *
 *      The XPilot Authors   <xpilot@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
 *      Jarrod Miller        <jarrod@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
 *      Bjørn Stabell        <bjoern@xpilot.org>
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
 * $Log: Fl_KbConfig.h,v $
 * Revision 1.8  2004/05/23 23:52:07  dick
 * src/common/IniClient.Defaults.cpp
 *
 * Revision 1.7  2002/07/08 06:23:40  dick
 * Don't include keys.h here
 *
 * Revision 1.6  2001/10/09 15:47:47  dick
 * Wrap debug printf statements with DEBUGEVENTS()
 *
 * Revision 1.5  2001/10/04 16:08:15  dick
 * Add an array of neverUsed commands, so they don't get in the Unused key.
 *
 * Revision 1.4  2001/10/01 13:49:56  dick
 * wrap printfs with DEBUGEVENTS and DEBUGSELECTION to control the noise.
 *
 * Revision 1.3  2001/09/27 17:43:51  dick
 * Create the Undoable event and act on it based on drag n drop.
 * Start work on "unused" and "copy" buttons.
 * Handle the Save button.
 *
 * Revision 1.2  2001/09/25 15:45:52  dick
 * Split KbConfig to KbConfig and KbConfigKb, since the keyboard is only a part
 * of the whole KbConfig window.
 *
 * Revision 1.1  2001/09/20 07:41:02  dick
 * Move Fl_Kb* to it's own library xp-flwidgets/kbconfig/libxp-kbconfig.a
 *
 * Revision 1.6  2001/09/17 08:57:35  dick
 * Draw the first command associated with each key.  Move Button to his own file.
 *
 * Revision 1.5  2001/09/07 21:24:03  dick
 * Handle mouse move events and highlight the key the mouse is over.
 *
 * Revision 1.4  2001/09/07 07:41:17  dick
 * Hightlight the key pressed.
 *
 * Revision 1.3  2001/09/05 13:16:21  dick
 * Draw a full generic US keyboard
 *
 * Revision 1.2  2001/09/05 09:27:27  dick
 * XPKbConfig on Linux.
 *
 * Revision 1.1  2001/09/05 09:15:41  dick
 * Display a picture of a keyboard with all of the keys defined.
 *
 */

#ifndef	_FL_KBCONFIG_H_
#define	_FL_KBCONFIG_H_

#include "FL/Fl_Menu_Button.H"
#include "FL/Fl_Window.H"
#include "FL/Fl_Double_Window.H"

#include "cstring.h"
#include "Obj.h"
#include "UndoMan.h"

#if 0
#define	DEBUGEVENTS(__d)	__d
#else
#define	DEBUGEVENTS(__d)
#endif

#if 0
#define	DEBUGSELECTION(__d)	__d
#else
#define	DEBUGSELECTION(__d)
#endif


class Fl_KbCButton;
class KbCKey;
class Fl_KbActionButton;
class Fl_KbDragger;
class Fl_KbConfigKb;

#define	Fl_KbConfigSUPERCLASS	Fl_Double_Window
class Fl_KbConfig : public Fl_KbConfigSUPERCLASS
{
public:
	Fl_KbConfig(int X, int Y, int W, int H, const char* label = 0);
	Fl_KbConfig(int W, int H, const char* label = 0);

	virtual	int		handle(int event);

	void			ActionButtonHandler(Fl_KbActionButton* b);
	static void		UndoButtonEnabler(void* myThis, bool enable);
	static void		RedoButtonEnabler(void* myThis, bool enable);
	static void		SaveButtonEnabler(void* myThis, bool enable);

	Fl_KbCButton*	unusedCommandsButton;
	Fl_KbCButton*	copyCommandsButton;

	Fl_KbDragger*	drag;
	Fl_KbConfigKb*		kb;

private:
	Fl_Menu_Button*		currentKeyboard;
	Fl_KbActionButton*	undoButton;
	Fl_KbActionButton*	redoButton;
	Fl_KbActionButton*	saveButton;
	Fl_KbActionButton*	cancelButton;

	UndoMan				undoMan;
};

Fl_KbConfig* CreateKbConfigWindow(PCSTR windowTitle);


/*
 * Each possible command in a fashion that we want it.
 */
class KeyCommand
{
public:
	PCSTR name;
	uchar*	bits;
	bool	unused;

};
extern KeyCommand	keyCommands[];	// Note that these are 1:1 with keys_t

extern PCSTR		sepa;
extern const int	numKeyCommands;
extern		 int	neverUsed[];
extern const int	numNeverUsed;

extern PCSTR		eventName[];	// fltk event names, for debugging

#endif		// _FL_KBCONFIG_H_
