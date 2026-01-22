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
#include "StdAfx.h"

#include "ShipWindow.h"
#include "ShipDocument.h"
#include "ShipUndoableCommand.h"
#include "controlconst.h"

///////////////////////////////////////////////////////////////////////////////
ShipUndoableCommand::ShipUndoableCommand()
{
}

///////////////////////////////////////////////////////////////////////////////
ShipUndoableCommand::ShipUndoableCommand(PCSTR title)
: ShipUndoableCommandSUPERCLASS(title)
{
}
///////////////////////////////////////////////////////////////////////////////
//Add one ship item to the specified list
ShipUndoableCommand::ShipUndoableCommand(ShipWindow *window, int cType,
										 int cSubType, XpPointList *lst, int mx, int my) :
		myShipWindow(window),
		CommandType(cType), //The type of command
		CommandSubType(cSubType), //Subtype of commands, for different styles of operation
		ptlst(lst)
{
	ShipDocument *shp = myShipWindow->Shplst.pCurrentShip;
	point.x = mx;
	point.y = my;
	nxtPt = NULL;

	spoint = new XpPoint(mx, my);
}
///////////////////////////////////////////////////////////////////////////////
//Work with an existing point, but not changing location EG: deleting
ShipUndoableCommand::ShipUndoableCommand(ShipWindow *window, int cType,
										 int cSubType, XpPointList *lst, XpPoint *pt) :
		myShipWindow(window),
		CommandType(cType), //The type of command
		CommandSubType(cSubType), //Subtype of commands, for different styles of operation
		ptlst(lst),
		spoint(pt)
{
}
///////////////////////////////////////////////////////////////////////////////
//Work with an existing point.
ShipUndoableCommand::ShipUndoableCommand(ShipWindow *window, int cType,
										 int cSubType, XpPointList *lst, XpPoint *pt,
										 int nx, int ny) :
		myShipWindow(window),
		CommandType(cType), //The type of command
		CommandSubType(cSubType), //Subtype of commands, for different styles of operation
		ptlst(lst),
		spoint(pt),
		newx(nx),
		newy(ny)
{
}

///////////////////////////////////////////////////////////////////////////////
ShipUndoableCommand::ShipUndoableCommand(ShipWindow *window, int cType,
									   int cSubType, XpPointList *lst, XpPoint *pt1,
									   XpPoint *pt2, int rev) :
		myShipWindow(window),
		CommandType(cType), //The type of command
		CommandSubType(cSubType), //Subtype of commands, for different styles of operation
		ptlst(lst),
		frstPt(pt1),
		nxtPt(pt2)
{
	ShipDocument *shp = myShipWindow->Shplst.pCurrentShip;

	spoint = new XpPoint(frstPt, nxtPt);
	switch(rev)
	{
		case 1:
			nxtPt = nxtPt;
			break;
		case 2:
			nxtPt = frstPt;
			break;
	}
}
///////////////////////////////////////////////////////////////////////////////
ShipUndoableCommand::ShipUndoableCommand(ShipWindow *window, int cType,
									   int cSubType) :
		myShipWindow(window),
		CommandType(cType), //The type of command
		CommandSubType(cSubType) //Subtype of commands, for different styles of operation
{
}
///////////////////////////////////////////////////////////////////////////////
void ShipUndoableCommand::Do()
{
	XpPoint *tmppt = NULL;
	ShipDocument *shp = myShipWindow->Shplst.pCurrentShip;
	switch (CommandType)
	{
	case DOACTION:
		switch (CommandSubType)	{
		case IDM_INSERT: //Insert a new "item" at shipx, shipy
			spoint = ptlst->CheckAdd(spoint);
			break;
		case IDM_DELETE: //Delete an "item" from xpos,ypos
				//Dont actually delete the point, just remove it from the list
				//Store the ->next pointer though, so we know where to come back to
				nxtPt = (XpPoint *) spoint->GetNext();

 					ptlst->Remove(spoint);
				break;
		case IDM_MOVE: //Move an "item" from
				//Store where we're at now to the "point" storage area
				point.x = spoint->x;
				point.y = spoint->y;

				//Move to the new location coords
				spoint->x = newx;
				spoint->y = newy;
				break;
		case IDM_SPLIT: //Split two points
				spoint = ptlst->CheckInsert(nxtPt, spoint);
				break;
		}
		break;
	case SHIFT: //Shift the ship
		switch (CommandSubType)
		{
		case SHIFTLEFT:
			shp->PumpShip(&ShipDocument::ShiftLeft);
			break;
		case SHIFTRIGHT:
			shp->PumpShip(&ShipDocument::ShiftRight);
			break;
		case SHIFTUP:
			shp->PumpShip(&ShipDocument::ShiftUp);
			break;
		case SHIFTDOWN:
			shp->PumpShip(&ShipDocument::ShiftDown);
			break;
		}
		break;
	case MIRROR: //Mirror the ship
		switch (CommandSubType)
		{
		case MIRRORH:
			shp->PumpShip(&ShipDocument::MirrorHoriz);
			break;
		case MIRRORV:
			shp->PumpShip(&ShipDocument::MirrorVert);
			break;
		}
		break;
	case ROTATE: //Rotate the ship
		switch (CommandSubType)
		{
		case CLOCKWISE:
			shp->PumpShip(&ShipDocument::RotateRight);
			break;
		case COUNTERCLOCKWISE:
			shp->PumpShip(&ShipDocument::RotateLeft);
			break;
		}
		break;
	}
	myShipWindow->Shplst.changed = true;
}
///////////////////////////////////////////////////////////////////////////////
void ShipUndoableCommand::Undo()
{
	XpPoint *tmppt = NULL;
	ShipDocument *shp = myShipWindow->Shplst.pCurrentShip;
	switch (CommandType) {
	case DOACTION:
		switch (CommandSubType)	{
		case IDM_INSERT: //We inserted an Item so no we should remove it.
			ptlst->Remove(spoint);
			break;
		case IDM_DELETE: //We deleted an Item so no we should re-insert it.
			//If we're not the last point, insert where we were before.
			//otherwise add to the end.
			if (nxtPt)
				spoint = ptlst->CheckInsert(nxtPt, spoint);
			else
				spoint = ptlst->CheckAdd(spoint);

			break;
		case IDM_MOVE: //Move an "item" from
			//Move back to the stored location
			spoint->x = point.x;
			spoint->y = point.y;
			break;
		case IDM_SPLIT:
			//Store where we go in the list
			//Then remove the specified point
			nxtPt = (XpPoint *) spoint->GetNext();
			ptlst->Remove(spoint);
			break;
		}
		break;
	case SHIFT: //Un-Shift the ship

		switch (CommandSubType)
		{
		case SHIFTLEFT:
			shp->PumpShip(&ShipDocument::ShiftRight);
			break;
		case SHIFTRIGHT:
			shp->PumpShip(&ShipDocument::ShiftLeft);
			break;
		case SHIFTUP:
			shp->PumpShip(&ShipDocument::ShiftDown);
			break;
		case SHIFTDOWN:
			shp->PumpShip(&ShipDocument::ShiftUp);
			break;
		}
		break;
	case MIRROR: //Un-Mirror the ship
		switch (CommandSubType)
		{
		case MIRRORH:
			shp->PumpShip(&ShipDocument::MirrorHoriz);
			break;
		case MIRRORV:
			shp->PumpShip(&ShipDocument::MirrorVert);
			break;
		}
		break;
	case ROTATE: //Un-Rotate the ship
		switch (CommandSubType)
		{
		case CLOCKWISE:
			shp->PumpShip(&ShipDocument::RotateLeft);
			break;
		case COUNTERCLOCKWISE:
			shp->PumpShip(&ShipDocument::RotateRight);
			break;
		}
		break;
	}
}

