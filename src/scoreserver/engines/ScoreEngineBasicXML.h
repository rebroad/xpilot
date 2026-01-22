/* $Id: ScoreEngineBasicXML.h,v 1.3 2004/01/30 16:02:49 dick Exp $
 *
 * XPScoreServer - Who's on first?
 * ScoreEngineBasicXML - A simple score server that extends ScoreEngineBasic
 *                       by keeping our data in an XML file.
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
 * $Log: ScoreEngineBasicXML.h,v $
 * Revision 1.3  2004/01/30 16:02:49  dick
 * Configure the scoreserver via the web interface.
 * Break out the engine configuration to it's own object; we don't want
 * to have the baggage of a big unused score engine loaded in memory if we
 * really are only interested in it's name and description.
 *
 * Revision 1.2  2004/01/24 00:46:59  dick
 * Web configuration for the ScoreServer including which ScoreEngine the user has selected.
 *
 * Revision 1.1  2002/06/25 05:32:53  dick
 * This Score Engine inherits from ScoreEngineBasic and provides persistence
 * of data via XML.
 *
 */



#ifndef	_SCOREENGINEBASICXML_H_
#define	_SCOREENGINEBASICXML_H_

#include "XMLArchive.h"
#include "ScoreEngineBasic.h"

class ScoreEngineBasicXMLCfg;

#define	ScoreEngineBasicXMLSUPERCLASS	ScoreEngineBasic
class ScoreEngineBasicXML : public ScoreEngineBasicXMLSUPERCLASS
{
public:
	ScoreEngineBasicXML(ScoreEngineCfg* cfg);
	virtual ~ScoreEngineBasicXML();

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
	void	LoadData();
	void	SaveData();

	time_t	dirtyTime;		// when we went dirty
	ScoreEngineBasicXMLCfg*	cfg;

};

#endif	// _SCOREENGINEBASICXML_H_
