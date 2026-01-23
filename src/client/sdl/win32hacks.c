/*
 * XPilotNG/SDL, an SDL/OpenGL XPilot client.
 *
 * Copyright (C) 2003-2004 Juha Lindström <juhal@users.sourceforge.net>
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>

/* needed by some server specific function in socklib.c */
HWND notifyWnd;

/* these are not used by the SDL client */
/* but they are referred by the common windows code */
int ThreadedDraw;
BOOL drawPending;
int RadarDivisor;
struct {
	HANDLE eventNotDrawing;
} dinfo;


/* SDL client does not use MFC crap */
/* Write trace output to a log file since Windows GUI apps have no console */
static FILE *trace_file = NULL;
static int trace_initialized = 0;

static void init_trace_file(void)
{
    if (!trace_initialized) {
        trace_initialized = 1;
        trace_file = fopen("xpilot-debug.log", "w");
        if (trace_file) {
            fprintf(trace_file, "XPilot NG SDL Client Debug Log\n");
            fprintf(trace_file, "==============================\n\n");
            fflush(trace_file);
        }
    }
}

void _Trace(char *fmt, ...)
{
    va_list ap;
    init_trace_file();
    va_start(ap, fmt);
    if (trace_file) {
        vfprintf(trace_file, fmt, ap);
        fflush(trace_file);  /* Flush immediately so we see output even on crash */
    }
    /* Also try OutputDebugString for debugger attachment */
    {
        char buf[1024];
        va_list ap2;
        va_start(ap2, fmt);
        vsnprintf(buf, sizeof(buf), fmt, ap2);
        va_end(ap2);
        OutputDebugStringA(buf);
    }
    va_end(ap);
}

void Progress(char *fmt, ...) {}
void MarkPlayersForRedraw(void) {}
