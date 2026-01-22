/* $Id: XPCheckButton.cpp,v 1.4 2004/05/28 17:24:03 dick Exp $
 *
 * XPCheckButton - a toggle type on/off button
 *
 * common/xp-flwidgets - A library of FLTK widgets for XPilot5.
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
 * $Log: XPCheckButton.cpp,v $
 * Revision 1.4  2004/05/28 17:24:03  dick
 * xpcheckbutton_cb() becomes static XPCheckButton::CallbackFl().
 *
 */
#include "StdAfx.h"

#include <stdio.h>
#include "XPCheckButton.h"

///////////////////////////////////////////////////////////////////////////////
XPCheckButton::XPCheckButton(int x,int y,int w,int h,const char *l)
        : Fl_Check_Button(x,y,w,h,l)
{
	bValue = NULL;
}

///////////////////////////////////////////////////////////////////////////////
XPCheckButton::~XPCheckButton()
{
}

///////////////////////////////////////////////////////////////////////////////
void XPCheckButton::Read()
{
	if (bValue)
		value(*bValue);
}

///////////////////////////////////////////////////////////////////////////////
void XPCheckButton::Store()
{
	int tmp = value();

	switch(tmp)
	{
	case 0: *bValue = false;
		break;
	case 1: *bValue = true;
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////
void XPCheckButton::SetBool(bool *bp)
{
	bValue  = bp;
};

///////////////////////////////////////////////////////////////////////////////
// static
void XPCheckButton::CallbackFl(XPCheckButton* w, void*) {
	w->Store();
}

///////////////////////////////////////////////////////////////////////////////
void XPCheckButton::Callback() {
	Store();
}
