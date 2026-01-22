/*
 * XPilotedit, the cross platform map editor for XPilot.  Copyright (C) 2001 by
 *
 *      The XPilot Authors           <xpilot@xpilot.org>
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
 * $Log: File.cpp,v $
 * Revision 1.6  2004/02/16 14:29:44  dick
 * server/sched.cpp and friends become serverlib/NetServer
 * which disconnects the udp listen and receive functionality from the game.
 *
 */
#include "StdAfx.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "cstring.h"

/***************************************************************************/
/* toeol                                                                   */
/* Arguments :                                                             */
/*   ifile                                                                 */
/* Purpose :                                                               */
/***************************************************************************/
void toeol(FILE *ifile)
{
	int                   ich;

	while (!feof(ifile))
		if ((ich = getc(ifile)) == '\n')
		{
			return;
		}
}

/***************************************************************************/
/* skipspace                                                               */
/* Arguments :                                                             */
/*   ifile                                                                 */
/* Purpose :                                                               */
/***************************************************************************/
char skipspace(FILE *ifile)
{
	int                   ich;

	while (!feof(ifile))
	{
		ich = getc(ifile);
		if (ich == '\n')
		{
			return (char) ich;
		}
		if (!isascii(ich) || !isspace(ich))
			return (char) ich;
	}
	return EOF;
}

/***************************************************************************/
/* char *getMultilineValue                                                 */
/* Arguments :                                                             */
/*   delimiter                                                             */
/*    ifile                                                                */
/* Purpose :                                                               */
/***************************************************************************/
String GetMultilineValue(FILE *ifile, int& line, char *delimiter)
{
	String	st;
	char*	s = (char *) malloc(32768);
	int		i = 0;
	int		slen = 32768;
	char*	bol;
	int		ich;

	bol = s;
	while (1)
	{
		ich = getc(ifile);
		if (ich == EOF)
		{
			s = (char *) realloc(s, i + 1);
			s[i] = '\0';
			st = s;
			free(s);
			return(st);
		}
		if (i == slen)
		{
			char* t = s;

			s = (char *) realloc(s, slen += 32768);
			bol += s - t;
		}
		if (ich == '\n')
		{
			line++;
			s[i] = 0;
			if (delimiter && !strcmp(bol, delimiter))
			{
				char* t = s;

				s = (char *) realloc(s, bol - s + 1);
				s[bol - t] = '\0';
				st = s;
				free(s);
				return st;
			}
			bol = &s[i + 1];
		}
		s[i++] = ich;
	}
}


/***************************************************************************/
/* YesNo                                                                   */
/* Arguments :                                                             */
/*   val                                                                   */
/* Purpose :                                                               */
/***************************************************************************/
int YesNo(PCSTR val)
{
	if ( (tolower(val[0]) == 'y') || (tolower(val[0]) == 't') )
		return 1;

	return 0;
}

#if 0
/***************************************************************************/
/* char *StrToNum                                                          */
/* Arguments :                                                             */
/*   string                                                                */
/*   len                                                                   */
/*   type                                                                  */
/* Purpose :                                                               */
/***************************************************************************/
char* StrToNum(char *string, int len, int type)
{
	char                  *returnval;

	returnval = (char *) malloc(len+1);
	returnval[0] = (char) NULL;

	if ( type == FLOAT || type == INT)
	{
		if ( (string[0] == '-') || ((string[0] >= '0') && (string[0] <= '9')) )
		sprintf(returnval,"%s%c",returnval,string[0]);

	}
	else if ((string[0] == '-')  || ((string[0] >= '0') &&
			 (string[0] <= '9')) || (string[0] == '.') )
		sprintf(returnval,"%s%c",returnval,string[0]);

	string++;
	while ( (string[0] != (char) NULL) && ((int) strlen(returnval) <= (len-1) ) )
	{
		if ( type == FLOAT || type == POSFLOAT )
		{
			sprintf(returnval,"%s%c",returnval,string[0]);
		}
		else if ((string[0] >= '0') && (string[0] <= '9'))
			sprintf(returnval,"%s%c",returnval,string[0]);
		string++;
	}
	return (char *) returnval;
}
#endif
