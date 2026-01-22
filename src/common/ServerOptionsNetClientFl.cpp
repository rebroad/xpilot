/* $Id: ServerOptionsNetClientFl.cpp,v 1.8 2004/05/22 15:18:58 dick Exp $
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
 * $Log: ServerOptionsNetClientFl.cpp,v $
 * Revision 1.8  2004/05/22 15:18:58  dick
 * Set the title of the ServerPrefsWindow to the name of the server
 *
 * Revision 1.7  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.6  2002/09/11 21:30:26  dick
 * Disconnect from the server when closing the ServerPrefsWindow.
 *
 * Revision 1.5  2002/09/09 23:35:37  dick
 * Remove unused FlSetup()
 *
 * Revision 1.4  2002/07/23 17:56:38  dick
 * When Connected(), save the password to our client password file.
 *
 * Revision 1.3  2002/07/12 15:55:02  dick
 * connectLock needs a type
 *
 * Revision 1.2  2002/07/12 15:41:38  dick
 * Put a lock around Disconnect() to keep from recursively calling Disconnect().
 *
 * Revision 1.1  2002/06/23 06:11:52  dick
 * Break the FLTK dependencies out of ServerOptionsNetClient by making a subclass
 * ServerOptionsNetClientFl, because sometimes you want a ServerOptionsNetClient
 * without the Fl baggage.
 *
 */
#include "StdAfx.h"

#include "xpprintf.h"

#include "ServerOptionsNetClientFl.h"
#include "ServerPrefsWindow.h"
#include "password.h"

#include <string.h>

///////////////////////////////////////////////////////////////////////////////
ServerOptionsNetClientFl::ServerOptionsNetClientFl()
{
	spw = NULL;
	SetErrMsgHandler(&ServerOptionsNetClientFl::ErrHandler, this);
	SetControlType("serverctl");
}

///////////////////////////////////////////////////////////////////////////////
ServerOptionsNetClientFl::~ServerOptionsNetClientFl()
{
	if (IsConnected())
		Disconnect();
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionsNetClientFl::Listener(ServerOption* so)
{
	String	server = sos->connectedToName->GetString();
	int		port = sos->connectedToPort->GetInt();
	String	password = sos->connectedToPassword->GetString();

	static	bool connectLock = false;	// Calling Disconnect() causes the Listener to fire
	if (connectLock)					// protect the stack overflow :) by ignoring
		return;							// the listener event if we caused it.

	if (so == sos->isConnected)
	{
		if (sos->isConnected->GetBool())
		{
			spw->SetStatus("Contacting %s:%d", (PCSTR)server, port);

			if (!Connect(server, port, password))			// Go!
			{
				connectLock = true;
				Disconnect();
				connectLock = false;
				return;
			}

			Fl::add_fd(rbuf.sock.fd, &ServerOptionsNetClientFl::RbufSelectedFl, this);
			Fl::add_timeout(TIMER_INTERVAL, &ServerOptionsNetClientFl::TimerTickFl, this);
			spw->SetTitle(server);
			if (!Download())
			{
				connectLock = true;
				Disconnect();
				connectLock = false;
				return;
			}

		}
		else
		{
			sos->isConnected->SetLabel("Connect to server:");
			connectLock = true;
			Disconnect();
				connectLock = false;
//			spw->SetStatus("Disconnected");
		}
	}
	// named option?  we should send it, unless it is not ours (masked by noSend)
	// and we are connected
	else if (connected && strlen(so->name) && !noSend)
	{
		SendOption(so);
	}

}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionsNetClientFl::RbufSelectedFl(int /* fd */, void* myThis)
{
	int		ret;
	ControlClient* cc = (ControlClient*) myThis;
	xpprintf("*");
	ret = cc->RbufSelected();
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionsNetClientFl::TimerTickFl(void* myThis)
{
	int		ret;
	ControlClient* cc = (ControlClient*) myThis;
	ret = cc->TimerTick();
}

///////////////////////////////////////////////////////////////////////////////
#if 0
void ServerOptionsNetClientFl::FlSetup()
{
	Fl::add_fd(rbuf.sock.fd, &ServerOptionsNetClientFl::RbufSelectedFl, this);
	Fl::add_timeout(TIMER_INTERVAL, &ServerOptionsNetClientFl::TimerTickFl, this);
}
#endif
///////////////////////////////////////////////////////////////////////////////
bool ServerOptionsNetClientFl::Disconnect()
{
	Fl::remove_timeout(&ServerOptionsNetClientFl::TimerTickFl, this);
	Fl::remove_fd(rbuf.sock.fd);
	return(ServerOptionsNetClientFlSUPERCLASS::Disconnect());
}

///////////////////////////////////////////////////////////////////////////////
int ServerOptionsNetClientFl::TimerTick()
{
	Fl::repeat_timeout(TIMER_INTERVAL, &ServerOptionsNetClientFl::TimerTickFl, this);
	return(ServerOptionsNetClientFlSUPERCLASS::TimerTick());
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionsNetClientFl::ErrHandler(void* myThis, ErrMsgType emt, PCSTR ctl, ...)
{
	ServerOptionsNetClientFl* sonc = (ServerOptionsNetClientFl*)myThis;
    char szBuffer[512];
	va_list marker;

    // Figure through the extra arguments.
    va_start(marker, ctl);
    vsprintf(szBuffer, ctl, marker);
    va_end(marker);

	sonc->spw->SetStatus(szBuffer);
}

///////////////////////////////////////////////////////////////////////////////
// If we have successfully connected, then save our password to a file
void ServerOptionsNetClientFl::Connected()
{
	String	e;
	SetPassword(sos->connectedToName->GetString(), sos->connectedToPort->GetInt(),
		sos->connectedToPassword->GetString(), e);
}

///////////////////////////////////////////////////////////////////////////////
// called after all options received
void ServerOptionsNetClientFl::OptionsDownloaded()
{
	spw->SetTitle(sos->serverHost->GetString());
}
