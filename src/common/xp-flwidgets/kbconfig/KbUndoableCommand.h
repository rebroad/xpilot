/* $Id: KbUndoableCommand.h,v 1.2 2001/10/04 16:09:17 dick Exp $
 *
 * KbUndoableFrag - Handle a command for the kb configurator
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
 * $Log: KbUndoableCommand.h,v $
 * Revision 1.2  2001/10/04 16:09:17  dick
 * Support the Unused and Copy buttons.
 *
 * Revision 1.1  2001/10/01 13:47:02  dick
 * Get rid of frags.  UndoableCommand supports undo and redo (these were
 * separate before).
 *
 * Revision 1.1  2001/09/27 17:50:50  dick
 * Undoable Command handler kbconfig
 *
 */


#ifndef	_KBUNDOABLECOMMAND_H_
#define	_KBUNDOABLECOMMAND_H_

#include "UndoableCommand.h"
#include "keys.h"

class Fl_KbConfig;
class Fl_KbCButton;

#define	KbUndoableCommandSUPERCLASS	UndoableCommand
class KbUndoableCommand : public KbUndoableCommandSUPERCLASS
{
public:
	KbUndoableCommand();
	KbUndoableCommand(PCSTR _menuTitle);
	virtual void	Do();
	virtual void	Undo();

	Fl_KbConfig*	myKbConfig;

	String			sourceKey;
	String			destKey;
	String			command;

private:
	bool			SetupPointers(Fl_KbCButton*& bs, Fl_KbCButton*& bt,
								  KeySym& ks, KeySym& kt,
								  keys_t& t);

	void			AddCommand(Fl_KbCButton* bt, keys_t t);
	void			DeleteCommand(Fl_KbCButton* bt, keys_t t);
};

#endif	// _KBUNDOABLECOMMAND_H_
