/* $Id: ServerPrefsWindow.h,v 1.17 2007/02/03 09:22:31 dick Exp $
 *
 * Xpilotedit, the cross platform map editor for Xpilot.
 *      Copyright (C) 2001 by
 *
 *      The XPilot Authors   <xpilot@xpilot.org>
 *      Jarrod Miller        <jarrod@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
 *      Bjørn Stabell        <bjoern@xpilot.org>
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
 * $Log: ServerPrefsWindow.h,v $
 * Revision 1.17  2007/02/03 09:22:31  dick
 * Remove the listeners before deleting the netclient
 *
 * Revision 1.16  2004/05/22 15:17:22  dick
 * Better 'disabled' color by changing the widget color on Activate()
 *
 * Revision 1.15  2004/01/28 00:08:06  dick
 * Move the PrefTab enum to the ServerOptions definition.  Swap Map and Sysinfo
 *
 * Revision 1.14  2002/09/11 21:30:27  dick
 * Disconnect from the server when closing the ServerPrefsWindow.
 *
 * Revision 1.13  2002/09/09 20:33:47  dick
 * Use Fl_Double_Window as our parent to avoid evil tooltip flicker.
 *
 * Revision 1.12  2002/07/23 12:42:00  jlmiller
 * Fix broken comments tab.
 *
 * Revision 1.11  2002/07/09 04:29:14  dick
 * so becomes sos (It's ServerOptions, not ServerOption)
 *
 * Revision 1.10  2002/07/08 06:24:42  dick
 * Add a new PrefsPushButton type.  Add the PrefsColorKey to the window.
 *
 * Revision 1.9  2002/06/23 06:23:52  dick
 * FLTK widgets use ServerOptionsNetClientFl instead of ServerOptionsNetClient.
 *
 * Revision 1.8  2002/05/28 19:26:51  jlmiller
 * Ability to remove server tabs, if we wish
 *
 * Revision 1.7  2002/05/23 21:20:12  dick
 * Move System to tab 1.  Shuffle a bunch of items to more logical places.
 *
 * Revision 1.6  2001/09/04 18:10:30  dick
 * Bump the window size a little bit so that all the "Items" pane fits.
 *
 * Revision 1.5  2001/08/26 09:35:04  dick
 * Use the new Status widget.
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

#ifndef	_FL_PREFSWINDOW_H_
#define	_FL_PREFSWINDOW_H_

#include "FL/Fl_Double_Window.H"
#include "FL/Fl_Input.H"
#include "FL/Fl_Check_Button.H"
#include "FL/Fl_Multiline_Input.H"
#include "FL/Fl_Tabs.H"

#include "cstring.h"

#include "Fl_Status.h"
#include "ServerOptions.h"

class	Fl_PrefsColorKey;


#define PREF_WIDTH            800 /*1024*/
#define PREF_HEIGHT           524
#define PREF_BTN_WIDTH        50
#define PREF_BTN_HEIGHT       18

class ServerOptionsNetClientFl;
class ServerPrefsWindow;

class PrefsColor {
public:
};
	
///////////////////////////////////////////////////////////////////////////////
class PrefsInput : public Fl_Input, public PrefsColor {
public:
	PrefsInput(int x,int y,int w,int h,const char *l=0);
	~PrefsInput();

	virtual	void	Listener();
	virtual void	Activate(bool onOff);
	static	void	HandleListener(void* myThis, ServerOption* so);
	static	void	HandleActivate(void* myThis, ServerOption* so, bool onOff);

	ServerOption*		so;
	ServerPrefsWindow*	spw;
//	int len;
};

///////////////////////////////////////////////////////////////////////////////
class PrefsCheckButton : public Fl_Check_Button {
public:
	PrefsCheckButton(int x,int y,int w,int h,const char *l=0);
	~PrefsCheckButton();

	virtual	void	Listener();
	virtual void	Activate(bool onOff);
	static	void	HandleListener(void* myThis, ServerOption* so);
	static	void	HandleActivate(void* myThis, ServerOption* so, bool onOff);

	ServerOption* so;
	ServerPrefsWindow*	spw;
};

///////////////////////////////////////////////////////////////////////////////
class PrefsPushButton : public Fl_Button {
public:
	PrefsPushButton(int x,int y,int w,int h,const char *l=0);
	~PrefsPushButton();

	void			Listener(ServerOption* so);
	virtual void	Activate(bool onOff);
	static	void	HandleListener(void* myThis, ServerOption* so);
	static	void	HandleActivate(void* myThis, ServerOption* so, bool onOff);

	ServerOption*		so;
	ServerPrefsWindow*	spw;
};

///////////////////////////////////////////////////////////////////////////////
class PrefsComments : public Fl_Multiline_Input {
public:
	PrefsComments(int x,int y,int w,int h,const char *l=0);

	String*				so;
	ServerPrefsWindow*	spw;
};

///////////////////////////////////////////////////////////////////////////////
#define	ServerPrefsWindowSUPERCLASS	Fl_Double_Window
class ServerPrefsWindow : public ServerPrefsWindowSUPERCLASS {
public:
	ServerPrefsWindow(int w,int h, const char* l= 0);
	ServerPrefsWindow(int x,int y,int w,int h,const char *l=0);
	virtual ~ServerPrefsWindow();

	static	Fl_Callback	FlCallback;

	void				SetStatus(PCSTR text, ...);
	void				Initialize(ServerOptions* lpServerOptions);
	void				SetTitle(PCSTR title);
	void				SetTab(PrefTab which);
	void				RemoveTab(PrefTab which);
	Fl_PrefsColorKey*	GetColorKey();

	ServerOptions*				sos;
	ServerOptionsNetClientFl*	sonc;
private:
	PCSTR				MakeTooltip(ServerOption* so);

	Fl_Tabs*			flTabs;
	Fl_Widget*			tabs[MaxTab];
	Fl_Status*			status;
	Fl_PrefsColorKey*	colorKey;

	bool				sosOwned;		// Does the Server Panel own the ServerOptions?
};

extern	ServerPrefsWindow*	CreateServerPrefsWindow(ServerOptions* lpServerOptions, PrefTab whichTab=TabMapInfo);

inline void ServerPrefsWindow::SetTab(PrefTab which) { flTabs->value(tabs[which]); };
inline void ServerPrefsWindow::RemoveTab(PrefTab which) { flTabs->remove(tabs[which]); };
inline Fl_PrefsColorKey* ServerPrefsWindow::GetColorKey() { return(colorKey); };

#endif	// _FL_PREFSWINDOW_H_
