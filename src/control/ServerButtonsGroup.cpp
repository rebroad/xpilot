/* $Id: ServerButtonsGroup.cpp,v 1.4 2004/05/30 16:14:37 dick Exp $
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
 * $Log: ServerButtonsGroup.cpp,v $
 * Revision 1.4  2004/05/30 16:14:37  dick
 * Hide all the team buttons and only show the ones actually available
 * (instead of dimming the unused ones).
 *
 * Revision 1.3  2004/05/13 09:20:37  dick
 * UpdateTeamButtons() becomes UpdateButtons() and handle them all
 *
 * Revision 1.2  2004/05/10 17:38:38  dick
 * gcc3 needs string.h
 *
 * Revision 1.1  2004/05/10 12:45:45  dick
 * Move the buttons at the top of XPWhereWindow to their own class ServerButtonsGroup
 * so we can use them in other lists.
 *
 */

#include "StdAfx.h"
#include <string.h>


#include "ServerButtonsGroup.h"
#include "controlconst.h"
#include "xpcTypes.h"

#include "ServerSite.h"

#define TEAMBUTOFS	4
const int	teamButOfs=BUTTON_SIZE_LG*TEAMBUTOFS+BUTTON_SIZE;

xpe_tbbutton XPWhereActionButtonDefs[] =
{
 {BUTTON_SIZE_LG*0,		    0,           BUTTON_SIZE_LG, BUTTON_SIZE_LG, "Refresh List", "refresh", IDM_REFRESH_LIST},
 {BUTTON_SIZE_LG*1,		    0,           BUTTON_SIZE_LG, BUTTON_SIZE_LG, "Measure Lag", "ping", IDM_MEASURE_LAG},
 {BUTTON_SIZE_LG*2,		    0,           BUTTON_SIZE_LG, BUTTON_SIZE_LG, "Admin Game", "serveroptions", IDM_ADMIN_GAME},
 {BUTTON_SIZE_LG*3,         0,           BUTTON_SIZE_LG, BUTTON_SIZE_LG, "Join Game", "xpilot", IDM_JOIN_GAME},
 {teamButOfs+BUTTON_SIZE*0, 0,           BUTTON_SIZE, BUTTON_SIZE, "Join Team 0", "num_0", IDM_JOINTEAM0},
 {teamButOfs+BUTTON_SIZE*1, 0,           BUTTON_SIZE, BUTTON_SIZE, "Join Team 1", "num_1", IDM_JOINTEAM1},
 {teamButOfs+BUTTON_SIZE*2, 0,           BUTTON_SIZE, BUTTON_SIZE, "Join Team 2", "num_2", IDM_JOINTEAM2},
 {teamButOfs+BUTTON_SIZE*3, 0,           BUTTON_SIZE, BUTTON_SIZE, "Join Team 3", "num_3", IDM_JOINTEAM3},
 {teamButOfs+BUTTON_SIZE*4, 0,           BUTTON_SIZE, BUTTON_SIZE, "Join Team 4", "num_4", IDM_JOINTEAM4},

 {teamButOfs+BUTTON_SIZE*0, BUTTON_SIZE, BUTTON_SIZE, BUTTON_SIZE, "Join Team 5", "num_5", IDM_JOINTEAM5},
 {teamButOfs+BUTTON_SIZE*1, BUTTON_SIZE, BUTTON_SIZE, BUTTON_SIZE, "Join Team 6", "num_6", IDM_JOINTEAM6},
 {teamButOfs+BUTTON_SIZE*2, BUTTON_SIZE, BUTTON_SIZE, BUTTON_SIZE, "Join Team 7", "num_7", IDM_JOINTEAM7},
 {teamButOfs+BUTTON_SIZE*3, BUTTON_SIZE, BUTTON_SIZE, BUTTON_SIZE, "Join Team 8", "num_8", IDM_JOINTEAM8},
 {teamButOfs+BUTTON_SIZE*4, BUTTON_SIZE, BUTTON_SIZE, BUTTON_SIZE, "Join Team 9", "num_9", IDM_JOINTEAM9},

 {teamButOfs+BUTTON_SIZE*9,0, BUTTON_SIZE, BUTTON_SIZE, "Configure Internet List", "properties", IDM_XPWHERE_CONFIG},
};

///////////////////////////////////////////////////////////////////////////////
ServerButtonsGroup::ServerButtonsGroup(int x, int y, int w,int h, SBGCallback* cb, void* o)
				   : ServerButtonsGroupSUPERCLASS(x,y,w,h, "")
{
	int	i;
	//We require a subgroup for the buttons, so they dont resize wierdly.
	//Build the Action buttons, Teamjoins, etc.
	for (i = 0; i < ButtonMAX; i++)
    {
		actionButtons[i] = new FunctionButton(XPWhereActionButtonDefs[i].x+2, XPWhereActionButtonDefs[i].y+2,
			XPWhereActionButtonDefs[i].w, XPWhereActionButtonDefs[i].h, "", XPWhereActionButtonDefs[i].pm);
		actionButtons[i]->callback(myCallback);
		actionButtons[i]->tooltip(XPWhereActionButtonDefs[i].ttip);
		actionButtons[i]->user_data(this);
		actionButtons[i]->id = XPWhereActionButtonDefs[i].id;
    }
	this->end();
	this->resizable(NULL);

	parentCallback = cb;
	parentObject = o;
}

///////////////////////////////////////////////////////////////////////////////
void ServerButtonsGroup::UpdateButtons(const ServerSite* si)
{
	bool	f = false;
	int		t;
	int		curTeamShown = 0;

	for (t = 0; t <= 9; t++)
	{
		if (si->teams)
			f = true;
		if (!si->freebases[t])
			f = false;

		if (f && !si->inactive)
		{
//			actionButtons[ButtonJoinTeam0+t]->activate();
			actionButtons[ButtonJoinTeam0+t]->show();
			actionButtons[ButtonJoinTeam0+t]->position(
				XPWhereActionButtonDefs[curTeamShown+TEAMBUTOFS].x,
				XPWhereActionButtonDefs[curTeamShown+TEAMBUTOFS].y);
			actionButtons[ButtonJoinTeam0+t]->redraw();
			curTeamShown++;
		}
		else
		{
//			actionButtons[ButtonJoinTeam0+t]->deactivate();
			actionButtons[ButtonJoinTeam0+t]->hide();
		}
	}

	if (strcmp(si->version,"5.0.0") < 0 || si->inactive)
		actionButtons[ButtonAdminServer]->deactivate();
	else
		actionButtons[ButtonAdminServer]->activate();

	if (si->inactive)
		actionButtons[ButtonConnectClient]->deactivate();
	else
		actionButtons[ButtonConnectClient]->activate();
}

///////////////////////////////////////////////////////////////////////////////
void ServerButtonsGroup::myCallback(Fl_Widget* w, void* v)
{
	ServerButtonsGroup* sbg = (ServerButtonsGroup*)v;
	sbg->myCallback(w);
}

void ServerButtonsGroup::myCallback(Fl_Widget* w)
{
	FunctionButton* but = (FunctionButton*)w;
	int id = but->id;
	this->parentCallback(id, parentObject);
}
