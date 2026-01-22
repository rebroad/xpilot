/* $Id: Archive.h,v 1.4 2002/08/01 14:56:13 dick Exp $
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
 * $Log: Archive.h,v $
 * Revision 1.4  2002/08/01 14:56:13  dick
 * Add OpenAppend()
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

#ifndef	_ARCHIVE_H_
#define	_ARCHIVE_H_

#include "cstring.h"

#define	NOFILE	((FILE*)-1)

class Archive
{
public:
	Archive();
	~Archive();
	void	SetFileName(PCSTR s) { fileName = s; };

	bool	FileExists();
	bool	OpenRead();
	bool	OpenCreate();
	bool	OpenAppend();
	void	Close();
	void	Chmod(int perms);
	long	SeekToBegin();
	long	SeekTo(long pos);
	long	SeekToEnd();
	long	Read(String& s, long len);
	void	WriteString(PCSTR s);


private:
	FILE*	file;
	String	fileName;
};

#endif		// _ARCHIVE_H_
