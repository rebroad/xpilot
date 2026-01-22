/* $Id: Fl_ServerList.cpp,v 1.40 2006/09/24 04:24:16 dick Exp $
 *
 * XPwhere - Where in the world can i find people playing XPilot.
 *      Copyright (C) 2001 by
 *
 *      The XPilot Authors   <xpilot@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
 *      Jarrod Miller        <jarrod@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
 *      Bj�rn Stabell        <bjoern@xpilot.org>
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
 * $Log: Fl_ServerList.cpp,v $
 * Revision 1.40  2006/09/24 04:24:16  dick
 * Last line missing LF
 *
 * Revision 1.39  2004/05/14 21:15:35  dick
 * Handle PrivateServerList updates
 *
 * Revision 1.38  2004/05/14 20:44:43  dick
 * Don't do the Select callback if it was just a FLVE_ROW_CHANGED
 *
 * Revision 1.37  2004/05/13 09:14:22  dick
 * MarkAllInactive() before PrivateServer updates them
 *
 * Revision 1.36  2004/05/10 06:24:18  dick
 * Default is 0 rows of data
 *
 * Revision 1.35  2004/02/29 03:05:04  dick
 * Use the glogal xpblue and xpred
 *
 * Revision 1.34  2004/02/10 18:20:18  dick
 * If the user clicks a column header, then sort on that column
 *
 * Revision 1.33  2004/02/09 18:18:46  dick
 * Fix columns so they are resizable.
 *
 * Revision 1.32  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.31  2004/01/26 03:25:22  jlmiller
 * Allow double-clicking a server to SERVERJOINGAME
 *
 * Revision 1.30  2002/09/05 17:53:16  dick
 * Whitespace
 *
 * Revision 1.29  2002/09/03 22:57:19  jlmiller
 * Lets not always FORCE the column size to the default width, if its already shown and >= the default.
 *
 * Revision 1.28  2002/09/02 15:38:28  dick
 * Pass a ServerListCfg to Fl_ServerList which contains all of the user config.
 * Use this ServerListCfg to determine which columns are on and their widths.
 *
 * Revision 1.27  2002/08/30 05:21:14  dick
 * serverList contains ObjSortables now.
 *
 * Revision 1.26  2002/08/29 04:04:52  dick
 * Notify the parent window when we receive data (may cause team buttons to change)
 *
 * Revision 1.25  2002/08/28 13:29:31  dick
 * Add "Lag" column.  Use ErrMsgHandler for message output.
 *
 * Revision 1.24  2002/08/20 16:31:24  jlmiller
 * Move column const's to guiconst.h
 *
 * Revision 1.23  2002/08/16 18:52:24  jlmiller
 * First pass as configurable columns
 *
 * Revision 1.22  2002/08/15 18:45:34  jlmiller
 * Get selected crashed if called without explicitly selecting a server. This will at least
 * keep it from crashing. Default selection needs to be fixed.
 *
 * Revision 1.21  2002/08/02 13:57:01  jlmiller
 * Add a new event, SELECTSERVER, so we can update XPWhereWindow dynamically.
 *
 * Revision 1.20  2002/08/01 15:14:51  dick
 * Comment out noisy mouse move debug
 *
 * Revision 1.19  2002/07/19 13:30:01  jlmiller
 * Go back to drawing cell lines correctly. Players should be shown in same cell as server visually.
 *
 * Revision 1.18  2002/07/17 18:27:52  jlmiller
 * Set up resizable columns, draw cell lines with standard style.
 *
 * Revision 1.17  2002/07/16 19:00:41  jlmiller
 * Show all columns, for now.
 *
 * Revision 1.16  2002/07/09 04:29:14  dick
 * so becomes sos (It's ServerOptions, not ServerOption)
 *
 * Revision 1.15  2002/07/08 18:17:24  dick
 * Wrap debug with a D()
 *
 * Revision 1.14  2002/06/26 07:07:17  dick
 * A naive attempt to add the port number to the command line to the Windows client.
 *
 * Revision 1.13  2002/06/23 06:23:52  dick
 * FLTK widgets use ServerOptionsNetClientFl instead of ServerOptionsNetClient.
 *
 * Revision 1.12  2002/06/09 18:36:50  dick
 * Display players on a server.
 *
 * Revision 1.11  2002/06/01 18:46:57  jlmiller
 * Loop to fill the user_data pointer.
 *
 * Revision 1.10  2002/06/01 05:46:00  dick
 * Comment out the player drawing test code
 *
 * Revision 1.9  2002/05/30 03:33:40  jlmiller
 *
 * Linux cleanups for compilation after reorg of ServerList
 *
 * Revision 1.8  2002/05/29 20:32:23  jlmiller
 * ServerList Widget can join or admin anywhere.
 *
 * Revision 1.7  2001/08/08 08:02:06  dick
 * Support tag for delete later
 *
 * Revision 1.6  2001/07/29 23:40:49  dick
 * Linux gcc tweaks.  The newer versions of g++ has stricter (and different)
 * requirements.
 *
 * Revision 1.5  2001/07/27 06:34:07  dick
 * Add a popup menu
 *
 * Revision 1.4  2001/07/26 23:34:22  dick
 * Draw in XPilot colors. (still needs work)
 *
 * Revision 1.3  2001/07/26 16:21:21  dick
 * Use Full row select.
 *
 * Revision 1.2  2001/07/26 13:03:05  dick
 * Add non-blocking to the socket.
 *
 * Revision 1.1  2001/07/26 10:52:11  dick
 * A table widget to display a ServerList
 *
 */
#include "StdAfx.h"

#ifndef	_WINDOWS
#include <unistd.h>
#endif

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Menu_Button.H>

#include "Fl_XPColors.h"

#include "config.h"
#include "FlEventNames.h"
#include "Fl_ServerList.h"
#include "ServerSite.h"
#include "guiconst.h"
#include "ServerOptionsNetClientFl.h"

void CheckCallback(void* who);

static	const char*	coltitles[] = {
	"server",
	"map",
	"author",
	"size",
	"#",
	"lag",
	"b",
	"t",
	"fps",
	"ver",
	"snd",
	"uptime",
	"IP",
	"race",
	"port"
};

static void Fl_ServerListCallBack(Fl_Widget* me, void* v);
static void MotdCallBack(Fl_Widget* me, void*);
static void StatusCallBack(Fl_Widget* me, void*);

Fl_Menu_Item serverListPopup[] = {
	{"Show &MOTD", 0, MotdCallBack},
	{"Server &Status", 0, StatusCallBack},
	{0},
};

///////////////////////////////////////////////////////////////////////////////
Fl_ServerList::Fl_ServerList(int X, int Y, int W, int H, const char* l)
 : Flv_Table(X,Y,W,H,l)
{
	int i;
	tagForDelete = false;
	serverList = NULL;
	rows(0);
	cols(15);
	feature_add(FLVF_ROW_HEADER|FLVF_ROW_SELECT);
	Fl::add_check(CheckCallback, this);
	selection_color(xpred);

	//Be sure we know which server list to come back to later in the callback
	//This may not be the best way to convey this info, but its the best
	//I can come up with for now.
	for (i = 0; i < sizeof(serverListPopup) / sizeof(Fl_Menu_Item); i++)
		serverListPopup[i].user_data(this);

	Fl_Menu_Button* menu = new Fl_Menu_Button(X, Y, W, H);
	menu->menu(serverListPopup);
	menu->type(Fl_Menu_Button::POPUP3);
	callback(Fl_ServerListCallBack);

	// InitDefaultColumns();
}

///////////////////////////////////////////////////////////////////////////////
Fl_ServerList::~Fl_ServerList()
{
	Fl::remove_check(CheckCallback, this);
	delete serverList;
}

///////////////////////////////////////////////////////////////////////////////
int Fl_ServerList::handle(int flags)
{
	int ret;
	int TX, TY;
	ret = Fl_ServerListSUPERCLASS::handle(flags);
	//D(PCSTR ev = GetFlEventName(flags);
	//			printf("Fl_ServerList::handle: ev=%s\n", ev);)
	Fl_Window* tmp = (Fl_Window *) parent();

	switch (flags)
	{
	case FL_PUSH:

		// Broken attempt to set the selection on button3
		if (Fl::event_button3() && !Fl::event_button3())
		{
			Fl::e_state |= FL_BUTTON1;
			Fl_ServerListSUPERCLASS::handle(flags);
			Fl::e_state &= ~FL_BUTTON1;
		}
		TY = Fl::event_y();
		TX = Fl::event_x();
		if (get_row(TX,TY) == -1)
		{
			int col = get_col(TX,TY);
			serverList->Sort(col);
			redraw();
			return(TRUE);
		}
		if (Fl::event_button1() || Fl::event_button3())
		{
			//ret = Fl_ServerListSUPERCLASS::handle(flags);
			int baseRow;
			int row = select_start_row();
			GetServerAtRow(row, baseRow);
			if (row != baseRow)
			{
				select_start_row(baseRow);
				redraw();
				return(1);
			}
			tmp->handle(SELECTSERVER);
			return(ret);
		}
		break;
	case SERVERJOINGAME:
		tmp->handle(SERVERJOINGAME);
		break;
	case FL_RELEASE:
		//return(0);
	case FL_DRAG:
		SetColumnSizesToCfg();
	default:
		break;
	}
	//ret = Fl_ServerListSUPERCLASS::handle(flags);
	return(ret);
}

///////////////////////////////////////////////////////////////////////////////
void Fl_ServerList::SetServerList(ServerList* sl)
{
	serverList = sl;
	sl->SetWindow(this);
	sl->SetErrMsgHandler(emh, emhThis);
}

///////////////////////////////////////////////////////////////////////////////
void Fl_ServerList::SetErrMsgHandler(ErrMsgHandler _emh, void* _emhThis)
{
	emh = _emh;
	emhThis = _emhThis;
	if (serverList)
		serverList->SetErrMsgHandler(emh, emhThis);

}

///////////////////////////////////////////////////////////////////////////////
void Fl_ServerList::SetCfg(ServerListCfg* slc)
{
	cfg = slc;
	SetColumnSizesFromCfg();
}

///////////////////////////////////////////////////////////////////////////////
void Fl_ServerList::SetColumnSizesFromCfg()
{
	int i;

	for (i = 0; i < numServerListColumns; i++)
	{
		if (cfg->columns[i])
			col_width(cfg->colSizes[i], i);
		else
			col_width(0, i);
	}
}

///////////////////////////////////////////////////////////////////////////////
void Fl_ServerList::SetColumnSizesToCfg()
{
	int i;

	for (i=0; i < numServerListColumns; i++)
	{
		int	width = col_width(i);
		if (cfg->columns[i])
		{
			cfg->colSizes[i] = width;
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
void CheckCallback(void* who)
{
	Fl_ServerList* me = (Fl_ServerList*)who;
	me->FlCheckHandler();
}

///////////////////////////////////////////////////////////////////////////////
void Fl_ServerList::FlCheckHandler()
{
	if (serverList)
		serverList->FlCheckHandler();
}

///////////////////////////////////////////////////////////////////////////////
//	Note: This is so flexible, you don't *have* to use style
//	if you'd rather program the conditions...
void Fl_ServerList::get_style( Flv_Style &s, int R, int C )
{

	Fl_ServerListSUPERCLASS::get_style(s,R,C);			//	Get standard style
	if (R<0)											//	Heading/Footing is bold
		s.font( (Fl_Font)(s.font()+FL_BOLD));
	s.background(xpblue);
	s.foreground(FL_WHITE);
	s.resizable(true);
//	s.border(FLVB_OUTER_ALL);						//	Nice dark box
}

///////////////////////////////////////////////////////////////////////////////
void Fl_ServerList::draw_row( int Offset, int &X, int &Y, int &W, int &H, int R )
{
	if (R == -2)
	{
		Flv_Style s;
		get_style(s,R,0);

		Fl_ServerListSUPERCLASS::draw_row(Offset, X, Y, W, H, R);
		fl_color(FL_BLACK);
		fl_draw("Server List", X-Offset+1, Y+1, W, H, s.align() );
		fl_color(FL_WHITE);
		fl_draw("Server List", X-Offset, Y, W, H, s.align() );
	}
	else
	{
		int	baseRow;
		ServerSite* ss = GetServerAtRow(R, baseRow);
		int	myOffset = 0;
		if (showPlayers && R >= 0 && baseRow != R)
		{
			Flv_Style s;
			get_style(s,R,0);
			String	text = GetText(R,0);

//			int dX, dY, dW, dH;

			//	Calculate clipping height
//			client_area(dX,dY,dW,dH);
//			dH = row_height(R);
//			fl_clip(dX, dY, dW, dH);

			fl_color(xpblue);
			fl_rectf(X, Y, W, H);

			Offset -= 20;
			fl_color(FL_BLACK);
			fl_draw(text, X-Offset+1, Y+1, W, H, s.align() ,0, 0);
			fl_color(FL_WHITE);
			fl_draw(text, X-Offset, Y, W, H, s.align() ,0, 0);
//			fl_pop_clip();
			return;
		}
		Fl_ServerListSUPERCLASS::draw_row(Offset, X, Y, W, H, R);
	}
}

///////////////////////////////////////////////////////////////////////////////
void Fl_ServerList::draw_cell( int Offset, int &X, int &Y, int &W, int &H, int R, int C )
{
	Flv_Style s;

	get_style(s,R,C);
	Fl_ServerListSUPERCLASS::draw_cell(Offset,X,Y,W,H,R,C);

	String	text = GetText(R,C);
	ServerSite* ss = GetServerAtRow(R);

	fl_color(FL_BLACK);
	fl_line(X-Offset,Y,X-Offset+W+1,Y);
	fl_draw(text, X-Offset+1, Y+1, W, H, s.align() ,0, 0);
	if (ss)
		fl_color(ss->color);
	else
		fl_color(FL_WHITE);
	fl_draw(text, X-Offset, Y, W, H, s.align() ,0, 0);

//	cfg->colSizes[C] = col_width(C);		// flush the column width back, in case it changed
}


///////////////////////////////////////////////////////////////////////////////
void Fl_ServerList::ServerListStateChanged(PCSTR status)
{
	rows(GetRowCount());
	redraw();
	Fl_Window* tmp = (Fl_Window*)parent();
	tmp->handle(SELECTSERVER);

}

///////////////////////////////////////////////////////////////////////////////
bool	Fl_ServerList::GetSelected(ServerSite& si)
{
	int		R = select_start_row();
	ServerSite* ss = GetServerAtRow(R);
	if (ss)
	{
		si = *ss;
	}
	return(true);
}
///////////////////////////////////////////////////////////////////////////////
ServerSite* Fl_ServerList::GetServerAtRow(int R, int& baseRow)
{
	ServerSite*		ss;
	ObjSortable*	obj;
	int				i;

	for (obj=(ObjSortable*)serverList->GetHead(), i=0; obj; obj=(ObjSortable*)obj->GetNext(), i++)
	{
		ss = (ServerSite*)obj->o;

		if (i == R)
		{
			baseRow = i;
			return(ss);
		}
		if (showPlayers && i < R && i+ss->playerCount >= R)
		{
			baseRow = i;
			return(ss);
		}
		if (showPlayers)
			i += ss->playerCount;
	}
	return(NULL);
}

///////////////////////////////////////////////////////////////////////////////
ServerSite* Fl_ServerList::GetServerAtRow(int R)
{
	int throwaway;
	return(GetServerAtRow(R, throwaway));
}

///////////////////////////////////////////////////////////////////////////////
String	Fl_ServerList::GetText(int R, int C)
{
	String	cs;
	int		baseRow;

	if (R == -1)
	{
		cs = coltitles[C];
		return(cs);
	}
	ServerSite* ss = GetServerAtRow(R, baseRow);
	if (!ss)
	{
		cs = "??";
		return(cs);
	}

	if (baseRow != R)
	{
		if (showPlayers && !C)
		{
			ServerSitePlayer* ssp = (ServerSitePlayer*)ss->playerList[R-baseRow-1];
			if (!ssp)
			{
				cs = "PlayerError";
				return(cs);
			}
			cs = ssp->name;
			return(cs);
		}
		else
		{
			return(cs);
		}

	}

	switch (C)
	{
	case COL_SERVERNAME:
		cs = ss->serverName;
		break;
	case COL_MAPNAME:
		cs = ss->mapName;
		break;
	case COL_MAPAUTHOR:
		cs = ss->author;
		break;
	case COL_MAPSIZE:
		cs = ss->mapSize;
		break;
	case COL_PLAYERCOUNT:
		cs = ss->playerCount;
		break;
	case COL_BASES:
		cs = ss->bases;
		break;
	case COL_TEAMS:
		cs = ss->teams;
		break;
	case COL_FPS:
		cs = ss->fps;
		break;
	case COL_VERSION:
		cs = ss->version;
		break;
	case COL_SOUND:
		cs = ss->sound;
		break;
	case COL_IP:
		cs = ss->serverIP;
		break;
	case COL_RACE:
		cs = ss->timing ? "Y" : "N";
		break;
	case COL_UPTIME:
		int	secs, mins, hrs, days;
		secs = ss->uptime % 60;
		mins = ss->uptime / 60;
		mins = mins % 60;
		hrs = ss->uptime / 3600;
		days = hrs / 24;
		hrs  = hrs % 24;
		cs.printf("%d:%02d:%02d.%02d", days, hrs, mins, secs);
		break;
	case COL_PORT:
		cs = ss->serverPort;
		break;
	case COL_LAG:
		if (ss->pingTime == PING_UNKNOWN)
			cs = "";
		else if (ss->pingTime == PING_NORESP)
			cs = "---";
		else
			cs = ss->pingTime;
		break;
	}
	return(cs);
}

///////////////////////////////////////////////////////////////////////////////
int Fl_ServerList::GetRowCount()
{
	int count = serverList->GetCount();
	if (showPlayers)
	{
		for (ObjSortable* os = (ObjSortable*)serverList->GetHead();
			 os;
			 os = (ObjSortable*)os->GetNext())
		{
			count += ((ServerSite*)os->o)->playerCount;
		}
	}
	return(count);
}

///////////////////////////////////////////////////////////////////////////////
static void Fl_ServerListCallBack(Fl_Widget* me, void* v)
{
	Fl_ServerList* fsl = (Fl_ServerList*)me;
//	D(printf("why_event = %d\n", fsl->why_event());)
	if (fsl->why_event() == FLVE_SELECTION_CHANGED && Fl::event_clicks() == 1)		// double click?
	{
		Fl::event_clicks(0);
	}
	else if (fsl->why_event() == FLVE_ROW_CHANGED)
		return;
	else if (Fl::event_clicks() == 1)
	{
		fsl->handle(SERVERJOINGAME);
	}
}

///////////////////////////////////////////////////////////////////////////////
static void MotdCallBack(Fl_Widget* me, void*)
{
}

///////////////////////////////////////////////////////////////////////////////
static void StatusCallBack(Fl_Widget* me, void*)
{
}

///////////////////////////////////////////////////////////////////////////////
void Fl_ServerList::MarkAllInactive()
{
	for (ObjSortable* os = (ObjSortable*)serverList->GetHead();
		 os;
		 os = (ObjSortable*)os->GetNext())
	{
		((ServerSite*)os->o)->color = xpgrey;
		((ServerSite*)os->o)->inactive = true;
	}
}
