/* $Id: math.cpp,v 1.8 2004/05/04 21:10:36 dick Exp $
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
 */
/*
 * $Log: math.cpp,v $
 * Revision 1.8  2004/05/04 21:10:36  dick
 * Need randommnt.h
 *
 * Revision 1.7  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.6  2004/01/08 18:10:50  dick
 * Update to XPilot 4.5.5beta
 *
 * Revision 1.5  2002/09/11 16:38:34  dick
 * findDir becomes FindDir. f2i() is never used?
 *
 * Revision 1.4  2001/07/27 05:47:40  dick
 * Client builds and runs in C++
 *
 */
#include "StdAfx.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#include "version.h"
#include "config.h"
#include "const.h"
#include "error.h"
#include "commonproto.h"
#include "randommt.h"
#include "cstring.h"

char math_version[] = VERSION;



DFLOAT		tbl_sin[TABLE_SIZE];
DFLOAT		tbl_cos[TABLE_SIZE];

///////////////////////////////////////////////////////////////////////////////
int mod(int x, int y)
{
    if (x >= y || x < 0)
	x = x - y*(x/y);

    if (x < 0)
	x += y;

    return x;
}

#if 0
///////////////////////////////////////////////////////////////////////////////
int f2i(DFLOAT f)
{
    return (f < 0) ? -(int)(0.5f - f) : (int)(f + 0.5f);
}
#endif

///////////////////////////////////////////////////////////////////////////////
DFLOAT FindDir(DFLOAT x, DFLOAT y)
{
    DFLOAT angle;

    if (x != 0.0 || y != 0.0)
	angle = atan2(y, x) / (2 * PI);
    else
	angle = 0.0;

    if (angle < 0)
	angle++;
    return angle * RES;
}


///////////////////////////////////////////////////////////////////////////////
double rfrac(void)
{
    /*
     * Return a pseudo-random value in the range { 0.0 <= x < 1.0 }.
     * Use randomMT() which returns a 32 bit PRN and multiply by 1/(1<<32).
     */
    return ((double) randomMT() * 0.00000000023283064365386962890625);
}


///////////////////////////////////////////////////////////////////////////////
void Make_table(void)
{
    int i;

    for (i = 0; i < TABLE_SIZE; i++) {
	tbl_sin[i] = sin(i * (2.0 * PI / TABLE_SIZE));
	tbl_cos[i] = cos(i * (2.0 * PI / TABLE_SIZE));
    }
}

