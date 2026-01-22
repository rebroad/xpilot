/* $Id: WildMap.cpp,v 1.8 2004/05/30 08:19:46 dick Exp $
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
 * $Log: WildMap.cpp,v $
 * Revision 1.8  2004/05/30 08:19:46  dick
 * randommt.h is all lower case.
 *
 * Revision 1.7  2004/05/30 08:11:49  dick
 * Use randomMT() instead of broken rand().
 * Use destructor to keep us from leaking.
 *
 * Revision 1.6  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.5  2004/01/22 00:22:54  dick
 * Try randomizing some of the "fixed" options.  Still needs work.
 *
 * Revision 1.4  2004/01/20 03:54:49  jlmiller
 * We're now using Wildmap in XPilot, not MapXpress
 *
 * Revision 1.3  2004/01/19 07:37:07  dick
 * Linux fixes
 *
 * Revision 1.2  2004/01/19 01:35:18  dick
 * MakeWildMap() takes an optional seed number, which will be used to seed the map.
 *
 * Revision 1.1  2004/01/17 19:37:16  dick
 * WildMap is now in the common library. Control and server use this,
 * instead of each having it's own copy.
 *
 */
#include "StdAfx.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <memory.h>
#include <string.h>

#if defined(_WINDOWS) && !defined(_CYGWIN)
#include <process.h>
#else
#include <sys/types.h>
#include <unistd.h>
#endif

#include "randommt.h"
#include "map.h"
#include "ServerOptions.h"
#include "WildMap.h"

///////////////////////////////////////////////////////////////////////////////
#define FUZZ_MASK	0xFFFFFFFFU
#define MAX_FUZZ	FUZZ_MASK

#define MAPOFFLEFT(i)	(((i) >= 1) ? (i) - 1 : map2.datasize - 1)
#define MAPOFFRIGHT(i)	(((i) + 1 < map2.datasize) ? (i) + 1 : 0)
#define MAPOFFUP(i)	(((i) >= map2.linewidth) \
                                ? (i) - map2.linewidth \
                                : (i) - map2.linewidth + map2.datasize)
#define MAPOFFDOWN(i)	(((i) + map2.linewidth < map2.datasize) \
                                ? (i) + map2.linewidth \
                                : (i) + map2.linewidth - map2.datasize)
#define MAPLEFT(i)	map2.data[MAPOFFLEFT(i)]
#define MAPRIGHT(i)	map2.data[MAPOFFRIGHT(i)]
#define MAPUP(i)	map2.data[MAPOFFUP(i)]
#define MAPDOWN(i)	map2.data[MAPOFFDOWN(i)]


///////////////////////////////////////////////////////////////////////////////
bool  WildMap::MakeWildMap(ServerOptions* _sos)
{
	bool ret;

	WildMap* wm = new WildMap;
	wm->sos = _sos;
	wm->map2.seed = (unsigned)randomMT() ^ time(NULL);
	wm->RandomizeOptions();
	ret = wm->Generate();
	delete wm;
	return(ret);
}

///////////////////////////////////////////////////////////////////////////////
bool  WildMap::MakeWildMap(ServerOptions* _sos, uint seed)
{
	bool ret;

	WildMap* wm = new WildMap;
	wm->sos = _sos;
	wm->map2.seed = seed;
	wm->RandomizeOptions();
	ret = wm->Generate();
	delete wm;
	return(ret);
}

///////////////////////////////////////////////////////////////////////////////
WildMap::WildMap()
{
	memset(&map2, 0, sizeof(_map2));
	map2.width = 200;
	map2.height = 200;
	map2.seed = (unsigned)getpid() ^ (unsigned)time(NULL) * (unsigned)getpid();
	map2.seedRatio = 0.19;
	map2.fillRatio = 0.18;
	map2.numBases = 16;
	map2.numTeams = 3;
	map2.cannonRatio = 0.0020;
	map2.fuelRatio   = 0.0006;
	map2.gravRatio   = 0.0006;
	map2.wormRatio		= 0.0002;
	map2.teamGameProb	= 0.2;
}

///////////////////////////////////////////////////////////////////////////////
WildMap::~WildMap()
{
	if (map2.data)
		free(map2.data);
}

///////////////////////////////////////////////////////////////////////////////
bool WildMap::Generate()
{
	LimitOptions();
	Alloc_map();
	Generate_map();
	Connect_map();
	Partition_map();
	Smooth_map();
	Decorate_map();
	LoadIntoServerOptions();
	sos->OptionsChanged();
	return true;
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Implement a private pseudo-random generator to be able
 * to regenerate the same map from the same seed on
 * different architectures.
 * It assumes an unsigned int with at least 32 bits (as I do always).
 * The magic constant used below is from Sedgewick's Algorithms.
 * Note that the low order digits are not random at all :(.
 */
uint WildMap::fuzz()
{
	map2.fuzz_word = (map2.fuzz_word * 31415821 + 1) & FUZZ_MASK;

	return map2.fuzz_word;
}

double WildMap::fuzzD()
{
    return ((double) fuzz() * 0.00000000023283064365386962890625);
}
///////////////////////////////////////////////////////////////////////////////
/*
 * Return a pseudo-random unsigned integer
 * in the range: [0-max] inclusive.
 */
uint WildMap::fuzz_bound(uint max, CurveFunc curve)
{
	return (unsigned)(((double)curve(fuzzD()) * (double) max));
}


double WildMap::fuzz_bound(double min, double max, CurveFunc curve)
{
	return(curve(fuzzD())  * (max-min) + min);
}

int WildMap::fuzz_bound(int min, int max, CurveFunc curve)
{
	return(fuzz_bound(max-min, curve) + min);
}

///////////////////////////////////////////////////////////////////////////////
// some curves to influence the random numbers where we don't really want
// an even distribution
// see http://www.buckosoft.com/~dick/pov/curves/ for a picture of the curves
double WildMap::curveFlat(double _i)
{
	return(_i);
}

double WildMap::curve3(double _i)
{
	return(_i*_i*_i);
}

double WildMap::curve5(double _i)
{
	return(_i*_i*_i*_i*_i);
}

double WildMap::curve7(double _i)
{
	return(_i*_i*_i*_i*_i*_i*_i);
}

double WildMap::curve0(double _i)
{
	return(0.0);
}

///////////////////////////////////////////////////////////////////////////////
struct _range {
	double	lo;
	double	hi;
};

#define	wmRangeD(_which, _lo, _hi)	\
	const double _range##_which##Lo = _lo;	\
	const double _range##_which##Hi = _hi;

#define	wmRangeI(_which, _lo, _hi)	\
	const int _range##_which##Lo = _lo;	\
	const int _range##_which##Hi = _hi;

wmRangeI(width,		90,		MAX_MAP_SIZE);
wmRangeI(height,	90,		MAX_MAP_SIZE);
wmRangeD(seedRatio,	0.02,	0.22);
wmRangeD(fillRatio,	0.02,	0.22);
wmRangeI(numBases,	2,		50);
wmRangeI(numTeams,	3,		MAX_TEAMS);
wmRangeD(cannonRatio,0.0,	0.2);
wmRangeD(fuelRatio,	0.0,	0.1);
wmRangeD(gravRatio,	0.0,	0.1);
wmRangeD(wormRatio,	0.0,	0.1);

#define	wmLimit(_which)	LIMIT(map2._which, _range##_which##Lo, _range##_which##Hi)

#define	wmRandom(_which, _curve) \
	map2._which = fuzz_bound(_range##_which##Lo, _range##_which##Hi, _curve)
///////////////////////////////////////////////////////////////////////////////
void WildMap::RandomizeOptions()
{
	map2.fuzz_word = map2.seed;
	//fuzz();	fuzz();					// throw away, so "wild 0" looks better
	wmRandom(width, curve5);
	wmRandom(height, curve5);
	wmRandom(seedRatio, curve3);
	wmRandom(fillRatio, curve3);
	wmRandom(numBases, curve3);
	wmRandom(numTeams, curve3);
	wmRandom(cannonRatio, curve7);
	wmRandom(fuelRatio, curve3);
	//wmRandom(gravRatio, curve7);
	//wmRandom(wormRatio, curve7);
	wmRandom(gravRatio, curve0);		// just pick the min for now
	wmRandom(wormRatio, curve0);
	if (fuzzD() < 0.5)
		map2.numTeams = 0;
}

///////////////////////////////////////////////////////////////////////////////
// Initialize the ranges for each attribute
void WildMap::LimitOptions()
{
	wmLimit(width);
	wmLimit(height);
	wmLimit(seedRatio);
	wmLimit(fillRatio);
	wmLimit(numBases);
	wmLimit(numTeams);
	wmLimit(cannonRatio);
	wmLimit(fuelRatio);
	wmLimit(gravRatio);
	wmLimit(wormRatio);

}

///////////////////////////////////////////////////////////////////////////////
/*
 * Allocate the map data.
 * The (0, 0) coordinate is in the top left corner.
 * An extra column is appended at the far right, which
 * will later be turned into newlines for easy printing.
 */
void WildMap::Alloc_map()
{
	map2.fuzz_word = map2.seed;
	map2.linewidth = map2.width + 1;
	map2.datasize = map2.linewidth * map2.height;
	if (map2.data)
	{
		free(map2.data);
		map2.data = NULL;
	}
	map2.data = (char *) malloc(map2.datasize + 1);
	if (!map2.data)
	{
		printf("no mem\n");
		exit(1);
	}
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Connect all adjacent (up, down, left, right) map objects,
 * which have the same type as map.flood_trigger.
 * The map objects will be marked with value map.flood_marker,
 * to distinguish them from map objects which already have
 * been processed.
 */
void WildMap::Flood_map(int i)
{
#define INTARR_SIZE	1024

	struct int_arr {
		struct int_arr	*next;
		int		n;
		int		arr[INTARR_SIZE];
	};

	struct int_arr	intarr, *putp = &intarr, *getp, *tmpp;
	int			k, j;

	if (map2.data[i] != map2.flood_trigger) {
		return;
	}
	map2.data[i] = map2.flood_marker;
	putp->next = NULL;
	putp->n = 1;
	putp->arr[0] = i;

	for (getp = &intarr; getp != NULL; getp = tmpp) {
		while (getp->n > 0) {
			k = getp->arr[--getp->n];
			if (putp->n + 4 > INTARR_SIZE) {
				if ((putp->next = (struct int_arr *)
								malloc(sizeof(struct int_arr))) == NULL) {
					fprintf(stderr, "No mem\n");
					exit(1);
				}
				putp = putp->next;
				putp->next = NULL;
				putp->n = 0;
			}
			j = MAPOFFUP(k);
			if (map2.data[j] == map2.flood_trigger) {
				map2.data[j] = map2.flood_marker;
				putp->arr[putp->n++] = j;
			}
			j = MAPOFFLEFT(k);
			if (map2.data[j] == map2.flood_trigger) {
				map2.data[j] = map2.flood_marker;
				putp->arr[putp->n++] = j;
			}
			j = MAPOFFDOWN(k);
			if (map2.data[j] == map2.flood_trigger) {
				map2.data[j] = map2.flood_marker;
				putp->arr[putp->n++] = j;
			}
			j = MAPOFFRIGHT(k);
			if (map2.data[j] == map2.flood_trigger) {
				map2.data[j] = map2.flood_marker;
				putp->arr[putp->n++] = j;
			}
		}
		tmpp = getp->next;
		if (getp != &intarr) {
			free(getp);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Initialize the map with noise.
 * Later the noise is either removed or connected,
 * depending upon the outcome of a randomizer.
 */
void WildMap::Generate_map()
{

	int		todo;
	int		i;
	uint	edge;

	map2.flood_trigger = 1;
	map2.flood_marker = 2;

	edge = (unsigned) (MAX_FUZZ * map2.seedRatio);
	for (todo = map2.datasize; todo--; ) {
		map2.data[todo] = (fuzz() < edge);
	}

	edge = (MAX_FUZZ / map2.datasize);
	for (todo = map2.datasize; todo--; ) {
		i = fuzz_bound(map2.datasize - 1);
		if (map2.data[i] == 0) {
			if (MAPUP(i) == 1) MAPUP(i) = 0;
			if (MAPLEFT(i) == 1) MAPLEFT(i) = 0;
			if (MAPDOWN(i) == 1) MAPDOWN(i) = 0;
			if (MAPRIGHT(i) == 1) MAPRIGHT(i) = 0;
		}
		else {
			if (map2.data[i] == 0) {
				map2.data[i] = 1;
				Flood_map(i);
				map2.data[i] = 1;
			}
			else if (map2.data[i] == 1) {
				Flood_map(i);
				if (MAPUP(i) == 0) {
					MAPUP(i) = 1;
					Flood_map(MAPOFFUP(i));
					MAPUP(i) = 1;
				}
				if (MAPLEFT(i) == 0) {
					MAPLEFT(i) = 1;
					Flood_map(MAPOFFLEFT(i));
					MAPLEFT(i) = 1;
				}
				if (MAPDOWN(i) == 0) {
					MAPDOWN(i) = 1;
					Flood_map(MAPOFFDOWN(i));
					MAPDOWN(i) = 1;
				}
				if (MAPRIGHT(i) == 0) {
					MAPRIGHT(i) = 1;
					Flood_map(MAPOFFRIGHT(i));
					MAPRIGHT(i) = 1;
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Connect small constructs to another if they are very close
 * to one another (1 or only 2 blocks apart).
 */
void WildMap::Connect_map()
	{

	char		*p0, *p1, *p2, *p3, *p4, *p5, *p6, *p7, *p8, *p9, *pa;
	char		*maxp;
	int			n;

	maxp = &map2.data[map2.datasize];

	do {
		n = 0;

		p0 = &map2.data[map2.datasize];
		p1 = p0 - 1;
		p2 = p0 - 2;
		p3 = p0 - 3;
		p4 = p0 - map2.linewidth;
		p5 = p4 - 1;
		p6 = p4 - 2;
		p7 = p4 - map2.linewidth;
		p8 = p7 - 1;
		p9 = p7 - 2;
		pa = p7 - map2.linewidth;
		p0 = map2.data;

		while (p0 < maxp) {
			if (*p0) {
				if (!*p1) {
					if (*p2) {
						*p1 = *p0;
						n++;
					}
					else if (*p3) {
						*p2 = *p1 = *p0;
						n++;
					}
				}
				if (!*p4) {
					if (*p7) {
						*p4 = *p0;
						n++;
					}
					else if (*pa) {
						*p7 = *p4 = *p0;
						n++;
					}
				}
				if (*p5) {
					if (!*p1 && !*p4) {
						*p4 = *p1 = *p0;
						n++;
					}
				}
			} else {
				if (!*p5 && *p1 && *p4) {
					*p0 = *p5 = *p1;
					n++;
				}
			}
			if (!*p5) {
				if ((*p0 || (*p1 && *p4)) && *p9) {
					*p5 = *p0;
					n++;
				}
				else if (*p6 && *p8 && *p0) {
					*p5 = *p0;
					n++;
				}
				else if (*p2 && (*p7 || (*p4 && *p8))) {
					*p5 = *p2;
					n++;
				}
				else if (*p7 && *p1 && *p6) {
					*p5 = *p7;
					n++;
				}
				else if (!*p0 && !*p1 && *p2
						&& !*p4 && *p6
						&& *p7 && *p8 && *p9) {
					*p5 = *p9;
					n++;
				}
				else if (*p0 && !*p1 && !*p2
						&& *p4 && !*p6
						&& *p7 && *p8 && *p9) {
					*p5 = *p7;
					n++;
				}
				else if (*p0 && *p1 && *p2
						&& *p4 && !*p6
						&& *p7 && !*p8 && !*p9) {
					*p5 = *p0;
					n++;
				}
				else if (*p0 && *p1 && *p2
						&& !*p4 && *p6
						&& !*p7 && !*p8 && *p9) {
					*p5 = *p0;
					n++;
				}
				else if (*p0 && !*p1 && !*p2
						&& !*p4 && !*p6
						&& !*p7 && *p8 && !*p9) {
					*p5 = *p8;
					n++;
				}
				else if (!*p0 && !*p1 && *p2
						&& !*p4 && !*p6
						&& !*p7 && *p8 && !*p9) {
					*p5 = *p8;
					n++;
				}
				else if (*p0 && !*p1 && !*p2
						&& !*p4 && *p6
						&& !*p7 && !*p8 && !*p9) {
					*p5 = *p6;
					n++;
				}
				else if (!*p0 && !*p1 && !*p2
						&& !*p4 && *p6
						&& *p7 && !*p8 && !*p9) {
					*p5 = *p6;
					n++;
				}
				else if (!*p0 && *p1 && !*p2
						&& !*p4 && !*p6
						&& !*p7 && !*p8 && *p9) {
					*p5 = *p1;
					n++;
				}
				else if (!*p0 && *p1 && !*p2
						&& !*p4 && !*p6
						&& *p7 && !*p8 && !*p9) {
					*p5 = *p1;
					n++;
				}
				else if (!*p0 && !*p1 && !*p2
						&& *p4 && !*p6
						&& !*p7 && !*p8 && *p9) {
					*p5 = *p4;
					n++;
				}
				else if (!*p0 && !*p1 && *p2
						&& *p4 && !*p6
						&& !*p7 && !*p8 && !*p9) {
					*p5 = *p4;
					n++;
				}
			}
			p3 = p2;
			p2 = p1;
			p1 = p0;
			p0++;
			p6 = p5;
			p5 = p4;
			if (++p4 >= maxp) {
				p4 = map2.data;
			}
			p9 = p8;
			p8 = p7;
			if (++p7 >= maxp) {
				p7 = map2.data;
			}
			if (++pa >= maxp) {
				pa = map2.data;
			}
		}
	} while (n > 0);
}

///////////////////////////////////////////////////////////////////////////////
/*
 * For each possible map value count the number of occurences.
 */
void WildMap::Count_labels(void)
{
	int			todo;

	memset(map2.flood_count, 0, sizeof map2.flood_count);

	for (todo = map2.datasize; todo--; ) {
		map2.flood_count[(unsigned char) map2.data[todo]]++;
	}
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Sort the map to have big map constructs with low map values
 * close to zero and small map constructs with high map values.
 */
void WildMap::Sort_labels(int tbl[256])
{
	int			i, j, n;

	Count_labels();

	memset(tbl, 0, sizeof(int *) * 256);

	tbl[0] = 0;
	tbl[1] = 1;
	n = 2;
	for (i = 2; i < 256; i++) {
		for (j = n; j > 2; j--) {
			if (map2.flood_count[i] > map2.flood_count[tbl[j - 1]]) {
				tbl[j] = tbl[j - 1];
			} else {
				break;
			}
		}
		tbl[j] = i;
		n++;
	}
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Give each map construct an unique value depending on how
 * big it is.  It is OK to delete small constructs if we are
 * running out of label values.
 */
void WildMap::Label_map(int label)
{

	int			todo,
				i,
				tbl[256],
				del[256];

	memset(tbl, 0, sizeof tbl);
	tbl[label] = 1;
	for (todo = map2.datasize; todo--; ) {
		map2.data[todo] = tbl[(unsigned char)map2.data[todo]];
	}

	map2.flood_trigger = 1;
	map2.flood_marker = 2;

	for (todo = map2.datasize; todo--; ) {
		if (map2.data[todo] == map2.flood_trigger) {
			if (map2.flood_marker >= 256) {
				Sort_labels(tbl);
				for (i = 0; i < 224; i++) {
					del[tbl[i]] = i;
				}
				for (i = 224; i < 256; i++) {
					del[tbl[i]] = 0;
				}
				for (i = map2.datasize; i--; ) {
					map2.data[i] = del[(unsigned char)map2.data[i]];
				}
				map2.flood_marker = 224;
			}
			Flood_map(todo);
			map2.flood_marker++;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Remove all the inaccessible holes from the map.
 * And remove enough small map constructs to meet our fillRatio.
 */
void WildMap::Partition_map()
{

	int			todo,
				i,
				j,
				marker,
				count,
				excess,
				tbl[256],
				del[256];

	Label_map(0);
	Count_labels();
	marker = 0;
	for (i = 2; i < 256; i++) {
		if (map2.flood_count[i] > map2.flood_count[marker]) {
			marker = i;
		}
	}
	memset(tbl, 0, sizeof tbl);
	tbl[marker] = 1;
	for (todo = map2.datasize; todo--; ) {
		map2.data[todo] = tbl[(unsigned char)map2.data[todo]];
	}

	Label_map(0);
	Sort_labels(tbl);
	for (i = 0; i < 256; i++) {
		del[tbl[i]] = tbl[i];
	}
	count = 0;
	excess = map2.datasize - map2.flood_count[0]
	- (int)(map2.fillRatio * map2.datasize + 0.5);
	for (i = 256; i-- > 2; ) {
		if (excess < count) {
			break;
		}
		count += map2.flood_count[tbl[i]];
	}
	if (++i < 256) {
		for (j = i + 1; j < 256; j++) {
			if (count - map2.flood_count[tbl[j]] > excess) {
				if (2 * map2.flood_count[tbl[j]] >= map2.flood_count[tbl[i]]) {
					continue;
				}
			}
			del[tbl[j]] = 0;
		}
		del[tbl[i]] = 0;
		for (todo = map2.datasize; todo--; ) {
			map2.data[todo] = del[(unsigned char)map2.data[todo]];
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Round all sharp edges and corners.
 */
void WildMap::Smooth_map()
{

	char		*p0, *p1, *p2, *p3, *p4, *p5, *p6, *p7, *p8;
	char		*maxp;
	int			todo, n;

	for (todo = map2.datasize; todo--; ) {
		map2.data[todo] = (map2.data[todo] ? MAP_FILLED : MAP_SPACE);
	}

	maxp = &map2.data[map2.datasize];

	do {
		n = 0;

		p0 = &map2.data[map2.datasize];
		p1 = p0 - 1;
		p2 = p0 - 2;
		p3 = p0 - map2.linewidth;
		p4 = p3 - 1;
		p5 = p4 - 2;
		p6 = p3 - map2.linewidth;
		p7 = p6 - 1;
		p8 = p7 - 2;
		p0 = map2.data;

		while (p0 < maxp) {
			if (*p4 == MAP_SPACE || *p4 == MAP_FILLED) {
				if (*p0 == MAP_FILLED && *p1 == MAP_FILLED
						&& *p3 == MAP_FILLED && *p5 == MAP_SPACE
						&& *p7 == MAP_SPACE && *p8 == MAP_SPACE) {
					*p4 = MAP_REC_RD;
					n++;
				}
				else if (*p1 == MAP_FILLED && *p2 == MAP_FILLED
						&& *p3 == MAP_SPACE && *p5 == MAP_FILLED
						&& *p6 == MAP_SPACE && *p7 == MAP_SPACE) {
					*p4 = MAP_REC_LD;
					n++;
				}
				else if (*p0 == MAP_SPACE && *p1 == MAP_SPACE
						&& *p3 == MAP_SPACE && *p5 == MAP_FILLED
						&& *p7 == MAP_FILLED && *p8 == MAP_FILLED) {
					*p4 = MAP_REC_LU;
					n++;
				}
				else if (*p1 == MAP_SPACE && *p2 == MAP_SPACE
						&& *p3 == MAP_FILLED && *p5 == MAP_SPACE
						&& *p6 == MAP_FILLED && *p7 == MAP_FILLED) {
					*p4 = MAP_REC_RU;
					n++;
				}
				else if (*p4 == MAP_FILLED) {
					if (*p5 == MAP_SPACE && *p8 == MAP_SPACE
							&& *p7 == MAP_SPACE
							&& *p6 == MAP_SPACE && *p3 == MAP_SPACE) {
						if (*p2 != MAP_FILLED && *p0 != MAP_FILLED) {
							*p4 = MAP_SPACE;
							n++;
						}
					}
					else if (*p7 == MAP_SPACE && *p6 == MAP_SPACE
							&& *p3 == MAP_SPACE
							&& *p0 == MAP_SPACE && *p1 == MAP_SPACE) {
						if (*p8 != MAP_FILLED && *p2 != MAP_FILLED) {
							*p4 = MAP_SPACE;
							n++;
						}
					}
					else if (*p3 == MAP_SPACE && *p0 == MAP_SPACE
							&& *p1 == MAP_SPACE
							&& *p2 == MAP_SPACE && *p5 == MAP_SPACE) {
						if (*p6 != MAP_FILLED && *p8 != MAP_FILLED) {
							*p4 = MAP_SPACE;
							n++;
						}
					}
					else if (*p1 == MAP_SPACE && *p2 == MAP_SPACE
							&& *p5 == MAP_SPACE
							&& *p8 == MAP_SPACE && *p7 == MAP_SPACE) {
						if (*p0 != MAP_FILLED && *p6 != MAP_FILLED) {
							*p4 = MAP_SPACE;
							n++;
						}
					}
				}
			}

			p2 = p1;
			p1 = p0;
			p0++;
			p5 = p4;
			p4 = p3;
			if (++p3 >= maxp) {
				p3 = map2.data;
			}
			p8 = p7;
			p7 = p6;
			if (++p6 >= maxp) {
				p6 = map2.data;
			}
		}

	} while (n > 0);
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Add the cannons, fuelstations, homebases,
 * wormholes and gravity objects.
 * A homebase should be free from influence from gravity objects.
 */
void WildMap::Decorate_map()
{

	struct xy {
		int		x;
		int		y;
	};

	int			margin,
	h,
	i,
	hori,
	vert,
	off,
	x,
	y,
	base,
	cannon,
	num_cannons,
	grav,
	num_gravs,
	fuel,
	num_fuels,
	worm,
	num_worms,
	type,
	wall_offset,
	team,
	tries;
	struct xy		*home;
	unsigned		size;

	size = map2.numBases * sizeof(struct xy);
	if ((home = (struct xy *) malloc(size)) == NULL) {
		fprintf(stderr, "No mem\n");
		exit(1);
	}
	margin = 2;
	if (map2.numTeams > map2.numBases) {
		map2.numTeams = map2.numBases;
	}
	team = map2.numTeams;
	for (i = 0; i < map2.numBases; i++)
	{
		for (tries = map2.datasize; tries; tries--)
		{
			x = margin + fuzz_bound(map2.width - 2*margin - 1);
			y = margin + fuzz_bound(map2.height - 2*margin - 1);
			base = x + y * map2.linewidth;
			if (map2.data[base] != MAP_SPACE)
				continue;
			if (map2.data[base + map2.linewidth] != MAP_FILLED)
				continue;
			if (map2.data[base - map2.linewidth] != MAP_SPACE)
				continue;
			if (map2.data[base - 2 * map2.linewidth] != MAP_SPACE)
				continue;
			if (map2.data[base - 1] != MAP_SPACE)
				continue;
			if (map2.data[base + 1] != MAP_SPACE)
				continue;
			if (--team < 0)
				team = map2.numTeams - 1;
			map2.data[base] = '0' + team;
			home[i].x = x;
			home[i].y = y;
			break;
		}
		if (tries == 0)
			break;
	}

	num_cannons = (int) (map2.cannonRatio * map2.datasize);
	margin = 1;
	for (i = 0; i < num_cannons; i++) {
		switch (fuzz_bound(3)) {
		case 0:
			type = MAP_CAN_LEFT;
			wall_offset = 1;
			break;
		case 1:
			type = MAP_CAN_UP;
			wall_offset = map2.linewidth;
			break;
		case 2:
			type = MAP_CAN_RIGHT;
			wall_offset = -1;
			break;
		default:
			type = MAP_CAN_DOWN;
			wall_offset = -map2.linewidth;
			break;
		}
		for (tries = map2.datasize; tries; tries--)
		{
			x = margin + fuzz_bound(map2.width - 2*margin - 1);
			y = margin + fuzz_bound(map2.height - 2*margin - 1);
			cannon = x + y * map2.linewidth;
			if (map2.data[cannon] != MAP_SPACE)
				continue;
			if (map2.data[cannon + wall_offset] == MAP_SPACE)
				continue;
			if (map2.data[cannon + wall_offset] != MAP_FILLED)
			{
				switch (type) {
				case MAP_CAN_LEFT:
					if (map2.data[cannon + wall_offset] != MAP_REC_LU &&
						map2.data[cannon + wall_offset] != MAP_REC_LD)
						continue;
					break;
				case MAP_CAN_UP:
					if (map2.data[cannon + wall_offset] != MAP_REC_LU &&
						map2.data[cannon + wall_offset] != MAP_REC_RU)
						continue;
					break;
				case MAP_CAN_RIGHT:
					if (map2.data[cannon + wall_offset] != MAP_REC_RU &&
					map2.data[cannon + wall_offset] != MAP_REC_RD)
						continue;
					break;
				default:
					if (map2.data[cannon + wall_offset] != MAP_REC_LD &&
						map2.data[cannon + wall_offset] != MAP_REC_LD)
						continue;
					break;
				}
			}
			for (h = 0; h < map2.numBases; h++) {
				if (((x < home[h].x)
						? (x + margin < home[h].x)
						: (x - margin > home[h].x))
					&& ((y < home[h].y)
						? (y + margin < home[h].y)
						: (y - margin > home[h].y)))
				{
					continue;
				}
				break;
			}
			if (h < map2.numBases)
				continue;
			map2.data[cannon] = type;
			break;
		}
		if (tries == 0) {
			break;
		}
	}

	num_fuels = (int) (map2.fuelRatio * map2.datasize);
	margin = 1;
	for (i = 0; i < num_fuels; i++) {
		for (tries = map2.datasize; tries; tries--) {
			x = margin + fuzz_bound(map2.width - 2*margin - 1);
			y = margin + fuzz_bound(map2.height - 2*margin - 1);
			fuel = x + y * map2.linewidth;
			if (map2.data[fuel] != MAP_FILLED) {
				continue;
			}
			if (map2.data[fuel + 1] != MAP_SPACE &&
				map2.data[fuel - 1] != MAP_SPACE &&
				map2.data[fuel + map2.linewidth] != MAP_SPACE &&
				map2.data[fuel - map2.linewidth] != MAP_SPACE)
			{
				continue;
			}
			map2.data[fuel] = MAP_FUEL;
			break;
		}
		if (tries == 0) {
			break;
		}
	}

	margin = 11;
	num_gravs = (int) (map2.gravRatio * map2.datasize);
	for (i = 0; i < num_gravs; i++) {
		switch (fuzz_bound(3)) {
		case 0:
			type = MAP_GRAV_POS;
			break;
		case 1:
			type = MAP_GRAV_NEG;
			break;
		case 2:
			type = MAP_GRAV_CWISE;
			break;
		default:
			type = MAP_GRAV_ACWISE	;
			break;
		}
		for (tries = map2.datasize; tries; tries--)
		{
			x = margin + fuzz_bound(map2.width - 2*margin - 1);
			y = margin + fuzz_bound(map2.height - 2*margin - 1);
			grav = x + y * map2.linewidth;
			if (map2.data[grav] != MAP_SPACE ||
				map2.data[grav + 1] != MAP_SPACE ||
				map2.data[grav - 1] != MAP_SPACE ||
				map2.data[grav + map2.linewidth] != MAP_SPACE ||
				map2.data[grav - map2.linewidth] != MAP_SPACE ||
				map2.data[grav + map2.linewidth + 1] != MAP_SPACE ||
				map2.data[grav - map2.linewidth + 1] != MAP_SPACE ||
				map2.data[grav + map2.linewidth - 1] != MAP_SPACE ||
				map2.data[grav - map2.linewidth - 1] != MAP_SPACE)
			{
				continue;
			}

			for (h = 0; h < map2.numBases; h++) {
				if ((x < home[h].x)
						? (x + margin < home[h].x)
						: (x - margin > home[h].x))
				{
					continue;
				}
				if ((y < home[h].y)
						? (y + margin < home[h].y)
						: (y - margin > home[h].y))
				{
					continue;
				}
				break;
			}
			if (h < map2.numBases) {
				continue;
			}
			map2.data[grav] = type;
			break;
		}
		if (tries == 0) {
			break;
		}
	}

	margin = 3;
	num_worms = (int) (map2.wormRatio * map2.datasize);
	for (i = 0; i < num_worms; i++) {
		switch (fuzz_bound(3)) {
		case 0:
			type = MAP_WORM_IN;
			break;
		case 1:
			type = MAP_WORM_OUT;
			break;
		default:
			type = MAP_WORM_NORMAL;
			break;
		}
		for (tries = map2.datasize; tries; tries--) {
			x = margin + fuzz_bound(map2.width - 2*margin - 1);
			y = margin + fuzz_bound(map2.height - 2*margin - 1);
			worm = x + y * map2.linewidth;
			if (map2.data[worm] != MAP_SPACE) {
				continue;
			}

			for (vert = -margin; vert <= margin; vert++) {
				off = x - margin + (y + vert) * map2.linewidth;
				for (hori = -margin; hori <= margin; hori++, off++) {
					switch (map2.data[off]) {
					case MAP_SPACE:
					case MAP_GRAV_POS:
					case MAP_GRAV_NEG:
					case MAP_GRAV_CWISE:
					case MAP_GRAV_ACWISE	:
					case MAP_WORM_NORMAL:
					case MAP_WORM_IN:
					case MAP_WORM_OUT:
						continue;
					default:
						break;
					}
					break;
				}
				if (hori <= margin)
					break;
			}
			if (vert <= margin)
				continue;

			for (h = 0; h < map2.numBases; h++) {
				if ((x < home[h].x)
						? (x + margin < home[h].x)
						: (x - margin > home[h].x))
				{
					continue;
				}
				if ((y < home[h].y)
						? (y + margin < home[h].y)
						: (y - margin > home[h].y))
				{
					continue;
				}
				break;
			}
			if (h < map2.numBases)
				continue;
			map2.data[worm] = type;
			break;
		}
		if (tries == 0)
			break;
	}

	free(home);
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Output the map in XPilot 2.0 map format.
 */
void WildMap::LoadIntoServerOptions()
{
	int		i, x, y, ch;
	char*	left;
	char*	middle;
	char*	right;
	String	header;
	String	t;

	left = &map2.data[map2.width - 1];
	middle = left + 1;
	right = &map2.data[0];
	for (i = 0; i < map2.height; i++) {
        *middle = '\n';
        if (*right == MAP_FILLED) {
			if (*left == MAP_REC_LD || *left == MAP_REC_LU) {
                *left = MAP_FILLED;
			}
        }
        if (*right == MAP_SPACE
			|| *right == MAP_REC_RD
			|| *right == MAP_REC_RU) {
			if (*left == MAP_REC_RD || *left == MAP_REC_RU) {
                *left = MAP_SPACE;
			}
        }
        if (*left == MAP_FILLED) {
			if (*right == MAP_REC_RD || *right == MAP_REC_RU) {
                *right = MAP_FILLED;
			}
        }
        if (*left == MAP_SPACE
			|| *left == MAP_REC_LU
			|| *left == MAP_REC_LD) {
			if (*right == MAP_REC_LD || *right == MAP_REC_LU) {
                *right = MAP_SPACE;
			}
        }
        left += map2.linewidth;
        middle += map2.linewidth;
        right += map2.linewidth;
	}
	map2.data[map2.datasize] = '\0';

	header = "Generated by the XPilot 'Wildmap' map generator\n\
		Generation Values used:\n";
	t.printf("Width:          %d\n", map2.width);			header += t;
	t.printf("Height:         %d\n", map2.height);			header += t;
	t.printf("Seed:           %d\n", map2.seed);			header += t;
	t.printf("Seed Ratio:     %f\n", map2.seedRatio);		header += t;
	t.printf("Fill Ratio:     %f\n", map2.fillRatio);		header += t;
	t.printf("Bases:          %d\n", map2.numBases);		header += t;
	t.printf("Teams:          %d\n", map2.numTeams);		header += t;
	t.printf("Cannon Ratio:   %f\n", map2.cannonRatio);		header += t;
	t.printf("Fuel Ratio:     %f\n", map2.fuelRatio);		header += t;
	t.printf("Gravity Ratio:  %f\n", map2.gravRatio);		header += t;
	t.printf("Wormhole Ratio: %f\n", map2.wormRatio);		header += t;


	sos->comments = header;

	*sos->mapWidth = map2.width;
	*sos->mapHeight = map2.height;
	String s;
	s.printf("Wild Map %u", map2.seed);
	*sos->mapName = s;
	*sos->mapAuthor = "The Wild Map Generator";
    *sos->edgeWrap = true;
	*sos->teamPlay = true;
	sos->mapData->optOrigin = OPT_WILD;


    /*This block of code translates the map2.data...which is a single
    character string...into a widthxheight array..dropping the newlines.
    Do it this way because we don't really want to do a lot of editing
    to the main code of Wildmap,*/

    ch = x = y = 0;


    for (i = 0, ch = 0; i < map2.datasize; i++, ch++)
    {
		if (ch == map2.linewidth-1) /*If were at a newline*/
		{
			i++; /*move past the newline*/
			y++; /*move to next row of array*/
			x=0; /*move to first column of array*/
			ch = 0; /*reset our line width counter*/
		}

		//ChangeMapData(x, y, map2.data[i]);
		sos->mapData->blocks[x][y].cdata = map2.data[i];
		x++;
    }

#ifdef	_DEBUG
	printf(header);
#endif
}
