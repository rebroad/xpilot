/* $Id: xinit.cpp,v 1.16 2007/02/17 06:18:15 dick Exp $
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
 * $Log: xinit.cpp,v $
 * Revision 1.16  2007/02/17 06:18:15  dick
 * client/Audio becomes common/AudioMan.
 *
 * Revision 1.15  2007/01/20 19:26:17  dick
 * Use uppercase on Menu strings
 *
 * Revision 1.14  2007/01/09 21:35:19  dick
 * Add support for a 'Mute' button on the menu.
 *
 * Revision 1.13  2004/05/20 22:23:42  dick
 * Add AddMessage(PCSTR, ...).  Wrap some debug with a D().
 *
 * Revision 1.12  2004/04/27 01:54:13  dick
 * Display urls in the message list as underlined.
 * Open a browser if the url is clicked on.
 *
 * Revision 1.11  2004/04/26 07:00:52  dick
 * Create XPilotMenu creates the pulldown
 *
 * Revision 1.10  2004/04/25 05:47:07  dick
 * Retab
 *
 * Revision 1.9  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.8  2004/01/06 04:48:27  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 * Revision 1.7  2002/09/02 21:06:06  dick
 * Encapsulate dpy and kdpy.  Singleton ini becomes iniClient.
 *
 * Revision 1.6  2002/09/02 15:42:40  dick
 * Singleton ini becomes iniClient.
 *
 * Revision 1.5  2002/07/19 17:26:34  dick
 * Pass String as (PCSTR) through ...  (ini.name became a String)
 *
 * Revision 1.4  2001/09/11 09:08:31  dick
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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#if defined(_UNIX) || defined(_CYGWIN)
# include <unistd.h>
# include <X11/Xlib.h>
# include <X11/Xos.h>
# include <X11/Xutil.h>
#else
# include "../common/NT/winX.h"
# include "NT/winclient.h"
# include "NT/winXXPilot.h"
#endif

#include "version.h"
#include "config.h"
#include "const.h"
#include "keys.h"
#include "icon.h"
#include "paint.h"
#include "xinit.h"
#include "widget.h"
#include "configure.h"
#include "error.h"
#include "netclient.h"
#include "dbuff.h"
#include "protoclient.h"
#include "portability.h"
#include "Ini.h"
#include "ScoreTable.h"

#ifdef	SOUND
#include "AudioMan.h"

static PCSTR s_Mute   = "MUTE";
static PCSTR s_Unmute = "UNMUTE";
#endif

/*
 * Item structures.
 *
 * If you add an item here then please make sure you also add
 * the item in the proper place in ../replay/xp-replay.c.
 */
#if defined(_WINDOWS) && !defined(_CYGWIN)
#pragma warning(disable : 4305)
#endif
#include "items/itemRocketPack.xbm"
#include "items/itemCloakingDevice.xbm"
#include "items/itemEnergyPack.xbm"
#include "items/itemWideangleShot.xbm"
#include "items/itemRearShot.xbm"
#include "items/itemMinePack.xbm"
#include "items/itemSensorPack.xbm"
#include "items/itemTank.xbm"
#include "items/itemEcm.xbm"
#include "items/itemArmor.xbm"
#include "items/itemAfterburner.xbm"
#include "items/itemTransporter.xbm"
#include "items/itemDeflector.xbm"
#include "items/itemHyperJump.xbm"
#include "items/itemPhasingDevice.xbm"
#include "items/itemMirror.xbm"
#include "items/itemLaser.xbm"
#include "items/itemEmergencyThrust.xbm"
#include "items/itemTractorBeam.xbm"
#include "items/itemAutopilot.xbm"
#include "items/itemEmergencyShield.xbm"

char xinit_version[] = VERSION;


/* How far away objects should be placed from each other etc... */
#define BORDER			10
#define BTN_BORDER		4

/* Information window dimensions */
#define ABOUT_WINDOW_WIDTH	600
#define ABOUT_WINDOW_HEIGHT	700

extern char		*HistMsg[];
extern int		RadarHeight;

/*
 * Globals.
 */
int			ButtonHeight;
Atom		ProtocolAtom, KillAtom;
int			buttonColor, windowColor, borderColor;
int			quitting = false;
int			top_width, top_height, top_x, top_y, top_posmask;
int			draw_width, draw_height;
int			players_width, players_height;
char		*geometry;
Cursor		pointerControlCursor;
//char		sparkColors[MSG_LEN];
int			spark_color[MAX_COLORS];
int			num_spark_colors;
int			ignoreWindowManager;

static message_t	*MsgBlock = NULL;
static message_t	*MsgBlock_pending = NULL;


/*
 * NB!  Is dependent on the order of the items in item.h!
 */
static struct {
	unsigned char*		data;
	const char* 		keysText;
} itemBitmapData[NUM_ITEMS] = {
	{
		itemEnergyPack_bits,
		"Extra energy/fuel"
	},
	{
		itemWideangleShot_bits,
		"Extra front cannons"
	},
	{
		itemRearShot_bits,
		"Extra rear cannon"
	},
	{
		itemAfterburner_bits,
		"Afterburner; makes your engines more powerful"
	},
	{
		itemCloakingDevice_bits,
		"Cloaking device; "
		"makes you almost invisible, both on radar and on screen"
	},
	{
		itemSensorPack_bits,
		"Sensor; "
		"enables you to see cloaked opponents more easily"
	},
	{
		itemTransporter_bits,
		"Transporter; enables you to steal equipment from "
		"other players"
	},
	{
		itemTank_bits,
		"Tank; "
		"makes refueling quicker, increases maximum fuel "
		"capacity and can be jettisoned to confuse enemies"
	},
	{
		itemMinePack_bits,
		"Mine; "
		"can be dropped as a bomb or as a stationary mine"
	},
	{
		itemRocketPack_bits,
		"Rocket; can be utilized as smart missile, "
		"heatseeking missile, nuclear missile or just a "
		"plain unguided missile (torpedo)"
	},
	{
		itemEcm_bits,
		"ECM (Electronic Counter Measures); "
		"can be used to disturb electronic equipment, for instance "
		"can it be used to confuse smart missiles and reprogram "
		"robots to seek certain players"
	},
	{
		itemLaser_bits,
		"Laser; "
		"limited range laser beam, costs a lot of fuel, "
		"having more laser items increases the range of the laser, "
		"they can be irrepairably damaged by ECMs"
	},
	{
		itemEmergencyThrust_bits,
		"Emergency Thrust; "
		"gives emergency thrust capabilities for a limited period"
	},
	{	itemTractorBeam_bits,
		"Tractor Beam; "
		"gives mutual attractive force to currently locked on ship, "
		"this means the heavier your ship, the less likely you will move "
		"when being tractored or using a tractor"
	},
	{
		itemAutopilot_bits,
		"Autopilot; "
		"when on, the ship will turn and thrust against the "
		"direction of travel"
	},
	{
		itemEmergencyShield_bits,
		"EmergencyShield; "
		"gives emergency shield capabilities for a limited period"
	},
	{
		itemDeflector_bits,
		"Deflector; "
		"pushes hostile objects away from your ship"
	},
	{
		itemHyperJump_bits,
		"Hyperjump; "
		"enables you to teleport to a random map location"
	},
	{
		itemPhasingDevice_bits,
		"Phasing Device; "
		"lets you fly through anything for a limited period"
	},
	{
		itemMirror_bits,
		"Mirror; "
		"reflects laser beams"
	},
	{
		itemArmor_bits,
		"Armor; "
		"absorbs shots in the absence of shields"
	},
};
#if defined(_WINDOWS) && !defined(_CYGWIN)
Pixmap	itemBitmaps[NUM_ITEMS][2];		/* Bitmaps for the items in 2 colors */
#else
Pixmap	itemBitmaps[NUM_ITEMS]; 		/* Bitmaps for the items */
#endif

char dashes[NUM_DASHES];
char cdashes[NUM_CDASHES];

static int Mute_callback(int, void *, const char **);
static int Quit_callback(int, void *, const char **);
static int Config_callback(int, void *, const char **);
static int Score_callback(int, void *, const char **);
static int Player_callback(int, void *, const char **);

static int button_form;
static int menu_button = NO_WIDGET;

const char *Item_get_text(int i)
{
	return itemBitmapData[i].keysText;
}

/*
 * Set specified font for that GC.
 * Return font that is used for this GC, even if setting a new
 * font failed (return default font in that case).
 */
static XFontStruct* Set_font(Display* dpy, GC gc,
							 const char* fontName,
							 const char *resName)
{
	XFontStruct*		font;

#if defined(_UNIX) || defined(_CYGWIN)
	if ((font = XLoadQueryFont(dpy, fontName)) == NULL) {
		error("Couldn't find font '%s' for %s, using default font",
			  fontName, resName);
		font = XQueryFont(dpy, XGContextFromGC(gc));
	} else
		XSetFont(dpy, gc, font->fid);
#else
		font = WinXLoadFont(fontName);
		XSetFont(dpy, gc, font->fid);
#endif

	return font;
}

/*
 * Convert a string of color numbers into an array
 * of "colors[]" indices stored by "spark_color[]".
 * Initialize "num_spark_colors".
 */
static void Init_spark_colors(void)
{
	char				buf[MSG_LEN];
	char				*src, *dst;
	unsigned			col;
	int 				i;

	num_spark_colors = 0;
	/*
	 * The sparkColors specification may contain
	 * any possible separator.	Only look at numbers.
	 */

	 /* hack but protocol will allow max 9 (MM) */ 
	for (src = iniClient.sparkColors; *src && (num_spark_colors < 9); src++) {
		if (isascii(*src) && isdigit(*src)) {
			dst = &buf[0];
			do {
				*dst++ = *src++;
			} while (*src &&
					 isascii(*src) &&
					 isdigit(*src) &&
					 ((dst - buf) < (sizeof(buf) - 1)));
			*dst = '\0';
			src--;
			if (sscanf(buf, "%u", &col) == 1) {
				if (col < (unsigned)iniClient.maxColors) {
					spark_color[num_spark_colors++] = col;
				}
			}
		}
	}
	if (num_spark_colors == 0) {
		if (iniClient.maxColors <= 8) {
			/* 3 colors ranging from 5 up to 7 */
			for (i = 5; i < iniClient.maxColors; i++) {
				spark_color[num_spark_colors++] = i;
			}
		}
		else {
			/* 7 colors ranging from 5 till 11 */
			for (i = 5; i < 12; i++) {
				spark_color[num_spark_colors++] = i;
			}
		}
		/* default spark colors always include RED. */
		spark_color[num_spark_colors++] = RED;
	}
	for (i = num_spark_colors; i < MAX_COLORS; i++) {
		spark_color[i] = spark_color[num_spark_colors - 1];
	}
}

/*
 * Initialize miscellaneous window hints and properties.
 */
#if defined(_UNIX) || defined(_CYGWIN)
extern char 			**Argv;
extern int				Argc;

static void Init_disp_prop(Display *d, Window win,
						   int w, int h, int x, int y,
						   int flags)
{
	XClassHint			xclh;
	XWMHints			xwmh;
	XSizeHints			xsh;
	char				msg[256];

	xwmh.flags	   = InputHint|StateHint|IconPixmapHint;
	xwmh.input	   = True;
	xwmh.initial_state = NormalState;
	xwmh.icon_pixmap   = XCreateBitmapFromData(d, win,
											   (char *)icon_bits,
											   icon_width, icon_height);

	xsh.flags = (flags|PMinSize|PMaxSize|PBaseSize|PResizeInc);
	xsh.width = w;
	xsh.base_width =
	xsh.min_width = MIN_TOP_WIDTH;
	xsh.max_width = MAX_TOP_WIDTH;
	xsh.width_inc = 1;
	xsh.height = h;
	xsh.base_height =
	xsh.min_height = MIN_TOP_HEIGHT;
	xsh.max_height = MAX_TOP_HEIGHT;
	xsh.height_inc = 1;
	xsh.x = x;
	xsh.y = y;

	xclh.res_name = NULL;		/* NULL: Automatically uses Argv[0], */
	xclh.res_class = iniClient.myClass; /* stripped of directory prefixes. */

	/*
	 * Set the above properties.
	 */
	XSetWMProperties(d, win, NULL, NULL, Argv, Argc,
					 &xsh, &xwmh, &xclh);

	/*
	 * Now initialize icon and window title name.
	 */
	if (iniClient.titleFlip)
		sprintf(msg, "Successful connection to server at \"%s\".",
				servername);
	else
		sprintf(msg, "%s -- Server at \"%s\".", TITLE, servername);
	XStoreName(d, win, msg);

	sprintf(msg, "%s:%s", (PCSTR)iniClient.name, servername);
	XSetIconName(d, win, msg);

	if (d != iniClient.dpy)
			return;

	/*
	 * Specify IO error handler and the WM_DELETE_WINDOW atom in
	 * an attempt to catch 'nasty' quits.
	 */
	ProtocolAtom = XInternAtom(d, "WM_PROTOCOLS", False);
	KillAtom = XInternAtom(d, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(d, win, &KillAtom, 1);
	XSetIOErrorHandler(FatalError);
}
#endif


/*
 * The following function initializes a toplevel window.
 * It returns 0 if the initialization was successful,
 * or -1 if it couldn't initialize the double buffering routine.
 */
int Init_top(void)
{
#if defined(_UNIX) || defined(_CYGWIN)
	int 								i;
	int 								top_x, top_y;
	int 								x, y;
	unsigned							w, h;
	int 								values;
	int 								top_flags;
	XGCValues							xgc;
	XSetWindowAttributes				sattr;
	unsigned long						mask;

	if (top) {
		error("Init_top called twice");
		exit(1);
	}

	if (Colors_init() == -1) {
		return -1;
	}

	if (iniClient.shieldDrawMode == -1) {
		iniClient.shieldDrawMode = 0;
		/*
		 * Default is solid for NCD X11 servers.  My NCD mono 19 inch
		 * terminal, vendor release 2002 suffers from terrible slowness
		 * when drawing dashed arcs with thick lines.
		 */
		if (strcmp (ServerVendor (iniClient.dpy),
					"DECWINDOWS (Compatibility String) "
					"Network Computing Devices Inc.") == 0
			&& ProtocolVersion (iniClient.dpy) == 11)
			iniClient.shieldDrawMode = 1;

		if (iniClient.useErase){
		/*
		 * The NeWS X server doesn't orrectly erase shields.
		 */
		if (!strcmp(ServerVendor(iniClient.dpy), "X11/NeWS - Sun Microsystems Inc."))
			iniClient.shieldDrawMode = 1;
		}
	}
#endif

	if (iniClient.hudColor >= iniClient.maxColors || iniClient.hudColor <= 0) {
		iniClient.hudColor = BLUE;
	}
	if (iniClient.hudLockColor >= iniClient.maxColors || iniClient.hudLockColor <= 0) {
		iniClient.hudLockColor = iniClient.hudColor;
	}
	if (iniClient.wallColor >= iniClient.maxColors || iniClient.wallColor <= 0) {
		iniClient.wallColor = BLUE;
	}
	if (iniClient.wallRadarColor >= iniClient.maxColors
		|| ((iniClient.wallRadarColor & 1) && iniClient.colorSwitch)) {
		iniClient.wallRadarColor = BLUE;
	}
	if (iniClient.targetRadarColor >= iniClient.maxColors
		|| ((iniClient.targetRadarColor & 1) && iniClient.colorSwitch)) {
		iniClient.targetRadarColor = BLUE;
	}
	if (iniClient.oldMessagesColor >= iniClient.maxColors || iniClient.oldMessagesColor < 0) {
		iniClient.oldMessagesColor = WHITE;
	}
	if (iniClient.decorColor >= iniClient.maxColors || iniClient.decorColor <= 0) {
		iniClient.decorColor = RED;
	}
	if (iniClient.decorRadarColor >= iniClient.maxColors
		|| ((iniClient.decorRadarColor & 1) && iniClient.colorSwitch)) {
		iniClient.decorRadarColor = 2;
	}

		iniClient.shieldDrawMode = iniClient.shieldDrawMode ? LineSolid : LineOnOffDash;
	radarDrawRectanglePtr = (iniClient.mono ? XDrawRectangle : XFillRectangle);

	/*
	 * Get toplevel geometry.
	 */
#if defined(_UNIX) || defined(_CYGWIN)
	top_flags = 0;
	if (geometry != NULL && geometry[0] != '\0') {
		mask = XParseGeometry(geometry, &x, &y, &w, &h);
	} else {
		mask = 0;
	}
	if ((mask & WidthValue) != 0) {
		top_width = w;
		top_flags |= USSize;
	} else {
		top_width = DEF_TOP_WIDTH;
		top_flags |= PSize;
	}
	LIMIT(top_width, MIN_TOP_WIDTH, MAX_TOP_WIDTH);
	if ((mask & HeightValue) != 0) {
		top_height = h;
		top_flags |= USSize;
	} else {
		top_height = DEF_TOP_HEIGHT;
		top_flags |= PSize;
	}
	LIMIT(top_height, MIN_TOP_HEIGHT, MAX_TOP_HEIGHT);
	if ((mask & XValue) != 0) {
		if ((mask & XNegative) != 0) {
			top_x = DisplayWidth(iniClient.dpy, DefaultScreen(iniClient.dpy)) - top_width + x;
		} else {
			top_x = x;
		}
		top_flags |= USPosition;
	} else {
		top_x = (DisplayWidth(iniClient.dpy, DefaultScreen(iniClient.dpy)) - top_width) /2;
		top_flags |= PPosition;
	}
	if ((mask & YValue) != 0) {
		if ((mask & YNegative) != 0) {
			top_y = DisplayHeight(iniClient.dpy, DefaultScreen(iniClient.dpy)) - top_height + y;
		} else {
			top_y = y;
		}
		top_flags |= USPosition;
	} else {
		top_y = (DisplayHeight(iniClient.dpy, DefaultScreen(iniClient.dpy)) - top_height) /2;
		top_flags |= PPosition;
	}
	if (geometry != NULL) {
		free(geometry);
		geometry = NULL;
	}

	/*
	 * Create toplevel window (we need this first so that we can create GCs)
	 */
	mask = 0;
	/*old debug: sattr.background_pixel = colors[WHITE].pixel;*/
	sattr.background_pixel = colors[BLACK].pixel;
	mask |= CWBackPixel;
	sattr.border_pixel = colors[WHITE].pixel;
	mask |= CWBorderPixel;
	if (colormap != 0) {
		sattr.colormap = colormap;
		mask |= CWColormap;
	}
	if (ignoreWindowManager) {
		sattr.override_redirect = True;
		mask |= CWOverrideRedirect;
	}
	top = XCreateWindow(iniClient.dpy,
						DefaultRootWindow(iniClient.dpy),
						top_x, top_y,
						top_width, top_height,
						0, dispDepth,
						InputOutput, visual,
						mask, &sattr);
	XSelectInput(iniClient.dpy, top,
				 KeyPressMask | KeyReleaseMask
				 | FocusChangeMask | StructureNotifyMask);
	Init_disp_prop(iniClient.dpy, top, top_width, top_height, top_x, top_y, top_flags);
	if (iniClient.kdpy) {
		int scr = DefaultScreen(iniClient.kdpy);
		keyboard = XCreateSimpleWindow(iniClient.kdpy,
									   DefaultRootWindow(iniClient.kdpy),
									   top_x, top_y,
									   top_width, top_height,
									   0, 0, BlackPixel(iniClient.dpy, scr));
		XSelectInput(iniClient.kdpy, keyboard,
					 KeyPressMask | KeyReleaseMask | FocusChangeMask);
		Init_disp_prop(iniClient.kdpy, keyboard, top_width, top_height,
					   top_x, top_y, top_flags);
	}
#else	/* _WINDOWS */
	/* MFC already gave us a nice top window...use it */
	{
		XRectangle	rect;
		WinXGetWindowRectangle(0, &rect);
		top_x = rect.x;
		top_y = rect.y;
		top_width = rect.width;
		top_height = rect.height;
	}
#endif	/* _WINDOWS */

#if defined(_UNIX) || defined(_CYGWIN)
	/*
	 * Create item bitmaps
	 */
	for (i = 0; i < NUM_ITEMS; i++) {
		itemBitmaps[i]
			= XCreateBitmapFromData(iniClient.dpy, top,
									(char *)itemBitmapData[i].data,
									ITEM_SIZE, ITEM_SIZE);
	}

	/*
	 * Creates and initializes the graphic contexts.
	 */
	xgc.line_width = 0;
	xgc.line_style = LineSolid;
	xgc.cap_style = CapButt;
	xgc.join_style = JoinMiter; 		/* I think this is fastest, is it? */
	xgc.graphics_exposures = False;
	values
		= GCLineWidth|GCLineStyle|GCCapStyle|GCJoinStyle|GCGraphicsExposures;

	messageGC
		= XCreateGC(iniClient.dpy, top, values, &xgc);
	radarGC
		= XCreateGC(iniClient.dpy, top, values, &xgc);
	buttonGC
		= XCreateGC(iniClient.dpy, top, values, &xgc);
	scoreListGC
		= XCreateGC(iniClient.dpy, top, values, &xgc);
	textGC
		= XCreateGC(iniClient.dpy, top, values, &xgc);
	talkGC
		= XCreateGC(iniClient.dpy, top, values, &xgc);
	motdGC
		= XCreateGC(iniClient.dpy, top, values, &xgc);
	gc
		= XCreateGC(iniClient.dpy, top, values, &xgc);
	XSetBackground(iniClient.dpy, gc, colors[BLACK].pixel);

	/*
	 * Set fonts
	 */
	gameFont
		= Set_font(iniClient.dpy, gc, iniClient.gameFontName, "gameFont");
	messageFont
		= Set_font(iniClient.dpy, messageGC, iniClient.messageFontName, "messageFont");
	scoreListFont
		= Set_font(iniClient.dpy, scoreListGC, iniClient.scoreListFontName, "scoreListFont");
	buttonFont
		= Set_font(iniClient.dpy, buttonGC, iniClient.buttonFontName, "buttonFont");
	textFont
		= Set_font(iniClient.dpy, textGC, iniClient.textFontName, "textFont");
	talkFont
		= Set_font(iniClient.dpy, talkGC, iniClient.talkFontName, "talkFont");
	motdFont
		= Set_font(iniClient.dpy, motdGC, iniClient.motdFontName, "motdFont");

	XSetState(iniClient.dpy, gc,
			  WhitePixel(iniClient.dpy, DefaultScreen(iniClient.dpy)),
			  BlackPixel(iniClient.dpy, DefaultScreen(iniClient.dpy)),
			  GXcopy, AllPlanes);
	XSetState(iniClient.dpy, radarGC,
			  WhitePixel(iniClient.dpy, DefaultScreen(iniClient.dpy)),
			  BlackPixel(iniClient.dpy, DefaultScreen(iniClient.dpy)),
			  GXcopy, AllPlanes);
	XSetState(iniClient.dpy, messageGC,
			  WhitePixel(iniClient.dpy, DefaultScreen(iniClient.dpy)),
			  BlackPixel(iniClient.dpy, DefaultScreen(iniClient.dpy)),
			  GXcopy, AllPlanes);
	XSetState(iniClient.dpy, buttonGC,
			  WhitePixel(iniClient.dpy, DefaultScreen(iniClient.dpy)),
			  BlackPixel(iniClient.dpy, DefaultScreen(iniClient.dpy)),
			  GXcopy, AllPlanes);
	XSetState(iniClient.dpy, scoreListGC,
			  WhitePixel(iniClient.dpy, DefaultScreen(iniClient.dpy)),
			  BlackPixel(iniClient.dpy, DefaultScreen(iniClient.dpy)),
			  GXcopy, AllPlanes);

	if (dbuf_state->type == COLOR_SWITCH) {
		XSetPlaneMask(iniClient.dpy, gc, dbuf_state->drawing_planes);
	}

#endif

	if (iniClient.mono) {
		buttonColor = BLACK;
		windowColor = BLACK;
		borderColor = WHITE;
	} else {
		windowColor = BLUE;
		buttonColor = RED;
		borderColor = WHITE;
	}

	return 0;
}


/*
 * Creates the playing windows.
 * Returns 0 on success, -1 on error.
 */
int Init_playing_windows(void)
{
#if defined(_UNIX) || defined(_CYGWIN)
	unsigned					w, h;
	Pixmap						pix;
	GC							cursorGC;
#else
	int 						i;
#endif

	if (!top) {
		if (Init_top()) {
			return -1;
		}
	}

	Scale_dashes();

	draw_width = top_width - (256 + 2);
	draw_height = top_height;
#if defined(_WINDOWS) && !defined(_CYGWIN)
	/*
	 * What follows is poor code.  WinX needs to know beforehand if its
	 * dealing with draw because it might want to create 2 bitmaps for it.
	 * Since i know draw is the first window created (after top), i can cheat it.
	 */
	draw = 1;
#endif
	draw = XCreateSimpleWindow(iniClient.dpy, top, 258, 0,
							   draw_width, draw_height,
							   0, 0, colors[BLACK].pixel);
	IFWINDOWS( if (draw != 1) error("draw != 1"); )
	XSelectInput(iniClient.dpy, draw, ButtonPressMask | ButtonReleaseMask);

	radar = XCreateSimpleWindow(iniClient.dpy, top, 0, 0,
							256, RadarHeight, 0, 0,
							colors[BLACK].pixel);

#if defined(_WINDOWS) && !defined(_CYGWIN)
//	WinXSetEventMask(draw, NoEventMask);
	radar_exposures = 1;
	radarGC = WinXCreateWinDC(radar);
	gc = WinXCreateWinDC(draw);

	textWindow = XCreateSimpleWindow(iniClient.dpy, top, 0, 0,
								0, 0, 0, 0,
								colors[BLACK].pixel);
	textGC = WinXCreateWinDC(textWindow);

	msgWindow = XCreateSimpleWindow(iniClient.dpy, top, 0, 0,
								0, 0, 0, 0,
								colors[BLACK].pixel);
	messageGC = WinXCreateWinDC(msgWindow);
	motdGC = WinXCreateWinDC(top);

	for (i = 0; i < MAX_COLORS; i++) {
		colors[i].pixel = i;
	}
	players_exposed = 1;
	/* p_radar = XCreatePixmap(iniClient.dpy, radar, 256, RadarHeight, dispDepth); */
	s_radar = XCreatePixmap(iniClient.dpy, radar, 256, RadarHeight, dispDepth);
	/*
	 * Create item bitmaps AFTER the windows
	 */
	WinXCreateItemBitmaps();
	/* create the fonts AFTER the windows */
	gameFont
		= Set_font(iniClient.dpy, gc, iniClient.gameFontName, "gameFont");
	messageFont
		= Set_font(iniClient.dpy, messageGC, iniClient.messageFontName, "messageFont");
	textFont
		= Set_font(iniClient.dpy, textGC, iniClient.textFontName, "textFont");
	motdFont
		= Set_font(iniClient.dpy, motdGC, iniClient.motdFontName, "motdFont");

	buttonWindow = XCreateSimpleWindow(iniClient.dpy, top, 0, 0,
								0, 0, 0, 0,
								colors[BLACK].pixel);
	buttonGC = WinXCreateWinDC(buttonWindow);
	buttonFont
		= Set_font(iniClient.dpy, buttonGC, iniClient.buttonFontName, "buttonFont");
#endif

	/* Create buttons */
#define BUTTON_WIDTH	84
	ButtonHeight = buttonFont->ascent + buttonFont->descent + 2*BTN_BORDER;

	button_form
		= Widget_create_form(0, top,
							 0, RadarHeight,
							 256, ButtonHeight + 2,
							 0);
	Widget_create_activate(button_form,
						   0 + 0*BUTTON_WIDTH, 0,
						   BUTTON_WIDTH, ButtonHeight,
						   1, "QUIT",
						   Quit_callback, NULL);
	Widget_create_activate(button_form,
						   1 + 1*BUTTON_WIDTH, 0,
						   BUTTON_WIDTH, ButtonHeight,
						   1, "ABOUT",
						   About_callback, NULL);
	CreateXPilotMenu();
	Widget_map_sub(button_form);

	/* Create score list window */
	players_width = RadarWidth;
	players_height = top_height - (RadarHeight + ButtonHeight + 2);
	players
		= XCreateSimpleWindow(iniClient.dpy, top,
							  0, RadarHeight + ButtonHeight + 2,
							  players_width, players_height,
							  0, 0,
							  colors[windowColor].pixel);
#if defined(_WINDOWS) && !defined(_CYGWIN)
	scoreListGC = WinXCreateWinDC(players);
	scoreListFont
		= Set_font(iniClient.dpy, scoreListGC, iniClient.scoreListFontName, "scoreListFont");
#endif

	/*
	 * Selecting the events we can handle.
	 */
	XSelectInput(iniClient.dpy, radar, ExposureMask);
	XSelectInput(iniClient.dpy, players, ExposureMask);
#if defined(_UNIX) || defined(_CYGWIN)
	if (!iniClient.selectionAndHistory) {
		XSelectInput(iniClient.dpy, draw, 0);
	} else {
		XSelectInput(iniClient.dpy, draw, ButtonPressMask | ButtonReleaseMask);
	}


	/*
	 * Initialize misc. pixmaps if we're not color switching.
	 * (This could be in dbuff_init_buffer completely IMHO, -- Metalite)
	 */
	switch (dbuf_state->type) {

	case PIXMAP_COPY:
		p_radar = XCreatePixmap(iniClient.dpy, radar, 256, RadarHeight, dispDepth);
		s_radar = XCreatePixmap(iniClient.dpy, radar, 256, RadarHeight, dispDepth);
		p_draw	= XCreatePixmap(iniClient.dpy, draw, draw_width, draw_height, dispDepth);
		break;

	case MULTIBUFFER:
		p_radar = XCreatePixmap(iniClient.dpy, radar, 256, RadarHeight, dispDepth);
		s_radar = XCreatePixmap(iniClient.dpy, radar, 256, RadarHeight, dispDepth);
		dbuff_init_buffer(dbuf_state);
		break;

	case COLOR_SWITCH:
		s_radar = radar;
		p_radar = radar;
		p_draw = draw;
		Paint_sliding_radar();
		break;
	}

	XAutoRepeatOff(iniClient.dpy);		/* We don't want any autofire, yet! */
	if (iniClient.kdpy) {
		XAutoRepeatOff(iniClient.kdpy);
	}

	/*
	 * Define a blank cursor for use with pointer control
	 */
	XQueryBestCursor(iniClient.dpy, draw, 1, 1, &w, &h);
	pix = XCreatePixmap(iniClient.dpy, draw, w, h, 1);
	cursorGC = XCreateGC(iniClient.dpy, pix, 0, NULL);
	XSetForeground(iniClient.dpy, cursorGC, 0);
	XFillRectangle(iniClient.dpy, pix, cursorGC, 0, 0, w, h);
	XFreeGC(iniClient.dpy, cursorGC);
	pointerControlCursor = XCreatePixmapCursor(iniClient.dpy, pix, pix, &colors[BLACK],
											   &colors[BLACK], 0, 0);
	XFreePixmap(iniClient.dpy, pix);

	/*
	 * Maps the windows, makes the visible. Voila!
	 */
	XMapSubwindows(iniClient.dpy, top);
	XMapWindow(iniClient.dpy, top);
	XSync(iniClient.dpy, False);

	if (iniClient.kdpy) {
		XMapWindow(iniClient.kdpy, keyboard);
		XSync(iniClient.kdpy, False);
	}
#else
		/* WinXSetEvent(players, WM_PAINT, WinXPaintPlayers); */
		pointerControlCursor = !None;
#endif

	Init_spark_colors();

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
struct ScoreMenu {
	PCSTR	title;
	uint	type;
};
ScoreMenu	scoreMenus[] = {
	{	"SCORE",	STScore,},
	{	"PLAYER",	STName},
	{	"KILLS",	STKills},
	{	"IP-VER",	STIPVer},
	{	"RANK",		STRank}
};

void CreateXPilotMenu()
{
	if (menu_button)
	{
		Widget_destroy_children(menu_button);
	}
	else
		menu_button
			= Widget_create_menu(button_form,
								 2 + 2*BUTTON_WIDTH, 0,
								 BUTTON_WIDTH, ButtonHeight,
								 1, "MENU");
#ifdef	SOUND
	Widget_add_pulldown_entry(menu_button,
							  s_Mute, Mute_callback, NULL);
#endif
	Widget_add_pulldown_entry(menu_button,
							  "MOTD", Motd_callback, NULL);
	Widget_add_pulldown_entry(menu_button,
							  "KEYS", Keys_callback, NULL);
	Widget_add_pulldown_entry(menu_button,
							  "CONFIG", Config_callback, NULL);
//	Widget_add_pulldown_entry(menu_button,
//							  "SCORE", Score_callback, NULL);
//	Widget_add_pulldown_entry(menu_button,
//							  "PLAYER", Player_callback, NULL);

	ScoreTableType	st;
	for (st=STFirst; st<STMAX; st=(ScoreTableType)(st+1))
	{
		if (scoreTablePages & 1<<scoreMenus[st].type)
			Widget_add_pulldown_entry(menu_button,
									  scoreMenus[st].title, 
									  Score_callback, 
									  (void*)scoreMenus[st].type);
	}
	Widget_map_sub(button_form);
}

#if defined(_WINDOWS) && !defined(_CYGWIN)
void WinXCreateItemBitmaps()
{
	int 				i;

	for (i = 0; i < NUM_ITEMS; i++) {
		itemBitmaps[i][ITEM_HUD]
			= WinXCreateBitmapFromData(iniClient.dpy, draw,
									   (char *)itemBitmapData[i].data,
									   ITEM_SIZE, ITEM_SIZE, colors[iniClient.hudColor].pixel);
		itemBitmaps[i][ITEM_PLAYFIELD]
			= WinXCreateBitmapFromData(iniClient.dpy, draw,
									   (char *)itemBitmapData[i].data,
									   ITEM_SIZE, ITEM_SIZE, colors[RED].pixel);
	}
	Colors_init_block_bitmaps();
	
}
#endif

int Alloc_msgs(void)
{
	message_t			*x, *x2 = 0;
	int 				i;

	if ((x = (message_t *)malloc(2 * MAX_MSGS * sizeof(message_t))) == NULL){
		error("No memory for messages");
		return -1;
	}

#if defined(_UNIX) || defined(_CYGWIN)
	if (iniClient.selectionAndHistory &&
		((x2 = (message_t *)malloc(2 * MAX_MSGS * sizeof(message_t))) == NULL)){
		error("No memory for history messages");
		free(x);
		return -1;
	}
	if (iniClient.selectionAndHistory) {
		MsgBlock_pending		= x2;
	}
#endif

	MsgBlock			= x;

	for (i = 0; i < 2 * MAX_MSGS; i++) {
		if (i < MAX_MSGS) {
			TalkMsg[i] = x;
			IFNWINDOWS( if (iniClient.selectionAndHistory) TalkMsg_pending[i] = x2; )
		} else {
			GameMsg[i - MAX_MSGS] = x;
			IFNWINDOWS( if (iniClient.selectionAndHistory) GameMsg_pending[i - MAX_MSGS] = x2; )
		}
		x->txt[0] = '\0';
		x->len = 0;
		x->life = 0;
		x++;

#if defined(_UNIX) || defined(_CYGWIN)
		if (iniClient.selectionAndHistory) {
			x2->txt[0] = '\0';
			x2->len = 0;
			x2->life = 0;
			x2++;
		}
#endif
	}
	return 0;
}

void Free_msgs(void)
{
	if (MsgBlock) {
		free(MsgBlock);
		MsgBlock = NULL;
	}

#if defined(_UNIX) || defined(_CYGWIN)
	if (MsgBlock_pending) {
		free(MsgBlock_pending);
		MsgBlock_pending = NULL;
	}
#endif
}


static int Config_callback(int widget_desc, void *data, const char **str)
{
	Config(true);
	return 0;
}


static int Score_callback(int widget_desc, void *data, const char **str)
{
	ScoreTableType st = (ScoreTableType)(int)data;
	D(xpprintf("Score_callback: st = %d\n ", st);)
	Config(false);
	if (scoreTableType != st) {
		scoreTableType =  st;
		scoresChanged = 1;
	}
	return 0;
}


static int Player_callback(int widget_desc, void *data, const char **str)
{
	Config(false);
	if (scoreTableType != STName) {
		scoreTableType =  STName;
		scoresChanged = 1;
	}
	return 0;
}


static int Quit_callback(int widget_desc, void *data, const char **str)
{
	quitting = true;
	return 0;
}

#ifdef	SOUND
static int Mute_callback(int widget_desc, void *data, const char **str)
{
	audioMan.SetMute(!audioMan.GetMute());
	if (audioMan.GetMute())
		*str = s_Unmute;
	else
		*str = s_Mute;
	return 0;
}
#endif

void Resize(Window w, int width, int height)
{
	if (w != top) {
		return;
	}
	/* ignore illegal resizes */
	LIMIT(width, MIN_TOP_WIDTH, MAX_TOP_WIDTH);
	LIMIT(height, MIN_TOP_HEIGHT, MAX_TOP_HEIGHT);
	if (width == top_width && height == top_height) {
		return;
	}
	top_width = width;
	top_height = height;
	if (!draw) {
		return;
	}
	draw_width = top_width - 258;
	draw_height = top_height;
	Send_display();
	Net_flush();
	XResizeWindow(iniClient.dpy, draw, draw_width, draw_height);
#if defined(_UNIX) || defined(_CYGWIN)
	if (dbuf_state->type == PIXMAP_COPY) {
		XFreePixmap(iniClient.dpy, p_draw);
		p_draw = XCreatePixmap(iniClient.dpy, draw, draw_width, draw_height, dispDepth);
	}
#endif
	players_height = top_height - (RadarHeight + ButtonHeight + 2);
	XResizeWindow(iniClient.dpy, players,
				  players_width, players_height);
#if defined(_WINDOWS) && !defined(_CYGWIN)
	WinXResize();
#endif
	Talk_resize();
	Config_resize();
}


/*
 * Cleanup player structure, close the display etc.
 */
void Quit(void)
{
#if defined(_UNIX) || defined(_CYGWIN)
	if (iniClient.dpy != NULL) {
		XAutoRepeatOn(iniClient.dpy);
		Colors_cleanup();
		XCloseDisplay(iniClient.dpy);
		iniClient.dpy = NULL;
		if (iniClient.kdpy) {
			XAutoRepeatOn(iniClient.kdpy);
			XCloseDisplay(iniClient.kdpy);
			iniClient.kdpy = NULL;
		}
	}
#else
	if (button_form) {
		Widget_destroy(button_form);
		button_form = 0;
	}
#endif
	Free_msgs();
	Widget_cleanup();
}


int FatalError(Display *dpy)
{
	Net_cleanup();
	/*
	 * Quit(&client);
	 * It's already a fatal I/O error, nothing to cleanup.
	 */
	exit(0);
	return(0);
}

void Scale_dashes()
{
	dashes[0] = WINSCALE(8);
	dashes[1] = WINSCALE(4);

	cdashes[0] = WINSCALE(3);
	cdashes[1] = WINSCALE(9);

	XSetDashes(iniClient.dpy, gc, 0, dashes, NUM_DASHES);
}
