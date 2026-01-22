/* $Id: Fl_Value_Input_Counter.h,v 1.2 2002/09/10 19:05:15 jlmiller Exp $
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
 * $Log: Fl_Value_Input_Counter.h,v $
 * Revision 1.2  2002/09/10 19:05:15  jlmiller
 * Tweak the way a few things work, shuffle a few constants around.
 *
 * Revision 1.1  2002/09/06 19:17:52  jlmiller
 * A nice buttoned valulator, for number values, similar to old client style, but in FLTK.
 *
 */

#ifndef	_FL_VALUE_INPUT_COUNTER_H_
#define	_FL_VALUE_INPUT_COUNTER_H_

#include "FL/Fl_Group.H"
#include "FL/Fl_Value_Input.H"
#include "FL/Fl_Repeat_Button.H"

#include "const.h"
#include "cstring.h"

enum Value_Types {
	integer,
	floating,
};

#define	Fl_Value_Input_CounterSUPERCLASS Fl_Group
class Fl_Value_Input_Counter : public Fl_Value_Input_CounterSUPERCLASS
{
public:
	Fl_Value_Input_Counter(int x, int y, int w, int h, PCSTR label = 0);

	static	void UpHandler(Fl_Widget*, void* myThis);
			void UpHandler();
	static	void DownHandler(Fl_Widget*, void* myThis);
			void DownHandler();
	static	void TextHandler(Fl_Widget*, void* myThis);
			void TextHandler();
	void bounds(double a, double b);
	void step(int n);
	void Read();
	void Store();
	void SetInt(int *ip);
	void SetFlt(DFLOAT *fltp);

private:
	DFLOAT Fl_Value_Input_Counter::CalcOffset(DFLOAT, DFLOAT, DFLOAT, DFLOAT, DFLOAT *);
	Value_Types type;
	int *intVal;
	DFLOAT *fltVal;


	Fl_Value_Input*		text;
	Fl_Repeat_Button*	up;
	Fl_Repeat_Button*	down;
};


#endif	// _FL_VALUE_INPUT_COUNTER_H_
