/* $Id: proto.h,v 1.20 2007/01/17 09:08:17 dick Exp $
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2001 by
 *
 *      Bjørn Stabell        <bjoern@xpilot.org>
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
 *
 *  $Log: proto.h,v $
 *  Revision 1.20  2007/01/17 09:08:17  dick
 *  enum Items becomes ITEM
 *
 *  Revision 1.19  2004/02/23 01:59:51  dick
 *  Move Robot's decls from here to Robot.h
 *
 *  Revision 1.18  2004/01/17 19:37:17  dick
 *  WildMap is now in the common library. Control and server use this,
 *  instead of each having it's own copy.
 *
 *  Revision 1.17  2004/01/08 18:38:19  dick
 *  Remove rfrac() declaration
 *
 *  Revision 1.16  2002/09/11 16:42:04  dick
 *  Here endeth the server encapsulation task.
 *  theWorld is now an array of World(s) and get loaded dynamically.
 *
 *  Revision 1.15  2002/07/09 21:42:50  dick
 *  Load the tweaks file after loading the map.
 *
 *  Revision 1.14  2002/06/01 06:06:48  dick
 *  Encapsulate (almost) everything.  Get rid of (almost) all refs to theWorld.
 *
 *  Revision 1.13  2002/05/22 05:09:01  dick
 *  Update to XPilot r4_5_4alpha.
 *
 *  Revision 1.12  2002/05/18 22:34:15  dick
 *  Move showtime to common.
 *
 *  Revision 1.11  2002/05/18 20:55:39  dick
 *  Update to XPilot-4.5.1
 *
 *  Revision 1.10  2001/08/09 07:43:00  dick
 *  SetMessage becomes member function of Player.
 *
 *  Revision 1.9  2001/08/05 08:05:36  dick
 *  Add Tuners to the server.  ComputeSensorRange(), AddTank(), RemoveTank()
 *  become members of Player.  ComputeGravity*() become members of World.
 *
 *  Revision 1.8  2001/08/02 19:51:15  dick
 *  Don't need old Parser anymore
 *
 *  Revision 1.7  2001/07/24 09:04:00  dick
 *  Remove all global options.  Use World.options instead.
 *
 *  Revision 1.6  2001/07/23 07:38:42  dick
 *  Fold contact, metaserver, netserver, rules, and server to be members of World.
 *
 *  Revision 1.5  2001/07/20 05:20:11  dick
 *  Yet another reorg.  Break server/xpilotedit common files into serverlib.
 *
 *  Revision 1.4  2001/07/19 07:01:48  dick
 *  Integrate OptOrigin into our ServerOption.
 *  stringvar becomes varString, intvar becomes varInt, etc.
 *
 *  Revision 1.3  2001/07/08 06:40:14  dick
 *  C++ conversion stuff.
 *
 *  Revision 1.2  2001/07/07 12:00:43  dick
 *  Rename classes to C++ "Style".  old World becomes theWorld.
 *
 */

#ifndef	PROTO_H
#define	PROTO_H

#include "../common/types.h"
#include "../common/item.h"

#ifndef LIST_H_INCLUDED
/* need list_t */
#include "list.h"
#endif


class Object;
class Player;
class PlayerFuel;
class World;

/*
 * Prototypes for cell.c
 */
//void Free_cells(World* w);
//void Alloc_cells(World* w);
//void Cell_init_object(Object *obj);
//void Cell_add_object(Object *obj);
//void Cell_remove_object(Object *obj);
//void Cell_get_objects(int x, int y, int r, int max, Object ***list, int *count);

/*
 * Prototypes for collision.c
 */
int IsOffensiveItem(ITEM i);
int IsDefensiveItem(ITEM i);
int CountOffensiveItems(Player *pl);
int CountDefensiveItems(Player *pl);

/*
 * Prototypes for id.c
 */
int peek_ID(void);
int request_ID(void);
void release_ID(int id);

/*
 * Prototypes for map.c
 */
DFLOAT WrapFindDir(World* w, DFLOAT dx, DFLOAT dy);
DFLOAT WrapLength(World* w, DFLOAT dx, DFLOAT dy);


int mod(int x, int y);
DFLOAT FindDir(DFLOAT x, DFLOAT y);
void Make_table(void);


/*
 * Prototypes for play.c
 */
//void Thrust(int ind);
//void Turn_thrust(int ind,int num_sparks);
void Recoil(Object *ship, Object *shot);
void Record_shove(Player *pl, Player *pusher, long time);
void Delta_mv(Object *ship, Object *obj);
void Delta_mv_elastic(Object *obj1, Object *obj2);
//void Obj_repel(Object *obj1, Object *obj2, int repel_dist);
void Item_damage(Player* pl, DFLOAT prob);
void Tank_handle_detach(Player*);
void Add_fuel(PlayerFuel*, long);
void Update_tanks(PlayerFuel *);
//int Choose_random_item(void);
char *Describe_shot(int type, long status, modifiers mods, int hit);


/*
 * Prototypes for command.c
 */
void Handle_player_command(Player *pl, char *cmd);

/*
 * Prototypes for player.c
 */
//void Player_hit_armor(int ind);
//void Player_used_kill(int ind);
//int Init_player(int ind, shipobj *ship);
//void Detach_ball(int ind, int ball);
//void Kill_player(int ind);
//void Player_death_reset(int ind);
//void Team_game_over(int winning_team, const char *reason);
//void Individual_game_over(int winner);
//void Race_game_over(void);
//int Team_immun(int id1, int id2);

/*
 * Prototypes for robot.c
 */

/*
 * Prototypes for rules.c
 */
//void Tune_item_probs(void);
//void Tune_item_packs(void);
//void Set_initial_resources(void);
//void Set_world_items(void);
//void Set_world_rules(void);
//void Set_world_asteroids(void);
//void Set_misc_item_limits(void);
//void Tune_asteroid_prob(void);

/*
 * Prototypes for server.c
 */
//int End_game(void);
//int Pick_team(int pick_for_type);
//void Server_info(char *str, unsigned max_size);
//void Log_game(const char *heading);
//void Game_Over(void);
int plock_server(int onoff);
//void Main_loop(void);


/*
 * Prototypes for contact.c
 */
//void Contact_cleanup(void);
//int Contact_init(void);
//void Contact(int fd, void *arg);
//void Queue_loop(void);
//int Queue_advance_player(char *name, char *msg);
//int Queue_show_list(char *msg);
//void Set_deny_hosts(void);

/*
 * Prototypes for metaserver.c
 */

/*
 * Prototypes for frame.c
 */
void Frame_update(World* w);
//void Set_message(const char *message);
//void Set_player_message(Player *pl, const char *message);

/*
 * Prototypes for update.c
 */
void Update_radar_target(int);
//void Update_objects(void);
//void Emergency_thrust(int ind, int on);
//void Emergency_shield(int ind, int on);
//void Autopilot(int ind, int on);
//void Cloak(int ind, int on);
//void Deflector(int ind, int on);
//void Phasing(int ind, int on);

/*
 * Prototypes for option.c
 */
void Options_parse(void);
void Options_free(void);
bool Convert_string_to_int(const char *value_str, int *int_ptr);
bool Convert_string_to_float(const char *value_str, DFLOAT *float_ptr);
bool Convert_string_to_bool(const char *value_str, bool *bool_ptr);
void Convert_list_to_string(list_t list, char **string);
void Convert_string_to_list(const char *value, list_t *list_ptr);

/*
 * Prototypes for parser.c
 */
int Parser_list_option(int index, char *buf);
//bool Parser(int argc, char **argv);
int Tune_option(char *name, char *val);
int Get_option_value(const char *name, char *value, unsigned size);

/*
 * Prototypes for fileparser.c
 */
bool	parseDefaultsFile(World* w, PCSTR filename);
bool	parsePasswordFile(World* w, PCSTR filename);
bool	parseMapFile(World* w, PCSTR filename);
bool	parseTweaksFile(World* w, PCSTR filename);
void	expandKeyword(PCSTR keyword);
bool	MakeTweaksFileName(String& outname, PCSTR inname);

/*
 * Prototypes for laser.c
 */
//void Laser_pulse_collision(void);

/*
 * Prototypes for alliance.c
 */
//int Invite_player(int ind, int ally_ind);
//int Cancel_invitation(int ind);
//int Refuse_alliance(int ind, int ally_ind);
//int Refuse_all_alliances(int ind);
//int Accept_alliance(int ind, int ally_ind);
//int Accept_all_alliances(int ind);
int Get_alliance_member_count(World* w, int id);
//void Player_join_alliance(int ind, int ally_ind);
void Dissolve_all_alliances(World* w);
//int Leave_alliance(int ind);
//void Alliance_player_list(int ind);

/*
 * Prototypes for object.c
 */
Object *Object_allocate(World* w);
void Object_free_ind(World* w, int ind);
void Object_free_ptr(Object *obj);
void Alloc_shots(World* w, int number);
void Free_shots(World* w);

/*
 * World.server.cpp (hmmm)
 */
void ServerLogAdminMessage(Player* pl, PCSTR msg);

#endif
