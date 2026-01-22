/* $Id: Fl_SoundEventList.cpp,v 1.2 2007/02/17 19:32:35 dick Exp $
 *
 * SoundEventList - A list of the available sound events
 *
 * common/xp-flwidgets - A library of FLTK widgets for XPilot5.
 *
 *      Copyright (C) 2007 by
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
 * $Log: Fl_SoundEventList.cpp,v $
 * Revision 1.2  2007/02/17 19:32:35  dick
 * Wrap DEBUG with D()
 *
 * Revision 1.1  2007/02/17 06:13:10  dick
 * A Sound Editor.  Connects events to sounds
 *
 */

#include "StdAfx.h"
#include "FL/fl_draw.H"

#include "cstring.h"
#include "guiconst.h"
#include "config.h"
#include "IniClient.h"
#include "Fl_XPColors.h"
#include "audioDefs.h"
#include "AudioMan.h"


#include "SoundWindow.h"

#include "Fl_SoundEventList.h"

class NamedSoundEvent {
public:
	int		id;
	PCSTR	name;
};

static NamedSoundEvent namedSoundEvents[MAX_SOUND_EVENTS] = {

{ START_SOUND, "START" },
{ FIRE_SHOT_SOUND, "FIRE_SHOT" },
{ FIRE_TORPEDO_SOUND, "FIRE_TORPEDO" },
{ FIRE_HEAT_SHOT_SOUND, "FIRE_HEAT_SHOT" },
{ FIRE_SMART_SHOT_SOUND, "FIRE_SMART_SHOT" },
{ PLAYER_EXPLOSION_SOUND, "PLAYER_EXPLOSION" },
{ PLAYER_HIT_PLAYER_SOUND, "PLAYER_HIT_PLAYER" },
{ PLAYER_HIT_CANNON_SOUND, "PLAYER_HIT_CANNON" },
{ PLAYER_HIT_MINE_SOUND, "PLAYER_HIT_MINE" },
{ PLAYER_EAT_TORPEDO_SHOT_SOUND, "PLAYER_EAT_TORPEDO_SHOT" },
{ PLAYER_EAT_HEAT_SHOT_SOUND, "PLAYER_EAT_HEAT_SHOT" },
{ PLAYER_EAT_SMART_SHOT_SOUND, "PLAYER_EAT_SMART_SHOT" },
{ DROP_MINE_SOUND, "DROP_MINE" },
{ PLAYER_HIT_WALL_SOUND, "PLAYER_HIT_WALL" },
{ WORM_HOLE_SOUND, "WORM_HOLE" },
{ WIDEANGLE_SHOT_PICKUP_SOUND, "WIDEANGLE_SHOT_PICKUP" },
{ SENSOR_PACK_PICKUP_SOUND, "SENSOR_PACK_PICKUP" },
{ BACK_SHOT_PICKUP_SOUND, "BACK_SHOT_PICKUP" },
{ ROCKET_PACK_PICKUP_SOUND, "ROCKET_PACK_PICKUP" },
{ CLOAKING_DEVICE_PICKUP_SOUND, "CLOAKING_DEVICE_PICKUP" },
{ ENERGY_PACK_PICKUP_SOUND, "ENERGY_PACK_PICKUP" },
{ MINE_PACK_PICKUP_SOUND, "MINE_PACK_PICKUP" },
{ REFUEL_SOUND, "REFUEL" },
{ THRUST_SOUND, "THRUST" },
{ CLOAK_SOUND, "CLOAK" },
{ CHANGE_HOME_SOUND, "CHANGE_HOME" },
{ ECM_PICKUP_SOUND, "ECM_PICKUP" },
{ AFTERBURNER_PICKUP_SOUND, "AFTERBURNER_PICKUP" },
{ TANK_PICKUP_SOUND, "TANK_PICKUP" },
{ DROP_MOVING_MINE_SOUND, "DROP_MOVING_MINE" },
{ MINE_EXPLOSION_SOUND, "MINE_EXPLOSION" },
{ ECM_SOUND, "ECM" },
{ TANK_DETACH_SOUND, "TANK_DETACH" },
{ CANNON_FIRE_SOUND, "CANNON_FIRE" },
{ PLAYER_SHOT_THEMSELF_SOUND, "PLAYER_SHOT_THEMSELF" },
{ DECLARE_WAR_SOUND, "DECLARE_WAR" },
{ PLAYER_HIT_CANNONFIRE_SOUND, "PLAYER_HIT_CANNONFIRE" },
{ OBJECT_EXPLOSION_SOUND, "OBJECT_EXPLOSION" },
{ PLAYER_EAT_SHOT_SOUND, "PLAYER_EAT_SHOT" },
{ TRANSPORTER_PICKUP_SOUND, "TRANSPORTER_PICKUP" },
{ TRANSPORTER_SUCCESS_SOUND, "TRANSPORTER_SUCCESS" },
{ TRANSPORTER_FAIL_SOUND, "TRANSPORTER_FAIL" },
{ DEFLECTOR_PICKUP_SOUND, "DEFLECTOR_PICKUP" },
{ DEFLECTOR_SOUND, "DEFLECTOR" },
{ HYPERJUMP_PICKUP_SOUND, "HYPERJUMP_PICKUP" },
{ HYPERJUMP_SOUND, "HYPERJUMP" },
{ PHASING_DEVICE_PICKUP_SOUND, "PHASING_DEVICE_PICKUP" },
{ PHASING_ON_SOUND, "PHASING_ON" },
{ PHASING_OFF_SOUND, "PHASING_OFF" },
{ MIRROR_PICKUP_SOUND, "MIRROR_PICKUP" },
{ ARMOR_PICKUP_SOUND, "ARMOR_PICKUP" },
{ NUKE_LAUNCH_SOUND, "NUKE_LAUNCH" },
{ NUKE_EXPLOSION_SOUND, "NUKE_EXPLOSION" },
{ PLAYER_RAN_OVER_PLAYER_SOUND, "PLAYER_RAN_OVER_PLAYER" },
{ LASER_PICKUP_SOUND, "LASER_PICKUP" },
{ EMERGENCY_THRUST_PICKUP_SOUND, "EMERGENCY_THRUST_PICKUP" },
{ AUTOPILOT_PICKUP_SOUND, "AUTOPILOT_PICKUP" },
{ TRACTOR_BEAM_PICKUP_SOUND, "TRACTOR_BEAM_PICKUP" },
{ PLAYER_BOUNCED_SOUND, "PLAYER_BOUNCED" },
{ FIRE_LASER_SOUND, "FIRE_LASER" },
{ AUTOPILOT_ON_SOUND, "AUTOPILOT_ON" },
{ AUTOPILOT_OFF_SOUND, "AUTOPILOT_OFF" },
{ EMERGENCY_THRUST_ON_SOUND, "EMERGENCY_THRUST_ON" },
{ EMERGENCY_THRUST_OFF_SOUND, "EMERGENCY_THRUST_OFF" },
{ TRACTOR_BEAM_SOUND, "TRACTOR_BEAM" },
{ PRESSOR_BEAM_SOUND, "PRESSOR_BEAM" },
{ CONNECT_BALL_SOUND, "CONNECT_BALL" },
{ DROP_BALL_SOUND, "DROP_BALL" },
{ EXPLODE_BALL_SOUND, "EXPLODE_BALL" },
{ DESTROY_BALL_SOUND, "DESTROY_BALL" },
{ DESTROY_TARGET_SOUND, "DESTROY_TARGET" },
{ TEAM_WIN_SOUND, "TEAM_WIN" },
{ TEAM_DRAW_SOUND, "TEAM_DRAW" },
{ PLAYER_WIN_SOUND, "PLAYER_WIN" },
{ PLAYER_DRAW_SOUND, "PLAYER_DRAW" },
{ PLAYER_ROASTED_SOUND, "PLAYER_ROASTED" },
{ PLAYER_EAT_LASER_SOUND, "PLAYER_EAT_LASER" },
{ EMERGENCY_SHIELD_PICKUP_SOUND, "EMERGENCY_SHIELD_PICKUP" },
{ EMERGENCY_SHIELD_ON_SOUND, "EMERGENCY_SHIELD_ON" },
{ EMERGENCY_SHIELD_OFF_SOUND, "EMERGENCY_SHIELD_OFF" },
{ CANNON_EXPLOSION_SOUND, "CANNON_EXPLOSION" },
{ ASTEROID_HIT_SOUND, "ASTEROID_HIT" },
{ ASTEROID_BREAK_SOUND, "ASTEROID_BREAK" },
};

///////////////////////////////////////////////////////////////////////////////
//static
void Fl_SoundEventList::myScrollbarCallback(Fl_Widget* w, void* v) {
	Fl_SoundEventList* sel = (Fl_SoundEventList*)v;
	sel->oldScrollbarCallback(w, v);
	sel->myScrollbarCallback();
}

///////////////////////////////////////////////////////////////////////////////
void Fl_SoundEventList::myScrollbarCallback() {
	D(printf("Scroll callback\n");)
	soundWindow->ScrollUpdate();
}

///////////////////////////////////////////////////////////////////////////////
Fl_SoundEventList::Fl_SoundEventList(int x,int y,int w,int h,const char *l)
        : Fl_SoundEventListSUPERCLASS(x,y,w,h,l) {
	rows(MAX_SOUND_EVENTS);
	callback(&Fl_SoundEventList::Callback, this);
	//callback_when(FLVEcb_CLICKED);
	playSoundPrimed = false;

	oldScrollbarCallback = scrollbar.callback();
	scrollbar.callback(myScrollbarCallback, this);
}

///////////////////////////////////////////////////////////////////////////////
Fl_SoundEventList::~Fl_SoundEventList() {
}

///////////////////////////////////////////////////////////////////////////////
// virtual
int Fl_SoundEventList::handle(int event) {
	//D(printf("SEL:handle(%d)\n", event);)
	switch (event) {
	case FL_PUSH:
	case FL_KEYDOWN:
	case FL_DRAG:
		playSoundPrimed = true;
		Fl_SoundEventListSUPERCLASS::handle(event);
		return(1);
	}
	int ret = Fl_SoundEventListSUPERCLASS::handle(event);
	//printf("SEL:endhandle(%d)\n", event);
	return(ret);
}

///////////////////////////////////////////////////////////////////////////////
//virtual
void Fl_SoundEventList::draw_row(int Offset, int &X, int &Y, int &W, int &H, int R ) {

	Fl_SoundEventListSUPERCLASS::draw_row(Offset, X, Y, W, H, R);

	Flv_Style s;
	get_style(s, R);       // Get trickled down row style

	fl_color(FL_BLACK);
	fl_draw(namedSoundEvents[R].name, X+1-Offset, Y+1, W-2, H-2, s.align());
}

///////////////////////////////////////////////////////////////////////////////
// static
void Fl_SoundEventList::Callback(Fl_Widget*, void* v) {
	Fl_SoundEventList* sel = (Fl_SoundEventList*)v;
	sel->Callback();
}

///////////////////////////////////////////////////////////////////////////////
void Fl_SoundEventList::Callback() {
	D(printf("SEL:Callback\n");)
	if (playSoundPrimed) {
		playSoundPrimed = false;
		D(printf("SEL:Callback row=%d when=%d event=%d\n", row(), when(), why_event());)
		soundWindow->DisplaySelectedEvent();
		audioMan.HandleEvent(row(), 100);
	}
}
