/* $Id: ScoreServerCfg.h,v 1.3 2004/01/28 00:00:21 dick Exp $
 *
 * XPScoreServer - Who's on first?
 * Configuration handler
 *
 *      Copyright (C) 2002 by
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
 * $Log: ScoreServerCfg.h,v $
 * Revision 1.3  2004/01/28 00:00:21  dick
 * Correctly determine and use GetScoreServerDirectory() for our writable data.
 *
 * Revision 1.2  2002/06/30 06:14:48  dick
 * Add sorting.
 *
 * Revision 1.1  2002/06/25 05:15:12  dick
 * Provide ScoreServer configuration by subclassing CfgBase.
 *
 */

#ifndef	_SCORESERVERCFG_H_
#define	_SCORESERVERCFG_H_

#include "Obj.h"
#include "CfgBase.h"
#include "XMLArchive.h"

#define	SCORE_SERVER_PORT	15346

xarDefineToken(ScoreServerCfg);

class ScoreServerCfg : public CfgBase {
public:
	ScoreServerCfg();

	Obj*	ObjectFactory(PCSTR name);
	Http*	HttpFactory();		// spit out a new http

	xarOPERATORS;


	String	webHost;			// name of our server (i.e. hostname)
	String	webMsg;				// A message displayed at client login
	bool	wantPlayMessages;	// Receive the game messages (useful for debugging Engines)

	String	xpServerName;		// name of server we connect to
	int		xpServerPort;		// his contact port
	String	xpServerPasswd;		// password allowing us to connect

	String	scoreEngine;		// Plugin score engine to use
#ifdef	_WINDOWS
	Rect	window;				// main window coordinates
	int		windowMode;			// 0 = normal, 1 = minimized
#endif


protected:
	xarHANDLERS;

};

//extern ScoreServerCfg	cfg;

#endif	/* _SCORESERVERCFG_H_ */
