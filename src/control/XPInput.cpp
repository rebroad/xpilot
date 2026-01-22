/* $Id: XPInput.cpp,v 1.13 2004/06/01 18:17:50 dick Exp $
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
 * $Log: XPInput.cpp,v $
 * Revision 1.13  2004/06/01 18:17:50  dick
 * xpinput_cb becomes XPInput::CallbackFl
 *
 * Revision 1.12  2004/05/27 21:36:25  dick
 * Whitespace argh.
 *
 * Revision 1.11  2004/05/27 21:33:16  dick
 * No, it must be a String*, so that it manages the master string (typically iniClient).
 *
 * Revision 1.10  2004/05/10 12:44:08  dick
 * Use a String, not a String*
 *
 * Revision 1.9  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.8  2002/08/20 16:34:24  jlmiller
 * More usable pass at configurable XPWhere window.
 * Still a few bugs to work smash.
 *
 * Revision 1.7  2002/08/20 10:47:29  jlmiller
 * File & Class Renamings:
 * ViewWindow -> MapEditWindow
 * XpInput -> XPInput
 * xpeTypes -> xpcTypes
 *
 * Revision 1.6  2002/07/19 11:06:35  jlmiller
 * Make name, realname, & hostname String(s)
 *
 * Revision 1.5  2002/06/25 20:20:10  jlmiller
 * Massive restructure to clean up includes, and objectify code more.
 *
 * Revision 1.4  2002/06/11 03:13:25  jlmiller
 * No need to be creating a new string everytime we set
 *
 * Revision 1.3  2002/06/01 17:14:26  jlmiller
 * Fix a few nasty little bugs that would crash only when the Debugger WASNT running.
 * Commentability
 *
 * Revision 1.2  2002/05/28 07:19:51  dick
 * Rename xpilotedit to XPilotControl
 *
 */

// Provide an Input Window that associates with a string.
// Read and Write with this string as needed.

#include "StdAfx.h"

#include "XPInput.h"
#include "cstring.h"

///////////////////////////////////////////////////////////////////////////////
XPInput::XPInput(int x,int y,int w,int h,const char *l)
        : Fl_Input(x,y,w,h,l)
{
	ps = NULL;
}

///////////////////////////////////////////////////////////////////////////////
XPInput::~XPInput()
{
	ps = NULL;
}

///////////////////////////////////////////////////////////////////////////////
void XPInput::Read()
{
	if (ps)
		value(*ps);
}

///////////////////////////////////////////////////////////////////////////////
void XPInput::Store()
{
	if (ps)
		ps->printf(value());
}

///////////////////////////////////////////////////////////////////////////////
void XPInput::SetStrng(String *sp)
{
	ps = sp;
}

///////////////////////////////////////////////////////////////////////////////
// static
void XPInput::CallbackFl(XPInput* w, void*) {
	w->Store();
}
