/* $Id: CfgBase.cpp,v 1.6 2004/02/03 03:39:52 dick Exp $
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
 * $Log: CfgBase.cpp,v $
 * Revision 1.6  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.5  2004/01/06 04:48:27  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 * Revision 1.4  2002/07/09 22:55:26  dick
 * xpprintf uses showtime()
 *
 * Revision 1.3  2002/06/26 06:57:57  dick
 * deadwood delete
 *
 * Revision 1.2  2002/06/26 04:51:26  dick
 * Deadwood delete
 *
 * Revision 1.1  2002/06/25 04:37:01  dick
 * An application configuration base class.  This should probably go away in
 * favor of something like Ini.  But this one uses XML.
 *
 */
#include "StdAfx.h"

#ifdef	_WINDOWS
#include <windows.h>
#endif

#include "CfgBase.h"
#include "Http.h"
#include "xpprintf.h"

extern int defaultHttpPort;

CfgBase*	CfgBase::theCfg;

///////////////////////////////////////////////////////////////////////////////
CfgBase::CfgBase()
{
	logLevel = LOGLOTS;
	logToFile = TRUE;
	webPort = defaultHttpPort;
	adminAccess = authPublic;
	readAccess = authPublic;
	writeAccess = authPublic;
	refreshRate = 60;
	liveLogEntries = 200;
	dirtyDelay = 600;

#if defined(_WINDOWS) && !defined(_CYGWIN)
	hWnd = NULL;		// must be overwritten by app for messages in the window
#endif
}

///////////////////////////////////////////////////////////////////////////////
CfgBase::~CfgBase()
{
}

///////////////////////////////////////////////////////////////////////////////
Obj* CfgBase::ObjectFactory(const char* name)
{
	Obj* o = NULL;
//	if (!strcmp(name, s_user))
//		o = new bsuser;
	return o;
}

///////////////////////////////////////////////////////////////////////////////
void CfgBase::SetCfg(CfgBase* cfg)
{
	theCfg = cfg;
	if (!cfg->bootDirectory.GetLength())
	{
		xpprintf("no boot directory\n");
	}
	if (GetCfg()->logLevel >= LOGMED)
		xpprintf("%s%s: bootDirectory %s\n", showtime(), 
			(PCSTR)cfg->appname, (PCSTR)cfg->bootDirectory);
}

///////////////////////////////////////////////////////////////////////////////
PCSTR	GetServerName()
{
//	PCSTR	s = strchr(GetCfg()->name, ':');
//	if (s)
//		return(s+1);
	return("??");
}
