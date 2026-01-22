/*
* XPilotedit, the cross platform map editor for XPilot.  Copyright (C) 2001 by
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
 * $Log: MapUndoableCommand.cpp,v $
 * Revision 1.11  2005/03/17 22:12:14  kps
 * Get rid of warnings from makedepend about "non-portable whitespace".
 *
 * Revision 1.10  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.9  2002/09/10 01:22:45  jlmiller
 * Added 'Closing Confirm' dialog boxes to mapeditor and shipeditor.
 *
 * Revision 1.8  2002/09/10 01:20:18  dick
 * Include stdlib.h
 *
 * Revision 1.7  2002/06/25 20:20:09  jlmiller
 * Massive restructure to clean up includes, and objectify code more.
 *
 * Revision 1.6  2002/06/07 23:10:08  jlmiller
 * Encapsulate almost all map functions into undoMan.
 *
 * Revision 1.5  2002/06/07 20:49:00  jlmiller
 * Begin handling undoMan properly...most functions still broken, expecially RotateMap.
 *
 * Revision 1.4  2002/05/30 14:19:25  jlmiller
 * Remove activeMapWindow global, which is unnecessay
 * MapWindow & ShipWindow store documents similarly now.
 *
 * Revision 1.3  2002/05/28 07:19:50  dick
 * Rename xpilotedit to XPilotControl
 *
 */
#include "StdAfx.h"

#ifndef	_WINDOWS
#	include <stdlib.h>
#endif

#include "MapWindow.h"
#include "MapUndoableCommand.h"
#include "MapBlockChange.h"
#include "controlconst.h"

///////////////////////////////////////////////////////////////////////////////
MapUndoableCommand::MapUndoableCommand()
{
}

///////////////////////////////////////////////////////////////////////////////
MapUndoableCommand::MapUndoableCommand(PCSTR title)
: MapUndoableCommandSUPERCLASS(title)
{
}
///////////////////////////////////////////////////////////////////////////////
MapUndoableCommand::MapUndoableCommand(MapWindow *window, int cType, bool swap,
									   ObjList *chngLst) :
		myMapWindow(window),
		CommandType(cType), //The type of command
		swapSize(swap) //Should we swap map sizes?
{
			changeList2 = chngLst;
}
///////////////////////////////////////////////////////////////////////////////
void MapUndoableCommand::Do()
{
	switch (CommandType) {
	case CHANGEAREA:
		{
			MapBlockChange *chg = (MapBlockChange *) changeList2->GetHead();
			while (chg != NULL)
			{
				myMapWindow->mapDoc.SelectGrid(chg->x, chg->y, chg->newchar, true);
				chg = (MapBlockChange *) chg->GetNext();
			}
			if (swapSize == true)
				myMapWindow->mapDoc.SwapMapSize();

		}
		myMapWindow->mapDoc.changed = true;
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////
void MapUndoableCommand::Undo()
{
	switch (CommandType) {
	case CHANGEAREA:
		{
			if (swapSize == true)
				myMapWindow->mapDoc.SwapMapSize();
			MapBlockChange *chg = (MapBlockChange *) changeList2->GetTail();
			while (chg != NULL)
			{
				myMapWindow->mapDoc.SelectGrid(chg->x, chg->y, chg->oldchar, true);
				chg = (MapBlockChange *) chg->prev;
			}
		}
		break;
	}
}




