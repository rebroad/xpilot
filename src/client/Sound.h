/* $Id: Sound.h,v 1.1 2004/05/05 01:01:06 dick Exp $
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
 * Define a Sound for XPilot5.
 *
 * See http://www.openal.org for details of the engine
 */
/*
 * $Log: Sound.h,v $
 * Revision 1.1  2004/05/05 01:01:06  dick
 * Make a list of Sounds and bind an array of SoundEvents to them.
 * This is so we don't load 20 sounds which are all the same.
 * Add support for volume/gain.  Server sends gain, client configures gain,
 * and each sound can have an optional gain parameter to mix down too loud sounds.
 *
 * Revision 1.3  2004/05/02 08:01:01  dick
 * No audio if no #ifdef SOUND
 *
 * Revision 1.2  2004/05/02 01:31:37  dick
 * Linux uses al.h in an AL/ subdirectory.  Windows does not.
 *
 * Revision 1.1  2004/05/01 19:25:26  dick
 * XPilot5 audio uses OpenAL
 *
 */

#ifndef	_SOUND_H_
#define	_SOUND_H_

#ifdef	_UNIX
#include <AL/al.h>
#else
#include <al.h>
#endif


#include "Obj.h"

#define	MAX_RANDOM_SOUNDS	6

class Sound : public Obj {
public:
	Sound();
	virtual ~Sound();

	bool	Load(PCSTR file, double gain = 1.0);

	String	filename;
	double	gain;				// 0.0 - 1.0 = volume
	ALuint	buffer;
	ALuint	source;
};

class SoundList : public ObjList {
public:
	Sound*	Find(PCSTR file);
};

#endif			// _SOUND_H_
