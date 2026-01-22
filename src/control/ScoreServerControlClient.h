/* $Id: ScoreServerControlClient.h,v 1.1 2004/04/21 23:24:34 dick Exp $
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
 * $Log: ScoreServerControlClient.h,v $
 * Revision 1.1  2004/04/21 23:24:34  dick
 * The client that controls the ScoreServer
 *
 */

#ifndef	_SCORESERVERCONTROLCLIENT_H_
#define	_SCORESERVERCONTROLCLIENT_H_

#include "ControlClient.h"

class LocalServerPanelAdvanced;

#define	ScoreServerControlClientSUPERCLASS	ControlClient
class ScoreServerControlClient : public ScoreServerControlClientSUPERCLASS
{
public:
	ScoreServerControlClient();
	virtual ~ScoreServerControlClient();
	void	FlSetup();
	void	SetPanel(LocalServerPanelAdvanced* _lsp);

	static void RbufSelectedFl(int, void* myThis);	// Fl callback when rbuf has data
	static void	TimerTickFl(void* myThis);			// Fl timer callback

	bool	SendShutdown();

	// virtual Overrides from parent class
protected:
	virtual void	Connected();		// callback when a connection is made
	virtual bool	Disconnect();
	virtual	int		TimerTick();


private:
	LocalServerPanelAdvanced*	lspa;	// back pointer to our parent
};

inline void ScoreServerControlClient::SetPanel(LocalServerPanelAdvanced* _lsp) { lspa = _lsp; };

#endif		// _SCORESERVERCONTROLCLIENT_H_
