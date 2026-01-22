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
 * $Log: ServerOption.cpp,v $
 * Revision 1.24  2004/05/23 23:52:07  dick
 * src/common/IniClient.Defaults.cpp
 *
 * Revision 1.23  2004/03/03 16:04:14  dick
 * Allow TuneFile overrides
 *
 * Revision 1.22  2004/02/28 07:43:53  dick
 * Whitespace
 *
 * Revision 1.21  2004/02/21 07:03:00  dick
 * Use HasGrandparent() to determine if we are the mapeditor or connected to
 * something bigger.  Allow OPT_MAP to override OPT_MAP settings.
 *
 * Revision 1.20  2004/02/03 03:39:54  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.19  2002/07/09 21:53:28  dick
 * OPT_TUNEFILE gets the same privileges as OPT_MAP during Set()

 Revision 1.18  2002/07/08 05:57:01  dick
 New OptOrigins: OPT_TUNE and OPT_TUNEFILE for options who's settings came
 from a "tweak" file which is loaded after the other options.

 Revision 1.17  2002/06/01 05:59:00  dick
 Add support for ipos. (used by gravityPoint)

 Revision 1.16  2002/05/22 16:03:33  dick
 Handle valSec for cannonDeadTime and maxPauseTime.

 Revision 1.15  2002/05/22 13:20:42  dick
 Handle valSec.

 Revision 1.14  2001/08/12 11:16:22  dick
 new option type valList which is an objlist.

 Revision 1.13  2001/08/07 20:55:08  dick
 Pass a this pointer to the ServerOption's listeners, so one callback can
 handle many different ServerOption(s).

 Revision 1.12  2001/08/05 08:16:02  dick
 Need operator=(bool)

 Revision 1.11  2001/08/05 08:05:37  dick
 Add Tuners to the server.  ComputeSensorRange(), AddTank(), RemoveTank()
 become members of Player.  ComputeGravity*() become members of World.

 Revision 1.10  2001/08/02 01:13:08  dick
 Zero the pointers early in the constructor.

 Revision 1.9  2001/08/01 19:42:48  dick
 New method Activate(), a partner of Listener().  When an attribute changes
 value, other attributes might be enabled or disabled.  Ex: if teamPlay is
 off, then teamCannons becomes undefined, or disabled.

 Revision 1.8  2001/08/01 17:37:16  dick
 Add ServerOption Listeners.  When a ServerOption is changed, notify all
 registered listeners. (Try opening two prefs windows on the same map).

 Revision 1.7  2001/07/24 09:04:01  dick
 Remove all global options.  Use World.options instead.

 Revision 1.6  2001/07/23 21:02:40  dick
 Support for GetDouble()

 Revision 1.5  2001/07/22 08:12:30  dick
 Add support for setting with origin and override

 Revision 1.4  2001/07/21 07:08:10  dick
 fix bug in Get(String&)

 Revision 1.3  2001/07/21 06:45:30  dick
 Hide the varInt varString, etc. and force access through the accessor funcs

 Revision 1.2  2001/07/21 03:12:53  jlmiller

 Added all the missing ServerOptions, and all the help, Replaced usage of
 STRING, POSINT etc, with ValType enums. StrToNum is no longer used by xpiloteditso killed it for now.

 Revision 1.1  2001/07/20 05:20:11  dick
 Yet another reorg.  Break server/xpilotedit common files into serverlib.

 Revision 1.4  2001/07/19 07:07:52  dick
 Linux warnings cleanup

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
#include "StdAfx.h"

#include <string.h>		// NULL on Windows
#include <stdlib.h>
#include <ctype.h>
#include "../common/config.h"
#include "../common/checknames.h"
#include "../common/error.h"
#include "ServerOption.h"
#include "ServerOptions.h"

///////////////////////////////////////////////////////////////////////////////
ServerOption::ServerOption(ServerOptions* _parent,
						   PCSTR _name, PCSTR _altname, PCSTR _label,
						   int _length, ValType _valType,
						   PCSTR _defval,
						   int _row, int _column, int _sheet, int _visibleto,
						   TunerFunc _tuner,
						   PCSTR _help)
{
	listeners = NULL;
	listenerCount = 0;
//	tuner = NULL;

	parent = _parent;
	name = _name;
	altname = _altname;
	label = _label;
	tuner = _tuner;

//	length = _length;
	valType = _valType;
	row = _row;
	column = _column;
	sheet = _sheet;
	output = false;
	visibleto = _visibleto;
	help = _help;
	optOrigin = OPT_INIT;
	optControl = (OptControl)(OPT_ORIGIN_ANY_MASK | OPT_VISIBLE);
	optOverride = 0;
	defval = _defval;

	*this = _defval;
}

///////////////////////////////////////////////////////////////////////////////
ServerOption::~ServerOption()
{
	if (listeners)
		delete listeners;
}

///////////////////////////////////////////////////////////////////////////////
ServerOptionBlocks::ServerOptionBlocks(ServerOptions* parent,
									   PCSTR n, PCSTR an, PCSTR l,
									   int len,
									   PCSTR def, int r, int c, int s,
									   int vis,
									   TunerFunc _tuner)
                : ServerOption(parent, n, an, l, len, valVoid, def, r, c, s, vis, _tuner)
{
        ;
}

///////////////////////////////////////////////////////////////////////////////
void ServerOption::Set(PCSTR s)
{
	switch (valType)
	{
	case valBool:
		varBool = (tolower(s[0]) == 'y' || tolower(s[0]) == 't');
		break;
	case valInt:
		varInt = atoi(s);
		break;
	case valString:
	case valEvent:
		varString = s;
		break;
	case valReal:
		varDouble = atof(s);
		break;
	case valIPos:
		sscanf(s, "%d,%d", &varIpos.x, &varIpos.y);
		break;
	case valList:
		ScanList(s);
		break;
	case valSec:
		varInt = atoi(s) * parent->GetFPS();
		break;
	case valVoid:
		break;		// do nothing
	default:
		xpprintf("BUG: unhandled varType in ServerOption::operator=(PCSTR) for %s\n", name);
	}
	NotifyListeners();
}

///////////////////////////////////////////////////////////////////////////////
void ServerOption::Set(const String& s)
{
	*this = s.s;
}

///////////////////////////////////////////////////////////////////////////////
void ServerOption::Set(const int i)
{
	switch (valType)
	{
	case valBool:
		varBool = (i != 0);
		break;
	case valInt:
		varInt = i;
		break;
	case valString:
		varString.printf("%d", i);
		break;
	case valReal:
		varDouble = i;
		break;
	case valSec:
		varInt = i * parent->GetFPS();
		break;
	default:
		xpprintf("BUG: unhandled varType in ServerOption::operator=(int) for %s\n", name);
	}
	NotifyListeners();
}

///////////////////////////////////////////////////////////////////////////////
void ServerOption::Set(const bool b)
{
	switch (valType)
	{
	case valBool:
		varBool = b;
		break;
	case valInt:
		varInt = b ? 1 : 0;
		break;
	case valString:
		varString = b ? "1" : "";
		break;
	case valReal:
		varDouble = b ? 1.0 : 0.0;
		break;
	default:
		xpprintf("BUG: unhandled varType in ServerOption::operator=(bool) for %s\n", name);
	}
	NotifyListeners();
}

///////////////////////////////////////////////////////////////////////////////
void ServerOption::Set(const double d)
{
	switch (valType)
	{
	case valBool:
		varBool = (d != 0);
		break;
	case valInt:
		varInt = (int)d;
		break;
	case valString:
		varString.printf("%g", d);
		break;
	case valReal:
		varDouble = d;
		break;
	case valSec:
		varInt = (int)(d * parent->GetFPS());
		break;
	default:
		xpprintf("BUG: unhandled varType in ServerOption::operator=(double) for %s\n", name);
	}
	NotifyListeners();
}

///////////////////////////////////////////////////////////////////////////////
void ServerOption::ScanList(PCSTR _s)
{
	String	s = _s;
	String	t;
	int		min;
	int		max;

	varList.Empty();
	while (s.GetLength())
	{
		char*	r;
		t = s;
		if (r = strchr(t, ','))
		{
			*r = '\0';
			r++;
			s = r;
		}
		else
			s = "";
		if (r = strchr(t, '-'))
		{
			if (sscanf(t, "%d-%d", &min, &max) != 2)
				return;
		}
		else
		{
			if (sscanf(t, "%d", &min) != 1)
				return;
			max = min;
		}
		Range* range = new Range;
		range->min = min;
		range->max = max;
		varList.Add(range);
	}
}

///////////////////////////////////////////////////////////////////////////////
void ServerOption::GetList(String& value)
{
	String	s;
	Obj*	o = varList.GetHead();
	if (!o)
	{
		value = "Empty";
		return;
	}
	while (o)
	{
		Range* r = (Range*) o;
		if (value.GetLength())
			value += ",";
		if (r->min == r->max)
			s.printf("%d", r->min);
		else
			s.printf("%d-%d", r->min, r->max);
		value += s;
		o = o->GetNext();
	}
}

///////////////////////////////////////////////////////////////////////////////
bool ServerOption::Get(String& value)
{
    switch (valType)
	{
    case valInt:
		value.printf("%d", varInt);
		break;
    case valReal:
		value.printf("%g", varDouble);
		break;
    case valBool:
		value.printf("%s", varBool ? "yes" : "no");
		break;
    case valString:
		value.printf("%s", (PCSTR)varString);
		break;
    case valSec:
		value.printf("%d", varInt / parent->GetFPS());
		break;
    case valPerSec:
		value.printf("%g", varDouble * parent->GetFPS());
		break;
    case valIPos:
		value.printf("%d,%d", varIpos.x, varIpos.y);
		break;
	case valList:
		GetList(value);
		break;
	case valEvent:
		value.printf("%s", (PCSTR)varString);
		break;

    default:
		return false;
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
String ServerOption::GetString()
{
	String s;
	Get(s);
	return(s);
}

///////////////////////////////////////////////////////////////////////////////
bool ServerOption::Get(int& value)
{
    switch (valType)
	{
    case valInt:
		value = varInt;
		break;
    case valReal:
		value = (int)varDouble;
		break;
    case valBool:
		value = varBool;
		break;
    case valString:
		value = atoi(varString);
		break;
    case valSec:
		value = varInt / parent->GetFPS();
		break;
/* ... incomplete ...
    case valPerSec:
		value.printf(value, "%g", varDouble * parent->GetFPS());
		break;
    case valIPos:
		value.printf(value, "%d, %d", varIpos.x, varIpos.y);
		break;
*/
    default:
		return false;
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
int ServerOption::GetInt()
{
	int i;
	Get(i);
	return(i);
}

///////////////////////////////////////////////////////////////////////////////
int ServerOption::GetSec()
{

    switch (valType)
	{
    case valInt:
		return(varInt*parent->GetFPS());
    case valReal:
		return((int)(varDouble*parent->GetFPS()));
    case valBool:
		return(varBool);
    case valString:
		return(atoi(varString)*parent->GetFPS());
    case valSec:
		return(varInt);
    default:
		return false;
	}
}

///////////////////////////////////////////////////////////////////////////////
bool ServerOption::Get(bool& value)
{
    switch (valType)
	{
    case valInt:
		value = varInt != 0;
		break;
    case valReal:
		value = varDouble != 0.0;
		break;
    case valBool:
		value = varBool;
		break;
    case valString:
		value = atoi(varString) != 0;
		break;
/* ... incomplete ...
    case valSec:
		value.printf(value, "%d", varInt / parent->GetFPS());
		break;
    case valPerSec:
		value.printf(value, "%g", varDouble * parent->GetFPS());
		break;
    case valIPos:
		value.printf(value, "%d, %d", varIpos.x, varIpos.y);
		break;
*/
    default:
		return false;
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool ServerOption::GetBool()
{
	bool b;
	Get(b);
	return(b);
}

///////////////////////////////////////////////////////////////////////////////
bool ServerOption::Get(double& value)
{
    switch (valType)
	{
    case valInt:
		value = varInt;
		break;
    case valReal:
		value = varDouble;
		break;
    case valBool:
		value = varBool;
		break;
    case valString:
		value = atof(varString);
		break;
/* ... incomplete ...
    case valSec:
		value.printf(value, "%d", varInt / parent->GetFPS());
		break;
    case valPerSec:
		value.printf(value, "%g", varDouble * parent->GetFPS());
		break;
    case valIPos:
		value.printf(value, "%d, %d", varIpos.x, varIpos.y);
		break;
*/
    default:
		return false;
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool ServerOption::Get(ipos& ip)
{
	ip = varIpos;
	return(true);
}

///////////////////////////////////////////////////////////////////////////////
double ServerOption::GetDouble()
{
	double d;
	Get(d);
	return(d);
}

///////////////////////////////////////////////////////////////////////////////
void ServerOption::Limit(int lo, int hi)
{
	int	val = GetInt();
	LIMIT(val, lo, hi);
	if (val != GetInt())
	{
		Set(val);
	}
}

///////////////////////////////////////////////////////////////////////////////
void ServerOption::Limit(double lo, double hi)
{
	double	val = GetDouble();
	LIMIT(val, lo, hi);
	if (val != GetDouble())
	{
		Set(val);
	}
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Convert an option origin enumerated constant
 * to a character representation.
 */
static const char* Origin_name(OptOrigin opt_origin)
{
    const char *source;

    switch (opt_origin) {
	case OPT_COMMAND: source = "command line"; break;
	case OPT_PASSWORD: source = "password file"; break;
	case OPT_DEFAULTS: source = "defaults file"; break;
	case OPT_MAP: source = "map file"; break;
	case OPT_TUNE: source = "tuned file"; break;
	default: source = "unknown origin"; break;
    }

    return source;
}

///////////////////////////////////////////////////////////////////////////////
String	ServerOption::GetOrigin()
{
	String	s;
	if (optOrigin & OPT_INIT)
		s += "INIT";
	if (optOrigin & OPT_PASSWORD)
	{
		if (!s.IsEmpty())
			s += "|";
		s += "PASSWORD";
	}
	if (optOrigin & OPT_PASSWORD)
	{
		if (!s.IsEmpty())
			s += "|";
		s += "PASSWORD";
	}
	if (optOrigin & OPT_DEFAULTS)
	{
		if (!s.IsEmpty())
			s += "|";
		s += "DEFAULTS";
	}
	if (optOrigin & OPT_MAP)
	{
		if (!s.IsEmpty())
			s += "|";
		s += "MAP";
	}
	if (optOrigin & OPT_TUNEFILE)
	{
		if (!s.IsEmpty())
			s += "|";
		s += "TUNEFILE";
	}
	if (optOrigin & OPT_TUNE)
	{
		if (!s.IsEmpty())
			s += "|";
		s += "TUNE";
	}
	if (s.IsEmpty())
		s = "UNKNOWN";
	return(s);
}

///////////////////////////////////////////////////////////////////////////////
bool ServerOption::Set(PCSTR value, int override, OptOrigin opt_origin)
{
    bool	set_ok = FALSE;
	/* check option description permissions. */
	if (!((int)optControl & (int)opt_origin))
	{
		warn("Not allowed to change option '%s' from %s.",
		      name, Origin_name(opt_origin));
		return(false);
	}

	switch (opt_origin) {
	case OPT_COMMAND:
		/* command line always overrides */
		set_ok = TRUE;
		break;

	case OPT_DEFAULTS:
		switch (optOrigin) {
		case OPT_COMMAND:
			/* never modify command line arg. */
			break;

		case OPT_DEFAULTS:
			/* can't change if previous value has override. */
			if (!optOverride) {
				set_ok = TRUE;
			}
			break;

		case OPT_MAP:
			/* defaults file override wins over map. */
			if (override) {
				set_ok = TRUE;
			}
			break;

		case OPT_PASSWORD:
			/* never modify if set by password file. */
			break;

		case OPT_INIT:
			set_ok = TRUE;
			break;

		default:
			fatal("unknown node->value origin in set value");
		}
		break;

	case OPT_MAP:
	case OPT_TUNEFILE:
		switch (optOrigin) {
		case OPT_COMMAND:
			/* never modify command line arg. */
			break;

		case OPT_DEFAULTS:
			/* can't change if defaults value has override. */
			if (!optOverride) {
			    set_ok = TRUE;
			}
			break;

		case OPT_MAP:
			/* can't change if previous value has override. */
			//if (!optOverride) {
			    set_ok = TRUE;
			//}
			break;

		case OPT_PASSWORD:
			/* never modify if set by password file. */
			break;

		case OPT_INIT:
		case OPT_TUNE:
		case OPT_TUNEFILE:
			set_ok = TRUE;
			break;

		default:
			fatal("unknown node->value origin in set value");
		}
		break;

	case OPT_PASSWORD:
		switch (optOrigin) {
		case OPT_COMMAND:
			/* never modify command line arg. */
			break;

		case OPT_DEFAULTS:
			/* password file always wins over defaults. */
			set_ok = TRUE;
			break;

		case OPT_MAP:
			/* password file always wins over map. */
			set_ok = TRUE;
			break;

		case OPT_PASSWORD:
			/* can't change if previous value has override. */
			if (!optOverride) {
				set_ok = TRUE;
			}
			break;

		case OPT_INIT:
			set_ok = TRUE;
			break;

		default:
			fatal("unknown node->value origin in set value");
		}
		break;

	default:
		fatal("unknown opt_origin in set value");
	}

	if (set_ok == TRUE)
	{
		optOverride = override;
		optOrigin = opt_origin;
		Set(value);
		return(true);
	}
	return(false);

#if 0
	// DIKSEZ: i don't understand why all this
	// so i just brought Option_change_node here
    hash_node	*np;
    hash_value	*vp;
    int		ix = Option_hash_string(name);

    for (np = Option_hash_array[ix]; np; np = np->next) {
	if (!strcasecmp(name, np->name)) {
	    Option_change_node(np, value, override, opt_origin);
	    return;
	}
    }

    if (!value) {
	return;
    }

    vp = Option_allocate_value(value, NULL, opt_origin);
    if (!vp) {
	exit(1);
    }
    vp->override = override;

    np = Option_allocate_node(name, vp);
    if (!np) {
	exit(1);
    }

    np->next = Option_hash_array[ix];
    Option_hash_array[ix] = np;
#endif
}

///////////////////////////////////////////////////////////////////////////////
int ServerOption::Tune(PCSTR val)
{
	String			sval(val);
	int				ival;
	double			fval;

	switch (valType)
	{
	case valInt:
		if (sval.ToInt(&ival) != TRUE)
			return 0;
		varInt = ival;
		DoTuner();
		return 1;
	case valBool:
		if (ON(val))
			varBool = true;
		else if (OFF(val))
			varBool = false;
		else
			return 0;
		DoTuner();
		return 1;
	case valReal:
		if (sval.ToDouble(&fval) != TRUE)
			return 0;
		varDouble = fval;
		DoTuner();
		return 1;
	case valSec:
		if (sval.ToInt(&ival) != TRUE)
			return 0;
		varInt = ival * parent->GetFPS();
		DoTuner();
		return 1;
	case valPerSec:
		if (sval.ToDouble(&fval) != TRUE)
			return 0;
		varDouble = fval / parent->GetFPS();
		DoTuner();
		return 1;
	case valString:
		varString = val;
		DoTuner();
		return 1;
	case valEvent:
		DoTuner();
		return(1);
	default:
		xpprintf("BUG: TuneOption %s has no varType\n", name);
		return -1;	/* Operation undefined */
	}
}

///////////////////////////////////////////////////////////////////////////////
void ServerOption::AddListener(ServerOptionListener sol,
							   ServerOptionActivator soa,
							   void* hisThis)
{
	if (!listeners)
	{
		listeners = new Listener;
		listeners[0].listener = sol;
		listeners[0].activator = soa;
		listeners[0].hisThis = hisThis;
		listenerCount = 1;
	}
	else
	{
		Listener*	newListeners;
		newListeners = new Listener[listenerCount + 1];
		for (int i=0; i<listenerCount; i++)
			newListeners[i] = listeners[i];
		newListeners[listenerCount].listener = sol;
		newListeners[listenerCount].activator = soa;
		newListeners[listenerCount].hisThis = hisThis;
		delete listeners;
		listeners = newListeners;
		listenerCount++;
	}
}

///////////////////////////////////////////////////////////////////////////////
void ServerOption::RemoveListener(ServerOptionListener sol, void* hisThis)
{
	int		i;
	int		j;
	bool	match = false;
	for (i=0; i<listenerCount; i++)
	{
		if (listeners[i].listener == sol && listeners[i].hisThis == hisThis)
			match = true;
	}
	if (match)
	{
		Listener*	newListeners;
		newListeners = new Listener[listenerCount - 1];
		for (i=0, j=0; i<listenerCount; i++, j++)
		{
			if (listeners[i].listener == sol && listeners[i].hisThis == hisThis)
				j--;
			else
				newListeners[j] = listeners[i];
		}
		delete listeners;
		listeners = newListeners;
		listenerCount--;

	}
}

///////////////////////////////////////////////////////////////////////////////
void ServerOption::NotifyListeners()
{
	int i;
	for (i=0; i<listenerCount; i++)
	{
		listeners[i].listener(listeners[i].hisThis, this);
	}
	parent->OptionsChanged(this);
}

///////////////////////////////////////////////////////////////////////////////
void ServerOption::Activate(bool onoff)
{
	int i;
	for (i=0; i<listenerCount; i++)
	{
		if (listeners[i].activator)
			listeners[i].activator(listeners[i].hisThis, this, onoff);
	}
}

