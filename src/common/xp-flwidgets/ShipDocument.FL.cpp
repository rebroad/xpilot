/* $Id: ShipDocument.FL.cpp,v 1.1 2004/05/28 17:35:32 dick Exp $
 *
 * ShipDocument.FL.cpp - FL methods for ShipDocument
 *
 * common/xp-flwidgets - A library of FLTK widgets for XPilot5.
 *
 *      Copyright (C) 2004 by
 *
 *      The XPilot Authors   <xpilot@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
 *      Jarrod Miller        <jarrod@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
 *      Bj�rn Stabell        <bjoern@xpilot.org>
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
 * $Log: ShipDocument.FL.cpp,v $
 * Revision 1.1  2004/05/28 17:35:32  dick
 * Move single ship drawing to here.  This is a member function of ShipDocument,
 * but ShipDocument is in common, where we don't want any fltk.
 * So, if you reference this function, it will drag in all of fltk.
 * Perhaps this was not the best way to do this.  I could have just had a
 * regular function that takes a ShipDocument* .
 *
 */
#include "StdAfx.h"

#include "FL/fl_draw.H"

#include "guiconst.h"
#include "ShipDocument.h"

/* Coordinate conversion */
int rs(int i)
{
	return(i);
//	float f=(SPM_SIZE/52.0);
//	return (int)((double)(i)*f);
}


///////////////////////////////////////////////////////////////////////////////
void ShipDocument::Draw(int baseX, int baseY, bool drawVert)
{
	XpPoint*	tmppt;
	int			c;

	tmppt = (XpPoint *) pts.GetHead();
	fl_color(FL_WHITE);
	c = pts.GetCount();
	if (c >= 2)
	{
		do
		{
			if (tmppt != (XpPoint*)pts.GetTail())
			{
				if (drawVert)//Are we a horizontal or vertical shiplistviewwindow?
					fl_line(rs(-tmppt->y)+baseX,
							rs(-tmppt->x)+baseY,
							rs(-((XpPoint *) tmppt->GetNext())->y)+baseX,
							rs(-((XpPoint *) tmppt->GetNext())->x)+baseY
							);
				else
					fl_line(rs(tmppt->x)+baseX,
							rs(-tmppt->y)+baseY,
							rs(((XpPoint *) tmppt->GetNext())->x)+baseX,
							rs(-((XpPoint *) tmppt->GetNext())->y)+baseY);
				tmppt = (XpPoint *) tmppt->GetNext();
			}
		} while (tmppt->GetNext() != NULL);
		if (drawVert)//Are we a horizontal or vertical shiplistviewwindow?
			fl_line(rs(-tmppt->y)+baseX,
					rs(-tmppt->x)+baseY,
					rs(-((XpPoint *)pts.GetHead())->y)+baseX,
					rs(-((XpPoint *)pts.GetHead())->x)+baseY
					);
		else
			fl_line(rs(tmppt->x)+baseX,
					rs(-tmppt->y)+baseY,
					rs( ((XpPoint *)pts.GetHead())->x)+baseX,
					rs(-((XpPoint *)pts.GetHead())->y)+baseY);
	}
}
