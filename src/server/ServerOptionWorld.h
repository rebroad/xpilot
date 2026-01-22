/* $Id: ServerOptionWorld.h,v 1.9 2004/05/20 21:45:47 dick Exp $
 *
 * Server side options as defined by the server world.
 * which has additional tuner functions over generic ServerOptions
 *
 *      Copyright (C) 2001 by
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
 * $Log: ServerOptionWorld.h,v $
 * Revision 1.9  2004/05/20 21:45:47  dick
 * TunerShowLocks() tunes the showLocks attribute.
 *
 * Revision 1.8  2004/04/26 07:07:14  dick
 * Define a list of pages that can be displayed on the ScoreTable in the client.
 * Each page can be controlled by the server (and the server admin tool).
 *
 * Revision 1.7  2004/01/12 05:23:06  dick
 * Add TunerPauseScoreReduce() TunerAllowViewing and TunerReportToMetaServer
 *
 * Revision 1.6  2004/01/09 17:28:52  dick
 * Add TunerFirewallPortList to activate changes.
 *
 * Revision 1.5  2002/07/08 06:18:55  dick
 * TunerSaveTuned() causes the options that have been tuned to be saved
 * to a file.
 *
 * Revision 1.4  2002/05/18 20:55:38  dick
 * Update to XPilot-4.5.1
 *
 * Revision 1.3  2001/09/03 00:51:49  dick
 * Tuners cleanup.
 *
 * Revision 1.2  2001/08/31 20:58:13  dick
 * Server needs to be able to receive reliable packets as well as send them.
 * Use the new emh error handling.
 *
 * Revision 1.1  2001/08/05 08:05:36  dick
 * Add Tuners to the server.  ComputeSensorRange(), AddTank(), RemoveTank()
 * become members of Player.  ComputeGravity*() become members of World.
 *
 */

#ifndef	_SERVEROPTIONWORLD_H_
#define	_SERVEROPTIONWORLD_H_

#include "ServerOption.h"

class ServerOptionWorld : public ServerOption
{
public:
	ServerOptionWorld(ServerOptions* _parent,
				 PCSTR _name, PCSTR _altname, PCSTR _label,
				 int _length, ValType _valType,
				 PCSTR _defval,
				 int _row, int _col, int _sheet, int _visibleto,
				 TunerFunc	_tuner,
				 PCSTR _help = NULL);
	int			Tune(PCSTR value);
	void DoTuner();

	void		TunerNone();
	void		TunerDummy();

	void		TunerGravity();
	void		TunerInitialItems();
	void		TunerMiscItemLimits();

	void		TunerPlock();
	void		TunerShotsMax();
	void		TunerShipMass();
	void		TunerBallMass();
	void		TunerMaxRobots();
	void		TunerMinRobots();
	void		TunerPlayerShielding();
	void		TunerPlayerStartsShielded();
	void		TunerWorldLives();
	void		TunerCannonSmartness();
	void		TunerTeamCannons();
	void		TunerCannonsUseItems();
	void		TunerWormTime();
	void		TunerModifiers();
	void		TunerMineLife();
	void		TunerMissileLife();
	void		TunerGameDuration();
	void		TunerRaceLaps();
	void		TunerAsteroidProb();
	void		TunerBounce();
	void		TunerRules();
	void		TunerPauseScoreReduce();
	void		TunerAllowViewing();
	void		TunerAllowAlliances();
	void		TunerAnnounceAlliances();
	void		TunerShowLocks();
	void		TunerFirewallPortList();
	void		TunerReportToMetaServer();
	void		TunerScoreTable();

	void		TunerSaveTuned();

	// These are broken! (
	void		TuneRobotRealName();
	void		TuneRobotHostName();
	void		TuneTankRealName();
	void		TuneTankHostName();

private:
	bool		MakeVarExist(String& error);

};


#endif		// _SERVEROPTIONWORLD_H_
