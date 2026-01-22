/* $Id: Obj.cpp,v 1.20 2007/02/17 06:10:47 dick Exp $
 *
 * XPwhere - Where in the world can i find people playing XPilot.
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
 * $Log: Obj.cpp,v $
 * Revision 1.20  2007/02/17 06:10:47  dick
 * Add GetAt(index) which is the same as operator[].
 *
 * Revision 1.19  2004/05/13 09:06:27  dick
 * operator[] returns NULL rather than walk off the end of the list and crashing
 *
 * Revision 1.18  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.17  2002/09/05 22:31:33  cppc
 * Fixed bug in InsertBefore with uninitialized pointer assignment.
 *
 * Revision 1.16  2002/07/23 17:51:13  dick
 * ParseHeaders(const char sep) given a list of LStrings, break each one up
 * so that the name becomes everything to the left of sep, and the s becomes
 * everything to the right.  Useful for parsing HTTP headers (among other things.)
 *
 * Revision 1.15  2002/07/17 23:55:47  dick
 * SortInto becomes SortObjInto and SortSortableInto because of the extra level
 * of indirection in an ObjSortable.
 *
 * Revision 1.14  2002/07/14 18:58:29  dick
 * Sorting is done on ObjSortable, not Obj
 *
 * Revision 1.13  2002/06/30 06:23:21  dick
 * New ObjSortable, an object which contains a pointer to an object.
 * These can be sorted in an ObjList without messing with the original list.
 *
 * Revision 1.12  2002/06/25 04:22:52  dick
 * New function int Index(Obj*) returns how far into a list is this object.
 *
 * Revision 1.11  2002/06/23 06:17:10  dick
 * Deadwood delete
 *
 * Revision 1.10  2002/06/09 18:30:33  dick
 * Array index operator on a list.  Add some constness.
 *
 * Revision 1.9  2002/05/30 18:36:13  jlmiller
 * Dik left an infinite loop
 *
 * Revision 1.8  2002/05/22 17:25:29  dick
 * Add func, a general object callback.  ObjList can PumpFuncs().
 *
 * Revision 1.7  2002/02/14 22:06:24  jlmiller
 * When calling Remove, run RemoveTail instead if necessary.
 *
 * Revision 1.6  2001/10/23 02:20:43  dick
 * Set the previous pointer when removing an object from ObjList
 *
 * Revision 1.5  2001/09/28 18:55:59  dick
 * New function ObjList::RemoveTail()
 *
 * Revision 1.4  2001/09/26 08:12:14  dick
 * Deadwood delete
 *
 * Revision 1.3  2001/09/20 06:10:59  dick
 * Commentality
 *
 * Revision 1.2  2001/07/26 12:13:11  dick
 * Linux XPwhere integration cleanup
 *
 * Revision 1.1  2001/07/26 10:54:50  dick
 * A Base object that provides linked list support
 *
 */
#include "StdAfx.h"

#include <string.h>
#include <ctype.h>
#include "error.h"
#include "Obj.h"


///////////////////////////////////////////////////////////////////////////////
ObjList::~ObjList()
{
	Obj*	o = GetHead();
	while (o)
	{
		Obj* o1 = o->GetNext();
		delete o;
		o = o1;
	}
}

///////////////////////////////////////////////////////////////////////////////
void ObjList::Add(Obj* o)
{
	Obj* t = GetTail();
	if (!t)
	{
		head = o;
		t = head;
		tail = o;
		return;
	}
	t->next = o;
	o->prev = t;
	tail = o;
}

///////////////////////////////////////////////////////////////////////////////
void ObjList::InsertBefore(Obj* p, Obj* n)
{
	Obj* o = GetHead();
	Obj* o1;

	if (!p)
	{
		Add(n);
	}
	else
	{
		if (p == o)
		{
			head = n;
			n->next = p;
			n->prev = NULL;
			p->prev = n;
		}
		else
		{
			o1 = o;
			for (o = o->next; o != NULL; o1 = o, o = o->next)
			{
				if (o == p)
				{
					o1->next = n;
					n->prev = o1;
					o->prev = n;
					n->next = o;
					break;
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
void ObjList::Add(PCSTR n, int value)
{
	char	buf[32];
	LString* ls = new LString;
	ls->name = n;
	sprintf(buf, "%d", value);
	ls->s = buf;
	Add(ls);
}

///////////////////////////////////////////////////////////////////////////////
void ObjList::Add(PCSTR n, PCSTR value)
{
	LString* ls = new LString;
	ls->name = n;
	ls->s = value;
	Add(ls);
}

///////////////////////////////////////////////////////////////////////////////
int	ObjList::GetValue(PCSTR name)
{
	LString* ls = (LString*)Find(name);
	if (!ls)
		return(0);
	return(ls->s.atoi());
}

///////////////////////////////////////////////////////////////////////////////
void ObjList::Empty()
{
	Obj*	o = GetHead();
	
	while (o)
	{
		Obj* o1 = o->GetNext();
		delete o;
		o = o1;
	}
	
	head = NULL;
	tail = NULL;
}

///////////////////////////////////////////////////////////////////////////////
int ObjList::GetCount()
{
	int		i = 0;
	Obj*	o = GetHead();
	while (o)
	{
		i++;
		o = o->GetNext();
	}
	return(i);
}

///////////////////////////////////////////////////////////////////////////////
Obj* ObjList::operator[](int n)
{
	Obj* o = GetHead();
	if (!o)
		return(NULL);
	for (int i=0; i<n && o; o = o->GetNext(), i++)
		;
	return(o);
}

///////////////////////////////////////////////////////////////////////////////
Obj* ObjList::GetAt(int n)
{
	Obj* o = GetHead();
	if (!o)
		return(NULL);
	for (int i=0; i<n && o; o = o->GetNext(), i++)
		;
	return(o);
}

///////////////////////////////////////////////////////////////////////////////
bool ObjList::IsEmpty()
{
	if (GetHead())
		return(false);
	return(true);
}

///////////////////////////////////////////////////////////////////////////////
Obj* ObjList::Find(PCSTR f) const
{
	Obj*	o = GetHead();
	while(o)
	{
		if (!strcmp(f, o->name))
			return(o);
		o = o->GetNext();
	}
	return(0);
}

///////////////////////////////////////////////////////////////////////////////
ObjSortable* ObjList::FindSortable(PCSTR f) const
{
	ObjSortable*	o = (ObjSortable*)GetHead();
	while(o)
	{
		if (!strcmp(f, o->o->name))
			return(o);
		o = (ObjSortable*)o->GetNext();
	}
	return(0);
}

///////////////////////////////////////////////////////////////////////////////
int ObjList::Index(const Obj* who) const
{
	Obj*	o = GetHead();
	int		i = 0;
	while(o)
	{
		if (o == who)
			return(i);
		i++;
		o = o->GetNext();
	}
	return(-1);
}

///////////////////////////////////////////////////////////////////////////////
void ObjList::Remove(Obj* o)
{
	Obj*	o1 = GetHead();
	Obj*	o2 = GetTail();
	if (o == o1)
	{
		head = o1->GetNext();
		o->next = NULL;
		o->prev = NULL;
		if(head) 
			head->prev = NULL;
		if(!head) 
			tail = NULL;
		return;
	}
	if (o == o2)
	{
		RemoveTail();
		return;
	}
	while (o1)
	{
		if (o1->GetNext() == o)
		{
			o1->next = o->GetNext();
			if (o1->next)
				o1->next->prev = o1;
			o->next = NULL;
			o->prev = NULL;
			return;
		}
		o1 = o1->GetNext();
	}
//	bprintf("ObjList: Bogus! can't remove Object type %d from %s\n", o->type, (PCSTR)name);
}

///////////////////////////////////////////////////////////////////////////////
Obj* ObjList::RemoveHead()
{
	Obj*	o = GetHead();
	if (!o)
		return(NULL);
	head = o->GetNext();
	if (head) head->prev = NULL;
	if(!head) tail = NULL;
	o->next = NULL;
	o->prev = NULL;
	return(o);
}

///////////////////////////////////////////////////////////////////////////////
Obj* ObjList::RemoveTail()
{
	Obj* o = GetTail();
	if (!o)
		return(NULL);
	Obj* o1 = o->prev;
	if (!o1)
		head = tail = NULL;
	else
		o1->next = NULL;
	tail = o1;
	o->next = NULL;
	o->prev = NULL;
	return(o);
}

///////////////////////////////////////////////////////////////////////////////
void ObjList::PumpFuncs(void* v)
{
	Obj* o = GetHead();
	while (o)
	{
		if (o->func)
			(o->*func)(v);
		o = o->GetNext();
	}
}

///////////////////////////////////////////////////////////////////////////////
bool Obj::Sort(ObjSortable* other, int priSort, int secSort)
{
	if (this < other->o)	// default is to sort by (the useless) object address
		return(false);		// hopefully this is overridden to something more meaningful
	return(true);
}

///////////////////////////////////////////////////////////////////////////////
bool ObjSortable::Sort(ObjSortable* other, int priSort, int secSort)
{
	if (this < other)		// default is to sort by (the useless) object address
		return(false);		// hopefully this is overridden to something more meaningful
	return(true);
}

///////////////////////////////////////////////////////////////////////////////
void ObjList::SortObjInto(ObjList& into, int priSort, int secSort)
{
	Obj*	o = (Obj*)GetHead();

	while (o)
	{
		ObjSortable*	o1 = (ObjSortable*)into.GetHead();
		while (o1 && o->Sort(o1, priSort, secSort))
			o1 = (ObjSortable*)o1->GetNext();
		ObjSortable*	newo = new ObjSortable;
		newo->o = o;
		into.InsertBefore(o1, newo);
		o = (Obj*)o->GetNext();
	}
}

///////////////////////////////////////////////////////////////////////////////
void ObjList::SortSortableInto(ObjList& into, int priSort, int secSort)
{
	ObjSortable*	o = (ObjSortable*)GetHead();

	while (o)
	{
		ObjSortable*	o1 = (ObjSortable*)into.GetHead();
		while (o1 && o->o->Sort(o1, priSort, secSort))
			o1 = (ObjSortable*)o1->GetNext();
		ObjSortable*	newo = new ObjSortable;
		newo->o = o->o;
		into.InsertBefore(o1, newo);
		o = (ObjSortable*)o->GetNext();
	}
}

///////////////////////////////////////////////////////////////////////////////
void ObjList::Sort(int priSort, int secSort)
{
	bool swapped = true;
	while (swapped)
	{
		swapped = false;
		ObjSortable* o = (ObjSortable*)GetHead();
		while (o)
		{
			ObjSortable* o1 = (ObjSortable*)o->GetNext();
			if (o1 && o->o->Sort(o1, priSort, secSort))
			{
				// Trace("Swap %s and %s\n", (PCSTR)o->name, (PCSTR)o1->name);
				Obj* ox;
				ox = o1->o;
				o1->o = o->o;
				o->o = ox;
				swapped = true;
				break;
			}
			o = (ObjSortable*)o->GetNext();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// Turn a list of lstrings into a key/value linked list
//
void ObjList::ParseHeaders(const char sep)
{

	LString*	ls = (LString*)GetHead();
	while (ls ) // && *(PCSTR)ls->s != '\n')		// until we do all headers or run out of strings
	{
		String	s;
		char*	u;
		s = (PCSTR)ls->s;
		u = (PSTR)(PCSTR)s;
		while (*u && *u != sep)
			u++;
		if (*u)
		{
			*u = '\0';
			ls->name = (PCSTR)s;
			u++;
			while (*u && isspace(*u))
				u++;
			ls->s = u;
		}
		if (ls->s[ls->s.GetLength()-1] == '\n')
			ls->s.SetAt(ls->s.GetLength()-1, '\0');
		ls = (LString*)ls->GetNext();
	}
}

