/* $Id: IniClient.h,v 1.12 2005/03/17 22:12:13 kps Exp $
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
 * $Log: IniClient.h,v $
 * Revision 1.12  2005/03/17 22:12:13  kps
 * Get rid of warnings from makedepend about "non-portable whitespace".
 *
 * Revision 1.11  2004/07/22 17:00:05  dick
 * PCSTR defaultShipString is the chars for the default XPilot ship.
 *
 * Revision 1.10  2004/05/28 17:55:58  dick
 * shipShape becomes a String (not a char*).
 * client's MAX_MSGS goes global to here.
 *
 * Revision 1.9  2004/05/13 17:13:27  dick
 * Don't do #ifdef SOUND in common.  Always compile it.
 *
 * Revision 1.8  2004/05/11 06:33:13  dick
 * audioServer is gone
 *
 * Revision 1.7  2004/04/29 16:08:27  dick
 * compile with SOUND enabled.
 *
 * Revision 1.6  2004/01/20 15:14:27  dick
 * Get rid of #define WINDOWSCALING.  Window scaling is always built, it's not an option.
 *
 * Revision 1.5  2004/01/06 04:48:28  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 * Revision 1.4  2002/09/16 21:26:19  dick
 * Stringize ParseOptions().
 *
 * Revision 1.3  2002/09/03 07:42:31  dick
 * Remove bad curDB (defined in base class).
 *
 * Revision 1.2  2002/09/02 21:09:13  dick
 * Move generic X resources (dpy and kdpy) from the subclass to the base class Ini.
 *
 * Revision 1.1  2002/09/02 15:45:49  dick
 * Break Ini up into base class Ini and subclass IniClient.
 *
 */
#ifndef	_INICLIENT_H_
#define	_INICLIENT_H_

#include <stdio.h>

#if defined(_UNIX) || defined(_CYGWIN)
# include <X11/Xlib.h>
# include <X11/Xos.h>
# include <X11/Xutil.h>
# include <X11/Xresource.h>
#endif

#include "keys.h"
#include "const.h"
#include "talk.h"
#include "cstring.h"

#include "Ini.h"

#define MAX_MSGS		15	/* Max. messages displayed ever */

#define MAX_POINTER_BUTTONS 5
#define	MAX_BUTTON_DEFS		3
#define	NUM_BUTTON_DEFS(i)	(buttonDefs[(i)][MAX_BUTTON_DEFS])

/*
 * Spark rand limits.
 */
#define MIN_SPARK_RAND	    0		/* Not display spark */
#define MAX_SPARK_RAND	    0x80	/* Always display spark */

#define MAX_SPARK_SIZE		8
#define MIN_SPARK_SIZE		1
#define MAX_MAP_POINT_SIZE	8
#define MIN_MAP_POINT_SIZE	0
#define MAX_SHOT_SIZE		8
#define MIN_SHOT_SIZE		1
#define MAX_TEAMSHOT_SIZE	8
#define MIN_TEAMSHOT_SIZE	1

#define MAX_COLORS		16	/* Max. switched colors ever */
#define MAX_COLOR_LEN		32	/* Max. length of a color name */

#define NUM_COLORS	    4

/* Bits in the instrument setting */
#define SHOW_HUD_INSTRUMENTS	(1L << 0)
#define SHOW_HUD_VERTICAL	(1L << 1)
#define SHOW_HUD_HORIZONTAL	(1L << 2)
#define SHOW_FUEL_METER		(1L << 3)
#define SHOW_FUEL_GAUGE		(1L << 4)
#define SHOW_TURNSPEED_METER	(1L << 5)
#define SHOW_POWER_METER	(1L << 6)
#define SHOW_SHIP_NAME		(1L << 7)
#define SHOW_SLIDING_RADAR	(1L << 8)
#define SHOW_PACKET_SIZE_METER	(1L << 10)
#define SHOW_PACKET_LOSS_METER	(1L << 11)
#define SHOW_PACKET_DROP_METER	(1L << 12)
#define SHOW_CLOCK		(1L << 13)
#define SHOW_ITEMS		(1L << 14)
#define SHOW_MESSAGES		(1L << 15)
#define SHOW_MINE_NAME		(1L << 16)
#define SHOW_OUTLINE_WORLD	(1L << 17)
#define SHOW_FILLED_WORLD	(1L << 18)
#define SHOW_TEXTURED_WALLS	(1L << 19)
#define SHOW_DECOR		(1L << 20)
#define SHOW_OUTLINE_DECOR	(1L << 21)
#define SHOW_FILLED_DECOR	(1L << 22)
#define SHOW_TEXTURED_DECOR	(1L << 23)
#define SHOW_CLOCK_AMPM_FORMAT	(1L << 24)
#define SHOW_TEXTURED_BALLS	(1L << 25)
#define SHOW_REVERSE_SCROLL	(1L << 26)
#define	SHOW_HUD_RADAR		(1L << 27)
#define	SHOW_PACKET_LAG_METER	(1L << 28)

#define MIN_SHOW_ITEMS_TIME	0.0
#define MAX_SHOW_ITEMS_TIME	10.0

#define MIN_RECEIVE_WINDOW_SIZE		1
#define MAX_RECEIVE_WINDOW_SIZE		4

#define MIN_SCALEFACTOR		0.2
#define MAX_SCALEFACTOR		8.0


class IniClient : public Ini
{
public:
	IniClient();
	~IniClient();


	virtual	void Save(FILE* fp);

	void	SetDefaultOptions();
	void	ParseOptions(int *argcp, char **argvp, String& realName, int *port,
						 int *my_team, bool* text, bool* list,
						 bool* join, bool* noLocalMotd,
						 String& nickName, String& dispName, String& hostName,
						 char *shut_msg);
	void	ParseSwitches(int *argcp, char **argvp);

	void	SetArgcArgv(int argc, char** argv);
	void	InsertKeyDef(KeySym ks, keys_t key);
	void	DeleteKeyDef(KeySym ks, keys_t key);

public:
	String	name;					/* Nick-name of player */
	String	realname;				/* Real name of player */
	String	hostname;				/* Host name of player */
	char	servername[MAX_CHARS];	/* Name of server connecting to */
	int		serverport;				/* port of server connecting to */
	unsigned short	team;			/* What team is the player on? */
	bool	autoServerMotdPopup;
	bool	refreshMotd;
	String	shipShape;				/* Shape of player's ship */
	char	shipShapeName[MAX_CHARS];	/* Name of players ship in shipShapeFile*/
	DFLOAT	power;					/* Force of thrust */
	DFLOAT	power_s;				/* Saved power fiks */
	DFLOAT	turnspeed;				/* How fast player acc-turns */
	DFLOAT	turnspeed_s;			/* Saved turnspeed */
	DFLOAT	turnresistance;			/* How much is lost in % */
	DFLOAT	turnresistance_s;		/* Saved (see above) */
	DFLOAT	sparkProb;				/* Sparkling effect configurable */
	u_byte	sparkRand;				/* Sparkling effect */
	int		charsPerSecond;			/* Message output speed (config) */
	int		showScoreDecimals;		/* number of chars to the right of the "." */
	bool	markingLights;			/* Marking lights on ships */
	int		mapPointDistance;		/* spacing of navigation points */
	int		mapPointSize;			/* size of navigation points */
	int		sparkSize;				/* size of sparks and debris */
	int		shotSize;				/* size of shot */
	int		teamShotSize;			/* size of team shot */
	bool	showNastyShots;			/* show original flavor shots or the new "nasty shots" */
	bool	titleFlip;				/* Do special title bar flipping? */
	int		shieldDrawMode;			/* How to draw players shield */
	bool	toggleShield;			/* Are shields toggled by a press? */
	bool	autoShield;				/* shield drops for fire */
	int		clientPortStart;		/* First UDP port for clients */
	int		clientPortEnd;			/* Last one (these are for firewalls) */
	char	modBankStr[NUM_MODBANKS][MAX_CHARS];	/* modifier banks */
	bool	mono;
	bool	multiBuffer;
	int		maxColors;				/* Max. number of colors to use */
	char	colorNames[MAX_COLORS][MAX_COLOR_LEN];
	bool	colorSwitch;
	int		hudColor;				/* Color index for HUD drawing */
	int		hudLockColor;			/* Color index for lock on HUD drawing */
	int		wallColor;				/* Color index for wall drawing */
	int		wallRadarColor;			/* Color index for walls on radar */
	int		targetRadarColor;		/* Color index for targets on radar */
	int		decorColor;				/* Color index for decoration drawing */
	int		decorRadarColor;		/* Color index for decorations on radar */
	int		oldMessagesColor;		/* Color index for old message strings */
	char	sparkColors[MSG_LEN];
	long	instruments;			/* Instruments on screen (bitmask) */
	bool	blockBitmaps;			/* Whether to draw everything as bitmaps. */
	bool	initialPointerControl;	/* Start by using mouse for control? */
	bool	useErase;				/* use the Erase hack for slow X */
	DFLOAT	showItemsTime;			/* How long to show changed item count for */
	DFLOAT	hudMoveFact;			/* scale the hud-movement (speed) */
	DFLOAT	ptrMoveFact;			/* scale the speed pointer length */
	int		fuelLevel1;				/* Fuel critical level */
	int		fuelLevel2;				/* Fuel warning level */
	int		fuelLevel3;				/* Fuel notify level */
	char	gameFontName[FONT_LEN];	/* The Fonts used in the game */
	char	messageFontName[FONT_LEN];
	char	scoreListFontName[FONT_LEN];
	char	buttonFontName[FONT_LEN];
	char	textFontName[FONT_LEN];
	char	talkFontName[FONT_LEN];
	char	motdFontName[FONT_LEN];
	int		maxMessages;			/* Max. number of messages to display */
	int		messagesToStdout;		/* Send messages to standard output */
	int		receiveWindowSize;
	char*	texturePath;			/* Path list of texture directories */
	char*	wallTextureFile;		/* Filename of wall texture */
	char*	decorTextureFile;		/* Filename of decor texture */
	char*	ballTextureFile;		/* Filename of ball texture */
	char*	recordFile;				/* Filename to write recorded games too */
	String	shipShapeFile;			/* Filename of shipshapes */
	int		maxFPS;					/* Client's own FPS */
	int		radarDivisor;			/* Radar window is updated every RadarDivisor frames.*/
#if defined(_WINDOWS) && !defined(_CYGWIN)
	bool	threadedDraw;
#endif

	char	sounds[MAX_CHARS];		/* audio mappings */
	int 	maxVolume;				/* maximum volume (in percent) */

	bool	selectionAndHistory;	// Why is this stuff not on Windows?
	int		ignoreWindowManager;
	int		maxLinesInHistory;		/* number of lines to save in history */
#	define MAX_HIST_MSGS	128		/* maximum */

	DFLOAT	scaleFactor;			/* scale the draw (main playfield) window */
	DFLOAT	scaleFactor_s;

	int			maxKeyDefs;
	KeyDef*		keyDefs;
	char*		talk_fast_msgs[TALK_FAST_NR_OF_MSGS];	/* talk macros */

private:
	void	GetShipshapeResource(String* ship_shape);
	int		FindKey(keys_t key, int start, int end, int *key_index);

	void	SaveKeys(FILE* fp);

#if defined(_WINDOWS) && !defined(_CYGWIN)
//	void	GetProfileString(PCSTR key,	PCSTR def, char* result, int size);
//	void	GetTeam(int* my_team);
//	void	GetName();
#else
	int		Xpilotrc_add(char *line);
#endif
};

extern	IniClient	iniClient;	// each app has one instance of an Ini

extern	keys_t		buttonDefs[MAX_POINTER_BUTTONS][MAX_BUTTON_DEFS+1];

extern	PCSTR		defaultShipString;	// the classic triangle.

#endif		// _INICLIENT_H_
