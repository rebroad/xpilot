/* $Id: XPClientConfig.cpp,v 1.26 2007/02/17 06:16:45 dick Exp $
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
 * $Log: XPClientConfig.cpp,v $
 * Revision 1.26  2007/02/17 06:16:45  dick
 * Add the Sound Editor.
 *
 * Revision 1.25  2004/06/01 18:18:28  dick
 * Save the config after handling XPInput
 *
 * Revision 1.24  2004/05/28 19:02:30  dick
 * xpchoice_cb() becomes XPChoice::CallbackFl()
 *
 * Revision 1.23  2004/05/28 18:20:29  dick
 * Automagically save changes as they occur, rather than have the user press a save button.
 *
 */

#include "StdAfx.h"

#include "FL/Fl_Window.H"
#include "FL/Fl_PNG_Image.H"
#include "FL/Fl_File_Chooser.H"

#include "XPClientConfig.h"
#include "XPPresetConfig.h"
#include "XPCheckButton.h"
#include "XPChoice.h"
#include "controlconst.h"
#include "guiconst.h"
#include "proto.h"
#include "Fl_KbConfig.h"
#include "pack.h"
#include "IniClient.h"

///////////////////////////////////////////////////////////////////////////////
static void saveclientconfig_cb(Fl_Widget* w, void* v) {
	iniClient.SaveAll();
}

///////////////////////////////////////////////////////////////////////////////
static void kbconfig_cb(Fl_Widget*, void*)
{
	Fl_Window* w = CreateKbConfigWindow("Keyboard Configuration");
#if defined(_WINDOWS) && !defined(_CYGWIN)
extern FL_EXPORT HINSTANCE fl_display;
	w->icon((char*)LoadIcon(fl_display, MAKEINTRESOURCE(1)));
#endif
	w->end();
	w->show();
}

///////////////////////////////////////////////////////////////////////////////
static void presetconfig_cb(Fl_Widget* w, void* v)
{
	XPClientConfig *xpcConfig = (XPClientConfig *) v;
	if (xpcConfig->xppConfig == NULL)
		xpcConfig->xppConfig = XPPresetConfig::Factory();
	else
		xpcConfig->xppConfig->show();
	saveclientconfig_cb(w, v);
}
///////////////////////////////////////////////////////////////////////////////
static void ccopenshipfile_cb(Fl_Widget* w, void* v) {
	XPClientConfig*	tmp = (XPClientConfig*) v;
    const char *p = NULL;
	p = fl_file_chooser("Open","XPilot Shipfile (*.shp)",NULL);
    if (p)
    {
		tmp->Shplst.ClearShipList();
		tmp->Shplst.LoadShipFile((char *) p);
		iniClient.shipShapeFile.printf(p);
		if (tmp->Shplst.pCurrentShip) //Set the shipname field
		tmp->shipName->value(tmp->Shplst.pCurrentShip->name.s);
	}

	tmp->shiplistview->redraw();
	tmp->shipFNameInp->Read();
	saveclientconfig_cb(w, v);
}

///////////////////////////////////////////////////////////////////////////////
static void xpcheckbutton_cb(XPCheckButton* w, void* v) {
	w->Callback();
	saveclientconfig_cb(w, v);
}

///////////////////////////////////////////////////////////////////////////////
static void xpinput_cb(XPInput* w, void* v) {
	XPInput::CallbackFl(w, v);
	saveclientconfig_cb(w, v);
}

///////////////////////////////////////////////////////////////////////////////
XPClientConfig* XPClientConfig::Factory()
{
	XPClientConfig*	window = new XPClientConfig(XPCLIENTCONFIG_WIDTH,XPCLIENTCONFIG_HEIGHT, "Client Configuration");
#if defined(_WINDOWS) && !defined(_CYGWIN)
	window->icon((char*)LoadIcon(fl_display, MAKEINTRESOURCE(ICON_WHEREWINDOW)));
#else
#endif
	window->end();
	window->show();
	window->shiplistview->ViewSelectedShip();
	return(window);
}

///////////////////////////////////////////////////////////////////////////////
static const char* CCTabSheets[] =
{
	"User Data",
	"Display",
	"Hud",
	"World",
	"Sound",
};

#define CC_INPUTLEFT XPCLIENTCONFIG_WIDTH-CC_INPUTWIDTH

#define CC_TABSTOP  BUTTON_SIZE_LG+4
#define CC_TABSLEFT	SHIPLIST_SIZE+4
#define	CCTAB_GROUP_INSET	10
#define	CCTAB_HEIGHT	35

#define	CCTAB_BOTTOM	(CC_TABSTOP + CCTAB_HEIGHT)
#define	CCOPTIONS_TOP	(CC_TABSTOP + CCTAB_HEIGHT + CCTAB_GROUP_INSET)
#define	CCOPTIONS_HEIGHT	(XPCLIENTCONFIG_HEIGHT-CCOPTIONS_TOP-10-CCTAB_GROUP_INSET)

///////////////////////////////////////////////////////////////////////////////
XPClientConfig::XPClientConfig(int w,int h,const char *l)
        : Fl_Window(w,h,l)
{
	int j;

	shiplistview = new ShipListViewWindow(0, CC_INPUTHEIGHT+4,
										  SHIPLIST_SIZE, h-(CC_INPUTHEIGHT+4),
										  true, "", &Shplst);
	shiplistview->end();
	shiplistview->pShipList->LoadShipFile(iniClient.shipShapeFile);
	shiplistview->pShipList->pCurrentShip = (ShipDocument *) shiplistview->pShipList->Find(iniClient.shipShapeName);

	shipName = new Fl_Output(0, 0, SHIPLIST_SIZE, CC_INPUTHEIGHT, NULL);
	if (Shplst.pCurrentShip) //Set the shipname field
		shipName->value(Shplst.pCurrentShip->name.s);
	shipName->textsize(10);


	FunctionButton *kbconfig_btn
		= new FunctionButton(SHIPLIST_SIZE+2, 0,
		BUTTON_SIZE_LG, BUTTON_SIZE_LG, NULL, "kbconfig");
	kbconfig_btn->callback(kbconfig_cb);
	kbconfig_btn->tooltip("Configure your player keyboard settings");
	kbconfig_btn->align(FL_ALIGN_RIGHT);

	Fl_PNG_Image *presetconfig_pm = new Fl_PNG_Image(GetButtonFile("kbconfig"));
	FunctionButton *presetconfig_btn
		= new FunctionButton(SHIPLIST_SIZE+BUTTON_SIZE_LG+4, 0,
		BUTTON_SIZE_LG, BUTTON_SIZE_LG, NULL, "clientpresets");
	presetconfig_btn->callback(presetconfig_cb);
	presetconfig_btn->user_data(this);
	presetconfig_btn->tooltip("Configure your player presets");
	presetconfig_btn->align(FL_ALIGN_RIGHT);

	Fl_Tabs* o = new Fl_Tabs(CC_TABSLEFT,
							 CC_TABSTOP,
							 XPCLIENTCONFIG_WIDTH-CC_TABSLEFT-4,
							 XPCLIENTCONFIG_HEIGHT-CC_TABSTOP-8);
	flTabs = o;
	for (j=0; j<CCMaxTab; j++)
	{
		Fl_Group* o = new Fl_Group(CC_TABSLEFT+CCTAB_GROUP_INSET,
								   CCTAB_BOTTOM,
								   XPCLIENTCONFIG_WIDTH-CC_TABSLEFT-(CCTAB_GROUP_INSET*2),
								   CCOPTIONS_HEIGHT,
								   CCTabSheets[j]);
		tabs[j] = o;
		switch (j)
		{
		case Usersetup:
		{

			nameInp = new XPInput(CC_INPUTLEFT, CCTAB_BOTTOM+4+CC_OPTIONSIZE,
								  CC_INPUTWIDTH, CC_INPUTHEIGHT, "XPilot Name:");
			nameInp->callback((Fl_Callback *)xpinput_cb);
			nameInp->SetStrng(&iniClient.name);
			nameInp->Read();

			userInp = new XPInput(CC_INPUTLEFT,  CCTAB_BOTTOM+4+CC_OPTIONSIZE*2,
								  CC_INPUTWIDTH, CC_INPUTHEIGHT, "Real User:");
			userInp->callback((Fl_Callback *)xpinput_cb);
			userInp->SetStrng(&iniClient.realname);
			userInp->Read();

			hostInp = new XPInput(CC_INPUTLEFT, CCTAB_BOTTOM+4+CC_OPTIONSIZE*3,
								  CC_INPUTWIDTH, CC_INPUTHEIGHT, "Host:");
			hostInp->callback((Fl_Callback *)xpinput_cb);
			hostInp->SetStrng(&iniClient.hostname);
			hostInp->Read();

			Config_create_power(CCTAB_BOTTOM+4+CC_OPTIONSIZE*4);
			Config_create_turnSpeed(CCTAB_BOTTOM+4+CC_OPTIONSIZE*5);
			Config_create_turnResistance(CCTAB_BOTTOM+4+CC_OPTIONSIZE*6);
			Config_create_altPower(CCTAB_BOTTOM+4+CC_OPTIONSIZE*7);
			Config_create_altTurnSpeed(CCTAB_BOTTOM+4+CC_OPTIONSIZE*8);
			Config_create_altTurnResistance(CCTAB_BOTTOM+4+CC_OPTIONSIZE*9);
			Config_create_toggleShield(CCTAB_BOTTOM+4+CC_OPTIONSIZE*10);
			Config_create_autoShield(CCTAB_BOTTOM+4+CC_OPTIONSIZE*11);

			shipFNameInp = new XPInput(CC_INPUTLEFT-BUTTON_SIZE-4, CCTAB_BOTTOM+4+CC_OPTIONSIZE*18,
									   CC_INPUTWIDTH, CC_INPUTHEIGHT, "ShipShape File Name:");
			shipFNameInp->callback((Fl_Callback *)xpinput_cb);
			shipFNameInp->SetStrng(&iniClient.shipShapeFile);
			shipFNameInp->Read();

			FunctionButton* b = new FunctionButton(shipFNameInp->x() + CC_INPUTWIDTH+4,
				shipFNameInp->y(), BUTTON_SIZE, BUTTON_SIZE,
				"", "fileopen");
			b->callback(ccopenshipfile_cb);
			b->tooltip("Open Shipfile");
			b->user_data(this);

			break;
		}
		case Visualization:
//			Config_create_showMessages(CCTAB_BOTTOM+4);
			Config_create_maxMessages(CCTAB_BOTTOM+4+CC_OPTIONSIZE);
			Config_create_messagesToStdout(CCTAB_BOTTOM+4+CC_OPTIONSIZE*2);
//			Config_create_reverseScroll(CCTAB_BOTTOM+4+CC_OPTIONSIZE*3);
			Config_create_oldMessagesColor(CCTAB_BOTTOM+4+CC_OPTIONSIZE*4);
			Config_create_speedFactPTR(CCTAB_BOTTOM+4+CC_OPTIONSIZE*5);
//			Config_create_slidingRadar(CCTAB_BOTTOM+4+CC_OPTIONSIZE*6);
			Config_create_sparkSize(CCTAB_BOTTOM+4+CC_OPTIONSIZE*7);
			Config_create_sparkProb(CCTAB_BOTTOM+4+CC_OPTIONSIZE*8);
			Config_create_markingLights(CCTAB_BOTTOM+4+CC_OPTIONSIZE*9);
			Config_create_showNastyShots(CCTAB_BOTTOM+4+CC_OPTIONSIZE*10);
			Config_create_shotSize(CCTAB_BOTTOM+4+CC_OPTIONSIZE*11);
			Config_create_teamShotSize(CCTAB_BOTTOM+4+CC_OPTIONSIZE*12);
			Config_create_maxFPS(CCTAB_BOTTOM+4+CC_OPTIONSIZE*13);
#if defined(_WINDOWS) && !defined(_CYGWIN)
			Config_create_threadedDraw(CCTAB_BOTTOM+4+CC_OPTIONSIZE*14);
#endif
			Config_create_scaleFactor(CCTAB_BOTTOM+4+CC_OPTIONSIZE*15);
			Config_create_altScaleFactor(CCTAB_BOTTOM+4+CC_OPTIONSIZE*16);
#if defined(_UNIX)
                        xpvl = new XPVisualList();
                        xpvl->PrintVisualInfo();
			Config_create_visual(CCTAB_BOTTOM+4+CC_OPTIONSIZE*17);
#endif
			break;
		case Hud:
//			Config_create_showHUD(CCTAB_BOTTOM+4);
//			Config_create_showHUDRadar(CCTAB_BOTTOM+4+CC_OPTIONSIZE);
//			Config_create_horizontalHUDLine(CCTAB_BOTTOM+4+CC_OPTIONSIZE*2);
//			Config_create_verticalHUDLine(CCTAB_BOTTOM+4+CC_OPTIONSIZE*3);
			Config_create_speedFactHUD(CCTAB_BOTTOM+4+CC_OPTIONSIZE*4);
			Config_create_fuelNotify(CCTAB_BOTTOM+4+CC_OPTIONSIZE*5);
			Config_create_fuelWarning(CCTAB_BOTTOM+4+CC_OPTIONSIZE*6);
			Config_create_fuelCritical(CCTAB_BOTTOM+4+CC_OPTIONSIZE*7);
//			Config_create_fuelGauge(CCTAB_BOTTOM+4+CC_OPTIONSIZE*8);
//			Config_create_showItems(CCTAB_BOTTOM+4+CC_OPTIONSIZE*9);
			Config_create_showItemsTime(CCTAB_BOTTOM+4+CC_OPTIONSIZE*10);
			Config_create_showScoreDecimals(CCTAB_BOTTOM+4+CC_OPTIONSIZE*11);
			Config_create_charsPerSecond(CCTAB_BOTTOM+4+CC_OPTIONSIZE*12);
			Config_create_hudColor(CCTAB_BOTTOM+4+CC_OPTIONSIZE*13);
			Config_create_hudLockColor(CCTAB_BOTTOM+4+CC_OPTIONSIZE*14);
//			Config_create_showShipName(CCTAB_BOTTOM+4+CC_OPTIONSIZE*15);
//			Config_create_showMineName(CCTAB_BOTTOM+4+CC_OPTIONSIZE*16);
//			Config_create_fuelMeter(CCTAB_BOTTOM+4+CC_OPTIONSIZE*17);
//			Config_create_powerMeter(CCTAB_BOTTOM+4+CC_OPTIONSIZE*18);
//			Config_create_turnSpeedMeter(CCTAB_BOTTOM+4+CC_OPTIONSIZE*19);
//			Config_create_packetSizeMeter(CCTAB_BOTTOM+4+CC_OPTIONSIZE*20);
//			Config_create_packetLossMeter(CCTAB_BOTTOM+4+CC_OPTIONSIZE*21);
//			Config_create_packetDropMeter(CCTAB_BOTTOM+4+CC_OPTIONSIZE*22);
//			Config_create_packetLagMeter(CCTAB_BOTTOM+4+CC_OPTIONSIZE*23);
//			Config_create_clock(CCTAB_BOTTOM+4+CC_OPTIONSIZE*24);
//			Config_create_clockAMPM(CCTAB_BOTTOM+4+CC_OPTIONSIZE*25);
			break;
		case World:
//			Config_create_outlineWorld(CCTAB_BOTTOM+4);
//			Config_create_filledWorld(CCTAB_BOTTOM+4+CC_OPTIONSIZE);
//			Config_create_texturedWalls(CCTAB_BOTTOM+4+CC_OPTIONSIZE*2);
			Config_create_texturedObjects(CCTAB_BOTTOM+4+CC_OPTIONSIZE*3);
			Config_create_backgroundPointDist(CCTAB_BOTTOM+4+CC_OPTIONSIZE*4);
			Config_create_backgroundPointSize(CCTAB_BOTTOM+4+CC_OPTIONSIZE*5);
			Config_create_wallColor(CCTAB_BOTTOM+4+CC_OPTIONSIZE*6);
			Config_create_decorColor(CCTAB_BOTTOM+4+CC_OPTIONSIZE*7);
//			Config_create_showDecor(CCTAB_BOTTOM+4+CC_OPTIONSIZE*8);
//			Config_create_outlineDecor(CCTAB_BOTTOM+4+CC_OPTIONSIZE*9);
//			Config_create_filledDecor(CCTAB_BOTTOM+4+CC_OPTIONSIZE*10);
//			Config_create_texturedDecor(CCTAB_BOTTOM+4+CC_OPTIONSIZE*11);
//			Config_create_texturedBalls(CCTAB_BOTTOM+4+CC_OPTIONSIZE*12);
			break;
		case Sound:
#ifdef SOUND_NOTUSED
			Config_create_maxVolume(CCTAB_BOTTOM+4);
#endif
			break;

		}
		o->end();
//		Fl_Group::current()->resizable(o);
	}
	o->end();
	xppConfig = NULL;
}

///////////////////////////////////////////////////////////////////////////////
XPClientConfig::XPClientConfig(int x, int y, int w,int h,const char *l)
        : Fl_Window(x,y,w,h,l)
{
}

///////////////////////////////////////////////////////////////////////////////
void XPClientConfig::Update()
{

	sprintf(iniClient.shipShapeName, "%s", (PCSTR)Shplst.pCurrentShip->name);
	iniClient.shipShape = Shplst.pCurrentShip->Convert_ship_2_string();
	shipName->value(Shplst.pCurrentShip->name);
	saveclientconfig_cb(NULL, NULL);
}

///////////////////////////////////////////////////////////////////////////////
int XPClientConfig::handle(int event)
{
    switch (event) {
    case SELECTSHIP :
		if (Shplst.changed)
		{
			Update();
			Shplst.changed = false;
			Fl_Widget* w = (Fl_Widget*)user_data();
			if (w)
				w->handle(SELECTSHIP);
			return TRUE;
		}
    }
    return Fl_Window::handle(event);
}

///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_int(int height, const char *str, int *val, int min, int max)
{
	Fl_Value_Input_Counter *o = new Fl_Value_Input_Counter(CC_INPUTLEFT, height,
		CC_INPUTWIDTH, CC_INPUTHEIGHT, str);
	o->align(FL_ALIGN_LEFT);
	o->bounds(min, max);
	o->SetInt(val);
	o->Read();
	o->callback(saveclientconfig_cb);
	o->end();
}

///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_float(int height, const char *str, DFLOAT *val, DFLOAT min, DFLOAT max)
{
	Fl_Value_Input_Counter *o = new Fl_Value_Input_Counter(CC_INPUTLEFT, height,
		CC_INPUTWIDTH, CC_INPUTHEIGHT, str);
	o->align(FL_ALIGN_LEFT);
	o->bounds(min, max);
	o->SetFlt(val);
	o->Read();
	o->callback(saveclientconfig_cb);
	o->end();
}

///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_bool(int height, const char *str, bool *val)
{
	XPCheckButton *o = new XPCheckButton(CC_INPUTLEFT, height,
		CC_INPUTWIDTH, CC_INPUTHEIGHT, str);
	o->align(FL_ALIGN_LEFT);
	o->SetBool(val);
	o->Read();
	o->callback((Fl_Callback *)xpcheckbutton_cb);
}

///////////////////////////////////////////////////////////////////////////////
#if defined (_UNIX)
void    XPClientConfig::Config_create_visual(int height)
{
	int i, numvisuals = xpvl->GetCount();
	XPVisual *tmpvis = NULL;
	int listval = -999;

	tmpvis = (XPVisual *) xpvl->GetHead();

	if (tmpvis == NULL)
		return;

	XPChoice *o = new XPChoice(CC_INPUTLEFT, height,
							   CC_INPUTWIDTH, CC_INPUTHEIGHT, "Visual Mode");

	o->SetChar(iniClient.visualName);
	o->callback((Fl_Callback *)XPChoice::CallbackFl);

	if (numvisuals > 0)
	{
		for (i = 0; i < numvisuals; i++)
		{
			char visname[30];
			sprintf(visname, "0x%lx", tmpvis->visualinfo.visualid);
			o->add(visname);

			//if the value we're adding matches the value in iniClient, set it as current
			//printf("X%sX%sX\n", visname, iniClient.visualName);
			if (strncmp(visname, iniClient.visualName, strlen(visname)) == 0)
				listval = i;

			tmpvis = (XPVisual *) tmpvis->GetNext();
		}
	}

	//Add the "auto" choice which is ignored by xpilot, thus using the system defaults
	o->add("Auto");

	if (listval == -999)
	listval = numvisuals; //Set to auto

	o->value(listval);
}
#endif

///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_power(int height)
{
	Config_create_float(height, "Power:", &iniClient.power,
		MIN_PLAYER_POWER, MAX_PLAYER_POWER);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_turnSpeed(int height)
{
	Config_create_float(height, "turnSpeed:",
			&iniClient.turnspeed, MIN_PLAYER_TURNSPEED, MAX_PLAYER_TURNSPEED);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_turnResistance(int height)
{
	Config_create_float(height, "turnResistance:",
			&iniClient.turnresistance, MIN_PLAYER_TURNRESISTANCE, MAX_PLAYER_TURNRESISTANCE);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_altPower(int height)
{
	Config_create_float(height, "altPower:",
			&iniClient.power_s, MIN_PLAYER_POWER, MAX_PLAYER_POWER);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_altTurnSpeed(int height)
{
	Config_create_float(height, "altTurnSpeed:",
			&iniClient.turnspeed_s, MIN_PLAYER_TURNSPEED, MAX_PLAYER_TURNSPEED);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_altTurnResistance(int height)
{
	Config_create_float(height, "altTurnResistance:",
			&iniClient.turnresistance_s, MIN_PLAYER_TURNRESISTANCE, MAX_PLAYER_TURNRESISTANCE);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_showMessages(int height)
{
//	Config_create_bool(height, "showMessages:", LJAOPIJEIJL:);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_maxMessages(int height)
{
	Config_create_int(height, "maxMessages:", &iniClient.maxMessages, 1, MAX_MSGS);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_messagesToStdout(int height)
{
	Config_create_int(height, "messagesToStdout:", &iniClient.messagesToStdout, 0, 2);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_reverseScroll(int height)
{
	//Config_create_bool(height, "reverseScroll", jlmljlj);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_oldMessagesColor(int height)
{
	Config_create_int(height, "oldMessagesColor", &iniClient.oldMessagesColor, 0, iniClient.maxColors - 1);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_showHUD(int height)
{
	//Config_create_bool(height, "showHUD", jlmljlj);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_showHUDRadar(int height)
{
	//Config_create_bool(height, "showHUDRadar", jlmljlj);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_horizontalHUDLine(int height)
{
	//Config_create_bool(height, "horizontalHUDLine", jlmljlj);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_verticalHUDLine(int height)
{
	//Config_create_bool(height, "verticalHUDLine", jlmljlj);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_speedFactHUD(int height)
{
	Config_create_float(height, "speedFactHUD:",
			&iniClient.hudMoveFact, -10.0, 10.0);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_speedFactPTR(int height)
{
	Config_create_float(height, "speedFactPTR:",
			&iniClient.ptrMoveFact, -10.0, 10.0);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_fuelNotify(int height)
{
	Config_create_int(height, "fuelNotify:",
			&iniClient.fuelLevel3, 0, 1000);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_fuelWarning(int height)
{
	Config_create_int(height, "fuelWarning:",
			&iniClient.fuelLevel2, 0, 1000);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_fuelCritical(int height)
{
	Config_create_int(height, "fuelCritical:",
			&iniClient.fuelLevel1, 0, 1000);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_fuelGauge(int height)
{
	//Config_create_bool(height, "fuelGauge", jlmljlj);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_outlineWorld(int height)
{
	//Config_create_bool(height, "outlineWorld", jlmljlj);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_filledWorld(int height)
{
	//Config_create_bool(height, "filledWorld", jlmljlj);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_texturedWalls(int height)
{
	//Config_create_bool(height, "texturedWalls", jlmljlj);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_texturedObjects(int height)
{
	Config_create_bool(height, "texturedObjects:", &iniClient.blockBitmaps);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_slidingRadar(int height)
{
	//Config_create_bool(height, "slidingRadar", jlmljlj);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_showItems(int height)
{
	//Config_create_bool(height, "showItems", jlmljlj);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_showItemsTime(int height)
{
	Config_create_float(height, "showItemsTime:", &iniClient.showItemsTime,
			MIN_SHOW_ITEMS_TIME, MAX_SHOW_ITEMS_TIME);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_showScoreDecimals(int height)
{
	Config_create_int(height, "showScoreDecimals:", &iniClient.showScoreDecimals,
			0, 2);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_backgroundPointDist(int height)
{
	Config_create_int(height, "backgroundPointDist", &iniClient.mapPointDistance,
		0, 10);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_backgroundPointSize(int height)
{
	Config_create_int(height, "backgroundPointSize", &iniClient.mapPointSize,
							 MIN_MAP_POINT_SIZE, MAX_MAP_POINT_SIZE);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_sparkSize(int height)
{
	Config_create_int(height, "sparkSize", &iniClient.sparkSize,
							 MIN_SPARK_SIZE, MAX_SPARK_SIZE);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_sparkProb(int height)
{
	Config_create_float(height, "sparkProb", &iniClient.sparkProb,
							   0.0, 1.0);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_charsPerSecond(int height)
{
	Config_create_int(height, "charsPerSecond", &iniClient.charsPerSecond,
							 10, 255);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_markingLights(int height)
{
	Config_create_bool(height, "markingLights", &iniClient.markingLights);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_toggleShield(int height)
{
	Config_create_bool(height, "toggleShield", &iniClient.toggleShield);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_autoShield(int height)
{
	Config_create_bool(height, "autoShield", &iniClient.autoShield);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_showNastyShots(int height)
{
	Config_create_bool(height, "nastyShots", &iniClient.showNastyShots);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_shotSize(int height)
{
	Config_create_int(height, "shotSize", &iniClient.shotSize,
							MIN_SHOT_SIZE, MAX_SHOT_SIZE);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_teamShotSize(int height)
{
	Config_create_int(height, "teamShotSize", &iniClient.teamShotSize,
						   MIN_TEAMSHOT_SIZE, MAX_TEAMSHOT_SIZE);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_hudColor(int height)
{
	Config_create_int(height, "hudColor", &iniClient.hudColor,
						   1, iniClient.maxColors - 1);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_hudLockColor(int height)
{
	Config_create_int(height, "hudLockColor", &iniClient.hudLockColor,
						   1, iniClient.maxColors - 1);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_wallColor(int height)
{
	Config_create_int(height, "wallColor", &iniClient.wallColor,
						   1, iniClient.maxColors - 1);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_decorColor(int height)
{
	Config_create_int(height, "decorColor", &iniClient.decorColor,
						   1, iniClient.maxColors - 1);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_showDecor(int height)
{
	//Config_create_bool(height, "showDecor", jlmljlj);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_outlineDecor(int height)
{
	//Config_create_bool(height, "outlineDecor", jlmljlj);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_filledDecor(int height)
{
	//Config_create_bool(height, "filledDecor", jlmljlj);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_texturedDecor(int height)
{
	//Config_create_bool(height, "texturedDecor", jlmljlj);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_texturedBalls(int height)
{
	//Config_create_bool(height, "texturedBalls", jlmljlj);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_maxFPS(int height)
{
//	Config_create_int(height, "maxFPS", &iniClient.maxFPS, FPS / 2, FPS);
}
///////////////////////////////////////////////////////////////////////////////
#ifdef SOUND_NOTUSED
void	XPClientConfig::Config_create_maxVolume(int height)
{
	Config_create_int(height, "maxVolume", &maxVolume, 0, 255);
}
#endif
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_showShipName(int height)
{
	//Config_create_bool(height, "showShipName", jlmljlj);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_showMineName(int height)
{
	//Config_create_bool(height, "showMineName", jlmljlj);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_fuelMeter(int height)
{
	//Config_create_bool(height, "fuelMeter", jlmljlj);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_powerMeter(int height)
{
	//Config_create_bool(height, "powerMeter", jlmljlj);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_turnSpeedMeter(int height)
{
	//Config_create_bool(height, "turnSpeedMeter", jlmljlj);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_packetSizeMeter(int height)
{
	//Config_create_bool(height, "packetSizeMeter", jlmljlj);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_packetLossMeter(int height)
{
	//Config_create_bool(height, "packetLossMeter"", jlmljlj);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_packetDropMeter(int height)
{
	//Config_create_bool(height, "packetDropMeter", jlmljlj);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_packetLagMeter(int height)
{
	//Config_create_bool(height, "packetLagMeter", jlmljlj);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_clock(int height)
{
	//Config_create_bool(height, "clock", jlmljlj);
}
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_clockAMPM(int height)
{
	//Config_create_bool(height, "clockAMPM", jlmljlj);
}
#if defined(_WINDOWS) && !defined(_CYGWIN)
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_threadedDraw(int height)
{
	Config_create_bool(height, "threadedDraw", &iniClient.threadedDraw);
}
#endif
///////////////////////////////////////////////////////////////////////////////
void	XPClientConfig::Config_create_scaleFactor(int height)
{
	Config_create_float(height, "scaleFactor", &iniClient.scaleFactor,
							   MIN_SCALEFACTOR, MAX_SCALEFACTOR);
}
///////////////////////////////////////////////////////////////////////////////
void XPClientConfig::Config_create_altScaleFactor(int height)
{
	Config_create_float(height, "altScaleFactor", &iniClient.scaleFactor_s,
							   MIN_SCALEFACTOR, MAX_SCALEFACTOR);
}
