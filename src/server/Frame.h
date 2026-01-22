/* $Id: Frame.h,v 1.4 2002/06/01 06:06:47 dick Exp $
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2001 by
 *
 *      Bjørn Stabell        <bjoern@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
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
 *  $Log: Frame.h,v $
 *  Revision 1.4  2002/06/01 06:06:47  dick
 *  Encapsulate (almost) everything.  Get rid of (almost) all refs to theWorld.
 *
 *  Revision 1.3  2002/05/22 05:09:00  dick
 *  Update to XPilot r4_5_4alpha.
 *
 *  Revision 1.2  2001/08/11 00:22:24  dick
 *  Fix shots and debris.
 *
 */
#ifndef	_FRAME_H_
#define	_FRAME_H_

#include "ConnectionPlayer.h"

/*
 * Frame: This class defines additional functions for ConnectionPlayer
 * as used by the frame generator.  This is so we have a (*this) pointer.
 * Perhaps there is a better way to describe this using friends or multiple
 * inheritence
 */

class Frame : public ConnectionPlayer
{
public:

private:
	friend void Frame_update(World* w);

	void	FastShotStore(int xf, int yf, int color, int offset);
	void	FastShotEnd();
	void	DebrisStore(int xf, int yf, int color);
	void	DebrisEnd();
	void	RadarBufferSend();

	int		Status(int ind);
	void	Map(int ind);
	void	Shots(int ind);
	void	Ships(int ind);
	void	Radar(int ind);
	void	Parameters(int ind);
	void	LoseItemState(int ind);

protected:
//	Frame();		// can't construct one of these
};

#endif
