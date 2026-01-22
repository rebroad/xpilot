/* $Id: command.cpp,v 1.31 2004/05/22 15:12:34 dick Exp $
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2001 by
 *
 *      Bj�rn Stabell        <bjoern@xpilot.org>
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
 *  $Log: command.cpp,v $
 *  Revision 1.31  2004/05/22 15:12:34  dick
 *  Remove #ifndef SILENT
 *
 *  Revision 1.30  2004/05/07 01:15:04  dick
 *  Retab
 *
 *  Revision 1.29  2004/04/26 07:07:14  dick
 *  Define a list of pages that can be displayed on the ScoreTable in the client.
 *  Each page can be controlled by the server (and the server admin tool).
 *
 *  Revision 1.28  2004/02/20 06:27:47  dick
 *  SendPlayMessage becomes BroadcastPlayMessage() because it sends a play message
 *  to all connections.
 *
 *  Revision 1.27  2004/02/18 01:22:14  dick
 *  Remove extraneous and obsolete #include "netserver.h"
 *
 *  Revision 1.26  2004/02/18 00:58:34  dick
 *  Delete_player becomes DeletePlayer
 *
 *  Revision 1.25  2004/02/16 14:29:44  dick
 *  server/sched.cpp and friends become serverlib/NetServer
 *  which disconnects the udp listen and receive functionality from the game.
 *
 *  Revision 1.24  2004/02/03 03:39:54  dick
 *  Add Windows precompiled header support.
 *
 *  Revision 1.23  2004/01/08 18:10:51  dick
 *  Update to XPilot 4.5.5beta
 *
 *  Revision 1.22  2002/09/01 00:13:46  dick
 *  Remove obsolete windows #include "winServer.h"
 *
 *  Revision 1.21  2002/07/11 16:36:03  dick
 *  Update to XPilot-4.5.4beta
 *
 *  Revision 1.20  2002/07/08 05:52:59  dick
 *  showtime() handles the space after itself
 *
 *  Revision 1.19  2002/06/30 06:43:24  dick
 *  SendMessage is a bad word.  It's a Windows function that is a macro and gets
 *  translated to SendMessageA or SendMessageW.  Although it works, renaming ours
 *  to SendPlayMessage makes browsing the source code easier.
 *
 *  Revision 1.18  2002/06/01 06:06:48  dick
 *  Encapsulate (almost) everything.  Get rid of (almost) all refs to theWorld.
 *
 *  Revision 1.17  2002/05/22 05:09:01  dick
 *  Update to XPilot r4_5_4alpha.
 *
 *  Revision 1.16  2002/05/18 22:34:15  dick
 *  Move showtime to common.
 *
 *  Revision 1.15  2002/05/18 20:55:39  dick
 *  Update to XPilot-4.5.1
 *
 *  Revision 1.14  2001/08/14 21:38:56  dick
 *  Log when a player gets operator status.  Notify everyone too.
 *
 *  Revision 1.13  2001/08/10 08:11:37  dick
 *  New class ConnectionPlayer subclasses Connection to provide player
 *  functions.  New class Frame subclasses ConnectionPlayer which provides
 *  Frame update functionality.
 *
 *  Revision 1.12  2001/08/09 07:43:00  dick
 *  SetMessage becomes member function of Player.
 *
 *  Revision 1.11  2001/07/30 01:19:55  dick
 *  Encapsulate the last few options (Strings)
 *
 *  Revision 1.10  2001/07/24 09:04:00  dick
 *  Remove all global options.  Use World.options instead.
 *
 *  Revision 1.9  2001/07/23 21:30:33  dick
 *  Can't pass String through ...
 *
 *  Revision 1.8  2001/07/23 07:38:42  dick
 *  Fold contact, metaserver, netserver, rules, and server to be members of World.
 *
 *  Revision 1.7  2001/07/22 08:20:39  dick
 *  GetOptionValue becomes GetValue
 *
 *  Revision 1.6  2001/07/19 07:01:48  dick
 *  Integrate OptOrigin into our ServerOption.
 *  stringvar becomes varString, intvar becomes varInt, etc.
 *
 *  Revision 1.5  2001/07/19 04:02:35  dick
 *  Working on encapsulating options.
 *
 *  Revision 1.4  2001/07/12 10:01:02  dick
 *  Go_home() becomes GoHome()
 *
 *  Revision 1.3  2001/07/08 06:40:14  dick
 *  C++ conversion stuff.
 *
 *  Revision 1.2  2001/07/07 12:00:42  dick
 *  Rename classes to C++ "Style".  old World becomes theWorld.
 *
 */
#include "StdAfx.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#ifndef _WINDOWS
# include <unistd.h>
#endif

#include "version.h"
#include "config.h"
#include "serverconst.h"
#include "global.h"
#include "proto.h"
#include "showtime.h"
#include "error.h"
#include "commonproto.h"
#include "Frame.h"
#include "score.h"


char command_version[] = VERSION;




static int Get_player_index_by_name(World* w, char *name)
{
	int 				i, j, len;

	if (!name || !*name) {
		return -1;
	}

	/* Id given directly */
	if (isdigit(*name)) {
		i = atoi(name);
		if ((i > 0 && i <= NUM_IDS)
			&& (j = w->getInd[i]) >= 0
			&& j < w->numPlayers
			&& w->players[j]->id == i) {
			return j;
		}
		else {
			return -1;
		}
	}

	/* look for an exact match on player nickname. */
	for (i = 0; i < w->numPlayers; i++) {
		if (strcasecmp(w->players[i]->name, name) == 0) {
			return i;
		}
	}

	/* now look for a partial match on both nick and realname. */
	len = strlen(name);
	for (j = -1, i = 0; i < w->numPlayers; i++) {
		if (strncasecmp(w->players[i]->name, name, len) == 0
			|| strncasecmp(w->players[i]->realname, name, len) == 0) {
			j = (j == -1) ? i : -2;
		}
	}

	return j;
}


static void Send_info_about_player(Player * pl)
{
	int 				i;
		World*			w = pl->world;

	for (i = 0; i < w->numPlayers; i++) {
		if (w->players[i]->conn) {
			w->players[i]->conn->SendPlayer(pl);
			w->players[i]->conn->SendScore(pl);
			w->players[i]->conn->SendBase(pl->id, pl->home_base);
		}
	}
}


#define CMD_RESULT_SUCCESS				0
#define CMD_RESULT_ERROR				(-1)
#define CMD_RESULT_NOT_OPERATOR 		(-2)
#define CMD_RESULT_NO_NAME				(-3)


static int Cmd_help(char *arg, Player *pl, int oper, char *msg);
static int Cmd_team(char *arg, Player *pl, int oper, char *msg);
static int Cmd_ally(char* arg, Player* pl, int oper, char* msg);
static int Cmd_version(char *arg, Player *pl, int oper, char *msg);
static int Cmd_lock(char *arg, Player *pl, int oper, char *msg);
static int Cmd_password(char *arg, Player *pl, int oper, char *msg);
static int Cmd_pause(char *arg, Player *pl, int oper, char *msg);
static int Cmd_reset(char *arg, Player *pl, int oper, char *msg);
static int Cmd_set(char *arg, Player *pl, int oper, char *msg);
static int Cmd_kick(char *arg, Player *pl, int oper, char *msg);
static int Cmd_queue(char *arg, Player *pl, int oper, char *msg);
static int Cmd_advance(char *arg, Player *pl, int oper, char *msg);
static int Cmd_get(char *arg, Player *pl, int oper, char *msg);


typedef struct {
	const char			*name;
	const char			*abbrev;
	const char			*help;
	int 				oper_only;
	int 				(*cmd)(char *arg, Player *pl, int oper, char *msg);
} Command_info;

/*
 * A list of all of the commands sorted alphabetically
 */
static Command_info commands[] = {
	{
		"advance",
		"ad",
		"/advance <name of player in the queue>. "
		"Move the player to the front of the queue.  (operator)",
		1,
		Cmd_advance
	},
		{
				"ally",
				"al",
				"/ally {invite|cancel|refuse|accept|leave|list} [<player name>]. "
				"Manages alliances and invitations for them",
				0,
				Cmd_ally
		},
	{
		"get",
		"g",
		"/get <option>.  Gets a server option.",
		0,
		Cmd_get
	},
	{
		"help",
		"h",
		"Print command list.  /help <command> gives more info.",
		0,
		Cmd_help
	},
	{
		"kick",
		"k",
		"/kick <player name or ID number>.  Remove a player from game.  (operator)",
		1,
		Cmd_kick
	},
	{
		"lock",
		"l",
		"Just /lock tells lock status.  /lock 1 locks, /lock 0 unlocks.  (operator)",
		0,		/* checked in the function */
		Cmd_lock
	},
	{
		"password",
		"pas",
		"/password <string>.  If string matches -password option "
		"gives operator status.",
		0,
		Cmd_password
	},
	{
		"pause",
		"pau",
		"/pause <player name or ID number>.  Pauses player.  (operator)",
		1,
		Cmd_pause
	},
	{
		"queue",
		"q",
		"/queue.  Show the names of players waiting to enter.",
		0,
		Cmd_queue
	},
	{
		"reset",
		"r",
		"Just /reset re-starts the round. "
		"/reset.  Resets all scores to 0.  (operator)",
		1,
		Cmd_reset
	},
	{
		"set",
		"s",
		"/set <option> <value>.  Sets a server option.  (operator)",
		1,
		Cmd_set
	},
	{
		"team",
		"t",
		"/team <team number> swaps you to given team.",
		0,
		Cmd_team
	},
	{
		"version",
		"v",
		"Print server version.",
		0,
		Cmd_version
	},
};


/*
 * cmd parameter has no leading slash.
 */
void Handle_player_command(Player *pl, char *cmd)
{
	int 				i, result;
	char				*args;
	char				msg[MSG_LEN];

	if (!cmd || !*cmd) {
		strlcpy(msg,
				"No command given.  Type /help for help.  [*Server reply*]",
				sizeof(msg));
		pl->SetMessage(msg);
		return;
	}

	args = strchr(cmd + 1, ' ');
	if (!args) {
		/* point to end of string. */
		args = cmd + strlen(cmd);
	}
	else {
		/* zero terminate cmd and advance 1 byte. */
		*args++ = '\0';
	}

	for (i = 0; i < NELEM(commands); i++) {
		if (!strncasecmp(cmd, commands[i].abbrev, strlen(commands[i].abbrev))) {
			break;
		}
	}
	if (i == NELEM(commands)) {
		sprintf(msg, "Unknown command '%s'.  [*Server reply*]", cmd);
		pl->SetMessage(msg);
		return;
	}

	msg[0] = '\0';
	result = (*commands[i].cmd)(args, pl, pl->isoperator, msg);
	switch (result) {
	case CMD_RESULT_SUCCESS:
		break;

	case CMD_RESULT_ERROR:
		if (msg[0] == '\0') {
			strcpy(msg, "ErrOr.");
		}
		break;

	case CMD_RESULT_NOT_OPERATOR:
		if (msg[0] == '\0') {
			strlcpy(msg,
					"You need operator status to use this command.",
					sizeof(msg));
		}
		break;

	case CMD_RESULT_NO_NAME:
		if (msg[0] == '\0') {
			strlcpy(msg,
					"You must give a player name as an argument.",
					sizeof(msg));
		}
		break;

	default:
		strcpy(msg, "Bug.");
		break;
	}

	if (msg[0]) {
		strlcat(msg, " [*Server reply*]", sizeof(msg));
		pl->SetMessage(msg);
	}
}


static int Cmd_advance(char *arg, Player *pl, int oper, char *msg)
{
	int 				result;

	if (!oper) {
		return CMD_RESULT_NOT_OPERATOR;
	}

	if (!arg || !*arg) {
		return CMD_RESULT_NO_NAME;
	}

	result = pl->world->QueueAdvancePlayer(arg, msg);

	if (result < 0) {
		return CMD_RESULT_ERROR;
	}

	return CMD_RESULT_SUCCESS;
}

static int Cmd_queue(char *arg, Player *pl, int oper, char *msg)
{
	int 				result;

	result = pl->world->QueueShowList(msg);

	if (result < 0) {
		return CMD_RESULT_ERROR;
	}

	return CMD_RESULT_SUCCESS;
}


static int Cmd_team(char *arg, Player *pl, int oper, char *msg)
{
		World*			w = pl->world;
	int 				ind = w->getInd[pl->id];
	int 				i;
	int 				team;
	int 				swap_allowed;

	/*
	 * Assume nothing will be said or done.
	 */
	msg[0] = '\0';
	swap_allowed = false;
	team = pl->team;

	if (!BIT(w->rules->mode, TEAM_PLAY)) {
		sprintf(msg, "No team play going on.");
	}
	else if (pl->team >= MAX_TEAMS) {
		sprintf(msg, "You do not currently have a team.");
	}
	else if (!arg) {
		sprintf(msg, "No team specified.");
	}
	else if (!isdigit(*arg)) {
		sprintf(msg, "Invalid team specification.");
	}
	else {
		team = atoi(arg);
		if (team < 0 || team >= MAX_TEAMS) {
			sprintf(msg, "Team %d is not a valid team.", team);
		}
		else if (team == pl->team) {
			sprintf(msg, "You already are on team %d.", team);
		}
		else if (w->teams[team].numBases == 0) {
			sprintf(msg, "There are no bases for team %d on this map.", team);
		}
		else if (w->options.reserveRobotTeam->GetBool() &&
						 w->options.robotTeam->GetInt() == team) {
			sprintf(msg, "You cannot join the robot team on this server.");
		}
		else if (w->teams[team].numBases <= w->teams[team].numMembers) {
			sprintf(msg, "Team %d is full.", team);
		}
		else {
			swap_allowed = true;
		}
	}

	if (swap_allowed != true) {
		return CMD_RESULT_ERROR;
	}

	sprintf(msg, "%s has swapped to team %d.", pl->name, team);
	w->BroadcastPlayMessage(msg);
	if (BIT(pl->have, HAS_BALL)) {
		pl->DetachBall(-1);
	}
	w->teams[pl->team].numMembers--;
		if (w->options.teamShareScore->GetBool())
				TEAM_SCORE(w, pl->team, -(pl->score));
	pl->team = team;
	w->teams[pl->team].numMembers++;
		if (w->options.teamShareScore->GetBool())
				TEAM_SCORE(w, pl->team, pl->score);
	if (BIT(w->rules->mode, LIMITED_LIVES)) {
		for (i = 0; i < w->numPlayers; i++) {
			if (!TEAM(w, ind, i) && !BIT(w->players[i]->status, PAUSE)) {
				/* put team swapping player waiting mode. */
				if (pl->mychar == ' ') {
					pl->mychar = 'W';
				}
				pl->prev_life = pl->life = 0;
				SET_BIT(pl->status, GAME_OVER | PLAYING);
				CLR_BIT(pl->status, SELF_DESTRUCT);
				pl->count = -1;
				break;
			}
		}
	}
	// Pick_startpos(GetInd[pl->id]);
	pl->PickStartpos();
	Send_info_about_player(pl);
	strcpy(msg, "");

	return CMD_RESULT_SUCCESS;
}


static int Cmd_ally(char *arg, Player *pl, int oper, char *msg)
{
		World*			w = pl->world;
	char				*command;
	int 				result = CMD_RESULT_SUCCESS;
	static const char	usage[] = "Usage: "
						"/ally {invite|cancel|refuse|accept|leave|list} "
						"[<player name>]";
	static const char *cmds[] = {
				"invite",
				"cancel",
				"refuse",
				"accept",
				"leave",
				"list",
	};
	enum AllyCmds {
				AllyInvite	= 0,
				AllyCancel	= 1,
				AllyRefuse	= 2,
				AllyAccept	= 3,
				AllyLeave	= 4,
				AllyList	= 5,
				NumAllyCmds = 6
	};
	int 				i, cmd;

	if (!BIT(w->rules->mode, ALLIANCES)) {
		strlcpy(msg, "Alliances are not allowed.", MSG_LEN);
		result = CMD_RESULT_ERROR;
	}
	else if (!arg || !(command = strtok(arg, " \t"))) {
		strlcpy(msg, usage, MSG_LEN);
		result = CMD_RESULT_ERROR;
	}
	else {
		if ((arg = strtok(NULL, "")) != NULL) {
			while (*arg == ' ') {
				++arg;
			}
		}
		cmd = -1;
		for (i = 0; i < NumAllyCmds; i++) {
			if (!strncasecmp(cmds[i], command, strlen(command))) {
				cmd = (cmd == -1) ? i : (-2);
			}
		}
		if (cmd < 0) {
			strlcpy(msg, usage, MSG_LEN);
			result = CMD_RESULT_ERROR;
		}
		else if (arg) {
			/* a name is specified */
			int i = Get_player_index_by_name(w, arg);
				Player* inv = w->players[i];
			if (i >= 0) {
				if (cmd == AllyInvite) {
					inv->InvitePlayer(w->getInd[pl->id]);
				}
				else if (cmd == AllyRefuse) {
					inv->RefuseAlliance(w->getInd[pl->id]);
				}
				else if (cmd == AllyAccept) {
					inv->AcceptAlliance(w->getInd[pl->id]);
				}
				else {
					strlcpy(msg, usage, MSG_LEN);
					result = CMD_RESULT_ERROR;
				}
			} else {
				if (i == -1) {
					sprintf(msg, "Name does not match any player.");
				}
				else if (i == -2) {
					sprintf(msg, "Name matches several players.");
				}
				else {
					sprintf(msg, "Error.");
				}
				result = CMD_RESULT_ERROR;
			}
		} else {
			/* no player name is specified */
			if (cmd == AllyCancel) {
						//Cancel_invitation(w->getInd[pl->id]);
						pl->CancelInvitation();
			}
			else if (cmd == AllyRefuse) {
						//Refuse_all_alliances(w->getInd[pl->id]);
						pl->RefuseAllAlliances();
			}
			else if (cmd == AllyAccept) {
						//Accept_all_alliances(w->getInd[pl->id]);
						pl->RefuseAllAlliances();
			}
			else if (cmd == AllyLeave) {
						//Leave_alliance(w->getInd[pl->id]);
						pl->LeaveAlliance();
			}
			else if (cmd == AllyList) {
						//Alliance_player_list(w->getInd[pl->id]);
						pl->AlliancePlayerList();
			}
			else {
				strlcpy(msg, usage, MSG_LEN);
				result = CMD_RESULT_ERROR;
			}
		}
	}
	return result;
}

static int Cmd_kick(char *arg, Player *pl, int oper, char *msg)
{
		int 					i;
		World*			w = pl->world;

		if (!oper) {
				return CMD_RESULT_NOT_OPERATOR;
		}

		if (!arg || !*arg) {
				return CMD_RESULT_NO_NAME;
		}

		i = Get_player_index_by_name(w, arg);
		if (i >= 0) {
				sprintf(msg, "%s kicked %s out! [*Server notice*]",
				pl->name, w->players[i]->name);
				if (!w->players[i]->conn) {
						DeletePlayer(w, i);
				}
				else {
						w->players[i]->conn->DestroyConnection("kicked out");
				}
				w->BroadcastPlayMessage(msg);
				strcpy(msg, "");
				return CMD_RESULT_SUCCESS;
		}
		else if (i == -1) {
		sprintf(msg, "Name does not match any player.");
		}
		else if (i == -2) {
		sprintf(msg, "Name matches several players.");
		}
		else {
		sprintf(msg, "Error.");
		}

		return CMD_RESULT_ERROR;
}


static int Cmd_version(char *arg, Player *pl, int oper, char *msg)
{
	sprintf(msg, "XPilot version %s.", VERSION);
	return CMD_RESULT_SUCCESS;
}


static int Cmd_help(char *arg, Player *pl, int oper, char *msg)
{
	int 				i;

	if (!*arg) {
		strcpy(msg, "Commands: ");
		for(i = 0; i < NELEM(commands); i++) {
			strcat(msg, commands[i].name);
			strcat(msg, " ");
		}
	}
	else {
		for (i = 0; i < NELEM(commands); i++) {
			if (!strncasecmp(arg, commands[i].name,
							 strlen(commands[i].abbrev))) {
				break;
			}
		}
		if (i == NELEM(commands)) {
			sprintf(msg, "No help for nonexistent command '%s'.", arg);
		}
		else {
			strcpy(msg, commands[i].help);
		}
	}

	return CMD_RESULT_SUCCESS;
}


static int Cmd_reset(char *arg, Player *pl, int oper, char *msg)
{
	int 				i;
		World*			w = pl->world;

	if (!oper) {
		return CMD_RESULT_NOT_OPERATOR;
	}

	if (arg && !strcasecmp(arg, "all")) {
		for (i = w->numPlayers - 1; i >= 0; i--) {
			w->players[i]->score = 0;
		}
		for (i=0; i< MAX_TEAMS; i++) {
				w->teams[i].score = 0;
		}
		ResetAllPlayers(w);
		if (w->options.gameDuration->GetDouble() == -1) {
			w->options.gameDuration->Set(0);
		}
		w->roundsPlayed = 0;

		sprintf(msg, " < Total reset by %s! >", pl->name);
		w->BroadcastPlayMessage(msg);
		strcpy(msg, "");
	}
	else {
		ResetAllPlayers(w);
		if (w->options.gameDuration->GetDouble() == -1) {
			w->options.gameDuration->Set(0);
		}

		sprintf(msg, " < Round reset by %s! >", pl->name);
		w->BroadcastPlayMessage(msg);
		strcpy(msg, "");
	}

	return CMD_RESULT_SUCCESS;
}


static int Cmd_password(char *arg, Player *pl, int oper, char *msg)
{
	World*	w = pl->world;

	if (!w->options.password->GetString().GetLength() ||
		!arg || strcmp(arg, w->options.password->GetString()))
	{
		strcpy(msg, "Wrong.");
		if (pl->isoperator)
		{
			pl->isoperator = 0;
			strcat(msg, "  You lost operator status.");
		}
	}
	else
	{
		if (!pl->isoperator)
			pl->isoperator = 1;
		//strcpy(msg, "You got operator status.");
	    sprintf(msg, " < %s has operator status >", pl->name);
	    w->BroadcastPlayMessage(msg);
	    strcpy(msg, "");
		xpprintf("%s%s has operator status\n", showtime(), pl->name);
	}
	return CMD_RESULT_SUCCESS;
}


static int Cmd_lock(char *arg, Player *pl, int oper, char *msg)
{
	bool						new_lock;

	if (!arg || !*arg) {
		sprintf(msg, "The game is currently %s.",
				pl->world->gameLocked ? "locked" : "unlocked");
		return CMD_RESULT_SUCCESS;
	}

	if (!oper) {
		return CMD_RESULT_NOT_OPERATOR;
	}

	if (!strcmp(arg, "1")) {
		new_lock = 1;
	}
	else if (!strcmp(arg, "0")) {
		new_lock = 0;
	}
	else {
		sprintf(msg, "Invalid argument '%s'.  Specify either 0 or 1.", arg);
		return CMD_RESULT_ERROR;
	}

	if (new_lock == pl->world->gameLocked) {
		sprintf(msg, "Game is already %s.",
				pl->world->gameLocked ? "locked" : "unlocked");
	}
	else {
		pl->world->gameLocked = new_lock;
		sprintf(msg, " < The game has been %s by %s! >",
				pl->world->gameLocked ? "locked" : "unlocked",
				pl->name);
		pl->world->BroadcastPlayMessage(msg);
		strcpy(msg, "");
	}

	return CMD_RESULT_SUCCESS;
}


static int Cmd_set(char *arg, Player *pl, int oper, char *msg)
{
	int 		i;
	char*		option;
	String		value;

	if (!oper) {
		return CMD_RESULT_NOT_OPERATOR;
	}

	if (!arg
		|| !(option = strtok(arg, " "))
		|| !(value = strtok(NULL, " "))) {

		sprintf(msg, "Usage: /set option value.");
		return CMD_RESULT_ERROR;
	}

	i = pl->world->options.TuneOption(option, value);
	if (i == 1) {
		if (!strcasecmp(option, "password"))
			sprintf(msg, "Operation successful.");
		else {
			pl->world->options.GetValue(option, value);
			sprintf(msg, " < Option %s set to %s by %s. >",
					option, (PCSTR)value, pl->name);
			pl->world->BroadcastPlayMessage(msg);
			strcpy(msg, "");
				ServerOption* so = pl->world->options.FindOptionByName(option);
			pl->world->BroadcastOption(so);
			return CMD_RESULT_SUCCESS;
		}
	}
	else if (i == 0) {
		sprintf(msg, "Invalid value.");
	}
	else if (i == -1) {
		sprintf(msg, "This option cannot be changed at runtime.");
	}
	else if (i == -2) {
		sprintf(msg, "No option named '%s'.", option);
	}
	else {
		sprintf(msg, "Error.");
	}

	return CMD_RESULT_ERROR;
}


static int Cmd_pause(char *arg, Player *pl, int oper, char *msg)
{
	int 				i;
		World*			w = pl->world;

	if (!oper) {
		return CMD_RESULT_NOT_OPERATOR;
	}

	if (!arg || !*arg) {
		return CMD_RESULT_NO_NAME;
	}

	i = Get_player_index_by_name(w, arg);
	if (i >= 0) {
		if (w->players[i]->conn) {
			if (BIT(w->players[i]->status, PLAYING | PAUSE | GAME_OVER | KILLED)
				== PLAYING) {
				w->players[i]->Kill();
			}
			w->PausePlayer(w->players[i], 1);
			sprintf(msg, "%s was paused by %s.", w->players[i]->name, pl->name);
			w->BroadcastPlayMessage(msg);
			strcpy(msg, "");
			return CMD_RESULT_SUCCESS;
		}
	}

	sprintf(msg, "Invalid player id.");

	return CMD_RESULT_ERROR;
}

static int Cmd_get(char *arg, Player *pl, int oper, char *msg)
{
		World*	w = pl->world;
	String		value;
	bool		b;

	if (!arg || !*arg) {
		strcpy(msg, "Usage: /get option.");
		return CMD_RESULT_ERROR;
	}

	if (!strcasecmp(arg, "password") ||
		!strcasecmp(arg, "mapData")) {
		strcpy(msg, "Cannot retrieve that option.");
		return CMD_RESULT_ERROR;
	}
	if (!(value = (char*)malloc(MAX_CHARS))) {
		strcpy(msg, "Out of memory.");
		return CMD_RESULT_ERROR;
	}

	b = w->options.GetValue(arg, value);

		if (b)
		{
				sprintf(msg, "The value of %s is %s.", arg, (PCSTR)value);
				return CMD_RESULT_SUCCESS;
		}
		sprintf(msg, "No option named %s.", arg);

	return CMD_RESULT_ERROR;
}
