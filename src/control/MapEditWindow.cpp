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
 * $Log: MapEditWindow.cpp,v $
 * Revision 1.38  2005/03/17 22:12:13  kps
 * Get rid of warnings from makedepend about "non-portable whitespace".
 *
 * Revision 1.37  2004/02/09 18:22:48  dick
 * Lots of whitespace changes to support fixing resizable columns.
 *
 * Revision 1.36  2004/02/03 05:32:33  cppc
 * Cast argument to disambiguate the sqrt call under .NET
 *
 * Revision 1.35  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.34  2002/09/10 01:20:18  dick
 * Include stdlib.h
 *
 * Revision 1.33  2002/09/02 16:27:32  dick
 * Singleton ini becomes iniClient.
 *
 * Revision 1.32  2002/08/20 10:47:29  jlmiller
 * File & Class Renamings:
 * ViewWindow -> MapEditWindow
 * XpInput -> XPInput
 * xpeTypes -> xpcTypes
 *
 * Revision 1.31  2002/07/31 13:18:13  jlmiller
 * We HAVE to call make_current after setting the coordDisp.
 *
 * Revision 1.30  2002/07/30 19:52:59  jlmiller
 * Clipboard buttons update automatically. Sort out a few "selection" bugs. Add a coordinate display.
 *
 * Revision 1.29  2002/07/30 17:23:48  jlmiller
 * Overlay functions become member functions of the drawing windows.
 * Clipboard is its own class.
 *
 * Revision 1.28  2002/07/23 23:28:03  jlmiller
 * draw() changes to flush(). Fixes paint problem on linux apparently, because FLTK is silly.
 *
 * Revision 1.27  2002/07/23 19:58:24  jlmiller
 * No, actually the #include "" was right the first time, so lets put it back.
 *
 * Revision 1.26  2002/07/23 13:24:12  jlmiller
 * #includes should be <> instead of ""
 *
 * Revision 1.25  2002/07/22 15:09:05  jlmiller
 * Use UndoMan's new DoAgain instead of doing it the long way. Helps with mouse drag drawing bigtime.
 *
 * Revision 1.24  2002/07/20 17:45:56  jlmiller
 * FillArea is undoable.
 *
 * Revision 1.23  2002/07/19 18:17:05  jlmiller
 * Handle ini.filledWorld and ini.filledDecor.
 *
 * Revision 1.22  2002/07/19 12:14:44  jlmiller
 * Undo and entire draw swipe at once.
 *
 * Revision 1.21  2002/07/12 15:00:06  jlmiller
 * Integrate tabbed main window, with clean up of globals.
 *
 * Revision 1.20  2002/06/26 13:54:36  jlmiller
 * Fix Quick Cannon tool.
 *
 * Revision 1.19  2002/06/25 20:20:09  jlmiller
 * Massive restructure to clean up includes, and objectify code more.
 *
 * Revision 1.18  2002/06/25 15:02:19  jlmiller
 * Move iSelection fields to the window, instead of the individual document,
 * as they didn't make any sense at document level.
 *
 * Revision 1.17  2002/06/08 20:45:44  jlmiller
 * Copy, Paste & Drag use undoMan
 *
 * Revision 1.16  2002/06/07 23:10:08  jlmiller
 * Encapsulate almost all map functions into undoMan.
 *
 * Revision 1.15  2002/06/07 20:49:00  jlmiller
 * Begin handling undoMan properly...most functions still broken, expecially RotateMap.
 *
 * Revision 1.14  2002/05/30 14:19:26  jlmiller
 * Remove activeMapWindow global, which is unnecessay
 * MapWindow & ShipWindow store documents similarly now.
 *
 * Revision 1.13  2002/05/28 19:29:03  jlmiller
 * lastTeamBase shouldn't be global. Hide the server tab from the preferences
 * while in the mapeditor, since it doesnt work through there anyway.
 *
 * Revision 1.12  2002/05/28 07:19:51  dick
 * Rename xpilotedit to XPilotControl
 *
 */
#include "StdAfx.h"

#ifndef	_WINDOWS
#	include <stdlib.h>
#endif
#include <math.h>
#include "FL/fl_draw.H"

#include "config.h"
#include "FlEventNames.h"
#include "MapWindow.h"
#include "MapEditWindow.h"
#include "controlconst.h"
#include "map.h"
#include "proto.h"
#include "MapBlockChange.h"
#include "MapUndoableCommand.h"
#include "default_colors.h"
#include "Clipboard.h"

#include "IniClient.h"
#include "bit.h"

///////////////////////////////////////////////////////////////////////////////
MapEditWindow::MapEditWindow(int x,int y,int w,int h,const char *l)
        : Fl_Double_Window(x,y,w,h,l)
{
box(FL_DOWN_BOX);
color(FL_BLACK);
view_zoom = DEFAULT_MAP_ZOOM;
iVscrollMax = iHscrollMax = iHscrollPos = iVscrollPos = 0;
ClearOverlay();
fPasting = false;
fDrawing = false;
fDragging = false;
};
///////////////////////////////////////////////////////////////////////////////
void MapEditWindow::GetZoomFont()
{
        fl_font(FL_TIMES, (int) (view_zoom*.7) );
}
///////////////////////////////////////////////////////////////////////////////
bool MapEditWindow::ZoomIn() {
	if (view_zoom < 100)
	{
		view_zoom += 5;
		return false;
	}
	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool MapEditWindow::ZoomOut() {
	if (view_zoom == 2)
		return true;

	if (view_zoom <= 6)
		view_zoom = 2;
	else
		view_zoom -= 5;
	return false;
}
///////////////////////////////////////////////////////////////////////////////
void MapEditWindow::ClearOverlay()
{
	ptBeg.x = ptBeg.y = ptEnd.x = ptEnd.y = 0;
}

///////////////////////////////////////////////////////////////////////////////
void MapEditWindow::flush() {
        int iVPaintBeg, iVPaintEnd, iHPaintBeg, iHPaintEnd;
        XpPoint ptOutbeg, ptOutend;
        MapWindow *ThisWindow;


        Fl_Double_Window::flush();

        fl_color(FL_BLACK);
        fl_rectf(0,0, w(), h());

        ThisWindow = (MapWindow *) parent();


        //Determine the area to paint
        iVscrollMax = MAX (0, pMap->optionsDoc->mapHeight->GetInt() - h() / view_zoom + 1) ;
        iVscrollPos = MIN (iVscrollPos, iVscrollMax) ;

        ThisWindow->sb_v->value(iVscrollPos, 1, 0, iVscrollMax);

        iHscrollMax = MAX (0, pMap->optionsDoc->mapWidth->GetInt() - w() / view_zoom + 1) ;
        iHscrollPos = MIN (iHscrollPos, iHscrollMax) ;

        ThisWindow->sb_h->value(iHscrollPos, 1, 0, iHscrollMax);

        iVPaintBeg = MAX (0, iVscrollPos);
        iVPaintEnd = MIN (pMap->optionsDoc->mapHeight->GetInt(), iVscrollPos + h() / view_zoom);
        iHPaintBeg = MAX (0, iHscrollPos);
        iHPaintEnd = MIN (pMap->optionsDoc->mapWidth->GetInt(), iHscrollPos + w() / view_zoom);

        fl_clip(0, 0, w(), h());
        DrawMapSection(iHPaintBeg, iVPaintBeg, iHPaintEnd, iVPaintEnd, 0, 0);


        if (pMap->fSelected)
        {
                ptBeg.x = (pMap->seldxbeg - iHscrollPos) * view_zoom;
                ptBeg.y = (pMap->seldybeg - iVscrollPos) * view_zoom;
                ptEnd.x = (pMap->seldxend - iHscrollPos) * view_zoom;
                ptEnd.y = (pMap->seldyend - iVscrollPos) * view_zoom;
                overlay_rect(ptBeg.x, ptBeg.y, ptEnd.x - ptBeg.x, ptEnd.y - ptBeg.y);
        }

        //The Border Box
        ptOutbeg.x = (0 - iHscrollPos) * view_zoom -1;
        ptOutbeg.y = (0 - iVscrollPos) * view_zoom -1;
        ptOutend.x = (pMap->optionsDoc->mapWidth->GetInt() - iHscrollPos) * view_zoom +1;
        ptOutend.y = (pMap->optionsDoc->mapHeight->GetInt() - iVscrollPos) * view_zoom +1;

        if (pMap->optionsDoc->extraBorder->GetBool())
                DrawMapOutline (ptOutbeg, ptOutend, FL_BLUE);
        else
        {
                if (pMap->optionsDoc->edgeWrap->GetBool())
                        DrawMapOutline (ptOutbeg, ptOutend, FL_GREEN);
                else
                        DrawMapOutline (ptOutbeg, ptOutend, FL_RED);
        }

        fl_pop_clip();
}
///////////////////////////////////////////////////////////////////////////////
int MapEditWindow::handle(int event)
{
	int w_xpos, w_ypos; //The position in the current window
	int m_xpos, m_ypos; //The position in the current map
	MapWindow*	tmpW = (MapWindow *) parent();
	static ObjList *chgListCur; //static chgList so we can undo an entire swipe at once.

	D(PCSTR ev = GetFlEventName(event);
				printf("MapEditWindow::handle: ev=%s\n", ev);)

	switch (event)
	{
	case FL_PUSH :
		if (Fl::event_button() == FL_LEFT_MOUSE)
		{
			make_current();

			w_xpos = Fl::event_x();
			w_ypos = Fl::event_y();

			m_xpos = w_xpos / view_zoom + iHscrollPos;
			m_ypos = w_ypos / view_zoom + iVscrollPos;

			//Way out of map limits, don't bother going any further
			if ((m_xpos >= pMap->optionsDoc->mapWidth->GetInt() + 1) ||
				(m_ypos >= pMap->optionsDoc->mapHeight->GetInt() + 1))
			{
				return TRUE;
			}

			if ((m_xpos >= pMap->seldxbeg) && (m_xpos < pMap->seldxend) &&
				(m_ypos >= pMap->seldybeg) && (m_ypos < pMap->seldyend) &&
				pMap->fSelected && !fPasting)
			{
				fDragging = true;
				pMap->strdx = m_xpos;
				pMap->strdy = m_ypos;
				pMap->dragArea(0, 0, FALSE);
				return TRUE;
			}

			if ((m_xpos <= pMap->optionsDoc->mapWidth->GetInt()) && (m_ypos <= pMap->optionsDoc->mapHeight->GetInt())
			 && (m_xpos >= 0) && (m_ypos >= 0) && !fPasting)
			{
				fDrawing = true;
				switch (tmpW->iSelectionTools)
				{
				case IDM_PEN :
				case IDM_ERASE :
					{
						int typ;
						chgListCur = new ObjList();

						if (tmpW->iSelectionTools == IDM_ERASE)
							typ = MAP_SPACE;
						else if (tmpW->iSelectionMapSyms == IDM_MAP_TEAMBASE)
							typ = (int) tmpW->lastTeamBase;
						else if (tmpW->iSelectionMapSyms == IDM_MAP_CHECKPOINT)
							typ = pMap->FindNextCheckPoint();
						else if (tmpW->iSelectionMapSyms == IDM_MAP_CAN_UNSPEC)
							typ = pMap->FindQuickCanDir(m_xpos, m_ypos);
						else
							typ = tmpW->iSelectionMapSyms;

						//If we arent changing anything, quit right now.
						if (typ == pMap->MapData(m_xpos, m_ypos))
							break;

						MapBlockChange *chg = new MapBlockChange();
						chg->x = m_xpos;
						chg->y = m_ypos;
						chg->newchar = typ;
						chg->oldchar = pMap->MapData(m_xpos, m_ypos);
						chgListCur->Add(chg);

						MapUndoableCommand* uc = new MapUndoableCommand(tmpW,
						CHANGEAREA, 0, chgListCur);

						uc->myMapWindow->undoMan.Do(uc);
						DrawMapSection(m_xpos, m_ypos, m_xpos+1, m_ypos+1,  w_xpos / view_zoom *view_zoom,
						w_ypos / view_zoom * view_zoom);
						break;
					}
				case IDM_FILL :
					{
						int typ;
						ObjList *objLst = NULL;
						if (tmpW->iSelectionMapSyms == IDM_MAP_TEAMBASE)
						typ = (int) tmpW->lastTeamBase;
						else
						typ = tmpW->iSelectionMapSyms;
						objLst = pMap->FillArea(m_xpos, m_ypos, typ);

						MapUndoableCommand* uc = new MapUndoableCommand(tmpW,
						CHANGEAREA, 0, objLst);
						uc->myMapWindow->undoMan.Do(uc);
					}
					redraw();
					break;
				case IDM_LINE :
					ptBeg.x = ptEnd.x = w_xpos / view_zoom *view_zoom + (view_zoom / 2);
					ptBeg.y = ptEnd.y = w_ypos / view_zoom * view_zoom + (view_zoom / 2);
					overlay_line(ptBeg.x, ptBeg.y, ptEnd.x, ptEnd.y);
					break;
				case IDM_SHAPE :
					switch (tmpW->iSelectionShape)
					{
					case IDM_CIRCLEEMPTY:
					case IDM_CIRCLEFILLED:
						ptBeg.x = ptEnd.x = w_xpos / view_zoom * view_zoom + (view_zoom / 2);
						ptBeg.y = ptEnd.y = w_ypos / view_zoom * view_zoom + (view_zoom / 2);
						overlay_circle(ptBeg.x, ptBeg.y, (int) sqrt((double)(
									  (ptEnd.x - ptBeg.x)*(ptEnd.x - ptBeg.x) +
									  (ptEnd.y - ptBeg.y)*(ptEnd.y - ptBeg.y)))
						);
						break;
					case IDM_RECTEMPTY:
					case IDM_RECTFILLED:
						ptBeg.x = ptEnd.x = w_xpos / view_zoom * view_zoom;
						ptBeg.y = ptEnd.y = w_ypos / view_zoom * view_zoom;
						overlay_rect(ptBeg.x, ptBeg.y, ptEnd.x - ptBeg.x, ptEnd.y - ptBeg.y);
						break;
					}
					break;
				case IDM_SELECT :
					overlay_rect_clear();
					fDrawing = true;
					ptBeg.x = ptEnd.x = w_xpos / view_zoom * view_zoom;
					ptBeg.y = ptEnd.y = w_ypos / view_zoom * view_zoom;
					pMap->seldxend = pMap->seldxbeg = m_xpos;
					pMap->seldyend = pMap->seldybeg = m_ypos;

					pMap->fSelected = true;
					pMap->ptTempBeg.x = 0;
					pMap->ptTempBeg.y = 0;
					pMap->ptTempEnd.x = 0;
					pMap->ptTempEnd.y = 0;
					break;
				}
			}
			if (fPasting)
			{
				ObjList *objList = NULL;
				fPasting = false;
				pMap->fSelected = false;
				objList = pMap->PasteData(m_xpos, m_ypos);

				MapUndoableCommand* uc = new MapUndoableCommand(tmpW,
				CHANGEAREA, 0, objList);
				uc->myMapWindow->undoMan.Do(uc);
				redraw();
			}
			return TRUE;
		}
		else if (Fl::event_button() == FL_RIGHT_MOUSE)
		{
			w_xpos = Fl::event_x();
			w_ypos = Fl::event_y();

			m_xpos = w_xpos / view_zoom + iHscrollPos;
			m_ypos = w_ypos / view_zoom + iVscrollPos;

			//Way out of map limits, don't bother going any further
			if ((m_xpos >= pMap->optionsDoc->mapWidth->GetInt() + 1) ||
				(m_ypos >= pMap->optionsDoc->mapHeight->GetInt() + 1))
			{
				return TRUE;
			}


			if ((m_xpos < pMap->optionsDoc->mapWidth->GetInt()) && (m_ypos < pMap->optionsDoc->mapHeight->GetInt()) && (m_xpos >= 0) && (m_ypos >= 0))
			{
				ObjList *objList = pMap->IncrementMapBlock(m_xpos, m_ypos);

				MapUndoableCommand* uc = new MapUndoableCommand(tmpW,
				CHANGEAREA, 0, objList);

				uc->myMapWindow->undoMan.Do(uc);
				DrawMapSection(m_xpos, m_ypos, m_xpos+1, m_ypos+1,  w_xpos / view_zoom *view_zoom,
							   w_ypos / view_zoom * view_zoom);
			}
			return TRUE;
		}
	case FL_DRAG :
		if (Fl::event_button() == FL_LEFT_MOUSE)
		{
			w_xpos = Fl::event_x();
			w_ypos = Fl::event_y();

			if (w_xpos > w()-1 || w_ypos > h()-1 ||
				w_xpos < 0 || w_ypos < 0)
			return TRUE;

			m_xpos = w_xpos / view_zoom + iHscrollPos;
			m_ypos = w_ypos / view_zoom + iVscrollPos;

			/*	if (lpMapDocData->fSelected)
			{
				if ((dx >= lpMapDocData->seldxbeg) && (dx < lpMapDocData->seldxend)
				 && (dy >= lpMapDocData->seldybeg) && (dy < lpMapDocData->seldyend))
					SetCursor (LoadCursor (NULL, IDC_SIZEALL)) ;
			}
			if ((dx < lpMapDocData->MapStruct.optionsDoc->mapWidth->GetInt()) && (dy < lpMapDocData->MapStruct.height))
			{
				if (fDragging)
					sprintf(sbtext, "X:%d Y:%d | MoveX:%d MoveY:%d",
						dx+1,dy+1,dx-lpMapDocData->strdx,dy-lpMapDocData->strdy);
				else
					sprintf(sbtext, "X:%d Y:%d", dx+1,dy+1);
				SendMessage(hwndStatusBar, SB_SETTEXT, (WPARAM) NULL, (LPARAM) (LPSTR) sbtext);
			}
			*/
			if (fDragging)
				tmpW->SetCoordDisp("X:%d, Y:%d | MoveX:%d MoveY:%d", m_xpos, m_ypos,
								   m_xpos-pMap->strdx, m_ypos-pMap->strdy);
			else
				tmpW->SetCoordDisp("X:%d, Y:%d", m_xpos, m_ypos);

			make_current();

			if ((m_xpos <= pMap->optionsDoc->mapWidth->GetInt()) &&
				(m_ypos <= pMap->optionsDoc->mapHeight->GetInt()) && (m_xpos >= 0) && (m_ypos >= 0))
			{
				if (fDrawing)
				{
					switch (tmpW->iSelectionTools)
					{
					case IDM_PEN :
						{
							int typ;
							if (tmpW->iSelectionTools == IDM_ERASE)
								typ = MAP_SPACE;
							else if (tmpW->iSelectionMapSyms == IDM_MAP_TEAMBASE)
								typ = (int) tmpW->lastTeamBase;
							else if (tmpW->iSelectionMapSyms == IDM_MAP_CHECKPOINT)
								typ = pMap->FindNextCheckPoint();
							else if (tmpW->iSelectionMapSyms == IDM_MAP_CAN_UNSPEC)
								typ = pMap->FindQuickCanDir(m_xpos, m_ypos);
							else
								typ = tmpW->iSelectionMapSyms;

							//If we arent changing anything, quit right now.
							if (typ == pMap->MapData(m_xpos, m_ypos))
								break;


							MapBlockChange *chg = new MapBlockChange();
							chg->x = m_xpos;
							chg->y = m_ypos;
							chg->newchar = typ;
							chg->oldchar = pMap->MapData(m_xpos, m_ypos);
							chgListCur->Add(chg);

							//So weve updated the change list, now we should redo the swipe.
							tmpW->undoMan.DoAgain();
							DrawMapSection(m_xpos, m_ypos, m_xpos+1, m_ypos+1,  w_xpos / view_zoom *view_zoom,
							w_ypos / view_zoom * view_zoom);
							break;
						}
					case IDM_LINE :
						fl_cursor(FL_CURSOR_CROSS);
						ptEnd.x = (m_xpos - iHscrollPos) * view_zoom + (view_zoom / 2);
						ptEnd.y = (m_ypos - iVscrollPos) * view_zoom + (view_zoom / 2);
						overlay_line(ptBeg.x, ptBeg.y, ptEnd.x, ptEnd.y);
						break;
					case IDM_SHAPE :
						fl_cursor(FL_CURSOR_CROSS);
						switch (tmpW->iSelectionShape)
						{
						case IDM_CIRCLEEMPTY:
						case IDM_CIRCLEFILLED:
							ptEnd.x = (m_xpos-iHscrollPos) * view_zoom + (view_zoom / 2);
							ptEnd.y = (m_ypos-iVscrollPos) * view_zoom + (view_zoom / 2);
							overlay_circle(ptBeg.x, ptBeg.y, (int) sqrt((double)(
										   (ptEnd.x - ptBeg.x)*(ptEnd.x - ptBeg.x) +
										   (ptEnd.y - ptBeg.y)*(ptEnd.y - ptBeg.y)))
							);
							break;
						case IDM_RECTEMPTY:
						case IDM_RECTFILLED:
							ptEnd.x = (m_xpos-iHscrollPos) * view_zoom;
							ptEnd.y = (m_ypos-iVscrollPos) * view_zoom;
							overlay_rect(ptBeg.x, ptBeg.y, ptEnd.x - ptBeg.x, ptEnd.y - ptBeg.y);
							break;
						}
						break;
					case IDM_SELECT :
						fl_cursor(FL_CURSOR_CROSS);
						ptEnd.x = (m_xpos-iHscrollPos) * view_zoom;
						ptEnd.y = (m_ypos-iVscrollPos) * view_zoom;
						pMap->seldxend = m_xpos;
						pMap->seldyend = m_ypos;
						overlay_rect(ptBeg.x, ptBeg.y, ptEnd.x - ptBeg.x, ptEnd.y - ptBeg.y);
						pMap->fSelected = true;
						break;
					}
				}
				if (fDragging)
				{
					fl_cursor(FL_CURSOR_MOVE);
					pMap->ptTempBeg.x = ptBeg.x+((m_xpos-pMap->strdx)* view_zoom);
					pMap->ptTempBeg.y = ptBeg.y+((m_ypos-pMap->strdy)* view_zoom);
					pMap->ptTempEnd.x = ptEnd.x+((m_xpos-pMap->strdx)* view_zoom);
					pMap->ptTempEnd.y = ptEnd.y+((m_ypos-pMap->strdy)* view_zoom);
					overlay_rect(pMap->ptTempBeg.x, pMap->ptTempBeg.y,
								 pMap->ptTempEnd.x - pMap->ptTempBeg.x,
								 pMap->ptTempEnd.y - pMap->ptTempBeg.y);
				}
			}
			return TRUE;
		}
	case FL_MOVE :
		{
			make_current();
			w_xpos = Fl::event_x();
			w_ypos = Fl::event_y();
			m_xpos = w_xpos / view_zoom + iHscrollPos;
			m_ypos = w_ypos / view_zoom + iVscrollPos;
#if 0
			if (pMap->fSelected)
			{
				if ((m_xpos >= pMap->seldxbeg) && (m_xpos < pMap->seldxend) &&
					(m_ypos >= pMap->seldybeg) && (m_ypos < pMap->seldyend) &&
					!fPasting)
				{
					fl_cursor(FL_CURSOR_MOVE);
				}
				else
				{
					fl_cursor(FL_CURSOR_ARROW);
				}
			}
#endif
			tmpW->SetCoordDisp("X:%d, Y:%d", m_xpos, m_ypos);
			//We have to call make_current again just cause we updated the coord display??
			//How stupid is that?
			make_current();
			if (fPasting)
			{
				pMap->ptTempBeg.x = w_xpos / view_zoom * view_zoom;
				pMap->ptTempBeg.y = w_ypos / view_zoom * view_zoom;
				pMap->ptTempEnd.x = pMap->ptTempBeg.x+clipboard.cWidth*view_zoom;
				pMap->ptTempEnd.y = pMap->ptTempBeg.y+clipboard.cHeight*view_zoom;
				overlay_rect(pMap->ptTempBeg.x, pMap->ptTempBeg.y,
							 pMap->ptTempEnd.x - pMap->ptTempBeg.x,
							 pMap->ptTempEnd.y - pMap->ptTempBeg.y);
				return TRUE;
			}
		}
		break;
	case FL_RELEASE :
		if (Fl::event_button() == FL_LEFT_MOUSE)
		{
			w_xpos = Fl::event_x();
			w_ypos = Fl::event_y();

			m_xpos = w_xpos / view_zoom + iHscrollPos;
			m_ypos = w_ypos / view_zoom + iVscrollPos;

			if (m_xpos > pMap->optionsDoc->mapWidth->GetInt())
				m_xpos = pMap->optionsDoc->mapWidth->GetInt();
			if (m_ypos > pMap->optionsDoc->mapHeight->GetInt())
				m_ypos = pMap->optionsDoc->mapHeight->GetInt();
			if (m_xpos < 0)
				m_xpos = 0;
			if (m_ypos < 0)
				m_ypos = 0;

			if (fDragging)
			{
				ObjList *objList = NULL;
				fDragging = false;
				overlay_rect_clear();
				objList = pMap->dragArea(m_xpos-pMap->strdx, m_ypos-pMap->strdy, TRUE);
				MapUndoableCommand* uc = new MapUndoableCommand(tmpW, CHANGEAREA, 0, objList);
				uc->myMapWindow->undoMan.Do(uc);

				pMap->strdx = 0;
				pMap->strdy = 0;
				fl_cursor(FL_CURSOR_ARROW);
				redraw();
				return TRUE;
			}

			if ((m_xpos <= pMap->optionsDoc->mapWidth->GetInt())
			 && (m_ypos <= pMap->optionsDoc->mapHeight->GetInt())
			 && (m_xpos >= 0)
			 && (m_ypos >= 0))
			{
				if (fDrawing)
				{
					switch (tmpW->iSelectionTools)
					{
					case IDM_PEN :
					case IDM_ERASE :
							chgListCur = NULL;
							break;
					case IDM_FILL :
							break;
					case IDM_LINE :
					{
						int typ;
						XpPoint tmpbeg, tmpend;
						ObjList *objLst = NULL;

						overlay_line_clear();
						tmpbeg.x  = ptBeg.x  / view_zoom + iHscrollPos;
						tmpbeg.y = ptBeg.y / view_zoom + iVscrollPos;
						tmpend.x  = ptEnd.x  / view_zoom + iHscrollPos;
						tmpend.y = ptEnd.y / view_zoom + iVscrollPos;

						if (tmpW->iSelectionMapSyms == IDM_MAP_TEAMBASE)
							typ = (int) tmpW->lastTeamBase;
						else
							typ = tmpW->iSelectionMapSyms;

						objLst = pMap->BlockLine(tmpbeg, tmpend, typ);

						MapUndoableCommand* uc = new MapUndoableCommand(tmpW,
														CHANGEAREA, 0, objLst);
						uc->myMapWindow->undoMan.Do(uc);

						redraw();
						fl_cursor(FL_CURSOR_DEFAULT);
						break;
					}

					case IDM_SHAPE :
						switch (tmpW->iSelectionShape)
						{
						case IDM_CIRCLEEMPTY:
						case IDM_CIRCLEFILLED:
						{
							int typ;//, stype;
							XpPoint tmpbeg, tmpend;
							ObjList *objLst = NULL;

							fl_cursor(FL_CURSOR_DEFAULT);
							overlay_circle_clear();

							tmpbeg.x  = ptBeg.x  / view_zoom + iHscrollPos;
							tmpbeg.y = ptBeg.y / view_zoom + iVscrollPos;
							tmpend.x  = ptEnd.x  / view_zoom + iHscrollPos;
							tmpend.y = ptEnd.y / view_zoom + iVscrollPos;

							if (tmpW->iSelectionMapSyms == IDM_MAP_TEAMBASE)
								typ = (int) tmpW->lastTeamBase;
							else
								typ = tmpW->iSelectionMapSyms;

							objLst = pMap->BlockCircle(tmpbeg, tmpend, typ, tmpW->iSelectionShape);

							MapUndoableCommand* uc = new MapUndoableCommand(tmpW,
															CHANGEAREA, 0, objLst);

							uc->myMapWindow->undoMan.Do(uc);

							break;
						}
						case IDM_RECTEMPTY:
						case IDM_RECTFILLED:
						{
							int typ;//, stype;
							XpPoint tmpbeg, tmpend;
							ObjList *objLst = NULL;

							fl_cursor(FL_CURSOR_DEFAULT);
							overlay_rect_clear();
							tmpbeg.x  = ptBeg.x  / view_zoom + iHscrollPos;
							tmpbeg.y = ptBeg.y / view_zoom + iVscrollPos;
							tmpend.x  = ptEnd.x  / view_zoom + iHscrollPos;
							tmpend.y = ptEnd.y / view_zoom + iVscrollPos;

							if (tmpW->iSelectionMapSyms == IDM_MAP_TEAMBASE)
								typ = (int) tmpW->lastTeamBase;
							else
								typ = tmpW->iSelectionMapSyms;

							objLst = pMap->BlockRectangle(tmpbeg, tmpend, typ, tmpW->iSelectionShape);

							MapUndoableCommand* uc = new MapUndoableCommand(tmpW,
															CHANGEAREA, 0, objLst);

							uc->myMapWindow->undoMan.Do(uc);

							break;
						}
						}
						redraw();
						break;
					case IDM_SELECT :
						fl_cursor(FL_CURSOR_DEFAULT);
						pMap->SortSelectionArea();
						tmpW->UpdateClipboardButtons();
						break;
					}
				}
			}
			fDrawing = false;
			return TRUE;
		}
	}
	return Fl_Window::handle(event);
}
//The minimum size to draw detailed blocks, any zoom smaller
//than this is drawn as a simple rect.
#define MIN_NORMAL_VIEW 4

segment_t         mapicon_seg[35] = {
        {MAP_FILLED,  5, {(float) 0.01,(float) 0.99,(float) 0.99,(float) 0.01,(float) 0.01}, {(float) 0.99,(float) 0.99,(float) 0.01,(float) 0.01,(float) 0.99} }, /*  0:x MAP_WALL */
        {MAP_REC_RD, 4, {(float) 0.99,(float) 0.99,(float) 0.01,(float) 0.99,(float) 0.00}, {(float) 0.01,(float) 0.99,(float) 0.99,(float) 0.01,(float) 0.00} }, /*  1:q */
        {MAP_REC_LD, 4, {(float) 0.01,(float) 0.01,(float) 0.99,(float) 0.01,(float) 0.00}, {(float) 0.01,(float) 0.99,(float) 0.99,(float) 0.01,(float) 0.00} }, /*  2:w */
        {MAP_REC_RU, 4, {(float) 0.01,(float) 0.99,(float) 0.99,(float) 0.01,(float) 0.00}, {(float) 0.01,(float) 0.01,(float) 0.99,(float) 0.01,(float) 0.00} }, /*  3:a */
        {MAP_REC_LU, 4, {(float) 0.01,(float) 0.99,(float) 0.01,(float) 0.01,(float) 0.00}, {(float) 0.01,(float) 0.01,(float) 0.99,(float) 0.01,(float) 0.00} }, /*  4:s */

        {MAP_FUEL, 5, {(float) 0.01,(float) 0.99,(float) 0.99,(float) 0.01,(float) 0.01}, {(float) 0.99,(float) 0.99,(float) 0.01,(float) 0.01,(float) 0.99} }, /*  5:# MAP_FUEL */

        {MAP_CAN_LEFT, 4, {(float) 0.70,(float) 0.99,(float) 0.99,(float) 0.70,(float) 0.00}, {(float) 0.50,(float) 0.99,(float) 0.01,(float) 0.50,(float) 0.00} }, /*  6:d MAP_CANNON*/
        {MAP_CAN_UP,  4, {(float) 0.01,(float) 0.99,(float) 0.50,(float) 0.01,(float) 0.00}, {(float) 0.99,(float) 0.99,(float) 0.70,(float) 0.99,(float) 0.00} }, /*  7:r MAP_CANNON*/
        {MAP_CAN_DOWN, 4, {(float) 0.50,(float) 0.01,(float) 0.99,(float) 0.50,(float) 0.00}, {(float) 0.30,(float) 0.01,(float) 0.01,(float) 0.30,(float) 0.00} }, /*  8:c MAP_CANNON*/
        {MAP_CAN_RIGHT, 4, {(float) 0.30,(float) 0.01,(float) 0.01,(float) 0.30,(float) 0.00}, {(float) 0.50,(float) 0.99,(float) 0.01,(float) 0.50,(float) 0.00} }, /*  9:f MAP_CANNON*/
        {MAP_BASE, 2, {(float) 0.01,(float) 0.99,(float) 0.01,(float) 0.00,(float) 0.00}, {(float) 0.99,(float) 0.99,(float) 0.01,(float) 0.00,(float) 0.00} }, /* 10:_ MAP_BASE (UP)*/


        {MAP_GRAV_POS, 5, {(float) 0.50,(float) 0.50,(float) 0.50,(float) 0.30,(float) 0.70}, {(float) 0.30,(float) 0.70,(float) 0.50,(float) 0.50,(float) 0.50} }, /* 11:+ MAP_GRAV_POS*/
        {MAP_GRAV_NEG, 2, {(float) 0.30,(float) 0.70,(float) 0.00,(float) 0.00,(float) 0.00}, {(float) 0.50,(float) 0.50,(float) 0.00,(float) 0.00,(float) 0.00} }, /* 12:- MAP_GRAV_NET*/
        {MAP_WORM_NORMAL, 0, {(float) 0.00,(float) 0.00,(float) 0.00,(float) 0.00,(float) 0.00}, {(float) 0.00,(float) 0.00,(float) 0.00,(float) 0.00,(float) 0.00} }, /* 13:@ MAP_WORM_NORMAL */
        {MAP_WORM_OUT, 5, {(float) 0.10,(float) 0.75,(float) 0.30,(float) 0.75,(float) 0.75}, {(float) 0.10,(float) 0.75,(float) 0.75,(float) 0.75,(float) 0.30} }, /* 14:) MAP_WORM_OUT */
        {MAP_WORM_IN, 5, {(float) 0.75,(float) 0.10,(float) 0.55,(float) 0.10,(float) 0.10}, {(float) 0.75,(float) 0.10,(float) 0.10,(float) 0.10,(float) 0.55} }, /* 15:( MAP_WORM_IN */
        {MAP_TREASURE, 4, {(float) 0.01,(float) 0.01,(float) 0.99,(float) 0.99,(float) 0.00}, {(float) 0.49,(float) 0.99,(float) 0.99,(float) 0.49,(float) 0.00} }, /* 16:* MAP_TREASURE*/
        {MAP_GRAV_ACWISE, 3, {(float) 0.70,(float) 0.60,(float) 0.70,(float) 0.00,(float) 0.00}, {(float) 0.05,(float) 0.15,(float) 0.30,(float) 0.00,(float) 0.00} }, /* 17:< COUNTER CLOCKWISE GRAVITY*/
        {MAP_GRAV_CWISE, 3, {(float) 0.30,(float) 0.40,(float) 0.30,(float) 0.00,(float) 0.00}, {(float) 0.05,(float) 0.15,(float) 0.30,(float) 0.00,(float) 0.00} }, /* 18:> CLOCKWISE GRAVITY*/
        {MAP_TARGET, 5, {(float) 0.01,(float) 0.99,(float) 0.99,(float) 0.01,(float) 0.01}, {(float) 0.99,(float) 0.99,(float) 0.01,(float) 0.01,(float) 0.99} }, /* 19:! MAP_TARGET */

        {MAP_SPACE, 0, {(float) 0.00,(float) 0.00,(float) 0.00,(float) 0.00,(float) 0.00}, {(float) 0.00,(float) 0.00,(float) 0.00,(float) 0.00,(float) 0.00} }, /* 20:  MAP_SPACE */

        {MAP_DEC_FLD, 5, {(float) 0.01,(float) 0.99,(float) 0.99,(float) 0.01,(float) 0.01}, {(float) 0.99,(float) 0.99,(float) 0.01,(float) 0.01,(float) 0.99} }, /* 21:b MAP_DECORATION */
        {MAP_DEC_RD, 4, {(float) 0.99,(float) 0.99,(float) 0.01,(float) 0.99,(float) 0.00}, {(float) 0.01,(float) 0.99,(float) 0.99,(float) 0.01,(float) 0.00} }, /* 22:t */
        {MAP_DEC_LD, 4, {(float) 0.01,(float) 0.01,(float) 0.99,(float) 0.01,(float) 0.00}, {(float) 0.01,(float) 0.99,(float) 0.99,(float) 0.01,(float) 0.00} }, /* 23:y */
        {MAP_DEC_RU, 4, {(float) 0.01,(float) 0.99,(float) 0.99,(float) 0.01,(float) 0.00}, {(float) 0.01,(float) 0.01,(float) 0.99,(float) 0.01,(float) 0.00} }, /* 24:g */
        {MAP_DEC_LU, 4, {(float) 0.01,(float) 0.99,(float) 0.01,(float) 0.01,(float) 0.00}, {(float) 0.01,(float) 0.01,(float) 0.99,(float) 0.01,(float) 0.00} }, /* 25:h */

        {MAP_BASE_ORNT, 4, {(float) 0.25,(float) 0.75,(float) 0.50,(float) 0.25,(float) 0.00}, {(float) 0.15,(float) 0.15,(float) 0.80,(float) 0.15,(float) 0.00} }, /* 26:$ BASE_ORIENT*/
        {MAP_ITEM_CONC, 4, {(float) 0.50,(float) 0.01,(float) 0.99,(float) 0.50,(float) 0.00}, {(float) 0.01,(float) 0.99,(float) 0.99,(float) 0.01,(float) 0.00} }, /* 27:% ITEM CONCENTRATOR*/
        {MAP_CRNT_UP, 5, {(float) 0.50,(float) 0.50,(float) 0.75,(float) 0.50,(float) 0.25}, {(float) 0.99,(float) 0.01,(float) 0.50,(float) 0.01,(float) 0.50} }, /* 28:i CURRENT UP*/
        {MAP_CRNT_LT, 5, {(float) 0.99,(float) 0.01,(float) 0.50,(float) 0.01,(float) 0.50}, {(float) 0.50,(float) 0.50,(float) 0.75,(float) 0.50,(float) 0.25} }, /* 29:j CURRENT LEFT*/
        {MAP_CRNT_RT, 5, {(float) 0.01,(float) 0.99,(float) 0.50,(float) 0.99,(float) 0.50}, {(float) 0.50,(float) 0.50,(float) 0.75,(float) 0.50,(float) 0.25} }, /* 30:k CURRENT RIGHT*/
        {MAP_CRNT_DN, 5, {(float) 0.50,(float) 0.50,(float) 0.75,(float) 0.50,(float) 0.25}, {(float) 0.01,(float) 0.99,(float) 0.50,(float) 0.99,(float) 0.50} }, /* 31:m CURRENT DOWN*/

        {MAP_EMPTYTREASURE, 4, {(float) 0.01,(float) 0.01,(float) 0.99,(float) 0.99,(float) 0.00}, {(float) 0.49,(float) 0.99,(float) 0.99,(float) 0.49,(float) 0.00} }, /* 32:^ MAP_EMPTYTREASURE*/
        {MAP_FRICTION, 5, {(float) 0.01,(float) 0.99,(float) 0.99,(float) 0.01,(float) 0.01}, {(float) 0.99,(float) 0.99,(float) 0.01,(float) 0.01,(float) 0.99} }, /* 33:z MAP_FRICTION */

		{MAP_ASTEROID_CONC, 5, {(float) 0.10,(float) 0.90,(float) 0.90,(float) 0.10,(float) 0.10}, {(float) 0.90,(float) 0.90,(float) 0.10,(float) 0.10,(float) 0.90} }, /* 34:& MAP_ASTEROID_CONC */
};
segment_t         mapicondet_seg[9] = {
        {MAP_TARGET, 5, {(float) 0.25,(float) 0.75,(float) 0.75,(float) 0.25,(float) 0.25}, {(float) 0.75,(float) 0.75,(float) 0.25,(float) 0.25,(float) 0.75} }, /* 0:! TARGET (DETAIL)*/
        {MAP_ITEM_CONC, 4, {(float) 0.55,(float) 0.25,(float) 0.90,(float) 0.55,(float) 0.00}, {(float) 0.15,(float) 0.85,(float) 0.75,(float) 0.15,(float) 0.00} }, /* 1:% ITEM CONCENTRATOR (DETAIL)*/
        {MAP_ITEM_CONC, 4, {(float) 0.65,(float) 0.45,(float) 0.80,(float) 0.65,(float) 0.00}, {(float) 0.30,(float) 0.65,(float) 0.55,(float) 0.30,(float) 0.00} }, /* 2:% ITEM CONCENTRATOR (DETAIL)*/
        {MAP_BASE, 2, {(float) 0.01,(float) 0.99,(float) 0.01,(float) 0.00,(float) 0.00}, {(float) 0.01,(float) 0.01,(float) 0.01,(float) 0.00,(float) 0.00} }, /* 3:_ MAP_BASE_TEAM (DOWN)*/
        {MAP_BASE, 2, {(float) 0.01,(float) 0.01,(float) 0.01,(float) 0.00,(float) 0.00}, {(float) 0.01,(float) 0.99,(float) 0.01,(float) 0.00,(float) 0.00} }, /* 4:_ MAP_BASE_TEAM (RIGHT)*/
        {MAP_BASE, 2, {(float) 0.99,(float) 0.99,(float) 0.99,(float) 0.00,(float) 0.00}, {(float) 0.01,(float) 0.99,(float) 0.01,(float) 0.00,(float) 0.00} }, /* 5:_ MAP_BASE_TEAM (LEFT)*/
        {MAP_BASE_ORNT, 5, {(float) 0.50,(float) 0.50,(float) 0.50,(float) 0.40,(float) 0.60}, {(float) 0.30,(float) 0.50,(float) 0.40,(float) 0.40,(float) 0.40} }, /* 6:$ BASE_ORIENT (DETAIL)*/
		{MAP_ASTEROID_CONC, 5, {(float) 0.25,(float) 0.75,(float) 0.75,(float) 0.25,(float) 0.25}, {(float) 0.75,(float) 0.75,(float) 0.25,(float) 0.25,(float) 0.75} }, /* 34:& MAP_ASTEROID_CONC */
		{MAP_ASTEROID_CONC, 5, {(float) 0.40,(float) 0.60,(float) 0.605,(float) 0.40,(float) 0.40}, {(float) 0.60,(float) 0.60,(float) 0.40,(float) 0.40,(float) 0.60} }, /* 34:& MAP_ASTEROID_CONC */
};

/* ascii char -32 */
/*int mapicon_ptr[91] = {	20, 19,  0,  5, 26,
27,  0,  0, 15, 14,
16, 11,  0, 12,  0,
0, 51, 51, 51, 51,
51, 51, 51, 51, 51,
51,  0,  0, 17,  0,
18,  0, 13, 50, 50,
50, 50, 50, 50, 50,
50, 50, 50, 50, 50,
50, 50, 50, 50, 50,
50, 50, 50, 50, 50,
50, 50, 50, 50,  0,
0,  0, 32, 10,  0,
3, 21,  8,  6,  0,
9, 24, 25, 28, 29,
30,  0, 31,  0,  0,
0,  1,  7,  4, 22,
0,  0,  2,  0, 23,
33};*/
///////////////////////////////////////////////////////////////////////////////
void MapEditWindow::DrawMapOutline (XpPoint ptB, XpPoint ptE, int color)
{
	fl_color(color);
	fl_rect(ptB.x, ptB.y, ptE.x - ptB.x, ptE.y - ptB.y);
}
/***************************************************************************/
/* DrawSmallBlock                                                          */
/* Arguments :                                                             */
/*   hdc                                                                   */
/*   x:xcoord to start at.                                                 */
/*   y:ycoord to start at.                                                 */
/*   hNewBrush: brush draw block in.                                       */
/*                                                                         */
/*                                                                         */
/* Purpose : Draw a simplified block as a filled in rectangle using a brush*/
/***************************************************************************/
void MapEditWindow::DrawSmallBlock (int x, int y, int color)
{
        fl_color(color);
        fl_rectf(x, y, view_zoom, view_zoom);

}
/***************************************************************************/
/* DrawMapSection                                                          */
/* Arguments :                                                             */
/*   x : the xcoord in map coordinates to start at.                        */
/*   y : the ycoord in map coordinates to start at.                        */
/*   width : the width of the section to draw.                             */
/*   height : the height of the section to draw.                           */
/*   xcrd : x coord in pixels to start at.                                 */
/*   ycrd : y coord in pixels to start at.                                 */
/* Purpose : Draw a section of the map, one block at a time.               */
/***************************************************************************/
bool MapEditWindow::DrawMapSection(int x, int y, int width, int height, int xcrd, int ycrd)
{
        int                   i,j,w,h,data;
        int xpos, ypos;

        w = width+x;
        h = height+y;

		make_current();
        /*Set the font so that later, when we draw checkpoints and bases, the text
        will look decent.*/
        GetZoomFont();

        for (i=x, xpos = xcrd;i<=w;i++, xpos+=view_zoom)
                for (j=y, ypos = ycrd;j<=h;j++, ypos+=view_zoom)
                        if ( (i<width) && (j<height) )
                        {
                                data = pMap->MapData(i, j);
                                DrawMapPic(xpos,ypos/*, mapicon_ptr[data-32]*/, data, i, j);
                        }
        return false;
}
/***************************************************************************/
/* DrawMapPic                                                              */
/* Arguments :                                                             */
/*   x: xcoord to draw at in pixels.                                       */
/*   y: ycoord to draw at in pixels.                                       */
/*   picnum: which block to draw.                                          */
/*   data: optional character to draw in for text.                         */
/*   mapx: x location in map coordinates.                                  */
/*   mapy: y location in map coordinates.                                  */
/* Purpose :  draw the specified block at the place & size given.          */
/***************************************************************************/
bool MapEditWindow::DrawMapPic(int x, int y,/* int picnum,*/ char data, int mapx, int mapy)
{
        int numpoints = 0;
        XpPoint points[5];
        int i, j, arc;
        char strng[2];
        char tteam[2];
        int t;

        fl_color(FL_BLACK);
		fl_rectf(x, y, view_zoom, view_zoom);
        //Find the team base closest the current grid space, if we need to.
        //We only need to if teamplay is on; if this is a fuel or cannon, we only need to if
        //the teamFuel or teamCannon options are on.
        if (pMap->optionsDoc->teamPlay->GetBool() &&
                ((data == MAP_FUEL && pMap->optionsDoc->teamFuel->GetBool()) ||
                (pMap->optionsDoc->teamCannons->GetBool() && (data == MAP_CAN_LEFT || data == MAP_CAN_UP || data == MAP_CAN_DOWN || data == MAP_CAN_RIGHT)) ||
                data == MAP_BASE || data == MAP_TREASURE || data == MAP_TARGET || data == MAP_EMPTYTREASURE) )
        {
                t=pMap->Find_closest_team(mapx, mapy);
		        sprintf(tteam, "%d\0", t);
                if (data == MAP_BASE)
                {
                        data = '0';
                }
        }

		if ( ((data >= 'A') && (data <= 'Z')) || ((data >= '0') && (data <= '9')) )
        {// TEAMBASES CHECKPOINTS OUTPUT TEXT
                strng[0] = data;
                strng[1] = '\0';
                if (view_zoom >= 5)
                {
                        fl_color(COLOR_TEXT);
                        fl_draw(strng, x+(int)(.3* view_zoom), y+(int)(view_zoom*.8));
                }
        }

		if ( (data >= 'A') && (data <= 'Z') )
        {// CHECKPOINTS ARE DONE
                return false;
        }
		if ( (data >= '0') && (data <= '9') )
        {// TEAMBASES SHOULD GO ON AS NORMAL BASES
                data = MAP_BASE;
        }

        //----------------FILL THE POINTS ARRAY WITH THE SIZES FROM THE ICON SEGMENT-------
		for (j = 0; j < 35; j++)
		{
			if (mapicon_seg[j].char_val == data)
			{
				numpoints = mapicon_seg[j].num_points;
				break;
			}
		}
        for (i=0;i<numpoints; i++)
        {
                points[i].x = (long) (mapicon_seg[j].x[i] * view_zoom + x);
                points[i].y = (long) (mapicon_seg[j].y[i] * view_zoom + y);
        }

        //------------------------------------------------------------
        if ((data == MAP_REC_RD) ||  (data == MAP_REC_LD)
			|| (data == MAP_FILLED) ||  (data == MAP_REC_LU)
			 ||  (data == MAP_REC_RU) )
        {// Walls
                if (view_zoom > MIN_NORMAL_VIEW)
                {
                        fl_color(COLOR_WALL);
						if (BIT(iniClient.instruments, SHOW_FILLED_WORLD))
						{
							fl_begin_polygon();
							for (i=0;i<numpoints-1; i++)
							{
								fl_vertex(points[i].x, points[i].y);
							}
							fl_end_polygon();
						}
						else
							for (i=0;i<numpoints-1; i++)
							{
								fl_line(points[i].x, points[i].y, points[i+1].x, points[i+1].y);
							}


               }
                else
                        DrawSmallBlock(x, y, COLOR_WALL);

                return false;
        }
        else if (data == MAP_FUEL)
        {// Fuels
                if (view_zoom > MIN_NORMAL_VIEW)
                {
                        fl_color(COLOR_WALL);
                        for (i=0;i<numpoints-1; i++)
                        {
                                fl_line(points[i].x, points[i].y, points[i+1].x, points[i+1].y);
                        }
                        fl_color(COLOR_FUEL);
                        fl_rectf(points[3].x+1, points[3].y+1, view_zoom-2, view_zoom-2);
                        if ((pMap->optionsDoc->teamPlay->GetBool()) && (pMap->optionsDoc->teamFuel->GetBool()))
                        {
                                fl_font(FL_TIMES, (int) (view_zoom*.5) );
                                fl_color(COLOR_TEXT);
                                fl_draw(tteam, x+(int)(.4* view_zoom), y+(int)(view_zoom*.65));
                                GetZoomFont();
                                fl_color(COLOR_FUEL);
                        }
                }
                else
                        DrawSmallBlock(x, y, COLOR_FUEL);

                return false;
        }
        else if ( (data == MAP_CAN_LEFT) ||  (data == MAP_CAN_UP)
			|| (data == MAP_CAN_DOWN) ||  (data == MAP_CAN_RIGHT) )
        {// Cannons
                if (view_zoom > MIN_NORMAL_VIEW)
                {
                        fl_color(COLOR_CANNON);
                        for (i=0;i<numpoints-1; i++)
                        {
                                fl_line(points[i].x, points[i].y, points[i+1].x, points[i+1].y);
                        }
                        if ((pMap->optionsDoc->teamPlay->GetBool()) && (pMap->optionsDoc->teamCannons->GetBool()))
                        {
                                fl_color(COLOR_TEXT);
                                fl_font(FL_TIMES, (int) (view_zoom*.7) );
                                switch (data)
                                {
                                case MAP_CAN_LEFT:
                                        fl_draw(tteam, x+(int)(.3* view_zoom), y+(int)(view_zoom*.7));
                                        break;
                                case MAP_CAN_UP:
                                        fl_draw(tteam, x+(int)(.35* view_zoom), y+(int)(view_zoom*.6));
                                        break;
                                case MAP_CAN_DOWN:
                                        fl_draw(tteam, x+(int)(.35* view_zoom), y+(int)(view_zoom*.9));
                                        break;
                                case MAP_CAN_RIGHT:
                                        fl_draw(tteam, x+(int)(.4* view_zoom), y+(int)(view_zoom*.7));
                                        break;
                                }
                                GetZoomFont();
                                fl_color(COLOR_CANNON);
                        }
                }
                else
                        DrawSmallBlock(x, y, COLOR_CANNON);

                return false;
        }
        else if (data == MAP_BASE)
        {// Bases
                if (view_zoom > MIN_NORMAL_VIEW)
                {
						int picnum;
                        bool orient = false;
                        //Orient Upwards - make sure we don't flip some other direction even though
                        //we should be forced upwards
                        if ((mapy > 0 && pMap->MapData(mapx ,mapy-1) == MAP_BASE_ORNT) ||
                                (mapy == 0 && pMap->MapData(mapx, pMap->optionsDoc->mapHeight->GetInt()-1) == MAP_BASE_ORNT && pMap->optionsDoc->edgeWrap->GetBool()) ) //wrap
                        {
                                ;
                        }
                        //Orient Downwards
                        else if ((mapy < pMap->optionsDoc->mapHeight->GetInt() - 1 && pMap->MapData(mapx, mapy+1) == MAP_BASE_ORNT) ||
                                (mapy == pMap->optionsDoc->mapHeight->GetInt()-1 &&
                                pMap->MapData(mapx, 0) == MAP_BASE_ORNT && pMap->optionsDoc->edgeWrap->GetBool())) //wrap
                        {
                                picnum=3;
                                orient = true;
                        }
                        //Orient Rightwards
                        else if ((mapx < pMap->optionsDoc->mapWidth->GetInt() - 1 && pMap->MapData(mapx+1, mapy) == MAP_BASE_ORNT) ||
                                (mapx == pMap->optionsDoc->mapWidth->GetInt() - 1 &&
                                pMap->MapData(0, mapy) == MAP_BASE_ORNT && pMap->optionsDoc->edgeWrap->GetBool())) //wrap
                        {
                                picnum=4;
                                orient = true;
                        }
                        //Orient Leftwards
                        else if ((mapx > 0 && pMap->MapData(mapx-1, mapy) == MAP_BASE_ORNT) ||
                                (mapx == 0 && pMap->MapData(pMap->optionsDoc->mapWidth->GetInt()-1, mapy) == MAP_BASE_ORNT &&
                                pMap->optionsDoc->edgeWrap->GetBool())) //wrap
                        {
                                picnum=5;
                                orient = true;
                        }
                        fl_color(COLOR_BASE);
                        if (orient)
                        {
                                numpoints = mapicondet_seg[picnum].num_points;
                                for (i=0;i<numpoints; i++)
                                {
                                        points[i].x = (long) (mapicondet_seg[picnum].x[i] * view_zoom + x);
                                        points[i].y = (long) (mapicondet_seg[picnum].y[i] * view_zoom + y);
                                }
                        }


                        for (i=0;i<numpoints-1; i++)
                        {
                                fl_line(points[i].x, points[i].y, points[i+1].x, points[i+1].y);
                        }
                }
                else
                        DrawSmallBlock(x, y, COLOR_BASE);

                return false;
        }
        else if ( (data == MAP_GRAV_POS) ||  (data == MAP_GRAV_NEG) )
        {// +/- gravity
                if (view_zoom > MIN_NORMAL_VIEW)
                {
                        fl_color(COLOR_GRAVITY);
                        for (i=0;i<numpoints-1; i++)
                        {
                                fl_line(points[i].x, points[i].y, points[i+1].x, points[i+1].y);
                        }
                }
                else
                        DrawSmallBlock(x, y, COLOR_GRAVITY);

                //Go on and draw the arcs too!
        }
        else if ((data == MAP_WORM_NORMAL) ||  (data == MAP_WORM_OUT)
			|| (data == MAP_WORM_IN))
        {// wormholes
                if (view_zoom > MIN_NORMAL_VIEW)
                {
                        fl_color(COLOR_WORMHOLE);
                        for (i=0;i<numpoints-1; i++)
                        {
                                fl_line(points[i].x, points[i].y, points[i+1].x, points[i+1].y);
                        }
                }
                else
                        DrawSmallBlock(x, y, COLOR_WORMHOLE);
                //Go on and draw the arcs too!
        }
        else if ((data == MAP_TREASURE) || (data==MAP_EMPTYTREASURE))
        {// Treasure
                if (view_zoom > MIN_NORMAL_VIEW)
                {
                        fl_color(COLOR_TREASURE);
                        for (i=0;i<numpoints-1; i++)
                        {
                                fl_line(points[i].x, points[i].y, points[i+1].x, points[i+1].y);
                        }

                        if (pMap->optionsDoc->teamPlay->GetBool())
                        {
                                fl_font(FL_TIMES, (int) (view_zoom*.5) );
                                fl_color(COLOR_TEXT);
                                fl_draw(tteam, x+(int)(.5* view_zoom), y+(int)(view_zoom*.5));
                                GetZoomFont();
                                fl_color(COLOR_TREASURE);
                        }
                }
                else
                        DrawSmallBlock(x, y, COLOR_TREASURE);

                //Go on and draw the arcs too!
        }
        else if ( (data == MAP_GRAV_ACWISE) ||  (data == MAP_GRAV_CWISE) )
        {// clockwise and anti clockwise gravity

                if (view_zoom > MIN_NORMAL_VIEW)
                {
                        fl_color(COLOR_GRAVITY);
                        for (i=0;i<numpoints-1; i++)
                        {
                                fl_line(points[i].x, points[i].y, points[i+1].x, points[i+1].y);
                        }
                }
                else
                        DrawSmallBlock(x, y, COLOR_GRAVITY);

                //Go on and draw the arcs too!
        }
        else if (data == MAP_TARGET)
        {// Target
                if (view_zoom > MIN_NORMAL_VIEW)
                {
						int picnum;
                        fl_color(COLOR_TARGET);
                        for (i=0;i<numpoints-1; i++)
                        {
                                fl_line(points[i].x, points[i].y, points[i+1].x, points[i+1].y);
                        }

                        //Draw the details
                        picnum=0;
                        numpoints = mapicondet_seg[picnum].num_points;
                        for (i=0;i<numpoints; i++)
                        {
                                points[i].x = (long) (mapicondet_seg[picnum].x[i] * view_zoom + x);
                                points[i].y = (long) (mapicondet_seg[picnum].y[i] * view_zoom + y);
                        }
                        for (i=0;i<numpoints-1; i++)
                        {
                                fl_line(points[i].x, points[i].y, points[i+1].x, points[i+1].y);
                        }

                        if (pMap->optionsDoc->teamPlay->GetBool())
                        {
                                fl_font(FL_TIMES, (int) (view_zoom*.5) );
                                fl_color(COLOR_TEXT);
                                fl_draw(tteam, x+(int)(.4* view_zoom), y+(int)(view_zoom*.65));
                                GetZoomFont();
                                fl_color(COLOR_TARGET);
                        }
                }
                else
                        DrawSmallBlock(x, y, COLOR_TARGET);

                return false;
        }
        else if (data == MAP_SPACE)
        {// Space
                return false;
        }
        else if ( (data == MAP_DEC_RD) ||  (data == MAP_DEC_LD)
			|| (data == MAP_DEC_FLD) ||  (data == MAP_DEC_LU)
			 ||  (data == MAP_DEC_RU) || (data == MAP_FRICTION) )
        {// Decorations

                if (view_zoom > MIN_NORMAL_VIEW)
                {
                        if (data == MAP_FRICTION)
                        {
                                fl_color(COLOR_FRICTION);
                        }
                        else
                        {
                                fl_color(COLOR_DECOR);
                        }
						if (BIT(iniClient.instruments, SHOW_FILLED_DECOR) && data != MAP_FRICTION)
						{
							fl_begin_polygon();
							for (i=0;i<numpoints-1; i++)
							{
								fl_vertex(points[i].x, points[i].y);
							}
							fl_end_polygon();
						}
						else
							for (i=0;i<numpoints-1; i++)
							{
                                fl_line(points[i].x, points[i].y, points[i+1].x, points[i+1].y);
							}

                        //			if (FilledWorld)
                        //				Polygon (hdc, points, mapicon_seg[picnum].num_points);
                        //			else
                        //				Polyline(hdc, points, mapicon_seg[picnum].num_points);
                }
                else
                        if (data == MAP_FRICTION)
                        {
                                DrawSmallBlock(x, y, COLOR_FRICTION);
                        }
                        else
                                DrawSmallBlock(x, y, COLOR_DECOR);

                        return false;
        }
        else if (data == MAP_BASE_ORNT)
        {// Base Orient

                if (view_zoom > MIN_NORMAL_VIEW)
                {
						int picnum;
                        fl_color(COLOR_BASE);
                        for (i=0;i<numpoints-1; i++)
                        {
                                fl_line(points[i].x, points[i].y, points[i+1].x, points[i+1].y);
                        }
                        picnum=6;
                        numpoints = mapicon_seg[picnum].num_points;
                        for (i=0;i<numpoints; i++) {
                                points[i].x = (long) (mapicondet_seg[picnum].x[i] * view_zoom + x);
                                points[i].y = (long) (mapicondet_seg[picnum].y[i] * view_zoom + y);
                        }
                        for (i=0;i<numpoints-1; i++)
                        {
                                fl_line(points[i].x, points[i].y, points[i+1].x, points[i+1].y);
                        }
                }

                return false;
        }
        else if (data == MAP_ITEM_CONC)
        {// Item Concentrator

                if (view_zoom > MIN_NORMAL_VIEW)
                {
						int picnum;
                        fl_color(COLOR_ITEM_CONC);
                        for (i=0;i<numpoints-1; i++)
                        {
                                fl_line(points[i].x, points[i].y, points[i+1].x, points[i+1].y);
                        }
                        //Draw the details
                        picnum=1;
                        numpoints = mapicondet_seg[picnum].num_points;
                        for (i=0;i<numpoints; i++)
                        {
                                points[i].x = (long) (mapicondet_seg[picnum].x[i] * view_zoom + x);
                                points[i].y = (long) (mapicondet_seg[picnum].y[i] * view_zoom + y);
                        }

                        for (i=0;i<numpoints-1; i++)
                        {
                                fl_line(points[i].x, points[i].y, points[i+1].x, points[i+1].y);
                        }
                        picnum=2;
                        numpoints = mapicondet_seg[picnum].num_points;
                        for (i=0;i<numpoints; i++)
                        {
                                points[i].x = (long) (mapicondet_seg[picnum].x[i] * view_zoom + x);
                                points[i].y = (long) (mapicondet_seg[picnum].y[i] * view_zoom + y);
                        }

                        for (i=0;i<numpoints-1; i++)
                        {
                                fl_line(points[i].x, points[i].y, points[i+1].x, points[i+1].y);
                        }
                }
                else
                        DrawSmallBlock(x, y, COLOR_ITEM_CONC);
                return false;
        }
        else if ((data == MAP_CRNT_UP) ||  (data == MAP_CRNT_LT)
			|| (data == MAP_CRNT_RT) ||  (data == MAP_CRNT_DN))
        {// Gravity Currents

                if (view_zoom > MIN_NORMAL_VIEW)
                {
                        fl_color(COLOR_GRAVITY);
                        for (i=0;i<numpoints-1; i++)
                        {
                                fl_line(points[i].x, points[i].y, points[i+1].x, points[i+1].y);
                        }
                }
                else
                        DrawSmallBlock(x, y, COLOR_GRAVITY);

                return false;
        }
        else if (data == MAP_ASTEROID_CONC)
        {// Asteroid Concentrator

                if (view_zoom > MIN_NORMAL_VIEW)
                {
						int picnum;
                        fl_color(COLOR_ITEM_CONC);
                        for (i=0;i<numpoints-1; i++)
                        {
                                fl_line(points[i].x, points[i].y, points[i+1].x, points[i+1].y);
                        }
                        //Draw the details
                        picnum=7;
                        numpoints = mapicondet_seg[picnum].num_points;
                        for (i=0;i<numpoints; i++)
                        {
                                points[i].x = (long) (mapicondet_seg[picnum].x[i] * view_zoom + x);
                                points[i].y = (long) (mapicondet_seg[picnum].y[i] * view_zoom + y);
                        }

                        for (i=0;i<numpoints-1; i++)
                        {
                                fl_line(points[i].x, points[i].y, points[i+1].x, points[i+1].y);
                        }
                        picnum=8;
                        numpoints = mapicondet_seg[picnum].num_points;
                        for (i=0;i<numpoints; i++)
                        {
                                points[i].x = (long) (mapicondet_seg[picnum].x[i] * view_zoom + x);
                                points[i].y = (long) (mapicondet_seg[picnum].y[i] * view_zoom + y);
                        }

                        for (i=0;i<numpoints-1; i++)
                        {
                                fl_line(points[i].x, points[i].y, points[i+1].x, points[i+1].y);
                        }
                }
                else
                        DrawSmallBlock(x, y, COLOR_ITEM_CONC);
                return false;
        }
        //---------------------------------------------------------------
        arc = (int)(.7*view_zoom);
        if ( ((view_zoom-arc)/2)*2 != (view_zoom-arc) )
                arc--;
        if (((data == MAP_GRAV_POS) ||  (data == MAP_GRAV_NEG)) && (view_zoom > MIN_NORMAL_VIEW))
        {// +/- Gravity
                //		fl_arc((int)(x+.10*view_zoom),(int)(y+.10*view_zoom), (int)(.8*view_zoom), (int)(.8*view_zoom), 0, 360);
                fl_circle((int)(x+.50*view_zoom),(int)(y+.50*view_zoom), (int)(.4*view_zoom));
                return false;
        }
        else if ( ((data == MAP_WORM_NORMAL) ||  (data == MAP_WORM_OUT)
			|| (data == MAP_WORM_IN)) && (view_zoom > MIN_NORMAL_VIEW) )
        {// Wormhole
                //		fl_arc((int)(x+.20*view_zoom),(int)(y+.20*view_zoom), (int)(.75*view_zoom), (int)(.75*view_zoom), 0, 360);
                //		fl_arc((int)(x+.15*view_zoom),(int)(y+.15*view_zoom), (int)(.4*view_zoom), (int)(.4*view_zoom), 0, 360);
                //		fl_arc((int)(x+.16*view_zoom),(int)(y+.16*view_zoom), (int)(.55*view_zoom), (int)(.55*view_zoom), 0, 360);
                fl_circle((int)(x+.50*view_zoom),(int)(y+.50*view_zoom), (int)(.38*view_zoom));
                fl_circle((int)(x+.35*view_zoom),(int)(y+.35*view_zoom), (int)(.28*view_zoom));
                fl_circle((int)(x+.25*view_zoom),(int)(y+.25*view_zoom), (int)(.19*view_zoom));
                return false;
        }
        else if (((data == MAP_TREASURE) || (data==MAP_EMPTYTREASURE)) && (view_zoom > MIN_NORMAL_VIEW))
        {// Treasure
                fl_arc((int)(x),(int)(y), (int)(view_zoom), (int)(view_zoom), 0, 180);
                if (data != MAP_EMPTYTREASURE)
                {
                        fl_color(COLOR_BALL);
                        //fl_arc((int)(x+.15*view_zoom),(int)(y+.6*view_zoom), (int)(.4*view_zoom), (int)(.4*view_zoom), 0, 360);
                        fl_circle((int)(x+.35*view_zoom),(int)(y+.7*view_zoom),(int)(.25*view_zoom));
                }
                return false;
        }
        else if (((data == MAP_GRAV_ACWISE) || (data == MAP_GRAV_CWISE)) && (view_zoom > MIN_NORMAL_VIEW))
        {// Gravity

                fl_arc((int)(x+.10*view_zoom),(int)(y+.10*view_zoom), (int)(.8*view_zoom), (int)(.8*view_zoom), (double) 120, (double) 60);
                return false;
        }
        return false;
}
//The following is based on the limited overlay support that's provided with
//fltk. The standard stuff wouldn't do enough for our purposes though, so
//these member functions were created. If FLTK is ever extended to include
//better overlay support, we should probably switch to that if possible, but
//for now these functions serve our purpose.
//-JLM
#include "FL/x.H"
///////////////////////////////////////////////////////////////////////////////
void MapEditWindow::draw_current_line() {
#ifdef WIN32
int old = SetROP2(fl_gc, R2_NOT);
fl_line(plx, ply, plx2, ply2);
SetROP2(fl_gc, old);
#else
XSetFunction(fl_display, fl_gc, GXxor);
XSetForeground(fl_display, fl_gc, 0xffffffff);
XDrawLine(fl_display, fl_window, fl_gc, plx, ply, plx2, ply2);
XSetFunction(fl_display, fl_gc, GXcopy);
#endif
}
///////////////////////////////////////////////////////////////////////////////
void MapEditWindow::overlay_line_clear() {
if (plx2 > 0) {draw_current_line(); plx2 = 0;}
}
///////////////////////////////////////////////////////////////////////////////
void MapEditWindow::overlay_line(int x, int y, int x2, int y2) {
if (plx2 > 0) {
if (x==plx && y==ply && x2==plx2 && y2==ply2) return;
draw_current_line();
}
plx = x; ply = y; plx2 = x2; ply2 = y2;
draw_current_line();
}
///////////////////////////////////////////////////////////////////////////////
void MapEditWindow::draw_current_rect() {
#ifdef WIN32
int old = SetROP2(fl_gc, R2_NOT);
fl_rect(px, py, pw, ph);
SetROP2(fl_gc, old);
#else
XSetFunction(fl_display, fl_gc, GXxor);
XSetForeground(fl_display, fl_gc, 0xffffffff);
XDrawRectangle(fl_display, fl_window, fl_gc, px, py, pw, ph);
XSetFunction(fl_display, fl_gc, GXcopy);
#endif
}
///////////////////////////////////////////////////////////////////////////////
void MapEditWindow::overlay_rect_clear() {
if (pw > 0) {draw_current_rect(); pw = 0;}
}
///////////////////////////////////////////////////////////////////////////////
void MapEditWindow::overlay_rect(int x, int y, int w, int h) {
if (w < 0) {x += w; w = -w;} else if (!w) w = 1;
if (h < 0) {y += h; h = -h;} else if (!h) h = 1;
if (pw > 0) {
if (x==px && y==py && w==pw && h==ph) return;
draw_current_rect();
}
px = x; py = y; pw = w; ph = h;
draw_current_rect();
}
///////////////////////////////////////////////////////////////////////////////
void MapEditWindow::draw_current_circle() {
#ifdef WIN32
int old = SetROP2(fl_gc, R2_NOT);
fl_circle(pcx, pcy, pcr);
SetROP2(fl_gc, old);
#else
XSetFunction(fl_display, fl_gc, GXxor);
XSetForeground(fl_display, fl_gc, 0xffffffff);
fl_circle(pcx, pcy, pcr);
XSetFunction(fl_display, fl_gc, GXcopy);
#endif
}
///////////////////////////////////////////////////////////////////////////////
void MapEditWindow::overlay_circle_clear() {
if (pcr > 0) {draw_current_circle(); pcr = 0;}
}
///////////////////////////////////////////////////////////////////////////////
void MapEditWindow::overlay_circle(int x, int y, int r) {
if (pcr > 0) {
if (x==pcx && y==pcy && r==pcr) return;
draw_current_circle();
}
pcx = x; pcy = y; pcr = r;
draw_current_circle();
}
