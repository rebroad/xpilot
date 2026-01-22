/* $Id: Fl_PrefsColorKey.h,v 1.1 2002/07/08 06:22:08 dick Exp $
 *
 * Fl_PrefsColorKey - Display a text line overviewing the colors
 *                    of the labels, and manage those different colors
 *
 *      Copyright (C) 2002 by
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
 * $Log: Fl_PrefsColorKey.h,v $
 * Revision 1.1  2002/07/08 06:22:08  dick
 * A new widget which is a status line showing a Key of the colors used to
 * represent an option's origins.
 *
 */

#ifndef	_FL_PREFSCOLORKEY_H_
#define	_FL_PREFSCOLORKEY_H_

#include "FL/Fl.H"
#include "FL/Fl_Widget.H"

#include "ServerOption.h"

#define	Fl_PrefsColorKeySUPERCLASS	Fl_Widget
class Fl_PrefsColorKey : public Fl_PrefsColorKeySUPERCLASS
{
public:
	Fl_PrefsColorKey(int x,int y,int w,int h,const char *l=0);

	Fl_Color	GetOptColor(OptOrigin org);

protected:
	void		draw();

private:
	void		DrawPiece(PCSTR s, int xofs);

	Fl_Color	flOptBrown;
	Fl_Color	flOptLtBrown;
	Fl_Color	flOptBlue;
	Fl_Color	flOptRed;
	Fl_Color	flOptGreen;
	Fl_Color	flOptDkGreen;

};

#endif	// _FL_PREFSCOLORKEY_H_
