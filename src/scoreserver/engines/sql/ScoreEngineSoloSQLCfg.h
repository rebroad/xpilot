/* $Id: ScoreEngineSoloSQLCfg.h,v 1.1 2007/01/29 04:52:19 dick Exp $
 *
 * XPScoreServer - Who's on first?
 * ScoreEngineSoloSQLCfg - Configure the SQL scoring engine.
 *
 *      Copyright (C) 2004 by
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
 * $Log: ScoreEngineSoloSQLCfg.h,v $
 * Revision 1.1  2007/01/29 04:52:19  dick
 * The first SQL Engine for the ScoreServer
 *
 */

#ifndef	_SCOREENGINEBASICSQLCFG_H_
#define	_SCOREENGINEBASICSQLCFG_H_

#include "../ScoreEngineBasicCfg.h"

#define	ScoreEngineSoloSQLCfgSUPERCLASS	ScoreEngineBasicCfg
class ScoreEngineSoloSQLCfg : public ScoreEngineSoloSQLCfgSUPERCLASS
{
public:
	ScoreEngineSoloSQLCfg();

	xarOPERATORS;
	virtual void	XMLOutVirtual(XMLArchive& xar);
protected:
	xarHANDLERS;

public:
	bool	fullStats;

};


#endif	// _SCOREENGINEBASICXMLCFG_H_
