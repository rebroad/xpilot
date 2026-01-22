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
 * $Log: SplashPanel.h,v $
 * Revision 1.4  2004/03/04 05:38:17  dick
 * Free up the splash panel's memory after it's no longer displayed.
 *
 * Revision 1.3  2003/12/22 05:59:51  dick
 * Use png(s) instead of xpm(s).
 *
 * Revision 1.2  2002/07/28 23:16:15  dick
 * Erase the panel to black before drawing the bitmap (why is bitmap drawing so slow?)
 *
 * Revision 1.1  2002/07/28 20:18:50  dick
 * Display a pretty graphic in the panel at startup
 *
 */

#ifndef	_SPLASHPANEL_H_
#define	_SPLASHPANEL_H_

#include "FL/Fl_Group.H"

class Fl_PNG_Image;

#define	SplashPanelSUPERCLASS	Fl_Group
class SplashPanel : public SplashPanelSUPERCLASS {
public:
	SplashPanel(int x, int y, int w,int h,const char *l=0);
	~SplashPanel();

	virtual void draw();

private:
	Fl_PNG_Image* splash;

	int		myX;
	int		myY;
	int		myW;
	int		myH;
};

#endif	// _SPLASHPANEL_H_
