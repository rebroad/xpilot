/* $Id: Fl_ServerList.h,v 1.16 2004/05/13 09:14:22 dick Exp $
 *
 * XPwhere - Where in the world can i find people playing XPilot.
 *      Copyright (C) 2001 by
 *
 *      The XPilot Authors   <xpilot@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
 *      Jarrod Miller        <jarrod@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
 *      Bjørn Stabell        <bjoern@xpilot.org>
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
 * $Log: Fl_ServerList.h,v $
 * Revision 1.16  2004/05/13 09:14:22  dick
 * MarkAllInactive() before PrivateServer updates them
 *
 * Revision 1.15  2004/02/29 03:05:04  dick
 * Use the glogal xpblue and xpred
 *
 * Revision 1.14  2004/02/09 18:18:46  dick
 * Fix columns so they are resizable.
 *
 * Revision 1.13  2002/09/02 15:38:28  dick
 * Pass a ServerListCfg to Fl_ServerList which contains all of the user config.
 * Use this ServerListCfg to determine which columns are on and their widths.
 *
 * Revision 1.12  2002/08/28 13:29:31  dick
 * Add "Lag" column.  Use ErrMsgHandler for message output.
 *
 * Revision 1.11  2002/08/20 16:31:24  jlmiller
 * Move column const's to guiconst.h
 *
 * Revision 1.10  2002/08/16 18:52:24  jlmiller
 * First pass as configurable columns
 *
 * Revision 1.9  2002/07/17 18:27:52  jlmiller
 * Set up resizable columns, draw cell lines with standard style.
 *
 * Revision 1.8  2002/06/09 18:36:50  dick
 * Display players on a server.
 *
 * Revision 1.7  2001/08/08 08:02:06  dick
 * Support tag for delete later
 *
 * Revision 1.6  2001/08/06 05:41:50  dick
 * struct sock_t becomes class Sock
 *
 * Revision 1.5  2001/07/29 23:40:49  dick
 * Linux gcc tweaks.  The newer versions of g++ has stricter (and different)
 * requirements.
 *
 * Revision 1.4  2001/07/26 23:34:22  dick
 * Draw in XPilot colors. (still needs work)
 *
 * Revision 1.3  2001/07/26 16:21:21  dick
 * Use Full row select.
 *
 * Revision 1.2  2001/07/26 12:13:12  dick
 * Linux XPwhere integration cleanup
 *
 * Revision 1.1  2001/07/26 10:52:11  dick
 * A table widget to display a ServerList
 *
 */

#ifndef	_XPFLSERVERLIST_H_
#define	_XPFLSERVERLIST_H_


#include "FL/Flv_Table.H"
#include "../ServerSite.h"
#include "../ServerList.h"
#include "ServerListCfg.h"
#include "guiconst.h"
#include "types.h"


#define	Fl_ServerListSUPERCLASS	Flv_Table
class Fl_ServerList : public Fl_ServerListSUPERCLASS
{
public:
	Fl_ServerList( int X, int Y, int W, int H, const char *l=0 );
	virtual ~Fl_ServerList();
	void		SetErrMsgHandler(ErrMsgHandler _emh, void* _emhThis);
	void		SetServerList(ServerList* sl);
	void		SetCfg(ServerListCfg* slc);

	void		ShowPlayers(bool show = true);
	bool		GetSelected(ServerSite& si);
	ServerSite* GetServerAtRow(int R, int& baseRow);
	ServerSite* GetServerAtRow(int R);
	int			GetRowCount();		// # of rows displayed (may include players)

	void		draw_row( int Offset, int &X, int &Y, int &W, int &H, int R );
	virtual void draw_cell( int Offset, int &X, int &Y, int &W, int &H, int R, int C );
	void		get_style( Flv_Style &s, int R, int C );
//	virtual int col_width( int C );
	virtual	int handle(int);

	ServerList*	GetServerList();
	void		ServerListStateChanged(PCSTR status);
//	void		InitDefaultColumns();
	void		SetColumnSizesFromCfg();
	void		SetColumnSizesToCfg();
	void		MarkAllInactive();

	void		FlCheckHandler();

	bool		tagForDelete;
public:
	ServerListCfg*	cfg;

private:
	ServerList*		serverList;
	bool			showPlayers;
	String			GetText(int R, int C);

protected:
	ErrMsgHandler	emh;
	void*			emhThis;

};

inline ServerList* Fl_ServerList::GetServerList() { return(serverList); };
inline void Fl_ServerList::ShowPlayers(bool show) { showPlayers = show; };
#endif
