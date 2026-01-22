/* $Id: ScoreEngineBasic.h,v 1.13 2004/05/07 04:38:26 dick Exp $
 *
 * XPScoreServer - Who's on first?
 * ScoreEngineBasic - A simple score server Keeps Score/Kills/Deaths/Playtime
 *                    in an XML file.
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
 * $Log: ScoreEngineBasic.h,v $
 * Revision 1.13  2004/05/07 04:38:26  dick
 * Send rank data to the server.
 *
 * Revision 1.12  2004/01/30 16:02:49  dick
 * Configure the scoreserver via the web interface.
 * Break out the engine configuration to it's own object; we don't want
 * to have the baggage of a big unused score engine loaded in memory if we
 * really are only interested in it's name and description.
 *
 * Revision 1.11  2004/01/28 00:00:21  dick
 * Correctly determine and use GetScoreServerDirectory() for our writable data.
 *
 * Revision 1.10  2004/01/24 00:46:59  dick
 * Web configuration for the ScoreServer including which ScoreEngine the user has selected.
 *
 * Revision 1.9  2002/09/06 19:41:07  dick
 * Add lastVisit as a player attribute
 *
 * Revision 1.8  2002/08/01 15:02:21  dick
 * Add support for cookies.
 *
 * Revision 1.7  2002/07/18 00:00:26  dick
 * Check if a player's rank has changed and send a message to the server if it has.
 *
 * Revision 1.6  2002/07/14 19:09:05  dick
 * Whenever we're talking to the server about PCSTR nick, also mention the PlayerType.
 *
 * Revision 1.5  2002/07/09 21:57:02  dick
 * Send the player type (human,robot,tank) with the PlayerEvent
 *
 * Revision 1.4  2002/07/08 18:16:18  dick
 * Make public as a field the rating that was calculated.
 *
 * Revision 1.3  2002/06/30 06:14:48  dick
 * Add sorting.
 *
 * Revision 1.2  2002/06/26 04:35:45  dick
 * PlayerScoreEvent becomes ScorePlayerEvent
 *
 * Revision 1.1  2002/06/25 05:28:33  dick
 * A Basic Score Engine that tracks total score, kills, deaths and playing time.
 *
 */



#ifndef	_SCOREENGINEBASIC_H_
#define	_SCOREENGINEBASIC_H_

#include "ScoreEngine.h"

const int SEMaxPlayers = 1000;	// artificial limit so we don't fill the guy's computer

// Define one player entry into the database
#define	SEBPlayerSUPERCLASS	SEPlayer
class SEBPlayer : public SEBPlayerSUPERCLASS
{
public:
	SEBPlayer() { score = 0; kills = 0; deaths = 0; sessions = 0; playTime = 0;
				  lastVisit = 0; isPlaying = false; playerType = PlayerHuman; }
	virtual	bool	Sort(ObjSortable* other, int priSort = 0, int secSort = 0);
	virtual	double	GetRating();

	DFLOAT	score;
	int		kills;
	int		deaths;
	int		sessions;
	time_t	playTime;
	time_t	lastVisit;		// actually last event

	// Not persistent
	time_t	startTime;		// time he started this session
};

class ScoreEngineCfg;
class ScoreEngineBasicCfg;

#define	ScoreEngineBasicSUPERCLASS	ScoreEngine
class ScoreEngineBasic : public ScoreEngineBasicSUPERCLASS
{
public:
	ScoreEngineBasic(ScoreEngineCfg* cfg);
	virtual ~ScoreEngineBasic();

	virtual PCSTR	GetTitle() { return(title); }
	virtual PCSTR	GetDescription() { return(description); }

	static	PCSTR			name;
	static	PCSTR			title;
	static	PCSTR			description;
	static	ScoreEngineCfg*	Install();


	SEBPlayer* ScoreEngineBasic::FindPlayer(PCSTR nick, PlayerType pt);

// General ScoreServer functions
protected:
	virtual void Startup();
	virtual	void Shutdown();
	virtual void TimerTick();

// Web score engine administration functions
	virtual	bool OnAdminPrintTop(HttpScore* http);
	virtual	bool OnAdminPrintTable(HttpScore* http);
	virtual	bool OnAdminSubmit(HttpScore* http);

// Network functions
protected:
	virtual bool GetInitialScore(PCSTR nick, PlayerType pt, uint& cookie,
								 DFLOAT& score, int& kills, int& deaths);
	virtual bool SetPlayerValues(PCSTR nick, PlayerType pt, uint cookie,
								 DFLOAT  score, int  kills, int  deaths);
	virtual void ReceivePlayerEvent(PCSTR name, PlayerType pt, ScorePlayerEvent spe,
									PCSTR real, PCSTR host, PCSTR addr, uint cookie);
	virtual void ReceiveScoreEvent(PCSTR killer, PlayerType ptr, DFLOAT wscore,
								   PCSTR killee, PlayerType pte, DFLOAT lscore,
								   ScoreType st);
	virtual void ReceiveGetRanks(bool onOff);

	// tell everyone if a player's rank has changed
	void	SendChangedRankMessage(PCSTR nick, PlayerType pt);


	void	SendAllPlayingRanks();		// send the ranks of eveyone playing
	void	SendRank(SEBPlayer* pl);	// send one player's rank to the server.

// Http accessor functions
protected:
	virtual String GetValue(SEPlayer* pl, PCSTR type, const ObjList& opts);

	SEPlayerList	rankList;

private:
	ScoreEngineBasicCfg*	cfg;

};

#endif	// _SCOREENGINEBASE_H_
