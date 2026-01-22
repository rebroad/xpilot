/* $Id: config.cpp,v 1.21 2007/02/17 06:11:32 dick Exp $
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
 * $Log: config.cpp,v $
 * Revision 1.21  2007/02/17 06:11:32  dick
 * Whitespace
 *
 * Revision 1.20  2005/03/17 22:12:13  kps
 * Get rid of warnings from makedepend about "non-portable whitespace".
 *
 * Revision 1.19  2004/05/13 16:21:19  dick
 * Linux needs to have a shipfile too.
 * Note on Windows, XPilot.shp moves to $/lib .
 *
 * Revision 1.18  2004/05/01 19:11:05  dick
 * sound config file is sounds.conf on all platforms
 *
 * Revision 1.17  2004/03/04 04:21:39  jlmiller
 *
 * Handle "visual" mode for client.
 *
 * Revision 1.16  2004/03/03 08:03:08  dick
 * Determine boot directory for Windows
 *
 * Revision 1.15  2004/02/20 05:11:39  dick
 * Windows server saves his window position and state.
 *
 * Revision 1.14  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.13  2004/01/27 04:38:02  dick
 * Define BROWSER=NoBrowser if its undef'd (prolly Windows where it's not needed
 *
 * Revision 1.12  2004/01/26 17:21:47  dick
 * Add Conf_browser which is the user's preferred browser
 *
 * Revision 1.11  2004/01/20 06:13:53  dick
 * Add support for `make install` to a non-priv'd directory, like /home/dick/xpilot.
 * If we are doing a default installation (/usr/local/xpilot) then the VARDIR
 * becomes /var/xpilot .  If we are not default then use $prefix/var.
 *
 * Revision 1.10  2004/01/13 05:10:00  dick
 * The default map is default.xp for Windows and Unix
 *
 * Revision 1.9  2002/09/12 19:52:23  dick
 * Add Conf_server_ini_file_name() to get path to XPilotServer.ini
 *
 * Revision 1.8  2002/07/24 06:00:17  dick
 * Add support for Linux client password and cookie files.
 *
 * Revision 1.7  2002/07/23 17:47:41  dick
 * Change PASSWORD_FILE_NAME to SERVER_PASSWORD_FILE_NAME and CLIENT_PASSWORD_FILE_NAME
 * and change the supporting routines to match.
 *
 * Revision 1.6  2002/07/08 06:26:07  dick
 * Add VARDIR which is a directory that the server can write data files into.
 *
 * Revision 1.5  2002/06/26 05:02:47  dick
 * Stray char outside of string
 *
 * Revision 1.4  2002/06/25 23:46:13  jlmiller
 *
 * Linux cleanups.
 *
 * Revision 1.3  2002/05/18 20:55:35  dick
 * Update to XPilot-4.5.1
 *
 *
 */
#include "StdAfx.h"

#include <stdlib.h>

#if defined (_WINDOWS)
#include <direct.h>
#endif
#include <string.h>
#include <ctype.h>

#include "version.h"
#include "config.h"
#include "cstring.h"

////////////////////////////////////////////////////////////////////////////////
/*
 * Configure these, that's what they're here for.
 * Explanation about these compile time configuration options
 * is in configure --help , but most likely, nowhere (in xpilot++)
 */
#ifndef LOCALGURU
#    define LOCALGURU		"xpilot@xpilot.org"
#endif

////////////////////////////////////////////////////////////////////////////////
#ifndef	DEFAULT_MAP
#	define DEFAULT_MAP		"default.xp"
#endif

////////////////////////////////////////////////////////////////////////////////
#ifndef LIBDIR
#    if defined(_WINDOWS)
#        define LIBDIR		"lib/"
#    else
#        define LIBDIR		"/usr/local/xpilot/lib/"
#    endif
#endif

////////////////////////////////////////////////////////////////////////////////
#ifndef	VARDIR
#    if defined(_WINDOWS)
#        define VARDIR		"var/"
#    else
#        define VARDIR		"/usr/local/xpilot/var/"
#    endif
#endif

////////////////////////////////////////////////////////////////////////////////
#ifndef	BINDIR
#    if defined(_WINDOWS)
#        define BINDIR		"bin/"
#    else
#        define BINDIR		"/usr/local/xpilot/bin/"
#    endif
#endif

////////////////////////////////////////////////////////////////////////////////
#ifndef DEFAULTS_FILE_NAME
#    if defined(_WINDOWS)
#        define DEFAULTS_FILE_NAME	LIBDIR "defaults.txt"
#    else
#        define DEFAULTS_FILE_NAME	LIBDIR "defaults"
#    endif
#endif

////////////////////////////////////////////////////////////////////////////////
#ifndef SERVER_PASSWORD_FILE_NAME
#    if defined(_WINDOWS)
#        define SERVER_PASSWORD_FILE_NAME	LIBDIR "ServerPassword.txt"
#    else
#        define SERVER_PASSWORD_FILE_NAME	LIBDIR "password"
#    endif
#endif

////////////////////////////////////////////////////////////////////////////////
#ifndef SERVER_INI_FILE_NAME
#    if defined(_WINDOWS)
#        define SERVER_INI_FILE_NAME	"XPilotServer.ini"
#    else
#        define SERVER_INI_FILE_NAME	"XPilotServer.ini"	// Will get $HOME prepended
#    endif
#endif

////////////////////////////////////////////////////////////////////////////////
#ifndef CLIENT_PASSWORD_FILE_NAME
#    if defined(_WINDOWS)
#        define CLIENT_PASSWORD_FILE_NAME	LIBDIR "XPilotPasswords.txt"
#    else
#        define CLIENT_PASSWORD_FILE_NAME	".xpilotpass"		// Will get "$HOME/" prepended
#    endif
#endif

////////////////////////////////////////////////////////////////////////////////
#ifndef CLIENT_COOKIES_FILE_NAME
#    if defined(_WINDOWS)
#        define CLIENT_COOKIES_FILE_NAME	LIBDIR "XPilotCookies.txt"
#    else
#        define CLIENT_COOKIES_FILE_NAME	".xpilotcookies"	// Will get "$HOME/" prepended
#    endif
#endif

////////////////////////////////////////////////////////////////////////////////
#ifndef ROBOTFILE
#    if defined(_WINDOWS)
#		 define	ROBOTFILE	LIBDIR "robots.txt"
#    else
#        define ROBOTFILE	LIBDIR "robots"
#    endif
#endif

////////////////////////////////////////////////////////////////////////////////
#ifndef SERVERMOTDFILE
#    if defined(_WINDOWS)
#		define	SERVERMOTDFILE	LIBDIR "servermotd.txt"
#    else
#        define SERVERMOTDFILE	LIBDIR "servermotd"
#    endif
#endif

////////////////////////////////////////////////////////////////////////////////
#ifndef LOCALMOTDFILE
#    if defined(_WINDOWS)
#		define	LOCALMOTDFILE	LIBDIR "localmotd.txt"
#    else
#        define LOCALMOTDFILE	LIBDIR "localmotd"
#    endif
#endif

////////////////////////////////////////////////////////////////////////////////
#ifndef LOGFILE
#    if defined(_WINDOWS)
#		define	LOGFILE		LIBDIR "log.txt"
#    else
#        define LOGFILE		LIBDIR "log"
#    endif
#endif

////////////////////////////////////////////////////////////////////////////////
#ifndef MAPDIR
#    define MAPDIR		LIBDIR "maps/"
#endif

////////////////////////////////////////////////////////////////////////////////
#ifndef SHIP_FILE
#	define SHIP_FILE	LIBDIR "XPilot.shp"
#endif

////////////////////////////////////////////////////////////////////////////////
#ifndef TEXTUREDIR
#	define TEXTUREDIR	LIBDIR "textures/"
#endif

////////////////////////////////////////////////////////////////////////////////
#ifndef BUTTONDIR
#	define BUTTONDIR	LIBDIR "buttons/"
#endif

////////////////////////////////////////////////////////////////////////////////
#ifndef	SOUNDDIR
#	define SOUNDDIR		LIBDIR "sounds/"
#endif

////////////////////////////////////////////////////////////////////////////////
#ifndef SOUNDFILE
#	define SOUNDFILE	LIBDIR "sounds.conf"
#endif

////////////////////////////////////////////////////////////////////////////////
#ifndef ZCAT_EXT
#    define ZCAT_EXT	".gz"
#endif

#ifndef ZCAT_FORMAT
#    define ZCAT_FORMAT "gzip -d -c < %s"
#endif


////////////////////////////////////////////////////////////////////////////////
/*
 * Please don't change this one.
 */
#ifndef CONTACTADDRESS
#    define CONTACTADDRESS	"xpilot@xpilot.org"
#endif



////////////////////////////////////////////////////////////////////////////////
char config_version[] = VERSION;

static String	bootDirectory;

/*
 * Windows builds all the directories relative to the boot directory. (See above)
 */
void ConfigStartup()
{
#ifdef	_WINDOWS
	String	s(128);	
	if (!getcwd(s.s, 128))
		xpprintf("*Failed to get rootDirectory, errno=%d\n", errno);
	else
	{
		if (s[s.GetLength()] != '\\' || s[s.GetLength()] != '/')
			s += "/";
		bootDirectory = s;
	}
#endif
}

////////////////////////////////////////////////////////////////////////////////
const char* Conf_libdir(void)
{
    static char conf[] = LIBDIR;
    return conf;
}

////////////////////////////////////////////////////////////////////////////////
const char* Conf_vardir(void)
{
    static char conf[] = VARDIR;
    return conf;
}

////////////////////////////////////////////////////////////////////////////////
const char* Conf_bindir(void)
{
    static char conf[] = BINDIR;
    return conf;
}


////////////////////////////////////////////////////////////////////////////////
const char* Conf_defaults_file_name(void)
{
    static char conf[] = DEFAULTS_FILE_NAME;

    return conf;
}

////////////////////////////////////////////////////////////////////////////////
const char* Conf_server_password_file_name(void)
{
    static char conf[] = SERVER_PASSWORD_FILE_NAME;

    return conf;
}

////////////////////////////////////////////////////////////////////////////////
const char* Conf_client_passwords_file_name(void)
{
#ifdef	_WINDOWS
	static	String	s = CLIENT_PASSWORD_FILE_NAME;
	static char conf[] = CLIENT_PASSWORD_FILE_NAME;
#else
	static String s;

	if (s.IsEmpty())
	{
	    const char		*home = getenv("HOME");
	    if (home)
	    {
			s = home;
			if (!s.EndsWith('/'))
				s += '/';
			s += CLIENT_PASSWORD_FILE_NAME;
		}
		else
		{
			s = "/dev/null";
			xpprintf("Can't find $HOME to make client password file\n");
		}
	}

#endif
    return s.s;
}

////////////////////////////////////////////////////////////////////////////////
const char* Conf_server_ini_file_name(void)
{
#ifdef	_WINDOWS
	static	String	s = SERVER_INI_FILE_NAME;
	static char conf[] = SERVER_INI_FILE_NAME;
#else
	static String s;

	if (s.IsEmpty())
	{
	    const char		*home = getenv("HOME");
	    if (home)
	    {
			s = home;
			if (!s.EndsWith('/'))
				s += '/';
			s += ".xpilot/";
			s += SERVER_INI_FILE_NAME;
		}
		else
		{
			s = "/dev/null";
			xpprintf("Can't find $HOME to make server ini file\n");
		}
	}

#endif
    return s.s;
}

////////////////////////////////////////////////////////////////////////////////
const char* Conf_client_cookies_file_name(void)
{
#ifdef	_WINDOWS
	static	String	s = CLIENT_COOKIES_FILE_NAME;
//	static char conf[] = CLIENT_COOKIES_FILE_NAME;
#else
	static String s;

	if (s.IsEmpty())
	{
	    const char		*home = getenv("HOME");
	    if (home)
	    {
			s = home;
			if (!s.EndsWith('/'))
				s += '/';
			s += CLIENT_COOKIES_FILE_NAME;
		}
		else
		{
			s = "/dev/null";
			xpprintf("Can't find $HOME to make client cookie file\n");
		}
	}

#endif
    return s.s;
}

////////////////////////////////////////////////////////////////////////////////
const char* Conf_mapdir(void)
{
    static char conf[] = MAPDIR;
    return conf;
}

////////////////////////////////////////////////////////////////////////////////
const char* Conf_browser(void)
{
#	ifndef	BROWSER
#	define	BROWSER	"NoBrowser"
#	endif
    static char conf[] = BROWSER;
    return conf;
}

////////////////////////////////////////////////////////////////////////////////
static char conf_default_map_string[] = DEFAULT_MAP;

const char* Conf_default_map(void)
{
    return conf_default_map_string;
}

////////////////////////////////////////////////////////////////////////////////
const char* Conf_servermotdfile(void)
{
    static char conf[] = SERVERMOTDFILE;
    static char env[] = "XPILOTSERVERMOTD";
    char *filename;

    filename = getenv(env);
    if (filename == NULL) {
	filename = conf;
    }

    return filename;
}

////////////////////////////////////////////////////////////////////////////////
const char* Conf_localmotdfile(void)
{
    static char conf[] = LOCALMOTDFILE;

    return conf;
}

////////////////////////////////////////////////////////////////////////////////
char	conf_logfile_string[] = LOGFILE;

const char* Conf_logfile(void)
{
    return conf_logfile_string;
}

////////////////////////////////////////////////////////////////////////////////
/* needed by client/default.c */
char conf_ship_file_string[] = SHIP_FILE;

const char* Conf_ship_file(void)
{
    return conf_ship_file_string;
}

////////////////////////////////////////////////////////////////////////////////
/* needed by client/default.c */
char conf_texturedir_string[] = TEXTUREDIR;

const char* Conf_texturedir(void)
{
    return conf_texturedir_string;
}

////////////////////////////////////////////////////////////////////////////////
String confButtonDir;

PCSTR Conf_buttondir(void)
{
	if (confButtonDir.IsEmpty())
	{
		confButtonDir.printf("%s%s", (PCSTR)bootDirectory, BUTTONDIR);
	}
    return confButtonDir;
}

////////////////////////////////////////////////////////////////////////////////
/* needed by client/default.c */
char conf_soundfile_string[] = SOUNDFILE;

const char* Conf_soundfile(void)
{
    return conf_soundfile_string;
}

////////////////////////////////////////////////////////////////////////////////
const char* Conf_localguru(void)
{
    static char conf[] = LOCALGURU;

    return conf;
}

////////////////////////////////////////////////////////////////////////////////
const char* Conf_contactaddress(void)
{
    static char conf[] = CONTACTADDRESS;

    return conf;
}

////////////////////////////////////////////////////////////////////////////////
static char conf_robotfile_string[] = ROBOTFILE;

const char* Conf_robotfile(void)
{
    return conf_robotfile_string;
}

////////////////////////////////////////////////////////////////////////////////
const char* Conf_zcat_ext(void)
{
    static char conf[] = ZCAT_EXT;

    return conf;
}

////////////////////////////////////////////////////////////////////////////////
const char* Conf_zcat_format(void)
{
    static char conf[] = ZCAT_FORMAT;

    return conf;
}

////////////////////////////////////////////////////////////////////////////////
const char* Conf_sounddir(void)
{
    static char conf[] = SOUNDDIR;

    return conf;
}

