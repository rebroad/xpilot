/* $Id: client.h,v 1.17 2006/09/24 05:00:17 dick Exp $
 *
 * client - map stuff, radar stuff, network stuff, misc stuff, globals.
 *
 * client - the user interface to the game.
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2001 by
 *
 *      Bj�rn Stabell        <bjoern@xpilot.org>
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
 * $Log: client.h,v $
 * Revision 1.17  2006/09/24 05:00:17  dick
 * scoresChanged is an int
 *
 * Revision 1.16  2005/03/17 22:12:13  kps
 * Get rid of warnings from makedepend about "non-portable whitespace".
 *
 * Revision 1.15  2004/06/03 06:04:42  dick
 * struct other_t becomes class Other.
 * array Others becomes ObjList others.
 * shipobj becomes ShipObj.
 *
 */

#ifndef CLIENT_H
#define CLIENT_H

#if defined(_WINDOWS) && !defined(_CYGWIN)
#	ifndef	_WINSOCKAPI_
#		include <winsock.h>
#	endif

#	ifndef	_WINX_H_
#		include "NT/winX.h"
#	endif
#endif

#ifndef DRAW_H
/* need shipobj */
#include "draw.h"
#endif
#ifndef ITEM_H
/* need NUM_ITEMS */
#include "item.h"
#endif

#include "cstring.h"
#include "ScoreTable.h"

#define PACKET_LOSS		0
#define PACKET_DROP		1
#define PACKET_DRAW		2

#define MAX_SCORE_OBJECTS	10


#define FIND_NAME_WIDTH(other)						\
    if ((other)->name_width == 0) {					\
	(other)->name_len = strlen((other)->name);			\
	(other)->name_width = 2 + XTextWidth(gameFont, (other)->name,	\
					 (other)->name_len);		\
    }

class Other;
#if 0
typedef struct {
    char		name[MAX_CHARS];
    char		real[MAX_CHARS];
    char		host[MAX_CHARS];
    short		id;
    short		warId;
    short		team;
    short		life;
	short		kills;
	short		deaths;
	DFLOAT		kdratio;	// kills/deaths
    DFLOAT		ratio;
    DFLOAT		score;
	DFLOAT		rate;
	int			rank;
	char		ipVer[MAX_CHARS];
    short		check;
    short		round;
    short		timing;
    long		timing_loops;
    short		mychar;
	short		alliance;
    short		name_width;	/* In pixels */
    short		name_len;	/* In bytes */
    shipobj*	ship;
} other_t;
#endif

typedef struct {
    int		pos;		/* Block index */
    long	fuel;		/* Amount of fuel available */
} fuelstation_t;

typedef struct {
    int		pos;		/* Block index */
    short	id,		/* Id of owner or -1 */
		team;		/* Team this base belongs to */
} homebase_t;

typedef struct {
    int		pos;		/* Block index */
    short	dead_time,	/* Frames inactive */
		dot;		/* Draw dot if inactive */
} cannontime_t;

typedef struct {
    int			pos;		/* Block index */
    short		dead_time;	/* Frames inactive */
    unsigned short	damage;		/* Damage to target */
} target_t;

typedef struct {
    int		pos;		/* Block index */
} checkpoint_t;

#define SCORE_OBJECT_COUNT	100
typedef struct {
    DFLOAT	score;
	int		x;
	int		y;
	int		count;
	int		hud_msg_len;
	int		hud_msg_width;
	int		msg_width;
	int		msg_len;
    char	msg[10];
	char	hud_msg[MAX_CHARS+10];
} score_object_t;


/*
 * is a selection pending (in progress), done, drawn emphasized?
 */
#define SEL_NONE       (1 << 0)
#define SEL_PENDING    (1 << 1)
#define SEL_SELECTED   (1 << 2)
#define SEL_EMPHASIZED (1 << 3)

/*
 * a selection (text, string indices, state,...)
 */
typedef struct {
    /* a selection in the talk window */
    struct {
        bool    state;	/* current state of the selection */
        int     x1;	/* string indices */
        int     x2;
        bool    incl_nl;/* include a `\n'? */
    } talk ;
    /* a selection in the draw window */
    struct {
        bool    state;
        int     x1;	/* string indices (for TalkMsg[].txt) */
        int     x2;	/* they are modified when the emphasized area */
        int     y1;	/* is scrolled down by new messages coming in */
        int     y2;
    } draw;
    char	*txt;   /* allocated when needed */
    int		txt_size;	/* size of txt buffer */
    int		len;
    /* when a message `jumps' from talk window to the player messages: */
    bool	keep_emphasizing;
} selection_t;

class Connectparam;

extern ipos	pos;
extern ipos	vel;
extern ipos	world;
extern ipos	realWorld;
extern short	heading;
extern short	nextCheckPoint;
extern u_byte	numItems[NUM_ITEMS];
extern u_byte	lastNumItems[NUM_ITEMS];
extern int	numItemsTime[NUM_ITEMS];
//extern DFLOAT	showItemsTime;
extern short	autopilotLight;


extern short	lock_id;		/* Id of player locked onto */
extern short	lock_dir;		/* Direction of lock */
extern short	lock_dist;		/* Distance to player locked onto */

extern short	selfVisible;		/* Are we alive and playing? */
extern short	damaged;		/* Damaged by ECM */
extern short	destruct;		/* If self destructing */
extern short	shutdown_delay;
extern short	shutdown_count;
extern short	thrusttime;
extern short	thrusttimemax;
extern short	shieldtime;
extern short	shieldtimemax;
extern short	phasingtime;
extern short	phasingtimemax;

extern int		roundDelay;
extern int		roundDelayMax;

extern int	RadarWidth;
extern int	RadarHeight;
extern long	control_count;		/* Display control for how long? */
//extern u_byte	spark_rand;		/* Sparkling effect */
extern u_byte	old_spark_rand;		/* previous value of spark_rand */

extern long	fuelSum;		/* Sum of fuel in all tanks */
extern long	fuelMax;		/* How much fuel can you take? */
extern short	fuelCurrent;		/* Number of currently used tank */
extern short	numTanks;		/* Number of tanks */
extern long	fuelCount;		/* Display fuel for how long? */
extern DFLOAT	displayedPower;		/* What the server is sending us */
extern DFLOAT	displayedTurnspeed;	/* What the server is sending us */
extern DFLOAT	displayedTurnresistance;/* What the server is sending us */
//extern DFLOAT	spark_prob;		/* Sparkling effect configurable */
//extern int	charsPerSecond;		/* Message output speed (config) */

//extern DFLOAT	hud_move_fact;		/* scale the hud-movement (speed) */
//extern DFLOAT	ptr_move_fact;		/* scale the speed pointer length */
extern char	mods[MAX_CHARS];	/* Current modifiers in effect */
//extern long	instruments;		/* Instruments on screen (bitmask) */
extern int	packet_size;		/* Current frame update packet size */
extern int	packet_loss;		/* lost packets per second */
extern int	packet_drop;		/* dropped packets per second */
extern int	packet_lag;			/* approximate lag in frames */
extern char	*packet_measure;	/* packet measurement in a second */
extern long	packet_loop;		/* start of measurement */

enum ScoreTableType;
extern uint				scoreTablePages;
extern ScoreTableType	scoreTableType;
extern char	servername[MAX_CHARS];	/* Name of server connecting to */
extern unsigned	version;		/* Version of the server */
extern int	scoresChanged;
extern int	shields;		/* When shields are considered up */
extern bool	initialPointerControl;	/* Start by using mouse for control? */
extern bool	pointerControl;		/* current state of mouse ship flying */

extern int 	oldMaxFPS;

extern u_byte	lose_item;		/* flag and index to drop item */
extern int	lose_item_active;	/* one of the lose keys is pressed */

//extern int	maxLinesInHistory;	/* number of lines to save in history */
#define MAX_HIST_MSGS	128		/* maximum */

int Fuel_by_pos(int x, int y);
int Target_alive(int x, int y, int *damage);
int Target_by_index(int ind, int *xp, int *yp, int *dead_time, int *damage);
int Handle_fuel(int ind, int fuel);
int Cannon_dead_time_by_pos(int x, int y, int *dot);
int Handle_cannon(int ind, int dead_time);
int Handle_target(int num, int dead_time, int damage);
int Base_info_by_pos(int x, int y, int *id, int *team);
int Handle_base(int id, int ind);
int Check_pos_by_index(int ind, int *xp, int *yp);
int Check_index_by_pos(int x, int y);
Other*		Other_by_id(int id);
ShipObj*	Ship_by_id(int id);
int Handle_leave(int id);
int Handle_player(int id, int team, int mychar, char *player_name,
		  char *real_name, char *host_name, char *shape);
int Handle_score(int id, DFLOAT score, int life, int mychar, int alliance,
				 short kills, short deaths);
int HandleScoreTablePages(uint pages);
int	HandleScoreTableIPVer(int id, PCSTR s);
int	HandleScoreTableRank(int id, int rank, DFLOAT rate);
int Handle_score_object(DFLOAT score, int x, int y, char *msg);
int	Handle_team_score(int team, DFLOAT score);
int Handle_timing(int id, int check, int round);
int Handle_war(int robot_id, int killer_id);
int Handle_seek(int programmer_id, int robot_id, int sought_id);
void Map_dots(void);
void Map_restore(int startx, int starty, int width, int height);
void Map_blue(int startx, int starty, int width, int height);
void Client_score_table(void);
//int Client_init(char *server, unsigned server_version);
int ClientInit(Connectparam* conpar);
int Client_setup(void);
void Client_cleanup(void);
int Client_start(void);
int Client_fps_request(void);
int Client_power(void);
int Client_fd(void);
int Client_input(int);
void Client_flush(void);
void Client_sync(void);
int Client_wrap_mode(void);
void Reset_shields(void);
void Set_toggle_shield(bool onoff);
void Set_auto_shield(bool onoff);

#ifdef XlibSpecificationRelease
void Key_event(XEvent *event);
#endif
#if defined (_UNIX) || defined(_CYGWIN)
int x_event(int);
#else
int win_xevent(XEvent event);
void MarkPlayersForRedraw(void);
void MarkMotdForRedraw(Window w);
#endif

int Key_init(void);
int Key_update(void);
int Check_client_fps(void);

#endif

