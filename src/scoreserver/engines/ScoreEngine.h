/* $Id: ScoreEngine.h,v 1.11 2004/05/07 04:38:26 dick Exp $
 *
 * XPScoreServer - Who's on first?
 * ScoreEngine - The base class for different scoring techniques
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
 * $Log: ScoreEngine.h,v $
 * Revision 1.11  2004/05/07 04:38:26  dick
 * Send rank data to the server.
 *
 * Revision 1.10  2004/01/30 16:02:49  dick
 * Configure the scoreserver via the web interface.
 * Break out the engine configuration to it's own object; we don't want
 * to have the baggage of a big unused score engine loaded in memory if we
 * really are only interested in it's name and description.
 *
 * Revision 1.9  2004/01/24 00:46:59  dick
 * Web configuration for the ScoreServer including which ScoreEngine the user has selected.
 *
 * Revision 1.8  2002/09/06 19:41:07  dick
 * Add lastVisit as a player attribute
 *
 * Revision 1.7  2002/08/01 15:02:21  dick
 * Add support for cookies.
 *
 * Revision 1.6  2002/07/17 23:59:47  dick
 * SEPlayerList derives from ObjList so we can add some specific sorting.
 *
 * Revision 1.5  2002/07/14 19:09:05  dick
 * Whenever we're talking to the server about PCSTR nick, also mention the PlayerType.
 *
 * Revision 1.4  2002/07/09 21:57:27  dick
 * xpprintf needs showtime()
 *
 * Revision 1.3  2002/06/30 06:14:48  dick
 * Add sorting.
 *
 * Revision 1.2  2002/06/26 04:35:45  dick
 * PlayerScoreEvent becomes ScorePlayerEvent
 *
 * Revision 1.1  2002/06/25 05:27:20  dick
 * The ScoreEngine Base class.  This file interfaces the ScoreServer to the user's
 * engine of choice.  I think i might move this class into the scoreserver main
 * directory.
 *
 */



#ifndef	_SCOREENGINE_H_
#define	_SCOREENGINE_H_

#include "cstring.h"
#include "Obj.h"
#include "types.h"
#include "PacketCtl.h"
#include "XMLArchive.h"


extern PCSTR	s_type;

enum PlayerFlags {
	Authenticated = 0x01,
};

// Define one player entry into the database
class SEPlayer : public ObjSortable
{
  public:
	SEPlayer() { cookie = 0; isPlaying = false; flags = (PlayerFlags)0; }

	String	GetName();

  public:
	virtual	bool	Sort(ObjSortable* other, int priSort = 0, int secSort = 0);

  public:
	PlayerType	playerType;
	uint		cookie;
	uint		flags;
	// Not persistent
	bool	isPlaying;		// is he currently playing?
};

enum {
	PlayerSortByRank,
	PlayerSortByNick,
	PlayerSortByScore,
	PlayerSortByKDRatio,
	PlayerSortByKills,
	PlayerSortByDeaths,
	PlayerSortBySessions,
	PlayerSortByPlaytime,
	PlayerSortByLastVisit
};

class SEPlayerList : public ObjList
{
  public:
	ObjSortable* FindSortable(PCSTR s, PlayerType pt) const;

};

class HttpScore;

class ScoreEngineCfg;

class ScoreEngine
{
  public:
	ScoreEngine(ScoreEngineCfg* cfg);
	virtual ~ScoreEngine();

// General ScoreServer functions
	virtual void	Startup();
	virtual	void	Shutdown();
	virtual	void	TimerTick();

	virtual PCSTR	GetTitle() { return(title); }
	virtual PCSTR	GetDescription() { return(description); }

	static	PCSTR			name;
	static	PCSTR			title;
	static	PCSTR			description;
	static	ScoreEngineCfg*	Install();

	uint	MakeNewCookie();

// Web score engine administration functions
	virtual	bool OnAdminPrintTop(HttpScore* http);
	virtual	bool OnAdminPrintTable(HttpScore* http);
	virtual	bool OnAdminSubmit(HttpScore* http);

// Network functions
	virtual bool GetInitialScore(PCSTR nick, PlayerType pt,  uint& cookie,
								 DFLOAT& score, int& kills, int& deaths);
	virtual bool SetPlayerValues(PCSTR nick, PlayerType pt, uint cookie,
								 DFLOAT  score, int  kills, int  deaths);
	virtual void ReceivePlayerEvent(PCSTR name, PlayerType pt,ScorePlayerEvent pse,
									PCSTR real, PCSTR host, PCSTR addr, uint cookie) {};
	virtual void ReceiveScoreEvent(PCSTR killer, PlayerType ptr, DFLOAT wscore,
								   PCSTR killee, PlayerType pte, DFLOAT score,
								   ScoreType st) {};
	virtual	void ReceiveGetRanks(bool onOff) {};

	String GetPlayerValue(SEPlayer* pl, const ObjList& opts);

	ObjList	playerList;

  protected:
	virtual String GetValue(SEPlayer* pl, PCSTR type, const ObjList& opts);

private:
	ScoreEngineCfg*		cfg;

};

#endif	// _SCOREENGINE_H_
