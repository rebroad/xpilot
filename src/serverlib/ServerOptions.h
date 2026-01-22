/* $Id: ServerOptions.h,v 1.32 2007/01/16 04:31:35 dick Exp $
* Xpilotedit, the cross platform map editor for Xpilot.  Copyright (C) 2001 by
*
*      The XPilot Authors           <xpilot@xpilot.org>
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
* See the file COPYRIGHT.TXT for current copyright information.
*
*/
/*
 $Log: ServerOptions.h,v $
 Revision 1.32  2007/01/16 04:31:35  dick
 Programmable frame reduction for paused players.

 Revision 1.31  2004/05/24 07:41:12  dick
 Add support for 2 serverMessage, which can displayed on a client when he starts up.

 Revision 1.30  2004/05/20 22:27:24  dick
 Attribute showLocks displays in the ScoreTable who everyone is locked on.

 Revision 1.29  2004/04/26 07:07:14  dick
 Define a list of pages that can be displayed on the ScoreTable in the client.
 Each page can be controlled by the server (and the server admin tool).

 Revision 1.28  2004/02/21 06:59:00  dick
 SaveOptions() takes the OptOrigin level we are saving at.

 Revision 1.27  2004/02/05 14:56:58  cppc
 First cut at integrating 'it' play mode.

 Revision 1.26  2004/01/28 00:04:01  dick
 Use the PrefTab enum when talking about which tab a ServerOption is in.

 Revision 1.25  2004/01/19 07:02:32  dick
 LoadOptions() takes a PCSTR not a char*

 Revision 1.24  2004/01/09 17:25:39  dick
 clientPortRange becomes firewallPortList

 Revision 1.23  2004/01/08 18:10:51  dick
 Update to XPilot 4.5.5beta

 Revision 1.22  2002/08/01 14:29:10  dick
 Remove unused option scoreTableFileName

 Revision 1.21  2002/07/19 03:21:20  dick
 Remove NoQuit option.

 Revision 1.20  2002/07/08 06:03:34  dick
 New option saveTuned, which is not so much an option as it is an event.
 By "setting" this option, the server "tunes" it, thus saving the tweaks
 file.  A status message is returned as the value.

 Revision 1.19  2002/06/23 06:30:33  dick
 Add a 'sparse' constructor

 Revision 1.18  2002/05/18 20:55:40  dick
 Update to XPilot-4.5.1

 Revision 1.17  2001/08/26 10:56:12  dick
 Linux cleanups

 Revision 1.16  2001/08/14 21:31:29  dick
 New option anonymousViewing.  When false (default), the player you are
 viewing (eyes) appears in the score table as if you have declared war
 on them.  This lets everyone know who you are watching.

 Revision 1.15  2001/08/13 03:01:17  dick
 serverPassword becomes connectedToPassword

 Revision 1.14  2001/08/12 11:16:22  dick
 new option type valList which is an objlist.

 Revision 1.13  2001/08/11 09:39:33  dick
 new option notifyServerChanges; do we spit a message to the clients when a
 ServerOption changes?

 Revision 1.12  2001/08/10 08:05:29  dick
 new option numControlConnections - number of allowed non-playing connections.

 Revision 1.11  2001/08/08 08:02:57  dick
 Don't run the listeners during construction.

 Revision 1.10  2001/08/07 20:53:50  dick
 Add connectedToPort, the port of the server we are connecting to.

 Revision 1.9  2001/08/05 08:02:25  dick
 ServerOptions needs to know who the parent is.  In server, this is a World.
 define parent as a void* because we don't know what type our parent will be.
 (determined in the subclass)

 Revision 1.8  2001/08/01 19:42:48  dick
 New method Activate(), a partner of Listener().  When an attribute changes
 value, other attributes might be enabled or disabled.  Ex: if teamPlay is
 off, then teamCannons becomes undefined, or disabled.

 Revision 1.7  2001/08/01 17:37:16  dick
 Add ServerOption Listeners.  When a ServerOption is changed, notify all
 registered listeners. (Try opening two prefs windows on the same map).

 Revision 1.6  2001/07/24 09:04:01  dick
 Remove all global options.  Use World.options instead.

 Revision 1.5  2001/07/23 07:30:08  dick
 Support OptOrigin when setting options

 Revision 1.4  2001/07/22 08:11:33  dick
 Add support for setting with origin and override

 Revision 1.3  2001/07/21 06:45:30  dick
 Hide the varInt varString, etc. and force access through the accessor funcs

 Revision 1.2  2001/07/21 03:12:53  jlmiller

 Added all the missing ServerOptions, and all the help, Replaced usage of
 STRING, POSINT etc, with ValType enums. StrToNum is no longer used by xpiloteditso killed it for now.

 Revision 1.1  2001/07/20 05:20:11  dick
 Yet another reorg.  Break server/xpilotedit common files into serverlib.

 Revision 1.2  2001/07/19 07:01:48  dick
 Integrate OptOrigin into our ServerOption.
 stringvar becomes varString, intvar becomes varInt, etc.

 Revision 1.1  2001/07/19 05:00:02  dick
 There is no real reason to have optionsStruct, all of the ServerOption(s)
 are attributes of ServerOptions.  And rename Options.* to ServerOptions.*
 (They could be just Options, but when the client comes along with his
 different format options...)

 Revision 1.2  2001/07/19 04:02:35  dick
 Working on encapsulating options.

 Revision 1.1  2001/07/18 20:15:32  dick
 The first pass at integrating xpilotedit's options with the server options.
 The server does not build yet.  xpilotedit works as before, with the new
 definitions.

 */

/********************OptionObjects************************/
#ifndef	_SERVEROPTIONS_H_
#define	_SERVEROPTIONS_H_

#include <stdio.h>

#include "cstring.h"
#include "const.h"
#include "ServerOption.h"

/*The total number of map options supported...must be updated to add new prefs*/
#define NUMPREFS 311

enum PrefTab {
	TabSys,
	TabMapInfo,
	TabRbt,
	TabVis,
	TabCan,
	TabRnd,
	TabItem,
	TabProb,
	TabScor,
	TabComments,
	TabServer,

	MaxTab
};


class ServerOptions
{
public://member functions
	ServerOptions(); //Default Constructor
	ServerOptions(bool);	// Create a ServerOptions without the big init
	~ServerOptions();

	void*			parent;			// World in server, null in xpilotedit
//	bool			OptionsInit();
	bool			SaveOptions(char *, bool, OptOrigin);
	bool			LoadOptions(PCSTR, OptOrigin, bool loadData);
	bool			AddOption(PCSTR, PCSTR, OptOrigin, bool, bool);
	bool			LoadMapData(PCSTR, OptOrigin, bool output);
	bool			ParseOpenFile(FILE*, OptOrigin);

	int				FindOption(PCSTR);
	ServerOption*	FindOptionByName(PCSTR name);
	bool			GetValue(PCSTR name, String& value);
	bool			SetValue(PCSTR name, PCSTR value, int override, OptOrigin origin);

	bool			SetupDefaultServerOptions();

	int				TuneOption(PCSTR name, PCSTR val);		// server runtime
	int				ListOptions(int i, char* buf);

	// when an option changes, it calls here.  Other options may be
	// enabled/disabled based on this guy (which).
	void			OptionsChanged(ServerOption* which = NULL);

	// attribute accessors
	int				GetFPS() { return(framesPerSecond->GetInt()); };
private:
	bool			ParseLine(FILE* file, int& line, bool loadData, OptOrigin origin);

public:
        ServerOption*	mapName;
        ServerOption*	mapAuthor;
        ServerOption*	mapWidth;
        ServerOption*	mapHeight;
        ServerOptionBlocks*	mapData;
        ServerOption*	gravity;
        ServerOption*	shipMass;
        ServerOption*	ballMass;
        ServerOption*	shotMass;
        ServerOption*	shotSpeed;
        ServerOption*	shotLife;
		ServerOption*	shotHitFuelDrainUsesKineticEnergy;
		ServerOption*	minItemMass;
        ServerOption*	fireRepeatRate;
        ServerOption*	maxRobots;
        ServerOption*	minRobots;
        ServerOption*	robotsTalk;
        ServerOption*	robotsLeave;
        ServerOption*	robotLeaveLife;
        ServerOption*	robotLeaveScore;
        ServerOption*	robotLeaveRatio;
        ServerOption*	robotTeam;
        ServerOption*	restrictRobots;
        ServerOption*	reserveRobotTeam;
        ServerOption*	robotRealName;
        ServerOption*	robotHostName;
        ServerOption*	tankRealName;
        ServerOption*	tankHostName;
        ServerOption*	tankScoreDecrement;
        ServerOption*	turnThrust;
        ServerOption*	selfImmunity;
        ServerOption*	defaultShipShape;
        ServerOption*	tankShipShape;
        ServerOption*	maxPlayerShots;
        ServerOption*	shotsGravity;
        ServerOption*	allowPlayerCrashes;
        ServerOption*	allowPlayerBounces;
        ServerOption*	allowPlayerKilling;
        ServerOption*	allowShields;
        ServerOption*	playerStartsShielded;
        ServerOption*	shotsWallBounce;
        ServerOption*	ballsWallBounce;
		ServerOption*	ballCollisions;
		ServerOption*	ballSparkCollisions;
        ServerOption*	minesWallBounce;
        ServerOption*	itemsWallBounce;
        ServerOption*	missilesWallBounce;
        ServerOption*	sparksWallBounce;
        ServerOption*	debrisWallBounce;
        ServerOption*	asteroidsWallBounce;
        ServerOption*	cloakedExhaust;
        ServerOption*	cloakedShield;
        ServerOption*	maxObjectWallBounceSpeed;
        ServerOption*	maxShieldedWallBounceSpeed;
        ServerOption*	maxUnshieldedWallBounceSpeed;
        ServerOption*	maxShieldedPlayerWallBounceAngle;
        ServerOption*	maxUnshieldedPlayerWallBounceAngle;
        ServerOption*	playerWallBounceBrakeFactor;
        ServerOption*	objectWallBounceBrakeFactor;
        ServerOption*	objectWallBounceLifeFactor;
        ServerOption*	wallBounceFuelDrainMult;
        ServerOption*	wallBounceDestroyItemProb;
        ServerOption*	limitedVisibility;
        ServerOption*	minVisibilityDistance;
        ServerOption*	maxVisibilityDistance;
        ServerOption*	limitedLives;
        ServerOption*	worldLives;
        ServerOption*	endOfRoundReset;
        ServerOption*	resetOnHuman;
		ServerOption*	allowAlliances;
		ServerOption*	announceAlliances;
        ServerOption*	teamPlay;
        ServerOption*	teamCannons;
        ServerOption*	teamFuel;
        ServerOption*	cannonSmartness;
        ServerOption*	cannonsUseItems;
		ServerOption*	cannonsDefend;
		ServerOption*	cannonFlak;
        ServerOption*	cannonDeadTime;
        ServerOption*	keepShots;
        ServerOption*	teamAssign;
        ServerOption*	teamImmunity;
		ServerOption*	teamShareScore;
        ServerOption*	ecmsReprogramMines;
        ServerOption*	ecmsReprogramRobots;
        ServerOption*	targetKillTeam;
        ServerOption*	targetTeamCollision;
        ServerOption*	targetSync;
        ServerOption*	targetDeadTime;
        ServerOption*	treasureKillTeam;
        ServerOption*	captureTheFlag;
        ServerOption*	treasureCollisionDestroys;
        ServerOption*	ballConnectorSpringConstant;
        ServerOption*	ballConnectorDamping;
        ServerOption*	maxBallConnectorRatio;
        ServerOption*	ballConnectorLength;
		ServerOption*	connectorIsString;
        ServerOption*	treasureCollisionMayKill;
        ServerOption*	wreckageCollisionMayKill;
        ServerOption*	asteroidCollisionMayKill;
        ServerOption*	timing;
		ServerOption*	ballRace;
		ServerOption*	ballRaceConnected;
        ServerOption*	edgeWrap;
        ServerOption*	edgeBounce;
        ServerOption*	extraBorder;
        ServerOption*	gravityPoint;
        ServerOption*	gravityAngle;
        ServerOption*	gravityPointSource;
        ServerOption*	gravityClockwise;
        ServerOption*	gravityAnticlockwise;
        ServerOption*	gravityVisible;
        ServerOption*	wormholeVisible;
        ServerOption*	itemConcentratorVisible;
		ServerOption*	asteroidConcentratorVisible;
        ServerOption*	wormTime;
        ServerOption*	framesPerSecond;
        ServerOption*	allowSmartMissiles;
        ServerOption*	allowHeatSeekers;
        ServerOption*	allowTorpedoes;
        ServerOption*	allowNukes;
        ServerOption*	allowClusters;
        ServerOption*	allowModifiers;
        ServerOption*	allowLaserModifiers;
        ServerOption*	allowShipShapes;
        ServerOption*	playersOnRadar;
        ServerOption*	missilesOnRadar;
        ServerOption*	minesOnRadar;
        ServerOption*	nukesOnRadar;
        ServerOption*	treasuresOnRadar;
        ServerOption*	asteroidsOnRadar;
        ServerOption*	distinguishMissiles;
        ServerOption*	maxMissilesPerPack;
        ServerOption*	maxMinesPerPack;
        ServerOption*	identifyMines;
        ServerOption*	shieldedItemPickup;
        ServerOption*	shieldedMining;
        ServerOption*	laserIsStunGun;
        ServerOption*	nukeMinSmarts;
        ServerOption*	nukeMinMines;
        ServerOption*	nukeClusterDamage;
        ServerOption*	mineFuseTime;
        ServerOption*	mineLife;
        ServerOption*	minMineSpeed;
        ServerOption*	missileLife;
        ServerOption*	baseMineRange;
		ServerOption*	mineShotDetonateDistance;
        ServerOption*	shotKillScoreMult;
        ServerOption*	torpedoKillScoreMult;
        ServerOption*	smartKillScoreMult;
        ServerOption*	heatKillScoreMult;
        ServerOption*	clusterKillScoreMult;
        ServerOption*	laserKillScoreMult;
        ServerOption*	tankKillScoreMult;
        ServerOption*	runoverKillScoreMult;
        ServerOption*	ballKillScoreMult;
        ServerOption*	explosionKillScoreMult;
        ServerOption*	shoveKillScoreMult;
        ServerOption*	crashScoreMult;
        ServerOption*	mineScoreMult;
		ServerOption*	selfKillScoreMult;
		ServerOption*	selfDestructScoreMult;
		ServerOption*	unownedKillScoreMult;
        ServerOption*	asteroidPoints;
		ServerOption*	cannonPoints;
		ServerOption*	asteroidMaxScore;
		ServerOption*	cannonMaxScore;
        ServerOption*	movingItemProb;
        ServerOption*	randomItemProb;
        ServerOption*	dropItemOnKillProb;
        ServerOption*	detonateItemOnKillProb;
        ServerOption*	destroyItemInCollisionProb;
		ServerOption*	asteroidItemProb;
		ServerOption*	asteroidMaxItems;
        ServerOption*	itemProbMult;
        ServerOption*	cannonItemProbMult;
        ServerOption*	maxItemDensity;
        ServerOption*	asteroidProb;
        ServerOption*	maxAsteroidDensity;
        ServerOption*	itemConcentratorRadius;
        ServerOption*	itemConcentratorProb;
		ServerOption*	asteroidConcentratorRadius;
		ServerOption*	asteroidConcentratorProb;
        ServerOption*	rogueHeatProb;
        ServerOption*	rogueMineProb;
        ServerOption*	itemEnergyPackProb;
        ServerOption*	itemTankProb;
        ServerOption*	itemECMProb;
        ServerOption*	itemArmorProb;
        ServerOption*	itemMineProb;
        ServerOption*	itemMissileProb;
        ServerOption*	itemCloakProb;
        ServerOption*	itemSensorProb;
        ServerOption*	itemWideangleProb;
        ServerOption*	itemRearshotProb;
        ServerOption*	itemAfterburnerProb;
        ServerOption*	itemTransporterProb;
        ServerOption*	itemMirrorProb;
        ServerOption*	itemDeflectorProb;
        ServerOption*	itemHyperJumpProb;
        ServerOption*	itemPhasingProb;
        ServerOption*	itemLaserProb;
        ServerOption*	itemEmergencyThrustProb;
        ServerOption*	itemTractorBeamProb;
        ServerOption*	itemAutopilotProb;
        ServerOption*	itemEmergencyShieldProb;
        ServerOption*	initialFuel;
        ServerOption*	initialTanks;
        ServerOption*	initialArmor;
        ServerOption*	initialECMs;
        ServerOption*	initialMines;
        ServerOption*	initialMissiles;
        ServerOption*	initialCloaks;
        ServerOption*	initialSensors;
        ServerOption*	initialWideangles;
        ServerOption*	initialRearshots;
        ServerOption*	initialAfterburners;
        ServerOption*	initialTransporters;
        ServerOption*	initialMirrors;
        ServerOption*	maxArmor;
        ServerOption*	initialDeflectors;
        ServerOption*	initialHyperJumps;
        ServerOption*	initialPhasings;
        ServerOption*	initialLasers;
        ServerOption*	initialEmergencyThrusts;
        ServerOption*	initialTractorBeams;
        ServerOption*	initialAutopilots;
        ServerOption*	initialEmergencyShields;
        ServerOption*	maxFuel;
        ServerOption*	maxTanks;
        ServerOption*	maxECMs;
        ServerOption*	maxMines;
        ServerOption*	maxMissiles;
        ServerOption*	maxCloaks;
        ServerOption*	maxSensors;
        ServerOption*	maxWideangles;
        ServerOption*	maxRearshots;
        ServerOption*	maxAfterburners;
        ServerOption*	maxTransporters;
        ServerOption*	maxMirrors;
        ServerOption*	maxDeflectors;
        ServerOption*	maxPhasings;
        ServerOption*	maxHyperJumps;
        ServerOption*	maxEmergencyThrusts;
        ServerOption*	maxLasers;
        ServerOption*	maxTractorBeams;
        ServerOption*	maxAutopilots;
        ServerOption*	maxEmergencyShields;
        ServerOption*	gameDuration;
        ServerOption*	allowViewing;
		ServerOption*	allowFullViewing;
		ServerOption*	viewingReduceDelay;
		ServerOption*	viewingReduceRate;
		ServerOption*	viewingKeepaliveDelay;
		ServerOption*	viewingKeepaliveRate;
		ServerOption*	anonymousViewing;

        ServerOption*	friction;
        ServerOption*	blockFriction;
        ServerOption*	blockFrictionVisible;
        ServerOption*	coriolis;
        ServerOption*	checkpointRadius;
        ServerOption*	raceLaps;
        ServerOption*	lockOtherTeam;
		ServerOption*	showLocks;
        ServerOption*	loseItemDestroys;
        ServerOption*	useWreckage;
        ServerOption*	maxOffensiveItems;
        ServerOption*	maxDefensiveItems;
        ServerOption*	roundDelay;
        ServerOption*	maxRoundTime;
        ServerOption*	roundsToPlay;
		ServerOption*	maxPauseTime;
		ServerOption*	pauseScoreReduce;
		ServerOption*	scoreTableIPVer;
		ServerOption*	scoreTableKills;
		ServerOption*	scoreTableRank;

        ServerOption*	mapFileName;
        ServerOption*	robotFile;
        ServerOption*	idleRun;
        ServerOption*	logRobots;
        ServerOption*	contactPort;
        ServerOption*	serverHost;
        ServerOption*	reportToMetaServer;
        ServerOption*	searchDomainForXPilot;
        ServerOption*	denyHosts;
		ServerOption*	maxClientsPerIP;
        ServerOption*	ignore20MaxFPS;
        ServerOption*	defaultsFileName;
        ServerOption*	passwordFileName;
        ServerOption*	motdFileName;
		ServerOption*	adminMessageFileName;
		ServerOption*	adminMessageFileSizeLimit;
        ServerOption*	maxVisibleObjects;
        ServerOption*	pLockServer;
        ServerOption*	timerResolution;
        ServerOption*	password;
        ServerOption*	clientPortStart;
        ServerOption*	clientPortEnd;
		ServerOption*	firewallPortList;
		ServerOption*	numControlConnections;
		ServerOption*	notifyServerChanges;
		ServerOption*	serverMessage1;
		ServerOption*	serverMessage2;
		ServerOption*	isConnected;
		ServerOption*	saveTuned;
		ServerOption*	refreshValues;
		ServerOption*	connectedToName;
		ServerOption*	connectedToPort;
		ServerOption*	connectedToPassword;
		ServerOption*	tagGame;
		ServerOption*	tagKillItMult;
		ServerOption*	tagItKillMult;

        /*This isn't actually a map or server option, but I
        think it's relevant to the option structure, so here it is.*/
		String	comments; /*Any comments attatched to the options struct.*/

public:
	ServerOption*	prefsArray[NUMPREFS]; //Array of pointers to all the options
	int				numPrefs;

private:
	bool			isInit;		// don't want to call listeners while constructing
	void			Init();
};
#endif	// _SERVEROPTIONS_H_
