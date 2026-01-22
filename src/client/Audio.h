/* $Id: Audio.h,v 1.4 2004/05/05 01:01:06 dick Exp $
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
 * Original XPilot audio code provided by Greg Renda (greg@ncd.com).
 *
 * Audio Manager for XPilot5.
 *
 * See http://www.openal.org for details of the engine
 */
/*
 * $Log: Audio.h,v $
 * Revision 1.4  2004/05/05 01:01:06  dick
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

#ifndef	_AUDIO_H_
#define	_AUDIO_H_

#ifdef	SOUND

#ifdef	_UNIX
#include <AL/al.h>
#else
#include <al.h>
#endif

#include "audioDefs.h"
#include "Sound.h"
#include "SoundEvent.h"

class Audio
{
public:
	Audio();
	void	Init(PCSTR confFile);
	void	Cleanup();

	void	SetGain(double newGain);
	int		Handle(int type, int volume);

	void	ListenerPosition (ALfloat* position, ALfloat* angle);
	void	SetSourcePosition (Sound& sound, float *position);
	void	SetSourceVelocity (Sound& sound, float *velocity);

private:
	void	InitAL();
	bool	Load(Sound& sound);

private:
	bool	audioEnabled;
	double	gain;
//	ALuint	nextBuffer;
	float	playerPos[3];			// camera/listener position

	SoundList	soundList;

	SoundEvent	sounds[MAX_SOUNDS];

};

extern	Audio	audio;	// singleton

#endif		// SOUND
#endif		// _AUDIO_H_
