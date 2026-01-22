/* $Id: XPChoice.h,v 1.3 2004/05/28 19:02:30 dick Exp $
 *
 * XPChoice - Manage a dropdown combo box with choices
 *
 * common/xp-flwidgets - A library of FLTK widgets for XPilot5.
 *
 *      Copyright (C) 2004 by
 *
 *      The XPilot Authors   <xpilot@xpilot.org>
 *      Jarrod Miller        <jarrod@xpilot.org>
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
 * $Log: XPChoice.h,v $
 * Revision 1.3  2004/05/28 19:02:30  dick
 * xpchoice_cb() becomes XPChoice::CallbackFl()
 *
 * Revision 1.2  2004/05/28 17:22:56  dick
 * Whitespace
 *
 */
#ifndef	_XPCHOICE_H_
#define	_XPCHOICE_H_
#include "FL/Fl_Choice.H"

class XPChoice : public Fl_Choice {
public:
	char *cValue;
	XPChoice(int x,int y,int w,int h,const char *l=0);
	~XPChoice();
	static	void CallbackFl(XPChoice* w, void*);

	void Read();
	void Store();
	void SetChar(char *cp);
};


#endif	// _XPCHOICE_H_
