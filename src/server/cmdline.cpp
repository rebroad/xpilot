/* $Id: cmdline.cpp,v 1.12 2002/09/01 00:13:46 dick Exp $
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
 *
 *  $Log: cmdline.cpp,v $
 *  Revision 1.12  2002/09/01 00:13:46  dick
 *  Remove obsolete windows #include "winServer.h"
 *
 *  Revision 1.11  2002/08/01 14:31:01  dick
 *  This file is now completly empty.
 *
 *  Revision 1.10  2002/07/23 18:00:57  dick
 *  Use new Conf_server_password_file_name().
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
 *  Revision 1.6  2001/07/22 08:16:56  dick
 *  Commentality (on a dying file...)
 *
 *  Revision 1.5  2001/07/20 05:20:11  dick
 *  Yet another reorg.  Break server/xpilotedit common files into serverlib.
 *
 *  Revision 1.4  2001/07/19 03:58:12  dick
 *  Move the tuners to ServerOption
 *
 *  Revision 1.3  2001/07/18 20:15:32  dick
 *  The first pass at integrating xpilotedit's options with the server options.
 *  The server does not build yet.  xpilotedit works as before, with the new
 *  definitions.
 *
 *  Revision 1.2  2001/07/07 12:00:42  dick
 *  Rename classes to C++ "Style".  old World becomes theWorld.
 *
 */
/* Options parsing code contributed by Ted Lemon <mellon@ncd.com> */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "version.h"
#include "config.h"
#include "const.h"
#include "global.h"
#include "proto.h"
#include "defaults.h"
#include "error.h"
#include "portability.h"
#include "checknames.h"
#include "commonproto.h"

char cmdline_version[] = VERSION;


//DFLOAT		Gravity;		/* Power of gravity */
//DFLOAT		ShipMass;		/* Default mass of ship */
//DFLOAT		ballMass;		/* Default mass of balls */
//DFLOAT		ShotsMass;		/* Default mass of shots */
//DFLOAT		ShotsSpeed;		/* Default speed of shots */
//int		ShotsLife;		/* Default number of ticks */
					/* each shot will live */
//int		maxRobots;		/* How many robots should enter */
//int		minRobots;		/* the game? */
//char		*robotFile;		/* Filename for robot parameters */
//int		robotsTalk;		/* Do robots talk? */
//int		robotsLeave;		/* Do robots leave at all? */
//int		robotLeaveLife;		/* Max life per robot (0=off)*/
//int		robotLeaveScore;	/* Min score for robot to live (0=off)*/
//int		robotLeaveRatio;	/* Min ratio for robot to live (0=off)*/
//int		robotTeam;		/* Team for robots */
//bool		restrictRobots;		/* Restrict robots to robotTeam? */
//bool		reserveRobotTeam;	/* Allow only robots in robotTeam? */
//int		ShotsMax;		/* Max shots pr. player */
//bool		ShotsGravity;		/* Shots affected by gravity */
//int		fireRepeatRate;		/* Frames per autorepeat fire (0=off) */

//bool		RawMode;		/* Let robots live even if there */
					/* are no players logged in */
//bool		NoQuit;			/* Don't quit even if there are */
					/* no human players playing */
//bool		logRobots;		/* log robots coming and going */
//char		*mapFileName;		/* Name of mapfile... */
//char		*mapData;		/* Raw map data... */
//int		mapWidth;		/* Width of the universe */
//int		mapHeight;		/* Height of the universe */
//char		*mapName;		/* Name of the universe */
//char		*mapAuthor;		/* Name of the creator */
//int		contactPort;		/* Contact port number */
//char		*serverHost;		/* Host name (for multihomed hosts) */

//bool		crashWithPlayer;	/* Can players overrun other players? */
//bool		bounceWithPlayer;	/* Can players bounce other players? */
//bool		playerKillings;		/* Can players kill each other? */
//bool		playerShielding;	/* Can players use shields? */
//bool		playerStartsShielded;	/* Players start with shields up? */
//bool		shotsWallBounce;	/* Do shots bounce off walls? */
//bool		minesWallBounce;	/* Do mines bounce off walls? */
//bool		itemsWallBounce;	/* Do items bounce off walls? */
//bool		missilesWallBounce;	/* Do missiles bounce off walls? */
//bool		sparksWallBounce;	/* Do sparks bounce off walls? */
//bool		debrisWallBounce;	/* Do sparks bounce off walls? */
//bool		ballsWallBounce;	/* Do balls bounce off walls? */
//bool		asteroidsWallBounce;	/* Do asteroids bounce off walls? */
//bool		cloakedExhaust;		/* Generate exhaust when cloaked? */
//bool		cloakedShield;		/* Allowed to use shields when cloaked? */
//bool		ecmsReprogramMines;	/* Do ecms reprogram mines? */
//bool		ecmsReprogramRobots;	/* Do ecms reprogram robots? */
//DFLOAT		maxObjectWallBounceSpeed;	/* max object bounce speed */
//DFLOAT		maxShieldedWallBounceSpeed;	/* max shielded bounce speed */
//DFLOAT		maxUnshieldedWallBounceSpeed; /* max unshielded bounce speed */
//DFLOAT		maxShieldedWallBounceAngle;	/* max angle for landing */
//DFLOAT		maxUnshieldedWallBounceAngle;	/* max angle for landing */
//DFLOAT		playerWallBrakeFactor;	/* wall lowers speed if less than 1 */
//DFLOAT		objectWallBrakeFactor;	/* wall lowers speed if less than 1 */
//DFLOAT		objectWallBounceLifeFactor;	/* reduce object life */
//DFLOAT		wallBounceFuelDrainMult;/* Wall bouncing fuel drain factor */
//DFLOAT		wallBounceDestroyItemProb;/* Wall bouncing item destroy prob */

//bool		limitedVisibility;	/* Is visibility limited? */
//DFLOAT		minVisibilityDistance;	/* Minimum visibility when starting */
//DFLOAT		maxVisibilityDistance;	/* Maximum visibility */
//bool		limitedLives;		/* Are lives limited? */
//int		worldLives;		/* If so, what's the max? */
//bool		endOfRoundReset;	/* Reset the world when round ends? */
//int		resetOnHuman;		/* Last human to reset round for */
//bool		teamPlay;		/* Are teams allowed? */
//bool		teamFuel;		/* Do fuelstations belong to teams? */
//bool		teamCannons;		/* Do cannons belong to teams? */
//int		cannonSmartness;	/* Accuracy of cannonfire */
//bool		cannonsUseItems;	/* Do cannons use items? */
//int		cannonDeadTime;		/* How long do cannons stay dead? */
//bool		keepShots;		/* Keep shots when player leaves? */
//bool		timing;			/* Is this a race? */
//bool		edgeWrap;		/* Do objects wrap when they cross
//					   the edge of the Universe? */
//bool		edgeBounce;		/* Do objects bounce when they hit
//					   the edge of the Universe? */
//bool		extraBorder;		/* Give map an extra border? */
//ipos		gravityPoint;		/* Where does gravity originate? */
//DFLOAT		gravityAngle;		/* If gravity is along a uniform line,
//					   at what angle is that line? */
//bool		gravityPointSource;	/* Is gravity a point source? */
//bool		gravityClockwise;	/* If so, is it clockwise? */
//bool		gravityAnticlockwise;	/* If not clockwise, anticlockwise? */
//bool		gravityVisible;		/* Is gravity visible? */
//bool		wormholeVisible;	/* Are wormholes visible? */
//bool		itemConcentratorVisible;/* Are itemconcentrators visible? */
//int		wormTime;
//char		*defaultsFileName;	/* Name of defaults file... */
//char		*passwordFileName;	/* Name of password file... */
//char		*motdFileName;		/* Name of motd file */
//char		*scoreTableFileName;	/* Name of score table file */

//int		nukeMinSmarts;		/* minimum smarts for a nuke */
//int		nukeMinMines;		/* minimum number of mines for nuke */
//DFLOAT		nukeClusterDamage;	/* multiplier for damage from nuke */
					/* cluster debris, reduces number */
					/* of particles by similar amount */
//int		mineFuseTime;		/* Length of time mine is fused */
//int		mineLife;		/* lifetime of mines */
//DFLOAT		minMineSpeed;		/* minimum speed of mines */
//int		missileLife;		/* lifetime of missiles */
//int		baseMineRange;		/* Distance from base mines may be used */

//DFLOAT		shotKillScoreMult;
//DFLOAT		torpedoKillScoreMult;
//DFLOAT		smartKillScoreMult;
//DFLOAT		heatKillScoreMult;
//DFLOAT		clusterKillScoreMult;
//DFLOAT		laserKillScoreMult;
//DFLOAT		tankKillScoreMult;
//DFLOAT		runoverKillScoreMult;
//DFLOAT		ballKillScoreMult;
//DFLOAT		explosionKillScoreMult;
//DFLOAT		shoveKillScoreMult;
//DFLOAT		crashScoreMult;
//DFLOAT		mineScoreMult;
//bool		asteroidScoring;

//DFLOAT 		movingItemProb;		/* Probability for moving items */
//DFLOAT		randomItemProb;		/* Probability for random-appearing items */
//DFLOAT		dropItemOnKillProb;	/* Probability for players items to */
					/* drop when player is killed */
//DFLOAT		detonateItemOnKillProb;	/* Probaility for remaining items to */
					/* detonate when player is killed */
//DFLOAT		destroyItemInCollisionProb;
//DFLOAT		rogueHeatProb;          /* prob. that unclaimed rocketpack */
//DFLOAT		rogueMineProb;          /* or minepack will "activate" */
//DFLOAT		itemProbMult;
//DFLOAT		cannonItemProbMult;
//DFLOAT		maxItemDensity;
//DFLOAT		maxAsteroidDensity;
//int		itemConcentratorRadius;
//DFLOAT		itemConcentratorProb;

//bool		allowSmartMissiles;
//bool		allowHeatSeekers;
//bool		allowTorpedoes;
//bool		allowNukes;
//bool		allowClusters;
//bool		allowModifiers;
//bool		allowLaserModifiers;
//bool		allowShipShapes;

//bool		playersOnRadar;		/* Are players visible on radar? */
//bool		missilesOnRadar;	/* Are missiles visible on radar? */
//bool		minesOnRadar;		/* Are mines visible on radar? */
//bool		nukesOnRadar;		/* Are nuke weapons radar visible? */
//bool		treasuresOnRadar;	/* Are treasure balls radar visible? */
//bool		asteroidsOnRadar;	/* Are asteroids radar visible? */
//bool		distinguishMissiles;	/* Smarts, heats & torps look diff.? */
//int		maxMissilesPerPack;	/* Number of missiles per item. */
//int		maxMinesPerPack;	/* Number of mines per item. */
//bool		identifyMines;		/* Mines have names displayed? */
//bool		shieldedItemPickup;	/* Pickup items with shields up? */
//bool		shieldedMining;		/* Detach mines with shields up? */
//bool		laserIsStunGun;		/* Is the laser a stun gun? */
//bool		reportToMetaServer;	/* Send status to meta-server? */
//bool		searchDomainForXPilot;	/* Do a DNS lookup for XPilot.domain? */
//char		*denyHosts;		/* Computers which are denied service */
//DFLOAT		gameDuration;		/* total duration of game in minutes */
//bool		allowViewing;		/* Are players allowed to watch others? */

//bool		teamAssign;		/* Assign player to team if not set? */
//bool		teamImmunity;		/* Is team immune from player action */

//bool		targetKillTeam;		/* if your target explodes, you die? */
//bool		targetTeamCollision;	/* Does team collide with target? */
//bool		targetSync;		/* all targets reappear together */
//int		targetDeadTime;		/* How long do targgets stay dead? */
//bool		treasureKillTeam;	/* die if treasure is destroyed? */
//bool		captureTheFlag;		/* must treasure be safe to cash balls? */
//bool		treasureCollisionDestroys;
//bool		treasureCollisionMayKill;
//bool		wreckageCollisionMayKill;
//bool		asteroidCollisionMayKill;

//DFLOAT		ballConnectorSpringConstant;
//DFLOAT		ballConnectorDamping;
//DFLOAT		maxBallConnectorRatio;
//DFLOAT		ballConnectorLength;

//DFLOAT		friction;		/* friction only affects ships */
//DFLOAT		blockFriction;		/* friction in friction blocks */
//bool		blockFrictionVisible;	/* if yes, friction blocks are decor; */
					/* if no, friction blocks are space */
//int		coriolis;		/* angle velocity turns each frame */
//DFLOAT		checkpointRadius;      	/* in blocks */
//int		raceLaps;		/* how many laps per race */
//bool		lockOtherTeam;		/* lock ply from other teams when dead? */
//bool		loseItemDestroys; 	/* destroy item on loseItem? */
//bool		useWreckage;		/* destroyed ships leave wreckage? */

//int		maxOffensiveItems;	/* how many offensive and defensive */
//int		maxDefensiveItems;	/* items can player carry */

//int		roundDelaySeconds;	/* delay before start of each round */
//int		maxRoundTime;		/* max. duration of each round */
//int		roundsToPlay;		/* # of rounds to play. */
//int		roundsPlayed;		/* # of rounds played sofar. */

//int		maxVisibleObject;	/* how many objects a player can see */
//bool		pLockServer;		/* Is server swappable out of memory?  */
//bool		ignore20MaxFPS;		/* ignore client maxFPS request if 20 */
//int		timerResolution;	/* OS timer resolution (times/sec) */
//char		*password;		/* password for operator status */
//int		clientPortStart;	/* First UDP port for clients */
//int		clientPortEnd;		/* Last one (these are for firewalls) */

//char		*robotRealName;		/* Real name for robot */
//char		*robotHostName;		/* Host name for robot */

//char		*tankRealName;		/* Real name for tank */
//char		*tankHostName;		/* Host name for tank */
//int		tankScoreDecrement;	/* Amount by which the tank's score */
					/* is decreased from the player's */

//bool		turnThrust;		/* Does turning use fuel and shoot sparks? */
//bool		selfImmunity;		/* Are players immune to their own weapons? */

//char		*defaultShipShape;	/* What ship shape is used for players */
					/* who do not define their own? */
//char		*tankShipShape;		/* What ship shape is used for tanks? */



