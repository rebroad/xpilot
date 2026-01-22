/* $Id: ServerOptionWorld.cpp,v 1.31 2007/02/03 09:21:12 dick Exp $
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2001 by
 *
 *      Bjørn Stabell        <bjoern@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
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
 *  $Log: ServerOptionWorld.cpp,v $
 *  Revision 1.31  2007/02/03 09:21:12  dick
 *  Whitespace
 *
 *  Revision 1.30  2007/01/10 18:14:47  dick
 *  All robot actions are now handled through RobotMan.
 *  There is one RobotMan per World.
 *
 *  Revision 1.29  2005/03/17 22:12:14  kps
 *  Get rid of warnings from makedepend about "non-portable whitespace".
 *
 *  Revision 1.28  2004/05/20 21:45:47  dick
 *  TunerShowLocks() tunes the showLocks attribute.
 *
 *  Revision 1.27  2004/05/07 04:27:41  dick
 *  _visibility becomes Visibility.  global updateScores becomes a member of World.
 *  Handle rank/rate data from the scoreserver and send it to the client.
 *
 *  Revision 1.26  2004/05/04 21:07:17  dick
 *  Need randommnt.h
 *
 *  Revision 1.25  2004/04/26 07:07:14  dick
 *  Define a list of pages that can be displayed on the ScoreTable in the client.
 *  Each page can be controlled by the server (and the server admin tool).
 *
 *  Revision 1.24  2004/02/23 02:06:09  dick
 *  robot.h becomes Robot.h
 *
 *  Revision 1.23  2004/02/03 03:39:54  dick
 *  Add Windows precompiled header support.
 *
 *  Revision 1.22  2004/01/16 17:32:22  dick
 *  Fix ReportToMetaTuner().  If initially disabled, enabling reportToMeta while
 *  running requires calling MetaInit() to lookup the metaservers.
 *
 *  Revision 1.21  2004/01/12 05:23:06  dick
 *  Add TunerPauseScoreReduce() TunerAllowViewing and TunerReportToMetaServer
 *
 *  Revision 1.20  2004/01/09 17:28:52  dick
 *  Add TunerFirewallPortList to activate changes.
 *
 *  Revision 1.19  2004/01/06 04:48:28  dick
 *  Add support for building under Cygwin.  Still needs work and the output
 *  is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 *  Revision 1.18  2002/09/16 22:15:00  dick
 *  Comment out broken Fix*Name() tuners.
 *
 *  Revision 1.17  2002/09/16 21:52:02  dick
 *  Use Stringized ServerT.
 *
 *  Revision 1.16  2002/09/11 16:42:04  dick
 *  Here endeth the server encapsulation task.
 *  theWorld is now an array of World(s) and get loaded dynamically.
 *
 *  Revision 1.15  2002/09/01 00:13:45  dick
 *  Remove obsolete windows #include "winServer.h"
 *
 *  Revision 1.14  2002/07/09 21:44:29  dick
 *  Send the player type (human,robot,tank) with the PlayerEvent to the ScoreServer.
 *
 *  Revision 1.13  2002/07/09 04:50:14  dick
 *  Server wants neither WANTHELPTEXT or WANTLABELS
 *
 *  Revision 1.12  2002/07/08 06:48:19  dick
 *  Linux tweaks
 *
 *  Revision 1.11  2002/07/08 06:18:55  dick
 *  TunerSaveTuned() causes the options that have been tuned to be saved
 *  to a file.
 *
 *  Revision 1.10  2002/06/23 06:38:41  dick
 *  Provide the ServerOptionFactory for the server
 *
 *  Revision 1.9  2002/06/09 21:13:12  dick
 *  Deadwood delete
 *
 *  Revision 1.8  2002/06/01 06:06:47  dick
 *  Encapsulate (almost) everything.  Get rid of (almost) all refs to theWorld.
 *
 *  Revision 1.7  2002/05/30 03:33:40  jlmiller
 *
 *  Linux cleanups for compilation after reorg of ServerList
 *
 *  Revision 1.6  2002/05/18 20:55:38  dick
 *  Update to XPilot-4.5.1
 *
 *  Revision 1.5  2001/09/03 00:51:49  dick
 *  Tuners cleanup.
 *
 *  Revision 1.4  2001/08/31 20:58:13  dick
 *  Server needs to be able to receive reliable packets as well as send them.
 *  Use the new emh error handling.
 *
 *  Revision 1.3  2001/08/14 21:37:14  dick
 *  Array Obj[] becomes objs[].  We now have a class called Obj.
 *
 *  Revision 1.2  2001/08/05 18:18:29  dick
 *  Linux builds and runs again
 *
 *  Revision 1.1  2001/08/05 08:05:36  dick
 *  Add Tuners to the server.  ComputeSensorRange(), AddTank(), RemoveTank()
 *  become members of Player.  ComputeGravity*() become members of World.
 *
 */
#include "StdAfx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#if defined(_UNIX) || defined(_CYGWIN)
# include <unistd.h>
# include <sys/file.h>
#else
# include <direct.h>
#endif

#include "version.h"
#include "portability.h"
#include "config.h"
#include "showtime.h"
#include "const.h"
#include "global.h"
#include "proto.h"
#include "bit.h"
#include "error.h"
#include "metaserver.h"
#include "commonproto.h"
#include "randommt.h"
#include "checknames.h"
#include "ConnectionPlayer.h"
#include "ConnectionControlScoreServer.h"

#include "Cannon.h"
#include "World.h"

#include "ServerOptionWorld.h"
#include "RobotMan.h"

///////////////////////////////////////////////////////////////////////////////
// SYSTEM WIDE Server Options initialization for XPilotServer
// Each application that uses a ServerOptions must include the init.cpp.h
// and provide a ServerOptionFactory
#define	HASTUNER		1
#define	WANTHELPTEXT	0
#define	WANTLABELS		0
#include "ServerOptions.init.cpp.h"

ServerOption*	ServerOptionFactory(ServerOptions* _parent,
				 PCSTR _name, PCSTR _altname, PCSTR _label,
				 int _length, ValType _valType,
				 PCSTR _defval,
				 int _row, int _col, int _sheet, int _visibleto,
				 TunerFunc	_tuner,
				 PCSTR _help)
{
	return(new ServerOptionWorld(_parent,
				_name, _altname, _label, _length, _valType,
				_defval,
				_row, _col, _sheet, _visibleto,
				_tuner,
				_help));
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
ServerOptionWorld::ServerOptionWorld(ServerOptions* _parent,
				 PCSTR _name, PCSTR _altname, PCSTR _label,
				 int _length, ValType _valType,
				 PCSTR _defval,
				 int _row, int _col, int _sheet, int _visibleto,
				 TunerFunc	_tuner,
				 PCSTR _help)
: ServerOption(_parent,
			   _name, _altname, _label,
			   _length, _valType,
			   _defval,
			   _row, _col, _sheet, _visibleto,
			   _tuner,
			   _help)
{
}


///////////////////////////////////////////////////////////////////////////////
void ServerOptionWorld::DoTuner()
{
	(this->*tuner)();
}

///////////////////////////////////////////////////////////////////////////////
/*
** Two functions which can be used if an option
** does not have its own function which should
** be called after the option value has been
** changed during runtime.  The tuner_none
** function should be specified when an option
** cannot be changed at all during runtime.
** The tuner_dummy can be specified if it
** is OK to modify the option during runtime
** and no follow up action is needed.
*/
// DIK: xpilot++ kinda ignores these, a bug for sure.
void ServerOptionWorld::TunerNone(void)  {}
void ServerOptionWorld::TunerDummy(void) {}


// These are broken! (
//void ServerOptionWorld::TuneRobotRealName(void) { FixRealName(GetString()); }
//void ServerOptionWorld::TuneRobotHostName(void) { FixHostName(GetString()); }
//void ServerOptionWorld::TuneTankRealName(void)  { FixRealName(GetString()); }
//void ServerOptionWorld::TuneTankHostName(void)  { FixHostName(GetString()); }
void ServerOptionWorld::TuneRobotRealName(void) { }
void ServerOptionWorld::TuneRobotHostName(void) { }
void ServerOptionWorld::TuneTankRealName(void)  { }
void ServerOptionWorld::TuneTankHostName(void)  { }


///////////////////////////////////////////////////////////////////////////////
void ServerOptionWorld::TunerGravity()
{
//	World* w = (World*)parent->parent;
	((World*)(parent->parent))->ComputeGravity();
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionWorld::TunerInitialItems()
{
	((World*)(parent->parent))->SetWorldItems();
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionWorld::TunerMiscItemLimits()
{
	((World*)(parent->parent))->SetMiscItemLimits();
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionWorld::TunerPlock(void)
{
	((World*)(parent->parent))->TunerPLock();
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionWorld::TunerShotsMax(void)
{
	World*	w = (World*)parent->parent;
	int i;

	for (i = 0; i < w->numPlayers; i++) {
		w->players[i]->shot_max = GetInt();
	}
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionWorld::TunerShipMass(void)
{
		World*	w = (World*)parent->parent;
	int i;

	for (i = 0; i < w->numPlayers; i++) {
		w->players[i]->emptymass = GetDouble();
	}
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionWorld::TunerBallMass(void)
{
	World*	w = (World*)parent->parent;
	int i;

	for (i = 0; i < w->numObjs; i++) {
		if (BIT(w->objs[i]->type, OBJ_BALL)) {
			w->objs[i]->mass = GetDouble();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionWorld::TunerMaxRobots(void)
{
	World*	w = (World*)parent->parent;

	if (GetInt() < 0) {
		Set(w->numBases);
	}

	if (GetInt() < parent->minRobots->GetInt()) {
		parent->minRobots->Set(GetInt());
	}

	while (GetInt() < w->numRobots) {
		w->robotMan->Delete(-1, true);
	}
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionWorld::TunerMinRobots(void)
{
	if (GetInt() < 0) {
		Set(parent->maxRobots->GetInt());
	}

	if (parent->maxRobots->GetInt() < GetInt()) {
		parent->maxRobots->Set(GetInt());
	}
}

///////////////////////////////////////////////////////////////////////////////
// allowShields
void ServerOptionWorld::TunerPlayerShielding(void)
{
	World*	w = (World*)parent->parent;
	int i;

	w->SetWorldRules();

	if (GetInt()) {
		SET_BIT(DEF_HAVE, HAS_SHIELD);

		for (i = 0; i < w->numPlayers; i++) {
			if (!IS_TANK_PTR(w->players[i])) {
				if (!BIT(w->players[i]->used, HAS_SHOT))
					SET_BIT(w->players[i]->used, HAS_SHIELD);

				SET_BIT(w->players[i]->have, HAS_SHIELD);
				w->players[i]->shield_time = 0;
			}
		}
	}
	else {
		CLR_BIT(DEF_HAVE, HAS_SHIELD);

		for (i = 0; i < w->numPlayers; i++) {
			w->players[i]->shield_time = 2 * w->GetFPS();
			/* 2 seconds to get to safety */
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionWorld::TunerPlayerStartsShielded(void)
{
	if (parent->allowShields->GetBool()) {
		Set(true);		/* Doesn't make sense to turn off when shields are on. */
	}
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionWorld::TunerWorldLives(void)
{
	World*	w = (World*)parent->parent;
	if (GetInt() < 0)
		Set(0);

	w->SetWorldRules();

	if (BIT(w->rules->mode, LIMITED_LIVES)) {
		ResetAllPlayers(w);
		if (parent->gameDuration->GetInt() == -1)
			parent->gameDuration->Set(0);
	}
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionWorld::TunerCannonSmartness(void)
{
	Limit(0, 3);
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionWorld::TunerTeamCannons(void)
{
	World*	w = (World*)parent->parent;
	int i;
	int team;

	if (GetBool()) {
		for (i = 0; i < w->numCannons; i++) {
			team = w->FindClosestTeam(w->cannons[i].blk_pos.x,
									 w->cannons[i].blk_pos.y);
			if (team == TEAM_NOT_SET) {
				warn("Couldn't find a matching team for the cannon.");
			}
			w->cannons[i].team = team;
		}
	}
	else {
		for (i = 0; i < w->numCannons; i++)
			w->cannons[i].team = TEAM_NOT_SET;
	}
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionWorld::TunerCannonsUseItems(void)
{
	World*	w = (World*)parent->parent;
	int i, j;
	Cannon* c;

	w->MoveInit();

	for (i = 0; i < w->numCannons; i++) {
		c = w->cannons + i;
		for (j = 0; j < NUM_ITEMS; j++) {
			c->items[j] = 0;

			if (GetBool())
				c->AddItem(j, (int)(rfrac() * (w->items[j].initial + 1)));
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionWorld::TunerWormTime(void)
{
	World*	w = (World*)parent->parent;
	int i;

	if (GetInt() < 0)
				Set(0);

	if (GetInt()) {
		for (i = 0; i < w->numWormholes; i++) {
			w->wormHoles[i].countdown = GetInt() * w->GetFPS();
		}
	}
	else {
		for (i = 0; i < w->numWormholes; i++) {
			if (w->wormHoles[i].temporary)
				w->RemoveTempWormhole(i);
			else
				w->wormHoles[i].countdown = WORMCOUNT;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionWorld::TunerModifiers(void)
{
	World*	w = (World*)parent->parent;
	int i;

	w->SetWorldRules();

	for (i = 0; i < w->numPlayers; i++) {
		w->FilterMods(&w->players[i]->mods);
	}
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionWorld::TunerMineLife(void)
{
	World*	w = (World*)parent->parent;
	int i;
	int life;

	if (GetInt() < 0)
		Set(0);

	for (i = 0; i < w->numObjs; i++) {
		if (w->objs[i]->type != OBJ_MINE)
			continue;

		if (!BIT(w->objs[i]->status, FROMCANNON)) {
			life =
				(GetInt() ? GetInt() : MINE_LIFETIME) / (w->objs[i]->mods.mini + 1);

			w->objs[i]->life = (int)(rfrac() * life);
			/* We wouldn't want all the mines
			   to explode simultaneously, now
			   would we? */
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionWorld::TunerMissileLife(void)
{
	World*	w = (World*)parent->parent;
	int i;
	int life;

	if (GetDouble() < 0)
		Set(0);

	for (i = 0; i < w->numObjs; i++) {
		if (w->objs[i]->type != OBJ_SMART_SHOT &&
			w->objs[i]->type != OBJ_HEAT_SHOT && w->objs[i]->type != OBJ_TORPEDO)
			continue;

		if (!BIT(w->objs[i]->status, FROMCANNON)) {
			life =
				(GetInt() 
						? GetInt() 
						: (MISSILE_LIFETIME * w->GetFPS())) / (w->objs[i]->mods.mini + 1);

			w->objs[i]->life = (int)(rfrac() * life);
			/* Maybe all the missiles are full
			   nukes. Going off together might
			   not be such a good idea. */
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionWorld::TunerGameDuration(void)
{
	if (GetDouble() <= 0.0) {
		Set((int)time((time_t *) NULL));
	}

	else
		Set((int)((time_t) (GetDouble() * 60) + time((time_t *) NULL)));
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionWorld::TunerRaceLaps(void)
{
	World*	w = (World*)parent->parent;
	if (BIT(w->rules->mode, TIMING)) {
		ResetAllPlayers(w);
		if (parent->gameDuration->GetInt() == -1)
			parent->gameDuration->Set(0);
	}
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionWorld::TunerAsteroidProb()
{
	World*	w = (World*)parent->parent;
	w->TuneAsteroidProb();
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionWorld::TunerBounce()
{
	World*	w = (World*)parent->parent;
	w->MoveInit();
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionWorld::TunerRules()
{
	World*	w = (World*)parent->parent;
	w->SetWorldRules();
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionWorld::TunerPauseScoreReduce()
{
	Limit(0,9999);
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionWorld::TunerAllowViewing()
{
	World*	w = (World*)parent->parent;
	w->updateScores = true;
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionWorld::TunerAnnounceAlliances()
{
	World*	w = (World*)parent->parent;
	w->updateScores = true;
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionWorld::TunerAllowAlliances()
{
	World*	w = (World*)parent->parent;

	if (BIT(w->rules->mode, TEAM_PLAY)) {
		CLR_BIT(w->rules->mode, ALLIANCES);
	}
	if (!BIT(w->rules->mode, ALLIANCES) && w->numAlliances > 0) {
		Dissolve_all_alliances(w);
	}
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionWorld::TunerFirewallPortList()
{
	World*	w = (World*)parent->parent;
	w->firewallPortList.Set(w->options.firewallPortList->GetString());
	w->options.firewallPortList->Set(w->firewallPortList.Get());
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionWorld::TunerReportToMetaServer()
{
	World*	w = (World*)parent->parent;
	if (GetBool())
		MetaInit(w);
	else
	{
		Set(true);		// temp enable us to shutdown meta
		MetaGone(w);
		Set(false);
	}
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionWorld::TunerScoreTable()
{
	int		i;
	World*	w = (World*)parent->parent;
	for (i=0; i<w->numPlayers; i++)
	{
		Player* pl = w->players[i];
		if (pl->conn)
			pl->conn->SendScoreTablePages();
	}
	if (w->scoreServer)
		w->scoreServer->RequestPlayerRanks(
							w->options.scoreTableRank->GetBool());
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionWorld::TunerShowLocks()
{
	int		i;
	int		j;
	World*	w = (World*)parent->parent;
	bool onOff = w->options.showLocks->GetBool();
	for (i=0; i<w->numPlayers; i++)
	{
		Player* pl = w->players[i];
		if (pl->conn)
		{
			for (j=0; j<w->numPlayers; j++)
			{
				Player* vic = w->players[j];
				if (vic->playerType == PlayerHuman)
					pl->conn->SendEyes(vic->id, onOff ? vic->lock.pl_id : NO_ID);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
bool ServerOptionWorld::MakeVarExist(String& error)
{
	String	e;
	String	fname = Conf_vardir();
	char	olddir[MSG_LEN];
	if (getcwd(olddir, MSG_LEN) == NULL)
	{
		error.printf("VarExist failed to get current directory");
		return(false);

	}
#	if defined(_UNIX) || defined(_CYGWIN)
#		define	MODEBITS	,666
#	else
#		define	MODEBITS
#	endif
	if (chdir(Conf_vardir()) != 0)
	{
		if (mkdir(Conf_vardir() MODEBITS) != 0)
		{
			error.printf("VarExist failed to create \"%s\"", Conf_vardir());
			return(false);
		}
		xpprintf("%sVarExist: Creating directory \"%s\"", showtime(), Conf_vardir());
	}
	chdir(olddir);
	return(true);
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionWorld::TunerSaveTuned()
{
	World*	w = (World*)parent->parent;
	String	e;
	int		i;

	if (!MakeVarExist(e))
	{
		w->options.saveTuned->Set(e);
		xpprintf("%s%s\n", showtime(), (PCSTR)e);
		w->BroadcastOption(w->options.saveTuned);
		return;
	}
	String	fname = Conf_vardir();
	String	tweakName;
	if (!MakeTweaksFileName(tweakName, w->options.mapFileName->GetString()))
	{
		String	e;
		e.printf("SaveTuned failed: \"%s\"", (PCSTR)tweakName);
		xpprintf("%s%s\n", showtime(), (PCSTR)e);
		w->options.saveTuned->Set(e);
		w->BroadcastOption(w->options.saveTuned);
		return;
	}	
	fname += tweakName;
	xpprintf("%sSaving tweaks to \"%s\"\n", showtime(), (PCSTR)fname);
	FILE*	fp;
	if (!(fp = fopen(fname, "w")))
	{
		e.printf("SaveTuned failed to open file \"%s\"", (PCSTR)fname);
		xpprintf("%s%s\n", showtime(), (PCSTR)e);
		w->options.saveTuned->Set(e);
		w->BroadcastOption(w->options.saveTuned);
		return;
	}
	for (i=0; i<w->options.numPrefs; i++)
	{
		ServerOption* so = w->options.prefsArray[i];
		if (so->optOrigin & (OPT_TUNE|OPT_TUNEFILE))
		{
			fprintf(fp, "%s:%s\n", so->name, (PCSTR)so->GetString());
		}
	}
	fclose(fp);
	for (i=0; i<w->options.numPrefs; i++)
	{
		ServerOption* so = w->options.prefsArray[i];
		if (so->optOrigin & (OPT_TUNE|OPT_TUNEFILE))
		{
			so->optOrigin = OPT_TUNEFILE;
			w->BroadcastOption(so);
		}
	}
	e.printf("SaveTuned: saved \"%s\"", (PCSTR)fname);
	xpprintf("%s%s\n", showtime(), (PCSTR)e);
	w->options.saveTuned->Set(e);
}
