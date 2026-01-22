/*
 * Xpilotedit, the cross platform map editor for Xpilot.  Copyright (C) 2001 by
 *
 *      The Xpilot Authors           <xpilot@xpilot.org>
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
 * $Log: XPPresetConfig.h,v $
 * Revision 1.4  2004/01/05 03:27:38  dick
 * Need to #include "FL/Fl_Window.H" which is a base class.
 *
 */

#ifndef	_XPPRESETCONFIG_H_
#define	_XPPRESETCONFIG_H_

#include "FL/Fl_Window.H"
#include "FL/Fl_Button.H"
#include "XPPreset_Group.h"

class XPPresetConfig : public Fl_Window {
public:
	XPPresetConfig(int x, int y, int w,int h,const char *l=0);
	XPPresetConfig(int w,int h,const char *l=0);
	static	XPPresetConfig*	Factory();
	void UpdatePresets();
public:
//	XPPreset_Group *preset1;
	XPPreset_Group *presets[NUM_MODBANKS];
	Fl_Button *closeBtn;
};
#endif	// _XPPRESETOCONFIG_H_
