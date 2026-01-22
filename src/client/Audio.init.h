/* $Id: Audio.init.h,v 1.2 2004/05/02 01:31:37 dick Exp $
 *
 * XPilot, a multiplayer gravity war game.
 *
 *      Copyright (C) 2004 by
 *
 *      The XPilot Authors   <xpilot@xpilot.org>
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
 *
 * See the file COPYRIGHT.TXT for current copyright information.
 *
 */
/*
 * $Log: Audio.init.h,v $
 * Revision 1.2  2004/05/02 01:31:37  dick
 * Linux uses al.h in an AL/ subdirectory.  Windows does not.
 *
 * Revision 1.1  2004/05/01 19:25:26  dick
 * XPilot5 audio uses OpenAL
 *
 */
#ifndef _AUDIO_INIT_H_
#define	_AUDIO_INIT_H_

static PCSTR soundNames[] =
{
    "start",
    "fire_shot",
    "fire_torpedo",
    "fire_heat_shot",
    "fire_smart_shot",
    "player_explosion",
    "player_hit_player",
    "player_hit_cannon",
    "player_hit_mine",
    "player_eat_torpedo_shot",
    "player_eat_heat_shot",
    "player_eat_smart_shot",
    "drop_mine",
    "player_hit_wall",
    "worm_hole",
    "wideangle_shot_pickup",
    "sensor_pack_pickup",
    "back_shot_pickup",
    "smart_shot_pickup",
    "cloaking_device_pickup",
    "energy_pack_pickup",
    "mine_pack_pickup",
    "refuel",
    "thrust",
    "cloak",
    "change_home",
    "ecm_pickup",
    "afterburner_pickup",
    "tank_pickup",
    "drop_moving_mine",
    "mine_explosion",
    "ecm",
    "tank_detach",
    "cannon_fire",
    "player_shot_themself",
    "declare_war",
    "player_hit_cannonfire",
    "object_explosion",
    "player_eat_shot",
    "transporter_pickup",
    "transporter_success",
    "transporter_fail",
    "deflector_pickup",
    "deflector",
    "hyperjump_pickup",
    "hyperjump",
    "phasing_device_pickup",
    "phasing_on",
    "phasing_off",
    "mirror_pickup",
    "armor_pickup",
    "nuke_launch",
    "nuke_explosion",
    "player_ran_over_player",
    "laser_pickup",
    "emergency_thrust_pickup",
    "autopilot_pickup",
    "tractor_beam_pickup",
    "player_bounced",
    "fire_laser",
    "autopilot_on",
    "autopilot_off",
    "emergency_thrust_on",
    "emergency_thrust_off",
    "tractor_beam",
    "pressor_beam",
    "connect_ball",
    "drop_ball",
    "explode_ball",
    "destroy_ball",
    "destroy_target",
    "team_win",
    "team_draw",
    "player_win",
    "player_draw",
    "player_roasted",
    "player_eat_laser",
    "emergency_shield_pickup",
    "emergency_shield_on",
    "emergency_shield_off",
    "cannon_explosion",
    "asteroid_hit",
    "asteroid_break",
};

#endif						// _AUDIO_INIT_H_
