/* $Id: Fl_Value_Input_Counter.cpp,v 1.3 2004/02/03 03:39:53 dick Exp $
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
 * $Log: Fl_Value_Input_Counter.cpp,v $
 * Revision 1.3  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.2  2002/09/10 19:05:15  jlmiller
 * Tweak the way a few things work, shuffle a few constants around.
 *
 * Revision 1.1  2002/09/06 19:17:52  jlmiller
 * A nice buttoned valulator, for number values, similar to old client style, but in FLTK.
 *
 */
#include "StdAfx.h"

#include "Fl_Value_Input_Counter.h"

#define	SB_WIDTH	20

Fl_Value_Input_Counter::Fl_Value_Input_Counter(int x, int y, int w, int h, PCSTR label)
: Fl_Value_Input_CounterSUPERCLASS(x, y, w, h, label)
{
	text = new Fl_Value_Input(x, y, w-SB_WIDTH, h);
	text->align(FL_ALIGN_LEFT);
	text->callback(&Fl_Value_Input_Counter::TextHandler, this);

	up = new Fl_Repeat_Button(x+(w-SB_WIDTH), y, SB_WIDTH, h/2);
	down = new Fl_Repeat_Button(x+(w-SB_WIDTH), y+(h/2), SB_WIDTH, h/2);

	up->callback(&Fl_Value_Input_Counter::UpHandler, this);
	down->callback(&Fl_Value_Input_Counter::DownHandler, this);
}

///////////////////////////////////////////////////////////////////////////////
void Fl_Value_Input_Counter::UpHandler(Fl_Widget*, void* myThis)
{
	Fl_Value_Input_Counter* _this = (Fl_Value_Input_Counter*)myThis;
	_this->UpHandler();
}
///////////////////////////////////////////////////////////////////////////////
DFLOAT Fl_Value_Input_Counter::CalcOffset(DFLOAT fval, DFLOAT min, DFLOAT max, DFLOAT delta, DFLOAT *fmin)
{
	DFLOAT offset;

	if (fval >= 0) {
		if (min < 0) {
			*fmin = 0;
		} else {
			*fmin = min;
		}
		offset = fval - *fmin;
	} else {
		if (max > 0) {
			*fmin = 0;
		} else {
			*fmin = max;
		}
		offset = -fval + *fmin;
	}
	return offset;

}

///////////////////////////////////////////////////////////////////////////////
void Fl_Value_Input_Counter::UpHandler()
{
	int ival;
    DFLOAT			fval,
		delta,
		fmin,
		offset,
		newoffset,
		max, min;

	fval = text->value();
	ival = (int) text->value();
	max = text->maximum();
	min = text->minimum();
	delta = max - min;

	offset = CalcOffset(fval, min, max, delta, &fmin);

	switch (type) {
	case integer:
		ival = (int)(ival * 1.05 + 0.5);
		if (ival == ival) {
			ival++;
		}
		newoffset = ival;
		break;
	case floating:
		newoffset = (float)(offset * 1.05);
		if (newoffset - offset < delta / 100.0) {
			newoffset = offset + delta / 100.0;
		}
		break;
	}


	if (fval >= 0) {
		fval = fmin + newoffset;
	} else {
		fval = fmin - newoffset;
	}

	LIMIT(fval, min, max);
	if (fval != text->value())
		text->value(fval);


	//	text->value(text->clamp(text->value()+text->step()));
	TextHandler();
}

///////////////////////////////////////////////////////////////////////////////
void Fl_Value_Input_Counter::DownHandler(Fl_Widget*, void* myThis)
{
	Fl_Value_Input_Counter* _this = (Fl_Value_Input_Counter*)myThis;
	_this->DownHandler();
}

///////////////////////////////////////////////////////////////////////////////
void Fl_Value_Input_Counter::DownHandler()
{
	int ival;
    DFLOAT			fval,
		delta,
		fmin,
		offset,
		newoffset,
		max, min;

	fval = text->value();
	ival = (int) text->value();
	max = text->maximum();
	min = text->minimum();
	delta = max - min;

	offset = CalcOffset(fval, min, max, delta, &fmin);


	switch (type) {
	case integer:
		ival = (int)(ival * 0.95);
		if (ival == ival) {
			ival--;
		}
		newoffset = ival;
		break;
	case floating:
		newoffset = (float)(offset * 0.95);
		if (newoffset - offset > -delta / 100.0) {
			newoffset = offset - delta / 100.0;
		}
		if (newoffset < 0 && offset > 0) {
			newoffset = 0;
		}
		break;
	}



	if (fval >= 0) {
		fval = fmin + newoffset;
	} else {
		fval = fmin - newoffset;
	}

	LIMIT(fval, min, max);
	if (fval != text->value())
		text->value(fval);


	//	text->value(text->clamp(text->value()-text->step()));
	TextHandler();
}
///////////////////////////////////////////////////////////////////////////////
void Fl_Value_Input_Counter::bounds(double a, double b)
{
	text->bounds(a, b);
}
///////////////////////////////////////////////////////////////////////////////
void Fl_Value_Input_Counter::step(int n)
{
	text->step(n);
}
///////////////////////////////////////////////////////////////////////////////
void Fl_Value_Input_Counter::SetInt(int *ip)
{
	intVal = ip;
	type = integer;
}
///////////////////////////////////////////////////////////////////////////////
void Fl_Value_Input_Counter::SetFlt(DFLOAT *fltp)
{
	fltVal = fltp;
	type = floating;
}
///////////////////////////////////////////////////////////////////////////////
void Fl_Value_Input_Counter::Read()
{
	switch(type) {
	case integer: text->value(*intVal);
		break;
	case floating: text->value(*fltVal);
		break;
	}
}
///////////////////////////////////////////////////////////////////////////////
void Fl_Value_Input_Counter::Store()
{
	switch(type) {
	case integer: *intVal = (int) text->value();
		break;
	case floating: *fltVal = (DFLOAT) text->value();
		break;
	}
}
///////////////////////////////////////////////////////////////////////////////
void Fl_Value_Input_Counter::TextHandler(Fl_Widget*, void* myThis)
{
	Fl_Value_Input_Counter* _this = (Fl_Value_Input_Counter*)myThis;
	_this->TextHandler();
}
///////////////////////////////////////////////////////////////////////////////
void  Fl_Value_Input_Counter::TextHandler() {
    Store();
}

