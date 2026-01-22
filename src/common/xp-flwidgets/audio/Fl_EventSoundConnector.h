/* $Id: Fl_EventSoundConnector.h,v 1.1 2007/02/17 06:13:10 dick Exp $
 *
 * Fl_EventSoundConnector.h - Display a connector between events and sounds
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
 * $Log: Fl_EventSoundConnector.h,v $
 * Revision 1.1  2007/02/17 06:13:10  dick
 * A Sound Editor.  Connects events to sounds
 *
 */

#ifndef	_FLEVENTSOUNDCONNECTOR_H_
#define	_FLEVENTSOUNDCONNECTOR_H_

#undef 	index
#include "FL/Fl_Widget.H"

class	SoundWindow;
class	ConnectButton;

#define	Fl_EventSoundConnectorSUPERCLASS	Fl_Widget
class Fl_EventSoundConnector : public Fl_EventSoundConnectorSUPERCLASS {
public:
	Fl_EventSoundConnector(int x,int y,int w,int h,const char *l=0);
	~Fl_EventSoundConnector();

	void	SetSoundWindow(SoundWindow* sw);

	void	SetRowHeight(int h);
	void	SetEventListRow(int row);
	void	SetSoundListRow(int row);
	void	SetEventListScrollOffset(int row);
	void	SetSoundListScrollOffset(int row);
	void	SetSelectedSoundRow(int row);

    virtual int		handle(int event);
	virtual void	draw();

	void	ConnectButtonPushed();
private:
	void	UpdateConnectButton(int x, int y);

	SoundWindow*	soundWindow;
	ConnectButton*	connectButton;

	int		eventListRow;
	int		soundListRow;
	int		eventListScrollOffset;
	int		soundListScrollOffset;
	int		rowHeight;
	int		selectedSoundRow;
};

inline void Fl_EventSoundConnector::SetSoundWindow(SoundWindow* sw) { soundWindow = sw; }
inline void Fl_EventSoundConnector::SetRowHeight(int h) { rowHeight = h; }


#endif	// _FLEVENTSOUNDCONNECTOR_H_
