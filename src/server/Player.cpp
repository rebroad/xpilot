/* $Id: Player.cpp,v 1.50 2007/01/19 07:14:49 dick Exp $
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
 *  $Log: Player.cpp,v $
 *  Revision 1.50  2007/01/19 07:14:49  dick
 *  Whitespace
 *
 *  Revision 1.49  2007/01/17 09:06:22  dick
 *  Send the RobotWatch packets to the client
 *
 *  Revision 1.48  2007/01/13 22:27:28  dick
 *  Now, a Robot* is a member of Player.
 *  Robot contains the interface to drive a robot.
 *  Robot4 is the old robotdef.cpp handler.
 *
 *  Revision 1.47  2007/01/10 18:14:47  dick
 *  All robot actions are now handled through RobotMan.
 *  There is one RobotMan per World.
 *
 *  Revision 1.46  2006/09/21 04:56:35  dick
 *  Retab
 *
 *  Revision 1.45  2004/06/03 06:06:14  dick
 *  shipobj becomes ShipObj.
 *
 *  Revision 1.44  2004/05/22 15:15:08  dick
 *  myEyes becomes a id instead of an ind
 *
 *  Revision 1.43  2004/05/20 22:12:38  dick
 *  Attribute showLocks displays in the ScoreTable who everyone is locked on.
 *
 *  Revision 1.42  2004/05/07 04:27:41  dick
 *  _visibility becomes Visibility.  global updateScores becomes a member of World.
 *  Handle rank/rate data from the scoreserver and send it to the client.
 *
 *  Revision 1.41  2004/04/29 16:08:27  dick
 *  compile with SOUND enabled.
 *
 *  Revision 1.40  2004/04/26 07:07:14  dick
 *  Define a list of pages that can be displayed on the ScoreTable in the client.
 *  Each page can be controlled by the server (and the server admin tool).
 *
 *  Revision 1.39  2004/02/23 02:06:09  dick
 *  robot.h becomes Robot.h
 *
 *  Revision 1.38  2004/02/20 06:27:47  dick
 *  SendPlayMessage becomes BroadcastPlayMessage() because it sends a play message
 *  to all connections.
 *
 *  Revision 1.37  2004/02/18 01:22:14  dick
 *  Remove extraneous and obsolete #include "netserver.h"
 *
 *  Revision 1.36  2004/02/18 00:58:34  dick
 *  Delete_player becomes DeletePlayer
 *
 *  Revision 1.35  2004/02/16 14:29:44  dick
 *  server/sched.cpp and friends become serverlib/NetServer
 *  which disconnects the udp listen and receive functionality from the game.
 *
 *  Revision 1.34  2004/02/05 14:56:58  cppc
 *  First cut at integrating 'it' play mode.
 *
 *  Revision 1.33  2004/02/03 03:39:54  dick
 *  Add Windows precompiled header support.
 *
 *  Revision 1.32  2004/01/16 01:35:07  dick
 *  Turn off the "Eyes" indicator on the client when a player unpauses.
 *
 *  Revision 1.31  2004/01/08 18:10:51  dick
 *  Update to XPilot 4.5.5beta
 *
 *  Revision 1.30  2004/01/06 04:48:28  dick
 *  Add support for building under Cygwin.  Still needs work and the output
 *  is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 *  Revision 1.29  2002/09/11 16:42:04  dick
 *  Here endeth the server encapsulation task.
 *  theWorld is now an array of World(s) and get loaded dynamically.
 *
 *  Revision 1.28  2002/09/01 00:13:45  dick
 *  Remove obsolete windows #include "winServer.h"
 *
 *  Revision 1.27  2002/07/14 19:03:47  dick
 *  type_ext becomes enum PlayerType
 *
 *  Revision 1.26  2002/07/09 21:44:29  dick
 *  Send the player type (human,robot,tank) with the PlayerEvent to the ScoreServer.
 *
 *  Revision 1.25  2002/06/30 06:50:30  dick
 *  If a player is deleted and there is a missile targeted to him, then set the
 *  missile's target to NO_ID.
 *
 *  Revision 1.24  2002/06/25 04:57:32  dick
 *  Player didn't really need to talk to the ScoreServer.
 *
 *  Revision 1.23  2002/06/23 06:37:46  dick
 *  Some basic ScoreServer handling.
 *
 *  Revision 1.22  2002/06/04 22:21:57  dick
 *  Initially we are watching noone
 *
 *  Revision 1.21  2002/06/02 05:53:38  dick
 *  No code change, just rearrange everything so inits are on top,
 *  classless funcs are at the bottom.
 *
 *  Revision 1.20  2002/06/01 06:06:47  dick
 *  Encapsulate (almost) everything.  Get rid of (almost) all refs to theWorld.
 *
 *  Revision 1.19  2002/05/22 05:09:00  dick
 *  Update to XPilot r4_5_4alpha.
 *
 *  Revision 1.18  2002/05/18 20:55:38  dick
 *  Update to XPilot-4.5.1
 *
 *  Revision 1.17  2001/08/31 20:58:13  dick
 *  Server needs to be able to receive reliable packets as well as send them.
 *  Use the new emh error handling.
 *
 *  Revision 1.16  2001/08/14 21:40:46  dick
 *  Fix a bug when taking over someone's home base.
 *
 *  Revision 1.15  2001/08/10 08:11:37  dick
 *  New class ConnectionPlayer subclasses Connection to provide player
 *  functions.  New class Frame subclasses ConnectionPlayer which provides
 *  Frame update functionality.
 *
 *  Revision 1.14  2001/08/09 07:43:00  dick
 *  SetMessage becomes member function of Player.
 *
 *  Revision 1.13  2001/08/05 08:05:36  dick
 *  Add Tuners to the server.  ComputeSensorRange(), AddTank(), RemoveTank()
 *  become members of Player.  ComputeGravity*() become members of World.
 *
 *  Revision 1.12  2001/08/04 23:12:43  dick
 *  Remove old debug
 *
 *  Revision 1.11  2001/08/04 17:34:07  dick
 *  TransportToHome and DoAutopilot become members of Player.
 *  RECOVERY_DELAY needs * GetFPS().
 *
 *  Revision 1.10  2001/07/30 01:08:30  dick
 *  ship parsing uses const(s).
 *
 *  Revision 1.9  2001/07/24 09:03:59  dick
 *  Remove all global options.  Use World.options instead.
 *
 *  Revision 1.8  2001/07/23 21:08:36  dick
 *  Encapsulate ShipMass, ShotsMax, ShotsLife, ShotsMass, FPS.
 *
 *  Revision 1.7  2001/07/23 07:38:42  dick
 *  Fold contact, metaserver, netserver, rules, and server to be members of World.
 *
 *  Revision 1.6  2001/07/12 10:07:36  dick
 *  Go_home becomes GoHome
 *
 *  Revision 1.5  2001/07/10 08:10:08  dick
 *  Don't need (deleted) map.h because World.h is included in global.h
 *
 *  Revision 1.4  2001/07/08 06:40:14  dick
 *  C++ conversion stuff.
 *
 *  Revision 1.3  2001/07/08 02:47:52  dick
 *  Update to XPilot 4.4.2alpha
 *
 *  Revision 1.2  2001/07/07 12:00:42  dick
 *  Rename classes to C++ "Style".  old World becomes theWorld.
 *
 */
#include "StdAfx.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

#if defined(_WINDOWS) && !defined(_CYGWIN)
# include <windows.h>
# include "NT/ServerThreadW32.h"
#endif

#include "version.h"
#include "config.h"
#include "pack.h"
#include "serverconst.h"
#include "Object.h"
#include "World.h"
#include "global.h"
#include "proto.h"
#include "score.h"
#include "bit.h"
#include "saudio.h"
#include "error.h"
#include "objpos.h"
#include "draw.h"
#include "ConnectionPlayer.h"
#include "ConnectionControlScoreServer.h"
#include "RobotMan.h"

char player_version[] = VERSION;



static Player*		playerArray;
static Visibility*	visibilityArray;

void Alloc_players(World* w, int number)
{
	Player*		p;
	Visibility*	t;
	int i;


	/* Allocate space for pointers */
	w->players = (Player **) calloc(number + 1, sizeof(Player *));

	/* Allocate space for all entries, all player structs */
	p = playerArray = (Player *) calloc(number, sizeof(Player));

	/* Allocate space for all visibility arrays, n arrays of n entries */
	t = visibilityArray =
		(Visibility *) calloc(number * number,
							  sizeof(Visibility));

	if (!w->players || !playerArray || !visibilityArray) {
		error("Not enough memory for Players.");
		exit(1);
	}

	/* Players[-1] should evaluate to NULL. */
	w->players++;

	for (i = 0; i < number; i++) {
		w->players[i] = p++;
		w->players[i]->visibility = t;
		w->players[i]->world = w;
		/* Advance to next block/array */
		t += number;
	}
}

void Free_players(World* w)
{
	if (w->players) {
		--w->players;
		free(w->players);
		w->players = NULL;

// FIXME:  playerArray is for what?  and not tied to a world
		free(playerArray);
		free(visibilityArray);
	}
}

///////////////////////////////////////////////////////////////////////////////
int Player::Init(ShipObj *_ship)
{
	bool				too_late = false;
	int 				i;


	vel.x = vel.y		= 0.0;
	acc.x = acc.y		= 0.0;
	float_dir = dir 	= DIR_UP;
	turnvel 			= 0.0;
	oldturnvel			= 0.0;
	turnacc 			= 0.0;
	mass				= world->options.shipMass->GetDouble();
	emptymass			= world->options.shipMass->GetDouble();

	for (i = 0; i < NUM_ITEMS; i++) {
		if (!BIT(1U << i, ITEM_BIT_FUEL | ITEM_BIT_TANK)) {
			item[i] = world->items[i].initial;
		}
	}

	fuel.sum = world->items[ITEM_FUEL].initial << FUEL_SCALE_BITS;
	InitFuel(fuel.sum);

	if (world->options.allowShipShapes->GetBool() == true && _ship) {
		ship = _ship;
	}
	else {
/*
		pl->ship = Default_ship();
*/
		ShipObj* tryship = Parse_shape_str(world->options.defaultShipShape->GetString());

		if (tryship)
			ship = tryship;
		else
			ship = Default_ship();
	}

	power				= 45.0;
	turnspeed			= 30.0;
	turnresistance		= 0.12;
	power_s 			= 35.0;
	turnspeed_s 		= 25.0;
	turnresistance_s	= 0.12;

	check				= 0;
	round				= 0;
	time				= 0;
	last_lap_time		= 0;
	last_lap			= 0;
	best_lap			= 0;
	count				= -1;
	shield_time			= 0;
	last_wall_touch 	= 0;

	type				= OBJ_PLAYER;
	playerType			= PlayerHuman;			// assume Human player
	shots				= 0;
	missile_rack		= 0;
	forceVisible		= 0;
	ComputeSensorRange();
	shot_max			= world->options.maxPlayerShots->GetInt();
	shot_time			= 0;
	color				= WHITE;
	score				= 0;
	prev_score			= 0;
	prev_check			= 0;
	prev_round			= 0;
	rank				= 0;
	prevRank			= 0;
	rate				= 0.0;
	prevRate			= 0.0;
	fs					= 0;
	repair_target		= 0;
	name[0] 			= '\0';
	num_pulses			= 0;
	emergency_thrust_left = 0;
	emergency_thrust_max = 0;
	emergency_shield_left = 0;
	emergency_shield_max = 0;
	phasing_left		= 0;
	phasing_max			= 0;
	ecmcount			= 0;
	damaged				= 0;
	stunned 			= 0;
	myEyes				= NO_ID;

	status				= PLAYING | GRAVITY | DEF_BITS;
	have				= DEF_HAVE;
	used				= DEF_USED;

	if (item[ITEM_CLOAK] > 0) {
		SET_BIT(have, HAS_CLOAKING_DEVICE);
	}

	CLEAR_MODS(mods);
	for (i = 0; i < NUM_MODBANKS; i++)
		CLEAR_MODS(modbank[i]);
	for (i = 0; i < LOCKBANK_MAX; i++)
		lockbank[i] = NOT_CONNECTED;

	{
		static unsigned short	pseudo_team_no = 0;
		pseudo_team = pseudo_team_no++;
	}
	mychar				= ' ';
	prev_mychar = mychar;
	life				= world->rules->lives;
	prev_life	= life;
	ball				= NULL;

	player_fps	= world->GetFPS();
	player_round		= 0;
	player_count		= 0;

	kills				= 0;
	deaths				= 0;

	if (BIT(world->rules->mode, ALLOW_CLUSTERS))
		SET_BIT(mods.warhead, CLUSTER);

	/*
	 * If limited lives and if nobody has lost a life yet, you may enter
	 * now, otherwise you will have to wait 'til everyone gets GAME OVER.
	 */
	if (BIT(world->rules->mode, LIMITED_LIVES)) {
		for (i = 0; i < world->numPlayers; i++) {
			/* If a non-team member has lost a life,
			 * then it's too late to join. */
			if (world->players[i]->life < world->rules->lives && !TEAM(world, Ind(), i)) {
				too_late = true;
				break;
			}
		}
		if (too_late) {
			mychar		= 'W';
			prev_life = life = 0;
			SET_BIT(status, GAME_OVER);
		}
	}

	team			= TEAM_NOT_SET;
	alliance		= ALLIANCE_NOT_SET;
	prev_alliance	= ALLIANCE_NOT_SET;
	invite			= NO_ID;

	lock.tagged		= LOCK_NONE;
	lock.pl_id		= 0;

	robot			= NULL;

	wormDrawCount	= 0;

	id				= peek_ID();
	world->getInd[id]= Ind();
	conn			= NULL;;
	audio			= NULL;

	lose_item	= 0;
	lose_item_state 	= 0;

	shove_next = 0;
	for (i = 0; i < MAX_RECORDED_SHOVES; i++) {
		shove_record[i].pusher_id = NO_ID;
	}

	frame_last_busy 	= frame_loops;

	isowner = 0;
	isoperator = 0;

	return id;
}

///////////////////////////////////////////////////////////////////////////////
void Player::PickStartpos(bool newbie)
{
	int		i, num_free;
	int		pick = 0, seen = 0;
	static int	prev_num_bases = 0;
	static char	*free_bases = NULL;

	if (IS_TANK_PTR(this))
	{
		home_base = 0;
		return;
	}

	if (prev_num_bases != world->numBases) 
	{
		prev_num_bases = world->numBases;
		if (free_bases != NULL) 
			free(free_bases);
		free_bases = (char *) malloc(world->numBases * sizeof(*free_bases));
		if (free_bases == NULL) 
		{
			error("Can't allocate memory for free_bases");
			world->EndGame();
		}
	}

	num_free = 0;
	// find all the free bases for our team
	for (i = 0; i < world->numBases; i++) 
	{
		if (world->bases[i].team == team) 
		{
			num_free++;
			free_bases[i] = 1;
		} 
		else
			free_bases[i] = 0;	/* other team */
	}
	// and subtract the occupied ones
	for (i = 0; i < world->numPlayers; i++) 
	{
		if (world->players[i] != this
			&& !IS_TANK_IND(world, i)
			&& free_bases[world->players[i]->home_base]) 
		{
			free_bases[world->players[i]->home_base] = 0;	/* occupado */
			num_free--;
		}
	}

	if (BIT(world->rules->mode, TIMING))
	{	/* pick first free base */
		for (i=0; i < world->numBases; i++) 
		{
			if (free_bases[world->baseorders[i].base_idx]) 
				break;
		}
	}
	else 
	{
		pick = (int)(rfrac() * num_free);
		seen = 0;
		for (i = 0; i < world->numBases; i++) 
		{
			if (free_bases[i] != 0) 
			{
				if (seen < pick) 
					seen++;
				else 
					break;
			}
		}
	}

	if (i == world->numBases) 
	{
		String e;
		e.printf("Can't pick startpos (num=%d,free=%d,pick=%d,seen=%d)",
			world->numBases, num_free, pick, seen);
		error(e);
		world->EndGame(e);
	}
	else 
	{
		home_base = BIT(world->rules->mode, TIMING) ?
						world->baseorders[i].base_idx : i;
		if (!newbie) 
		{
			for (i = 0; i < world->numPlayers; i++) 
			{
				if (world->players[i]->conn) 
					world->players[i]->conn->SendBase(id, home_base);
			}
			if (BIT(status, PLAYING) == 0) 
				count = RECOVERY_DELAY*world->GetFPS();
			else if (BIT(status, PAUSE|GAME_OVER)) 
				GoHome();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
int Player::Ind()
{
	for (int i=0; i<=world->numPlayers; i++)
		if (world->players[i] == this)
			return(i);
	// assert(0);		// Can't match player 
	return(0);
}

///////////////////////////////////////////////////////////////////////////////
void Player::GoHome()
{
    int			i, _x, _y, _dir, _check;
    DFLOAT		vx, vy, velo;
	int		ind = Ind();

    if (IS_TANK_PTR(this)) {
		/*NOTREACHED*/
		/* Tanks have no homebase. */
		error("BUG: gohome tank");
		return;
    }

    if (BIT(world->rules->mode, TIMING)
	 && round
	 && !BIT(status, GAME_OVER)) {
		if (check)
		    _check = check - 1;
		else
			_check = world->numChecks - 1;
		_x = world->check[_check].x;
		_y = world->check[_check].y;
		vx = (rfrac() - 0.5) * 0.1;
		vy = (rfrac() - 0.5) * 0.1;
		velo = LENGTH(vx, vy);
		_dir = last_check_dir;
		_dir = MOD2(dir + (int)((rfrac() - 0.5) * (RES / 8)), RES);
	} else {
		_x = world->bases[home_base].pos.x;
		_y = world->bases[home_base].pos.y;
		_dir = world->bases[home_base].dir;
		vx = vy = velo = 0;
    }

    dir = _dir;
    float_dir = _dir;
    Player_position_init_pixels(this,
				(_x + 0.5) * BLOCK_SZ + vx,
				(_y + 0.5) * BLOCK_SZ + vy);
    vel.x = vx;
    vel.y = vy;
    velocity = velo;
    acc.x = acc.y = 0.0;
    turnacc = turnvel = 0.0;
    memset(last_keyv, 0, sizeof(last_keyv));
    memset(prev_keyv, 0, sizeof(prev_keyv));
    UsedKill();

	if (world->options.playerStartsShielded->GetBool() == true)	{
		SET_BIT(used, HAS_SHIELD);
		if (world->options.allowShields->GetBool() == false) {
			shield_time = 2 * world->GetFPS();
			SET_BIT(have, HAS_SHIELD);
		}
		if (BIT(have, HAS_DEFLECTOR)) {
			Deflector(true);
		}
	}
	CLR_BIT(status, THRUSTING);
	updateVisibility = 1;
	for (i = 0; i < world->numPlayers; i++)	{
		visibility[i].lastChange = 0;
		world->players[i]->visibility[ind].lastChange = 0;
	}

	if (IS_ROBOT_PTR(this)) {
		world->robotMan->GoHome(ind);
	}
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Compute the current sensor range for player `pl'.  This is based on the
 * amount of fuel, the number of sensor items (each one adds 25%), and the
 * minimum and maximum visibility limits in effect.
 */
void Player::ComputeSensorRange()
{
	static int			init = 0;
	static DFLOAT		EnergyRangeFactor;

	if (!init) {
		if (world->options.minVisibilityDistance->GetDouble() <= 0.0)
			world->options.minVisibilityDistance->Set(VISIBILITY_DISTANCE);
		else
				world->options.minVisibilityDistance->Set(
						world->options.minVisibilityDistance->GetDouble() * BLOCK_SZ);
		if (world->options.maxVisibilityDistance->GetDouble() <= 0.0)
			world->options.maxVisibilityDistance->Set(world->hypotenuse);
		else
				world->options.minVisibilityDistance->Set(
						world->options.minVisibilityDistance->GetDouble() * BLOCK_SZ);

		if (world->items[ITEM_FUEL].initial > 0.0) {
			EnergyRangeFactor = world->options.minVisibilityDistance->GetDouble() /
				(world->items[ITEM_FUEL].initial
					* (1.0 + ((DFLOAT)world->items[ITEM_SENSOR].initial * 0.25)));
			EnergyRangeFactor /= FUEL_SCALE_FACT;
		} else {
			EnergyRangeFactor = ENERGY_RANGE_FACTOR;
		}
		init = 1;
	}

	sensor_range = fuel.sum * EnergyRangeFactor;
	sensor_range *= (1.0 + ((DFLOAT)item[ITEM_SENSOR] * 0.25));
		if (sensor_range < world->options.minVisibilityDistance->GetDouble())
				sensor_range = world->options.minVisibilityDistance->GetDouble();
		if (sensor_range > world->options.maxVisibilityDistance->GetDouble())
				sensor_range = world->options.maxVisibilityDistance->GetDouble();
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Give ship one more tank, if possible.
 */
void Player::AddTank(long tank_fuel)
{
	long		tank_cap, add_fuel;

	if (fuel.num_tanks < MAX_TANKS) {
		fuel.num_tanks++;
		tank_cap = TANK_CAP(fuel.num_tanks);
		add_fuel = tank_fuel;
		LIMIT(add_fuel, 0, tank_cap);
		fuel.sum += add_fuel;
		fuel.max += tank_cap;
		fuel.tank[fuel.num_tanks] = add_fuel;
		emptymass += TANK_MASS(world);
		item[ITEM_TANK] = fuel.num_tanks;
	}
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Remove a tank from a ship, if possible.
 */
void Player::RemoveTank(int which_tank)
{
	int			i, tank_ind;
	long		tank_fuel, tank_cap;

	if (fuel.num_tanks > 0) {
		tank_ind = which_tank;
		LIMIT(tank_ind, 1, fuel.num_tanks);
		emptymass -= TANK_MASS(world);
		tank_fuel = fuel.tank[tank_ind];
		tank_cap = TANK_CAP(tank_ind);
		fuel.max -= tank_cap;
		fuel.sum -= tank_fuel;
		fuel.num_tanks--;
		if (fuel.current > fuel.num_tanks) {
			fuel.current = 0;
		} else {
			for (i = tank_ind; i <= fuel.num_tanks; i++) {
				fuel.tank[i] = fuel.tank[i + 1];
			}
		}
		item[ITEM_TANK] = fuel.num_tanks;
	}
}

///////////////////////////////////////////////////////////////////////////////
void Player::SetEyes(int plId)
{
	Player* pl = NULL;
//	int		plId = _id;

	if (myEyes != plId)
	{
		if (plId == this->id)
			plId = NO_ID;
		myEyes = plId;
		if (!world->options.anonymousViewing->GetBool())
		{
			for (int i = 0; i < world->numPlayers; i++) 
			{
				if (world->players[i]->conn)
					world->players[i]->conn->SendEyes(id, plId);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
void Player::SetLock(int plId)
{
	if (lock.pl_id != plId)
	{
		lock.pl_id = plId;
		if (world->options.showLocks->GetBool())
		{
			for (int i = 0; i < world->numPlayers; i++) 
			{
				if (world->players[i]->conn)
					world->players[i]->conn->SendEyes(id, plId);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
void Player::HitArmor()
{
    if (--item[ITEM_ARMOR] <= 0)
	CLR_BIT(have, HAS_ARMOR);
}

///////////////////////////////////////////////////////////////////////////////
void Player::UsedKill()
{
	used &= ~USED_KILL;
	if (!BIT(DEF_HAVE, HAS_SHIELD)) {
		CLR_BIT(have, HAS_SHIELD);
	}
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Calculate the mass of a player.
 */
void Player::SetMass()
{
	DFLOAT		sum_item_mass = 0;
	DFLOAT		item_mass;
	int			it;

	for (it = 0; it < NUM_ITEMS; it++) {
		switch (it) {
			case ITEM_FUEL:
			case ITEM_TANK:
				item_mass = 0;
				break;
			case ITEM_ARMOR:
				item_mass = item[ITEM_ARMOR] * ARMOR_MASS(world);
				break;
			default:
				item_mass = item[it] * world->options.minItemMass->GetDouble();
				break;
		}
		sum_item_mass += item_mass;
	}

	mass = emptymass
			+ FUEL_MASS(fuel.sum)
			+ sum_item_mass;
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Give player the initial number of tanks and amount of fuel.
 * Upto the maximum allowed.
 */
void Player::InitFuel(long total_fuel)
{
    long		_fuel = total_fuel;
    int			i;

    fuel.num_tanks  = 0;
    fuel.current    = 0;
    fuel.max	= TANK_CAP(0);
    fuel.sum	= MIN(_fuel, fuel.max);
    fuel.tank[0]	= fuel.sum;
    emptymass	= world->options.shipMass->GetDouble();
    item[ITEM_TANK]	= fuel.num_tanks;

    _fuel -= fuel.sum;

    for (i = 1; i <= world->items[ITEM_TANK].initial; i++) {
	AddTank(_fuel);
	_fuel -= fuel.tank[i];
    }
}

///////////////////////////////////////////////////////////////////////////////
void Player::DetachBall(int obj)
{
    int			i, cnt;

    if (obj == -1 || BALL_PTR(world->objs[obj]) == ball) {
		ball = NULL;
		CLR_BIT(used, HAS_CONNECTOR);
    }

    if (BIT(have, HAS_BALL)) {
		for (cnt = i = 0; i < world->numObjs; i++) {
		    if (world->objs[i]->type == OBJ_BALL && world->objs[i]->id == id) {
				if (obj == -1 || obj == i) {
				    world->objs[i]->id = NO_ID;
				    /* Don't reset owner so you can throw balls */
				} else {
				    cnt++;
				}
		    }
		}
		if (cnt == 0)
			CLR_BIT(have, HAS_BALL);
		else {
		    SoundPlaySensors(world, pos.x, pos.y, DROP_BALL_SOUND);
		}
    }
}

///////////////////////////////////////////////////////////////////////////////
void Player::Kill()
{
    ExplodeFighter();
    DeathReset();
}

///////////////////////////////////////////////////////////////////////////////
void Player::DeathReset()
{
    long		minfuel;
    int			i;


    if (IS_TANK_PTR(this)) {
	DeletePlayer(world, Ind());
	return;
    }

    DetachBall(-1);
    if (BIT(used, HAS_AUTOPILOT) || BIT(status, HOVERPAUSE)) {
		CLR_BIT(status, HOVERPAUSE);
		Autopilot(false);
    }

    vel.x		= vel.y	= 0.0;
    acc.x		= acc.y	= 0.0;
    emptymass	= mass	= world->options.shipMass->GetDouble();;
    status		|= DEF_BITS;
    status		&= ~(KILL_BITS);

    for (i = 0; i < NUM_ITEMS; i++) {
	if (!BIT(1U << i, ITEM_BIT_FUEL | ITEM_BIT_TANK)) {
	    item[i] = world->items[i].initial;
	}
    }

    forceVisible	= 0;
    shot_max	= world->options.maxPlayerShots->GetInt();
    count		= MAX(RECOVERY_DELAY*world->GetFPS(), count);
    ecmcount	= 0;
    emergency_thrust_left = 0;
    emergency_thrust_max = 0;
    emergency_shield_left = 0;
    emergency_shield_max = 0;
    phasing_left	= 0;
    phasing_max	= 0;
    damaged 	= 0;
    stunned		= 0;
    lock.distance	= 0;

    fuel.sum       	= (long)(fuel.sum*0.90);	/* Loose 10% of fuel */
    minfuel		= (world->items[ITEM_FUEL].initial * FUEL_SCALE_FACT);
    minfuel		+= (int)(rfrac() * (1 + minfuel) * 0.2f);
    fuel.sum	= MAX(fuel.sum, minfuel);
    InitFuel(fuel.sum);

    /*-BA Handle the combination of limited life games and
     *-BA robotLeaveLife by making a robot leave iff it gets
     *-BA eliminated in any round.  Means that robotLeaveLife
     *-BA is ignored, but that robotsLeave is still respected.
     *-KK Added check on race mode. Since in race mode everyone
     *-KK gets killed at the end of the round, all robots would
     *-KK be replaced in the next round. I don't think that's
     *-KK the Right Thing to do.
     *-KK Also, only check a robot's score at the end of the round.
     *-KK 27-2-98 Check on team mode too. It's very confusing to
     *-KK have different robots in your team every round.
     */

    if (!BIT(status, PAUSE)) {

		deaths++;

		if (BIT(world->rules->mode, LIMITED_LIVES)) { 
			life--;
			if (life == -1) {
				if (IS_ROBOT_PTR(this)) {
					if (!BIT(world->rules->mode, TIMING|TEAM_PLAY)
					 || (world->options.robotsLeave->GetBool()
					 && score < world->options.robotLeaveScore->GetDouble())) {
						world->robotMan->Delete(Ind(), false);
						return;
					}
				}
				life = 0;
				SET_BIT(status, GAME_OVER);
				mychar = 'D';
				world->PlayerLockClosest(this, 0);
			}
		}
		else {
			life++;
		}
    }

    have	= DEF_HAVE;
    used	|= DEF_USED;
    used	&= ~(USED_KILL);
    used	&= have;
}

///////////////////////////////////////////////////////////////////////////////
void Player::TransportToHome()
{
    /*
     * Transport a corpse from the place where it died back to its homebase,
     * or if in race mode, back to the last passed check point.
     * 
     * During the first part of the distance we give it a positive constant
     * acceleration G, during the second part we make this a negative one -G.
     * This results in a visually pleasing take off and landing.
     */
	DFLOAT		bx, by, dx, dy,	t, m;
	const int		T = RECOVERY_DELAY*world->GetFPS();

	if (BIT(world->rules->mode, TIMING) && round) 
	{
		int _check;

		if (check)
		_check = check - 1;
		else
		_check = world->numChecks - 1;
		bx = (world->check[_check].x + 0.5) * BLOCK_SZ;
		by = (world->check[_check].y + 0.5) * BLOCK_SZ;
	} 
	else 
	{
		bx = (world->bases[home_base].pos.x + 0.5) * BLOCK_SZ;
		by = (world->bases[home_base].pos.y + 0.5) * BLOCK_SZ;
	}
	dx = WRAP_DX(world, bx - pos.x);
	dy = WRAP_DY(world, by - pos.y);
	t = count + 0.5f;
	if (2 * t <= T) 
	{
		m = 2 / t;
	}
	else 
	{
		t = T - t;
		m = (4 * t) / (T * T - 2 * t * t);
	}
	vel.x = dx * m;
	vel.y = dy * m;
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Automatic pilot will try to hold the ship steady, turn to face away
 * from direction of travel, if so then turn on thrust which will
 * cause the ship to come to a rest within a short period of time.
 * This code is fairly self contained.
 */
void Player::DoAutopilot()
{
	int 		vad;	/* Velocity Away Delta */
	int 		_dir;
	int 		afterburners;
	int 		ix, iy;
	DFLOAT		gx, gy;
	DFLOAT		delta;
	DFLOAT		acc;
		DFLOAT	_vel;
	DFLOAT		_turnspeed;
		DFLOAT	_power;
	const DFLOAT		emergency_thrust_settings_delta = 150.0 / world->GetFPS();
	const DFLOAT		auto_pilot_settings_delta = 15.0 / world->GetFPS();
	const DFLOAT		auto_pilot_turn_factor = 2.5;
	const DFLOAT		auto_pilot_dead_velocity = 0.5;

	/*
	 * If the last movement touched a wall then we shouldn't
	 * mess with the position (speed too?) settings.
	 */
	if (last_wall_touch + 1 >= frame_loops) {
		return;
	}

	/*
	 * Having more autopilot items or using emergency thrust causes a much
	 * quicker deceleration to occur than during normal flight.  Having
	 * no autopilot items will cause minimum delta to occur, this is because
	 * the autopilot code is used by the pause code.
	 */
	delta = auto_pilot_settings_delta;
	if (item[ITEM_AUTOPILOT])
		delta *= item[ITEM_AUTOPILOT];

	if (BIT(used, HAS_EMERGENCY_THRUST)) {
		afterburners = MAX_AFTERBURNER;
		if (delta < emergency_thrust_settings_delta)
			delta = emergency_thrust_settings_delta;
	} else {
		afterburners = item[ITEM_AFTERBURNER];
	}

	ix = OBJ_X_IN_BLOCKS(this);
	iy = OBJ_Y_IN_BLOCKS(this);
	gx = world->gravity[ix][iy].x;
	gy = world->gravity[ix][iy].y;

	/*
	 * Due to rounding errors if the velocity is very small we were probably
	 * on target to stop last time round, so we actually absolutely stop.
	 * This enables the ship to orient away from gravity and set up the
	 * thrust to counteract it.
	 */
	if ((_vel = VECTOR_LENGTH(vel)) < auto_pilot_dead_velocity) {
		vel.x = vel.y = _vel = 0.0;
		Player_position_restore(this);
	}

	/*
	 * Calculate power needed to change instantaneously to stopped.  We
	 * must include gravity here for next time round the update loop.
	 */
	acc = LENGTH(gx, gy) + _vel;
	_power = acc * mass;
	if (afterburners)
		_power /= AFTER_BURN_POWER_FACTOR(afterburners);

	/*
	 * Calculate direction change needed to reduce velocity to zero.
	 */
	if (_vel == 0.0) {
		if (gx == 0 && gy == 0)
			vad = dir;
		else
			vad = (int)FindDir(-gx, -gy);
	} else {
		vad = (int)FindDir(-vel.x, -vel.y);
	}
	vad = MOD2(vad - dir, RES);
	if (vad > RES/2) {
		vad = RES - vad;
		_dir = -1;
	} else {
		_dir = 1;
	}

	/*
	 * Calculate turnspeed needed to change direction instantaneously by
	 * above direction change.
	 */
	_turnspeed = ((DFLOAT)vad) / turnresistance - turnvel;
	if (_turnspeed < 0) {
		_turnspeed = -_turnspeed;
		_dir = -_dir;
	}

	/*
	 * Change the turnspeed setting towards the perfect value, and limit
	 * to the maximum only (limiting to the minimum causes oscillation).
	 */
	if (_turnspeed < turnspeed) {
		turnspeed -= delta;
		if (_turnspeed > turnspeed)
			turnspeed = _turnspeed;
	} else if (_turnspeed > turnspeed) {
		turnspeed += delta;
		if (_turnspeed < turnspeed)
			turnspeed = _turnspeed;
	}
	if (turnspeed > MAX_PLAYER_TURNSPEED)
		turnspeed = MAX_PLAYER_TURNSPEED;

	/*
	 * Decide if its wise to turn this time.
	 */
	if (turnspeed > (_turnspeed*auto_pilot_turn_factor)) {
		turnacc = 0.0;
		turnvel = 0.0;
	} else {
		turnacc = _dir * turnspeed;
	}

	/*
	 * Change the power setting towards the perfect value, and limit
	 * to the maximum only (limiting to the minimum causes oscillation).
	 */
	if (_power < power) {
		power -= delta;
		if (_power > power)
			power = _power;
	} else if (_power > power) {
		power += delta;
		if (_power < power)
			power = _power;
	}
	if (power > MAX_PLAYER_POWER)
				power = MAX_PLAYER_POWER;

	/*
	 * Don't thrust if the direction will not be absolutely correct and hasn't
	 * been very close last time.  The latter clause was added such that
	 * when a fine direction adjustment is needed, but the turnspeed is too
	 * high at the moment, it gets the ship slowing down even though it
	 * will impart some sideways velocity.
	 */
	if (turnspeed != _turnspeed && vad > RES/32) {
		CLR_BIT(status, THRUSTING);
		return;
	}

	/*
	 * Only thrust if the power setting is correct or less than correct,
	 * we don't want to over thrust.
	 */
	if (power > _power) {
		CLR_BIT(status, THRUSTING);
	} else {
		SET_BIT(status, THRUSTING);
	}
}

///////////////////////////////////////////////////////////////////////////////
void Player::DoHyperjump()
{
	SET_BIT(status, WARPING);
	wormHoleHit = -1;
}

///////////////////////////////////////////////////////////////////////////////
void Player::SetMessage(PCSTR message)
{
	int 				i;
	const char			*msg;
	char				tmp[MSG_LEN];

	if ((i = strlen(message)) >= MSG_LEN) {
#ifndef SILENT
		errno = 0;
		error("Max message len exceed (%d,%s)", i, message);
#endif
		memcpy(tmp, message, MSG_LEN - 1);
		tmp[MSG_LEN - 1] = '\0';
		msg = tmp;
	} else {
		msg = message;
	}
	if (conn) {
		conn->SendPlayMessage(msg);
	}
	else if (IS_ROBOT_PTR(this)) {
		world->robotMan->Message(world->getInd[id], msg);
	}
}

///////////////////////////////////////////////////////////////////////////////
void Player::SetRobotWatch(int y, PCSTR message)
{
	int 				i;
	const char			*msg;
	char				tmp[MSG_LEN];

	if ((i = strlen(message)) >= MSG_LEN) {
#ifndef SILENT
		errno = 0;
		error("Max message len exceed (%d,%s)", i, message);
#endif
		memcpy(tmp, message, MSG_LEN - 1);
		tmp[MSG_LEN - 1] = '\0';
		msg = tmp;
	} else {
		msg = message;
	}
	if (conn) {
		conn->SendRobotWatch(y, msg);
	}
}


///////////////////////////////////////////////////////////////////////////////
/* determines if two players are immune to eachother */
int Player::TeamImmune(int id2)
{
	int 		ind1, ind2;

	if (id == id2) {
		/* owned stuff is never team immune */
		return 0;
	}
	if (!world->options.teamImmunity->GetBool()) {
		return 0;
	}
	if (id == NO_ID
		|| id2 == NO_ID) {
		/* can't find owner for cannon stuff */
		return 0;
	}

	ind1 = world->getInd[id];
	ind2 = world->getInd[id2];

	if (TEAM(world, ind1, ind2)) {
		/* players are teammates */
		return 1;
	}

	if (ALLIANCE(world, ind1, ind2)) {
		/* players are allies */
		return 1;
	}

	return 0;
}


///////////////////////////////////////////////////////////////////////////////
void UpdateScoreTable(World* w)
{
	int			i, j, check;
	Player*		pl;

	for (j = 0; j < w->numPlayers; j++) 
	{
		pl = w->players[j];
		if (pl->score != pl->prev_score
			|| pl->life != pl->prev_life
			|| pl->mychar != pl->prev_mychar
			|| pl->alliance != pl->prev_alliance)
		{
			pl->prev_score = pl->score;
			pl->prev_life = pl->life;
			pl->prev_mychar = pl->mychar;
			pl->prev_alliance = pl->alliance;
			for (i = 0; i < w->numPlayers; i++) 
			{
				if (w->players[i]->conn) 
					w->players[i]->conn->SendScore(pl);
			}
		}
		if (w->scoreServer && w->options.scoreTableRank->GetBool())
		{
			if (pl->rank != pl->prevRank
			 || pl->rate != pl->prevRate)
			{
				pl->prevRank = pl->rank;
				pl->prevRate = pl->rate;
				for (i=0; i<w->numPlayers; i++)
				{
					if (w->players[i]->conn)
						w->players[i]->conn->SendScoreTableRank(pl);
				}
			}
		}
		if (BIT(w->rules->mode, TIMING)) 
		{
			if (pl->check != pl->prev_check
			|| pl->round != pl->prev_round) 
			{
				pl->prev_check = pl->check;
				pl->prev_round = pl->round;
				check = (pl->round == 0)
						? 0
						: (pl->check == 0)
						? (w->numChecks - 1)
						: (pl->check - 1);
				for (i = 0; i < w->numPlayers; i++) 
				{
					if (w->players[i]->conn) 
						w->players[i]->conn->SendTiming(pl->id, check, pl->round);
				}
			}
		}
	}
	if (BIT(w->rules->mode, TEAM_PLAY)) {
		Team*	team;
		for (j=0; j<MAX_TEAMS; j++) {
			team = &w->teams[j];
			if (team->score != team->prevScore) {
				for (i=0; i<w->numPlayers; i++) {
					if (w->players[i]->conn) {
						w->players[i]->conn->SendTeamScore(j, team->score);
					}
				}
			}
		}
	}

	w->updateScores = false;
}


///////////////////////////////////////////////////////////////////////////////
void ResetAllPlayers(World* w)
{
    Player		*pl;
    int			i, j;
    char		msg[MSG_LEN];

    w->updateScores = true;

	for (i = 0; i < w->numPlayers; i++) {
		pl = w->players[i];
		if (w->options.endOfRoundReset->GetBool()) {
			if (BIT(pl->status, PAUSE)) {
				pl->DeathReset();
			} else {
				pl->Kill();
				if (pl != w->players[i]) {
					/* player was deleted. */
					i--;
					continue;
				}
			}
		}
		CLR_BIT(pl->status, GAME_OVER);
		CLR_BIT(pl->have, HAS_BALL);
		pl->kills = 0;
		pl->deaths = 0;
		pl->round = 0;
		pl->check = 0;
		pl->time = 0;
		pl->best_lap = 0;
		pl->last_lap = 0;
		pl->last_lap_time = 0;
		if (!BIT(pl->status, PAUSE)) {
			pl->mychar = ' ';
			pl->frame_last_busy = frame_loops;
			pl->life = w->rules->lives;
			if (BIT(w->rules->mode, TIMING)) {
				pl->count = RECOVERY_DELAY * w->GetFPS();
			}
		}
		if (IS_TANK_PTR(pl))
			pl->mychar = 'T';
		else if (IS_ROBOT_PTR(pl))
			pl->mychar = 'R';
	}
    if (BIT(w->rules->mode, TEAM_PLAY)) {

	/* Detach any balls and kill ball */
	/* We are starting all over again */
	for (j = w->numObjs - 1; j >= 0 ; j--) {
	    if (BIT(w->objs[j]->type, OBJ_BALL)) {
		BallObject *ball = BALL_IND(w, j);
		ball->id = NO_ID;
		ball->life = 0;
		ball->owner = 0;	/* why not -1 ??? */
		CLR_BIT(ball->status, RECREATE);
		w->DeleteShot(j);
	    }
	}

	/* Reset the treasures */
	for (i = 0; i < w->numTreasures; i++) {
	    w->treasures[i].destroyed = 0;
	    w->treasures[i].have = false;
	    w->MakeTreasureBall(i);
	}

	/* Reset the teams */
	for (i = 0; i < MAX_TEAMS; i++) {
	    w->teams[i].treasuresDestroyed = 0;
	    w->teams[i].treasuresLeft = 
			w->teams[i].numTreasures - w->teams[i].numEmptyTreasures;
	}

	if (w->options.endOfRoundReset->GetBool()) {
	    /* Reset the targets */
	    for (i = 0; i < w->numTargets; i++) {
		if (w->targets[i].damage != TARGET_DAMAGE
		    || w->targets[i].dead_time != 0) {
			    w->block[w->targets[i].pos.x][w->targets[i].pos.y]
					= TARGET;
				w->targets[i].dead_time = 0;
				w->targets[i].damage = TARGET_DAMAGE;
				w->targets[i].conn_mask = 0;
				w->targets[i].update_mask = (unsigned)-1;
				w->targets[i].last_change = frame_loops;
			}
	    }
	}
    }

    if (w->options.endOfRoundReset->GetBool()) {
	for (i = 0; i < w->numObjs; i++) {
	    Object *obj = w->objs[i];
	    if (BIT(obj->type, OBJ_SHOT|OBJ_MINE|OBJ_DEBRIS|OBJ_SPARK
			       |OBJ_CANNON_SHOT|OBJ_TORPEDO|OBJ_SMART_SHOT
				   |OBJ_HEAT_SHOT|OBJ_ITEM)) {
		obj->life = 0;
		if (BIT(obj->type, OBJ_TORPEDO|OBJ_SMART_SHOT|OBJ_HEAT_SHOT
				   |OBJ_CANNON_SHOT|OBJ_MINE)) {
		    /* Take care that no new explosions are made. */
		    obj->mass = 0;
		}
	    }
	}
    }

	if (w->roundDelaySend > 0) {
		w->roundDelaySend--;
    }
    if (w->options.roundDelay->GetInt()) {
	/* Hold your horses! The next round will start in a few moments. */
	w->roundDelay = w->options.roundDelay->GetInt() * w->GetFPS();
	/* Send him an extra seconds worth to be sure he gets the 0. */
	w->roundDelaySend = w->roundDelay+w->GetFPS();
	w->roundTime = -1;
	sprintf(msg, "Delaying %d seconds until start of next %s.",
		w->options.roundDelay->GetInt(),
		(BIT(w->rules->mode, TIMING)? "race" : "round"));
	w->BroadcastPlayMessage(msg);
    } else {
	w->roundTime = w->options.maxRoundTime->GetInt()
		* w->roundDelay+w->GetFPS();
    }

    UpdateScoreTable(w);
}

///////////////////////////////////////////////////////////////////////////////
void DeletePlayer(World* w, int ind)
{
	Player*	pl = w->players[ind];
	Object*	obj;
	int		i;
	int		j;
	int		id = pl->id;

	w->ScoreServerPlayerEvent(pl, Quit);
	/* call before important player structures are destroyed */
	pl->LeaveAlliance();

	// The currently 'it' player has left during a tag game. Reset 'it' to NO_ID.
	// A new 'it' player will be chosen in update.
	if (w->options.tagGame->GetBool() && w->tag == pl->id)
		w->tag = NO_ID;

	if (IS_ROBOT_PTR(pl)) {
		w->robotMan->Destroy(ind);
	}

	/* Delete remaining shots */
	for (i = w->numObjs - 1; i >= 0; i--) {
		obj = w->objs[i];
		if (obj->id == id) {
			if (obj->type == OBJ_BALL) {
				w->DeleteShot(i);
				BALL_PTR(obj)->owner = NO_ID;
			}
			else if (BIT(obj->type, OBJ_DEBRIS | OBJ_SPARK)) {
				/* Okay, so you want robot explosions to exist,
				* even if the robot left the game. */
				obj->id = NO_ID;
			}
			else {
				if (!w->options.keepShots->GetBool()) {
					obj->life = 0;
					if (BIT(obj->type,
						OBJ_CANNON_SHOT|OBJ_MINE|OBJ_SMART_SHOT|OBJ_HEAT_SHOT|OBJ_TORPEDO)) 
					{
						obj->mass = 0;
					}
				}
				obj->id = NO_ID;
				if (BIT(obj->type, OBJ_MINE)) {
					MINE_PTR(obj)->owner = NO_ID;
				}
			}
		}
		/* Not our object, check any other conditions that need fixing */
		else {
			if (BIT(obj->type, OBJ_MINE)) {
				MineObject *mine = MINE_PTR(obj);
				if (mine->owner == id) {
					mine->owner = NO_ID;
					if (!w->options.keepShots->GetBool()) {
						obj->life = 0;
						obj->mass = 0;
					}
				}
			}
			else if (BIT(obj->type, OBJ_CANNON_SHOT)) {
				if (!w->options.keepShots->GetBool()) {
					obj->life = 0;
					obj->mass = 0;
				}
			}
			else if (BIT(obj->type, OBJ_BALL)) {
				BALL_PTR(obj)->owner = NO_ID;
				BallObject* ball = BALL_PTR(obj);
				if (ball->owner == id) {
					ball->owner = NO_ID;
				}
			}
			// missle targeted on us?
			else if (BIT(obj->type, OBJ_HEAT_SHOT|OBJ_SMART_SHOT)) {
				if (obj->info == id) {
					obj->info = NO_ID;
				}
			}
		}
	}

	if (pl->num_pulses) {
		for (i = 0; i < w->numPulses; i++) {
			if (w->pulses[i]->id == pl->id) {
				free(w->pulses[i]);
				if (--w->numPulses > i) {
					w->pulses[i] = w->pulses[w->numPulses];
					i--;
				}
			}
		}
		pl->num_pulses = 0;
	}
	Free_ship_shape(pl->ship);

	SoundClose(pl);

	w->numPlayers--;
	if (IS_TANK_PTR(pl)) {
		w->numPseudoPlayers--;
	}

	if (pl->team != TEAM_NOT_SET && !IS_TANK_PTR(pl)) {
		w->teams[pl->team].numMembers--;
		if (w->options.teamShareScore->GetBool())
			TEAM_SCORE(w, pl->team, -(pl->score));		/* recalculate teamscores */
		if (IS_ROBOT_PTR(pl))
			w->teams[pl->team].numRobots--;
	}

	if (IS_ROBOT_PTR(pl)) {
		w->numRobots--;
	}

	/*
	 * Swap entry no 'ind' with the last one.
	 *
	 * Change the Players[] pointer array to have Players[ind] point to
	 * a valid player and move our leaving player to Players[NumPlayers].
	 */
	pl = w->players[w->numPlayers];	/* Swap pointers... */
	w->players[w->numPlayers]	= w->players[ind];
	w->players[ind]	= pl;
	pl = w->players[w->numPlayers];	/* Restore pointer. */

	w->getInd[w->players[ind]->id] = ind;
	w->getInd[w->players[w->numPlayers]->id] = w->numPlayers;

	w->CheckTeamMembers(pl->team);

	for (i = w->numPlayers - 1; i >= 0; i--) {
		if (IS_TANK_IND(w, i)
			&& w->players[i]->lock.pl_id == id) 
		{
			/* remove tanks which were released by this player. */
			if (w->options.keepShots->GetBool()) {
				w->players[i]->lock.pl_id = NO_ID;
			} else {
				DeletePlayer(w, i);
			}
			continue;
		}
		if (BIT(w->players[i]->lock.tagged, LOCK_PLAYER|LOCK_VISIBLE)
			&& (w->players[i]->lock.pl_id == id || w->numPlayers <= 1)) 
		{
			CLR_BIT(w->players[i]->lock.tagged, LOCK_PLAYER|LOCK_VISIBLE);
			CLR_BIT(w->players[i]->used, HAS_TRACTOR_BEAM);
		}
		if (IS_ROBOT_IND(w, i)
		  && w->robotMan->WarOnPlayer(i) == id) {
			w->robotMan->ResetWar(i);
		}
		for (j = 0; j < LOCKBANK_MAX; j++) {
			if (w->players[i]->lockbank[j] == id)
				w->players[i]->lockbank[j] = NOT_CONNECTED;
		}
		for (j = 0; j < MAX_RECORDED_SHOVES; j++) {
			if (w->players[i]->shove_record[j].pusher_id == id) {
				w->players[i]->shove_record[j].pusher_id = NO_ID;
			}
		}
	}

	for (i = w->numPlayers - 1; i >= 0; i--) 
	{
		if (w->players[i]->conn)
		w->players[i]->conn->SendLeave(id);
		else if (IS_TANK_IND(w, i)) 
		{
			if (w->players[i]->lock.pl_id == id) 
			{
				DeletePlayer(w, i);
			}
		}
	}

	release_ID(id);
}
