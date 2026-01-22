/* $Id: Fl_KbDragger.cpp,v 1.5 2004/02/03 03:39:53 dick Exp $
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
 * $Log: Fl_KbDragger.cpp,v $
 * Revision 1.5  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.4  2001/10/04 16:10:45  dick
 * Don't create the dragger if there was no command on the key.
 *
 * Revision 1.3  2001/09/27 18:03:57  dick
 * Need stdlib.h
 *
 * Revision 1.2  2001/09/27 17:34:21  dick
 * Draw the dragger as a yellow command button.
 *
 * Revision 1.1  2001/09/25 15:47:19  dick
 * New widget that represents a command being dragged.
 *
 */
#include "StdAfx.h"

#include <stdlib.h>

#include "FL/Fl.H"
#include "FL/fl_draw.H"

#include "Fl_KbDragger.h"
#include "Fl_KbCButton.h"
#include "Fl_KbConfig.h"

///////////////////////////////////////////////////////////////////////////////
Fl_KbDragger::Fl_KbDragger(int X, int Y, int W, int H, const char* label)
: Fl_KbDraggerSUPERCLASS(X,Y,W,H,label)
{
	bits = 0;
	argument(ButtonTypeDragger);
}

///////////////////////////////////////////////////////////////////////////////
Fl_KbDragger::~Fl_KbDragger()
{
	if (bits)
		free(bits);
}

///////////////////////////////////////////////////////////////////////////////
Fl_KbDragger* CreateKbDragger(Fl_KbCButton* b)
{
	KbCKeyCommand* ckc = (KbCKeyCommand*)b->GetCommands().GetHead();
	if (!ckc)
		return(0);
	Fl_KbDragger* d = new Fl_KbDragger(b->x(), b->y(), b->w(), b->h());
	d->sourceButton = b;
	d->command = &keyCommands[ckc->key->key];
	if (d->command->bits)
		d->MakeBits(d->command);
	if (b->parentButton)
		d->sourceButton = b->parentButton;
	return(d);
}

///////////////////////////////////////////////////////////////////////////////
#define	ARCRAD	12
#define	FONT_LABEL_HEIGHT	10
void Fl_KbDragger::draw()
{
	int	BW = w() - BUTTON_SPACE -1;
	int BH = h() - BUTTON_SPACE -1;

	fl_color(FL_YELLOW);
//	fl_rectf(x(), y(), w(), h());
		// draw a key's background
//	fl_color(xpblue);
	fl_begin_polygon();
	fl_vertex(x()+ARCRAD, y());
	fl_vertex(x()+(BW-ARCRAD), y());
	fl_arc(x()+(BW-ARCRAD), y()+ARCRAD, ARCRAD, 90, 0);
	fl_vertex(x()+BW, y()+ARCRAD);
	fl_vertex(x()+BW, y()+BH-ARCRAD);
	fl_arc(x()+(BW-ARCRAD), y()+BH-ARCRAD, ARCRAD, 0, -90);
	fl_vertex(x()+(BW-ARCRAD), y()+BH);
	fl_vertex(x()+ARCRAD, y()+BH);
	fl_arc(x()+ARCRAD, y()+(BH-ARCRAD), ARCRAD, -90, -180);
	fl_vertex(x(), y()+(BH-ARCRAD));
	fl_vertex(x(), y()+ARCRAD);
	fl_arc(x()+ARCRAD, y()+ARCRAD, ARCRAD, -180, -270);
	fl_end_polygon();

	fl_color(FL_BLACK);
	fl_font(FL_HELVETICA, FONT_LABEL_HEIGHT);
	KeyCommand* kc = command;
	if (1)
	{
		char	line0[40];
		char	line1[40];
		char	line2[40];
//		fl_color(FL_WHITE);
		int i;
		int	j;
		line0[0] = line1[0] = line2[0] = '\0';
		for (i=0; i<40; i++)
		{
			line0[i] = kc->name[i];
			if (line0[i] == '\n')
			{
				line0[i] = '\0';
				break;
			}
			if (kc->name[i] == '\0')
				break;
		}
		j = i;
		if (kc->name[j])
		{
			if (kc->name[j] == '\n')
				j++;
			for (i=0; i<40; i++, j++)
			{
				line1[i] = kc->name[j];
				if (line1[i] == '\n')
				{
					line1[i] = '\0';
					break;
				}
				if (kc->name[j] == '\0')
					break;
			}
		}
		if (kc->name[j])
		{
			if (kc->name[j] == '\n')
				j++;
			for (i=0; i<40; i++, j++)
			{
				line2[i] = kc->name[j];
				if (line2[i] == '\n')
				{
					line2[i] = '\0';
					break;
				}
				if (kc->name[j] == '\0')
					break;
			}
		}
		if (bits)	// draw a command that has a pic
		{
			int	_w;
			int	_h;
			fl_measure(line0, _w, _h);
			fl_draw(line0, x()+w()/2-_w/2, y()+h()/2-8-4);
			fl_measure(line1, _w, _h);
			#define	FUDGE	4
			fl_draw(line1, x()+w()/2-_w/2, y()+h()/2+8+4+FUDGE);
			fl_draw_image(bits, x()+w()/2-8, y()+h()/2-8, 16, 16);
		}
		else
		{
			int	_w;
			int	_h;
			int lineCount = 1;
			if (line1[0])
				lineCount++;
			if (line2[0])
				lineCount++;
			if (lineCount == 1)
			{
				_w = 0;
				fl_measure(line0, _w, _h);
				fl_draw(line0, x()+w()/2-_w/2, y()+h()/2);
			}
			else if (lineCount == 2)
			{
				_w = 0;
				fl_measure(line0, _w, _h);
				fl_draw(line0, x()+w()/2-_w/2, y()+h()/2-_h/2);
				_w = 0;
				fl_measure(line1, _w, _h);
				fl_draw(line1, x()+w()/2-_w/2, y()+h()/2+_h/2);
			}
			else
			{
				_w = 0;
				fl_measure(line0, _w, _h);
				fl_draw(line0, x()+w()/2-_w/2, y()+h()/2-_h+1);
				_w = 0;
				fl_measure(line1, _w, _h);
				fl_draw(line1, x()+w()/2-_w/2, y()+h()/2);
				_w = 0;
				fl_measure(line2, _w, _h);
				fl_draw(line2, x()+w()/2-_w/2, y()+h()/2+_h-1);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// turn a white on blue pixmap into a black on yellow pixmap
void Fl_KbDragger::MakeBits(KeyCommand* kc)
{
	uchar* b = kc->bits;
	if (!b)
	{
		bits = 0;
		return;
	}
	int		i;
	int		j;
	uchar	rc, gc, bc;
	uchar	rb, gb, bb;
	Fl::get_color(FL_YELLOW, rc, gc, bc);
	bits = (uchar*)malloc(sizeof(uchar)*16*16*3);
//		for (i=0; i<sizeof(uchar)*16*16*3; i++)
//			*f++ = '\0';
	uchar* f = bits;
	for (i=0; i<16; i++)
	{
		for (j=0; j<16; j++)					// swap the bits in the bytes
		{
			rb = *b++;
			gb = *b++;
			bb = *b++;
			if (rb == 0xFF && gb == 0xFF && bb == 0xFF)	// white?
			{
				*f++ = 0x00;
				*f++ = 0x00;
				*f++ = 0x00;
			}
			else
			{
				*f++ = rc;		// xpblue broken down
				*f++ = gc;
				*f++ = bc;
			}
		}
	}
}
