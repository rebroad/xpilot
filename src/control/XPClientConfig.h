/* $Id: XPClientConfig.h,v 1.13 2004/05/28 18:20:29 dick Exp $
 *
 * XPClientConfig - Configure options for the XPilot client
 *
 * XPilot5 - Manage all local XPilot activity.
 *
 *      Copyright (C) 2001 by
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
/*
 * $Log: XPClientConfig.h,v $
 * Revision 1.13  2004/05/28 18:20:29  dick
 * Automagically save changes as they occur, rather than have the user press a save button.
 *
 */

#ifndef	_XPCLIENTCONFIG_H_
#define	_XPCLIENTCONFIG_H_

#include "FL/Fl_Output.H"
#include "FL/Fl_Tabs.H"
#include "Fl_Value_Input_Counter.h"

#include "XPPresetConfig.h"

#ifdef _UNIX
#include "XPVisualList.h"
#endif

#include "ShipListViewWindow.h"
#include "XPInput.h"
#include "xpcTypes.h"

enum ClientConfigTab {
	Usersetup,
	Visualization,
	Hud,
	World,
	Sound,

	CCMaxTab
};

class XPClientConfig : public Fl_Window {
public:
	XPClientConfig(int x, int y, int w,int h,const char *l=0);
	XPClientConfig(int w,int h,const char *l=0);
	int handle(int);
	void Update();
	static	XPClientConfig*	Factory();
public:
	XPPresetConfig *xppConfig;
	Fl_Tabs*			flTabs;
	Fl_Widget*			tabs[CCMaxTab];

	ShipListViewWindow *shiplistview;
	ShipList Shplst;
	XPInput *shipFNameInp;
	XPInput *nameInp;
	XPInput *userInp;
	XPInput *hostInp;
	Fl_Output *shipName;

	Fl_Value_Input_Counter *power;
#ifdef _UNIX
        XPVisualList *xpvl;
#endif

private:
	void	Config_create_int(int, const char *, int *, int, int);
	void	Config_create_float(int, const char *, DFLOAT *, DFLOAT, DFLOAT);
	void	Config_create_bool(int, const char *, bool *);
#ifdef _UNIX
        void    Config_create_visual(int);
#endif
	void	Config_create_power(int);
	void	Config_create_turnSpeed(int);
	void	Config_create_turnResistance(int);
	void	Config_create_altPower(int);
	void	Config_create_altTurnSpeed(int);
	void	Config_create_altTurnResistance(int);
	void	Config_create_showMessages(int);
	void	Config_create_maxMessages(int);
	void	Config_create_messagesToStdout(int);
	void	Config_create_reverseScroll(int);
	void	Config_create_oldMessagesColor(int);
	void	Config_create_showHUD(int);
	void	Config_create_showHUDRadar(int);
	void	Config_create_horizontalHUDLine(int);
	void	Config_create_verticalHUDLine(int);
	void	Config_create_speedFactHUD(int);
	void	Config_create_speedFactPTR(int);
	void	Config_create_fuelNotify(int);
	void	Config_create_fuelWarning(int);
	void	Config_create_fuelCritical(int);
	void	Config_create_fuelGauge(int);
	void	Config_create_outlineWorld(int);
	void	Config_create_filledWorld(int);
	void	Config_create_texturedWalls(int);
	void	Config_create_texturedObjects(int);
	void	Config_create_slidingRadar(int);
	void	Config_create_showItems(int);
	void	Config_create_showItemsTime(int);
	void	Config_create_showScoreDecimals(int);
	void	Config_create_backgroundPointDist(int);
	void	Config_create_backgroundPointSize(int);
	void	Config_create_sparkSize(int);
	void	Config_create_sparkProb(int);
	void	Config_create_charsPerSecond(int);
	void	Config_create_markingLights(int);
	void	Config_create_toggleShield(int);
	void	Config_create_autoShield(int);
	void	Config_create_showNastyShots(int);
	void	Config_create_shotSize(int);
	void	Config_create_teamShotSize(int);
	void	Config_create_hudColor(int);
	void	Config_create_hudLockColor(int);
	void	Config_create_wallColor(int);
	void	Config_create_decorColor(int);
	void	Config_create_showDecor(int);
	void	Config_create_outlineDecor(int);
	void	Config_create_filledDecor(int);
	void	Config_create_texturedDecor(int);
	void	Config_create_texturedBalls(int);
	void	Config_create_maxFPS(int);
#ifdef SOUND
	void	Config_create_maxVolume(int);
#endif
	void	Config_create_showShipName(int);
	void	Config_create_showMineName(int);
	void	Config_create_fuelMeter(int);
	void	Config_create_powerMeter(int);
	void	Config_create_turnSpeedMeter(int);
	void	Config_create_packetSizeMeter(int);
	void	Config_create_packetLossMeter(int);
	void	Config_create_packetDropMeter(int);
	void	Config_create_packetLagMeter(int);
	void	Config_create_clock(int);
	void	Config_create_clockAMPM(int);
#ifdef _WINDOWS
	void	Config_create_threadedDraw(int);
#endif
	void	Config_create_scaleFactor(int);
	void	Config_create_altScaleFactor(int);
};
#endif	// _XPCLIENTCONFIG_H_
