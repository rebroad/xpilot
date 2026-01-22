/* $Id: ScoreServer.h,v 1.11 2004/05/07 04:38:25 dick Exp $
 *
 * XPScoreServer - Who's on first?
 * The primary application object
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
 * $Log: ScoreServer.h,v $
 * Revision 1.11  2004/05/07 04:38:25  dick
 * Send rank data to the server.
 *
 * Revision 1.10  2004/04/16 21:07:59  dick
 * NetServer becomes a little UDP engine that can be plugged into different apps.
 *
 * Revision 1.9  2004/01/28 00:00:21  dick
 * Correctly determine and use GetScoreServerDirectory() for our writable data.
 *
 * Revision 1.8  2004/01/26 17:07:06  dick
 * Add StartScoreEngine(), so we can change which engine is running on the fly.
 *
 * Revision 1.7  2004/01/24 00:46:59  dick
 * Web configuration for the ScoreServer including which ScoreEngine the user has selected.
 *
 * Revision 1.6  2002/08/01 15:02:21  dick
 * Add support for cookies.
 *
 * Revision 1.5  2002/07/17 23:58:20  dick
 * SendPlayerMessage hides details from callers about what is a netClient.
 *
 * Revision 1.4  2002/07/14 19:09:05  dick
 * Whenever we're talking to the server about PCSTR nick, also mention the PlayerType.
 *
 * Revision 1.3  2002/07/09 21:56:15  dick
 * Send the player type (human,robot,tank) with the PlayerEvent
 *
 * Revision 1.2  2002/06/26 04:43:01  dick
 * PlayerScoreEvent becomes ScorePlayerEvent
 *
 * Revision 1.1  2002/06/25 05:13:13  dick
 * The main ScoreServer application object.
 *
 */

#define	APPNAME	"XPScoreServer"

#include "XMLArchive.h"
#include "ScoreServerCfg.h"
#include "ScoreEngineSelector.h"
#include "types.h"
#include "PacketCtl.h"
#include "Sockbuf.h"

class ScoreServerNetClient;
class ScoreEngine;

class ScoreServer
{
public:
	ScoreServer();
	//~ScoreServer();	// static singleton doesn't get a destructor
	void	Startup();
	void	Shutdown();
	void	LoadCfg();
	void	SaveCfg();
	String	GetScoreServerDirectory();
	void	StartScoreEngine();
	void	TimerTick();
	void	UdpSelected();

	void	ReceiveGetRanks(bool onOff);

	void	ReceiveGetScore(PCSTR nick, PlayerType pt, uint cookie);
	void	ReceiveSetScore(PCSTR nick, PlayerType pt, DFLOAT score,
							int kills, int deaths, uint cookie);
	void	ReceivePlayerEvent(PCSTR name, PlayerType pt, ScorePlayerEvent pse,
							   PCSTR real, PCSTR host, PCSTR addr, uint cookie);
	void	ReceiveScoreEvent(PCSTR killer, PlayerType ptr, DFLOAT wscore,
							  PCSTR killee, PlayerType pte, DFLOAT lscore,
							  ScoreType st);

	void	SendPlayerMessage(PCSTR message, PCSTR player = NULL, PlayerType = PlayerAny);
	void	SendPlayerRank(PCSTR player, PlayerType pt, int rank, DFLOAT rate);

	static void	ErrHandler(void* myThis, ErrMsgType emt, PCSTR ctl, ...);

	XML_Parser				xmlp;
	ScoreServerCfg			cfg;
	ScoreServerNetClient*	netClient;
	ScoreEngine*			scoreEngine;
	ScoreEngineSelector		selector;
	bool					sendRanks;

private:
	xarOPERATORS;
	xarHANDLERS;

};


extern	ScoreServer	theApp;
