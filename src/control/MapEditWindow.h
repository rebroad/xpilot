/*
* Xpilotedit, the cross platform map editor for Xpilot.  Copyright (C) 2001 by
*
*      The Xpilot Authors           <xpilot@xpilot.org>
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
#ifndef	_MAPEDITWINDOW_H_
#define	_MAPEDITWINDOW_H_
#include "FL/Fl_Double_Window.H"
#include "MapDocument.h"

typedef struct {
char					 char_val;
int                   num_points;
float                 x[5],y[5];
} segment_t;

class MapEditWindow : public Fl_Double_Window {
        void flush();
        int handle(int);
private:
        int view_zoom; //The current zoom magnification.
		XpPoint ptBeg, ptEnd;
		int pcx,pcy,pcr;
		int px,py,pw,ph;
		int plx,ply,plx2,ply2;
		void draw_current_line();
		void overlay_line(int x, int y, int x2, int y2);
		void draw_current_rect();
		void overlay_rect(int x, int y, int w, int h);
		void draw_current_circle();
		void overlay_circle(int x, int y, int r);
public:
		void overlay_line_clear();
		void overlay_rect_clear();
		void overlay_circle_clear();
public:
		MapDocument *pMap; //pointer to the map we're dealing with
        int iVscrollMax, iHscrollMax, iHscrollPos, iVscrollPos;
	    bool fPasting;
		bool fDrawing;
		bool fDragging;
public:

        MapEditWindow(int x,int y,int w,int h,const char *l=0);

		void GetZoomFont();
		bool ZoomIn();
		bool ZoomOut();
		void ClearOverlay();

		void DrawMapOutline (XpPoint, XpPoint, int); //Draw an outline around the map
        void DrawSmallBlock (int, int, int); //Draw a simple rect for the block, instead of details
        bool DrawMapSection(int, int, int, int, int, int); //Draw an area of the map
        bool DrawMapPic(int, int, char, int, int); //Draw a block at the selected pos
};
#endif	// _MAPEDITWINDOW_H_
