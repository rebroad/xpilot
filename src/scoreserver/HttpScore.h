/***************************************************************************\
*  HttpScore.h - The http server for the Score server						*
*																			*
*  server/																	*
*																			*
*  Part of the BuckoSoft Animation Controller								*
*  Copyright© 1998 - BuckoSoft, Inc.										*
*																			*
*  $Id: HttpScore.h,v 1.5 2004/02/05 04:53:53 dick Exp $						*
\***************************************************************************/

/*
 *  $Log: HttpScore.h,v $
 *  Revision 1.5  2004/02/05 04:53:53  dick
 *  TokenSubst() can take ServerOptions and put them on the web page.
 *
 *  Revision 1.4  2004/01/30 16:02:49  dick
 *  Configure the scoreserver via the web interface.
 *  Break out the engine configuration to it's own object; we don't want
 *  to have the baggage of a big unused score engine loaded in memory if we
 *  really are only interested in it's name and description.
 *
 *  Revision 1.3  2002/07/14 19:07:45  dick
 *  Toggle whether we're displaying robots in the list or not.
 *
 *  Revision 1.2  2002/06/30 06:14:48  dick
 *  Add sorting.
 *
 *  Revision 1.1  2002/06/25 05:10:55  dick
 *  Subclass the Http server to interface the ScoreServer to the WebServer.
 *
 *  Revision 1.27  2002/02/10 05:54:49  dick
 *  New httpc::frd does frame display, picture and text.
 *  Add support for always sending the .bse (povray output) file if project
 *  specs wantText.  frd filters on @@begin and @@end if wantDebugOnly is
 *  speced.
 *
 *  Revision 1.26  2002/02/03 15:46:19  dick
 *  Add plugin engines to the server.
 *
 *  Revision 1.25  2002/01/15 15:37:17  dick
 *  Server Availability configuration
 *
 *  Revision 1.24  2002/01/13 16:16:50  dick
 *  Command textOutput becomes remoteStatus, which also now prints the "rendered
 *  and held" list.
 *
 *  Revision 1.23  2002/01/04 19:37:01  dick
 *  New GetClient() gets an sclient from the headers.  ProjectCheck now uses it.
 *
 *  Revision 1.22  2001/12/26 03:11:39  dick
 *  TextOutput command now displays the server status (with a red background
 *  because it will probably be on a remote client).
 *
 *  Revision 1.21  1999/12/15 14:11:26  dick
 *  Getting meta to define projects on the server
 *
 *  Revision 1.20  1999/11/30 07:55:53  dick
 *  Use the new DoAgentCommand handling type with the implied command name.
 *
 *  Revision 1.19  1999/11/27 02:51:10  dick
 *  Add live log entries to the bsac application files.
 *
 */

#ifndef	_HTTPSCORE_H_
#define	_HTTPSCORE_H_

#include "Http.h"

class SEPlayer;

class HttpScore : public Http
{
  public:
	HttpScore();
	virtual	~HttpScore();

	virtual void DoCommand();
	//virtual void DoAgentCommand();

  private:
	void DoCommandAbout();
	void DoCommandConfig();
	void DoCommandAdmin();
	void DoCommandCfgEngine();
	void DoCommandScoreList();
//	void DoCommandStatus();
	void DoCommandLogs();
	void DoCommandLogMem();
//	void DoCommandLogAll();

	void	HandlePlayerLine();
	void	HandleRobotToggle();
	void	HandleRobotFlag();
	void	HandlePlayer(const ObjList& opts);
	void	HandleServerOpt(const ObjList& opts, const String soptname);

	virtual void PrintServer();	
	virtual	void TokenSubst(String s);


//	void PrintStatus(bool remote=false);
//	void PrintStatusHead(bool remote);
	void PrintLogHead(const char* which);
	void PrintConfigHead(PCSTR which);
//	void PrintIntStat(const char* h, int i);


	ObjSortable*	currentLine;		// which line of the sorted array we are on
	int				currentLineNumber;
	SEPlayer*		currentPlayer;		// which player is in that line
	ObjList*		rankList;			// sort by rank
	ObjList*		sortedList;			// sort by user request
	ObjList*		playerList;			// the list of players we are grokking (may exclude bots)
};

#endif		// _HTTPSCORE_H_
