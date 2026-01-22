/* $Id: fileUtils.cpp,v 1.2 2007/02/17 19:47:11 dick Exp $
 *
 * Contains misc file and dir handling utilities
 *
 * XPilot, a multiplayer gravity war game.
 *
 *      Copyright (C) 2007 by
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
 */
/*
 * $Log: fileUtils.cpp,v $
 * Revision 1.2  2007/02/17 19:47:11  dick
 * Fix GetFileList on Linux
 *
 * Revision 1.1  2007/02/17 06:10:01  dick
 * Misc file and dir handling utilities
 *
 */

#include "StdAfx.h"

#ifdef	_WINDOWS
#	include <stdio.h>
#	include <stdarg.h>
#	include <io.h>
#	include <sys/stat.h>
#else
#	define	_GNU_SOURCE
#	include <stdio.h>
#	include <time.h>
#	include <stdarg.h>
#	include <errno.h>
#	include <dirent.h>
#	include <sys/stat.h>
#	include <unistd.h>
#	include <ctype.h>
#	include <string.h>
#endif

#include <fcntl.h>
#include "config.h"
#include "sockerrs.h"
#include "fileUtils.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
file::~file()
{
}



///////////////////////////////////////////////////////////////////////////////
#ifdef	_WINDOWS
void file::Parse(struct _finddata_t* fdt)
{
	name = fdt->name;
	lastWrite = fdt->time_write;
	size = fdt->size;
}
#endif


///////////////////////////////////////////////////////////////////////////////
/*
 * Fill an ObjList with files.
 * Note that we truncate the write time to 2 second resolution to accomodate Win95
 */
void GetFileList(ObjList* fileList, PCSTR name, filterfunc filter, ObjList* filterlist)
{
	char* dir = strrchr(name, '/');
	if (dir)
	{
		*dir++ = '\0';
		PCSTR s = dir;
		dir = (char*)name;
		name = s;
	}
#ifdef	_WINDOWS

	struct _finddata_t fdt;
	long hFile;
	String	oname;
	if (dir)
	{
		oname = dir;
		oname += '/';
	}
	oname += name;

	/* Find first file in current directory */
	if( (hFile = _findfirst(oname, &fdt)) != -1L )
	{
		file*	f = new file;
		if (0 && dir)
		{
			f->name = dir;
			f->name += '/';
		}
		f->name += fdt.name;
		f->lastWrite = fdt.time_write;
		f->size = fdt.size;
		if (!filter || filter(f, filterlist))
			fileList->Add(f);
		else
			delete f;
		while( _findnext( hFile, &fdt ) == 0 )
		{
			file*	f = new file;
			if (0 && dir)
			{
				f->name = dir;
				f->name += '/';
			}
			f->name += fdt.name;
			f->lastWrite = fdt.time_write & -2;
			f->size = fdt.size;
			if (fdt.attrib & _A_SUBDIR)
				f->ftype = FTYPE_DIR;
			if (!filter || filter(f, filterlist))
				fileList->Add(f);
			else
				delete f;
		}

		_findclose( hFile );
	}
#else
	DIR*	adir = opendir(dir ? dir : ".");
	xpprintf(LOGMAX, "opendir \"%s\" of \"%s\"\n", dir ? dir : ".", name);
	if (!adir)
	{
		xpprintf("GetFileList: error %d (%s) opening \"%s\"\n",
			errno, GetSockErrText(errno), name);
	}
	else
	{
		struct dirent* de;
		struct stat	sbuf;
		while ((de = readdir(adir)))
		{
			file*	f = new file;
			if (0 && dir)
			{
				f->name = dir;
				f->name += '/';
			}
			f->name += de->d_name;

			stat(f->name, &sbuf);
			// bprintf(LOGMAX, "GetFileList: stat of \"%s\" size=%d mode=%08X\n", (PCSTR)f->name, sbuf.st_size, sbuf.st_mode);
			f->lastWrite = sbuf.st_mtime & -2;
			f->size = sbuf.st_size;
			if (S_ISDIR(sbuf.st_mode))
			{
				f->ftype = FTYPE_DIR;
				xpprintf(LOGMAX, "GetFileList: \"%s\" ISDIR\n", (PCSTR)f->name);
			}
			// bprintf(LOGMAX, "GetFileList: filter [%s] against [%s]\n", (PCSTR)f->name, name);
			if (filter && filter(f, filterlist) && strcmp(f->name, name) == 0)
				fileList->Add(f);
			else
			{
				delete f;
			}
		}
		closedir(adir);
	}
#endif
}

