/* $Id: fileUtils.h,v 1.1 2007/02/17 06:10:01 dick Exp $
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
 * $Log: fileUtils.h,v $
 * Revision 1.1  2007/02/17 06:10:01  dick
 * Misc file and dir handling utilities
 *
 */
#ifndef	_FILEUTILS_H_
#define	_FILEUTILS_H_

#include "Obj.h"
///////////////////////////////////////////////////////////////////////////////
// a file

#define	FTYPE_DIR		0x01
#define	FTYPE_RO		0x02	// not used
#define	FTYPE_RW		0x04	// not used
#define	FTYPE_SUBDIR	0x08	// subdirectory directive for filelists
#define	FTYPE_URL		0x10	// "file" is an url to a remote resource

class file : public Obj {
  public:
	file() { lastWrite = 0; size = 0; nameAsString = NULL; ftype=0; };
	virtual ~file();

	void		Parse(PCSTR s);
	String		GetNameAsString();

#ifdef	_WINDOWS
	void		Parse(struct _finddata_t* fdt);
#endif
	String	subdir;
	time_t	lastWrite;
	int		size;
	int		ftype;

	String*	nameAsString;
};


// Fetching filenames
typedef bool (*filterfunc)(file* fdt, ObjList* filterlist);
extern void GetFileList(ObjList* fileList, const char* name,
						filterfunc filter = NULL, ObjList* filterlist = NULL);

#endif		// _FILEUTILS_H_
