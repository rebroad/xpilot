/* $Id: Sound.cpp,v 1.4 2005/03/17 22:12:13 kps Exp $
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
 * Original XPilot audio code provided by Greg Renda (greg@ncd.com).
 *
 * Define a Sound for XPilot5.
 *
 * See http://www.openal.org for details of the engine
 */
/*
 * $Log: Sound.cpp,v $
 * Revision 1.4  2005/03/17 22:12:13  kps
 * Get rid of warnings from makedepend about "non-portable whitespace".
 *
 * Revision 1.3  2004/05/16 01:25:37  dick
 * Can't pass String through ...
 *
 * Revision 1.2  2004/05/13 19:09:53  dick
 * Fix Linux sound
 *
 * Revision 1.1  2004/05/05 01:01:06  dick
 * Make a list of Sounds and bind an array of SoundEvents to them.
 * This is so we don't load 20 sounds which are all the same.
 * Add support for volume/gain.  Server sends gain, client configures gain,
 * and each sound can have an optional gain parameter to mix down too loud sounds.
 *
 */

#include "StdAfx.h"

#ifdef SOUND


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <math.h>

#ifdef	_UNIX
#include <AL/alut.h>
#else
#include <alut.h>
#endif

#include "Obj.h"
#include "config.h"
#include "error.h"
#include "xpprintf.h"
#include "audioDefs.h"
#include "Sound.h"
//#include "Audio.h"


///////////////////////////////////////////////////////////////////////////////
Sound::Sound()
{
	buffer = 0;
	source = 0;
	gain = 1.0;
}

///////////////////////////////////////////////////////////////////////////////
Sound::~Sound()
{
	if (buffer)
		alDeleteBuffers(1, &buffer);
	if (source)
		alDeleteSources(1, &source);
}

///////////////////////////////////////////////////////////////////////////////
bool Sound::Load(PCSTR fname, double _gain)
{
	ALenum	err;

	filename = fname;
	gain = _gain;

	// create buffer
	alGetError(); /* clear */
	alGenBuffers(1, &buffer);
	if((err = alGetError()) != AL_NO_ERROR) {
		xpprintf("Sound:Load GenBuffers %x %s\n", err, alGetString(err));
		return false;
	}
	// create source
	alGetError(); /* clear */
	alGenSources(1, &source);
	if((err = alGetError()) != AL_NO_ERROR) {
		xpprintf("Sound:Load GenSources %x %s\n", err, alGetString(err));
		alDeleteBuffers(1, &buffer);
		buffer = 0;
		return false;
	}

	// load data into buffer
#	ifdef _WINDOWS
		ALsizei size, freq;
		ALenum format;
		ALvoid *data;
		ALboolean looping;

		alutLoadWAVFile(filename.s, &format, &data, &size, &freq, &looping);
		if((err = alGetError()) != AL_NO_ERROR
			|| !data
			|| !size) {
			error("Could not load sound \"%s\"\n", fname);
			alDeleteBuffers(1, &buffer);
			alDeleteSources(1, &source);
			return false;
		}
		alBufferData(buffer, format, data, size, freq);
		if((err = alGetError()) != AL_NO_ERROR) {
			xpprintf("Sound:Load BufferData %x %s\n", err, alGetString(err));
			alDeleteBuffers(1, &buffer);
			alDeleteSources(1, &source);
			return false;
		}
		alutUnloadWAV(format, data, size, freq);
#	endif

#	ifdef MACOS
		ALsizei size, freq;
		ALenum format;
		ALvoid *data;

		alutLoadWAVFile(filename, &format, &data, &size, &freq);
		alBufferData (sound.buffer, format, data, size, freq);
#	endif

#	ifdef _UNIX
		ALsizei size, freq, bits;
		ALenum format;
		ALvoid *data;

		err = alutLoadWAV(filename, &data, &format, &size, &bits, &freq);
		if(err == AL_FALSE) {
			error("Could not load %s\n", (PCSTR)filename);
			return false;
		}

		alBufferData (buffer, format, data, size, freq);
#	endif


	// set static source properties
	alSourcei(source, AL_BUFFER, buffer);
	alSourcei(source, AL_LOOPING, 0);
	alSourcef(source, AL_REFERENCE_DISTANCE, 10);

	D(xpprintf("Loaded sound buffer=%d source=%d\n",
		buffer, source);)

//	alSourcePlay(sound.source);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
Sound* SoundList::Find(PCSTR file)
{
	Sound* sptr = (Sound*)GetHead();
	while (sptr)
	{
		if (!strcmp(sptr->filename, file))
			return(sptr);
		sptr = (Sound*)sptr->GetNext();
	}
	return(NULL);
}

#endif		// SOUND
