/* $Id: Fl_KbCButton.cpp,v 1.8 2004/02/28 07:31:42 dick Exp $
 *
 * Fl_KbCButton - Handle a single button/key on the Fl_KbConfig keyboard
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
 * $Log: Fl_KbCButton.cpp,v $
 * Revision 1.8  2004/02/28 07:31:42  dick
 * Move colors to Fl_XPColors
 *
 * Revision 1.7  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.6  2002/07/19 17:09:59  dick
 * Don't walk through widgets that aren't our keys by setting user_data to non-null.
 *
 * Revision 1.5  2001/10/04 16:29:37  dick
 * Better destruction of multiwindow (handles Unused having a different parent)
 *
 * Revision 1.4  2001/10/01 13:51:21  dick
 * Don't draw button texts if there is a multiwindow over us.
 *
 * Revision 1.3  2001/09/27 17:46:43  dick
 * Use new RTTI type (fltk userdata).  Move ButtonType to Fl_KbConfig.h
 * Stregthen multiwindow handling.
 *
 * Revision 1.2  2001/09/25 15:46:48  dick
 * Split KbConfig to KbConfig and KbConfigKb, since the keyboard is only a part
 * of the whole KbConfig window.
 *
 * Revision 1.1  2001/09/20 07:41:02  dick
 * Move Fl_Kb* to it's own library xp-flwidgets/kbconfig/libxp-kbconfig.a
 *
 * Revision 1.2  2001/09/20 07:21:09  dick
 * If a button has multiple commands, display those in a "flyout" window
 * when the mouse is over the key.
 *
 * Revision 1.1  2001/09/17 08:56:28  dick
 * Handle a single button/key on the Fl_KbConfig keyboard.
 *
 */
#include "StdAfx.h"

#include "FL/Fl.H"
#include "FL/fl_draw.H"
#include "FL/Fl_Window.H"

#include "Fl_XPColors.h"
#include "Fl_KbCButton.h"
#include "Fl_KbConfigKb.h"
#include "Fl_KbMultiComm.h"
#include "Fl_KbConfig.h"


///////////////////////////////////////////////////////////////////////////////
Fl_KbCButton::Fl_KbCButton(int X, int Y, int W, int H, PCSTR keyCap)
: Fl_Widget(X, Y, W, H, keyCap)
{
	argument(ButtonTypeNone);
	selected = false;
	multiWindow = 0;
	parentButton = 0;
	user_data((void*)1);
}

Fl_KbCButton::~Fl_KbCButton()
{
	// One would think that i own these, but apparently in fltk, not.
	// So remove (and i would think, leak) all members before letting 
	// fltk delete them.
	while (commands.RemoveHead());
//	commands.Empty();
	// commands should just delete itself (and it's members) on destruction
	// but my Purify analysis sez that this destructor gets called twice
	// during the group's destruction.
	// It appears the choices are: leak some of the time, or crash all of the time.
	// *argh*
}

///////////////////////////////////////////////////////////////////////////////
#define	ARCRAD	12
#define	FONT_LABEL_HEIGHT	10
void Fl_KbCButton::draw()
{
	if (ktype == ButtonTypeBlank)
		return;
	int	BW = w() - BUTTON_SPACE -1;
	int BH = h() - BUTTON_SPACE -1;

	fl_push_matrix();
	fl_scale(1,1);
	fl_rotate(0);
	fl_translate(0, 0);

	fl_color(FL_BLACK);
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

		// draw a key's background
	fl_color(xpblue);
	fl_begin_polygon();
	fl_vertex(x()+ARCRAD, y());
	fl_vertex(x()+(BW-ARCRAD), y());
	fl_arc(x()+(BW-ARCRAD), y()+ARCRAD, ARCRAD, 90, 0);
	fl_vertex(x()+BW, y()+ARCRAD);
	fl_vertex(x()+BW, y()+BH-ARCRAD);
	fl_arc(x()+(BW-ARCRAD), y()+BH-ARCRAD, ARCRAD, 0, -90);
	fl_vertex(x()+(BW-ARCRAD), y()+BH);
	fl_vertex(x()+ARCRAD, y()+BH);
	fl_arc(x()+ARCRAD, y()+(BH-ARCRAD), ARCRAD, -90, -180);
	fl_vertex(x(), y()+(BH-ARCRAD));
	fl_vertex(x(), y()+ARCRAD);
	fl_arc(x()+ARCRAD, y()+ARCRAD, ARCRAD, -180, -270);
	fl_end_polygon();

	if (!multiWindow)
	{
		if (selected)
		{
			fl_color(xpred);
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
		}
		// draw the key label
		fl_color(FL_BLACK);
		if (!keyDef)
			fl_color(FL_RED);
		fl_font(FL_HELVETICA, FONT_LABEL_HEIGHT);
		fl_draw(kname, x()+ARCRAD-3, y()+BH-2);

		KbCKeyCommand* ckc = (KbCKeyCommand*)commands.GetHead();
		if (ckc)
		{
			char	line0[40];
			char	line1[40];
			char	line2[40];
			fl_color(FL_WHITE);
			KeyCommand* kc = &keyCommands[ckc->key->key];
			int i;
			int	j;
			line0[0] = line1[0] = line2[0] = '\0';
			for (i=0; i<40; i++)
			{
				line0[i] = kc->name[i];
				if (line0[i] == '\n')
				{
					line0[i] = '\0';
					break;
				}
				if (kc->name[i] == '\0')
					break;
			}
			j = i;
			if (kc->name[j])
			{
				if (kc->name[j] == '\n')
					j++;
				for (i=0; i<40; i++, j++)
				{
					line1[i] = kc->name[j];
					if (line1[i] == '\n')
					{
						line1[i] = '\0';
						break;
					}
					if (kc->name[j] == '\0')
						break;
				}
			}
			if (kc->name[j])
			{
				if (kc->name[j] == '\n')
					j++;
				for (i=0; i<40; i++, j++)
				{
					line2[i] = kc->name[j];
					if (line2[i] == '\n')
					{
						line2[i] = '\0';
						break;
					}
					if (kc->name[j] == '\0')
						break;
				}
			}
			if (kc->bits)	// draw a command that has a pic
			{
				int	_w;
				int	_h;
				fl_measure(line0, _w, _h);
				fl_draw(line0, x()+w()/2-_w/2, y()+h()/2-8-4);
				fl_measure(line1, _w, _h);
				#define	FUDGE	4
				fl_draw(line1, x()+w()/2-_w/2, y()+h()/2+8+4+FUDGE);
				fl_draw_image(kc->bits, x()+w()/2-8, y()+h()/2-8, 16, 16);
			}
			else
			{
				int	_w;
				int	_h;
				int lineCount = 1;
				if (line1[0])
					lineCount++;
				if (line2[0])
					lineCount++;
				if (lineCount == 1)
				{
					_w = 0;
					fl_measure(line0, _w, _h);
					fl_draw(line0, x()+w()/2-_w/2, y()+h()/2);
				}
				else if (lineCount == 2)
				{
					_w = 0;
					fl_measure(line0, _w, _h);
					fl_draw(line0, x()+w()/2-_w/2, y()+h()/2-_h/2);
					_w = 0;
					fl_measure(line1, _w, _h);
					fl_draw(line1, x()+w()/2-_w/2, y()+h()/2+_h/2);
				}
				else
				{
					_w = 0;
					fl_measure(line0, _w, _h);
					fl_draw(line0, x()+w()/2-_w/2, y()+h()/2-_h+1);
					_w = 0;
					fl_measure(line1, _w, _h);
					fl_draw(line1, x()+w()/2-_w/2, y()+h()/2);
					_w = 0;
					fl_measure(line2, _w, _h);
					fl_draw(line2, x()+w()/2-_w/2, y()+h()/2+_h-1);
				}
			}
		}
	}

	fl_pop_matrix();
}

///////////////////////////////////////////////////////////////////////////////
int	Fl_KbCButton::handle(int event)
{
//	printf("button handle %d\n", event);
	switch (event)
	{
	case FL_ENTER:
	case FL_LEAVE:
		return(1);
	}
	return(Fl_Widget::handle(event));
}

void Fl_KbCButton::Select()
{
	selected = true;
	DEBUGSELECTION(printf("Button %s has %d commands\n", (PCSTR)kname, commands.GetCount());)
	if (config && !multiWindow && commands.GetCount() > 1)
	{
		DEBUGSELECTION(printf("create multiWindow %d/%d  %d/%d\n",
							x()-MULTICOMM_BORDER, 
							y()-MULTICOMM_BORDER, 
							commands.GetCount() * BUTTON_WIDTH+MULTICOMM_BORDER*2,
							BUTTON_HEIGHT+MULTICOMM_BORDER*2);)
		multiWindow = new Fl_KbMultiComm(
							x()-MULTICOMM_BORDER, 
							y()-MULTICOMM_BORDER, 
							commands.GetCount() * BUTTON_WIDTH+MULTICOMM_BORDER*2,
							BUTTON_HEIGHT+MULTICOMM_BORDER*2);
		
		//config->add(*multiWindow);
		((Fl_Group*)parent())->add(*multiWindow);
		multiWindow->Init(config, this);
		redraw();
		multiWindow->redraw();
	}
	else
		redraw();

}

void Fl_KbCButton::UnSelect()
{
	selected = false;
	if (multiWindow)
	{
		DEBUGSELECTION(printf("delete multiWindow %p\n", multiWindow);)
		Fl_KbMultiComm*	m = multiWindow;
		multiWindow = 0;
		m->hide();
		((Fl_Group*)parent())->remove(m);
		delete m;
		config->GetCommands(GetCommands(), this);
	}
	redraw();
}

