/* $Id: UndoableCommand.cpp,v 1.3 2004/02/03 03:39:52 dick Exp $
 *
 * UndoableCommand - Handle an individual command
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
 * $Log: UndoableCommand.cpp,v $
 * Revision 1.3  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.2  2001/10/01 13:41:57  dick
 * Get rid of frags.  UndoableCommand supports undo and redo (these were
 * separate before).
 *
 * Revision 1.1  2001/09/27 17:48:37  dick
 * New Undo Manager.
 * Currently takes a command and acts on it.  No actual undo yet.
 *
 */
#include "StdAfx.h"

#include "UndoableCommand.h"

///////////////////////////////////////////////////////////////////////////////
UndoableCommand::UndoableCommand()
{
}

UndoableCommand::UndoableCommand(PCSTR _menuTitle)
{
	menuTitle = _menuTitle;
}

///////////////////////////////////////////////////////////////////////////////
UndoableCommand::~UndoableCommand()
{
}
