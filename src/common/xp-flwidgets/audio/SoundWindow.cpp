/* $Id: SoundWindow.cpp,v 1.1 2007/02/17 06:13:10 dick Exp $
 *
 * SoundWindow - The Sound selection 'applet'
 *
 * XPilot5 - Manage all local XPilot activity.
 *
 *      Copyright (C) 2007 by
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
 * $Log: SoundWindow.cpp,v $
 * Revision 1.1  2007/02/17 06:13:10  dick
 * A Sound Editor.  Connects events to sounds
 *
 */
#include "StdAfx.h"
#include <assert.h>

#include "FL/Fl_File_Chooser.H"

#include "guiconst.h"
#include "portability.h"
#include "config.h"

#include "IniClient.h"

#include "AudioMan.h"
#include "flUtil.h"
#include "SoundWindow.h"
#include "SoundUndoableCommand.h"
#include "Fl_SoundEventList.h"
#include "Fl_SoundList.h"
#include "Fl_EventSoundConnector.h"
#include "Fl_MainGainSlider.h"

static void addship_cb(Fl_Widget*, void*);
//static void remship_cb(Fl_Widget*, void*);
static void copyship_cb(Fl_Widget*, void*);

#define SPACER 6


//Note:
//Prototypes of callbacks are in SoundWindow.h
///////////////////////////////////////////////////////////////////////////////
static void soundundo_cb(Fl_Widget* w, void* v) {
	SoundWindow* sw = (SoundWindow *) v;
	sw->undoMan.Undo();
}

///////////////////////////////////////////////////////////////////////////////
static void soundredo_cb(Fl_Widget* w, void* v) {
	SoundWindow* sw = (SoundWindow *) v;
	sw->undoMan.Redo();
}

///////////////////////////////////////////////////////////////////////////////
static void dummy_cb(Fl_Widget*, void* v) {
	SoundWindow *sw = (SoundWindow *) v;
	sw->redraw();
}

///////////////////////////////////////////////////////////////////////////////
static void saveSoundFile_cb(Fl_Widget* w, void* v)
{
	SoundWindow *sw = (SoundWindow *) v;
	sw->SaveSoundFile();
}

///////////////////////////////////////////////////////////////////////////////
static void saveAsSoundFile_cb(Fl_Widget* w, void* v)
{
	SoundWindow *sw = (SoundWindow *) v;
	sw->SaveAsSoundFile();
}
///////////////////////////////////////////////////////////////////////////////
static void openshipfile_cb(Fl_Widget*, void* v)
{
/*
	SoundWindow *tmp = (SoundWindow *) v;
	const char *p = NULL;
	p = fl_file_chooser("Open","XPilot Shipfile (*.shp)",NULL);
	if (p)
	{
		tmp->Shplst.ClearShipList();
		tmp->Shplst.LoadShipFile((char *) p);
		tmp->shipedit->pShip = (ShipDocument *) tmp->Shplst.pCurrentShip;
	}
	tmp->undoMan.Clear();

	tmp->shiplistview->redraw();
	tmp->shipedit->redraw();
	tmp->UpdateInfoFields();
*/
}

///////////////////////////////////////////////////////////////////////////////
static void soundclose_cb(Fl_Widget* win, void* v)
{

	SoundWindow* w = (SoundWindow*)v;
	bool close = false;
//	int ret;

/*	if (w->Shplst.changed)
	{
		ret = fl_choice("Ship Editor has unsaved changes. Do you still wish to quit?",
			"Close", "Cancel", "Save and Close");
		switch (ret)
		{
			case 0:
				close = true;
				break;
			case 1:
				close = false;
				break;
			case 2:
				if (w->SaveShipFile())
				  close = true;
				else
				  close = false;
				break;
		}
	}
	else
*/		close = true;

	if (close)
	{
		w->hide();
		delete w;
	}

}

#define SHIPTOOLSTOP BUTTON_SIZE*3
#define SHIPITEMSTOP BUTTON_SIZE*10

#define	EVENTLIST_WIDTH		200
#define	SOUNDLIST_WIDTH		200
#define	BOTTOM_AREA_HEIGHT	 25
#define	MARGIN				  2
#define	CONNECTOR_WIDTH		200
#define	GAIN_WIDTH			 50

#define	CONNECTOR_X			(MARGIN + EVENTLIST_WIDTH)
#define	WANTED_WINDOW_WIDTH	(MARGIN + EVENTLIST_WIDTH + CONNECTOR_WIDTH + SOUNDLIST_WIDTH + GAIN_WIDTH + MARGIN)
#define	MAIN_GAIN_X			(MARGIN + EVENTLIST_WIDTH)
#define	MAIN_GAIN_WIDTH		(CONNECTOR_WIDTH)
#define	TOOLBARLEFT			(MAIN_GAIN_X - (BUTTON_SIZE*5) - MARGIN)
///////////////////////////////////////////////////////////////////////////////
SoundWindow* SoundWindow::Factory() {
	SoundWindow *window = new SoundWindow(WANTED_WINDOW_WIDTH, 700, "XPilot Sound Editor");
	window->end();
	window->callback((Fl_Callback *)soundclose_cb, window);
	window->show();
	return(window);
}

///////////////////////////////////////////////////////////////////////////////
SoundWindow::SoundWindow(int w,int h,const char *l)
        : Fl_Double_Window(w,h,l)
{
#ifdef SOUND
	audioMan.IsEditor(true);
	audioMan.Init(iniClient.sounds);
	audioMan.SetGain((double)(iniClient.maxVolume)/100.0);
#endif
	iniClientChanged = false;

	flSoundEventList = new Fl_SoundEventList(MARGIN, MARGIN,
											 EVENTLIST_WIDTH, h-MARGIN-BOTTOM_AREA_HEIGHT);
	flSoundEventList->SetSoundWindow(this);
	flSoundEventList->end();

	flEventSoundConnector = new Fl_EventSoundConnector(CONNECTOR_X, MARGIN,
													   CONNECTOR_WIDTH, h-MARGIN-BOTTOM_AREA_HEIGHT);
	flEventSoundConnector->SetSoundWindow(this);
	flEventSoundConnector->SetRowHeight(flSoundEventList->row_height(0));

	flSoundList = new Fl_SoundList(EVENTLIST_WIDTH+CONNECTOR_WIDTH+MARGIN, MARGIN,
								   SOUNDLIST_WIDTH, h-MARGIN-BOTTOM_AREA_HEIGHT);
	flSoundList->SetSoundWindow(this);
	flSoundList->end();

	mainGain = new Fl_MainGainSlider(MAIN_GAIN_X, h-BOTTOM_AREA_HEIGHT-MARGIN,
									 MAIN_GAIN_WIDTH, BOTTOM_AREA_HEIGHT);
	mainGain->range(0,100);
	mainGain->value(iniClient.maxVolume);
	mainGain->type(FL_HOR_NICE_SLIDER);
	mainGain->SetSoundWindow(this);

	saveAsButton = new FunctionButton(TOOLBARLEFT+BUTTON_SIZE*0, h-MARGIN-BUTTON_SIZE, BUTTON_SIZE, BUTTON_SIZE,
		"", "filesave");
	saveAsButton->callback(saveAsSoundFile_cb);
	saveAsButton->tooltip("Save Sound File");
	saveAsButton->user_data(this);

	saveButton = new FunctionButton(TOOLBARLEFT+BUTTON_SIZE*1, h-MARGIN-BUTTON_SIZE, BUTTON_SIZE, BUTTON_SIZE,
		"", "filesave");
	saveButton->callback(saveSoundFile_cb);
	saveButton->tooltip("Save Sound File");
	saveButton->user_data(this);

	undoButton = new FunctionRepeatButton(TOOLBARLEFT+BUTTON_SIZE*3, h-MARGIN-BUTTON_SIZE, BUTTON_SIZE, BUTTON_SIZE,
		"", "undo");
	undoButton->callback(soundundo_cb);
	undoButton->tooltip("Undo");
	undoButton->user_data(this);

	redoButton = new FunctionRepeatButton(TOOLBARLEFT+BUTTON_SIZE*4, h-MARGIN-BUTTON_SIZE, BUTTON_SIZE, BUTTON_SIZE,
		"", "redo");
	redoButton->callback(soundredo_cb);
	redoButton->tooltip("Redo");
	redoButton->user_data(this);

	undoMan.RegisterButtons(UndoButtonEnabler,
		RedoButtonEnabler,
		SaveButtonEnabler, this);
}

///////////////////////////////////////////////////////////////////////////////
void SoundWindow::GainChanged(int value) {
	if (iniClient.maxVolume != value) {
		//iniClient.maxVolume = value;
		iniClientChanged = true;
		SoundUndoableCommand* suc = new SoundUndoableCommand(this, CHANGE_MAIN_GAIN, 0);
		suc->oldGain = iniClient.maxVolume;
		suc->newGain = (int)mainGain->value();
		this->undoMan.Do(suc);
	}
}

///////////////////////////////////////////////////////////////////////////////
void SoundWindow::DisplaySelectedEvent() {
	int	seIndex = flSoundEventList->row();
	printf("Event: row=%d\n", seIndex);
	SoundEvent* se = audioMan.GetSoundEvent(seIndex);
	if (!se) {
		assert(se != NULL);
		return;
	}
	Sound* s = se->sounds[0];
	int	sIndex = -1;
	if (s)
		sIndex = audioMan.GetSoundList()->Index(s);
	flSoundList->row(sIndex);
	flEventSoundConnector->SetEventListRow(seIndex);
	flEventSoundConnector->SetSoundListRow(sIndex);
}

void SoundWindow::DisplaySelectedSound() {
	flEventSoundConnector->SetSelectedSoundRow(flSoundList->row());
}

///////////////////////////////////////////////////////////////////////////////
void SoundWindow::ScrollUpdate() {
	printf("SW: sel:row_offset = %d\n", flSoundEventList->top_row());
	flEventSoundConnector->SetEventListScrollOffset(flSoundEventList->top_row());
	flEventSoundConnector->SetSoundListScrollOffset(flSoundList->top_row());
}

///////////////////////////////////////////////////////////////////////////////
void SoundWindow::SetStatus(PCSTR fmt, ...) {
	va_list	ap;
	char	s[512];

	va_start(ap, fmt);
	vsnprintf(s, 511, fmt, ap);
	status->value(s);
	va_end(ap);
	Fl::check();
};

///////////////////////////////////////////////////////////////////////////////
void SoundWindow::UndoButtonEnabler(void* myThis, bool enable) {
	SoundWindow* cfg = (SoundWindow*)myThis;
	if (enable)
		cfg->undoButton->activate();
	else
		cfg->undoButton->deactivate();
}

///////////////////////////////////////////////////////////////////////////////
void SoundWindow::RedoButtonEnabler(void* myThis, bool enable) {
	SoundWindow* sw = (SoundWindow*)myThis;
	if (enable)
		sw->redoButton->activate();
	else
		sw->redoButton->deactivate();
}

///////////////////////////////////////////////////////////////////////////////
void SoundWindow::SaveButtonEnabler(void* myThis, bool enable) {
	SoundWindow* sw = (SoundWindow*)myThis;
	if (enable)
		sw->saveButton->activate();
	else
		sw->saveButton->deactivate();

}

///////////////////////////////////////////////////////////////////////////////
// virtual
#if 0
int SoundWindow::handle(int event) {
	printf("SW:handle(%d)\n", event);
	return Fl_Window::handle(event);
}
#endif

///////////////////////////////////////////////////////////////////////////////
bool SoundWindow::SaveAsSoundFile(){

	char *p = NULL;
	bool rValue;
	p = fl_file_chooser("Save As", "XPilot Sound Configuration File (*.conf)", NULL);

	if (TestFileOverwrite(p)) {
		if (p) {
			if (rValue = audioMan.SaveSoundFile(p)) {
				return true;
			}
		}
	}
	redraw();
	return false;

}
///////////////////////////////////////////////////////////////////////////////
bool SoundWindow::SaveSoundFile() {
	String p = Conf_soundfile();
	if (!audioMan.SaveSoundFile(p)) {
		return(false);
	}
	redraw();
	return(true);
}
