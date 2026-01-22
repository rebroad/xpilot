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
#include <assert.h>

#include "SoundWindow.h"
#include "IniClient.h"
#include "AudioMan.h"
#include "Fl_SoundEventList.h"
#include "Fl_SoundList.h"
#include "Fl_MainGainSlider.h"
#include "Fl_EventSoundConnector.h"

#include "SoundUndoableCommand.h"

///////////////////////////////////////////////////////////////////////////////
SoundUndoableCommand::SoundUndoableCommand()
{
}

///////////////////////////////////////////////////////////////////////////////
SoundUndoableCommand::SoundUndoableCommand(PCSTR title)
: SoundUndoableCommandSUPERCLASS(title)
{
}
#if 0
///////////////////////////////////////////////////////////////////////////////
//Add one Sound item to the specified list
SoundUndoableCommand::SoundUndoableCommand(SoundWindow *window, int cType,
										 int cSubType, XpPointList *lst, int mx, int my) :
		mySoundWindow(window),
		CommandType(cType), //The type of command
		CommandSubType(cSubType), //Subtype of commands, for different styles of operation
		ptlst(lst)
{
	SoundDocument *shp = mySoundWindow->Shplst.pCurrentSound;
	point.x = mx;
	point.y = my;
	nxtPt = NULL;

	spoint = new XpPoint(mx, my);
}
///////////////////////////////////////////////////////////////////////////////
//Work with an existing point, but not changing location EG: deleting
SoundUndoableCommand::SoundUndoableCommand(SoundWindow *window, int cType,
										 int cSubType, XpPointList *lst, XpPoint *pt) :
		mySoundWindow(window),
		CommandType(cType), //The type of command
		CommandSubType(cSubType), //Subtype of commands, for different styles of operation
		ptlst(lst),
		spoint(pt)
{
}
///////////////////////////////////////////////////////////////////////////////
//Work with an existing point.
SoundUndoableCommand::SoundUndoableCommand(SoundWindow *window, int cType,
										 int cSubType, XpPointList *lst, XpPoint *pt,
										 int nx, int ny) :
		mySoundWindow(window),
		CommandType(cType), //The type of command
		CommandSubType(cSubType), //Subtype of commands, for different styles of operation
		ptlst(lst),
		spoint(pt),
		newx(nx),
		newy(ny)
{
}

///////////////////////////////////////////////////////////////////////////////
SoundUndoableCommand::SoundUndoableCommand(SoundWindow *window, int cType,
									   int cSubType, XpPointList *lst, XpPoint *pt1,
									   XpPoint *pt2, int rev) :
		mySoundWindow(window),
		CommandType(cType), //The type of command
		CommandSubType(cSubType), //Subtype of commands, for different styles of operation
		ptlst(lst),
		frstPt(pt1),
		nxtPt(pt2)
{
	SoundDocument *shp = mySoundWindow->Shplst.pCurrentSound;

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
#endif
///////////////////////////////////////////////////////////////////////////////
SoundUndoableCommand::SoundUndoableCommand(SoundWindow *window, int cType,
									   int cSubType) :
		mySoundWindow(window),
		CommandType(cType), //The type of command
		CommandSubType(cSubType) //Subtype of commands, for different styles of operation
{
}
///////////////////////////////////////////////////////////////////////////////
void SoundUndoableCommand::Do()
{
	switch (CommandType) {
	case CHANGE_MAIN_GAIN:
		mySoundWindow->mainGain->value(newGain);
		iniClient.maxVolume = newGain;
		audioMan.SetGain((double)(newGain/100.0));
		break;
	case CHANGE_BINDING:
		DoChangeBinding();
		break;
	}

}
///////////////////////////////////////////////////////////////////////////////
void SoundUndoableCommand::Undo()
{
	switch (CommandType) {
	case CHANGE_MAIN_GAIN:
		mySoundWindow->mainGain->value(oldGain);
		iniClient.maxVolume = oldGain;
		audioMan.SetGain((double)(oldGain/100.0));
		break;
	case CHANGE_BINDING:
		UndoChangeBinding();
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////
void SoundUndoableCommand::DoChangeBinding() {
	SoundEvent* se = audioMan.GetSoundEvent(eventRow);
	Sound* s = (Sound*)audioMan.GetSoundList()->GetAt(this->newSoundRow);
	se->AddEvent(s, 0);
	mySoundWindow->flSoundEventList->row(eventRow);
	mySoundWindow->DisplaySelectedEvent();

}
///////////////////////////////////////////////////////////////////////////////
void SoundUndoableCommand::UndoChangeBinding() {
	SoundEvent* se = audioMan.GetSoundEvent(eventRow);
	Sound* s = (Sound*)audioMan.GetSoundList()->GetAt(this->oldSoundRow);
	se->AddEvent(s, 0);
	mySoundWindow->flSoundEventList->row(eventRow);
	mySoundWindow->DisplaySelectedEvent();

}
