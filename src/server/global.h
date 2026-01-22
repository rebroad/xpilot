/* $Id: global.h,v 1.18 2004/05/07 04:27:41 dick Exp $
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
 *  $Log: global.h,v $
 *  Revision 1.18  2004/05/07 04:27:41  dick
 *  _visibility becomes Visibility.  global updateScores becomes a member of World.
 *  Handle rank/rate data from the scoreserver and send it to the client.
 *
 *  Revision 1.17  2004/02/16 14:29:44  dick
 *  server/sched.cpp and friends become serverlib/NetServer
 *  which disconnects the udp listen and receive functionality from the game.
 *
 *  Revision 1.16  2002/09/11 16:42:04  dick
 *  Here endeth the server encapsulation task.
 *  theWorld is now an array of World(s) and get loaded dynamically.
 *
 *  Revision 1.15  2002/09/09 23:38:59  dick
 *  Remove commented out deadwood.
 *
 *  Revision 1.14  2002/07/12 15:33:30  dick
 *  Track controlsConnected so we can run fast sched() if there is only a
 *  control connected.
 *
 *  Revision 1.13  2002/06/01 06:06:48  dick
 *  Encapsulate (almost) everything.  Get rid of (almost) all refs to theWorld.
 *
 *  Revision 1.12  2001/08/14 21:37:14  dick
 *  Array Obj[] becomes objs[].  We now have a class called Obj.
 *
 *  Revision 1.11  2001/08/11 00:36:53  dick
 *  frame.cpp becomes Frame.cpp.  object.h becomes Object.h
 *
 *  Revision 1.10  2001/07/30 01:19:55  dick
 *  Encapsulate the last few options (Strings)
 *
 *  Revision 1.9  2001/07/24 09:04:00  dick
 *  Remove all global options.  Use World.options instead.
 *
 *  Revision 1.8  2001/07/23 21:08:37  dick
 *  Encapsulate ShipMass, ShotsMax, ShotsLife, ShotsMass, FPS.
 *
 *  Revision 1.7  2001/07/23 07:38:42  dick
 *  Fold contact, metaserver, netserver, rules, and server to be members of World.
 *
 *  Revision 1.6  2001/07/19 04:02:35  dick
 *  Working on encapsulating options.
 *
 *  Revision 1.5  2001/07/18 20:15:33  dick
 *  The first pass at integrating xpilotedit's options with the server options.
 *  The server does not build yet.  xpilotedit works as before, with the new
 *  definitions.
 *
 *  Revision 1.4  2001/07/15 01:24:27  dick
 *  Break Player out to his own file
 *
 *  Revision 1.3  2001/07/10 08:11:58  dick
 *  Delete map.h Add World.h
 *
 *  Revision 1.2  2001/07/07 12:00:42  dick
 *  Rename classes to C++ "Style".  old World becomes theWorld.
 *
 */

#ifndef	GLOBAL_H
#define	GLOBAL_H

#include "Object.h"
#include "config.h"
#include "Player.h"
#include "World.h"

#ifndef MAX
#define MAX(a,b)  ((a) > (b) ? (a) : (b))
#define MIN(a,b)  ((a) < (b) ? (a) : (b))
#endif


/*
 * Global data.
 */
extern long		frame_loops;

extern World*	theWorld[MAX_WORLDS];
extern long		DEF_BITS, KILL_BITS, DEF_HAVE, DEF_USED, USED_KILL;


extern long		KILLING_SHOTS;
extern unsigned		SPACE_BLOCKS;

#endif /* GLOBAL_H */

