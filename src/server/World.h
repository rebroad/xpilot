/* $Id: World.h,v 1.46 2007/01/10 18:14:47 dick Exp $
 *
 * World - The primary class that defines the XPilot world.
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
 *  $Log: World.h,v $
 *  Revision 1.46  2007/01/10 18:14:47  dick
 *  All robot actions are now handled through RobotMan.
 *  There is one RobotMan per World.
 *
 *  Revision 1.45  2004/05/30 16:16:27  dick
 *  Properly initialize Item
 *
 *  Revision 1.44  2004/05/07 04:27:41  dick
 *  _visibility becomes Visibility.  global updateScores becomes a member of World.
 *  Handle rank/rate data from the scoreserver and send it to the client.
 *
 *  Revision 1.43  2004/04/16 21:07:59  dick
 *  NetServer becomes a little UDP engine that can be plugged into different apps.
 *
 *  Revision 1.42  2004/03/04 03:47:31  dick
 *  Save off the version that the client reported as origVersion.
 *  This is for stats only and doesn't affect gameplay.
 *
 *  Revision 1.41  2004/02/20 06:27:47  dick
 *  SendPlayMessage becomes BroadcastPlayMessage() because it sends a play message
 *  to all connections.
 *
 *  Revision 1.40  2004/02/16 14:29:44  dick
 *  server/sched.cpp and friends become serverlib/NetServer
 *  which disconnects the udp listen and receive functionality from the game.
 *
 *  Revision 1.39  2004/02/06 21:14:41  cppc
 *  Some changes to 'it'.
 *  * Pausing player loses 'it'.
 *  * Random 'it' selection will not choose paused player.
 *  ** Added World::RandomPlayerWithStatus(mask, bits) to find random player matching status.
 *  * Robots don't leave if they're 'it'.
 *
 *  Revision 1.38  2004/02/05 14:56:58  cppc
 *  First cut at integrating 'it' play mode.
 *
 *  Revision 1.37  2004/01/21 17:53:21  dick
 *  Open the localAddr always on 15345.  Otherwise the localCtl can't connect.
 *
 *  Revision 1.36  2004/01/16 17:32:22  dick
 *  Fix ReportToMetaTuner().  If initially disabled, enabling reportToMeta while
 *  running requires calling MetaInit() to lookup the metaservers.
 *
 *  Revision 1.35  2004/01/09 17:33:58  dick
 *  Add a firewallPortList and initialize it with our game defaults.
 *
 *  Revision 1.34  2004/01/09 05:28:25  dick
 *  When a Connection is destroyed, call World::NumLogoutsBump so meta will be
 *  notified during the loop.  Input becomes a member of World.
 *
 *  Revision 1.33  2004/01/08 18:10:51  dick
 *  Update to XPilot 4.5.5beta
 *
 *  Revision 1.32  2002/09/16 21:50:39  dick
 *  Get rid of STR80.  Stringize class ServerT.
 *
 *  Revision 1.31  2002/09/11 16:42:04  dick
 *  Here endeth the server encapsulation task.
 *  theWorld is now an array of World(s) and get loaded dynamically.
 *
 *  Revision 1.30  2002/08/01 14:33:40  dick
 *  SetPlayerScore also receives a cookie from the ScoreServer
 *
 *  Revision 1.29  2002/07/14 19:05:24  dick
 *  When talking to the ScoreServer about PCSTR nick, always mention the PlayerType too.
 *
 *  Revision 1.28  2002/07/12 15:36:48  dick
 *  Open a listen port on localhost (127.0.0.1) as well as any requested.
 *
 *  Revision 1.27  2002/07/09 21:44:29  dick
 *  Send the player type (human,robot,tank) with the PlayerEvent to the ScoreServer.
 *
 *  Revision 1.26  2002/07/08 05:58:50  dick
 *  BroadcastOption(ServerOption*) sends this option to all controls who care.
 *
 *  Revision 1.25  2002/06/30 07:04:29  dick
 *  Add some overloaded variations to ScoreServerScoreEvent() to handle different
 *  types of parameters.
 *
 *  Revision 1.24  2002/06/25 06:02:01  dick
 *  gcc won't play with the anonymous enum, so include PacketCtl.h :(
 *
 *  Revision 1.23  2002/06/25 05:05:23  dick
 *  Using World.ScoreServer.cpp, hide the ScoreServer from the rest of the system.
 *
 *  Revision 1.22  2002/06/23 06:32:18  dick
 *  Add a ScoreServer object
 *
 *  Revision 1.21  2002/06/02 05:48:54  dick
 *  EndGame takes an optional string, which gets sent to the connections.
 *
 *  Revision 1.20  2002/06/01 06:06:48  dick
 *  Encapsulate (almost) everything.  Get rid of (almost) all refs to theWorld.
 *
 *  Revision 1.19  2002/05/18 20:55:38  dick
 *  Update to XPilot-4.5.1
 *
 *  Revision 1.18  2001/09/03 00:51:50  dick
 *  Tuners cleanup.
 *
 *  Revision 1.17  2001/08/31 20:58:13  dick
 *  Server needs to be able to receive reliable packets as well as send them.
 *  Use the new emh error handling.
 *
 *  Revision 1.16  2001/08/12 11:18:20  dick
 *  ConnectionControl uses dpy, the display name, as the name of the control type.
 *  i.e. serverctl, logctl, scorectl.
 *
 *  Revision 1.15  2001/08/11 09:38:20  dick
 *  new class ConnectionControl, a Connection for a control.
 *
 *  Revision 1.14  2001/08/10 08:11:37  dick
 *  New class ConnectionPlayer subclasses Connection to provide player
 *  functions.  New class Frame subclasses ConnectionPlayer which provides
 *  Frame update functionality.
 *
 *  Revision 1.13  2001/08/09 07:43:34  dick
 *  New class Connection
 *
 *  Revision 1.12  2001/08/06 05:41:50  dick
 *  struct sock_t becomes class Sock
 *
 *  Revision 1.11  2001/08/05 08:05:36  dick
 *  Add Tuners to the server.  ComputeSensorRange(), AddTank(), RemoveTank()
 *  become members of Player.  ComputeGravity*() become members of World.
 *
 *  Revision 1.10  2001/07/24 09:03:59  dick
 *  Remove all global options.  Use World.options instead.
 *
 *  Revision 1.9  2001/07/23 21:08:36  dick
 *  Encapsulate ShipMass, ShotsMax, ShotsLife, ShotsMass, FPS.
 *
 *  Revision 1.8  2001/07/23 07:38:42  dick
 *  Fold contact, metaserver, netserver, rules, and server to be members of World.
 *
 *  Revision 1.7  2001/07/22 08:16:14  dick
 *  Fix Parser() to be a member of World
 *
 *  Revision 1.6  2001/07/20 05:20:10  dick
 *  Yet another reorg.  Break server/xpilotedit common files into serverlib.
 *
 *  Revision 1.5  2001/07/19 07:01:48  dick
 *  Integrate OptOrigin into our ServerOption.
 *  stringvar becomes varString, intvar becomes varInt, etc.
 *
 *  Revision 1.4  2001/07/19 05:00:02  dick
 *  There is no real reason to have optionsStruct, all of the ServerOption(s)
 *  are attributes of ServerOptions.  And rename Options.* to ServerOptions.*
 *  (They could be just Options, but when the client comes along with his
 *  different format options...)
 *
 *  Revision 1.3  2001/07/19 03:57:43  dick
 *  World contains Options
 *
 *  Revision 1.2  2001/07/18 20:15:32  dick
 *  The first pass at integrating xpilotedit's options with the server options.
 *  The server does not build yet.  xpilotedit works as before, with the new
 *  definitions.
 *
 *  Revision 1.1  2001/07/10 08:06:16  dick
 *  map becomes World.  A tough choice... The object was called world but the
 *  functions were called Map.
 *
 *  Revision 1.2  2001/07/07 12:00:42  dick
 *  Rename classes to C++ "Style".  old World becomes theWorld.
 *
 */

#ifndef	MAP_H
#define	MAP_H

#ifndef TYPES_H
/* need position */
#include "types.h"
#endif
#ifndef RULES_H
/* need rules_t */
#include "rules.h"
#endif
#ifndef ITEM_H
/* need NUM_ITEMS */
#include "item.h"
#endif

// temp until we move collision out.
#include "proto.h"

#include "ServerOptions.h"
#include "Sock.h"
#include "Sockbuf.h"

#include "PacketCtl.h"

class Object;
class Cannon;
class move_state_t;
class CellDist;
class CellOffset;
class RobotMan;

#define SPACE			0
#define BASE			1
#define FILLED			2
#define REC_LU			3
#define REC_LD			4
#define REC_RU			5
#define REC_RD			6
#define FUEL			7
#define CANNON			8
#define CHECK			9
#define POS_GRAV		10
#define NEG_GRAV		11
#define CWISE_GRAV		12
#define ACWISE_GRAV		13
#define WORMHOLE		14
#define TREASURE		15
#define TARGET			16
#define ITEM_CONCENTRATOR	17
#define DECOR_FILLED		18
#define DECOR_LU		19
#define DECOR_LD		20
#define DECOR_RU		21
#define DECOR_RD		22
#define UP_GRAV			23
#define DOWN_GRAV		24
#define RIGHT_GRAV		25
#define LEFT_GRAV		26
#define FRICTION		27
#define	ASTEROID_CONCENTRATOR	28
#define BASE_ATTRACTOR		127

#define SPACE_BIT		(1 << SPACE)
#define BASE_BIT		(1 << BASE)
#define FILLED_BIT		(1 << FILLED)
#define REC_LU_BIT		(1 << REC_LU)
#define REC_LD_BIT		(1 << REC_LD)
#define REC_RU_BIT		(1 << REC_RU)
#define REC_RD_BIT		(1 << REC_RD)
#define FUEL_BIT		(1 << FUEL)
#define CANNON_BIT		(1 << CANNON)
#define CHECK_BIT		(1 << CHECK)
#define POS_GRAV_BIT		(1 << POS_GRAV)
#define NEG_GRAV_BIT		(1 << NEG_GRAV)
#define CWISE_GRAV_BIT		(1 << CWISE_GRAV)
#define ACWISE_GRAV_BIT		(1 << ACWISE_GRAV)
#define WORMHOLE_BIT		(1 << WORMHOLE)
#define TREASURE_BIT		(1 << TREASURE)
#define TARGET_BIT		(1 << TARGET)
#define ITEM_CONCENTRATOR_BIT	(1 << ITEM_CONCENTRATOR)
#define DECOR_FILLED_BIT	(1 << DECOR_FILLED)
#define DECOR_LU_BIT		(1 << DECOR_LU)
#define DECOR_LD_BIT		(1 << DECOR_LD)
#define DECOR_RU_BIT		(1 << DECOR_RU)
#define DECOR_RD_BIT		(1 << DECOR_RD)
#define UP_GRAV_BIT             (1 << UP_GRAV)
#define DOWN_GRAV_BIT           (1 << DOWN_GRAV)
#define RIGHT_GRAV_BIT          (1 << RIGHT_GRAV)
#define LEFT_GRAV_BIT           (1 << LEFT_GRAV)
#define FRICTION_BIT		(1 << FRICTION)
#define	ASTEROID_CONCENTRATOR_BIT	(1 << ASTEROID_CONCENTRATOR)

#define DIR_RIGHT		0
#define DIR_UP			(RES/4)
#define DIR_LEFT		(RES/2)
#define DIR_DOWN		(3*RES/4)


///////////////////////////////////////////////////////////////////////////////
// laser.cpp
class Vicbuf;
class Victim;

///////////////////////////////////////////////////////////////////////////////
// World.ScoreServer
class ConnectionControlScoreServer;

///////////////////////////////////////////////////////////////////////////////
class ServerT {
  public:
	String	owner;
	String	host;
};


///////////////////////////////////////////////////////////////////////////////
class QueuedPlayer {
  public:
	QueuedPlayer*	next;
	char			real_name[MAX_CHARS];
	char			nick_name[MAX_CHARS];
	char			disp_name[MAX_CHARS];
	char			host_name[MAX_CHARS];
	char			host_addr[24];
	Sockbuf*		contactBuf;
	int				port;
	int				team;
	uint			version;
	uint			origVersion;
	int				login_port;
	long			last_ack_sent;
	long			last_ack_recv;
};


///////////////////////////////////////////////////////////////////////////////
class Fuel {
  public:
	ipos	blk_pos;
	position	pix_pos;
	long	fuel;
	unsigned	conn_mask;
	long	last_change;
	int		team;
};

///////////////////////////////////////////////////////////////////////////////
// gravity
const int	gravRange = 10;
class Grav {
  public:
    ipos	pos;
    DFLOAT	force;
};

///////////////////////////////////////////////////////////////////////////////
class Base {
  public:
    ipos		pos;
    int			dir;
    unsigned short	team;
};

///////////////////////////////////////////////////////////////////////////////
class BaseOrder {
  public:
    int		base_idx;	/* Index in World.base[] */
    DFLOAT	dist;		/* Distance to first checkpoint */
};

///////////////////////////////////////////////////////////////////////////////
class Item {
  public:
    DFLOAT	prob;		/* Probability [0..1] for item to appear */
    int		max;		/* Max on world at a given time */
    int		num;		/* Number active right now */
    int		chance;		/* Chance [0..127] for this item to appear */
    DFLOAT	cannonprob;	/* Relative probability for item to appear */
    int		min_per_pack;	/* minimum number of elements per item. */
    int		max_per_pack;	/* maximum number of elements per item. */
    int		initial;	/* initial number of elements per player. */
    int		limit;		/* max number of elements per player/cannon. */
public:
	Item() { prob = 0.0; max = 0; num = 0; chance = 0; cannonprob = 0.0;
			 min_per_pack = 0; max_per_pack = 0; initial = 0; limit = 0; };
};

///////////////////////////////////////////////////////////////////////////////
class Asteroids	{		// There is only one of these per world.
  public:
//    DFLOAT	prob;	/* Probability [0..1] for asteroid to appear (obsolete see ServerOptions) */
    int		max;		/* Max on world at a given time */
    int		num;		/* Number active right now */
    int		chance;		/* Chance [0..127] for asteroid to appear */
};

///////////////////////////////////////////////////////////////////////////////
typedef enum { WORM_NORMAL, WORM_IN, WORM_OUT } wormType;

///////////////////////////////////////////////////////////////////////////////
class Wormhole {
  public:
    ipos	pos;
    int		lastdest;	/* last destination wormhole */
	int		countdown;	/* if >0 warp to lastdest else random */
	int		lastplayer;	/* last player to pass through */
    bool	temporary;	/* wormhole was left by hyperjump */
    wormType	type;
	u_byte	lastblock;	/* block it occluded */
	ushort	lastID;
};

///////////////////////////////////////////////////////////////////////////////
class Treasure {
  public:
    ipos		pos;
    bool		have;	/* true if this treasure has ball in it */
    unsigned short	team;	/* team of this treasure */
    int 		destroyed;	/* how often this treasure destroyed */
    bool		empty;	/* true if this treasure never had a ball in it */
};

///////////////////////////////////////////////////////////////////////////////
class Target {
  public:
    ipos		pos;
    unsigned short	team;
    int			dead_time;
    int			damage;
    unsigned		conn_mask;
    unsigned 		update_mask;
    long		last_change;
};

///////////////////////////////////////////////////////////////////////////////
class Team {
  public:
    int		numMembers;			/* Number of current members */
    int		numRobots;			/* Number of robot players */
    int		numBases;			/* Number of bases owned */
    int		numTreasures;		/* Number of treasures owned */
    int		numEmptyTreasures;	/* Number of empty treasures owned */
    int		treasuresDestroyed;	/* Number of destroyed treasures */
    int		treasuresLeft;		/* Number of treasures left */
	DFLOAT	score;
	DFLOAT	prevScore;
};

///////////////////////////////////////////////////////////////////////////////
class ItemConcentrator {
  public:
    ipos	pos;
};

///////////////////////////////////////////////////////////////////////////////
class AsteroidConcentrator {
  public:
    ipos	pos;
};

///////////////////////////////////////////////////////////////////////////////
class World {
  public:
	World();
	~World();
	static void	ErrMsgHandler(void* myThis, ErrMsgType emt, PCSTR ctl, ...);


	int			blockWidth, blockHeight;	/* Size of world in blocks */
	int			diagonal;	/* Diagonal length in blocks */
	int			width, height;	/* Size of world in pixels (optimization) */
	int			hypotenuse;	/* Diagonal length in pixels (optimization) */
	Rules		*rules;
	int			mapWidth;		/* Width of the universe */
	int			mapHeight;		/* Height of the universe */
	ServerOptions					options;
	ConnectionControlScoreServer*	scoreServer;
	FirewallPortList				firewallPortList;
	u_byte		**block;        /* type of item in each block */

	RobotMan*	robotMan;

	/* index into mapobject depending on value of corresponding block,
	** -1 for space, walls, etc */
	unsigned short	**itemID;

	vector		**gravity;

	Item		items[NUM_ITEMS];

	Asteroids	asteroids;

	Team		teams[MAX_TEAMS];

	int			obj_1;
	Object*		objs[MAX_TOTAL_SHOTS];
//	int			obj_2;
	int			getInd_1;			// for broken getInd[-1] lookups
	int			getInd[NUM_IDS+1];
	ServerT		server;
	int			shutdownServer;
	int			shutdownDelay;
	char		shutdownReason[MAX_CHARS];
	//int 		framesPerSecond = 18;
//	long		mainLoops;

	int			numTeamBases;      /* How many 'different' teams are allowed */
	int			numBases;
	Base*		bases;
	BaseOrder*	baseorders;
	int			numFuels;
	Fuel*		fuels;
	int			numGravs;
	Grav*		gravs;
	int			numCannons;
	Cannon*		cannons;
	int			numChecks;
	ipos		check[MAX_CHECKS];
	int			numWormholes;
	Wormhole*	wormHoles;
	int			numTreasures;
	Treasure*	treasures;
	int			numTargets;
	Target*		targets;
	int			numItemConcentrators;
	ItemConcentrator*	itemConcentrators;
	int			numAsteroidConcs;
	AsteroidConcentrator*	asteroidConcs;
	Pulse*		pulses[MAX_TOTAL_PULSES];
	Ecm*		ecms[MAX_TOTAL_ECMS];
	Transporter* transporters[MAX_TOTAL_TRANSPORTERS];
	Player**	players;
	Visibility*	visibilityArray;

	int			numPlayers;
	int			numAlliances;
	int			numPseudoPlayers;
	int			numQueuedPlayers;
	int			numControlConnections;

	int			numObjs;
	int			numPulses;
	int			numEcms;
	int			numTransporters;
	int			numRobots;
	int			minRobots;
	int			maxRobots;

	bool		gameLocked;
	int			roundsPlayed;
	int			roundDelay;
	int			roundDelaySend;
	int			roundTime;			// time remaining in this round

//	int			numLogins;			// number of players in and out since last meta
//	int			numLogouts;	// NOT USED YET
	bool		metaInited;
	int			tag;				// id of it during game of tag
	bool		updateScores;		// do we need to send to scores to the clients?

	int			GetFPS() { return options.GetFPS(); };
	bool		Parser(int argc, char** argv);
	bool		GrokMap(void);		// This needs to be changed to pass the map in
private:
#ifdef	DEBUG
	void		PrintMap();
#endif

// World.cpp
public:
	void		RemoveTempWormhole(int ind);
	int			Main(int argc, char **argv);
	DFLOAT		WrapFindDir(DFLOAT dx, DFLOAT dy);
	DFLOAT		WrapLength(DFLOAT dx, DFLOAT dy);
private:
	void		AllocMap();
	void		FreeMap();
	void		GenerateRandomMap();
	void		FindBaseDirection();
	void		FindBaseOrder();
	void		AddTempWormholes(int xin, int yin, int xout, int yout);

public:
	void		BroadcastPlayMessage(const char* message);
	unsigned short FindClosestTeam(int posx, int posy);

	// gravity sucks
public:
	void		ComputeGravity();
private:
	void		ComputeGlobalGravity();
	void		ComputeLocalGravity();
	void		ComputeGravTab(vector grav_tab[gravRange+1][gravRange+1]);

// World.cell.cpp
public:
	void		CellInitObject(Object* obj);
	void		CellAddObject(Object* obj);
	void		CellGetObjects(int x, int y, int range,
							   int max_obj_count,
							   Object*** obj_list,
							   int* count_ptr);
	void		CellRemoveObject(Object* obj);
private:
	void		AllocCells();
	void		InitCellDist();
	//int			CompareCellDist(const void *a, const void *b);
	void		FreeCells();
	void		FreeCellDist();

	CellNode**	cells;
	int			objectNodeOffset;
	CellDist*	cellDist;
	int			cellDistSize;

// World.rules.cpp
public:
	void		SetWorldItems();
	void		SetWorldRules();
	void		TuneAsteroidProb();
	void		SetMiscItemLimits();

private:
	void		SetItemChance(int item);
	void		SetWorldAsteroids();
	void		SetInitialItems();
	void		InitItem(int item, int initial, int limit, double prob, int minpp=1, int maxpp=1);
	void		TuneItemPacks();

// Networking
public:
	Sock		remoteContactSocket;
	Sockbuf		remoteContactBuf;
	String		serverHost;		/* Host name (for multihomed hosts) */
	String		serverAddr;		// dotted quad name

// World.contact.cpp
public:
	bool		ContactInit();
	bool		ContactInit(Sockbuf& ibuf, Sock& initSock, PCSTR addr, int port);
	void		Contact(Sockbuf& contactBuf);
	static void	ContactFromRemote(int fd, void* world);
	static void	ContactFromLocal(int fd, void* world);
	void		ContactCleanup();
	int			QueueAdvancePlayer(char *name, char *msg);
	int			QueueShowList(char *msg);
	void		QueueLoop(void);
	void		QueueAck(QueuedPlayer* qp, int qpos);
	void		QueueRemove(QueuedPlayer* qp, QueuedPlayer* prev);
	int			QueuePlayer(PCSTR real, String& nick, PCSTR disp, int team,
							PCSTR addr, PCSTR host, uint version, uint origVersion, int port,
							int *qpos, Sockbuf* ibuf);
	int			EnterPlayer(PCSTR real, String& nick, PCSTR disp, int team,
							PCSTR addr, PCSTR host, uint version, uint origVersion, int port,
							int *login_port);
	bool		IsOwner(char request, PCSTR real_name, PCSTR host_addr,
						int host_port, int pass);
	int			KickRobotPlayers(int team);
	int			KickPausedPlayers(int team);

// World.netserver.cpp
public:
//	int			Input(void);		// Process all Connections one time
//	int			SetupNetServer();
	int			InitSetup();
	int			SetupConnectionPlayer(PCSTR real, PCSTR nick, PCSTR dpy, int team,
								PCSTR addr, PCSTR host, uint version, uint origVersion);
	int			SetupConnectionControl(PCSTR real, PCSTR nick, PCSTR dpy,
									   PCSTR addr, PCSTR host, int* port,
									   uint version, uint origVersion);
	int			HandleLogin(int ind, char *errmsg, int errsize);
	int			GetMotd(char *buf, int offset, int maxlen, int *size_ptr);
	void		BroadcastOption(ServerOption* so);
	bool		CheckMaxClientsPerIP(PCSTR hostAddr);
	void		NumLogoutsBump();	// Somebody left

// World.ScoreServer.cpp
	void		ScoreServerRequestPlayerScore(Player* pl);
	void		ScoreServerRequestPlayerRanks();
	void		ScoreServerPlayerEvent(Player* pl, ScorePlayerEvent pse);
	void		ScoreServerScoreEvent(Player* killer, DFLOAT wscore, 
									  Player* killee, DFLOAT lscore,
									  ScoreType st);
	void		ScoreServerScoreEvent(Player* killer, DFLOAT wscore, 
									  PCSTR killee, PlayerType pt, DFLOAT lscore, 
									  ScoreType st);
	void		ScoreServerScoreEvent(PCSTR killer, PlayerType pt, DFLOAT wscore, 
									  Player* killee, DFLOAT lscore,
									  ScoreType st);

public:
// World.server.cpp
public:
	static void	MainLoop(void* world);
	void		MainLoop();
	bool		EndGame(PCSTR msg = NULL);
	int			PickTeam(int pick_for_type);
	void		TunerPLock();
	void		GameOver();
	void		LogGame(const char *heading);
	void		ServerInfo(char *str, unsigned max_size);

// sched.cpp
	void		io_dummy(int fd, void *arg);

// World.walls.cpp
	void		WallsInit(void);
	void		TreasureInit(void);
	void		MoveInit();
	void		MoveObject(Object *obj);
	void		MovePlayer(int ind);
	void		TurnPlayer(int ind);
	void		MoveSegment(move_state_t *ms);
	void		CannonDies(move_state_t *ms);
private:
	void		ObjectHitsTarget(move_state_t *ms, long player_cost);
	void		ObjectCrash(move_state_t *ms);
	void		PlayerCrash(move_state_t *ms, int pt, bool turning);
	/*
	 * Two dimensional array giving for each point the distance
	 * to the nearest wall.  Measured in blocks times 2.
	 */
	void		WalldistAlloc(void);
	void		WalldistInit(void);
	void		WalldistDump(void);
	unsigned char**	walldist;


// Collision.cpp
public:
	void CheckCollision();

private:
	void		PlayerCollision();
	int			InRangeAcd(int p1x, int p1y, int p2x, int p2y,
						   int q1x, int q1y, int q2x, int q2y,
						   int r);

	void		PlayerObjectCollision(int ind);
	void		AsteroidCollision();
	void		BallCollision();
	void		MineCollision();
	void		PlayerCollidesWithBall(int ind, Object *obj, int radius);
	void		PlayerCollidesWithItem(int ind, Object *obj);
	void		PlayerCollidesWithMine(int ind, Object *obj);
	void		PlayerCollidesWithDebris(int ind, Object *obj);
	void		PlayerCollidesWithAsteroid(int ind, WireObject *ast);
	void		PlayerCollidesWithKillingShot(int ind, Object *obj);
	void		PlayerPassCheckpoint(int ind);
	int			WormXY(int x, int y);
	void		ObjRepel(Object* obj1, Object* obj2, int repel_dist);

// event.cpp
public:
	int			HandleKeyboard(Player* pl);
	void		PausePlayer(Player* pl, int onoff);
	int			PlayerLockClosest(Player* pl, int next);
	bool		IsTeamDead(int team);
	void		FilterMods(modifiers *mods);
private:
	bool		PlayerLockAllowed(Player* pl, int lock);

// item.cpp
public:
	void		PlaceItem(int item, int ind);
	void		MakeItem(int px, int py,
						  int vx, int vy,
						  int item, int num_per_pack,
						  long status);
	void		ThrowItems(int ind);
	void		DetonateItems(Player* pl);
	void		TractorBeam(Player* pl);
	void		GeneralTractorBeam(Player* pl, DFLOAT x, DFLOAT y,
								   int items, int target, bool pressor);
	void		DoDeflector(Player* pl);
	void		DoTransporter(Player* pl);
	void		DoGeneralTransporter(Player* pl, DFLOAT x, DFLOAT y, int target,
									 int *itemp, long *amountp);
	void		DoLoseItem(Player* pl);
	void		FireGeneralEcm(Player* pl, unsigned short team, DFLOAT x, DFLOAT y);
	int			ChooseRandomItem();
private:

// laser.cpp
public:
	void		LaserPulseCollision();
private:
	void		LaserPulseDestroyOne(int pulse_index);
	void		LaserPulseDestroyAll();
	void		LaserPulseFindVictims(Vicbuf* vicbuf, Pulse *pulse, DFLOAT midx, DFLOAT midy);
	void		LaserPulseHitsPlayer(Pulse *pulse, Object *obj,
									 DFLOAT x, DFLOAT y,
									 Victim *victim, bool *refl);
	int			LaserPulseCheckPlayerHits(Pulse *pulse, Object *obj,
										  DFLOAT x, DFLOAT y,
										  Vicbuf *vicbuf, bool *refl);
	list_t		LaserPulseGetObjectList(list_t input_obj_list, Pulse *pulse,
											DFLOAT midx, DFLOAT midy);

// play.cpp
public:
	Player*		FindPlayer(PCSTR nick, PlayerType pt);
	int			PunishTeam(Player* pl, int t_destroyed, int t_target);
	void		ComputeGameStatus();
	void		CheckTeamMembers(int team);
	void		IndividualGameOver(int winner);
	void		TeamGameOver(int winning_team, const char *reason);
	void		RaceGameOver(void);
	void		ComputeEndOfRoundValues(DFLOAT* average_score,
										int* num_best_players,
										DFLOAT* best_ratio,
										int best_players[]);
	void		GiveBestPlayerBonus(DFLOAT average_score,
									int num_best_players,
									DFLOAT best_ratio,
									int best_players[]);
	void		GiveIndividualBonus(int ind, DFLOAT average_score);
	void		CountRounds();
	void		MakeDebris(
					/* pos.x, pos.y   */ DFLOAT  x,          DFLOAT y,
					/* vel.x, vel.y   */ DFLOAT  velx,       DFLOAT vely,
					/* owner id       */ int    id,
					/* owner team     */ unsigned short team,
					/* type           */ int    type,
					/* mass           */ DFLOAT  mass,
					/* status         */ long   status,
					/* color          */ int    color,
					/* radius         */ int    radius,
					/* min,max debris */ int    min_debris, int    max_debris,
					/* min,max dir    */ int    min_dir,    int    max_dir,
					/* min,max speed  */ DFLOAT  min_speed,  DFLOAT  max_speed,
					/* min,max life   */ int    min_life,   int    max_life);

// ship.cpp
public:
	void		MakeWreckage(
					/* pos.x, pos.y     */ DFLOAT x,            DFLOAT y,
					/* vel.x, vel.y     */ DFLOAT velx,         DFLOAT vely,
					/* owner id         */ int    id,
					/* owner team		*/ unsigned short team,
					/* min,max mass     */ DFLOAT min_mass,     DFLOAT max_mass,
					/* total mass       */ DFLOAT total_mass,
					/* status           */ long   status,
					/* color            */ int    color,
					/* max wreckage     */ int    max_wreckage,
					/* min,max dir      */ int    min_dir,      int    max_dir,
					/* min,max speed    */ DFLOAT min_speed,    DFLOAT max_speed,
					/* min,max life     */ int    min_life,     int    max_life);
// score.cpp
	void		SetPlayerScore(PCSTR nick, PlayerType pt, 
							   DFLOAT score, int kills, int deaths, int cookie);
	void		SetPlayerRank(PCSTR nick, PlayerType pt,
							  int rank, DFLOAT rating);



// shot.cpp
	void		PlaceGeneralMine(int ind, unsigned short team, long status,
								   DFLOAT x, DFLOAT y,
								   DFLOAT vx, DFLOAT vy, modifiers mods);
	void		MakeTreasureBall(int treasure);
	void		FireGeneralShot(int ind, unsigned short team, bool cannon,
								DFLOAT x, DFLOAT y,
								int type, int dir, 
								modifiers mods, int target);
	void		DeleteShot(int ind);
	void		FireGeneralLaser(int ind, unsigned short team, DFLOAT x, DFLOAT y,
								 int dir, modifiers mods);
	void		MoveBall(int ind);
	void		MoveMissile(int ind);
	void		MoveMine(int ind);

// update.cpp
public:
	void		UpdateObjects();
    int         RandomPlayerWithStatus(long mask, long bits);

// World.tag.cpp
	void		TransferTag(int oldtag, int newtag);
	DFLOAT		HandleTag(DFLOAT score, int victim, int killer);

};

inline int World::WormXY(int x, int y) { return itemID[x][y]; };

#endif
