/* World.tag.cpp,v 1.3 2004/02/06 00:08:40 dick Exp
 *
 * Handle the rules for playing TAG.
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 2004 by
 *
 *      Chris Caserio        <cppc_xpilot_org>
 *      The XPilot Authors   <xpilot@xpilot.org>
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
 * World.tag.cpp,v
 * Revision 1.3  2004/02/06 00:08:40  dick
 * More whitespace
 *
 */
#include "StdAfx.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <math.h>

#include "version.h"
#include "config.h"
#include "serverconst.h"
#include "global.h"

void World::TransferTag(int oldtag, int newtag)
{
	char	msg[MSG_LEN];

	if (tag != players[oldtag]->id || oldtag == newtag)
		return;

	tag = players[newtag]->id;

	sprintf(msg, " < %s killed %s and is now 'it' > ",
		players[newtag]->name,
		players[oldtag]->name);
	BroadcastPlayMessage(msg);
}

///////////////////////////////////////////////////////////////////////////////
DFLOAT World::HandleTag(DFLOAT score, int victim, int killer)
{
	 if (tag == players[killer]->id)
		 return options.tagItKillMult->GetDouble() * score;
	 else if (tag == players[victim]->id)
	 {
		 TransferTag(victim, killer);
		 return options.tagKillItMult->GetDouble() * score;
	 }
	 return score;
}
