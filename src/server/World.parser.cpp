/* $Id: World.parser.cpp,v 1.18 2004/02/03 03:39:54 dick Exp $
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2001 by
 *
 *      Bj�rn Stabell        <bjoern@xpilot.org>
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
/* Original options parsing code contributed by Ted Lemon <mellon@ncd.com> */
/*
 * $Log: World.parser.cpp,v $
 * Revision 1.18  2004/02/03 03:39:54  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.17  2004/01/20 20:01:07  dick
 * Map loading debug to try to help venable's boot woes
 *
 * Revision 1.16  2004/01/19 06:59:00  dick
 * Don't deal with "wild" here.  It gets handled much lower.
 *
 * Revision 1.15  2004/01/19 01:30:08  dick
 * Deal with the special 'wild' filename
 *
 * Revision 1.14  2004/01/15 07:11:20  dick
 * Can't pass object fname through ...
 *
 */
#include "StdAfx.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "version.h"
#include "config.h"
#include "serverconst.h"
#include "global.h"
#include "proto.h"
#include "defaults.h"
#include "error.h"
#include "portability.h"
#include "checknames.h"
#include "commonproto.h"
#include "WildMap.h"


char parser_version[] = VERSION;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#ifdef	_DIK_
/*
 * Print the option list in "-help" format.
 * NT uses this to generate the ServerOpts.txt file
 */
static void Parse_help(char *progname)
{
    int			j;
    int			flags, all_flags;
    const char		*str;
    option_desc		*options;
    int			option_count;
    char		msg[MSG_LEN];

    options = Get_option_descs(&option_count);

    printf("Usage: %s [ options ]\n"
	   "\n",
	   progname);
    printf("Option flags specify where an option can be used and whether it\n"
	   "is visible or not.  Default flags are \"any\" if not otherwise\n"
	   "specified by one or more of these keywords:\n"
	   "\n");
    printf("command       May be specified on the command-line.\n"
	   "passwordfile  May be specified in the password file.\n"
	   "defaults      May be specified in the defaults file.\n"
	   "any           May be specified in the map file, defaults file,\n"
	   "              or on the command line.\n"
	   "invisible     Is not displayed when a list of options is\n"
	   "              requested from the server by an operator.\n"
	   "\n");
    printf("The possible options include:\n"
	   "\n");
    for (j = 0; j < option_count; j++) {
	printf("    %s%s",
	       options[j].type == valBool ? "-/+" : "-",
	       options[j].name);
	if (strcasecmp(options[j].commandLineOption, options[j].name))
	    xpprintf(" or %s", options[j].commandLineOption);
	printf(" %s\n",
	       options[j].type == valInt ? "<integer>" :
	       options[j].type == valReal ? "<real>" :
	       options[j].type == valString ? "<string>" :
	       options[j].type == valIPos ? "<position>" :
	       options[j].type == valSec ? "<seconds>" :
	       options[j].type == valPerSec ? "<per-second>" :
		   options[j].type == valList ? "<list>" :
	       "");
	for (str = options[j].helpLine; *str; str++) {
	    if (str == options[j].helpLine || str[-1] == '\n') {
		putchar('\t');
	    }
	    putchar(*str);
	}
	if (str > options[j].helpLine && str[-1] != '\n') {
	    putchar('\n');
	}
	flags = options[j].flags;
	all_flags = (OPT_ORIGIN_ANY | OPT_VISIBLE);
	if ((flags & all_flags) != all_flags && flags != 0) {
	    strlcpy(msg, "[ Flags: command, ", sizeof(msg));
	    if ((flags & OPT_PASSWORD) != 0) {
		strlcat(msg, "passwordfile, ", sizeof(msg));
	    }
	    if ((flags & (OPT_DEFAULTS | OPT_MAP)) == OPT_DEFAULTS) {
		strlcat(msg, "defaults, ", sizeof(msg));
	    }
	    if ((flags & OPT_MAP) != 0) {
		strlcat(msg, "any, ", sizeof(msg));
	    }
	    if ((flags & OPT_VISIBLE) == 0) {
		strlcat(msg, "invisible, ", sizeof(msg));
	    }
	    msg[strlen(msg) - 2] = '\0';
	    strlcat(msg, " ]", sizeof(msg));
	    printf("\t%s\n", msg);
	}
	putchar('\n');
    }
    printf(
"    \n"
"    The probabilities are in the range [0.0-1.0] and they refer to the\n"
"    probability that an event will occur in a block per second.\n"
"    Boolean options are turned off by using +<option>.\n"
"    \n"
"    Please refer to the manual pages, xpilots(6) and xpilot(6),\n"
"    for more specific help.\n"
	  );
}


/*
 * Print the option list.
 */
static void Parser_dump_options(char *progname)
{
    int			j;
    option_desc		*options;
    int			option_count;

    options = Get_option_descs(&option_count);

    for (j = 0; j < option_count; j++) {
	if (options[j].type != valVoid) {
	    int len = strlen(options[j].name);
	    xpprintf("%s:%*s%s\n", options[j].name,
		   (len < 40) ? (40 - len) : 1, "",
		   (options[j].defaultValue != NULL)
		       ? options[j].defaultValue
		       : "");
	}
    }
    xpprintf("\n");
}


/*
 * Print the option flags.
 */
static void Parser_dump_flags(char *progname)
{
    int			j;
    option_desc		*options;
    int			option_count;
    char		msg[MSG_LEN];

    options = Get_option_descs(&option_count);

    for (j = 0; j < option_count; j++) {
	int len = strlen(options[j].name);
	strlcpy(msg, "{", sizeof(msg));
	if ((options[j].flags & OPT_COMMAND) != 0) {
	    strlcat(msg, "command, ", sizeof(msg));
	}
	if ((options[j].flags & OPT_PASSWORD) != 0) {
	    strlcat(msg, "passwordfile, ", sizeof(msg));
	}
	if ((options[j].flags & OPT_DEFAULTS) != 0) {
	    strlcat(msg, "defaults, ", sizeof(msg));
	}
	if ((options[j].flags & OPT_MAP) != 0) {
	    strlcat(msg, "map, ", sizeof(msg));
	}
	if ((options[j].flags & OPT_VISIBLE) != 0) {
	    strlcat(msg, "visible, ", sizeof(msg));
	}
	if ((strlen(msg) >= 2)) {
	    msg[strlen(msg) - 2] = '\0';
	}
	strlcat(msg, "}", sizeof(msg));
	xpprintf("%s:%*s%s\n", options[j].name,
	       (len < 40) ? (40 - len) : 1, "", msg);
    }
    xpprintf("\n");
}


/*
 * Print some compile time configuration parameters.
 */
static void Parser_dump_config(char *progname)
{
	option_desc		*options;
	int			option_count;

	options = Get_option_descs(&option_count);

	xpprintf("\n");
	xpprintf("# %s option dump\n", progname);
	xpprintf("# \n");
	xpprintf("# LIBDIR = %s\n", Conf_libdir());
	xpprintf("# DEFAULTS_FILE_NAME = %s\n", Conf_defaults_file_name());
	xpprintf("# PASSWORD_FILE_NAME = %s\n", Conf_server_password_file_name());
	xpprintf("# MAPDIR = %s\n", Conf_mapdir());
	xpprintf("# DEFAULT_MAP = %s\n", Conf_default_map());
	xpprintf("# SERVERMOTDFILE = %s\n", Conf_servermotdfile());
	xpprintf("# ROBOTFILE = %s\n", Conf_robotfile());
	xpprintf("# \n");
}


/*
 * Print the option list and
 * some compile time configuration parameters.
 */
static void Parser_dump_all(char *progname)
{
	Parser_dump_config(progname);
	Parser_dump_options(progname);
}



/*
 * Check if the i-th command line argument
 * is a request for help or info.
 */
static bool Parse_check_info_request(char **argv, int i)
{
    char		*arg = argv[i];

    if (arg[0] == '-' && arg[1] == '-') {
	/* when arg starts with two dashes skip first one */
	arg++;
    }
    if (strcmp(arg, "-help") == 0
	|| strcmp(arg, "-h") == 0) {
	Parse_help(*argv);
	return TRUE;
    }
    if (strcmp(arg, "-dump") == 0) {
	Parser_dump_all(*argv);
	return TRUE;
    }
    if (strcmp(arg, "-dumpMan") == 0) {
	Parser_dump_options(*argv);
	return TRUE;
    }
    if (strcmp(arg, "-dumpWindows") == 0) {
	Parser_dump_options(*argv);
	return TRUE;
    }
    if (strcmp(arg, "-dumpFlags") == 0) {
	Parser_dump_flags(*argv);
	return TRUE;
    }
    if (strcmp(arg, "-version") == 0 || strcmp(arg, "-v") == 0) {
	puts(TITLE);
	return TRUE;
    }

    return FALSE;
}

#endif	// _DIK_
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
static	String	GetMapName(int world)
{
	PCSTR	d = Conf_server_ini_file_name();
	String	s;
	FILE*	fp;
	char	buf[256];
	D(xpprintf("GetMapName: ini file = \"%s\"\n", d);)
	if (!(fp = fopen(d, "r")))
	{
		D(xpprintf("GetMapName: Failed to open ini file\n", d);)
		return(s);
	}
	String	t;
#ifndef	_WINDOWS
	PCSTR	px = "xpilot.";
#else
	PCSTR	px = "";
#endif
	t.printf("%sWorld%d", px, world);
	while (fgets(buf, 256, fp) != NULL)
	{
		D(xpprintf("GetMapName buf=\"%s\"\n", buf);)
		if (!strncmp(buf, t, t.GetLength()))
		{
			s = &buf[t.GetLength()+1];
			s.TrimWhiteSpace();
			fclose(fp);
			return(s);
		}
	}
	fclose(fp);
	return(s);
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Parse all command line arguments
 * and read the server defaults file and map file.
 * Then convert the map data into a World structure.
 */
bool World::Parser(int argc, char** argv)
{
//	int				i;
	bool			status = true;
//	char*			fname;
//	ServerOption*	opt;

#if 0
    for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-' || argv[i][0] == '+')
		{
			opt = options.FindOptionByName(argv[i] + 1);
			if (opt != NULL)
			{
				if (opt->valType == valBool)
				{
					const char *bool_value;
					if (argv[i][0] == '-')
					{
						bool_value = "true";
					}
					else
					{
						bool_value = "false";
					}
					options.SetValue(opt->name, bool_value, 1, OPT_COMMAND);
				}
				else if (opt->valType == valVoid)
				{
				}
				else
				{
					if (i + 1 == argc)
					{
						seterrno(0);
						error("Option '%s' needs an argument", opt->name);
					}
					else
					{
						options.SetValue(opt->name, argv[++i], 1, OPT_COMMAND);
					}
				}
				continue;
			}
		}
	seterrno(0);
	error("Unknown option '%s'", argv[i]);
	}
#endif

    /*
     * Read local defaults file
     */
	String	fname;
	fname = options.defaultsFileName->GetString();
	if (fname.GetLength())
		parseDefaultsFile(this, fname);
	else
		parseDefaultsFile(this, Conf_defaults_file_name());

	/*
	* Read local password file
	*/
	fname = options.passwordFileName->GetString();
	if (fname.GetLength())
		parsePasswordFile(this, fname);
	else
		parsePasswordFile(this, Conf_server_password_file_name());

	/*
	* Read map file if map data not found yet.
	*/
	if (!options.mapData->output)	// no map data loaded
	{
		bool gotmap = false;
		fname = options.mapFileName->GetString();
		if (fname.IsEmpty())
		{
			fname = GetMapName(0);		// XXX: This will fail if we are not World 0
		}
		if (fname.GetLength())
		{
			if (parseMapFile(this, fname))
			{
				gotmap = true;
			}
			else
			{
				xpprintf("Unable to read %s, trying to open %s\n",
					(PCSTR)fname, Conf_default_map());
				if (!parseMapFile(this, Conf_default_map()))
				{
					xpprintf("Unable to read %s\n", Conf_default_map());
				}
				else
					gotmap = true;
			}
		} else
		{
			fname = Conf_default_map();
			xpprintf("Map not specified, trying to open %s\n", (PCSTR)fname);
			if (!parseMapFile(this, fname))
			{
				xpprintf("Unable to read %s\n", Conf_default_map());
				fname = "";
			}
			else
				gotmap = true;
		}
		if (gotmap)
			options.mapFileName->Set(fname);
		if (!gotmap)
		{
			xpprintf("Generating random world...");
			WildMap::MakeWildMap(&options);
		}
		parseTweaksFile(this, fname);
	}
	return status;
}
