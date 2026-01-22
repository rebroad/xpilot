/* $Id: config.h,v 1.29 2005/03/17 22:12:13 kps Exp $
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
 * $Log: config.h,v $
 * Revision 1.29  2005/03/17 22:12:13  kps
 * Get rid of warnings from makedepend about "non-portable whitespace".
 *
 * Revision 1.28  2004/03/03 08:03:08  dick
 * Determine boot directory for Windows
 *
 * Revision 1.27  2004/01/26 17:21:47  dick
 * Add Conf_browser which is the user's preferred browser
 *
 * Revision 1.26  2004/01/20 06:13:53  dick
 * Add support for `make install` to a non-priv'd directory, like /home/dick/xpilot.
 * If we are doing a default installation (/usr/local/xpilot) then the VARDIR
 * becomes /var/xpilot .  If we are not default then use $prefix/var.
 *
 * Revision 1.25  2002/09/12 19:52:23  dick
 * Add Conf_server_ini_file_name() to get path to XPilotServer.ini
 *
 * Revision 1.24  2002/09/09 23:27:52  dick
 * define MAX_WORLDS as max worlds supported by server and control
 *
 */

#ifndef CONFIG_H
#define CONFIG_H


// max worlds that a server can run
#define	MAX_WORLDS		4


/*
 * The following macros decide the speed of the game and
 * how often the server should draw a frame.  (Hmm...)
 */

#ifndef	UPDATES_PR_FRAME
#    define UPDATES_PR_FRAME	1
#endif

/*
 * If COMPRESSED_MAPS is defined, the server will attempt to uncompress
 * maps on the fly (but only if neccessary). ZCAT_FORMAT should produce
 * a command that will unpack the given .Z file to stdout (for use in popen).
 * ZCAT_EXT should define the proper compressed file extension.
 */

#if defined(_WINDOWS)
#    ifdef COMPRESSED_MAPS
	/*
	 * Couldn't find a popen(), also compress and gzip don't exist.
	 */
#        undef COMPRESSED_MAPS
#    endif
#else
#    define COMPRESSED_MAPS
#endif

#ifdef	_DEBUG
#define	DEBUG	1
#endif

#ifdef _WINDOWS
#	ifdef	_DEBUG
#		define	DEBUG	1
#		define	D(x)	{x;}
#	else
#		define	D(x)
#	endif
#else
#	ifdef	DEBUG
#		define D(x)	{ {x}; fflush(stdout); }
#	else
#		define D(x)
#	endif
#endif

/* Trace during socket debugging */
#ifdef	_SOCKDEBUG
#	ifndef	_DEBUG
#	error "_SOCKDEBUG requires _DEBUG to be defined as well"
#	else
#	define	SOTRACE		Trace
#	endif
#else
#define	SOTRACE
#endif


/* Windows doesn't play with stdin/out well at all...
 * So for the client i route the "debug" printfs to the debug stream
 * The server gets 'real' messages routed to the messages window 
 */
extern void ErrorHandler(const char *, ...);

extern void warn(const char*, ...);
extern void fatal(const char*, ...);

#define	LOGNONE		0		// log nothing
#define	LOGERR		1		// log only errors
#define	LOGMIN		2		// minimal info
#define	LOGMED		3
#define	LOGLOTS		4
#define	LOGMAX		5		// ludicrous speed!

extern	void	xpprintf(const char* lpszFormat, ...);
extern	void	xpprintf(int level, const char* lpszFormat, ...);

#if defined(_WINDOWS)
#	if defined	_XPILOTNTSERVER_
#		define	xpprintf	xpprintfW
		extern	void	xpprintfW(const char* lpszFormat, ...);
#	endif
#endif

const char* Conf_libdir(void);
const char* Conf_vardir(void);
const char* Conf_bindir(void);
const char* Conf_browser(void);
const char* Conf_defaults_file_name(void);
const char* Conf_server_password_file_name(void);
const char* Conf_client_passwords_file_name(void);
const char* Conf_client_cookies_file_name(void);
const char* Conf_server_ini_file_name(void);
const char* Conf_mapdir(void);
const char* Conf_default_map(void);
const char* Conf_servermotdfile(void);
const char* Conf_localmotdfile(void);
const char* Conf_logfile(void);
const char* Conf_ship_file(void);
const char* Conf_mapdir(void);
const char* Conf_texturedir(void);
const char* Conf_buttondir(void);
const char* Conf_sounddir(void);
const char* Conf_soundfile(void);
const char* Conf_localguru(void);
const char* Conf_contactaddress(void);
const char* Conf_robotfile(void);
const char* Conf_zcat_ext(void);
const char* Conf_zcat_format(void);

extern void ConfigStartup();

#endif /* CONFIG_H */
