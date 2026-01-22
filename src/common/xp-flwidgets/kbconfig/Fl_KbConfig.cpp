/* $Id: Fl_KbConfig.cpp,v 1.8 2004/05/23 23:52:07 dick Exp $
 *
 * Fl_KbConfig - Display a picture of a keyboard with all of the keys defined
 *
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
 * $Log: Fl_KbConfig.cpp,v $
 * Revision 1.8  2004/05/23 23:52:07  dick
 * src/common/IniClient.Defaults.cpp
 *
 * Revision 1.7  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.6  2002/09/02 15:35:19  dick
 * Singleton ini becomes iniClient.
 *
 * Revision 1.5  2001/10/04 16:08:15  dick
 * Add an array of neverUsed commands, so they don't get in the Unused key.
 *
 * Revision 1.4  2001/10/01 13:52:39  dick
 * Wire up the buttons Undo/Redo/Save to the UndoMan and handle them.
 *
 * Revision 1.3  2001/09/27 17:43:51  dick
 * Create the Undoable event and act on it based on drag n drop.
 * Start work on "unused" and "copy" buttons.
 * Handle the Save button.
 *
 * Revision 1.2  2001/09/25 15:45:52  dick
 * Split KbConfig to KbConfig and KbConfigKb, since the keyboard is only a part
 * of the whole KbConfig window.
 *
 * Revision 1.1  2001/09/20 07:41:02  dick
 * Move Fl_Kb* to it's own library xp-flwidgets/kbconfig/libxp-kbconfig.a
 *
 * Revision 1.10  2001/09/20 07:21:09  dick
 * If a button has multiple commands, display those in a "flyout" window
 * when the mouse is over the key.
 *
 * Revision 1.9  2001/09/17 23:03:21  dick
 * Comment out debug
 *
 * Revision 1.8  2001/09/17 10:57:42  dick
 * Use ushort, not WORD
 *
 * Revision 1.7  2001/09/17 08:57:35  dick
 * Draw the first command associated with each key.  Move Button to his own file.
 *
 * Revision 1.6  2001/09/07 21:24:03  dick
 * Handle mouse move events and highlight the key the mouse is over.
 *
 * Revision 1.5  2001/09/07 07:41:17  dick
 * Hightlight the key pressed.
 *
 * Revision 1.4  2001/09/06 00:07:39  dick
 * Add XPKbConfig.exe to the Windows binary distribution.
 *
 * Revision 1.3  2001/09/05 13:24:20  dick
 * Linux needs stdlib.h
 *
 * Revision 1.2  2001/09/05 13:16:21  dick
 * Draw a full generic US keyboard
 *
 * Revision 1.1  2001/09/05 09:15:41  dick
 * Display a picture of a keyboard with all of the keys defined.
 *
 */
#include "StdAfx.h"

#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "FL/Fl.H"
#include "FL/fl_draw.H"
#include "FL/Fl_Window.H"

#include "Fl_KbActionButton.h"
#include "Fl_KbCButton.h"
#include "Fl_KbConfigKb.h"
#include "Fl_KbConfig.h"
#include "Fl_KbKbSelector.h"
#include "Fl_KbDragger.h"
#include "KbUndoableCommand.h"

#include "const.h"
#include "IniClient.h"


#include "items/itemRocketPack.xbm"
#include "items/itemCloakingDevice.xbm"
#include "items/itemEnergyPack.xbm"
#include "items/itemWideangleShot.xbm"
#include "items/itemRearShot.xbm"
#include "items/itemMinePack.xbm"
#include "items/itemSensorPack.xbm"
#include "items/itemTank.xbm"
#include "items/itemEcm.xbm"
#include "items/itemArmor.xbm"
#include "items/itemAfterburner.xbm"
#include "items/itemTransporter.xbm"
#include "items/itemDeflector.xbm"
#include "items/itemHyperJump.xbm"
#include "items/itemPhasingDevice.xbm"
#include "items/itemMirror.xbm"
#include "items/itemLaser.xbm"
#include "items/itemEmergencyThrust.xbm"
#include "items/itemTractorBeam.xbm"
#include "items/itemAutopilot.xbm"
#include "items/itemEmergencyShield.xbm"


#define	KBCWIN_WIDTH	600
#define	KBCWIN_HEIGHT	500

//extern Fl_Color	xpblue;

extern PCSTR	usGenericKB;

PCSTR sepa = " \r\n\t\0";

///////////////////////////////////////////////////////////////////////////////
KeyCommand keyCommands[] = {
/* 0*/	"Dummy", 0, false,
		"Lock\nNext", 0, true,
		"Lock\nPrev", 0, true,
		"Lock\nClose", 0, true,
		"Change\nHome", 0, true,
/* 5*/	"Shield", 0, true,
		"Fire\nShot", 0, true,
		"Fire\nMissle", itemRocketPack_bits, true,
		"Fire\nTorpedo", itemRocketPack_bits, true,
		"Weapon\nModify\nNuclear", 0, true,
/*10*/	"Heat\nSeeker", itemRocketPack_bits, true,
		"Place\nMine", itemMinePack_bits, true,
		"Throw\nMine", itemMinePack_bits, true,
		"Turn\nLeft", 0, true,
		"Turn\nRight", 0, true,
/*15*/	"Self\nDestruct", 0, true,
		"Drop\nItem", 0, true,
		"Pause", 0, true,
		"Detach\nTank", itemTank_bits, true,
		"Next\nTank", itemTank_bits, true,
/*20*/	"Prev\nTank", itemTank_bits, true,
		"Weapon\nModify\nVelocity", 0, true,
		"Weapon\nModify\nCluster", 0, true,
		"Swap\nSettings", 0, true,
		"Refuel", 0, true,
/*25*/	"Connect\nBall", 0, true,
		"More\nPower", 0, true,
		"Less\nPower",  0, true,
		"Faster\nTurnrate", 0, true,
		"Slower\nTurnrate", 0, true,
/*30*/	"Thrust", 0, true,
		"Cloak", itemCloakingDevice_bits, true,
		"Activate\nECM", itemEcm_bits, true,
		"Drop\nBall", 0, true,
		"Transporter", itemTransporter_bits, true,
/*35*/	"Talk\nWindow", 0, true,
		"Fire\nLaser", itemLaser_bits, true,
		"Lock\nNext\nClosest", 0, true,
		"Toggle\nCompass", 0, true,
		"Toggle\nMini", 0, true,
/*40*/	"Weapon\nModify\nSpread", 0, true,
		"Toggle\nPower", 0, true,
		"Autopilot", itemAutopilot_bits, true,
		"Weapon\nModify\nLaser", 0, true,
		"Emerg\nThrust", itemEmergencyThrust_bits, true,
/*45*/	"Tractor\nBeam", itemTractorBeam_bits, true,
		"Pressor\nBeam", itemTractorBeam_bits, true,
		"Weapon\nClear\nModifiers", 0, true,
		"Weapon\nModifier\nBank 1", 0, true,
		"Weapon\nModifier\nBank 2", 0, true,
/*50*/	"Weapon\nModifier\nBank 3", 0, true,
		"Weapon\nModifier\nBank 4", 0, true,
		"Select\nItem", 0, true,
		"Phasing\nDevice", itemPhasingDevice_bits, true,
		"Repair", 0, true,
/*55*/	"Weapon\nModify\nImplode", 0, true,
		"Reprogram", 0, true,
		"Player\nLock\nBank 1", 0, true,
		"Player\nLock\nBank 2", 0, true,
		"Player\nLock\nBank 3",  0, true,
/*60*/	"Player\nLock\nBank 4", 0, true,
		"Emerg\nShield", itemEmergencyShield_bits, true,
		"Hyperjump", itemHyperJump_bits, true,
		"Detonate\nMines", itemMinePack_bits, true,
		"Activate\nDeflector", itemDeflector_bits, true,
/*65*/	"Unused 65", 0, false,
		"Unused 66", 0, false,
		"Unused 67", 0, false,
		"Unused 68", 0, false,
		"Unused 69",  0, false,
/*70*/	"Unused 70", 0, false,
		"Unused 71", 0, false,
		"NumKeys", 0, false,
		"Talk\nMacro 1", 0, true,
		"Talk\nMacro 2", 0, true,
/*75*/	"Talk\nMacro 3", 0, true,
		"Talk\nMacro 4", 0, true,
		"Talk\nMacro 5", 0, true,
		"Talk\nMacro 6", 0, true,
		"Talk\nMacro 7", 0, true,
/*80*/	"Talk\nMacro 8", 0, true,
		"Talk\nMacro 9", 0, true,
		"Talk\nMacro 10", 0, true,
		"Talk\nMacro 11", 0, true,
		"Talk\nMacro 12", 0, true,
/*85*/	"Talk\nMacro 13", 0, true,
		"Talk\nMacro 14", 0, true,
		"Talk\nMacro 15", 0, true,
		"Talk\nMacro 16", 0, true,
		"Talk\nMacro 17", 0, true,
/*90*/	"Talk\nMacro 18", 0, true,
		"Talk\nMacro 19", 0, true,
		"Talk\nMacro 20", 0, true,
		"Toggle\nShow\nID", 0, true,
		"Toggle\nShow\nItems", 0, true,
/*95*/	"Toggle\nShow\nMessages", 0, true,
		"Toggle\nPointer\nControl", 0, true,
		"Toggle\nRecord", 0, true,
		"Toggle\nprint\nstdout", 0, true,
		"Talk\nCursor\nLeft", 0, true,
/*100*/	"Talk\nCursor\nRight", 0, true,
		"Talk\nCursor\nUp", 0, true,
		"Talk\nCursor\nDown", 0, true,
		"Swap\nScale\nFactor", 0, true,
		"Show\nStdout", 0, true,
		"Num\nClient\nKeys", 0, false,
};

const int	numKeyCommands = NELEM(keyCommands);
int	neverUsed[] = { KEY_DUMMY, KEY_UNUSED_65, KEY_UNUSED_66, KEY_UNUSED_67,
					KEY_UNUSED_68, KEY_UNUSED_69, KEY_UNUSED_70, KEY_UNUSED_71,
					NUM_KEYS, NUM_CLIENT_KEYS};
const int	numNeverUsed = NELEM(neverUsed);

///////////////////////////////////////////////////////////////////////////////
PCSTR	eventName[] =
{
"FL_NO_EVENT",
"FL_PUSH",
"FL_RELEASE",
"FL_ENTER",
"FL_LEAVE",
"FL_DRAG",
"FL_FOCUS",
"FL_UNFOCUS",
"FL_KEYDOWN",
"FL_KEYUP",
"FL_CLOSE",
"FL_MOVE",
"FL_SHORTCUT",
"FL_DEACTIVATE",
"FL_ACTIVATE",
"FL_HIDE",
"FL_SHOW",
"FL_PASTE",
"FL_SELECTIONCLEAR",
"FL_MOUSEWHEEL"
};

///////////////////////////////////////////////////////////////////////////////
Fl_KbConfig::Fl_KbConfig(int W, int H, const char* label)
: Fl_KbConfigSUPERCLASS(W,H,label)
{
	drag = 0;
	box(FL_FLAT_BOX);
	color(FL_BLACK, FL_BLACK);
	kb = new Fl_KbConfigKb(1, 1, KBCWIN_WIDTH-1, KBCWIN_HEIGHT-1);
	kb->type(Fl_Scroll::HORIZONTAL);
	kb->color(FL_BLACK, FL_BLACK);
	end();
	kb->Init(usGenericKB);
	int _w = Fl::w();
	if (_w > kb->lastlastcol)
		_w = kb->lastlastcol;
	// printf("max=%d\n",_w);
	size(_w, h());
	kb->size(_w, kb->h());
	kb->position(-1, 0);

#define	BL	10		// BOTTOM LEFT MARGIN
#define	BS	5		// BOTTOM SPACE
	unusedCommandsButton = new Fl_KbCButton(BL, kb->h(),
											BUTTON_WIDTH, BUTTON_HEIGHT);
	unusedCommandsButton->keyDef = XStringToXPKeyDef(s_unused);
	unusedCommandsButton->kname = unusedCommandsButton->keyDef->c;
	unusedCommandsButton->SetConfig(kb);
	kb->SetUnusedButton(unusedCommandsButton);

	copyCommandsButton = new Fl_KbCButton(BL+BUTTON_WIDTH+BS, kb->h(),
											BUTTON_WIDTH, BUTTON_HEIGHT);
	copyCommandsButton->keyDef = XStringToXPKeyDef("_Copy");
	copyCommandsButton->kname = copyCommandsButton->keyDef->c;
	kb->SetCopyButton(copyCommandsButton);
	copyCommandsButton->SetConfig(kb);

	currentKeyboard = new Fl_KbKbSelector(BL, kb->h()+BUTTON_HEIGHT+2,
									 200, 30, "US Generic KB");
	currentKeyboard->add("US Generic KB");
	currentKeyboard->add("Dell Inspiron 7500");
	currentKeyboard->add("Hi mom");

	undoButton = new Fl_KbActionButton(BL+currentKeyboard->w()+BS,
									   kb->h()+BUTTON_HEIGHT+2,
									   ACTIONBUTTONWIDTH,
									   ACTIONBUTTONHEIGHT,
									   "Undo");
	undoButton->myKbConfig = this;

	redoButton = new Fl_KbActionButton(BL+currentKeyboard->w()+BS+ACTIONBUTTONWIDTH+BS,
									   kb->h()+BUTTON_HEIGHT+2,
									   ACTIONBUTTONWIDTH,
									   ACTIONBUTTONHEIGHT,
									   "Redo");
	redoButton->myKbConfig = this;

	cancelButton = new Fl_KbActionButton(_w-ACTIONBUTTONWIDTH-BL,
									   kb->h()+BUTTON_HEIGHT+2,
									   ACTIONBUTTONWIDTH,
									   ACTIONBUTTONHEIGHT,
									   "Cancel");
	cancelButton->myKbConfig = this;

	saveButton = new Fl_KbActionButton(_w-BL-ACTIONBUTTONWIDTH-BS-ACTIONBUTTONWIDTH,
									   kb->h()+BUTTON_HEIGHT+2,
									   ACTIONBUTTONWIDTH,
									   ACTIONBUTTONHEIGHT,
									   "Save");
	saveButton->myKbConfig = this;

	undoMan.RegisterButtons(UndoButtonEnabler,
							RedoButtonEnabler,
							SaveButtonEnabler, this);

	kb->UpdateUnusedCommands();
}

///////////////////////////////////////////////////////////////////////////////
Fl_KbConfig* CreateKbConfigWindow(PCSTR windowTitle)
{
	Fl_KbConfig* w = new Fl_KbConfig(KBCWIN_WIDTH, KBCWIN_HEIGHT, windowTitle);
	return(w);
}

///////////////////////////////////////////////////////////////////////////////
void Fl_KbConfig::UndoButtonEnabler(void* myThis, bool enable)
{
	Fl_KbConfig* cfg = (Fl_KbConfig*)myThis;
	if (enable)
		cfg->undoButton->activate();
	else
		cfg->undoButton->deactivate();
}

///////////////////////////////////////////////////////////////////////////////
void Fl_KbConfig::RedoButtonEnabler(void* myThis, bool enable)
{
	Fl_KbConfig* cfg = (Fl_KbConfig*)myThis;
	if (enable)
		cfg->redoButton->activate();
	else
		cfg->redoButton->deactivate();
}

///////////////////////////////////////////////////////////////////////////////
void Fl_KbConfig::SaveButtonEnabler(void* myThis, bool enable)
{
	Fl_KbConfig* cfg = (Fl_KbConfig*)myThis;
	if (enable)
		cfg->saveButton->activate();
	else
		cfg->saveButton->deactivate();
}

///////////////////////////////////////////////////////////////////////////////
int	Fl_KbConfig::handle(int event)
{
	// delay handling mouse move until we see if its ours
	//printf("handle %d\n", event);
	int ret = 0;
	if (event != FL_DRAG)
		ret = Fl_KbConfigSUPERCLASS::handle(event);
	if (ret && event != FL_MOVE)
		return(ret);
	DEBUGEVENTS(printf("Fl_KbConfig::handle(%s)\n", eventName[event]);)
	switch (event)
	{
	case FL_MOVE:
	{
		kb->SelectKeyUnderMouse();
		return(0);
	}
	case FL_PUSH:
	{
		if (kb->lastMouseKey)
		{
			if (!drag)
			{
				Fl_KbCButton* b = kb->lastMouseKey;
				if (!b)
					return(0);
				if (b->multiWindow)
				{
					b = b->multiWindowChild;
				}
				if (!b)
					return(0);
				drag = CreateKbDragger(b);
				if (!drag)
					return(0);
				drag->mouseX = Fl::event_x() - drag->x();
				drag->mouseY = Fl::event_y() - drag->y();
				add(drag);
				drag->redraw();
			}
			return(1);
		}
	}
	case FL_DRAG:
	{
		if (drag)
		{
			drag->position(Fl::event_x() - drag->mouseX,
						   Fl::event_y() - drag->mouseY);
			kb->SelectKeyUnderMouse();

			redraw();
			drag->redraw();
			return(1);
		}
	}
	case FL_RELEASE:
	{
		if (drag)
		{
			if (kb->SelectKeyUnderMouse())
			{
				Fl_KbCButton* b;
				b = kb->lastMouseKey;
				if (b != drag->sourceButton)
				{
					printf("Moving command \"%s\" from key %s to key %s\n",
						drag->command->name, drag->sourceButton->keyDef->s, b->keyDef->s);
//					KbUndoableFrag* uf = new KbUndoableFrag;
					KbUndoableCommand* uc = new KbUndoableCommand("Move command");
					uc->myKbConfig = this;
					uc->command = drag->command->name;
					uc->sourceKey = drag->sourceButton->keyDef->s;
					uc->destKey   = b->keyDef->s;
					undoMan.Do(uc);
				}
			}
			remove(drag);
			delete drag;
			drag = 0;
			redraw();
			return(1);
		}
	}
	}
	return(ret);
}

///////////////////////////////////////////////////////////////////////////////
void Fl_KbConfig::ActionButtonHandler(Fl_KbActionButton* b)
{
	if (b == saveButton)
	{
		iniClient.SaveAll();
		printf("Saved.\n");
	}
	if (b == undoButton)
		undoMan.Undo();
	if (b == redoButton)
		undoMan.Redo();
	if (b == cancelButton)
		hide();
}

