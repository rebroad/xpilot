/* $Id: ShipDocument.cpp,v 1.9 2004/07/22 16:58:45 dick Exp $
 *
 * ShipDocument - Represent a single shipshape
 *
 * common - The common library for XPilot
 *
 *      Copyright (C) 2001 by
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
 * $Log: ShipDocument.cpp,v $
 * Revision 1.9  2004/07/22 16:58:45  dick
 * Remove default constructor.  Constructor(int TRUE/FALSE) tells whether
 * to init with the default ship (TRUE) or not (FALSE).
 *
 * Revision 1.8  2004/05/30 08:14:02  dick
 * Rehash ship parsing 1) so it works, 2) gets rid of evil char[800].
 * Use better XpPointList copy constructor.
 *
 * Revision 1.7  2004/05/28 17:43:19  dick
 * Add member Draw(x,y,orient), which lives in xp-flwidgets.
 * Convert_ship_2_string returns a String, not a String*.
 * Convert_string_2_ship(PCSTR) overwrites this ship with a new parse,
 * (used in NameAndShipButton).
 *
 * Revision 1.6  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.5  2004/01/04 06:01:03  dick
 * No newline at end of file
 *
 * Revision 1.4  2002/09/10 19:03:44  jlmiller
 * Add copy constructor for cloning of ships & shiplists.
 *
 * Revision 1.3  2002/09/10 01:19:40  jlmiller
 * Fix bug in saving of ships with no mainguns/engines set.
 *
 * Revision 1.2  2002/07/29 19:19:40  jlmiller
 * Don't use num_* int fields to store count when we have GetCount() member funcs
 * for that purpose already. This is a beginning pass at integrating ShipDocument.
 *
 * Revision 1.1  2002/06/25 20:21:06  jlmiller
 * Add to common.
 *
 * Revision 1.23  2002/06/17 21:00:11  jlmiller
 * Clear undo buffer, if called for.
 *
 * Revision 1.22  2002/06/02 22:44:05  jlmiller
 * Add undoMan to shipeditor.
 * Commentability.
 *
 * Revision 1.21  2002/06/01 17:33:00  jlmiller
 * Fix a few nasty little bugs that would crash only when the Debugger WASNT running.
 * Commentability
 *
 * Revision 1.20  2002/05/30 18:33:58  jlmiller
 * Splitting of consecutive vertices now available.
 *
 * Revision 1.19  2002/05/30 17:14:21  jlmiller
 * Moving of items now available.
 *
 * Revision 1.18  2002/05/28 07:19:50  dick
 * Rename xpilotedit to XPilotControl
 *
 */
#include "StdAfx.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <math.h>

#include "portability.h"
#include "const.h"
#include "error.h"
#include "ShipDocument.h"
#include "xpprintf.h"


static int	debugShapeParsing = 0;
static int	verboseShapeParsing = 0;
static int	shapeLimits;

///////////////////////////////////////////////////////////////////////////////
void ShipDocument::Rotate_pointlist(XpPointList *ptl)
{
//    int			i;

//    for (i = 1; i < RES; i++) {
//	pt[i].x = tcos(i) * pt[0].x - tsin(i) * pt[0].y;
//	pt[i].y = tsin(i) * pt[0].x + tcos(i) * pt[0].y;
//    }
}


///////////////////////////////////////////////////////////////////////////////
ShipDocument::ShipDocument(int useDefault)
{
	//This Constructor will need to change should we ever support editing
	//Robot files, as right now they have different Max number's for several
	//Items.



	sel_start = false;
	start = end = NULL;

	SetMaximums();

	name.Add("Default");
	author.Add("Your Name");

	if (useDefault)
	{
		num_points = 3;
		pts.Add(new XpPoint(15, 0));
		pts.Add(new XpPoint(-9, 8));
		pts.Add(new XpPoint(-9, -8));
		engine.CheckAdd(new XpPoint(-9, 0));
		m_gun.CheckAdd(new XpPoint(15, 0));
		l_lights.CheckAdd(new XpPoint(-9, 8));
		r_lights.CheckAdd(new XpPoint(-9, -8));
		m_racks.CheckAdd(new XpPoint(15, 0));
	}
}
///////////////////////////////////////////////////////////////////////////////
ShipDocument* ShipDocument::copy()
{
	ShipDocument *cop = new ShipDocument(false);
	// *cop->Reset();
	if (cop)
	{
		//cop->pts = *pts.copy();
		cop->pts = pts;
		cop->m_gun = m_gun;
		cop->engine = engine;

		cop->m_racks = m_racks;
		cop->l_guns = l_guns;
		cop->r_guns = r_guns;
		cop->l_rguns = l_rguns;
		cop->r_rguns = r_rguns;
		cop->l_lights = l_lights;
		cop->r_lights = r_lights;
		cop->author = author;
		cop->name = name;
		return cop;
	}
	else
		return NULL;
}

///////////////////////////////////////////////////////////////////////////////
void ShipDocument::Reset()
{
	sel_start = false;
	start = end = NULL;
	SetMaximums();
	num_points = 0;

	pts.Empty();
	engine.Empty();
	m_gun.Empty();
	l_guns.Empty();
	r_guns.Empty();
	l_rguns.Empty();
	r_rguns.Empty();
	l_lights.Empty();
	r_lights.Empty();
	m_racks.Empty();
}

///////////////////////////////////////////////////////////////////////////////
ShipDocument::ShipDocument(PCSTR s)
{
	Convert_string_2_ship(s);
}

///////////////////////////////////////////////////////////////////////////////
void ShipDocument::Convert_string_2_ship(PCSTR ship_shape_str)
{
//Here We'll create a new ship from a string of text.
//Note: most of this code is lifted from the server and old editss.
//		it would be great to encapsulate ShipDocument into serverlib at somepoint.
#define GRID_PT(x,y)	grid.pt[(x)+15][(y)+15]
#define GRID_ADD(x,y)	(GRID_PT(x, y) = 2, \
						 grid.chk[grid.todo][0] = (x) + 15, \
						 grid.chk[grid.todo][1] = (y) + 15, \
						 grid.todo++)
#define GRID_GET(x,y)	((x) = (int)grid.chk[grid.done][0] - 15, \
						 (y) = (int)grid.chk[grid.done][1] - 15, \
						 grid.done++)
#define GRID_CHK(x,y)	(GRID_PT(x, y) == 2)
#define GRID_READY()	(grid.done >= grid.todo)
#define GRID_RESET()	(memset(grid.pt, 0, sizeof grid.pt), \
						 grid.done = 0, \
						 grid.todo = 0)

	struct grid_t {
		int 			todo, done;
		unsigned char	pt[32][32];
		unsigned char	chk[32*32][2];
	} grid;

	int 				i, j, x, y, dx, dy,
						inx, iny, max,
						ofNum, ofLeft, ofRight, 		/* old format */
						shape_version = 0;
	ipos				pt[MAX_SHIP_PTS],
						local_engine,
						local_m_gun,
						local_l_light[MAX_LIGHT_PTS],
						local_r_light[MAX_LIGHT_PTS],
						local_l_gun[MAX_GUN_PTS],
						local_r_gun[MAX_GUN_PTS],
						local_l_rgun[MAX_GUN_PTS],
						local_r_rgun[MAX_GUN_PTS],
						local_m_rack[MAX_RACK_PTS];
	bool				mainGunSet = false,
						engineSet = false;
	PCSTR				str;
	PCSTR 				teststr;
	char				keyw[20],
						buf[MSG_LEN];
	Reset();
	int count = 0;

	int   num_l_gun = 0;
	int   num_r_gun = 0;
	int   num_l_rgun = 0;
	int   num_r_rgun = 0;
	int   num_l_light = 0;
	int   num_r_light = 0;
	int   num_m_rack  = 0;


	if (debugShapeParsing) {
		xpprintf("parsing shape: %s\n", ship_shape_str);
	}

	for (str = ship_shape_str; (str = strchr(str, '(' )) != NULL; ) {

		str++;

		if (shape_version == 0) {
			if (isdigit(*str)) {
				shape_version = 0x3100;
				if (verboseShapeParsing) {
					xpprintf("ship shape is in old format\n");
				}
				break;
			}
			else {
				shape_version = 0x3200;
			}
		}

		for (i = 0; (keyw[i] = str[i]) != '\0'; i++) {
			if (i == sizeof(keyw) - 1) {
				keyw[i] = '\0';
				break;
			}
			if (keyw[i] == ':') {
				keyw[i + 1] = '\0';
				break;
			}
		}
		if (str[i] != ':') {
			if (verboseShapeParsing) {
				xpprintf("Missing colon in ship shape: %s\n", keyw);
			}
			continue;
		}
		for (teststr = &buf[++i]; (buf[i] = str[i]) != '\0'; i++) {
			if (buf[i] == ')' ) {
				buf[i++] = '\0';
				break;
			}
		}
		str += i;

		switch (Get_shape_keyword(keyw)) {

		case 0: 		/* Keyword is 'shape' */
			while (teststr) {
				while (*teststr == ' ') teststr++;
				if (sscanf(teststr, "%d,%d", &inx, &iny) != 2) {
					if (verboseShapeParsing) {
						xpprintf("Missing ship shape coordinate in: \"%s\"\n",
							   teststr);
					}
					break;
				}
				if (num_points >= MAX_SHIP_PTS) {
					if (verboseShapeParsing) {
						xpprintf("Too many ship shape coordinates\n");
					}
				}
				else {
					pt[num_points].x = inx;
					pt[num_points].y = iny;
					num_points++;
					if (debugShapeParsing) {
						xpprintf("ship point at %d,%d\n", inx, iny);
					}
				}
				teststr = strchr(teststr, ' ');
			}
			break;

		case 1: 		/* Keyword is 'mainGun' */
			if (mainGunSet) {
				if (verboseShapeParsing) {
					xpprintf("Ship shape keyword \"%s\" multiple defined\n",
						   keyw);
				}
				break;
			}
			while (*teststr == ' ') teststr++;
			if (sscanf(teststr, "%d,%d", &inx, &iny) != 2) {
				if (verboseShapeParsing) {
					xpprintf("Missing main gun coordinate in: \"%s\"\n",
						   teststr);
				}
			}
			else {
				local_m_gun.x = inx;
				local_m_gun.y = iny;
				mainGunSet = true;
				if (debugShapeParsing) {
					xpprintf("main gun at %d,%d\n", inx, iny);
				}
			}
			break;

		case 2: 		/* Keyword is 'leftGun' */
			while (teststr) {
				while (*teststr == ' ') teststr++;
				if (sscanf(teststr, "%d,%d", &inx, &iny) != 2) {
					if (verboseShapeParsing) {
						xpprintf("Missing left gun coordinate in: \"%s\"\n",
							   teststr);
					}
					break;
				}
				if (num_l_gun >= MAX_GUN_PTS) {
					if (verboseShapeParsing) {
						xpprintf("Too many left gun coordinates\n");
					}
				}
				else {
					local_l_gun[num_l_gun].x = inx;
					local_l_gun[num_l_gun].y = iny;
					num_l_gun++;
					if (debugShapeParsing) {
						xpprintf("left gun at %d,%d\n", inx, iny);
					}
				}
				teststr = strchr(teststr, ' ');
			}
			break;

		case 3: 		/* Keyword is 'rightGun' */
			while (teststr) {
				while (*teststr == ' ') teststr++;
				if (sscanf(teststr, "%d,%d" ,&inx, &iny) != 2) {
					if (verboseShapeParsing) {
						xpprintf("Missing right gun coordinate in: \"%s\"\n",
							   teststr);
					}
					break;
				}
				if (num_r_gun >= MAX_GUN_PTS) {
					if (verboseShapeParsing) {
						xpprintf("Too many right gun coordinates\n");
					}
				}
				else {
					local_r_gun[num_r_gun].x = inx;
					local_r_gun[num_r_gun].y = iny;
					num_r_gun++;
					if (debugShapeParsing) {
						xpprintf("right gun at %d,%d\n", inx, iny);
					}
				}
				teststr = strchr(teststr, ' ');
			}
			break;

		case 4: 		/* Keyword is 'leftLight' */
			while (teststr) {
				while (*teststr == ' ') teststr++;
				if (sscanf(teststr, "%d,%d", &inx, &iny) != 2) {
					if (verboseShapeParsing) {
						xpprintf("Missing left light coordinate in: \"%s\"\n",
							   teststr);
					}
					break;
				}
				if (num_l_light >= MAX_LIGHT_PTS) {
					if (verboseShapeParsing) {
						xpprintf("Too many left light coordinates\n");
					}
				}
				else {
					local_l_light[num_l_light].x = inx;
					local_l_light[num_l_light].y = iny;
					num_l_light++;
					if (debugShapeParsing) {
						xpprintf("left light at %d,%d\n", inx, iny);
					}
				}
				teststr = strchr(teststr, ' ');
			}
			break;

		case 5: 		/* Keyword is 'rightLight' */
			while (teststr) {
				while (*teststr == ' ') teststr++;
				if (sscanf(teststr, "%d,%d", &inx, &iny) != 2) {
					if (verboseShapeParsing) {
						xpprintf("Missing right light coordinate in: \"%s\"\n",
							   teststr);
					}
					break;
				}
				if (num_r_light >= MAX_LIGHT_PTS) {
					if (verboseShapeParsing) {
						xpprintf("Too many right light coordinates\n");
					}
				}
				else {
					local_r_light[num_r_light].x = inx;
					local_r_light[num_r_light].y = iny;
					num_r_light++;
					if (debugShapeParsing) {
						xpprintf("right light at %d,%d\n", inx, iny);
					}
				}
				teststr = strchr(teststr, ' ');
			}
			break;

		case 6: 		/* Keyword is 'engine' */
			if (engineSet) {
				if (verboseShapeParsing) {
					xpprintf("Ship shape keyword \"%s\" multiple defined\n",
						   keyw);
				}
				break;
			}
			while (*teststr == ' ') teststr++;
			if (sscanf(teststr, "%d,%d", &inx, &iny) != 2) {
				if (verboseShapeParsing) {
					xpprintf("Missing engine coordinate in: \"%s\"\n",
						   teststr);
				}
			}
			else {
				local_engine.x = inx;
				local_engine.y = iny;
				engineSet = true;
				if (debugShapeParsing) {
					xpprintf("engine at %d,%d\n", inx, iny);
				}
			}
			break;

		case 7: 		/* Keyword is 'missileRack' */
			while (teststr) {
				while (*teststr == ' ') teststr++;
				if (sscanf(teststr, "%d,%d", &inx, &iny) != 2) {
					if (verboseShapeParsing) {
						xpprintf("Missing missile rack coordinate in: \"%s\"\n",
							   teststr);
					}
					break;
				}
				if (num_m_rack >= MAX_RACK_PTS) {
					if (verboseShapeParsing) {
						xpprintf("Too many missile rack coordinates\n");
					}
				}
				else {
					local_m_rack[num_m_rack].x = inx;
					local_m_rack[num_m_rack].y = iny;
					num_m_rack++;
					if (debugShapeParsing) {
						xpprintf("missile rack at %d,%d\n", inx, iny);
					}
				}
				teststr = strchr(teststr, ' ');
			}
			break;

		case 8: 		/* Keyword is 'name' */
			name = teststr;
			break;

		case 9: 		/* Keyword is 'author' */
			author = teststr;
			break;

		case 10:				/* Keyword is 'leftRearGun' */
			while (teststr) {
				while (*teststr == ' ') teststr++;
				if (sscanf(teststr, "%d,%d", &inx, &iny) != 2) {
					if (verboseShapeParsing) {
						xpprintf("Missing left rear gun coordinate in: \"%s\"\n",
							   teststr);
					}
					break;
				}
				if (num_l_rgun >= MAX_GUN_PTS) {
					if (verboseShapeParsing) {
						xpprintf("Too many left rear gun coordinates\n");
					}
				}
				else {
					local_l_rgun[num_l_rgun].x = inx;
					local_l_rgun[num_l_rgun].y = iny;
					num_l_rgun++;
					if (debugShapeParsing) {
						xpprintf("left rear gun at %d,%d\n", inx, iny);
					}
				}
				teststr = strchr(teststr, ' ');
			}
			break;

		case 11:				/* Keyword is 'rightRearGun' */
			while (teststr) {
				while (*teststr == ' ') teststr++;
				if (sscanf(teststr, "%d,%d" ,&inx, &iny) != 2) {
					if (verboseShapeParsing) {
						xpprintf("Missing right rear gun coordinate in: \"%s\"\n",
							   teststr);
					}
					break;
				}
				if (num_r_rgun >= MAX_GUN_PTS) {
					if (verboseShapeParsing) {
						xpprintf("Too many right rear gun coordinates\n");
					}
				}
				else {
					local_r_rgun[num_r_rgun].x = inx;
					local_r_rgun[num_r_rgun].y = iny;
					num_r_rgun++;
					if (debugShapeParsing) {
						xpprintf("right rear gun at %d,%d\n", inx, iny);
					}
				}
				teststr = strchr(teststr, ' ');
			}
			break;

		default:
			if (verboseShapeParsing) {
				xpprintf("Invalid ship shape keyword: \"%s\"\n", keyw);
			}
			/* the good thing about this format is that we can just ignore
			 * this.  it is likely to be a new extension we don't know
			 * about yet. */
			break;
		}
	}

	if (shape_version == 0x3100) {
		str = ship_shape_str;

		if (sscanf(str, "(%d,%d,%d)", &ofNum, &ofLeft, &ofRight) != 3
			|| ofNum < MIN_SHIP_PTS
			|| ofNum > MAX_SHIP_PTS
			|| ofLeft < 0
			|| ofLeft >= ofNum
			|| ofRight < 0
			|| ofRight >= ofNum
			) {
			if (verboseShapeParsing) {
				xpprintf("Invalid ship shape header: \"%s\"\n", str);
			}
			return;
		}

		for (i = 0; i < ofNum; i++) {
			str = strchr(str + 1, '(');
			if (!str) {
				if (verboseShapeParsing) {
					xpprintf("Bad ship shape: "
						   "only %d points defined, %d expected\n",
						   i, ofNum);
				}
				return;
			}
			if (sscanf(str, "(%d,%d)", &inx, &iny) != 2) {
				if (verboseShapeParsing) {
					xpprintf("Bad ship shape: format error in point %d\n",
						   i);
				}
				return;
			}
			pt[i].x = inx;
			pt[i].y = iny;
		}

		num_points = ofNum;

		local_m_gun = pt[0];
		mainGunSet = true;

		local_l_light[0] = pt[ofLeft];
		num_l_light = 1;

		local_r_light[0] = pt[ofRight];
		num_r_light = 1;

		local_engine.x = (pt[ofLeft].x + pt[ofRight].x) / 2;
		local_engine.y = (pt[ofLeft].y + pt[ofRight].y) / 2;
		engineSet = true;
	}

	/* Check for some things being set, and give them defaults if not */

	if (num_points < 3) {
		if (verboseShapeParsing) {
			xpprintf("not enough ship points defined\n");
		}
		return;
	}
	if (!mainGunSet) {			/* No main gun set, put at foremost point */
		max = 0;
		for (i = 1; i < num_points; i++) {
			if (pt[i].x > pt[max].x
				|| (pt[i].x == pt[max].x
					&& ABS(pt[i].y) < ABS(pt[max].y))) {
				max = i;
			}
		}
		local_m_gun = pt[max];
		mainGunSet = true;
	}
	if (!l_lights.GetCount()) { /* No left light set, put at leftmost point */
		max = 0;
		for (i = 1; i < num_points; i++) {
			if (pt[i].y > pt[max].y
				|| (pt[i].y == pt[max].y
					&& pt[i].x <= pt[max].x)) {
				max = i;
			}
		}
		local_l_light[0] = pt[max];
//		num_l_light = 1;
	}
	if (!r_lights.GetCount()) { /* No right light set, put at rightmost point */
		max = 0;
		for (i = 1; i < num_points; i++) {
			if (pt[i].y < pt[max].y
				|| (pt[i].y == pt[max].y
					&& pt[i].x <= pt[max].x)) {
				max = i;
			}
		}
		local_r_light[0] = pt[max];
//		num_r_light = 1;
	}
	if (!engineSet) {			/* No engine position, put at rear of ship */
		max = 0;
		for (i = 1; i < num_points; i++) {
			if (pt[i].x < pt[max].x) {
				max = i;
			}
		}
		local_engine.x = pt[max].x;
		local_engine.y = 0; 			/* this may lay outside of ship. */
		engineSet = true;
	}
	if (!m_racks.GetCount()) {	/* No missile racks, put at main gun position*/
		local_m_rack[0] = local_m_gun;
//		num_m_rack = 1;
	}

	if (shapeLimits) {
		const int		isLow = -8, isHi = 8, isLeft = 8, isRight = -8,
						minLow = 1, minHi = 1, minLeft = 1, minRight = 1,
						horMax = 15, verMax = 15, horMin = -15, verMin = -15,
						minCount = 3, minSize = 22 + 16;
		int 			low = 0, hi = 0, left = 0, right = 0,
						count = 0, change, max = 0,
						lowest = 0, highest = 0, leftmost = 0, rightmost = 0;
		int 			invalid = 0;
		const int		checkWidthAgainstLongestAxis = 1;

		for (i = 0; i < num_points; i++) {
			x = pt[i].x;
			y = pt[i].y;
			change = 0;
			if (y >= isLeft) {
				change++, left++;
				if (y > leftmost) leftmost = y;
			}
			if (y <= isRight) {
				change++, right++;
				if (y < rightmost) rightmost = y;
			}
			if (x <= isLow) {
				change++, low++;
				if (x < lowest) lowest = x;
			}
			if (x >= isHi) {
				change++, hi++;
				if (x > highest) highest = x;
			}
			if (change) count++;
			if (y > horMax || y < horMin) max++;
			if (x > verMax || x < verMin) max++;
		}
		if (low < minLow
			|| hi < minHi
			|| left < minLeft
			|| right < minRight
			|| count < minCount) {
			if (verboseShapeParsing) {
				xpprintf("Ship shape does not meet size requirements (%d,%d,%d,%d,%d)\n",
					   low, hi, left, right, count);
			}
			return;
		}
		if (max != 0) {
			if (verboseShapeParsing) {
				xpprintf("Ship shape exceeds size maxima.\n");
			}
			return;
		}
		if (leftmost - rightmost + highest - lowest < minSize) {
			if (verboseShapeParsing) {
				xpprintf("Ship shape is not big enough.\n"
					   "The ship's width and height added together should\n"
					   "be at least %d.\n", minSize);
			}
			return;
		}

		if (checkWidthAgainstLongestAxis) {
			/*
			 * For making sure the ship is the right width!
			 */
			int pair[2];
			int dist = 0, tmpDist = 0;
			double vec[2], width, dTmp;
			const int minWidth = 12;

			/*
			 * Loop over all the points and find the two furthest apart
			 */
			for (i = 0; i < num_points; i++) {
				for (j = i + 1; j < num_points; j++) {
					/*
					 * Compare the points if they are not the same ones.
					 * Get this distance -- doesn't matter about sqrting
					 * it since only size is important.
					 */
					if ((tmpDist = ((pt[i].x - pt[j].x) * (pt[i].x - pt[j].x) +
									(pt[i].y - pt[j].y) * (pt[i].y - pt[j].y)))
						> dist) {
						/*
						 * Set new separation thingy.
						 */
						dist = tmpDist;
						pair[0] = i;
						pair[1] = j;
					}
				}
			}

			/*
			 * Now we know the vector that is _|_ to the one above
			 * is simply found by (x,y) -> (y,-x) => dot-prod = 0
			 */
			vec[0] = (double)(pt[pair[1]].y - pt[pair[0]].y);
			vec[1] = (double)(pt[pair[0]].x - pt[pair[1]].x);

			/*
			 * Normalise
			 */
			dTmp = LENGTH(vec[0], vec[1]);
			vec[0] /= dTmp;
			vec[1] /= dTmp;

			/*
			 * Now check the width _|_ to the ship main line.
			 */
			for (i = 0, width = dTmp = 0.0; i < num_points; i++) {
				for (j = i + 1; j < num_points; j++) {
					/*
					 * Check the line if the points are not the same ones
					 */
					if ((width = fabs(vec[0] * (double)(pt[i].x - pt[j].x) +
									  vec[1] * (double)(pt[i].y - pt[j].y)))
						> dTmp) {
						dTmp = width;
					}
				}
			}

			/*
			 * And make sure it is nice and far away
			 */
			if (((int)dTmp) < minWidth) {
				if (verboseShapeParsing) {
					printf("Ship shape is not big enough.\n"
						   "The ship's width should be at least %d.\n"
						   "Player's is %d\n", minWidth, (int)dTmp);
				}
				return;
			}
		}

		/*
		 * Check that none of the special points are outside the
		 * shape defined by the normal points.
		 * First the shape is drawn on a grid.	Then all grid points
		 * on the outside of the shape are marked.	Thusly for each
		 * special point can be determined if it is outside the shape.
		 */
		GRID_RESET();

		/* Draw the ship outline first. */
		for (i = 0; i < num_points; i++) {
			j = i + 1;
			if (j == num_points) j = 0;

			GRID_PT(pt[i].x, pt[i].y) = 1;

			dx = pt[j].x - pt[i].x;
			dy = pt[j].y - pt[i].y;
			if (ABS(dx) >= ABS(dy)) {
				if (dx > 0) {
					for (x = pt[i].x + 1; x < pt[j].x; x++) {
						y = pt[i].y + (dy * (x - pt[i].x)) / dx;
						GRID_PT(x, y) = 1;
					}
				}
				else {
					for (x = pt[j].x + 1; x < pt[i].x; x++) {
						y = pt[j].y + (dy * (x - pt[j].x)) / dx;
						GRID_PT(x, y) = 1;
					}
				}
			}
			else {
				if (dy > 0) {
					for (y = pt[i].y + 1; y < pt[j].y; y++) {
						x = pt[i].x + (dx * (y - pt[i].y)) / dy;
						GRID_PT(x, y) = 1;
					}
				}
				else {
					for (y = pt[j].y + 1; y < pt[i].y; y++) {
						x = pt[j].x + (dx * (y - pt[j].y)) / dy;
						GRID_PT(x, y) = 1;
					}
				}
			}
		}

		/* Check the borders of the grid for blank points. */
		for (y = -15; y <= 15; y++) {
			for (x = -15; x <= 15; x += (y == -15 || y == 15) ? 1 : 2*15) {
				if (GRID_PT(x, y) == 0) {
					GRID_ADD(x, y);
				}
			}
		}

		/* Check from the borders of the grid to the centre. */
		while (!GRID_READY()) {
			GRID_GET(x, y);
			if (x <  15 && GRID_PT(x + 1, y) == 0) GRID_ADD(x + 1, y);
			if (x > -15 && GRID_PT(x - 1, y) == 0) GRID_ADD(x - 1, y);
			if (y <  15 && GRID_PT(x, y + 1) == 0) GRID_ADD(x, y + 1);
			if (y > -15 && GRID_PT(x, y - 1) == 0) GRID_ADD(x, y - 1);
		}

		/*
		 * Note that for the engine, old format shapes may well have the
		 * engine position outside the ship, so this check not used for those.
		 */

		if (GRID_CHK(local_m_gun.x, local_m_gun.y)) {
			if (verboseShapeParsing) {
				xpprintf("Main gun outside ship\n");
			}
			invalid++;
		}
		for (i = 0; i < num_l_gun; i++) {
			if (GRID_CHK(local_l_gun[i].x, local_l_gun[i].y)) {
				if (verboseShapeParsing) {
					xpprintf("Left gun %d outside ship\n", i);
				}
				invalid++;
			}
		}
		for (i = 0; i < num_r_gun; i++) {
			if (GRID_CHK(local_r_gun[i].x, local_r_gun[i].y)) {
				if (verboseShapeParsing) {
					xpprintf("Right gun %d outside ship\n", i);
				}
				invalid++;
			}
		}
		for (i = 0; i < num_l_rgun; i++) {
			if (GRID_CHK(local_l_rgun[i].x, local_l_rgun[i].y)) {
				if (verboseShapeParsing) {
					xpprintf("Left rear gun %d outside ship\n", i);
				}
				invalid++;
			}
		}
		for (i = 0; i < num_r_rgun; i++) {
			if (GRID_CHK(local_r_rgun[i].x, local_r_rgun[i].y)) {
				if (verboseShapeParsing) {
					xpprintf("Right rear gun %d outside ship\n", i);
				}
				invalid++;
			}
		}
		for (i = 0; i < num_l_light; i++) {
			if (GRID_CHK(local_l_light[i].x, local_l_light[i].y)) {
				if (verboseShapeParsing) {
					xpprintf("Left light %d outside ship\n", i);
				}
				invalid++;
			}
		}
		count = r_lights.GetCount();
		for (i = 0; i < num_r_light; i++) {
			if (GRID_CHK(local_r_light[i].x, local_r_light[i].y)) {
				if (verboseShapeParsing) {
					xpprintf("Right light %d outside ship\n", i);
				}
				invalid++;
			}
		}
		for (i = 0; i < num_m_rack; i++) {
			if (GRID_CHK(local_m_rack[i].x, local_m_rack[i].y)) {
				if (verboseShapeParsing) {
					xpprintf("Missile rack %d outside ship\n", i);
				}
				invalid++;
			}
		}
		if (GRID_CHK(local_engine.x, local_engine.y)) {
			if (verboseShapeParsing) {
				xpprintf("Engine outside of ship\n");
			}
			invalid++;
			/* this could happen in case of an old format ship shape. */
			if (shape_version == 0x3100 && invalid == 1) {
				/* move engine until it is legal. */
				for (x = -15, y = 0; x <= 15; x++) {
					if (!GRID_CHK(x, y)) {
						local_engine.x = x;
						local_engine.y = y;
						invalid--;
						break;
					}
				}
			}
		}

		if (debugShapeParsing) {
			for (i = -15; i <= 15; i++) {
				for (j = -15; j <= 15; j++) {
					switch (GRID_PT(j, i)) {
					case 0: putchar(' '); break;
					case 1: putchar('*'); break;
					case 2: putchar('.'); break;
					default: putchar('?'); break;
					}
				}
				putchar('\n');
			}
		}

		if (invalid != 0) {
			return;
		}
	}

	for (i = 0; i < num_points; i++)
		pts.Add(new XpPoint(pt[i].x, pt[i].y));
	if (engineSet)
		engine.Add(new XpPoint(local_engine.x, local_engine.y));
	if (mainGunSet)
		m_gun.Add(new XpPoint(local_m_gun.x, local_m_gun.y));
	for (i = 0; i < num_l_gun; i++)
		l_guns.Add(new XpPoint(local_l_gun[i].x, local_l_gun[i].y));
	for (i = 0; i < num_r_gun; i++)
		r_guns.Add(new XpPoint(local_r_gun[i].x, local_r_gun[i].y));
	for (i = 0; i < num_l_rgun; i++)
		l_rguns.Add(new XpPoint(local_l_rgun[i].x, local_l_rgun[i].y));
	for (i = 0; i < num_r_rgun; i++)
		r_rguns.Add(new XpPoint(local_r_rgun[i].x, local_r_rgun[i].y));
	for (i = 0; i < num_l_light; i++)
		l_lights.Add(new XpPoint(local_l_light[i].x, local_l_light[i].y));
	for (i = 0; i < num_r_light; i++)
		r_lights.Add(new XpPoint(local_r_light[i].x, local_r_light[i].y));
	for (i = 0; i < num_m_rack; i++)
		m_racks.Add(new XpPoint(local_m_rack[i].x, local_m_rack[i].y));

//		PumpShip(&Rotate_pointlist);

}

///////////////////////////////////////////////////////////////////////////////
int ShipDocument::Get_shape_keyword(char *keyw)
{
#define NUM_SHAPE_KEYS	12

	static char 		shape_keys[NUM_SHAPE_KEYS][16] = {
							"shape:",
							"mainGun:",
							"leftGun:",
							"rightGun:",
							"leftLight:",
							"rightLight:",
							"engine:",
							"missileRack:",
							"name:",
							"author:",
							"leftRearGun:",
							"rightRearGun:",
						};
	static char 		abbrev_keys[NUM_SHAPE_KEYS][4] = {
							"SH:",
							"MG:",
							"LG:",
							"RG:",
							"LL:",
							"RL:",
							"EN:",
							"MR:",
							"NM:",
							"AU:",
							"LR:",
							"RR:",
						};
	int 				i;

	/* non-abbreviated keywords start with a lower case letter. */
	if (islower(*keyw)) {
		for (i = 0; i < NUM_SHAPE_KEYS; i++) {
			if (!strcmp(keyw, shape_keys[i])) {
				break;
			}
		}
	}
	/* abbreviated keywords start with an upper case letter. */
	else if (isupper(*keyw)) {
		for (i = 0; i < NUM_SHAPE_KEYS; i++) {
			if (!strcmp(keyw, abbrev_keys[i])) {
				break;
			}
		}
	}
	/* dunno what this is. */
	else {
		i = -1;
	}
	return(i);
}
///////////////////////////////////////////////////////////////////////////////
String ShipDocument::Convert_ship_2_string()
{
	int	i;
	String		buffer;
	XpPoint*	tmppt;

	String	t;
	buffer.printf("(NM:%s)", (PCSTR)name);
	buffer += t.printf("(AU:%s)", (PCSTR)author);

	tmppt = (XpPoint *) pts.GetHead();

	buffer += "(SH:";
	int		npts = pts.GetCount();
	for (i = 0; i < npts; i++) {
		buffer += t.printf(" %d,%d", tmppt->x, tmppt->y);
		tmppt = (XpPoint *) tmppt->GetNext();
	}
	buffer += ")";

	tmppt = (XpPoint *) engine.GetHead();
	if (tmppt)
	{
		buffer += t.printf("(EN: %d,%d)", tmppt->x, tmppt->y);
	}

	tmppt = (XpPoint *) m_gun.GetHead();
	if (tmppt)
	{
		buffer += t.printf("(MG: %d,%d)", tmppt->x, tmppt->y);
	}

	npts = l_guns.GetCount();
	if (npts > 0) {
		tmppt = (XpPoint *) l_guns.GetHead();
		buffer += "(LG:";
		for (i = 0; i < npts; i++) {
			buffer += t.printf(" %d,%d", tmppt->x, tmppt->y);
			tmppt = (XpPoint *) tmppt->GetNext();
		}
		buffer += ")";
	}
	npts = r_guns.GetCount();
	if (npts > 0) {
		tmppt = (XpPoint *) r_guns.GetHead();
		buffer += "(RG:";
		for (i = 0; i < npts; i++) {
			buffer += t.printf(" %d,%d", tmppt->x, tmppt->y);
			tmppt = (XpPoint *) tmppt->GetNext();
		}
	}

	npts = l_rguns.GetCount();
	if (npts > 0) {
		tmppt = (XpPoint *) l_rguns.GetHead();
		buffer += "(LR:";
		for (i = 0; i < npts; i++) {
			buffer += t.printf(" %d,%d", tmppt->x, tmppt->y);
			tmppt = (XpPoint *) tmppt->GetNext();
		}
		buffer += ")";
	}

	npts = r_rguns.GetCount();
	if (npts > 0) {
		tmppt = (XpPoint *) r_rguns.GetHead();
		buffer += "(RR:";
		for (i = 0; i < npts; i++) {
			buffer += t.printf(" %d,%d", tmppt->x, tmppt->y);
			tmppt = (XpPoint *) tmppt->GetNext();
		}
		buffer += ")";
	}

	npts = l_lights.GetCount();
	if (npts > 0) {
		tmppt = (XpPoint *) l_lights.GetHead();
		buffer += "(LL:";
		for (i = 0; i < npts; i++) {
			buffer += t.printf(" %d,%d", tmppt->x, tmppt->y);
			tmppt = (XpPoint *) tmppt->GetNext();
		}
		buffer += ")";
	}

	npts = r_lights.GetCount();
	if (npts > 0) {
		tmppt = (XpPoint *) r_lights.GetHead();
		buffer += "(RL:";
		for (i = 0; i < npts; i++) {
			buffer += t.printf(" %d,%d", tmppt->x, tmppt->y);
			tmppt = (XpPoint *) tmppt->GetNext();
		}
		buffer += ")";
	}

	npts = m_racks.GetCount();
	if (npts > 0) {
		buffer += "(MR:";
		tmppt = (XpPoint *) m_racks.GetHead();
		for (i = 0; i < npts; i++) {
			buffer += t.printf(" %d,%d", tmppt->x, tmppt->y);
			tmppt = (XpPoint *) tmppt->GetNext();
		}
		buffer += ")";
	}
	return(buffer);
}

///////////////////////////////////////////////////////////////////////////////
void ShipDocument::ShiftLeft(XpPoint *pt)
{
	pt->x += -1;
}

///////////////////////////////////////////////////////////////////////////////
void ShipDocument::ShiftRight(XpPoint *pt)
{
	pt->x += +1;
}
///////////////////////////////////////////////////////////////////////////////
void ShipDocument::ShiftUp(XpPoint *pt)
{
	pt->y += +1;
}

///////////////////////////////////////////////////////////////////////////////
void ShipDocument::ShiftDown(XpPoint *pt)
{
	pt->y += -1;
}
///////////////////////////////////////////////////////////////////////////////
void ShipDocument::MirrorHoriz(XpPoint *pt)
{
	pt->x = -pt->x;
}
///////////////////////////////////////////////////////////////////////////////
void ShipDocument::MirrorVert(XpPoint *pt)
{
	pt->y = -pt->y;
}
///////////////////////////////////////////////////////////////////////////////
void ShipDocument::RotateLeft(XpPoint *pt)
{
	//Rotate around the point 0,0, counter clockwise
	XpPoint tmp;
	tmp.x = pt->x;
	tmp.y = pt->y;

	pt->x = -tmp.y;
	pt->y = tmp.x;
}
///////////////////////////////////////////////////////////////////////////////
void ShipDocument::RotateRight(XpPoint *pt)
{
	//Rotate around the point 0,0, clockwise
	XpPoint tmp;
	tmp.x = pt->x;
	tmp.y = pt->y;

	pt->x = tmp.y;
	pt->y = -tmp.x;
}
///////////////////////////////////////////////////////////////////////////////
bool ShipDocument::AtEdge(XpPoint *pt, int d)
{
	//Are we already at the edge of the valid shipfield?
	//if yes return true
	switch (d)
	{
	case 1: if (pt->x == -15)
		return true;
		break;
	case 2: if (pt->x == 15)
		return true;
		break;
	case 3: if (pt->y == 15)
		return true;
		break;
	case 4: if (pt->y == -15)
		return true;
		break;
	}
	return false;
}
///////////////////////////////////////////////////////////////////////////////
void ShipDocument::PumpShip(PumpFunc pf)
{
	//Cycle through all the ship object lists, performing the specified
	//function on them. This allows us to easily move or rotate etc the entire ship.
	PumpShip(pf, pts);
	PumpShip(pf, m_gun);
	PumpShip(pf, engine);
	PumpShip(pf, m_racks);
	PumpShip(pf, l_guns);
	PumpShip(pf, r_guns);
	PumpShip(pf, l_rguns);
	PumpShip(pf, r_rguns);
	PumpShip(pf, l_lights);
	PumpShip(pf, r_lights);
}

///////////////////////////////////////////////////////////////////////////////
void ShipDocument::PumpShip(PumpFunc pf, XpPointList& ol)
{
	//Cycle through all the points in an object list, performing the specified
	//function on each one.
	XpPoint*	pt = (XpPoint*)ol.GetHead();
	while(pt)
	{
		(this->*pf)(pt);
		pt = (XpPoint*)pt->GetNext();
	}
}

///////////////////////////////////////////////////////////////////////////////
bool ShipDocument::PumpShipBool(PumpFuncBool pf, XpPointList& ol, int d)
{
	//Similar to the above, but if any of the functions called return false
	//we go no further. Usefull for verification on ships, such as position in the field.
	XpPoint*	pt = (XpPoint*)ol.GetHead();
	while(pt)
	{
		if ( (this->*pf)(pt, d) ) //call a bool function
			return true;

		pt = (XpPoint*)pt->GetNext();
	}
	return false;
}
///////////////////////////////////////////////////////////////////////////////
bool ShipDocument::VerifyMove(int dir)
{
	if (PumpShipBool(&ShipDocument::AtEdge, pts, dir))
		return false; //We cant move
	else if (PumpShipBool(&ShipDocument::AtEdge, m_gun, dir))
		return false; //We cant move
	else if (PumpShipBool(&ShipDocument::AtEdge, engine, dir))
		return false; //We cant move
	else if (PumpShipBool(&ShipDocument::AtEdge, m_racks, dir))
		return false; //We cant move
	else if (PumpShipBool(&ShipDocument::AtEdge, l_guns, dir))
		return false; //We cant move
	else if (PumpShipBool(&ShipDocument::AtEdge, r_guns, dir))
		return false; //We cant move
	else if (PumpShipBool(&ShipDocument::AtEdge, l_rguns, dir))
		return false; //We cant move
	else if (PumpShipBool(&ShipDocument::AtEdge, r_rguns, dir))
		return false; //We cant move
	else if (PumpShipBool(&ShipDocument::AtEdge, l_lights, dir))
		return false; //We cant move
	else if (PumpShipBool(&ShipDocument::AtEdge, r_lights, dir))
		return false; //We cant move
	else
		return true; //We can move


}
///////////////////////////////////////////////////////////////////////////////
void ShipDocument::SetMaximums()
{
	pts.SetMax(MAX_SHIP_PTS);

	engine.SetMax(1);

	m_gun.SetMax(1);

	l_lights.SetMax(MAX_LIGHT_PTS);

	r_lights.SetMax(MAX_LIGHT_PTS);

	m_racks.SetMax(MAX_RACK_PTS);


	l_guns.SetMax(MAX_GUN_PTS);
	r_guns.SetMax(MAX_GUN_PTS);
	l_rguns.SetMax(MAX_GUN_PTS);
	r_rguns.SetMax(MAX_GUN_PTS);
}
