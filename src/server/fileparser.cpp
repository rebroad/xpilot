/* $Id: fileparser.cpp,v 1.16 2007/01/13 22:09:52 dick Exp $
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2001 by
 *
 *      Bjørn Stabell        <bjoern@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
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
 * $Log: fileparser.cpp,v $
 * Revision 1.16  2007/01/13 22:09:52  dick
 * Note on the console that we are reading a tweaks file
 *
 * Revision 1.15  2004/02/03 03:39:54  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.14  2004/01/19 07:00:05  dick
 * parseMapFile just calls LoadOptions() rather than dorking the file around first.
 *
 * Revision 1.13  2004/01/09 17:47:14  dick
 * whitespace
 *
 * Revision 1.12  2004/01/04 06:06:18  dick
 * No newline at end of file
 *
 * Revision 1.11  2002/09/11 16:42:04  dick
 * Here endeth the server encapsulation task.
 * theWorld is now an array of World(s) and get loaded dynamically.
 *
 * Revision 1.10  2002/09/01 00:13:46  dick
 * Remove obsolete windows #include "winServer.h"
 *
 * Revision 1.9  2002/07/09 21:42:50  dick
 * Load the tweaks file after loading the map.
 *
 * Revision 1.8  2002/05/20 07:30:17  dick
 * Smarter fileJoin() which concats a path with a file
 *
 *
 */
#include "StdAfx.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>
#include <sys/types.h>

#ifndef _WINDOWS
# include <unistd.h>
#endif

#ifdef _WINDOWS
# include <windows.h>
# include <io.h>
# define read(__a, __b, __c)	_read(__a, __b, __c)
#endif

#include "version.h"
#include "config.h"
#include "serverconst.h"
#include "global.h"
#include "proto.h"
#include "defaults.h"
#include "error.h"
#include "types.h"
#include "commonproto.h"


char fileparser_version[] = VERSION;



static char	*FileName;
static int	LineNumber;

#ifdef	DIK

/*
 * Skips to the end of the line.
 */
static void toeol(char **map_ptr)
{
    int		ich;

    while (**map_ptr) {
	ich = **map_ptr;
	(*map_ptr)++;
        if (ich == '\n') {
            ++LineNumber;
            return;
        }
    }
}


/*
 * Skips to the first non-whitespace character, returning that character.
 */
static int skipspace(char **map_ptr)
{
    int		ich;

    while (**map_ptr) {
	ich = **map_ptr;
	(*map_ptr)++;
        if (ich == '\n') {
            ++LineNumber;
            return ich;
        }
        if (!isascii(ich) || !isspace(ich))
            return ich;
    }
    return '\0';
}


/*
 * Read in a multiline value.
 */
static char *getMultilineValue(char **map_ptr, char *delimiter)
{
    char       *s = (char *)malloc(32768);
    int         i = 0;
    int         slen = 32768;
    char       *bol;
    int         ich;

    bol = s;
    for (;;) {
	ich = **map_ptr;
	(*map_ptr)++;
	if (ich == '\0') {
	    s = (char *)realloc(s, i + 1);
	    s[i] = '\0';
	    return s;
	}
	if (i == slen) {
	    char       *t = s;

	    slen += (slen / 2) + 8192;
	    s = (char *)realloc(s, slen);
	    bol += s - t;
	}
	if (ich == '\n') {
	    s[i] = 0;
	    if (delimiter && !strcmp(bol, delimiter)) {
		char       *t = s;

		s = (char *)realloc(s, bol - s + 1);
		s[bol - t] = '\0';
		return s;
	    }
	    bol = &s[i + 1];
	    ++LineNumber;
	}
	s[i++] = ich;
    }
}


/*
 * Parse a standard line from a defaults file, in the form Name: value.
 * Name must start at the beginning of the line with an alphabetic character.
 * Whitespace within name is ignored. Value may contain any character other
 * than # or newline, but leading and trailing whitespace are discarded.
 *
 * Characters after a # are ignored - this can be used for comments.
 *
 * If value begins with \override:, the override flag is set when
 * Option_set_value is called, so that this value will override
 * an existing value.
 *
 * If value starts with \multiline:, then the rest of the line is used
 * as a delimiter, and subsequent lines are read and saved as the value
 * until the delimiter is encountered.   No interpretation is done on
 * the text in the multiline sequence, so # does not serve as a comment
 * character there, and newlines and whitespace are not discarded.
 *
 * Lines can also be in the form "define: name value".
 * And similarly "define: name \multiline: TAG".
 *
 * Names can be macros which are expanded inplace with the keyword expand like:
 * expand: name
 *
 */
#define EXPAND					\
    if (i == slen) {				\
	s = (char *)realloc(s, slen *= 2);	\
    }
static void parseLine(char **map_ptr, OptOrigin opt_origin)
{
    int		ich;
    char       *value,
	       *head,
	       *name,
	       *s = (char *)malloc(128);
    char       *p;
    int         slen = 128;
    int         i = 0;
    int         override = 0;
    int         multiline = 0;

    ich = **map_ptr;
    (*map_ptr)++;

    /* Skip blank lines... */
    if (ich == '\n') {
	++LineNumber;
	free(s);
	return;
    }
    /* Skip leading space... */
    if (isascii(ich) && isspace(ich)) {
	ich = skipspace(map_ptr);
	if (ich == '\n') {
	    free(s);
	    return;
	}
    }
    /* Skip lines that start with comment character... */
    if (ich == '#') {
	toeol(map_ptr);
	free(s);
	return;
    }
    /* Skip lines that start with the end of the file... :') */
    if (ich == '\0') {
	free(s);
	return;
    }
    /* *** I18nize? *** */
    if (!isascii(ich) || !isalpha(ich)) {
	error("%s line %d: Names must start with an alphabetic.\n",
	      FileName, LineNumber);
	toeol(map_ptr);
	free(s);
	return;
    }
    s[i++] = ich;
    do {
	ich = **map_ptr;
	(*map_ptr)++;

	if (ich == '\n' || ich == '#' || ich == '\0') {
	    error("%s line %d: No colon found on line.\n",
		  FileName, LineNumber);
	    if (ich == '#')
		toeol(map_ptr);
	    else
		++LineNumber;
	    free(s);
	    return;
	}
	if (isascii(ich) && isspace(ich))
	    continue;
	if (ich == ':')
	    break;
	EXPAND;
	s[i++] = ich;
    } while (1);

    ich = skipspace(map_ptr);

    EXPAND;
    s[i++] = '\0';
    name = s;

    s = (char *)malloc(slen = 128);
    i = 0;
    do {
	EXPAND;
	s[i++] = ich;
	ich = **map_ptr;
	(*map_ptr)++;
    } while (ich != '\0' && ich != '#' && ich != '\n');

    if (ich == '\n')
	++LineNumber;

    if (ich == '#')
	toeol(map_ptr);

    EXPAND;
    s[i++] = 0;
    head = value = s;
    s = value + strlen(value) - 1;
    while (s >= value && isascii(*s) && isspace(*s))
	--s;
    *++s = 0;

    /* Deal with `define: MACRO \multiline: TAG'. */
    if (strcmp(name, "define") == 0) {
	p = value;
	while (*p && isascii(*p) && !isspace(*p)) {
	    p++;
	}
	*p++ = '\0';

	/* name becomes value */
	free(name);
	name = (char *)malloc(p - value + 1);
	memcpy(name, value, p - value);
	name[p - value] = '\0';

	/* Move value to \multiline */
	while (*p && isspace(*p)) {
	    p++;
	}
	value = p;
    }

    if (!strncmp(value, "\\override:", 10)) {
	override = 1;
	value += 10;
    }
    while (*value && isascii(*value) && isspace(*value))
	++value;
    if (!strncmp(value, "\\multiline:", 11)) {
	multiline = 1;
	value += 11;
    }
    while (*value && isascii(*value) && isspace(*value))
	++value;
    if (!*value) {
	error("%s line %d: no value specified.\n",
	      FileName, LineNumber);
	free(name);
	free(head);
	return;
    }
    if (multiline) {
	value = getMultilineValue(map_ptr, value);
    }

    /* Deal with `expand: MACRO'. */
    if (strcmp(name, "expand") == 0) {
		expandKeyword(value);
    }
#ifdef REGIONS /* not yet */
    /* Deal with `region: \multiline: TAG'. */
    else if (strcmp(name, "region") == 0) {
	if (!multiline) { /* Must be multiline. */
	    error("regions must use `\\multiline:'.\n");
	    free(name);
	    free(head);
	    return;
	}
	p = value;
	while (*p) {
	    parseLine(&p, opt_origin);
	}
    }
#endif
    else {
	Option_set_value(name, value, override, opt_origin);
    }

    /*
     * if (multiline) free (value);
     */
    if (multiline) free (value);
    free(name);
    free(head);
    return;
}
#undef EXPAND


/*
 * Parse a file containing defaults (and possibly a map).
 */
static bool parseOpenFile(FILE *ifile, OptOrigin opt_origin)
{
    int		fd, map_offset, map_size, n;
    char       *map_buf;

    LineNumber = 1;

    fd = fileno(ifile);

    /* Using a 200 map sample, the average map size is 37k.
       This chunk size could be increased to avoid lots of
       reallocs. */
#define MAP_CHUNK_SIZE	8192

    map_offset = 0;
    map_size = 2*MAP_CHUNK_SIZE;
    map_buf = (char *) malloc(map_size + 1);
    if (!map_buf) {
	error("Not enough memory to read the map!");
	return false;
    }

    for (;;) {
	n = read(fd, &map_buf[map_offset], map_size - map_offset);
	if (n < 0) {
	    error("Error reading map!");
	    free(map_buf);
	    return false;
	}
	if (n == 0) {
	    break;
	}
	map_offset += n;

	if (map_size - map_offset < MAP_CHUNK_SIZE) {
	    map_size += (map_size / 2) + MAP_CHUNK_SIZE;
	    map_buf = (char *) realloc(map_buf, map_size + 1);
	    if (!map_buf) {
		error("Not enough memory to read the map!");
		return false;
	    }
	}
    }

    map_buf = (char *) realloc(map_buf, map_offset + 1);
    map_buf[map_offset] = '\0'; /* EOF */

    if (isdigit(*map_buf)) {
	errno = 0;
	error("%s is in old (v1.x) format, please convert it with mapmapper",
	      FileName);
	free(map_buf);
	return false;
    } else {
	/* Parse all the lines in the file. */
	char *map_ptr = map_buf;
	while (*map_ptr) {
//	    parseLine(&map_ptr, opt_origin);
		theWorld.options.ParseLine(&map_ptr, true /* sure why not */);
	}
    }

    free(map_buf);

    return true;
}


void expandKeyword(const char *keyword)
{
    optOrigin	expand_origin;
    char	*p;

    p = Option_get_value(keyword, &expand_origin);
    if (p == NULL) {
	warn("Can't expand `%s' because it has not been defined.\n",
	      keyword);
    }
    else {
	while (*p) {
	    parseLine(&p, expand_origin);
	}
    }
}


#endif	// DIK

///////////////////////////////////////////////////////////////////////////////
static int copyFilename(const char *file)
{
    if (FileName) {
	free(FileName);
    }
    FileName = xp_strdup(file);
    return (FileName != 0);
}

///////////////////////////////////////////////////////////////////////////////
static FILE *fileOpen(const char *file)
{
    FILE *fp = fopen(file, "r");
    if (fp ) {
	if (!copyFilename(file)) {
	    fclose(fp);
	    fp = NULL;
	}
    }
    return fp;
}

///////////////////////////////////////////////////////////////////////////////
static void fileClose(FILE *fp)
{
    fclose(fp);
    if (FileName) {
	free(FileName);
	FileName = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Test if filename has the XPilot map extension.
 */
static int hasMapExtension(const char *filename)
{
    int fnlen = strlen(filename);
    if (fnlen > 3 && !strcmp(&filename[fnlen - 3], ".xp")){ 
	return 1;
    }
    if (fnlen > 4 && !strcmp(&filename[fnlen - 3], ".map")){ 
	return 1;
    }
    return 0;
}


///////////////////////////////////////////////////////////////////////////////
/*
 * Test if filename has a directory component.
 */
static int hasDirectoryPrefix(const char *filename)
{
    static const char	sep = '/';
    return (strchr(filename, sep) != NULL);
}


///////////////////////////////////////////////////////////////////////////////
/*
 * Combine a directory and a file.
 * Returns new path as dynamically allocated memory.
 */
static String fileJoin(const char *dir, const char *file)
{
	String	s;
	
	s = dir;
	if (!s.EndsWith('/'))
		s += '/';
	s += file;
    return s;
}


///////////////////////////////////////////////////////////////////////////////
/*
 * Combine a file and a filename extension.
 * Returns new path as dynamically allocated memory.
 */
static char *fileAddExtension(const char *file, const char *ext)
{
    char		*path;

    path = (char *) malloc(strlen(file) + strlen(ext) + 1);
    if (path) {
	sprintf(path, "%s%s", file, ext);
    }
    return path;
}


#if defined(COMPRESSED_MAPS)
static int	usePclose;


///////////////////////////////////////////////////////////////////////////////
static int isCompressed(const char *filename)
{
    int fnlen = strlen(filename);
    int celen = strlen(Conf_zcat_ext());
    if (fnlen > celen && !strcmp(&filename[fnlen - celen], Conf_zcat_ext())) {
	return 1;
    }
    return 0;
}


///////////////////////////////////////////////////////////////////////////////
static void closeCompressedFile(FILE *fp)
{
    if (usePclose) {
	pclose(fp);
	usePclose = 0;
	if (FileName) {
	    free(FileName);
	    FileName = NULL;
	}
    } else {
	fileClose(fp);
    }
}


///////////////////////////////////////////////////////////////////////////////
static FILE *openCompressedFile(const char *filename)
{
    FILE		*fp = NULL;
    char		*cmdline = NULL;
    char		*newname = NULL;

    usePclose = 0;
    if (!isCompressed(filename)) {
	if (access(filename, 4) == 0) {
	    return fileOpen(filename);
	}
	newname = fileAddExtension(filename, Conf_zcat_ext());
	if (!newname) {
	    return NULL;
	}
	filename = newname;
    }
    if (access(filename, 4) == 0) {
	cmdline = (char *) malloc(strlen(Conf_zcat_format()) + strlen(filename) + 1);
	if (cmdline) {
	    sprintf(cmdline, Conf_zcat_format(), filename);
	    fp = popen(cmdline, "r");
	    if (fp) {
		usePclose = 1;
		if (!copyFilename(filename)) {
		    closeCompressedFile(fp);
		    fp = NULL;
		}
	    }
	}
    }
    if (newname) free(newname);
    if (cmdline) free(cmdline);
    return fp;
}

#else

///////////////////////////////////////////////////////////////////////////////
static int isCompressed(const char *filename)
{
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
static void closeCompressedFile(FILE *fp)
{
    fileClose(fp);
}

///////////////////////////////////////////////////////////////////////////////
static FILE *openCompressedFile(const char *filename)
{
    return fileOpen(filename);
}
#endif

///////////////////////////////////////////////////////////////////////////////
/*
 * Open a map file.
 * Filename argument need not contain map filename extension
 * or compress filename extension.
 * The search order should be:
 *      filename
 *      filename.gz              if COMPRESSED_MAPS is true
 *      filename.xp
 *      filename.xp.gz           if COMPRESSED_MAPS is true
 *      filename.map
 *      filename.map.gz          if COMPRESSED_MAPS is true
 *      MAPDIR filename
 *      MAPDIR filename.gz       if COMPRESSED_MAPS is true
 *      MAPDIR filename.xp
 *      MAPDIR filename.xp.gz    if COMPRESSED_MAPS is true
 *      MAPDIR filename.map
 *      MAPDIR filename.map.gz   if COMPRESSED_MAPS is true
 */
static FILE *openMapFile(PCSTR filename)
{
    FILE		*fp = NULL;
    char		*newname;
    String		newpath;

    fp = openCompressedFile(filename);
    if (fp) {
	return fp;
    }
    if (!isCompressed(filename)) {
	if (!hasMapExtension(filename)) {
	    newname = fileAddExtension(filename, ".xp");
	    fp = openCompressedFile(newname);
	    free(newname);
	    if (fp) {
		return fp;
	    }
	    newname = fileAddExtension(filename, ".map");
	    fp = openCompressedFile(newname);
	    free(newname);
	    if (fp) {
		return fp;
	    }
	}
    }
    if (!hasDirectoryPrefix(filename)) {
	newpath = fileJoin(Conf_mapdir(), filename);
	if (!newpath) {
	    return NULL;
	}
	if (hasDirectoryPrefix(newpath)) {
	    /* call recursively. */
	    fp = openMapFile(newpath);
	}
	if (fp) {
	    return fp;
	}
    }
    return NULL;
}


///////////////////////////////////////////////////////////////////////////////
static void closeMapFile(FILE *fp)
{
    closeCompressedFile(fp);
}


///////////////////////////////////////////////////////////////////////////////
static FILE *openDefaultsFile(PCSTR filename)
{
    return fileOpen(filename);
}


///////////////////////////////////////////////////////////////////////////////
static void closeDefaultsFile(FILE *fp)
{
    fileClose(fp);
}


///////////////////////////////////////////////////////////////////////////////
/*
 * Parse a file containing defaults.
 */
bool parseDefaultsFile(World* w, PCSTR filename)
{
    FILE*	ifile;
    bool	result;

    if ((ifile = openDefaultsFile(filename)) == NULL) {
		return false;
    }
    result = w->options.ParseOpenFile(ifile, OPT_DEFAULTS);
    closeDefaultsFile(ifile);

    return true;
}


///////////////////////////////////////////////////////////////////////////////
/*
 * Parse a file containing password.
 */
bool parsePasswordFile(World* w, PCSTR filename)
{
    FILE*	ifile;
    bool	result;

	if ((ifile = openDefaultsFile(filename)) == NULL) {
		return false;
	}
	result = w->options.ParseOpenFile(ifile, OPT_PASSWORD);
	closeDefaultsFile(ifile);

    return true;
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Parse a file containing a map.
 */
bool parseMapFile(World* w, PCSTR filename)
{
	return(w->options.LoadOptions(filename, OPT_MAP, true));
}

///////////////////////////////////////////////////////////////////////////////
bool parseTweaksFile(World* w, PCSTR filename)
{
    FILE*	ifile;
	bool	result;
	String	fname;

	if (!MakeTweaksFileName(fname, filename))
		return(false);
	String path = Conf_vardir();
	path += fname;
    if ((ifile = openDefaultsFile(path)) == NULL) {
		return false;
    }
	xpprintf("Parsing tweaks: %s\n", (PCSTR)path);
    result = w->options.ParseOpenFile(ifile, OPT_TUNEFILE);
    closeDefaultsFile(ifile);

	return(false);
}

///////////////////////////////////////////////////////////////////////////////
bool	MakeTweaksFileName(String& outname, PCSTR inname)
{
	String	in = inname;
	int		dot = in.Find('.');
	if (dot == -1)
	{
		String	e;
		outname.printf("bad filename (no .)\"%s\"", (PCSTR)in);
		return(false);
	}
	outname  = in.Left(dot);
	outname += "_tweak";
	outname += in.Mid(dot);
	return(true);
}
