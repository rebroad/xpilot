/* $Id: XPilotControl.h,v 1.36 2004/05/10 12:48:00 dick Exp $
 *
 * XPilot5.  Manage XPilot activities.  Copyright (C) 2001 by
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
 * $Log: XPilotControl.h,v $
 * Revision 1.36  2004/05/10 12:48:00  dick
 * Add a Private Server button to the main panel, which allows the user to
 * manually track servers that don't appear in meta.
 *
 */

#ifndef	_XPILOTCONTROL_H_
#define	_XPILOTCONTROL_H_

#include "xpcTypes.h"

extern	void	StartLocalServer();
extern	void	StartScoreServer();
extern	void	BrowseScoreServer();
extern	void	AdminServer(PCSTR serverName, PCSTR serverIP, int port);
extern	void	PlayGame(PCSTR serverIP, int port, int team = -1);

#endif// _XPILOTCONTROL_H_
