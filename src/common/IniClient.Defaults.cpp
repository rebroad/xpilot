/* $Id: IniClient.Defaults.cpp,v 1.10 2007/01/17 08:54:29 dick Exp $
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
 * $Log: IniClient.Defaults.cpp,v $
 * Revision 1.10  2007/01/17 08:54:29  dick
 * KEY_FLIP_ROBOT_PAGE cycles through the robot's brain pages
 *
 * Revision 1.9  2004/07/22 17:00:05  dick
 * PCSTR defaultShipString is the chars for the default XPilot ship.
 *
 * Revision 1.8  2004/05/23 23:52:07  dick
 * src/common/IniClient.Defaults.cpp
 *
 * Revision 1.7  2004/05/13 17:38:08  dick
 * Don't do #ifdef SOUND in common.  Always compile it.
 *
 * Revision 1.6  2004/05/11 06:33:13  dick
 * audioServer is gone
 *
 * Revision 1.5  2004/05/01 19:11:52  dick
 * Retab
 *
 * Revision 1.4  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.3  2004/01/20 15:14:27  dick
 * Get rid of #define WINDOWSCALING.  Window scaling is always built, it's not an option.
 *
 * Revision 1.2  2002/09/03 07:41:35  dick
 * Move visual to the top of the list (debugging aid)
 *
 * Revision 1.1  2002/09/02 15:45:49  dick
 * Break Ini up into base class Ini and subclass IniClient.
 *
 */
#include "StdAfx.h"

#include "pack.h"
#include "IniClient.h"

/*
 * Default fonts
 */
#define GAME_FONT	"-*-times-*-*-*--18-*-*-*-*-*-iso8859-1"
#define MESSAGE_FONT	"-*-times-*-*-*--14-*-*-*-*-*-iso8859-1"
#define SCORE_LIST_FONT	"-*-fixed-bold-*-*--13-*-*-*-c-*-iso8859-1"
#define BUTTON_FONT	"-*-*-bold-o-*--14-*-*-*-*-*-iso8859-1"
#define TEXT_FONT	"-*-*-bold-i-*--14-*-*-*-p-*-iso8859-1"
#define TALK_FONT	"-*-fixed-bold-*-*--15-*-*-*-c-*-iso8859-1"
#define KEY_LIST_FONT	"-*-fixed-medium-r-*--10-*-*-*-c-*-iso8859-1"
#define MOTD_FONT	"-*-courier-bold-r-*--14-*-*-*-*-*-iso8859-1"

PCSTR	defaultShipString = "(NM:Default)(AU:Unknown)(SH: 15,0 -9,8 -9,-8)"
							"(MG: 15,0)(LG: 15,0)(RG: 15,0)(EN: -9,0)"
							"(LR: -9,8)(RR: -9,-8)(LL: -9,8)(RL: -9,-8)(MR: 15,0)";
/* from common/config.c */
extern char conf_ship_file_string[];
extern char conf_texturedir_string[];
extern char conf_soundfile_string[];


/*
 * Structure to store all the client options.
 * The most important field is the help field.
 * It is used to self-document the client to
 * the user when "xpilot -help" is issued.
 * Help lines can span multiple lines, but for
 * the key help window only the first line is used.
 */
Option clientOptions[] = {
	{
		"help",
		"Yes",
		"",
		KEY_DUMMY,
		"Display this help message.\n"
	},
	{
		"version",
		"Yes",
		"",
		KEY_DUMMY,
		"Show the source code version.\n"
	},
	{
		"visual",
		NULL,
		"",
		KEY_DUMMY,
		"Specify which visual to use for allocating colors.\n"
		"To get a listing of all possible visuals on your dislay\n"
		"set the argument for this option to list.\n"
	},
	{
		"name",
		NULL,
		"",
		KEY_DUMMY,
		"Set the nickname.\n"
	},
	{
		"user",
		NULL,
		"",
		KEY_DUMMY,
		"Set the realname.\n"
	},
	{
		"host",
		NULL,
		"",
		KEY_DUMMY,
		"Set the hostname.\n"
	},
	{
		"join",
		"Yes",
		"",
		KEY_DUMMY,
		"Join the game immediately, no questions asked.\n"
	},
	{
		"noLocalMotd",
		"Yes",
		"",
		KEY_DUMMY,
		"Do not display the local Message Of The Day.\n"
	},
	{
		"autoServerMotdPopup",
		NULL,
#ifdef _WINDOWS
		"No",					/* temporary till i straighten out the motd woes. */
#else
		"Yes",
#endif
		KEY_DUMMY,
		"Automatically popup the MOTD of the server on startup.\n"
	},
	{
		"refreshMotd",
		NULL,
		"No",
		KEY_DUMMY,
		"Get a fresh copy of the server MOTD every time it is displayed.\n"
	},
	{
		"text",
		"Yes",
		"No",
		KEY_DUMMY,
		"Use the simple text interface to contact a server\n"
		"instead of the graphical user interface.\n"
	},
	{
		"list",
		"Yes",
		"",
		KEY_DUMMY,
		"List all servers running on the local network.\n"
	},
	{
		"team",
		NULL,
		TEAM_NOT_SET_STR,
		KEY_DUMMY,
		"Set the team to join.\n"
	},
	{
		"display",
		NULL,
		"",
		KEY_DUMMY,
		"Set the X display.\n"
	},
	{
		"keyboard",
		NULL,
		"",
		KEY_DUMMY,
		"Set the X keyboard input if you want keyboard input from\n"
		"another display.  The default is to use the keyboard input from\n"
		"the X display.\n"
	},
	{
		"geometry",
		NULL,
		"",
		KEY_DUMMY,
		"Set the window size and position in standard X geometry format.\n"
		"The maximum allowed window size is 1282x1024.\n"
	},
	{
		"ignoreWindowManager",
		NULL,
		"",
		KEY_DUMMY,
		"Ignore the window manager when opening the top level player window.\n"
		"This can be handy if you want to have your XPilot window on a preferred\n"
		"position without window manager borders.  Also sometimes window managers\n"
		"may interfere when switching colormaps.  This option may prevent that.\n"
	},
	{
		"shutdown",
		NULL,
		"",
		KEY_DUMMY,
		"Shutdown the server with a message.\n"
		"The message used is the first argument to this option.\n"
	},
	{
		"port",
		NULL,
		SERVER_PORT_STR,
		KEY_DUMMY,
		"Set the port number of the server.\n"
		"Almost all servers use the default port, which is the\n"
		"recommended policy.  You can find out about which port\n"
		"is used by a server by querying the XPilot Meta server.\n"
	},
	{
		"shipShape",
		NULL,
		"",
		KEY_DUMMY,
		"Define the ship shape to use.  Because the argument to this option\n"
		"is rather large (up to 500 bytes) the recommended way to set\n"
		"this option is in the .xpilotrc file in your home directory.\n"
		"The exact format is defined in the file doc/README.SHIPS in the XPilot\n"
		"distribution.  Note that there is a nifty Unix tool called editss for\n"
		"easy ship creation.  There is XPShipEditor for Windows\n"
		"and Ship Shaper for Java.  See the XPilot FAQ for details.\n"
		"See also the \"shipShapeFile\" option below.\n"
	},
	{
		"shipShapeName",
		NULL,
		"",
		KEY_DUMMY,
		"Select the ship shape to use, by name.\n"
	},
	{
		"shipShapeFile",
		NULL,
		conf_ship_file_string,
		KEY_DUMMY,
		"An optional file where shipshapes can be stored.\n"
		"If this resource is defined and it refers to an existing file\n"
		"then shipshapes can be referenced to by their name.\n"
		"For instance if you define shipShapeFile to be\n"
		"/home/myself/.shipshapes and this file contains one or more\n"
		"shipshapes then you can select the shipshape by starting xpilot as:\n"
		"	xpilot -shipShapeName myshipshapename\n"
		"Where \"myshipshapename\" should be the \"name:\" or \"NM:\" of\n"
		"one of the shipshapes defined in /home/myself/.shipshapes.\n"
		"Each shipshape definition should be defined on only one line,\n"
		"where all characters up to the first left parenthesis don't matter.\n"
		/* shipshopshapshepshit getting nuts from all these shpshp-s. */
	},
	{
		"power",
		NULL,
		"45.0",
		KEY_DUMMY,
		"Set the engine power.\n"
		"Valid values are in the range 5-55.\n"
	},
	{
		"turnSpeed",
		NULL,
		"35.0",
		KEY_DUMMY,
		"Set the ship's turn speed.\n"
		"Valid values are in the range 4-64.\n"
		"See also turnResistance.\n"
	},
	{
		"turnResistance",
		NULL,
		"0.12",
		KEY_DUMMY,
		"Set the ship's turn resistance.\n"
		"This determines the speed at which a ship stops turning\n"
		"after a turn key has been released.\n"
		"Valid values are in the range 0.0-1.0.\n"
		"Useful values are in the range 0.05-0.15.\n"
		"See also turnSpeed.\n"
	},
	{
		"altPower",
		NULL,
		"35.0",
		KEY_DUMMY,
		"Set the alternate engine power.\n"
		"See also the keySwapSettings option.\n"
	},
	{
		"altTurnSpeed",
		NULL,
		"25.0",
		KEY_DUMMY,
		"Set the alternate ship's turn speed.\n"
		"See also the keySwapSettings option.\n"
	},
	{
		"altTurnResistance",
		NULL,
		"0.12",
		KEY_DUMMY,
		"Set the alternate ship's turn resistance.\n"
		"See also the keySwapSettings option.\n"
	},
	{
		"showHUD",
		NULL,
		"Yes",
		KEY_DUMMY,
		"Should the HUD be displayed or not.\n"
	},
		{
				"showHUDRadar",
				NULL,
				"No",
				KEY_DUMMY,
				"Should the HUD radar be displayed or not.\n"
		},
	{
		"fuelNotify",
		NULL,
		"500",
		KEY_DUMMY,
		"The limit when the HUD fuel bar will become visible.\n"
	},
	{
		"fuelWarning",
		NULL,
		"200",
		KEY_DUMMY,
		"The limit when the HUD fuel bar will start flashing.\n"
	},
	{
		"fuelCritical",
		NULL,
		"100",
		KEY_DUMMY,
		"The limit when the HUD fuel bar will flash faster.\n"
	},
	{
		"verticalHUDLine",
		NULL,
		"No",
		KEY_DUMMY,
		"Should the vertical lines in the HUD be drawn.\n"
	},
	{
		"horizontalHUDLine",
		NULL,
		"Yes",
		KEY_DUMMY,
		"Should the horizontal lines in the HUD be drawn.\n"
	},
	{
		"speedFactHUD",
		NULL,
		"0.0",
		KEY_DUMMY,
		"Should the HUD be moved, to indicate the current velocity?\n"
	},
	{
		"speedFactPTR",
		NULL,
		"0.0",
		KEY_DUMMY,
		"Uses a red line to indicate the current velocity and direction.\n"
	},
	{
		"fuelMeter",
		NULL,
		"No",
		KEY_DUMMY,
		"Determines if the fuel meter should be visible.\n"
	},
	{
		"fuelGauge",
		NULL,
		"Yes",
		KEY_DUMMY,
		"Determines if the fuel gauge should be visible.\n"
	},
	{
		"turnSpeedMeter",
		NULL,
		"No",
		KEY_DUMMY,
		"Should the turn speed meter be visible at all times.\n"
	},
	{
		"powerMeter",
		NULL,
		"No",
		KEY_DUMMY,
		"Should the power meter be visible at all times.\n"
	},
	{
		"packetSizeMeter",
		NULL,
		"No",
		KEY_DUMMY,
		"Should the network packet size meter be visible at all times.\n"
		"Each bar is equavalent to 1024 bytes, for a maximum of 4096 bytes.\n"
	},
	{
		"packetLossMeter",
		NULL,
		"No",
		KEY_DUMMY,
		"Should the packet loss meter be visible.\n"
		"This gives the percentage of lossed frames due to network failure.\n"
	},
	{
		"packetDropMeter",
		NULL,
		"No",
		KEY_DUMMY,
		"Should the packet drop meter be visible.\n"
		"This gives the percentage of dropped frames due to display slowness.\n"
	},
		{
				"packetLagMeter",
				NULL,
				"No",
				KEY_DUMMY,
				"Should the packet lag meter be visible.\n"
				"This gives the amount of lag in frames over the past one second.\n"
		},
	{
		"slidingRadar",
		NULL,
		"No",
		KEY_DUMMY,
		"If the game is in edgewrap mode then the radar will keep your\n"
		"position on the radar in the center and raw the rest of the radar\n"
		"around it.  Note that this requires a fast graphics system.\n"
	},
	{
		"outlineWorld",
		NULL,
		"No",
		KEY_DUMMY,
		"Draws only the outline of all the blue map constructs.\n"
	},
	{
		"filledWorld",
		NULL,
		"No",
		KEY_DUMMY,
		"Draws the walls solid, filled with one color.\n"
		"Be warned that this option needs fast graphics.\n"
	},
	{
		"texturedWalls",
		NULL,
		"No",
		KEY_DUMMY,
		"Draws the walls filled with a texture pattern.\n"
		"See also the wallTextureFile option.\n"
		"Be warned that this needs a very fast graphics system.\n"
	},
	{
		"wallTextureFile",
		NULL,
		"",
		KEY_DUMMY,
		"Specify a XPM format pixmap file to load the wall texture from.\n"
	},
	{
		"texturePath",
		NULL,
		conf_texturedir_string,
		KEY_DUMMY,
		"Search path for texture files.\n"
		"This is a list of one or more directories separated by colons.\n"
	},
	{
		"texturedObjects",
		NULL,
		"No",
		KEY_DUMMY,
		"Whether to draw ships, shots and walls with textures.\n"
		"Be warned that this needs a very fast graphics system.\n"
		"You may also need to enable multibuffering or double-buffering.\n"
	},
	{
		"markingLights",
		NULL,
		"Yes",
		KEY_DUMMY,
		"Should the fighters have marking lights, just like airplanes?\n"
	},
	{
		"sparkProb",
		NULL,
		"0.50",
		KEY_DUMMY,
		"The chance that sparks are drawn or not.\n"
		"This gives a sparkling effect.\n"
		"Valid values are in the range [0.0-1.0]\n"
	},
	{
		"sparkSize",
		NULL,
		"2",
		KEY_DUMMY,
		"Size of sparks in pixels.\n"
	},
	{
		"charsPerSecond",
		NULL,
		"50",
		KEY_DUMMY,
		"Speed in which messages appear on screen in characters per second.\n"
	},
	{
		"clock",
		NULL,
		"No",
		KEY_DUMMY,
		"Should a clock be displayed in the top right of the score window.\n"
	},
	{
		"clockAMPM",
		NULL,
		"No",
		KEY_DUMMY,
		"12 or 24 hour format for clock display.\n",
	},
	{
		"pointerControl",
		NULL,
		"No",
		KEY_DUMMY,
		"Enable mouse control.  This allows ship direction control by\n"
		"moving the mouse to the left for an anti-clockwise turn and\n"
		"moving the mouse to the right for a clockwise turn.\n"
		"Also see the pointerButton options for use of the mouse buttons.\n"
	},
	{
		"gameFont",
		NULL,
		GAME_FONT,
		KEY_DUMMY,
		"The font used on the HUD and for most other text.\n"
	},
	{
		"scoreListFont",
		NULL,
		SCORE_LIST_FONT,
		KEY_DUMMY,
		"The font used on the score list.\n"
		"This must be a non-proportional font.\n"
	},
	{
		"buttonFont",
		NULL,
		BUTTON_FONT,
		KEY_DUMMY,
		"The font used on all buttons.\n"
	},
	{
		"textFont",
		NULL,
		TEXT_FONT,
		KEY_DUMMY,
		"The font used in the help and about windows.\n"
	},
	{
		"talkFont",
		NULL,
		TALK_FONT,
		KEY_DUMMY,
		"The font used in the talk window.\n"
	},
	{
		"motdFont",
		NULL,
		MOTD_FONT,
		KEY_DUMMY,
		"The font used in the MOTD window and key list window.\n"
		"This must be a non-proportional font.\n"
	},
	{
		"messageFont",
		NULL,
		MESSAGE_FONT,
		KEY_DUMMY,
		"The font used for drawing messages.\n"
	},
	{
		"maxMessages",
		NULL,
		"8",
		KEY_DUMMY,
		"The maximum number of messages to display.\n"
	},
		{
				"messagesToStdout",
				NULL,
				"0",
				KEY_DUMMY,
				"Send messages to standard output.\n0: Don't.\n1: Only player "
				"messages.\n2: Player and status messages.\n"
		},
	{
		"reverseScroll",
		NULL,
		"No",
		KEY_DUMMY,
		"Reverse scroll direction of messages.\n"
	},
#ifndef _WINDOWS
	{
		"selectionAndHistory",
		NULL,
		"Yes",
		KEY_DUMMY,
		"Provide cut&paste for the player messages and the talk window and\n"
		"a `history' for the talk window.\n"
	},
	{
		"maxLinesInHistory",
		NULL,
		"32",
		KEY_DUMMY,
		"Number of your messages saved in the `history' of the talk window.\n"
		"`history' is accessible with `keyTalkCursorUp/Down'.\n"
	},
#endif
	{
		"shotSize",
		NULL,
		"3",
		KEY_DUMMY,
		"The size of shots in pixels.\n"
	},
	{
		"teamShotSize",
		NULL,
		"2",
		KEY_DUMMY,
		"The size of team shots in pixels.\n"
		"Note that team shots are drawn in blue.\n"
	},
	{
		"showNastyShots",
		NULL,
		"No",
		KEY_DUMMY,
		"Use the new Nasty Looking Shots or the original rectangle shots,\n"
		"You will probably want to increase your shotSize if you use this.\n"
	},
	{
		"backgroundPointDist",
		NULL,
		"8",
		KEY_DUMMY,
		"The distance between points in the background measured in blocks.\n"
		"These are drawn in empty map regions to keep feeling for which\n"
		"direction the ship is moving to.\n"
	},
	{
		"backgroundPointSize",
		NULL,
		"2",
		KEY_DUMMY,
		"Specifies the size of the background points.  0 means no points.\n"
	},
	{
		"titleFlip",
		NULL,
		"Yes",
		KEY_DUMMY,
		"Should the title bar change or not.\n"
		"Some window managers like twm may have problems with\n"
		"flipping title bars.  Hence this option to turn it off.\n"
	},
	{
		"toggleShield",
		NULL,
		"No",
		KEY_DUMMY,
		"Are shields toggled by a keypress only?\n"
	},
	{
		"autoShield", /* Don auto-shield hack */
		NULL,
		"Yes",
		KEY_DUMMY,
		"Are shields lowered automatically for weapon fire?\n"
	},
	{
		"shieldDrawSolid",
		NULL,
		"Default",
		KEY_DUMMY,
		"Are shields drawn in a solid line.\n"
		"Not setting a value for this option will select the best value\n"
		"automatically for your particular display system.\n"
	},
	{
		"showShipName",
		NULL,
		"Yes",
		KEY_DUMMY,
		"Should all ships have the name of the player drawn below them.\n"
	},
	{
		"showMineName",
		NULL,
		"Yes",
		KEY_DUMMY,
		"Should the name of the owner of the mine be drawn below the mine.\n"
	},
	{
		"showMessages",
		NULL,
		"Yes",
		KEY_DUMMY,
		"Should messages appear on screen.\n"
	},
	{
		"showItems",
		NULL,
		"Yes",
		KEY_DUMMY,
		"Should owned items be displayed permanently on the HUD,\n"
		"or only when their amount has changed?\n"
	},
	{
		"showItemsTime",
		NULL,
		"2.0",
		KEY_DUMMY,
		"The time in seconds to display item information when\n"
		"it has changed and the showItems option is turned on.\n"
	},
		{
				"showScoreDecimals",
				NULL,
				"1",
				KEY_DUMMY,
				"The number of decimals to use when displaying scores.\n"
		},
	{
		"receiveWindowSize",
		NULL,
		"3",
		KEY_DUMMY,
		"Too complicated.  Keep it on 3.\n"
	},
	{
		"mono",
		"Yes",
		"",
		KEY_DUMMY,
		"Only use black and white (may be very slow).\n"
	},
	{
		"erase",
		"Yes",
		"",
		KEY_DUMMY,
		"Use Erase(-hack) for slow X.\n"
	},
	{
		"colorSwitch",
		NULL,
		"Yes",
		KEY_DUMMY,
		"Use color buffering or not.\n"
		"Usually color buffering is faster, especially on 8-bit PseudoColor displays.\n"
	},
	{
		"multibuffer",
		NULL,
		"No",
		KEY_DUMMY,
		"Use the X windows multibuffer extension if present.\n"
	},
	{
		"maxColors",
		NULL,
		"8",
		KEY_DUMMY,
		"The number of colors to use.  Valid values are 4, 8 and 16.\n"
	},
	{
		"black",
		NULL,
		"",
		KEY_DUMMY,
		"The color value for black (better use color0 instead).\n"
	},
	{
		"white",
		NULL,
		"",
		KEY_DUMMY,
		"The color value for white (better use color1 instead).\n"
	},
	{
		"blue",
		NULL,
		"",
		KEY_DUMMY,
		"The color value for blue (better use color2 instead).\n"
	},
	{
		"red",
		NULL,
		"",
		KEY_DUMMY,
		"The color value for red (better use color3 instead).\n"
	},
	{
		"color0",
		NULL,
		"",
		KEY_DUMMY,
		"The color value for the first color.\n"
	},
	{
		"color1",
		NULL,
		"",
		KEY_DUMMY,
		"The color value for the second color.\n"
	},
	{
		"color2",
		NULL,
		"",
		KEY_DUMMY,
		"The color value for the third color.\n"
	},
	{
		"color3",
		NULL,
		"",
		KEY_DUMMY,
		"The color value for the fourth color.\n"
	},
	{
		"color4",
		NULL,
		"",
		KEY_DUMMY,
		"The color value for the fifth color.\n"
		"This is only used if maxColors is set to 8 or 16.\n"
	},
	{
		"color5",
		NULL,
		"",
		KEY_DUMMY,
		"The color value for the sixth color.\n"
		"This is only used if maxColors is set to 8 or 16.\n"
	},
	{
		"color6",
		NULL,
		"",
		KEY_DUMMY,
		"The color value for the seventh color.\n"
		"This is only used if maxColors is set to 8 or 16.\n"
	},
	{
		"color7",
		NULL,
		"",
		KEY_DUMMY,
		"The color value for the eighth color.\n"
		"This is only used if maxColors is set to 8 or 16.\n"
	},
	{
		"color8",
		NULL,
		"",
		KEY_DUMMY,
		"The color value for the nineth color.\n"
		"This is only used if maxColors is set to 16.\n"
	},
	{
		"color9",
		NULL,
		"",
		KEY_DUMMY,
		"The color value for the tenth color.\n"
		"This is only used if maxColors is set to 16.\n"
	},
	{
		"color10",
		NULL,
		"",
		KEY_DUMMY,
		"The color value for the eleventh color.\n"
		"This is only used if maxColors is set to 16.\n"
	},
	{
		"color11",
		NULL,
		"",
		KEY_DUMMY,
		"The color value for the twelfth color.\n"
		"This is only used if maxColors is set to 16.\n"
	},
	{
		"color12",
		NULL,
		"",
		KEY_DUMMY,
		"The color value for the thirteenth color.\n"
		"This is only used if maxColors is set to 16.\n"
	},
	{
		"color13",
		NULL,
		"",
		KEY_DUMMY,
		"The color value for the fourteenth color.\n"
		"This is only used if maxColors is set to 16.\n"
	},
	{
		"color14",
		NULL,
		"",
		KEY_DUMMY,
		"The color value for the fifteenth color.\n"
		"This is only used if maxColors is set to 16.\n"
	},
	{
		"color15",
		NULL,
		"",
		KEY_DUMMY,
		"The color value for the sixteenth color.\n"
		"This is only used if maxColors is set to 16.\n"
	},
	{
		"hudColor",
		NULL,
		"4",
		KEY_DUMMY,
		"Which color number to use for drawing the HUD.\n"
	},
	{
		"hudLockColor",
		NULL,
		"4",
		KEY_DUMMY,
		"Which color number to use for drawing the lock on the HUD.\n"
	},
	{
		"wallColor",
		NULL,
		"2",
		KEY_DUMMY,
		"Which color number to use for drawing walls.\n"
	},
	{
		"wallRadarColor",
		NULL,
		"2",
		KEY_DUMMY,
		"Which color number to use for drawing walls on the radar.\n"
		"Valid values all even numbers smaller than maxColors.\n"
	},
	{
		"decorColor",
		NULL,
		"6",
		KEY_DUMMY,
		"Which color number to use for drawing decorations.\n"
	},
	{
		"showDecor",
		NULL,
		"Yes",
		KEY_DUMMY,
		"Should decorations be displayed on the screen and radar?\n"
	},
	{
		"decorRadarColor",
		NULL,
		"6",
		KEY_DUMMY,
		"Which color number to use for drawing decorations on the radar.\n"
		"Valid values are all even numbers smaller than maxColors.\n"
	},
	{	
		"oldMessagesColor",
		NULL,
		"1",
		KEY_DUMMY,
		"Which color number to use for drawing old messages.\n"
	},
	{
		"outlineDecor",
		NULL,
		"No",
		KEY_DUMMY,
		"Draws only the outline of the map decoration.\n"
	},
	{
		"filledDecor",
		NULL,
		"No",
		KEY_DUMMY,
		"Draws filled decorations.\n"
	},
	{
		"texturedDecor",
		NULL,
		"No",
		KEY_DUMMY,
		"Draws the map decoration filled with a texture pattern.\n"
		"See also the decorTextureFile and texturedWalls options.\n"
	},
	{
		"decorTextureFile",
		NULL,
		"",
		KEY_DUMMY,
		"Specify a XPM format pixmap file to load the decor texture from.\n"
	},
	{
		"texturedBalls",
		NULL,
		"No",
		KEY_DUMMY,
		"Draw the balls with a texture specified by the ballTextureFile option.\n"
	},
	{
		"ballTextureFile",
		NULL,
		"",
		KEY_DUMMY,
		"Specify a XPM format pixmap file to load the ball texture from.\n"
	},
	{
		"targetRadarColor",
		NULL,
		"4",
		KEY_DUMMY,
		"Which color number to use for drawing targets on the radar.\n"
		"Valid values are all even numbers smaller than maxColors.\n"
	},
	{
		"sparkColors",
		NULL,
		"5,6,7,3",
		KEY_DUMMY,
		"Which color numbers to use for spark and debris particles.\n"
	},
	{
		"modifierBank1",
		NULL,
		"",
		KEY_DUMMY,
		"The default weapon modifier values for the first modifier bank.\n"
	},
	{
		"modifierBank2",
		NULL,
		"",
		KEY_DUMMY,
		"The default weapon modifier values for the second modifier bank.\n"
	},
	{
		"modifierBank3",
		NULL,
		"",
		KEY_DUMMY,
		"The default weapon modifier values for the third modifier bank.\n"
	},
	{
		"modifierBank4",
		NULL,
		"",
		KEY_DUMMY,
		"The default weapon modifier values for the fourth modifier bank.\n"
	},

	///////////////////////////////////////////////////////////////////////////
	// real keys
	{
		"keyTurnLeft",
		NULL,
		"a",
		KEY_TURN_LEFT,
		"Turn left (anti-clockwise).\n"
	},
	{
		"keyTurnRight",
		NULL,
		"s",
		KEY_TURN_RIGHT,
		"Turn right (clockwise).\n"
	},
	{
		"keyThrust",
		NULL,
		"Shift_R Shift_L",
		KEY_THRUST,
		"Thrust.\n"
	},
	{
		"keyShield",
		NULL,
		"space Caps_Lock",
		KEY_SHIELD,
		"Raise or toggle the shield.\n"
	},
	{
		"keyFireShot",
		NULL,
		"Return Linefeed",
		KEY_FIRE_SHOT,
		"Fire shot.\n"
		"Note that shields must be down to fire.\n"
	},
	{
		"keyFireMissile",
		NULL,
		"backslash",
		KEY_FIRE_MISSILE,
		"Fire smart missile.\n"
	},
	{
		"keyFireTorpedo",
		NULL,
		"quoteright",
		KEY_FIRE_TORPEDO,
		"Fire unguided torpedo.\n"
	},
	{
		"keyFireHeat",
		NULL,
		"semicolon",
		KEY_FIRE_HEAT,
		"Fire heatseeking missile.\n"
	},
	{
		"keyFireLaser",
		NULL,
		"slash",
		KEY_FIRE_LASER,
		"Activate laser beam.\n"
	},
	{
		"keyDropMine",
		NULL,
		"Tab",
		KEY_DROP_MINE,
		"Drop a stationary mine.\n"
	},
	{
		"keyDetachMine",
		NULL,
		"bracketright",
		KEY_DETACH_MINE,
		"Detach a moving mine.\n"
	},
	{
		"keyDetonateMines",
		NULL,
		"equal",
		KEY_DETONATE_MINES,
		"Detonate the mine you have dropped or thrown, which is closest to you.\n"
	},
	{
		"keyLockClose",
		NULL,
		"Select Up",
		KEY_LOCK_CLOSE,
		"Lock on closest player.\n"
	},
	{
		"keyLockNextClose",
		NULL,
		"Down",
		KEY_LOCK_NEXT_CLOSE,
		"Lock on next closest player.\n"
	},
	{
		"keyLockNext",
		NULL,
		"Next Right",
		KEY_LOCK_NEXT,
		"Lock on next player.\n"
	},
	{
		"keyLockPrev",
		NULL,
		"Prior Left",
		KEY_LOCK_PREV,
		"Lock on previous player.\n"
	},
	{
		"keyRefuel",
		NULL,
		"f Control_L Control_R",
		KEY_REFUEL,
		"Refuel.\n"
	},
	{
		"keyRepair",
		NULL,
		"f",
		KEY_REPAIR,
		"Repair target.\n"
	},
	{
		"keyCloak",
		NULL,
		"Delete BackSpace",
		KEY_CLOAK,
		"Toggle cloakdevice.\n"
	},
	{
		"keyEcm",
		NULL,
		"bracketleft",
		KEY_ECM,
		"Use ECM.\n"
	},
	{
		"keySelfDestruct",
		NULL,
		"End",
		KEY_SELF_DESTRUCT,
		"Toggle self destruct.\n"
	},
	{
		"keyIdMode",
		NULL,
		"u",
		KEY_ID_MODE,
		"Toggle User mode (show real names).\n"
	},
	{
		"keyPause",
		NULL,
		"Pause",
		KEY_PAUSE,
		"Toggle pause mode.\n"
		"When the ship is stationary on its homebase.\n"
	},
	{
		"keySwapSettings",
		NULL,
		"Escape",
		KEY_SWAP_SETTINGS,
		"Swap control settings.\n"
		"These are the power, turn speed and turn resistance settings.\n"
	},
	{
		"keySwapScaleFactor",
		NULL,
		"",
		KEY_SWAP_SCALEFACTOR,
		"Swap scalefactor settings.\n"
		"These are the scalefactor settings.\n"
	},
	{
		"keyShowStdout",
		NULL,
		"KP_1",
		KEY_SHOW_STDOUT,
		"Show the messages that appear on the startup screen.\n"
	},
	{
		"keyChangeHome",
		NULL,
		"Home h",
		KEY_CHANGE_HOME,
		"Change home base.\n"
		"When the ship is stationary on a new homebase.\n"
	},
	{
		"keyConnector",
		NULL,
		"Control_L",
		KEY_CONNECTOR,
		"Connect to a ball.\n"
	},
	{
		"keyDropBall",
		NULL,
		"d",
		KEY_DROP_BALL,
		"Drop a ball.\n"
	},
	{
		"keyTankNext",
		NULL,
		"e",
		KEY_TANK_NEXT,
		"Use the next tank.\n"
	},
	{
		"keyTankPrev",
		NULL,
		"w",
		KEY_TANK_PREV,
		"Use the the previous tank.\n"
	},
	{
		"keyTankDetach",
		NULL,
		"r",
		KEY_TANK_DETACH,
		"Detach the current tank.\n"
	},
	{
		"keyFlipRobotPage",
		NULL,
		"r",
		KEY_FLIP_ROBOT_PAGE,
		"Flip to the next robot page (if watching).\n"
	},
	{
		"keyIncreasePower",
		NULL,
		"KP_Multiply",
		KEY_INCREASE_POWER,
		"Increase engine power.\n"
	},
	{
		"keyDecreasePower",
		NULL,
		"KP_Divide",
		KEY_DECREASE_POWER,
		"Decrease engine power.\n"
	},
	{
		"keyIncreaseTurnspeed",
		NULL,
		"KP_Add",
		KEY_INCREASE_TURNSPEED,
		"Increase turnspeed.\n"
	},
	{
		"keyDecreaseTurnspeed",
		NULL,
		"KP_Subtract",
		KEY_DECREASE_TURNSPEED,
		"Decrease turnspeed.\n"
	},
	{
		"keyTransporter",
		NULL,
		"t",
		KEY_TRANSPORTER,
		"Use transporter to steal an item.\n"
	},
	{
		"keyDeflector",
		NULL,
		"o",
		KEY_DEFLECTOR,
		"Toggle deflector.\n"
	},
	{
		"keyHyperJump",
		NULL,
		"q",
		KEY_HYPERJUMP,
		"Teleport.\n"
	},
	{
		"keyPhasing",
		NULL,
		"p",
		KEY_PHASING,
		"Use phasing device.\n"
	},
	{
		"keyTalk",
		NULL,
		"m",
		KEY_TALK,
		"Toggle talk window.\n"
	},
	{
		"keyToggleNuclear",
		NULL,
		"n",
		KEY_TOGGLE_NUCLEAR,
		"Toggle nuclear weapon modifier.\n"
	},
	{
		"keyToggleCluster",
		NULL,
		"c",
		KEY_TOGGLE_CLUSTER,
		"Toggle cluster weapon modifier.\n"
	},
	{
		"keyToggleImplosion",
		NULL,
		"i",
		KEY_TOGGLE_IMPLOSION,
		"Toggle implosion weapon modifier.\n"
	},
	{
		"keyToggleVelocity",
		NULL,
		"v",
		KEY_TOGGLE_VELOCITY,
		"Toggle explosion velocity weapon modifier.\n"
	},
	{
		"keyToggleMini",
		NULL,
		"x",
		KEY_TOGGLE_MINI,
		"Toggle mini weapon modifier.\n"
	},
	{
		"keyToggleSpread",
		NULL,
		"z",
		KEY_TOGGLE_SPREAD,
		"Toggle spread weapon modifier.\n"
	},
	{
		"keyTogglePower",
		NULL,
		"b",
		KEY_TOGGLE_POWER,
		"Toggle power weapon modifier.\n"
	},
	{
		"keyToggleCompass",
		NULL,
		"KP_7",
		KEY_TOGGLE_COMPASS,
		"Toggle HUD/radar compass lock.\n"
	},
	{
		"keyToggleAutoPilot",
		NULL,
		"h",
		KEY_TOGGLE_AUTOPILOT,
		"Toggle automatic pilot mode.\n"
	},
	{
		"keyToggleLaser",
		NULL,
		"l",
		KEY_TOGGLE_LASER,
		"Toggle laser modifier.\n"
	},
	{
		"keyEmergencyThrust",
		NULL,
		"j",
		KEY_EMERGENCY_THRUST,
		"Pull emergency thrust handle.\n"
	},
	{
		"keyEmergencyShield",
		NULL,
		"g",
		KEY_EMERGENCY_SHIELD,
		"Toggle emergency shield power.\n"
	},
	{
		"keyTractorBeam",
		NULL,
		"comma",
		KEY_TRACTOR_BEAM,
		"Use tractor beam in attract mode.\n"
	},
	{
		"keyPressorBeam",
		NULL,
		"period",
		KEY_PRESSOR_BEAM,
		"Use tractor beam in repulse mode.\n"
	},
	{
		"keyClearModifiers",
		NULL,
		"k",
		KEY_CLEAR_MODIFIERS,
		"Clear current weapon modifiers.\n"
	},
	{
		"keyLoadModifiers1",
		NULL,
		"1",
		KEY_LOAD_MODIFIERS_1,
		"Load the weapon modifiers from bank 1.\n"
	},
	{
		"keyLoadModifiers2",
		NULL,
		"2",
		KEY_LOAD_MODIFIERS_2,
		"Load the weapon modifiers from bank 2.\n"
	},
	{
		"keyLoadModifiers3",
		NULL,
		"3",
		KEY_LOAD_MODIFIERS_3,
		"Load the weapon modifiers from bank 3.\n"
	},
	{
		"keyLoadModifiers4",
		NULL,
		"4",
		KEY_LOAD_MODIFIERS_4,
		"Load the weapon modifiers from bank 4.\n"
	},
	{
		"keyToggleOwnedItems",
		NULL,
		"KP_8",
		KEY_TOGGLE_OWNED_ITEMS,
		"Toggle list of owned items on HUD.\n"
	},
	{
		"keyToggleMessages",
		NULL,
		"KP_9",
		KEY_TOGGLE_MESSAGES,
		"Toggle showing of messages.\n"
	},
	{
		"keyReprogram",
		NULL,
		"quoteleft",
		KEY_REPROGRAM,
		"Reprogram modifier or lock bank.\n"
	},
	{
		"keyLoadLock1",
		NULL,
		"5",
		KEY_LOAD_LOCK_1,
		"Load player lock from bank 1.\n"
	},
	{
		"keyLoadLock2",
		NULL,
		"6",
		KEY_LOAD_LOCK_2,
		"Load player lock from bank 2.\n"
	},
	{
		"keyLoadLock3",
		NULL,
		"7",
		KEY_LOAD_LOCK_3,
		"Load player lock from bank 3.\n"
	},
	{
		"keyLoadLock4",
		NULL,
		"8",
		KEY_LOAD_LOCK_4,
		"Load player lock from bank 4.\n"
	},
	{
		"keyToggleRecord",
		NULL,
		"KP_5",
		KEY_TOGGLE_RECORD,
		"Toggle recording of session (see recordFile).\n"
	},
	{
		"keySelectItem",
		NULL,
		"KP_0 KP_Insert",
		KEY_SELECT_ITEM,
		"Select an item to lose.\n"
	},
	{
		"keyLoseItem",
		NULL,
		"KP_Delete KP_Decimal",
		KEY_LOSE_ITEM,
		"Lose the selected item.\n"
	},
#ifndef _WINDOWS
	{
		"keyPrintMessagesStdout",
		NULL,
		"Print",
		KEY_PRINT_MSGS_STDOUT,
		"Print the current messages to stdout.\n"
	},
	{
		"keyTalkCursorLeft",
		NULL,
		"Left",
		KEY_TALK_CURSOR_LEFT,
		"Move Cursor to the left in the talk window.\n"
	},
	{
		"keyTalkCursorRight",
		NULL,
		"Right",
		KEY_TALK_CURSOR_RIGHT,
		"Move Cursor to the right in the talk window.\n"
	},
	{
		"keyTalkCursorUp",
		NULL,
		"Up",
		KEY_TALK_CURSOR_UP,
		"Browsing in the history of the talk window.\n"
	},
	{
		"keyTalkCursorDown",
		NULL,
		"Down",
		KEY_TALK_CURSOR_DOWN,
		"Browsing in the history of the talk window.\n"
	},
#endif
	{
		"keyPointerControl",
		NULL,
		"KP_Enter",
		KEY_POINTER_CONTROL,
		"Toggle pointer control.\n"
	},
	{
		"pointerButton1",
		NULL,
		"keyFireShot",
		KEY_DUMMY,
		"The key to activate when pressing the first mouse button.\n"
	},
	{
		"pointerButton2",
		NULL,
		"keyThrust",
		KEY_DUMMY,
		"The key to activate when pressing the second mouse button.\n"
	},
	{
		"pointerButton3",
		NULL,
		"keyShield",
		KEY_DUMMY,
		"The key to activate when pressing the third mouse button.\n"
	},
	{
		"pointerButton4",
		NULL,
		"",
		KEY_DUMMY,
		"The key to activate when pressing the fourth mouse button.\n"
	},
	{
		"pointerButton5",
		NULL,
		"",
		KEY_DUMMY,
		"The key to activate when pressing the fifth mouse button.\n"
	},
	{
		"maxFPS",
		NULL,
		"20",
		KEY_DUMMY,
		"Set client's maximum FPS supported.\n"
	},
	{
		"recordFile",
		NULL,
		"",
		KEY_DUMMY,
		"An optional file where a recording of a game can be made.\n"
		"If this file is undefined then recording isn't possible.\n"
	},
	{
		"clientPortStart",
		NULL,
		"0",
		KEY_DUMMY,
		"Use UDP ports clientPortStart - clientPortEnd (for firewalls).\n"
	},
	{
		"clientPortEnd",
		NULL,
		"0",
		KEY_DUMMY,
		"Use UDP ports clientPortStart - clientPortEnd (for firewalls).\n"
	},
#ifdef _WINDOWS
	{
		"threadedDraw",
		NULL,
		"No",
		KEY_DUMMY,
		"Tell Windows to do the heavy BitBlt in another thread\n"
		},
	{
		"radarDivisor",
		NULL,
		"1",
		KEY_DUMMY,
		"Specifies how many frames between radar window updates.\n"
		},
#endif
	{
		"scaleFactor",
		NULL,
		"1.0",
		KEY_DUMMY,
		"Specifies scaling factor for the drawing window.\n"
	},
	{
		"altScaleFactor",
		NULL,
		"2.0",
		KEY_DUMMY,
		"Specifies alternative scaling factor for the drawing window.\n"
	},
	{
		"sounds",
		NULL,
		conf_soundfile_string,
		KEY_DUMMY,
		"Specifies the sound file.\n"
	},
	{
		"maxVolume",
		NULL,
		"100",
		KEY_DUMMY,
		"Specifies the volume to play sounds with.\n"
	},
#ifdef SPARC_CMAP_HACK
	{
		"frameBuffer",
		NULL,
		"/dev/fb",
		KEY_DUMMY,
		"Specifies the device name of the frame buffer.\n"
	},
#endif	  
#ifdef DEVELOPMENT
	{
		"test",
		NULL,
		"",
		KEY_DUMMY,
		"Which development testing parameters to use?\n"
	},
#endif
/* talk macros: */
	{
		"keySendMsg1",
		NULL,
		"F1",
		KEY_MSG_1,
		"Sends the talkmessage stored in msg1.\n"
	},
	{
		"msg1",
		NULL,
		"",
		KEY_DUMMY,
		"Talkmessage 1.\n"
	},
	{
		"keySendMsg2",
		NULL,
		"F2",
		KEY_MSG_2,
		"Sends the talkmessage stored in msg2.\n"
	},
	{
		"msg2",
		NULL,
		"",
		KEY_DUMMY,
		"Talkmessage 2.\n"
	},
	{
		"keySendMsg3",
		NULL,
		"F3",
		KEY_MSG_3,
		"Sends the talkmessage stored in msg3.\n"
	},
	{
		"msg3",
		NULL,
		"",
		KEY_DUMMY,
		"Talkmessage 3.\n"
	},
	{
		"keySendMsg4",
		NULL,
		"F4",
		KEY_MSG_4,
		"Sends the talkmessage stored in msg4.\n"
	},
	{
		"msg4",
		NULL,
		"",
		KEY_DUMMY,
		"Talkmessage 4.\n"
	},
	{
		"keySendMsg5",
		NULL,
		"F5",
		KEY_MSG_5,
		"Sends the talkmessage stored in msg5.\n"
	},
	{
		"msg5",
		NULL,
		"",
		KEY_DUMMY,
		"Talkmessage 5.\n"
	},
	{
		"keySendMsg6",
		NULL,
		"F6",
		KEY_MSG_6,
		"Sends the talkmessage stored in msg6.\n"
	},
	{
		"msg6",
		NULL,
		"",
		KEY_DUMMY,
		"Talkmessage 6.\n"
	},
	{
		"keySendMsg7",
		NULL,
		"F7",
		KEY_MSG_7,
		"Sends the talkmessage stored in msg7.\n"
	},
	{
		"msg7",
		NULL,
		"",
		KEY_DUMMY,
		"Talkmessage 7.\n"
	},
	{
		"keySendMsg8",
		NULL,
		"F8",
		KEY_MSG_8,
		"Sends the talkmessage stored in msg8.\n"
	},
	{
		"msg8",
		NULL,
		"",
		KEY_DUMMY,
		"Talkmessage 8.\n"
	},
	{
		"keySendMsg9",
		NULL,
		"F9",
		KEY_MSG_9,
		"Sends the talkmessage stored in msg9.\n"
	},
	{
		"msg9",
		NULL,
		"",
		KEY_DUMMY,
		"Talkmessage 9.\n"
	},
	{
		"keySendMsg10",
		NULL,
		"F10",
		KEY_MSG_10,
		"Sends the talkmessage stored in msg10.\n"
	},
	{
		"msg10",
		NULL,
		"",
		KEY_DUMMY,
		"Talkmessage 10.\n"
	},
	{
		"keySendMsg11",
		NULL,
		"F11",
		KEY_MSG_11,
		"Sends the talkmessage stored in msg11.\n"
	},
	{
		"msg11",
		NULL,
		"",
		KEY_DUMMY,
		"Talkmessage 11.\n"
	},
	{
		"keySendMsg12",
		NULL,
		"F12",
		KEY_MSG_12,
		"Sends the talkmessage stored in msg12.\n"
	},
	{
		"msg12",
		NULL,
		"",
		KEY_DUMMY,
		"Talkmessage 12.\n"
	},
	{
		"keySendMsg13",
		NULL,
		"",
		KEY_MSG_13,
		"Sends the talkmessage stored in msg13.\n"
	},
	{
		"msg13",
		NULL,
		"",
		KEY_DUMMY,
		"Talkmessage 13.\n"
	},
	{
		"keySendMsg14",
		NULL,
		"",
		KEY_MSG_14,
		"Sends the talkmessage stored in msg14.\n"
	},
	{
		"msg14",
		NULL,
		"",
		KEY_DUMMY,
		"Talkmessage 14.\n"
	},
	{
		"keySendMsg15",
		NULL,
		"",
		KEY_MSG_15,
		"Sends the talkmessage stored in msg15.\n"
	},
	{
		"msg15",
		NULL,
		"",
		KEY_DUMMY,
		"Talkmessage 15.\n"
	},
	{
		"keySendMsg16",
		NULL,
		"",
		KEY_MSG_16,
		"Sends the talkmessage stored in msg16.\n"
	},
	{
		"msg16",
		NULL,
		"",
		KEY_DUMMY,
		"Talkmessage 16.\n"
	},
	{
		"keySendMsg17",
		NULL,
		"",
		KEY_MSG_17,
		"Sends the talkmessage stored in msg17.\n"
	},
	{
		"msg17",
		NULL,
		"",
		KEY_DUMMY,
		"Talkmessage 17.\n"
	},
	{
		"keySendMsg18",
		NULL,
		"",
		KEY_MSG_18,
		"Sends the talkmessage stored in msg18.\n"
	},
	{
		"msg18",
		NULL,
		"",
		KEY_DUMMY,
		"Talkmessage 18.\n"
	},
	{
		"keySendMsg19",
		NULL,
		"",
		KEY_MSG_19,
		"Sends the talkmessage stored in msg19.\n"
	},
	{
		"msg19",
		NULL,
		"",
		KEY_DUMMY,
		"Talkmessage 19.\n"
	},
	{
		"keySendMsg20",
		NULL,
		"",
		KEY_MSG_20,
		"Sends the talkmessage stored in msg20.\n"
	},
	{
		"msg20",
		NULL,
		"",
		KEY_DUMMY,
		"Talkmessage 20.\n"
	},
};

void IniClient::SetDefaultOptions()
{
		numOptions = NELEM(clientOptions);
		options = clientOptions;
}

