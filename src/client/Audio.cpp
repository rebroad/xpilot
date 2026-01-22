/* $Id: Audio.cpp,v 1.6 2004/05/20 22:23:42 dick Exp $
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
 * Audio Manager for XPilot5.
 *
 * See http://www.openal.org for details of the engine
 */
/*
 * $Log: Audio.cpp,v $
 * Revision 1.6  2004/05/20 22:23:42  dick
 * Add AddMessage(PCSTR, ...).  Wrap some debug with a D().
 *
 * Revision 1.5  2004/05/13 19:09:32  dick
 * Put quotes around the filename error
 *
 * Revision 1.4  2004/05/05 01:01:06  dick
 * Make a list of Sounds and bind an array of SoundEvents to them.
 * This is so we don't load 20 sounds which are all the same.
 * Add support for volume/gain.  Server sends gain, client configures gain,
 * and each sound can have an optional gain parameter to mix down too loud sounds.
 *
 * Revision 1.3  2004/05/03 03:58:10  dick
 * Wrap debug with a D()
 *
 * Revision 1.2  2004/05/02 01:31:37  dick
 * Linux uses al.h in an AL/ subdirectory.  Windows does not.
 *
 * Revision 1.1  2004/05/01 19:25:26  dick
 * XPilot5 audio uses OpenAL
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
#include "Audio.h"

#include "soundNames.cpp.h"

// The one and only Audio object
Audio	audio;

///////////////////////////////////////////////////////////////////////////////
Audio::Audio()
{
	audioEnabled = true;
	gain = 1.0;
}

///////////////////////////////////////////////////////////////////////////////
void Audio::Init(PCSTR confFile)
{
	FILE*	fp;
	char	buf[512];
	String	soundFile;
	char*	file;
	char*	sound;
	char*	comma;
	Sound*	sptr;
	double	gain;
	int 	i;
//	int		j;

	if (!(fp = fopen(confFile, "r")))
	{
		error("Could not open soundfile \"%s\"\n"
			  "Sounds are disabled", confFile);
		audioEnabled = false;
		return;
	}

	InitAL();
	while (fgets(buf, sizeof(buf), fp))
	{
		/* ignore comments */
		if (*buf == '\n' || *buf == '#')
			continue;

		sound = strtok(buf, " \t");
		file = strtok(NULL, " \t\n");
		gain = 1.0;
		if ((comma = strchr(file, ',')) != NULL)
		{
			*comma = '\0';
			comma++;
			gain = atof(comma);
		}

		for (i = 0; i < MAX_SOUNDS; i++)
		{
			if (!strcmp(sound, soundNames[i]))
			{
				//if (i != 1)
				//	continue;
				soundFile = "lib/sounds/";
				soundFile += file;

				if ((sptr = soundList.Find(soundFile)) == NULL)
				{
					sptr = new Sound;
					if (!sptr->Load(soundFile, gain))
					{
						error("Failed to load sound \"%s\"\n"
							  "Ignoring...", (PCSTR)soundFile);
						delete sptr;
						break;
					}
					float tempPos[3];
					tempPos[0] = 0.0f;
					tempPos[1] = 0.0f;
					tempPos[2] = 0.0f;
					SetSourcePosition(*sptr, tempPos);
					SetSourceVelocity(*sptr, tempPos);
					soundList.Add(sptr);
				}
				sounds[i].AddEvent(sptr);
				break;

			}
		}
		if (i == MAX_SOUNDS)
			fprintf(stderr, "Unknown sound '%s' (ignored)\n", sound);
	}
	fclose(fp);
	xpprintf("Audio: loaded %d sounds\n", soundList.GetCount());
}

///////////////////////////////////////////////////////////////////////////////
void Audio::SetGain(double _gain)
{
	gain = _gain;
}

///////////////////////////////////////////////////////////////////////////////
void Audio::Cleanup()
{
	soundList.Empty();
	alutExit();
}

///////////////////////////////////////////////////////////////////////////////
// Receive an audio event from the server
int Audio::Handle(int type, int volume)
{
	int 		pick = 0;

	if (!audioEnabled || !sounds[type].count)
		return 0;


	Sound* sptr = sounds[type].PickSound();
	if (sptr && sptr->source)
	{
		double	gain = ((double)volume/100.0)*sptr->gain*this->gain;
		alSourcef(sptr->source, AL_GAIN, (float)gain);
		alSourcePlay(sptr->source);
//		D(xpprintf("Audio:Handle type=%d, volume = %d gain=%lf\n", type, volume, gain);)
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
void Audio::InitAL()
{
   alutInit (NULL, 0); // init OpenAL

   // global settings
   alListenerf(AL_GAIN, 1.0);
   alDopplerFactor(1.0); // don't exaggerate doppler shift
   alDopplerVelocity(343); // using meters/second

   // position player
   playerPos[0] = 0.0f; // x
   playerPos[1] = 0.0f; // y
   playerPos[2] = 0.0f; // z

   // orient player
   float ori[6];
   ori[0] = 0;
   ori[1] = 0;
   ori[2] = -1;
   ori[3] = 0;
   ori[4] = 1;
   ori[5] = 0;

   ListenerPosition(playerPos, ori);

}


///////////////////////////////////////////////////////////////////////////////
// ListenerPostition -- update listener's position and direction
void Audio::ListenerPosition (ALfloat* position, ALfloat* angle)
{
	alListenerfv(AL_POSITION, position);
	alListenerfv(AL_ORIENTATION, angle);
}

///////////////////////////////////////////////////////////////////////////////
//SetSourcePosition
void Audio::SetSourcePosition (Sound& sound, float *position)
{
	alSourcefv(sound.source, AL_POSITION, position);
}

///////////////////////////////////////////////////////////////////////////////
//SetSourceVelocity
void Audio::SetSourceVelocity (Sound& sound, float *velocity)
{
	alSourcefv(sound.source, AL_VELOCITY, velocity);
}

#endif /* SOUND */
