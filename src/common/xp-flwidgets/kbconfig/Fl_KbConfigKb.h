/* $Id: Fl_KbConfigKb.h,v 1.4 2001/10/04 17:33:39 dick Exp $
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
 * $Log: Fl_KbConfigKb.h,v $
 * Revision 1.4  2001/10/04 17:33:39  dick
 * Use uint instead of UINT.
 *
 * Revision 1.3  2001/10/04 16:21:40  dick
 * New UpdateAllCommands() resets the buttons' pointers after shuffling the
 * keyDefs table.  UpdateUnusedCommands() scans for unused commands and adds
 * them to the Unused button.
 *
 * Revision 1.2  2001/09/27 17:39:34  dick
 * Better selecting the key under the mouse.
 * Parse off a bit of the head of the keyboard definition string, like name.
 *
 * Revision 1.1  2001/09/25 15:45:52  dick
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

#ifndef	_FL_KBCONFIGKB_H_
#define	_FL_KBCONFIGKB_H_

#include "FL/Fl_Scroll.H"
#include "cstring.h"
#include "Obj.h"
#include "keys.h"

class Fl_KbCButton;
class KbCKey;

#define	Fl_KbConfigKbSUPERCLASS	Fl_Scroll
class Fl_KbConfigKb : public Fl_KbConfigKbSUPERCLASS
{
public:
	Fl_KbConfigKb(int X, int Y, int W, int H, const char* label = 0);

	void			Init(PCSTR kbString);
	void			GetCommands(ObjList& olist, const KbCKey* key);
	bool			SelectKeyUnderMouse();
	Fl_KbCButton*	FindButton(uint key);
	Fl_KbCButton*	FindButton(XPKeyDef* kd);

	void			SetUnusedButton(Fl_KbCButton* b);
	void			SetCopyButton(Fl_KbCButton* b);
	Fl_KbCButton*	GetCopyButton() { return(copyButton); };
	void			UpdateAllCommands();
	void			UpdateUnusedCommands();
	virtual	int		handle(int event);

	int				lastlastcol;	// calculated width of the keyboard
	Fl_KbCButton*	lastMouseKey;	// last known key that the mouse is over

	String			name;
private:
	Fl_KbCButton*	GetButtonBelowMouse();

	ObjList			keyChain;		// list of all keys (KbCKey)

	Fl_KbCButton*	unusedButton;
	Fl_KbCButton*	copyButton;
};

Fl_KbConfigKb* CreateKbConfigKbWindow(PCSTR windowTitle);

inline void Fl_KbConfigKb::SetCopyButton(Fl_KbCButton* b) { copyButton = b; keyChain.Add(b); };
#endif		// _FL_KBCONFIG_H_
