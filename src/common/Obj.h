/* $Id: Obj.h,v 1.14 2004/04/27 01:17:22 dick Exp $
 *
 * XPwhere - Where in the world can i find people playing XPilot.
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
 * $Log: Obj.h,v $
 * Revision 1.14  2004/04/27 01:17:22  dick
 * Some Rect manipulation functions
 *
 * Revision 1.13  2004/02/28 07:28:04  dick
 * Add a Rect(x,y, x,y) constructor
 *
 * Revision 1.12  2002/07/23 17:51:13  dick
 * ParseHeaders(const char sep) given a list of LStrings, break each one up
 * so that the name becomes everything to the left of sep, and the s becomes
 * everything to the right.  Useful for parsing HTTP headers (among other things.)
 *
 * Revision 1.11  2002/07/17 23:55:47  dick
 * SortInto becomes SortObjInto and SortSortableInto because of the extra level
 * of indirection in an ObjSortable.
 *
 * Revision 1.10  2002/06/30 06:23:21  dick
 * New ObjSortable, an object which contains a pointer to an object.
 * These can be sorted in an ObjList without messing with the original list.
 *
 * Revision 1.9  2002/06/25 04:22:52  dick
 * New function int Index(Obj*) returns how far into a list is this object.
 *
 * Revision 1.8  2002/06/23 06:17:00  dick
 * Rect tweaks
 *
 * Revision 1.7  2002/06/20 22:24:33  dick
 * Objs can have a type.  Add a simple Rect.
 *
 * Revision 1.6  2002/06/09 18:30:33  dick
 * Array index operator on a list.  Add some constness.
 *
 * Revision 1.5  2002/05/22 17:25:29  dick
 * Add func, a general object callback.  ObjList can PumpFuncs().
 *
 * Revision 1.4  2001/09/28 18:56:00  dick
 * New function ObjList::RemoveTail()
 *
 * Revision 1.3  2001/08/12 11:02:00  dick
 * new object Range, which contains a min and a max int.
 *
 * Revision 1.2  2001/08/07 20:50:44  dick
 * Commentality
 *
 * Revision 1.1  2001/07/26 10:54:50  dick
 * A Base object that provides linked list support
 *
 */

/***************************************************************************\
*  obj.h - The root of all evil.  Base objects								*
*																			*
*  Part of the BuckoSoft Root Project										*
*  Copyright� 1998-1999 - BuckoSoft, Inc.									*
*																			*
*  $Id: Obj.h,v 1.14 2004/04/27 01:17:22 dick Exp $							*
\***************************************************************************/

#ifndef	_OBJ_H_
#define	_OBJ_H_

#include <stdio.h>
#include <time.h>

#include "cstring.h"

typedef const char* otype;

class Obj;
class ObjSortable;

typedef	void (Obj::*ObjFunc)(void* data);

class Obj {
  public:
	// classic linked list support
	Obj*	prev;
	Obj*	next;

	// most objects are named.  Makes it easy to find them.
	String	name;

	// The object type.  I wanted to use some kind of IsKindOf() scheme
	// but i couldn't find one that was platform portable
	otype	type;

	// construct an empty object
	Obj() {prev = NULL; next = NULL; func = NULL; type = NULL; }

	// contruct a named empty object
//	obj(PCSTR _name) {prev = NULL; next = NULL; name = _name; }

	// overridable destructor
	virtual ~Obj() {};

	// get the next object in the list.
	// There are too many cast-the-return-value associated with this.
	// Templates would probably clean that up --- if they worked on all platforms
	Obj*	GetNext() const { return(next); }

	// An object can have a general callback.
	// objlist can pump all callbacks in his list.

	ObjFunc	func;

	virtual	bool	Sort(ObjSortable* other, int priSort = 0, int secSort = 0);

// A failed experiment at getting each object's source file id.
// On Windoze anyway, it seemed to create a cvsid string with each object
// created, which was not the idea and is a waste of CPU time.
//	static const char* CvsId();
//protected:
//	static const char* cvsid;
};

class ObjSortable : public Obj
{
public:
	Obj*	o;				// point to the object we sorted
	virtual	bool	Sort(ObjSortable* other, int priSort = 0, int secSort = 0);
};

///////////////////////////////////////////////////////////////////////////////
// A Classic linked list of objects
// We own the objects
class ObjList : public Obj {
  public:
	ObjList() { head = NULL; tail = NULL; };
	virtual		~ObjList();
	Obj*		GetHead() const { return(head); };
	Obj*		GetTail() const { return(tail); };
	void		Add(Obj* o);
	void		InsertBefore(Obj* place, Obj* _new);
	void		Add(PCSTR name, int value);
	void		Add(PCSTR name, PCSTR value);
	int			GetValue(PCSTR name);			// return the value of the named object
	void		Remove(Obj* o);
	Obj*		RemoveHead();
	Obj*		RemoveTail();
	void		Empty();						// make the list be empty
	int			GetCount();
	int			Index(const Obj* who) const;	// how far down the list is this guy
	bool		IsEmpty();
	Obj*		Find(PCSTR s) const;
	ObjSortable* FindSortable(PCSTR s) const;
	Obj*		operator[](int n);				// get the nth object
	void		SortObjInto(ObjList& into, int priSort = 0, int secSort = 0);
	void		SortSortableInto(ObjList& into, int priSort = 0, int secSort = 0);
	void		Sort(int priSort = 0, int secSort = 0);
	void		ParseHeaders(const char sep);

	// Run through all the objects calling it's func() passing v
	void		PumpFuncs(void* v);

  private:
	Obj*	head;
	Obj*	tail;
};

#if 0
///////////////////////////////////////////////////////////////////////////////
// A linked list that we own the objects
class ObjList : public ObjListNoOwn
{
public:
	virtual	~ObjList();
};
#endif

///////////////////////////////////////////////////////////////////////////////
// A linkable string
class LString : public Obj {
  public:
	LString() { /* type = s_lstring; */};
	LString(PCSTR ns) { /* type = s_lstring; */ name = ns; };

	String	s;
	operator PCSTR() { return((PCSTR)s.s); };
	void	operator=(String& ns) { s = ns; };
};

class Range : public Obj {
  public:
	int		min;
	int		max;
};

class Rect {
  public:
	Rect()	{ top = 0; left = 0; bottom = 0; right = 0; }
	Rect(int x0, int y0, int x1, int y1)
		{ top = y0; left = x0; bottom = y1; right = x1; }
	void	Set(int x0, int y0, int x1, int y1);
	void	SetNull();
	bool	InRect(int x, int y);
	long	top;
	long	left;
	long	bottom;
	long	right;
};
inline bool Rect::InRect(int x, int y)
							{ return(x >= left && x <= right && y >= top && y <= bottom); };
inline void Rect::SetNull() { top = 0; left = 0; bottom = 0; right = 0; };
inline void	Rect::Set(int x0, int y0, int x1, int y1)
							{ left = x0; top = y0; right = x1; bottom = y1; };

#endif	/* _OBJ_H_ */
