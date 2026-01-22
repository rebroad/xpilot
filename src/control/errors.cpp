/*
* XPilotedit, the cross platform map editor for XPilot.  Copyright (C) 2001 by
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
 * $Log: errors.cpp,v $
 * Revision 1.10  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.9  2002/07/23 19:58:24  jlmiller
 * No, actually the #include "" was right the first time, so lets put it back.
 *
 * Revision 1.8  2002/07/23 13:24:12  jlmiller
 * #includes should be <> instead of ""
 *
 * Revision 1.7  2002/06/25 20:20:09  jlmiller
 * Massive restructure to clean up includes, and objectify code more.
 *
 * Revision 1.6  2002/05/28 07:19:51  dick
 * Rename xpilotedit to XPilotControl
 *
 */
#include "StdAfx.h"

#include <stdio.h>
#include <stdarg.h>

#include "FL/fl_ask.H"


/***************************************************************************/
/* ErrorHandler                                                            */
/* Arguments :                                                             */
/*   szMessage - error messge to be output                                 */
/*   ... - variable number of arguments, subsituted for %s, %d, etc.       */
/* Purpose : Display an Error notification box with the specified message  */
/***************************************************************************/
void ErrorHandler(const char *szMessage, ...)
{
        va_list marker;
        char szBuffer[512];

        // Figure through the extra arguments.
        va_start(marker, szMessage);
        vsprintf(szBuffer, szMessage, marker);
        va_end(marker);

        // Display the error message.
#ifdef	WIN32
		fl_alert(szBuffer);
#else
        printf("%s", szBuffer);
#endif
//	fl_alert("%s", szBuffer);
}
