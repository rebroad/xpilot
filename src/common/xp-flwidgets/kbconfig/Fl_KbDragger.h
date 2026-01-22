/* $Id: Fl_KbDragger.h,v 1.2 2001/09/27 17:34:21 dick Exp $
 *
 * Fl_KbDragger - A command in the middle of being dragged
 *
 *      Copyright (C) 2001 by
 *
 *      The XPilot Authors   <xpilot@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
 *      Jarrod Miller        <jarrod@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
 *      Bjørn Stabell        <bjoern@xpilot.org>
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
 * $Log: Fl_KbDragger.h,v $
 * Revision 1.2  2001/09/27 17:34:21  dick
 * Draw the dragger as a yellow command button.
 *
 * Revision 1.1  2001/09/25 15:47:19  dick
 * New widget that represents a command being dragged.
 *
 */

#ifndef	_FL_KBDRAGGER_H_
#define	_FL_KBDRAGGER_H_

#include "FL/Fl_Window.H"

class Fl_KbCButton;
class KeyCommand;

#define	Fl_KbDraggerSUPERCLASS	Fl_Widget
class Fl_KbDragger : public Fl_KbDraggerSUPERCLASS
{
public:
	Fl_KbDragger(int X, int Y, int W, int H, const char* label = 0);
	virtual ~Fl_KbDragger();

	void		draw();
	int			mouseX;
	int			mouseY;
	Fl_KbCButton*	sourceButton;
	KeyCommand*	command;

	void MakeBits(KeyCommand* kc);
private:

	uchar*	bits;		// If there is a bitmap...

};

extern Fl_KbDragger* CreateKbDragger(Fl_KbCButton* b);

#endif		// _FL_KBDRAGGER_H_
