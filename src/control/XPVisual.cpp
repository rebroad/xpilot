/*
 * Xpilotedit, the cross platform map editor for Xpilot.  Copyright (C) 2001 by
 *
 *      The Xpilot Authors           <xpilot@xpilot.org>
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
 * $Log: XPVisual.cpp,v $
 * Revision 1.2  2004/02/26 06:28:39  dick
 * Wrap the debug printf()s with D()
 *
 */

#include "config.h"
#include "XPVisual.h"

///////////////////////////////////////////////////////////////////////////////
XPVisual::XPVisual()
{
}
///////////////////////////////////////////////////////////////////////////////
XPVisual::XPVisual(XVisualInfo *vip)
{
  visualinfo = *vip;
}
///////////////////////////////////////////////////////////////////////////////
void XPVisual::PrintInfo()
{
    char errorbuf[40];			/* for sprintfing into */

    char *c_class = NULL;			/* for printing */

    switch (visualinfo.c_class) {
      case StaticGray:    c_class = "StaticGray"; break;
      case GrayScale:    c_class = "GrayScale"; break;
      case StaticColor:    c_class = "StaticColor"; break;
      case PseudoColor:    c_class = "PseudoColor"; break;
      case TrueColor:    c_class = "TrueColor"; break;
      case DirectColor:    c_class = "DirectColor"; break;
      default:
	sprintf (errorbuf, "unknown class %d", visualinfo.c_class);
	c_class = errorbuf;
	break;
    }

	D(
		printf ("  visual:\n");
		printf ("    visual id:    0x%lx\n", visualinfo.visualid);
		printf ("    class:    %s\n", c_class);
		printf ("    depth:    %d plane%s\n", visualinfo.depth,
			visualinfo.depth == 1 ? "" : "s");
		printf ("    size of colormap:    %d entries\n", visualinfo.colormap_size);
		printf ("    red, green, blue masks:    0x%lx, 0x%lx, 0x%lx\n",
			visualinfo.red_mask, visualinfo.green_mask, visualinfo.blue_mask);
		printf ("    significant bits in color specification:    %d bits\n",
			visualinfo.bits_per_rgb);
	)
    return;
}
