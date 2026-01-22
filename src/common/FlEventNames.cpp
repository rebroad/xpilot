/* $Id: FlEventNames.cpp,v 1.3 2004/02/03 03:39:52 dick Exp $
 *
 * FlEventNames - convert FLTK event numbers to names
 *
 *      Copyright (C) 2001 by
 *
 *      The XPilot Authors   <xpilot@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
 *      Jarrod Miller        <jarrod@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
 *      Bjørn Stabell        <bjoern@xpilot.org>
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
 * $Log: FlEventNames.cpp,v $
 * Revision 1.3  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.2  2002/08/14 19:39:24  dick
 * Use local fltk ( "" instead of <> )
 *
 * Revision 1.1  2002/05/26 19:37:38  bert
 * Add FlEventNames.cpp FlEventNames.h to convert FLTK events to strings.
 *
 */
#include "StdAfx.h"

#include "FlEventNames.h"
#include "FL/Enumerations.H"
#include "const.h"

struct EventName {
	int	number;
	const char *name;
};

static struct EventName eventNames[] =
{
	{ FL_NO_EVENT         , "FL_NO_EVENT" },
	{ FL_PUSH             , "FL_PUSH" },
	{ FL_RELEASE          , "FL_RELEASE" },
	{ FL_ENTER            , "FL_ENTER" },
	{ FL_LEAVE            , "FL_LEAVE" },
	{ FL_DRAG             , "FL_DRAG" },
	{ FL_FOCUS            , "FL_FOCUS" },
	{ FL_UNFOCUS          , "FL_UNFOCUS" },
	{ FL_KEYDOWN          , "FL_KEYDOWN" },
	{ FL_KEYUP            , "FL_KEYUP" },
	{ FL_CLOSE            , "FL_CLOSE" },
	{ FL_MOVE             , "FL_MOVE" },
	{ FL_SHORTCUT         , "FL_SHORTCUT" },
	{ FL_DEACTIVATE       , "FL_DEACTIVATE" },
	{ FL_ACTIVATE         , "FL_ACTIVATE" },
	{ FL_HIDE             , "FL_HIDE" },
	{ FL_SHOW             , "FL_SHOW" },
	{ FL_PASTE            , "FL_PASTE" },
	{ FL_SELECTIONCLEAR   , "FL_SELECTIONCLEAR" },
	{ FL_MOUSEWHEEL       , "FL_MOUSEWHEEL" },
	{ FL_DND_ENTER        , "FL_DND_ENTER" },
	{ FL_DND_DRAG         , "FL_DND_DRAG" },
	{ FL_DND_LEAVE        , "FL_DND_LEAVE" },
	{ FL_DND_RELEASE      , "FL_DND_RELEASE" }
};

const char *GetFlEventName(int event)
{
	int i;
	for (i = 0; i < NELEM(eventNames); i++) {
		if (eventNames[i].number == event) {
			return eventNames[i].name;
		}
	}
	return "Unknown FLTK event";
}

