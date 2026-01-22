/*
* Xpilotedit, the cross platform map editor for Xpilot.  Copyright (C) 2001 by
*
*      The XPilot Authors           <xpilot@xpilot.org>
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
 * $Log: flUtil.cpp,v $
 * Revision 1.1  2007/02/17 06:14:28  dick
 * Some general FLTK utilities
 *
 * Revision 1.13  2004/03/03 07:54:35  dick
 * Don't make pixmaps at load time; wait until runtime init, after Strings have
 * actually been inited.
 *
 * Revision 1.12  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.11  2004/01/20 03:13:47  jlmiller
 * If the file exists, we don't need to overwrite, but we still need to say we CAN.
 *
 * Revision 1.10  2004/01/13 21:29:27  jlmiller
 * Function for checking if a file exists.
 *
 * Revision 1.9  2003/12/22 05:59:51  dick
 * Use png(s) instead of xpm(s).
 *
 * Revision 1.8  2002/07/23 18:03:31  dick
 * Try to find the buttons in the development directories if they don't exist
 * elsewhere.
 *
 * Revision 1.7  2002/06/25 20:20:09  jlmiller
 * Massive restructure to clean up includes, and objectify code more.
 *
 * Revision 1.6  2002/05/28 07:19:51  dick
 * Rename xpilotedit to XPilotControl
 *
 */
#include "StdAfx.h"

#include "FL/fl_ask.H"

#include "cstring.h"
#include "config.h"

#ifndef	WINDOWS
#include <sys/stat.h>
#endif

/*************************************************************************\
* String GetButtonFile                                                    *
* Arguments :                                                             *
*   PCSTR file : the base file name of the button to find                 *
*                                                                         *
* Returns :                                                               *
* Purpose :  Build the full file name of a button file which may be a     *
* relative name "buttons/f.xpm" or an absolute                            *
* "/usr/local/xpilot/lib/buttons/f.xpm"                                   *
\*************************************************************************/
String	GetButtonFile(PCSTR f)
{
	String	s;
	struct stat st;
	s = Conf_buttondir();
	s += f;
	s += ".png";
	if (stat(s, &st) == 0)
		return(s);
	s = "buttons/";
	s += f;
	s += ".png";
	if (stat(s, &st) == 0)
		return(s);
	return(s);
}

bool TestFileOverwrite(PCSTR f)
{
	String	s;
	struct stat st;
	int ret;
	bool overwrite = true;


	s += f;
	if (stat(s, &st) == 0)
	{
		ret = fl_choice("File Exists, Overwrite?",
			"OK", "Cancel", NULL);
		switch (ret)
		{
			case 0:
				overwrite = true;
				break;
			case 1:
				overwrite = false;
				break;
		}
	}
    else
		//we need to return true if the file doesn't exist
		return(true);

	return(overwrite);
}
