/* $Id: Fl_EventSoundConnector.cpp,v 1.1 2007/02/17 06:13:10 dick Exp $
 *
 * XPInput - Provide a text input window that associates with a String
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
 * $Log: Fl_EventSoundConnector.cpp,v $
 * Revision 1.1  2007/02/17 06:13:10  dick
 * A Sound Editor.  Connects events to sounds
 *
 */

// Provide an Input Window that associates with a string.
// Read and Write with this string as needed.

#include "StdAfx.h"
#include <stdio.h>

#include "FL/Fl.H"
#include "FL/fl_draw.H"
#include "portability.h"
#include "xpcTypes.h"

#include "SoundUndoableCommand.h"
#include "SoundWindow.h"

#include "Fl_EventSoundConnector.h"

#define	CONNECT_BUTTON_WIDTH	16

///////////////////////////////////////////////////////////////////////////////
class ConnectButton : public FunctionButton {
public:
	ConnectButton(int x,int y,int w,int h,const char *l=0, PCSTR pm=NULL) :
	  FunctionButton(x,y,w,h,l,pm) {}
	virtual	int	handle(int event);
	bool	hover;
	Fl_EventSoundConnector*	myParent;
};


///////////////////////////////////////////////////////////////////////////////
int ConnectButton::handle(int event) {
	int ret = FunctionButton::handle(event);
	switch(event) {
	case FL_ENTER:
		printf("ConnectButton: ENTER\n");
		this->hover = true;
		return(1);
	case FL_LEAVE:
		printf("ConnectButton: LEAVE\n");
		this->hover = false;
		return(1);
	case FL_PUSH:
		printf("ConnectButton: LEAVE\n");
		this->myParent->ConnectButtonPushed();
		return(1);
	default:
		printf("ConnectButton:handle(%d)\n", event);

	}
	return(ret);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Fl_EventSoundConnector::Fl_EventSoundConnector(int x,int y,int w,int h,const char *l)
: Fl_Widget(x,y,w,h,l) {
//	origFont = textfont();
	soundWindow = NULL;
	eventListRow = 0;
	soundListRow = 0;
	eventListScrollOffset = 0;
	soundListScrollOffset = 0;
	rowHeight = 1;
	selectedSoundRow = 0;

	connectButton = new ConnectButton(0, 0, CONNECT_BUTTON_WIDTH, CONNECT_BUTTON_WIDTH, "", "cycler");
	connectButton->myParent = this;
	connectButton->hide();
}

///////////////////////////////////////////////////////////////////////////////
Fl_EventSoundConnector::~Fl_EventSoundConnector() {
}

///////////////////////////////////////////////////////////////////////////////
int Fl_EventSoundConnector::handle(int event) {
	int ret = Fl_EventSoundConnectorSUPERCLASS::handle(event);
	printf("ESC:handle(%d)\n", event);
	switch (event) {
	case FL_RELEASE:
//		if (soundWindow)
//			soundWindow->GainChanged((int)value());
		break;
	case FL_ENTER:
	case FL_MOVE:
		UpdateConnectButton(Fl::event_x(), Fl::event_y());
		return(1);
	case FL_LEAVE:
		UpdateConnectButton(-100,-100);
		return(1);
/*		if (!connectButton->hover) {
			connectButton->hide();
			connectButton->position(-100,-100);
		}
		return(1);
*/	}
	return(ret);
}

///////////////////////////////////////////////////////////////////////////////
//virtual
void Fl_EventSoundConnector::draw() {
	fl_clip(x(), y(), w(), h());
	color(FL_BLACK);
	fl_rectf(x(), y(), w(), h(), FL_BLACK);
	fl_color(FL_RED);

	int	y0 = (eventListRow-eventListScrollOffset)*rowHeight + rowHeight/2;
	int	y1 = (soundListRow-soundListScrollOffset)*rowHeight + rowHeight/2;
	fl_line(x(), y()+y0, x()+w(), y()+y1);
	fl_pop_clip();
	connectButton->draw();
}

///////////////////////////////////////////////////////////////////////////////
void	Fl_EventSoundConnector::SetEventListRow(int row) {
	eventListRow = row;
	redraw();
}

///////////////////////////////////////////////////////////////////////////////
void	Fl_EventSoundConnector::SetSoundListRow(int row) {
	printf("SetSoundListRow(%d)\n", row);
	soundListRow = row;
	selectedSoundRow = row;
	redraw();
}

///////////////////////////////////////////////////////////////////////////////
void	Fl_EventSoundConnector::SetEventListScrollOffset(int row) {
	eventListScrollOffset = row;
	redraw();
}

///////////////////////////////////////////////////////////////////////////////
void	Fl_EventSoundConnector::SetSoundListScrollOffset(int row) {
	soundListScrollOffset = row;
	redraw();
}

///////////////////////////////////////////////////////////////////////////////
void	Fl_EventSoundConnector::SetSelectedSoundRow(int r) {
	selectedSoundRow = r;
	printf("SetSelectedSoundRow(%d)\n", r);
}

///////////////////////////////////////////////////////////////////////////////
// static
//void Fl_MainGainSlider::CallbackFl(Fl_MainGainSlider* w, void*) {
//	printf("Callback\n");
//	//w->Store();
//}

///////////////////////////////////////////////////////////////////////////////
void	Fl_EventSoundConnector::UpdateConnectButton(int evx, int evy) {
	printf("updateConnect: x/y = %d/%d, w/h = %d/%d  ex/y = %d/%d\n", x(), y(), w(), h(), evx, evy);
	int	left = x() + w() - CONNECT_BUTTON_WIDTH-4;
	int	right = x() + w();
	int	btop = selectedSoundRow*rowHeight + y();

	if (evx >= left-CONNECT_BUTTON_WIDTH && evx < right
	 && selectedSoundRow != soundListRow) {
		connectButton->position(left,btop);
		connectButton->show();
	} else {
		connectButton->position(-100,-100);
		connectButton->hide();
		//damage();
		//redraw();
	}
}

///////////////////////////////////////////////////////////////////////////////
void	Fl_EventSoundConnector::ConnectButtonPushed() {
	SoundUndoableCommand* suc = new SoundUndoableCommand(soundWindow, CHANGE_BINDING, 0);
	suc->eventRow = this->eventListRow;
	suc->oldSoundRow = this->soundListRow;
	suc->newSoundRow = this->selectedSoundRow;
	soundWindow->undoMan.Do(suc);
}
