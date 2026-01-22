/* $Id: ConnectionControlLocalCtl.h,v 1.2 2004/05/30 16:21:13 dick Exp $
 *
 * ConnectionControlLocalCtl - The Server side connection to a local XPilot control
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 2002 by
 *
 *      Dick Balaska         <dick@xpilot.org>
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
 */
/*
 *  $Log: ConnectionControlLocalCtl.h,v $
 *  Revision 1.2  2004/05/30 16:21:13  dick
 *  Whitespace
 *
 *  Revision 1.1  2002/09/09 23:38:24  dick
 *  New class to handle the server side of a LocalCtl connection.
 *
 */

#ifndef	_ConnectionControlLocalCtl_h_
#define	_ConnectionControlLocalCtl_h_

#include "ConnectionControl.h"
#include "PacketCtl.h"

#define	ConnectionControlLocalCtlSUPERCLASS	ConnectionControl
class ConnectionControlLocalCtl : public ConnectionControlLocalCtlSUPERCLASS
{
public:
	ConnectionControlLocalCtl();


protected:
	// overridden from Connection
//	virtual int	HandleSetup();

	// functions to handle each subcommand
	virtual int		ReceiveShutdown();
	virtual int		ReceiveRestartWorld();
	virtual int		ReceiveWorldInfo();

private:
	void	SendWorldInfo(World* w);

};

#endif	// _ConnectionControlLocalCtl_h_
