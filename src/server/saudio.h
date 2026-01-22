/* $Id: saudio.h,v 1.4 2004/05/01 19:21:29 dick Exp $
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
 */
/* This piece of code was provided by Greg Renda (greg@ncd.com). */

#ifndef _saudio_h
#define _saudio_h

#if defined(SERVER_SOUND) && !defined(SOUND)
/* Enable only sound support in the server, not in the client. */
#define SOUND	1
#endif

#define SDBG(x)	/*#x*/

#ifndef SOUND

/*
 * Define like this to avoid having to put #ifdef SOUND all over the place.
 */
#define SoundPlayerInit(player)		((player)->audio = NULL)
#define SoundPlayerOnOff(player, onoff)
#define SoundPlayPlayer(player, index)
#define SoundPlayAll(w, index)
#define SoundPlaySensors(w, x, y, index)
#define SoundPlayQueued(player)
#define SoundClose(player)

#else						/* SOUND */

#include "audioDefs.h"

int			SoundPlayerInit(Player *);
void		SoundPlayerOnOff(Player* pl, int onoff);
void		SoundPlayPlayer(Player*, int);
void		SoundPlayAll(World* w, int);
void		SoundPlaySensors(World* w, DFLOAT, DFLOAT, int);
void		SoundPlayQueued(Player* pl);
void		SoundClose(Player* pl);

#endif						/* SOUND */

#endif						/* _saudio_h */
