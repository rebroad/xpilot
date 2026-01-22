/* $Id: password.cpp,v 1.5 2004/02/03 03:39:52 dick Exp $
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 2002 by
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
 */
/*
 * $Log: password.cpp,v $
 * Revision 1.5  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.4  2004/01/04 06:01:03  dick
 * No newline at end of file
 *
 * Revision 1.3  2002/08/01 14:47:41  dick
 * Add support for a cookie file.
 *
 * Revision 1.2  2002/07/24 06:00:58  dick
 * Set the password file to chmod(0600) after writing it.
 *
 * Revision 1.1  2002/07/23 17:35:50  dick
 * Read the ServerPassword and XPilotPasswords files.  Write to XPilotPasswords.
 *
 */
#include "StdAfx.h"

#include <string.h>

#include "cstring.h"
#include "types.h"
#include "Obj.h"
#include "config.h"
#include "password.h"
#include "Archive.h"

PCSTR	s_localhost = "localhost";

///////////////////////////////////////////////////////////////////////////////
static String GetPassword(PCSTR fname, PCSTR host, int port)
{
	String	s;
	String	cs;
	Archive	file;
	long	ret;

	if (!strcmp(host, s_localhost))
	{
		file.SetFileName(Conf_server_password_file_name());
		file.SeekTo(9);		// skip "password:"
		ret = file.Read(s, 64);
		if (ret >= 0)
			s.SetAt(ret, '\0');
		s.TrimTrailingWhiteSpace();
		return(s);
	}
	// read the whole passwords file
	file.SetFileName(fname);
	ret = file.SeekToEnd();
	if (!ret)
		return(s);
	file.SeekToBegin();
	ret = file.Read(cs, ret);
	cs.SetAt(ret, '\0');
	ObjList lines;
	cs.ToObjList(&lines);
	lines.ParseHeaders(' ');
	LString*	line = (LString*)lines.GetHead();
	String	t;
	t.printf("%s:%d", host, port);
	while (line)
	{
		if (!strcmp(line->name, t))
		{
			s = line->s;
			return(s);
		}
		line = (LString*)line->GetNext();
	}
	return(s);	// empty string
}

///////////////////////////////////////////////////////////////////////////////
String GetPassword(PCSTR host, int port)
{
	return(GetPassword(Conf_client_passwords_file_name(), host, port));
}

///////////////////////////////////////////////////////////////////////////////
int GetCookie(PCSTR host, int port)
{
	String s = GetPassword(Conf_client_cookies_file_name(), host, port);
	return(s.atoi());
}

///////////////////////////////////////////////////////////////////////////////
static bool SetPassword(PCSTR fname, PCSTR host, int port, PCSTR password, String& error)
{
	String	s;
	String	cs;
	Archive	file;
	ObjList lines;
	long	ret;

	// read the whole passwords file
	file.SetFileName(fname);
	ret = file.SeekToEnd();
	if (ret)
	{
		file.SeekToBegin();
		ret = file.Read(cs, ret);
		file.Close();
		cs.SetAt(ret, '\0');
		cs.ToObjList(&lines);
		lines.ParseHeaders(' ');
	}
	LString*	line = (LString*)lines.GetHead();
	String	t;
	t.printf("%s:%d", host, port);
	bool found = false;
	while (line)
	{
		if (!strcmp(line->name, t))
		{
			if (!strcmp(line->s, password))
				return(true);		// password didn't change, don't rewrite the file
			line->s = password;
			found = true;
			break;
		}
		line = (LString*)line->GetNext();
	}
	if (!found)
	{
		lines.Add(t, password);
	}
	if (!file.OpenCreate())
	{
		return(false);
	}
	line = (LString*)lines.GetHead();
	while (line)
	{
		s.printf("%s %s\n", (PCSTR)line->name, (PCSTR)line->s);
		file.WriteString(s);
		line = (LString*)line->GetNext();
	}
	file.Chmod(0600);
	return(true);
}

bool SetPassword(PCSTR host, int port, PCSTR password, String& error)
{
	PCSTR f = Conf_client_passwords_file_name();
	return(SetPassword(f, host, port, password, error));
}

bool SetCookie(PCSTR host, int port, uint cookie, String& error)
{
	PCSTR f = Conf_client_cookies_file_name();
	String	s;
	s.printf("%d", cookie);

	return(SetPassword(f, host, port, (PCSTR)s, error));

}
