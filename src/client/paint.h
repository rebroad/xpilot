/* $Id: paint.h,v 1.20 2007/01/17 21:35:15 dick Exp $
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
 * $Log: paint.h,v $
 * Revision 1.20  2007/01/17 21:35:15  dick
 * Encapsulate all of the RobotWatch features into a RobotWatchMan object.
 *
 * Revision 1.19  2007/01/17 08:59:49  dick
 * RobotWatch is a list of Strings sent from the client when a player is paused
 * and watching a robot.  This list contains diagnostic information about
 * what the heck the robot thinks it's doing.
 * It's kinda like the Terminator view where he's looking at a 6502 dump.
 *
 * Revision 1.18  2004/06/03 06:04:42  dick
 * struct other_t becomes class Other.
 * array Others becomes ObjList others.
 * shipobj becomes ShipObj.
 *
 * Revision 1.17  2004/05/28 18:03:16  dick
 * MAX_MSGS goes global and now lives in Ini.
 *
 */

#ifndef PAINT_H
#define PAINT_H


/* includes begin */
#ifndef TYPES_H
/* need u_byte */
#include "types.h"
#endif
#ifndef CLIENT_H
/* need other_t */
#include "client.h"
#endif

#include "IniClient.h"
#include "Obj.h"

/* includes end */


/* constants begin */

#define NUM_DASHES				2
#define NUM_CDASHES 			2
#define DASHES_LENGTH			12

#define HUD_SIZE				90		/* Size/2 of HUD lines */
#define HUD_OFFSET				20		/* Hud line offset */
#define FUEL_GAUGE_OFFSET		6
#define HUD_FUEL_GAUGE_SIZE 	(2*(HUD_SIZE-HUD_OFFSET-FUEL_GAUGE_OFFSET))
#define FUEL_NOTIFY 			(3*theWorld.GetFPS())


#define WARNING_DISTANCE		(VISIBILITY_DISTANCE*0.8)
/* constants end */


/* macros begin */
#define X(co)	((int) ((co) - world.x))
#define Y(co)	((int) (world.y + ext_view_height - (co)))
/* macros end */


/* typedefs begin */
typedef struct {
	char	txt[MSG_LEN];
	short	len;
	short	pixelLen;
	int 	life;
} message_t;
/* typedefs end */


/* which index a message actually has (consider SHOW_REVERSE_SCROLL) */
#define TALK_MSG_SCREENPOS(_total,_pos) \
	(BIT(iniClient.instruments, SHOW_REVERSE_SCROLL)?(_total)-(_pos):(_pos))

/* how to draw a selection */
#define DRAW_EMPHASIZED 		BLUE

/*
 * Global objects.
 */

extern char 	dashes[NUM_DASHES];
extern char 	cdashes[NUM_CDASHES];

/* The fonts used in the game */
extern XFontStruct* gameFont;
extern XFontStruct* messageFont;
extern XFontStruct* scoreListFont;
extern XFontStruct* buttonFont;
extern XFontStruct* textFont;
extern XFontStruct* talkFont;
extern XFontStruct* motdFont;

extern short	about_page; 			/* Which page is the player on? */
extern bool 	players_exposed;		/* Is score window exposed? */
extern int		radar_exposures;		/* Is radar window exposed? */

										/* windows has 2 sets of item bitmaps */
#define ITEM_HUD		0				/* one color for the HUD */
#define ITEM_PLAYFIELD	1				/* and one color for the playfield */
#if defined(_WINDOWS) && !defined(_CYGWIN)
extern Pixmap	itemBitmaps[][2];
#else
extern Pixmap	itemBitmaps[];
#endif

extern GC		gc, messageGC, radarGC, buttonGC, scoreListGC, textGC, talkGC;
extern GC		motdGC;
extern XGCValues gcv;
extern Window	top, draw, keyboard, radar, players;
#if defined(_WINDOWS) && !defined(_CYGWIN)
extern Window	textWindow, msgWindow, buttonWindow;
#endif
extern Pixmap	p_draw; 				/* Drawing area pixmap */
extern Pixmap	p_radar;				/* Radar drawing pixmap */
extern Pixmap	s_radar;				/* Second radar drawing pixmap */
extern long 	dpl_1[2];				/* Used by radar hack */
extern long 	dpl_2[2];				/* Used by radar hack */
extern Window	about_w;				/* The About window */
extern Window	about_close_b;			/* About close button */
extern Window	about_next_b;			/* About next page button */
extern Window	about_prev_b;			/* About prev page button */
extern Window	talk_w; 				/* Talk window */
extern XColor	colors[MAX_COLORS]; 	/* Colors */
extern Colormap colormap;				/* Private colormap */
extern bool 	gotFocus;				/* Do we have the mouse pointer */
extern bool 	talk_mapped;			/* Is talk window visible */
extern short	ext_view_width; 		/* Width of extended visible area */
extern short	ext_view_height;		/* Height of extended visible area */
extern int		active_view_width;		/* Width of active map area displayed. */
extern int		active_view_height; 	/* Height of active map area displayed. */
extern int		ext_view_x_offset;		/* Offset of ext_view_width */
extern int		ext_view_y_offset;		/* Offset of ext_view_height */
extern u_byte	debris_colors;			/* Number of debris intensities */
extern DFLOAT	charsPerTick;			/* Output speed of messages */

extern message_t*	TalkMsg[];
extern message_t*	GameMsg[];
extern message_t*	TalkMsg_pending[];
extern message_t*	GameMsg_pending[];

extern Rect 					urlRects[MAX_MSGS];

extern int		(*radarDrawRectanglePtr)		/* Function to draw player on radar */
				(Display *disp, Drawable d, GC gc,
				 int x, int y, unsigned width, unsigned height);

extern long 	loops;

extern short	scaleArray[];
extern void 	Init_scale_array(void);
#define WINSCALE(__n)	((__n) >= 0 ? scaleArray[(__n)] : -scaleArray[-(__n)])

/*
 * Prototypes from the paint*.c files.
 */

void AddMessage(PCSTR message, ...);
int Handle_start(long server_loops);
int Handle_end(long server_loops);
int Handle_self(int x, int y, int vx, int vy, int dir,
	float power, float turnspeed, float turnresistance,
	int lock_id, int lock_dist, int lock_dir,
	int nextCheckPoint, int autopilotLight,
	u_byte *newNumItems,
	int currentTank, int fuel_sum, int fuel_max, int packet_size);
int Handle_self_items(u_byte *newNumItems);
int Handle_modifiers(char *m);
int Handle_damaged(int damaged);
int Handle_destruct(int count);
int Handle_shutdown(int count, int delay);
int Handle_thrusttime(int count, int max);
int Handle_shieldtime(int count, int max);
int Handle_phasingtime(int count, int max);
int Handle_rounddelay(int count, int max);
int Handle_refuel(int x0, int y0, int x1, int y1);
int Handle_connector(int x0, int y0, int x1, int y1, int tractor);
int Handle_laser(int color, int x, int y, int len, int dir);
int Handle_missile(int x, int y, int dir, int len);
int Handle_ball(int x, int y, int id);
int Handle_ship(int x, int y, int id, int dir, int shield, int cloak, int eshield, int phased, int deflector);
int Handle_mine(int x, int y, int teammine, int id);
int Handle_item(int x, int y, int type);
int Handle_fastshot(int type, u_byte *p, int n);
int Handle_debris(int type, u_byte *p, int n);
int Handle_wreckage(int x, int y, int wrecktype, int size, int rotation);
int Handle_asteroid(int x, int y, int type, int size, int rotation);
int Handle_wormhole(int x, int y);
int Handle_ecm(int x, int y, int size);
int Handle_trans(int x1, int y1, int x2, int y2);
int Handle_paused(int x, int y, int count);
int Handle_radar(int x, int y, int size);
int Handle_vcannon(int x, int y, int type);
int Handle_vfuel(int x, int y, long fuel);
int Handle_vbase(int x, int y, int xi, int yi, int type);
int Handle_vdecor(int x, int y, int xi, int yi, int type);
int Handle_message(char* msg);
int	HandleRobotWatch(int y, char* msg);
int Handle_eyes(int watcherId, int watchedId);
void Paint_item_symbol(u_byte type, Drawable d, GC mygc, int x, int y, int color);
void Paint_item(u_byte type, Drawable d, GC mygc, int x, int y);
void Paint_shots(void);
void Paint_ships(void);
void Paint_radar(void);
void Paint_sliding_radar(void);
void Paint_world_radar(void);
void Paint_radar_block(int, int, int);
void Paint_vcannon(void);
void Paint_vfuel(void);
void Paint_vbase(void);
void Paint_vdecor(void);
void Paint_world(void);
void Paint_score_entry(int entry_num, Other* other, bool best);
void Paint_score_start();
void Paint_score_objects();
void Paint_meters(void);
void Paint_HUD(void);
int  Get_message(int* pos, char * message, int req_length, int key );
void Paint_messages(void);
void Add_pending_messages(void);
void Paint_recording(void);
void Paint_frame(void);
int Handle_time_left(long sec);
void Game_over_action(u_byte stat);

/*
 * Prototype from blockbitmaps.c
 */
int Block_bitmaps_create(void);

#endif
