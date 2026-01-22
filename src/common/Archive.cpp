/* $Id: Archive.cpp,v 1.6 2004/02/03 03:39:52 dick Exp $
 *
 * Wrap FILE with some functions
 *
 *      Copyright (C) 2002 by
 *
 *      The XPilot Authors   <xpilot@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
 *      Jarrod Miller        <jarrod@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
 *      Bjørn Stabell        <bjoern@xpilot.org>
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
 */
/*
 * $Log: Archive.cpp,v $
 * Revision 1.6  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.5  2002/08/01 14:56:13  dick
 * Add OpenAppend()
 *
 * Revision 1.4  2002/07/30 21:55:56  jlmiller
 *
 * Must include <string.h>
 *
 * Revision 1.3  2002/07/24 05:59:16  dick
 * Add Chmod().
 *
 * Revision 1.2  2002/07/23 17:54:27  dick
 * Close() for explicit closing (not really needed; just being anal ;)
 * SeekTo(pos) to position within the file.
 *
 * Revision 1.1  2002/06/25 04:35:23  dick
 * Wrap FILE with some basic functions.
 *
 */
#include "StdAfx.h"

#ifdef	_WINDOWS
#include <windows.h>
#endif

#ifndef	_WINDOWS
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include <stdio.h>
#include <string.h>

#include "Archive.h"

///////////////////////////////////////////////////////////////////////////////
Archive::Archive()
{
	file = NOFILE;
}

///////////////////////////////////////////////////////////////////////////////
Archive::~Archive()
{
	Close();
}

///////////////////////////////////////////////////////////////////////////////
void Archive::Close()
{
	if (file != NOFILE)
		fclose(file);
	file = NOFILE;
}

///////////////////////////////////////////////////////////////////////////////
void Archive::Chmod(int perms)
{
#ifndef	_WINDOWS	// as if!
	chmod(fileName, perms);
#endif
}

///////////////////////////////////////////////////////////////////////////////
long Archive::Read(String& s, long len)
{
	if (file == NOFILE)
	{
		if (!OpenRead())
			return(0);
	}
	s.SetLength(len);
	return(fread(s.s, 1, len, file));
}


///////////////////////////////////////////////////////////////////////////////
long Archive::SeekToBegin()
{
	if (file == NOFILE)
	{
		if (!OpenRead())
			return(0);
	}
	return(fseek(file, 0, SEEK_SET));
}

///////////////////////////////////////////////////////////////////////////////
long Archive::SeekToEnd()
{
	if (file == NOFILE)
	{
		if (!OpenRead())
			return(0);
	}
	if (fseek(file, 0, SEEK_END))
		return(0);
	return(ftell(file));
}

///////////////////////////////////////////////////////////////////////////////
long Archive::SeekTo(long pos)
{
	if (file == NOFILE)
	{
		if (!OpenRead())
			return(0);
	}
	return(fseek(file, pos, SEEK_SET));
}


///////////////////////////////////////////////////////////////////////////////
bool Archive::OpenRead()
{
	if (file != NOFILE)
		fclose(file);
	file = fopen(fileName, "r");
	if (file)
		return(true);
	file = NOFILE;
	return(false);

}

///////////////////////////////////////////////////////////////////////////////
bool Archive::OpenCreate()
{
	if (file != NOFILE)
		fclose(file);
	file = fopen(fileName, "w");
	if (file)
		return(true);
	file = NOFILE;
	return(false);
}

///////////////////////////////////////////////////////////////////////////////
bool Archive::OpenAppend()
{
	if (file != NOFILE)
		fclose(file);
	file = fopen(fileName, "a");
	if (file)
		return(true);
	file = NOFILE;
	return(false);
}

///////////////////////////////////////////////////////////////////////////////
void Archive::WriteString(PCSTR s)
{
	fwrite(s, 1, strlen(s), file);
}
