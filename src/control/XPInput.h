/* $Id: XPInput.h,v 1.12 2004/06/01 18:17:50 dick Exp $
 *
 * XPInput - Provide a text input window that associates with a String
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
 * $Log: XPInput.h,v $
 * Revision 1.12  2004/06/01 18:17:50  dick
 * xpinput_cb becomes XPInput::CallbackFl
 *
 * Revision 1.11  2004/05/27 21:36:25  dick
 * Whitespace argh.
 *
 * Revision 1.10  2004/05/27 21:33:16  dick
 * No, it must be a String*, so that it manages the master string (typically iniClient).
 *
 * Revision 1.9  2004/05/10 13:01:56  dick
 * undef index (bleh)
 *
 * Revision 1.8  2004/05/10 12:44:08  dick
 * Use a String, not a String*
 *
 */

#ifndef	_XPINPUT_H_
#define	_XPINPUT_H_

#undef 	index
#include "FL/Fl_Input.H"

class String;

class XPInput : public Fl_Input {
public:
	XPInput(int x,int y,int w,int h,const char *l=0);
	~XPInput();
	static void XPInput::CallbackFl(XPInput* w, void*);

	void	Read();
	void	Store();
	void	SetStrng(String *sp);

	String*	ps;
};
#endif	// _XPINPUT_H_
