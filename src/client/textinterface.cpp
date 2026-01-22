/* $Id: textinterface.cpp,v 1.3 2004/06/03 05:46:43 dick Exp $
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
 */
/*
 * $Log: textinterface.cpp,v $
 * Revision 1.3  2004/06/03 05:46:43  dick
 * Retab
 *
 * Revision 1.2  2004/03/27 16:38:50  dick
 * Kristian Soederblom submitted a patch to allow xp5 clients to join ng servers.
 *
 * Revision 1.1  2004/03/24 18:11:27  dick
 * I was playing with StatCvs and it won't work on the XPilot5 repo because
 * this file is broken.  The server reports "linefeed expected" when trying
 * to do a `cvs log`.  Eyeballing the repo file looks ok and it does
 * `cvs update` ok too.  So i moved the original to the Attic as
 * textinterfaceBrokenCvsServerFile.cpp,v and recreated the last revision
 * in the repo.
 *
 * Revision 1.18  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.17  2004/01/08 18:31:17  dick
 * Return E_TOOMANYIPS if that is the case.
 *
 * Revision 1.16  2004/01/06 04:48:27  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 * Revision 1.15  2002/09/16 22:17:35  dick
 * Stringize conpar usage
 *
 * Revision 1.14  2002/09/16 21:42:05  dick
 * Use Stringized Connectparam.
 *
 * Revision 1.13  2002/08/01 15:01:27  dick
 * Send the cookie with the verify message to the server. Retab.
 *
 * Revision 1.12  2002/07/15 21:54:36  bert
 * these files all had file corruptions at several places.
 * mostly two adjacent bytes were replaced with bogus random data.
 * fixed.
 *
 * Revision 1.11  2002/06/26 06:47:03  dick
 * Replace errno with xperrno and errno=0 with seterrno(0).
 *
 * Revision 1.10  2002/05/18 20:55:33  dick
 * Update to XPilot-4.5.1
 *
 * Revision 1.9  2001/08/31 20:48:32  dick
 * Don't warn windows about failed contact messages
 *
 * Revision 1.8  2001/08/14 21:48:26  dick
 * Contact_servers returns a bool
 *
 * Revision 1.7  2001/08/12 11:18:58  dick
 * Pretty code.
 *
 * Revision 1.6  2001/08/07 22:14:22  dick
 * usleep becomes microdelay.
 *
 * Revision 1.5  2001/08/07 21:01:07  dick
 * struct Connect_param_t becomes class Connectparam
 *
 * Revision 1.4  2001/08/06 05:41:49  dick
 * struct sock_t becomes class Sock
 *
 * Revision 1.3  2001/07/29 06:01:46  dick
 * net.cpp becomes a very nice class Sockbuf.
 *
 * Revision 1.2  2001/07/27 05:47:39  dick
 * Client builds and runs in C++
 */
#include "StdAfx.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>

#if defined(_UNIX) || defined(_CYGWIN)
# include <unistd.h>
# include <sys/param.h>
# include <netdb.h>
# ifndef __hpux
#  include <sys/time.h>
# endif
#else
# include "NT/winNet.h"
# include "NT/winClient.h"
#endif

#include "version.h"
#include "config.h"
#include "const.h"
#include "types.h"
#include "pack.h"
#include "bit.h"
#include "error.h"
#include "Sock.h"
#include "Sockbuf.h"
#include "datagram.h"
#include "portability.h"
#include "checknames.h"
#include "Connectparam.h"
#include "protoclient.h"
#include "commonproto.h"


char textinterface_version[] = VERSION;


#define MAX_LINE	MSG_LEN	/* should not be smaller than MSG_LEN */


extern int		dgram_one_socket;	/* from datagram.c */
extern String	hostname;			/* my local hostname */



/*
 * just like fgets() but strips newlines like gets().
 */
static char* getline(char* buf, int len, FILE* stream)
{
	char				*nl;

	if (fgets(buf, len, stream)) {
		nl = strchr(buf, '\n');
		if (nl) {
			*nl = '\0'; 		/* strip newline */
			if (nl > buf && nl[-1] == '\r') {
				nl[-1] = '\0';
			}
		}
	}
	return buf;
}


/*
 * Replace control characters with a space.
 */
static void Clean_string(char *buf)
{
	char				*str;
	int 				c;

	for (str = buf; *str; str++) {
		c = (*str & 0xFF);
		if (!isascii(c) || iscntrl(c)) {
			if (!strchr("\r\n", c)) {
				*str = ' ';
			}
		}
	}
}


static bool Get_contact_message(Sockbuf* sbuf,
								const char *contact_server,
								Connectparam* conpar)
{
	int 				len;
	int 				version, allow;
	unsigned			magic;
	unsigned char		reply_to, status;
	bool				readable = false;

	sbuf->sock.SetTimeout(2, 0);
	while (readable == false && sbuf->sock.Readable() > 0) {

		sbuf->Clear();
		len = sbuf->sock.ReceiveAny(sbuf->buf, sbuf->size);
		if (len <= 0) {
			if (len == 0) {
				continue;
			}
			IFNWINDOWS(warn("Error from sock_receive_any, contact message failed.\n");)
			/* exit(1);  no good since meta gui. */
			return false;
		}
		sbuf->len = len;

		/*
		 * Get server's host and port.
		 */
		conpar->serverAddr = sbuf->sock.GetLastAddr();
		conpar->serverPort = sbuf->sock.GetLastPort();
		/*
		 * If the name of the server to contact is the same as its
		 * IP address then we don't want to do a reverse lookup.
		 * Doing a reverse lookup may result in a long and annoying delay.
		 */
		if (!strcmp(conpar->serverAddr, contact_server)) {
			conpar->serverName = conpar->serverAddr;
		} else {
			conpar->serverName = sbuf->sock.GetLastName();
		}

		if (sbuf->scanf("%u%c%c", &magic, &reply_to, &status) <= 0) {
			seterrno(0);
			error("Incomplete contact reply message (%d)", len);
		}
		else if ((magic & 0xFFFF) != (MAGIC & 0xFFFF)) {
			seterrno(0);
			error("Bad magic on contact message (0x%x).", magic);
		}
		else {
			version = MAGIC2VERSION(magic);
			if (version > 0x4501 && version < 0x5000)
				version = 0x4501;
			allow = version;
			LIMIT(allow, MIN_SERVER_VERSION, MAX_SERVER_VERSION);
			if (version != allow) {
				printf("Incompatible version with server %s.\n", (PCSTR)conpar->serverName);
				printf("We run version %04x, while server is running %04x.\n",
				MY_VERSION, MAGIC2VERSION(magic));
				if ((MY_VERSION >> 4) < (MAGIC2VERSION(magic) >> 4)) {
						printf("Time for us to upgrade?\n");
				}
			}
			else {
				/*
				 * Found one which we can talk to.
				 */
				conpar->serverVersion = version;
				readable = true;
			}
		}
	}

	return readable;
}



static int Get_reply_message(Sockbuf* ibuf,
							 Connectparam *conpar)
{
	int 				len;
	unsigned			magic;


	if (ibuf->sock.Readable()) {
		ibuf->Clear();
		if ((len = ibuf->sock.Read(ibuf->buf, ibuf->size)) == -1) {
			error("Can't read reply message from %s/%d",
				  (PCSTR)conpar->serverAddr, conpar->serverPort);
			exit(1);
		}

		ibuf->len = len;
		if (ibuf->scanf("%u", &magic) <= 0) {
				seterrno(0);
			error("Incomplete reply packet (%d)", len);
			return 0;
		}

		if ((magic & 0xFFFF) != (MAGIC & 0xFFFF)) {
				seterrno(0);
			error("Wrong MAGIC in reply pack (0x%x).", magic);
			return 0;
		}

		if (MAGIC2VERSION(magic) != conpar->serverVersion) {
			printf("Incompatible version with server on %s.\n",
					(PCSTR)conpar->serverName);
			printf("We run version %04x, while server is running %04x.\n",
				   MY_VERSION, MAGIC2VERSION(magic));
			return 0;
		}

		return len;
	}

	return 0;
}



static void Command_help(void)
{
	printf("Supported commands are:\n"
		   "H/?  -   Help - this text.\n"
		   "N    -   Next server, skip this one.\n"
		   "S    -   list Status.\n"
		   "T    -   set Team.\n"
		   "Q    -   Quit.\n"
		   "K    -   Kick a player.                (only owner)\n"
		   "M    -   send a Message.               (only owner)\n"
		   "L    -   Lock/unLock server access.    (only owner)\n"
		   "D(*) -   shutDown/cancel shutDown.     (only owner)\n"
		   "R(#) -   set maximum number of Robots. (only owner)\n"
		   "O    -   Modify a server option.       (only owner)\n"
		   "V    -   View the server options.\n"
		   "J(&) or just Return enters the game.\n"
		   "(*) If you don't specify any delay, you will signal that\n"
		   "    the server should stop an ongoing shutdown.\n"
		   "(#) Not specifying the maximum number of robots is\n"
		   "    the same as specifying 0 robots.\n"
		   "(&) You may specify a team number after the J.\n"
		  );
}



/*
 * This is the routine that interactively (if not auto_connect) prompts
 * the user on his/her next action.  Returns true if player joined this
 * server (connected to server), or false if the player wants to have a
 * look at the next server.
 */
static bool Process_commands(Sockbuf* ibuf,
							 int auto_connect, int list_servers,
							 int auto_shutdown, char *shutdown_reason,
							 Connectparam* conpar)
{
	int 				i, len, retries, delay, max_robots, success;
	char				c, status, reply_to;
	char				linebuf[MAX_LINE];
	unsigned short		port, qpos;
	int 				has_credentials = 0;
	int 				cmd_credentials = 0;
	int 				privileged_cmd;
	int 				max_replies;
	long				key = 0;
	time_t				qsent = 0;
	static char 		localhost[] = "127.0.0.1";

#if defined(_WINDOWS) && !defined(_CYGWIN)
	auto_connect = TRUE;		/* I want to join */
	auto_shutdown = FALSE;
#endif

	if (auto_connect && !list_servers && !auto_shutdown) {
		xpprintf("*** Connected to %s\n", (PCSTR)conpar->serverName);
	}

	for (;;) {

		max_replies = 1;

		/*
		 * Now, what do you want from the server?
		 */
		if (cmd_credentials) {
			c = cmd_credentials;
			cmd_credentials = 0;
		}
		else if (!auto_connect) {
			printf("*** Server on %s. Enter command> ", (PCSTR)conpar->serverName);

			getline(linebuf, MAX_LINE-1, stdin);
			if (feof(stdin)) {
				puts("");
				c = 'Q';
			} else {
				c = linebuf[0];
				if (c == '\0')
					c = 'J';
			}
			CAP_LETTER(c);
		} else {
			if (list_servers)
				c = 'S';
			else if (auto_shutdown)
				c = 'D';
			else
				c = 'J';
			linebuf[0] = linebuf[1] = '\0';
		}

		/*
		 * For each command to the server create a new socket and bind
		 * the socket to the server's address and destination port.
		 * This assures us that we only get replies to the last command sent.
		 */
		if (ibuf->sock.fd != SOCK_FD_INVALID) {
			close_dgram_socket(&ibuf->sock);
			ibuf->sock.fd = SOCK_FD_INVALID;
		}

		privileged_cmd = (strchr("DKLMOR", c) != NULL);
		if (privileged_cmd) {
			if (!has_credentials) {
				success = create_dgram_addr_socket(&ibuf->sock, conpar->serverAddr, 0);
				if (success == SOCK_IS_ERROR) {
					printf("Server %s is not local, privileged command not possible.\n",
						   (PCSTR)conpar->serverAddr);
					continue;
				}
				close_dgram_socket(&ibuf->sock);
			}
			if ((success = create_dgram_addr_socket(&ibuf->sock, localhost, 0)) == SOCK_IS_ERROR) {
				error("Could not create localhost socket");
				exit(1);
			}
			if (ibuf->sock.Connect(localhost, conpar->serverPort) == SOCK_IS_ERROR) {
				error("Can't connect to local server %s on port %d\n",
					  localhost, conpar->serverPort);
				return false;
			}
		} else {
			if ((success = create_dgram_socket(&ibuf->sock, 0)) == SOCK_IS_ERROR) {
				error("Could not create socket");
				exit(1);
			}
			if (ibuf->sock.Connect(conpar->serverAddr, conpar->serverPort) == SOCK_IS_ERROR
				&& !dgram_one_socket) {
				error("Can't connect to server %s on port %d\n",
					  (PCSTR)conpar->serverAddr, conpar->serverPort);
				return false;
			}
		}

		ibuf->Clear();
		ibuf->printf("%u%s%hu", VERSION2MAGIC(conpar->serverVersion),
					  (PCSTR)conpar->realName, ibuf->sock.GetPort());

		if (privileged_cmd && !has_credentials) {
			ibuf->printf("%c%ld", CREDENTIALS_pack, 0L);
		} else {

			switch (c) {

				/*
				 * Owner only commands:
				 */

			case 'K':
				printf("Enter name of victim: ");
				fflush(stdout);
				if (!getline(linebuf, MAX_LINE-1, stdin)) {
					printf("Nothing changed.\n");
					continue;
				}
				linebuf[MAX_NAME_LEN - 1] = '\0';
				ibuf->printf("%c%ld%s", KICK_PLAYER_pack, key, linebuf);
				break;

			case 'R':
				printf("Enter maximum number of robots: ");
				fflush(stdout);
				if (!getline(linebuf, MAX_LINE-1, stdin)) {
					printf("Nothing changed.\n");
					continue;
				}
				if (sscanf(linebuf, "%d", &max_robots) <= 0 || max_robots < 0) {
					printf("Invalid number of robots \"%s\".\n", linebuf);
					continue;
				}
				ibuf->printf("%c%ld%d", MAX_ROBOT_pack, key, max_robots);
				break;

			case 'M':							/* Send a message to server. */
				printf("Enter message: ");
				fflush(stdout);
				if (!getline(linebuf, MAX_LINE-1, stdin) || !linebuf[0]) {
					printf("No message sent.\n");
					continue;
				}
				linebuf[MAX_CHARS - 1] = '\0';
				ibuf->printf("%c%ld%s", MESSAGE_pack, key, linebuf);
				break;

			case 'L':							/* Lock the game. */
				ibuf->printf("%c%ld", LOCK_GAME_pack, key);
				break;

			case 'D':							/* Shutdown */
				if (!auto_shutdown) {
					printf("Enter delay in seconds or return for cancel: ");
					getline(linebuf, MAX_LINE-1, stdin);
					/*
					 * No argument = cancel shutdown = arg_inar0
					 */
					if (sscanf(linebuf, "%d", &delay) <= 0) {
						delay = 0;
					} else
						if (delay <= 0)
							delay = 1;

					printf("Enter reason: ");
					getline(linebuf, MAX_LINE-1, stdin);
				} else {
					strlcpy(linebuf, shutdown_reason, sizeof(linebuf));
					delay = 60;
				}
				linebuf[MAX_CHARS - 1] = '\0';
				ibuf->printf("%c%ld%d%s", SHUTDOWN_pack, key, delay, linebuf);
				break;

			case 'O':							/* Tune an option. */
				printf("Enter option: ");
				fflush(stdout);
				if (!getline(linebuf, MAX_LINE-1, stdin)
					|| (len=strlen(linebuf)) == 0) {
					printf("Nothing changed.\n");
					continue;
				}
				printf("Enter new value for %s: ", linebuf);
				fflush(stdout);
				strcat(linebuf, ":"); len++;
				if (!getline(&linebuf[len], MAX_LINE-1-len, stdin)
					|| linebuf[len] == '\0') {
					printf("Nothing changed.\n");
					continue;
				}
				printf("option \"%s\"\n", linebuf); fflush(stdout);
				ibuf->printf("%c%ld%S", OPTION_TUNE_pack, key, linebuf);
				break;

				/*
				 * Public commands:
				 */

			case 'J':							/* Trying to enter game. */
				if (linebuf[1] == '0') {
					printf("Team '0' is reserved for robots.");
					conpar->team = TEAM_NOT_SET;
				}
				else if (linebuf[1] > '0' && linebuf[1] <= '9') {
					conpar->team = linebuf[1] - '0';
					printf("Joining team %d\n", conpar->team);
				}
				else if (linebuf[1] == '-') {
					conpar->team = TEAM_NOT_SET;
					printf("Team set to unspecified\n");
				}
				else if (linebuf[1] != '\0') {
					conpar->team = TEAM_NOT_SET;
				}
				if (conpar->serverVersion < 0x3430) {
					ibuf->printf("%c%s%s%s%d", ENTER_GAME_pack,
								  (PCSTR)conpar->nick, (PCSTR)conpar->dispName,
								  (PCSTR)hostname, conpar->team);
				} else {
					ibuf->printf("%c%s%s%s%d", ENTER_QUEUE_pack,
								  (PCSTR)conpar->nick, (PCSTR)conpar->dispName,
								  (PCSTR)hostname, conpar->team);
					time(&qsent);
				}
				break;

			case 'S':							/* Report status. */
				ibuf->printf("%c", REPORT_STATUS_pack);
				break;

			case 'V':							/* View options. */
				ibuf->printf("%c", OPTION_LIST_pack);
				max_replies = 5;
				break;

				/*
				 * User interface commands:
				 */

			case 'N':							/* Next server. */
				return false;

			case 'T':							/* Set team. */
				printf("Enter team: ");
				fflush(stdout);
				if (!getline(linebuf, MAX_LINE-1, stdin)
					|| (len = strlen(linebuf)) == 0) {
					printf("Nothing changed.\n");
				}
				else {
					int newteam;
					if (sscanf(linebuf, " %d", &newteam) != 1) {
						printf("Invalid team specification: %s.\n", linebuf);
					}
					else if (newteam >= 0 && newteam <= 9) {
						conpar->team = newteam;
						printf("Team set to %d\n", conpar->team);
					}
					else {
						conpar->team = TEAM_NOT_SET;
						printf("Team set to unspecified\n");
					}
				}
				continue;

			case 'Q':
				exit (0);
				break;

			case '?':
			case 'H':							/* Help. */
			default:
				Command_help();

				/*
				 * Next command.
				 */
				continue;
			}
		}

		retries = (c == 'J' || c == 'S') ? 2 : 0;
		for (i = 0; i <= retries; i++)
		{
				if (i > 0)
				{
						ibuf->sock.SetTimeout(1, 0);
						if (ibuf->sock.Readable())
								break;
				}
				if (ibuf->sock.Write(ibuf->buf, ibuf->len) != ibuf->len)
				{
						error("Couldn't send request to server.");
						exit(1);
				}
		}

		/*
		 * Get reply message(s).  If we failed, return false (next server).
		 */
		ibuf->sock.SetTimeout(2, 0);
		do {
			ibuf->Clear();
			if (Get_reply_message(ibuf, conpar) <= 0) {
						seterrno(0);
						error("No answer from server");
						return false;
			}
			if (ibuf->scanf("%c%c", &reply_to, &status) <= 0) {
						seterrno(0);
						error("Incomplete reply from server");
						return false;
			}

			ibuf->sock.SetTimeout(0, 500*1000);

			/*
			 * Now try and interpret the result.
			 */
				seterrno(0);
			switch (status) {

			case SUCCESS:
				/*
				 * Oh glorious success.
				 */
				switch (reply_to & 0xFF) {

				case OPTION_LIST_pack:
					while (ibuf->scanf("%S", linebuf) > 0) {
						printf("%s\n", linebuf);
					}
					break;

				case REPORT_STATUS_pack:
					/*
					 * Did the reply include a string?
					 */
					if (ibuf->len > ibuf->ptr - ibuf->buf
						&& (!auto_connect || list_servers)) {
						if (list_servers)
							printf("SERVER HOST......: %s\n", (PCSTR)conpar->serverName);
						if (*ibuf->ptr != '\0') {
							if (ibuf->len < ibuf->size) {
								ibuf->buf[ibuf->len] = '\0';
							} else {
								ibuf->buf[ibuf->size - 1] = '\0';
							}
							Clean_string(ibuf->ptr);
							printf("%s", ibuf->ptr);
							if (ibuf->ptr[strlen(ibuf->ptr) - 1] != '\n') {
								printf("\n");
							}
						}
					}
					break;

				case SHUTDOWN_pack:
					if (delay == 0) {
						puts("*** Shutdown stopped.");
					} else {
						puts("*** Shutdown initiated.");
					}
					break;

				case ENTER_GAME_pack:
					if (ibuf->scanf("%hu", &port) <= 0) {
						seterrno(0);
						error("Incomplete login reply from server");
						conpar->loginPort = -1;
					} else {
						conpar->loginPort = port;
						printf("*** Login allowed.\n");
					}
					break;

				case ENTER_QUEUE_pack:
					if (ibuf->scanf("%hu", &qpos) <= 0) {
								seterrno(0);
								error("Incomplete queue reply from server");
					} else {
								printf("... queued at position %2d\n", qpos);
								IFWINDOWS(Progress("Queued at position %2d\n", qpos);)
					}
					/*
					 * Acknowledge each 10 seconds that we are still
					 * interested to be on the waiting queue.
					 */
					if (qsent + 10 <= time(NULL)) {
						ibuf->Clear();
						ibuf->printf("%u%s%hu",
									  VERSION2MAGIC(conpar->serverVersion),
									  (PCSTR)conpar->realName, ibuf->sock.GetPort());
						ibuf->printf("%c%s%s%s%d", ENTER_QUEUE_pack,
									  (PCSTR)conpar->nick, (PCSTR)conpar->dispName,
									  (PCSTR)hostname, conpar->team);
						if (ibuf->sock.Write(ibuf->buf, ibuf->len) != ibuf->len) {
							error("Couldn't send request to server.");
							exit(1);
						}
						time(&qsent);
					}
					ibuf->sock.SetTimeout(12, 0);
					max_replies = 2;
					break;

				case CREDENTIALS_pack:
					if (ibuf->scanf("%ld", &key) <= 0) {
								seterrno(0);
								error("Incomplete credentials reply from server");
					}
					else {
								has_credentials++;
								cmd_credentials = c;
								continue;
					}
					break;

				default:
					puts("*** Operation successful.");
					break;
				}
				break;

			case E_NOT_OWNER:
				error("Permission denied, not owner");
				break;
			case E_GAME_FULL:
				error("Sorry, game full");
				break;
			case E_TEAM_FULL:
				error("Sorry, team %d is full", conpar->team);
				break;
			case E_TEAM_NOT_SET:
				error("Sorry, team play selected "
					  "and you haven't specified your team");
				break;
			case E_GAME_LOCKED:
				error("Sorry, game locked");
				break;
			case E_NOT_FOUND:
				error("That player is not logged on this server");
				break;
			case E_IN_USE:
				error("Your nick is already used");
				break;
			case E_SOCKET:
				error("Server can't setup socket");
				break;
			case E_INVAL:
				error("Invalid input parameters says the server");
				break;
			case E_VERSION:
				error("We have an incompatible version says the server");
				break;
			case E_NOENT:
				error("No such variable, says the server");
				break;
			case E_UNDEFINED:
				error("Requested operation is undefined, says the server");
				break;
			case E_TOOMANYIPS:
				error("Too many connections from your computer");
				break;
			default:
				error("Server answers with unknown error status '%02x'", status);
				break;
			}

			if (list_servers)	/* If listing servers, go to next one */
				return false;

			if (auto_shutdown)	/* Do the same if we've sent a -shutdown */
				return false;

			if (auto_connect && status != SUCCESS)
				return false;

			/*
			 * If we wanted to enter the game and we were allowed to, return
			 * true (we are done).	If we weren't allowed, either return false
			 * (get next server) if we are auto_connecting or get next command
			 * if we aren't auto_connecting (interactive).
			 */
			if (reply_to == ENTER_GAME_pack) {
				if (status == SUCCESS && conpar->loginPort > 0) {
					return true;
				} else {
					if (auto_connect)
						return false;
				}
			}

		} while (--max_replies > 0 && ibuf->sock.Readable());

		/*
		 * Get next command.
		 */
	}

	/*NOTREACHED*/
}



/*
 * Setup a socket and a buffer for client-server messages.
 * We do this again for each server to prevent getting
 * old messages from past servers.
 */
int Connect_to_server(int auto_connect, int list_servers,
					  int auto_shutdown, char *shutdown_reason,
					  Connectparam *conpar)
{
	Sockbuf 			ibuf;					/* info buffer */
	int 				result;

	if (ibuf.Init(NULL, CLIENT_RECV_SIZE,
					 SOCKBUF_READ | SOCKBUF_WRITE | SOCKBUF_DGRAM) == -1) {
		error("No memory for info buffer");
		exit(1);
	}
	result = Process_commands(&ibuf,
							 auto_connect, list_servers,
							 auto_shutdown, shutdown_reason,
							 conpar);
	if (ibuf.sock.fd != SOCK_FD_INVALID) {
		close_dgram_socket(&ibuf.sock);
		ibuf.sock.fd = SOCK_FD_INVALID;
	}
	ibuf.Cleanup();

	return result;
}


bool Contact_servers(int count, char **servers,
					int auto_connect, int list_servers,
					int auto_shutdown, char *shutdown_reason,
					int find_max, int *num_found,
					char **server_addresses, char **server_names,
					Connectparam *conpar)
{
	int 				connected = false;
	const int			max_retries = 2;
	int 				i;
	int 				status;
	Sock				sock;
	int 				retries;
	int 				contacted;
	Sockbuf 			sbuf;					/* contact buffer */


	if ((status = create_dgram_socket(&sock, 0)) == SOCK_IS_ERROR) {
		error("Could not create connection socket");
		exit(1);
	}
	if (sbuf.Init(&sock, CLIENT_RECV_SIZE,
					 SOCKBUF_READ | SOCKBUF_WRITE | SOCKBUF_DGRAM) == -1) {
		error("No memory for contact buffer");
		exit(1);
	}

	if (!count) {
		retries = 0;
		contacted = 0;
		do {
			sbuf.Clear();
			sbuf.printf("%u%s%hu%c", MAGIC,
						  (PCSTR)conpar->realName, sbuf.sock.GetPort(), CONTACT_pack);
			if (Query_all(&sbuf.sock, conpar->contactPort, sbuf.buf, sbuf.len) == -1) {
				error("Couldn't send contact requests");
				exit(1);
			}
			if (retries == 0) {
				printf("Searching for a \"xpilots\" server on the local net...\n");
				IFWINDOWS( Progress("Searching for a \"xpilots\" server on the local net..."); )
			} else {
				printf("Searching once more...\n");
				IFWINDOWS( Progress("Searching once more..."); )
			}
			while (Get_contact_message(&sbuf, "", conpar)) {
				contacted++;
				if (list_servers == 2) {
					if (count < find_max) {
						if (server_names) {
							strlcpy(server_names[count],
									conpar->serverName,
									MAX_HOST_LEN);
						}
						if (server_addresses) {
							strlcpy(server_addresses[count],
									conpar->serverAddr,
									MAX_HOST_LEN);
						}
						count++;
					}
					if (num_found) {
						*num_found = count;
					}
				} else {
					connected = Connect_to_server(auto_connect,
												  list_servers,
												  auto_shutdown,
												  shutdown_reason,
												  conpar);
					if (connected != 0) {
						break;
					}
				}
			}
		} while (!contacted && retries++ < max_retries);
	}
	else {
		for (i = 0; i < count && !connected; i++) {
			retries = 0;
			contacted = 0;
			do {
				IFWINDOWS( Progress("Contacting server %s", servers[i]); )
				sbuf.Clear();
				sbuf.printf("%u%s%hu%c", MAGIC,
							  (PCSTR)conpar->realName, sbuf.sock.GetPort(), CONTACT_pack);
				if (sbuf.sock.SendDest(servers[i],
							  conpar->contactPort,
							  sbuf.buf, sbuf.len) == -1) {
					if (sbuf.sock.sockError.call == SOCK_CALL_GETHOSTBYNAME) {
						printf("Can't find %s\n", servers[i]);
						IFWINDOWS( Progress("Can't find %s", servers[i]); )
						break;
					}
					error("Can't contact %s on port %d",
						  servers[i], conpar->contactPort);
				}
				if (retries) {
					printf("Retrying %s...\n", servers[i]);
					IFWINDOWS( Progress("Retrying %s...", servers[i]); )
				}
				if (Get_contact_message(&sbuf, servers[i], conpar)) {
					contacted++;
					IFWINDOWS( Progress("Contacted %s", servers[i]); )
					connected = Connect_to_server(auto_connect, list_servers,
												  auto_shutdown, shutdown_reason,
												  conpar);
					if (connected != 0) {
						break;
					}
				}
			} while (!contacted && retries++ < max_retries);
		}
	}
	sbuf.Cleanup();
	close_dgram_socket(&sock);

	return connected ? true : false;
}

