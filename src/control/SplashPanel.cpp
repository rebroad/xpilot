/*
 * XPilotControl, .  Copyright (C) 2002 by
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
 * $Log: SplashPanel.cpp,v $
 * Revision 1.10  2004/03/04 05:38:17  dick
 * Free up the splash panel's memory after it's no longer displayed.
 *
 * Revision 1.9  2004/02/29 03:07:33  dick
 * Use green over red for the version string to match the graphic
 *
 * Revision 1.8  2004/02/28 07:42:22  dick
 * Use Fl_XPColors.h file for colors
 *
 * Revision 1.7  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.6  2004/01/20 06:13:53  dick
 * Add support for `make install` to a non-priv'd directory, like /home/dick/xpilot.
 * If we are doing a default installation (/usr/local/xpilot) then the VARDIR
 * becomes /var/xpilot .  If we are not default then use $prefix/var.
 *
 * Revision 1.5  2004/01/09 19:16:05  dick
 * Display the version number in the splash panel
 *
 * Revision 1.4  2003/12/22 05:59:51  dick
 * Use png(s) instead of xpm(s).
 *
 * Revision 1.3  2002/09/18 14:49:22  jlmiller
 * Changes necessary for Main window to be resizable, and tiled.
 *
 * Revision 1.2  2002/07/28 23:16:15  dick
 * Erase the panel to black before drawing the bitmap (why is bitmap drawing so slow?)
 *
 * Revision 1.1  2002/07/28 20:18:50  dick
 * Display a pretty graphic in the panel at startup
 *
 */
#include "StdAfx.h"

#include "FL/Fl_Group.H"
#include "FL/Fl_Box.H"
#include "FL/Fl_PNG_Image.H"
#include "FL/fl_draw.H"
#include "FL/Fl_Window.H"
#include "cstring.h"

#include "Fl_XPColors.h"

#include "proto.h"
#include "controlconst.h"
#include "SplashPanel.h"
#include "version.h"

#define	VERSION_Y	70

///////////////////////////////////////////////////////////////////////////////
SplashPanel::SplashPanel(int x, int y, int w,int h,const char *l)
        : SplashPanelSUPERCLASS(x,y,w,h,l)
{
	myX = x;
	myY = y;
	myW = w;
	myH = h;

	begin();
	String	fname = GetButtonFile("splash");
	Fl_Box* bigbox = new Fl_Box(FL_DOWN_BOX, x, y,
								   w, h, "");
	Fl_Group *o = new Fl_Group(x, y, w, h, "");

	splash = new Fl_PNG_Image(fname);
	o->end();
	o->resizable(NULL);

	end();
}

///////////////////////////////////////////////////////////////////////////////
SplashPanel::~SplashPanel()
{
	if (splash)
		delete splash;
	splash = NULL;
}

///////////////////////////////////////////////////////////////////////////////
void SplashPanel::draw()
{
	fl_clip(myX, myY, myW, myH);
	fl_color(FL_BLACK);
	myX = x();
	myY = y();
	fl_rectf(myX, myY, myW, myH);
	splash->draw(myX, myY);
	fl_font(FL_TIMES_BOLD_ITALIC, 28);
	fl_color(xpred);
	fl_draw(TITLE, myX+1, myY+VERSION_Y+1, myW, 30, FL_ALIGN_CENTER);
	fl_color(xpgreen);
	fl_draw(TITLE, myX, myY+VERSION_Y, myW, 30, FL_ALIGN_CENTER);
	fl_pop_clip();
}
