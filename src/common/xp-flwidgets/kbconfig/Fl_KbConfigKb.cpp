/* $Id: Fl_KbConfigKb.cpp,v 1.10 2004/02/03 03:39:53 dick Exp $
 *
 * Fl_KbConfig - Display a picture of a keyboard with all of the keys defined
 *
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
 * $Log: Fl_KbConfigKb.cpp,v $
 * Revision 1.10  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.9  2002/09/15 13:58:03  dick
 * The first time we init a kbconfig we have to swirl
 * the bitmaps,  The second time, we don't have to
 * (because they have already been fixed)
 *
 * Revision 1.8  2002/09/02 15:35:19  dick
 * Singleton ini becomes iniClient.
 *
 * Revision 1.7  2002/07/19 17:09:59  dick
 * Don't walk through widgets that aren't our keys by setting user_data to non-null.
 *
 * Revision 1.6  2001/10/04 17:33:39  dick
 * Use uint instead of UINT.
 *
 * Revision 1.5  2001/10/04 16:21:40  dick
 * New UpdateAllCommands() resets the buttons' pointers after shuffling the
 * keyDefs table.  UpdateUnusedCommands() scans for unused commands and adds
 * them to the Unused button.
 *
 * Revision 1.4  2001/10/01 13:49:56  dick
 * wrap printfs with DEBUGEVENTS and DEBUGSELECTION to control the noise.
 *
 * Revision 1.3  2001/09/27 18:03:47  dick
 * Use strcasecmp, not stricmp
 *
 * Revision 1.2  2001/09/27 17:39:34  dick
 * Better selecting the key under the mouse.
 * Parse off a bit of the head of the keyboard definition string, like name.
 *
 * Revision 1.1  2001/09/25 15:45:52  dick
 * Split KbConfig to KbConfig and KbConfigKb, since the keyboard is only a part
 * of the whole KbConfig window.
 *
 * Revision 1.1  2001/09/20 07:41:02  dick
 * Move Fl_Kb* to it's own library xp-flwidgets/kbconfig/libxp-kbconfig.a
 *
 * Revision 1.10  2001/09/20 07:21:09  dick
 * If a button has multiple commands, display those in a "flyout" window
 * when the mouse is over the key.
 *
 * Revision 1.9  2001/09/17 23:03:21  dick
 * Comment out debug
 *
 * Revision 1.8  2001/09/17 10:57:42  dick
 * Use ushort, not WORD
 *
 * Revision 1.7  2001/09/17 08:57:35  dick
 * Draw the first command associated with each key.  Move Button to his own file.
 *
 * Revision 1.6  2001/09/07 21:24:03  dick
 * Handle mouse move events and highlight the key the mouse is over.
 *
 * Revision 1.5  2001/09/07 07:41:17  dick
 * Hightlight the key pressed.
 *
 * Revision 1.4  2001/09/06 00:07:39  dick
 * Add XPKbConfig.exe to the Windows binary distribution.
 *
 * Revision 1.3  2001/09/05 13:24:20  dick
 * Linux needs stdlib.h
 *
 * Revision 1.2  2001/09/05 13:16:21  dick
 * Draw a full generic US keyboard
 *
 * Revision 1.1  2001/09/05 09:15:41  dick
 * Display a picture of a keyboard with all of the keys defined.
 *
 */
#include "StdAfx.h"

#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "FL/Fl.H"
#include "FL/fl_draw.H"
#include "FL/Fl_Window.H"
#include "FL/Fl_Scroll.H"

#include "Fl_KbCButton.h"
#include "Fl_KbMultiComm.h"
#include "Fl_KbConfigKb.h"
#include "Fl_KbDragger.h"
#include "Fl_KbConfig.h"

#include "const.h"
#include "IniClient.h"


extern Fl_Color	xpblue;

bool	bitsFixed = false;	// The first time we init a kbconfig we have to swirl
							// the bitmaps,  The second time, we don't have to
							// (because they have already been fixed)

///////////////////////////////////////////////////////////////////////////////
Fl_KbConfigKb::Fl_KbConfigKb(int X, int Y, int W, int H, const char* label)
: Fl_KbConfigKbSUPERCLASS(X,Y,W,H,label)
{
	box(FL_FLAT_BOX);
	unusedButton = 0;
	copyButton = 0;
}

///////////////////////////////////////////////////////////////////////////////
void Fl_KbConfigKb::Init(PCSTR kbs)
{
	Fl_KbCButton* b;
	begin();
	lastlastcol = 0;	// find the end of the keyboard (it's size in pixels)
	lastMouseKey = 0;	// The key that the mouse is over
	String	s = kbs;
	String	l;
	String	s1;
	while (s.GetLength())
	{
		char* t = strchr(s, '\n');	// Get the title
		if (!t)
			break;
		*t++ = 0;
		l = s;
		s1 = t;
		s = s1;
		// l = line. s = remaining
		t = l.s;
		if (*t == '#')
			continue;
		if (!strcasecmp(t, "<NAME>"))
		{
			t += 6;				// skip <NAME>
			name = t;
			continue;
		}
		if (!strcasecmp(t, "<KEYS>"))
			break;
	}
	while (s.GetLength())
	{
		char* t = strchr(s, '\n');
		if (!t)
			break;
		*t++ = 0;
		l = s;
		s1 = t;
		s = s1;
		// printf("line: <%s>\n", (PCSTR)l);
		if (l[0] == '#')
			continue;
		String	name;
		String	row;
		String	width;
		String	height;
		char* tok;
		if ((tok = strtok(l.s, sepa)) == NULL) break;
		name = tok;
		if ((tok = strtok(NULL, sepa)) == NULL) break;
		row = tok;
		if ((tok = strtok(NULL, sepa)) == NULL) break;
		width = tok;
		if ((tok = strtok(NULL, sepa)) == NULL) break;
		height = tok;
		int _r = row.atoi();
		Obj* o = keyChain.GetHead();
		int	lastcol = 0;
		while (o)
		{
			KbCKey* k = (KbCKey*)o;
			if (k->kr == _r)
			{
				lastcol += (int)(k->kw * BUTTON_WIDTH);
				if (lastlastcol < lastcol)
					lastlastcol = lastcol;
			}
			o = o->GetNext();
		}
		int _w = (int)(atof(width)*BUTTON_WIDTH);
		b = new Fl_KbCButton(lastcol, _r*(BUTTON_HEIGHT),
						 _w, height.atoi()*BUTTON_HEIGHT);
		if (lastlastcol < lastcol+_w)
			lastlastcol = lastcol+_w;
		b->kr = _r;
		b->kw = atof(width);
		b->kh = height.atoi();
		b->ktype = ButtonTypeNone;
		b->keyDef = XStringToXPKeyDef(name);
//		b->config = this;
		b->SetConfig(this);
		if (b->keyDef)
			b->kname = b->keyDef->c;
		else
		{
			name += "??";
			b->kname = name;
		}

		if (name[0] == '!')
			b->ktype = ButtonTypeBlank;
		keyChain.Add(b);
		if (b->keyDef)
			GetCommands(b->GetCommands(), b);
	}
	end();
	// ok, now figure out the size of the window.
	int lastrow = 0;
	Obj* o = keyChain.GetHead();
	while (o)
	{
		KbCKey* k = (KbCKey*)o;
		if (lastrow < k->kr)
			lastrow = k->kr;
		o = o->GetNext();
	}
	int _w = (int)hscrollbar.maximum();
	lastrow = (lastrow+1)*BUTTON_HEIGHT + 20;
	// printf("lastrow=%d lastcol=%d\n", lastrow, lastlastcol);
	size(w(), lastrow);

	// Und now, we fixup the "bitmap"s
	if (!bitsFixed)
	{
		int	i;
		int	j;
		uchar	rc, gc, bc;
		Fl::get_color(xpblue, rc, gc, bc);
		for (int n=0; n<numKeyCommands; n++)
		{
			uchar* b = keyCommands[n].bits;
			if (!b)
				continue;
			ushort* e = (ushort*)b;
			keyCommands[n].bits = (uchar*)malloc(sizeof(uchar)*16*16*3);
			uchar* f = keyCommands[n].bits;
	//		for (i=0; i<sizeof(uchar)*16*16*3; i++)
	//			*f++ = '\0';
			f = keyCommands[n].bits;
			for (i=0; i<16; i++)
			{
				ushort	w = *e++;
				ushort	z = 0;
				for (j=0; j<16; j++)					// swap the bits in the bytes
				{
					if (w & (1<<j))
					{
						*f++ = 0xff;
						*f++ = 0xff;
						*f++ = 0xff;
					}
					else
					{
						*f++ = rc;		// xpblue broken down
						*f++ = gc;
						*f++ = bc;
					}
				}
			}
		}
		bitsFixed = true;
	}
}

///////////////////////////////////////////////////////////////////////////////
int	Fl_KbConfigKb::handle(int event)
{
	// delay handling mouse move until we see if its ours
	//printf("handle %d\n", event);
	int ret;
	ret = Fl_KbConfigKbSUPERCLASS::handle(event);
	if (event != FL_MOVE && ret)
		return(ret);
	DEBUGEVENTS(printf("Fl_KbConfigKb::handle(%s)\n", eventName[event]);)
	switch (event)
	{
	case FL_FOCUS:
		DEBUGEVENTS(printf("focus\n");)
		return(1);
	case FL_UNFOCUS:
		DEBUGEVENTS(printf("unfocus\n");)
		if (lastMouseKey)
		{
			lastMouseKey->UnSelect();
			lastMouseKey = 0;
		}
		return(1);		// yeah, sure we're interested
	case FL_MOVE:
	{
		SelectKeyUnderMouse();
		return(Fl_KbConfigKbSUPERCLASS::handle(event));
	}
	case FL_KEYBOARD:
	{
		int	key = Fl::event_key();
		int _w = (int)hscrollbar.maximum();
		// printf("key=%X max=%d\n", key, _w);
		Fl_KbCButton* b = FindButton(key);
		if (!b)
			return(0);
		b->Select();

		return(1);
	}

	}
	return(0);
}

///////////////////////////////////////////////////////////////////////////////
bool Fl_KbConfigKb::SelectKeyUnderMouse()
{
//	Fl_KbCButton*	mk = (Fl_KbCButton*)Fl::belowmouse();
	Fl_KbCButton*	mk = GetButtonBelowMouse();
//	DEBUGSELECTION(printf("mouse=%d/%d mk=%p\n", Fl::event_x(), Fl::event_y(), mk);)
	if (lastMouseKey && lastMouseKey->multiWindow)
	{
		Fl_KbMultiComm* mw = lastMouseKey->multiWindow;
		bool myselect = false;
		lastMouseKey->multiWindowChild = 0;
		if (mw == (Fl_KbMultiComm*)mk)
		{
			DEBUGSELECTION(printf("mouseover multiwindow\n");)
			myselect = true;
		}
		int mwc = mw->children();
		for (int i=0; i<mwc; i++)
		{
			Fl_KbCButton* kcb;
//			if (i == mwc)
//				kcb = unusedButton;
//			else
				kcb = (Fl_KbCButton*)mw->child(i);
			if (mk == kcb)
			{
				DEBUGSELECTION(printf("mouseover mw child %d\n", i);)
				if (!kcb->IsSelected())
					kcb->Select();
				myselect = true;
				lastMouseKey->multiWindowChild = kcb;
			}
			else if (kcb->IsSelected())
			{
				kcb->UnSelect();
			}
		}
		if (myselect)
			return(true);
	}
	Obj* o = keyChain.GetHead();
	while (o)
	{
		Fl_KbCButton* k = (Fl_KbCButton*)o;
		if (mk == k)
		{
			if (lastMouseKey && lastMouseKey == k)
				return(true);
			if (lastMouseKey && lastMouseKey != k)
			{
				lastMouseKey->UnSelect();
			}
			if (k->ktype == ButtonTypeBlank)
			{
				lastMouseKey = 0;
				return(true);
			}
			lastMouseKey = k;
			lastMouseKey->Select();
			DEBUGSELECTION(printf("mouseover %s\n", k->keyDef->s);)
			return(true);
		}
		o = o->GetNext();
	}
	//DEBUGSELECTION(printf("mouseover nothing!\n");)
	if (lastMouseKey)
	{
		lastMouseKey->UnSelect();
		lastMouseKey = 0;
	}
	return(false);
}

///////////////////////////////////////////////////////////////////////////////
Fl_KbCButton* Fl_KbConfigKb::GetButtonBelowMouse()
{
	int	_x = Fl::event_x();
	int _y = Fl::event_y();
	int	i;
	Fl_KbCButton* kcb;
	// check multiwindows first
	DEBUGSELECTION(printf("GBBM: checking %d children\n", children()+1);)
	for (i=0; i<=children()+1; i++)
	{
		if (i == children()+0)
			kcb = unusedButton;
		else if (i == children()+1)
			kcb = copyButton;
		else
			kcb = (Fl_KbCButton*)child(i);
		if (!kcb->user_data())
			continue;			// not our button
		if (kcb->argument() == ButtonTypeMulti || kcb->argument() == ButtonTypeDragger)
			continue;
		if (!kcb->keyDef)
			continue;
		if (kcb->multiWindow)
		{
			if (_x > kcb->multiWindow->x() && _x < kcb->multiWindow->x()+kcb->multiWindow->w()
			 && _y > kcb->multiWindow->y() && _y < kcb->multiWindow->y()+kcb->multiWindow->h())
			{
				// we got a multiwindow, check the children
				DEBUGSELECTION(printf("GBBM: got child %d\n", i);)
				for (i=0; i<kcb->multiWindow->children(); i++)
				{
					Fl_KbCButton* kmb = (Fl_KbCButton*)kcb->multiWindow->child(i);
					if (_x > kmb->x() && _x < kmb->x()+kmb->w()
					 && _y > kmb->y() && _y < kmb->y()+kmb->h())
					{
						DEBUGSELECTION(printf("multiWindow ButtonBelowMouse = %p\n", kmb);)
						return(kmb);
					}
				}
				DEBUGSELECTION(printf("multiWindow BelowMouse = %p\n", kcb);)
				return(kcb);
			}
		}
	}
	for (i=0; i<=children()+1; i++)
	{
		if (i == children()+0)
			kcb = unusedButton;
		else if (i == children()+1)
			kcb = copyButton;
		else
			kcb = (Fl_KbCButton*)child(i);
		if (!kcb->keyDef)
			continue;
		if (_x > kcb->x() && _x < kcb->x()+kcb->w()
		 && _y > kcb->y() && _y < kcb->y()+kcb->h())
		{
			//DEBUGSELECTION(printf("ButtonBelowMouse = %p\n", kcb);)
			return(kcb);
		}
	}
	return(0);
}

///////////////////////////////////////////////////////////////////////////////
Fl_KbCButton* Fl_KbConfigKb::FindButton(uint key)
{
	Obj* o = keyChain.GetHead();
	while (o)
	{
		Fl_KbCButton* k = (Fl_KbCButton*)o;
		if (k->keyDef && k->keyDef->f == key)
		{
			return(k);
		}
		o = o->GetNext();
	}
	return(NULL);
}

///////////////////////////////////////////////////////////////////////////////
Fl_KbCButton* Fl_KbConfigKb::FindButton(XPKeyDef* kd)
{
	if (kd->s == s_unused)
		return(unusedButton);
	if (kd->s == s_copy)
		return(copyButton);
	Obj* o = keyChain.GetHead();
	while (o)
	{
		Fl_KbCButton* k = (Fl_KbCButton*)o;
		if (k->keyDef && k->keyDef->k == kd->k)
		{
			return(k);
		}
		o = o->GetNext();
	}
	return(NULL);
}


///////////////////////////////////////////////////////////////////////////////
void Fl_KbConfigKb::GetCommands(ObjList& olist, const KbCKey* key)
{
	if (key->keyDef && key->keyDef->s == s_unused)
	{
		UpdateUnusedCommands();
		return;
	}
	olist.Empty();
	// printf("GetCommands for \"%s\" (%d)\n", key->keyDef->s, key->keyDef->k);
	for (int i=0; i<iniClient.maxKeyDefs; i++)
	{
		if (iniClient.keyDefs[i].keySym == key->keyDef->k)
		{
			KbCKeyCommand* ckc = new KbCKeyCommand;
			ckc->key = &iniClient.keyDefs[i];
			olist.Add(ckc);
			// printf("Add %d to %s\n", ckc->key->key, key->keyDef->s);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
void Fl_KbConfigKb::UpdateAllCommands()
{
	Obj* o = keyChain.GetHead();
	while (o)
	{
		Fl_KbCButton* k = (Fl_KbCButton*)o;
		if (k->keyDef)
			GetCommands(k->GetCommands(), k);
		o = o->GetNext();
	}
}

///////////////////////////////////////////////////////////////////////////////
void Fl_KbConfigKb::UpdateUnusedCommands()
{
	int		i;
	int		j;

//	return;

	if (!unusedButton)
		return;
	bool kcheck[NUM_CLIENT_KEYS];
	for (i=0; i<NUM_CLIENT_KEYS; i++)
		kcheck[i] = false;

	unusedButton->GetCommands().Empty();

	bool match;
	for (i=0; i<NUM_CLIENT_KEYS; i++)
	{
		match = false;
		for (j=0; j<numNeverUsed; j++)
		{
			if (i == neverUsed[j])
			{
				match = true;
				break;
			}
		}
		if (match)
			continue;
		match = false;
		for (j=0; j<iniClient.maxKeyDefs; j++)
		{
			if (iniClient.keyDefs[j].key == i)
			{
				match = true;
				break;
			}
		}
		if (!match)
		{
			KbCKeyCommand* ckc = new KbCKeyCommand;
			ckc->key = new KeyDef;
			ckc->key->key = (keys_t)i;
			unusedButton->GetCommands().Add(ckc);
			// printf("Add %d to %s\n", ckc->key->key, key->keyDef->s);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
void Fl_KbConfigKb::SetUnusedButton(Fl_KbCButton* b)
{
	unusedButton = b;
	b->SetConfig(this);
	keyChain.Add(b);
	UpdateUnusedCommands();
}
