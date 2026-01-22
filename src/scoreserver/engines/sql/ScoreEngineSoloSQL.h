/* $Id: ScoreEngineSoloSQL.h,v 1.1 2007/01/29 04:52:19 dick Exp $
 *
 * XPScoreServer - Who's on first?
 * ScoreEngineSoloSQL - A score server engine that extends ScoreEngine
 *                      by keeping our data in an SQL Database
 *
 *      Copyright (C) 2007 by
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
 * $Log: ScoreEngineSoloSQL.h,v $
 * Revision 1.1  2007/01/29 04:52:19  dick
 * The first SQL Engine for the ScoreServer
 *
 */



#ifndef	_SCOREENGINESOLOSQL_H_
#define	_SCOREENGINESOLOSQL_H_

#include "XMLArchive.h"
#include "../ScoreEngineBasic.h"

class ScoreEngineSoloSQLCfg;

#define	ScoreEngineSoloSQLSUPERCLASS	ScoreEngineBasic
class ScoreEngineSoloSQL : public ScoreEngineSoloSQLSUPERCLASS
{
public:
	ScoreEngineSoloSQL(ScoreEngineCfg* cfg);
	virtual ~ScoreEngineSoloSQL();

	virtual PCSTR	GetTitle() { return(title); }
	virtual PCSTR	GetDescription() { return(description); }

	static	PCSTR			name;
	static	PCSTR			title;
	static	PCSTR			description;
	static	ScoreEngineCfg*	Install();


	xarOPERATORS;
	XML_Parser				xmlp;

protected:
	xarHANDLERS;

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

private:
	ScoreEngineSoloSQLCfg*	cfg;

	void*	eng;					// private implementation data
};

#endif	// _SCOREENGINESOLOSQL_H_
