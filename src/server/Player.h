/* $Id: Player.h,v 1.24 2007/02/12 07:55:27 dick Exp $
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2001 by
 *
 *      The XPilot Authors           <xpilot@xpilot.org>
 *                                   http://www.xpilot.org/
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
 *  $Log: Player.h,v $
 *  Revision 1.24  2007/02/12 07:55:27  dick
 *  Support RobotWatchDeco, which is decorated shapes displayed on the playfield.
 *
 *  Revision 1.23  2007/01/17 09:10:45  dick
 *  RobotWatch for a player (and his connection, yuck) are managed here.
 *  robotWatchList contains a list of RobotWatch.
 *  During a Robot's play cycle, he may stuff data in this list.
 *  Once his play cycle is over, the list is locked as full until the connection
 *  emptys it.
 *
 *  Revision 1.22  2007/01/13 22:27:28  dick
 *  Now, a Robot* is a member of Player.
 *  Robot contains the interface to drive a robot.
 *  Robot4 is the old robotdef.cpp handler.
 *
 *  Revision 1.21  2004/06/03 06:06:14  dick
 *  shipobj becomes ShipObj.
 *
 *  Revision 1.20  2004/05/22 15:15:08  dick
 *  myEyes becomes a id instead of an ind
 *
 *  Revision 1.19  2004/05/20 21:43:24  dick
 *  Add IsPaused() and IsRobot()
 *
 *  Revision 1.18  2004/05/07 04:27:41  dick
 *  _visibility becomes Visibility.  global updateScores becomes a member of World.
 *  Handle rank/rate data from the scoreserver and send it to the client.
 *
 *  Revision 1.17  2004/02/23 07:19:55  dick
 *  Move name to the top of the class so it shows up first in debug displays
 *  (instead of buried way down there).  It's most helpful to know who we're
 *  looking at.
 *
 *  Revision 1.16  2004/02/18 00:58:34  dick
 *  Delete_player becomes DeletePlayer
 *
 *  Revision 1.15  2004/01/17 19:37:17  dick
 *  WildMap is now in the common library. Control and server use this,
 *  instead of each having it's own copy.
 *
 *  Revision 1.14  2004/01/08 18:10:51  dick
 *  Update to XPilot 4.5.5beta
 *
 *  Revision 1.13  2002/09/11 16:42:04  dick
 *  Here endeth the server encapsulation task.
 *  theWorld is now an array of World(s) and get loaded dynamically.
 *
 *  Revision 1.12  2002/07/14 19:03:47  dick
 *  type_ext becomes enum PlayerType
 *
 *  Revision 1.11  2002/06/25 04:57:32  dick
 *  Player didn't really need to talk to the ScoreServer.
 *
 *  Revision 1.10  2002/06/23 06:37:46  dick
 *  Some basic ScoreServer handling.
 *
 *  Revision 1.9  2002/06/01 06:06:47  dick
 *  Encapsulate (almost) everything.  Get rid of (almost) all refs to theWorld.
 *
 *  Revision 1.8  2002/05/22 05:09:00  dick
 *  Update to XPilot r4_5_4alpha.
 *
 *  Revision 1.7  2002/05/18 20:55:38  dick
 *  Update to XPilot-4.5.1
 *
 *  Revision 1.6  2001/08/14 21:33:07  dick
 *  new attribute myEyes.  Who am i looking at?
 *
 *  Revision 1.5  2001/08/10 08:11:37  dick
 *  New class ConnectionPlayer subclasses Connection to provide player
 *  functions.  New class Frame subclasses ConnectionPlayer which provides
 *  Frame update functionality.
 *
 *  Revision 1.4  2001/08/09 07:43:00  dick
 *  SetMessage becomes member function of Player.
 *
 *  Revision 1.3  2001/08/05 08:05:36  dick
 *  Add Tuners to the server.  ComputeSensorRange(), AddTank(), RemoveTank()
 *  become members of Player.  ComputeGravity*() become members of World.
 *
 *  Revision 1.2  2001/08/04 17:34:07  dick
 *  TransportToHome and DoAutopilot become members of Player.
 *  RECOVERY_DELAY needs * GetFPS().
 *
 *  Revision 1.1  2001/07/15 01:24:27  dick
 *  Break Player out to his own file
 *
 */

#ifndef	_PLAYER_H_
#define	_PLAYER_H_

#include "Obj.h"
#include "PacketCtl.h"	// For PlayerType
#include "rules.h"

class World;
/*
 * Shove-information.
 *
 * This is for keeping a record of the last N times the player was shoved,
 * for assigning wall-smash-blame, where N=MAX_RECORDED_SHOVES.
 */
#define MAX_RECORDED_SHOVES 4

class Shove
{
public:
    int		pusher_id;
    int		time;
};

class Robot;
class ConnectionPlayer;

class Player : public ObjectBase
{
public:
    Player() {}
	int		Init(ShipObj *ship);
	int		Ind();
	void	PickStartpos(bool newbie = FALSE);
	void	GoHome();
	void	TransportToHome();
	int		TeamImmune(int id);

	void	DoAutopilot();
	void	DoHyperjump();

	void	ComputeSensorRange();
	void	SetMass();
	void	AddTank(long tank_fuel);
	void	RemoveTank(int which_tank);
	void	SetMessage(PCSTR message);
	void	SetRobotWatch(int y, PCSTR message);

	void	SetEyes(int id);		/* Tell everyone who i'm looking at */
	void	SetLock(int id);
	void	HitArmor();
	void	UsedKill();
	void	Kill();					// kill this player
	void	DeathReset();
	void	DetachBall(int obj);
	bool	IsRobot();
	bool	IsPaused();
private:
	void	InitFuel(long total_fuel);

// Alliance
public:
	void	InvitePlayer(int ally_ind);
	void	CancelInvitation();
	void	RefuseAlliance(int ally_ind);
	void	RefuseAllAlliances();
	void	AcceptAlliance(int ally_ind);
	void	AcceptAllAlliances();
	void	JoinAlliance(int ally_ind);
	int		LeaveAlliance();
	void	AlliancePlayerList();

// shot.cpp
public:
	void	FireShot(int type, int dir);
	void	PlaceMine();
	void	PlaceMovingMine();
	void	DetonateMines();
	void	FireNormalShots();
	void	FireLaser();
	void	FireEcm();
private:
	void	FireMainShot(int type, int dir);
	void	FireLeftShot(int type, int dir, int gun);
	void	FireRightShot(int type, int dir, int gun);
	void	FireLeftRShot(int type, int dir, int gun);
	void	FireRightRShot(int type, int dir, int gun);

// ship.cpp
public:
	void	Thrust();
	void	TurnThrust(int num_sparks);
	void	ExplodeFighter();

// update.cpp
	void	Deflector(bool on);	// turn deflector on and off
	void	Phasing (bool on);
	void	Cloak(bool on);
	void	EmergencyShield(bool on);
	void	EmergencyThrust(bool on);
	void	Autopilot(bool on);


public:
	PlayerType	playerType;			// PlayerHuman, PlayerRobot, PlayerTank

    char	name[MAX_CHARS];		/* Nick-name of player */
    char	realname[MAX_CHARS];	/* Real name of player */
    char	hostname[MAX_CHARS];	/* Hostname of client player uses */

    DFLOAT	turnspeed;				/* How fast player acc-turns */
    DFLOAT	velocity;				/* Absolute speed */

    int		kills;					/* Number of kills this round */
    int		deaths;					/* Number of deaths this round */

    long	used;					/** Items you use **/
    long	have;					/** Items you have **/

	int		myEyes;					/* who am i looking at? */
    int		shield_time;			/* Shields if no playerShielding */
    PlayerFuel	fuel;				/* ship tanks and the stored fuel */
    DFLOAT	emptymass;				/* Mass of empty ship */
    DFLOAT	float_dir;				/* Direction, in float var */
    DFLOAT	turnresistance;			/* How much is lost in % */
    DFLOAT	turnvel;				/* Current velocity of turn (right) */
    DFLOAT	oldturnvel;				/* Last velocity of turn (right) */
    DFLOAT	turnacc;				/* Current acceleration of turn */
    DFLOAT	score;					/* Current score of player */
    DFLOAT	prev_score;				/* Last score that has been updated */
    int		prev_life;				/* Last life that has been updated */
	int		rank;					// Current rank of player
	int		prevRank;				// Last rank that was updated
	DFLOAT	rate;					// Current rating of player
	DFLOAT	prevRate;				// Last rate that was updated

    ShipObj* ship;					/* wire model of ship shape */
    DFLOAT	power;					/* Force of thrust */
    DFLOAT	power_s;				/* Saved power fiks */
    DFLOAT	turnspeed_s;			/* Saved turnspeed */
    DFLOAT	turnresistance_s;		/* Saved (see above) */
    DFLOAT	sensor_range;			/* Range of sensors (radar) */
    int		shots;					/* Number of active shots by player */
    int		missile_rack;			/* Next missile rack to be active */

    int		num_pulses;				/* Number of laser pulses in the air. */

    int		emergency_thrust_left;	/* how much emergency thrust left */
    int		emergency_thrust_max;	/* maximum time left */
    int		emergency_shield_left;	/* how much emergency shield left */
    int		emergency_shield_max;	/* maximum time left */
    int		phasing_left;			/* how much time left */
    int		phasing_max;			/* maximum time left */

    int		item[NUM_ITEMS];		/* for each item type how many */
    int		lose_item;				/* which item to drop */
    int		lose_item_state;		/* lose item key state, 2=up,1=down */

    DFLOAT	auto_power_s;			/* autopilot saves of current */
									/* power, turnspeed and */
    DFLOAT	auto_turnspeed_s;		/* turnresistance settings. Restored */
    DFLOAT	auto_turnresistance_s;	/* when autopilot turned off */
    modifiers	modbank[NUM_MODBANKS];	/* useful modifier settings */
    bool	tractor_is_pressor;		/* on if tractor is pressor */
	int		shot_max;				/* Maximum number of shots active */
    long	shot_time;				/* Time of last shot fired by player */
    int		repair_target;			/* Repairing this target */
    int		fs;						/* Connected to fuel station fs */
    int		check;					/* Next check point to pass */
    int		prev_check;				/* Previous check point for score */
    int		time;					/* The time a player has used */
    int		round;					/* Number of rounds player have done */
    int		prev_round;				/* Previous rounds value for score */
    int		best_lap;				/* Players best lap time */
    int		last_lap;				/* Time on last pass */
    int		last_lap_time;			/* What was your last pass? */
    int		last_check_dir;			/* player dir at last checkpoint */
    long	last_wall_touch;		/* last time player touched a wall */

    int		home_base;				/* Num of home base */
    struct {
		int	tagged;					/* Flag, what is tagged? */
		int	pl_id;					/* Tagging player id */
		DFLOAT	distance;			/* Distance to object */
    } lock;
    int		lockbank[LOCKBANK_MAX]; /* Saved player locks */

    u_byte	dir;					/* Direction of acceleration */
    u_byte	unused1;				/* padding for alignment */
    char	mychar;					/* Special char for player */
    char	prev_mychar;			/* Special char for player */
    unsigned short	pseudo_team;		/* Which team for detaching tanks */
	int		alliance;				// member of which alliance?
	int		prev_alliance;			// prev. alliance for score
	int		invite;					// Invitation for alliance
    BallObject*	ball;

    /*
     * Pointer to Robot object (dynamically allocated).
     * Only used in robot code.
     */
    Robot*	robot;
	int		robotWatchPage;			// which robot logic page am i looking at? (if any)
	ObjList	robotWatchList;			// list of RobotWatch
	ObjList	robotWatchDecoList;		// list of RobotWatchDeco
	bool	robotWatchFull;			// The list is data to be emptied.
									// Robot is locked from putting in more data until
									// this latch is cleared
    /*
     * A record of who's been pushing me (a circular buffer).
     */
    Shove		shove_record[MAX_RECORDED_SHOVES];
    int         shove_next;

    Visibility*	visibility;

    int		updateVisibility, forceVisible, damaged;
    int		wormDrawCount, wormHoleHit, wormHoleDest;
    int		stunned;

    int		last_target_update;		/* index of last updated target */
    int		last_cannon_update;		/* index of last updated cannon */
    int		last_fuel_update;		/* index of last updated fuel */
    int		last_wormhole_update;	/* index of last updated wormhole */

    int		ecmcount;				/* number of active ecms */

	ConnectionPlayer*	conn;		/* connection, NULL if robot */
    unsigned	version;			/* XPilot version number of client */

    BITV_DECL(last_keyv, NUM_KEYS);	/* Keyboard state */
    BITV_DECL(prev_keyv, NUM_KEYS);	/* Keyboard state */

    long	frame_last_busy;		/* When player touched keyboard. */

    void	*audio;					/* audio private data */

    int		player_fps;				/* FPS that this player can do */
    int		player_round;			/* Divisor for player FPS calculation */
    int		player_count;			/* Player's current frame count */

    int		isowner;				/* If player started this server. */
    int		isoperator;				/* If player has operator privileges. */
};

inline bool Player::IsRobot() { return (playerType == PlayerRobot); };
inline bool Player::IsPaused() { return (BIT(status, PAUSE) ? true : false); };

extern void Alloc_players(World* w, int number);
extern void Free_players(World* w);
extern void UpdateScoreTable(World* w);
extern void ResetAllPlayers(World* w);
extern void DeletePlayer(World* w, int ind);

#endif		// _PLAYER_H_
