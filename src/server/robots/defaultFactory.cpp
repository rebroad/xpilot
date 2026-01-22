/* $Id: defaultFactory.cpp,v 1.1 2007/01/13 22:27:28 dick Exp $
 *
 * defaultFactory - A canned array of RobotDescriptors to use for the factory
 *					when no robots file can be read.
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2007 by
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
 *  $Log: defaultFactory.cpp,v $
 *  Revision 1.1  2007/01/13 22:27:28  dick
 *  Now, a Robot* is a member of Player.
 *  Robot contains the interface to drive a robot.
 *  Robot4 is the old robotdef.cpp handler.
 *
 */
/*
 * Robot code originally submitted by Maurice Abraham.
 */
#include "StdAfx.h"
#include "Obj.h"
#include "const.h"

#include "Robot.h"
#include "RobotMan.h"

/*
 * Array of different robots which are used
 * when we cannot read a robot configuration file.
 *
 * Each robot has a robot driver,
 * a name as seen by the human players,
 * some optional configuration string,
 * a usage count,
 * and a shipshape.
 */
RobotDescriptor defaultFactory[] = {
	{
		DEFAULT_ROBOT_TYPE,
		"Mad Max",
		"94 20",
		0,
		"(15,8,7)(15,0)(7,1)(7,2)(2,4)(-1,11)"
		"(-3,11)(-2,3)(-8,6)(-8,-6)(-2,-3)(-3,-11)"
		"(-1,-11)(2,-4)(7,-2)(7,-1)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Blackie",
		"10 90",
		0,
		"(16,6,10)(15,0)(6,2)(-2,3)(-1,4)(-2,5)"
		"(-10,8)(-13,8)(-13,1)(-15,0)(-13,-1)"
		"(-13,-8)(-10,-8)(-2,-5)(-1,-4)(-2,-3)(6,-2)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Kryten",
		"70 40",
		0,
		"(4,1,3)(15,0)(0,8)(-8,0)(0,-8)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Marvin",
		"30 70",
		0,
		"(15,4,5)(10,0)(10,7)(5,14)(-5,14)(-10,7)"
		"(-10,-7)(-5,-14)(5,-14)(10,-7)(10,0)(5,5)"
		"(2,7)(5,0)(2,-7)(5,-5)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"R2D2",
		"50 60",
		0,
		"(15,8,9)(15,0)(14,1)(-1,2)(-2,9)(0,10)"
		"(-4,10)(-7,2)(-8,2)(-8,-2)(-7,-2)(-4,-10)"
		"(0,-10)(-2,-9)(-1,-2)(14,-1)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"C3PO",
		"60 50",
		0,
		"(16,1,15)(10,0)(0,5)(0,15)(15,10)(0,15)"
		"(-15,10)(0,15)(0,5)(-7,0)(0,-5)(0,-15)"
		"(-15,-10)(0,-15)(15,-10)(0,-15)(0,-5)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"K9",
		"50 50",
		0,
		"(14,0,5)(15,0)(15,5)(5,5)(5,-5)(15,-5)"
		"(15,0)(-15,0)(-15,5)(5,5)(5,-5)(-15,-5)"
		"(-15,-8)(-15,8)(-15,0)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Robby",
		"45 55",
		0,
		"(5,2,3)(15,0)(0,12)(-9,8)(-9,-8)(0,-12)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Mickey",
		"05 95",
		0,
		"(15,6,7)(5,-1)(8,-5)(7,-9)(4,-11)(-1,-10)"
		"(-5,-6)(-8,-10)(-8,10)(-5,6)(-1,10)(4,11)"
		"(7,9)(8,5)(5,1)(0,0)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Hermes",
		"15 85",
		0,
		"(16,12,11)(10,1)(12,8)(-11,8)(-10,3)"
		"(-7,0)(-5,2)(-7,0)(-10,-1)(-10,-3)"
		"(-13,-4)(-13,-7)(-15,-8)(-15,-13)(-5,-5)"
		"(-2,-4)(5,-2)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Pan",
		"60 60",
		0,
		"(14,6,5)(15,-1)(15,0)(5,0)(5,-1)(5,9)(-15,9)"
		"(-15,-4)(-5,-7)(-3,-8)(-7,-8)(-5,-7)(5,-4)"
		"(5,-1)(-15,-1)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Azurion",
		"40 30",
		0,
		"(6,2,4)(15,0)(0,2)(-9,8)(-3,0)(-9,-8)(0,-2)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Droidion",
		"60 30",
		0,
		"(6,2,4)(9,0)(4,8)(-5,8)(-10,0)(-5,-8)(4,-8)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Terminator",
		"80 40",
		0,
		"(6,2,4)(15,0)(0,2)(-9,8)(-3,0)"
		"(-9,-8)(0,-2)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Sniper",
		"30 90",
		0,
		"(15,6,9)(15,0)(4,2)(-2,8)(-4,7)(-3,2)"
		"(-8,5)(-8,2)(-6,1)(-6,-1)(-8,-2)(-8,-5)"
		"(-3,-2)(-4,-7)(-2,-8)(4,-2)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Slugger",
		"40 40",
		0,
		"(15,8,7)(13,0)(11,1)(3,2)(-1,8)(-3,8)"
		"(-3,2)(-5,2)(-8,5)(-8,-5)(-5,-2)(-3,-2)"
		"(-3,-8)(-1,-8)(3,-2)(11,-1)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Uzi",
		"95 5",
		0,
		"(16,9,8)(15,3)(7,3)(7,-8)(3,-8)(3,1)(-2,1)"
		"(-3,-1)(-5,-1)(-14,-5)(-15,2)(-3,4)(-1,8)"
		"(0,6)(13,6)(14,8)(15,6)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Capone",
		"80 50",
		0,
		"(15,9,6)(14,0)(2,2)(0,8)(1,8)(-3,8)(-3,2)"
		"(-8,4)(-7,1)(-7,-1)(-8,-4)(-3,-2)(-3,-8)"
		"(1,-8)(0,-8)(2,-2)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Tanx",
		"40 70",
		0,
		"(16,7,8)(15,1)(2,0)(1,-2)(6,-3)(6,-5)(3,-8)"
		"(-10,-8)(-13,-6)(-13,-3)(-10,-2)(-11,2)(-7,2)"
		"(-7,8)(-7,2)(1,2)(2,1)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Chrome Star",
		"60 60",
		0,
		"(5,1,4)(8,0)(-8,5)(2,-8)(2,8)(-8,-5)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Bully",
		"80 10",
		0,
		"(15,6,9)(11,0)(12,-3)(9,-3)(8,-2)(-5,-5)"
		"(-9,-11)(-14,-14)(-5,-3)(-5,3)(-14,14)"
		"(-9,11)(-5,5)(8,2)(9,3)(12,3)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Metal Hero",
		"40 45",
		0,
		"(16,7,9)(15,5)(12,-2)(9,-2)(10,-1)"
		"(-8,-1)(-4,-1)(1,-3)(-13,-9)(-9,0)"
		"(-15,8)(1,3)(-4,1)(-8,1)(-8,-1)(-8,1)"
		"(11,1)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Aurora",
		"60 55",
		0,
		"(16,5,11)(15,0)(-1,3)(-3,5)(-3,9)(7,10)"
		"(-12,10)(-6,9)(-6,4)(-8,0)(-6,-4)(-6,-9)"
		"(-12,-10)(7,-10)(-3,-9)(-3,-5)(-1,-3)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Dalt Wisney",
		"30 75",
		0,
		"(16,10,6)(14,0)(7,-4)(0,-1)(-5,-4)"
		"(2,-8)(0,-10)(-14,-10)(-5,-7)"
		"(-14,0)(-5,7)(-14,10)(0,10)(2,8)"
		"(-5,4)(0,1)(7,4)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Psycho",
		"65 55",
		0,
		"(11,5,6)(8,0)(5,8)(3,12)(0,15)(0,0)"
		"(-8,3)(-8,-3)(0,0)(0,-15)(3,-12)(5,-8)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Gorgon",
		"30 40",
		0,
		"(15,7,8)(15,0)(5,2)(3,8)(2,2)(-9,2)"
		"(-10,4)(-12,2)(-14,4)(-14,-4)(-12,-2)"
		"(-10,-4)(-9,-2)(2,-2)(3,-8)(5,-2)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Pompel",
		"50 50",
		0,
		"(15,7,8)(15,0)(14,4)(10,5)(5,2)(-7,3)"
		"(-7,6)(5,8)(-9,8)(-9,-8)(5,-8)(-7,-6)"
		"(-7,-3)(5,-2)(10,-5)(14,-4)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Pilt",
		"50 50",
		0,
		"(16,8,7)(15,0)(13,-2)(9,-3)(3,-3)(-3,-3)"
		"(-5,-2)(-13,-2)(-15,-3)(-15,3)(-13,2)"
		"(-5,2)(-3,3)(-3,-8)(-3,8)(-3,3)(8,3)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Sparky",
		"20 40",
		0,
		"(15,8,7)(15,-8)(6,-5)(7,-4)(1,-2)(2,-1)"
		"(-4,0)(-3,2)(-15,8)(-15,2)(-8,0)(-9,-2)"
		"(-3,-3)(-4,-4)(3,-5)(2,-7)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Cobra",
		"85 60",
		0,
		"(16,5,11)(8,0)(8,-6)(6,-8)(0,-7)(5,-6)"
		"(-8,-4)(5,-2)(0,-1)(5,0)(0,1)(5,2)(-8,4)"
		"(5,6)(0,7)(6,8)(8,6)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Falcon",
		"70 20",
		0,
		"(16,5,6)(14,2)(14,4)(2,10)(-5,10)(-10,8)"
		"(-12,3)(-12,-3)(-10,-8)(-5,-10)(9,-11)"
		"(10,-8)(7,-8)(14,-4)(14,-2)(4,-2)(4,2)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Boson",
		"25 35",
		0,
		"(16,11,12)(15,0)(10,-5)(4,-8)(7,-2)(7,2)"
		"(4,8)(6,0)(4,-8)(-10,-8)(-10,8)(-10,-8)"
		"(-15,-7)(-15,7)(-10,8)(4,8)(10,5)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Blazy",
		"40 40",
		0,
		"(12,4,8)(4,0)(2,4)(-5,11)(10,12)(-8,12)"
		"(-4,6)(-2,0)(-4,-6)(-8,-12)(10,-12)"
		"(-5,-11)(2,-4)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Pixie",
		"15 93",
		0,
		"(13,6,7)(15,0)(7,4)(11,1)(-4,3)(3,5)"
		"(-7,10)(-9,2)(-9,-2)(-7,-10)(3,-5)(-4,-3)"
		"(11,-1)(7,-4)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Wimpy",
		"5 98",
		0,
		"(16,9,7)(3,0)(6,5)(8,10)(5,11)(1,10)(-1,8)"
		"(-4,9)(-8,6)(-5,0)(-8,-6)(-4,-9)(-1,-8)"
		"(1,-10)(5,-11)(8,-10)(6,-5)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Bonnie",
		"30 40",
		0,
		"(16,9,6)(13,3)(5,3)(5,1)(4,-1)(0,-1)"
		"(-2,-8)(-8,-8)(-5,3)(-6,6)(-8,7)(-7,8)"
		"(-4,7)(8,7)(10,8)(12,8)(13,7)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Clyde",
		"40 45",
		0,
		"(16,5,11)(14,0)(5,5)(6,2)(0,2)(0,8)(-13,8)"
		"(-13,4)(-4,4)(-6,0)(-4,-4)(-13,-4)(-13,-8)"
		"(0,-8)(0,-2)(6,-2)(5,-5)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Neuro",
		"70 70",
		0,
		"(16,7,5)(12,-7)(12,-12)(5,-12)(2,-10)"
		"(1,-5)(-9,-4)(-11,2)(-8,8)(-3,11)(3,11)"
		"(9,8)(11,2)(13,0)(12,-3)(12,-7)(7,-7)"
	},
};

int defaultFactoryNELEM() {
	return(NELEM(defaultFactory));
}
