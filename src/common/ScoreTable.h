/* $Id: ScoreTable.h,v 1.1 2004/04/26 07:07:14 dick Exp $
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2004 by
 *
 *      Dick Balaska         <dick@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
 *      Bj�rn Stabell        <bjoern@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
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
 * $Log: ScoreTable.h,v $
 * Revision 1.1  2004/04/26 07:07:14  dick
 * Define a list of pages that can be displayed on the ScoreTable in the client.
 * Each page can be controlled by the server (and the server admin tool).
 *
 */

#ifndef	_ScoreTable_h_
#define	_ScoreTable_h_

///////////////////////////////////////////////////////////////////////////////
enum ScoreTableType {
	STPages = -1,	// Send the list of Pages that can be scrolled through
	STFirst,
	STScore = STFirst,	// Normal score page
	STName,			// Normal Player "realname" page
	STKills,		// Kills/Deaths
	STIPVer,		// IP+Version number
	STRank,			// Rank info from the ScoreServer

	STMAX			// number of types available
};

//#define	STPagesB	(1<<STPages)	// no bit field for overview
#define	STScoreB	(1<<STScore)
#define	STNameB		(1<<STName)
#define	STKillsB	(1<<STKills)
#define	STIPVerB	(1<<STIPVer)
#define	STRankB		(1<<STRank)

/*
 * All these commands begin with PKT_SCORE_TABLE followed by the subcommand and args.
 *
 * STPages hd - bitfield of available pages
 * STIPVer hd, s - ID, String of player's IP and version info
 * STKills hd, d, d - ID, kills, deaths
 * STRank  hd, d, d, c - ID, rank, rating, rating divisor (number of decimals in the rating)
 */
#endif
