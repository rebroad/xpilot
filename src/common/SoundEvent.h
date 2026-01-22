/* $Id: SoundEvent.h,v 1.1 2007/02/17 06:09:05 dick Exp $
 *
 * XPilot, a multiplayer gravity war game.
 *
 *      Copyright (C) 2004 by
 *
 *      The XPilot Authors   <xpilot@xpilot.org>
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
 * See the file COPYRIGHT.TXT for current copyright information.
 *
 * A SoundEvent binds an array of events to Sounds
 *
 */
/*
 * $Log: SoundEvent.h,v $
 * Revision 1.1  2007/02/17 06:09:05  dick
 * Sound moves from client to common, so the SoundEditor can get at it
 *
 * Revision 1.1  2004/05/05 01:01:06  dick
 * Make a list of Sounds and bind an array of SoundEvents to them.
 * This is so we don't load 20 sounds which are all the same.
 * Add support for volume/gain.  Server sends gain, client configures gain,
 * and each sound can have an optional gain parameter to mix down too loud sounds.
 *
 */

#ifndef	_SOUNDEVENT_H_
#define	_SOUNDEVENT_H_

#include "Obj.h"

#define	MAX_RANDOM_SOUNDS	6

class Sound;

class SoundEvent : public Obj {
public:
	SoundEvent();
	void	AddEvent(Sound* sptr);
	void	AddEvent(Sound* sptr, int index);
	Sound*	PickSound();
	int		count;
	Sound*	sounds[MAX_RANDOM_SOUNDS];
};
#endif			// _SOUNDEVENT_H_
