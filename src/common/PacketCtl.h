/* $Id: PacketCtl.h,v 1.14 2004/05/07 04:36:33 dick Exp $
 *
 * PacketCtl - Define the subcommands in a PKT_CTL
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
 * $Log: PacketCtl.h,v $
 * Revision 1.14  2004/05/07 04:36:33  dick
 * Get rank data from the scoreserver to the server to the client
 *
 * Revision 1.13  2004/02/05 04:47:44  dick
 * New command ServerInfo so the scoreserver can get our hostname
 *
 * Revision 1.12  2004/01/15 05:38:25  dick
 * Add a StartScoreServer button to the local panel
 *
 * Revision 1.11  2002/09/09 23:33:19  dick
 * Add Ctl subcommands for the LocalCtl.
 *
 * Revision 1.10  2002/07/17 23:56:26  dick
 * new ctl type PlayerMessage, new PlayerType PlayerAny, matches any PlayerType.
 *
 * Revision 1.9  2002/07/14 18:56:35  dick
 * Add PlayerNone
 *
 * Revision 1.8  2002/07/09 22:54:53  dick
 * Enum PlayerType
 *
 * Revision 1.7  2002/06/30 06:20:40  dick
 * New Ctl type WantPlayMessages to turn on/off play messages to a control.
 *
 * Revision 1.6  2002/06/26 07:26:16  dick
 * Call it ServerActivate instead of Activate.
 *
 * Revision 1.5  2002/06/26 06:51:01  dick
 * New command 'Activate' which tells the server that the scoreserver is ready
 * to receive scores.  Otherwise, score events start coming before we have
 * finished the setup/verify cycle which breaks things.
 *
 * Revision 1.4  2002/06/25 04:21:59  dick
 * Add an enum to define each ScoreType event
 *
 * Revision 1.3  2002/06/23 06:17:46  dick
 * Add score commands
 *
 * Revision 1.2  2001/08/30 07:16:31  dick
 * Send and receive options between the server and xpilotedit.
 *
 * Revision 1.1  2001/08/26 09:37:13  dick
 * Define the subcommands in a PKT_CTL
 *
 */

#ifndef	_PACKETCTL_H_
#define	_PACKETCTL_H_

/*
 * Control commands use the PKT_CTL as the packet type and then
 * this value as the control command.
 */

enum PacketCtlTypes {
	Nop,
	DownloadOptions,
	DownloadMap,
	SetOption,
	SetOptions,
	OptionCount,		// number of named downloadable options on server
	WantPlayMessages,	// please send the control the play messages (top and bottom)
	PlayerMessage,		// display this message
	ServerInfo,			// Send the server name

	// Score commands
	ServerActivate,			// turn on/off the scoreserver (allow ScoreEvents)
	GetScore,				// request score for a player
	GetRanks,				// request ranks for players
	SendScore,				// send a player's score to the database
	PlayerEvent,			// Join/Leave/Pause
	ScoreEvent,				// killer,killee,type,
	WebInfo,				// ScoreServer Web info

	// localctl commands
	Shutdown,				// XPilot sez shutdown the server
	RestartWorld,			// restart world $1
	WorldInfo,				// Send info (map names) for all worlds
	PlayerInfo,				// Send info on players, name and score

	PacketCtlTypesMAX,
};

enum ScorePlayerEvent {
	Join,
	Quit,
	Pause,
	Unpause,

	ScorePlayerEventMAX
};

// Enums for sending kill types to the ScoreServer
enum ScoreType {
	ScoreNone,		// no event
	ScoreShot,
	ScoreTorpedo,
	ScoreSmart,
	ScoreHeat,
	ScoreCluster,
	ScoreLaser,
	ScoreTank,
	ScoreRunover,
	ScoreBall,
	ScoreExplode,
	ScoreShove,
	ScoreCrash,
	ScoreMine,
	ScoreSelf,
	ScoreSelfDestruct,
	ScoreAsteroid,
	ScoreTreasure,
	ScoreUnknown,

	ScoreTypeMAX
};

enum PlayerType {
	PlayerHuman,
	PlayerTank,
	PlayerRobot,
	PlayerNone,		// maybe make this PlayerWall, PlayerMissile :)

	PlayerAny,

	PlayerTypeMAX
};

#endif	// _PACKETCTL_H_
