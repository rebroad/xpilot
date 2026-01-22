/* $Id: CfgBase.h,v 1.5 2004/01/30 15:59:41 dick Exp $
 *
 * http type configuration
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
 * $Log: CfgBase.h,v $
 * Revision 1.5  2004/01/30 15:59:41  dick
 * Deadwood delete
 *
 * Revision 1.4  2004/01/06 04:48:27  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 * Revision 1.3  2002/06/26 06:57:57  dick
 * deadwood delete
 *
 * Revision 1.2  2002/06/26 04:51:49  dick
 * use Obj.h, not obj.h
 *
 * Revision 1.1  2002/06/25 04:37:01  dick
 * An application configuration base class.  This should probably go away in
 * favor of something like Ini.  But this one uses XML.
 *
 */


#ifndef	_CFGBASE_H_
#define	_CFGBASE_H_

/*
	Each application that links to CfgBase should set up a config object
	and set it with bsrCfg::SetCfg(myCfg);
*/

#include "cstring.h"
#include "Obj.h"

#define	LOGNONE		0		// log nothing
#define	LOGERR		1		// log only errors
#define	LOGMIN		2		// minimal info
#define	LOGMED		3
#define	LOGLOTS		4
#define	LOGMAX		5		// ludicrous speed!

#define	LOGAPPEND		0
#define	LOGOVERWRITE	1

//#define	ACCESS_SECURE	0
//#define	ACCESS_LIMITED	1
//#define	ACCESS_PUBLIC	2


//////////////////////////////////////////////////////////
// The root cfg

class Http;

class CfgBase {
  public:
	CfgBase();
	virtual ~CfgBase();
//	void Load(cfgscanf* cfgsf, const char* filename);
//	virtual void Save() = 0;

	static	CfgBase* GetCfg() { return(theCfg); };
	static	void SetCfg(CfgBase* cfg);

	virtual Obj* ObjectFactory(const char* name) = 0;
	// each application uses a subclass of http, which needs to be instantiated from BuckoRoot
	virtual Http* HttpFactory() = 0;


	int logLevel;		// LOGNONE to LOGMAX
	int	logToFile;		// TRUE/FALSE
	int	logType;		// LOGAPPEND or LOGOVERWRITE
	int	webPort;
	int	udpPort;
	int	adminAccess;	// see auth* in http.h
	int	readAccess;
	int	writeAccess;
	int	refreshRate;
	int	liveLogEntries;
	int	dirtyDelay;			// how long to wait before flushing databases

	int	totalUpTime;		// how long we've existed
	int	totalUpTimeAtStart;	// how long we existed when we started (needed for new calcs)

	String	bootDirectory;
	String	welcomeHtml;	// home page
	String	helpDirectory;

// not persistent
	const char*	appname;	// !!must be initialized to get ini name to load the rest of cfg
	String	htmlDirectory;	// subdirectory of the html files
	int	startTime;			// when we booted

#if defined	(_WINDOWS) && !defined(_CYGWIN)
	HWND	hWnd;		// the main window for the app
#endif
  private:
	static	CfgBase*	theCfg;

};

inline CfgBase*	GetCfg() { return(CfgBase::GetCfg()); };

extern	PCSTR		GetServerName();

#endif		// _CFGBASE_H_
