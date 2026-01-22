/* $Id: Object.cpp,v 1.6 2004/02/23 07:21:46 dick Exp $
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
 */
/*
 *  $Log: Object.cpp,v $
 *  Revision 1.6  2004/02/23 07:21:46  dick
 *  Argh.  Use SmartObject as our "largest" object in the big wrong malloc.
 *  MineObject, the previous choice of largest, was incorrect.
 *  (It's wrong in a C++ way).
 *
 *  Revision 1.5  2004/02/03 03:39:54  dick
 *  Add Windows precompiled header support.
 *
 *  Revision 1.4  2002/09/11 16:42:03  dick
 *  Here endeth the server encapsulation task.
 *  theWorld is now an array of World(s) and get loaded dynamically.
 *
 *  Revision 1.3  2002/09/01 00:13:45  dick
 *  Remove obsolete windows #include "winServer.h"
 *
 *  Revision 1.2  2002/06/01 06:06:47  dick
 *  Encapsulate (almost) everything.  Get rid of (almost) all refs to theWorld.
 *
 *  Revision 1.1  2002/05/18 20:55:38  dick
 *  Update to XPilot-4.5.1
 */

/*
 * This file deals with low-level object structure manipulations.
 */
#include "StdAfx.h"

#include <stdlib.h>

#include "version.h"
#include "config.h"
#include "types.h"
#include "serverconst.h"
#include "global.h"
#include "proto.h"
#include "error.h"
#include "portability.h"
#include "commonproto.h"


char object_version[] = VERSION;


/*
 * Global variables
 */
//int				objCount = 0;
#define	objCount	w->numObjs			// yuck


static void Object_incr_count(World* w)
{
    objCount++;
}

static void Object_decr_count(World* w)
{
    objCount--;
}

Object *Object_allocate(World* w)
{
    Object	*obj = OBJ_PTR(NULL);

    if (objCount < MAX_TOTAL_SHOTS) {
		obj = w->objs[objCount];
		Object_incr_count(w);

		obj->type = OBJ_DEBRIS;
		obj->life = 0;
    }

    return obj;
}

void Object_free_ind(World* w, int ind)
{
    if ((0 <= ind) && (ind < objCount) && (objCount <= MAX_TOTAL_SHOTS)) {
		Object *obj = w->objs[ind];
		Object_decr_count(w);
		w->objs[ind] = w->objs[objCount];
		w->objs[objCount] = obj;
    }
    else {
		warn("Cannot free object %d, when count = %d, and total = %d !",
		     ind, objCount, MAX_TOTAL_SHOTS);
    }
}

void Object_free_ptr(Object *obj)
{
	World*	w = obj->world;
    int		i;

    for (i = objCount - 1; i >= 0; i--) {
		if (w->objs[i] == obj) {
			Object_free_ind(w, i);
			break;
		}
    }
    if (i < 0) {
		warn("Could NOT free object!");
    }
}


// This totally super sucks in a C++ way.
// Allocing a big array and calling it objects. bleh.
// We can't use anyobject.  Use SmartObject as are supposed largest possible object
//static anyobject *objArray;
static Object *objArray;

void Alloc_shots(World* w, int number)
{
    SmartObject      *x;
    int         i;

    x = (SmartObject *) calloc(number, sizeof(SmartObject));
    if (!x) {
		error("Not enough memory for shots.");
		exit(1);
    }

    objArray = x;
    for (i = 0; i < number; i++) {
		w->objs[i] = x;
		w->objs[i]->world = w;
		w->CellInitObject(w->objs[i]);
		x++;
    }
}

// FIXME: Broken (frees all world's shots)
void Free_shots(World* w)
{
    if (objArray != NULL) {
	free(objArray);
	objArray = NULL;
    }
}

