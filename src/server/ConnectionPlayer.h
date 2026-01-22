/* $Id: ConnectionPlayer.h,v 1.11 2004/06/03 06:06:14 dick Exp $
 *
 * Describe a server's connection to a client.
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2001 by
 *
 *      Bj�rn Stabell        <bjoern@xpilot.org>
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
 */
/*
 *  $Log: ConnectionPlayer.h,v $
 *  Revision 1.11  2004/06/03 06:06:14  dick
 *  shipobj becomes ShipObj.
 *
 *  Revision 1.10  2004/04/26 07:07:14  dick
 *  Define a list of pages that can be displayed on the ScoreTable in the client.
 *  Each page can be controlled by the server (and the server admin tool).
 *
 *  Revision 1.9  2004/02/18 01:23:11  dick
 *  GetAddr and GetDpy move to Connection where the attributes live (not ConnectionPlayer)
 *
 *  Revision 1.8  2004/02/16 14:29:44  dick
 *  server/sched.cpp and friends become serverlib/NetServer
 *  which disconnects the udp listen and receive functionality from the game.
 *
 *  Revision 1.7  2002/08/01 14:45:35  dick
 *  When a cookie comes in from the ScoreServer, send it to the client.
 *
 *  Revision 1.6  2002/06/30 06:43:24  dick
 *  SendMessage is a bad word.  It's a Windows function that is a macro and gets
 *  translated to SendMessageA or SendMessageW.  Although it works, renaming ours
 *  to SendPlayMessage makes browsing the source code easier.
 *
 *  Revision 1.5  2002/06/01 06:06:47  dick
 *  Encapsulate (almost) everything.  Get rid of (almost) all refs to theWorld.
 *
 *  Revision 1.4  2002/05/18 20:55:38  dick
 *  Update to XPilot-4.5.1
 *
 *  Revision 1.3  2001/08/14 21:51:35  dick
 *  New functionality !anonymousViewing.  When you are viewing someone, everyone
 *  gets notified.  This appears in the score table as if you have declared war
 *  on the player.  Older clients actually SendWar().  New clients reuse PKT_EYES
 *  which was previously defined as a server SendEyes() and client HandleEyes(),
 *  neither of which were actually ever used.
 *
 *  Revision 1.2  2001/08/11 09:38:20  dick
 *  new class ConnectionControl, a Connection for a control.
 *
 *  Revision 1.1  2001/08/10 08:11:37  dick
 *  New class ConnectionPlayer subclasses Connection to provide player
 *  functions.  New class Frame subclasses ConnectionPlayer which provides
 *  Frame update functionality.
 *
 */

/*
 * The decision to put member functions in this class as opposed to
 * Connection is completely arbitrary based on whether i thought a
 * command might be useful to a ConnectionControl.
 */

#ifndef	_CONNECTIONPLAYER_
#define	_CONNECTIONPLAYER_

#include "Connection.h"

class Player;

///////////////////////////////////////////////////////////////////////////////

#define	ConnectionPlayerSUPERCLASS	Connection
class ConnectionPlayer : public ConnectionPlayerSUPERCLASS
{
public:
	ConnectionPlayer();
	virtual int		HandleSetup();
	virtual	void	DestroyConnection(PCSTR reason);

	void	GetDisplayParameters(int *width, int *height,
								 int *_debris_colors, int *_spark_rand);
//	int		GetPlayerId();
//	int		GetConnVersion();
	Player*	GetPlayer();


	int		SetCookie(uint cookie);

	virtual	int		ReceivePlay();
	virtual	int		ReceivePower();
	virtual	int		ReceiveDisplay();
	virtual	int		ReceiveModifierBank();
	virtual	int		ReceiveAudioRequest();
	virtual	int		ReceiveFpsRequest();
	virtual	int		ReceiveKeyboard();
	virtual	int		ReceiveAckCannon();
	virtual	int		ReceiveAckFuel();
	virtual	int		ReceiveAckTarget();
	virtual	int		ReceivePointerMove();
	virtual	int		ReceiveMotd();
	virtual	int		ReceiveShape();
	virtual	int		ReceiveTalk();

	int		SendModifiers(char *mods);
	int		SendSelfItems(Player *pl);
	int		SendSelf(Player *pl, int lock_id, int lock_dist,
					  int lock_dir, int autopilotlight, long status, char *mods);
	int		SendMotd();
	int		SendLeave(int id);
	int		SendWar(int robot_id, int killer_id);
	int		SendSeek(int programmer_id, int robot_id, int sought_id);
	int		SendPlayer(Player* pl);
//	int		SendScore(int id, DFLOAT score, int life, int mychar, int alliance);
	int		SendScore(Player* pl);
	int		SendTeamScore(int team, DFLOAT score);
	int		SendScoreTablePages();			// bit mask of available pages.  Once per new connection
	int		SendScoreTableIPVer(Player* pl);
	int		SendScoreTableKills(Player* pl);
	int		SendScoreTableRank(Player* pl);
	int		SendTiming(int id, int check, int round);
	int		SendBase(int id, int num);
	int		SendFuel(int num, int fuel);
	int		SendScoreObject(DFLOAT score, int x, int y, const char *string);
	int		SendCannon(int num, int dead_time);
	int		SendDestruct(int count);
	int		SendThrustTime(int count, int max);
	int		SendShieldTime(int count, int max);
	int		SendPhasingTime(int count, int max);
	int		SendRoundDelay(int count, int max);
	int		SendDebris(int type, unsigned char *p, int n);
	int		SendWreckage(int x, int y, u_byte wrtype, u_byte size, u_byte rot);
	int		SendAsteroid(int x, int y, u_byte type, u_byte size, u_byte rot);
	int		SendFastshot(int type, unsigned char *p, int n);
	int		SendMissile(int x, int y, int len, int dir);
	int		SendBall(int x, int y, int id);
	int		SendMine(int x, int y, int teammine, int id);
	int		SendTarget(int num, int dead_time, int damage);
	int		SendWormhole(int x, int y);
	int		SendItem(int x, int y, int type);
	int		SendPaused(int x, int y, int count);
	int		SendEcm(int x, int y, int size);
	int		SendTrans(int x1, int y1, int x2, int y2);
	int		SendShip(int x, int y, int id, int dir,
					 int shield, int cloak, int emergency_shield,
					 int phased, int deflector);
	int		SendRefuel(int x0, int y0, int x1, int y1);
	int		SendConnector(int x0, int y0, int x1, int y1, int tractor);
	int		SendLaser(int color, int x, int y, int len, int dir);
	int		SendRadar(int x, int y, int size);
	int		SendFastRadar(unsigned char *buf, int n);
	int		SendDamaged(int damaged);
	int		SendAudio(int type, int vol);
	int		SendTimeLeft(long sec);
	int		SendEyes(int watcherId, int watchedId);
	int		SendPlayMessage(const char *msg);
	int		SendLoseItem(int lose_item_index);
	int		SendStartOfFrame();
	int		SendEndOfFrame();

	ShipObj*	ship;				/* ship shape of player */
	World*		world;				/* which world this connection belongs to */

	int 		team;				/* team of player */
	long		lastKeyChange;		/* last keyboard change */
	long		talkSequenceNum;	/* talk acknowledgement */
	long		motdOffset; 		/* offset into motd or -1 */
	long		motdStop;			/* max offset into motd */
	int 		numKeyboardUpdates; /* Keyboards in one packet */
	int 		viewWidth;			/* Viewable area dimensions */
	int			viewHeight;
	int 		debris_colors;		/* Max. debris intensities */
	int 		spark_rand; 		/* Sparkling effect */

private:
	void		HandleTalk(char *str);

};

inline Player* ConnectionPlayer::GetPlayer()
{ return((id != NO_ID) ? world->players[world->getInd[id]] : NULL); };

extern	int CheckPlayerConnection(PCSTR real, PCSTR nick, PCSTR dpy, PCSTR addr);


#endif
