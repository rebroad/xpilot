/* $Id: LocalServerControlClient.h,v 1.1 2002/09/09 23:46:09 dick Exp $
 *
 * A network client that controls the local server.
 *
 *      Copyright (C) 2002 by
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
 * $Log: LocalServerControlClient.h,v $
 * Revision 1.1  2002/09/09 23:46:09  dick
 * New LocalServerControlClient handles the client side connection of a LocalCtl.
 *
 */

#ifndef	_LocalServerControlClient_h_
#define	_LocalServerControlClient_h_

#include "ControlClient.h"

class LocalServerPanel;

#define	LocalServerControlClientSUPERCLASS	ControlClient
class LocalServerControlClient : public LocalServerControlClientSUPERCLASS
{
public:
	LocalServerControlClient();
	virtual ~LocalServerControlClient();
	void	FlSetup();
	void	SetPanel(LocalServerPanel* _lsp);

	static void RbufSelectedFl(int, void* myThis);	// Fl callback when rbuf has data
	static void	TimerTickFl(void* myThis);			// Fl timer callback

	bool	SendShutdown();
	bool	SendRestartWorld(int world);
	bool	SendRequestWorldInfo();

	// virtual Overrides from parent class
protected:
	virtual void	Connected();		// callback when a connection is made
	virtual bool	Disconnect();
	virtual	int		TimerTick();

	virtual int		ReceiveWorldInfo();

private:
	LocalServerPanel*	lsp;	// back pointer to our parent
};

inline void LocalServerControlClient::SetPanel(LocalServerPanel* _lsp) { lsp = _lsp; };

#endif		// _LocalServerControlClient_h_
