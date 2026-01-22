/* $Id: WildMap.h,v 1.4 2004/05/30 08:11:49 dick Exp $
 *
 * WildMap - Generate Random Maps
 *
 * common - The common library for XPilot
 *
 *      Copyright (C) 2001 by
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
 * $Log: WildMap.h,v $
 * Revision 1.4  2004/05/30 08:11:49  dick
 * Use randomMT() instead of broken rand().
 * Use destructor to keep us from leaking.
 *
 * Revision 1.3  2004/01/22 00:22:54  dick
 * Try randomizing some of the "fixed" options.  Still needs work.
 *
 * Revision 1.2  2004/01/19 01:35:18  dick
 * MakeWildMap() takes an optional seed number, which will be used to seed the map.
 *
 * Revision 1.1  2004/01/17 19:37:16  dick
 * WildMap is now in the common library. Control and server use this,
 * instead of each having it's own copy.
 *
 */

#ifndef	_WILDMAP_H_
#define	_WILDMAP_H_

#include "types.h"

class ServerOptions;

class WildMap {
public:
	static	bool	MakeWildMap(ServerOptions* sos);
	static	bool	MakeWildMap(ServerOptions* sos, uint seed);

private:
	typedef	double (*CurveFunc)(double i);

	WildMap();
	~WildMap();
	bool	Generate();
	void	RandomizeOptions();		// based on seed, set our options
	void	LimitOptions();			// set reasonable limits on our options
	void	Alloc_map();
	void	Flood_map(int i);
	void	Generate_map();
	void	Connect_map();
	void	Count_labels();
	void	Sort_labels(int tbl[256]);
	void	Label_map(int label);
	void	Partition_map();
	void	Smooth_map();
	void	Decorate_map();
	void	LoadIntoServerOptions();
	uint	fuzz();
	double	fuzzD();
	uint	fuzz_bound(uint max, CurveFunc = curveFlat);
	int		fuzz_bound(int min, int max, CurveFunc = curveFlat);
	double	fuzz_bound(double min, double max, CurveFunc = curveFlat);

	// influence the shape of the random number
	static double	curveFlat(double _i);	// i
	static double	curve3(double _i);		// i^3
	static double	curve5(double _i);		// i^5
	static double	curve7(double _i);		// i^7
	static double	curve0(double _i);		// 0

	struct _map2 {
		uint	seed;
		uint	fuzz_word;
		char*	data;
		double	fillRatio;
		double	seedRatio;
		double	cannonRatio;
		double	fuelRatio;
		double	gravRatio;
		double	wormRatio;
		double	teamGameProb;
		int		width;
		int		height;
		int		linewidth;
		int		datasize;
		int		numBases;
		int		numTeams;
		int		flood_marker;
		int		flood_trigger;
		int		flood_count[256];
	};

	struct _map2	map2;

	ServerOptions* sos;
};

#endif	// _WILDMAP_H_
