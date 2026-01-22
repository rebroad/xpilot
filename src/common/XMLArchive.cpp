/* $Id: XMLArchive.cpp,v 1.8 2005/03/17 22:12:13 kps Exp $
 *
 *  Extend Archive to do XML writing.
 *  (XML reading is done by the expat library)
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
 * $Log: XMLArchive.cpp,v $
 * Revision 1.8  2005/03/17 22:12:13  kps
 * Get rid of warnings from makedepend about "non-portable whitespace".
 *
 * Revision 1.7  2004/09/14 00:22:07  dick
 * Remove unneeded includes
 *
 * Revision 1.6  2004/05/01 19:10:40  dick
 * Float is a float string
 *
 * Revision 1.5  2004/02/13 05:11:47  dick
 * Encode strings in and out of XML with Enbracket() and Debracket().
 * It's a very bad thing to have '<' as data in the file.
 * (although its in quotes, so why should it matter?)
 *
 * Revision 1.4  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.3  2002/06/26 17:39:42  dick
 * Wrap debugging with a D()
 *
 * Revision 1.2  2002/06/26 04:48:46  dick
 * Includes cleanup for Linux
 *
 * Revision 1.1  2002/06/25 04:35:49  dick
 * Wrap Archive with XML handling.
 *
 */
#include "StdAfx.h"

#ifdef	_WINDOWS
#	include <windows.h>
#else
#	include <string.h>
#endif

#include "portability.h"
#include "XMLArchive.h"

extern const char* s_true;
extern const char* s_false;

#if 0
#	define	D(__d)	__d
#else
#	define	D(__d)
#endif
///////////////////////////////////////////////////////////////////////////////
bool ParseBool(PCSTR s)
{
	if (!strcasecmp(s, s_true))
		return(true);
	return(false);
}

///////////////////////////////////////////////////////////////////////////////
int ParseInt(PCSTR s)
{
	return(atoi(s));
}

///////////////////////////////////////////////////////////////////////////////
double ParseFloat(PCSTR s)
{
	return(atof(s));
}

///////////////////////////////////////////////////////////////////////////////
String ParseString(PCSTR s)
{
	String	cs;
	cs = s;
	cs.Debracket();
	return(cs);
}

///////////////////////////////////////////////////////////////////////////////
Rect ParseRect(PCSTR s)
{
	Rect	r;
	String	cs(s);
	sscanf(cs, "%d %d %d %d", &r.left, &r.top, &r.right, &r.bottom);
	return(r);
}

///////////////////////////////////////////////////////////////////////////////
void XMLArchive::WriteHead()
{
	WriteString("<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>\n");
	level = 0;
}

///////////////////////////////////////////////////////////////////////////////
void XMLArchive::WriteIndent()
{
	int i;
	for (i=0; i<level; i++)
		WriteString("\t");
}

///////////////////////////////////////////////////////////////////////////////
void XMLArchive::_WriteElementStart(PCSTR s)
{
	String cs;
	cs.printf("<%s>\n", s);
	WriteIndent();
	XMLArchiveSUPERCLASS::WriteString(cs);
	level++;
	D(Trace("_Start: level=%d\n", level);)
}

///////////////////////////////////////////////////////////////////////////////
void XMLArchive::_WriteElementEnd(PCSTR s)
{
	String cs;
	level--;
	cs.printf("</%s>\n", s);
	WriteIndent();
	XMLArchiveSUPERCLASS::WriteString(cs);
	D(Trace("_End: level=%d\n", level);)
}

///////////////////////////////////////////////////////////////////////////////
void XMLArchive::WriteString(PCSTR s, ...)
{
	String	cs;
	va_list argList;
	va_start(argList, s);
	cs.vprintf(s, argList);
	va_end(argList);
	XMLArchiveSUPERCLASS::WriteString(cs);
}

///////////////////////////////////////////////////////////////////////////////
void XMLArchive::_WriteBoolAttr(PCSTR s, int b)
{
	String cs;
	cs.printf("<%s bool=\"%s\"/>\n", s, b ? s_true : s_false);
	WriteIndent();
	XMLArchiveSUPERCLASS::WriteString(cs);
}

///////////////////////////////////////////////////////////////////////////////
void XMLArchive::_WriteIntAttr(PCSTR s, int b)
{
	String cs;
	cs.printf("<%s int=\"%d\"/>\n", s, b);
	WriteIndent();
	XMLArchiveSUPERCLASS::WriteString(cs);
}

///////////////////////////////////////////////////////////////////////////////
void XMLArchive::_WriteFloatAttr(PCSTR s, double b)
{
	String cs;
	cs.printf("<%s float=\"%f\"/>\n", s, b);
	WriteIndent();
	XMLArchiveSUPERCLASS::WriteString(cs);
}

///////////////////////////////////////////////////////////////////////////////
void XMLArchive::_WriteStringAttr(PCSTR s, PCSTR b)
{
	String cs;
	String es(b);
	es.Enbracket();
	cs.printf("<%s string=\"%s\"/>\n", s, (PCSTR)es);
	WriteIndent();
	XMLArchiveSUPERCLASS::WriteString(cs);
}

///////////////////////////////////////////////////////////////////////////////
void XMLArchive::_WriteRectAttr(PCSTR s, const Rect& r)
{
	String cs;
	cs.printf("<%s rect=\"%d %d %d %d\"/>\n", s, r.left, r.top, r.right, r.bottom);
	WriteIndent();
	XMLArchiveSUPERCLASS::WriteString(cs);
}

