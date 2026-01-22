/* $Id: MapDocument.h,v 1.27 2004/05/30 16:11:38 dick Exp $
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
 * $Log: MapDocument.h,v $
 * Revision 1.27  2004/05/30 16:11:38  dick
 * Use randommt() instead of rand()
 *
 * Revision 1.26  2004/02/21 06:57:32  dick
 * In the mapeditor, the prefs should have no knowledge of a ServerOptionsNetClient.
 *
 */

#ifndef	_MAPDOCUMENT_H_
#define	_MAPDOCUMENT_H_

#include "ServerOptions.h"
#include "XpPoint.h"


//structures and macro's for tracking bases, checkpoints.
//Used when finding the closest teams for treasure, target
//ownership...etc
typedef struct {
    XpPoint	pos;
    unsigned int team;
} base_t;

class XpTeamBase : public XpPoint {
public:
	unsigned int team;
};

typedef struct {
//    int NumBases;
//    base_t	*base;
	ObjList bases;
    int NumChecks;
    int checkpoints[26];
} world_items_t;

typedef struct grow_t {
   int                   x,y;
   struct grow_t         *next;
} grow_t;

class MapDocument
{
private:
        world_items_t mapItems;

public:
        ServerOptions*  optionsDoc;
        bool changed; //Has the current map changed?
        int seldxbeg, seldybeg, seldxend, seldyend;

        bool fSelected; //Do we have geometry selected
        int strdx, strdy; //Used for dragging.

        XpPoint ptTempBeg, ptTempEnd;


public: //Member Functions
		MapDocument(); //Default Constructor
		~MapDocument(); //Default Destructor

        bool SaveMap(const char *pstrFileName);
		bool SaveXbmFile(const char *);
        bool SaveXpmFile(const char *);
        bool SavePbmPlusFile(const char *, int);

        bool LoadMap(const char *pstrFileName);
        bool LoadXbmFile(const char *);
        bool LoadPbmFile(const char *);

        void CountBases();
        void CountCheckPoints();
        char FindNextCheckPoint();
		char FindQuickCanDir(int, int);

		//Functions should return a list of changes instead of directly changing
		//the MapData.
        ObjList * BlockLine(XpPoint, XpPoint, char);
        ObjList * BlockCircle(XpPoint, XpPoint, char, int);
        ObjList * BlockRectangle(XpPoint, XpPoint, char, int);
		ObjList * ClearMapArea(int);
        ObjList * RoundMapArea(ObjList *);
		ObjList * GrowMapArea(ObjList *);
        ObjList * RotateMapArea();
        ObjList * IncrementMapBlock(int, int);
        ObjList * MirrorMap(bool);
        ObjList * CycleMapHorizontal(bool);
        ObjList * CycleMapVertical(bool);
        ObjList * NegativeMap();
        ObjList * FillClipData(bool);
        ObjList * PasteData(int, int);
        ObjList * dragArea(int, int, bool);
        ObjList * FillArea(int, int, int);

//        void FillArea(int, int, int);
        void SelectGrid(int, int, int, bool);
        void ChangeMapData(int, int, char);
		void ChangeMapBackData(int, int, int, int);
        void SwapMapSize();
        void SortSelectionArea();
        char MapData(int, int);
        bool InsideMap(int, int);
        void build_image(char *, int);
        char *convert_map();
        int Find_closest_team(int, int);
        double Wrap_length(double, double);
        int WildMap();
		void ResetMap();

private:
        void CleanMap();
};

#endif	// _MAPDOCUMENT_H_
