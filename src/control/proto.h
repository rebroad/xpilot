/*
* XPilotedit, the cross platform map editor for XPilot.  Copyright (C) 2001 by
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
 * $Log: proto.h,v $
 * Revision 1.24  2004/02/26 05:59:42  dick
 * Need to declare Fl_Widget
 *
 * Revision 1.23  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.22  2004/01/13 21:30:14  jlmiller
 * Use the provided TestFileExists function when Save As... to an existing file.
 *
 * Revision 1.21  2002/07/30 17:21:33  jlmiller
 * Overlay functions become member functions of the drawing windows.
 *
 * Revision 1.20  2002/07/12 15:00:06  jlmiller
 * Integrate tabbed main window, with clean up of globals.
 *
 * Revision 1.19  2002/07/10 04:02:49  dick
 * Remove gui_main functions
 *
 */

#ifndef	_PROTO_H_
#define	_PROTO_H_

class Fl_Widget;

//general window funcs
int SetupMainWin(int, char**);

/*errors.cpp*/
void ErrorHandler(const char *, ...);

/*tools.cpp*/
String	GetButtonFile(PCSTR f);
bool	TestFileOverwrite(PCSTR f);

/*gui_main.cpp*/
//void newmap_cb(Fl_Widget* w, void*);
//void newship_cb(Fl_Widget*, void*);
//void openmap_cb(Fl_Widget* w, void*);

/*gui_toolkit.cpp*/
void togglebar_cb(Fl_Widget *w, void*);

#endif// _PROTO_H_

