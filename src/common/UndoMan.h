/* $Id: UndoMan.h,v 1.4 2002/07/22 15:07:37 jlmiller Exp $
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
 * $Log: UndoMan.h,v $
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

/*
This module handles commands.  Rather than directly act on your data,
you create a UndoManCommand and feed it to UndoMan.
The UndoManCommand has two parts:  a command to Do (or Redo) an action
and an optional part to Undo an action.  If the Undo part is NULL,
then the command is not undoable, and the undo queue is flushed.

The UndoMan has 3 inputs:
1) Process an UndoManCommand
2) Process a signal from an Undo Button
3) Process a signal from a Redo Button.

The outputs are:
1) Notify a button of a change in state of availabilty of Undo/Redo.
2) (indirect) Process a command on the list.
*/

#ifndef	_UNDOMAN_H_
#define	_UNDOMAN_H_

#include "Obj.h"
#include "UndoableCommand.h"


typedef void	(*UndoButtonEnabler)(void* myThis, bool enable);

class UndoMan
{
public:
	UndoMan();
	~UndoMan();
	void	RegisterButtons(UndoButtonEnabler _undoButton,
							UndoButtonEnabler _redoButton,
							UndoButtonEnabler _saveButton,
							void* _hisThis);

	int		Do(UndoableCommand* command);
	bool	IsDirty();

	void	Undo();		// Call this when user clicks the Undo button
	void	Redo();		// Call this when user clicks the Redo button
	void	Save();		// Call this when user clicks the Save button
	int		Clear();

	void DoAgain(); //Redo the last Do. Much faster than an Undo-Redo combo.

private:
	void	UpdateQueue(UndoableCommand* command);
	void	UpdateButtons();
	void	UpdateUndoButton();
	void	UpdateRedoButton();
	void	UpdateSaveButton();

	UndoButtonEnabler	undoButton;
	UndoButtonEnabler	redoButton;
	UndoButtonEnabler	saveButton;
	void*				hisThis;

	ObjList	queue;
	Obj*	current;
	bool	permdirty;


};

#endif
