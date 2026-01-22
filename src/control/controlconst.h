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
 * $Log: controlconst.h,v $
 * Revision 1.11  2004/05/10 12:48:00  dick
 * Add a Private Server button to the main panel, which allows the user to
 * manually track servers that don't appear in meta.
 *
 * Revision 1.10  2002/09/18 14:49:22  jlmiller
 * Changes necessary for Main window to be resizable, and tiled.
 *
 * Revision 1.9  2002/08/02 20:24:48  jlmiller
 * Add teamjoin buttons.
 *
 * Revision 1.8  2002/08/01 14:57:55  dick
 * Display the program name as "XPilot"
 *
 * Revision 1.7  2002/07/28 20:18:50  dick
 * Display a pretty graphic in the panel at startup
 *
 * Revision 1.6  2002/07/19 07:23:59  dick
 * Add a pane for the Local Server button.
 *
 * Revision 1.5  2002/07/15 19:13:23  jlmiller
 * Provide a (very) basic ClientConfig window.
 *
 * Revision 1.4  2002/07/14 23:04:36  jlmiller
 * Move status line to main window. Don't create ServerList's till
 * we need them.
 *
 * Revision 1.3  2002/07/12 15:00:06  jlmiller
 * Integrate tabbed main window, with clean up of globals.
 *
 * Revision 1.2  2002/07/10 04:03:19  dick
 * Add Windows icon numbers
 *
 */

#ifndef	_CONTROLCONST_H_
#define	_CONTROLCONST_H_

#define MAX_MAP_SIZE          900
//#define DEFAULT_WIDTH         400
//#define DEFAULT_HEIGHT        400
#define DEFAULT_MAP_ZOOM      10

#define MAINWIN_WIDTH		620
#define MAINWIN_HEIGHT		600

#define	STATUS_HEIGHT	20

#define XPCLIENTCONFIG_WIDTH	400
#define XPCLIENTCONFIG_HEIGHT	MAINWIN_HEIGHT-STATUS_HEIGHT-10

#define BUTTON_SIZE		20
#define	BUTTON_SIZE_LG	36
#define SCROLLBAR_SIZE	20

#define	LEFTOFS		 2
#define	TOPOFS		 2
#define	YOFS		 5		// extra space between the icons

#define	XPCONTROLBUTTON0LEFT	LEFTOFS
#define	XPCONTROLBUTTON1LEFT	XPCONTROLBUTTON0LEFT+220

#define VOIDBACK	999

#define	APPNAME "XPilot"
extern const char* szAppName;


//Command types
#define CHANGEAREA		0
#define DOACTION		1 //Shipeditor actions
#define UPDATECONTROL	500


#define SELECTGRID		0
#define BLOCKLINE		1
#define BLOCKCIRCLEEMPTY 2
#define BLOCKCIRCLEFILLED 3
#define BLOCKRECTEMPTY 4
#define BLOCKRECTFILLED 5
#define CLEARMAP		6
#define CROPMAP			7
#define ROUNDMAP		8
#define GROWMAP			9
#define ROTATEMAP		10
#define SHIFT			11
#define ROTATE			12
#define MIRROR			13

//Constants for Shifting
#define SHIFTLEFT		1
#define SHIFTRIGHT		2
#define SHIFTUP			3
#define SHIFTDOWN		4

//Constants for Mirroring
#define MIRRORH			1
#define MIRRORV			2

//COnstants for Rotating
#define CLOCKWISE			1
#define COUNTERCLOCKWISE	2

// ServerButtonsGroup buttons
#define IDM_JOINTEAM0		2000
#define IDM_JOINTEAM1		2001
#define IDM_JOINTEAM2		2002
#define IDM_JOINTEAM3		2003
#define IDM_JOINTEAM4		2004
#define IDM_JOINTEAM5		2005
#define IDM_JOINTEAM6		2006
#define IDM_JOINTEAM7		2007
#define IDM_JOINTEAM8		2008
#define IDM_JOINTEAM9		2009

#define	IDM_REFRESH_LIST	2010
#define	IDM_MEASURE_LAG		2011
#define	IDM_ADMIN_GAME		2012
#define	IDM_JOIN_GAME		2013
#define	IDM_XPWHERE_CONFIG	2014

// Main panel resources
#define	IDM_SPLASH				15000
#define IDM_CONNECT_LOCAL		15001
#define IDM_CONNECT_INTERNET	15002
#define IDM_CONNECT_LAN			15003
#define IDM_CONNECT_PRIVATE		15004
#define IDM_EDITORS				15005

// Map editor buttons
#define IDM_MAP_CAN_UNSPEC		900
#define IDM_MAP_TEAMBASE		901
#define IDM_MAP_CHECKPOINT		902

#define IDM_PEN				70
#define IDM_ERASE			71
#define IDM_LINE			72
#define IDM_SELECT			73
#define IDM_FILL			74
#define IDM_LINEFILL		75
#define IDM_SHAPE			76

#define IDM_CIRCLEEMPTY		800
#define IDM_CIRCLEFILLED	801
#define IDM_RECTEMPTY		802
#define IDM_RECTFILLED		803

//Ship Editor Specific buttons
#define IDM_SHIP_VERTEX		899
#define IDM_SHIP_MAINGUN	900
#define IDM_SHIP_ENGINE		901
#define IDM_SHIP_MISSLERACK	902
#define IDM_SHIP_LGUN		903
#define IDM_SHIP_RGUN		904
#define IDM_SHIP_LRGUN		905
#define IDM_SHIP_RRGUN		906
#define IDM_SHIP_LLIGHT		907
#define IDM_SHIP_RLIGHT		908

#define IDM_INSERT			1000
#define IDM_DELETE			1001
#define IDM_MOVE			1002
#define IDM_SPLIT			1003

#define LENGTH(x, y)		( hypot( (double) (x), (double) (y) ) )
/*
* Two macros for edge wrap of differences in position.
* If the absolute value of a difference is bigger than
* half the map size then it is wrapped.
*/
#define XPCWRAP_DX(dx)	\
        (optionsDoc->edgeWrap \
        ? ((dx) < - (width >> 1) \
                ? (dx) + width \
                : ((dx) > (width >> 1) \
                ? (dx) - width \
                : (dx))) \
        : (dx))
#define XPCWRAP_DY(dy)	\
        (optionsDoc->edgeWrap \
        ? ((dy) < - (height >> 1) \
                ? (dy) + height \
                : ((dy) > (height >> 1) \
                ? (dy) - height \
                : (dy))) \
        : (dy))

//#ifndef MAX
//#   define MIN(x, y)		( (x)>(y) ? (y) : (x) )
//#   define MAX(x, y)		( (x)>(y) ? (x) : (y) )
//#endif

// Windows Window icon numbers as used in XPilotControl.rc
#define	ICON_CONTROLWINDOW	1
#define	ICON_WHEREWINDOW	2
#define	ICON_SERVERWINDOW	3
#define	ICON_KBCONFIGWINDOW	4

#endif// _CONTROLCONST_H_
