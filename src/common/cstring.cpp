/* $Id: cstring.cpp,v 1.32 2005/03/17 22:12:13 kps Exp $
 *
 * cstring - A String class
 *           (silly M$ uses the filename string.* for their class CString)
 *
 * common - The common library for XPilot
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
 * $Log: cstring.cpp,v $
 * Revision 1.32  2005/03/17 22:12:13  kps
 * Get rid of warnings from makedepend about "non-portable whitespace".
 *
 * Revision 1.31  2004/05/30 08:09:32  dick
 * printf returns a reference to itself
 *
 * Revision 1.30  2004/05/14 20:43:52  dick
 * Add Empty()
 *
 * Revision 1.29  2004/05/13 09:06:55  dick
 * New Mid(int start, int length)
 *
 * Revision 1.28  2004/05/10 06:23:21  dick
 * operator+ concats two strings and returns a string.
 * Add some consts.
 *
 * Revision 1.27  2004/04/27 01:16:24  dick
 * RFind(char) does a reverse find
 *
 * Revision 1.26  2004/02/13 05:09:55  dick
 * Add Debracket(), the opposite of Enbracket().  Replaces &lt; and &gt; with < and > .
 *
 * Revision 1.25  2004/02/07 06:29:41  dick
 * Fix bug in ToLower. "*ns++ = tolower(*ns)" is ambiguous.  Bug showed up
 * only in gcc3 optimized compiles.
 *
 * Revision 1.24  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.23  2004/01/19 01:32:54  dick
 * GetFileName shouldn't back up beyond the beginning of the string
 *
 * Revision 1.22  2004/01/09 17:36:12  dick
 * Add operator+=(int) which writes the int as a '%d'
 *
 * Revision 1.21  2004/01/07 04:38:00  dick
 * Add operator!=(PCSTR)
 *
 * Revision 1.20  2003/12/22 06:01:11  dick
 * Correctly convert > to &gt;
 *
 * Revision 1.19  2002/09/12 23:01:26  dick
 * New TrimWhiteSpace() and TrimLeadingWhiteSpace().
 *
 * Revision 1.18  2002/09/12 19:48:14  dick
 * Add GetFileName() which returns the filename component of a full path.
 *
 * Revision 1.17  2002/08/01 14:56:48  dick
 * Preset atoi result to 0
 *
 * Revision 1.16  2002/07/23 17:46:26  dick
 * ToObjList converts a string into a list of LStrings, with one line per
 * LString, CRLF stripped.
 *
 * Revision 1.15  2002/06/25 04:32:59  dick
 * new Format(ObjList opts) formats a string based on the options.  Supported are
 * width, left/right justify, and number of decimals.  This is useful like in
 * a <pre> block.
 *
 * Revision 1.14  2002/06/23 06:20:14  dick
 * SetLength(int n) sets the size of the string buffer to n.
 *
 * Revision 1.13  2002/06/20 22:34:24  dick
 * Linux get va_list from stdarg.h
 *
 * Revision 1.12  2002/06/20 21:53:55  dick
 * Add vprintf()
 *
 * Revision 1.11  2002/06/07 15:40:45  dick
 * 3 funcs Find(c) Left(i) Mid(i) to help with string parsing.
 *
 * Revision 1.10  2002/06/04 20:02:45  dick
 * New TrimTrailingWhiteSpace()
 *
 * Revision 1.9  2002/05/20 07:28:08  dick
 * New function bool EndsWith(char); checks the last char of the string for
 * a match (like does a path end with '/').
 *
 * Revision 1.8  2001/08/12 10:58:24  dick
 * operator==
 *
 * Revision 1.7  2001/08/05 07:57:52  dick
 * Allow 1 and 0 to be equivalent to true and false
 *
 * Revision 1.6  2001/07/26 10:58:00  dick
 * Add header text info
 *
 */
#include "StdAfx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "cstring.h"	//can't use string.h - it's taken
#include "types.h"
#include "Obj.h"

#ifdef	WIN32
#define	vsnprintf	_vsnprintf
#define	snprintf	_snprintf
#endif

// Options supported in the Format() method
PCSTR	s_decimals		= "decimals";
PCSTR	s_width			= "width";
PCSTR	s_just			= "just";
PCSTR	s_left			= "left";
PCSTR	s_right			= "right";

///////////////////////////////////////////////////////////////////////////////
String::String()
{
	s = (char*)malloc(1);
	*s = '\0';
}

///////////////////////////////////////////////////////////////////////////////
String::String(const char* ns, int i)
{
	s = (char*)malloc(i+1);
	strncpy(s, ns, i);
	s[i] = '\0';
}

///////////////////////////////////////////////////////////////////////////////
String::String(int size)
{
	s = (char*)malloc(size);
	s[0] = '\0';
}

///////////////////////////////////////////////////////////////////////////////
String::~String()
{
	free(s);
	s = NULL;
}

///////////////////////////////////////////////////////////////////////////////
String::String(const String& os)
{
	s = (char*)malloc(os.GetLength()+1);
	strcpy(s, os.s);
}

///////////////////////////////////////////////////////////////////////////////
String::String(PCSTR ns)
{
	s = (char*)malloc(strlen(ns)+1);
	strcpy(s, ns);
}

/*
void String::operator+=(PCSTR ns)
{
	char*	newname;
	int		i;
	i = strlen(s);
	i += strlen(ns);
	newname = (char*)malloc(i+1);
	strcpy(newname, s);
	strcat(newname, ns);
	free(s);
	s = newname;
}
*/

///////////////////////////////////////////////////////////////////////////////
void String::Add(PCSTR ns)
{
	char*	newname;
	int		i;
	i = strlen(s);
	i += strlen(ns);
	newname = (char*)malloc(i+1);
	strcpy(newname, s);
	strcat(newname, ns);
	free(s);
	s = newname;
}

///////////////////////////////////////////////////////////////////////////////
void String::operator+=(char c)
{
	char	buf[2];
	buf[0] = c;
	buf[1] = '\0';
	Add(buf);
}

///////////////////////////////////////////////////////////////////////////////
String String::operator+(PCSTR cs)
{
	String	s;
	s = *this;
	s += cs;
	return(s);
}

///////////////////////////////////////////////////////////////////////////////
void String::operator+=(int ni)
{
	char	ns[32];
	sprintf(ns, "%d", ni);
	Add(ns);
}

///////////////////////////////////////////////////////////////////////////////
void String::operator=(PCSTR ns)
{
	int		i;
	free(s);
	i = strlen(ns);
	s = (char*)malloc(i+1);
	strcpy(s, ns);
}

///////////////////////////////////////////////////////////////////////////////
bool String::operator=(char* ns)
{
	int		i;
	free(s);
	i = strlen(ns);
	s = (char*)malloc(i+1);
	strcpy(s, ns);
	return(i != 0);
}

///////////////////////////////////////////////////////////////////////////////
void String::operator=(String ns)
{
	int		i;
	free(s);
	i = strlen(ns.s);
	s = (char*)malloc(i+1);
	strcpy(s, ns.s);
}

///////////////////////////////////////////////////////////////////////////////
void String::operator=(int ni)
{
	char	ns[32];
	sprintf(ns, "%d", ni);
	operator=(ns);
}

///////////////////////////////////////////////////////////////////////////////
const int String::operator<(PCSTR cs)
{
	return(strcmp(s, cs) < 0);
}

///////////////////////////////////////////////////////////////////////////////
const int String::operator<(char* cs)
{
	return(strcmp(s, cs) < 0);
}

///////////////////////////////////////////////////////////////////////////////
const bool String::operator==(PCSTR cs)
{
	return(strcmp(s, cs) == 0);
}

///////////////////////////////////////////////////////////////////////////////
const bool String::operator!=(PCSTR cs)
{
	return(!(strcmp(s, cs) == 0));
}

///////////////////////////////////////////////////////////////////////////////
int	String::GetLength() const
{
	return(strlen(s));
}

///////////////////////////////////////////////////////////////////////////////
bool String::SetLength(long len)
{
	free(s);
	s = (char*)malloc(len);
	return(s != NULL);
}

///////////////////////////////////////////////////////////////////////////////
void String::Empty()
{
	if (*s != '\0')
	{
		free(s);
		s = (char*)malloc(1);
		*s = '\0';
	}
}

///////////////////////////////////////////////////////////////////////////////
const bool String::EndsWith(char c)
{
	if (s[0] == '\0')
	{
		if (c == '\0')
			return(true);
		return(false);
	}
	if (s[strlen(s)-1] == c)
		return(true);
	return(false);
}

///////////////////////////////////////////////////////////////////////////////
int String::Find(const char c) const
{
	int	i = 0;
	int j=GetLength();
	for (i=0; i<j; i++)
	{
		if (s[i] == c)
			return(i);
	}
	return(-1);
}

///////////////////////////////////////////////////////////////////////////////
int String::RFind(const char c) const
{
	int	i = 0;
	int j=GetLength();
	for (i=j; i>=0; i--)
	{
		if (s[i] == c)
			return(i);
	}
	return(-1);
}

///////////////////////////////////////////////////////////////////////////////
String& String::printf(const char* lpszFormat, ...)
{
    va_list	 ap;			/* Argument pointer */
	char	u[512];
	int		i;
	va_start(ap, lpszFormat);
	vsnprintf(u, 511, lpszFormat, ap);
	va_end(ap);
	free(s);
	i = strlen(u);
	s = (char*)malloc(i+1);
	strcpy(s, u);
	return(*this);
}

///////////////////////////////////////////////////////////////////////////////
void String::vprintf(PCSTR fmt, va_list argList)
{
	char	u[512];
	int		i;

    //va_start(argList, fmt);
	vsnprintf(u, 511, fmt, argList);
	va_end(argList);

	free(s);
	i = strlen(u);
	s = (char*)malloc(i+1);
	strcpy(s, u);
}

///////////////////////////////////////////////////////////////////////////////
int	String::atoi()
{
	int		i=0;
	sscanf(s, "%d", &i);
	return(i);
}

///////////////////////////////////////////////////////////////////////////////
// Recover a string that was passed via an url.  Replace %2A with :, etc.
// replaces current string with Detox'd version.
void String::Detox()
{
	char* os = s;
	char* ns = s;
	char  b[4];
	int	  i;

	while(*ns)
	{
		if (*ns == '%')
		{
			ns++;
			b[0] = *ns++;
			b[1] = *ns++;
			b[2] = '\0';
			sscanf(b, "%x", &i);
			*os++ = i;
		}
		else if (*ns == '+')
		{
			*os++ = ' ';
			ns++;
		}
		else
			*os++ = *ns++;
	}
	*os = '\0';
}

///////////////////////////////////////////////////////////////////////////////
// Make a string url friendly by converting : to %2A, etc.
// replaces current string with Entox'd version.
void String::Entox()
{
	int		l = strlen(s)+1;
	char*	ns = s;
	char*	os = s;
	char*	xs;

	while (*ns)
	{
		if (!((*ns >= '0' && *ns <= '9')
		   || (*ns >= 'A' && *ns <= 'Z')
		   || (*ns >= 'a' && *ns <= 'z')))
			l+=2;
		ns++;
	}
	xs = (char*)malloc(l);
	ns = xs;
	char  b[4];
	while (*os)
	{
		if ((*os >= '0' && *os <= '9')
		 || (*os >= 'A' && *os <= 'Z')
		 || (*os >= 'a' && *os <= 'z'))
		{
			*ns++ = *os++;
		}
		else
		{
			::sprintf(b, "%02X", *os++);
			*ns++ = '%';
			*ns++ = b[0];
			*ns++ = b[1];
		}
	}
	*ns = '\0';
	os = s;
	s = xs;
	free(os);
}

///////////////////////////////////////////////////////////////////////////////
// Remove the brackets that confuse html
void String::Enbracket()
{
	int		l = strlen(s)+1;
	char*	ns = s;
	char*	os = s;
	char*	xs;

	while (*ns)
	{
		if (*ns == '<' || *ns == '>')
			l+=3;
		ns++;
	}
	xs = (char*)malloc(l);
	ns = xs;
	while (*os)
	{
		if (*os == '<')
		{
			*ns++ = '&';
			*ns++ = 'l';
			*ns++ = 't';
			*ns++ = ';';
			os++;
		}
		else
		if (*os == '>')
		{
			*ns++ = '&';
			*ns++ = 'g';
			*ns++ = 't';
			*ns++ = ';';
			os++;
		}
		else
		{
			*ns++ = *os++;
		}
	}
	*ns = '\0';
	os = s;
	s = xs;
	free(os);
}

///////////////////////////////////////////////////////////////////////////////
// Replace &lt; and &gt; with < and >
void String::Debracket()
{
	int		l = strlen(s)+1;
	char*	t = (char*)malloc(l);

	char*	os = s;
	char*	ns = t;

	while (*os)
	{
		if (*os != '&')
		{
			*ns++ = *os++;
			continue;
		}
		if (!strcmp(os, "&lt;"))
		{
			*ns++ = '<';
			os += 4;
			continue;
		}
		if (!strcmp(os, "&gt;"))
		{
			*ns++ = '>';
			os += 4;
			continue;
		}
	}
	*ns = '\0';
	free(s);
	s = t;
}

///////////////////////////////////////////////////////////////////////////////
void String::ToLower()
{
	int	l = GetLength();
	int	i;
	char* ns = s;
	for (i=0; i<l; i++, ns++)
		*ns = tolower(*ns);
}

///////////////////////////////////////////////////////////////////////////////
bool String::ToInt(int* int_ptr)
{
    char	*end_ptr = NULL;
    long	value;
    bool	result;

    /* base 0 has special meaning. */
    value = strtol(s, &end_ptr, 0);

    /* store value regardless of error. */
    *int_ptr = (int) value;

    /* if at least one digit was found we're satisfied. */
    if (end_ptr > s)
		result = true;
    else
		result = false;

    return result;
}

///////////////////////////////////////////////////////////////////////////////
bool String::ToDouble(double* float_ptr)
{
    char	*end_ptr = NULL;
    double	value;

    value = strtod(s, &end_ptr);

    /* store value regardless of error. */
    *float_ptr = (double) value;

    /* if at least one digit was found we're satisfied. */
    return (end_ptr > s);
}

///////////////////////////////////////////////////////////////////////////////
bool String::ToBool(bool *bool_ptr)
{
    bool	result;

    if (!strcasecmp(s, "yes")
	|| !strcasecmp(s, "on")
	|| !strcasecmp(s, "true")) {
	*bool_ptr = true;
	result = true;
    }
    else if (!strcasecmp(s, "no")
	     || !strcasecmp(s, "off")
	     || !strcasecmp(s, "false")) {
	*bool_ptr = false;
	result = true;
    }
    else {
	result = false;
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
void String::TrimWhiteSpace()
{
	TrimLeadingWhiteSpace();
	TrimTrailingWhiteSpace();
}

///////////////////////////////////////////////////////////////////////////////
void String::TrimLeadingWhiteSpace()
{
	char*	p = s;
	char*	e = s + GetLength();
	while (p <= e && isascii(*p) && isspace(*p))
		++p;
	if (p == s)
		return;		// no whitespace, don't do the copy
	String st(p);
	*this = st;
}

///////////////////////////////////////////////////////////////////////////////
void String::TrimTrailingWhiteSpace()
{
	if (GetLength() < 1)
		return;
	char* p = s + GetLength() -1;
	while (p >= s && isascii(*p) && isspace(*p))
		--p;
	*(p+1) = '\0';
}

///////////////////////////////////////////////////////////////////////////////
String String::Left(int i) const
{
	int j = GetLength();
	if (i > j)
		i = j;
	char* cs = (char*)malloc(i+1);
	strncpy(cs, s, i);
	cs[i] = '\0';
	String st(cs);
	free(cs);
	return(st);
}

///////////////////////////////////////////////////////////////////////////////
String String::Mid(int i) const
{
	int j = GetLength();
	if (i > j)
	{
		String nothing;
		return(nothing);
	}
	char* cs = (char*)malloc((j-i)+1);
	strncpy(cs, &s[i], j-i);
	cs[j-i] = '\0';
	String st(cs);
	free(cs);
	return(st);
}

///////////////////////////////////////////////////////////////////////////////
String String::Mid(int i, int count) const
{
	int j = GetLength();
	if (i > j)
	{
		String nothing;
		return(nothing);
	}
	if (i+count > j)
		count = j-i;
	j = i+count;
	char* cs = (char*)malloc((j-i)+1);
	strncpy(cs, &s[i], j-i);
	cs[j-i] = '\0';
	String st(cs);
	free(cs);
	return(st);
}

///////////////////////////////////////////////////////////////////////////////
String String::GetFileName() const
{
	char* p = s+strlen(s);
	while (p > s)
	{
		if (*p == '/'
#ifdef	_WINDOWS
		 || *p == '\\' || *p == ':'
#endif
		   )
		{
			p++;
			break;
		}
		p--;
	}
	String	st(p);
	return(st);
}

///////////////////////////////////////////////////////////////////////////////
void String::Format(const ObjList& opts)
{
	LString*	ls = (LString*)opts.Find(s_decimals);
	if (ls)
	{
		int d = ::atoi(ls->s);
		double f = atof(s);
		this->printf("%.*f", d, f);
	}
	ls = (LString*)opts.Find(s_width);
	if (ls)
	{
		int w = ::atoi(ls->s);
		int	l = strlen(s);
		if (w <= l)
		{
			s[w] = '\0';
			return;
		}
		char* e = (char*) malloc(w+1);
		strcpy(e, s);
		memset(&e[l], ' ', w-l);
		e[w] = '\0';
		free(s);
		s = e;
	}
	ls = (LString*)opts.Find(s_just);
	if (ls)
	{
		if (!strcmp(ls->s, s_right))
		{
			int l = strlen(s)-1;
			int	k = l;
			bool prescan = true;
			int i;
			for (i=l; i>=0; i--)
			{
				if (prescan && s[i] == ' ')
					continue;
				prescan = false;
				s[k] = s[i];
				k--;
			}
			for (; k>=0; k--)
				s[k] = ' ';
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// convert to LStrings
bool String::ToObjList(ObjList* objlist)
{
	char* p = s;
	char* e = p+GetLength();
	LString*	ls;

	// loop to unload each line of data
	while (p < e)
	{
		// find a \n or end of data
		char* st;
		for (st = p; st<e; st++)
		{
			if (*st == '\n')
			{
				st++;
				break;
			}
		}
		ls = new LString;
		objlist->Add(ls);
		int count = st-p;
		ls->s.SetLength(count+1);
		strncpy(ls->s.s, p, count);

		if (ls->s.s[count-1] == '\r' || ls->s.s[count-1] == '\n')
			count--;
#	ifdef	_WINDOWS
		if (ls->s.s[count-1] == '\r' || ls->s.s[count-1] == '\n')
			count--;
#	endif
		ls->s.s[count] = '\0';
		p = st;
	}
	return(true);
}

///////////////////////////////////////////////////////////////////////////////
// These should be made member functions when Stringizing is complete...
int ON(PCSTR optval)
{
    return (strncasecmp(optval, "true", 4) == 0
	    || strncasecmp(optval, "on", 2) == 0
	    || strncasecmp(optval, "yes", 3) == 0
		|| *optval == '1');
}


///////////////////////////////////////////////////////////////////////////////
int OFF(PCSTR optval)
{
    return (strncasecmp(optval, "false", 5) == 0
	    || strncasecmp(optval, "off", 3) == 0
	    || strncasecmp(optval, "no", 2) == 0
		|| *optval == '0');
}

