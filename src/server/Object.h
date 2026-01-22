/* $Id: Object.h,v 1.18 2007/01/20 19:27:27 dick Exp $
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
 *  $Log: Object.h,v $
 *  Revision 1.18  2007/01/20 19:27:27  dick
 *  Deadwood delete
 *
 *  Revision 1.17  2007/01/19 07:14:49  dick
 *  Whitespace
 *
 *  Revision 1.16  2007/01/17 09:03:58  dick
 *  Whitespace
 *
 *  Revision 1.15  2004/05/07 04:27:41  dick
 *  _visibility becomes Visibility.  global updateScores becomes a member of World.
 *  Handle rank/rate data from the scoreserver and send it to the client.
 *
 *  Revision 1.14  2004/02/23 07:21:46  dick
 *  Argh.  Use SmartObject as our "largest" object in the big wrong malloc.
 *  MineObject, the previous choice of largest, was incorrect.
 *  (It's wrong in a C++ way).
 *
 *  Revision 1.13  2004/01/06 04:48:28  dick
 *  Add support for building under Cygwin.  Still needs work and the output
 *  is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 *  Revision 1.12  2002/09/11 16:42:03  dick
 *  Here endeth the server encapsulation task.
 *  theWorld is now an array of World(s) and get loaded dynamically.
 *
 *  Revision 1.11  2002/07/14 19:03:47  dick
 *  type_ext becomes enum PlayerType
 *
 *  Revision 1.10  2002/06/01 06:06:47  dick
 *  Encapsulate (almost) everything.  Get rid of (almost) all refs to theWorld.
 *
 *  Revision 1.9  2002/05/22 05:09:00  dick
 *  Update to XPilot r4_5_4alpha.
 *
 *  Revision 1.8  2002/05/18 20:55:38  dick
 *  Update to XPilot-4.5.1
 *
 *  Revision 1.7  2001/08/14 21:37:14  dick
 *  Array Obj[] becomes objs[].  We now have a class called Obj.
 *
 *  Revision 1.6  2001/07/15 01:24:27  dick
 *  Break Player out to his own file
 *
 *  Revision 1.5  2001/07/12 10:01:02  dick
 *  Go_home() becomes GoHome()
 *
 *  Revision 1.4  2001/07/08 06:40:14  dick
 *  C++ conversion stuff.
 *
 *  Revision 1.3  2001/07/07 12:00:42  dick
 *  Rename classes to C++ "Style".  old World becomes theWorld.
 *
 */

#ifndef	OBJECT_H
#define	OBJECT_H

#ifndef	SERVERCONST_H
/* need MAX_TANKS */
#include "serverconst.h"
#endif

#ifndef KEYS_H
/* need NUM_KEYS */
#include "keys.h"
#endif
#ifndef BIT_H
/* need BITV_DECL */
#include "bit.h"
#endif
#ifndef DRAW_H
/* need shipobj */
#include "draw.h"
#endif
#ifndef ITEM_H
/* need NUM_ITEMS */
#include "item.h"
#endif
#ifndef CLICK_H
/* need CLICK */
#include "click.h"
#endif

#if defined(_WINDOWS) && !defined(_CYGWIN)
#include "NT/winNet.h"
#endif

/*
 * Different types of objects, including player.
 * Robots and tanks are players but have an additional type_ext field.
 * Smart missile, heatseeker and torpedoe can be merged into missile.
 * ECM doesn't really need an object type.
 */
#define OBJ_PLAYER		(1U<<0)		// 0001	     1
#define OBJ_DEBRIS		(1U<<1)		// 0002	     2
#define OBJ_SPARK		(1U<<2)		// 0004	     4
#define OBJ_BALL		(1U<<3)		// 0008	     8
#define OBJ_SHOT		(1U<<4)		// 0010	    16
#define OBJ_SMART_SHOT	(1U<<5)		// 0020	    32
#define OBJ_MINE		(1U<<6)		// 0040	    64
#define OBJ_TORPEDO		(1U<<7)		// 0080	   128
#define OBJ_HEAT_SHOT	(1U<<8)		// 0100	   256
#define OBJ_PULSE		(1U<<9)		// 0200	   512
#define OBJ_ITEM		(1U<<10)	// 0400	  1024
#define OBJ_WRECKAGE	(1U<<11)	// 0800	  2048
#define OBJ_ASTEROID	(1U<<12)	// 1000	  4096
#define	OBJ_CANNON_SHOT	(1U<<13)	// 2000	  8192


/*
 * Some object types are overloaded.
 * These bits are set in the player->type_ext field.
 */
#define OBJ_EXT_TANK		(1U<<1)
#define OBJ_EXT_ROBOT		(1U<<2)

/* macro's to query the type of player. */
#define IS_TANK_IND(__w, __ind)		IS_TANK_PTR(__w->players[__ind])
#define IS_ROBOT_IND(__w, __ind)	IS_ROBOT_PTR(__w->players[__ind])
#define IS_HUMAN_IND(__w, __ind)	IS_HUMAN_PTR(__w->players[__ind])
#define IS_HUMAN_PTR(__pl)	((__pl)->playerType == PlayerHuman)
#define IS_ROBOT_PTR(__pl)	((__pl)->playerType == PlayerRobot)
#define IS_TANK_PTR(__pl)	((__pl)->playerType == PlayerTank)



/*
 * Different types of attributes a player can have.
 * These are the bits of the player->have and player->used fields.
 */
#define HAS_EMERGENCY_THRUST	(1U<<30)
#define HAS_AUTOPILOT			(1U<<29)
#define HAS_TRACTOR_BEAM		(1U<<28)
#define HAS_LASER				(1U<<27)
#define HAS_CLOAKING_DEVICE		(1U<<26)
#define HAS_SHIELD				(1U<<25)
#define HAS_REFUEL				(1U<<24)
#define HAS_REPAIR				(1U<<23)
#define HAS_COMPASS				(1U<<22)
#define HAS_AFTERBURNER			(1U<<21)
#define HAS_CONNECTOR			(1U<<20)
#define HAS_EMERGENCY_SHIELD	(1U<<19)
#define HAS_DEFLECTOR			(1U<<18)
#define HAS_PHASING_DEVICE		(1U<<17)
#define HAS_MIRROR				(1U<<16)
#define HAS_ARMOR				(1U<<15)

#define HAS_SHOT				(1U<<4)
#define HAS_BALL				(1U<<3)


/*
 * Weapons modifiers.
 */
struct modifiers {
    unsigned int	nuclear	:2;	/* N  modifier */
    unsigned int	warhead	:2;	/* CI modifier */
    unsigned int	velocity:2;	/* V# modifier */
    unsigned int	mini	:2;	/* X# modifier */
    unsigned int	spread	:2;	/* Z# modifier */
    unsigned int	power	:2;	/* B# modifier */
    unsigned int	laser	:2;	/* LS LB modifier */
    unsigned int	spare	:2;	/* padding for alignment */
};

#define CLEAR_MODS(mods)	memset(&(mods), 0, sizeof(modifiers))

#define MODS_NUCLEAR_MAX	2	/* - N FN */
#define NUCLEAR			(1U<<0)
#define FULLNUCLEAR		(1U<<1)

#define MODS_WARHEAD_MAX	3	/* - C I CI */
#define CLUSTER			(1U<<0)
#define IMPLOSION		(1U<<1)

#define MODS_VELOCITY_MAX	3	/* - V1 V2 V3 */
#define MODS_MINI_MAX		3	/* - X2 X3 X4 */
#define MODS_SPREAD_MAX		3	/* - Z1 Z2 Z3 */
#define MODS_POWER_MAX		3	/* - B1 B2 B3 */

#define MODS_LASER_MAX		2	/* - LS LB */
#define STUN			(1U<<0)
#define BLIND			(1U<<1)

#define LOCK_NONE		0x00	/* No lock */
#define LOCK_PLAYER		0x01	/* Locked on player */
#define LOCK_VISIBLE		0x02	/* Lock information was on HUD */
									/* computed just before frame shown */
									/* and client input checked */
#define LOCKBANK_MAX		4	/* Maximum number of locks in bank */

#define NOT_CONNECTED		(-1)

class World;
/*
 * Object position is non-modifiable, except at one place.
 *
 * NB: position in pixels used to be a float.
 */
class ObjPosition
{
public:
    int		cx, cy;			/* object position in clicks. */
    int		x, y;			/* object position in pixels. */
    int		bx, by;			/* object position in blocks. */
};
#define OBJ_X_IN_CLICKS(obj)	((obj)->pos.cx)
#define OBJ_Y_IN_CLICKS(obj)	((obj)->pos.cy)
#define OBJ_X_IN_PIXELS(obj)	((obj)->pos.x)
#define OBJ_Y_IN_PIXELS(obj)	((obj)->pos.y)
#define OBJ_X_IN_BLOCKS(obj)	((obj)->pos.bx)
#define OBJ_Y_IN_BLOCKS(obj)	((obj)->pos.by)


/*
 * Node within a Cell list.
 */
class CellNode
{
public:
    CellNode		*next;
    CellNode		*prev;
};

class ObjectBase
{
public:
    ObjectBase() {}

    int			type;			/* one bit of OBJ_XXX */
    short		id;				/* For shots => id of player */
    unsigned short	team;		/* Team of player or cannon */
    ObjPosition		pos;		/* World coordinates */
	World*		world;			/* which world this object belongs to */
    ipos		prevpos;		/* previous position */
    vector		vel;			/* speed in x,y */
    vector		acc;			/* acceleration in x,y */
    DFLOAT		mass;			/* mass in unigrams */
    long		life;			/* No of ticks left to live */
    long		status;			/* gravity, etc. */
    int			count;			/* Misc timings */
    modifiers	mods;			/* Modifiers to this object */
    u_byte		color;			/* Color of object */
    u_byte		missile_dir;	/* missile direction */
/* up to here all object types are the same as all player types. */
};

class ObjectExtend
{
public:
    ObjectExtend() {}

    CellNode	cell;		/* node in cell linked list */
    long		info;		/* Miscellaneous info */
    long		fuselife;	/* fuse duration ticks */
    int			pl_range;	/* distance for collision */
    int			pl_radius;	/* distance for hit */
/* up to here all object types are the same. */
};

/*
 * Generic object
 */
class Object : public ObjectBase, public ObjectExtend
{
public:
	Object() {}
};

#define OBJ_IND(ind)	(Obj[(ind)])
#define OBJ_PTR(ptr)	((Object *)(ptr))


/*
 * Mine object
 */
class MineObject : public Object
{
public:
	MineObject() {}

    int 		owner;		/* Who's object is this ? */
    DFLOAT		ecm_range;	/* Range from last ecm center */
    int			spread_left;	/* how much spread time left */
};

#define MINE_IND(__w, __ind)	((MineObject *)__w->objs[(__ind)])
#define MINE_PTR(ptr)	((MineObject *)(ptr))

/*
 * Generic missile object
 */
class MissileObject : public Object
{
public:
	MissileObject() {}
    DFLOAT		max_speed;	/* speed limitation */		\
    DFLOAT		turnspeed;	/* how fast to turn */
};

#define MISSILE_IND(__w, __ind)	((MissileObject *)__w->objs[(__ind)])
#define MISSILE_PTR(__ptr)		((MissileObject *)(__ptr))

/* up to here all missiles types are the same. */


/*
 * Smart missile is a generic missile with extras.
 */
class SmartObject : public MissileObject
{
public:
	SmartObject() {}

    int			new_info;	/* smart re-lock id */
    DFLOAT		ecm_range;	/* Range from last ecm center */
};

#define SMART_IND(ind)	((SmartObject *)objs[(ind)])
#define SMART_PTR(ptr)	((SmartObject *)(ptr))

/*
 * Torpedo is a generic missile with extras
 */
class TorpObject : public MissileObject
{
public:
	TorpObject() {}

    int			spread_left;	/* how much spread time left */
};

#define TORP_IND(ind)	((TorpObject *)objs[(ind)])
#define TORP_PTR(ptr)	((TorpObject *)(ptr))


/*
 * The ball object.
 */
class BallObject : public Object
{
public:
	BallObject() {}
    int 		owner;		/* Who's object is this ? */
    int			treasure;	/* treasure for ball */	
    DFLOAT		length;		/* distance ball to player */
};

#define BALL_IND(__w, __ind)	((BallObject *)__w->objs[(__ind)])
#define BALL_PTR(obj)			((BallObject *)(obj))

/*
 * Object with a wireframe representation.
 */
class WireObject : public Object
{
public:
	WireObject() {}

    DFLOAT		turnspeed;	/* how fast to turn */
    u_byte		size;		/* Size of object (wreckage) */
    u_byte		rotation;	/* Rotation direction */
};

#define WIRE_IND(ind)	((WireObject *)objs[(ind)])
#define WIRE_PTR(obj)	((WireObject *)(obj))


/*
 * Fuel structure, used by player
 */
class PlayerFuel
{
public:
    long	sum;			/* Sum of fuel in all tanks */
    long	max;			/* How much fuel can you take? */
    int		current;		/* Number of currently used tank */
    int		num_tanks;		/* Number of tanks */
    long	tank[1 + MAX_TANKS];	/* main fixed tank + extra tanks. */
    long	l1;				/* Fuel critical level */
    long	l2;				/* Fuel warning level */
    long	l3;				/* Fuel notify level */
};

class Visibility {
public:
    int		canSee;
    long	lastChange;
};

#define MAX_PLAYER_ECMS		8	/* Maximum simultaneous per player */
class Ecm
{
public:
    int		size;
    position	pos;
    int		id;
};

/*
 * Structure holding the info for one pulse of a laser.
 */
class Pulse
{
public:
    position		pos;
    int			dir;
    int			len;
    int			life;
    int			id;
    unsigned short	team;
    modifiers		mods;
    bool		refl;
};

/*
 * Transporter info.
 */
class Transporter
{
public:
    position	pos;
    int		target;
    int		id;
    int		count;
};


#endif
