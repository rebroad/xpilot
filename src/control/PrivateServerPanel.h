/* $Id: PrivateServerPanel.h,v 1.4 2004/05/28 18:47:51 dick Exp $
 *
 * PrivateServerPanel - Handle the private server panel.
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
 * $Log: PrivateServerPanel.h,v $
 * Revision 1.4  2004/05/28 18:47:51  dick
 * Whitespace
 *
 * Revision 1.3  2004/05/13 09:22:34  dick
 * UpdateTeamButtons() becomes UpdateButtons()
 *
 * Revision 1.2  2004/05/11 06:49:31  dick
 * Edit/Add/Change/Delete the entries in the PrivateServerList.
 *
 * Revision 1.1  2004/05/10 12:48:00  dick
 * Add a Private Server button to the main panel, which allows the user to
 * manually track servers that don't appear in meta.
 *
 * Revision 1.15  2004/02/09 18:22:48  dick
 * Lots of whitespace changes to support fixing resizable columns.
 *
 * Revision 1.14  2002/09/09 23:47:01  dick
 * Prefix button names with "Button"
 *
 * Revision 1.13  2002/09/05 16:51:27  dick
 * actionButtons is an array of function buttons created.
 *
 */

#ifndef	_PRIVATESERVERPANEL_H_
#define	_PRIVATESERVERPANEL_H_

#include "FL/Fl_Double_Window.H"
#include "types.h"
#include "xpcTypes.h"



class Fl_ServerList;
class ServerButtonsGroup;
class PrivateServerConfig;
class PrivatePanelEditButton;
class PrivatePanelEditor;

#define	PrivateServerPanelSUPERCLASS	Fl_Double_Window
class PrivateServerPanel : public PrivateServerPanelSUPERCLASS {
public:
	PrivateServerPanel(int x, int y, int w,int h,const char *l=0);
	void	SetErrMsgHandler(ErrMsgHandler _emh, void* _emhThis);
	void	FlushToIni();	// call before saving

	int		handle(int);

	void	ActionButtonHandler(PrivatePanelEditButton* b);
	void	PanelEditorCallback(PrivatePanelEditButton*);

	void	UpdateButtons();
	void	PingServers();
	void	SortByLag();

	static void		ServerButtonsCallback(int id, void* v);

public:
	PrivateServerConfig*	pscConfig;
	Fl_ServerList*			fsl;
	ServerButtonsGroup*		serverButtonsGroup;

private:
	void		ServerButtonsCallback(int id);

	ErrMsgHandler	emh;
	void*			emhThis;

	PrivatePanelEditButton*	addButton;
	PrivatePanelEditButton*	changeButton;
	PrivatePanelEditButton*	deleteButton;

	PrivatePanelEditor*		ppEditor;

};

#endif	// _PRIVATESERVERPANEL_H_
