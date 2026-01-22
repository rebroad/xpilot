/*
 * $Id: LocalServerPanelSimple.h,v 1.7 2004/06/01 23:13:00 dick Exp $
 *
 * The "Simple" local server configuration panel
 * i.e. newbie mode = ON
 *
 * XPilot.  Copyright (C) 2002 by
 *
 *      The XPilot Authors   <xpilot@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
 *      Jarrod Miller        <jarrod@xpilot.org>
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
 * $Log: LocalServerPanelSimple.h,v $
 * Revision 1.7  2004/06/01 23:13:00  dick
 * You can't really make a new button from an object and stick it in a static.
 * (Who owns it if you open 2 MapEditors?)  So now each window keeps track of
 * his own buttons.
 *
 * Revision 1.6  2004/01/26 17:01:41  dick
 * LocalSimple panel only has Start Server, Play Game, and Change Map.
 * LocalAdvanced also has AdminServer, StartScoreServer, BrowseScoreServer
 * and dummy edit defaults and edit robots buttons.
 *
 * Revision 1.5  2004/01/15 05:38:25  dick
 * Add a StartScoreServer button to the local panel
 *
 * Revision 1.4  2002/09/12 20:02:03  dick
 * Handle selecting and changing the map that the server will use.
 *
 * Revision 1.3  2002/09/09 23:50:55  dick
 * Route commands (button presses) back to the LocalServerPanel for handling.
 *
 * Revision 1.2  2002/09/07 18:30:26  dick
 * Don't include Fl_Output.H, rather just have a forward class reference.
 *
 * Revision 1.1  2002/09/07 17:52:00  dick
 * Skeleton support for the Simple/Advanced Local server.
 *
 */

#ifndef	_LOCALSERVERPANELSIMPLE_H_
#define	_LOCALSERVERPANELSIMPLE_H_

#include "FL/Fl_Group.H"

class LocalServerPanel;
class Fl_Output;
class FunctionButton;

const int numLocalServerPanelSimpleButtons = 2;

#define	LocalServerPanelSimpleSUPERCLASS	Fl_Group
class LocalServerPanelSimple : public LocalServerPanelSimpleSUPERCLASS {
public:
	LocalServerPanelSimple(int x, int y, int w,int h,const char *l=0);

	void	SetLocalServerPanel(LocalServerPanel* _lsp);

	void	UpdateWidgets();

	static void	ChangeMapCallback(Fl_Widget* w, void* v);
	static void	PlayGameCallback(Fl_Widget* w, void* v);

	void	ChangeMapCallback();
	void	PlayGameCallback();

	Fl_Output*	mapName;

private:
	FunctionButton*		localServerPanelSimpleButtons[numLocalServerPanelSimpleButtons];

	LocalServerPanel*		lsp;
};

inline void LocalServerPanelSimple::SetLocalServerPanel(LocalServerPanel* _lsp)
	{ lsp = _lsp; };

#endif	// _LOCALSERVERPANELSIMPLE_H_
