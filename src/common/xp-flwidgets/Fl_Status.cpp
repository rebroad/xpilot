/* $Id: Fl_Status.cpp,v 1.4 2004/02/03 03:39:53 dick Exp $
 *
 * Fl_Status - a one line status bar with a backscroll buffer
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
 * $Log: Fl_Status.cpp,v $
 * Revision 1.4  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.3  2004/01/04 06:06:12  dick
 * No newline at end of file
 *
 * Revision 1.2  2002/09/18 14:47:40  jlmiller
 * Group scroll buttons so they don't resize improperly.
 *
 * Revision 1.1  2001/08/26 09:32:54  dick
 * The status line becomes his own widget which has a backscroll buffer.
 *
 */
#include "StdAfx.h"

#include "Fl_Status.h"

#define	SB_WIDTH	20

Fl_Status::Fl_Status(int x, int y, int w, int h, PCSTR label)
: Fl_StatusSUPERCLASS(x, y, w, h, label)
{
	text = new Fl_Output(x, y, w-SB_WIDTH, h);
//	sb = new Fl_Scrollbar(x+(w-SB_WIDTH), y, SB_WIDTH, h);
//	sb->slider_size(0.0);
	Fl_Group *o = new Fl_Group(x+(w-SB_WIDTH), y, SB_WIDTH, h);
	up = new Fl_Repeat_Button(x+(w-SB_WIDTH), y, SB_WIDTH, h/2);
	down = new Fl_Repeat_Button(x+(w-SB_WIDTH), y+(h/2), SB_WIDTH, h/2);
	o->end();
	o->resizable(NULL);
	current = -1;
	last = -1;
	up->callback(&Fl_Status::UpHandler, this);
	down->callback(&Fl_Status::DownHandler, this);
}

///////////////////////////////////////////////////////////////////////////////
void Fl_Status::UpHandler(Fl_Widget*, void* myThis)
{
	Fl_Status* _this = (Fl_Status*)myThis;
	_this->UpHandler();
}

///////////////////////////////////////////////////////////////////////////////
void Fl_Status::UpHandler()
{
	if (current > 0)
	{
		current--;
		text->value(backScroll[current]);
	}
}

///////////////////////////////////////////////////////////////////////////////
void Fl_Status::DownHandler(Fl_Widget*, void* myThis)
{
	Fl_Status* _this = (Fl_Status*)myThis;
	_this->DownHandler();
}

///////////////////////////////////////////////////////////////////////////////
void Fl_Status::DownHandler()
{
	if (current < last)
	{
		current++;
		text->value(backScroll[current]);
	}
}

///////////////////////////////////////////////////////////////////////////////
void Fl_Status::value(PCSTR s)
{
	if (last < BACKSCROLL-1)
	{
		if (current == last)
		{
			backScroll[++last] = s;
			current = last;
			text->value(s);
		}
	}
	else
	{
		for (int i=0; i<BACKSCROLL-1; i++)
			backScroll[i] = backScroll[i+1];
		if (current == last)
		{
			backScroll[last] = s;
			text->value(s);
		}
	}
}
