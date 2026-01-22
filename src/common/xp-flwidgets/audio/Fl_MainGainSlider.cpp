/* $Id: Fl_MainGainSlider.cpp,v 1.1 2007/02/17 06:13:10 dick Exp $
 *
 * XPInput - Provide a text input window that associates with a String
 *
 * XPilot5 - Manage all local XPilot activity.
 *
 *      Copyright (C) 2001 by
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
 * $Log: Fl_MainGainSlider.cpp,v $
 * Revision 1.1  2007/02/17 06:13:10  dick
 * A Sound Editor.  Connects events to sounds
 *
 */

// Provide an Input Window that associates with a string.
// Read and Write with this string as needed.

#include "StdAfx.h"
#include <stdio.h>

#include "portability.h"
//#include "../proto.h"
#include "SoundWindow.h"

#include "Fl_MainGainSlider.h"

///////////////////////////////////////////////////////////////////////////////
Fl_MainGainSlider::Fl_MainGainSlider(int x,int y,int w,int h,const char *l)
: Fl_Value_Slider(x,y,w,h,l) {
//	origFont = textfont();
	soundWindow = NULL;
	fontSize = textsize();
}

///////////////////////////////////////////////////////////////////////////////
Fl_MainGainSlider::~Fl_MainGainSlider() {
}

///////////////////////////////////////////////////////////////////////////////
int Fl_MainGainSlider::handle(int event) {
	int ret = Fl_MainGainSliderSUPERCLASS::handle(event);
	printf("MGS:handle(%d)\n", event);
	switch (event) {
	case FL_RELEASE:
		if (soundWindow)
			soundWindow->GainChanged((int)value());
	}
	return(ret);
}

///////////////////////////////////////////////////////////////////////////////
// static
//void Fl_MainGainSlider::CallbackFl(Fl_MainGainSlider* w, void*) {
//	printf("Callback\n");
//	//w->Store();
//}

///////////////////////////////////////////////////////////////////////////////
//virtual
int Fl_MainGainSlider::format(char* s) {
	int	v = (int)value();
	if (v == 100) {
		v = 11;
		textfont(FL_HELVETICA_BOLD);
		textsize(fontSize+1);
	} else {
		textfont(FL_HELVETICA);
		textsize(fontSize);
	}
	return(sprintf(s, "%d", v));
}
