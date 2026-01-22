/* $Id: Ini.Save.cpp,v 1.15 2004/05/28 17:55:58 dick Exp $
 *
 * Class to provide the client side resource file handling.
 * Unix this is ~/.xpilotrc and Windows is XPilot.ini
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2001 by
 *
 *      Bjørn Stabell        <bjoern@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
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
 */
/*
 * $Log: Ini.Save.cpp,v $
 * Revision 1.15  2004/05/28 17:55:58  dick
 * shipShape becomes a String (not a char*).
 * client's MAX_MSGS goes global to here.
 *
 * Revision 1.14  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.13  2004/01/06 04:48:28  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 * Revision 1.12  2002/09/03 07:44:39  dick
 * Use fileName for location of the Unix .ini file.
 *
 * Revision 1.11  2002/09/02 16:55:18  dick
 * Linux needs a buf[512]
 *
 * Revision 1.10  2002/09/02 15:47:42  dick
 * Break Ini up into base class Ini and subclass IniClient.
 *
 * Revision 1.9  2002/08/23 14:17:35  jlmiller
 * ShipShapeFile is a String
 *
 * Revision 1.8  2002/08/18 17:57:18  dick
 * Save and restore the window position for the client
 *
 * Revision 1.7  2002/07/19 11:05:55  jlmiller
 * Make name, realname, & hostname String(s)
 *
 * Revision 1.6  2002/07/17 19:43:26  jlmiller
 * Save shipShapeName.
 *
 * Revision 1.5  2002/05/18 20:55:35  dick
 * Update to XPilot-4.5.1
 *
 * Revision 1.4  2001/09/27 17:29:59  dick
 * member keysym becomes keySym
 *
 * Revision 1.3  2001/09/11 10:16:16  dick
 * Windows doesn't like unistd.h
 *
 * Revision 1.2  2001/09/11 09:08:31  dick
 * Encapsulate all client resources into new class Ini (Linux cleanups)
 *
 * Revision 1.1  2001/09/11 07:33:45  dick
 * Encapsulate all client resources into new class Ini.
 *
 */
#include "StdAfx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_UNIX) || defined(_CYGWIN)
#include <unistd.h>
#endif

#include "const.h"
#include "types.h"
#include "commonproto.h"
#include "portability.h"
#include "bit.h"
#include "Ini.h"

typedef struct xpilotrc {
	char		*line;
	short		size;
} xpilotrc_t;

static xpilotrc_t		*xpilotrc_ptr;
static int				num_xpilotrc, max_xpilotrc;

///////////////////////////////////////////////////////////////////////////////
#if defined(_UNIX) || defined(_CYGWIN)
int Ini::Xpilotrc_add(char *line)
{
	int 				size;
	char				*str;

	if (strncmp(line, "XPilot", 6) != 0 && strncmp(line, "xpilot", 6) != 0) {
		return 0;
	}
	if (line[6] != '.' && line[6] != '*') {
		return 0;
	}
	if ((str = strchr(line + 7, ':')) == NULL) {
		return 0;
	}
	size = str - (line + 7);
	if (max_xpilotrc <= 0 || xpilotrc_ptr == NULL) {
		num_xpilotrc = 0;
		max_xpilotrc = 75;
		if ((xpilotrc_ptr = (xpilotrc_t *)
				malloc(max_xpilotrc * sizeof(xpilotrc_t))) == NULL) {
			max_xpilotrc = 0;
			return -1;
		}
	}
	if (num_xpilotrc >= max_xpilotrc) {
		max_xpilotrc *= 2;
		if ((xpilotrc_ptr = (xpilotrc_t *) realloc(xpilotrc_ptr,
				max_xpilotrc * sizeof(xpilotrc_t))) == NULL) {
			max_xpilotrc = 0;
			return -1;
		}
	}
	if ((str = xp_strdup(line)) == NULL) {
		return -1;
	}
	xpilotrc_ptr[num_xpilotrc].line = str;
	xpilotrc_ptr[num_xpilotrc].size = size;
	num_xpilotrc++;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
static void Xpilotrc_end(FILE *fp)
{
	int 				i;

	if (max_xpilotrc <= 0 || xpilotrc_ptr == NULL) {
		return;
	}
	for (i = 0; i < num_xpilotrc; i++) {
		/* a bug in 3.2.8 saved maxFPS, which is wrong!  don't save maxFPS! */
		if (strncmp(xpilotrc_ptr[i].line + 7, "maxFPS:",
					xpilotrc_ptr[i].size + 1) != 0) {
			fprintf(fp, "%s", xpilotrc_ptr[i].line);
		}
		free(xpilotrc_ptr[i].line);
	}
	free(xpilotrc_ptr);
	xpilotrc_ptr = NULL;
	max_xpilotrc = 0;
	num_xpilotrc = 0;
}

///////////////////////////////////////////////////////////////////////////////
static void Xpilotrc_use(char *line)
{
	int 				i;

	for (i = 0; i < num_xpilotrc; i++) {
		if (strncmp(xpilotrc_ptr[i].line + 7, line + 7,
					xpilotrc_ptr[i].size + 1) == 0) {
			free(xpilotrc_ptr[i].line);
			xpilotrc_ptr[i--] = xpilotrc_ptr[--num_xpilotrc];
		}
	}
}
#endif


///////////////////////////////////////////////////////////////////////////////
#if defined(_UNIX) || defined(_CYGWIN)
void Ini::SaveResource(FILE *fp, PCSTR resource, PCSTR value)
{
	char				buf[256];

	sprintf(buf, "xpilot.%s:\t\t%s\n", resource, value);
	Xpilotrc_use(buf);
	fprintf(fp, "%s", buf);
}
#endif


///////////////////////////////////////////////////////////////////////////////
void Ini::SaveFloat(FILE *fp, PCSTR resource, DFLOAT value)
{
	char				buf[40];

	sprintf(buf, "%.3f", value);
	SaveResource(fp, resource, buf);
}

///////////////////////////////////////////////////////////////////////////////
void Ini::SaveInt(FILE *fp, PCSTR resource, int value)
{
	char				buf[20];

	sprintf(buf, "%d", value);
	SaveResource(fp, resource, buf);
}

///////////////////////////////////////////////////////////////////////////////
void Ini::SaveBool(FILE *fp, PCSTR resource, int value)
{
	char				buf[20];

	sprintf(buf, "%s", (value != 0) ? "True" : "False");
	SaveResource(fp, resource, buf);
}



///////////////////////////////////////////////////////////////////////////////
int Ini::SaveAll()
{
	FILE*		fp = NULL;
#if defined(_UNIX) || defined(_CYGWIN) /* Windows does no file handling on its own. */
		char	oldfile[PATH_MAX + 1];
		char	newfile[PATH_MAX + 1];
		char	buf[512];

//	  *strptr = "Saving...";
//	  Widget_draw(widget_desc);
//	  Client_flush();

	//GetXPilotRcFile(oldfile, sizeof(oldfile));
	strncpy(oldfile, fileName, sizeof(oldfile));
	if (oldfile[0] == '\0') {
//		Config_save_failed("Can't find .xpilotrc file", strptr);
		return 1;
	}
	if ((fp = fopen(oldfile, "r")) != NULL) {
		while (fgets(buf, sizeof buf, fp)) {
			Xpilotrc_add(buf);
		}
		fclose(fp);
	}
	sprintf(newfile, "%s.new", oldfile);
	unlink(newfile);
	if ((fp = fopen(newfile, "w")) == NULL) {
//		Config_save_failed("Can't open file to save to.", strptr);
		return 1;
	}
#endif

		// call the subclass's Save to write the data
		Save(fp);

#if defined(_UNIX) || defined(_CYGWIN)
	Xpilotrc_end(fp);
	fclose(fp);
	sprintf(newfile, "%s.bak", oldfile);
	rename(oldfile, newfile);
	unlink(oldfile);
	sprintf(newfile, "%s.new", oldfile);
	rename(newfile, oldfile);
#endif

//		*strptr = "";
	return 1;
}

