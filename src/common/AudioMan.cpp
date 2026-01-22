/* $Id: AudioMan.cpp,v 1.2 2007/02/17 20:19:45 dick Exp $
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
 * $Log: AudioMan.cpp,v $
 * Revision 1.2  2007/02/17 20:19:45  dick
 * Move the #ifdef SOUND around so that the basic objects function even if they
 * can't emit any audio. This allows the SoundEditor to work.
 *
 * Revision 1.1  2007/02/16 22:22:43  dick
 * src/client/Audio.* becomes src/common/AudioMan.*
 * Add sound.conf file writing to AudioMan.
 *
 * Revision 1.7  2007/01/09 21:35:19  dick
 * Add support for a 'Mute' button on the menu.
 *
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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <math.h>

#ifdef	SOUND
#ifdef	_UNIX
#include <AL/alut.h>
#else
#include <alut.h>
#endif
#endif

#include "portability.h"
#include "Obj.h"
#include "config.h"
#include "error.h"
#include "xpprintf.h"
#include "fileUtils.h"
#include "audioDefs.h"
#include "Sound.h"
#include "AudioMan.h"

#include "soundNames.cpp.h"

// The one and only Audio object
AudioMan	audioMan;

///////////////////////////////////////////////////////////////////////////////
AudioMan::AudioMan()
{
	audioEnabled = true;
	muted = false;
	gain = 1.0;
	inited = false;
	isEditor = false;
}

///////////////////////////////////////////////////////////////////////////////
void AudioMan::Init(PCSTR confFile)
{
	FILE*	fp;
	char	buf[512];
	String	soundFile;
	char*	fname;
	char*	sound;
	char*	comma;
	Sound*	sptr;
	double	gain;
	bool	hasSeenData = false;
	int 	i;
//	int		j;

	if (!(fp = fopen(confFile, "r"))) {
		error("Could not open soundfile \"%s\"\n"
			  "Sounds are disabled", confFile);
		audioEnabled = false;
		return;
	}

	if (!inited)
		InitAL();
	inited = true;
	if (isEditor)
		fileName = confFile;
	commentString = "";

	while (fgets(buf, sizeof(buf), fp)) {
		/* ignore comments */
		if (*buf == '\n' || *buf == '#') {
			if (!isEditor || hasSeenData)
				continue;
			commentString += buf;
			continue;
		}
		hasSeenData = true;

		sound = strtok(buf, " \t");
		fname = strtok(NULL, " \t\n");
		gain = 1.0;
		if ((comma = strchr(fname, ',')) != NULL) {
			*comma = '\0';
			comma++;
			gain = atof(comma);
		}

		for (i = 0; i < MAX_SOUND_EVENTS; i++) {
			if (!strcmp(sound, soundNames[i])) {
				//if (i != 1)
				//	continue;
				//
				soundFile = fname;

				if ((sptr = soundList.Find(soundFile)) == NULL) {
					sptr = new Sound;
					if (!sptr->Load(soundFile, gain)) {
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
				soundEvents[i].AddEvent(sptr);
				break;
			}
		}
		if (i == MAX_SOUND_EVENTS)
			fprintf(stderr, "Unknown sound event'%s' (ignored)\n", sound);
	}
	fclose(fp);
	xpprintf("Audio: loaded %d sounds\n", soundList.GetCount());
	if (isEditor) {
		ObjList fileList;
		String path = Conf_sounddir();
		path += "*.wav";
		GetFileList(&fileList, path);
		xpprintf("Audio: got %d wav files\n", fileList.GetCount());
		file* f = (file*)fileList.GetHead();
		while (f) {
			if ((sptr = soundList.Find(f->name)) == NULL) {
				sptr = new Sound;
					if (!sptr->Load(f->name, 1.0)) {
						error("Failed to load sound \"%s\"\n"
							  "Ignoring...", (PCSTR)f->name);
						delete sptr;
						f = (file*)f->GetNext();
						continue;
					}
					float tempPos[3];
					tempPos[0] = 0.0f;
					tempPos[1] = 0.0f;
					tempPos[2] = 0.0f;
					SetSourcePosition(*sptr, tempPos);
					SetSourceVelocity(*sptr, tempPos);
					soundList.Add(sptr);
			}
			f = (file*)f->GetNext();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
bool AudioMan::SaveSoundFile(PCSTR fname) {
	FILE*	fp;
	int		i;
	int		j;

	xpprintf("Saving Sound File \"%s\"\n", fname);
	if (!(fp = fopen(fname, "w"))) {
		error("Could not open output soundfile \"%s\"\n", fname);
		return(false);
	}
	if (!this->commentString.IsEmpty()) {
		if (fwrite(commentString, commentString.GetLength(), 1, fp) != 1) {
			fclose(fp);
			error("Could not write to soundfile \"%s\"\n", fname);
			return(false);
		}
	}
	for (i = 0; i < MAX_SOUND_EVENTS; i++) {
		SoundEvent& se = soundEvents[i];
		if (se.count) {
			fprintf(fp, "%s\t", soundNames[i]);
			if (strlen(soundNames[i]) < 20)
				fputc('\t', fp);
			if (strlen(soundNames[i]) < 16)
				fputc('\t', fp);
			if (strlen(soundNames[i]) < 12)
				fputc('\t', fp);
			if (strlen(soundNames[i]) < 8)
				fputc('\t', fp);
			if (strlen(soundNames[i]) < 4)
				fputc('\t', fp);
			for(j=0; j<MAX_RANDOM_SOUNDS; j++) {
				if (se.sounds[j] != NULL) {
					if (j != 0)
						fputc('|', fp);
					fprintf(fp, "%s", (PCSTR)se.sounds[j]->name);
					if (se.sounds[j]->gain != 1.0) {
						fprintf(fp, ",%.2f", se.sounds[j]->gain);
					}
				}
			}
			fprintf(fp, "%s", fileEol);
		}
	}
	fclose(fp);
	return(true);
}

///////////////////////////////////////////////////////////////////////////////
void AudioMan::SetGain(double _gain) {
	gain = _gain;
}

///////////////////////////////////////////////////////////////////////////////
void AudioMan::Cleanup() {
	soundList.Empty();
#ifdef	SOUND
	alutExit();
#endif
}

///////////////////////////////////////////////////////////////////////////////
// Receive an audio event from the server
int AudioMan::HandleEvent(int type, int volume) {
	//int 		pick = 0;

	if (muted || !audioEnabled || !soundEvents[type].count)
		return 0;


	Sound* sptr = soundEvents[type].PickSound();
#ifdef	SOUND
	if (sptr && sptr->source) {
		double	gain = ((double)volume/100.0)*sptr->gain*this->gain;
		alSourcef(sptr->source, AL_GAIN, (float)gain);
		alSourcePlay(sptr->source);
//		D(xpprintf("Audio:Handle type=%d, volume = %d gain=%lf\n", type, volume, gain);)
	}
#endif
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Receive an audio event from the server
int AudioMan::PlaySound(int type, int volume) {

	if (muted || !audioEnabled)
		return 0;


	Sound* sptr = (Sound*)soundList[type];
#ifdef	SOUND
	if (sptr && sptr->source) {
		double	gain = ((double)volume/100.0)*sptr->gain*this->gain;
		alSourcef(sptr->source, AL_GAIN, (float)gain);
		alSourcePlay(sptr->source);
//		D(xpprintf("Audio:Handle type=%d, volume = %d gain=%lf\n", type, volume, gain);)
	}
#endif
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
void AudioMan::InitAL() {
   // position player
   playerPos[0] = 0.0f; // x
   playerPos[1] = 0.0f; // y
   playerPos[2] = 0.0f; // z

#ifdef	SOUND
   alutInit (NULL, 0); // init OpenAL

   // global settings
   alListenerf(AL_GAIN, 1.0);
   alDopplerFactor(1.0); // don't exaggerate doppler shift
   alDopplerVelocity(343); // using meters/second


   // orient player
   float ori[6];
   ori[0] = 0;
   ori[1] = 0;
   ori[2] = -1;
   ori[3] = 0;
   ori[4] = 1;
   ori[5] = 0;

   ListenerPosition(playerPos, ori);

#endif
}


///////////////////////////////////////////////////////////////////////////////
// ListenerPostition -- update listener's position and direction
#ifdef	SOUND
void AudioMan::ListenerPosition (ALfloat* position, ALfloat* angle) {
	alListenerfv(AL_POSITION, position);
	alListenerfv(AL_ORIENTATION, angle);
}
#endif

///////////////////////////////////////////////////////////////////////////////
//SetSourcePosition
void AudioMan::SetSourcePosition (Sound& sound, float *position) {
#ifdef	SOUND
	alSourcefv(sound.source, AL_POSITION, position);
#endif
}

///////////////////////////////////////////////////////////////////////////////
//SetSourceVelocity
void AudioMan::SetSourceVelocity (Sound& sound, float *velocity) {
#ifdef	SOUND
	alSourcefv(sound.source, AL_VELOCITY, velocity);
#endif
}

