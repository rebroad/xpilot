/* $Id: Fl_PrefsColorKey.cpp,v 1.3 2004/02/28 07:30:15 dick Exp $
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
 * $Log: Fl_PrefsColorKey.cpp,v $
 * Revision 1.3  2004/02/28 07:30:15  dick
 * Change the colors to look good on a black background
 *
 * Revision 1.2  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.1  2002/07/08 06:22:08  dick
 * A new widget which is a status line showing a Key of the colors used to
 * represent an option's origins.
 *
 */
#include "StdAfx.h"

#include "FL/fl_draw.H"
#include "FlEventNames.h"
#include "Fl_XPColors.h"

#include "Fl_PrefsColorKey.h"

static	PCSTR	tooltipText =
	"The label of each option is displayed in a color that represents\n"
	"where the option was set from.";

///////////////////////////////////////////////////////////////////////////////
Fl_PrefsColorKey::Fl_PrefsColorKey(int x,int y,int w,int h,const char *l)
        : Fl_PrefsColorKeySUPERCLASS(x,y,w,h,l)
{
	color(xpblack,xpblue);
	flOptBrown = fl_color_cube(180*FL_NUM_RED/256,130*FL_NUM_GREEN/256,24*FL_NUM_BLUE/256);
	flOptLtBrown = fl_color_cube(254*FL_NUM_RED/256,203*FL_NUM_GREEN/256,67*FL_NUM_BLUE/256);
	flOptBlue = fl_color_cube(63*FL_NUM_RED/256,109*FL_NUM_GREEN/256,255*FL_NUM_BLUE/256);
	flOptRed = fl_color_cube(188*FL_NUM_RED/256,10*FL_NUM_GREEN/256,46*FL_NUM_BLUE/256);
	flOptGreen = fl_color_cube(95*FL_NUM_RED/256,194*FL_NUM_GREEN/256,3*FL_NUM_BLUE/256);
	flOptDkGreen = fl_color_cube(0*FL_NUM_RED/256,128*FL_NUM_GREEN/256,128*FL_NUM_BLUE/256);
	tooltip(tooltipText);
}

///////////////////////////////////////////////////////////////////////////////
Fl_Color	Fl_PrefsColorKey::GetOptColor(OptOrigin org)
{
	switch (org)
	{
	case OPT_INIT:
		return(FL_WHITE);
    case OPT_DEFAULTS:
		return(flOptBrown);
	case OPT_MAP:
		return(flOptLtBrown);
    case OPT_COMMAND:
		return(flOptBlue);
	case OPT_PASSWORD:
		//return(flOptRed);
		return(xpred);
//	OPT_EDITOR		= OPT_MAP,
	case OPT_TUNEFILE:
		return(flOptDkGreen);
	case OPT_TUNE:
		return(flOptGreen);
	}
	return(FL_WHITE);
}

void Fl_PrefsColorKey::DrawPiece(PCSTR s, int xofs)
{
	fl_draw(s, x()+xofs+5, y()+h()-fl_descent()+1);
}

#define	INTERSPACE	4
void Fl_PrefsColorKey::draw()
{
	PCSTR	s;
	int	mw;
	int	mh;
	int	xofs = 0;

	int	oldFont = fl_font();
	int	oldSize = fl_size();
	fl_font(FL_HELVETICA, 12);

	fl_push_clip(x(), y(), w(), h());

	s = "Init";
	fl_color(GetOptColor(OPT_INIT));
	mw = 0;
	fl_measure(s, mw, mh);
	DrawPiece(s, xofs);
	xofs += mw + INTERSPACE;

	s = "Defaults";
	fl_color(GetOptColor(OPT_DEFAULTS));
	mw = 0;
	fl_measure(s, mw, mh);
	DrawPiece(s, xofs);
	xofs += mw + INTERSPACE;

	s = "Map";
	fl_color(GetOptColor(OPT_MAP));
	mw = 0;
	fl_measure(s, mw, mh);
	DrawPiece(s, xofs);
	xofs += mw + INTERSPACE;

	s = "Command Line";
	fl_color(GetOptColor(OPT_COMMAND));
	mw = 0;
	fl_measure(s, mw, mh);
	DrawPiece(s, xofs);
	xofs += mw + INTERSPACE;

	s = "Password";
	fl_color(GetOptColor(OPT_PASSWORD));
	mw = 0;
	fl_measure(s, mw, mh);
	DrawPiece(s, xofs);
	xofs += mw + INTERSPACE;

	s = "Tune File";
	fl_color(GetOptColor(OPT_TUNEFILE));
	mw = 0;
	fl_measure(s, mw, mh);
	DrawPiece(s, xofs);
	xofs += mw + INTERSPACE;

	s = "Tuned";
	fl_color(GetOptColor(OPT_TUNE));
	mw = 0;
	fl_measure(s, mw, mh);
	DrawPiece(s, xofs);
	xofs += mw + INTERSPACE;

	fl_font(oldFont, oldSize);
	fl_pop_clip();
}
