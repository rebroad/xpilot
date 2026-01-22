/* $Id: ScoreEngineSelector.h,v 1.4 2004/07/07 19:29:34 dick Exp $
 *
 * XPScoreServer - Who's on first?
 * ScoreEngineSelector - Create a ScoreEngine based on which one we want
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
 * $Log: ScoreEngineSelector.h,v $
 * Revision 1.4  2004/07/07 19:29:34  dick
 * Delete dead secfg
 *
 * Revision 1.3  2004/01/30 16:02:49  dick
 * Configure the scoreserver via the web interface.
 * Break out the engine configuration to it's own object; we don't want
 * to have the baggage of a big unused score engine loaded in memory if we
 * really are only interested in it's name and description.
 *
 * Revision 1.2  2004/01/24 00:46:59  dick
 * Web configuration for the ScoreServer including which ScoreEngine the user has selected.
 *
 * Revision 1.1  2002/06/25 05:12:26  dick
 * Select and install the ScoreEngine as configured by the user.
 *
 */

#ifndef	_SCOREENGINESELECTOR_H_
#define	_SCOREENGINESELECTOR_H_

#include "Obj.h"
#include "XMLArchive.h"

class ScoreEngine;
class ScoreEngineCfg;

class ScoreEngineSelector : public Obj
{
public:
	ScoreEngineSelector();
	virtual ~ScoreEngineSelector();

	void			Init();

	xarOPERATORS;

	ScoreEngine*	Select(PCSTR engineName);
	ScoreEngineCfg*	GetSelectedSECfg();
	ScoreEngineCfg*	Find(PCSTR _name);

	ObjList			list;	// list of ScoreEngineDefs

protected:
	xarHANDLERS;

private:
	ScoreEngineCfg*	selectedSECfg;


};

#endif	// _SCOREENGINESELECTOR_H_
