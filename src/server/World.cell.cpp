/* $Id: World.cell.cpp,v 1.2 2004/02/03 03:39:54 dick Exp $
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
 *  $Log: World.cell.cpp,v $
 *  Revision 1.2  2004/02/03 03:39:54  dick
 *  Add Windows precompiled header support.
 *
 *  Revision 1.1  2002/09/11 16:39:50  dick
 *  cell.cpp becomes World.cell.cpp; collision.cpp becomes World.collision.cpp
 *
 *  Revision 1.7  2002/09/01 00:13:46  dick
 *  Remove obsolete windows #include "winServer.h"
 *
 *  Revision 1.6  2002/06/01 06:06:48  dick
 *  Encapsulate (almost) everything.  Get rid of (almost) all refs to theWorld.
 *
 *  Revision 1.5  2002/05/18 20:55:38  dick
 *  Update to XPilot-4.5.1
 *
 *  Revision 1.4  2001/08/11 00:36:52  dick
 *  frame.cpp becomes Frame.cpp.  object.h becomes Object.h
 *
 *  Revision 1.3  2001/07/23 07:38:42  dick
 *  Fold contact, metaserver, netserver, rules, and server to be members of World.
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
#include <limits.h>
#include <assert.h>

#include "version.h"
#include "config.h"
#include "serverconst.h"
#include "global.h"
#include "proto.h"
#include "error.h"


char cell_version[] = VERSION;


/* we only search for objects which are at most 28 blocks away. */
#define MAX_CELL_DIST			28

/* sqrt(2) */
#undef SQRT2
#define SQRT2	1.41421356237309504880


class CellDist
{
public:
	float		dist;
	short		x;
	short		y;
};

class CellOffset
{
public:
	short		x;
	short		y;
};


void World::FreeCellDist(void)
{
	if (cellDist) {
		free(cellDist);
		cellDist = NULL;
	}
}


static int CompareCellDist(const void *a, const void *b)
{
	const CellDist*	c = (const CellDist*) a;
	const CellDist*	d = (const CellDist*) b;
	int acx, acy, adx, ady, maxc, maxd;

	if (c->dist < d->dist) return -1;
	if (c->dist > d->dist) return +1;
	acx = ABS(c->x);
	acy = ABS(c->y);
	adx = ABS(d->x);
	ady = ABS(d->y);
	maxc = MAX(acx, acy);
	maxd = MAX(adx, ady);
	if (maxc < maxd) return -1;
	if (maxc > maxd) return +1;
	return 0;
}


void World::InitCellDist()
{
	CellDist*	dists;
	int 		x, y;
	int 		cell_dist_width;
	int 		cell_dist_height;
	int 		cell_max_left;
	int 		cell_max_right;
	int 		cell_max_up;
	int 		cell_max_down;

	FreeCellDist();

	if (BIT(rules->mode, WRAP_PLAY)) {
		cell_max_right = MIN(MAX_CELL_DIST, (blockWidth / 2));
		cell_max_left = MIN(MAX_CELL_DIST, ((blockWidth - 1) / 2));
		cell_max_up = MIN(MAX_CELL_DIST, (blockHeight / 2));
		cell_max_down = MIN(MAX_CELL_DIST, ((blockHeight - 1) / 2));
	}
	else {
		cell_max_right = MIN(MAX_CELL_DIST, (blockWidth - 1));
		cell_max_left = MIN(MAX_CELL_DIST, (blockWidth - 1));
		cell_max_up = MIN(MAX_CELL_DIST, (blockHeight - 1));
		cell_max_down = MIN(MAX_CELL_DIST, (blockHeight - 1));
	}
	cell_dist_width = cell_max_left + 1 + cell_max_right;
	cell_dist_height = cell_max_down + 1 + cell_max_up;
	cellDistSize = cell_dist_width * cell_dist_height;

	cellDist = (CellDist *)malloc(cellDistSize * sizeof(CellDist));
	if (cellDist == NULL) {
				error("No cell dist mem");
				EndGame();
				return;
	}

	dists = cellDist;
	for (y = -cell_max_down; y <= cell_max_up; y++) {
		for (x = -cell_max_left; x <= cell_max_right; x++) {
			dists->x = x;
			dists->y = y;
			dists->dist = (float) LENGTH(x, y);
			dists++;
		}
	}

	qsort(cellDist, cellDistSize, sizeof(CellDist), CompareCellDist);
}


void World::FreeCells()
{
	if (cells) {
		free(cells);
		cells = NULL;
	}

	FreeCellDist();
}


void World::AllocCells()
{
	unsigned			size;
	CellNode			*cell_ptr;
	int 				x, y;

	FreeCells();

	size = sizeof(CellNode *) * blockWidth;
	size += sizeof(CellNode) * blockWidth * blockHeight;
	if (!(cells = (CellNode **) malloc(size))) {
		error("No Cell mem");
		EndGame();
		return;
	}
	cell_ptr = (CellNode *) &cells[blockWidth];
	for (x = 0; x < blockWidth; x++) {
		cells[x] = cell_ptr;
		for (y = 0; y < blockHeight; y++) {
			/* init list to point to itself. */
			cell_ptr->next = cell_ptr;
			cell_ptr->prev = cell_ptr;
			cell_ptr++;
		}
	}

	InitCellDist();
}


void World::CellInitObject(Object *obj)
{
	/* put obj on list with only itself. */
	obj->cell.next = &(obj->cell);
	obj->cell.prev = &(obj->cell);

	if (objectNodeOffset == 0) {
		objectNodeOffset = ((char *)&(obj->cell) - (char *)obj);
	}
}


void World::CellAddObject(Object *obj)
{
	int 				bx, by;
	CellNode			*obj_node_ptr, *cell_node_ptr;
	CellNode			*prev, *next;

	bx = OBJ_X_IN_BLOCKS(obj);
	by = OBJ_Y_IN_BLOCKS(obj);

	obj_node_ptr = &(obj->cell);
	next = obj_node_ptr->next;
	prev = obj_node_ptr->prev;

	assert(next->prev == obj_node_ptr);
	assert(prev->next == obj_node_ptr);

	/* remove obj from current list */
	next->prev = prev;
	prev->next = next;

	if (bx < 0 || bx >= blockWidth || by < 0 || by >= blockHeight) {
		/* put obj on list with only itself. */
		obj_node_ptr->next = obj_node_ptr;
		obj_node_ptr->prev = obj_node_ptr;
	}
	else {
		/* put obj in cell list. */
		cell_node_ptr = &cells[bx][by];
		obj_node_ptr->next = cell_node_ptr->next;
		obj_node_ptr->prev = cell_node_ptr;
		cell_node_ptr->next->prev = obj_node_ptr;
		cell_node_ptr->next = obj_node_ptr;
	}
}


void World::CellRemoveObject(Object *obj)
{
	CellNode			*obj_node_ptr;
	CellNode			*next, *prev;

	obj_node_ptr = &(obj->cell);
	next = obj_node_ptr->next;
	prev = obj_node_ptr->prev;

	assert(next->prev == obj_node_ptr);
	assert(prev->next == obj_node_ptr);

	/* remove obj from current list */
	next->prev = prev;
	prev->next = next;

	/* put obj on list with only itself. */
	obj_node_ptr->next = obj_node_ptr;
	obj_node_ptr->prev = obj_node_ptr;

}


void World::CellGetObjects(
		int x,
		int y,
		int range,
		int max_obj_count,
		Object ***obj_list,
		int *count_ptr)
{
	static Object		*ObjectList[MAX_TOTAL_SHOTS + 1];
	int 				i, count;
	int 				xw, yw;
	int 				wrap;
	Object				*obj;
	CellNode			*cell_node_ptr, *next;
	float				dist;


	wrap = (BIT(rules->mode, WRAP_PLAY) != 0);
	dist = (float) (range * SQRT2);
	count = 0;
	for (i = 0; i < cellDistSize && count < max_obj_count; i++) {
		if (dist < cellDist[i].dist) {
			break;
		}
		else {
			xw = x + cellDist[i].x;
			yw = y + cellDist[i].y;
			if (xw < 0) {
				if (wrap) {
					xw += blockWidth;
				} else {
					continue;
				}
			}
			else if (xw >= blockWidth) {
				if (wrap) {
					xw -= blockWidth;
				} else {
					continue;
				}
			}
			if (yw < 0) {
				if (wrap) {
					yw += blockHeight;
				} else {
					continue;
				}
			}
			else if (yw >= blockHeight) {
				if (wrap) {
					yw -= blockHeight;
				} else {
					continue;
				}
			}
			cell_node_ptr = &cells[xw][yw];
			next = cell_node_ptr->next;
			while (next != cell_node_ptr && count < max_obj_count) {
				obj = (Object *)((char *)next - objectNodeOffset);
				ObjectList[count++] = obj;
				next = next->next;
			}
		}
	}

	ObjectList[count] = NULL;
	*obj_list = &ObjectList[0];
	if (count_ptr != NULL) {
		*count_ptr = count;
	}
}


