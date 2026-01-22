/* $Id: ShipEditWindow.cpp,v 1.28 2004/05/30 16:02:34 dick Exp $
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
 * $Log: ShipEditWindow.cpp,v $
 * Revision 1.28  2004/05/30 16:02:34  dick
 * Fix uninitialized memory read in draw_shadow_rects
 *
 * Revision 1.27  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.26  2004/01/04 05:24:58  dick
 * Use xpm files from the builtin directory
 *
 * Revision 1.25  2003/12/22 05:59:51  dick
 * Use png(s) instead of xpm(s).
 *
 * Revision 1.24  2002/09/10 21:43:40  jlmiller
 *
 * These files suddenly need stdlib.h to compile on some linux machines??
 *
 * Revision 1.23  2002/08/15 16:02:26  jlmiller
 * Don't draw the last ship vertex different from all the others.
 *
 * Revision 1.22  2002/08/02 13:36:39  jlmiller
 * Add coorDisplay to the ShipEditor. Useful for debug but might be useful for users too.
 *
 * Revision 1.21  2002/07/30 17:22:46  jlmiller
 * Overlay functions become member functions of the drawing windows.
 *
 * Revision 1.20  2002/07/23 19:58:24  jlmiller
 * No, actually the #include "" was right the first time, so lets put it back.
 *
 * Revision 1.19  2002/07/23 13:24:12  jlmiller
 * #includes should be <> instead of ""
 *
 * Revision 1.18  2002/07/12 15:00:06  jlmiller
 * Integrate tabbed main window, with clean up of globals.
 *
 * Revision 1.17  2002/07/09 14:19:56  jlmiller
 * Debugging garbage
 *
 * Revision 1.16  2002/06/25 23:46:14  jlmiller
 *
 * Linux cleanups.
 *
 * Revision 1.15  2002/06/25 20:20:09  jlmiller
 * Massive restructure to clean up includes, and objectify code more.
 *
 * Revision 1.14  2002/06/25 15:02:19  jlmiller
 * Move iSelection fields to the window, instead of the individual document,
 * as they didn't make any sense at document level.
 *
 * Revision 1.13  2002/06/17 20:59:43  jlmiller
 * Properly calculate the ship coordianates
 *
 * Revision 1.12  2002/06/02 22:44:05  jlmiller
 * Add undoMan to shipeditor.
 * Commentability.
 *
 * Revision 1.11  2002/05/30 17:14:21  jlmiller
 * Moving of items now available.
 *
 * Revision 1.10  2002/05/30 02:26:14  jlmiller
 * Eliminate unnecessary global pointer. Should help eliminate null pointer bugs.
 *
 * Revision 1.9  2002/05/28 07:19:50  dick
 * Rename xpilotedit to XPilotControl
 *
 */
#include "StdAfx.h"

#include <stdlib.h>
#include "FL/fl_draw.H"
#include "FL/Fl_Pixmap.H"

#include "ShipWindow.h"
#include "ShipEditWindow.h"
#include "controlconst.h"

#include "buttons/builtin/m_gun.xpm"
#include "buttons/builtin/engine.xpm"
#include "buttons/builtin/missile_rack.xpm"
#include "buttons/builtin/l_gun.xpm"
#include "buttons/builtin/r_gun.xpm"
#include "buttons/builtin/l_rgun.xpm"
#include "buttons/builtin/r_rgun.xpm"
#include "buttons/builtin/l_light.xpm"
#include "buttons/builtin/r_light.xpm"

Fl_Pixmap *m_gun_pm = new Fl_Pixmap(m_gun);
Fl_Pixmap *engine_pm = new Fl_Pixmap(engine);
Fl_Pixmap *missile_rack_pm = new Fl_Pixmap(missile_rack);
Fl_Pixmap *l_gun_pm = new Fl_Pixmap(l_gun);
Fl_Pixmap *r_gun_pm = new Fl_Pixmap(r_gun);
Fl_Pixmap *l_rgun_pm = new Fl_Pixmap(l_rgun);
Fl_Pixmap *r_rgun_pm = new Fl_Pixmap(r_rgun);
Fl_Pixmap *l_light_pm = new Fl_Pixmap(l_light);
Fl_Pixmap *r_light_pm = new Fl_Pixmap(r_light);



///////////////////////////////////////////////////////////////////////////////
ShipEditWindow::ShipEditWindow(int x,int y,int w,int h,const char *l, ShipDocument *sd)
        : Fl_Double_Window(x,y,w,h,l)
{
	box(FL_THIN_DOWN_BOX);
	color(FL_WHITE);
	pShip = sd;
	mpx1 = 0;	mpy1 = 0;	mpw1 = 0;	mph1 = 0;
	mpx2 = 0;	mpy2 = 0;	mpw2 = 0;	mph2 = 0;
	mpx3 = 0;	mpy3 = 0;	mpw3 = 0;	mph3 = 0;
	mpx4 = 0;	mpy4 = 0;	mpw4 = 0;	mph4 = 0;
};
///////////////////////////////////////////////////////////////////////////////
void ShipEditWindow::draw() {
	int i = 0, adj;
	XpPoint *tmppt;
	ShipWindow *tmp = (ShipWindow *) parent();
	Fl_Double_Window::draw();

	fl_clip(0, 0, w(), h());


	adj = w()/31/2;
	//Draw the grid..decors etc...
	fl_color(FL_GRAY);
	for (i = 1; i < 32; i++)
		fl_line(i*(w()/31), 0, i*(w()/31), h());
	for (i = 1; i < 32; i++)
		fl_line(0, i*(h()/31), w(), i*(h()/31));

	fl_color (FL_GREEN);
	fl_rectf(w()/2-adj, h()/2-adj, adj*2+1, adj*2+1);
	fl_rect(cx(-8)+adj, cy(8)+adj, 16*(w()/31)+1, 16*(w()/31)+1);
	fl_line(cx(14)+adj, cy(0)+adj, cx(-14)+adj, cy(14)+adj);
	fl_line(cx(-14)+adj, cy(14)+adj, cx(-14)+adj, cy(-14)+adj);
	fl_line(cx(-14)+adj, cy(-14)+adj,cx(14)+adj, cy(0)+adj);




	pShip = tmp->Shplst.pCurrentShip;
	if (pShip)
	{
		tmppt = (XpPoint *) pShip->pts.GetHead();

		if (pShip->pts.GetCount()==1)
		{
				fl_color (FL_RED);
				fl_rectf(cx(tmppt->x)+adj-3, cy(tmppt->y)+adj-3, 8, 8);
		}
		else if (pShip->pts.GetCount()>=2)
		{
			fl_line_style(FL_SOLID, 2, NULL);
			do  {
				fl_color (FL_RED);
				fl_rectf(cx(tmppt->x)+adj-3, cy(tmppt->y)+adj-3, 8, 8);
				fl_color(FL_BLUE);
				fl_line(cx(tmppt->x)+adj, cy(tmppt->y)+adj, cx( ((XpPoint *) tmppt->GetNext())->x )+adj, cy( ((XpPoint *) tmppt->GetNext())->y )+adj );
				tmppt = (XpPoint *) tmppt->GetNext();
			} while (tmppt->GetNext() != NULL);

			//Draw the last vertex and the closing line
			fl_color (FL_RED);
			fl_rectf(cx(tmppt->x)+adj-3, cy(tmppt->y)+adj-3, 8, 8);
			fl_color(FL_CYAN);
			fl_line(cx(tmppt->x)+adj, cy(tmppt->y)+adj, cx( ((XpPoint *) pShip->pts.GetHead())->x )+adj, cy( ((XpPoint *) pShip->pts.GetHead())->y )+adj );
			fl_line_style(0);
		}

		if (pShip->sel_start)
		{
				fl_color (FL_RED);
                fl_circle(cx(pShip->start->x)+adj,cy(pShip->start->y)+adj,adj+1.5);
		}

		//	if (pShip->num_points>2)
		//		fl_line(cx(pShip->pts[pShip->num_points-1].x)+adj, cy(pShip->pts[pShip->num_points-1].y)+adj, cx(pShip->pts[0].x)+adj, cy(pShip->pts[0].y)+adj);
		/*	for (i=0; i<pShip->num_points-1; i++) {
		fl_line(cx(pShip->pts[i].x)+adj, cy(pShip->pts[i].y)+adj, cx(pShip->pts[i+1].x)+adj, cy(pShip->pts[i+1].y)+adj );
		}
		fl_color(FL_CYAN);
		if (pShip->num_points>2)
		fl_line(cx(pShip->pts[pShip->num_points-1].x)+adj, cy(pShip->pts[pShip->num_points-1].y)+adj, cx(pShip->pts[0].x)+adj, cy(pShip->pts[0].y)+adj);*/
		fl_color(FL_BLACK);
		tmppt = (XpPoint *) pShip->m_gun.GetHead();
		while (tmppt != NULL)
		{
			draw_m_gun(tmppt, adj);
			tmppt = (XpPoint *) tmppt->GetNext();
		}
		tmppt = (XpPoint *) pShip->engine.GetHead();
		while (tmppt != NULL)
		{
			draw_engine(tmppt, adj);
			tmppt = (XpPoint *) tmppt->GetNext();
		}

		tmppt = (XpPoint *) pShip->m_racks.GetHead();
		while (tmppt != NULL)
		{
			draw_missilerack(tmppt, adj);
			tmppt = (XpPoint *) tmppt->GetNext();
		}

		tmppt = (XpPoint *) pShip->r_guns.GetHead();
		while (tmppt != NULL)
		{
			draw_r_gun(tmppt, adj);
			tmppt = (XpPoint *) tmppt->GetNext();
		}

		tmppt = (XpPoint *) pShip->l_guns.GetHead();
		while (tmppt != NULL)
		{
			draw_l_gun(tmppt, adj);
			tmppt = (XpPoint *) tmppt->GetNext();
		}

		tmppt = (XpPoint *) pShip->l_rguns.GetHead();
		while (tmppt != NULL)
		{
			draw_l_rgun(tmppt, adj);
			tmppt = (XpPoint *) tmppt->GetNext();
		}

		tmppt = (XpPoint *) pShip->r_rguns.GetHead();
		while (tmppt != NULL)
		{
			draw_r_rgun(tmppt, adj);
			tmppt = (XpPoint *) tmppt->GetNext();
		}

		tmppt = (XpPoint *) pShip->l_lights.GetHead();
		while (tmppt != NULL)
		{
			draw_l_light(tmppt, adj);
			tmppt = (XpPoint *) tmppt->GetNext();
		}

		tmppt = (XpPoint *) pShip->r_lights.GetHead();
		while (tmppt != NULL)
		{
			draw_r_light(tmppt, adj);
			tmppt = (XpPoint *) tmppt->GetNext();
		}
	}
	//Draw the overlay rects.
	draw_mirror_rects();
    fl_pop_clip();
}
///////////////////////////////////////////////////////////////////////////////
int ShipEditWindow::handle(int event) {
	int w_xpos, w_ypos; //The position in the current window
	int adj = w()/31/2;
	ShipWindow *tmp = (ShipWindow *) parent();
	XpPointList *ptlst = NULL;
	XpPoint *tmppt = NULL;

	switch (event) {
	case FL_PUSH :
		if (Fl::event_button() == FL_LEFT_MOUSE)
		{
			w_xpos = Fl::event_x();
			w_ypos = Fl::event_y();

			if (pShip !=  NULL)
			{
				//If we're splitting, only bother with vertexes.
				if (tmp->iSelectionShipTools == IDM_SPLIT)
					ptlst = &tmp->Shplst.pCurrentShip->pts;
				else
					switch (tmp->iSelectionShipSyms)
					{
					case IDM_SHIP_VERTEX:
						ptlst = &tmp->Shplst.pCurrentShip->pts;
						break;
					case IDM_SHIP_MAINGUN:
						ptlst = &tmp->Shplst.pCurrentShip->m_gun;
						break;
					case IDM_SHIP_ENGINE:
						ptlst = &tmp->Shplst.pCurrentShip->engine;
						break;
					case IDM_SHIP_MISSLERACK:
						ptlst = &tmp->Shplst.pCurrentShip->m_racks;
						break;
					case IDM_SHIP_LGUN:
						ptlst = &tmp->Shplst.pCurrentShip->l_guns;
						break;
					case IDM_SHIP_RGUN:
						ptlst = &tmp->Shplst.pCurrentShip->r_guns;
						break;
					case IDM_SHIP_LRGUN:
						ptlst = &tmp->Shplst.pCurrentShip->l_rguns;
						break;
					case IDM_SHIP_RRGUN:
						ptlst = &tmp->Shplst.pCurrentShip->r_rguns;
						break;
					case IDM_SHIP_LLIGHT:
						ptlst = &tmp->Shplst.pCurrentShip->l_lights;
						break;
					case IDM_SHIP_RLIGHT:
						ptlst = &tmp->Shplst.pCurrentShip->r_lights;
						break;
					}
				switch (tmp->iSelectionShipTools)
				{
				case IDM_INSERT:
					{
						//If we're already full, dont do anything
						if (ptlst->GetCount() < ptlst->GetMax() )
						{
							ShipUndoableCommand* uc = new ShipUndoableCommand((ShipWindow *) parent(),
								DOACTION, tmp->iSelectionShipTools, ptlst, rx(w_xpos), ry(w_ypos));
							uc->myShipWindow->undoMan.Do(uc);
						}
					}
					break;
				case IDM_DELETE:
					{
						tmppt = ptlst->IsPointAt(rx(w_xpos), ry(w_ypos));
						//If there's no obj of the right type here, do nothing.
						if (tmppt)
						{
							ShipUndoableCommand* uc = new ShipUndoableCommand((ShipWindow *) parent(),
								DOACTION, IDM_DELETE, ptlst, tmppt);
							uc->myShipWindow->undoMan.Do(uc);
						}
					}
					break;
				case IDM_MOVE:
					if (!tmp->Shplst.pCurrentShip->sel_start) //Do we have a point selected? If not check for a point of current type
					{               //at xpos,ypos
						tmppt = ptlst->IsPointAt(rx(w_xpos), ry(w_ypos));
						if (tmppt != NULL)
						{
							tmp->Shplst.pCurrentShip->start = tmppt;
							tmp->Shplst.pCurrentShip->sel_start = true;
						}
						//If theres an "item" of the type we're looking for, we select it and leave
						//we move the point only after a second point has been picked.
					}
					else if (tmp->Shplst.pCurrentShip->start) //Move the currently selected point.
					{
						ShipUndoableCommand* uc = new ShipUndoableCommand((ShipWindow *) parent(),
							DOACTION, IDM_MOVE, ptlst, tmp->Shplst.pCurrentShip->start,
							rx(w_xpos), ry(w_ypos));
						uc->myShipWindow->undoMan.Do(uc);

						tmp->Shplst.pCurrentShip->sel_start = false;
					}
					break;
				case IDM_SPLIT:
					if (!tmp->Shplst.pCurrentShip->sel_start) //Do we have a point selected? If not check for a point of current type
					{
						tmppt = tmp->Shplst.pCurrentShip->pts.IsPointAt(rx(w_xpos), ry(w_ypos));
						if (tmppt != NULL)
						{
							tmp->Shplst.pCurrentShip->start = tmppt;
							tmp->Shplst.pCurrentShip->sel_start = true;
						}
						//If there's a vertex here, we select it and leave
					}
					else if (tmp->Shplst.pCurrentShip->start) //We already have our first point, find the second
					{
						bool clear = false;
						tmppt = tmp->Shplst.pCurrentShip->pts.IsPointAt(rx(w_xpos), ry(w_ypos));
						if (tmppt != NULL)
						{
							ShipUndoableCommand* uc = NULL;
							tmp->Shplst.pCurrentShip->end = tmppt;
							//Special dealings if the points we've selected are the head and tail
							//If so, we must ALWAYS add the point at the end
							if ( ((tmp->Shplst.pCurrentShip->start == tmp->Shplst.pCurrentShip->pts.GetHead()) && (tmp->Shplst.pCurrentShip->end == tmp->Shplst.pCurrentShip->pts.GetTail())) ||
								((tmp->Shplst.pCurrentShip->end == tmp->Shplst.pCurrentShip->pts.GetHead()) && (tmp->Shplst.pCurrentShip->start == tmp->Shplst.pCurrentShip->pts.GetTail())) )

							{
								tmppt = new XpPoint(tmp->Shplst.pCurrentShip->start,tmp->Shplst.pCurrentShip->end);
								uc = new ShipUndoableCommand((ShipWindow *) parent(),
									DOACTION, IDM_INSERT, ptlst, tmppt->x, tmppt->y);
							}
							else
							{
								int rev = tmp->Shplst.pCurrentShip->pts.CheckConsecutive(tmp->Shplst.pCurrentShip->start, tmp->Shplst.pCurrentShip->end);
								if (rev)
									uc = new ShipUndoableCommand((ShipWindow *) parent(),
										DOACTION, IDM_SPLIT, ptlst, tmp->Shplst.pCurrentShip->start,
										tmp->Shplst.pCurrentShip->end, rev);
							}
							if (uc)
								uc->myShipWindow->undoMan.Do(uc);
							tmp->Shplst.pCurrentShip->sel_start = false;
							tmp->Shplst.pCurrentShip->start = tmp->Shplst.pCurrentShip->end = NULL;

						}
					}
					break;
				}
				redraw();
				tmp->UpdateInfoFields();
				tmp->shiplistview->redraw();
				return TRUE;
			}
        }
		break;
	case FL_MOVE :
		{
		make_current();
		w_xpos = Fl::event_x();
		w_ypos = Fl::event_y();

		adj = w()/31;

		tmp->SetCoordDisp("X:%d, Y:%d", rx(w_xpos), ry(w_ypos));
		make_current();

		int tempx = abs(w_xpos/adj)*adj;
		int tempy = abs(w_ypos/adj)*adj;

		mirror_overlay(tempx, tempy, adj, adj);
		return TRUE;
		}
	case FL_RELEASE :
		if (Fl::event_button() == FL_LEFT_MOUSE)
		{
			return TRUE;
		}
		break;
    }
    return Fl_Window::handle(event);
}

///////////////////////////////////////////////////////////////////////////////
/* Coordinate conversion */
int ShipEditWindow::cx(int x)
{
	int i;
	i=x+15;
	i=(int)(i*w()/31);//+10;
	return i;
}

///////////////////////////////////////////////////////////////////////////////
/* Coordinate conversion */
int ShipEditWindow::cy(int y)
{
	int i;
	i=-y+15;
	i=(int)(i*h()/31);//+10;
	return i;
}

///////////////////////////////////////////////////////////////////////////////
//Caluclate the ship coordinates, based on window position.
/* Coordinate conversion */
int ShipEditWindow::rx(int x)
{
	int i;
	i=x;
	i=(int)(i*31/w())-15;
	if (i<-15) i=-15;
	if (i>15) i=15;
	return i;
}

///////////////////////////////////////////////////////////////////////////////
/* Coordinate conversion */
int ShipEditWindow::ry(int y)
{
	int i;
	i=y;
	i=(int)(i*31/h())-15;
	if (i<-15) i=-15;
	if (i>15) i=15;
	return -i;
}

///////////////////////////////////////////////////////////////////////////////
void ShipEditWindow::draw_m_gun(XpPoint *p, int adj)
{
	m_gun_pm->draw(cx(p->x), cy(p->y));
}
///////////////////////////////////////////////////////////////////////////////
void ShipEditWindow::draw_engine(XpPoint *p, int adj)
{
	engine_pm->draw(cx(p->x), cy(p->y));
}
///////////////////////////////////////////////////////////////////////////////
void ShipEditWindow::draw_missilerack(XpPoint *p, int adj)
{
	missile_rack_pm->draw(cx(p->x), cy(p->y));
}
///////////////////////////////////////////////////////////////////////////////
void ShipEditWindow::draw_l_gun(XpPoint *p, int adj)
{
	l_gun_pm->draw(cx(p->x), cy(p->y));
}
///////////////////////////////////////////////////////////////////////////////
void ShipEditWindow::draw_r_gun(XpPoint *p, int adj)
{
	r_gun_pm->draw(cx(p->x), cy(p->y));
}
///////////////////////////////////////////////////////////////////////////////
void ShipEditWindow::draw_l_rgun(XpPoint *p, int adj)
{
	l_rgun_pm->draw(cx(p->x), cy(p->y));
}
///////////////////////////////////////////////////////////////////////////////
void ShipEditWindow::draw_r_rgun(XpPoint *p, int adj)
{
	r_rgun_pm->draw(cx(p->x), cy(p->y));
}
///////////////////////////////////////////////////////////////////////////////
void ShipEditWindow::draw_l_light(XpPoint *p, int adj)
{
	l_light_pm->draw(cx(p->x), cy(p->y));
}
///////////////////////////////////////////////////////////////////////////////
void ShipEditWindow::draw_r_light(XpPoint *p, int adj)
{
	r_light_pm->draw(cx(p->x), cy(p->y));
}

///////////////////////////////////////////////////////////////////////////////
//The following is based on the limited overlay support that's provided with
//fltk. The standard stuff wouldn't do enough for our purposes though, so
//these member functions were created. If FLTK is ever extended to include
//better overlay support, we should probably switch to that if possible, but
//for now these functions serve our purpose.
//-JLM
#include "FL/x.H"
///////////////////////////////////////////////////////////////////////////////
void ShipEditWindow::draw_mirror_rects() {
#ifdef WIN32
	int old = SetROP2(fl_gc, R2_NOT);
	fl_rect(mpx1, mpy1, mpw1, mph1);
	if (mpx2 != mpx1)
		fl_rect(mpx2, mpy2, mpw2, mph2);
	if (mpy3 != mpy1)
		fl_rect(mpx3, mpy3, mpw3, mph3);
	if (mpx4 != mpx1 && mpy4 != mpy1)
		fl_rect(mpx4, mpy4, mpw4, mph4);
	SetROP2(fl_gc, old);
#else
	XSetFunction(fl_display, fl_gc, GXxor);
	XSetForeground(fl_display, fl_gc, 0xffffffff);
	XDrawRectangle(fl_display, fl_window, fl_gc, mpx1, mpy1, mpw1, mph1);
	XDrawRectangle(fl_display, fl_window, fl_gc, mpx2, mpy2, mpw2, mph2);
	XDrawRectangle(fl_display, fl_window, fl_gc, mpx3, mpy3, mpw3, mph3);
	XDrawRectangle(fl_display, fl_window, fl_gc, mpx4, mpy4, mpw4, mph4);
	XSetFunction(fl_display, fl_gc, GXcopy);
#endif
}
///////////////////////////////////////////////////////////////////////////////
void ShipEditWindow::mirror_overlay(int x, int y, int wid, int hei) {
	if (wid < 0) {x += wid; wid = -wid;} else if (!wid) wid = 1;
	if (hei < 0) {y += hei; hei = -hei;} else if (!hei) hei = 1;
	if (mpw1 > 0) {
		if (x==mpx1 && y==mpy1 && wid==mpw1 && hei==mph1) return;
		draw_mirror_rects();
	}
	mpx1 = x; mpy1 = y; mpw1 = wid; mph1 = hei;
	mpx2 = w()-x-wid; mpy2 = y; mpw2 = wid; mph2 = hei;
	mpx3 = x; mpy3 = h()-y-hei; mpw3 = wid; mph3 = hei;
	mpx4 = w()-x-wid; mpy4 = h()-y-hei; mpw4 = wid; mph4 = hei;
	draw_mirror_rects();
}
