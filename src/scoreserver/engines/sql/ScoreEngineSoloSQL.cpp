/* $Id: ScoreEngineSoloSQL.cpp,v 1.2 2007/01/30 06:55:01 dick Exp $
 *
 * XPScoreServer - Who's on first?
 * ScoreEngineSoloSQL - A score server engine that extends ScoreEngine
 *                      by keeping our data in an SQL Database
 *
 *      Copyright (C) 2007 by
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
 * $Log: ScoreEngineSoloSQL.cpp,v $
 * Revision 1.2  2007/01/30 06:55:01  dick
 * Don't pass objects through ...
 *
 * Revision 1.1  2007/01/29 04:52:19  dick
 * The first SQL Engine for the ScoreServer
 *
 */
#include "StdAfx.h"

#include <mysql.h>

#include <string.h>
#include "xpprintf.h"
#include "portability.h"
#include "const.h"
#include "config.h"
#include "error.h"
#include "XMLArchive.h"
#include "ScoreEngineSoloSQL.h"
#include "ScoreEngineSoloSQLCfg.h"
#include "../../ScoreServer.h"
#include "../../HttpScore.h"

PCSTR ScoreEngineSoloSQL::name = "SoloSQL";
PCSTR ScoreEngineSoloSQL::title  = "Solo SQL";
PCSTR ScoreEngineSoloSQL::description =
"This engine extends the basic score server and keeps statistics in an "
"XML file that is read at startup and saved at shutdown.";

PCSTR	s_SoloSQL	= ScoreEngineSoloSQL::name;

PCSTR	s_BasicSQL	= ScoreEngineSoloSQL::name;


///////////////////////////////////////////////////////////////////////////////
// Extend a ScoreEnginePlayer with some SQL handling
class SEBPlayerSQL	: public SEBPlayer
{
  public:
	SEBPlayerSQL();
	SEBPlayerSQL(PCSTR nick, PlayerType pt,
  				 PCSTR real, PCSTR host, PCSTR addr, uint cookie);

	int		pid;
	String	real;
	String	host;

};

///////////////////////////////////////////////////////////////////////////////
SEBPlayerSQL::SEBPlayerSQL() : SEBPlayer() { pid = -1; }

///////////////////////////////////////////////////////////////////////////////
SEBPlayerSQL::SEBPlayerSQL(PCSTR nick, PlayerType pt,
  				 PCSTR real, PCSTR host, PCSTR addr, uint cookie)
{
	pid = -1;
	this->name = nick;
	this->playerType = pt;
	this->cookie = cookie;
}

///////////////////////////////////////////////////////////////////////////////
// private implementation of a ScoreEngineSoloSQL
class SESSEng {
  public:
	SESSEng() { myData = NULL; }
	void		ConnectToMySQL();
	void		DisconnectFromMySQL();
	bool		LoadAllPlayers();
	bool		LoadAllSummaryStats();
	SEBPlayerSQL* FindPlayer(PCSTR nick, PlayerType pt);
	SEBPlayerSQL* FindPlayerByPid(int pid);
	SEBPlayerSQL* SQLFindPlayer(PCSTR nick, PlayerType pt);
	SEBPlayerSQL* SQLNewPlayer(PCSTR nick, PlayerType pt,
							   PCSTR real, PCSTR host, PCSTR addr, uint cookie);
	bool		SQLInsertPlayer(SEBPlayerSQL* pl);
	bool		SQLUpdateSummaryStats(SEBPlayerSQL* pl);

	ScoreEngineSoloSQL*	parent;

  private:
	MYSQL*	myData;
};

//#define	myDATA	(((SESSEng*)eng)->myData)
#define	GetEng()	((SESSEng*)eng)

PCSTR	s_DB = "xpilot5";		// database name


///////////////////////////////////////////////////////////////////////////////
ScoreEngineSoloSQL::ScoreEngineSoloSQL(ScoreEngineCfg* _cfg)
: ScoreEngineSoloSQLSUPERCLASS(_cfg)
{
	cfg = (ScoreEngineSoloSQLCfg*)_cfg;
	eng = new SESSEng();
	GetEng()->parent = this;
}

///////////////////////////////////////////////////////////////////////////////
ScoreEngineSoloSQL::~ScoreEngineSoloSQL()
{
}

///////////////////////////////////////////////////////////////////////////////
// static
ScoreEngineCfg* ScoreEngineSoloSQL::Install()
{
	ScoreEngineCfg* def = new ScoreEngineSoloSQLCfg;
	def->name = name;
	def->title = title;
	def->description = description;

	return(def);
}

///////////////////////////////////////////////////////////////////////////////
void ScoreEngineSoloSQL::Startup()
{
	GetEng()->ConnectToMySQL();
	GetEng()->LoadAllPlayers();
	GetEng()->LoadAllSummaryStats();
	ScoreEngineSoloSQLSUPERCLASS::Startup();
}

///////////////////////////////////////////////////////////////////////////////
void ScoreEngineSoloSQL::Shutdown()
{
	GetEng()->DisconnectFromMySQL();
//	SaveData();
}

///////////////////////////////////////////////////////////////////////////////
void ScoreEngineSoloSQL::TimerTick()
{
	ScoreEngineSoloSQLSUPERCLASS::TimerTick();
	/*
	if (!dirtyTime)
		return;
	time_t now = time(NULL);
	if (dirtyTime + cfg->flushDelay < now)
	{
		SaveData();
		dirtyTime = 0;
	}
	*/
}

///////////////////////////////////////////////////////////////////////////////
bool ScoreEngineSoloSQL::OnAdminPrintTop(HttpScore* http)
{
	return(false);
}

///////////////////////////////////////////////////////////////////////////////
bool ScoreEngineSoloSQL::OnAdminPrintTable(HttpScore* http)
{
	ScoreEngineSoloSQLSUPERCLASS::OnAdminPrintTable(http);
//	http->printf("<TR><TD>Flush Cycle</TD>"
//				 "<TD><INPUT TYPE=text NAME=%s size=5 value=%d></TD>\n",
//			s_flushDelay, cfg->flushDelay);
//	http->printf("<TD>How long to delay between disk saves</TD></TR>\n");
	return(false);
}

///////////////////////////////////////////////////////////////////////////////
bool ScoreEngineSoloSQL::OnAdminSubmit(HttpScore* http)
{
//	LString*	l;

	////////////////////////////////////////////////
//	if (l = (LString*)http->args.Find(s_flushDelay))
//		sscanf(l->s, "%d", &cfg->flushDelay);
	return(false);
}


///////////////////////////////////////////////////////////////////////////////
bool ScoreEngineSoloSQL::GetInitialScore(PCSTR nick, PlayerType pt, uint& cookie,
								 DFLOAT& score, int& kills, int& deaths)
{
	SEBPlayerSQL*	player = GetEng()->FindPlayer(nick, pt);
	return(ScoreEngineSoloSQLSUPERCLASS::GetInitialScore(nick, pt, cookie,
														  score, kills, deaths));
}

///////////////////////////////////////////////////////////////////////////////
bool ScoreEngineSoloSQL::SetPlayerValues(PCSTR nick, PlayerType pt, uint cookie,
								 DFLOAT  score, int  kills, int  deaths)
{
	return(ScoreEngineSoloSQLSUPERCLASS::SetPlayerValues(nick, pt, cookie,
														  score, kills, deaths));
}

///////////////////////////////////////////////////////////////////////////////
void ScoreEngineSoloSQL::ReceivePlayerEvent(PCSTR name, PlayerType pt, ScorePlayerEvent spe,
									PCSTR real, PCSTR host, PCSTR addr, uint cookie)
{
	SEBPlayerSQL* pl = GetEng()->FindPlayer(name, pt);
	if (!pl && spe == Join)
	{
		pl = GetEng()->SQLFindPlayer(name, pt);
		if (!pl)
			pl = GetEng()->SQLNewPlayer(name, pt, real, host, addr, cookie);
		if (cookie < COOKIESPECIAL)
			pl->cookie = MakeNewCookie();
		else
			pl->cookie = cookie;
		playerList.Add(pl);
		xpprintf(LOGMIN, "%sEngine: PlayerEvent Join: Creating new player %d: '%s'\n",
			showtime(), pl->pid, (PCSTR)pl->name);
		ObjSortable* os = new ObjSortable;
		os->o = pl;
		rankList.Add(os);
		rankList.Sort(PlayerSortByRank);
	}
	if (!pl)
	{
		xpprintf(LOGERR, "%sEngine: PlayerEvent %s Can't find player.%s type %s\n",
			showtime(),
			spe < ScorePlayerEventMAX ? ScorePlayerEventText[spe]
									  : ScorePlayerEventText[ScorePlayerEventMAX],
			name,
			pt < PlayerTypeMAX ? PlayerTypeText[pt]
							   : PlayerTypeText[PlayerTypeMAX]);
		return;
	}
	time_t now = time(NULL);
	pl->lastVisit = now;
	switch (spe)
	{
	case Join:
		pl->sessions++;
		pl->startTime = now;
		pl->isPlaying = true;
		xpprintf(LOGMED, "%sEngine: Player %s is joining for the %d time\n",
			showtime(), (PCSTR)pl->GetName(), pl->sessions);
		SendRank(pl);
		break;
	case Quit:
		if (!pl->isPlaying)
		{
			xpprintf(LOGERR, "%sEngine: Player %s quit and we didn't know he was playing\n",
				showtime(), (PCSTR)pl->GetName());
		}
		else
		{
			pl->playTime += (now - pl->startTime);
			pl->isPlaying = false;
			xpprintf(LOGMED, "%sEngine: Player %s left after playing for %d seconds\n",
				showtime(), (PCSTR)pl->GetName(), (now - pl->startTime));
		}
		break;
	case Pause:
	case Unpause:
		xpprintf(LOGERR, "%sNeed to handle pause/unpause\n", showtime());
		break;
	}
	GetEng()->SQLUpdateSummaryStats(pl);
}
///////////////////////////////////////////////////////////////////////////////
void ScoreEngineSoloSQL::ReceiveScoreEvent(PCSTR killer, PlayerType ptr, DFLOAT wscore,
								   PCSTR killee, PlayerType pte, DFLOAT lscore,
								   ScoreType st)
{
	ScoreEngineSoloSQLSUPERCLASS::ReceiveScoreEvent(killer, ptr, wscore,
													 killee, pte, lscore,
													 st);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void	SESSEng::ConnectToMySQL() {
	if ( (myData = mysql_init((MYSQL*) 0)) &&
		mysql_real_connect( myData, "localhost", "xp5", "xp5", s_DB, MYSQL_PORT,
							NULL, 0 ) )
	{
		myData->reconnect= 1;
		if ( mysql_select_db( myData, s_DB ) < 0 ) {
			xpprintf( "Can't select the %s database !\n", s_DB ) ;
			mysql_close(myData);
			myData = NULL;
			return;
		}
	}
	else {
		xpprintf( "Can't connect to the mysql server on port %d !\n",	MYSQL_PORT ) ;
		mysql_close( myData ) ;
		myData = NULL;
		return;
	}
}

///////////////////////////////////////////////////////////////////////////////
void	SESSEng::DisconnectFromMySQL() {
	if (myData)
		mysql_close(myData);
	myData = NULL;
	return;
}

///////////////////////////////////////////////////////////////////////////////
SEBPlayerSQL*	SESSEng::FindPlayer(PCSTR nick, PlayerType pt) {
	SEBPlayerSQL* pl;
	pl = (SEBPlayerSQL*)parent->ScoreEngineSoloSQLSUPERCLASS::FindPlayer(nick, pt);
	if (pl)
		return(pl);
	pl = SQLFindPlayer(nick, pt);
	return(pl);
}

///////////////////////////////////////////////////////////////////////////////
SEBPlayerSQL* SESSEng::FindPlayerByPid(int pid) {
	SEBPlayerSQL*	p = (SEBPlayerSQL*)parent->playerList.GetHead();
	while (p) {
		if (p->pid == pid)
			return(p);
		p = (SEBPlayerSQL*)p->GetNext();
	}
	return(NULL);

}

///////////////////////////////////////////////////////////////////////////////
SEBPlayerSQL*	SESSEng::SQLFindPlayer(PCSTR nick, PlayerType pt) {
	String sql;
	sql.printf("SELECT * FROM Players WHERE nick = \"%s\" AND playerType = \"%d\"",
				nick, pt);
	MYSQL_RES*	res ;
	MYSQL_FIELD* fd ;
	MYSQL_ROW	row ;
	String		fields[ 25 ];
	int			fieldCount;
//	int			i, j, k, l, x ;
	int			i;
	int			x;
	if ( !mysql_query(myData, sql)) {
		res = mysql_store_result(myData);
		i = (int)mysql_num_rows(res);
		if (i == 0) {
			mysql_free_result(res);
			xpprintf("SQLFindPlayer resultSet == 0 rows??\n");
			return(NULL);
		}
		if (i > 1) {
			xpprintf("SQLFindPlayer resultSet is %d rows!\n", i);
		}
		//l = 1;
		xpprintf( "Query:  %s\nNumber of records found:  %ld\n", (PCSTR)sql, i ) ;
		//....we can get the field-specific characteristics here....
		for (x = 0 ; fd = mysql_fetch_field(res) ; x++ )
			fields[ x ] = fd->name;
		fieldCount = x;
		//....
		SEBPlayerSQL* pl = new SEBPlayerSQL();

		while ( row = mysql_fetch_row( res ) ) {
			for (i=0; i<fieldCount; i++) {
				if (!strcmp(fields[i], "pid"))
					pl->pid = atoi(row[i] ? row[i] : "-4");
				else if (!strcmp(fields[i], "nick"))
					pl->name = row[i] ? row[i] : "???";
				else if (!strcmp(fields[i], "real"))
					pl->real = row[i] ? row[i] : "???";
				else if (!strcmp(fields[i], "host"))
					pl->host = row[i] ? row[i] : "???";
				else if (!strcmp(fields[i], "playerType"))
					pl->playerType = (PlayerType)atoi(row[i] ? row[i] : "-1");
				else if (!strcmp(fields[i], "cookie"))
					pl->cookie = atoi(row[i] ? row[i] : 0);
			}
		}
		mysql_free_result( res ) ;
		return(pl);
/*
			j = mysql_num_fields( res ) ;
			xpprintf( "Record #%ld:-\n", l++ ) ;
			for ( k = 0 ; k < j ; k++ )
				xpprintf( "  Fld #%d (%s): %s\n", k + 1, aszFlds[ k ],
						(((row[k]==NULL)||(!strlen(row[k])))?"NULL":row[k])) ;
*/
//			xpprintf( "==============================\n" ) ;
//		}
	}
		return(NULL);
}

///////////////////////////////////////////////////////////////////////////////
bool	SESSEng::LoadAllPlayers() {
	if (!myData)
		return(false);

	String sql;
	sql.printf("SELECT * FROM Players");
	MYSQL_RES*	res ;
	MYSQL_FIELD* fd ;
	MYSQL_ROW	row ;
	String		fields[ 25 ];
	int			fieldCount;
//	int			i, j, k, l, x ;
	int			i;
	int			x;
	if ( !mysql_query(myData, sql)) {
		res = mysql_store_result(myData);
		i = (int)mysql_num_rows(res);
		if (i == 0) {
			mysql_free_result(res);
			xpprintf("SQLFindPlayer resultSet == 0 rows??\n");
			return(false);
		}
		xpprintf( "Query:  %s\nNumber of records found:  %ld\n", (PCSTR)sql, i ) ;
		//....we can get the field-specific characteristics here....
		for (x = 0 ; fd = mysql_fetch_field(res) ; x++ )
			fields[ x ] = fd->name;
		fieldCount = x;
		//....

		while ( row = mysql_fetch_row( res ) ) {
			SEBPlayerSQL* pl = new SEBPlayerSQL();
			for (i=0; i<fieldCount; i++) {
				if (!strcmp(fields[i], "pid"))
					pl->pid = atoi(row[i] ? row[i] : "-4");
				else if (!strcmp(fields[i], "nick"))
					pl->name = row[i] ? row[i] : "???";
				else if (!strcmp(fields[i], "real"))
					pl->real = row[i] ? row[i] : "???";
				else if (!strcmp(fields[i], "host"))
					pl->host = row[i] ? row[i] : "???";
				else if (!strcmp(fields[i], "playerType"))
					pl->playerType = (PlayerType)atoi(row[i] ? row[i] : "-1");
				else if (!strcmp(fields[i], "cookie"))
					pl->cookie = atoi(row[i] ? row[i] : 0);
			}
			parent->playerList.Add(pl);
		}
		mysql_free_result( res );

		return(true);
/*
			j = mysql_num_fields( res ) ;
			xpprintf( "Record #%ld:-\n", l++ ) ;
			for ( k = 0 ; k < j ; k++ )
				xpprintf( "  Fld #%d (%s): %s\n", k + 1, aszFlds[ k ],
						(((row[k]==NULL)||(!strlen(row[k])))?"NULL":row[k])) ;
*/
//			xpprintf( "==============================\n" ) ;
//		}
	}
		return(false);
}

///////////////////////////////////////////////////////////////////////////////
bool	SESSEng::LoadAllSummaryStats() {
	if (!myData)
		return(false);

	String sql;
	sql.printf("SELECT * FROM SummaryStats");
	MYSQL_RES*	res ;
	MYSQL_FIELD* fd ;
	MYSQL_ROW	row ;
	String		fields[ 25 ];
	int			fieldCount;
//	int			i, j, k, l, x ;
	int			i;
	int			x;
	if ( !mysql_query(myData, sql)) {
		res = mysql_store_result(myData);
		i = (int)mysql_num_rows(res);
		if (i == 0) {
			mysql_free_result(res);
			xpprintf("SQLFindPlayer resultSet == 0 rows??\n");
			return(false);
		}
		xpprintf( "Query:  %s\nNumber of records found:  %ld\n", (PCSTR)sql, i ) ;
		//....we can get the field-specific characteristics here....
		for (x = 0 ; fd = mysql_fetch_field(res) ; x++ )
			fields[ x ] = fd->name;
		fieldCount = x;
		//....

		while ( row = mysql_fetch_row( res ) ) {
			if (!row[0])
				continue;
			SEBPlayerSQL* pl = FindPlayerByPid(atoi(row[0]));
			D(Trace("pid = %d\n", pl->pid);)
			if (!pl)
				continue;
			for (i=0; i<fieldCount; i++) {
				if (!strcmp(fields[i], "score"))
					pl->score = atof(row[i] ? row[i] : "0");
				else if (!strcmp(fields[i], "kills"))
					pl->kills = atoi(row[i] ? row[i] : 0);
				else if (!strcmp(fields[i], "deaths"))
					pl->deaths = atoi(row[i] ? row[i] : 0);
				else if (!strcmp(fields[i], "sessions"))
					pl->sessions = atoi(row[i] ? row[i] : 0);
				else if (!strcmp(fields[i], "playTime"))
					pl->playTime = atoi(row[i] ? row[i] : "0");
			}
		}
		mysql_free_result( res );

		return(true);
/*
			j = mysql_num_fields( res ) ;
			xpprintf( "Record #%ld:-\n", l++ ) ;
			for ( k = 0 ; k < j ; k++ )
				xpprintf( "  Fld #%d (%s): %s\n", k + 1, aszFlds[ k ],
						(((row[k]==NULL)||(!strlen(row[k])))?"NULL":row[k])) ;
*/
//			xpprintf( "==============================\n" ) ;
//		}
	}
		return(false);
}

///////////////////////////////////////////////////////////////////////////////
SEBPlayerSQL*	SESSEng::SQLNewPlayer(PCSTR nick, PlayerType pt,
  									  PCSTR real, PCSTR host, PCSTR addr, uint cookie) {
	if (!myData)
		return(new SEBPlayerSQL());
	String sql = "SELECT COUNT(*) FROM Players";
	MYSQL_RES*	res ;
	MYSQL_ROW	row ;
	int			i;
	if ( ! mysql_query( myData, sql ) ) {
		res = mysql_store_result( myData );
		i = (int) mysql_num_rows( res );
		int newpid = -2;
		while ( row = mysql_fetch_row( res ) ) {
			newpid = atoi(row[0] ? row[0] : "-3");
		}
		mysql_free_result( res ) ;
		SEBPlayerSQL* pl = new SEBPlayerSQL(nick, pt, real, host, addr, cookie);
		pl->pid = newpid + 1;
		if (pl->pid > 0)
			SQLInsertPlayer(pl);
		return(pl);
	}
	else
		xpprintf( "Couldn't execute %s on the server !\n", (PCSTR)sql );

	return(new SEBPlayerSQL(nick, pt, real, host, addr, cookie));
}

///////////////////////////////////////////////////////////////////////////////
String	GetSQLTime(time_t when) {
	char	c[80];
	struct tm*	tm;
	tm = localtime(&when);
	strftime(c, 80, "%Y-%m-%d %H:%M:%S", tm);
	String s = c;
	return(s);
};

///////////////////////////////////////////////////////////////////////////////
String	GetNow() {
	time_t	now = time(NULL);
	return(GetSQLTime(now));
};

///////////////////////////////////////////////////////////////////////////////
bool SESSEng::SQLInsertPlayer(SEBPlayerSQL* pl) {
	//INSERT INTO `Players` ( `pid` , `nick` , `real` , `host` , `playerType` , `cookie` , `joinedDate` )
	//VALUES ('2', 'dickless', 'dick', 'voidbuckosoft.com', '0', '2', '2007-01-23 03:18:38'
	if (!myData)
		return(false);
	String	sql = "INSERT INTO Players (`pid`,`nick`,`real`,`host`,`playerType`,`cookie`,`joinedDate` )";
	String	t;
	t.printf("VALUES ('%d','%s','%s','%s','%d','%d', '%s')",
			pl->pid, (PCSTR)pl->name, (PCSTR)pl->real, (PCSTR)pl->host, pl->playerType, 0, (PCSTR)GetNow());
	sql += t;
//	MYSQL_RES*	res ;
//	MYSQL_FIELD* fd ;
//	MYSQL_ROW	row ;
//	char		aszFlds[ 25 ][ 25 ];
//	int			i, j, k, l, x ;
//	int			i;
	if ( ! mysql_query( myData, sql ) ) {
//		res = mysql_store_result( myData );
//		i = (int) mysql_num_rows( res );
		//l = 1;
		/*
		xpprintf( "Query:  %s\nNumber of records found:  %ld\n", sql, i ) ;
		//....we can get the field-specific characteristics here....

		for ( x = 0 ; fd = mysql_fetch_field( res ) ; x++ )
			strcpy( aszFlds[ x ], fd->name ) ;
		//....

		while ( row = mysql_fetch_row( res ) ) {
			j = mysql_num_fields( res ) ;
			xpprintf( "Record #%ld:-\n", l++ ) ;
			for ( k = 0 ; k < j ; k++ )
				xpprintf( "  Fld #%d (%s): %s\n", k + 1, aszFlds[ k ],
						(((row[k]==NULL)||(!strlen(row[k])))?"NULL":row[k])) ;
			xpprintf( "==============================\n" ) ;
		}
		mysql_free_result( res ) ;
		*/
/*		int newpid = -2;
		while ( row = mysql_fetch_row( res ) ) {
			newpid = atoi(row[0] ? row[0] : "-3");
		}
		mysql_free_result( res ) ;
		SEBPlayerSQL* pl = new SEBPlayerSQL(nick, pt, real, host, addr, cookie);
		pl->pid = newpid + 1;
		if (pl->pid > 0)
			SQLInsertPlayer(pl);
*/		return(true);
	}
	else
		xpprintf( "Couldn't execute %s on the server !\n", (PCSTR)sql );
	return(false);
}

///////////////////////////////////////////////////////////////////////////////
bool	SESSEng::SQLUpdateSummaryStats(SEBPlayerSQL* pl) {
	if (!myData)
		return(false);
	String	sql = "Replace INTO SummaryStats (`pid`,`score`,`kills`,`deaths`,`sessions`,`playTime`,`lastVisit`)";
	String	t;
	t.printf("VALUES ('%d','%f','%d','%d','%d','%d','%s')",
			 pl->pid, pl->score, pl->kills, pl->deaths, pl->sessions, pl->playTime,
			 (PCSTR)GetSQLTime(pl->lastVisit));
	sql += t;
	if ( ! mysql_query( myData, sql ) ) {
		//xpprintf( "Executed: %s \n", (PCSTR)sql );
		return(true);
	}
	else
		xpprintf( "Couldn't execute %s on the server !\n", (PCSTR)sql );
	return(false);
}

