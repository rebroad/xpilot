/* $Id: ServerButtonsGroup.h,v 1.3 2004/05/30 16:14:37 dick Exp $
 *
 * ServerButtonsGroup - Handle the buttons at the top of PrivatePanel and InternetPanel.
 *
 * XPilot5 - Manage all local XPilot activity.
 *
 *      Copyright (C) 2004 by
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
 * $Log: ServerButtonsGroup.h,v $
 * Revision 1.3  2004/05/30 16:14:37  dick
 * Hide all the team buttons and only show the ones actually available
 * (instead of dimming the unused ones).
 *
 * Revision 1.2  2004/05/13 09:20:37  dick
 * UpdateTeamButtons() becomes UpdateButtons() and handle them all
 *
 * Revision 1.1  2004/05/10 12:45:45  dick
 * Move the buttons at the top of XPWhereWindow to their own class ServerButtonsGroup
 * so we can use them in other lists.
 *
 */

#ifndef	_SERVERBUTTONGSGROUP_H_
#define	_SERVERBUTTONGSGROUP_H_

#include "FL/Fl_Group.H"
#include "xpcTypes.h"

enum ButtonTypes {
	ButtonRefresh,
	ButtonMeasureLag,
	ButtonAdminServer,
	ButtonConnectClient,
	ButtonJoinTeam0,
	ButtonJoinTeam1,
	ButtonJoinTeam2,
	ButtonJoinTeam3,
	ButtonJoinTeam4,
	ButtonJoinTeam5,
	ButtonJoinTeam6,
	ButtonJoinTeam7,
	ButtonJoinTeam8,
	ButtonJoinTeam9,
	ButtonConfigureInetList,

	ButtonMAX
};



class ServerSite;
class FunctionButton;

#define	ServerButtonsGroupSUPERCLASS	Fl_Group
class ServerButtonsGroup : public ServerButtonsGroupSUPERCLASS {
public:
	ServerButtonsGroup(int x, int y, int w,int h, SBGCallback* c, void* o);
	ServerButtonsGroup(int x, int y, int w,int h);	//, SBGCallback c, void* o);
	//virtual ~ServerButtonsGroup();

	void	UpdateButtons(const ServerSite* si);

	static void myCallback(Fl_Widget* w, void* v);
	void	myCallback(Fl_Widget* w);


	FunctionButton*	actionButtons[ButtonMAX];

private:
	SBGCallback*	parentCallback;
	void*			parentObject;

};

#endif		// _SERVERBUTTONGSGROUP_H_
