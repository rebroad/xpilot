/* $Id: Fl_SoundList.cpp,v 1.2 2007/02/17 19:32:35 dick Exp $
 *
 * Fl_SoundList - Display a list of all sounds in the lib/sounds directory
 *
 * common/xp-flwidgets - A library of FLTK widgets for XPilot5.
 *
 *      Copyright (C) 2007 by
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
 * $Log: Fl_SoundList.cpp,v $
 * Revision 1.2  2007/02/17 19:32:35  dick
 * Wrap DEBUG with D()
 *
 * Revision 1.1  2007/02/17 06:13:10  dick
 * A Sound Editor.  Connects events to sounds
 *
 */

#include "StdAfx.h"
#include "FL/fl_draw.H"

#include "cstring.h"
#include "guiconst.h"
#include "config.h"
#include "IniClient.h"
#include "Fl_XPColors.h"
#include "audioDefs.h"
#include "AudioMan.h"
#include "SoundWindow.h"

#include "Fl_SoundList.h"

class NamedSoundEvent {
public:
	int		id;
	PCSTR	name;
};

///////////////////////////////////////////////////////////////////////////////
Fl_SoundList::Fl_SoundList(int x,int y,int w,int h,const char *l)
        : Fl_SoundListSUPERCLASS(x,y,w,h,l)
{
	rows(audioMan.GetSoundList()->GetCount());
	D(printf("FSL: Init %d rows\n", audioMan.GetSoundList()->GetCount());)
	callback(&Fl_SoundList::Callback, this);
	//callback_when(FLVEcb_CLICKED);
	playSoundPrimed = false;

	oldScrollbarCallback = scrollbar.callback();
	scrollbar.callback(myScrollbarCallback, this);
}

///////////////////////////////////////////////////////////////////////////////
Fl_SoundList::~Fl_SoundList()
{
}

///////////////////////////////////////////////////////////////////////////////
// virtual
int Fl_SoundList::handle(int event)
{
	//D(printf("SL:handle(%d)\n", event);)
	switch (event) {
	case FL_PUSH:
	case FL_KEYDOWN:
	case FL_DRAG:
		playSoundPrimed = true;
		Fl_SoundListSUPERCLASS::handle(event);
		return(1);
	}
	int ret = Fl_SoundListSUPERCLASS::handle(event);
	//printf("SL:endhandle(%d)\n", event);
	return(ret);
}

///////////////////////////////////////////////////////////////////////////////
//virtual
void Fl_SoundList::draw_row(int Offset, int &X, int &Y, int &W, int &H, int R ) {
	Fl_SoundListSUPERCLASS::draw_row(Offset, X, Y, W, H, R);

	Flv_Style s;
	get_style(s, R);       // Get trickled down row style
	fl_color(FL_BLACK);
	fl_draw(audioMan.GetSoundList()->GetAt(R)->name, X+1-Offset, Y+1, W-2, H-2, s.align());
}

///////////////////////////////////////////////////////////////////////////////
// static
void Fl_SoundList::Callback(Fl_Widget*, void* v) {
	Fl_SoundList* sl = (Fl_SoundList*)v;
	sl->Callback();
}

///////////////////////////////////////////////////////////////////////////////
void Fl_SoundList::Callback() {
	printf("SL:Callback\n");
	if (playSoundPrimed) {
		playSoundPrimed = false;
		D(printf("SL:Callback row=%d when=%d event=%d\n", row(), when(), why_event());)
		soundWindow->DisplaySelectedSound();
		audioMan.PlaySound(row(), 100);
	}
}

///////////////////////////////////////////////////////////////////////////////
//static
void Fl_SoundList::myScrollbarCallback(Fl_Widget* w, void* v) {
	Fl_SoundList* sl = (Fl_SoundList*)v;
	sl->oldScrollbarCallback(w, v);
	sl->myScrollbarCallback();
}

///////////////////////////////////////////////////////////////////////////////
void Fl_SoundList::myScrollbarCallback() {
	D(printf("Scroll callback\n");)
	soundWindow->ScrollUpdate();
}

