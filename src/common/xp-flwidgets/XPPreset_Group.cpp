/* $Id: XPPreset_Group.cpp,v 1.6 2004/05/28 17:17:02 dick Exp $
 *
 * XPPreset_Group - Handle the handle that configures the user's modifier presets (XVBCZ)
 *
 * common/xp-flwidgets - A library of FLTK widgets for XPilot5.
 *
 *      Copyright (C) 2002 by
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
 * $Log: XPPreset_Group.cpp,v $
 * Revision 1.6  2004/05/28 17:17:02  dick
 * xpcheckbutton_cb() becomes static XPCheckButton::CallbackFl()
 *
 * Revision 1.5  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.4  2004/01/06 04:48:28  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 * Revision 1.3  2003/07/18 23:55:48  jlmiller
 * Able to store changes to settings now.
 *
 * Revision 1.2  2002/09/10 21:44:35  jlmiller
 *
 * need ctype.h on linux.
 *
 * Revision 1.1  2002/09/10 19:09:08  jlmiller
 * A group of widgets for a single preset.
 *
 */
#include "StdAfx.h"

#include <ctype.h>
#include "XPPreset_Group.h"
#include "cstring.h"

//XXX This is evil: You can't reference server (application) from common (library).
#include "../server/Object.h"
#include "guiconst.h"

///////////////////////////////////////////////////////////////////////////////
static int str2num (char **strp, int min, int max)
{
    char *str = *strp;
    int num = 0;

    while (isdigit(*str)) {
	num *= 10;
	num += *str++ - '0';
    }
    *strp = str;
    if (num < min || num > max)
	return min;
    return num;
}
///////////////////////////////////////////////////////////////////////////////
XPPreset_Group::XPPreset_Group(int x, int y, int w, int h, PCSTR label)
: XPPreset_GroupSUPERCLASS(x, y, w, h, label)
{
	settingMini = 1;
	settingVelocity = 0;
	settingSpread = 0;
	settingPower = 0;
	toggleCluster = false;
	toggleImplosion = false;

	presetMini = new Fl_Value_Input_Counter(x, y+CC_INPUTHEIGHT, CC_INPUTWIDTHSMALL, CC_INPUTHEIGHT, "Mini:");
	presetMini->end();
	presetMini->align(FL_ALIGN_LEFT);
	presetMini->bounds(1, 4);
	presetMini->SetInt(&settingMini);

	presetVelocity = new Fl_Value_Input_Counter(x, y+CC_INPUTHEIGHT*2, CC_INPUTWIDTHSMALL, CC_INPUTHEIGHT, "Velocity:");
	presetVelocity->end();
	presetVelocity->align(FL_ALIGN_LEFT);
	presetVelocity->bounds(0, 3);
	presetVelocity->SetInt(&settingVelocity);

	presetSpread = new Fl_Value_Input_Counter(x, y+CC_INPUTHEIGHT*3, CC_INPUTWIDTHSMALL, CC_INPUTHEIGHT, "Spread:");
	presetSpread->end();
	presetSpread->align(FL_ALIGN_LEFT);
	presetSpread->bounds(0, 3);
	presetSpread->SetInt(&settingSpread);

	presetPower = new Fl_Value_Input_Counter(x, y+CC_INPUTHEIGHT*4, CC_INPUTWIDTHSMALL, CC_INPUTHEIGHT, "Power:");
	presetPower->end();
	presetPower->align(FL_ALIGN_LEFT);
	presetPower->bounds(0, 2);
	presetPower->SetInt(&settingPower);

	presetCluster = new XPCheckButton(x, y+CC_INPUTHEIGHT*5, CC_INPUTWIDTHSMALL, CC_INPUTHEIGHT, "Cluster?:");
	presetCluster->align(FL_ALIGN_LEFT);
	presetCluster->SetBool(&toggleCluster);
	presetCluster->callback((Fl_Callback *)XPCheckButton::CallbackFl);

	presetImplosion = new XPCheckButton(x, y+CC_INPUTHEIGHT*6, CC_INPUTWIDTHSMALL, CC_INPUTHEIGHT, "Implosion?:");
	presetImplosion->align(FL_ALIGN_LEFT);
	presetImplosion->SetBool(&toggleImplosion);
	presetImplosion->callback((Fl_Callback *)XPCheckButton::CallbackFl);
}
///////////////////////////////////////////////////////////////////////////////
PCSTR XPPreset_Group::GetStrng()
{
	WriteSettingsToStr();
	return ((PCSTR) macroStr);
//	macroStr = strng;
//	ParseSettingsFromStr();
}

///////////////////////////////////////////////////////////////////////////////
void XPPreset_Group::SetStrng(char *strng)
{
//	macroStr = strng;
	macroStr.printf(strng);
	ParseSettingsFromStr();
}
///////////////////////////////////////////////////////////////////////////////
void XPPreset_Group::UpdateSettings()
{
	presetMini->Read();
	presetMini->Read();
	presetSpread->Read();
	presetPower->Read();
	presetCluster->Read();
	presetImplosion->Read();
}
///////////////////////////////////////////////////////////////////////////////
void XPPreset_Group::WriteSettingsToStr()
{
	String tmp;
	String bld;

	if (toggleCluster == true)
		tmp += "C ";

	if (toggleImplosion == true)
		tmp += "I ";

    if (settingVelocity > 0)
	{
		bld.printf("V%d ", settingVelocity);
		tmp += bld;
	}

	if (settingMini > 1)
	{
		bld.printf("X%d ", settingMini);
		tmp += bld;
	}

	if (settingSpread > 0)
	{
		bld.printf("Z%d ", settingSpread);
		tmp += bld;
	}

	if (settingPower > 0)
	{
		bld.printf("B%d ", settingPower);
		tmp += bld;
	}
	macroStr.printf((PCSTR)tmp);

}
///////////////////////////////////////////////////////////////////////////////
void XPPreset_Group::ParseSettingsFromStr()
{
    char*	cp;
	for (cp = macroStr.s; *cp; cp++) {
		switch (*cp) {
/*		case 'F': case 'f':
			if (*(cp+1) == 'N' || *(cp+1) == 'n')
				SET_BIT(mods.nuclear, FULLNUCLEAR);
			break;
		case 'N': case 'n':
			if (!BIT(world->rules->mode, ALLOW_NUKES))
				break;
			SET_BIT(mods.nuclear, NUCLEAR);
			break;*/
		case 'C': case 'c':
			toggleCluster = true;
			break;
		case 'I': case 'i':
			toggleImplosion = true;
			break;
		case 'V': case 'v':
			cp++; settingVelocity = str2num (&cp, 0, MODS_VELOCITY_MAX);
			cp--;
			break;
		case 'X': case 'x':
			cp++; settingMini = str2num (&cp, 1, MODS_MINI_MAX+1);
			cp--;
			break;
		case 'Z': case 'z':
			cp++; settingSpread = str2num (&cp, 0, MODS_SPREAD_MAX);
			cp--;
			break;
		case 'B': case 'b':
			cp++; settingPower = str2num (&cp, 0, MODS_POWER_MAX);
			cp--;
			break;
/*		case 'L': case 'l':
			cp++;
			if (!BIT(world->rules->mode, ALLOW_LASER_MODIFIERS))
				break;
			if (*cp == 'S' || *cp == 's')
				SET_BIT(mods.laser, STUN);
			if (*cp == 'B' || *cp == 'b')
				SET_BIT(mods.laser, BLIND);
			break;*/
		}
	}
	UpdateSettings();
}
