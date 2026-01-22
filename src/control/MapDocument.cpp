/* $Id: MapDocument.cpp,v 1.48 2005/03/17 22:12:13 kps Exp $
 *
 * MapDocument - Define a map.
 *
 * XPilot5 - Manage all local XPilot activity.
 *
 *      Copyright (C) 2001 by
 *
 *      The XPilot Authors           <xpilot@xpilot.org>
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
 * $Log: MapDocument.cpp,v $
 * Revision 1.48  2005/03/17 22:12:13  kps
 * Get rid of warnings from makedepend about "non-portable whitespace".
 *
 * Revision 1.47  2004/05/30 16:11:38  dick
 * Use randommt() instead of rand()
 *
 * Revision 1.46  2004/03/03 07:54:35  dick
 * Don't make pixmaps at load time; wait until runtime init, after Strings have
 * actually been inited.
 *
 * Revision 1.45  2004/02/21 06:57:32  dick
 * In the mapeditor, the prefs should have no knowledge of a ServerOptionsNetClient.
 *
 * Revision 1.44  2004/02/03 05:32:34  cppc
 * Cast argument to disambiguate the sqrt call under .NET
 *
 * Revision 1.43  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.42  2004/01/20 04:05:45  jlmiller
 * Provide "ResetMap", so we can clear the map and start over fresh easily.
 *
 * Revision 1.41  2004/01/19 06:55:49  dick
 * Don't try to test open the file before calling LoadOptions, because it may
 * be a pseudo-filename ("wild 123").
 *
 * Revision 1.40  2004/01/17 19:37:17  dick
 * WildMap is now in the common library. Control and server use this,
 * instead of each having it's own copy.
 *
 * Revision 1.39  2004/01/13 21:02:43  jlmiller
 * Make mapeditor properly prompt on exit.
 *
 * Revision 1.38  2004/01/06 04:48:28  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 * Revision 1.37  2002/09/10 01:22:45  jlmiller
 * Added 'Closing Confirm' dialog boxes to mapeditor and shipeditor.
 *
 * Revision 1.36  2002/09/10 01:20:18  dick
 * Include stdlib.h
 *
 * Revision 1.35  2002/08/20 10:47:29  jlmiller
 * File & Class Renamings:
 * ViewWindow -> MapEditWindow
 * XpInput -> XPInput
 * xpeTypes -> xpcTypes
 *
 * Revision 1.34  2002/07/30 21:58:43  jlmiller
 *
 * must include <string.h>
 *
 * Revision 1.33  2002/07/30 17:23:47  jlmiller
 * Overlay functions become member functions of the drawing windows.
 * Clipboard is its own class.
 *
 * Revision 1.32  2002/07/24 18:31:35  jlmiller
 * Update button structs. Use large size buttons for map block toggles. Much
 * easier to see.
 *
 * Revision 1.31  2002/07/23 13:04:34  jlmiller
 * Handle user canceling fl_file_chooser.
 *
 * Revision 1.30  2002/07/23 12:27:37  jlmiller
 * Activate image saving routines, which were present but not available before.
 *
 * Revision 1.29  2002/07/20 17:45:56  jlmiller
 * FillArea is undoable.
 *
 * Revision 1.28  2002/07/12 15:00:06  jlmiller
 * Integrate tabbed main window, with clean up of globals.
 *
 * Revision 1.27  2002/06/26 13:54:35  jlmiller
 * Fix Quick Cannon tool.
 *
 * Revision 1.26  2002/06/25 23:46:13  jlmiller
 *
 * Linux cleanups.
 *
 * Revision 1.25  2002/06/25 20:20:09  jlmiller
 * Massive restructure to clean up includes, and objectify code more.
 *
 * Revision 1.24  2002/06/25 15:02:19  jlmiller
 * Move iSelection fields to the window, instead of the individual document,
 * as they didn't make any sense at document level.
 *
 * Revision 1.23  2002/06/23 06:27:32  dick
 * Each application that uses a ServerOptions must include the init.cpp.h
 * and provide a ServerOptionFactory
 *
 * Revision 1.22  2002/06/08 20:45:43  jlmiller
 * Copy, Paste & Drag use undoMan
 *
 * Revision 1.21  2002/06/07 23:10:08  jlmiller
 * Encapsulate almost all map functions into undoMan.
 *
 * Revision 1.20  2002/06/07 20:49:00  jlmiller
 * Begin handling undoMan properly...most functions still broken, expecially RotateMap.
 *
 * Revision 1.19  2002/06/02 22:44:05  jlmiller
 * Add undoMan to shipeditor.
 * Commentability.
 *
 * Revision 1.18  2002/05/29 20:31:27  jlmiller
 * ServerOptions should all be defined in its proper location.
 *
 * Revision 1.17  2002/05/28 19:29:03  jlmiller
 * lastTeamBase shouldn't be global. Hide the server tab from the preferences
 * while in the mapeditor, since it doesnt work through there anyway.
 *
 * Revision 1.16  2002/05/28 07:19:50  dick
 * Rename xpilotedit to XPilotControl
 *
 */
#include "StdAfx.h"
#if defined(_UNIX) || defined(_CYGWIN)
#	include <stdlib.h>
#endif

#include <string.h>
#include <ctype.h>
#include <math.h>

#include "randommt.h"
#include "MapDocument.h"
#include "MapBlockChange.h"
#include "xpcTypes.h"
#include "controlconst.h"
#include "map.h"
#include "proto.h"
#include "XpPointList.h"
#include "Clipboard.h"
#include "WildMap.h"

map_data_t	clipdata;
/***************************************************************************/
/* char EmptyTempData                                                      */
/* Arguments :                                                             */
/*   none                                                                  */
/*                                                                         */
/* Purpose :  Clean out the temporary array used for rotating, mirroring.. */
/* shifting, etc.                                                          */
/***************************************************************************/
void EmptyTempData()
{
        int x,y;

        for(x = 0; x < MAX_MAP_SIZE; x++)
                for(y = 0; y < MAX_MAP_SIZE; y++)
                        clipdata[x][y] = MAP_SPACE;
}

///////////////////////////////////////////////////////////////////////////////
MapDocument::MapDocument()
{
    optionsDoc = new ServerOptions;
    fSelected = false;
	changed = false;

	seldxbeg = seldybeg = seldxend = seldyend = 0;
	strdx = strdy = 0;
	ptTempBeg.x = ptTempEnd.x = ptTempBeg.y = ptTempEnd.y = 0;;
	CleanMap();
}

///////////////////////////////////////////////////////////////////////////////
MapDocument::  ~MapDocument()
{
    delete optionsDoc;
}

///////////////////////////////////////////////////////////////////////////////
void MapDocument::ResetMap()
{
    delete optionsDoc;
    optionsDoc = new ServerOptions;
    fSelected = false;
	changed = false;

	seldxbeg = seldybeg = seldxend = seldyend = 0;
	strdx = strdy = 0;
	ptTempBeg.x = ptTempEnd.x = ptTempBeg.y = ptTempEnd.y = 0;;
	CleanMap();
}

///////////////////////////////////////////////////////////////////////////////
bool MapDocument::SaveMap(const char *pstrFileName)
{
	optionsDoc->SaveOptions((char *) pstrFileName, true, OPT_MAP);

	return false;
}

///////////////////////////////////////////////////////////////////////////////
bool MapDocument::LoadMap(const char *pstrFileName)
{

	int                   corrupted=0;
	char                  *tmpstr;


	tmpstr = strrchr(pstrFileName,(int) '.');
	if (tmpstr != NULL) {
		if (strcmp(tmpstr, ".xbm") == 0) {
			return LoadXbmFile(pstrFileName);
		}
		if (strcmp(tmpstr, ".pbm") == 0) {
			return LoadPbmFile(pstrFileName);
		}
	}

	if (!optionsDoc->LoadOptions((char *) pstrFileName, OPT_MAP, true))
		return(false);
	CountBases();
	CountCheckPoints();
	return true;
}

///////////////////////////////////////////////////////////////////////////////
void MapDocument::CountBases()
{
	int x,y, i, team, count = mapItems.bases.GetCount();
	bool update  = false;
	int width = optionsDoc->mapWidth->GetInt();
	int height = optionsDoc->mapHeight->GetInt();
	XpTeamBase *tmpBase;

	if (count > 0)
	{
		tmpBase = (XpTeamBase *) mapItems.bases.GetHead();
		for (i = 0; i < count; i++)
		{
			mapItems.bases.Remove(tmpBase);
			delete(tmpBase);
			tmpBase = (XpTeamBase *) mapItems.bases.GetHead();
		}
	}

	count = 0;

	//count the number of items
	for (x = 0; x < width; x++)
		for (y = 0; y < height; y++)
		{
			switch (MapData(x, y))
			{
			case '_':
				team = 0;
				update = true;
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				team = MapData(x, y) - '0';
				update = true;
				break;
			}
			//reallocate the memory for the new number of items
			if (update)
			{
				tmpBase = new XpTeamBase();
				tmpBase->x = x;
				tmpBase->y = y;
				tmpBase->team = team;

				mapItems.bases.Add(tmpBase);
			}

			update = false;
		}
}

///////////////////////////////////////////////////////////////////////////////
void MapDocument::CountCheckPoints()
{
	int x,y;
	char tmp;

	mapItems.NumChecks = 0;


	for (x = 0; x <= 25; x++)
		mapItems.checkpoints[x] = 0;

	for (x = 0; x < optionsDoc->mapWidth->GetInt(); x++)
	{
		for (y = 0; y < optionsDoc->mapHeight->GetInt(); y++)
		{
			tmp = MapData(x, y);
			if ((tmp >= 'A') &&
				(tmp <= 'Z'))
			{
				mapItems.checkpoints[tmp - 65]++;
				mapItems.NumChecks++;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
char MapDocument::FindNextCheckPoint()
{
	int x, next;

	next = 0;

	for (x = 1; x <= 25;x++)
		if (mapItems.checkpoints[x] < mapItems.checkpoints[x-1])
			next = x;

	return (char)(next+65);
}

///////////////////////////////////////////////////////////////////////////////
ObjList * MapDocument::FillArea(int x, int y, int selected)
{
	char icon, old, ch;
	short temp_x, temp_y, hold_x, hold_y;
	int i, j;
	int width = optionsDoc->mapWidth->GetInt();
	int height = optionsDoc->mapHeight->GetInt();
	MapBlockChange *chg = NULL;
	ObjList *changeList = new ObjList();
	XpPointList backList;
	map_data_t tempMap;

	if ((selected >= '0') && (selected <= '9'))
	{
		icon = selected;
	}
	switch (selected)
	{
	case IDM_MAP_CAN_UNSPEC:
		if (MapData(x, y) == MAP_SPACE)
		{
			ch = MapData(x, y-1);
			if((ch == MAP_FILLED) || (ch == MAP_REC_LD) || (ch == MAP_REC_RD))
				icon = MAP_CAN_DOWN;
			else
			{
				ch = MapData(x+1, y);
				if((ch == MAP_FILLED) || (ch == MAP_REC_LD) || (ch == MAP_REC_LU))
					icon = MAP_CAN_LEFT;
				else
				{
					ch = MapData(x, y+1);
					if((ch == MAP_FILLED) || (ch == MAP_REC_RU) || (ch == MAP_REC_LU))
						icon = MAP_CAN_UP;
					else
					{
						ch = MapData(x-1, y);
						if((ch == MAP_FILLED) || (ch == MAP_REC_RD) || (ch == MAP_REC_RU))
							icon = MAP_CAN_RIGHT;
						else
							return NULL;
					}
				}
			}
		}
		else
			return NULL;
		break;
	case IDM_MAP_TEAMBASE:
		icon = selected;
		break;
	case IDM_MAP_CHECKPOINT:
		icon = MAP_SPACE;
		break;
	default:
		icon = selected;
		break;

	}


	//Store the current block and coords
	old = MapData(x, y);
	if (icon == old)
        return NULL;


	//set up a temporary map array, so we dont lose track of where we are
	//later.
	for (i = 0; i < MAX_MAP_SIZE; i++)
		for (j = 0; j < MAX_MAP_SIZE; j++)
			tempMap[i][j] = MapData(i, j);

	chg = new MapBlockChange();
	chg->x = x;
	chg->y = y;
	chg->newchar = icon;
	chg->oldchar = MapData(x, y);
	changeList->Add(chg);

	temp_x = x;
	temp_y = y;

	while(true)
	{
		if( (temp_x < width - 1) && (tempMap[temp_x+1][temp_y] == old) )
		{

			chg = new MapBlockChange();
			chg->x = temp_x+1;
			chg->y = temp_y;
			chg->newchar = icon;
			chg->oldchar = tempMap[temp_x+1][temp_y];
			changeList->Add(chg);
			tempMap[temp_x+1][temp_y] = icon;
			backList.Add(new XpPoint(temp_x, temp_y));
			temp_x++;
			continue;
		}
		if( (temp_x > 0) && (tempMap[temp_x-1][temp_y] == old) )
		{
			chg = new MapBlockChange();
			chg->x = temp_x-1;
			chg->y = temp_y;
			chg->newchar = icon;
			chg->oldchar = tempMap[temp_x-1][temp_y];
			changeList->Add(chg);
			tempMap[temp_x-1][temp_y] = icon;
			backList.Add(new XpPoint(temp_x, temp_y));
			temp_x--;
			continue;
		}
		if( (temp_y < height - 1) && (tempMap[temp_x][temp_y+1] == old) )
		{
			chg = new MapBlockChange();
			chg->x = temp_x;
			chg->y = temp_y+1;
			chg->newchar = icon;
			chg->oldchar = tempMap[temp_x][temp_y+1];
			changeList->Add(chg);
			tempMap[temp_x][temp_y+1] = icon;
			backList.Add(new XpPoint(temp_x, temp_y));
			temp_y++;
			continue;
		}
		if( (temp_y > 0) && (tempMap[temp_x][temp_y-1] == old) )
		{
			chg = new MapBlockChange();
			chg->x = temp_x;
			chg->y = temp_y-1;
			chg->newchar = icon;
			chg->oldchar = tempMap[temp_x][temp_y-1];
			changeList->Add(chg);
			tempMap[temp_x][temp_y-1] = icon;
			backList.Add(new XpPoint(temp_x, temp_y));
			temp_y--;
			continue;
		}

		if( (temp_x == width - 1 ) && (tempMap[0][temp_y] == old) && optionsDoc->edgeWrap )
		{
			chg = new MapBlockChange();
			chg->x = 0;
			chg->y = temp_y;
			chg->newchar = icon;
			chg->oldchar = tempMap[0][temp_y];
			changeList->Add(chg);
			tempMap[0][temp_y] = icon;
			backList.Add(new XpPoint(temp_x, temp_y));
			temp_x = 0;
			continue;
		}
		if( (temp_x == 0) && (tempMap[width-1][temp_y] == old) && optionsDoc->edgeWrap )
		{
			chg = new MapBlockChange();
			chg->x = width-1;
			chg->y = temp_y;
			chg->newchar = icon;
			chg->oldchar = tempMap[width-1][temp_y];
			changeList->Add(chg);
			tempMap[width-1][temp_y] = icon;
			backList.Add(new XpPoint(temp_x, temp_y));
			temp_x = width-1;
			continue;
		}
		if( (temp_y == height - 1) && (tempMap[temp_x][0] == old) && optionsDoc->edgeWrap )
		{
			chg = new MapBlockChange();
			chg->x = temp_x;
			chg->y = 0;
			chg->newchar = icon;
			chg->oldchar = tempMap[temp_x][0];
			changeList->Add(chg);
			tempMap[temp_x][0] = icon;
			backList.Add(new XpPoint(temp_x, temp_y));
			temp_y = 0;
			continue;
		}
		if( (temp_y == 0) && (tempMap[temp_x][height-1] == old) && optionsDoc->edgeWrap )
		{
			chg = new MapBlockChange();
			chg->x = temp_x;
			chg->y = height-1;
			chg->newchar = icon;
			chg->oldchar = tempMap[temp_x][height-1];
			changeList->Add(chg);
			tempMap[temp_x][height-1] = icon;
			backList.Add(new XpPoint(temp_x, temp_y));
			temp_y = height-1;
			continue;
		}


		XpPoint *tmp = (XpPoint *) backList.GetTail();
		hold_x = tmp->x;
		hold_y = tmp->y;
		backList.Remove(tmp);
		delete(tmp);
		temp_x = hold_x;
		temp_y = hold_y;
		if (backList.GetHead() == NULL)
			break;
	}
	//End of filling loop!!

	return changeList;
}

///////////////////////////////////////////////////////////////////////////////
char MapDocument::FindQuickCanDir(int x, int y)
{
	char icon, ch;
	if (MapData(x, y) == MAP_SPACE)
	{
		ch = MapData(x, y-1);
		if((ch == MAP_FILLED) || (ch == MAP_REC_LD) || (ch == MAP_REC_RD))
		{
			icon = MAP_CAN_DOWN;
		}
		else
		{
			ch = MapData(x+1, y);
			if((ch == MAP_FILLED) || (ch == MAP_REC_LD) || (ch == MAP_REC_LU))
			{
				icon = MAP_CAN_LEFT;
			}
			else
			{
				ch = MapData(x, y+1);
				if((ch == MAP_FILLED) || (ch == MAP_REC_RU) || (ch == MAP_REC_LU))
				{
					icon = MAP_CAN_UP;
				}
				else
				{
					ch = MapData(x-1, y);
					if((ch == MAP_FILLED) || (ch == MAP_REC_RD) || (ch == MAP_REC_RU))
					{
						icon = MAP_CAN_RIGHT;
					}
					else
						return MAP_SPACE;
				}
			}
		}
		return icon;
	}
	else //If we're not a space, return the original block
		return MapData(x, y);
}

/***************************************************************************/
/* SelectGrid                                                              */
/* Arguments :                                                             */
/*   x: xcoord                                                             */
/*   y: ycoord                                                             */
/*   selected: current selected block                                      */
/*   count: should we count bases?                                         */
/*                                                                         */
/* Purpose : Translates current selected tool to mapicon, calls change to  */
/* mapdata, draws icon                                                     */
/***************************************************************************/
void MapDocument::SelectGrid(int x, int y, int selected, bool count)
{
	char icon, old;

	if (!InsideMap(x, y))
		return;

	if  ((selected >= 'A') && (selected <= 'Z'))
	{
		mapItems.checkpoints[toupper(selected)-65]++;
		icon = toupper((char)(selected));
	}
	else
		icon = selected;


	old = MapData(x, y);

	ChangeMapData(x,y,icon);


	if  ( (((old >= '0') && (old <= '9')) || old == MAP_BASE) && count)
	{
		CountBases();
	}

	if  ((old >= 'A') && (old <= 'Z'))
	{
		CountCheckPoints();
	}

}

///////////////////////////////////////////////////////////////////////////////
void MapDocument::ChangeMapData(int x, int y, char icon)
{
	if ( !InsideMap(x, y) )
		return;


	if (MapData(x, y) == icon)
		return;

	optionsDoc->mapData->blocks[x][y].cdata = icon;

}

///////////////////////////////////////////////////////////////////////////////
void MapDocument::ChangeMapBackData(int x, int y, int mbackx, int mbacky)
{
	if ((x < 0) || (y < 0) || (x >= MAX_MAP_SIZE) || (y >= MAX_MAP_SIZE))
		return;

	optionsDoc->mapData->blocks[x][y].backx = mbackx;
	optionsDoc->mapData->blocks[x][y].backy = mbacky;
}

///////////////////////////////////////////////////////////////////////////////
void MapDocument::SwapMapSize()
{
	int temp2;

	temp2 = optionsDoc->mapHeight->GetInt();
	*optionsDoc->mapHeight = optionsDoc->mapWidth->GetInt();
	*optionsDoc->mapWidth = temp2;

}

///////////////////////////////////////////////////////////////////////////////
void MapDocument::SortSelectionArea()
{
	int temp;

	if (seldxbeg > seldxend)
	{
		temp = seldxbeg;
		seldxbeg = seldxend;
		seldxend = temp;
	}
	if (seldybeg > seldyend)
	{
		temp = seldybeg;
		seldybeg = seldyend;
		seldyend = temp;
	}
}

///////////////////////////////////////////////////////////////////////////////
void MapDocument::CleanMap()
//This function should only be called privately. Its a faster version
//Of the ClearMap function, that ignores undoMan and clear's the map data directly,
//which is dangerous.
{
	int                   i,j;
	int width = optionsDoc->mapWidth->GetInt();
	int height = optionsDoc->mapHeight->GetInt();




	for (i = 0; i < width; i++)
	{
		for (j = 0; j <height; j++)
		{
			ChangeMapData(i,j,MAP_SPACE);
			optionsDoc->mapData->blocks[i][j].backx = VOIDBACK;
			optionsDoc->mapData->blocks[i][j].backy = VOIDBACK;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
char MapDocument::MapData(int x, int y)
//return the character stored at the specified location.
{
	int width = optionsDoc->mapWidth->GetInt();
	int height = optionsDoc->mapHeight->GetInt();
	if (optionsDoc->edgeWrap->GetBool())
	{
		if ( x<0 ) {
			x = width + x ;
		} else if (x>=width) {
			x = x - width;
		}
		if ( y<0 ) {
			y = height + y;
		} else if (y>=height) {
			y = y - height;
		}
	}
	else
	{
		if ( x<0 ) {
			x = 0;
		} else if (x>width) {
			x = width;
		}
		if ( y<0 ) {
			y = 0;
		} else if (y>height) {
			y = height;
		}
	}

	return optionsDoc->mapData->blocks[x][y].cdata;
}

///////////////////////////////////////////////////////////////////////////////
bool MapDocument::InsideMap(int x, int y)
{
	if ( (x < 0) || (x >= optionsDoc->mapWidth->GetInt())
		|| (y < 0) || (y >= optionsDoc->mapHeight->GetInt()) )
		return false;

	return true;
}

/* Public variables
* ================
*/
//int	xsize, ysize;
int	invert = 0;

/******************************************************************/
/* -USED IN LoadPbmFile-                                          */

/* The man-page for pbm says no line can exceed 70 chars (except, of
course, the pixel-data in bit-packed mode, but we don't buffer
that through 'buf' */
#define PBMSIZE 70
#define WHITESPACE " \t\r\n"

///////////////////////////////////////////////////////////////////////////////
bool MapDocument::LoadXbmFile(const char *file)
{
	FILE                  *fp;
	max_str_t             line;
	char				  *tmp;
	int                   bits,x=0,y=0;

	if ((fp = fopen(file, "r")) == NULL) {
		ErrorHandler("File Opening Error: %s", file);
		return false;
	}
	fgets(line, sizeof(max_str_t), fp);
	tmp = strrchr(line, (int) MAP_SPACE);
	if (tmp == NULL) return false;
	tmp++;
	//        width = atoi(tmp);
	*optionsDoc->mapWidth = atoi(tmp);

	fgets(line, sizeof(max_str_t), fp);
	tmp = strrchr(line, (int) MAP_SPACE);
	if (tmp == NULL) return false;
	tmp++;
	//        height = atoi(tmp);
	*optionsDoc->mapHeight = atoi(tmp);

	while ( (fgets(line, sizeof(max_str_t), fp)) != 0 ) {
		tmp = strstr(line,"0x");
		while ( tmp != NULL) {
			tmp += 2;
			if ((int) tmp[0] > 96) {
				bits = ((int)(tmp[0])-87)*16;
			} else {
				bits = ((int)(tmp[0])-48)*16;
			}
			if ((int) tmp[1] > 96) {
				bits += (int)(tmp[1])-87;
			} else {
				bits += (int)(tmp[1])-48;
			}
			if ( (bits & 128) == 128) ChangeMapData(x+7, y, MAP_FILLED);
			if ( (bits & 64) == 64) ChangeMapData(x+6, y, MAP_FILLED);
			if ( (bits & 32) == 32) ChangeMapData(x+5, y, MAP_FILLED);
			if ( (bits & 16) == 16) ChangeMapData(x+4, y, MAP_FILLED);
			if ( (bits & 8) == 8) ChangeMapData(x+3, y, MAP_FILLED);
			if ( (bits & 4) == 4) ChangeMapData(x+2, y, MAP_FILLED);
			if ( (bits & 2) == 2) ChangeMapData(x+1, y, MAP_FILLED);
			if ( (bits & 1) == 1) ChangeMapData(x, y, MAP_FILLED);
			x += 8;
			if ( x>= optionsDoc->mapWidth->GetInt()) {
				y++;
				x=0;
			}
			tmp = strstr(tmp,"0x");
		}
	}
	fclose(fp);
	return true;
}

#define NEXT_TOKEN()							       \
	state++;								       \
	if (!(p = strtok(NULL, " \t\r\n")))					       \
break;

///////////////////////////////////////////////////////////////////////////////
bool MapDocument::LoadPbmFile(const char *file)
{
	FILE                  *fp;
	int             state = 0, width, height, count;
	char            buf[PBMSIZE];
	char *p;
	int x, y;
	int bit_packed;
	int c, mask;


	if ((fp = fopen(file, "r")) == NULL) {
		ErrorHandler("File Opening Error: %s", file);
		return false;
	}

	while (fgets(buf, PBMSIZE, fp))
	{
	//Comments can only start at beginning of line.  I'm not sure
	//if this is an accurate interpretation of the pbm spec.
		p = strchr(buf, MAP_FUEL);
		if (p)
			*p = 0;

        //Blank lines are ignored
		if (!(p = strtok(buf, WHITESPACE)))
			continue;

			//Each case is responsible for getting the next token before
			//iterating thru the loop again, except in bit-packed mode,
			//there can only be one whitespace char between height and the
			//pixel data.  When all tokens are exhausted, we fall through
			//to the outer loop to get another line, except in bit-packed
			//mode which processes all remaining input and does not return.


		while (p)
		{

			switch (state)
			{
			case 0:
				bit_packed = !strcmp(p, "P4");

				if (strcmp(p, "P1") && !bit_packed)
				{
					ErrorHandler("Unrecognized PBM format!");
				}
				NEXT_TOKEN();
			case 1:
				width = atoi(p);
				NEXT_TOKEN();
			case 2:
				height = atoi(p);
				*optionsDoc->mapWidth = width;
				*optionsDoc->mapHeight = height;
				count = 0;
				x = 0;
				y = 0;

				if (bit_packed)
					state++;
				else
				{
					NEXT_TOKEN();
				}
			default:
				if (bit_packed)
				{
				//In bit-packed mode, there are no line-breaks,
				//so all remaining chars in the input stream
				//need to be read by process_bit_packed() and
				//then we exit.
					y=0;

					while ((c = fgetc(fp)) != EOF)
					{
						for (mask = 128 ; mask > 0 ; mask = mask >> 1)
						{
							if ((c & mask) == mask)
								ChangeMapData(x, y, MAP_FILLED);
							else
								ChangeMapData(x, y, MAP_SPACE);
							count++;
							x++;
							if (!(count % width))
							{
							//end of row, skip over remaining bits
							//to next char
								y++;
								x = 0;
								break;
							}
						}
					}
					fclose(fp);
					return true;
				}
				else
				{
				//ASCII mode processes a single token and then
				//iterates through the token loop again

					while (*p)
					{
					//Break output into 'width' wide rows, which is not
					//necessarily at the end of a line of input
						if (!(count % width))
							y++;
						x = 0;
						if (*p == '1')
						{
							ChangeMapData(x, y, MAP_FILLED);
							count++;
							x++;
						}
						else if (*p == '0')
						{
							ChangeMapData(x, y, MAP_SPACE);
							count++;
							x++;
						}
						p++;
					}


					NEXT_TOKEN();
				}
			}
		}

	}

	fclose(fp);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
bool MapDocument::SaveXbmFile(const char *pstrFileName)
{
	FILE	*ofile;
	char		*image;
	int		x, y;
	int		i;
	int		value;
	char		*p;
	int	count, Xsize;
	int xsize, ysize;


	xsize = optionsDoc->mapWidth->GetInt();

	ysize = optionsDoc->mapHeight->GetInt();

	image = (char *) malloc(xsize * ysize);
	memset(image, invert, xsize * ysize);

	//Build the image
	build_image(image, xsize);

	if (NULL == (ofile = fopen (pstrFileName, "wb")))
	{
		ErrorHandler("Couldn't open the destination file: %s", pstrFileName);
		return false ;
	}


	//Output the image.  The output image is in xbm format.
	count = 0;
	Xsize = (xsize) & 0xfff8;

	fprintf(ofile, "#define maps_width %d\n#define maps_height %d\n", Xsize, ysize);
	fprintf(ofile, "static unsigned char maps_bits[] = {\n   ");
	p = image;
	value = 0;
	i = 0x80;
	for (y = 0; y < ysize; y++)
	{
		for (x = 0; x < Xsize; x++)
		{
			if (*p)
				value |= 0x80;
			i >>= 1;
			if (i == 0)
			{
				fprintf(ofile, "0x%02x, ", value & 0xff);
				count++;
				if (count >= 12)
				{
					count = 0;
					fprintf(ofile, "\n   ");
				}
				i = 0x80;
				value = 0;
			}
			value >>= 1;
			p++;
		}
		if (i != 0x80)
		{
			fprintf(ofile, "0x%02x, ", value & 0xff);
			count++;
			if (count >= 12)
			{
				count = 0;
				fprintf(ofile, "\n   ");
			}
			i = 0x80;
			value = 0;
		}
#if 0
		value = 0;
#endif
		p -= Xsize - xsize;
	}
	fprintf(ofile, "\n};\n");

	fclose(ofile);
	return false;
}



/***************************************************************************/
/* SavePbmPlusFile                                                         */
/* Arguments :                                                             */
/*   file                                                                  */
/* Purpose : Save as a pbmplus file                                        */
/* Some code ported from maps2image.c by Andrew W. Scherpbier              */
/***************************************************************************/
bool MapDocument::SavePbmPlusFile(const char *pstrFileName, int PBMPLUSTYPE)
{
	FILE	*ofile;
	char		*image;
	int x, y, numcolors;
	int		i;
	int		value;
	char		*p;
	int width = optionsDoc->mapWidth->GetInt();
	int height = optionsDoc->mapHeight->GetInt();
	int xsize = width;
	int ysize = height;

	numcolors = 255;

	image = (char *) malloc(xsize * ysize);
	memset(image, invert, xsize * ysize);

	//Build the image
	build_image(image, xsize);

	if (NULL == (ofile = fopen (pstrFileName, "wb")))
	{
		ErrorHandler("Couldn't open the destination file: %s", pstrFileName);
		return false;
	}


	//Output the image.  The output image can be either pbm ppm or pgm
	switch (PBMPLUSTYPE)
	{
	case 0: //PORTABLE BITMAP
		fprintf(ofile, "P4\n%d %d\n", xsize, ysize);
		p = image;
		for (y = 0; y < ysize; y++)
		{
			value = 0;
			i = 0x80;
			for (x = 0; x < xsize; x++)
			{
				if (*p)
					value |= i;
				i >>= 1;
				if (i == 0)
				{
					fprintf(ofile, "%c", value);
					i = 0x80;
					value = 0;
				}
				p++;
			}
			if (i != 0x80)
				fprintf(ofile, "%c", value);
			value = 0;
		}
        break;
	case 1: //PORTABLE PIXMAP SMALL SIZE NON BINARY
		fprintf(ofile, "P3\n%d %d\n%d\n", xsize, ysize,numcolors);
		for (y = 0; y < height; y++)
		{
			for (x = 0; x < width; x++)
			{
				switch (MapData(x, y))
				{
					//Walls are blue
				case MAP_REC_RD: case MAP_REC_LD: case MAP_FILLED: case MAP_REC_RU:
				case MAP_REC_LU: fprintf(ofile, "0 0 255");
					break;
					//Gravities, currents, & wormholes are green
				case MAP_GRAV_POS: case MAP_GRAV_NEG: case MAP_GRAV_ACWISE: case MAP_GRAV_CWISE: case MAP_WORM_NORMAL: case MAP_WORM_IN:
				case MAP_WORM_OUT: case MAP_CRNT_UP: case MAP_CRNT_LT: case MAP_CRNT_RT: case MAP_CRNT_DN:
					fprintf(ofile, "0 255 0");
					break;
					//Fuels, targets, treasures, and item concentrators are red
				case MAP_FUEL: case MAP_TARGET: case MAP_TREASURE: case MAP_ITEM_CONC: case MAP_ASTEROID_CONC: case MAP_EMPTYTREASURE:
					fprintf(ofile, "255 0 0");
					break;
					//Bases and Cannons are white
				case MAP_CAN_LEFT: case MAP_CAN_UP: case MAP_CAN_DOWN: case MAP_CAN_RIGHT:	case MAP_BASE:
				case '1': case '2': case '3': case '4': case '5': case '6':
				case '7': case '8': case '9': case '0':
					fprintf(ofile, "255 255 255");
					break;
					//Deco are orange, so is friction
				case MAP_DEC_RD: case MAP_DEC_LD: case MAP_DEC_FLD: case MAP_DEC_RU: case MAP_DEC_LU: case MAP_FRICTION:
					fprintf(ofile, "255 128 0");
					break;

				default : fprintf(ofile, "0 0 0");
					break;
				}
				fprintf(ofile, " ");
			}
			fprintf(ofile, "\n");
		}
        break;
	case 2: //PORTABLE PIXMAP SMALL SIZE BINARY
		fprintf(ofile, "P6\n%d %d\n%d\n", xsize, ysize,numcolors);
		for (y = 0; y < height; y++)
		{
			for (x = 0; x < width; x++)
			{
				switch (MapData(x, y))
				{
					//Walls are blue
				case MAP_REC_RD: case MAP_REC_LD: case MAP_FILLED: case MAP_REC_RU:
				case MAP_REC_LU: putc((char) 0, ofile);
					putc((char) 0, ofile);
					putc((char) 255, ofile);
					break;
					//Gravities, currents, & wormholes are green
				case MAP_GRAV_POS: case MAP_GRAV_NEG: case MAP_GRAV_ACWISE: case MAP_GRAV_CWISE: case MAP_WORM_NORMAL: case MAP_WORM_IN:
				case MAP_WORM_OUT: case MAP_CRNT_UP: case MAP_CRNT_LT: case MAP_CRNT_RT: case MAP_CRNT_DN:
					putc((char) 0, ofile);
					putc((char) 255, ofile);
					putc((char) 0, ofile);
					break;
					//Fuels, targets, treasures, and item concentrators are red
				case MAP_FUEL: case MAP_TARGET: case MAP_TREASURE: case MAP_ITEM_CONC: case MAP_ASTEROID_CONC: case MAP_EMPTYTREASURE:
					putc((char) 255, ofile);
					putc((char) 0, ofile);
					putc((char) 0, ofile);
					break;
					//Bases and Cannons are white
				case MAP_CAN_LEFT: case MAP_CAN_UP: case MAP_CAN_DOWN: case MAP_CAN_RIGHT:	case MAP_BASE:
				case '1': case '2': case '3': case '4': case '5': case '6':
				case '7': case '8': case '9': case '0':
					putc((char) 255, ofile);
					putc((char) 255, ofile);
					putc((char) 255, ofile);
					break;
					//Deco are orange, so is friction
				case MAP_DEC_RD: case MAP_DEC_LD: case MAP_DEC_FLD: case MAP_DEC_RU: case MAP_DEC_LU: case MAP_FRICTION:
					putc((char) 255, ofile);
					putc((char) 128, ofile);
					putc((char) 0, ofile);
					break;

				default :
					putc((char) 0, ofile);
					putc((char) 0, ofile);
					putc((char) 0, ofile);
					break;
				}
			}
		}
        }

        fclose(ofile);
        return false;
}
///////////////////////////////////////////////////////////////////////////////
bool MapDocument::SaveXpmFile(const char *pstrFileName)
{
	FILE *ofile = NULL;
	int x, y, numcolors;
	int width = optionsDoc->mapWidth->GetInt();
	int height = optionsDoc->mapHeight->GetInt();

	numcolors = 34;

	if (NULL == (ofile = fopen (pstrFileName, "wb")))
	{
		ErrorHandler("Couldn't write to file: %s", pstrFileName);
		return false ;
	}

	//Output the X11 pixmap header
	fprintf(ofile,"/* XPM */\n");
	fprintf(ofile,"static char *%s[] = {\n",optionsDoc->mapName->GetString().s);
	fprintf(ofile,"/* columns rows colors chars-per-pixel */\n");
	fprintf(ofile,"\"%d %d %d 1\",\n", width, height, numcolors);
	fprintf(ofile,"\"%d\tc #000000000000\",\n", MAP_SPACE);

	//Set up the colors & corresponding characters
	fprintf(ofile,"\"q\tc Blue\",\n", MAP_REC_RD);
	fprintf(ofile,"\"w\tc Blue\",\n", MAP_REC_LD);
	fprintf(ofile,"\"x\tc Blue\",\n", MAP_FILLED);
	fprintf(ofile,"\"a\tc Blue\",\n", MAP_REC_RU);
	fprintf(ofile,"\"s\tc Blue\",\n", MAP_REC_LU);

	fprintf(ofile,"\"d\tc Yellow\",\n", MAP_CAN_LEFT);
	fprintf(ofile,"\"r\tc Yellow\",\n", MAP_CAN_UP);
	fprintf(ofile,"\"c\tc Yellow\",\n", MAP_CAN_DOWN);
	fprintf(ofile,"\"f\tc Yellow\",\n", MAP_CAN_RIGHT);

	fprintf(ofile,"\"_\tc #000000000000\",\n", MAP_BASE);
	fprintf(ofile,"\"$\tc #000000000000\",\n", MAP_BASE_ORNT);

	fprintf(ofile,"\"#\tc Red\",\n", MAP_FUEL);
	fprintf(ofile,"\"!\tc Red\",\n", MAP_TARGET);
	fprintf(ofile,"\"*\tc Red\",\n", MAP_TREASURE);
	fprintf(ofile,"\"%%\tc Red\",\n", MAP_ITEM_CONC);
	fprintf(ofile,"\"^\tc Red\",\n", MAP_EMPTYTREASURE);
	fprintf(ofile,"\"&\tc Red\",\n", MAP_ASTEROID_CONC);

	fprintf(ofile,"\"+\tc Green\",\n", MAP_GRAV_POS);
	fprintf(ofile,"\"-\tc Green\",\n", MAP_GRAV_NEG);
	fprintf(ofile,"\"<\tc Green\",\n", MAP_GRAV_ACWISE);
	fprintf(ofile,"\">\tc Green\",\n", MAP_GRAV_CWISE);
	fprintf(ofile,"\"@\tc Green\",\n", MAP_WORM_NORMAL);
	fprintf(ofile,"\")\tc Green\",\n", MAP_WORM_OUT);
	fprintf(ofile,"\"(\tc Green\",\n", MAP_WORM_IN);

	fprintf(ofile,"\"i\tc Green\",\n", MAP_CRNT_UP);
	fprintf(ofile,"\"j\tc Green\",\n", MAP_CRNT_LT);
	fprintf(ofile,"\"k\tc Green\",\n", MAP_CRNT_RT);
	fprintf(ofile,"\"m\tc Green\",\n", MAP_CRNT_DN);

	fprintf(ofile,"\"t\tc Magenta\",\n", MAP_DEC_RD);
	fprintf(ofile,"\"y\tc Magenta\",\n", MAP_DEC_LD);
	fprintf(ofile,"\"b\tc Magenta\",\n", MAP_DEC_FLD);
	fprintf(ofile,"\"g\tc Magenta\",\n", MAP_DEC_RU);
	fprintf(ofile,"\"h\tc Magenta\",\n", MAP_DEC_LU);
	fprintf(ofile,"\"z\tc Magenta\",\n", MAP_FRICTION);
	fprintf(ofile,"/* pixels */\n");
	//Output the image itself
	for (y = 0; y < height; y++)
	{
		fprintf(ofile, "\"");
		for (x = 0; x < width; x++)
			fprintf(ofile, "%c", MapData(x, y));
		fprintf(ofile, "\",\n");
	}
	fseek(ofile, -2L, SEEK_CUR);
	fprintf(ofile, "\n};");
	fclose(ofile);
	return false;
}
///////////////////////////////////////////////////////////////////////////////
//Read a map and create the image for it.
//
void MapDocument::build_image(char *image, int xsize)
{
	char	*p;
	int	j;
	int width = optionsDoc->mapWidth->GetInt();
	int height = optionsDoc->mapHeight->GetInt();


	p = convert_map();

	for (j = 0; j < height; j++)
	{
		memcpy(image, p, width);
		image += xsize;
		p += width;
	}
}

/**************************************************************************
* static char *convert_map(int width, int height, FILE *fl)
*   Create an image from map data.
*/
char *MapDocument::convert_map()
{
	int	x, y;
	int	value = 1;
	int width = optionsDoc->mapWidth->GetInt();
	int height = optionsDoc->mapHeight->GetInt();
	char	*output = (char *) malloc(width * height);


	value ^= invert;

	memset((char *) output, invert, width * height);


	for(y = 0; y < height; y++)
		for (x = 0; x < width; x++)
			if (strchr("xswqa#", MapData(x, y)))
			{
				output[y * width + x] = MapData(x, y);
			}

			return output;
}


///////////////////////////////////////////////////////////////////////////////
int MapDocument::Find_closest_team(int posx, int posy)
{
	int team = 0;	//If no team bases are found..targets, cannons, treasures etc..
					//will belong to team "0"
	int i;
	double closest = 900; //an arbitrary number
	double l;
	int count = mapItems.bases.GetCount();
	XpTeamBase *tmpBase = (XpTeamBase *) mapItems.bases.GetHead();

	for (i=0; i < count; i++)
	{
		l = Wrap_length((posx - tmpBase->x),(posy - tmpBase->y));

		if (l < closest)
		{
			team = tmpBase->team;
			closest = l;
		}
		tmpBase = (XpTeamBase *) tmpBase->GetNext();
	}
	return team;
}
///////////////////////////////////////////////////////////////////////////////
//Purpose : find the shortest distance to a point.
double MapDocument::Wrap_length(double dx, double dy)
{
	int width = optionsDoc->mapWidth->GetInt();
	int height = optionsDoc->mapHeight->GetInt();

	dx = XPCWRAP_DX(dx);
	dy = XPCWRAP_DY(dy);
	return LENGTH(dx, dy);
}

///////////////////////////////////////////////////////////////////////////////
ObjList * MapDocument::IncrementMapBlock(int dx, int dy)
{
	char icon;//, old;
	int inced = true; //We can increment (toggle) to the next block in some cases.
	MapBlockChange *chg = NULL;
	ObjList *changeList = new ObjList();

	switch( MapData(dx, dy) )
	{
	case MAP_CAN_LEFT :
		icon = MAP_CAN_UP;
		break;
	case MAP_CAN_UP :
		icon = MAP_CAN_DOWN;
		break;
	case MAP_CAN_DOWN :
		icon = MAP_CAN_RIGHT;
		break;
	case MAP_CAN_RIGHT :
		icon = MAP_CAN_LEFT;
		break;
	case MAP_GRAV_CWISE :
		icon = MAP_GRAV_ACWISE;
		break;
	case MAP_GRAV_ACWISE :
		icon = MAP_GRAV_CWISE;
		break;
	case MAP_GRAV_POS :
		icon = MAP_GRAV_NEG;
		break;
	case MAP_GRAV_NEG :
		icon = MAP_GRAV_POS;
		break;
	case MAP_WORM_NORMAL :
		icon = MAP_WORM_OUT;
		break;
	case MAP_WORM_OUT :
		icon = MAP_WORM_IN;
		break;
	case MAP_WORM_IN :
		icon = MAP_WORM_NORMAL;
		break;
	case MAP_CRNT_UP :
		icon = MAP_CRNT_LT;
		break;
	case MAP_CRNT_LT :
		icon = MAP_CRNT_RT;
		break;
	case MAP_CRNT_RT :
		icon = MAP_CRNT_DN;
		break;
	case MAP_CRNT_DN :
		icon = MAP_CRNT_UP;
		break;
	case '0' :
	case '1' :
	case '2' :
	case '3' :
	case '4' :
	case '5' :
	case '6' :
	case '7' :
	case '8' :
		icon = MapData(dx, dy)+1;
		break;
	case '9' :	icon = MAP_BASE;
		break;

	case MAP_BASE : icon = '0';
		break;
	default :
		inced = false;
		break;
	}

	//Are we able to toggle? If so do it.
	if (inced)
	{
		MapBlockChange *chg = new MapBlockChange();
		chg->x = dx;
		chg->y = dy;
		chg->newchar = icon;
		chg->oldchar = MapData(dx, dy);
		changeList->Add(chg);
	}
	return changeList;
}

///////////////////////////////////////////////////////////////////////////////
ObjList * MapDocument::MirrorMap(bool direct)
//direct: true is horizontal, false is vertical
{
	char icon;
	int x,y,i,j;
	int xbegin, xend, ybegin, yend, ibegin, iinc, jbegin, jinc;
	MapBlockChange *chg = NULL;
	ObjList *changeList = new ObjList();

	EmptyTempData();

	if (fSelected)
	{
		SortSelectionArea();
		xbegin = seldxbeg;
		xend = seldxend;
		ybegin = seldybeg;
		yend = seldyend;
	}
	else
	{
		xbegin = 0;
		xend = optionsDoc->mapWidth->GetInt();
		ybegin = 0;
		yend = optionsDoc->mapHeight->GetInt();
	}

	//fill the clipdata
	for (x = xbegin, i = 0; x < xend; x++, i++)
		for (y = ybegin, j = 0; y < yend; y++, j++)
		{
			if (!InsideMap(x, y))
				continue;
			clipdata[i][j] = MapData(x, y) ;
		}
		// mirror the clipdata and refill the main map
        //We start from different places depending on the direction we are
        //mirroring.
        if (direct)
        {
			ibegin = 0;
			iinc = 1;
			jbegin = yend-ybegin-1;
			jinc = -1;
        }
        else
        {
			ibegin = xend-xbegin-1;
			iinc = -1;
			jbegin = 0;
			jinc = +1;
        }



        for (x = xbegin, i = ibegin; x < xend; x++, i=i+iinc)
			for (y = ybegin, j = jbegin; y < yend; y++, j=j+jinc)
			{
				switch(clipdata[i][j])
				{
				case MAP_REC_RD :
					icon = direct ? MAP_REC_RU : MAP_REC_LD;
					break;
				case MAP_REC_LD :
					icon = direct ? MAP_REC_LU : MAP_REC_RD;
					break;
				case MAP_REC_LU :
					icon = direct ? MAP_REC_LD : MAP_REC_RU;
					break;
				case MAP_REC_RU :
					icon = direct ? MAP_REC_RD : MAP_REC_LU;
					break;
				case MAP_CAN_UP :
					icon = direct ? MAP_CAN_DOWN : MAP_CAN_UP;
					break;
				case MAP_CAN_DOWN :
					icon = direct ? MAP_CAN_UP : MAP_CAN_DOWN;
					break;
				case MAP_CAN_LEFT :
					icon = direct ? MAP_CAN_LEFT : MAP_CAN_RIGHT;
					break;
				case MAP_CAN_RIGHT :
					icon = direct ? MAP_CAN_RIGHT : MAP_CAN_LEFT;
					break;
				case MAP_CRNT_UP :
					icon = direct ? MAP_CRNT_DN : MAP_CRNT_UP;
					break;
				case MAP_CRNT_DN :
					icon = direct ? MAP_CRNT_UP : MAP_CRNT_DN;
					break;
				case MAP_CRNT_RT :
					icon = direct ? MAP_CRNT_RT : MAP_CRNT_LT;
					break;
				case MAP_CRNT_LT :
					icon = direct ? MAP_CRNT_LT : MAP_CRNT_RT;
					break;
				case MAP_DEC_RD :
					icon = direct ? MAP_DEC_RU : MAP_DEC_LD;
					break;
				case MAP_DEC_LD :
					icon = direct ? MAP_DEC_LU : MAP_DEC_RD;
					break;
				case MAP_DEC_LU :
					icon = direct ? MAP_DEC_LD : MAP_DEC_RU;
					break;
				case MAP_DEC_RU :
					icon = direct ? MAP_DEC_RD : MAP_DEC_LU;
					break;
				default :
					icon = clipdata[i][j];
					break;
				}

				MapBlockChange *chg = new MapBlockChange();
				chg->x = x;
				chg->y = y;
				chg->newchar = icon;
				chg->oldchar = MapData(x, y);
				changeList->Add(chg);

			}

	return changeList;
}

///////////////////////////////////////////////////////////////////////////////
ObjList * MapDocument::CycleMapHorizontal(bool direct)
//direct: true is left, false is right
{
	int x,y,i,j;
	int xbegin, xend, ybegin, yend;
	char tmp;
	MapBlockChange *chg = NULL;
	ObjList *changeList = new ObjList();

	EmptyTempData();

	if (fSelected)
	{
		SortSelectionArea();
		xbegin = seldxbeg;
		xend = seldxend;
		ybegin = seldybeg;
		yend = seldyend;
	}
	else
	{
		xbegin = 0;
		xend = optionsDoc->mapWidth->GetInt();
		ybegin = 0;
		yend = optionsDoc->mapHeight->GetInt();
	}

	//fill the clipdata map
	for (x = xbegin, i = 0; x < xend; x++, i++)
	{
		for (y = ybegin, j = 0; y < yend; y++, j++)
		{
			if (!InsideMap(x, y))
				continue;
			tmp = MapData(x, y);

			clipdata[i][j] = tmp;
		}
	}

	//If "direct" is true scroll to the left
	if (direct)
	{
		//cycle the clipdata and refill the main map
		for (x = xbegin, i = 1; x < xend; x++, i++)
			for (y = ybegin, j = 0; y < yend; y++, j++)
			{
				MapBlockChange *chg = new MapBlockChange();
				chg->x = x;
				chg->y = y;
				chg->newchar = clipdata[i][j];
				chg->oldchar = MapData(x, y);
				changeList->Add(chg);
			}

			x = xend-1;
			i = 0;
			for (y = ybegin, j = 0; y < yend; y++, j++)
			{
				MapBlockChange *chg = new MapBlockChange();
				chg->x = x;
				chg->y = y;
				chg->newchar = clipdata[i][j];
				chg->oldchar = MapData(x, y);
				changeList->Add(chg);
			}
	}
	//otherwise scroll to the right
	else
	{
		// cycle the clipdata and refill the main map
		for (x = xbegin+1, i = 0; x < xend; x++, i++)
			for (y = ybegin, j = 0; y < yend; y++, j++)
			{
				MapBlockChange *chg = new MapBlockChange();
				chg->x = x;
				chg->y = y;
				chg->newchar = clipdata[i][j];
				chg->oldchar = MapData(x, y);
				changeList->Add(chg);
			}
			x = xbegin;
			i = xend-xbegin-1;
			for (y = ybegin, j = 0; y < yend; y++, j++)
			{
				MapBlockChange *chg = new MapBlockChange();
				chg->x = x;
				chg->y = y;
				chg->newchar = clipdata[i][j];
				chg->oldchar = MapData(x, y);
				changeList->Add(chg);
			}
	}
	return changeList;
}

///////////////////////////////////////////////////////////////////////////////
//direct: true is upwards, false is downwards
ObjList * MapDocument::CycleMapVertical(bool direct)
{
	int x,y,i,j;
	int xbegin, xend, ybegin, yend;
	char tmp;
	MapBlockChange *chg = NULL;
	ObjList *changeList = new ObjList();

	EmptyTempData();

	if (fSelected)
	{
		SortSelectionArea();
		xbegin = seldxbeg;
		xend = seldxend;
		ybegin = seldybeg;
		yend = seldyend;
	}
	else
	{
		xbegin = 0;
		xend = optionsDoc->mapWidth->GetInt();
		ybegin = 0;
		yend = optionsDoc->mapHeight->GetInt();
	}

	//fill the clipdata map
	for (x = xbegin, i = 0; x < xend; x++, i++)
		for (y = ybegin, j = 0; y < yend; y++, j++)
		{
			if (!InsideMap(x, y))
				continue;
			tmp = MapData(x, y);

			clipdata[i][j] = tmp;
		}

	//if "direction" is true, scroll upwards
	if (direct)
	{
		//rotate the clipdata and refill the main map
		for (x = xbegin, i = 0; x < xend; x++, i++)
			for (y = ybegin, j = 1; y < yend; y++, j++)
			{
				MapBlockChange *chg = new MapBlockChange();
				chg->x = x;
				chg->y = y;
				chg->newchar = clipdata[i][j];
				chg->oldchar = MapData(x, y);
				changeList->Add(chg);
			}

		y = yend-1;
		j = 0;
		for (x = xbegin, i = 0; x < xend; x++, i++)
		{
			MapBlockChange *chg = new MapBlockChange();
			chg->x = x;
			chg->y = y;
			chg->newchar = clipdata[i][j];
			chg->oldchar = MapData(x, y);
			changeList->Add(chg);
		}
	}
	//otherwise scroll downwards
	else
	{
		//rotate the clipdata and refill the main map
		for (x = xbegin, i = 0; x < xend; x++, i++)
			for (y = ybegin+1, j = 0; y < yend; y++, j++)
			{
				MapBlockChange *chg = new MapBlockChange();
				chg->x = x;
				chg->y = y;
				chg->newchar = clipdata[i][j];
				chg->oldchar = MapData(x, y);
				changeList->Add(chg);
			}

			y = ybegin;
			j = yend-ybegin-1;
			for (x = xbegin, i = 0; x < xend; x++, i++)
			{
				MapBlockChange *chg = new MapBlockChange();
				chg->x = x;
				chg->y = y;
				chg->newchar = clipdata[i][j];
				chg->oldchar = MapData(x, y);
				changeList->Add(chg);
			}
	}
	return changeList;
}
///////////////////////////////////////////////////////////////////////////////
ObjList * MapDocument::NegativeMap()
{
	int i,j;
	int xbegin, xend, ybegin, yend;
	char icon;
	MapBlockChange *chg = NULL;
	ObjList *changeList = new ObjList();

	if (fSelected)
	{
		SortSelectionArea();
		xbegin = seldxbeg;
		xend = seldxend;
		ybegin = seldybeg;
		yend = seldyend;
	}
	else
	{
		xbegin = 0;
		xend = optionsDoc->mapWidth->GetInt();
		ybegin = 0;
		yend = optionsDoc->mapHeight->GetInt();
	}

	for (i=xbegin;i<xend;i++)
		for(j=ybegin;j<yend;j++)
		{
			if (!InsideMap(i, j))
				continue;
			switch(MapData(i, j))
			{
			case MAP_SPACE:	icon = MAP_FILLED;
				break;
			case MAP_FILLED: icon = MAP_SPACE;
				break;
			case MAP_REC_RD: icon = MAP_REC_LU;
				break;
			case MAP_REC_LU: icon = MAP_REC_RD;
				break;
			case MAP_REC_LD: icon = MAP_REC_RU;
				break;
			case MAP_REC_RU: icon = MAP_REC_LD;
				break;
			default: continue;
			}
			MapBlockChange *chg = new MapBlockChange();
			chg->x = i;
			chg->y = j;
			chg->newchar = icon;
			chg->oldchar = MapData(i, j);
			changeList->Add(chg);
		}
	return changeList;
}

///////////////////////////////////////////////////////////////////////////////
ObjList* MapDocument::BlockLine(XpPoint beg, XpPoint end, char blockType)
{
	int deltax,deltay,sign,i,x2,y2;//, icon;
	ObjList *changeList = new ObjList();

	deltax = end.x - beg.x;
	deltay = end.y - beg.y;

	if ( (deltax == 0) && (deltay == 0) ) {
		MapBlockChange *chg = new MapBlockChange();
		chg->x = beg.x;
		chg->y = beg.y;
		chg->newchar = blockType;
		chg->oldchar = MapData(beg.x, beg.y);
		changeList->Add(chg);
		return changeList;
	}

	if (abs(deltax) >= abs(deltay))
	{
		sign = (deltax < 0) ? -1 : 1;
		for (i = 0; abs(i) <= abs(deltax); i += sign) {
			x2 = beg.x + i;
			y2 = beg.y + (i * deltay) / deltax;

			MapBlockChange *chg = new MapBlockChange();
			chg->x = x2;
			chg->y = y2;
			chg->newchar = blockType;
			chg->oldchar = MapData(x2, y2);
			changeList->Add(chg);
		}
	} else {
		sign = (deltay < 0) ? -1 : 1;
		for (i = 0; abs(i) <= abs(deltay); i += sign) {
			x2 = beg.x + (i * deltax) / deltay ;
			y2 = beg.y + i;
			MapBlockChange *chg = new MapBlockChange();
			chg->x = x2;
			chg->y = y2;
			chg->newchar = blockType;
			chg->oldchar = MapData(x2, y2);
			changeList->Add(chg);
		}
	}
	return changeList;
}
///////////////////////////////////////////////////////////////////////////////
ObjList* MapDocument::BlockCircle(XpPoint beg, XpPoint end, char blockType, int circ_type)
{
        int x2, y2;
        double deltax,deltay, radius, radius2;
        int scale = 1;
		ObjList *changeList = new ObjList();

        bool changeBool = false;

        deltax = end.x - beg.x;
        deltay = end.y - beg.y;

        //This gives us the radius to check against.
        radius = sqrt(deltax*deltax + deltay*deltay);

        if ( radius == 0 )
         if ( (deltax == 0) && (deltay == 0) ) {
			MapBlockChange *chg = new MapBlockChange();
			chg->x = beg.x;
			chg->y = beg.y;
			chg->newchar = blockType;
			chg->oldchar = MapData(beg.x, beg.y);
			changeList->Add(chg);
            return changeList;
        }


        for(x2 = beg.x - (int)radius; x2<= beg.x+(int)radius; x2++)
                for(y2 = beg.y - (int)radius; y2<= beg.y+(int)radius; y2++)
                {
                        if (!InsideMap(x2, y2))
                                continue;
                        //The radius of the current block from the start point
                        radius2 = sqrt((double)(abs(beg.x-x2)*abs(beg.x-x2) + abs(beg.y-y2)*abs(beg.y-y2)));
                        if (circ_type == IDM_CIRCLEEMPTY) //Empty circle
                        {
                                //If this block is on the desired radius, draw it
                                if ((int) radius2 == (int) radius)
                                        changeBool=true;
                                else
                                        changeBool=false;
                        }
                        else if (circ_type == IDM_CIRCLEFILLED)//We want to draw a filled circle
                        {
                                //If this block is on or inside the desired radius...
                                if ((int) radius2 <= (int) radius)
                                        changeBool=true;
                                else
                                        changeBool=false;
                        }
                        if (changeBool)
                        {
							MapBlockChange *chg = new MapBlockChange();
							chg->x = x2;
							chg->y = y2;
							chg->newchar = blockType;
							chg->oldchar = MapData(x2, y2);
							changeList->Add(chg);
                        }

                }
		return changeList;
}
///////////////////////////////////////////////////////////////////////////////
ObjList * MapDocument::BlockRectangle(XpPoint beg, XpPoint end, char blockType, int rect_type)
{
        int  x, y, temp;
		ObjList *changeList = new ObjList();

        bool changeBool = true;

        if (end.y < beg.y)
        {
                temp = end.y;
                end.y = beg.y;
                beg.y = temp;
        }
        if (end.x < beg.x)
        {
                temp = end.x;
                end.x = beg.x;
                beg.x = temp;
        }

        //We need to adjust the end points, because we have one block outside of the area
        //we want selected, and the easiest way to correct this is to fix it here.
        end.x += -1;
        end.y += -1;


	    if ( beg.x == end.x && beg.y == end.y ) {
			MapBlockChange *chg = new MapBlockChange();
			chg->x = beg.x;
			chg->y = beg.y;
			chg->newchar = blockType;
			chg->oldchar = MapData(beg.x, beg.y);
			changeList->Add(chg);
            return changeList;
        }


        for(x = beg.x; x <= end.x; x++)
                for(y = beg.y; y <= end.y; y++)
                {
                        if (!InsideMap(x, y))
                                continue;
                        if (rect_type == IDM_RECTEMPTY) //Empty Rectangle
                        {
                                if ((x == beg.x) || (x == end.x) || (y ==beg.y) || (y == end.y))
                                        changeBool=true;
                                else
                                        changeBool=false;
                        }
                        if (changeBool)
                        {
							MapBlockChange *chg = new MapBlockChange();
							chg->x = x;
							chg->y = y;
							chg->newchar = blockType;
							chg->oldchar = MapData(x, y);
							changeList->Add(chg);
                        }

                }
		return changeList;
}
///////////////////////////////////////////////////////////////////////////////
ObjList * MapDocument::ClearMapArea(int type)
{
        int                   i,j;
        int startx, starty, endx, endy;
		ObjList *changeList = new ObjList();

        if (fSelected)
        {
                SortSelectionArea();
        }

        startx = starty = 0;
        endx = optionsDoc->mapWidth->GetInt();
        endy = optionsDoc->mapHeight->GetInt();

        if (fSelected && (type != CROPMAP))
        {
                startx = seldxbeg;
                endx = seldxend;
                starty = seldybeg;
                endy = seldyend;
        }

        for (i = startx; i < endx; i++)
			for (j = starty; j < endy; j++)
			{
				if (!InsideMap(i, j))
					continue;

				if ((type == CROPMAP) && fSelected)
				{
					if ((i >= seldxbeg) && (i < seldxend) &&
						(j >= seldybeg) && (j < seldyend) )
						continue;
				}
				MapBlockChange *chg = new MapBlockChange();
				chg->x = i;
				chg->y = j;
				chg->newchar = MAP_SPACE;
				chg->oldchar = MapData(i, j);
				changeList->Add(chg);
			}

		return changeList;
}
///////////////////////////////////////////////////////////////////////////////
ObjList * MapDocument::RoundMapArea(ObjList *objList)
{
	int x,
		y,
		xmax, ymax,
		mapcursorx, mapcursory,
		mask,
		change = false;
	char type;
	unsigned char           up_filled[256],
		left_filled[256],
		down_filled[256],
		right_filled[256],
		lu_filled[256],
		ld_filled[256],
		rd_filled[256],
		ru_filled[256];
	MapBlockChange *chg = NULL;
	ObjList *changeList = NULL;

	if (objList)
		changeList = objList;
	else
		changeList = new ObjList();

	if (fSelected) // area selected, do just this area
	{
		if (seldxbeg > seldxend)
		{
			mapcursorx = seldxend;
			xmax = seldxbeg;
		}
		else
		{
			mapcursorx = seldxbeg;
			xmax = seldxend;
		}
		if (seldybeg > seldyend)
		{
			mapcursory = seldyend;
			ymax = seldybeg;
		}
		else
		{
			mapcursory = seldybeg;
			ymax = seldyend;
		}
	}
	else // no area selected, do entire map
	{
		mapcursorx = 0;
		mapcursory = 0;
		xmax = optionsDoc->mapWidth->GetInt();;
		ymax = optionsDoc->mapHeight->GetInt();;
	}

	memset(up_filled, 0, sizeof up_filled);
	memset(left_filled, 0, sizeof left_filled);
	memset(down_filled, 0, sizeof down_filled);
	memset(right_filled, 0, sizeof right_filled);
	memset(lu_filled, 0, sizeof lu_filled);
	memset(ld_filled, 0, sizeof ld_filled);
	memset(rd_filled, 0, sizeof rd_filled);
	memset(ru_filled, 0, sizeof ru_filled);

	up_filled[MAP_FILLED] = 1;
	up_filled[MAP_FUEL] = 1;
	up_filled[MAP_REC_RD] = 1;
	up_filled[MAP_REC_LD] = 1;

	left_filled[MAP_FILLED] = 1;
	left_filled[MAP_FUEL] = 1;
	left_filled[MAP_REC_RU] = 1;
	left_filled[MAP_REC_RD] = 1;

	down_filled[MAP_FILLED] = 1;
	down_filled[MAP_FUEL] = 1;
	down_filled[MAP_REC_RU] = 1;
	down_filled[MAP_REC_LU] = 1;

	right_filled[MAP_FILLED] = 1;
	right_filled[MAP_FUEL] = 1;
	right_filled[MAP_REC_LD] = 1;
	right_filled[MAP_REC_LU] = 1;

	lu_filled[MAP_FILLED] = 1;
	lu_filled[MAP_FUEL] = 1;
	lu_filled[MAP_REC_LD] = 1;
	lu_filled[MAP_REC_RD] = 1;
	lu_filled[MAP_REC_RU] = 1;

	ld_filled[MAP_FILLED] = 1;
	ld_filled[MAP_FUEL] = 1;
	ld_filled[MAP_REC_LU] = 1;
	ld_filled[MAP_REC_RU] = 1;
	ld_filled[MAP_REC_RD] = 1;

	rd_filled[MAP_FILLED] = 1;
	rd_filled[MAP_FUEL] = 1;
	rd_filled[MAP_REC_RU] = 1;
	rd_filled[MAP_REC_LU] = 1;
	rd_filled[MAP_REC_LD] = 1;

	ru_filled[MAP_FILLED] = 1;
	ru_filled[MAP_FUEL] = 1;
	ru_filled[MAP_REC_RD] = 1;
	ru_filled[MAP_REC_LD] = 1;
	ru_filled[MAP_REC_LU] = 1;

#define UP_FILLED       (up_filled[MapData( (int) x, (int) y-1) & 0xFF] == 1)
#define LEFT_FILLED     (left_filled[MapData( (int) x-1, (int) y) & 0xFF] == 1)
#define DOWN_FILLED     (down_filled[MapData( (int) x, (int) y+1) & 0xFF] == 1)
#define RIGHT_FILLED    (right_filled[MapData( (int) x+1, (int) y) & 0xFF] == 1)
#define LU_FILLED       (lu_filled[MapData( (int) x-1, (int) y-1) & 0xFF] == 1)
#define LD_FILLED       (ld_filled[MapData( (int) x-1, (int) y+1) & 0xFF] == 1)
#define RD_FILLED       (rd_filled[MapData( (int) x+1, (int) y+1) & 0xFF] == 1)
#define RU_FILLED       (ru_filled[MapData( (int) x+1, (int) y-1) & 0xFF] == 1)

#define UP_BIT          (1 << 0)
#define LEFT_BIT        (1 << 1)
#define DOWN_BIT        (1 << 2)
#define RIGHT_BIT       (1 << 3)
#define LU_BIT          (1 << 4)
#define LD_BIT          (1 << 5)
#define RD_BIT          (1 << 6)
#define RU_BIT          (1 << 7)


	// see if we can convert spaces into (half) blocks if
	// the space is surrounded by (half) blocks.

	for (y = mapcursory; y < ymax; y++)
	{
        for (x = mapcursorx; x < xmax; x++)
		{
			type = MapData( (int) x, (int) y);
			if (type == MAP_SPACE) {
                mask = (UP_FILLED << 0) |
					(LEFT_FILLED << 1) |
					(DOWN_FILLED << 2) |
					(RIGHT_FILLED << 3);
                switch (mask) {
                case UP_BIT | LEFT_BIT | DOWN_BIT | RIGHT_BIT:
                case UP_BIT | LEFT_BIT | DOWN_BIT:
                case UP_BIT | LEFT_BIT | RIGHT_BIT:
                case UP_BIT | DOWN_BIT | RIGHT_BIT:
                case LEFT_BIT | DOWN_BIT | RIGHT_BIT:
                case DOWN_BIT | UP_BIT:
                case LEFT_BIT | RIGHT_BIT:
					type = MAP_FILLED;
					break;
                case UP_BIT | LEFT_BIT:
					type = MAP_REC_LU;
					break;
                case UP_BIT | RIGHT_BIT:
					type = MAP_REC_RU;
					break;
                case DOWN_BIT | RIGHT_BIT:
					type = MAP_REC_RD;
					break;
                case DOWN_BIT | LEFT_BIT:
					type = MAP_REC_LD;
					break;
                }
                if (type != MapData( (int) x, (int) y) ) {
					chg = new MapBlockChange();
					chg->x = x;
					chg->y = y;
					chg->newchar = type;
					chg->oldchar = MapData(x, y);
					changeList->Add(chg);

					change = true;
					x++;
                }
			}
        }
	}

	if (change == false)
	{

	// now also check if we can convert half diagonal
	// blocks into filled blocks if they're surrounded by (half) blocks.

        for (y = mapcursory; y < ymax; y++)
		{
			for (x = mapcursorx; x < xmax; x++)
			{
				type = MapData( (int) x, (int) y);
                switch (type) {
                case MAP_REC_RD:
                case MAP_REC_RU:
                case MAP_REC_LD:
                case MAP_REC_LU:
					mask = (UP_FILLED << 0) |
                        (LEFT_FILLED << 1) |
                        (DOWN_FILLED << 2) |
                        (RIGHT_FILLED << 3);
					switch (mask) {
					case UP_BIT | LEFT_BIT | DOWN_BIT | RIGHT_BIT:
					case UP_BIT | LEFT_BIT | DOWN_BIT:
					case UP_BIT | LEFT_BIT | RIGHT_BIT:
					case UP_BIT | DOWN_BIT | RIGHT_BIT:
					case LEFT_BIT | DOWN_BIT | RIGHT_BIT:
					case DOWN_BIT | UP_BIT:
					case LEFT_BIT | RIGHT_BIT:
                        type = MAP_FILLED;
                        break;
					case UP_BIT | LEFT_BIT:
                        type = MAP_REC_LU;
                        break;
					case UP_BIT | RIGHT_BIT:
                        type = MAP_REC_RU;
                        break;
					case DOWN_BIT | RIGHT_BIT:
                        type = MAP_REC_RD;
                        break;
					case DOWN_BIT | LEFT_BIT:
                        type = MAP_REC_LD;
                        break;
					}
					if (type != MapData( (int) x, (int) y) )
					{
						chg = new MapBlockChange();
						chg->x = x;
						chg->y = y;
						chg->newchar = type;
						chg->oldchar = MapData(x, y);
						changeList->Add(chg);

						change = true;
						x++;
					}
                }
			}
        }
	}

	if (change == false)
	{
	// now also check if we can convert half diagonal blocks into
	// filled blocks if their diagonal is next to a (half) block.
        for (y = mapcursory; y < ymax; y++)
		{
			for (x = mapcursorx; x < xmax; x++)
			{
				type = MapData( (int) x, (int) y);
                switch (type) {
                case MAP_REC_LU:
					if (RIGHT_FILLED == 1 || DOWN_FILLED == 1)
					{
                        type = MAP_FILLED;
					}
					break;
                case MAP_REC_LD:
					if (RIGHT_FILLED == 1 || UP_FILLED == 1)
					{
                        type = MAP_FILLED;
					}
					break;
                case MAP_REC_RU:
					if (LEFT_FILLED == 1 || DOWN_FILLED == 1)
					{
                        type = MAP_FILLED;
					}
					break;
                case MAP_REC_RD:
					if (LEFT_FILLED == 1 || UP_FILLED == 1)
					{
                        type = MAP_FILLED;
					}
					break;
                default:
					continue;
                }
                if (type != MapData( (int) x, (int) y) )
				{
					chg = new MapBlockChange();
					chg->x = x;
					chg->y = y;
					chg->newchar = type;
					chg->oldchar = MapData(x, y);
					changeList->Add(chg);

					change = true;
					x++;
                }
			}
        }
	}

	if (change == false) {
	// now also check if we can convert half diagonal blocks into
	// filled blocks if one of their backs is next to a space.
        for (y = mapcursory; y < ymax; y++)
		{
			for (x = mapcursorx; x < xmax; x++)
			{
				type = MapData( (int) x, (int) y);
                switch (type) {
                case MAP_REC_RD:
					if (RIGHT_FILLED == 0 || DOWN_FILLED == 0)
					{
                        type = MAP_FILLED;
					}
					break;
                case MAP_REC_RU:
					if (RIGHT_FILLED == 0 || UP_FILLED == 0)
					{
                        type = MAP_FILLED;
					}
					break;
                case MAP_REC_LD:
					if (LEFT_FILLED == 0 || DOWN_FILLED == 0)
					{
                        type = MAP_FILLED;
					}
					break;
                case MAP_REC_LU:
					if (LEFT_FILLED == 0 || UP_FILLED == 0)
					{
                        type = MAP_FILLED;
					}
					break;
                default:
					continue;
                }
                if (type != MapData( (int) x, (int) y) )
				{
					chg = new MapBlockChange();
					chg->x = x;
					chg->y = y;
					chg->newchar = type;
					chg->oldchar = MapData(x, y);
					changeList->Add(chg);

					change = true;
					x++;
                }
			}
        }
	}

	if (change == false) {
	// check if we can convert half diagonal blocks into filled
	// blocks if their diagonal is next to a (half) filled block.
        for (y = mapcursory; y < ymax; y++)
		{
			for (x = mapcursorx; x < xmax; x++)
			{
				type = MapData( (int) x, (int) y);
                switch (type) {
                case MAP_REC_RD:
					if (LEFT_FILLED == 1 || UP_FILLED == 1)
					{
                        type = MAP_FILLED;
					}
					break;
                case MAP_REC_RU:
					if (LEFT_FILLED == 1 || DOWN_FILLED == 1)
					{
                        type = MAP_FILLED;
					}
					break;
                case MAP_REC_LD:
					if (RIGHT_FILLED == 1 || UP_FILLED == 1)
					{
                        type = MAP_FILLED;
					}
					break;
                case MAP_REC_LU:
					if (RIGHT_FILLED == 1 || DOWN_FILLED == 1)
					{
                        type = MAP_FILLED;
					}
					break;
                default:
					continue;
                }
                if (type != MapData( (int) x, (int) y) )
				{
					chg = new MapBlockChange();
					chg->x = x;
					chg->y = y;
					chg->newchar = type;
					chg->oldchar = MapData(x, y);
					changeList->Add(chg);

					change = true;
					x++;
                }
			}
        }
	}

	if (change == false) {
	// now also check if we can convert filled blocks
	// into half blocks if they're surrounded by spaces on two sides.
        for (y = mapcursory; y < ymax; y++)
		{
			for (x = mapcursorx; x < xmax; x++)
			{
				type = MapData( (int) x, (int) y);
                if (type == MAP_FILLED) {
					mask = (LU_FILLED << 4) |
                        (LD_FILLED << 5) |
                        (RD_FILLED << 6) |
                        (RU_FILLED << 7) |
                        (UP_FILLED << 0) |
                        (LEFT_FILLED << 1) |
                        (DOWN_FILLED << 2) |
                        (RIGHT_FILLED << 3);
					switch (mask)
					{
					case RIGHT_BIT | DOWN_BIT | RD_BIT:
					case RIGHT_BIT | DOWN_BIT:
                        type = MAP_REC_RD;
                        break;
					case RIGHT_BIT | UP_BIT | RU_BIT:
					case RIGHT_BIT | UP_BIT:
                        type = MAP_REC_RU;
                        break;
					case LEFT_BIT | DOWN_BIT | LD_BIT:
					case LEFT_BIT | DOWN_BIT:
                        type = MAP_REC_LD;
                        break;
					case LEFT_BIT | UP_BIT | LU_BIT:
					case LEFT_BIT | UP_BIT:
                        type = MAP_REC_LU;
                        break;
					}
					if (type != MapData( (int) x, (int) y) )
					{
						chg = new MapBlockChange();
						chg->x = x;
						chg->y = y;
						chg->newchar = type;
						chg->oldchar = MapData(x, y);
						changeList->Add(chg);

                        change = true;
                        x++;
					}
                }
			}
        }
	}

	if (change == false)
	{
	// now also check if we can convert filled blocks
	// into half blocks if they're surrounded by spaces on two sides
	// but with a possible half block on the far side.
        for (y = mapcursory; y < ymax; y++)
		{
			for (x = mapcursorx; x < xmax; x++)
			{
				type = MapData( (int) x, (int) y);
                if (type == MAP_FILLED)
				{
					mask = (LU_FILLED << 4) |
                        (LD_FILLED << 5) |
                        (RD_FILLED << 6) |
                        (RU_FILLED << 7) |
                        (UP_FILLED << 0) |
                        (LEFT_FILLED << 1) |
                        (DOWN_FILLED << 2) |
                        (RIGHT_FILLED << 3);
					switch (mask) {
					case LD_BIT | RU_BIT | RIGHT_BIT | DOWN_BIT | RD_BIT:
					case LD_BIT | RU_BIT | RIGHT_BIT | DOWN_BIT:
					case LD_BIT | RIGHT_BIT | DOWN_BIT | RD_BIT:
					case LD_BIT | RIGHT_BIT | DOWN_BIT:
					case RU_BIT | RIGHT_BIT | DOWN_BIT | RD_BIT:
					case RU_BIT | RIGHT_BIT | DOWN_BIT:
                        type = MAP_REC_RD;
                        break;
					case LU_BIT | RD_BIT | RIGHT_BIT | UP_BIT | RU_BIT:
					case LU_BIT | RD_BIT | RIGHT_BIT | UP_BIT:
					case LU_BIT | RIGHT_BIT | UP_BIT | RU_BIT:
					case LU_BIT | RIGHT_BIT | UP_BIT:
					case RD_BIT | RIGHT_BIT | UP_BIT | RU_BIT:
					case RD_BIT | RIGHT_BIT | UP_BIT:
                        type = MAP_REC_RU;
                        break;
					case LU_BIT | RD_BIT | LEFT_BIT | DOWN_BIT | LD_BIT:
					case LU_BIT | RD_BIT | LEFT_BIT | DOWN_BIT:
					case LU_BIT | LEFT_BIT | DOWN_BIT | LD_BIT:
					case LU_BIT | LEFT_BIT | DOWN_BIT:
					case RD_BIT | LEFT_BIT | DOWN_BIT | LD_BIT:
					case RD_BIT | LEFT_BIT | DOWN_BIT:
                        type = MAP_REC_LD;
                        break;
					case LD_BIT | RU_BIT | LEFT_BIT | UP_BIT | LU_BIT:
					case LD_BIT | RU_BIT | LEFT_BIT | UP_BIT:
					case LD_BIT | LEFT_BIT | UP_BIT | LU_BIT:
					case LD_BIT | LEFT_BIT | UP_BIT:
					case RU_BIT | LEFT_BIT | UP_BIT | LU_BIT:
					case RU_BIT | LEFT_BIT | UP_BIT:
                        type = MAP_REC_LU;
                        break;
					}
					if (type != MapData( (int) x, (int) y) )
					{
						chg = new MapBlockChange();
						chg->x = x;
						chg->y = y;
						chg->newchar = type;
						chg->oldchar = MapData(x, y);
						changeList->Add(chg);
                        change = true;
                        x++;
					}
                }
			}
        }
	}

	if (change == false)
	{
	// now also check if we can remove filled blocks
	// if they're surrounded by spaces on three or four sides.
        for (y = mapcursory; y < ymax; y++)
		{
			for (x = mapcursorx; x < xmax; x++)
			{
				type = MapData( (int) x, (int) y);
				if (type == MAP_FILLED) {
					mask = (UP_FILLED << 0) |
                        (LEFT_FILLED << 1) |
                        (DOWN_FILLED << 2) |
                        (RIGHT_FILLED << 3);
					switch (mask) {
					case UP_BIT:
					case LEFT_BIT:
					case DOWN_BIT:
					case RIGHT_BIT:
					case 0:
                        type = MAP_SPACE;
                        break;
					}
					if (type != MapData( (int) x, (int) y) )
					{
						chg = new MapBlockChange();
						chg->x = x;
						chg->y = y;
						chg->newchar = type;
						chg->oldchar = MapData(x, y);
						changeList->Add(chg);

						change = true;
                        x++;
					}
                }
			}
        }
	}

//	if (change)
//	{
//		changed = true;
//	}

	return changeList;
}

int               grow_minx,grow_miny,grow_maxx,grow_maxy,
grow_w,grow_h,grow_centerx, grow_centery;
int               grow_filled = 0;

double                   grow_xa = 1.0, grow_ya = 1.0;
grow_t                   *grow = NULL;

double rfrac(void)
{
// Return a pseudo-random value in the range { 0 <= x < 1 }.
// Assume all RAND_MAXs are at least 32767 and divide by 32768.
        return (double)((double)(randomMT() & 0x7FFF) * 0.000030517578125);
}
///////////////////////////////////////////////////////////////////////////////
ObjList * MapDocument::GrowMapArea(ObjList *objList)
{
	grow_t                *next, *delgrow;
	int                   i,j,growat;
	int                   angle;
	double                 x,y,dx,dy;
	double				 temp;
	MapBlockChange *chg = NULL;
	ObjList *changeList = NULL;

	if (objList == NULL)
		changeList = RoundMapArea(objList);
	else
		changeList = objList;

	// free grow structure
	next = grow;
	while ( next != NULL) {
		delgrow = next->next;
		free(next);
		next = delgrow;
	}
	grow = NULL;

	grow_xa = grow_ya = 1.0;



	if (fSelected) // area selected, do just this area
	{
		SortSelectionArea();
		grow_minx = seldxbeg;
		grow_maxx = seldxend-1;
		grow_miny = seldybeg;
		grow_maxy = seldyend-1;
	}
	else /* no area selected, do entire map */
	{
        grow_minx = 0;
        grow_miny = 0;
        grow_maxx = optionsDoc->mapWidth->GetInt();;
        grow_maxy = optionsDoc->mapHeight->GetInt();;

	}

	grow_w = grow_maxx-grow_minx;
	grow_h = grow_maxy-grow_miny;
	grow_centerx = (grow_minx+grow_maxx)/2;
	grow_centery = (grow_miny+grow_maxy)/2;

	grow_filled = 0;
	for (i=grow_minx;i<grow_maxx;i++) {
        for (j=grow_miny;j<grow_maxy;j++) {
			if (!InsideMap(i, j))
				continue;
			if (MapData((int) i, (int) j) != MAP_FILLED) {
				;
			} else {
				grow_filled++;
				next = grow;
				grow = (grow_t *) malloc(sizeof(grow_t));
				grow->x = i;
				grow->y = j;
				grow->next = next;
			}
        }
	}

	// place a square in the center if there are none
	if (grow == NULL) {
		chg = new MapBlockChange();
		chg->x = grow_centerx;
		chg->y = grow_centery;
		chg->newchar = MAP_FILLED;
		chg->oldchar = MapData(grow_centerx, grow_centery);
		changeList->Add(chg);

        grow = (grow_t *) malloc(sizeof(grow_t));
        grow->x = grow_centerx;
        grow->y = grow_centery;
        grow->next = NULL;
        grow_filled=1;
        if (grow_w > grow_h) {
			grow_ya = ((double) grow_h)/ ((double) grow_w);
        } else {
			grow_xa = ((double) grow_w)/ ((double) grow_h);
        }
	}


	if (grow_filled > 1) {
		growat = randomMT() % (grow_filled-1);
		next = grow;
		while ( (next != NULL) && (growat != 0) ) {
			next = next->next;
			growat--;
		}
	} else {
		next = grow;
	}

	temp = rfrac() * 1000;
	angle = (int) temp % 1000;

	dx = grow_xa * cos ((2*3.14*angle/1000) );
	dy = grow_ya * sin ((2*3.14*angle/1000) );
	x = next->x + dx;
	y = next->y + dy;

	while (MapData((int) x, (int) y) == MAP_FILLED) {
		x += dx;
		y += dy;
	}
	if (  ((int) x > grow_maxx) || ((int) y > grow_maxy) ||
        ((int) x < grow_minx) || ((int) y < grow_miny) ) {
		return changeList;
	}

	chg = new MapBlockChange();
	chg->x = (int) x;
	chg->y = (int) y;
	chg->newchar = MAP_FILLED;
	chg->oldchar = MapData((int) x, (int) y);
	changeList->Add(chg);

	next = grow;
	grow = (grow_t *) malloc(sizeof(grow_t));
	grow->x = (int) x;
	grow->y = (int) y;
	grow->next = next;
	grow_filled++;

	return changeList;
}
///////////////////////////////////////////////////////////////////////////////
ObjList * MapDocument::RotateMapArea()
{
	char icon;
	int x,y,i,j;
	int xbegin, xend, ybegin, yend, xlen, ylen, temp,
		xcent, ycent;
	MapBlockChange *chg = NULL;
	ObjList *changeList = new ObjList();

	EmptyTempData();

	if (fSelected)
	{
		SortSelectionArea();
		xbegin = seldxbeg;
		xend = seldxend;
		xlen = seldxend - seldxbeg;
		ybegin = seldybeg;
		yend = seldyend;
		ylen = seldyend - seldybeg;

	}
	else
	{
		xbegin = 0;
		xend = optionsDoc->mapWidth->GetInt();;
		ybegin = 0;
		yend = optionsDoc->mapHeight->GetInt();;
	}

	//fill the clipdata
	for (x = xbegin, i = 0; x < xend; x++, i++)
		for (y = ybegin, j = 0; y < yend; y++, j++)
		{
			if (!InsideMap(x, y))
				continue;
			clipdata[i][j] = MapData(x, y);
		}

        //empty the square
        for(x = xbegin; x < xend; x++)
			for(y = ybegin; y < yend; y++)
			{
				if (!InsideMap(x, y))
					continue;
				chg = new MapBlockChange();
				chg->x = x;
				chg->y = y;
				chg->newchar = MAP_SPACE;
				chg->oldchar = MapData(x, y);
				changeList->Add(chg);
			}

			if (!fSelected)
				if (optionsDoc->mapWidth->GetInt() != optionsDoc->mapHeight->GetInt() )
				{
					temp = xend;
					xend = yend;
					yend = temp;
				}

				//If this is a selected area, then it may not be a perfect square...we need
				//to be able to rotate rectangles as well, so find the center of the selected area,
				//and position from there.
				if (fSelected)
				{
					xcent = xbegin + xlen/2;
					ycent = ybegin + ylen/2;
					xbegin = xcent - ylen/2;
					ybegin = ycent - xlen/2;
					xend = xbegin + ylen;
					yend = ybegin + xlen;
					seldxend = xend;
					seldxbeg = xbegin;
					seldyend = yend;
					seldybeg = ybegin;
				}

				// rotate the clipdata and refill the main map
				for (x = xbegin, i = xend-xbegin-1; x < xend; x++, i--)
					for (y = ybegin, j=0; y < yend; y++, j++)
					{
                        switch(clipdata[j][i])
                        {
                        case MAP_REC_RD :
							icon = MAP_REC_LD;
							break;
                        case MAP_REC_LD :
							icon = MAP_REC_LU;
							break;
                        case MAP_REC_LU :
							icon = MAP_REC_RU;
							break;
                        case MAP_REC_RU :
							icon = MAP_REC_RD;
							break;
                        case MAP_CAN_LEFT :
							icon = MAP_CAN_UP;
							break;
                        case MAP_CAN_UP :
							icon = MAP_CAN_RIGHT;
							break;
                        case MAP_CAN_RIGHT :
							icon = MAP_CAN_DOWN;
							break;
                        case MAP_CAN_DOWN :
							icon = MAP_CAN_LEFT;
							break;
                        case MAP_CRNT_UP :
							icon = MAP_CRNT_RT;
							break;
                        case MAP_CRNT_RT :
							icon = MAP_CRNT_DN;
							break;
                        case MAP_CRNT_DN :
							icon = MAP_CRNT_LT;
							break;
                        case MAP_CRNT_LT :
							icon = MAP_CRNT_UP;
							break;
                        case MAP_DEC_RD :
							icon = MAP_DEC_LD;
							break;
                        case MAP_DEC_LD :
							icon = MAP_DEC_LU;
							break;
                        case MAP_DEC_LU :
							icon = MAP_DEC_RU;
							break;
                        case MAP_DEC_RU :
							icon = MAP_DEC_RD;
							break;
                        default :
							icon = clipdata[j][i];
							break;

                        }
						chg = new MapBlockChange();
						chg->x = x;
						chg->y = y;
						chg->newchar = icon;
						chg->oldchar = MapData(x, y);
						changeList->Add(chg);
					}

	return changeList;
}
///////////////////////////////////////////////////////////////////////////////
ObjList * MapDocument::dragArea(int transx, int transy, bool drop)
//   transx: Distance to translate in x direction
//   transy: Distance to translate in y direction
//   drop: Drop at the specified translocate distance
{
	int x,y,i,j;
	int xbegin, xend, ybegin, yend;
	ObjList *changeList = new ObjList();

	//If we're not dropping the data, we must be storing it in the
	//clipdata array.
	if (!drop)
	{
		EmptyTempData();
		SortSelectionArea();
		xbegin = seldxbeg;
		xend = seldxend;
		ybegin = seldybeg;
		yend = seldyend;

		//fill the clipdata map
        for (x = xbegin, i = 0; x < xend; x++, i++)
			for (y = ybegin, j = 0; y < yend; y++, j++)
			{
				if (!InsideMap(x, y))
					continue;
				clipdata[i][j] = MapData(x, y);
			}
			return NULL;
	}

	//We are dropping the data, so don't store anything.
	//Make sure we aren't off the left or top of the map.
	if (drop)
	{
		if (transx < 0)
		{
			xbegin = seldxbeg+transx;
			xend = seldxend+transx;
		}
		else
		{
			xbegin = seldxbeg+transx;
			xend = seldxend+transx;
		}


		if (transy < 0)
		{
			ybegin = seldybeg+transy;
			yend = seldyend+transy;
		}
		else
		{
			ybegin = seldybeg+transy;
			yend = seldyend+transy;
		}

		for(x = seldxbeg; x < seldxend; x++)
			for(y = seldybeg; y < seldyend; y++)
			{
				MapBlockChange *chg = new MapBlockChange();
				chg->x = x;
				chg->y = y;
				chg->newchar = MAP_SPACE;
				chg->oldchar = MapData(x, y);
				changeList->Add(chg);
			}

			for(x = xbegin, i = 0; x < xend; x++, i++)
				for(y = ybegin, j = 0; y < yend; y++, j++)
				{
					if (!InsideMap(x, y))
						continue;

					MapBlockChange *chg = new MapBlockChange();
					chg->x = x;
					chg->y = y;
					chg->newchar = clipdata[i][j];
					chg->oldchar = MapData(x, y);
					changeList->Add(chg);
				}
                seldxbeg = xbegin;
                seldxend = xend;
                seldybeg = ybegin;
                seldyend = yend;
	}
	return changeList;
}
///////////////////////////////////////////////////////////////////////////////
ObjList * MapDocument::FillClipData(bool cut)
//   cut: Is this a cut or a copy?
{

	int x,y,i,j,xbegin,ybegin,xend,yend;
	char tmp;
	ObjList *changeList = new ObjList();

	clipboard.EmptyClipData();


	SortSelectionArea();
	xbegin = seldxbeg;
	xend = seldxend;
	ybegin = seldybeg;
	yend = seldyend;


	clipboard.cWidth = xend - xbegin;
	clipboard.cHeight = yend - ybegin;

	for (x=0, i=xbegin; i < xend; x++, i++)
		for(y=0, j=ybegin; j < yend; y++, j++)
		{
			if (!InsideMap(i, j))
				continue;
			tmp = MapData(i, j);

			clipboard.SetData(x, y, tmp);
			if (cut)
			{
			MapBlockChange *chg = new MapBlockChange();
			chg->x = i;
			chg->y = j;
			chg->newchar = MAP_SPACE;
			chg->oldchar = MapData(i, j);
			changeList->Add(chg);
			}

		}

	return changeList;
}

///////////////////////////////////////////////////////////////////////////////
ObjList * MapDocument::PasteData(int xbeg, int ybeg)
{
	int x,y,i,j;
	char tmp;
	ObjList *changeList = new ObjList();

	for (x=xbeg, i = 0; i < clipboard.cWidth; x++, i++)
		for (y = ybeg, j = 0; j < clipboard.cHeight; y++, j++)
		{
			if (!InsideMap(i, j))
				continue;
			tmp = MapData(i, j);

			MapBlockChange *chg = new MapBlockChange();
			chg->x = x;
			chg->y = y;
			chg->newchar = clipboard.GetData(i, j);
			chg->oldchar = MapData(x, y);
			changeList->Add(chg);
		}
		seldxbeg = xbeg;
		seldxend = xbeg+clipboard.cWidth;
		seldybeg = ybeg;
		seldyend = ybeg+clipboard.cHeight;

		return changeList;
}

///////////////////////////////////////////////////////////////////////////////
int MapDocument::WildMap()
{
	WildMap::MakeWildMap(optionsDoc);
	optionsDoc->OptionsChanged();
	return 0;
}
