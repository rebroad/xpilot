/* $Id: World.rules.cpp,v 1.16 2004/06/03 06:06:14 dick Exp $
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
 *  $Log: World.rules.cpp,v $
 *  Revision 1.16  2004/06/03 06:06:14  dick
 *  shipobj becomes ShipObj.
 *
 *  Revision 1.15  2004/05/22 15:13:48  dick
 *  LASER_MODIFIERS becomes LASER_MODS
 *
 *  Revision 1.14  2004/02/03 03:39:54  dick
 *  Add Windows precompiled header support.
 *
 *  Revision 1.13  2004/01/16 06:42:10  dick
 *  Whitespace
 *
 *  Revision 1.12  2002/09/01 00:13:45  dick
 *  Remove obsolete windows #include "winServer.h"
 *
 *  Revision 1.11  2002/06/01 06:29:59  dick
 *  cannon.* becomes Cannon.*
 *
 *  Revision 1.10  2002/06/01 06:06:48  dick
 *  Encapsulate (almost) everything.  Get rid of (almost) all refs to theWorld.
 *
 *  Revision 1.9  2002/05/22 05:09:00  dick
 *  Update to XPilot r4_5_4alpha.
 *
 *  Revision 1.8  2002/05/18 20:55:38  dick
 *  Update to XPilot-4.5.1
 *
 *  Revision 1.7  2001/09/03 00:51:50  dick
 *  Tuners cleanup.
 *
 *  Revision 1.6  2001/08/31 20:56:43  dick
 *  Try to fix realtime tuning of asteroids
 *
 *  Revision 1.5  2001/08/14 21:37:14  dick
 *  Array Obj[] becomes objs[].  We now have a class called Obj.
 *
 *  Revision 1.4  2001/08/05 08:05:36  dick
 *  Add Tuners to the server.  ComputeSensorRange(), AddTank(), RemoveTank()
 *  become members of Player.  ComputeGravity*() become members of World.
 *
 *  Revision 1.3  2001/07/24 09:03:59  dick
 *  Remove all global options.  Use World.options instead.
 *
 *  Revision 1.2  2001/07/23 21:08:17  dick
 *  items[] gets initialized from options.
 *
 *  Revision 1.1  2001/07/23 07:38:42  dick
 *  Fold contact, metaserver, netserver, rules, and server to be members of World.
 *
 *  Revision 1.3  2001/07/10 08:10:08  dick
 *  Don't need (deleted) map.h because World.h is included in global.h
 *
 *  Revision 1.2  2001/07/07 12:00:43  dick
 *  Rename classes to C++ "Style".  old World becomes theWorld.
 *
 */
#include "StdAfx.h"

#include <stdlib.h>
#include <stdio.h>

#ifdef _WINDOWS
# include <windows.h>
#endif

#include "version.h"
#include "config.h"
#include "serverconst.h"
#include "global.h"
#include "proto.h"
#include "rules.h"
#include "bit.h"
#include "Cannon.h"

char rules_version[] = VERSION;


#define MAX_FUEL			 10000
#define MAX_WIDEANGLE			99
#define MAX_REARSHOT			99
#define MAX_CLOAK				99
#define MAX_SENSOR				99
#define MAX_TRANSPORTER			99
#define MAX_MINE				99
#define MAX_MISSILE				99
#define MAX_ECM					99
#define MAX_ARMOR				99
#define MAX_EMERGENCY_THRUST	99
#define MAX_AUTOPILOT			99
#define MAX_EMERGENCY_SHIELD	99
#define MAX_DEFLECTOR			99
#define MAX_MIRROR				99
#define MAX_PHASING				99
#define MAX_HYPERJUMP			99
#define MAX_LASER				99
#define MAX_TRACTOR_BEAM		99

long	KILLING_SHOTS = (OBJ_SHOT|OBJ_CANNON_SHOT|OBJ_SMART_SHOT
						 |OBJ_TORPEDO|OBJ_HEAT_SHOT|OBJ_PULSE);
long	DEF_BITS = 0;
long	KILL_BITS = (THRUSTING|PLAYING|KILLED|SELF_DESTRUCT|WARPING|WARPED);

long	DEF_HAVE =	(HAS_SHIELD|HAS_COMPASS|HAS_REFUEL|HAS_REPAIR|HAS_CONNECTOR
					|HAS_SHOT|HAS_LASER);

long	DEF_USED =	(HAS_SHIELD|HAS_COMPASS);

long	USED_KILL =	(HAS_REFUEL|HAS_REPAIR|HAS_CONNECTOR|HAS_SHOT|HAS_LASER|HAS_ARMOR
					|HAS_TRACTOR_BEAM|HAS_CLOAKING_DEVICE|HAS_PHASING_DEVICE
					|HAS_DEFLECTOR|HAS_MIRROR|HAS_EMERGENCY_SHIELD|HAS_EMERGENCY_THRUST);



/*
 * Convert between probability for something to happen a given second on a
 * given block, to chance for such an event to happen on any block this tick.
 */
void World::SetItemChance(int item)
{
    DFLOAT	max = options.itemProbMult->GetDouble()
				* options.maxItemDensity->GetDouble()
				* blockWidth * blockHeight;
    DFLOAT	sum = 0;
    int		i, num = 0;

    if (options.itemProbMult->GetDouble() * items[item].prob > 0)
	{
		items[item].chance = (int)(1.0
			/ (options.itemProbMult->GetDouble() 
			* items[item].prob * blockWidth * blockHeight * GetFPS()));
		items[item].chance = MAX(items[item].chance, 1);
    } else {
		items[item].chance = 0;
    }
    if (max > 0) {
		if (max < 1) {
			items[item].max = 1;
		} else {
			items[item].max = (int)max;
		}
    } else {
		items[item].max = 0;
    }
    if (!BIT(CANNON_USE_ITEM, 1U << item)) {
		items[item].cannonprob = 0;
		return;
    }
    for (i = 0; i < NUM_ITEMS; i++) {
		if (items[i].prob > 0.0
			&& BIT(CANNON_USE_ITEM, 1U << i)) {
			sum += items[i].prob;
			num++;
		}
    }
    if (num) {
		items[item].cannonprob = items[item].prob
									* (num / sum)
									* (options.maxItemDensity->GetDouble() / 0.00012);
    } else {
		items[item].cannonprob = 0;
    }
}

/*
 * An item probability has been changed during game play.
 * Update the World.items structure and test if there are more items
 * in the world than wanted for the new item probabilities.
 * This function is also called when itemProbMult or maxItemDensity changes.
 */
#if 0
void Tune_item_probs(void)
{
    int			i, j, excess;

    for (i = 0; i < NUM_ITEMS; i++) {
	Set_item_chance(i);
	excess = world->items[i].num - world->items[i].max;
	if (excess > 0) {
	    for (j = 0; j < NumObjs; j++) {
		Object *obj = objs[j];
		if (obj->type == OBJ_ITEM) {
		    if (obj->info == i) {
			Delete_shot(j);
			j--;
			if (--excess == 0) {
			    break;
			}
		    }
		}
	    }
	}
    }
}
#endif

void World::TuneAsteroidProb()
{
	DFLOAT	max = options.maxAsteroidDensity->GetDouble()
		* blockWidth * blockHeight;

	if (options.asteroidProb->GetDouble() > 0)
	{
		asteroids.chance = (int)(1.0
			/ (options.asteroidProb->GetDouble() 
			* blockWidth * blockHeight * GetFPS()));
		asteroids.chance = MAX(asteroids.chance, 1);
	}
	else
	{
		asteroids.chance = 0;
	}
	if (max > 0)
	{
		if (max < 1)
		{
			asteroids.max = 1;
		} else
		{
			asteroids.max = (int)max;
		}
	} else
	{
		asteroids.max = 0;
	}
	/* superfluous asteroids are handled by Asteroid_update() */

	/* Tune asteroid concentrator parameters */
    options.asteroidConcentratorRadius->Limit(1, diagonal);
    options.asteroidConcentratorProb->Limit(0.0, 1.0);
}

/*
 * Postprocess a change command for the number of items per pack.
 */
void World::TuneItemPacks()
{
    items[ITEM_MINE].max_per_pack = options.maxMinesPerPack->GetInt();
    items[ITEM_MISSILE].max_per_pack = options.maxMissilesPerPack->GetInt();
}


/*
 * Initializes special items.
 * First parameter is type,
 * second and third parameters are minimum and maximum number
 * of elements one item gives when picked up by a ship.
 */
void World::InitItem(int item, int initial, int limit, double prob, int minpp, int maxpp)
{
    items[item].num = 0;
	items[item].initial = initial;
	items[item].limit = limit;
	items[item].prob = prob;
    items[item].min_per_pack = minpp;
    items[item].max_per_pack = maxpp;

    SetItemChance(item);
}


/*
 * Give (or remove) capabilities of the ships depending upon
 * the availability of initial items.
 * Limit the initial resources between minimum and maximum possible values.
 */
void World::SetInitialItems(void)
{
	int			i;

	LIMIT(items[ITEM_FUEL].limit, 0, MAX_FUEL);
	LIMIT(items[ITEM_WIDEANGLE].limit, 0, MAX_WIDEANGLE);
	LIMIT(items[ITEM_REARSHOT].limit, 0, MAX_REARSHOT);
	LIMIT(items[ITEM_AFTERBURNER].limit, 0, MAX_AFTERBURNER);
	LIMIT(items[ITEM_CLOAK].limit, 0, MAX_CLOAK);
	LIMIT(items[ITEM_SENSOR].limit, 0, MAX_SENSOR);
	LIMIT(items[ITEM_TRANSPORTER].limit, 0, MAX_TRANSPORTER);
	LIMIT(items[ITEM_TANK].limit, 0, MAX_TANKS);
	LIMIT(items[ITEM_MINE].limit, 0, MAX_MINE);
	LIMIT(items[ITEM_MISSILE].limit, 0, MAX_MISSILE);
	LIMIT(items[ITEM_ECM].limit, 0, MAX_ECM);
	LIMIT(items[ITEM_LASER].limit, 0, MAX_LASER);
	LIMIT(items[ITEM_EMERGENCY_THRUST].limit, 0, MAX_EMERGENCY_THRUST);
	LIMIT(items[ITEM_TRACTOR_BEAM].limit, 0, MAX_TRACTOR_BEAM);
	LIMIT(items[ITEM_AUTOPILOT].limit, 0, MAX_AUTOPILOT);
	LIMIT(items[ITEM_EMERGENCY_SHIELD].limit, 0, MAX_EMERGENCY_SHIELD);
	LIMIT(items[ITEM_DEFLECTOR].limit, 0, MAX_DEFLECTOR);
	LIMIT(items[ITEM_PHASING].limit, 0, MAX_PHASING);
	LIMIT(items[ITEM_HYPERJUMP].limit, 0, MAX_HYPERJUMP);
	LIMIT(items[ITEM_MIRROR].limit, 0, MAX_MIRROR);
	LIMIT(items[ITEM_ARMOR].limit, 0, MAX_ARMOR);

	for (i = 0; i < NUM_ITEMS; i++) {
		LIMIT(items[i].initial, 0, items[i].limit);
	}

    CLR_BIT(DEF_HAVE,
			HAS_CLOAKING_DEVICE |
			HAS_EMERGENCY_THRUST |
			HAS_EMERGENCY_SHIELD |
			HAS_PHASING_DEVICE |
			HAS_TRACTOR_BEAM |
			HAS_AUTOPILOT |
			HAS_DEFLECTOR |
			HAS_MIRROR |
			HAS_ARMOR);

	if (items[ITEM_CLOAK].initial > 0)
		SET_BIT(DEF_HAVE, HAS_CLOAKING_DEVICE);
	if (items[ITEM_EMERGENCY_THRUST].initial > 0)
		SET_BIT(DEF_HAVE, HAS_EMERGENCY_THRUST);
	if (items[ITEM_EMERGENCY_SHIELD].initial > 0)
		SET_BIT(DEF_HAVE, HAS_EMERGENCY_SHIELD);
	if (items[ITEM_PHASING].initial > 0)
		SET_BIT(DEF_HAVE, HAS_PHASING_DEVICE);
	if (items[ITEM_TRACTOR_BEAM].initial > 0)
		SET_BIT(DEF_HAVE, HAS_TRACTOR_BEAM);
	if (items[ITEM_AUTOPILOT].initial > 0)
		SET_BIT(DEF_HAVE, HAS_AUTOPILOT);
	if (items[ITEM_DEFLECTOR].initial > 0)
		SET_BIT(DEF_HAVE, HAS_DEFLECTOR);
	if (items[ITEM_MIRROR].initial > 0)
		SET_BIT(DEF_HAVE, HAS_MIRROR);
	if (items[ITEM_ARMOR].initial > 0)
		SET_BIT(DEF_HAVE, HAS_ARMOR);
}


void World::SetMiscItemLimits(void)
{
    options.dropItemOnKillProb->Limit(0.0, 1.0);
    options.detonateItemOnKillProb->Limit(0.0, 1.0);
    options.movingItemProb->Limit(0.0, 1.0);
    options.randomItemProb->Limit(0.0, 1.0);
    options.destroyItemInCollisionProb->Limit(0.0, 1.0);

    options.itemConcentratorRadius->Limit(1, diagonal);
    options.itemConcentratorProb->Limit(0.0, 1.0);

	options.asteroidItemProb->Limit(0.0, 1.0);
	if (options.asteroidMaxItems < 0)
		options.asteroidMaxItems = 0;
}


/*
 * First time initialization of all global item stuff.
 */
void World::SetWorldItems(void)
{
    InitItem(ITEM_FUEL,			options.initialFuel->GetInt(),
								options.maxFuel->GetInt(),
								options.itemEnergyPackProb->GetDouble(),
								0, 0);

    InitItem(ITEM_TANK,			options.initialTanks->GetInt(),
								options.maxTanks->GetInt(),
								options.itemTankProb->GetDouble());

    InitItem(ITEM_ECM,			options.initialECMs->GetInt(),
								options.maxECMs->GetInt(),
								options.itemECMProb->GetDouble());

    InitItem(ITEM_ARMOR,		options.initialArmor->GetInt(),
								options.maxArmor->GetInt(),
								options.itemArmorProb->GetDouble());

    InitItem(ITEM_MINE,			options.initialMines->GetInt(),
								options.maxMines->GetInt(),
								options.itemMineProb->GetDouble(),
								1, options.maxMinesPerPack->GetInt());

    InitItem(ITEM_MISSILE,		options.initialMissiles->GetInt(),
								options.maxMissiles->GetInt(),
								options.itemMissileProb->GetDouble(),
								1, options.maxMissilesPerPack->GetInt());

    InitItem(ITEM_CLOAK,		options.initialCloaks->GetInt(),
								options.maxCloaks->GetInt(),
								options.itemCloakProb->GetDouble());

    InitItem(ITEM_SENSOR,		options.initialSensors->GetInt(),
								options.maxSensors->GetInt(),
								options.itemSensorProb->GetDouble());

    InitItem(ITEM_WIDEANGLE,	options.initialWideangles->GetInt(),
								options.maxWideangles->GetInt(),
								options.itemWideangleProb->GetDouble());

    InitItem(ITEM_REARSHOT,		options.initialRearshots->GetInt(),
								options.maxRearshots->GetInt(),
								options.itemRearshotProb->GetDouble());

    InitItem(ITEM_AFTERBURNER,	options.initialAfterburners->GetInt(),
								options.maxAfterburners->GetInt(),
								options.itemAfterburnerProb->GetDouble());

    InitItem(ITEM_TRANSPORTER,	options.initialTransporters->GetInt(),
								options.maxTransporters->GetInt(),
								options.itemTransporterProb->GetDouble());

    InitItem(ITEM_MIRROR,		options.initialMirrors->GetInt(),
								options.maxMirrors->GetInt(),
								options.itemMirrorProb->GetDouble());

    InitItem(ITEM_DEFLECTOR,	options.initialDeflectors->GetInt(),
								options.maxDeflectors->GetInt(),
								options.itemDeflectorProb->GetDouble());

    InitItem(ITEM_HYPERJUMP,	options.initialHyperJumps->GetInt(),
								options.maxHyperJumps->GetInt(),
								options.itemHyperJumpProb->GetDouble());

    InitItem(ITEM_PHASING,		options.initialPhasings->GetInt(),
								options.maxPhasings->GetInt(),
								options.itemPhasingProb->GetDouble());

    InitItem(ITEM_LASER,		options.initialLasers->GetInt(),
								options.maxLasers->GetInt(),
								options.itemLaserProb->GetDouble());

    InitItem(ITEM_EMERGENCY_THRUST, options.initialEmergencyThrusts->GetInt(),
								options.maxEmergencyThrusts->GetInt(),
								options.itemEmergencyThrustProb->GetDouble());

    InitItem(ITEM_EMERGENCY_SHIELD, options.initialEmergencyShields->GetInt(),
								options.maxEmergencyShields->GetInt(),
								options.itemEmergencyShieldProb->GetDouble());

    InitItem(ITEM_TRACTOR_BEAM, options.initialTractorBeams->GetInt(),
								options.maxTractorBeams->GetInt(),
								options.itemTractorBeamProb->GetDouble());

    InitItem(ITEM_AUTOPILOT,	options.initialAutopilots->GetInt(),
								options.maxAutopilots->GetInt(),
								options.itemAutopilotProb->GetDouble());

    SetMiscItemLimits();

    SetInitialItems();
}


void World::SetWorldRules()
{
    static Rules _rules;

    _rules.mode =
      ((options.allowPlayerCrashes->GetBool() ? CRASH_WITH_PLAYER : 0)
       | (options.allowPlayerBounces->GetBool() ? BOUNCE_WITH_PLAYER : 0)
       | (options.allowPlayerKilling->GetBool() ? PLAYER_KILLINGS : 0)
       | (options.allowShields->GetBool() ? PLAYER_SHIELDING : 0)
       | (options.limitedVisibility->GetBool() ? LIMITED_VISIBILITY : 0)
       | (options.limitedLives->GetBool() ? LIMITED_LIVES : 0)
       | (options.teamPlay->GetBool() ? TEAM_PLAY : 0)
	   | (options.allowAlliances->GetBool() ? ALLIANCES : 0)
       | (options.timing->GetBool() ? TIMING : 0)
       | (options.allowNukes->GetBool() ? ALLOW_NUKES : 0)
       | (options.allowClusters->GetBool() ? ALLOW_CLUSTERS : 0)
       | (options.allowModifiers->GetBool() ? ALLOW_MODIFIERS : 0)
       | (options.allowLaserModifiers ->GetBool()? ALLOW_LASER_MODS : 0)
       | (options.edgeWrap->GetBool() ? WRAP_PLAY : 0));
    _rules.lives = options.worldLives->GetInt();
    rules = &_rules;

	if (BIT(rules->mode, TEAM_PLAY)) {
		CLR_BIT(rules->mode, ALLIANCES);
	}
	if (!BIT(rules->mode, PLAYER_KILLINGS)) {
		CLR_BIT(KILLING_SHOTS,
				OBJ_SHOT|OBJ_CANNON_SHOT|OBJ_SMART_SHOT
				|OBJ_TORPEDO|OBJ_HEAT_SHOT|OBJ_PULSE);
	}
	if (!BIT(rules->mode, PLAYER_SHIELDING)) {
		CLR_BIT(DEF_HAVE, HAS_SHIELD);
	}
    DEF_USED &= DEF_HAVE;
}

void World::SetWorldAsteroids(void)
{
    asteroids.num = 0;
    TuneAsteroidProb();
}
