/* $Id: SoundEvent.cpp,v 1.1 2004/05/05 01:01:06 dick Exp $
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
 * $Log: SoundEvent.cpp,v $
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
//#include "config.h"
#include "error.h"
//#include "xpprintf.h"
//#include "audioDefs.h"
#include "randommt.h"
#include "Sound.h"
#include "SoundEvent.h"



///////////////////////////////////////////////////////////////////////////////
SoundEvent::SoundEvent()
{
	count = 0;
	for (int i=0; i<MAX_RANDOM_SOUNDS; i++)
		sounds[i] = NULL;
}

///////////////////////////////////////////////////////////////////////////////
void SoundEvent::AddEvent(Sound* sptr)
{
	if (count >= MAX_RANDOM_SOUNDS)
	{
		error("Too many events for sound.\n"
			  "\"%s\"\n"
			  "Ignoring...", (PCSTR)sptr->filename);
		return;
	}
	sounds[count] = sptr;
	count++;
}

Sound* SoundEvent::PickSound()
{
	if (count == 0)
		return(NULL);
	if (count == 1)
		return(sounds[0]);
	return(sounds[randomMT() % count]);
}
#endif		// SOUND
