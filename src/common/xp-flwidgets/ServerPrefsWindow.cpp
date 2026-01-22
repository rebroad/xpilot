/* $Id: ServerPrefsWindow.cpp,v 1.23 2004/05/24 07:35:26 dick Exp $
 *
 * XPilotedit, the cross platform map editor for XPilot.
 *      Copyright (C) 2001 by
 *
 *      The XPilot Authors   <xpilot@xpilot.org>
 *      Jarrod Miller        <jarrod@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
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
 * $Log: ServerPrefsWindow.cpp,v $
 * Revision 1.23  2004/05/24 07:35:26  dick
 * Fix the dim color for PrefsInput (the text widget)
 *
 * Revision 1.22  2004/05/22 15:17:22  dick
 * Better 'disabled' color by changing the widget color on Activate()
 *
 * Revision 1.21  2004/03/03 16:04:33  dick
 * Use IsConnected() to determine if we are mapediting or online
 *
 * Revision 1.20  2004/02/28 07:51:54  dick
 * Case sensitive Unix on the fl_draw.h
 *
 * Revision 1.19  2004/02/28 07:35:15  dick
 * Use XPilot blackened colors
 *
 * Revision 1.18  2004/02/21 07:04:34  dick
 * Use HasGrandParent() to determine whether to set OPT_MAP with the attribute change
 * or if we should change anonymously and let someone else figure out the origin.
 *
 * Revision 1.17  2004/02/06 05:05:55  dick
 * Apparently with fltk 1.1.4, i do own the sonc (which i thought i did...).
 * So we clean that bit up, which closes the connection to the server.
 *
 * Revision 1.16  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.15  2004/01/28 00:08:06  dick
 * Move the PrefTab enum to the ServerOptions definition.  Swap Map and Sysinfo
 *
 * Revision 1.14  2004/01/15 06:38:49  dick
 * Don't delete the ServerOptionsNetClientFl.  fltk will do that.
 *
 * Revision 1.13  2002/09/12 19:49:02  dick
 * Whitespace
 *
 * Revision 1.12  2002/09/11 21:30:27  dick
 * Disconnect from the server when closing the ServerPrefsWindow.
 *
 * Revision 1.11  2002/07/23 12:42:00  jlmiller
 * Fix broken comments tab.
 *
 * Revision 1.10  2002/07/09 04:29:14  dick
 * so becomes sos (It's ServerOptions, not ServerOption)
 *
 * Revision 1.9  2002/07/08 06:24:42  dick
 * Add a new PrefsPushButton type.  Add the PrefsColorKey to the window.
 *
 * Revision 1.8  2002/05/23 21:20:12  dick
 * Move System to tab 1.  Shuffle a bunch of items to more logical places.
 *
 * Revision 1.7  2002/05/19 04:56:01  jlmiller
 * Show option help as the tooltip.
 *
 * Revision 1.6  2001/08/26 09:35:04  dick
 * Use the new Status widget.
 *
 * Revision 1.5  2001/08/12 11:17:21  dick
 * Init valList
 *
 * Revision 1.4  2001/08/07 20:44:52  dick
 * Add a status bar to the bottom of the window
 *
 * Revision 1.3  2001/08/01 19:42:48  dick
 * New method Activate(), a partner of Listener().  When an attribute changes
 * value, other attributes might be enabled or disabled.  Ex: if teamPlay is
 * off, then teamCannons becomes undefined, or disabled.
 *
 * Revision 1.2  2001/08/01 17:37:16  dick
 * Add ServerOption Listeners.  When a ServerOption is changed, notify all
 * registered listeners. (Try opening two prefs windows on the same map).
 *
 * Revision 1.1  2001/08/01 05:10:08  dick
 * PrefsWindow becomes ServerPrefsWindow
 *
 * Revision 1.1  2001/07/31 04:29:23  dick
 * gui_prefs becomes PrefsWindow
 *
 */
#include "StdAfx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#include "FL/Fl.H"
#include "FL/Fl_Tabs.H"
#include "FL/Enumerations.H"
#include "FL/fl_draw.H"

#include "portability.h"
#include "ServerPrefsWindow.h"
#include "Fl_XPColors.h"

#include "Fl_PrefsColorKey.h"

#include "portability.h"
#include "config.h"
#include "ServerOptions.h"
#include "ServerPrefsWindow.h"
#include "ServerOptionsNetClientFl.h"


static const char* TabSheets[] =
	{
		"System",
		"Map Info",
		"Rbts\nBounce",
		"Visibility\nTeams",
		"Cannons\nMines",
		"Rounds\nConnection",
		"Items",
		"Probs",
		"Scoring",
		"Comments",
		"Server",
	};

PrefsInput::PrefsInput(int x,int y,int w,int h,const char *l)
        : Fl_Input(x,y,w,h,l)
{
}
PrefsCheckButton::PrefsCheckButton(int x,int y,int w,int h,const char *l)
        : Fl_Check_Button(x,y,w,h,l)
{
}
PrefsComments::PrefsComments(int x,int y,int w,int h,const char *l)
        : Fl_Multiline_Input(x,y,w,h,l)
{
}
PrefsPushButton::PrefsPushButton(int x,int y,int w,int h,const char *l)
        : Fl_Button(x,y,w,h,l)
{

}

///////////////////////////////////////////////////////////////////////////////
void prefs_input_changed_cb(PrefsInput* w, void*)
{
	if (!w->so->IsConnected())
	{
		w->so->Set(w->value(), FALSE, OPT_MAP);
	}
	else
    *w->so = w->value();
}

///////////////////////////////////////////////////////////////////////////////
void prefs_checkbutton_pressed_cb(PrefsCheckButton* w, void*)
{
	if (!w->so->IsConnected())
	{
		w->so->Set(w->value() ? "t" : "f", FALSE, OPT_MAP);
	}
	else
		*w->so = w->value();
}

///////////////////////////////////////////////////////////////////////////////
void prefs_comments_changed_cb(PrefsComments* w, void*)
{
	*w->so = w->value();
}

///////////////////////////////////////////////////////////////////////////////
void prefs_pushbutton_pressed_cb(PrefsPushButton* w, void*)
{
	*w->so = "";
}

///////////////////////////////////////////////////////////////////////////////
ServerPrefsWindow* CreateServerPrefsWindow(ServerOptions* lpServerOptions, PrefTab whichTab)
{
	ServerPrefsWindow* pw = new ServerPrefsWindow(PREF_WIDTH, PREF_HEIGHT);
	pw->Initialize(lpServerOptions);
	pw->SetTab(whichTab);
	return(pw);
}

///////////////////////////////////////////////////////////////////////////////
ServerPrefsWindow::ServerPrefsWindow(int w, int h, const char* l)
: ServerPrefsWindowSUPERCLASS(w, h, l)
{
	colorKey = NULL;
	sonc = NULL;
}

///////////////////////////////////////////////////////////////////////////////
ServerPrefsWindow::~ServerPrefsWindow()
{
	if (sonc)
		delete sonc;
	sonc = NULL;
}

///////////////////////////////////////////////////////////////////////////////
#define	WIDGET_INSET	10
#define	TAB_TOP			WIDGET_INSET
#define	TAB_HEIGHT		35
#define	STATUS_HEIGHT	20
#define	KEY_HEIGHT		14

#define	TAB_GROUP_INSET	10
#define	TAB_BOTTOM	(TAB_TOP + TAB_HEIGHT)
#define	OPTIONS_TOP	(TAB_TOP + TAB_HEIGHT + TAB_GROUP_INSET)
#define	OPTIONS_HEIGHT	(PREF_HEIGHT-OPTIONS_TOP-WIDGET_INSET-TAB_GROUP_INSET-KEY_HEIGHT)
#define	KEY_TOP			(PREF_HEIGHT-WIDGET_INSET-STATUS_HEIGHT-KEY_HEIGHT)
#define	STATUS_TOP		(PREF_HEIGHT-WIDGET_INSET-STATUS_HEIGHT)

///////////////////////////////////////////////////////////////////////////////
void ServerPrefsWindow::Initialize(ServerOptions* lpServerOptions)
{
	sos = lpServerOptions;
	int width = (PREF_WIDTH-80)/6;
	int height = PREF_BTN_HEIGHT+2;
	int i, j;
	int wintype;
	PrefsInput*			inp;
	PrefsCheckButton*	chkb;
	PrefsPushButton*	eventb;

	color(xpblack);

	if (sos)
		wintype = OPTIONMAP;
	else
	{
		sos = new ServerOptions;
		wintype = OPTIONALL;
	}

	colorKey = new Fl_PrefsColorKey(WIDGET_INSET,
						   KEY_TOP,
						   PREF_WIDTH-(WIDGET_INSET*2),
						   KEY_HEIGHT);

	Fl_Tabs* o = new Fl_Tabs(10,
							 TAB_TOP,
							 PREF_WIDTH-(WIDGET_INSET*2),
							 PREF_HEIGHT-(WIDGET_INSET*2)-STATUS_HEIGHT-KEY_HEIGHT);
	flTabs = o;
	Fl_Color cr = fl_contrast(xpred, xpblue);
	o->color(cr, cr);

	for (j=0; j<MaxTab; j++)
	{
		Fl_Group* o = new Fl_Group(TAB_GROUP_INSET,
								   TAB_BOTTOM,
								   PREF_WIDTH-(TAB_GROUP_INSET*2),
								   OPTIONS_HEIGHT,
								   TabSheets[j]);
		tabs[j] = o;
		o->color(xpblack,xpblue);
		o->labelfont(FL_TIMES_ITALIC);
		if (j==TabComments)
		{
			PrefsComments* comm =
				new PrefsComments(WIDGET_INSET+TAB_GROUP_INSET,
								  OPTIONS_TOP,
								  PREF_WIDTH-(TAB_GROUP_INSET*2)-(WIDGET_INSET*2),
								  OPTIONS_HEIGHT-(WIDGET_INSET*2));
			comm->value(sos->comments);
			comm->so = &sos->comments;
			comm->callback((Fl_Callback *)prefs_comments_changed_cb);
		}
		for (i=0; i<sos->numPrefs; i++)
		{
			if (wintype != OPTIONALL)
				if ((sos->prefsArray[i]->visibleto != wintype) &&
					(sos->prefsArray[i]->visibleto != OPTIONALL))
					continue;
			if (sos->prefsArray[i]->sheet == j)
			{
				switch (sos->prefsArray[i]->valType)
				{
				case valInt:
				case valReal:
				case valIPos:
				case valString:
				case valSec:
				case valPerSec:
				case valList:
					inp = new PrefsInput(20+width+sos->prefsArray[i]->column*(width*2+20),
										 OPTIONS_TOP+sos->prefsArray[i]->row*height,
										 width, 20, sos->prefsArray[i]->label);
					inp->spw = this;
					inp->value(sos->prefsArray[i]->GetString());
					inp->so = sos->prefsArray[i];
//					inp->len = sos->prefsArray[i]->length;	// XXX: Do we care?
					inp->tooltip(MakeTooltip(inp->so));
					inp->color(xpred, xpwhite);
					inp->labelcolor(xpwhite);
					inp->labelfont(FL_TIMES_ITALIC);

					inp->callback((Fl_Callback *)prefs_input_changed_cb);
					sos->prefsArray[i]->AddListener(&PrefsInput::HandleListener,
													&PrefsInput::HandleActivate,
													inp);
					inp->Listener();
					//inp->when(FL_WHEN_CHANGED);
					break;
				case valBool:
					chkb = new PrefsCheckButton(20+width+sos->prefsArray[i]->column*(width*2+20)/*+width/3*/,
												OPTIONS_TOP+sos->prefsArray[i]->row*height,
												width, 20, sos->prefsArray[i]->label);
					chkb->spw = this;
					chkb->align(FL_ALIGN_LEFT);
					chkb->down_box(FL_DOWN_BOX);
					chkb->so = sos->prefsArray[i];
					chkb->value(sos->prefsArray[i]->GetInt());
					chkb->tooltip(sos->prefsArray[i]->help);
					chkb->color(xpred, xpblack);
					chkb->labelcolor(xpwhite);
					chkb->labelfont(FL_TIMES_ITALIC);
					chkb->selection_color(xpred);
					chkb->tooltip(MakeTooltip(chkb->so));

					chkb->callback((Fl_Callback *)prefs_checkbutton_pressed_cb);
					sos->prefsArray[i]->AddListener(&PrefsCheckButton::HandleListener,
													&PrefsCheckButton::HandleActivate,
													chkb);
					chkb->Listener();
					//inp->when(FL_WHEN_CHANGED);
				break;
				case valEvent:
					eventb = new PrefsPushButton(20+width+sos->prefsArray[i]->column*(width*2+20),
										 OPTIONS_TOP+sos->prefsArray[i]->row*height,
										 width, 20, sos->prefsArray[i]->label);
					eventb->spw = this;
					eventb->value(0);
					eventb->so = sos->prefsArray[i];
					//eventb->len = so->prefsArray[i]->length;	// XXX: Do we care?
					eventb->tooltip(MakeTooltip(eventb->so));

					eventb->callback((Fl_Callback *)prefs_pushbutton_pressed_cb);
					sos->prefsArray[i]->AddListener(&PrefsPushButton::HandleListener,
													&PrefsPushButton::HandleActivate,
													eventb);
					//inp->when(FL_WHEN_CHANGED);
					break;
				}
			}
		}
		o->end();
		Fl_Group::current()->resizable(o);
	}

	o->end();

	status = new Fl_Status(WIDGET_INSET,
						   STATUS_TOP,
						   PREF_WIDTH-(WIDGET_INSET*2),
						   STATUS_HEIGHT);
	status->color(FL_GRAY);
	status->value("Ready");

	Fl_Group::current()->resizable(o);
	o->end();

	Fl_Widget *v = flTabs->value();
	v->color(xpblack);

	this->callback((Fl_Callback *)&ServerPrefsWindow::FlCallback, this);
	this->resizable(NULL);
	this->show();
}

///////////////////////////////////////////////////////////////////////////////
void ServerPrefsWindow::SetTitle(PCSTR title)
{
	String	s;
	s = title;
	s += " - XPAdmin";
	label(s);
}

///////////////////////////////////////////////////////////////////////////////
void ServerPrefsWindow::FlCallback(Fl_Widget* win, void* v)
{
	ServerPrefsWindow* spw = (ServerPrefsWindow*)v;
	spw->hide();
	delete spw;
}

///////////////////////////////////////////////////////////////////////////////
PCSTR ServerPrefsWindow::MakeTooltip(ServerOption* so)
{
//	String s = so->help;	// I wanted to build a tooltip with the optionname
//	s += so->name;          // and default value (and most importantly, the symbol :)
	return(so->help);		// but tooltip wants a PCSTR, so skip it for now.
}

///////////////////////////////////////////////////////////////////////////////
void ServerPrefsWindow::SetStatus(PCSTR fmt, ...)
{
    va_list	ap;
    char	s[512];

    va_start(ap, fmt);
    vsnprintf(s, 511, fmt, ap);
	status->value(s);
    va_end(ap);
//	status->redraw();
	Fl::check();
};


//-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static Fl_Color DimColor(Fl_Color c)
{
	return(fl_color_average(c, FL_BLACK, 0.10f));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PrefsInput::HandleListener(void* myThis, ServerOption* so)
{
	PrefsInput* pi = (PrefsInput*)myThis;
	pi->Listener();
}

///////////////////////////////////////////////////////////////////////////////
void PrefsInput::Listener()
{
	value(so->GetString());
	Activate(active() ? true : false);
}

///////////////////////////////////////////////////////////////////////////////
PrefsInput::~PrefsInput()
{
	so->RemoveListener(&PrefsInput::HandleListener, this);
}

///////////////////////////////////////////////////////////////////////////////
void PrefsInput::HandleActivate(void* myThis, ServerOption* so, bool onOff)
{
	PrefsInput* pi = (PrefsInput*)myThis;
	pi->Activate(onOff);
}

///////////////////////////////////////////////////////////////////////////////
void PrefsInput::Activate(bool onOff)
{
	if (onOff)
		activate();
	else
		deactivate();

	if (spw && spw->GetColorKey())
	{
		Fl_Color color = spw->GetColorKey()->GetOptColor(so->optOrigin);
		if (!onOff)
			color = DimColor(color);
		if (color != labelcolor())
		{
			labelcolor(color);
			parent()->redraw();
		}
	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
PrefsCheckButton::~PrefsCheckButton()
{
	so->RemoveListener(&PrefsCheckButton::HandleListener, this);
}

///////////////////////////////////////////////////////////////////////////////
void PrefsCheckButton::HandleListener(void* myThis, ServerOption* so)
{
	PrefsCheckButton* pcb = (PrefsCheckButton*)myThis;
	pcb->Listener();
}

///////////////////////////////////////////////////////////////////////////////
void PrefsCheckButton::Listener()
{
	value(so->GetInt());
	Activate(active() ? true : false);
}

///////////////////////////////////////////////////////////////////////////////
void PrefsCheckButton::HandleActivate(void* myThis, ServerOption* so, bool onOff)
{
	PrefsCheckButton* pcb = (PrefsCheckButton*)myThis;
	pcb->Activate(onOff);
}

///////////////////////////////////////////////////////////////////////////////
void PrefsCheckButton::Activate(bool onOff)
{
	if (onOff)
		activate();
	else
		deactivate();
	if (spw && spw->GetColorKey())
	{
		Fl_Color color = spw->GetColorKey()->GetOptColor(so->optOrigin);
		if (!onOff)
			color = DimColor(color);
		if (color != labelcolor())
		{
			labelcolor(color);
			parent()->redraw();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
PrefsPushButton::~PrefsPushButton()
{
	so->RemoveListener(&PrefsPushButton::HandleListener, this);
}

///////////////////////////////////////////////////////////////////////////////
void PrefsPushButton::HandleListener(void* myThis, ServerOption* so)
{
	PrefsPushButton* pcb = (PrefsPushButton*)myThis;
	pcb->Listener(so);
}

///////////////////////////////////////////////////////////////////////////////
void PrefsPushButton::Listener(ServerOption* so)
{
	if (!strcmp(so->name, "saveTuned"))
	{
		String	s = so->GetString();	// saveTuned returns a status message as a String
		if (!s.IsEmpty())
			spw->SetStatus(s);
	}
	Activate(active() ? true : false);
}

///////////////////////////////////////////////////////////////////////////////
void PrefsPushButton::HandleActivate(void* myThis, ServerOption* so, bool onOff)
{
	PrefsPushButton* pcb = (PrefsPushButton*)myThis;
	pcb->Activate(onOff);
}

///////////////////////////////////////////////////////////////////////////////
void PrefsPushButton::Activate(bool onOff)
{
	if (onOff)
		activate();
	else
		deactivate();
	/*
	if (spw && spw->GetColorKey())
	{
		Fl_Color color = spw->GetColorKey()->GetOptColor(so->optOrigin);
		if (!onOff)
		{
			color = DimColor(color);
		}
		//D(xpprintf("old color=%d new color=%d\n", labelcolor(), color);)
		if (color != labelcolor())
		{
			labelcolor(color);
			parent()->redraw();
		}
	}
	*/
}

