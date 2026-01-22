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
 * $Log: XPVisualList.cpp,v $
 * Revision 1.2  2004/03/08 02:13:39  dick
 * Wrap some debug with a D()
 *
 * Revision 1.1  2004/02/26 04:07:02  jlmiller
 *
 * First pass at adding visual list. List is built, but nothing done with it yet.
 *
 */
#include <X11/Xlib.h>
#include "XPVisualList.h"

#include "config.h"
#include "proto.h"

///////////////////////////////////////////////////////////////////////////////
XPVisualList::XPVisualList()
{
    Display *dpy;			/* X connection */
    char *displayname = NULL;		/* server to contact */
    int i;				/* temp variable:  iterator */
    Bool multibuf = False;
    int mbuf_event_base, mbuf_error_base;

    //    ProgramName = "XPilotControl";

    //    dpy = XOpenDisplay (displayname);
    dpy = XOpenDisplay("");
    if (!dpy) {
      ErrorHandler("Unable to open display. Can't create visual list.\n");
      //	fprintf (stderr, "%s:  unable to open display.\n", ProgramName);

      //TODO:
      //What do we want to return here if we can't open the display? NULL maybe.
      return;
      //	exit (1);
    }



    for (i = 0; i < ScreenCount (dpy); i++) {
	CreateVisuals(dpy, i);
    }

    XCloseDisplay (dpy);

}
void XPVisualList::CreateVisuals(Display *dpy, int scr)
{
    Screen *s = ScreenOfDisplay (dpy, scr);  /* opaque structure */
    XVisualInfo viproto;		/* fill in for getting info */
    XVisualInfo *vip;			/* retured info */
    int nvi;				/* number of elements returned */
    int i;				/* temp variable: iterator */
    XPVisual *xpv = NULL;

    nvi = 0;
    viproto.screen = scr;
    vip = XGetVisualInfo (dpy, VisualScreenMask, &viproto, &nvi);
    //    printf ("  number of visuals:    %d\n", nvi);
    //    printf ("  default visual id:  0x%lx\n",
    //	    XVisualIDFromVisual (DefaultVisual (dpy, scr)));
    for (i = 0; i < nvi; i++) {
      //	PrintVisualInfo (vip+i);
        xpv = new XPVisual(vip+i);
        Add(xpv);
    }
    if (vip) XFree ((char *) vip);

    return;
}

void XPVisualList::PrintVisualInfo()
{
  int i, numvisuals = GetCount();
  XPVisual *tmpvis = NULL;

  tmpvis = (XPVisual *) GetHead();

  if (tmpvis == NULL)
    return;

  D(printf("Number of Visuals: %d\n", numvisuals);)
  for (i = 0; i < numvisuals; i++)
  {
    tmpvis->PrintInfo();
    tmpvis = (XPVisual *) tmpvis->GetNext();
  }

  /*  char errorbuf[40];			// for sprintfing into

  char *c_class = NULL;			// for printing

    switch (vip->c_class) {
      case StaticGray:    c_class = "StaticGray"; break;
      case GrayScale:    c_class = "GrayScale"; break;
      case StaticColor:    c_class = "StaticColor"; break;
      case PseudoColor:    c_class = "PseudoColor"; break;
      case TrueColor:    c_class = "TrueColor"; break;
      case DirectColor:    c_class = "DirectColor"; break;
      default:
	sprintf (errorbuf, "unknown class %d", vip->c_class);
	c_class = errorbuf;
	break;
    }

    printf ("  visual:\n");
    printf ("    visual id:    0x%lx\n", vip->visualid);
    printf ("    class:    %s\n", c_class);
    printf ("    depth:    %d plane%s\n", vip->depth,
	    vip->depth == 1 ? "" : "s");
    printf ("    size of colormap:    %d entries\n", vip->colormap_size);
    printf ("    red, green, blue masks:    0x%lx, 0x%lx, 0x%lx\n",
	    vip->red_mask, vip->green_mask, vip->blue_mask);
    printf ("    significant bits in color specification:    %d bits\n",
	    vip->bits_per_rgb);

	    return;*/

}

