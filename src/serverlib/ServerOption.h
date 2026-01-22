/*
* Xpilotedit, the cross platform map editor for Xpilot.  Copyright (C) 2001 by
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
 $Log: ServerOption.h,v $
 Revision 1.24  2004/05/23 23:52:07  dick
 src/common/IniClient.Defaults.cpp

 Revision 1.23  2004/04/16 21:07:59  dick
 NetServer becomes a little UDP engine that can be plugged into different apps.

 Revision 1.22  2004/03/03 16:04:33  dick
 Use IsConnected() to determine if we are mapediting or online

 Revision 1.21  2004/02/21 07:03:00  dick
 Use HasGrandparent() to determine if we are the mapeditor or connected to
 something bigger.  Allow OPT_MAP to override OPT_MAP settings.

 Revision 1.20  2004/01/22 00:27:48  dick
 Swap OPT_MAP and OPT_DEFAULTS based on priority.

 Revision 1.19  2004/01/19 03:30:34  dick
 New OPT_WILD for OptOrigin, sez WildMap made us

 Revision 1.18  2002/09/10 01:11:57  dick
 Don't include stdlib.h here

 Revision 1.17  2002/07/08 05:57:01  dick
 New OptOrigins: OPT_TUNE and OPT_TUNEFILE for options who's settings came
 from a "tweak" file which is loaded after the other options.

 Revision 1.16  2002/07/02 06:48:48  dick
 Comment out unused parameter in a null function

 Revision 1.15  2002/06/01 05:59:00  dick
 Add support for ipos. (used by gravityPoint)

 Revision 1.14  2002/05/22 16:03:33  dick
 Handle valSec for cannonDeadTime and maxPauseTime.

 Revision 1.13  2002/05/18 20:55:40  dick
 Update to XPilot-4.5.1

 Revision 1.12  2001/08/26 09:49:10  dick
 SetLabel() allows you to change the label (still dummied out)

 Revision 1.11  2001/08/12 11:16:22  dick
 new option type valList which is an objlist.

 Revision 1.10  2001/08/07 20:55:08  dick
 Pass a this pointer to the ServerOption's listeners, so one callback can
 handle many different ServerOption(s).

 Revision 1.9  2001/08/05 18:18:29  dick
 Linux builds and runs again

 Revision 1.8  2001/08/05 08:05:37  dick
 Add Tuners to the server.  ComputeSensorRange(), AddTank(), RemoveTank()
 become members of Player.  ComputeGravity*() become members of World.

 Revision 1.7  2001/08/01 19:42:48  dick
 New method Activate(), a partner of Listener().  When an attribute changes
 value, other attributes might be enabled or disabled.  Ex: if teamPlay is
 off, then teamCannons becomes undefined, or disabled.

 Revision 1.6  2001/08/01 17:37:16  dick
 Add ServerOption Listeners.  When a ServerOption is changed, notify all
 registered listeners. (Try opening two prefs windows on the same map).

 Revision 1.5  2001/07/24 09:04:01  dick
 Remove all global options.  Use World.options instead.

 Revision 1.4  2001/07/22 08:12:30  dick
 Add support for setting with origin and override

 Revision 1.3  2001/07/21 06:45:30  dick
 Hide the varInt varString, etc. and force access through the accessor funcs

 Revision 1.2  2001/07/21 03:12:53  jlmiller

 Added all the missing ServerOptions, and all the help, Replaced usage of
 STRING, POSINT etc, with ValType enums. StrToNum is no longer used by xpiloteditso killed it for now.

 Revision 1.1  2001/07/20 05:20:11  dick
 Yet another reorg.  Break server/xpilotedit common files into serverlib.

 Revision 1.3  2001/07/19 07:01:48  dick
 Integrate OptOrigin into our ServerOption.
 stringvar becomes varString, intvar becomes varInt, etc.

 Revision 1.2  2001/07/19 03:56:55  dick
 Add the tuner functions.

 Revision 1.1  2001/07/18 20:15:32  dick
 The first pass at integrating xpilotedit's options with the server options.
 The server does not build yet.  xpilotedit works as before, with the new
 definitions.

 */

#ifndef	_SERVEROPTION_H_
#define	_SERVEROPTION_H_

#include "cstring.h"
#include "serverconst.h"
#include "types.h"
#include "Obj.h"

enum ValType {
    valVoid,		/* variable is not a variable */
    valInt,		/* variable is type int */
    valReal,		/* variable is type float */
    valBool,		/* variable is type bool */
    valIPos,		/* variable is type ipos */
    valString,		/* variable is type char* */
    valSec,		/* variable is type int (converted to frames) */
    valPerSec,		/* variable is type float (converted to per-frame) */
	valList,		/* variable is an objlist */
	valEvent,		/* variable is an event, i.e. pushbutton */
};

/*
 * bitflags for the origin of an option.
 * Sent to the Controls as a %hd (16 bits) so be careful!
 */
enum OptOrigin
{
    OPT_INIT		= 1<<0,
    OPT_DEFAULTS	= 1<<1,
    OPT_MAP			= 1<<2,
    OPT_COMMAND		= 1<<3,
    OPT_PASSWORD	= 1<<4,
	OPT_EDITOR		= OPT_MAP,		// DIKSEZ: maybe this should be different?
									// maybe OPT_MAP means loaded from the file
	OPT_TUNEFILE	= 1<<5,
	OPT_TUNE		= 1<<6,			// Option was tuned
	OPT_WILD		= 1<<7,			// Randomly generated value
};

/*
 * extended control flags for option origin
 */
enum OptControl
{

    OPT_NONE		= 0,			// not settable (used by help,version,dump)
									// allow any of {map,defaults,command}
	OPT_ORIGIN_ANY_MASK	= OPT_MAP|OPT_DEFAULTS|OPT_COMMAND
						  |OPT_EDITOR|OPT_TUNE|OPT_TUNEFILE,
    OPT_VISIBLE		= 1<<7,			// can we query this option value?

};

// xpilotedit values need to be merged back in?
/*Visible to: for options*/
#define OPTIONALL		0
#define OPTIONDEFAULTS	1
#define OPTIONMAP		2

class BlockData
{
public:
	int	backx;
	int	backy;
	char		cdata;
};

class ServerOption;
class ServerOptions;

class ServerOptionWorld;		// server defines this class which is extended with tuners

typedef	void	(ServerOptionWorld::*TunerFunc)();
typedef void	(*ServerOptionListener)(void* myThis, ServerOption* so);
typedef void	(*ServerOptionActivator)(void* myThis, ServerOption* so, bool onoff);

// Your application must define a ServerOptionFactory
// which returns your overriden (or not) ServerOption
extern ServerOption*	ServerOptionFactory(ServerOptions* _parent,
				 PCSTR _name, PCSTR _altname, PCSTR _label,
				 int _length, ValType _valType,
				 PCSTR _defval,
				 int _row, int _col, int _sheet, int _visibleto,
				 TunerFunc	_tuner,
				 PCSTR _help = NULL);

class ServerOption
{
public:
	ServerOption(ServerOptions* _parent,
				 PCSTR _name, PCSTR _altname, PCSTR _label,
				 int _length, ValType _valType,
				 PCSTR _defval,
				 int _row, int _col, int _sheet, int _visibleto,
				 TunerFunc	_tuner,
				 PCSTR _help = NULL);
	~ServerOption();

	PCSTR		name;		// These 3 come from hard coded constants,
	PCSTR		altname;	// which is why they aren't string.
	PCSTR		label;

	int			length;
	OptOrigin	optOrigin;
	OptControl	optControl;		// who is allowed to change this option
	int			optOverride;

	// map editing values.
	int			row,column,sheet;
	bool		output;
	int			visibleto;
	PCSTR		defval;
	PCSTR		help;

	ServerOptions*	parent;

	void	AddListener(ServerOptionListener sol,
						ServerOptionActivator soa,
						void* hisThis);
	void	RemoveListener(ServerOptionListener sol, void* hisThis);
	void	OptionsChanged(ServerOption* which);
	void	Activate(bool onoff);	// enable or disable the listeners of this option

	// inelegent way of checking if we are in the map editor
	bool	IsConnected() { return(listenerCount != 0); }

	void	SetLabel(PCSTR /*s*/) {};

	// Only allow access to the variable through the accessors.
	void Set(const int i);
	void Set(const String& s);
	void Set(PCSTR s);
	void Set(const double d);
	void Set(const bool b);

	void operator=(const int i)		{ Set(i); };
	void operator=(const String& s)	{ Set(s); };
	void operator=(PCSTR s)			{ Set(s); };
	void operator=(const double d)	{ Set(d); };
	void operator=(const bool b)	{ Set(b); };

	bool Set(PCSTR value, int override, OptOrigin opt_origin);

	bool Get(int& i);
	bool Get(String& s);
	bool Get(double& d);
	bool Get(bool& b);
	bool Get(ipos& ip);

	int			GetInt();
	int			GetSec();
	String		GetString();
	double		GetDouble();
	bool		GetBool();
	ObjList&	GetList();

	ValType		valType;
private:
	int			varInt;
	String		varString;
	double		varDouble;
	bool		varBool;
	ipos		varIpos;
	ObjList		varList;

	void		ScanList(PCSTR _s);
	void		GetList(String& value);

	// tune the value if required.
public:
	void		Limit(double lo, double hi);
	void		Limit(int lo, int hi);
	int			Tune(PCSTR value);
	TunerFunc	tuner;
	virtual	void DoTuner() {};

	String		GetOrigin();	// return a string rep of our optOrigin
private:
	class Listener
	{
	public:
		ServerOptionListener	listener;
		ServerOptionActivator	activator;
		void*					hisThis;
	};

	void		NotifyListeners();
	Listener*	listeners;
	int			listenerCount;
};

class ServerOptionBlocks : public ServerOption
{
public:
        ServerOptionBlocks(ServerOptions* _parent, PCSTR _n, PCSTR _an, PCSTR _l,
						   int _len,
						   PCSTR _def, int _r, int _c, int _s,
						   int _vis,
						   TunerFunc _tuner);
        BlockData	blocks[MAX_MAP_SIZE+1][MAX_MAP_SIZE+1];

		// convienence attributes reflected from the data.  Call UpdateCounts after modifying blocks
		void		UpdateCounts();
		int			numBases;
		int			numCannons;

};

inline ObjList& ServerOption::GetList() { return(varList); };
#endif
