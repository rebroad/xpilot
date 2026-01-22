/* $Id: ShipEditWindow.h,v 1.9 2004/05/30 16:02:34 dick Exp $
 *
 * ShipEditWindow - Draw a detailed ship for editing
 *
 * XPilot5 - Manage all local XPilot activity.
 *
 *      Copyright (C) 2002 by
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
 * $Log: ShipEditWindow.h,v $
 * Revision 1.9  2004/05/30 16:02:34  dick
 * Fix uninitialized memory read in draw_shadow_rects
 *
 */
#ifndef	_SHIPEDITWINDOW_H_
#define	_SHIPEDITWINDOW_H_

#include "FL/Fl_Double_Window.H"

#include "ShipDocument.h"

class ShipEditWindow : public Fl_Double_Window {
	void draw();
	int handle(int);
private:
	int mpx1,mpy1,mpw1,mph1;
	int mpx2,mpy2,mpw2,mph2;
	int mpx3,mpy3,mpw3,mph3;
	int mpx4,mpy4,mpw4,mph4;
	void mirror_overlay(int x, int y, int w, int h);
	void draw_mirror_rects();

public:
	ShipEditWindow(int x,int y,int w,int h,const char *l=0, ShipDocument *sd=NULL);
	ShipDocument *pShip;
	int cx(int x);
	int cy(int y);
	int rx(int x);
	int ry(int y);
	void draw_m_gun(XpPoint *p, int adj);
	void draw_engine(XpPoint *p, int adj);
	void draw_missilerack(XpPoint *p, int adj);
	void draw_l_gun(XpPoint *p, int adj);
	void draw_r_gun(XpPoint *p, int adj);
	void draw_l_rgun(XpPoint *p, int adj);
	void draw_r_rgun(XpPoint *p, int adj);
	void draw_l_light(XpPoint *p, int adj);
	void draw_r_light(XpPoint *p, int adj);
};
#endif	// _SHIPEDITWINDOW_H_
