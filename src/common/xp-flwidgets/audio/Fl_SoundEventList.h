/* $Id: Fl_SoundEventList.h,v 1.1 2007/02/17 06:13:10 dick Exp $
 *
 * SoundEventList - A list of the available sound events
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
 * $Log: Fl_SoundEventList.h,v $
 * Revision 1.1  2007/02/17 06:13:10  dick
 * A Sound Editor.  Connects events to sounds
 *
 * Revision 1.1  2004/05/28 17:33:09  dick
 * A button that displays the player's selected ship and his nickname.
 *
 */

#ifndef	_FLSOUNDEVENTLIST_H_
#define	_FLSOUNDEVENTLIST_H_

#include "FL/Flv_List.H"

class SoundWindow;

#define	Fl_SoundEventListSUPERCLASS Flv_List
class Fl_SoundEventList : public Fl_SoundEventListSUPERCLASS {
public:
	Fl_SoundEventList(int x,int y,int w,int h,const char *l=0);
	~Fl_SoundEventList();

	void	SetSoundWindow(SoundWindow* sw);

	virtual void	draw_row( int Offset, int &X, int &Y, int &W, int &H, int R );
	virtual int		handle(int);

	static	void	Callback(Fl_Widget* w, void* v);
			void	Callback();

	static	void	myScrollbarCallback(Fl_Widget* w, void* v);
			void	myScrollbarCallback();

private:
	SoundWindow*	soundWindow;
	bool			playSoundPrimed;
	Fl_Callback*	oldScrollbarCallback;
};

inline void Fl_SoundEventList::SetSoundWindow(SoundWindow* sw) { soundWindow = sw; }

#endif	// _FLSOUNDEVENTLIST_H_
