/* $Id: paint.cpp,v 1.25 2007/01/17 21:35:15 dick Exp $
 *
 * paint - the master drawing handling.
 *
 * client - the user interface to the game.
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
 * $Log: paint.cpp,v $
 * Revision 1.25  2007/01/17 21:35:15  dick
 * Encapsulate all of the RobotWatch features into a RobotWatchMan object.
 *
 * Revision 1.24  2007/01/17 08:59:49  dick
 * RobotWatch is a list of Strings sent from the client when a player is paused
 * and watching a robot.  This list contains diagnostic information about
 * what the heck the robot thinks it's doing.
 * It's kinda like the Terminator view where he's looking at a 6502 dump.
 *
 * Revision 1.23  2004/06/03 06:04:42  dick
 * struct other_t becomes class Other.
 * array Others becomes ObjList others.
 * shipobj becomes ShipObj.
 *
 * Revision 1.22  2004/05/28 18:03:16  dick
 * MAX_MSGS goes global and now lives in Ini.
 *
 * Revision 1.21  2004/05/23 23:52:06  dick
 * src/common/IniClient.Defaults.cpp
 *
 * Revision 1.20  2004/05/20 22:23:42  dick
 * Add AddMessage(PCSTR, ...).  Wrap some debug with a D().
 *
 * Revision 1.19  2004/05/07 04:45:04  dick
 * Display rank data in the ScoreTable.
 *
 * Revision 1.18  2004/04/27 01:54:13  dick
 * Display urls in the message list as underlined.
 * Open a browser if the url is clicked on.
 *
 * Revision 1.17  2004/04/26 07:03:10  dick
 * Display the various new score table types
 *
 * Revision 1.16  2004/04/23 17:31:30  dick
 * Retab
 *
 * Revision 1.15  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.14  2004/01/26 16:55:49  dick
 * Fix bug where showDecimals = 0
 *
 * Revision 1.13  2004/01/06 04:48:27  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 * Revision 1.12  2002/09/02 21:06:06  dick
 * Encapsulate dpy and kdpy.  Singleton ini becomes iniClient.
 *
 * Revision 1.11  2002/09/02 15:42:40  dick
 * Singleton ini becomes iniClient.
 *
 * Revision 1.10  2002/08/23 12:41:08  bert
 * Fix broken compile due to undefined showScoreDecimals.
 * Do people actually compile before checkin...? ;-)
 *
 * Revision 1.9  2002/07/15 21:54:36  bert
 * these files all had file corruptions at several places.
 * mostly two adjacent bytes were replaced with bogus random data.
 * fixed.
 *
 * Revision 1.8  2002/06/26 06:47:03  dick
 * Replace errno with xperrno and errno=0 with seterrno(0).
 *
 * Revision 1.7  2002/05/18 20:55:33  dick
 * Update to XPilot-4.5.1
 *
 * Revision 1.6  2001/09/17 10:58:10  dick
 * Move dpy and kdpy to Ini
 *
 * Revision 1.5  2001/09/17 08:32:50  dick
 * Items has moved to common
 *
 * Revision 1.4  2001/09/11 09:08:30  dick
 * Encapsulate all client resources into new class Ini (Linux cleanups)
 *
 * Revision 1.3  2001/09/11 07:33:44  dick
 * Encapsulate all client resources into new class Ini.
 *
 * Revision 1.2  2001/07/27 05:47:39  dick
 * Client builds and runs in C++
 *
 */
#include "StdAfx.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <time.h>
#include <sys/types.h>

#if defined(_UNIX) || defined(_CYGWIN)
# include <unistd.h>
# include <X11/Xlib.h>
# include <X11/Xos.h>
#else
# include "NT/winX.h"
# include "NT/winClient.h"
# include "NT/winXXPilot.h"
# include "netclient.h"
#endif

#include "version.h"
#include "config.h"
#include "const.h"
#include "error.h"
#include "bit.h"
#include "types.h"
#include "keys.h"
#include "rules.h"
#include "setup.h"
#include "dbuff.h"
#include "texture.h"
#include "paint.h"
#include "paintdata.h"
#include "record.h"
#include "xinit.h"
#include "Other.h"
#include "blockbitmaps.h"
#include "portability.h"
#include "client.h"
#include "commonproto.h"
#include "Ini.h"
#include "ScoreTable.h"
#include "RobotWatchMan.h"

char paint_version[] = VERSION;

extern setup_t		*Setup;
extern int		RadarHeight;

/*
 * Globals.
 */
XFontStruct* gameFont;		/* The fonts used in the game */
XFontStruct* messageFont;
XFontStruct* scoreListFont;
XFontStruct* buttonFont;
XFontStruct* textFont;
XFontStruct* talkFont;
XFontStruct* motdFont;
//char	gameFontName[FONT_LEN];	/* The fonts used in the game */
//char	messageFontName[FONT_LEN];
//char	scoreListFontName[FONT_LEN];
//char	buttonFontName[FONT_LEN];
//char	textFontName[FONT_LEN];
//char	talkFontName[FONT_LEN];
//char	motdFontName[FONT_LEN];

//Display	*dpy;			/* Display of player (pointer) */
//Display	*kdpy;			/* Keyboard display */
short	about_page;		/* Which page is the player on? */
//unsigned short	team;		/* What team is the player on? */

GC	gc;			/* GC for the game area */
GC	messageGC;		/* GC for messages in the game area */
GC	radarGC;		/* GC for the radar */
GC	buttonGC;		/* GC for the buttons */
GC	scoreListGC;		/* GC for the player list */
GC	textGC;			/* GC for the info text */
GC	talkGC;			/* GC for the message window */
GC	motdGC;			/* GC for the motd text */
XGCValues	gcv;

Window	top;			/* Top-level window (topshell) */
Window	draw;			/* Main play window */
Window	keyboard;		/* Keyboard window */

#if defined(_WINDOWS) && !defined(_CYGWIN)
	/* Windows needs some dummy windows (size 0,0) */
	/* so we can store the active fonts.  Windows only */
	/* supports 1 active font per window */
Window	textWindow;		/* for the GC into the config window */
Window	msgWindow;		/* for meesages into the playfield */
Window	buttonWindow;		/* to calculate size of buttons */
#endif

Pixmap	p_draw;			/* Saved pixmap for the drawing */
				/* area (monochromes use this) */
Window	players;		/* Player list window */
				/* monochromes) */
//int	maxMessages;		/* Max. number of messages to display */
Window	about_w;		/* About window */
Window	about_close_b;		/* About window's close button */
Window	about_next_b;		/* About window's next button */
Window	about_prev_b;		/* About window's previous button */
Window	keys_close_b;		/* Help window's close button */
Window	talk_w;			/* Talk window */
XColor	colors[MAX_COLORS];	/* Colors */
Colormap	colormap;	/* Private colormap */
//int	maxColors;		/* Max. number of colors to use */
bool	gotFocus;
bool	players_exposed;
short	ext_view_width;		/* Width of extended visible area */
short	ext_view_height;	/* Height of extended visible area */
int	active_view_width;	/* Width of active map area displayed. */
int	active_view_height;	/* Height of active map area displayed. */
int	ext_view_x_offset;	/* Offset ext_view_width */
int	ext_view_y_offset;	/* Offset ext_view_height */

//bool	titleFlip;		/* Do special title bar flipping? */
//int	shieldDrawMode = -1;	/* Either LineOnOffDash or LineSolid */
//char	modBankStr[NUM_MODBANKS][MAX_CHARS];	/* modifier banks */
//char	*texturePath = NULL;		/* Path list of texture directories */
//bool	useErase;		/* use Erase hack for slow X */

long	loops = 0;

int 	cacheShips = 0; 		/* cache some ship bitmaps every frame */

static void Paint_clock(int redraw);

///////////////////////////////////////////////////////////////////////////////
void Game_over_action(u_byte stat)
{
	static u_byte old_stat = 0;

	if (BIT(old_stat, GAME_OVER) && !BIT(stat, GAME_OVER)
		&& !BIT(stat,PAUSE)) {
		XMapRaised(iniClient.dpy, top);
	}
	/* GAME_OVER -> PLAYING */
	if (BIT(old_stat, PLAYING|PAUSE|GAME_OVER) != PLAYING) {
		if (BIT(stat, PLAYING|PAUSE|GAME_OVER) == PLAYING) {
			Reset_shields();
		}
	}

	old_stat = stat;
}

///////////////////////////////////////////////////////////////////////////////
void Paint_frame(void)
{
	static long 		scroll_i = 0;
	static int			prev_damaged = 0;
	static int			prev_prev_damaged = 0;

#if defined(_WINDOWS) && !defined(_CYGWIN)		/* give any outgoing data a head start to the server */
	Net_flush();		/* send anything to the server before returning to Windows */
#endif

	if (start_loops != end_loops) {
				seterrno(0);
				error("Start neq. End (%ld,%ld,%ld)", start_loops, end_loops, loops);
	}
	loops = end_loops;

	/*
	 * Switch between two different window titles.
	 */
	if (iniClient.titleFlip && (loops % TITLE_DELAY) == 0) {
		scroll_i = !scroll_i;
		if (scroll_i)
			XStoreName(iniClient.dpy, top, COPYRIGHT);
		else
			XStoreName(iniClient.dpy, top, TITLE);

	}
	/* This seems to have a bug (in Windows) 'cause last frame we ended
	   with an XSetForeground(white) confusing SET_FG */
	SET_FG(colors[BLACK].pixel);

#if defined(_WINDOWS) && !defined(_CYGWIN)
	p_draw = draw;				/* let's try this */
	XSetForeground(iniClient.dpy, gc, colors[BLACK].pixel);
	XFillRectangle(iniClient.dpy, p_draw, gc, 0, 0, draw_width, draw_height);
#endif

	rd.newFrame();


	/*
	 * Do we really need to draw all this if the player is damaged?
	 */
	if (damaged <= 0) {
		if (prev_damaged || prev_prev_damaged) {
			/* clean up ecm damage */
			SET_FG(colors[BLACK].pixel);
			XFillRectangle(iniClient.dpy, draw, gc, 0, 0, draw_width, draw_height);
		}

		Erase_start();

		Arc_start();

		Rectangle_start();
		Segment_start();

		Paint_world();

		Segment_end();
		Rectangle_end();

		Rectangle_start();
		Segment_start();

		Paint_vfuel();
		Paint_vdecor();
		Paint_vcannon();
		Paint_vbase();
		Paint_shots();

		Rectangle_end();
		Segment_end();

		Rectangle_start();
		Segment_start();

		Paint_ships();
		Paint_meters();
		Paint_HUD();
		Paint_recording();

		Rectangle_end();
		Segment_end();

		Arc_end();

		Paint_messages();
		Paint_radar();
		Paint_score_objects();
		robotWatchMan.Paint();
	}
	else {
		/* Damaged. */

		XSetFunction(iniClient.dpy, gc, GXxor);
		SET_FG(colors[BLACK].pixel ^ colors[BLUE].pixel);
		XFillRectangle(iniClient.dpy, draw, gc, 0, 0, draw_width, draw_height);
		XSetFunction(iniClient.dpy, gc, GXcopy);
		SET_FG(colors[BLACK].pixel);
	}
	if (cacheShips && iniClient.blockBitmaps) {
		Cache_ships(draw);
	}
	prev_prev_damaged = prev_damaged;
	prev_damaged = damaged;

	rd.endFrame();

	if (radar_exposures == 1) {
		Paint_world_radar();
	}

	/*
	 * Now switch planes and clear the screen.
	 */
	if (p_radar != radar && radar_exposures > 0) {
		if (BIT(iniClient.instruments, SHOW_SLIDING_RADAR) == 0
			|| BIT(Setup->mode, WRAP_PLAY) == 0) {
#if defined(_UNIX) || defined(_CYGWIN)
			XCopyArea(iniClient.dpy, p_radar, radar, gc,
					  0, 0, 256, RadarHeight, 0, 0);
#else
			WinXBltPixToWin(p_radar, radar, 
							0, 0, 256, RadarHeight, 0, 0);
#endif
		} else {

			int x, y, w, h;
			float xp, yp, xo, yo;

			xp = (float) (pos.x * 256) / Setup->width;
			yp = (float) (pos.y * RadarHeight) / Setup->height;
			xo = (float) 256 / 2;
			yo = (float) RadarHeight / 2;
			if (xo <= xp) {
				x = (int) (xp - xo + 0.5);
			} else {
				x = (int) (256 + xp - xo + 0.5);
			}
			if (yo <= yp) {
				y = (int) (yp - yo + 0.5);
			} else {
				y = (int) (RadarHeight + yp - yo + 0.5);
			}
			y = RadarHeight - y - 1;
			w = 256 - x;
			h = RadarHeight - y;

#if defined(_UNIX) || defined(_CYGWIN)
			XCopyArea(iniClient.dpy, p_radar, radar, gc,
					  0, 0, x, y, w, h);
			XCopyArea(iniClient.dpy, p_radar, radar, gc,
					  x, 0, w, y, 0, h);
			XCopyArea(iniClient.dpy, p_radar, radar, gc,
					  0, y, x, h, w, 0);
			XCopyArea(iniClient.dpy, p_radar, radar, gc,
					  x, y, w, h, 0, 0);
#else
			Paint_world_radar();						  
#endif
		}
	}
	else if (radar_exposures > 2) {
		Paint_world_radar();
	}

#if defined(_UNIX) || defined(_CYGWIN)
	if (dbuf_state->type == PIXMAP_COPY) {
		XCopyArea(iniClient.dpy, p_draw, draw, gc,
				  0, 0, ext_view_width, ext_view_height, 0, 0);
	}

	dbuff_switch(dbuf_state);

	if (dbuf_state->type == COLOR_SWITCH) {
		XSetPlaneMask(iniClient.dpy, gc, dbuf_state->drawing_planes);
		XSetPlaneMask(iniClient.dpy, messageGC, dbuf_state->drawing_planes);
	}
#endif

	if (!damaged) {
		/* Prepare invisible buffer for next frame by clearing. */
		if (iniClient.useErase) {
			Erase_end();
		}
		else {
			/*
			 * DBE's XdbeBackground switch option is
			 * probably faster than XFillRectangle.
			 */
#if defined(_UNIX) || defined(_CYGWIN)
			if (dbuf_state->multibuffer_type != MULTIBUFFER_DBE) {
				SET_FG(colors[BLACK].pixel);
				XFillRectangle(iniClient.dpy, p_draw, gc, 0, 0, draw_width, draw_height);
			}
#endif
		}
	}

#if defined(_UNIX) || defined(_CYGWIN)
	if (talk_mapped == true) {
		static bool toggle;
		static long last_toggled;

		if (loops >= last_toggled + FPS / 2 || loops < last_toggled) {
			toggle = (toggle == false) ? true : false;
			last_toggled = loops;
		}
		Talk_cursor(toggle);
	}
#endif

#if defined(_WINDOWS) && !defined(_CYGWIN)
	Client_score_table();
	PaintWinClient();
#endif

	Paint_clock(0);

	XFlush(iniClient.dpy);
}


///////////////////////////////////////////////////////////////////////////////
#define SCORE_BORDER			6
static void Paint_score_background(int thisLine)
{
	if (!iniClient.blockBitmaps) {
		XClearWindow(iniClient.dpy, players);
	} else {
		XSetForeground(iniClient.dpy, scoreListGC, colors[BLACK].pixel);

		IFWINDOWS( XFillRectangle(iniClient.dpy, players, scoreListGC, 
								  0, 0, 
								  players_width, BG_IMAGE_HEIGHT); )

		PaintBitmap(players, BM_SCORE_BG,
					0, 0,
					players_width, BG_IMAGE_HEIGHT,
					0);

		if (players_height > BG_IMAGE_HEIGHT + LOGO_HEIGHT) {
			XFillRectangle(iniClient.dpy, players, scoreListGC, 
						   0, BG_IMAGE_HEIGHT, 
						   players_width,
						   players_height - (BG_IMAGE_HEIGHT + LOGO_HEIGHT));
		}
		PaintBitmap(players, BM_LOGO, 
					0, players_height - LOGO_HEIGHT, 
					players_width, LOGO_HEIGHT,
					0);

		XFlush(iniClient.dpy);
	}

}


///////////////////////////////////////////////////////////////////////////////
void Paint_score_start(void)
{
	char		headingStr[MSG_LEN];
	static int thisLine;

	thisLine = SCORE_BORDER + scoreListFont->ascent;

	if (scoreTableType == STName) {
		strlcpy(headingStr, "NICK=USER@HOST", sizeof(headingStr));
	} else if (scoreTableType == STIPVer) {
		strlcpy(headingStr, "NICK=IP:VER", sizeof(headingStr));
	} else {
		strlcpy(headingStr, "  ", sizeof(headingStr));
		if (BIT(Setup->mode, TIMING)) {
			if (version >= 0x3261) {
				strcat(headingStr, "LAP ");
			}
		}
		if (BIT(Setup->mode, TEAM_PLAY)) {
			strlcpy(headingStr, "TM", sizeof(headingStr));
		} else {
			strlcpy(headingStr, "AL", sizeof(headingStr));
		}
		
		if (scoreTableType == STKills)
			strcat(headingStr, " KILL DETH RATIO ");
		else if (scoreTableType == STRank)
			strcat(headingStr, " RANK   RATE ");
		else
			strcat(headingStr, "   SCORE  ");
		if (BIT(Setup->mode, LIMITED_LIVES)) {
			strlcat(headingStr, "LIFE", sizeof(headingStr));
		}
		strlcat(headingStr, " NAME", sizeof(headingStr));
	}
	Paint_score_background(thisLine);

	ShadowDrawString(iniClient.dpy, players, scoreListGC,
					 SCORE_BORDER, thisLine,
					 headingStr,
					 colors[WHITE].pixel,
					 colors[BLACK].pixel);

	gcv.line_style = LineSolid;
	XChangeGC(iniClient.dpy, scoreListGC, GCLineStyle, &gcv);
	XDrawLine(iniClient.dpy, players, scoreListGC,
			  SCORE_BORDER, thisLine,
			  players_width - SCORE_BORDER, thisLine);

	gcv.line_style = LineOnOffDash;
	XChangeGC(iniClient.dpy, scoreListGC, GCLineStyle, &gcv);

	Paint_clock(1);
}

///////////////////////////////////////////////////////////////////////////////
void Paint_score_entry(int entry_num,
					   Other* other,
					   bool best)
{
	static char 	raceStr[8], teamStr[4], lifeStr[8], label[MSG_LEN];
	static int		lineSpacing = -1, firstLine;
	int 			thisLine;
	//char			scoreStr[16];
	String			scoreStr;

	/*
	 * First time we're here, set up miscellaneous strings for
	 * efficiency and calculate some other constants.
	 */
	if (lineSpacing == -1) {
		memset(raceStr, '\0', sizeof raceStr);
		memset(teamStr, '\0', sizeof teamStr);
		memset(lifeStr, '\0', sizeof lifeStr);
		teamStr[1] = ' ';
		raceStr[2] = ' ';

		lineSpacing
			= scoreListFont->ascent + scoreListFont->descent + 3;
		firstLine
			= 2*SCORE_BORDER + scoreListFont->ascent + lineSpacing;
	}
	thisLine = firstLine + lineSpacing * entry_num;

	/*
	 * Setup the status line
	 */
	if (scoreTableType == STName) {
		sprintf(label, "%s=%s@%s", (PCSTR)other->name, (PCSTR)other->real, (PCSTR)other->host);
	} else if (scoreTableType == STIPVer) {
		sprintf(label, "%s=%s", (PCSTR)other->name, (PCSTR)other->ipVer);
	} else {
		//D(xpprintf("id=%d War id = %d\n", other->id, other->warId);)
		Other*		war = Other_by_id(other->warId);

		if (BIT(Setup->mode, TIMING)) {
			raceStr[0] = ' ';
			raceStr[1] = ' ';
			if (version >= 0x3261) {
				if ((other->mychar == ' ' || other->mychar == 'R')
					&& other->round + other->check > 0) {
					if (other->round > 99) {
						sprintf(raceStr, "%3d", other->round);
					}
					else {
						sprintf(raceStr, "%d.%c",
								other->round, other->check + 'a');
					}
				}
			}
		}
		if (BIT(Setup->mode, TEAM_PLAY)) {
			teamStr[0] = other->team + '0';
		} else {
			sprintf(teamStr, "%c", other->alliance);
		}

		if (scoreTableType == STKills)
		{
			scoreStr.printf("%4d %4d %5.1f", 
					other->kills, other->deaths, other->ratio);
		}
		else if (scoreTableType == STRank)
		{
			scoreStr.printf("%4d %6.2f", other->rank, other->rate);
		}
		else
		{
			if (BIT(Setup->mode, LIMITED_LIVES))
				sprintf(lifeStr, " %3d", other->life);

			if (iniClient.showScoreDecimals > 0 && version >= 0x4500) {
				scoreStr.printf("%*.*f",
						8 - iniClient.showScoreDecimals, iniClient.showScoreDecimals,
						other->score);
			}
			else {
				scoreStr.printf("%5d", (int) rint(other->score));
			}
		}
		sprintf(label, "%c %s%s%s%s  %s",
				other->mychar, raceStr, teamStr,
				(PCSTR)scoreStr, lifeStr,
				(PCSTR)other->name);
		if (war) {
			if (strlen(label) + strlen(war->name) + 5 < sizeof(label)) {
				sprintf(label + strlen(label), " (%s)", (PCSTR)war->name);
			}
		}
	}

	/*
	 * Draw the line
	 */
	if ((other->mychar == 'D'
		|| other->mychar == 'P'
		|| other->mychar == 'W')
		&& !iniClient.mono) {

		if (!iniClient.blockBitmaps) {
			XSetForeground(iniClient.dpy, scoreListGC, colors[BLACK].pixel);
		} else { 
			/*
			** hm, this grey color is pretty, but am i guaranteed that there is 
			** 16 standard colors just because blockBitmaps = true?
			*/
			XSetForeground(iniClient.dpy, scoreListGC, colors[12].pixel);
		}		
		XDrawString(iniClient.dpy, players, scoreListGC,
					SCORE_BORDER, thisLine,
					label, strlen(label));
	} else {
		ShadowDrawString(iniClient.dpy, players, scoreListGC,
						 SCORE_BORDER, thisLine,
						 label,
						 colors[WHITE].pixel,
						 colors[BLACK].pixel);
	}

	/*
	 * Underline the best player
	 */
	if (best) {
		XDrawLine(iniClient.dpy, players, scoreListGC,
				  SCORE_BORDER, thisLine,
				  players_width - SCORE_BORDER, thisLine);
	}
}

///////////////////////////////////////////////////////////////////////////////
static void Paint_clock(int redraw)
{
	int 				minute,
						hour,
						height = scoreListFont->ascent + scoreListFont->descent
								+ 3,
						border = 3;
	time_t				t;
	struct tm			*m;
	char				buf[16];
	static long 		prev_loops;
	static int			width;

	if (BIT(iniClient.instruments, SHOW_CLOCK) == 0) {
		if (width != 0) {
			XSetForeground(iniClient.dpy, scoreListGC, colors[windowColor].pixel);
			XFillRectangle(iniClient.dpy, players, scoreListGC,
						   256 - (width + 2 * border), 0,
						   width + 2 * border, height);
			width = 0;
		}
		return;
	}
	if (redraw == 0
		&& loops > prev_loops
		&& loops - prev_loops < (FPS << 5)) {
		return;
	}
	prev_loops = loops;
	time(&t);
	m = localtime(&t);

	/* round seconds up to next minute. */
	minute = m->tm_min;
	hour = m->tm_hour;
	if (minute++ == 59) {
		minute = 0;
		if (hour++ == 23) {
			hour = 0;
		}
	}
	if (!BIT(iniClient.instruments, SHOW_CLOCK_AMPM_FORMAT)) {
		sprintf(buf, "%02d:%02d", hour, minute);
	} else {
		char tmpchar = 'A';
		/* strftime(buf, sizeof(buf), "%l:%M%p", m); */
		if (m->tm_hour > 12){
			tmpchar = 'P';
			m->tm_hour %= 12;
		}
		sprintf(buf, "%2d:%02d%cM", m->tm_hour, m->tm_min, tmpchar);
	}
	width = XTextWidth(scoreListFont, buf, strlen(buf));
	XSetForeground(iniClient.dpy, scoreListGC, colors[windowColor].pixel);
	XFillRectangle(iniClient.dpy, players, scoreListGC,
				   256 - (width + 2 * border), 0,
				   width + 2 * border, height);
	ShadowDrawString(iniClient.dpy, players, scoreListGC,
					 256 - (width + border),
					 scoreListFont->ascent + 4,
					 buf,
					 colors[WHITE].pixel,
					 colors[BLACK].pixel);
}

///////////////////////////////////////////////////////////////////////////////
void ShadowDrawString(Display* dpy, Window w, GC gc,
					  int x, int y, const char* str,
					  unsigned long fg, unsigned long bg)
{
	if (!iniClient.mono) {
		XSetForeground(dpy, gc, bg);
		XDrawString(dpy, w, gc, x+1, y+1, str, strlen(str));
		x--; y--;
	}
	XSetForeground(dpy, gc, fg);
	XDrawString(dpy, w, gc, x, y, str, strlen(str));
}


