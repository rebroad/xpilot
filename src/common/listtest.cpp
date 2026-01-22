/* $Id: listtest.cpp,v 1.1.1.1 2001/07/04 07:13:42 dick Exp $
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
 */

/*
 * Test the list.c implementation.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef _WINDOWS
# include <unistd.h>
#endif

#include "list.h"

#define eq(a,b)		(!strcmp((a),(b)))
#define eqn(a,b,n)	(!strncmp((a),(b),(n)))

int main(int argc, char **argv)
{
    list_t	l = List_new();

    char	buf[1024];
    char	*args[10];
    int		i;

    while (printf("-> "),
	   fflush(stdout),
	   fgets(buf, sizeof buf, stdin)) {
	char *a0, *a1, *a2, *a3, *a4;
	for (i = 0; i < 10; i++) {
	    args[i] = strtok(i ? NULL : buf, " \t\r\n");
	}
	args[9] = NULL;
	a0 = args[0];
	a1 = args[1];
	a2 = args[2];
	a3 = args[3];
	a4 = args[4];
	if (eqn(a0, "new", 3)) {
	    l = List_new();
	}
	if (eqn(a0, "del", 3)) {
	    List_delete(l);
	}
	if (eqn(a0, "pr", 2)) {
	    list_iter_t li = List_begin(l);
	    printf("\n");
	    while (li != List_end(l)) {
		void *data = List_iter_data(li);
		printf("\titer pos %d = data %d\n", (int) li, (int) data);
		LI_FORWARD(li);
	    }
	    printf("\n");
	}
	if (eqn(a0, "push", 2)) {
	    if (eqn(a1, "front", 2)) {
		List_push_front(l, (void *) atoi(a2));
	    }
	    if (eqn(a1, "back", 2)) {
		List_push_back(l, (void *) atoi(a2));
	    }
	}
	if (eqn(a0, "pop", 2)) {
	    if (eqn(a1, "front", 2)) {
		List_pop_front(l);
	    }
	    if (eqn(a1, "back", 2)) {
		List_pop_back(l);
	    }
	}
	if (eqn(a0, "find", 2)) {
	    if (eqn(a1, "range", 2)) {
		list_iter_t pos = List_find_range((list_iter_t) atoi(a2),
						  (list_iter_t) atoi(a3),
						  (void *) atoi(a4));
		void *data = List_iter_data(pos);
		printf("\titer pos %d = data %d\n", (int) pos, (int) data);
	    }
	    else {
		list_iter_t pos = List_find(l, (void *) atoi(a1));
		void *data = List_iter_data(pos);
		printf("\titer pos %d = data %d\n", (int) pos, (int) data);
	    }
	}
	if (eqn(a0, "remove", 2)) {
	    List_remove(l, (void *) atoi(a1));
	}
    }

    return 0;
}

