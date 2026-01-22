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
#ifndef	_GUICONST_H_
#define	_GUICONST_H_

#define XPPRESETCONFIG_WIDTH	500
#define XPPRESETCONFIG_HEIGHT	400

//Widget Sizes
#define BUTTON_SIZE		20
#define SCROLLBAR_SIZE	20
#define SPM_SIZE	52
#define SHIPLIST_SIZE SPM_SIZE+SCROLLBAR_SIZE
#define CC_INPUTWIDTH 120
#define CC_INPUTWIDTHSMALL 35
#define CC_INPUTHEIGHT 20
#define CC_OPTIONSIZE 24

//Events
#define SELECTSHIP			356 //An arbitrary number? These must not conflict with FLTK event numbers
#define SELECTSERVER		357
#define	SERVERLISTCHANGED	358
#define SERVERJOINGAME		359

#endif// _GUICONST_H_
