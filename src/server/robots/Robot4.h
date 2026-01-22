/* $Id: Robot4.h,v 1.2 2007/01/17 09:13:01 dick Exp $
 *
 * The XPilot4 series default robot driver
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2007 by
 *
 *      Dick Balaska         <dick@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
 *      Bjørn Stabell        <bjoern@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
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
 *  $Log: Robot4.h,v $
 *  Revision 1.2  2007/01/17 09:13:01  dick
 *  Enum Items becomes ITEM
 *
 *  Revision 1.1  2007/01/13 22:27:28  dick
 *  Now, a Robot* is a member of Player.
 *  Robot contains the interface to drive a robot.
 *  Robot4 is the old robotdef.cpp handler.
 *
 */

#ifndef	_ROBOT4_H_
#define	_ROBOT4_H_

#include "Robot.h"

class Robot4 : public Robot {

  public:
	Robot4();
	virtual ~Robot4();

	virtual	void	RoundTick();
	virtual	void	Create(char* str);
	virtual	void	GoHome();
	virtual	void	Play();
	virtual	void	SetWar(int killer);
	virtual	int		WarOnPlayer();
	virtual	void	Message(PCSTR str);
	virtual	void	Destroy();
	virtual	void	Invite(int invInd);

	static	Robot*		Factory();

  public:
    int		robot_lock;		/* lock mode */
    int		robot_lock_id;		/* target player if in war mode */
    int		robot_mode;		/* ultrashort term mode of robot. */
    int		robot_count;		/* Misc timings, minimizes rand use */
    int		attack;			/* how aggressive (1-99) */
    int		defense;		/* how defensive (1-99) */
    DFLOAT	robot_normal_speed;
    DFLOAT	robot_attack_speed;
    DFLOAT	robot_max_speed;
    int		last_used_ecm;          /* relative to robot_count */
    int		last_dropped_mine;      /* relative to robot_count */
    int		last_fired_missile;     /* relative to robot_count */
    int		last_thrown_ball;	/* relative to robot_count */
    int		longterm_mode;		/* long term robot mode */
    int		lock_last_seen;		/* last time robot saw target */
    position	lock_last_pos;		/* last known position of target */

  private:
	bool	Check_robot_evade(int mine_i, int ship_i);
	bool	Really_empty_space(int x, int y);
	void	Choose_weapon_modifier(int weapon_type);
	bool	Check_robot_target(int item_x, int item_y, int new_mode);
	bool	Detect_hunt(int j);
	bool	Check_robot_hunt();
	bool	Ball_handler();
	void	Robot_default_play_check_objects(int *item_i, int *item_dist,
											 int *item_imp,
											 int *mine_i, int *mine_dist);
	int		Robot_default_play_check_map();
	void	Robot_default_play_check_lasers();
	int		Rank_item_value(ITEM itemtype);

	DFLOAT	visibility_distance;
	DFLOAT	max_enemy_distance;

};

#endif	// _ROBOT4_H_
