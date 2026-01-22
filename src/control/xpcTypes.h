/* $Id: xpcTypes.h,v 1.28 2004/06/01 23:13:00 dick Exp $
 *
 * xpcTypes - buttons for XPilot5
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
 * $Log: xpcTypes.h,v $
 * Revision 1.28  2004/06/01 23:13:00  dick
 * You can't really make a new button from an object and stick it in a static.
 * (Who owns it if you open 2 MapEditors?)  So now each window keeps track of
 * his own buttons.
 *
 * Revision 1.27  2004/05/30 15:36:14  dick
 * Use Destructors so that the pngs in the buttons don't leak.
 *
 * Revision 1.26  2004/05/10 12:48:00  dick
 * Add a Private Server button to the main panel, which allows the user to
 * manually track servers that don't appear in meta.
 *
 * Revision 1.25  2004/03/03 07:54:35  dick
 * Don't make pixmaps at load time; wait until runtime init, after Strings have
 * actually been inited.
 *
 * Revision 1.24  2004/02/28 07:40:21  dick
 * Add ToggleButton support for bitmap buttons.  Caller gives a base name and
 * provides baseU.png, baseD.png, and baseX.png bitmaps which are used in the
 * drawing routines.
 *
 * Revision 1.23  2003/12/22 05:59:51  dick
 * Use png(s) instead of xpm(s).
 *
 * Revision 1.22  2002/09/05 16:46:36  dick
 * xpe_tbbutton doesn't keep a pointer to the button anymore.
 *
 * Revision 1.21  2002/08/28 15:29:59  jlmiller
 * make a simple version of the xpe_tbbutton struct, for panel usages.
 * These different button types should probably be consolidated at some point.
 *
 * Revision 1.20  2002/08/20 16:34:24  jlmiller
 * More usable pass at configurable XPWhere window.
 * Still a few bugs to work smash.
 *
 * Revision 1.19  2002/08/02 20:24:48  jlmiller
 * Add teamjoin buttons.
 *
 * Revision 1.18  2002/07/24 18:30:30  jlmiller
 * Remove a few unneeded fields from the toggle button struct.
 *
 * Revision 1.17  2002/07/23 19:58:24  jlmiller
 * No, actually the #include "" was right the first time, so lets put it back.
 *
 * Revision 1.16  2002/07/23 13:24:12  jlmiller
 * #includes should be <> instead of ""
 *
 * Revision 1.15  2002/07/19 07:23:59  dick
 * Add a pane for the Local Server button.
 *
 */
#ifndef	_XPCTYPES_H_
#define	_XPCTYPES_H_

#include "FL/Fl_Repeat_Button.H"
//#include "FL/Fl_Pixmap.H"

#include "cstring.h"
#include "ServerOption.h"

typedef char    map_data_t[MAX_MAP_SIZE+1][MAX_MAP_SIZE+1];
typedef char    max_str_t[255];

class MapWindow;
class Fl_PNG_Image;

///////////////////////////////////////////////////////////////////////////////
class FunctionRepeatButton : public Fl_Repeat_Button {
private:
	Fl_PNG_Image*	pixmap;
	Fl_PNG_Image*	dpixmap;
public:
	FunctionRepeatButton(int x,int y,int w,int h,const char *l=0,
		PCSTR pm=NULL);
	~FunctionRepeatButton();
	void draw();
};

///////////////////////////////////////////////////////////////////////////////
class FunctionToggleButton : public Fl_Button {
public:
	FunctionToggleButton(int x,int y,int w,int h,const char* l=0,
		PCSTR pm=NULL,
		PCSTR _bgUp=NULL, PCSTR _bgDown=NULL, PCSTR _bgDiabled=NULL);
	~FunctionToggleButton();

	virtual void	draw();

	int		id;
	int*	togglevar;
	PCSTR	myLabel;
private:
	Fl_PNG_Image*	pixmap;
	Fl_PNG_Image*	dpixmap;
	Fl_PNG_Image*	bgUp;
	Fl_PNG_Image*	bgDown;
	Fl_PNG_Image*	bgDisabled;
	int				myX;
	int				myY;
	int				myW;
	int				myH;
};

///////////////////////////////////////////////////////////////////////////////
class FunctionButton : public Fl_Button {
private:
	Fl_PNG_Image*	pixmap;
	Fl_PNG_Image*	dpixmap;
public:
	FunctionButton(int x,int y,int w,int h,const char *l=0,
		PCSTR pm=NULL);
	~FunctionButton();
	int id;
	void draw();
};

///////////////////////////////////////////////////////////////////////////////
//XPILOTCONTROLBUTTONTYPES

typedef void (SBGCallback)(int, void*);

//Standard button
struct xpe_tbbutton {
	int			x; //x location
	int			y; //y location
	int			w; //button width
	int			h; //button height
	char*		ttip; //button label
	PCSTR		pm; //pixmap char array
	int id;
};

struct xpe_stbbutton {
	int				x;		// x location
	int				y;		// y location
	char*			lbl;	// String for label
	char*			ttip;	// button label
	PCSTR			pm;		// image for button
	Fl_Callback*	cb;		// buttons callback function
};

///////////////////////////////////////////////////////////////////////////////
//Repeat button
struct xpe_tbrbutton {
	int				x; //x location
	int				y; //y location
//	int				w; //button width
//	int				h; //button height
	char*			ttip; //button label
	PCSTR			pm; //pixmap char array
	Fl_Callback*	cb; //buttons callback function
};

///////////////////////////////////////////////////////////////////////////////
//Toggle button
struct xpe_tbtbutton {
	PCSTR	pm;			// pixmap char array
	int		id;			// the int id
	char*	lbl;		// String for label
	char*	ttip;		// String for tooltip
};

///////////////////////////////////////////////////////////////////////////////
//Toggle button w
struct xpe_tbtblkbutton {
	PCSTR	pm;		// pixmap char array
	PCSTR	bgU;	// button pixmap Up
	PCSTR	bgD;	// button pixmap Down
	PCSTR	bgX;	// button pixmap Disabled
	int		id;		// the int id
	Rect*	rect;
	PCSTR	lbl;	// String for label
	PCSTR	ttip;	// String for tooltip
};


#endif	// _XPCTYPES_H_
