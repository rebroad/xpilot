/* $Id: Ini.cpp,v 1.29 2004/05/11 06:45:45 dick Exp $
 *
 * Class to provide the client side resource file handling.
 * Unix this is ~/.xpilotrc and Windows is XPilot.ini
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2001 by
 *
 *      Bj�rn Stabell        <bjoern@xpilot.org>
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
 * $Log: Ini.cpp,v $
 * Revision 1.29  2004/05/11 06:45:45  dick
 * Handle the wildcard trailing prefix
 *
 * Revision 1.28  2004/05/10 06:26:13  dick
 * Allow an optional prefix to be passed, so that the same Options can fill
 * different objects (inet columns and private columns).
 *
 * Revision 1.27  2004/02/20 05:11:39  dick
 * Windows server saves his window position and state.
 *
 * Revision 1.26  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.25  2004/01/06 04:48:28  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 * Revision 1.24  2002/09/12 19:50:33  dick
 * Add GetStringResource() which returns a String.
 *
 * Revision 1.23  2002/09/03 07:43:31  dick
 * Use ~/.xpilot for the Unix config directory.
 *
 * Revision 1.22  2002/09/02 15:45:49  dick
 * Break Ini up into base class Ini and subclass IniClient.
 *
 * Revision 1.21  2002/08/23 14:17:35  jlmiller
 * ShipShapeFile is a String
 *
 * Revision 1.20  2002/08/06 14:48:40  jlmiller
 * Hack Ini to make -team switch work on windows.
 *
 * Revision 1.19  2002/08/02 07:02:30  dick
 * Fix mouse button binding bug
 *
 * Revision 1.18  2002/08/01 02:56:30  jlmiller
 * properly handle shipShapeName, and possibility of old school shipShape defines.
 *
 * Revision 1.17  2002/07/24 07:08:48  dick
 * Support the -port command line switch for Windows.
 *
 * Revision 1.16  2002/07/19 11:05:55  jlmiller
 * Make name, realname, & hostname String(s)
 *
 * Revision 1.15  2002/07/17 19:44:15  jlmiller
 * Add option shipShapeName to Ini.
 *
 * Revision 1.14  2002/07/15 19:10:19  jlmiller
 * Ini should handle shipShapeFile as a item
 *
 * Revision 1.13  2002/07/12 15:43:59  dick
 * Wrap some debug with D()
 *
 * Revision 1.12  2002/05/20 02:17:44  dick
 * Windows needs errno and seterrno in portability.h.
 * Windows also needs rint() defined if not using fltk.
 *
 * Revision 1.11  2002/05/18 22:34:14  dick
 * Move showtime to common.
 *
 * Revision 1.10  2002/05/18 20:55:35  dick
 * Update to XPilot-4.5.1
 *
 * Revision 1.9  2001/10/11 15:56:03  dick
 * Code rearrange.
 *
 * Revision 1.8  2001/10/09 15:43:03  dick
 * Break options out to it's own file Ini.Defaults.cpp.
 * Fix a bug reading resources on Linux.
 *
 * Revision 1.7  2001/10/04 16:02:52  dick
 * New functions InsertKeyDef() and DeleteKeyDef() to act on individual commands
 * in the key command table.
 *
 * Revision 1.6  2001/09/27 17:29:59  dick
 * member keysym becomes keySym
 *
 * Revision 1.5  2001/09/17 23:03:02  dick
 * InitKeysyms() must be called before using the xpkeydefs table to adjust
 * Unix keysyms.
 *
 * Revision 1.4  2001/09/17 10:58:10  dick
 * Move dpy and kdpy to Ini
 *
 * Revision 1.3  2001/09/17 08:46:56  dick
 * Move keyDefs into Ini
 *
 * Revision 1.2  2001/09/11 09:08:31  dick
 * Encapsulate all client resources into new class Ini (Linux cleanups)
 *
 * Revision 1.1  2001/09/11 07:33:45  dick
 * Encapsulate all client resources into new class Ini.
 */
#include "StdAfx.h"


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>

#if defined(_UNIX) || defined(_CYGWIN)
# include <unistd.h>
# include <dirent.h>
# include <sys/stat.h>
# include <X11/Xos.h>
# include <X11/keysym.h>
# include <X11/Xlib.h>
# include <X11/Xresource.h>
# ifdef	__apollo
#  include <X11/ap_keysym.h>
# endif
# include <sys/param.h>
#else
# include <direct.h>
//# include "NT/winX.h"
//# include "NT/winXXPilot.h"
//# include "NT/winConfig.h"
#endif

#include "cstring.h"
#include "version.h"
#include "config.h"
#include "const.h"
#include "showtime.h"
#include "pack.h"
#include "bit.h"
#include "keys.h"
#include "error.h"
#include "types.h"
#include "commonproto.h"
#include "portability.h"
#include "talk.h"
#include "checknames.h"
#include "Ini.h"


char default_version[] = VERSION;

extern	const char* szAppName;


#define DISPLAY_ENV	"DISPLAY"
#define DISPLAY_DEF	":0.0"
#define KEYBOARD_ENV	"KEYBOARD"


#ifdef SPARC_CMAP_HACK
char  frameBuffer[MAX_CHARS]; /* frame buffer */
#endif

static void Get_test_resources(XrmDatabase rDB);
//static void Usage(void);		// This needs to be moved to xpilot.cpp and XPKbConfig.cpp

PCSTR	s_Settings = "Settings";
PCSTR	s_L = "Window_Left";
PCSTR	s_T = "Window_Top";
PCSTR	s_R = "Window_Right";
PCSTR	s_B = "Window_Bottom";
PCSTR	s_windowMode = "Window_Mode";

///////////////////////////////////////////////////////////////////////////////
Ini::Ini()
{
	dpy = 0;
	kdpy = 0;
	myName = "xpilot";
	SetFileName(szAppName);
}

///////////////////////////////////////////////////////////////////////////////
Ini::~Ini()
{
}

///////////////////////////////////////////////////////////////////////////////
void Ini::SetFileName(PCSTR fname)
{
	char	buf[256];
#if defined(_WINDOWS) && !defined(_CYGWIN)
	fileName = getcwd(buf, 256);
	if (!fileName.EndsWith('/') && !fileName.EndsWith('\\'))
		fileName += '/';
#else
    PCSTR home = getenv("HOME");

    if (home == NULL)
    {
    	xpprintf(LOGERR, "$HOME not found\n");
		xpprintf(LOGERR, "You will not be able to save any configuration data\n");
		fileName = "";
		return;
    }
    else
    {
    	fileName = home;
    	fileName += "/.xpilot";

		// see if the .xpilot dir exists
		DIR*	dir;
		if ((dir = opendir(fileName)) == NULL)
		{
			xpprintf(LOGMIN, "Creating directory \"%s\"\n", (PCSTR)fileName);
			if (!mkdir(fileName, 0700))
			{
				xpprintf(LOGERR, "Can't create directory \"%s\"\n", (PCSTR)fileName);
				xpprintf(LOGERR, "You will not be able to save any configuration data\n");
				fileName = "";
				return;
			}
		}
		else
		{
			closedir(dir);
		}
		fileName += "/";
    }
#endif
	fileName += fname;
	fileName += ".ini";
}

///////////////////////////////////////////////////////////////////////////////
void Ini::SkipSwitches(int *argcp, char **argvp, int start, int count)
{
	int		i;
	for (i=start; i<*argcp; i++)
		argvp[i] = argvp[i+count];
	*argcp -= count;
}

///////////////////////////////////////////////////////////////////////////////
unsigned Ini::StringHash(const char *s)
{
	unsigned			hash = 0;

		for (; *s; s++) {
				hash = (((hash >> 29) & 7) | (hash << 3)) ^ *s;
		}
		return hash;
}

///////////////////////////////////////////////////////////////////////////////
int Ini::LocateResource(PCSTR resource, char *result, unsigned size, int *index, PCSTR prefix)
{
#if defined(_UNIX) || defined(_CYGWIN)
	String				s;
	int 				i;
	int 				len;
	char				str_name[80],
						str_class[80],
						*str_type[10];
	XrmValue			rmValue;
	unsigned			hash = StringHash(resource);

	for (i = 0;;) {
		if (hash == options[i].hash && !strcmp(resource, options[i].name)) {
			*index = i;
			break;
		}
		if (++i >= numOptions) {
			seterrno(0);
			error("BUG: Can't find option \"%s\"", resource);
			exit(1);
		}
	}
	s = resource;
	if (s.EndsWith('*'))
	{
		s = s.Left(s.GetLength()-1);
		s += prefix;
	}
	else if (prefix)
	{
		s = prefix;
		s += resource;
	}
	sprintf(str_name, "%s.%s", myName, (PCSTR)s);
	sprintf(str_class, "%s.%c%s", myClass,
			islower(*resource) ? toupper(*resource) : *resource, resource + 1);

	if (XrmGetResource(curDB, str_name, str_class, str_type, &rmValue) == True) {
		if (rmValue.addr == NULL) {
			len = 0;
		} else {
			len = MIN(rmValue.size, size - 1);
			memcpy(result, rmValue.addr, len);
		}
		result[len] = '\0';
		return 1;
	}
	seterrno(0);
	strlcpy(result, options[*index].fallback, size);

	return 0;

#else	/* _WINDOWS */
	GetResource(resource, result, size, index, prefix);

	return 1;
#endif
}


int Ini::GetResource(PCSTR resource, char *result, unsigned size, PCSTR prefix)
{
	int 				index;

	return(LocateResource(resource, result, size, &index, prefix));
}


int Ini::GetStringResource(PCSTR resource, char *result, unsigned size, PCSTR prefix)
{
	char			*src, *dst;
	int 					index, val;

	val = LocateResource(resource, result, size, &index, prefix);
	src = dst = result;
	while ((*src & 0x7f) == *src && isgraph(*src) == 0 && *src != '\0') {
		src++;
	}
	while ((*src & 0x7f) != *src || isgraph(*src) != 0) {
		*dst++ = *src++;
	}
	*dst = '\0';

	return val;
}

int Ini::GetStringResource(PCSTR resource, String* result, PCSTR prefix)
{
	char			*src, *dst;
	int 					index, val;
	char	tmp[256];

	val = LocateResource(resource, tmp, 256, &index, prefix);
	src = dst = tmp;
	while ((*src & 0x7f) == *src && isgraph(*src) == 0 && *src != '\0') {
		src++;
	}
	while ((*src & 0x7f) != *src || isgraph(*src) != 0) {
		*dst++ = *src++;
	}
	*dst = '\0';
	*result = tmp;

	return val;
}


void Ini::GetIntResource(PCSTR resource, int *result, PCSTR prefix)
{
	int 				index;
	char				resValue[MAX_CHARS];

	LocateResource(resource, resValue, sizeof resValue, &index, prefix);
	if (sscanf(resValue, "%d", result) <= 0) {
		error("Bad value \"%s\" for option \"%s\", using default...",
			  resValue, resource);
		sscanf(options[index].fallback, "%d", result);
	}
}


void Ini::GetFloatResource(PCSTR resource, DFLOAT *result)
{
	int 				index;
	double				temp_result;
	char				resValue[MAX_CHARS];

	temp_result = 0.0;
	LocateResource(resource, resValue, sizeof resValue, &index);
	if (sscanf(resValue, "%lf", &temp_result) <= 0) {
		error("Bad value \"%s\" for option \"%s\", using default...",
			  resValue, resource);
		sscanf(options[index].fallback, "%lf", &temp_result);
	}
	*result = (DFLOAT) temp_result;
}


void Ini::GetBoolResource(PCSTR resource, bool *result, PCSTR prefix)
{
	int 				index;
	char				resValue[MAX_CHARS];

	LocateResource(resource, resValue, sizeof resValue, &index, prefix);
	*result = (ON(resValue) != 0);
}


void Ini::GetBitResource(PCSTR resource, long *mask, int bit)
{
	int 				index;
	char				resValue[MAX_CHARS];

	LocateResource(resource, resValue, sizeof resValue, &index);
	if (ON(resValue)) {
		SET_BIT(*mask, bit);
	}
}

#if defined(_UNIX) || defined(_CYGWIN)
void Ini::GetFileDefaults(XrmDatabase *rDBptr)
{
	int 				len;
	char				*ptr,
						*lang = getenv("LANG"),
						*home = getenv("HOME"),
						path[PATH_MAX + 1];
	XrmDatabase 		tmpDB;

	sprintf(path, "%s%s", Conf_libdir(), myClass);
	*rDBptr = XrmGetFileDatabase(path);

	if (lang != NULL) {
		sprintf(path, "/usr/lib/X11/%s/app-defaults/%s", lang, myClass);
		if (access(path, 0) == -1) {
			sprintf(path, "/usr/lib/X11/app-defaults/%s", myClass);
		}
	} else {
		sprintf(path, "/usr/lib/X11/app-defaults/%s", myClass);
	}
	tmpDB = XrmGetFileDatabase(path);
	XrmMergeDatabases(tmpDB, rDBptr);

	if ((ptr = getenv("XUSERFILESEARCHPATH")) != NULL) {
		sprintf(path, "%s/%s", ptr, myClass);
		tmpDB = XrmGetFileDatabase(path);
		XrmMergeDatabases(tmpDB, rDBptr);
	}
	else if ((ptr = getenv("XAPPLRESDIR")) != NULL) {
		if (lang != NULL) {
			sprintf(path, "%s/%s/%s", ptr, lang, myClass);
			if (access(path, 0) == -1) {
				sprintf(path, "%s/%s", ptr, myClass);
			}
		} else {
			sprintf(path, "%s/%s", ptr, myClass);
		}
		tmpDB = XrmGetFileDatabase(path);
		XrmMergeDatabases(tmpDB, rDBptr);
	}
	else if (home != NULL) {
		if (lang != NULL) {
			sprintf(path, "%s/app-defaults/%s/%s", home, lang, myClass);
			if (access(path, 0) == -1) {
				sprintf(path, "%s/app-defaults/%s", home, myClass);
			}
		} else {
			sprintf(path, "%s/app-defaults/%s", home, myClass);
		}
		tmpDB = XrmGetFileDatabase(path);
		XrmMergeDatabases(tmpDB, rDBptr);
	}

	if (dpy && (ptr = XResourceManagerString(dpy)) != NULL) {
		tmpDB = XrmGetStringDatabase(ptr);
		XrmMergeDatabases(tmpDB, rDBptr);
	}
	else if (home != NULL) {
		sprintf(path, "%s/.Xdefaults", home);
		tmpDB = XrmGetFileDatabase(path);
		XrmMergeDatabases(tmpDB, rDBptr);
	}

	if ((ptr = getenv("XENVIRONMENT")) != NULL) {
		tmpDB = XrmGetFileDatabase(ptr);
		XrmMergeDatabases(tmpDB, rDBptr);
	}
	else if (home != NULL) {
		sprintf(path, "%s/.Xdefaults-", home);
		len = strlen(path);
		gethostname(&path[len], sizeof path - len);
		path[sizeof path - 1] = '\0';
		tmpDB = XrmGetFileDatabase(path);
		XrmMergeDatabases(tmpDB, rDBptr);
	}

	//GetXPilotRcFile(path, sizeof(path));
	if (fileName.GetLength())
	{
				tmpDB = XrmGetFileDatabase(fileName);
				XrmMergeDatabases(tmpDB, rDBptr);
	}
}
#endif	/* _WINDOWS*/


#ifdef DEVELOPMENT
static int X_error_handler(Display *display, XErrorEvent *xev)
{
	char				buf[1024];

	fflush(stdout);
	fprintf(stderr, "X error\n");
	XGetErrorText(display, xev->error_code, buf, sizeof buf);
	buf[sizeof(buf) - 1] = '\0';
	fprintf(stderr, "%s\n", buf);
	fflush(stderr);
	*(double *) -3 = 2.10;		/*core dump*/
	exit(1);
	return 0;
}

static void X_after(Display *display)
{
	static int			n;

	if (n < 1000) {
		printf("_X_ %4d\n", n++);
	}
}

static void Get_test_resources(XrmDatabase rDB)
{
	char		*s;
	char testBuffer[256];

	Get_string_resource(rDB, "test", testBuffer, sizeof testBuffer);

	for (s = strtok(testBuffer, ":"); s != NULL; s = strtok(NULL, ":")) {
		if (!strncasecmp(s, "xsync", 3)) {
			XSynchronize(dpy, True);
			XSetErrorHandler(X_error_handler);
		}
		else if (!strncasecmp(s, "xdebug", 4)) {
			XSetErrorHandler(X_error_handler);
		}
		else if (!strncasecmp(s, "after", 5)) {
			XSetAfterFunction(dpy, (int (*)(
#if NeedNestedPrototypes
											Display *
#endif
											)) X_after);
		}
		else if (!strncasecmp(s, "color", 3)) {
			Colors_debug();
		}
		else {
			printf("typo %s\n", s);
			exit(1);
		}
	}
}
#else
static void Get_test_resources(XrmDatabase rDB)
{
}
#endif
