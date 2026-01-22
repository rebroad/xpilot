/* $Id: Fl_MainGainSlider.h,v 1.1 2007/02/17 06:13:10 dick Exp $
 *
 * Fl_MainGainSlider - Display a slider for the primary volume control
 *
 * XPilot5 - Manage all local XPilot activity.
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
 * $Log: Fl_MainGainSlider.h,v $
 * Revision 1.1  2007/02/17 06:13:10  dick
 * A Sound Editor.  Connects events to sounds
 *
 */

#ifndef	_FLMAINGAINSLIDER_H_
#define	_FLMAINGAINSLIDER_H_

#undef 	index
#include "FL/Fl_Value_Slider.H"

class	SoundWindow;

#define	Fl_MainGainSliderSUPERCLASS	Fl_Value_Slider
class Fl_MainGainSlider : public Fl_MainGainSliderSUPERCLASS {
public:
	Fl_MainGainSlider(int x,int y,int w,int h,const char *l=0);
	~Fl_MainGainSlider();

	void	SetSoundWindow(SoundWindow* sw);

    virtual int handle(int event);
	virtual int format(char*);

private:
	SoundWindow*	soundWindow;
	uchar			fontSize;
};

inline void Fl_MainGainSlider::SetSoundWindow(SoundWindow* sw) { soundWindow = sw; }

#endif	// _FLMAINGAINSLIDER_H_
