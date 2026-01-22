/* $Id: cstring.h,v 1.31 2004/05/30 08:09:32 dick Exp $
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
 * $Log: cstring.h,v $
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
 * Revision 1.25  2004/02/05 04:48:36  dick
 * operator PCSTR() const is just like operator PCSTR()
 *
 * Revision 1.24  2004/01/09 17:36:12  dick
 * Add operator+=(int) which writes the int as a '%d'
 *
 * Revision 1.23  2004/01/07 04:38:00  dick
 * Add operator!=(PCSTR)
 *
 * Revision 1.22  2002/09/12 23:01:26  dick
 * New TrimWhiteSpace() and TrimLeadingWhiteSpace().
 *
 * Revision 1.21  2002/09/12 19:48:14  dick
 * Add GetFileName() which returns the filename component of a full path.
 *
 * Revision 1.20  2002/08/28 13:31:16  dick
 * Define uchar
 *
 * Revision 1.19  2002/07/23 17:46:26  dick
 * ToObjList converts a string into a list of LStrings, with one line per
 * LString, CRLF stripped.
 *
 * Revision 1.18  2002/07/08 06:25:35  dick
 * operator=(PCSTR) doesn't need to be virtual.
 *
 * Revision 1.17  2002/06/25 04:32:59  dick
 * new Format(ObjList opts) formats a string based on the options.  Supported are
 * width, left/right justify, and number of decimals.  This is useful like in
 * a <pre> block.
 *
 * Revision 1.16  2002/06/23 06:20:14  dick
 * SetLength(int n) sets the size of the string buffer to n.
 *
 * Revision 1.15  2002/06/20 22:34:24  dick
 * Linux get va_list from stdarg.h
 *
 * Revision 1.14  2002/06/20 21:53:55  dick
 * Add vprintf()
 *
 * Revision 1.13  2002/06/09 18:32:23  dick
 * IsEmpty() is a const function.
 *
 * Revision 1.12  2002/06/07 15:40:45  dick
 * 3 funcs Find(c) Left(i) Mid(i) to help with string parsing.
 *
 * Revision 1.11  2002/06/04 20:02:45  dick
 * New TrimTrailingWhiteSpace()
 *
 * Revision 1.10  2002/05/20 07:28:08  dick
 * New function bool EndsWith(char); checks the last char of the string for
 * a match (like does a path end with '/').
 *
 * Revision 1.9  2001/08/12 10:58:24  dick
 * operator==
 *
 * Revision 1.8  2001/07/26 10:58:00  dick
 * Add header text info
 */

#ifndef	_CSTRING_H_
#define	_CSTRING_H_

#ifndef	PCSTR
typedef	const char* PCSTR;
#endif

#ifndef	PSTR
typedef char* PSTR;
#endif

#ifndef	va_list
#include <stdio.h>
#endif

#ifndef	uchar
typedef	unsigned char	uchar;
#endif

#ifndef	va_list
#include <stdarg.h>
#endif

class ObjList;

class String
{
  public:
	String();
	String(PCSTR p);
	String(PCSTR p, int size);
	String(int size);
	String(const String&);
	~String();

	char*	s;

	void			Add(PCSTR cs);
	operator PCSTR() { return((PCSTR)s); }
	operator PCSTR() const { return((PCSTR)s); }
//	operator PSTR() { return((PSTR)s); }	// don't allow this - do it the hard way if you
											// want to directly drive s
	char operator[](int nIndex) { return(s[nIndex]); }
	String			operator+(PCSTR cs);
	void			operator+=(PCSTR cs) { Add(cs); };
	void			operator+=(char);
	void			operator+=(int);
	void			SetAt(int nIndex, char c) { s[nIndex] = c; }
	void			operator=(PCSTR);
	void			operator=(String);
	bool			operator=(char*);
	void			operator=(int);
	const int		operator<(PCSTR cs);	// strcmp(this, cs)
	const int		operator<(char* cs);	// strcmp(this, cs)
	const bool		operator==(PCSTR);
	const bool		operator!=(PCSTR);

	String&			printf(PCSTR control, ...);
	void			vprintf(PCSTR control, va_list argList);
	int				atoi();
	int				Find(const char c) const;	// return index of char, -1 if not found
	int				RFind(const char c) const;	// return index of char, Start at end.  -1 if not found
	String			Left(int i) const;			// like basic Left. returns 0,i chars
	String			Mid(int i) const;			// like Basic Mid. returns i,length chars
	String			Mid(int i, int count) const;// like Basic Mid. returns i,count chars

	String			GetFileName() const;		// strip off any directory component \ or /

	int				GetLength() const;
	bool			SetLength(long len);

	const bool		IsEmpty() const	{ return(GetLength() == 0); };
	const bool		EndsWith(char c);	// is the last char of the string c?
	void			Empty();
	void			Detox();		// Squash the %7E's from the string
	void			Entox();		//  %7E's and %20's in the string
	void			Enbracket();	//  < and > become &lt; and &gt;
	void			Debracket();	// &lt; and &gt; become < and >
	void			ToLower();		// make all lower case chars
	void			TrimWhiteSpace();			// trim from this string
	void			TrimLeadingWhiteSpace();
	void			TrimTrailingWhiteSpace();

	bool			ToDouble(double* float_ptr);
	bool			ToInt(int* int_ptr);
	bool			ToBool(bool* bool_ptr);
	bool			ToObjList(ObjList* objlist);	// convert to LStrings

	void			Format(const ObjList& opts);
  private:
	int		length;							// used by raw "binary" strings
};

extern int ON(PCSTR optval);
extern int OFF(PCSTR optval);
extern char *xp_strdup(PCSTR );
extern char *xp_safe_strdup(PCSTR old_string);


#endif	// _CSTRING_H_
