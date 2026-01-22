/* $Id: Fl_KbCButton.h,v 1.4 2001/10/04 16:29:37 dick Exp $
 *
 * Fl_KbCButton - Handle a single button/key on the Fl_KbConfig keyboard
 *
 *      Copyright (C) 2001 by
 *
 *      The XPilot Authors   <xpilot@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
 *      Jarrod Miller        <jarrod@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
 *      Bj�rn Stabell        <bjoern@xpilot.org>
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
 * $Log: Fl_KbCButton.h,v $
 * Revision 1.4  2001/10/04 16:29:37  dick
 * Better destruction of multiwindow (handles Unused having a different parent)
 *
 * Revision 1.3  2001/09/27 17:46:43  dick
 * Use new RTTI type (fltk userdata).  Move ButtonType to Fl_KbConfig.h
 * Stregthen multiwindow handling.
 *
 * Revision 1.2  2001/09/25 15:46:48  dick
 * Split KbConfig to KbConfig and KbConfigKb, since the keyboard is only a part
 * of the whole KbConfig window.
 *
 * Revision 1.1  2001/09/20 07:41:02  dick
 * Move Fl_Kb* to it's own library xp-flwidgets/kbconfig/libxp-kbconfig.a
 *
 * Revision 1.2  2001/09/20 07:21:09  dick
 * If a button has multiple commands, display those in a "flyout" window
 * when the mouse is over the key.
 *
 * Revision 1.1  2001/09/17 08:56:28  dick
 * Handle a single button/key on the Fl_KbConfig keyboard.
 *
 */

#ifndef	_FL_KBCBUTTON_H_
#define	_FL_KBCBUTTON_H_

#include "FL/Fl_Widget.H"

#include "Obj.h"
#include "keys.h"

#define	BUTTON_WIDTH	52
#define	BUTTON_HEIGHT	52
#define	BUTTON_SPACE	2

enum ButtonType {
	ButtonTypeNone,
	ButtonTypeBlank,
	ButtonTypeMulti,
	ButtonTypeDragger,
};

class Fl_KbConfigKb;
class KeyDef;
class Fl_KbMultiComm;

// Describe a key on the screen
class KbCKey : public Obj
{
public:
	XPKeyDef*	keyDef;
	String		kname;
	ButtonType	ktype;
//	int		kc;
	int			kr;
	double		kw;	// 1.0 is standard width key
	int			kh;	// 1 is standard height key

};

/*
 * Each button has a list of commands that are the comands associated
 * with this button.  This class contains a pointer to the command
 */
class KbCKeyCommand : public Obj
{
public:
	KeyDef*	key;
};


// Describe a button/key on the keyboard on the screen
class Fl_KbCButton : public Fl_Widget, public KbCKey
{
public:
	Fl_KbCButton(int X, int Y, int W, int H, const char* keyCap = 0);
	virtual ~Fl_KbCButton();
	void			draw();
	virtual	int		handle(int event);
	void			Select();
	void			UnSelect();
	bool			IsSelected() { return(selected); };
	ObjList&		GetCommands() { return(commands); };
	void			SetConfig(Fl_KbConfigKb* _config);

	// If this button contains multiple commands, then pop up a
	// window that shows each of the multiple commands
	Fl_KbMultiComm*	multiWindow;

	// which child the mouse is over
	Fl_KbCButton*	multiWindowChild;

	// if this button is a multi-child, then this is our parent.
	Fl_KbCButton*	parentButton;

private:
	Fl_KbConfigKb*	config;
	ObjList			commands;
	bool			selected;
};

inline void Fl_KbCButton::SetConfig(Fl_KbConfigKb* _c) { config = _c; };

#endif	// _FL_KBCBUTTON_H_
