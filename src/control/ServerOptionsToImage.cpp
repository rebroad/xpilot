/* $Id: ServerOptionsToImage.cpp,v 1.3 2004/02/03 03:39:53 dick Exp $
 *
 * Given a ServerOptions, return an Fl_Image
 *
 *      Copyright (C) 2004 by
 *
 *      The XPilot Authors   <xpilot@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
 *      Jarrod Miller        <jarrod@xpilot.org>
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
 * $Log: ServerOptionsToImage.cpp,v $
 * Revision 1.3  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.2  2004/01/19 22:10:38  dick
 * Colorize all the known block types
 *
 * Revision 1.1  2004/01/19 06:49:06  dick
 * Convert a ServerOptions to an Fl_Image.
 */
#include "StdAfx.h"

#include <string.h>

#include "FL/Fl.H"
#include "FL/Fl_Image.H"

#include "ServerOptions.h"
#include "ServerOptionsToImage.h"
#include "map.h"

#define	SetColor(_type, _r, _g, _b)	 { r[_type] = _r; g[_type] = _g; b[_type] = _b; }

///////////////////////////////////////////////////////////////////////////////
Fl_Image* ServerOptionsToImage(ServerOptions* sos)
{
	int			w = sos->mapWidth->GetInt();
	int			h = sos->mapHeight->GetInt();
	int			x;
	int			y;
	ServerOptionBlocks*	map = sos->mapData;

	uchar*		data = new uchar[w*h*3];
	uchar*		i = (uchar*)data;
	uchar		r[256];
	uchar		g[256];
	uchar		b[256];
	char		c;

	memset(r, 0, sizeof(r));
	memset(g, 0, sizeof(r));
	memset(b, 0, sizeof(r));

	SetColor(MAP_REC_RD,	0x00,0x00,0xFF);
	SetColor(MAP_REC_LD,	0x00,0x00,0xFF);
	SetColor(MAP_FILLED,	0x00,0x00,0xFF);
	SetColor(MAP_REC_RU,	0x00,0x00,0xFF);
	SetColor(MAP_REC_LU,	0x00,0x00,0xFF);

	SetColor(MAP_CAN_LEFT,	0xFF,0xFF,0xFF);
	SetColor(MAP_CAN_UP,	0xFF,0xFF,0xFF);
	SetColor(MAP_CAN_DOWN,	0xFF,0xFF,0xFF);
	SetColor(MAP_CAN_RIGHT,	0xFF,0xFF,0xFF);

	SetColor(MAP_BASE,		0xFF,0xFF,0xFF);

	SetColor(MAP_FUEL,		0xFF,0x00,0x00);
	SetColor(MAP_TARGET,	0xFF,0x00,0x00);
	SetColor(MAP_TREASURE,	0xFF,0x00,0x00);
	SetColor(MAP_ITEM_CONC,	0xFF,0x00,0x00);

	SetColor(MAP_FRICTION,	0xFF,0x77,0x00);
	SetColor(MAP_EMPTYTREASURE,	0xFF,0x00,0x00);
	SetColor(MAP_ASTEROID_CONC,	0xFF,0x00,0x00);

	SetColor(MAP_GRAV_POS,	0xFF,0x00,0x00);
	SetColor(MAP_GRAV_NEG,	0xFF,0x00,0x00);
	SetColor(MAP_GRAV_ACWISE,0xFF,0x00,0x00);
	SetColor(MAP_GRAV_CWISE,0xFF,0x00,0x00);

	SetColor(MAP_WORM_NORMAL,0xFF,0x00,0x00);
	SetColor(MAP_WORM_OUT,	0xFF,0x00,0x00);
	SetColor(MAP_WORM_IN,	0xFF,0x00,0x00);

	SetColor(MAP_CRNT_UP,	0xFF,0x00,0x00);
	SetColor(MAP_CRNT_LT,	0xFF,0x00,0x00);
	SetColor(MAP_CRNT_RT,	0xFF,0x00,0x00);
	SetColor(MAP_CRNT_DN,	0xFF,0x00,0x00);

	SetColor(MAP_DEC_RD,	0xFF,0x77,0x00);
	SetColor(MAP_DEC_LD,	0xFF,0x77,0x00);
	SetColor(MAP_DEC_FLD,	0xFF,0x77,0x00);
	SetColor(MAP_DEC_RU,	0xFF,0x77,0x00);
	SetColor(MAP_DEC_LU,	0xFF,0x77,0x00);

	for (y=0; y<h; y++)
	{
		for (x=0; x<w; x++)
		{
			c = map->blocks[x][y].cdata;
			*i++ = r[c];
			*i++ = g[c];
			*i++ = b[c];
		}
	}

	Fl_RGB_Image* im = new Fl_RGB_Image(data, w, h);
	return(im);
}
