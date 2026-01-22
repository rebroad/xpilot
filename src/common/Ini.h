/* $Id: Ini.h,v 1.30 2004/05/11 06:45:45 dick Exp $
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
 *
 * -----------------------------------------------------------------------
 * Note about the use of the optional "prefix" parameter:
 * I wanted to have two sets of values, but tied to one set of Options.
 * No need to duplicate a list of 30ish identical attributes.
 * (Specifically, the inet ServerListCfg and the private ServerListCfg).
 * So, given an attribute of "ColumnMap" and a prefix of "inet",
 * the option is mapped to ColumnMap, but in the file it is stored 
 * as "inetColumnMap".
 *
 * Later, i wanted a slightly different attribute.  The PrivateServerList
 * contains servers to store, but how many?  I didn't want to create a
 * hard limit.
 * So, a attribute name can end in a star "PrivateServer*".
 * The Option is matched to PrivateServer, and the value of prefix
 * is APPENDED to the attribute name in the file.
 * -----------------------------------------------------------------------
 */
/*
 * $Log: Ini.h,v $
 * Revision 1.30  2004/05/11 06:45:45  dick
 * Handle the wildcard trailing prefix
 *
 * Revision 1.29  2004/05/10 06:26:13  dick
 * Allow an optional prefix to be passed, so that the same Options can fill
 * different objects (inet columns and private columns).
 *
 * Revision 1.28  2004/02/20 05:11:39  dick
 * Windows server saves his window position and state.
 *
 * Revision 1.27  2004/01/06 04:48:28  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 * Revision 1.26  2002/09/12 19:50:33  dick
 * Add GetStringResource() which returns a String.
 *
 * Revision 1.25  2002/09/03 08:00:41  dick
 * Windows needs dpy and myName for dummy placeholders
 *
 * Revision 1.24  2002/09/03 07:43:04  dick
 * Comment out unused code.
 *
 * Revision 1.23  2002/09/02 21:09:13  dick
 * Move generic X resources (dpy and kdpy) from the subclass to the base class Ini.
 *
 * Revision 1.22  2002/09/02 16:54:58  dick
 * Encapsulate myName myClass kdpy and dpy
 *
 * Revision 1.21  2002/09/02 15:45:49  dick
 * Break Ini up into base class Ini and subclass IniClient.
 *
 */
#ifndef	_INI_H_
#define	_INI_H_

#include <stdio.h>

#if defined(_UNIX) || defined(_CYGWIN)
# include <X11/Xlib.h>
# include <X11/Xos.h>
# include <X11/Xutil.h>
# include <X11/Xresource.h>
#endif

#include "portability.h"
#include "keys.h"
#include "const.h"
#include "talk.h"
#include "cstring.h"


#define MAX_VISUAL_NAME	12

#ifndef PATH_MAX
#define PATH_MAX	1023
#endif

class Option {
public:
    PCSTR		name;		/* option name */
    PCSTR		noArg;		/* value for non-argument options */
    PCSTR		fallback;	/* default value */
    keys_t		key;		/* key if not KEY_DUMMY */
    PCSTR		help;		/* user help (multiline) */
    unsigned	hash;		/* option name hashed. */
};


class Ini
{
public:
	Ini();
	~Ini();

	void	SetFileName(PCSTR fname);
	void	SkipSwitches(int *argcp, char **argvp, int start, int count);


	void	SetArgcArgv(int argc, char** argv);
	void	InsertKeyDef(KeySym ks, keys_t key);
	void	DeleteKeyDef(KeySym ks, keys_t key);

	int		SaveAll();
	virtual	void Save(FILE* fp) = 0;

#if defined(_WINDOWS) && !defined(_CYGWIN)
	void	SaveWindowPositions(HWND hWnd);
	bool	GetWindowPositions(RECT* rect);
#endif

public:
	int			numOptions;
	Option*		options;

	Display*	dpy;				// X thingies
	Display*	kdpy;
	char*		myClass;
	char*		myName;
#if defined(_UNIX) || defined(_CYGWIN)
	char		visualName[MAX_VISUAL_NAME];
	char*		geometry;
#endif
#if defined(_WINDOWS)
	int		windowMode;		// 0 = normal, 1 = minimized
#endif
protected:
	String	fileName;
	int		LocateResource(PCSTR resource, char* result, unsigned size, int *index, PCSTR prefix = NULL);
	int		GetResource(PCSTR resource, char* result, unsigned size, PCSTR prefix = NULL);
	void	GetResource(PCSTR resource, char* result, unsigned size, int* index, PCSTR prefix = NULL);
	int		GetStringResource(PCSTR resource, char* result, unsigned size, PCSTR prefix = NULL);
	int		GetStringResource(PCSTR resource, String* result, PCSTR prefix = NULL);
//	void	GetShipshapeResource(char** ship_shape);
//	void	GetIntResource(PCSTR resource, int *result);
	void	GetIntResource(PCSTR resource, int *result, PCSTR prefix = NULL);
	void	GetFloatResource(PCSTR resource, DFLOAT* result);
//	void	GetBoolResource(PCSTR resource, bool* result);
	void	GetBoolResource(PCSTR resource, bool* result, PCSTR prefix = NULL);
	void	GetBitResource(PCSTR resource, long *mask, int bit);
	void	GetFileDefaults(XrmDatabase *rDBptr);

//	void	GetXPilotRcFile(char* path, unsigned size);
	void	SaveResource(FILE* fp, PCSTR resource, PCSTR value);

	void	SaveBool(FILE* fp, PCSTR resource, int value);
	void	SaveFloat(FILE* fp, PCSTR resource, DFLOAT value);
	void	SaveInt(FILE* fp, PCSTR resource, int value);

	unsigned	StringHash(PCSTR s);

#if defined(_WINDOWS) && !defined(_CYGWIN)
	void	GetProfileString(PCSTR key,	PCSTR def, char* result, int size);
#else
	int		Xpilotrc_add(char *line);
#endif
	int				Argc;
	char**			Argv;
	XrmDatabase		curDB;

};

//extern	Ini	ini;	// each app has one instance of an Ini

extern PCSTR s_Settings;
extern PCSTR s_L;
extern PCSTR s_T;
extern PCSTR s_R;
extern PCSTR s_B;
extern PCSTR s_windowMode;

#endif		// _INI_H_
