/* $Id: Frame.cpp,v 1.28 2007/02/17 20:29:18 dick Exp $
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2002 by
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
 *
 */
/*
 *  $Log: Frame.cpp,v $
 *  Revision 1.28  2007/02/17 20:29:18  dick
 *  Wrap Trace(...) debug with D().
 *
 *  Revision 1.27  2007/02/12 07:55:27  dick
 *  Support RobotWatchDeco, which is decorated shapes displayed on the playfield.
 *
 *  Revision 1.26  2007/01/17 21:37:13  dick
 *  Retab
 *
 *  Revision 1.25  2007/01/17 09:06:22  dick
 *  Send the RobotWatch packets to the client
 *
 *  Revision 1.24  2007/01/16 04:31:35  dick
 *  Programmable frame reduction for paused players.
 *
 *  Revision 1.23  2004/05/22 15:15:08  dick
 *  myEyes becomes a id instead of an ind
 *
 *  Revision 1.22  2004/05/20 22:12:38  dick
 *  Attribute showLocks displays in the ScoreTable who everyone is locked on.
 *
 *  Revision 1.21  2004/05/04 21:07:17  dick
 *  Need randommnt.h
 *
 *  Revision 1.20  2004/04/29 16:08:27  dick
 *  compile with SOUND enabled.
 *
 *  Revision 1.19  2004/02/18 01:22:14  dick
 *  Remove extraneous and obsolete #include "netserver.h"
 *
 *  Revision 1.18  2004/02/03 03:39:54  dick
 *  Add Windows precompiled header support.
 *
 *  Revision 1.17  2004/01/12 05:25:45  dick
 *  whitespace.
 *
 *  Revision 1.16  2002/09/11 16:42:03  dick
 *  Here endeth the server encapsulation task.
 *  theWorld is now an array of World(s) and get loaded dynamically.
 *
 *  Revision 1.15  2002/09/01 00:13:45  dick
 *  Remove obsolete windows #include "winServer.h"
 *
 *  Revision 1.14  2002/08/01 14:36:58  dick
 *  view_width becomes viewWidth, etc.
 *
 *  Revision 1.13  2002/06/02 06:14:28  dick
 *  whitespace.
 *
 *  Revision 1.12  2002/06/01 06:06:47  dick
 *  Encapsulate (almost) everything.  Get rid of (almost) all refs to theWorld.
 *
 *  Revision 1.11  2002/05/22 05:09:00  dick
 *  Update to XPilot r4_5_4alpha.
 *
 *  Revision 1.10  2002/05/20 02:17:45  dick
 *  Windows needs errno and seterrno in portability.h.
 *  Windows also needs rint() defined if not using fltk.
 *
 *  Revision 1.9  2002/05/18 20:55:38  dick
 *  Update to XPilot-4.5.1
 *
 *  Revision 1.8  2001/08/14 21:37:14  dick
 *  Array Obj[] becomes objs[].  We now have a class called Obj.
 *
 *  Revision 1.7  2001/08/11 00:22:24  dick
 *  Fix shots and debris.
 *
 *  Revision 1.6  2001/08/10 08:11:38  dick
 *  New class ConnectionPlayer subclasses Connection to provide player
 *  functions.  New class Frame subclasses ConnectionPlayer which provides
 *  Frame update functionality.
 *
 *  Revision 1.5  2001/08/09 07:43:34  dick
 *  New class Connection
 *
 *  Revision 1.4  2001/07/24 09:04:00  dick
 *  Remove all global options.  Use World.options instead.
 *
 *  Revision 1.3  2001/07/23 21:08:36  dick
 *  Encapsulate ShipMass, ShotsMax, ShotsLife, ShotsMass, FPS.
 *
 *  Revision 1.2  2001/07/07 12:00:42  dick
 *  Rename classes to C++ "Style".  old World becomes theWorld.
 *
 */
#include "StdAfx.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <time.h>
#include <limits.h>
#include <sys/types.h>

#ifndef _WINDOWS
# include <unistd.h>
# include <sys/param.h>
#endif

#include "version.h"
#include "config.h"
#include "serverconst.h"
#include "global.h"
#include "proto.h"
#include "bit.h"
#include "saudio.h"
#include "error.h"
#include "commonproto.h"
#include "Frame.h"
#include "portability.h"
#include "Cannon.h"
#include "randommt.h"
#include "Robot.h"

char frame_version[] = VERSION;


#define MAX_SHUFFLE_INDEX	65535
#define MAX_VISIBLE_OBJECTS	maxVisibleObject


typedef unsigned short shuffle_t;

/*
 * Structure for calculating if a pixel is visible by a player.
 * The following always holds:
 *		(world.x >= realWorld.x && world.y >= realWorld.y)
 */
typedef struct {
	position	world;					/* Lower left hand corner is this */
										/* world coordinate */
	position	realWorld;				/* If the player is on the edge of
										   the screen, these are the world
										   coordinates before adjustment... */
} pixel_visibility_t;

/*
 * Structure with player position info measured in blocks instead of pixels.
 * Used for map state info updating.
 */
typedef struct {
	ipos				world;
	ipos				realWorld;
} block_visibility_t;

typedef struct {
	unsigned char		x, y;
} debris_t;

typedef struct {
	short				x, y, size;
} radar_t;


extern time_t			gameOverTime;
long					frame_loops = 1;
static long 			last_frame_shuffle;
static shuffle_t		*object_shuffle_ptr;
static int				num_object_shuffle;
static int				max_object_shuffle;
static shuffle_t		*player_shuffle_ptr;
static int				num_player_shuffle;
static int				max_player_shuffle;
static radar_t			*radar_ptr;
static int				num_radar, max_radar;

static pixel_visibility_t pv;
//static int			view_width;
//static int			view_height
static int				horizontal_blocks;
static int				vertical_blocks;
static int				debris_x_areas;
static int				debris_y_areas;
static int				debris_areas;
//static int			debris_colors;
//static int			spark_rand;
static debris_t 		*debris_ptr[DEBRIS_TYPES];
static unsigned 		debris_num[DEBRIS_TYPES],
						debris_max[DEBRIS_TYPES];
static debris_t 		*fastshot_ptr[DEBRIS_TYPES * 2];
static unsigned 		fastshot_num[DEBRIS_TYPES * 2],
						fastshot_max[DEBRIS_TYPES * 2];

/*
 * Macro to make room in a given dynamic array for new elements.
 * P is the pointer to the array memory.
 * N is the current number of elements in the array.
 * M is the current size of the array.
 * T is the type of the elements.
 * E is the number of new elements to store in the array.
 * The goal is to keep the number of malloc/realloc calls low
 * while not wasting too much memory because of over-allocation.
 */
#define EXPAND(P,N,M,T,E)												\
	if ((N) + (E) > (M)) {												\
		if ((M) <= 0) { 												\
			M = (E) + 2;												\
			P = (T *) malloc((M) * sizeof(T));							\
			N = 0;														\
		} else {														\
			M = ((M) << 1) + (E);										\
			P = (T *) realloc(P, (M) * sizeof(T));						\
		}																\
		if (P == NULL) {												\
			error("No memory"); 										\
			N = M = 0;													\
			return; 	/* ! */ 										\
		}																\
	}

#define inview(x_, y_)															\
	(	(	((x_) > pv.world.x && (x_) < pv.world.x + viewWidth)				\
		 || ((x_) > pv.realWorld.x && (x_) < pv.realWorld.x + viewWidth))		\
	 && (	((y_) > pv.world.y && (y_) < pv.world.y + viewHeight)				\
		 || ((y_) > pv.realWorld.y && (y_) < pv.realWorld.y + viewHeight)))

static int block_inview(block_visibility_t *bv, int x, int y)
{
	return ((x > bv->world.x && x < bv->world.x + horizontal_blocks)
			|| (x > bv->realWorld.x && x < bv->realWorld.x + horizontal_blocks))
		&& ((y > bv->world.y && y < bv->world.y + vertical_blocks)
			|| (y > bv->realWorld.y && y < bv->realWorld.y + vertical_blocks));
}
#define DEBRIS_STORE(xd,yd,color,offset) \
	int 				i;												  \
	if (xd < 0) {														  \
		xd += world->width; 											  \
	}																	  \
	if (yd < 0) {														  \
		yd += world->height;											  \
	}																	  \
	if ((unsigned) xd >= (unsigned)viewWidth || (unsigned) yd >= (unsigned)viewHeight) {		  \
		/*																  \
		 * There's some rounding error or so somewhere. 				  \
		 * Should be possible to resolve it.							  \
		 */ 															  \
		return; 														  \
	}																	  \
																		  \
	i = offset + color * debris_areas									  \
		+ (((yd >> 8) % debris_y_areas) * debris_x_areas)				  \
		+ ((xd >> 8) % debris_x_areas); 								  \
																		  \
	if (num_ >= 255) {													  \
		return; 														  \
	}																	  \
	if (num_ >= max_) { 												  \
		if (num_ == 0) {												  \
			ptr_ = (debris_t *) malloc((max_ = 16) * sizeof(*ptr_));	  \
		} else {														  \
			ptr_ = (debris_t *) realloc(ptr_, (max_ += max_) * sizeof(*ptr_)); \
		}																  \
		if (ptr_ == 0) {												  \
			error("No memory for debris");								  \
			num_ = 0;													  \
			return; 													  \
		}																  \
	}																	  \
	ptr_[num_].x = (unsigned char) xd;									  \
	ptr_[num_].y = (unsigned char) yd;									  \
	num_++;

void Frame::FastShotStore(int xf, int yf, int color, int offset)
{
#define ptr_			(fastshot_ptr[i])
#define num_			(fastshot_num[i])
#define max_			(fastshot_max[i])
	DEBRIS_STORE(xf, yf, color, offset);
#undef ptr_
#undef num_
#undef max_
}

void Frame::DebrisStore(int xf, int yf, int color)
{
#define ptr_			(debris_ptr[i])
#define num_			(debris_num[i])
#define max_			(debris_max[i])
	DEBRIS_STORE(xf, yf, color, 0);
#undef ptr_
#undef num_
#undef max_
}


void Frame::FastShotEnd()
{
	int 				i;

	for (i = 0; i < DEBRIS_TYPES * 2; i++) {
		if (fastshot_num[i] != 0) {
			SendFastshot(i,
						  (unsigned char *) fastshot_ptr[i],
						  fastshot_num[i]);
			fastshot_num[i] = 0;
		}
	}
}

void Frame::DebrisEnd()
{
	int 				i;
	for (i = 0; i < DEBRIS_TYPES; i++) {
		if (debris_num[i] != 0) {
			SendDebris(i,
						(unsigned char *) debris_ptr[i],
						debris_num[i]);
			debris_num[i] = 0;
		}
	}
}

static void Frame_radar_buffer_reset(void)
{
	num_radar = 0;
}

static void Frame_radar_buffer_add(int x, int y, int s)
{
	radar_t 			*p;

	EXPAND(radar_ptr, num_radar, max_radar, radar_t, 1);
	p = &radar_ptr[num_radar++];
	p->x = x;
	p->y = y;
	p->size = s;
}

void Frame::RadarBufferSend()
{
	int 				i;
	int 				dest;
	int 				tmp;
	radar_t 			*p;
	const int			radar_width = 256;
	int 				radar_height = (radar_width * world->blockHeight) / world->blockWidth;
	int 				radar_x;
	int 				radar_y;
	int 				send_x;
	int 				send_y;
	shuffle_t			*radar_shuffle;
	size_t				shuffle_bufsize;

	if (num_radar > MIN(256, MAX_SHUFFLE_INDEX)) {
		num_radar = MIN(256, MAX_SHUFFLE_INDEX);
	}
	shuffle_bufsize = (num_radar * sizeof(shuffle_t));
	radar_shuffle = (shuffle_t *) malloc(shuffle_bufsize);
	if (radar_shuffle == (shuffle_t *) NULL) {
		return;
	}
	for (i = 0; i < num_radar; i++) {
		radar_shuffle[i] = i;
	}
	/* permute. */
	for (i = 0; i < num_radar; i++) {
		dest = (int)(rfrac() * num_radar);
		tmp = radar_shuffle[i];
		radar_shuffle[i] = radar_shuffle[dest];
		radar_shuffle[dest] = tmp;
	}

	if (version <= 0x4400) {
		for (i = 0; i < num_radar; i++) {
			p = &radar_ptr[radar_shuffle[i]];
			radar_x = (radar_width * p->x) / world->width;
			radar_y = (radar_height * p->y) / world->height;
			send_x = (world->width * radar_x) / radar_width;
			send_y = (world->height * radar_y) / radar_height;
			SendRadar(send_x, send_y, p->size);
		}
	}
	else {
		unsigned char buf[3*256];
		int buf_index = 0;
		int fast_count = 0;

		if (num_radar > 256) {
			num_radar = 256;
		}
		for (i = 0; i < num_radar; i++) {
			p = &radar_ptr[radar_shuffle[i]];
			radar_x = (radar_width * p->x) / world->width;
			radar_y = (radar_height * p->y) / world->height;
			if (radar_y >= 1024) {
				continue;
			}
			buf[buf_index++] = (unsigned char)(radar_x);
			buf[buf_index++] = (unsigned char)(radar_y & 0xFF);
			buf[buf_index] = (unsigned char)((radar_y >> 2) & 0xC0);
			if (p->size & 0x80) {
				buf[buf_index] |= (unsigned char)(0x20);
			}
			buf[buf_index] |= (unsigned char)(p->size & 0x07);
			buf_index++;
			fast_count++;
		}
		if (fast_count > 0) {
			SendFastRadar(buf, fast_count);
		}
	}

	free(radar_shuffle);
}

static void Frame_radar_buffer_free(void)
{
	free(radar_ptr);
	radar_ptr = NULL;
	num_radar = 0;
	max_radar = 0;
}


/*
 * Fast conversion of `num' into `str' starting at position `i', returns
 * index of character after converted number.
 */
static int num2str(int num, char *str, int i)
{
	int digits, t;

	if (num < 0) {
		str[i++] = '-';
		num = -num;
	}
	if (num < 10) {
		str[i++] = '0' + num;
		return i;
	}
	for (t = num, digits = 0; t; t /= 10, digits++)
		;
	for (t = i+digits-1; t >= 0; t--) {
		str[t] = num % 10;
		num /= 10;
	}
	return i + digits;
}

int Frame::Status(int ind)
{
	static char mods[MAX_CHARS];
	Player* 			pl = world->players[ind];

	int 				n;
		int 					lock_ind;
		int 					lock_id = NO_ID;
		int 					lock_dist = 0;
		int 					lock_dir = 0;
		int 					i;
		int 					showautopilot;

	/*
	 * Don't make lock visible during this frame if;
	 * 0) we are not player locked or compass is not on.
	 * 1) we have limited visibility and the player is out of range.
	 * 2) the player is invisible and he's not in our team.
	 * 3) he's not actively playing.
	 * 4) we have blind mode and he's not on the visible screen.
	 * 5) his distance is zero.
	 */

	CLR_BIT(pl->lock.tagged, LOCK_VISIBLE);
	if (BIT(pl->lock.tagged, LOCK_PLAYER) && BIT(pl->used, HAS_COMPASS)) {
		lock_id = pl->lock.pl_id;
		lock_ind = world->getInd[lock_id];

		if ((!BIT(world->rules->mode, LIMITED_VISIBILITY)
			 || pl->lock.distance <= pl->sensor_range)
#ifndef SHOW_CLOAKERS_RANGE
			&& (pl->visibility[lock_ind].canSee || OWNS_TANK(world, ind, lock_ind))
#endif
			&& BIT(world->players[lock_ind]->status, PLAYING|GAME_OVER) == PLAYING
			&& (world->options.playersOnRadar->GetBool()
				|| inview(world->players[lock_ind]->pos.x, 
								  world->players[lock_ind]->pos.y))
			&& pl->lock.distance != 0) {
			SET_BIT(pl->lock.tagged, LOCK_VISIBLE);
			lock_dir = (int)world->WrapFindDir((int)(world->players[lock_ind]->pos.x - pl->pos.x),
									(int)(world->players[lock_ind]->pos.y - pl->pos.y));
			lock_dist = (int)pl->lock.distance;
		}
	}

	if (BIT(pl->status, HOVERPAUSE))
		showautopilot = (pl->count <= 0 || (frame_loops % 8) < 4);
	else if (BIT(pl->used, HAS_AUTOPILOT))
		showautopilot = (frame_loops % 8) < 4;
	else
		showautopilot = 0;

	/*
	 * Don't forget to modify Receive_modifier_bank() in netserver.c
	 */
	i = 0;
	if (BIT(pl->mods.nuclear, FULLNUCLEAR))
		mods[i++] = 'F';
	if (BIT(pl->mods.nuclear, NUCLEAR))
		mods[i++] = 'N';
	if (BIT(pl->mods.warhead, CLUSTER))
		mods[i++] = 'C';
	if (BIT(pl->mods.warhead, IMPLOSION))
		mods[i++] = 'I';
	if (pl->mods.velocity) {
		if (i) mods[i++] = ' ';
		mods[i++] = 'V';
		i = num2str (pl->mods.velocity, mods, i);
	}
	if (pl->mods.mini) {
		if (i) mods[i++] = ' ';
		mods[i++] = 'X';
		i = num2str (pl->mods.mini + 1, mods, i);
	}
	if (pl->mods.spread) {
		if (i) mods[i++] = ' ';
		mods[i++] = 'Z';
		i = num2str (pl->mods.spread, mods, i);
	}
	if (pl->mods.power) {
		if (i) mods[i++] = ' ';
		mods[i++] = 'B';
		i = num2str (pl->mods.power, mods, i);
	}
	if (pl->mods.laser) {
		if (i) mods[i++] = ' ';
		mods[i++] = 'L';
		mods[i++] = (BIT(pl->mods.laser, STUN) ? 'S' : 'B');
	}
	mods[i] = '\0';
	n = SendSelf(
				  pl,
				  lock_id,
				  lock_dist,
				  lock_dir,
				  showautopilot,
				  world->players[world->getInd[id]]->status,  mods);
	if (n <= 0) {
		return 0;
	}

	if (BIT(pl->used, HAS_EMERGENCY_THRUST))
		SendThrustTime(pl->emergency_thrust_left,
								   pl->emergency_thrust_max);
	if (BIT(pl->used, HAS_EMERGENCY_SHIELD))
		SendShieldTime(
						pl->emergency_shield_left,
						pl->emergency_shield_max);
	if (BIT(pl->status, SELF_DESTRUCT) && pl->count > 0) {
		SendDestruct(pl->count);
	}
	if (BIT(pl->used, HAS_PHASING_DEVICE))
		SendPhasingTime(pl->phasing_left,
										pl->phasing_max);
	if (world->shutdownServer != -1) {
		SendShutdown(world->shutdownServer, world->shutdownDelay);
	}

	if (world->roundDelaySend > 0) {
		SendRoundDelay(world->roundDelay, 
				world->options.roundDelay->GetInt() * world->GetFPS());
	}

	return 1;
}

///////////////////////////////////////////////////////////////////////////////
void Frame::Map(int ind)
{
	Player* 	pl = world->players[ind];
	int 		i;
	int 		k;
	int 		x;
	int 		y;
	int 		conn_bit = (1 << myInd);
	block_visibility_t	bv;
	const int	fuel_packet_size = 5;
	const int	cannon_packet_size = 5;
	const int	target_packet_size = 7;
	const int	wormhole_packet_size = 5;
		int 	bytes_left = 2000;
	int 		max_packet;
	int 		packet_count;

	x = pl->pos.bx;
	y = pl->pos.by;
	bv.world.x = x - (horizontal_blocks >> 1);
	bv.world.y = y - (vertical_blocks >> 1);
	bv.realWorld = bv.world;
	if (BIT(world->rules->mode, WRAP_PLAY)) {
		if (bv.world.x < 0 && bv.world.x + horizontal_blocks < world->blockWidth) {
			bv.world.x += world->blockWidth;
		}
		else if (bv.world.x > 0 && bv.world.x + horizontal_blocks > world->blockWidth) {
			bv.realWorld.x -= world->blockWidth;
		}
		if (bv.world.y < 0 && bv.world.y + vertical_blocks < world->blockHeight) {
			bv.world.y += world->blockHeight;
		}
		else if (bv.world.y > 0 && bv.world.y + vertical_blocks > world->blockHeight) {
			bv.realWorld.y -= world->blockHeight;
		}
	}

	packet_count = 0;
	max_packet = MAX(5, bytes_left / target_packet_size);
	i = MAX(0, pl->last_target_update);
	for (k = 0; k < world->numTargets; k++) {
		Target* targ;
		if (++i >= world->numTargets) {
			i = 0;
		}
		targ = &world->targets[i];
		if (BIT(targ->update_mask, conn_bit)
			|| (BIT(targ->conn_mask, conn_bit) == 0
			&& block_inview(&bv, targ->pos.x, targ->pos.y))) 
		{
			SendTarget(i, targ->dead_time, targ->damage);
			pl->last_target_update = i;
			bytes_left -= target_packet_size;
			if (++packet_count >= max_packet) {
				break;
			}
		}
	}

	packet_count = 0;
	max_packet = MAX(5, bytes_left / cannon_packet_size);
	i = MAX(0, pl->last_cannon_update);
	for (k = 0; k < world->numCannons; k++) {
		if (++i >= world->numCannons) {
			i = 0;
		}
		if (block_inview(&bv,
					 world->cannons[i].blk_pos.x,
					 world->cannons[i].blk_pos.y)) {
			if (BIT(world->cannons[i].conn_mask, conn_bit) == 0) {
				SendCannon(i, world->cannons[i].dead_time);
				pl->last_cannon_update = i;
				bytes_left -= max_packet * cannon_packet_size;
				if (++packet_count >= max_packet) {
					break;
				}
			}
		}
	}

	packet_count = 0;
	max_packet = MAX(5, bytes_left / fuel_packet_size);
	i = MAX(0, pl->last_fuel_update);
	for (k = 0; k < world->numFuels; k++) {
		if (++i >= world->numFuels) {
			i = 0;
		}
		if (BIT(world->fuels[i].conn_mask, conn_bit) == 0) {
			if (world->block[world->fuels[i].blk_pos.x]
			   [world->fuels[i].blk_pos.y] == FUEL) 
			{
				if (block_inview(&bv,
								 world->fuels[i].blk_pos.x,
								 world->fuels[i].blk_pos.y)) 
				{
					SendFuel(i, (int) world->fuels[i].fuel);
					pl->last_fuel_update = i;
					bytes_left -= max_packet * fuel_packet_size;
					if (++packet_count >= max_packet) {
						break;
					}
				}
			}
		}
	}

	packet_count = 0;
	max_packet = MAX(5, bytes_left / wormhole_packet_size);
	i = MAX(0, pl->last_wormhole_update);
	for (k = 0; k < world->numWormholes; k++) {
		Wormhole*		worm;
		if (++i >= world->numWormholes) {
			i = 0;
		}
		worm = &world->wormHoles[i];
		if (world->options.wormholeVisible->GetBool()
			&& worm->temporary
			&& (worm->type == WORM_IN
				|| worm->type == WORM_NORMAL)
			&& block_inview(&bv, worm->pos.x, worm->pos.y)) {
			/* This is really a stupid bug: he first converts
			   the perfect blocksizes to pixels which the
			   client is perfectly capable of doing itself.
			   Then he sends the pixels in signed shorts.
			   This will fail on big maps. */
			int x = (worm->pos.x * BLOCK_SZ) + BLOCK_SZ / 2,
				y = (worm->pos.y * BLOCK_SZ) + BLOCK_SZ / 2;
			SendWormhole(x, y);
			pl->last_wormhole_update = i;
			bytes_left -= max_packet * wormhole_packet_size;
			if (++packet_count >= max_packet) {
				break;
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
static void Frame_shuffle_objects(World* w)
{
	int 						i;
	size_t						memsize;

	num_object_shuffle = MIN(w->numObjs, w->options.maxVisibleObjects->GetInt());

	if (max_object_shuffle < num_object_shuffle) {
		if (object_shuffle_ptr != NULL) {
			free(object_shuffle_ptr);
		}
		max_object_shuffle = num_object_shuffle;
		memsize = max_object_shuffle * sizeof(shuffle_t);
		object_shuffle_ptr = (shuffle_t *) malloc(memsize);
		if (object_shuffle_ptr == NULL) {
			max_object_shuffle = 0;
		}
	}

	if (max_object_shuffle < num_object_shuffle) {
		num_object_shuffle = max_object_shuffle;
	}

	for (i = 0; i < num_object_shuffle; i++) {
		object_shuffle_ptr[i] = i;
	}
	/* permute. */
	for (i = num_object_shuffle - 1; i >= 0; --i) {
		if (object_shuffle_ptr[i] == i) {
			int j = (int)(rfrac() * i);
			shuffle_t tmp = object_shuffle_ptr[j];
			object_shuffle_ptr[j] = object_shuffle_ptr[i];
			object_shuffle_ptr[i] = tmp;
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
static void Frame_shuffle_players(World* w)
{
	int 						i;
	size_t						memsize;

	num_player_shuffle = MIN(w->numPlayers, MAX_SHUFFLE_INDEX);

	if (max_player_shuffle < num_player_shuffle) {
		if (player_shuffle_ptr != NULL) {
			free(player_shuffle_ptr);
		}
		max_player_shuffle = num_player_shuffle;
		memsize = max_player_shuffle * sizeof(shuffle_t);
		player_shuffle_ptr = (shuffle_t *) malloc(memsize);
		if (player_shuffle_ptr == NULL) {
			max_player_shuffle = 0;
		}
	}

	if (max_player_shuffle < num_player_shuffle) {
		num_player_shuffle = max_player_shuffle;
	}

	for (i = 0; i < num_player_shuffle; i++) {
		player_shuffle_ptr[i] = i;
	}
	/* permute. */
	for (i = 0; i < num_player_shuffle; i++) {
		int j = (int)(rfrac() * num_player_shuffle);
		shuffle_t tmp = player_shuffle_ptr[j];
		player_shuffle_ptr[j] = player_shuffle_ptr[i];
		player_shuffle_ptr[i] = tmp;
	}
}


///////////////////////////////////////////////////////////////////////////////
static void Frame_shuffle(World* w)
{
	if (last_frame_shuffle != frame_loops) {
		last_frame_shuffle = frame_loops;
		Frame_shuffle_objects(w);
		Frame_shuffle_players(w);
	}
}

///////////////////////////////////////////////////////////////////////////////
void Frame::Shots(int ind)
{
	Player* 		pl = world->players[ind];
	register int	x, y;
	int 			i, k, color;
	int 			fuzz = 0, teamshot, len;
	int 			obj_count;
	Object*			shot;
	Object**		obj_list;
	int 			hori_blocks, vert_blocks;

	hori_blocks = (viewWidth + (BLOCK_SZ - 1)) / (2 * BLOCK_SZ);
	vert_blocks = (viewHeight + (BLOCK_SZ - 1)) / (2 * BLOCK_SZ);
	world->CellGetObjects(OBJ_X_IN_BLOCKS(pl), OBJ_Y_IN_BLOCKS(pl),
					 MAX(hori_blocks, vert_blocks), num_object_shuffle,
					 &obj_list,
					 &obj_count);
	for (k = 0; k < num_object_shuffle; k++) {
		i = object_shuffle_ptr[k];
		if (i >= obj_count) {
			continue;
		}
		shot = obj_list[i];
		x = shot->pos.x;
		y = shot->pos.y;
		if (!inview(x, y)) {
			continue;
		}
		if ((color = shot->color) == BLACK) {
			xpprintf("black %d,%d\n", shot->type, shot->id);
			color = WHITE;
		}
		switch (shot->type) {
		case OBJ_SPARK:
		case OBJ_DEBRIS:
			if ((fuzz >>= 7) < 0x40) {
				fuzz = randomMT();
			}
			if ((fuzz & 0x7F) >= spark_rand) {
				/*
				 * produce a sparkling effect by not displaying
				 * particles every frame.
				 */
				break;
			}
			/*
			 * The number of colors which the client
			 * uses for displaying debris is bigger than 2
			 * then the color used denotes the temperature
			 * of the debris particles.
			 * Higher color number means hotter debris.
			 */
			if (debris_colors >= 3) {
				if (debris_colors > 4) {
					if (color == BLUE) {
						color = (shot->life >> 1);
					} else {
						color = (shot->life >> 2);
					}
				} else {
					if (color == BLUE) {
						color = (shot->life >> 2);
					} else {
						color = (shot->life >> 3);
					}
				}
				if (color >= debris_colors) {
					color = debris_colors - 1;
				}
			}

			DebrisStore((int)(shot->pos.x - pv.world.x),
						 (int)(shot->pos.y - pv.world.y),
						 color);
			break;

		case OBJ_WRECKAGE:
			if (spark_rand != 0 || world->options.wreckageCollisionMayKill->GetBool()) {
				WireObject *wreck = WIRE_PTR(shot);
				SendWreckage(x, y, (u_byte)wreck->info,
							  wreck->size, wreck->rotation);
			}
			break;

		case OBJ_ASTEROID: {
				WireObject *ast = WIRE_PTR(shot);
				SendAsteroid(x, y,
							  (u_byte)ast->info, ast->size, ast->rotation);
			}
			break;

		case OBJ_SHOT:
		case OBJ_CANNON_SHOT:
			if (pl->TeamImmune(shot->id)
				|| (shot->id != NO_ID
					//&& BIT(world->players[world->getInd[shot->id]]->status, PAUSE))
					&& BIT(world->players[world->getInd[shot->id]]->status, PAUSE))
				|| (shot->id == NO_ID
					&& BIT(world->rules->mode, TEAM_PLAY)
					&& shot->team == pl->team))
			{
				color = BLUE;
				teamshot = DEBRIS_TYPES;
			}
			else if (shot->id == pl->id
					&& world->options.selfImmunity->GetBool()) {
				color = BLUE;
				teamshot = DEBRIS_TYPES;
			} else if (shot->mods.nuclear && (frame_loops & 2)) {
				color = RED;
				teamshot = DEBRIS_TYPES;
			} else {
				teamshot = 0;
			}
			FastShotStore((int)(shot->pos.x - pv.world.x),
						  (int)(shot->pos.y - pv.world.y),
						  color, teamshot);
			break;

		case OBJ_TORPEDO:
			len =(world->options.distinguishMissiles->GetBool() ? TORPEDO_LEN : MISSILE_LEN);
			SendMissile(x, y, len, shot->missile_dir);
			break;
		case OBJ_SMART_SHOT:
			len =(world->options.distinguishMissiles->GetBool() ? SMART_SHOT_LEN : MISSILE_LEN);
			SendMissile(x, y, len, shot->missile_dir);
			break;
		case OBJ_HEAT_SHOT:
			len =(world->options.distinguishMissiles->GetBool() ? HEAT_SHOT_LEN : MISSILE_LEN);
			SendMissile(x, y, len, shot->missile_dir);
			break;
		case OBJ_BALL:
			SendBall(x, y, shot->id);
			break;
		case OBJ_MINE:
			{
				int id = 0;
				int laid_by_team = 0;
				int confused = 0;
				MineObject *mine = MINE_PTR(shot);

				/* calculate whether ownership of mine can be determined */
				if (world->options.identifyMines->GetBool()
					&& (world->WrapLength(pl->pos.x - mine->pos.x,
									pl->pos.y - mine->pos.y)
						< (SHIP_SZ + MINE_SENSE_BASE_RANGE
						   + pl->item[ITEM_SENSOR] * MINE_SENSE_RANGE_FACTOR))) {
					id = mine->id;
					if (id == NO_ID)
						id = EXPIRED_MINE_ID;
					if (BIT(mine->status, CONFUSED))
						confused = 1;
				}
				if (mine->id != NO_ID
						&& BIT(world->players[world->getInd[mine->id]]->status, PAUSE)) 
				{
						laid_by_team = 1;
				} else 
				{
						laid_by_team = (pl->TeamImmune(mine->id)
										|| (BIT(mine->status, OWNERIMMUNE)
												&& mine->owner == pl->id));
						if (confused) {
								id = 0;
								laid_by_team = (rfrac() < 0.5f);
						}
				}
				SendMine(x, y, laid_by_team, id);
			}
			break;

		case OBJ_ITEM:
			{
				int item_type = shot->info;

				if (BIT(shot->status, RANDOM_ITEM)) {
						item_type = world->ChooseRandomItem();
				}

				SendItem(x, y, item_type);
			}
			break;

		default:
			error("Frame_shots: Shot type %d not defined.", shot->type);
			break;
		}
	}
}

void Frame::Ships(int ind)
{
	Player* 			pl = world->players[ind];
		Player* 		pl_i;
	Pulse*				pulse;
	int 				i, j, k, color, dir;
	DFLOAT				x, y;

	for (j = 0; j < world->numPulses; j++) {
		pulse = world->pulses[j];
		if (pulse->len <= 0) {
			continue;
		}
		x = pulse->pos.x;
		y = pulse->pos.y;
		if (BIT (world->rules->mode, WRAP_PLAY)) {
			if (x < 0) {
				x += world->width;
			}
			else if (x >= world->width) {
				x -= world->width;
			}
			if (y < 0) {
				y += world->height;
			}
			else if (y >= world->height) {
				y -= world->height;
			}
		}
		if (inview(x, y)) {
				dir = pulse->dir;
		} else {
			x += tcos(pulse->dir) * pulse->len;
			y += tsin(pulse->dir) * pulse->len;
			if (BIT (world->rules->mode, WRAP_PLAY)) {
				if (x < 0) {
					x += world->width;
				}
				else if (x >= world->width) {
					x -= world->width;
				}
				if (y < 0) {
					y += world->height;
				}
				else if (y >= world->height) {
					y -= world->height;
				}
			}
			if (inview(x, y)) {
					dir = MOD2(pulse->dir + RES/2, RES);
			}
			else {
					continue;
			}
		}
		if (pl->TeamImmune(pulse->id)) {
				color = BLUE;
		} else if (pulse->id == pl->id
								&& world->options.selfImmunity->GetBool()) {
				color = BLUE;
		} else {
				color = RED;
		}
		SendLaser(color, (int)x, (int)y, pulse->len, dir);
	}
	for (i = 0; i < world->numEcms; i++) {
		Ecm *ecm = world->ecms[i];
		SendEcm((int)ecm->pos.x, (int)ecm->pos.y, ecm->size);
	}
	for (i = 0; i < world->numTransporters; i++) {
		Transporter* trans = world->transporters[i];
		Player* victim = world->players[world->getInd[trans->target]];
		Player* pl = (trans->id == NO_ID ? NULL : world->players[world->getInd[trans->id]]);
		DFLOAT	x = (pl ? pl->pos.x : trans->pos.x);
		DFLOAT	y = (pl ? pl->pos.y : trans->pos.y);
		SendTrans(victim->pos.x, victim->pos.y, (int)x, (int)y);
	}
	for (i = 0; i < world->numCannons; i++) {
		Cannon *cannon = world->cannons + i;
		if (cannon->tractor_count > 0) {
			Player *t = world->players[world->getInd[cannon->tractor_target]];
			if (inview(t->pos.x, t->pos.y)) {
				int j;
				for (j = 0; j < 3; j++) {
					SendConnector(
						(int)(t->pos.x + t->ship->pts[j][t->dir].x),
						(int)(t->pos.y + t->ship->pts[j][t->dir].y),
						(int)cannon->pix_pos.x,
						(int)cannon->pix_pos.y, 1);
				}
			}
		}
	}

	for (k = 0; k < num_player_shuffle; k++) {
		i = player_shuffle_ptr[k];
		pl_i = world->players[i];
		if (!BIT(pl_i->status, PLAYING|PAUSE)) {
			continue;
		}
		if (BIT(pl_i->status, GAME_OVER)) {
			continue;
		}
		if (!inview(pl_i->pos.x, pl_i->pos.y)) {
			continue;
		}
		if (BIT(pl_i->status, PAUSE)) {
			SendPaused(pl_i->pos.x,
					   pl_i->pos.y,
					   pl_i->count);
			continue;
		}

		/* Don't transmit information if fighter is invisible */
		if (pl->visibility[i].canSee
			|| i == ind
			|| TEAM(world, i, ind)
				|| ALLIANCE(world, i, ind)) {
			/*
			 * Transmit ship information
			 */
			SendShip(pl_i->pos.x,
					 pl_i->pos.y,
					 pl_i->id,
					 pl_i->dir,
					 BIT(pl_i->used, HAS_SHIELD) != 0,
					 BIT(pl_i->used, HAS_CLOAKING_DEVICE) != 0,
					 BIT(pl_i->used, HAS_EMERGENCY_SHIELD) != 0,
					 BIT(pl_i->used, HAS_PHASING_DEVICE) != 0,
					 BIT(pl_i->used, HAS_DEFLECTOR) != 0
			);
		}
		if (BIT(pl_i->used, HAS_REFUEL)) {
			if (inview(world->fuels[pl_i->fs].pix_pos.x,
					   world->fuels[pl_i->fs].pix_pos.y)) {
				SendRefuel((int)world->fuels[pl_i->fs].pix_pos.x,
						   (int)world->fuels[pl_i->fs].pix_pos.y,
						   pl_i->pos.x,
						   pl_i->pos.y);
			}
		}
		if (BIT(pl_i->used, HAS_REPAIR)) {
			DFLOAT x = (DFLOAT)(world->targets[pl_i->repair_target].pos.x + 0.5) * BLOCK_SZ;
			DFLOAT y = (DFLOAT)(world->targets[pl_i->repair_target].pos.y + 0.5) * BLOCK_SZ;
			if (inview(x, y)) {
				/* same packet as refuel */
				SendRefuel(pl_i->pos.x, pl_i->pos.y, (int) x, (int) y);
			}
		}
		if (BIT(pl_i->used, HAS_TRACTOR_BEAM)) {
			Player *t = world->players[world->getInd[pl_i->lock.pl_id]];
			if (inview(t->pos.x, t->pos.y)) {
				int j;

				for (j = 0; j < 3; j++) {
					SendConnector(
								   (int)(t->pos.x + t->ship->pts[j][t->dir].x),
								   (int)(t->pos.y + t->ship->pts[j][t->dir].y),
								   pl_i->pos.x,
								   pl_i->pos.y, 1);
				}
			}
		}

		if (pl_i->ball != NULL
			&& inview(pl_i->ball->pos.x, pl_i->ball->pos.y)) {
			SendConnector(
						   pl_i->ball->pos.x,
						   pl_i->ball->pos.y,
						   pl_i->pos.x,
						   pl_i->pos.y, 0);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
void Frame::Radar(int ind)
{
	int 				i, k, mask, shownuke, size;
	Player* 			pl = world->players[ind];
	Object* 			shot;
	DFLOAT				x, y;

		Frame_radar_buffer_reset();

#ifndef NO_SMART_MIS_RADAR
	if (world->options.nukesOnRadar->GetBool()) {
		mask = OBJ_SMART_SHOT|OBJ_TORPEDO|OBJ_HEAT_SHOT|OBJ_MINE;
	} else {
		mask = (world->options.missilesOnRadar->GetBool() ?
				(OBJ_SMART_SHOT|OBJ_TORPEDO|OBJ_HEAT_SHOT) : 0);
		mask |= (world->options.minesOnRadar->GetBool()) ? OBJ_MINE : 0;
	}
	if (world->options.treasuresOnRadar->GetBool())
		mask |= OBJ_BALL;
	if (world->options.asteroidsOnRadar->GetBool())
		mask |= OBJ_ASTEROID;

	if (mask) {
		for (i = 0; i < world->numObjs; i++) {
			shot = world->objs[i];
			if (! BIT(shot->type, mask))
				continue;

			shownuke = (world->options.nukesOnRadar->GetBool() && (shot)->mods.nuclear);
			if (shownuke && (frame_loops & 2)) {
				size = 3;
			} else {
				size = 0;
			}

			if (BIT(shot->type, OBJ_MINE)) {
				if (!world->options.minesOnRadar->GetBool() && !shownuke)
					continue;
				if (frame_loops % 8 >= 6)
					continue;
			} else if (BIT(shot->type, OBJ_BALL)) {
				size = 2;
			} else if (BIT(shot->type, OBJ_ASTEROID)) {
				size = WIRE_PTR(shot)->size + 1;
				size |= 0x80;
			} else {
				if (!world->options.missilesOnRadar->GetBool() && !shownuke)
					continue;
				if (frame_loops & 1)
					continue;
			}

			x = shot->pos.x;
			y = shot->pos.y;
			if (world->WrapLength(pl->pos.x - x,
							pl->pos.y - y) <= pl->sensor_range) {
				Frame_radar_buffer_add((int)x, (int)y, size);
			}
		}
	}
#endif

	if (world->options.playersOnRadar->GetBool()
				|| BIT(world->rules->mode, TEAM_PLAY)
				|| world->numPseudoPlayers > 0
				|| world->numAlliances > 0) {
		for (k = 0; k < num_player_shuffle; k++) {
			i = player_shuffle_ptr[k];
			/*
			 * Don't show on the radar:
			 *			Ourselves (not necessarily same as who we watch).
			 *			People who are not playing.
			 *			People in other teams or alliances if;
			 *					no playersOnRadar or if not visible
			 */
			if (world->players[i]->conn == this
				|| BIT(world->players[i]->status, PLAYING|PAUSE|GAME_OVER) != PLAYING
				|| (!TEAM(world, i, ind)
						&& !ALLIANCE(world, ind, i)
						&& !OWNS_TANK(world, ind, i)
					&& (!world->options.playersOnRadar->GetBool() || !pl->visibility[i].canSee))) {
				continue;
			}
			x = world->players[i]->pos.x;
			y = world->players[i]->pos.y;
			if (BIT(world->rules->mode, LIMITED_VISIBILITY)
				&& world->WrapLength(pl->pos.x - x,
							   pl->pos.y - y) > pl->sensor_range) {
				continue;
			}
			if (BIT(pl->used, HAS_COMPASS)
				&& BIT(pl->lock.tagged, LOCK_PLAYER)
				&& world->getInd[pl->lock.pl_id] == i
				&& frame_loops % 5 >= 3) {
				continue;
			}
			size = 3;
			if (TEAM(world, i, ind) || ALLIANCE(world, ind, i) || OWNS_TANK(world, ind, i)) {
				size |= 0x80;
			}
			Frame_radar_buffer_add((int)x, (int)y, size);
		}
	}

		RadarBufferSend();
}

///////////////////////////////////////////////////////////////////////////////
void Frame::LoseItemState(int ind)
{
		Player* 		pl = world->players[ind];
	if (pl->lose_item_state != 0) {
				SendLoseItem(pl->lose_item);
				if (pl->lose_item_state == 1)
						pl->lose_item_state = -5;
				if (pl->lose_item_state < 0)
						pl->lose_item_state++;
	}
}

///////////////////////////////////////////////////////////////////////////////
void Frame::Parameters(int ind)
{
	Player				*pl = world->players[ind];

	GetDisplayParameters(&viewWidth, &viewHeight,
						   &debris_colors, &spark_rand);
	debris_x_areas = (viewWidth + 255) >> 8;
	debris_y_areas = (viewHeight + 255) >> 8;
	debris_areas = debris_x_areas * debris_y_areas;
	horizontal_blocks = (viewWidth + (BLOCK_SZ - 1)) / BLOCK_SZ;
	vertical_blocks = (viewHeight + (BLOCK_SZ - 1)) / BLOCK_SZ;

	pv.world.x = pl->pos.x - viewWidth / 2; 	/* Scroll */
	pv.world.y = pl->pos.y - viewHeight / 2;
	pv.realWorld = pv.world;
	if (BIT (world->rules->mode, WRAP_PLAY)) {
		if (pv.world.x < 0 && pv.world.x + viewWidth < world->width) {
			pv.world.x += world->width;
		}
		else if (pv.world.x > 0 && pv.world.x + viewWidth >= world->width) {
			pv.realWorld.x -= world->width;
		}
		if (pv.world.y < 0 && pv.world.y + viewHeight < world->height) {
			pv.world.y += world->height;
		}
		else if (pv.world.y > 0 && pv.world.y + viewHeight >= world->height) {
			pv.realWorld.y -= world->height;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
void Frame_update(World* w)
{
	int			i;
	//			conn;
	int			ind;
	ConnectionPlayer*	connp;
	Frame*		frame;
	Player		*pl;
	time_t		newTimeLeft = 0;
	static time_t	oldTimeLeft;
	static bool		game_over_called = false;

	if (++frame_loops >= LONG_MAX)	/* Used for misc. timing purposes */
		frame_loops = 1;

	Frame_shuffle(w);

	if (w->options.gameDuration->GetDouble() > 0.0
		&& game_over_called == false
		&& oldTimeLeft != (newTimeLeft = gameOverTime - time(NULL))) 
	{
		/*
		* Do this once a second.
		*/
		if (newTimeLeft <= 0) 
		{
			w->GameOver();
			w->shutdownServer = 30 * w->GetFPS();	/* Shutdown in 30 seconds */
			game_over_called = true;
		}
	}
	int	viewingReduceRate =	w->options.viewingReduceRate->GetInt();
	int viewingReduceDelay = w->options.viewingReduceDelay->GetInt() * w->GetFPS();
	int	viewingKeepaliveRate = w->options.viewingKeepaliveRate->GetInt();
	int	viewingKeepaliveDelay = w->options.viewingKeepaliveDelay->GetInt() * w->GetFPS();

	for (i = 0; i < num_player_shuffle; i++) 
	{
		pl = w->players[i];
		if (!pl->conn) 
			continue;
		connp = pl->conn;
		frame = (Frame*)connp;
		if (BIT(pl->status, PAUSE|GAME_OVER)
			// && !w->options.allowViewing->GetBool()
			&& !pl->isowner) 
		{
			/*
			* Lower the frame rate for non-playing players
			* to reduce network load.
			* Owner always gets full framerate even if paused.
			* With allowViewing on, everyone gets full framerate.
			*/
			/*
			if (BIT(pl->status, PAUSE)) 
			{
				if (frame_loops & 0x03) 
					continue;
			}
			else 
			{
				if (frame_loops & 0x01) 
					continue;
			}
			*/
			if (viewingReduceRate) {
				long pausedFrameCount = frame_loops - pl->frame_last_busy;
				if (pausedFrameCount == viewingKeepaliveDelay) {
					pl->SetMessage("Reducing paused player to keepalive frame rate");
				}
				else if (pausedFrameCount == viewingReduceDelay) {
					pl->SetMessage("Reducing paused player frame rate");
				}
				if (pausedFrameCount > viewingKeepaliveDelay) {
					if (pausedFrameCount % viewingKeepaliveRate)
						continue;
				}
				else if (pausedFrameCount > viewingReduceDelay) {
					if (pausedFrameCount % viewingReduceRate)
						continue;
				}
			}
		}

		/*
		* Reduce frame rate to player's own rate.
		*/
		if (pl->player_count > 0) 
		{
			pl->player_round++;
			if (pl->player_round >= pl->player_count) 
			{
				pl->player_round = 0;
				continue;
			}
		}

		if (frame->SendStartOfFrame() == -1) 
			continue;
		if (newTimeLeft != oldTimeLeft) {
			frame->SendTimeLeft(newTimeLeft);
		} else if (w->options.maxRoundTime->GetInt() > 0
			&& w->roundTime >= 0) 
		{
			frame->SendTimeLeft((w->roundTime+w->GetFPS()-1) / w->GetFPS());
		}

	   /*
		* If status is GAME_OVER or PAUSE'd, the user may look through the
		* other players 'eyes'.  If PAUSE'd this only works on team members.
		* We can't use TEAM() macro as PAUSE'd players are always on
		* equivalent teams.
		*
		* This is done by using two indexes, one
		* determining which data should be used (ind, set below) and
		* one determining which connection to send it to (conn).
		*/
		if (BIT(pl->lock.tagged, LOCK_PLAYER)) 
		{
			int eyesId = NO_ID;
			if ((BIT(pl->status, (GAME_OVER|PLAYING)) == (GAME_OVER|PLAYING))
				|| (BIT(pl->status, PAUSE)
				&&
					((BIT(w->rules->mode, TEAM_PLAY)
						&& pl->team != TEAM_NOT_SET
						&& pl->team == w->players[w->getInd[pl->lock.pl_id]]->team)
					|| pl->isowner
					|| w->options.allowViewing->GetBool()))) 
			{
				ind = w->getInd[pl->lock.pl_id];
				eyesId = pl->lock.pl_id;
			} 
			else 
			{
				ind = i;
				eyesId = w->getInd[i];
			}
			if (!w->options.anonymousViewing->GetBool() && pl->IsPaused()) {
				pl->SetEyes(eyesId);
				// Trace("+++\n");
				if (!pl->robotWatchList.IsEmpty()) {
					// Trace("===\n");
					RobotWatch* rw = (RobotWatch*)pl->robotWatchList.GetHead();
					while (rw) {
						frame->SendRobotWatch(rw->line, (PCSTR)rw->s);
						// Trace("%d %s", rw->line, (PCSTR)rw->s);
						rw = (RobotWatch*)rw->GetNext();
					}
					pl->robotWatchList.Empty();
				}
				if (!pl->robotWatchDecoList.IsEmpty()) {
					D(Trace("Deco count %d\n", pl->robotWatchDecoList.GetCount());)
					RobotWatchDeco* rwd = (RobotWatchDeco*)pl->robotWatchDecoList.GetHead();
					while (rwd) {
						frame->SendRobotWatchDeco(rwd);
						rwd = (RobotWatchDeco*)rwd->GetNext();
					}
					pl->robotWatchDecoList.Empty();
				}
				pl->robotWatchFull = false;
			}
		} 
		else 
		{
			ind = i;
		}
		if (w->players[ind]->damaged > 0) 
		{
			frame->SendDamaged(w->players[ind]->damaged);
		}
		else 
		{
			frame->Parameters(ind);
			if (frame->Status(ind) <= 0) 
			{
				continue;
			}
			frame->Map(ind);
			frame->Ships(ind);
			frame->Shots(ind);
			frame->Radar(ind);
			frame->LoseItemState(ind);
			if (pl->lose_item_state != 0) 
			{
				frame->SendLoseItem(pl->lose_item);
				if (pl->lose_item_state == 1)
					pl->lose_item_state = -5;
				if (pl->lose_item_state < 0)
					pl->lose_item_state++;
			}
			frame->DebrisEnd();
			frame->FastShotEnd();
		}
		SoundPlayQueued(w->players[ind]);
		frame->SendEndOfFrame();
	}
	oldTimeLeft = newTimeLeft;

	Frame_radar_buffer_free();
}

