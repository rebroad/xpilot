/*
* XPIlotpilotedit, the cross platform map editor for Xpilot.  Copyright (C) 2001 by
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
#ifndef	_XPVISUALLIST_H_
#define	_XPVISUALLIST_H_

#include "XPVisual.h"

class XPVisualList : public ObjList {
 public:
	XPVisualList();
        void PrintVisualInfo();
        int FindVisualByID(char *);

 private:
	void CreateVisuals(Display *, int);

};

#endif	// _XPVISUALLIST_H_
