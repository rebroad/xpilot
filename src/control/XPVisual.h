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
#ifndef	_XPVISUAL_H_
#define	_XPVISUAL_H_

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "Obj.h"

class XPVisual : public Obj {
 public:
  XPVisual();
  XPVisual(XVisualInfo *vip);

  void PrintInfo();

 public:
  XVisualInfo visualinfo; //Xlib Visual Struct
};

#endif	// _XPVISUAL_H_
