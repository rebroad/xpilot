/* $Id: ServerOptionsNetClientFl.h,v 1.5 2004/05/22 15:18:58 dick Exp $
 *
 * ServerOptionsNetClientFl - connect a ServerOptionsNetClient to a server via FLTK
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
 * $Log: ServerOptionsNetClientFl.h,v $
 * Revision 1.5  2004/05/22 15:18:58  dick
 * Set the title of the ServerPrefsWindow to the name of the server
 *
 * Revision 1.4  2002/09/11 21:30:26  dick
 * Disconnect from the server when closing the ServerPrefsWindow.
 *
 * Revision 1.3  2002/09/09 23:35:37  dick
 * Remove unused FlSetup()
 *
 * Revision 1.2  2002/07/23 17:56:38  dick
 * When Connected(), save the password to our client password file.
 *
 * Revision 1.1  2002/06/23 06:11:52  dick
 * Break the FLTK dependencies out of ServerOptionsNetClient by making a subclass
 * ServerOptionsNetClientFl, because sometimes you want a ServerOptionsNetClient
 * without the Fl baggage.
 *
 */

#ifndef	_SERVEROPTIONSNETCLIENTFL_H_
#define	_SERVEROPTIONSNETCLIENTFL_H_

#include "ServerOptionsNetClient.h"

class ServerPrefsWindow;


//const int receive_window_size = 2;


#define	ServerOptionsNetClientFlSUPERCLASS	ServerOptionsNetClient
class ServerOptionsNetClientFl : public ServerOptionsNetClientFlSUPERCLASS
{
public:
	// Initialization
	ServerOptionsNetClientFl();
	virtual ~ServerOptionsNetClientFl();

	static void RbufSelectedFl(int, void* myThis);	// Fl callback when rbuf has data
	static void	TimerTickFl(void* myThis);	// Fl timer callback
	static void	ErrHandler(void* myThis, ErrMsgType emt, PCSTR ctl, ...);

	void	SetWindow(ServerPrefsWindow* _spw);

	// virtual Overrides from parent class
protected:
	virtual bool	Disconnect();
	virtual void	Listener(ServerOption* so);
	virtual	int		TimerTick();
	virtual void	Connected();
	virtual	void	OptionsDownloaded();	// callback after all options received

private:
	ServerPrefsWindow*	spw;

};

inline void ServerOptionsNetClientFl::SetWindow(ServerPrefsWindow* _spw) { spw = _spw; };
#endif		// _SERVEROPTIONSNETCLIENT_H_
