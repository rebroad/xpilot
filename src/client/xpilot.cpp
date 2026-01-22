/* $Id: xpilot.cpp,v 1.26 2007/02/17 06:18:15 dick Exp $
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
 * $Log: xpilot.cpp,v $
 * Revision 1.26  2007/02/17 06:18:15  dick
 * client/Audio becomes common/AudioMan.
 *
 * Revision 1.25  2007/01/18 21:09:55  dick
 * Add an ErrHandler to the client.
 *
 * Revision 1.24  2005/01/17 04:50:16  dick
 * Set the volume after reading the ini settings
 *
 * Revision 1.23  2004/05/23 23:52:06  dick
 * src/common/IniClient.Defaults.cpp
 *
 * Revision 1.22  2004/05/05 01:01:44  dick
 * Need randommnt.h
 *
 * Revision 1.21  2004/05/02 03:08:00  dick
 * Audio support.  Separate client and server audio support.
 * Servers will probably always build with -DSOUND
 * Clients will build -DSOUND only if OpenAL is present.
 *
 * Revision 1.20  2004/05/01 20:37:36  dick
 * Use the new OpenAL audio object
 *
 * Revision 1.19  2004/04/29 16:08:26  dick
 * compile with SOUND enabled.
 *
 * Revision 1.18  2004/04/26 07:01:07  dick
 * Whitespace
 *
 * Revision 1.17  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.16  2004/01/26 16:56:07  dick
 * Get_process_id becomes GetProcessID
 *
 * Revision 1.15  2004/01/06 04:48:27  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 * Revision 1.14  2002/09/16 21:42:05  dick
 * Use Stringized Connectparam.
 *
 * Revision 1.13  2002/09/02 15:42:40  dick
 * Singleton ini becomes iniClient.
 *
 * Revision 1.12  2002/08/11 21:08:08  dick
 * Remove MFC from the Windows client.
 *
 * Revision 1.11  2002/08/01 15:01:27  dick
 * Send the cookie with the verify message to the server. Retab.
 *
 * Revision 1.10  2002/06/26 04:53:12  dick
 * Client needs szAppName
 *
 * Revision 1.9  2002/05/18 20:55:33  dick
 * Update to XPilot-4.5.1
 *
 * Revision 1.8  2001/09/11 07:33:44  dick
 * Encapsulate all client resources into new class Ini.
 *
 * Revision 1.7  2001/08/14 21:48:26  dick
 * Contact_servers returns a bool
 *
 * Revision 1.6  2001/08/07 22:14:22  dick
 * usleep becomes microdelay.
 *
 * Revision 1.5  2001/08/07 21:01:07  dick
 * struct Connect_param_t becomes class Connectparam
 *
 * Revision 1.4  2001/08/06 05:41:49  dick
 * struct sock_t becomes class Sock
 *
 * Revision 1.3  2001/07/29 06:01:47  dick
 * net.cpp becomes a very nice class Sockbuf.
 *
 * Revision 1.2  2001/07/27 05:47:39  dick
 * Client builds and runs in C++
 */
#include "StdAfx.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>

#if defined(_UNIX) || defined(_CYGWIN)
# include <unistd.h>
# ifndef __hpux
#  include <sys/time.h>
# endif
# include <sys/param.h>
# include <netdb.h>
#else
# include "NT/winNet.h"
# include "NT/winClient.h"
#endif

#include "version.h"
#include "config.h"
#include "xpprintf.h"
#include "client.h"
#include "ClientWorld.h"
#include "const.h"
#include "types.h"
#include "pack.h"
#include "bit.h"
#include "error.h"
#include "Sock.h"
#include "Sockbuf.h"
#include "Connectparam.h"
#include "protoclient.h"
#include "portability.h"
#include "checknames.h"
#include "commonproto.h"
#include "IniClient.h"
#include "draw.h"
#include "AudioMan.h"
#include "randommt.h"

char xpilot_version[] = VERSION;

#ifndef	lint
char xpilot_versionid[] = "@(#)$" TITLE " $";
#endif


//char			hostname[SOCK_HOSTNAME_LENGTH];
String		hostname;

char			**Argv;
int			Argc;
PCSTR	szAppName = "XPilotClient";

static void Check_client_versions(void);

ClientWorld	theWorld;

ErrMsgHandler	emh;			/* Error message output handler */
void*			emhThis;		/* The "this" of the outputter.  Usually a window, maybe stdout */

///////////////////////////////////////////////////////////////////////////////
static void printfile(const char *filename)
{
	FILE				*fp;
	int 				c;


	if ((fp = fopen(filename, "r")) == NULL) {
		return;
	}

	while ((c = fgetc(fp)) != EOF)
		putchar(c);

	fclose(fp);
}

///////////////////////////////////////////////////////////////////////////////
void ErrHandler(void* myThis, ErrMsgType emt, PCSTR ctl, ...)
{
//	ScoreServer* ss = (ScoreServer*)myThis;
    char u[1001];
	va_list marker;

    // Figure through the extra arguments.
	strcpy(u, showtime());
	va_start(marker, ctl);
	vsnprintf(&u[strlen(u)], 1000, ctl, marker);
    va_end(marker);
	strcat(u, "\n");

	xpprintf(ErrMsgTypeToLogLevel(emt), u);
}


///////////////////////////////////////////////////////////////////////////////
/*
 * Oh glorious main(), without thee we cannot exist.
 */
int main(int argc, char *argv[])
{
	int 						result;
	bool				auto_connect = false,
								text = false,
								list_servers = false,
								auto_shutdown = false,
								noLocalMotd = false;
	char						*cp;
	Connectparam				*conpar;
	static char 				shutdown_reason[MAX_CHARS];

	/*
	 * --- Output copyright notice ---
	 */
	xpprintf("  " COPYRIGHT ".\n"
		   "  " TITLE " comes with ABSOLUTELY NO WARRANTY; "
			  "for details see the\n"
		   "  provided LICENSE file.\n\n");
	if (strcmp(Conf_localguru(), "xpilot@xpilot.org")
		&& strcmp(Conf_localguru(), "xpilot@cs.uit.no")) {
		xpprintf("  %s is responsible for the local installation.\n\n",
			   Conf_localguru());
	}

	Argc = argc;
	Argv = argv;

	/*
	 * --- Miscellaneous initialization ---
	 */
	emh = &ErrHandler;
	emhThis = NULL;
	init_error(argv[0]);

	seedMT( (unsigned)time((time_t *)0) ^ GetProcessId());

	Check_client_versions();

	//conpar = (Connectparam *) calloc(1, sizeof(Connectparam));
	conpar = new Connectparam;
	if (!conpar) {
		error("Not enough memory");
		exit(1);
	}
	conpar->contactPort = SERVER_PORT;
	conpar->team = TEAM_NOT_SET;

	cp = getenv("XPILOTHOST");
	if (cp) {
		hostname = cp;
	}
	else {
		SockGetLocalHostname(hostname, 0);
	}
	if (CheckHostName(hostname) == NAME_ERROR) {
		xpprintf("fixing host from \"%s\" ", (PCSTR)hostname);
		FixHostName(hostname);
		xpprintf("to \"%s\"\n", (PCSTR)hostname);
	}

	cp = getenv("XPILOTUSER");
	if (cp) {
		conpar->realName = cp;
	}
	else {
		GetLoginName(conpar->realName);
	}
	if (CheckRealName(conpar->realName) == NAME_ERROR) {
		xpprintf("fixing name from \"%s\" ", (PCSTR)conpar->realName);
		FixRealName(conpar->realName);
		xpprintf("to \"%s\"\n", (PCSTR)conpar->realName);
	}

//	  IFWINDOWS( conpar->disp_name[0] = '\0'; )

	/*
	 * --- Check commandline arguments and resource files ---
	 */
	iniClient.ParseOptions(&argc, argv, conpar->realName,
			  &conpar->contactPort, &conpar->team,
			  &text, &list_servers,
			  &auto_connect, &noLocalMotd,
			  conpar->nick, conpar->dispName,
			  hostname, shutdown_reason);
	Record_init(iniClient.recordFile);
	Validate_shape_str(iniClient.shipShape);
	if (CheckNickName(conpar->nick) == NAME_ERROR) {
		xpprintf("fixing nick from \"%s\" ", (PCSTR)conpar->nick);
		FixNickName(conpar->nick);
		xpprintf("to \"%s\"\n", (PCSTR)conpar->nick);
	}
	if (list_servers) {
		auto_connect = true;
	}
	if (shutdown_reason[0] != '\0') {
		auto_shutdown = true;
		auto_connect = true;
	}

#ifdef SOUND
	audioMan.Init(iniClient.sounds);
	audioMan.SetGain((double)(iniClient.maxVolume)/100.0);
#endif /* SOUND */
	/*
	 * --- Message of the Day ---
	 */
	if (!noLocalMotd)
		printfile(Conf_localmotdfile());

	Simulate();

	if (text || auto_connect || argv[1] || is_this_windows()) {
		xpprintf("Contacting servers\n");
		if (list_servers)
			printf("LISTING AVAILABLE SERVERS:\n");

		result = Contact_servers(argc - 1, &argv[1],
								 auto_connect, list_servers,
								 auto_shutdown, shutdown_reason,
								 0, 0, 0, 0,
								 conpar);
		if (!result)
				result = -1;
	}
	else {
		IFNWINDOWS(result = Welcome_screen(conpar);)
	}

	if (result == true) {
		result = Join(conpar);
	}
	delete conpar;
	return result;
}

/*
 * Verify that all source files making up this program have been
 * compiled for the same version.  Too often bugs have been reported
 * for incorrectly compiled programs.
 */
extern char about_version[];
#ifdef SOUND
extern char audio_version[];
#endif
extern char blockbitmaps_version[];
extern char checknames_version[];
extern char client_version[];
extern char colors_version[];
extern char config_version[];
extern char configure_version[];
extern char datagram_version[];
extern char dbuff_version[];
extern char default_version[];
extern char error_version[];
extern char gfx2d_version[];
extern char guimap_version[];
extern char guiobjects_version[];
extern char join_version[];
extern char math_version[];
extern char net_version[];
extern char netclient_version[];
extern char paint_version[];
extern char paintdata_version[];
extern char painthud_version[];
extern char paintmap_version[];
extern char paintobjects_version[];
extern char paintradar_version[];
extern char portability_version[];
extern char query_version[];
extern char record_version[];
extern char shipshape_version[];
extern char socklib_version[];
extern char talk_version[];
extern char talkmacros_version[];
extern char textinterface_version[];
extern char texture_version[];
extern char welcome_version[];
extern char widget_version[];
extern char xevent_version[];
extern char xeventhandlers_version[];
extern char xinit_version[];
extern char xpilot_version[];
extern char xpmread_version[];


static void Check_client_versions(void)
{
#ifndef _WINDOWS		/* gotta put this back in before source released */
	static struct file_version {
		char			filename[16];
		char			*versionstr;
	} file_versions[] = {
		{ "about", about_version },
		{ "blockbitmaps", blockbitmaps_version },
		{ "checknames", checknames_version },
		{ "client", client_version },
		{ "colors", colors_version },
		{ "config", config_version },
		{ "configure", configure_version },
		{ "datagram", datagram_version },
		{ "dbuff", dbuff_version },
		{ "default", default_version },
		{ "error", error_version },
		{ "gfx2d", gfx2d_version },
		{ "guimap", guimap_version },
		{ "guiobjects", guiobjects_version },
		{ "join", join_version },
		{ "math", math_version },
		{ "net", net_version },
		{ "netclient", netclient_version },
		{ "paint", paint_version },
		{ "paintdata", paintdata_version },
		{ "painthud", painthud_version },
		{ "paintmap", paintmap_version },
		{ "paintobjects", paintobjects_version },
		{ "paintradar", paintradar_version },
		{ "portability", portability_version },
		{ "query", query_version },
		{ "record", record_version },
		{ "shipshape", shipshape_version },
		{ "socklib", socklib_version },
		{ "talk", talk_version },
		{ "talkmacros", talkmacros_version },
		{ "textinterface", textinterface_version },
		{ "texture", texture_version },
		{ "welcome", welcome_version },
		{ "widget", widget_version },
		{ "xevent", xevent_version },
		{ "xeventhandlers", xeventhandlers_version },
		{ "xinit", xinit_version },
		{ "xpilot", xpilot_version },
		{ "xpmread", xpmread_version },
	};
	int 				i;
	int 				oops = 0;

	for (i = 0; i < NELEM(file_versions); i++) {
		if (strcmp(VERSION, file_versions[i].versionstr)) {
			oops++;
			error("Source file %s.c (\"%s\") is not compiled "
				  "for the current version (\"%s\")!",
				  file_versions[i].filename,
				  file_versions[i].versionstr,
				  VERSION);
		}
	}
	if (oops) {
		error("%d version inconsistency errors, cannot continue.", oops);
		error("Please recompile this program properly.");
		exit(1);
	}
#endif
}

