/* $Id: packet.h,v 1.12 2007/02/12 07:49:02 dick Exp $
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
 * $Log: packet.h,v $
 * Revision 1.12  2007/02/12 07:49:02  dick
 * Define PKT_ROBOT_WATCHDECO which displays stuff on the playfield.  Bump version to 0x5011.
 *
 * Revision 1.11  2007/01/18 20:59:42  dick
 * Increase MAX_RELIABLE_DATA_PACKET_SIZE from 1024 to 2048
 * to accomodate the RobotWatch messages which can be big.
 * Will this break backwards compatibility?
 *
 * Revision 1.10  2007/01/17 08:56:53  dick
 * PKT_ROBOT_WATCH sends brain strings to the client
 *
 * Revision 1.9  2004/05/23 23:52:07  dick
 * src/common/IniClient.Defaults.cpp
 *
 * Revision 1.8  2004/04/26 07:07:14  dick
 * Define a list of pages that can be displayed on the ScoreTable in the client.
 * Each page can be controlled by the server (and the server admin tool).
 *
 */

#ifndef PACKET_H
#define PACKET_H

//#define MAX_RELIABLE_DATA_PACKET_SIZE	1024
#define MAX_RELIABLE_DATA_PACKET_SIZE	2048


/* before version 3.8.0 this was 8 bytes. */
#define KEYBOARD_SIZE		9

/*
 * Definition of various client/server packet types.
 */

/* packet types: 0 - 9 */
#define PKT_UNDEFINED		 0	// 00
#define PKT_VERIFY			 1	// 01
#define PKT_REPLY			 2	// 02
#define PKT_PLAY			 3	// 03
#define PKT_QUIT			 4	// 04
#define PKT_MESSAGE			 5	// 05
#define PKT_START			 6	// 06
#define PKT_END				 7	// 07
#define PKT_SELF			 8	// 08
#define PKT_DAMAGED			 9	// 09

/* packet types: 10 - 19 */
#define PKT_CONNECTOR		10	// 0A
#define PKT_REFUEL			11	// 0B
#define PKT_SHIP			12	// 0C
#define PKT_ECM				13	// 0D
#define PKT_PAUSED			14	// 0E
#define PKT_ITEM			15	// 0F
#define PKT_MINE			16	// 10
#define PKT_BALL			17	// 11
#define PKT_MISSILE			18	// 12
#define PKT_SHUTDOWN		19	// 13

/* packet types: 20 - 29 */
#define PKT_STRING			20	// 14
#define PKT_DESTRUCT		21	// 15
#define PKT_RADAR			22	// 16
#define PKT_TARGET			23	// 17
#define PKT_KEYBOARD		24	// 18
#define PKT_SEEK			25	// 19
#define PKT_SELF_ITEMS		26	// 1A /* still under development */
#define PKT_TEAM_SCORE		27	// 1B /* was PKT_SEND_BUFSIZE */
#define PKT_PLAYER			28	// 1C
#define PKT_SCORE			29	// 1D

/* packet types: 30 - 39 */
#define PKT_FUEL			30	// 1E
#define PKT_BASE			31	// 1F
#define PKT_CANNON			32	// 20
#define PKT_LEAVE			33	// 21
#define PKT_POWER			34	// 22
#define PKT_POWER_S			35	// 23
#define PKT_TURNSPEED		36	// 24
#define PKT_TURNSPEED_S		37	// 25
#define PKT_TURNRESISTANCE	38	// 26
#define PKT_TURNRESISTANCE_S 39	// 27

/* packet types: 40 - 49 */
#define PKT_WAR				40	// 28
#define PKT_MAGIC			41	// 29
#define PKT_RELIABLE		42	// 2A
#define PKT_ACK				43	// 2B
#define PKT_FASTRADAR		44	// 2C
#define PKT_TRANS			45	// 2D
#define PKT_ACK_CANNON		46	// 2E
#define PKT_ACK_FUEL		47	// 2F
#define PKT_ACK_TARGET		48	// 30
#define	PKT_SCORE_OBJECT	49	// 31

/* packet types: 50 - 59 */
#define PKT_AUDIO			50	// 32
#define PKT_TALK			51	// 33
#define PKT_TALK_ACK		52	// 34
#define PKT_TIME_LEFT		53	// 35
#define PKT_LASER			54	// 36
#define PKT_DISPLAY			55	// 37
#define PKT_EYES			56	// 38
#define PKT_SHAPE			57	// 39
#define PKT_MOTD			58	// 3A
#define PKT_LOSEITEM		59	// 3B

/* packet types: 60 - 69 */
#define PKT_AUDIO2			60	// 3C
#define PKT_NOT_USED_61		61	// 3D
#define PKT_NOT_USED_62		62	// 3E
#define PKT_NOT_USED_63		63	// 3F
#define PKT_NOT_USED_64		64	// 40
#define PKT_NOT_USED_65		65	// 41
#define PKT_NOT_USED_66		66	// 42
#define PKT_NOT_USED_67		67	// 43
#define PKT_MODIFIERS		68	// 44
#define PKT_FASTSHOT		69	// 45 /* replaces SHOT/TEAMSHOT */

/* packet types: 70 - 79 */
#define PKT_THRUSTTIME		70	// 46
#define PKT_MODIFIERBANK	71	// 47
#define PKT_SHIELDTIME		72	// 48
#define PKT_POINTER_MOVE	73	// 49
#define PKT_REQUEST_AUDIO	74	// 4A
#define PKT_ASYNC_FPS		75	// 4B
#define PKT_TIMING			76	// 4C
#define PKT_PHASINGTIME		77	// 4D
#define PKT_ROUNDDELAY		78	// 4E
#define PKT_WRECKAGE		79	// 4F

/* packet types: 80 - 89 */
#define PKT_ASTEROID		80	// 50
#define PKT_WORMHOLE		81	// 51
#define PKT_CTL				82	// 52
#define PKT_NOP				83	// 53	/* Keep alive ping no-op */
#define PKT_COOKIE			84	// 54
#define PKT_SCORE_TABLE		85	// 55
#define PKT_ROBOT_WATCH		86	// 56
#define PKT_ROBOT_WATCHDECO	87	// 57
#define PKT_NOT_USED_88		88	// 58
#define PKT_NOT_USED_89		89	// 59

/* packet types: 90 - 99 */
/*
 * Use these 10 packet type numbers for
 * experimenting with new packet types.
 */

/* status reports: 101 - 102 */
#define PKT_FAILURE			101
#define PKT_SUCCESS			102

/* optimized packet types: 128 - 255 */
#define PKT_DEBRIS			128		/* + color + x + y */

#endif

