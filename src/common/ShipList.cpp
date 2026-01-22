/* $Id: ShipList.cpp,v 1.10 2004/07/22 17:01:54 dick Exp $
 *
 * ShipList - Extend an ObjList to manage ShipDocument(s).
 *
 * common - The common library for XPilot
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
 * $Log: ShipList.cpp,v $
 * Revision 1.10  2004/07/22 17:01:54  dick
 * Add the default ship to the head of the list when loading.  Skip the first ship
 * (default ship) when saving.
 *
 * Revision 1.9  2004/05/28 17:38:03  dick
 * Convert_ship_2_string outputs a String instead of a String*,
 * (prevents memory leaks and overwrites).
 *
 * Revision 1.8  2004/05/13 16:21:19  dick
 * Linux needs to have a shipfile too.
 * Note on Windows, XPilot.shp moves to $/lib .
 *
 * Revision 1.7  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.6  2004/01/13 20:31:31  jlmiller
 * Store the ShipFileName when we load a shipfile, for future use.
 *
 * Revision 1.5  2002/09/10 19:03:44  jlmiller
 * Add copy constructor for cloning of ships & shiplists.
 *
 * Revision 1.4  2002/08/23 15:58:04  dick
 * The filename is a const for ShipList.
 *
 * Revision 1.3  2002/07/17 19:42:59  jlmiller
 * Be sure LoadShipFile was passed a string.
 *
 * Revision 1.2  2002/07/15 19:10:20  jlmiller
 * Ini should handle shipShapeFile as a item
 *
 * Revision 1.1  2002/06/25 20:21:06  jlmiller
 * Add to common.
 *
 * Revision 1.8  2002/06/25 15:02:19  jlmiller
 * Move iSelection fields to the window, instead of the individual document,
 * as they didn't make any sense at document level.
 *
 * Revision 1.7  2002/05/28 07:19:50  dick
 * Rename xpilotedit to XPilotControl
 *
 */
#include "StdAfx.h"

#include <string.h>

#include "ShipList.h"
#include "const.h"
#include "config.h"

///////////////////////////////////////////////////////////////////////////////
ShipList::ShipList()
{
	pCurrentShip = NULL;
	changed = false;
}

///////////////////////////////////////////////////////////////////////////////
int ShipList::LoadShipFile(PCSTR filename)
{
	FILE *fin = NULL;
	ShipDocument* sd;
	char a[500];

	D(xpprintf("ShipList: load \"%s\"\n", filename);)

	Empty();

	sd = new ShipDocument(true);		// always put the default ship first
	Add(sd);

	if (filename)
	{
		fin=fopen(filename,"r");
		if (fin)
		{
			while(getoneshipdef(a, fin))
			{
				sd=new ShipDocument(a);
				Add(sd);
			}
			fclose(fin);
		}

		ShipFileName = filename;

		pCurrentShip = (ShipDocument*)GetHead();
	}
	return TRUE;

}

///////////////////////////////////////////////////////////////////////////////
/* Get a ship from the shiplib file.  Name into sname, ship into sship */
/* if same=TRUE, file will not reset, close on subsequent calls */
int ShipList::getoneshipdef(char *sship, FILE *fin)
{
	char *sd;
	static char c[500], d[800];

	if ((fin==NULL)||(feof(fin))) return FALSE;
	sship[0]='\0';
	fscanf(fin,"%[^\n]",c);
	fscanf(fin,"%*[\n]");
	sd=strchr(c,'\\');
	while (sd!=NULL) {
		*sd='\0';
		fscanf(fin,"%[^\n]",d);
		fscanf(fin,"%*[\n]");
		strcat(c, d);
		sd=strchr(c,'\\');
	}
	strcpy(sship, c);
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
void ShipList::ClearShipList()
{
	ShipDocument *tmpShp = (ShipDocument *) GetHead();
	int count = GetCount();
	int i;

	if (count == 0)
		return;

	for (i = 0; i < count; i++)
	{
		Remove(tmpShp);
		delete(tmpShp);
		tmpShp = (ShipDocument *) GetHead();
	}
}

///////////////////////////////////////////////////////////////////////////////
void ShipList::CloneShip(ShipDocument *shp)
{
	ShipDocument *tmpShp = shp->copy();

	tmpShp->name += "_2";
	if (tmpShp)
		Add(tmpShp);
	pCurrentShip = tmpShp;
}
///////////////////////////////////////////////////////////////////////////////
/* save or print the ship */
bool ShipList::SaveShipFile(PCSTR filename)
{
	ShipDocument *shp = NULL;
	String		ship;
	FILE*		fout;

	shp = (ShipDocument *) GetHead();

	if (!shp)								//Shiplist is empty
		return true;
	shp = (ShipDocument*)shp->GetNext();	// skip the default ship
	if (!shp)
		return(true);						// only the default ship, don't save

	fout = fopen(filename, "wb");

	while (shp != NULL)
	{
		ship = shp->Convert_ship_2_string();
		fprintf(fout,"%s\n",(PCSTR)ship);
		shp = (ShipDocument *) shp->GetNext();
	}

	if (fout) fclose(fout);
	return true;
}

