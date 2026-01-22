/* $Id: World.ScoreServer.cpp,v 1.7 2004/05/07 04:27:41 dick Exp $
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 2002 by
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
 */
/*
 * $Log: World.ScoreServer.cpp,v $
 * Revision 1.7  2004/05/07 04:27:41  dick
 * _visibility becomes Visibility.  global updateScores becomes a member of World.
 * Handle rank/rate data from the scoreserver and send it to the client.
 *
 * Revision 1.6  2004/02/03 03:39:54  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.5  2002/07/14 19:05:24  dick
 * When talking to the ScoreServer about PCSTR nick, always mention the PlayerType too.
 *
 * Revision 1.4  2002/07/09 21:44:29  dick
 * Send the player type (human,robot,tank) with the PlayerEvent to the ScoreServer.
 *
 * Revision 1.3  2002/06/30 07:04:29  dick
 * Add some overloaded variations to ScoreServerScoreEvent() to handle different
 * types of parameters.
 *
 * Revision 1.2  2002/06/26 07:02:43  dick
 * Don't send ScoreEvents unless we are Active.
 *
 * Revision 1.1  2002/06/25 04:53:47  dick
 * Interface the World to the ScoreServer so that noone else has to care whether
 * a ScoreServer is attached or not.
 *
 */
#include "StdAfx.h"

#include "Object.h"
#include "World.h"
#include "score.h"
#include "Player.h"
#include "ConnectionControlScoreServer.h"

///////////////////////////////////////////////////////////////////////////////
void World::ScoreServerPlayerEvent(Player* pl, ScorePlayerEvent pse)
{
	if (!scoreServer || !scoreServer->IsActive())
		return;
	scoreServer->SendPlayerEvent(pl, pse);
}

///////////////////////////////////////////////////////////////////////////////
void World::ScoreServerRequestPlayerScore(Player* pl)
{
	if (!scoreServer || !scoreServer->IsActive())
		return;
	scoreServer->RequestPlayerScore(pl);
}

///////////////////////////////////////////////////////////////////////////////
void World::ScoreServerRequestPlayerRanks()
{
	if (!scoreServer || !scoreServer->IsActive())
		return;
	scoreServer->RequestPlayerRanks(options.scoreTableRank->GetBool());
}

///////////////////////////////////////////////////////////////////////////////
void World::ScoreServerScoreEvent(Player* killer, DFLOAT wscore,
								  Player* killee, DFLOAT lscore,
								  ScoreType st)
{
	if (!scoreServer || !scoreServer->IsActive())
		return;
	PCSTR	wnick = "";
	PCSTR	lnick = "";
	PlayerType	ptr = PlayerNone;
	PlayerType	pte = PlayerNone;

	if (killer && killer->name)
	{
		wnick = killer->name;
		ptr = killer->playerType;
	}
	if (killee && killee->name)
	{
		lnick = killee->name;
		pte = killee->playerType;
	}
	scoreServer->SendScoreEvent(wnick, ptr, wscore, lnick, pte, lscore, st);
}

///////////////////////////////////////////////////////////////////////////////
void World::ScoreServerScoreEvent(PCSTR killer, PlayerType ptr, DFLOAT wscore,
								  Player* killee, DFLOAT lscore,
								  ScoreType st)
{
	if (!scoreServer || !scoreServer->IsActive())
		return;
	PCSTR	wnick = "";
	PCSTR	lnick = "";
	PlayerType pte = PlayerNone;
	if (killer)
		wnick = killer;
	if (killee && killee->name)
	{
		lnick = killee->name;
		pte = killee->playerType;
	}
	scoreServer->SendScoreEvent(wnick, ptr, wscore, lnick, pte, lscore, st);
}

///////////////////////////////////////////////////////////////////////////////
void World::ScoreServerScoreEvent(Player* killer, DFLOAT wscore,
								  PCSTR killee, PlayerType pte, DFLOAT lscore,
								  ScoreType st)
{
	if (!scoreServer || !scoreServer->IsActive())
		return;
	PCSTR	wnick = "";
	PlayerType ptr = PlayerNone;
	PCSTR	lnick = "";
	if (killer && killer->name)
	{
		wnick = killer->name;
		ptr = killer->playerType;
	}
	if (killee)
		lnick = killee;
	scoreServer->SendScoreEvent(wnick, ptr, wscore, lnick, pte, lscore, st);
}

