/*
* XPilotedit, the cross platform map editor for XPilot.  Copyright (C) 2001 by
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
 * $Log: gui_toolkit.cpp,v $
 * Revision 1.28  2004/05/30 15:36:14  dick
 * Use Destructors so that the pngs in the buttons don't leak.
 *
 * Revision 1.27  2004/03/03 07:54:35  dick
 * Don't make pixmaps at load time; wait until runtime init, after Strings have
 * actually been inited.
 *
 * Revision 1.26  2004/02/28 07:40:21  dick
 * Add ToggleButton support for bitmap buttons.  Caller gives a base name and
 * provides baseU.png, baseD.png, and baseX.png bitmaps which are used in the
 * drawing routines.
 *
 * Revision 1.25  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.24  2003/12/22 05:59:51  dick
 * Use png(s) instead of xpm(s).
 *
 * Revision 1.23  2002/08/20 10:47:29  jlmiller
 * File & Class Renamings:
 * ViewWindow -> MapEditWindow
 * XpInput -> XPInput
 * xpeTypes -> xpcTypes
 *
 * Revision 1.22  2002/07/30 17:21:32  jlmiller
 * Overlay functions become member functions of the drawing windows.
 *
 * Revision 1.21  2002/07/27 03:17:40  jlmiller
 * Fix very stupid typo.
 *
 * Revision 1.20  2002/07/27 02:56:05  jlmiller
 * Check for image data in a different way.
 *
 * Revision 1.19  2002/07/25 14:23:21  jlmiller
 * Don't try to draw the button pixmap if it doesn't exist.
 *
 * Revision 1.18  2002/07/24 18:30:09  jlmiller
 * Remove a few unneeded fields from the toggle button struct.
 *
 * Revision 1.17  2002/07/23 19:58:24  jlmiller
 * No, actually the #include "" was right the first time, so lets put it back.
 *
 * Revision 1.16  2002/07/23 13:24:12  jlmiller
 * #includes should be <> instead of ""
 *
 * Revision 1.15  2002/07/19 14:04:49  jlmiller
 * Center the pixmap, even if the button is oblong.
 *
 * Revision 1.14  2002/07/12 15:00:06  jlmiller
 * Integrate tabbed main window, with clean up of globals.
 *
 * Revision 1.13  2002/06/25 20:20:09  jlmiller
 * Massive restructure to clean up includes, and objectify code more.
 *
 * Revision 1.12  2002/06/14 20:15:07  jlmiller
 * First pass at properly showing disabled buttons.
 *
 * Revision 1.11  2002/05/28 07:19:51  dick
 * Rename xpilotedit to XPilotControl
 *
 */
#include "StdAfx.h"

#include "FL/fl_draw.H"
#include "FL/Fl_PNG_Image.H"

#include "Fl_XPColors.h"

#include "cstring.h"
#include "proto.h"
#include "xpcTypes.h"

///////////////////////////////////////////////////////////////////////////////
void togglebar_cb(Fl_Widget *w, void*)
{
	FunctionToggleButton *btn = (FunctionToggleButton *)w;
	*btn->togglevar = btn->id;
}

///////////////////////////////////////////////////////////////////////////////
FunctionRepeatButton::FunctionRepeatButton(int x,int y,int w,int h, PCSTR l,
										   PCSTR pm)
        : Fl_Repeat_Button(x,y,w,h,l)
{
	if (pm)
		pixmap = new Fl_PNG_Image(GetButtonFile(pm));
	dpixmap = NULL;
}

///////////////////////////////////////////////////////////////////////////////
FunctionRepeatButton::~FunctionRepeatButton()
{
	if (pixmap)
		delete pixmap;
	if (dpixmap)
		delete dpixmap;
}

///////////////////////////////////////////////////////////////////////////////
void FunctionRepeatButton::draw()
{
	//Be sure to do the normal drawing stuff, so that the button looks right.
	Fl_Repeat_Button::draw();

	//Draw the properly (dis)enabled pixmap.
	if (active())
	{
		//Draw the picture in the center of the button.
		//even if the button dimensions aren't equal.
		if (pixmap->count())
			pixmap->draw(x()+w()/2-pixmap->w()/2, y()+h()/2-pixmap->h()/2);
	}
	else
	{
		if(!dpixmap)
		{
			if (pixmap->count())
			{
				dpixmap = (Fl_PNG_Image *) pixmap->copy();
				dpixmap->inactive();
			}
		}
		if (dpixmap)
			dpixmap->draw(x()+w()/2-pixmap->w()/2, y()+h()/2-pixmap->h()/2);
	}
}

///////////////////////////////////////////////////////////////////////////////
FunctionButton::FunctionButton(int x,int y,int w,int h, PCSTR l, PCSTR pm)
        : Fl_Button(x,y,w,h,l)
{
	pixmap = new Fl_PNG_Image(GetButtonFile(pm)),
	dpixmap = NULL;
}

///////////////////////////////////////////////////////////////////////////////
FunctionButton::~FunctionButton()
{
	if (pixmap)
		delete pixmap;
	if (dpixmap)
		delete dpixmap;
}

///////////////////////////////////////////////////////////////////////////////
void FunctionButton::draw()
{
	//Be sure to do the normal drawing stuff, so that the button looks right.
	Fl_Button::draw();

	//Draw the properly (dis)enabled pixmap.
	if (active())
	{
		//Draw the picture in the center of the button.
		//even if the button dimensions aren't equal.
		if (pixmap->count())
			pixmap->draw(x()+w()/2-pixmap->w()/2, y()+h()/2-pixmap->h()/2);
	}
	else
	{
		if(!dpixmap)
		{
			if (pixmap->count())
			{
				dpixmap = (Fl_PNG_Image*)pixmap->copy();
				dpixmap->inactive();
			}
		}
		if (dpixmap)
		dpixmap->draw(x()+w()/2-pixmap->w()/2, y()+h()/2-pixmap->h()/2);
	}
}

///////////////////////////////////////////////////////////////////////////////
FunctionToggleButton::FunctionToggleButton(int x,int y,int w,int h,const char *l,
										   PCSTR pm,
										   PCSTR _bgUp, PCSTR _bgDown, PCSTR _bgX)
	: Fl_Button(x,y,w,h,l)
{
	pixmap = NULL;
	dpixmap = NULL;
	bgUp = NULL;
	bgDown = NULL;
	bgDisabled = NULL;
	if (pm)
		pixmap = new Fl_PNG_Image(GetButtonFile(pm));
	if (_bgUp)
		bgUp = new Fl_PNG_Image(GetButtonFile(_bgUp));
	if (_bgDown)
		bgDown = new Fl_PNG_Image(GetButtonFile(_bgDown));
	if (_bgX)
		bgDisabled = new Fl_PNG_Image(GetButtonFile(_bgX));

	myX = x;
	myY = y;
	myW = w;
	myH = h;
	myLabel = l;
}

///////////////////////////////////////////////////////////////////////////////
FunctionToggleButton::~FunctionToggleButton()
{
	if (pixmap)
		delete pixmap;
	if (dpixmap)
		delete dpixmap;
	if (bgUp)
		delete bgUp;
	if (bgDown)
		delete bgDown;
	if (bgDisabled)
		delete bgDisabled;
}

///////////////////////////////////////////////////////////////////////////////
void FunctionToggleButton::draw()
{
	if (!bgUp)
	{
		//Be sure to do the normal drawing stuff, so that the button looks right.
		Fl_Button::draw();

		//Draw the properly (dis)enabled pixmap.
		if (active())
		{
			//Draw the picture in the center of the button.
			//even if the button dimensions aren't equal.
			if (pixmap->count())
				pixmap->draw(x()+w()/2-pixmap->w()/2, y()+h()/2-pixmap->h()/2);
		}
		else
		{
			if(!dpixmap)
			{
				if (pixmap->count())
				{
					dpixmap = (Fl_PNG_Image*) pixmap->copy();
					dpixmap->inactive();
				}
			}
			if (dpixmap)
				dpixmap->draw(x()+w()/2-pixmap->w()/2, y()+h()/2-pixmap->h()/2);
		}
	}
	else	// has bg image
	{
		fl_clip(myX, myY, myW, myH);
		fl_color(FL_BLACK);
		myX = x();
		myY = y();
		fl_rectf(myX, myY, myW, myH);
		if (active())
		{
			if (id == *togglevar)
			{
				if (bgDown)
					bgDown->draw(myX, myY);
				else
					bgUp->draw(myX, myY);
			}
			else
			{
				bgUp->draw(myX, myY);
			}
		}
		else
		{
			if (bgDisabled)
				bgDisabled->draw(myX, myY);
			else
				bgUp->draw(myX, myY);
		}
		fl_pop_clip();

	}
}
