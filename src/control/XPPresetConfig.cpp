/*
* XPilotControl, .  Copyright (C) 2001 by
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
 * $Log: XPPresetConfig.cpp,v $
 * Revision 1.4  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.3  2004/01/06 04:48:28  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 * Revision 1.2  2003/07/18 23:57:05  jlmiller
 * Flesh out the PresetConfigs a bit more...
 *
 * Revision 1.1  2002/09/10 19:10:29  jlmiller
 * Opening pass at preset config. Quite a few more controls added to ClientConfig.
 *
 */
#include "StdAfx.h"

#include "FL/Fl_Window.H"
#include "FL/Fl_XPM_Image.H"

#include "XPPresetConfig.h"
#include "controlconst.h"
#include "guiconst.h"
///////////////////////////////////////////////////////////////////////////////
void closepresets_cb(Fl_Widget* , void* v) {
	XPPresetConfig *w = (XPPresetConfig *) v;
	int i;

	for (i = 0; i < NUM_MODBANKS; i++)
	{
	XPPreset_Group *prGP = (XPPreset_Group *) w->presets[i];
//	prGP->GetStrng(iniClient.modBankStr[i]);
	sprintf(iniClient.modBankStr[i], prGP->GetStrng());
	}
	w->hide();
}

///////////////////////////////////////////////////////////////////////////////
XPPresetConfig* XPPresetConfig::Factory()
{
	XPPresetConfig*	window = new XPPresetConfig(XPPRESETCONFIG_WIDTH, XPPRESETCONFIG_HEIGHT, "Preset Configuration");
#if defined(_WINDOWS) && !defined(_CYGWIN)
	window->icon((char*)LoadIcon(fl_display, MAKEINTRESOURCE(ICON_WHEREWINDOW)));
#else
#endif
	window->end();
	window->show();
	window->make_current();
	return(window);
}

///////////////////////////////////////////////////////////////////////////////
XPPresetConfig::XPPresetConfig(int w,int h,const char *l)
        : Fl_Window(w,h,l)
{
	int i;
	for (i = 0; i < NUM_MODBANKS; i++)
	{
		presets[i] = new XPPreset_Group((i+1)*75+(i*55), 20, CC_INPUTWIDTHSMALL, h, "");
		presets[i]->align(FL_ALIGN_TOP);
		presets[i]->end();
		presets[i]->SetStrng(iniClient.modBankStr[i]);
	}

//	preset1 = new XPPreset_Group(60, 20, CC_INPUTWIDTHSMALL, h, "Preset 1");
//	preset1->align(FL_ALIGN_TOP);
//	preset1->end();
//	preset1->SetStrng(iniClient.modBankStr[0]);

	closeBtn = new Fl_Button(w/2-20, h-20, 40, 20, "Close");
	closeBtn->callback((Fl_Callback *)closepresets_cb, this);
	closeBtn->tooltip("Close the preset window, saving changes.");
	closeBtn->align(FL_ALIGN_CENTER);

};
///////////////////////////////////////////////////////////////////////////////
XPPresetConfig::XPPresetConfig(int x, int y, int w,int h,const char *l)
        : Fl_Window(x,y,w,h,l)
{
};
///////////////////////////////////////////////////////////////////////////////
void XPPresetConfig::UpdatePresets()
{
	int i;

	for (i = 0; i < NUM_MODBANKS; i++)
	{
		presets[i]->UpdateSettings();
	}
}
