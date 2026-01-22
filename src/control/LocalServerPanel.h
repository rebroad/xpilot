/*
 * $Id: LocalServerPanel.h,v 1.13 2004/06/01 23:13:00 dick Exp $
 *
 * XPilot.  Copyright (C) 2002 by
 *
 *      The XPilot Authors   <xpilot@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
 *      Jarrod Miller        <jarrod@xpilot.org>
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
 * $Log: LocalServerPanel.h,v $
 * Revision 1.13  2004/06/01 23:13:00  dick
 * You can't really make a new button from an object and stick it in a static.
 * (Who owns it if you open 2 MapEditors?)  So now each window keeps track of
 * his own buttons.
 *
 * Revision 1.12  2004/04/21 23:27:16  dick
 * Handle starting up and shutting down the ScoreServer from the Advanced panel.
 *
 * Revision 1.11  2004/02/01 15:51:48  dick
 * Fix tooltips.  LocalServerPanel caused the widgets to be updated every
 * two seconds (even when this window is hidden), which resets the tooltips timer.
 * Use forceUpdate in combination with current settings to determine whether
 * to actually alter the widgets.  (forceUpdate is needed for initial settings)
 * I don't really like the forceUpdate technique, it's a runaway stack waiting
 * to happen, but it works.
 *
 * Revision 1.10  2004/01/26 17:01:41  dick
 * LocalSimple panel only has Start Server, Play Game, and Change Map.
 * LocalAdvanced also has AdminServer, StartScoreServer, BrowseScoreServer
 * and dummy edit defaults and edit robots buttons.
 *
 * Revision 1.9  2004/01/15 05:38:25  dick
 * Add a StartScoreServer button to the local panel
 *
 * Revision 1.8  2002/09/12 20:02:03  dick
 * Handle selecting and changing the map that the server will use.
 *
 * Revision 1.7  2002/09/10 05:16:50  dick
 * Use a timer to heartbeat the connection to the Local Server.
 *
 * Revision 1.6  2002/09/09 23:52:51  dick
 * Create and use a LocalServerControlClient to talk to the server (and to determine
 * if the server is running).
 *
 * Revision 1.5  2002/09/07 17:52:00  dick
 * Skeleton support for the Simple/Advanced Local server.
 *
 * Revision 1.4  2002/07/23 19:58:24  jlmiller
 * No, actually the #include "" was right the first time, so lets put it back.
 *
 * Revision 1.3  2002/07/23 18:06:16  dick
 * The button callbacks become member functions.  Disable unused server buttons
 * for now.
 *
 * Revision 1.2  2002/07/23 13:24:12  jlmiller
 * #includes should be <> instead of ""
 *
 * Revision 1.1  2002/07/19 07:18:34  dick
 * The panel for the Local Server on the main window.
 *
 */

#ifndef	_LOCALSERVERPANEL_H_
#define	_LOCALSERVERPANEL_H_

#include "FL/Fl_Group.H"

#include "types.h"

class LocalServerControlClient;
class LocalServerPanelSimple;
class LocalServerPanelAdvanced;
class Fl_Check_Button;
class FunctionButton;

const int numLocalServerPanelButtons = 1;

#define	LocalServerPanelSUPERCLASS	Fl_Group
class LocalServerPanel : public LocalServerPanelSUPERCLASS {
public:
	LocalServerPanel(int x, int y, int w,int h,const char *l=0);
	virtual ~LocalServerPanel();
	void		SetErrMsgHandler(ErrMsgHandler _emh, void* _emhThis);

	void	DetermineServerRunning();
	void	UpdateWidgets(bool forceUpdate=false);
	bool	IsServerRunning();

	static Fl_Callback	StartServerCallback;
	static Fl_Callback	AdvancedCheckCallback;
	static void			FlTimeoutHandler(void* v);
	static void			ServerExitedTimeout(void* v);

	void	AdminServerCallback(int world);
	void	BrowseScoreServerCallback(int world);
	void	PlayGameCallback(int world);
	void	ChangeMapCallback(int world);

	// control callbacks
	void	UpdateWorldInfo(int	worldIndex, PCSTR mapName, bool forceUpdate=false);

private:
	void	StartServerCallback();
	void	AdvancedCheckCallback();
	void	FlTimeoutHandler();
	void	ServerExitedTimeout();

	void	ShowSelectedPanel(bool forceUpdate=false);
	void	EmptyWorldAttributes(bool forceUpdate=false);

private:
	FunctionButton*	localServerPanelButtons[numLocalServerPanelButtons];

	LocalServerControlClient*	lscc;
	Fl_Check_Button*			advancedCheck;
	LocalServerPanelSimple*		lspSimple;
	LocalServerPanelAdvanced*	lspAdvanced;

	bool	serverWasRunning;
	bool	advancedShown;
private:
	ErrMsgHandler	emh;
	void*			emhThis;
};

#endif	// _LOCALSERVERPANEL_H_
