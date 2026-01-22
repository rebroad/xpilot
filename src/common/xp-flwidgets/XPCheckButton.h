/* $Id: XPCheckButton.h,v 1.2 2004/05/28 17:24:03 dick Exp $
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
 * $Log: XPCheckButton.h,v $
 * Revision 1.2  2004/05/28 17:24:03  dick
 * xpcheckbutton_cb() becomes static XPCheckButton::CallbackFl().
 *
 */

#ifndef	_XPCHECKBUTTON_H_
#define	_XPCHECKBUTTON_H_
#include "FL/Fl_Check_Button.H"

class XPCheckButton : public Fl_Check_Button {
public:
	XPCheckButton(int x,int y,int w,int h,const char *l=0);
	~XPCheckButton();

	static void CallbackFl(XPCheckButton* w, void*);
	void	Callback();
	void	Read();
	void	Store();
	void	SetBool(bool* bp);

	bool*	bValue;
};

///////////////////////////////////////////////////////////////////////////////
//XPCheckButton definition struct
struct xpc_xpcheckbutton {
	int		x;	 //x location
	int		y;	 //y location
	int		w;	 //button width
	int		h;	 //button height
	char*	lbl; //String for label
	XPCheckButton* button; //Handle to button once its created
};

#endif	// _XPCHECKBUTTON_H_
