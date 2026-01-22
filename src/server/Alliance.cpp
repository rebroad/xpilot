/* $Id: Alliance.cpp,v 1.11 2004/05/07 04:27:41 dick Exp $
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2001 by
 *
 *      Bj�rn Stabell        <bjoern@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
 *      Kimiko Koopman        <kimiko@xpilot.org>
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
 * $Log: Alliance.cpp,v $
 * Revision 1.11  2004/05/07 04:27:41  dick
 * _visibility becomes Visibility.  global updateScores becomes a member of World.
 * Handle rank/rate data from the scoreserver and send it to the client.
 *
 * Revision 1.10  2004/02/23 02:06:09  dick
 * robot.h becomes Robot.h
 *
 * Revision 1.9  2004/02/20 06:27:47  dick
 * SendPlayMessage becomes BroadcastPlayMessage() because it sends a play message
 * to all connections.
 *
 * Revision 1.8  2004/02/18 01:22:14  dick
 * Remove extraneous and obsolete #include "netserver.h"
 *
 * Revision 1.7  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.6  2002/09/01 00:13:45  dick
 * Remove obsolete windows #include "winServer.h"
 *
 * Revision 1.5  2002/08/01 14:44:29  dick
 * Remove ref to theWorld.
 *
 * Revision 1.4  2002/06/30 06:43:24  dick
 * SendMessage is a bad word.  It's a Windows function that is a macro and gets
 * translated to SendMessageA or SendMessageW.  Although it works, renaming ours
 * to SendPlayMessage makes browsing the source code easier.
 *
 * Revision 1.3  2002/06/01 06:06:47  dick
 * Encapsulate (almost) everything.  Get rid of (almost) all refs to theWorld.
 *
 * Revision 1.2  2002/05/18 22:33:16  dick
 * Commentality
 *
 *
 */
#include "StdAfx.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "version.h"
#include "config.h"
#include "serverconst.h"
#include "global.h"
#include "proto.h"
#include "World.h"
#include "Robot.h"
#include "bit.h"
#include "error.h"
#include "commonproto.h"


char alliance_version[] = VERSION;


/*
 * Alliance information.
 */
typedef struct {
    int		id;		/* the ID of this alliance */
    int		numMembers;	/* the number of members in this alliance */
	World*	world;		/* which world is this alliance in */
} alliance_t;


static alliance_t	*Alliances[MAX_TEAMS];


static int New_alliance_ID(World* w);
static void Alliance_add_player(alliance_t *alliance, Player *pl);
static int Alliance_remove_player(alliance_t *alliance, Player *pl);
static void Set_alliance_message(World* w, alliance_t *alliance, const char *msg);
static int Create_alliance(World* w, Player* pl1, Player* pl2);
static void Dissolve_alliance(World* w,int id);
static void Merge_alliances(World* w, int id1, int id2);


void Player::InvitePlayer(int ally_ind)
{
//    Player	*pl = Players[ind]
	Player*	ally = world->players[ally_ind];

    if (this == ally) {
		/* we can never form an alliance with ourselves */
		return; // 0;
    }
    if (IS_TANK_PTR(ally)) {
		/* tanks can't handle invitations */
		return; // 0;
    }
    if (ALLIANCE(world, Ind(), ally_ind)) {
		/* we're already in the same alliance */
		return; // 0;
    }
    if (invite == ally->id) {
		/* player has already been invited by us */
		return; // 0;
    }
    if (ally->invite == id) {
	/* player has already invited us. accept invitation */
		AcceptAlliance(ally_ind);
		return; //1;
    }
    if (invite != NO_ID) {
	/* we have already invited another player. cancel that invitation */
		CancelInvitation();
    }
    /* set & send invitation */
    invite = ally->id;
    if (IS_ROBOT_PTR(ally)) {
	Robot_invite(world, ally_ind, Ind());
    }
    else if (IS_HUMAN_PTR(ally)) {
	char msg[MSG_LEN];
	sprintf(msg, " < %s seeks an alliance with you >", name);
	ally->SetMessage(msg);
    }
    //return 1;
}

void Player::CancelInvitation()
{
	Player*	ally;

    if (invite == NO_ID) {
		/* we have not invited anyone */
		return; // 0;
    }
    ally = world->players[world->getInd[invite]];
    invite = NO_ID;
    if (IS_HUMAN_PTR(ally)) {
		char msg[MSG_LEN];
		sprintf(msg, " < %s has cancelled the invitation for an alliance >",
			name);
		ally->SetMessage(msg);
    }
    //return 1;
}

/* refuses invitation from a specific player */
void Player::RefuseAlliance(int ally_ind)
{
	Player*	ally = world->players[ally_ind];

    if (ally->invite != id) {
		/* we were not invited anyway */
		return; // 0;
    }
    ally->invite = NO_ID;
    if (IS_HUMAN_PTR(ally)) {
		char msg[MSG_LEN];
		sprintf(msg, " < %s has declined your invitation for an alliance >",
			name);
		ally->SetMessage(msg);
    }
    return; // 1;
}

/* refuses invitations from any player */
void Player::RefuseAllAlliances()
{
    int		i, j = 0;

    for (i = 0; i < world->numPlayers; i++) {
		if (world->players[i]->invite == id) {
			RefuseAlliance(i);
			j++;
		}
    }
    if (IS_HUMAN_PTR(this)) {
		char msg[MSG_LEN];
		if (j == 0) {
			sprintf(msg, " < You were not invited for any alliance >");
		} else {
			sprintf(msg, " < %d invitation%s for %s declined >", j,
				(j > 1 ? "s" : ""), (j > 1 ? "alliances" : "an alliance"));
		}
		SetMessage(msg);
    }
    return; // j;
}

/* accepts an invitation from a specific player */
void Player::AcceptAlliance(int ally_ind)
{
//    Player	*pl = Players[ind],
	Player* ally = world->players[ally_ind];
    int		success = 1;

    if (ally->invite != id) {
	/* we were not invited */
	return; // 0;
    }
    ally->invite = NO_ID;
    if (ally->alliance != ALLIANCE_NOT_SET) {
	if (alliance != ALLIANCE_NOT_SET) {
	    /* both players are in alliances */
	    Merge_alliances(world, ally_ind, alliance);
	} else {
	    /* inviting player is in an alliance */
	    JoinAlliance(ally_ind);
	}
    } else {
	if (alliance != ALLIANCE_NOT_SET) {
	    /* accepting player is in an alliance */
	    ally->JoinAlliance(Ind());
	} else {
	    /* neither player is in an alliance */
	    success = Create_alliance(world, this, ally);
	}
    }
    return; // success;
}

/* accepts invitations from any player */
void Player::AcceptAllAlliances()
{
    //Player	*pl = Players[ind];
    int		i, j = 0;

    for (i = 0; i < world->numPlayers; i++) {
		if (world->players[i]->invite == id) {
			AcceptAlliance(i);
			j++;
		}
    }
    if (IS_HUMAN_PTR(this)) {
		char msg[MSG_LEN];
		if (j == 0) {
			sprintf(msg, " < You were not invited for any alliance >");
		} else {
			sprintf(msg, " < %d invitation%s for %s accepted >", j,
				(j > 0 ? "s" : ""), (j > 0 ? "alliances" : "an alliance"));
		}
		SetMessage(msg);
    }
    return; // j;
}

/* returns a pointer to the alliance with a given ID */
static alliance_t *Find_alliance(World* w, int id)
{
    int i;

    if (id != ALLIANCE_NOT_SET) {
	for (i = 0; i < w->numAlliances; i++) {
	    if (Alliances[i]->id == id) {
		return Alliances[i];
	    }
	}
    }

    return NULL;
}

/*
 * Return the number of members in a particular alliance.
 */
int Get_alliance_member_count(World* w, int id)
{
    alliance_t	*alliance = Find_alliance(w, id);

    if (alliance != NULL) {
	return alliance->numMembers;
    }

    return 0;
}

/* sends a message to all the members of an alliance */
static void Set_alliance_message(alliance_t *alliance, const char *msg)
{
    int	i;

    for (i = 0; i < alliance->world->numPlayers; i++) {
		if (IS_HUMAN_IND(alliance->world, i)) {
			if (alliance->world->players[i]->alliance == alliance->id) {
				alliance->world->players[i]->SetMessage(msg);
			}
		}
    }
}

/* returns an unused ID for an alliance */
static int New_alliance_ID(World* w)
{
    int i, try_id;

    for (try_id = 0; try_id < MAX_TEAMS; try_id++) {
	for (i = 0; i < w->numAlliances; i++) {
	    if (Alliances[i]->id == try_id) {
		break;
	    }
	}
	if (i == w->numAlliances) {
	    break;
	}
    }
    if (try_id < MAX_TEAMS) {
	return try_id;
    }
    return ALLIANCE_NOT_SET;
}

/* creates an alliance between two players */
static int Create_alliance(World* w, Player* pl1, Player* pl2)
{
    alliance_t	*alliance = (alliance_t *)malloc(sizeof(alliance_t));
    char	msg[MSG_LEN];

    if (alliance == NULL) {
	error("Not enough memory for new alliance.\n");
	return 0;
    }

    alliance->id = New_alliance_ID(w);
    if (alliance->id == ALLIANCE_NOT_SET) {
	warn("Maximum number of alliances reached.\n");
	free(alliance);
	return 0;
    }
    alliance->numMembers = 0;
	alliance->world = w;
    Alliances[w->numAlliances] = alliance;
    w->numAlliances++;
    Alliance_add_player(alliance, pl1);
    Alliance_add_player(alliance, pl2);
    /* announcement */
    if (w->options.announceAlliances->GetBool()) {
	sprintf(msg, " < %s and %s have formed alliance %d >", pl1->name,
		pl2->name, alliance->id);
	w->BroadcastPlayMessage(msg);
    } else {
	sprintf(msg, " < You have formed an alliance with %s >", pl2->name);
	pl1->SetMessage(msg);
	sprintf(msg, " < You have formed an alliance with %s >", pl1->name);
	pl2->SetMessage(msg);
    }
    return 1;
}

/* adds a player to an existing alliance */
void Player::JoinAlliance(int ally_ind)
{
    //Player	*pl = Players[ind],
	Player*	ally = world->players[ally_ind];
    alliance_t	*alliance = Find_alliance(world, ally->alliance);
    char	msg[MSG_LEN];

    if (!IS_TANK_IND(world, Ind())) {
	/* announce first to avoid sending the player two messages */
	if (world->options.announceAlliances->GetBool()) {
	    sprintf(msg, " < %s has joined alliance %d >",
		    name, alliance->id);
	    world->BroadcastPlayMessage(msg);
	}
	else {
	    sprintf(msg, " < %s has joined your alliance >", name);
	    Set_alliance_message(alliance, msg);
	    if (IS_HUMAN_PTR(this)) {
			sprintf(msg, " < You have joined %s's alliance >", ally->name);
		SetMessage(msg);
	    }
	}
    }

    Alliance_add_player(alliance, this);
}

/* atomic addition of player to alliance */
static void Alliance_add_player(alliance_t *alliance, Player *pl)
{
    int	i;

    /* drop invitations for this player from other members */
    for (i = 0; i < alliance->world->numPlayers; i++) {
	if (alliance->world->players[i]->invite == pl->id) {
	    pl->CancelInvitation();
	}
    }
    pl->alliance = alliance->id;
    alliance->numMembers++;
    alliance->world->updateScores = true;
}

/* removes a player from an alliance and dissolves the alliance if necessary */
int Player::LeaveAlliance()
{
    alliance_t	*alliance;
    char	msg[MSG_LEN];

    if (this->alliance == ALLIANCE_NOT_SET) {
	/* we're not in any alliance */
	return 0;
    }
    alliance = Find_alliance(world, this->alliance);
    Alliance_remove_player(alliance, this);
    /* announcement */
    if (!IS_TANK_PTR(this)) {
	if (world->options.announceAlliances->GetBool()) {
	    sprintf(msg, " < %s has left alliance %d >", name,
		    alliance->id);
	    world->BroadcastPlayMessage(msg);
	} else {
	    sprintf(msg, " < %s has left your alliance >", name);
	    Set_alliance_message(alliance, msg);
	    if (IS_HUMAN_PTR(this)) {
			SetMessage(" < You have left the alliance >");
	    }
	}
    }
    if (alliance->numMembers <= 1) {
	Dissolve_alliance(world, alliance->id);
    }
    return 1;
}

/* atomic removal of player from alliance */
static int Alliance_remove_player(alliance_t *alliance, Player *pl)
{
    if (pl->alliance == alliance->id) {
	pl->alliance = ALLIANCE_NOT_SET;
	alliance->numMembers--;
	alliance->world->updateScores = true;
	return 1;
    }
    return 0;
}

static void Dissolve_alliance(World* w, int id)
{
    alliance_t	*alliance = Find_alliance(w, id);
    int		i;

    /* remove all remaining members from the alliance */
    for (i = 0; i < w->numPlayers; i++) {
	if (w->players[i]->alliance == id) {
	    Alliance_remove_player(alliance, w->players[i]);
	    if (!w->options.announceAlliances->GetBool()
			&& IS_HUMAN_IND(w, i))
		{
			w->players[i]->SetMessage(" < Your alliance has been dissolved >");
	    }
	}
    }
    /* check */
    if (alliance->numMembers != 0) {
	warn("Dissolve_alliance after dissolve %d remain!",
	     alliance->numMembers);
    }

    /* find the index of the alliance to be removed */
    for (i = 0; i < w->numAlliances; i++) {
	if (Alliances[i]->id == alliance->id) {
	    break;
	}
    }
    /* move the last alliance to that index */
    Alliances[i] = Alliances[w->numAlliances - 1];
    /* announcement */
    if (w->options.announceAlliances->GetBool()) {
	char msg[MSG_LEN];
	sprintf(msg, " < Alliance %d has been dissolved >", alliance->id);
	w->BroadcastPlayMessage(msg);
    }
    /* and clean up that alliance */
    free(alliance);
    w->numAlliances--;
}

/*
 * Destroy all alliances.
 */
void Dissolve_all_alliances(World* w)
{
    int		i;

    for (i = w->numAlliances - 1; i >= 0; i--) {
	Dissolve_alliance(w, Alliances[i]->id);
    }
}

/* merges two alliances by moving the members of the second to the first */
static void Merge_alliances(World* w, int ind, int id2)
{
    alliance_t	*alliance2 = Find_alliance(w, id2);
    int		i;

    /* move each member of alliance2 to alliance1 */
    for (i = 0; i < w->numPlayers; i++) {
	if (w->players[i]->alliance == id2) {
	    Alliance_remove_player(alliance2, w->players[i]);
	    w->players[i]->JoinAlliance(ind);
	}
    }
    Dissolve_alliance(w, id2);
}

void Player::AlliancePlayerList()
{
    int		i;
    char	msg[MSG_LEN];

    if (alliance == ALLIANCE_NOT_SET) {
		SetMessage(" < You are not a member of any alliance >");
    }
    else {
	if (world->options.announceAlliances->GetBool()) {
	    sprintf(msg, " < Alliance %d:", alliance);
	} else {
	    sprintf(msg, " < Your alliance: ");
	}
	for (i = 0; i < world->numPlayers; i++) {
	    if (world->players[i]->alliance == alliance) {
		if (IS_HUMAN_IND(world, i)) {
		    if (strlen(msg) > 80) {
				strlcat(msg, ">", sizeof(msg));
				SetMessage(msg);
				strlcpy(msg, " <            ", sizeof(msg));
		    }
		    strlcat(msg, world->players[i]->name, sizeof(msg));
		    strlcat(msg, ", ", sizeof(msg));
		}
	    }
	}
	for (i = 0; i < world->numPlayers; i++) {
	    if (world->players[i]->alliance == alliance) {
		if (IS_ROBOT_IND(world, i)) {
		    if (strlen(msg) > 80) {
			strlcat(msg, ">", sizeof(msg));
			SetMessage(msg);
			strlcpy(msg, " <            ", sizeof(msg));
		    }
		    strlcat(msg, world->players[i]->name, sizeof(msg));
		    strlcat(msg, ", ", sizeof(msg));
		}
	    }
	}
	if (strlen(msg) >= 2 && !strcmp(msg + strlen(msg) - 2, ", ")) {
	    msg[strlen(msg) - 2] = '\0';
	}
	strlcat(msg, " >", sizeof(msg));
	SetMessage(msg);
    }
}

