/* $Id: UndoMan.cpp,v 1.5 2004/02/03 03:39:52 dick Exp $
 *
 * UndoMan - Handle command chains
 *      Copyright (C) 2001 by
 *
 *      The XPilot Authors   <xpilot@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
 *      Jarrod Miller        <jarrod@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
 *      Bj�rn Stabell        <bjoern@xpilot.org>
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
 * $Log: UndoMan.cpp,v $
 * Revision 1.5  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.4  2002/07/22 15:07:37  jlmiller
 * Lets allow a quick DoAgain, instead of needing to undo then redo.
 *
 * Revision 1.3  2002/06/17 20:57:51  jlmiller
 * Provide Clear()
 *
 * Revision 1.2  2001/10/01 13:40:54  dick
 * Undo/Redo is functional.  Add ButtonEnablers to manage the state of Undo/Redo/Save
 *
 * Revision 1.1  2001/09/27 17:48:36  dick
 * New Undo Manager.
 * Currently takes a command and acts on it.  No actual undo yet.
 *
 */
#include "StdAfx.h"

#include "UndoMan.h"


///////////////////////////////////////////////////////////////////////////////
UndoMan::UndoMan()
{
	current = 0;
	permdirty = false;
}

///////////////////////////////////////////////////////////////////////////////
UndoMan::~UndoMan()
{
}

///////////////////////////////////////////////////////////////////////////////
void UndoMan::RegisterButtons(UndoButtonEnabler _undoButton,
							  UndoButtonEnabler _redoButton,
							  UndoButtonEnabler _saveButton,
							  void* _hisThis)
{
	undoButton = _undoButton;
	redoButton = _redoButton;
	saveButton = _saveButton;
	hisThis = _hisThis;
	UpdateButtons();
}

///////////////////////////////////////////////////////////////////////////////
void UndoMan::UpdateButtons()
{
	UpdateUndoButton();
	UpdateRedoButton();
	UpdateSaveButton();
}

///////////////////////////////////////////////////////////////////////////////
void UndoMan::UpdateUndoButton()
{
	bool enable = false;
	if (current)
		enable = true;
	if (undoButton)
		undoButton(hisThis, enable);
}

///////////////////////////////////////////////////////////////////////////////
void UndoMan::UpdateRedoButton()
{
	bool enable = false;
	if (current != queue.GetTail())
		enable = true;
	if (redoButton)
		redoButton(hisThis, enable);
}

///////////////////////////////////////////////////////////////////////////////
void UndoMan::UpdateSaveButton()
{
	if (saveButton)
		saveButton(hisThis, IsDirty());
}

///////////////////////////////////////////////////////////////////////////////
int UndoMan::Clear()
{
	queue.Empty();
	current = NULL;
	UpdateButtons();
	return(1);
}

///////////////////////////////////////////////////////////////////////////////
int UndoMan::Do(UndoableCommand* command)
{
	command->Do();
	UpdateQueue(command);
	UpdateButtons();
	return(1);
}
///////////////////////////////////////////////////////////////////////////////
void UndoMan::DoAgain()
{
	UndoableCommand *command = (UndoableCommand*)current;
	command->Do();
}

///////////////////////////////////////////////////////////////////////////////
void UndoMan::Undo()
{
	if (current)
	{
		((UndoableCommand*)current)->Undo();
		current = current->prev;
	}
	UpdateButtons();
}

///////////////////////////////////////////////////////////////////////////////
void UndoMan::Redo()
{
	if (!current)
	{
		if (queue.GetCount())
		{
			current = queue.GetHead();
			((UndoableCommand*)current)->Do();
		}
	}
	else
	{
		if (current->next)
		{
			current = current->next;
			((UndoableCommand*)current)->Do();
		}
	}
	UpdateButtons();
}

///////////////////////////////////////////////////////////////////////////////
bool UndoMan::IsDirty()
{
	if (permdirty)
		return(true);
	if (!current)
		return(false);
	return(true);
}

///////////////////////////////////////////////////////////////////////////////
void UndoMan::UpdateQueue(UndoableCommand* command)
{
	if (!queue.GetCount() || (current && !current->next))
	{
		current = command;
		queue.Add(command);
	}
	else
	{
		if (current)
		{
			if (current != queue.GetTail())
			{
				while (current != queue.GetTail())
				{
					UndoableCommand* c = (UndoableCommand*)queue.RemoveTail();
					delete c;
				}
			}
			current = command;
			queue.Add(command);
		}
		else
		{
			queue.Empty();
			current = command;
			queue.Add(command);
		}
	}
}
