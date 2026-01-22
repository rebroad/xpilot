/* $Id: ConnectionLocalCtl.h,v 1.3 2004/05/07 04:38:25 dick Exp $
 *
 * The Server side connection to a local XPilot control
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2001 by
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
 *  $Log: ConnectionLocalCtl.h,v $
 *  Revision 1.3  2004/05/07 04:38:25  dick
 *  Send rank data to the server.
 *
 *  Revision 1.2  2004/04/22 05:58:10  dick
 *  Add an override for ReceiveUndefined() just so gcc will do a pointer compare.
 *
 *  Revision 1.1  2004/04/22 00:21:09  dick
 *  The Server side connection to a local XPilot control.
 *
 */

#ifndef	_ConnectionCtl_h_
#define	_ConnectionCtl_h_

#include "Connection.h"
#include "PacketCtl.h"

#define	ConnectionLocalCtlSUPERCLASS	Connection
class ConnectionLocalCtl : public ConnectionLocalCtlSUPERCLASS
{
public:
	ConnectionLocalCtl();
	bool	IsActive();


	// functions to handle each subcommand
			int		ReceiveShutdown();

	virtual	int		ReceiveUndefined();

protected:
	// overridden from Connection
	virtual int	HandleSetup();

	// Receive a PKT_CTL command from the client
	virtual int		ReceiveCtl();
//	virtual int		ReceiveRestartWorld();
//	virtual int		ReceiveWorldInfo();

private:
//	void	SendWorldInfo(World* w);

};

extern void InitConnectionLocalCtl();

#endif	// _ConnectionCtl_h_
