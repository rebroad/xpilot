/* $Id: Fl_KbMultiComm.cpp,v 1.5 2004/02/03 03:39:53 dick Exp $
 *
 * Fl_KbMultiComm - A little window that contains all of the commands for a key
 *
 *      Copyright (C) 2001 by
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
 * $Log: Fl_KbMultiComm.cpp,v $
 * Revision 1.5  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.4  2001/10/04 16:16:11  dick
 * Use more appropriately named SetConfig() instead of Init()
 *
 * Revision 1.3  2001/09/27 17:37:19  dick
 * Init fltk's user data via argument() which we use as an RTTI.
 *
 * Revision 1.2  2001/09/25 15:45:52  dick
 * Split KbConfig to KbConfig and KbConfigKb, since the keyboard is only a part
 * of the whole KbConfig window.
 *
 * Revision 1.1  2001/09/20 07:41:02  dick
 * Move Fl_Kb* to it's own library xp-flwidgets/kbconfig/libxp-kbconfig.a
 *
 * Revision 1.1  2001/09/20 07:21:09  dick
 * If a button has multiple commands, display those in a "flyout" window
 * when the mouse is over the key.
 *
 */
#include "StdAfx.h"

#include "FL/Fl.H"
#include "FL/fl_draw.H"
#include "FL/Fl_Window.H"

#include "Fl_KbCButton.h"
#include "Fl_KbConfig.h"
#include "Fl_KbMultiComm.h"


///////////////////////////////////////////////////////////////////////////////
Fl_KbMultiComm::Fl_KbMultiComm(int X, int Y, int W, int H, const char* L)
: Fl_Group(X, Y, W, H, L)
{
	argument(ButtonTypeMulti);
}

///////////////////////////////////////////////////////////////////////////////
void Fl_KbMultiComm::Init(Fl_KbConfigKb* /*pw*/, Fl_KbCButton* pb)
{
	int		i;
	Obj*	o;
	KbCKeyCommand* kc;

	box(FL_UP_FRAME);
	color(FL_YELLOW);
	begin();
	for (o = pb->GetCommands().GetHead(), i=0; o; o=o->GetNext(), i++)
	{
		kc = (KbCKeyCommand*)o;
		Fl_KbCButton* b = new Fl_KbCButton(x()+i*BUTTON_WIDTH+MULTICOMM_BORDER,
										   y()+MULTICOMM_BORDER, 
										   BUTTON_WIDTH,
										   BUTTON_HEIGHT);
		b->parentButton = pb;
		b->kr = 0;
		b->kw = BUTTON_WIDTH;
		b->kh = BUTTON_HEIGHT;
		b->ktype = ButtonTypeNone;
		b->keyDef = pb->keyDef;
		b->SetConfig(0);
		if (b->keyDef)
			b->kname = b->keyDef->c;
		else
			b->kname = "??";
		DEBUGSELECTION(printf("Add mwchild %d %s\n", i, (PCSTR)b->kname);)
		b->GetCommands().Add(kc);
		
	}
	end();
}

#define	ARCRAD	15
void Fl_KbMultiComm::draw()
{
	int	BW = w() - BUTTON_SPACE -1;
	int BH = h() - BUTTON_SPACE -1;
	fl_color(FL_YELLOW);
//	fl_rectf(x(), y(), w(), h(), FL_YELLOW);
	fl_line_style(0, 2);
	fl_begin_line();
	fl_vertex(x()+ARCRAD, y()+1);								// top left
	fl_vertex(x()+(BW-ARCRAD), y()+1);							// top line
	fl_arc(x()+(BW-ARCRAD), y()+1+ARCRAD, ARCRAD, 90, 0);		// upper right curve
	fl_vertex(x()+BW, y()+BH-ARCRAD);							// right line
	fl_arc(x()+(BW-ARCRAD), y()+BH-ARCRAD, ARCRAD, 0, -90);		// lower right curve
	fl_vertex(x()+ARCRAD, y()+BH);								// bottom line
	fl_arc(x()+1+ARCRAD, y()+(BH-ARCRAD), ARCRAD, -90, -180);	// lower left curve
	fl_vertex(x()+1, y()+1+ARCRAD);								// left line
	fl_arc(x()+1+ARCRAD, y()+1+ARCRAD, ARCRAD, -180, -270);		// upper left curve
	fl_end_line();
	fl_line_style(0);

	for (int i=0; i<children(); i++)
		child(i)->draw();
}
