/* $Id: Fl_KbMultiComm.h,v 1.3 2001/10/04 16:15:46 dick Exp $
 *
 * Fl_KbMultiComm - A little window that contains all of the commands for a key
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
 * $Log: Fl_KbMultiComm.h,v $
 * Revision 1.3  2001/10/04 16:15:46  dick
 * Lame leak work. (comment out unused destructor)
 *
 * Revision 1.2  2001/09/25 15:45:52  dick
 * Split KbConfig to KbConfig and KbConfigKb, since the keyboard is only a part
 * of the whole KbConfig window.
 *
 * Revision 1.1  2001/09/20 07:41:02  dick
 * Move Fl_Kb* to it's own library xp-flwidgets/kbconfig/libxp-kbconfig.a
 *
 * Revision 1.1  2001/09/20 07:21:09  dick
 * If a button has multiple commands, display those in a "flyout" window
 * when the mouse is over the key.
 *
 */

#define	MULTICOMM_BORDER	3

class Fl_KbCButton;

class Fl_KbMultiComm : public Fl_Group
{
public:
	Fl_KbMultiComm(int X, int Y, int W, int H, const char* keyCap = 0);
//	virtual ~Fl_KbMultiComm() {};
	void Init(Fl_KbConfigKb* parent, Fl_KbCButton* button);

	virtual void		draw();

};
