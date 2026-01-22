/* $Id: ServerOptionsNetClient.h,v 1.12 2004/05/23 23:52:07 dick Exp $
 *
 * ServerOptionsNetClient - connect a ServerOptions to a server
 *
 *      Copyright (C) 2001 by
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
 * $Log: ServerOptionsNetClient.h,v $
 * Revision 1.12  2004/05/23 23:52:07  dick
 * src/common/IniClient.Defaults.cpp
 *
 * Revision 1.11  2004/02/05 04:47:01  dick
 * Add a callback when the options are finished downloading
 *
 * Revision 1.10  2004/01/22 00:23:27  dick
 * Deadwood delete
 *
 * Revision 1.9  2002/09/11 21:30:26  dick
 * Disconnect from the server when closing the ServerPrefsWindow.
 *
 * Revision 1.8  2002/09/09 23:34:07  dick
 * Remove unused FlSetup()
 *
 * Revision 1.7  2002/07/23 17:56:04  dick
 * Pass Connected() to our subclass
 *
 * Revision 1.6  2002/07/12 15:43:29  dick
 * SetServer(PCSTR hostname, int contactPort) for when we don't have a ServerSite.
 *
 * Revision 1.5  2002/06/30 07:09:43  dick
 * SendOption is protected, not private.
 *
 * Revision 1.4  2002/06/30 06:18:47  dick
 * Make some methods private
 *
 * Revision 1.3  2002/06/23 06:11:52  dick
 * Break the FLTK dependencies out of ServerOptionsNetClient by making a subclass
 * ServerOptionsNetClientFl, because sometimes you want a ServerOptionsNetClient
 * without the Fl baggage.
 *
 * Revision 1.2  2002/06/01 20:52:33  dick
 * White space
 *
 * Revision 1.1  2002/05/29 20:36:40  jlmiller
 * Move to common, since we're now used in both control and xpwhere
 *
 * Revision 1.7  2001/08/31 21:01:58  dick
 * Use outbound reliable buffer.
 *
 * Revision 1.6  2001/08/30 07:16:53  dick
 * Send and receive options between the server and xpilotedit.
 *
 * Revision 1.5  2001/08/26 10:43:13  dick
 * Mark the time we last spoke to the server, so we can KEEPALIVE if its been
 * awhile.  ServerOptions* goes in ControlClient.
 *
 * Revision 1.4  2001/08/13 03:03:43  dick
 * SockbufClient becomes ControlClient
 *
 * Revision 1.3  2001/08/12 11:19:31  dick
 * A little bit closer to getting the control to work...
 *
 * Revision 1.2  2001/08/11 13:18:42  dick
 * work on ServerOptionsNetClient
 *
 * Revision 1.1  2001/08/07 20:56:01  dick
 * Connect a ServerOptions to a server.
 *
 */

#ifndef	_SERVEROPTIONSNETCLIENT_H_
#define	_SERVEROPTIONSNETCLIENT_H_

//#include "ServerOptions.h"
#include "ControlClient.h"

class ServerPrefsWindow;
class ServerSite;




#define	ServerOptionsNetClientSUPERCLASS	ControlClient
class ServerOptionsNetClient : public ServerOptionsNetClientSUPERCLASS
{
public:
	// Initialization
	ServerOptionsNetClient();
	virtual ~ServerOptionsNetClient();

	void	SetServerOptions(ServerOptions* _so);
	void	SetServer(PCSTR hostname, PCSTR ip, int contactPort);
	void	SetServer(const ServerSite& ss);
	void	operator=(const ServerSite& ss)		{ SetServer(ss); };
	//void	SetWindow(ServerPrefsWindow* _spw);

	ServerOptions*	GetServerOptions();


	// override from superclass
protected:
	virtual bool	Disconnect();
	virtual void	Listener(ServerOption* so);
	virtual	int		TimerTick();
	virtual void	Connected() {};

	// Control (PKT_CTL) receive overrides
protected:
	virtual	int		ReceiveOptionCount();
	virtual	int		ReceiveSetOption();

	virtual	void	OptionsDownloaded();	// overridable callback

protected:
	bool		Download();			// Download ServerOptions
	int			SendOption(ServerOption* so);

	enum { Idle, WaitCount, WaitOptions}	state;

	short		numOptions;
	int			curOptions;
	int			partialReceive;
	int			optsRequested;
	bool		noSend;			// don't send this change back (it was an inbound change)

private:
	void		InitListeners();
	static void	HandleListener(void* myThis, ServerOption* so);

	void		RequestOptions();
};

inline	ServerOptions*	ServerOptionsNetClient::GetServerOptions() { return(sos); }
inline	void			ServerOptionsNetClient::OptionsDownloaded() { return; }

#endif		// _SERVEROPTIONSNETCLIENT_H_
