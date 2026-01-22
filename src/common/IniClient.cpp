/* $Id: IniClient.cpp,v 1.26 2004/07/22 17:00:52 dick Exp $
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
 * $Log: IniClient.cpp,v $
 * Revision 1.26  2004/07/22 17:00:52  dick
 * If the ShipShape (or name) is "" after parsing, then give us the default ship string.
 *
 * Revision 1.25  2004/06/07 19:03:00  dick
 * Use MAX_HOST_LEN for host, not MAX_NAME_LEN
 *
 * Revision 1.24  2004/05/30 08:16:02  dick
 * Don't use xp_strdup to a String, because that just leaks the copy (cause String copied it).
 *
 * Revision 1.23  2004/05/28 18:57:22  dick
 * gcc3 sez you can't pass a String through ...
 *
 * Revision 1.22  2004/05/28 17:55:58  dick
 * shipShape becomes a String (not a char*).
 * client's MAX_MSGS goes global to here.
 *
 * Revision 1.21  2004/05/13 17:38:08  dick
 * Don't do #ifdef SOUND in common.  Always compile it.
 *
 * Revision 1.20  2004/05/13 17:13:27  dick
 * Don't do #ifdef SOUND in common.  Always compile it.
 *
 * Revision 1.19  2004/05/11 06:33:13  dick
 * audioServer is gone
 *
 * Revision 1.18  2004/05/01 19:11:20  dick
 * Whitespace
 *
 * Revision 1.17  2004/04/29 16:08:27  dick
 * compile with SOUND enabled.
 *
 * Revision 1.16  2004/03/04 04:21:39  jlmiller
 *
 * Handle "visual" mode for client.
 *
 * Revision 1.15  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.14  2004/01/24 02:51:26  dick
 * Fix the determination of nick=user@host .
 *
 * Revision 1.13  2004/01/20 15:14:27  dick
 * Get rid of #define WINDOWSCALING.  Window scaling is always built, it's not an option.
 *
 * Revision 1.12  2004/01/09 05:48:18  dick
 * If nickname is empty, parse one up
 *
 * Revision 1.11  2004/01/08 02:23:45  dick
 * Compare the nick's length, not it's address
 *
 * Revision 1.10  2004/01/07 04:41:05  dick
 * More CYGWIN changes
 *
 * Revision 1.9  2004/01/06 04:48:28  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 * Revision 1.8  2003/12/22 09:44:00  dick
 * Don't exit on a bad nickname
 *
 * Revision 1.7  2002/09/16 22:16:09  dick
 * Use Stringized nickName.
 *
 * Revision 1.6  2002/09/16 21:26:19  dick
 * Stringize ParseOptions().
 *
 * Revision 1.5  2002/09/03 08:01:07  dick
 * Windows doesn't know about the "visual" resource.
 *
 * Revision 1.4  2002/09/03 07:42:03  dick
 * whitespace
 *
 * Revision 1.3  2002/09/02 21:09:13  dick
 * Move generic X resources (dpy and kdpy) from the subclass to the base class Ini.
 *
 * Revision 1.2  2002/09/02 16:54:58  dick
 * Encapsulate myName myClass kdpy and dpy
 *
 * Revision 1.1  2002/09/02 15:45:49  dick
 * Break Ini up into base class Ini and subclass IniClient.
 *
 */
#include "StdAfx.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>

#ifndef _WINDOWS
# include <unistd.h>
# include <X11/Xos.h>
# include <X11/keysym.h>
# include <X11/Xlib.h>
# include <X11/Xresource.h>
# ifdef	__apollo
#  include <X11/ap_keysym.h>
# endif
# include <sys/param.h>
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
#include "Sock.h"
#include "IniClient.h"
//#include "client.h"

#define DISPLAY_ENV	"DISPLAY"
#define DISPLAY_DEF	":0.0"
#define KEYBOARD_ENV	"KEYBOARD"

#ifndef PATH_MAX
#define PATH_MAX	1023
#endif



#ifdef SPARC_CMAP_HACK
char  frameBuffer[MAX_CHARS]; /* frame buffer */
#endif



//extern char *talk_fast_msgs[];	/* talk macros */
char talk_fast_temp_buf[7];		/* can handle up to 999 fast msgs */
char *talk_fast_temp_buf_big;


static void Get_test_resources(XrmDatabase rDB);
//static void Usage(void);		// This needs to be moved to xpilot.cpp and XPKbConfig.cpp


keys_t buttonDefs[MAX_POINTER_BUTTONS][MAX_BUTTON_DEFS+1];

//extern	Option options[];
//extern	const int	numOptions;

// The one and only ini for the client
IniClient	iniClient;

///////////////////////////////////////////////////////////////////////////////
IniClient::IniClient()
{
	SetDefaultOptions();
	serverport = SERVER_PORT;
	keyDefs = NULL;
#ifdef	_UNIX
	myName = "xpilot";
	myClass = "XPilot";
#endif
}

///////////////////////////////////////////////////////////////////////////////
IniClient::~IniClient()
{
	if (keyDefs) {
		free(keyDefs);
		keyDefs = NULL;
	}
	if (texturePath) {
		free(texturePath);
		texturePath = NULL;
	}
	if (wallTextureFile) {
		free(wallTextureFile);
		wallTextureFile = NULL;
	}
	if (decorTextureFile) {
		free(decorTextureFile);
		decorTextureFile = NULL;
	}
	if (ballTextureFile) {
		free(ballTextureFile);
		ballTextureFile = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
void IniClient::ParseOptions(int *argcp, char **argvp, String& realName, int *port,
							 int *my_team, bool* text, bool* list,
							 bool* join, bool* noLocalMotd,
							 String& nickName, String& dispName, String& hostName,
							 char *shut_msg)
{
	char*		ptr;
	char*		str;
	int			i;
	int			j;
	int			num;
	int			firstKeyDef;
	keys_t		key;
	KeySym		ks;

	char				resValue[MAX(2*MSG_LEN, PATH_MAX + 1)];
	XrmDatabase			argDB = 0;
	XrmDatabase			rDB = 0;

	D(xpprintf("%s Start parse\n", showtime());)
#if defined(_WINDOWS) && !defined(_CYGWIN)
	ParseSwitches(argcp, argvp);
	if (port)
	{
		*port = serverport;
		port = NULL;
	}
	if (my_team)
	{
		*my_team = team;
		my_team = NULL;
	}
#else
	XrmOptionDescRec	*xopt;
	int 				size;

	XrmInitialize();

	/*
	 * Construct a Xrm Option table from our options array.
	 */
	size = sizeof(*xopt) * numOptions;
	for (i = 0; i < numOptions; i++) {
		size += 2 * (strlen(options[i].name) + 2);
	}
	if ((ptr = (char *)malloc(size)) == NULL) {
		error("No memory for options");
		exit(1);
	}
	xopt = (XrmOptionDescRec *)ptr;
	ptr += sizeof(*xopt) * numOptions;
	for (i = 0; i < numOptions; i++) {
		options[i].hash = StringHash(options[i].name);
		xopt[i].option = ptr;
		xopt[i].option[0] = '-';
		strcpy(&xopt[i].option[1], options[i].name);
		size = strlen(options[i].name) + 2;
		ptr += size;
		xopt[i].specifier = ptr;
		xopt[i].specifier[0] = '.';
		strcpy(&xopt[i].specifier[1], options[i].name);
		ptr += size;
		if (options[i].noArg) {
			xopt[i].argKind = XrmoptionNoArg;
			xopt[i].value = (char *)options[i].noArg;
		}
		else {
			xopt[i].argKind = XrmoptionSepArg;
			xopt[i].value = NULL;
		}
	}

	XrmParseCommand(&argDB, xopt, numOptions, myName, argcp, argvp);

	/*
	 * Check for bad arguments.
	 */
	for (i = 1; i < *argcp; i++) {
		if (argvp[i][0] == '-' || argvp[i][0] == '+') {
			seterrno(0);
			error("Unknown or incomplete option '%s'", argvp[i]);
			error("Type: %s -help to see a list of options", argvp[0]);
			exit(1);
		}
		/* The rest of the arguments are hostnames of servers. */
	}
	curDB = argDB;
/*	if (GetResource("help", resValue, sizeof resValue) != 0) {
		Usage();
	}
*/
	if (GetResource("version", resValue, sizeof resValue) != 0) {
		puts(TITLE);
		exit(0);
	}
	if (dispName)	// always true
	{

		if (GetStringResource("display", &dispName) == 0
				|| dispName.IsEmpty()) 
		{
			if ((ptr = getenv(DISPLAY_ENV)) != NULL) 
				dispName = ptr;
			else
				dispName = DISPLAY_DEF;
		}
		if ((dpy = XOpenDisplay(dispName)) == NULL) {
			error("Can't open display '%s'", (PCSTR)dispName);
			if (strcmp(dispName, "NO_X") == 0) {
				/* user does not want X stuff.	experimental.  use at own risk. */
				if (nickName)
					nickName = realName;
				if (my_team)
					*my_team = TEAM_NOT_SET;
				if (port)
					GetIntResource("port", port);
				if (list)
					GetBoolResource("list", list);
				if (text)
					*text = true;
				if (join)
					*join = false;
				if (noLocalMotd)
					*noLocalMotd = true;
				XrmDestroyDatabase(argDB);
				free(xopt);
				return;
			}
		exit(1);
		}
	}

	if (GetStringResource("keyboard", resValue, MAX_DISP_LEN) == 0
		|| resValue[0] == '\0') {
		if ((ptr = getenv(KEYBOARD_ENV)) != NULL) {
			strlcpy(resValue, ptr, MAX_DISP_LEN);
		}
	}
	if (resValue[0] == '\0') {
		iniClient.kdpy = NULL;
	} else if ((iniClient.kdpy = XOpenDisplay(resValue)) == NULL) {
		error("Can't open keyboard '%s'", resValue);
		exit(1);
	}
	
	GetStringResource("visual", visualName, MAX_VISUAL_NAME);
		// InitKeysyms();
	GetFileDefaults(&rDB);

	XrmMergeDatabases(argDB, &rDB);
	curDB = rDB;

	GetStringResource("geometry", resValue, sizeof resValue);
	geometry = xp_strdup(resValue);
#endif
	if ((talk_fast_temp_buf_big = (char *)malloc(TALK_FAST_MSG_SIZE)) != NULL) {
		for (i = 0; i < TALK_FAST_NR_OF_MSGS; ++i) {
			sprintf (talk_fast_temp_buf, "msg%d", i + 1);
			GetResource(talk_fast_temp_buf, talk_fast_temp_buf_big, TALK_FAST_MSG_SIZE);
			talk_fast_msgs[i] = xp_strdup (talk_fast_temp_buf_big);
		}
		free (talk_fast_temp_buf_big);
	}
	else {
		for (i = 0; i < TALK_FAST_NR_OF_MSGS; ++i) {
			talk_fast_msgs[i] = NULL;
		}
	}

	if (shut_msg)
		GetResource("shutdown", shut_msg, MAX_CHARS);

	IFNWINDOWS( GetIntResource("ignoreWindowManager", &ignoreWindowManager); )


	GetResource("user", resValue, MAX_NAME_LEN);
	if (resValue[0] && realName)
	{
		realName = resValue;
		FixRealName(realName);
		//strlcpy(realname, realName, sizeof(realname));
		realname.printf(realName);
	}
	else if (resValue[0])
	{
		realName.printf(resValue);
	}

	ptr = getenv("XPILOTUSER");
	if (ptr)
		realName = ptr;
	else
	{
		GetResource("user", resValue, MAX_NAME_LEN);
		if (resValue[0])
		{
			realName = resValue;
			FixRealName(realName);
			//strlcpy(realname, realName, sizeof(realname));
			realname.printf(realName);
		}
		else
			GetLoginName(realName);

	}
	if (CheckRealName(realName) == NAME_ERROR) 
	{
		xpprintf("fixing name from \"%s\" ", (PCSTR)realName);
		FixRealName(realname);
		xpprintf("to \"%s\"\n", (PCSTR)realName);
	}

	ptr = getenv("XPILOTHOST");
	if (ptr)
		hostName = ptr;
	else {
		GetResource("host", resValue, MAX_HOST_LEN);
		if (resValue[0])
		{
			hostName = resValue;
			FixHostName(hostName);
			hostname.printf(hostName);
		}
		else
			SockGetLocalHostname(hostName, 0);
	}
	if (CheckHostName(hostName) == NAME_ERROR) {
		xpprintf("fixing host from \"%s\" ", (PCSTR)hostName);
		FixHostName(hostName);
		xpprintf("to \"%s\"\n", (PCSTR)hostName);
	}


	GetResource("name", resValue, MAX_NAME_LEN);
	if (!nickName.GetLength())
	{
		char	buf[MAX_NAME_LEN];
		GetResource("name", buf, MAX_NAME_LEN);
		if (!buf[0])
			strlcpy(buf, realName, MAX_NAME_LEN);
		buf[0] = CAP_LETTER(buf[0]);
		/*
		if (buf[0] < 'A' || buf[0] > 'Z') 
		{
			error("Your player name \"%s\" should start with an uppercase letter",
				  buf);
		}
		*/
		nickName = buf;
		nickName.TrimTrailingWhiteSpace();
		name.printf(nickName);
	}
	else
	{
		GetResource("name", resValue, MAX_NAME_LEN);
		if (resValue[0])
			name.printf(resValue);
	}


	if (my_team)
	{
		GetIntResource("team", my_team);
//		IFWINDOWS( GetTeam(my_team); )
		if (*my_team < 0 || *my_team > 9) 
			*my_team = TEAM_NOT_SET;
		team = *my_team;
	}

	IFWINDOWS( if (name.s[0] == '\0') {
					/* Windows may have no default name */
					name.printf("NoName");
				}
	)


	if (port)
		GetIntResource("port", port);
	if (text)
		GetBoolResource("text", text);
	if (list)
		GetBoolResource("list", list);
	if (join)
		GetBoolResource("join", join);
	if (noLocalMotd)
		GetBoolResource("noLocalMotd", noLocalMotd);
	GetBoolResource("autoServerMotdPopup", &autoServerMotdPopup);
//	  autoServerMotdPopup = (i != 0) ? true : false;
	GetBoolResource("refreshMotd", &refreshMotd);
//	  refreshMotd = (i != 0) ? true : false;

	GetShipshapeResource(&shipShape);
//	  GetResource("shipShapeName", shipShapeName, MAX_NAME_LEN);
//FIXME    Validate_shape_str(shipShape);

	GetFloatResource("power", &power);
	GetFloatResource("turnSpeed", &turnspeed);
	GetFloatResource("turnResistance", &turnresistance);
	GetFloatResource("altPower", &power_s);
	GetFloatResource("altTurnSpeed", &turnspeed_s);
	GetFloatResource("altTurnResistance", &turnresistance_s);

	GetFloatResource("sparkProb", &sparkProb);
	sparkRand = (int)(sparkProb * MAX_SPARK_RAND + 0.5f);
	GetIntResource("charsPerSecond", &charsPerSecond);
	GetBoolResource("markingLights", &markingLights);
//	  markingLights = (i == false) ? false : true;

	GetIntResource("backgroundPointDist", &mapPointDistance);
	GetIntResource("backgroundPointSize", &mapPointSize);
	LIMIT(mapPointSize, MIN_MAP_POINT_SIZE, MAX_MAP_POINT_SIZE);
	GetIntResource("sparkSize", &sparkSize);
	LIMIT(sparkSize, MIN_SPARK_SIZE, MAX_SPARK_SIZE);
	GetIntResource("shotSize", &shotSize);
	LIMIT(shotSize, MIN_SHOT_SIZE, MAX_SHOT_SIZE);
	GetIntResource("teamShotSize", &teamShotSize);
	LIMIT(teamShotSize, MIN_TEAMSHOT_SIZE, MAX_TEAMSHOT_SIZE);
	GetBoolResource("showNastyShots", &showNastyShots);
	GetBoolResource("titleFlip", &titleFlip);
	/*
	 * This is a special value; default or not defined means choose depending
	 * on the display, otherwise its a boolean value.
	 */
	GetStringResource("shieldDrawSolid", resValue, sizeof resValue);
	if (strncasecmp(resValue, "default", 7) == 0)
		shieldDrawMode = -1;
	else
		shieldDrawMode = ON(resValue);
	GetBoolResource("toggleShield", &toggleShield);
	GetBoolResource("autoShield", &autoShield);

	GetIntResource("clientPortStart", &clientPortStart);
	GetIntResource("clientPortEnd", &clientPortEnd);


	GetResource("modifierBank1", modBankStr[0], sizeof modBankStr[0]);
	GetResource("modifierBank2", modBankStr[1], sizeof modBankStr[1]);
	GetResource("modifierBank3", modBankStr[2], sizeof modBankStr[2]);
	GetResource("modifierBank4", modBankStr[3], sizeof modBankStr[3]);

	IFNWINDOWS( GetResource("visual", visualName, sizeof visualName); )
	GetBoolResource("mono", &mono);
	GetBoolResource("colorSwitch", &colorSwitch);
	GetBoolResource("multibuffer", &multiBuffer);

	/* Windows already derived maxColors in InitWinX */
	IFNWINDOWS( GetIntResource("maxColors", &maxColors); )

	GetStringResource("black", colorNames[0], sizeof(colorNames[0]));
	GetStringResource("white", colorNames[1], sizeof(colorNames[1]));
	GetStringResource("blue", colorNames[2], sizeof(colorNames[2]));
	GetStringResource("red", colorNames[3], sizeof(colorNames[3]));
	for (i = 0; i < MAX_COLORS; i++) {
		char buf[16];
		sprintf(buf, "color%d", i);
		if (!GetStringResource(buf, resValue, MAX_COLOR_LEN)) {
			if (i < NUM_COLORS) {
				strlcpy(resValue, colorNames[i], MAX_COLOR_LEN);
			}
		}
		strlcpy(colorNames[i], resValue, MAX_COLOR_LEN);
	}
	GetIntResource("hudColor", &hudColor);
	GetIntResource("hudLockColor", &hudLockColor);
	GetIntResource("wallColor", &wallColor);
	GetIntResource("wallRadarColor", &wallRadarColor);
	GetIntResource("decorColor", &decorColor);
	GetIntResource("decorRadarColor", &decorRadarColor);
	GetIntResource("targetRadarColor", &targetRadarColor);
	GetIntResource("oldMessagesColor", &oldMessagesColor);
	GetResource("sparkColors", sparkColors, MSG_LEN);

	instruments = 0;
	GetBitResource("showShipName", &instruments, SHOW_SHIP_NAME);
	GetBitResource("showMineName", &instruments, SHOW_MINE_NAME);
	GetBitResource("showMessages", &instruments, SHOW_MESSAGES);
	GetBitResource("showHUD", &instruments, SHOW_HUD_INSTRUMENTS);
		GetBitResource("showHUDRadar", &instruments, SHOW_HUD_RADAR);
	GetBitResource("verticalHUDLine", &instruments, SHOW_HUD_VERTICAL);
	GetBitResource("horizontalHUDLine", &instruments, SHOW_HUD_HORIZONTAL);
	GetBitResource("fuelMeter", &instruments, SHOW_FUEL_METER);
	GetBitResource("fuelGauge", &instruments, SHOW_FUEL_GAUGE);
	GetBitResource("turnSpeedMeter", &instruments, SHOW_TURNSPEED_METER);
	GetBitResource("powerMeter", &instruments, SHOW_POWER_METER);
	GetBitResource("packetSizeMeter", &instruments, SHOW_PACKET_SIZE_METER);
	GetBitResource("packetLossMeter", &instruments, SHOW_PACKET_LOSS_METER);
	GetBitResource("packetDropMeter", &instruments, SHOW_PACKET_DROP_METER);
		GetBitResource("packetLagMeter", &instruments, SHOW_PACKET_LAG_METER);
	GetBitResource("slidingRadar", &instruments, SHOW_SLIDING_RADAR);
	GetBitResource("showItems", &instruments, SHOW_ITEMS);
	GetBitResource("clock", &instruments, SHOW_CLOCK);
	GetBitResource("clockAMPM", &instruments, SHOW_CLOCK_AMPM_FORMAT);
	GetBitResource("outlineWorld", &instruments, SHOW_OUTLINE_WORLD);
	GetBitResource("filledWorld", &instruments, SHOW_FILLED_WORLD);
	GetBitResource("texturedWalls", &instruments, SHOW_TEXTURED_WALLS);
	GetBitResource("showDecor", &instruments, SHOW_DECOR);
	GetBitResource("outlineDecor", &instruments, SHOW_OUTLINE_DECOR);
	GetBitResource("filledDecor", &instruments, SHOW_FILLED_DECOR);
	GetBitResource("texturedDecor", &instruments, SHOW_TEXTURED_DECOR);
	GetBitResource("texturedBalls", &instruments, SHOW_TEXTURED_BALLS);
	GetBitResource("reverseScroll", &instruments, SHOW_REVERSE_SCROLL);

	GetBoolResource("texturedObjects", &blockBitmaps);
	GetBoolResource("pointerControl", &initialPointerControl);
	GetBoolResource("erase", &useErase);
	GetFloatResource("showItemsTime", &showItemsTime);
	LIMIT(showItemsTime, MIN_SHOW_ITEMS_TIME, MAX_SHOW_ITEMS_TIME);

	GetIntResource("showScoreDecimals", &showScoreDecimals);
	LIMIT(showScoreDecimals, 0, 2);

	GetFloatResource("speedFactHUD", &hudMoveFact);
	GetFloatResource("speedFactPTR", &ptrMoveFact);
	GetIntResource("fuelNotify", &fuelLevel3);
	GetIntResource("fuelWarning", &fuelLevel2);
	GetIntResource("fuelCritical", &fuelLevel1);

	GetResource("gameFont", gameFontName, sizeof gameFontName);
	GetResource("messageFont", messageFontName, sizeof messageFontName);
	GetResource("scoreListFont", scoreListFontName, sizeof scoreListFontName);
	GetResource("buttonFont", buttonFontName, sizeof buttonFontName);
	GetResource("textFont", textFontName, sizeof textFontName);
	GetResource("talkFont", talkFontName, sizeof talkFontName);
	GetResource("motdFont", motdFontName, sizeof motdFontName);

	GetIntResource("maxMessages", &maxMessages);
		GetIntResource("messagesToStdout", &messagesToStdout);
		LIMIT(messagesToStdout, 0, 2);

#ifndef _WINDOWS
	GetBoolResource("selectionAndHistory", &selectionAndHistory);
	GetIntResource("maxLinesInHistory", &maxLinesInHistory);
	LIMIT(maxLinesInHistory, 1, MAX_HIST_MSGS);
#endif

	GetIntResource("receiveWindowSize", &receiveWindowSize);
	LIMIT(receiveWindowSize, MIN_RECEIVE_WINDOW_SIZE, MAX_RECEIVE_WINDOW_SIZE);

	GetResource("recordFile", resValue, sizeof resValue);
	recordFile = xp_strdup(resValue);
//FIXME    Record_init(resValue);
	GetResource("texturePath", resValue, sizeof resValue);
	texturePath = xp_strdup(resValue);
	GetResource("wallTextureFile", resValue, sizeof resValue);
	wallTextureFile = xp_strdup(resValue);
	GetResource("decorTextureFile", resValue, sizeof resValue);
	decorTextureFile = xp_strdup(resValue);
	GetResource("ballTextureFile", resValue, sizeof resValue);
	ballTextureFile = xp_strdup(resValue);
	GetResource("shipShapeFile", resValue, sizeof resValue);
	shipShapeFile = resValue;

	GetIntResource("maxFPS", &maxFPS);
//FIXME    oldMaxFPS = maxFPS;

	IFWINDOWS( GetIntResource("radarDivisor", &radarDivisor); )
	IFWINDOWS( GetBoolResource("threadedDraw", &threadedDraw); )

	GetFloatResource("scaleFactor", &scaleFactor);
	if (scaleFactor == 0.0) {
		scaleFactor = 1.0;
	}
	LIMIT(scaleFactor, MIN_SCALEFACTOR, MAX_SCALEFACTOR);
	GetFloatResource("altScaleFactor", &scaleFactor_s);
	if (scaleFactor_s == 0.0) {
		scaleFactor_s = 2.0;
	}
	LIMIT(scaleFactor_s, MIN_SCALEFACTOR, MAX_SCALEFACTOR);

	GetStringResource("sounds", sounds, sizeof sounds);
	GetIntResource("maxVolume", &maxVolume);

#ifdef SPARC_CMAP_HACK
	Get_string_resource(rDB, "frameBuffer", frameBuffer, sizeof frameBuffer);
#endif

	Get_test_resources(rDB);

	/*
	* Key bindings
	*/
	InitKeysyms();
	maxKeyDefs = 2 * NUM_KEYS;
	if (!(keyDefs = (KeyDef*)malloc(maxKeyDefs * sizeof(KeyDef)))) 
	{
		error("No memory for key bindings");
		exit(1);
	}
	num = 0;
	for (i = 0; i < numOptions; i++) 
	{
		if ((key = options[i].key) == KEY_DUMMY) 
				continue;
		GetResource(options[i].name, resValue, sizeof resValue);
		// xpprintf("ParseOptions: %d name \"%s\" value \"%s\"\n", i, options[i].name, resValue);
		firstKeyDef = num;
		for (str = strtok(resValue, " \t\r\n");
			 str != NULL;
			 str = strtok(NULL, " \t\r\n"))
		{

			if ((ks = XStringToKeysym(str)) == NoSymbol)
			{
				printf("Invalid keysym \"%s\" for key \"%s\".\n", str, options[i].name);
				continue;
			}
			// xpprintf("ParseOptions: String \"%s\" becomes %d\n", str, ks);

			for (j = firstKeyDef; j < num; j++)
			{
				if (keyDefs[j].keySym == ks
				 && keyDefs[j].key == key)
				{
					break;
				}
			}
			if (j < num)
				continue;
			if (num >= maxKeyDefs)
			{
				maxKeyDefs += NUM_KEYS;
				if (!(keyDefs = (KeyDef*)realloc(keyDefs, maxKeyDefs * sizeof(KeyDef))))
				{
					error("No memory for key bindings");
					exit(1);
				}
			}

			/* insertion sort. */
			for (j = num; j > 0; j--) 
			{
				if (ks >= keyDefs[j - 1].keySym)
					break;
				keyDefs[j] = keyDefs[j - 1];
			}
			keyDefs[j].keySym = ks;
			keyDefs[j].key = key;
			num++;
			if (!key) 
			{
				printf("bug key 0\n");
				exit(1);
			}
		}
	}
	if (num < maxKeyDefs)
	{
		maxKeyDefs = num;
		if (!(keyDefs = (KeyDef*)realloc(keyDefs, maxKeyDefs * sizeof(KeyDef))))
		{
			error("No memory for key bindings");
			exit(1);
		}
	}

	/*
	 * Pointer button bindings
	 */
	for (i = 0; i < MAX_POINTER_BUTTONS; i++) {
		sprintf(resValue, "pointerButton%d", i+1);
		GetResource(resValue, resValue, sizeof resValue);
		ptr = resValue;
		if (*ptr != '\0') {
			for (ptr = strtok(resValue, " \t\r\n");
				 ptr != NULL;
				 ptr = strtok(NULL, " \t\r\n")) {
				if (!strncasecmp(ptr, "key", 3))
					ptr += 3;
				for (j = 0; j < numOptions; j++) {
					if (options[j].key != KEY_DUMMY) {
						if (!strcasecmp(ptr, options[j].name + 3)) {
							if (NUM_BUTTON_DEFS(i) == MAX_BUTTON_DEFS) {
								seterrno(0);
								error("Can only have %d keys bound to"
									  " pointer button %d",
									  MAX_BUTTON_DEFS, i);
								break;
							}
							buttonDefs[i][((int)(NUM_BUTTON_DEFS(i)))]
								= options[j].key;
							buttonDefs[i][MAX_BUTTON_DEFS] =
								(keys_t)(buttonDefs[i][MAX_BUTTON_DEFS] + 1);

							break;
						}
					}
				}
				if (j == numOptions) {
					seterrno(0);
					error("Unknown key \"%s\" for pointer button %d", ptr, i);
				}
			}
		}
	}

#ifndef _WINDOWS
	XrmDestroyDatabase(rDB);

	free(xopt);
#endif

		D(xpprintf("%s End parse\n", showtime());)
}

///////////////////////////////////////////////////////////////////////////////
void IniClient::ParseSwitches(int *argcp, char **argvp)
{
	int 			i;
	for (i = 1; i < *argcp; i++) 
	{
		if (argvp[i][0] == '-' || argvp[i][0] == '+') 
		{
			if (!strcmp(&argvp[i][1], "team"))
			{
				team = atoi(argvp[i+1]);
				SkipSwitches(argcp, argvp, i, 2);
			}
			if (!strcmp(&argvp[i][1], "port"))
			{
				serverport = atoi(argvp[i+1]);
				SkipSwitches(argcp, argvp, i, 2);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
void IniClient::Save(FILE* fp)
{
	SaveResource(fp, "shipShapeFile", shipShapeFile);
	SaveResource(fp, "name", name);
	SaveResource(fp, "shipShapeName", shipShapeName);
	SaveResource(fp, "user", realname);
	SaveResource(fp, "host", hostname);
	SaveFloat(fp, "power", power);
	SaveFloat(fp, "turnSpeed", turnspeed);
	SaveFloat(fp, "turnResistance", turnresistance);
	SaveFloat(fp, "altPower", power_s);
	SaveFloat(fp, "altTurnSpeed", turnspeed_s);
	SaveFloat(fp, "altTurnResistance", turnresistance_s);
	SaveFloat(fp, "speedFactHUD", hudMoveFact);
	SaveFloat(fp, "speedFactPTR", ptrMoveFact);
	SaveFloat(fp, "fuelNotify", (DFLOAT)fuelLevel3);
	SaveFloat(fp, "fuelWarning", (DFLOAT)fuelLevel2);
	SaveFloat(fp, "fuelCritical", (DFLOAT)fuelLevel1);
	SaveBool(fp, "showShipName", BIT(instruments, SHOW_SHIP_NAME));
	SaveBool(fp, "showMineName", BIT(instruments, SHOW_MINE_NAME));
	SaveBool(fp, "showMessages", BIT(instruments, SHOW_MESSAGES));
	SaveInt(fp, "maxMessages", maxMessages);
	SaveInt(fp, "messagesToStdOut", messagesToStdout);
	SaveInt(fp, "oldMessagesColor", oldMessagesColor);
	SaveBool(fp, "reverseScroll", BIT(instruments, SHOW_REVERSE_SCROLL));
	SaveBool(fp, "showHUD", BIT(instruments, SHOW_HUD_INSTRUMENTS));
	SaveBool(fp, "showHUDRadar", BIT(instruments, SHOW_HUD_RADAR));
	SaveBool(fp, "verticalHUDLine", BIT(instruments, SHOW_HUD_VERTICAL));
	SaveBool(fp, "horizontalHUDLine", BIT(instruments, SHOW_HUD_HORIZONTAL));
	SaveBool(fp, "fuelMeter", BIT(instruments, SHOW_FUEL_METER));
	SaveBool(fp, "fuelGauge", BIT(instruments, SHOW_FUEL_GAUGE));
	SaveBool(fp, "turnSpeedMeter", BIT(instruments, SHOW_TURNSPEED_METER));
	SaveBool(fp, "powerMeter", BIT(instruments, SHOW_POWER_METER));
	SaveBool(fp, "packetSizeMeter", BIT(instruments, SHOW_PACKET_SIZE_METER));
	SaveBool(fp, "packetLossMeter", BIT(instruments, SHOW_PACKET_LOSS_METER));
	SaveBool(fp, "packetDropMeter", BIT(instruments, SHOW_PACKET_DROP_METER));
	SaveBool(fp, "packetLagMeter", BIT(instruments, SHOW_PACKET_LAG_METER));
	SaveBool(fp, "slidingRadar", BIT(instruments, SHOW_SLIDING_RADAR));
	SaveBool(fp, "showItems", BIT(instruments, SHOW_ITEMS));
	SaveFloat(fp, "showItemsTime", showItemsTime);
	SaveInt(fp,   "showScoreDecimals", showScoreDecimals);
	SaveBool(fp, "outlineWorld", BIT(instruments, SHOW_OUTLINE_WORLD));
	SaveBool(fp, "filledWorld", BIT(instruments, SHOW_FILLED_WORLD));
	SaveBool(fp, "texturedWalls", BIT(instruments, SHOW_TEXTURED_WALLS));
	SaveBool(fp, "texturedObjects", blockBitmaps);
	SaveBool(fp, "clock", BIT(instruments, SHOW_CLOCK));
	SaveBool(fp, "clockAMPM", BIT(instruments, SHOW_CLOCK_AMPM_FORMAT));
	SaveInt(fp, "backgroundPointDist", mapPointDistance);
	SaveInt(fp, "backgroundPointSize", mapPointSize);
	SaveInt(fp, "sparkSize", sparkSize);
	SaveFloat(fp, "sparkProb", sparkProb);
	SaveInt(fp, "shotSize", shotSize);
	SaveInt(fp, "teamShotSize", teamShotSize);
	SaveBool(fp, "showNastyShots", showNastyShots);
	SaveInt(fp, "hudColor", hudColor);
	SaveInt(fp, "hudLockColor", hudLockColor);
	SaveInt(fp, "wallColor", wallColor);
	SaveInt(fp, "decorColor", decorColor);
	SaveBool(fp, "showDecor", BIT(instruments, SHOW_DECOR));
	SaveBool(fp, "outlineDecor", BIT(instruments, SHOW_OUTLINE_DECOR));
	SaveBool(fp, "filledDecor", BIT(instruments, SHOW_FILLED_DECOR));
	SaveBool(fp, "texturedDecor", BIT(instruments, SHOW_TEXTURED_DECOR));
	SaveBool(fp, "texturedBalls", BIT(instruments, SHOW_TEXTURED_BALLS));
	SaveInt(fp, "receiveWindowSize", receiveWindowSize);
	SaveInt(fp, "charsPerSecond", charsPerSecond);
	SaveBool(fp, "markingLights", markingLights);
	SaveBool(fp, "toggleShield", toggleShield);
	SaveBool(fp, "autoShield", autoShield);
	SaveInt(fp, "clientPortStart", clientPortStart);
	SaveInt(fp, "clientPortEnd", clientPortEnd);
	SaveInt(fp, "maxVolume", maxVolume);
#if defined(_WINDOWS) && !defined(_CYGWIN)
	SaveBool(fp, "threadedDraw", threadedDraw);
#endif
	SaveFloat(fp, "scaleFactor", scaleFactor);
	SaveFloat(fp, "altScaleFactor", scaleFactor_s);
	/* don't save maxFPS */

	SaveKeys(fp);

	int 	i;
	String		buf;
	for (i = 0; i < NUM_MODBANKS; i++) {
		buf.printf("modifierBank%d", i + 1);
		SaveResource(fp, buf, modBankStr[i]);
	}

#if defined (_UNIX)
	SaveResource(fp, "visual", visualName);
#endif
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Find a key in keyDefs[].
 * On success set output pointer to index into keyDefs[] and return TRUE.
 * On failure return FALSE.
 */
int IniClient::FindKey(keys_t key, int start, int end, int *key_index)
{
	int 				i;

	for (i = start; i < end; i++) {
		if (keyDefs[i].key == key) {
			*key_index = i;
			return TRUE;
		}
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
void IniClient::SaveKeys(FILE* fp)
{
	int 				i, j;
	KeySym				ks;
	keys_t				key;
	const char			*str,
						*res;
	char				buf[512];

	buf[0] = '\0';
	for (i = 0; i < maxKeyDefs; i++) {
		ks = keyDefs[i].keySym;
		key = keyDefs[i].key;

		/* try and see if we have already saved this key. */
		if (FindKey(key, 0, i, &j) == TRUE) {
			/* yes, saved this one before.	skip it now. */
			continue;
		}

		if ((str = XKeysymToString(ks)) == NULL) {
			continue;
		}

		if ((res = GetKeyResourceString(key)) != NULL) {
			strlcpy(buf, str, sizeof(buf));
			/* find all other keysyms which map to the same key. */
			j = i;
			while (FindKey(key, j + 1, maxKeyDefs, &j) == TRUE) {
				ks = keyDefs[j].keySym;
				if ((str = XKeysymToString(ks)) != NULL) {
					strcat(buf, " ");
					strcat(buf, str);
				}
			}
			SaveResource(fp, res, buf);
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
char* GetKeyHelpString(keys_t key)
{
	int 				i;
	char				*nl;
	static char 		buf[MAX_CHARS];

	for (i = 0; i < iniClient.numOptions; i++) {
		if (iniClient.options[i].key == key) {
			strlcpy(buf, iniClient.options[i].help, sizeof buf);
			if ((nl = strchr(buf, '\n')) != NULL) {
				*nl = '\0';
			}
			return buf;
		}
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
const char* GetKeyResourceString(keys_t key)
{
	int 				i;

	for (i = 0; i < iniClient.numOptions; i++) {
		if (iniClient.options[i].key == key) {
			return iniClient.options[i].name;
		}
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
#if 0
static void Usage(void)
{
	int 				i;

	printf(
"Usage: xpilot [-options ...] [server]\n"
"Where options include:\n"
"\n"
		  );
	for (i = 0; i < numOptions; i++) {
		printf("    -%s %s\n", options[i].name,
			   (options[i].noArg == NULL) ? "<value>" : "");
		if (options[i].help && options[i].help[0]) {
			const char *str;
			printf("        ");
			for (str = options[i].help; *str; str++) {
				putchar(*str);
				if (*str == '\n' && str[1]) {
					printf("        ");
				}
			}
			if (str[-1] != '\n') {
				putchar('\n');
			}
		}
		if (options[i].fallback && options[i].fallback[0]) {
			printf("        The default %s: %s.\n",
				   (options[i].key == KEY_DUMMY)
					   ? "value is"
					   : (strchr(options[i].fallback, ' ') == NULL)
						   ? "key is"
						   : "keys are",
				   options[i].fallback);
		}
		printf("\n");
	}
	printf(
"Most of these options can also be set in the .xpilotrc file\n"
"in your home directory.\n"
"Each key option may have multiple keys bound to it and\n"
"one key may be used by multiple key options.\n"
"If no server is specified then xpilot will search\n"
"for servers on your local network.\n"
"For a listing of remote servers try: telnet meta.xpilot.org 4400 \n"
		  );

	exit(1);
}
#endif

///////////////////////////////////////////////////////////////////////////////
#if 0
int Ini::FindResource(PCSTR resource, char *result, unsigned size, int *index)
{
#ifndef _WINDOWS
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
	sprintf(str_name, "%s.%s", myName, resource);
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
	GetResource(resource, result, size, index);

	return 1;
#endif
}


///////////////////////////////////////////////////////////////////////////////
int Ini::GetResource(PCSTR resource, char *result, unsigned size)
{
	int 				index;

	return(FindResource(resource, result, size, &index));
}


///////////////////////////////////////////////////////////////////////////////
int Ini::GetStringResource(PCSTR resource, char *result, unsigned size)
{
	char				*src, *dst;
	int 				index, val;

	val = FindResource(resource, result, size, &index);
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


///////////////////////////////////////////////////////////////////////////////
void Ini::GetIntResource(PCSTR resource, int *result)
{
	int 				index;
	char				resValue[MAX_CHARS];

	FindResource(resource, resValue, sizeof resValue, &index);
	if (sscanf(resValue, "%d", result) <= 0) {
		error("Bad value \"%s\" for option \"%s\", using default...",
			  resValue, resource);
		sscanf(options[index].fallback, "%d", result);
	}
}


///////////////////////////////////////////////////////////////////////////////
void Ini::GetFloatResource(PCSTR resource, DFLOAT *result)
{
	int 				index;
	double				temp_result;
	char				resValue[MAX_CHARS];

	temp_result = 0.0;
	FindResource(resource, resValue, sizeof resValue, &index);
	if (sscanf(resValue, "%lf", &temp_result) <= 0) {
		error("Bad value \"%s\" for option \"%s\", using default...",
			  resValue, resource);
		sscanf(options[index].fallback, "%lf", &temp_result);
	}
	*result = (DFLOAT) temp_result;
}


///////////////////////////////////////////////////////////////////////////////
void Ini::GetBoolResource(PCSTR resource, bool *result)
{
	int 				index;
	char				resValue[MAX_CHARS];

	FindResource(resource, resValue, sizeof resValue, &index);
	*result = (ON(resValue) != 0);
}


///////////////////////////////////////////////////////////////////////////////
void Ini::GetBitResource(PCSTR resource, long *mask, int bit)
{
	int 				index;
	char				resValue[MAX_CHARS];

	FindResource(resource, resValue, sizeof resValue, &index);
	if (ON(resValue)) {
		SET_BIT(*mask, bit);
	}
}
#endif

///////////////////////////////////////////////////////////////////////////////
void IniClient::GetShipshapeResource(String* ship_shape)
{
	char				resValue[MAX(2*MSG_LEN, PATH_MAX + 1)];

	//First get the ship name
	GetResource("shipShapeName", shipShapeName, MAX_NAME_LEN);
	*ship_shape = shipShapeName;
	//If the name wasn't available in shipShapeName, maybe we're reading from
	//an old style file, where the ship name was stored in shipShape
	if (strlen(shipShapeName) == 0)
	{
		GetResource("shipShape", resValue, sizeof resValue);
		*ship_shape = xp_strdup(resValue);
		*ship_shape = resValue;
		if (ship_shape->IsEmpty())
			*ship_shape = defaultShipString;
	}
	if (!ship_shape->IsEmpty() && !strchr(*ship_shape, '(' )) {
		/* so it must be the name of shipshape defined in the shipshapefile. */
		//in which case, lets store the name for future reference.
		if (strlen(shipShapeName) == 0)
		{
			sprintf(shipShapeName, "%s", (PCSTR)*ship_shape);
		}
		GetResource("shipShapeFile", resValue, sizeof resValue);
		if (resValue[0] != '\0') {
			FILE *fp = fopen(resValue, "r");
			if (!fp) {
				perror(resValue);
			} else {
				char *ptr;
				char *str;
				char line[1024];
				while (fgets(line, sizeof line, fp)) {
					if ((str = strstr(line, "(name:" )) != NULL
						|| (str = strstr(line, "(NM:" )) != NULL) {
						str = strchr(str, ':');
						while (*++str == ' ');
						if ((ptr = strchr(str, ')' )) != NULL) {
							*ptr = '\0';
						}
						if (!strcmp(str, *ship_shape)) {
							/* Gotcha */
							if (ptr != NULL) {
								*ptr = ')';
							}
							//*ship_shape = xp_strdup(line);
							*ship_shape = line;
							break;
						}
					}
				}
				fclose(fp);
			}
		}
	}
	//We were given a shipShape string thats a ship definition. shipShape: (NM: x-wing)(AU: di...etc.
	//right now we leave shipShapeFile and shipShapeName as they are and do nothing else.
	//Eventually we should determine if shipShape should be settable direcly like this, as it breaks
	//the ClientConfig.

}

///////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef _WINDOWS
void Ini::GetXpilotRcFile(char *path, unsigned size)
{
	const char			*home = getenv("HOME");
	const char			*defaultFile = ".xpilotrc";
	const char			*optionalFile = getenv("XPILOTRC");

	if (optionalFile != NULL) {
		strlcpy(path, optionalFile, size);
	}
	else if (home != NULL) {
		strlcpy(path, home, size);
		strlcat(path, "/", size);
		strlcat(path, defaultFile, size);
	}
	else {
		strlcpy(path, "", size);
	}
}
#endif


///////////////////////////////////////////////////////////////////////////////
#ifndef _WINDOWS
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

	if ((ptr = XResourceManagerString(dpy)) != NULL) {
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

	GetXpilotRcFile(path, sizeof(path));
	if (path[0] != '\0') {
		tmpDB = XrmGetFileDatabase(path);
		XrmMergeDatabases(tmpDB, rDBptr);
	}
}
#endif	/* _WINDOWS*/
#endif	// #if 0



///////////////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////////////
static void X_after(Display *display)
{
	static int			n;

	if (n < 1000) {
		printf("_X_ %4d\n", n++);
	}
}

///////////////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////////////
void IniClient::InsertKeyDef(KeySym ks, keys_t key)
{
	int 	j;
	if (!(keyDefs = (KeyDef*)realloc(keyDefs, (maxKeyDefs+1) * sizeof(KeyDef))))
	{
		error("No memory for key bindings");
		exit(1);
	}
	/* insertion sort. */
	for (j = maxKeyDefs; j > 0; j--) 
	{
		if (ks >= keyDefs[j - 1].keySym)
			break;
		keyDefs[j] = keyDefs[j - 1];
	}
	keyDefs[j].keySym = ks;
	keyDefs[j].key = key;
	maxKeyDefs++;
}

///////////////////////////////////////////////////////////////////////////////
void IniClient::DeleteKeyDef(KeySym ks, keys_t key)
{
	int 	j;
	for (j=0; j<maxKeyDefs; j++)
	{
		if (ks == keyDefs[j].keySym && key == keyDefs[j].key)
			break;
	}
	for (; j<maxKeyDefs-1; j++)
		keyDefs[j] = keyDefs[j+1];
	maxKeyDefs--;
}
