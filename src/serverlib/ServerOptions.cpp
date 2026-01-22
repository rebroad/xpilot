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
 * $Log: ServerOptions.cpp,v $
 * Revision 1.44  2004/05/30 16:23:10  dick
 * Empty the array in the destructor.
 * Use NUMPREFS in Init() because numPrefs hasn't been determined yet.
 *
 * Revision 1.43  2004/05/23 23:52:07  dick
 * src/common/IniClient.Defaults.cpp
 *
 * Revision 1.42  2004/04/26 07:24:45  dick
 * Use strcasecmp not stricmp
 *
 * Revision 1.41  2004/04/26 07:07:14  dick
 * Define a list of pages that can be displayed on the ScoreTable in the client.
 * Each page can be controlled by the server (and the server admin tool).
 *
 * Revision 1.40  2004/02/21 06:59:14  dick
 * SaveOptions() takes the OptOrigin level we are saving at.
 *
 * Revision 1.39  2004/02/06 03:41:19  dick
 * Disable Tag multipliers if not playing Tag
 *
 * Revision 1.38  2004/02/03 03:39:54  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.37  2004/01/28 00:04:01  dick
 * Use the PrefTab enum when talking about which tab a ServerOption is in.
 *
 * Revision 1.36  2004/01/21 04:55:36  dick
 * More Debug!
 *
 * Revision 1.35  2004/01/21 04:47:10  dick
 * More debug.
 *
 * Revision 1.34  2004/01/19 16:48:27  dick
 * Look in the maps dir if we can't find the map in the current dir
 *
 * Revision 1.33  2004/01/19 07:04:16  dick
 * Handle the pseudo-filename "wild" and call the wild generator instead of trying
 * to actually load it as a file.
 *
 * Revision 1.32  2004/01/09 17:25:00  dick
 * ich needs to be a char so String+= works
 *
 * Revision 1.31  2004/01/08 18:10:51  dick
 * Update to XPilot 4.5.5beta
 *
 * Revision 1.30  2002/09/16 21:44:28  dick
 * Deadwood delete
 *
 * Revision 1.29  2002/07/23 18:23:48  dick
 * Need ctype.h
 *
 * Revision 1.28  2002/07/23 12:23:29  jlmiller
 * Write creation date using showtime()
 *
 * Revision 1.27  2002/07/12 15:34:32  dick
 * Deadwood delete (Windows specific yea!)
 *
 * Revision 1.26  2002/07/09 21:52:44  dick
 * xpprintf needs showtime()
 *
 * Revision 1.25  2002/07/09 05:01:17  dick
 * Enable more options for team mode
 *
 * Revision 1.24  2002/07/08 06:01:07  dick
 * If an option is set via tuner, then set its optOrigin to OPT_TUNE.
 *
 * Revision 1.23  2002/07/01 07:00:12  dick
 * Default return value for ParseOpenFile
 *
 * Revision 1.22  2002/06/23 06:31:05  dick
 * Don't handle the factory here; each app must provide it
 *
 * Revision 1.21  2002/06/09 21:16:57  dick
 * Pass String as PCSTR through ...
 *
 * Revision 1.20  2002/06/09 21:10:30  dick
 * Don't include ServerOptions.init.cpp in the lib
 *
 * Revision 1.19  2002/06/04 20:18:16  dick
 * Use String instead of (char*)malloc() to squash some leaks.
 *
 * Revision 1.18  2002/05/29 20:30:59  jlmiller
 * ServerOptions should all be define here.
 *
 * Revision 1.17  2002/05/23 21:21:23  dick
 * System tab becomes tab 1.  Shuffle a bunch of options to more logical places.
 *
 * Revision 1.16  2002/05/21 22:39:24  jlmiller
 * Clean up option display, filter a few correctly
 *
 * Revision 1.15  2002/05/18 20:55:40  dick
 * Update to XPilot-4.5.1
 *
 * Revision 1.14  2001/08/11 09:39:33  dick
 * new option notifyServerChanges; do we spit a message to the clients when a
 * ServerOption changes?
 *
 * Revision 1.13  2001/08/08 08:02:57  dick
 * Don't run the listeners during construction.
 *
 * Revision 1.12  2001/08/05 08:05:37  dick
 * Add Tuners to the server.  ComputeSensorRange(), AddTank(), RemoveTank()
 * become members of Player.  ComputeGravity*() become members of World.
 *
 * Revision 1.11  2001/08/02 19:49:53  dick
 * Better grokking of the World (from mapdata to worlddata)
 *
 * Revision 1.10  2001/08/01 21:55:42  dick
 * Work on ErrorHandler vs. fatal vs. warn on Linux.
 *
 * Revision 1.9  2001/08/01 20:48:44  dick
 * Commentality
 *
 * Revision 1.8  2001/08/01 19:42:48  dick
 * New method Activate(), a partner of Listener().  When an attribute changes
 * value, other attributes might be enabled or disabled.  Ex: if teamPlay is
 * off, then teamCannons becomes undefined, or disabled.
 *
 * Revision 1.7  2001/08/01 17:37:16  dick
 * Add ServerOption Listeners.  When a ServerOption is changed, notify all
 * registered listeners. (Try opening two prefs windows on the same map).
 *
 * Revision 1.6  2001/07/24 09:04:01  dick
 * Remove all global options.  Use World.options instead.
 *
 * Revision 1.5  2001/07/23 07:30:08  dick
 * Support OptOrigin when setting options
 *
 * Revision 1.4  2001/07/22 08:11:22  dick
 * Add support for setting with origin and override
 *
 * Revision 1.3  2001/07/21 06:45:30  dick
 * Hide the varInt varString, etc. and force access through the accessor funcs
 *
 * Revision 1.2  2001/07/21 03:12:53  jlmiller
 *
 * Added all the missing ServerOptions, and all the help, Replaced usage of
 * STRING, POSINT etc, with ValType enums. StrToNum is no longer used by xpiloteditso killed it for now.
 *
 * Revision 1.1  2001/07/20 05:20:11  dick
 * Yet another reorg.  Break server/xpilotedit common files into serverlib.
 *
 * Revision 1.3  2001/07/19 07:07:52  dick
 * Linux warnings cleanup
 *
 * Revision 1.2  2001/07/19 07:01:48  dick
 * Integrate OptOrigin into our ServerOption.
 * stringvar becomes varString, intvar becomes varInt, etc.
 *
 * Revision 1.1  2001/07/19 05:00:02  dick
 * There is no real reason to have optionsStruct, all of the ServerOption(s)
 * are attributes of ServerOptions.  And rename Options.* to ServerOptions.*
 * (They could be just Options, but when the client comes along with his
 * different format options...)
 *
 * Revision 1.3  2001/07/19 04:01:11  dick
 * Beautify the options initing
 *
 * Revision 1.2  2001/07/18 22:14:48  dick
 * xpilotedit built with standard xpilot Imakefile now.
 * Moved xpilotedit/fltk-widgets to common/fltk-widgets
 *
 * Revision 1.1  2001/07/18 20:15:32  dick
 * The first pass at integrating xpilotedit's options with the server options.
 * The server does not build yet.  xpilotedit works as before, with the new
 * definitions.
 *
 */
#include "StdAfx.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "version.h"
#include "config.h"
#include "xpprintf.h"
#include "File.h"
#include "ServerOptions.h"
#include "WildMap.h"

extern	const char* szAppName;

extern	bool	saveAllPrefs;

///////////////////////////////////////////////////////////////////////////////
ServerOptions::ServerOptions(bool /* unused */)
{
	Init();
}

///////////////////////////////////////////////////////////////////////////////
ServerOptions::~ServerOptions()
{
	for (int i=0; i<NUMPREFS; i++)
	{
		if (prefsArray[i])
			delete prefsArray[i];
		prefsArray[i] = NULL;
	}
	if (mapData)
		delete mapData;
	mapData = NULL;
}

void ServerOptions::Init()
{
	for (int i=0; i<NUMPREFS; i++)
	{
		prefsArray[i] = NULL;
	}
	mapData = NULL;
	parent = NULL;
}

/***************************************************************************/
/* SaveOptions                                                             */
/* Arguments :                                                             */
/*   pstrFileName                                                          */
/*   saveData                                                              */
/* Purpose :                                                               */
/***************************************************************************/
bool ServerOptions::SaveOptions(char *pstrFileName, bool saveData, OptOrigin origin)
{
	FILE *ofile = NULL;
	int n,i,j;
	time_t tim;
	int w, h;

	mapName->output = true;		// always output these 5
	mapAuthor->output = true;
	mapWidth->output = true;
	mapHeight->output = true;
	mapData->output = true;

	if (NULL == (ofile = fopen (pstrFileName, "wb")))
	{
		xpprintf("%sCouldn't open file *%s* for writing!", showtime(), pstrFileName);
		return false ;
	}

	time(&tim);
	//fprintf(ofile,"#Created by %s on %s",szAppName,ctime(&tim));
	fprintf(ofile,"#Created by %s on %s\n",szAppName,showtime());
	fprintf(ofile,"#Options updated for %s\n\n",VERSION);

	if (comments.GetLength())
	{
		//	fprintf(ofile,"#");
		for (i = 0; i < comments.GetLength(); i++)
			if (comments[i] != '\n')
				fprintf(ofile,"%c", comments.s[i]);
			else
				fprintf(ofile,"\n");
		fprintf(ofile,"\n\n");
	}

	w = mapWidth->GetInt();
	h = mapHeight->GetInt();

	for ( n=0; n< numPrefs; n++ )
	{
		if (prefsArray[n]->optOrigin == origin)
			fprintf(ofile,"%s : %s\n", prefsArray[n]->name, (PCSTR)prefsArray[n]->GetString());
	}
	if (saveData)
	{
		fprintf(ofile,"\nmapData: \\multiline: EndOfMapdata\n");
		for (i=0;i<h;i++)
		{
			for(j=0;j<w;j++)
			{
				fprintf(ofile,"%c",mapData->blocks[j][i].cdata);
			}
			fprintf(ofile,"\n");
		}
		fprintf(ofile,"EndOfMapdata\n");
	}
	fclose(ofile);

	*mapFileName = pstrFileName;
	return false;
}
/***************************************************************************/
/* LoadOptions                                                             */
/* Arguments :                                                             */
/*   pstrFileName                                                          */
/*   loadData                                                              */
/* Purpose :                                                               */
/***************************************************************************/
bool ServerOptions::LoadOptions(PCSTR pstrFileName, OptOrigin origin, bool loadData)
{
	FILE*	ifile = NULL;
	int		ich;
	int		corrupted=0;
	int		line = 0;
	String	fname(pstrFileName);

	if (!strncasecmp(fname, "wild", 4))
	{
		if (fname.GetLength() < 5)
			return(WildMap::MakeWildMap(this));
		else
		{
			String se;
			se = fname.Mid(4);
			se.TrimWhiteSpace();
			int seed = se.atoi();
			return(WildMap::MakeWildMap(this, seed));
		}
	}
	D(xpprintf("Try opening \"%s\" in current dir...\n", (PCSTR)fname);)
	if ((ifile = fopen (fname, "rb")) == NULL)
	{
		D(xpprintf("That failed.\n");)
		fname.printf("%s%s", Conf_mapdir(), pstrFileName);
		D(xpprintf("Try opening \"%s\"...\n", (PCSTR)fname);)
		if ((ifile = fopen (fname, "rb")) == NULL)
		{
			D(xpprintf("That failed too.\n");)
			return false ;
		}
	}
	D(xpprintf("File is open \"%s\"\n", (PCSTR)fname);)
	ich = getc(ifile);
	if (ich != EOF)
		ungetc(ich, ifile);
	while (!feof(ifile))
	{
		line++;
		if (!ParseLine(ifile, line, loadData, origin))
			corrupted = 1;
	}
	if (ifile)
		fclose(ifile);
	if (corrupted)
	{
		xpprintf("%s\nFile is invalid or corrupted at line %d!", pstrFileName, line);
		return false;
	}
	return true;
}

//#define EXPAND \
//        if (i == slen) {                   \
//        s = (char *) realloc(s, slen *= 2);      \
//        }
/***************************************************************************/
/* ParseLine                                                               */
/* Arguments :                                                             */
/*   ifile                                                                 */
/*   loadData                                                              */
/* Purpose :                                                               */
/***************************************************************************/
bool ServerOptions::ParseLine(FILE *ifile, int& line, bool loadData, OptOrigin origin)
{
	char	ich;
	String	value;
	//char*	head;
	String	name;
	String	s;
	char*	pvalue;
	String	commentline;
	int   slen = 128;
	int   i=0;
	int   override = 0;
	int   multiline = 0;
	bool  ret = false;

	ich = getc(ifile);

	/* Skip blank lines... */
	if (ich == '\n')
	{
		line++;
		return true;
	}
	/* Skip leading space... */
	if (isascii(ich) && isspace(ich))
	{
		ich = skipspace(ifile);
		if (ich == '\n')
		{
			line++;
			return true;
		}
	}
	/* Skip lines that start with comment character... */
	if (ich == '#')
	{
		commentline = "#";
		ich = getc(ifile);
		while ( (ich != EOF) && (ich != '\n') )
		{
			commentline += ich;
			ich = getc(ifile);
		}
		if (ich == '\n')
		{
			commentline += ich;
			line++;
		}

		/* only add comment lines not created by mapxpress or mapedit*/
		if (strstr(commentline,"Created by") == NULL)
		{
			comments += commentline;
		}
		return true;
	}
	/* Skip lines that start with the end of the file... :') */
	if (ich == EOF)
	{
		return true;
	}
	/* Start with ascii? */
	if (!isascii(ich) || !isalpha(ich))
	{
		toeol(ifile);
		return false;
	}
	s += ich;
	do
	{
		ich = getc(ifile);
		if (ich == '\n' || ich == '#' || ich == EOF)
		{
			if (ich == '#')
				toeol(ifile);
			line++;
			return true;
		}
		if (isascii(ich) && isspace(ich))
			continue;
		if (ich == ':')
			break;
		s += ich;
	} while (1);

	ich = skipspace(ifile);

	name = s;

	s = "";
	i = 0;
	do
	{
		s += ich;
		ich = getc(ifile);
	}
	while (ich != EOF && ich != '#' && ich != '\n');

	if (ich == '#')
		toeol(ifile);

	value = s;
	value.TrimTrailingWhiteSpace();
	pvalue = value.s;
	if (!strncmp(pvalue, "\\override:", 10))
	{
		override = 1;
		pvalue += 10;
	}
	while (*pvalue && isascii(*pvalue) && isspace(*pvalue))
		++pvalue;
	if (!strncmp(pvalue, "\\multiline:", 11))
	{
		multiline = 1;
		pvalue += 11;
	}

	while (*pvalue && isascii(*pvalue) && isspace(*pvalue))
		++pvalue;
	if (!*pvalue)
	{
		return true;
	}
	if (multiline)
	value = GetMultilineValue(ifile, line, pvalue);
	ret = AddOption(name, value, origin, true, loadData);
	if (!ret)
	{
		warn("Bad Option -%s- value -%s-", (PCSTR)name, pvalue);
	}
	return ret;
}

///////////////////////////////////////////////////////////////////////////////
bool ServerOptions::ParseOpenFile(FILE* file, OptOrigin origin)
{
	bool ret = false;
	int line = 0;
	while (!feof(file))
		ret = ParseLine(file, line, true, origin);
	return(ret);
}

/***************************************************************************/
/* AddOption                                                               */
/* Arguments :                                                             */
/*    *name                                                                */
/*    *value                                                               */
/*    output                                                               */
/*    loadData                                                             */
/* Purpose :                                                               */
/***************************************************************************/
bool ServerOptions::AddOption(PCSTR _name, PCSTR value, OptOrigin origin, bool output, bool loadData)
{
	int		option;
	//      char                  *tmp;
	String	temp;
	String	n;

	n = _name;
	n.ToLower();

	if (!strcasecmp(n, "mapdata"))
	{
		if (loadData)
			return (LoadMapData(value, origin, output));
		return true;
	}
	option = FindOption(n);
	if (option >= numPrefs)
	{
		temp.printf("%s:%s \n",(PCSTR)n,value);
		comments += temp;
		return true;
	}
	prefsArray[option]->Set(value, 1, origin);
	if (output)
		prefsArray[option]->output = true;
//	*prefsArray[option] = value;

	return true;
}
/***************************************************************************/
/* LoadMapData                                                             */
/* Arguments :                                                             */
/*   value                                                                 */
/* Purpose :                                                               */
/***************************************************************************/
bool ServerOptions::LoadMapData(PCSTR value, OptOrigin origin, bool output)
{
	int                   x=0, y=0;

	while ( *value != '\0' )
	{
		if ( *value == '\n' )
		{
			x = 0;
			y ++;
		}
		else
		{
			mapData->blocks[x++][y].cdata = *value;
		}
		value ++;
	}
	mapData->output = output;			// map was loaded
	mapData->optOrigin = origin;
	return true;
}
/***************************************************************************/
/* FindOption                                                              */
/* Arguments :                                                             */
/*    *name                                                                */
/* Purpose :                                                               */
/***************************************************************************/
int ServerOptions::FindOption(PCSTR name)
{
	int                   option;

	for (option = 0; option < numPrefs; option++)
	{
		if(!strcasecmp(name, prefsArray[option]->name))
			break;
		/*If the first name isn't it, check the alternative*/
		if(!strcmp(name, prefsArray[option]->altname))
			break;
	}

	return option;
}


#if 0
/***************************************************************************/
/* OptionsInit                                                             */
/* Arguments :                                                             */
/* Purpose :                                                               */
/***************************************************************************/
//unnessary, this was handled during object construction
bool ServerOptions::OptionsInit()
{
	int                   i;

	comments = "";

	for (i = 4; i < numPrefs; i++)			// DIKSEZ: magic number 4? bleh
		prefsArray[i]->output = false;		// oh i see, always output width,height,name,author

	SetupDefaultServerOptions();			// but then here, you start at 0 setting to false

	return false;
}
#endif

/***************************************************************************/
/* Setup_default_server_options                                            */
/* Arguments :                                                             */
/* Purpose :   Set all options to their default                            */
/***************************************************************************/
bool ServerOptions::SetupDefaultServerOptions()
{
	int i;

	for(i=0;i<numPrefs-1;i++)
//		AddOption(default_settings[i].name,default_settings[i].value, false, true);
		AddOption(prefsArray[i]->name, prefsArray[i]->defval, OPT_INIT, false, true);

	return false;
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Modify an option during the game.
 *
 * Options which can be modified have a so called tuner function,
 * which checks the validity of the new option value, and possibly
 * does something extra depending upon the option in question.
 * Options which don't need such a tuner function set it to `tuner_dummy'.
 * Options which cannot be modified have the tuner set to `tuner_none'.
 */
int ServerOptions::TuneOption(PCSTR name, PCSTR val)
{
	ServerOption*	opt;

	if (!(opt = FindOptionByName(name))) {
		return -2;	/* Variable not found */
	}

//	if (opt->tuner == &ServerOption::TunerNone) {
//		return -1;	/* Operation undefined */
//	}
	int	ret = opt->Tune(val);
	if (ret)
		opt->optOrigin = OPT_TUNE;
	return(ret);
}

///////////////////////////////////////////////////////////////////////////////
ServerOption* ServerOptions::FindOptionByName(PCSTR name)
{
    int			j;

    for (j = 0; j < numPrefs; j++) {
	if (!strcasecmp(prefsArray[j]->name, name)
	    || !strcasecmp(prefsArray[j]->altname, name))
	{
	    return(prefsArray[j]);
	}
    }
    return NULL;
}

///////////////////////////////////////////////////////////////////////////////
bool ServerOptions::GetValue(const char *name, String& value)
{
    ServerOption*	opt;

    if (!(opt = FindOptionByName(name)))
		return(false);						// Variable not found
	if ((opt->optControl & OPT_VISIBLE) == 0)
		return(false);						// not allowed to get
	return(opt->Get(value));
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Get the current value of a server option
 * in a text buffer.
 *
 * This is called when a client requests
 * to see the current server parameter list.
 */
int ServerOptions::ListOptions(int i, char* buf)
{
	if (i < 0 || i >= numPrefs)
		return -1;
	if (prefsArray[i]->defval == NULL)
		return 0;
	if ((prefsArray[i]->optControl & OPT_VISIBLE) == 0)
		return 0;

	String s;
	s.printf("%s:", prefsArray[i]->name);
	s += prefsArray[i]->GetString();
	return 1;
}

///////////////////////////////////////////////////////////////////////////////
bool ServerOptions::SetValue(PCSTR name, PCSTR value,
							 int override,	OptOrigin opt_origin)
{
	ServerOption* opt = FindOptionByName(name);
	return(opt->Set(value, override, opt_origin));
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Return the value of the specified option,
 * or NULL if there is no value for that option.
 */
#if 0
char *Option_get_value(const char *name, optOrigin *origin_ptr)
{
    hash_node	*np = Get_hash_node_by_name(name);

    if (np != NULL) {
	if (origin_ptr != NULL) {
	    *origin_ptr = np->value->origin;
	}
	return np->value->value;
    }

    return (char *)NULL;
}

#endif

///////////////////////////////////////////////////////////////////////////////
void ServerOptions::OptionsChanged(ServerOption* which)
{
	bool b;

	if (isInit)	// no mucking during object construction
		return;

	// if which is NULL, then iterate through all the options
	// this is useful when creating a ServerPrefsWindow on an
	// existing ServerOptions.
	if (!which)
	{
		for (int i=0; i<numPrefs; i++)
			OptionsChanged(prefsArray[i]);
	}
	// if we are currently refreshing values from a server, then we can't connect to that server
	if (which == refreshValues)
	{
		isConnected->Activate(!refreshValues->GetBool());
	}
	// if we are currently connected to a server, then there is no need to refresh (its automatic)
	else if (which == isConnected)
	{
		refreshValues->Activate(!isConnected->GetBool());
	}
	// Try to set the map window with the map name
//	else if (which == mapName)
//	{
//		if (!isConnected->GetBool())
//			;
//
//	}
	// check if limitedVisibility
	else if (which == limitedVisibility)
	{
		b = limitedVisibility->GetBool();
		minVisibilityDistance->Activate(b);
		maxVisibilityDistance->Activate(b);
	}
	else if (which == limitedLives)
	{
		b = limitedLives->GetBool();
		worldLives->Activate(b);
		endOfRoundReset->Activate(b);
		resetOnHuman->Activate(b);
	}
	else if (which == timing)
	{
		b = timing->GetBool();
		raceLaps->Activate(b);
		checkpointRadius->Activate(b);
		ballRace->Activate(b);
		ballRaceConnected->Activate(b);
	}
	else if (which == useWreckage)
	{
		b = useWreckage->GetBool();
		wreckageCollisionMayKill->Activate(b);
	}
	else if (which == teamPlay)
	{
		b = teamPlay->GetBool();
		teamAssign->Activate(b);
		teamImmunity->Activate(b);
		teamCannons->Activate(b);
		teamFuel->Activate(b);
		captureTheFlag->Activate(b);
		treasureKillTeam->Activate(b);
		teamShareScore->Activate(b);
		robotTeam->Activate(b);
		reserveRobotTeam->Activate(b);
		restrictRobots->Activate(b);
		targetKillTeam->Activate(b);
		lockOtherTeam->Activate(b);
	}
	else if (which == maxAsteroidDensity
		  || which == asteroidProb)
	{
		b = maxAsteroidDensity->GetBool();
		bool b1 = this->asteroidProb->GetBool();
		b = b & b1;
// FIXME:		asteroidScoring->Activate(b);
		asteroidsWallBounce->Activate(b);
		asteroidsOnRadar->Activate(b);
		asteroidCollisionMayKill->Activate(b);
	}
	else if (which == allowNukes)
	{
		b = allowNukes->GetBool();
		nukeClusterDamage->Activate(b);
		nukeMinMines->Activate(b);
		nukeMinSmarts->Activate(b);
		nukesOnRadar->Activate(b);
	}
	else if (which == allowAlliances)
	{
		b = allowAlliances->GetBool();
		announceAlliances->Activate(b);
	}
	else if (which == cannonsUseItems)
	{
		b = cannonsUseItems->GetBool();
		cannonsDefend->Activate(b);
	}
	else if (which == tagGame)
	{
		b = tagGame->GetBool();
		tagKillItMult->Activate(b);
		tagItKillMult->Activate(b);
	}
	/* TODO
	ok, now we need to count various objects,
	like number of wormholes and if 0, then disable "Wormhole Visible".
	same schtick for "Item Conc Vis".
	and "Target" attributes.  No targets means no Target Dead Time
	*/
}
