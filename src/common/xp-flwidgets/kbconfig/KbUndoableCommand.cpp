/* $Id: KbUndoableCommand.cpp,v 1.4 2004/02/03 03:39:53 dick Exp $
 *
 * KbUndoableFrag - Handle a command for the kb configurator
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
 * $Log: KbUndoableCommand.cpp,v $
 * Revision 1.4  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.3  2002/09/02 15:35:19  dick
 * Singleton ini becomes iniClient.
 *
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
#include "StdAfx.h"

#include "IniClient.h"

#include "KbUndoableCommand.h"
#include "Fl_KbCButton.h"
#include "Fl_KbConfigKb.h"
#include "Fl_KbConfig.h"

///////////////////////////////////////////////////////////////////////////////
KbUndoableCommand::KbUndoableCommand()
{
}

KbUndoableCommand::KbUndoableCommand(PCSTR title)
: KbUndoableCommandSUPERCLASS(title)
{
}

///////////////////////////////////////////////////////////////////////////////
void KbUndoableCommand::Do()
{

	printf("KbUndoableCommand: moving \"%s\" from \"%s\" to \"%s\"\n",
		(PCSTR)command, (PCSTR)sourceKey, (PCSTR)destKey);

	int			i;
	keys_t		t = (keys_t)-1;
	Fl_KbCButton*	bs;
	Fl_KbCButton*	bt;
	KeySym		ks;				// source button KeySym
	KeySym		kt;				// target button KeySym

	if (!SetupPointers(bs, bt, ks, kt, t))
		return;
	bs->UnSelect();
	bt->UnSelect();
	if (bs->keyDef->s == s_unused)			// moving a command from unused?
	{
		AddCommand(bt, t);
		myKbConfig->kb->UpdateAllCommands(); // possibly inserted a command, screwing the pointers
	}
	else if (bt->keyDef->s == s_unused)
	{
		DeleteCommand(bs, t);
		myKbConfig->kb->UpdateAllCommands(); // possibly inserted a command, screwing the pointers
	}
	else if (bt->keyDef->s == s_copy)
	{
		AddCommand(myKbConfig->kb->GetCopyButton(), t);
		myKbConfig->kb->UpdateAllCommands(); // possibly inserted a command, screwing the pointers
	}
	else
	{
		// Change the value of the source button to be the dest button
		for (i=0; i<iniClient.maxKeyDefs; i++)
		{
			if (iniClient.keyDefs[i].keySym == ks && iniClient.keyDefs[i].key == t)
			{
				iniClient.keyDefs[i].keySym = kt;
			}
		}
		myKbConfig->kb->GetCommands(bs->GetCommands(), bs);
		myKbConfig->kb->GetCommands(bt->GetCommands(), bt);
	}

	myKbConfig->kb->lastMouseKey = 0;
	myKbConfig->kb->SelectKeyUnderMouse();
}

///////////////////////////////////////////////////////////////////////////////
void KbUndoableCommand::Undo()
{

	printf("KbUndoableCommand: moving \"%s\" from \"%s\" to \"%s\"\n",
		(PCSTR)command, (PCSTR)sourceKey, (PCSTR)destKey);

	int			i;
	keys_t		t = (keys_t)-1;
	Fl_KbCButton*	bs;
	Fl_KbCButton*	bt;
	KeySym		ks;				// source button KeySym
	KeySym		kt;				// target button KeySym

	if (!SetupPointers(bs, bt, ks, kt, t))
		return;

	if (bs->keyDef->s == s_unused)			// moving a command from unused?
	{
		DeleteCommand(bt, t);
		myKbConfig->kb->UpdateAllCommands(); // possibly inserted a command, screwing the pointers
	}
	else if (bt->keyDef->s == s_unused)
	{
		AddCommand(bs, t);
		myKbConfig->kb->UpdateAllCommands(); // possibly inserted a command, screwing the pointers
	}
	else if (bt->keyDef->s == s_copy)
	{
		DeleteCommand(myKbConfig->kb->GetCopyButton(), t);
		myKbConfig->kb->UpdateAllCommands(); // possibly inserted a command, screwing the pointers
	}
	else
	{
		// Change the value of the source button to be the dest button
		for (i=0; i<iniClient.maxKeyDefs; i++)
		{
			if (iniClient.keyDefs[i].keySym == kt && iniClient.keyDefs[i].key == t)
			{
				iniClient.keyDefs[i].keySym = ks;
			}
		}
		myKbConfig->kb->GetCommands(bs->GetCommands(), bs);
		myKbConfig->kb->GetCommands(bt->GetCommands(), bt);
	}

	bs->UnSelect();
	bt->UnSelect();
	myKbConfig->kb->lastMouseKey = 0;
	myKbConfig->kb->SelectKeyUnderMouse();
}

///////////////////////////////////////////////////////////////////////////////
bool KbUndoableCommand::SetupPointers(Fl_KbCButton*& bs, Fl_KbCButton*& bt,
								   KeySym& ks, KeySym& kt,
								   keys_t& t)
{
	XPKeyDef*	xk;				// command
	int			i;

	for (i=0; i<NUM_CLIENT_KEYS; i++)
	{
		if (!strcmp(keyCommands[i].name, command))
		{
			t = (keys_t)i;
			break;
		}
	}
	if (t == -1)
	{
		printf("KbUndoableCommand: whoa.  Couldn't match the command\n");
		return(false);
	}
	xk = XStringToXPKeyDef(sourceKey);
	if (!xk)
	{
		printf("KbUndoableCommand: whoa.  Couldn't match the sourceKey\n");
		return(false);
	}
	bs = myKbConfig->kb->FindButton(xk);
	ks = xk->k;

	xk = XStringToXPKeyDef(destKey);
	if (!xk)
	{
		printf("KbUndoableCommand: whoa.  Couldn't match the destKey\n");
		return(false);
	}
	bt = myKbConfig->kb->FindButton(xk);
	kt = xk->k;
	return(true);
}

///////////////////////////////////////////////////////////////////////////////
void KbUndoableCommand::AddCommand(Fl_KbCButton* bt, keys_t t)
{
	iniClient.InsertKeyDef(bt->keyDef->k, t);
}

///////////////////////////////////////////////////////////////////////////////
void KbUndoableCommand::DeleteCommand(Fl_KbCButton* bt, keys_t t)
{
	iniClient.DeleteKeyDef(bt->keyDef->k, t);
}
