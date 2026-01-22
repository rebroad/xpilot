/* $Id: InetServerPanel.h,v 1.1 2004/05/13 09:17:08 dick Exp $
 *
 * InetServerPanel - Display the XPilot servers as reported by meta,
 *
 * XPilot5 - Manage all local XPilot activity.  Copyright (C) 2001 by
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
 * $Log: InetServerPanel.h,v $
 * Revision 1.1  2004/05/13 09:17:08  dick
 * XPWhereWindow becomes InetServerPanel, (which matches LocalServerPanel and PrivateServerPanel).
 * XPWhereConfig becomes InetServerConfig.
 *
 * Revision 1.17  2004/05/10 17:06:55  dick
 * Get rid of "type", which was supposed to be a switch between different configs.
 * Instead use different subclasses for the config.
 *
 * Revision 1.16  2004/05/10 12:45:45  dick
 * Move the buttons at the top of XPWhereWindow to their own class ServerButtonsGroup
 * so we can use them in other lists.
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

#ifndef	_INETSERVERPANEL_H_
#define	_INETSERVERPANEL_H_

#include "FL/Fl_Double_Window.H"
#include "InetServerConfig.h"
#include "Fl_ServerList.h"
#include "types.h"
#include "xpcTypes.h"

class ServerButtonsGroup;

#define	InetServerPanelSUPERCLASS	Fl_Double_Window
class InetServerPanel : public InetServerPanelSUPERCLASS {
public:
	InetServerPanel(int x, int y, int w,int h,const char *l=0);
	void	SetErrMsgHandler(ErrMsgHandler _emh, void* _emhThis);

	virtual int		handle(int);

	void	UpdateButtons();
	void	PingServers();
	void	SortByLag();

	static void ServerButtonsCallback(int id, void* v);
	void		ServerButtonsCallback(int id);
public:
	InetServerConfig*	isConfig;
	Fl_ServerList*	fsl;
	ServerButtonsGroup*	serverButtonsGroup;

private:
	ErrMsgHandler	emh;
	void*			emhThis;

};

#endif	// _INETSERVERPANEL_H_
