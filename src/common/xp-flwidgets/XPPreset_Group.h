/* $Id: XPPreset_Group.h,v 1.3 2004/05/28 17:17:02 dick Exp $
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
 * $Log: XPPreset_Group.h,v $
 * Revision 1.3  2004/05/28 17:17:02  dick
 * xpcheckbutton_cb() becomes static XPCheckButton::CallbackFl()
 *
 * Revision 1.2  2003/07/18 23:55:48  jlmiller
 * Able to store changes to settings now.
 *
 * Revision 1.1  2002/09/10 19:09:08  jlmiller
 * A group of widgets for a single preset.
 *
 */

#ifndef	_XPPRESET_GROUP_H_
#define	_XPPRESET_GROUP_H_

#include "FL/Fl_Group.H"
#include "Fl_Value_Input_Counter.h"
#include "XPCheckButton.h"

#include "IniClient.h"
#include "cstring.h"

#define	XPPreset_GroupSUPERCLASS Fl_Group

class XPPreset_Group : public XPPreset_GroupSUPERCLASS
{
public:
	XPPreset_Group(int x, int y, int w, int h, PCSTR label = 0);
	void SetStrng(char *strng);
	const char *GetStrng();
	void UpdateSettings();
	void ParseSettingsFromStr();
	void WriteSettingsToStr();

private:

//	char *macroStr;
	String macroStr;

	int settingMini;
	int settingVelocity;
	int settingSpread;
	int settingPower;
	bool toggleCluster;
	bool toggleImplosion;
	Fl_Value_Input_Counter *presetMini;
	Fl_Value_Input_Counter *presetVelocity;
	Fl_Value_Input_Counter *presetSpread;
	Fl_Value_Input_Counter *presetPower;
	XPCheckButton  *presetCluster;
	XPCheckButton  *presetImplosion;

};


#endif	// _XPPRESET_GROUP_H_
