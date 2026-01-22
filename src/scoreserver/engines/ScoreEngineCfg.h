/* $Id: ScoreEngineCfg.h,v 1.1 2004/01/30 16:02:49 dick Exp $
 *
 * XPScoreServer - Who's on first?
 * ScoreEngineCfg - Config for the Root Score Engine
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
 * $Log: ScoreEngineCfg.h,v $
 * Revision 1.1  2004/01/30 16:02:49  dick
 * Configure the scoreserver via the web interface.
 * Break out the engine configuration to it's own object; we don't want
 * to have the baggage of a big unused score engine loaded in memory if we
 * really are only interested in it's name and description.
 *
 */

#ifndef	_SCOREENGINECFG_H_
#define	_SCOREENGINECFG_H_

// A definition for ScoreEngine
class ScoreEngineCfg : public Obj
{
  public:
	PCSTR	title;
	PCSTR	description;

	xarOPERATORS;
	virtual void	XMLOutVirtual(XMLArchive& xar);

  protected:
	xarHANDLERS;

};

#endif	// _SCOREENGINECFG_H_
