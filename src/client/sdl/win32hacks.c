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
#include <string.h>

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
static char trace_path[MAX_PATH] = "";

static void init_trace_file(void)
{
    char exe_path[MAX_PATH];
    char *last_slash;
    const char *try_paths[3];
    int i, n = 0;

    if (!trace_initialized) {
        trace_initialized = 1;
        /* Prefer: 1) next to exe, 2) %TEMP%, 3) CWD */
        if (GetModuleFileNameA(NULL, exe_path, sizeof(exe_path)) > 0) {
            last_slash = strrchr(exe_path, '\\');
            if (last_slash) {
                *(last_slash + 1) = '\0';
                snprintf(trace_path, sizeof(trace_path), "%sxpilot-debug.log", exe_path);
                try_paths[n++] = trace_path;
            }
        }
        if (n == 0)
            try_paths[n++] = "xpilot-debug.log";

        for (i = 0; i < n && !trace_file; i++) {
            trace_file = fopen(try_paths[i], "w");
            if (trace_file)
                snprintf(trace_path, sizeof(trace_path), "%s", try_paths[i]);
        }
        /* Fallback: %TEMP% if exe dir not writable (e.g. Program Files) */
        if (!trace_file) {
            if (GetTempPathA(sizeof(exe_path), exe_path) > 0) {
                snprintf(trace_path, sizeof(trace_path), "%sxpilot-debug.log", exe_path);
                trace_file = fopen(trace_path, "w");
            }
        }
        if (trace_file) {
            fprintf(trace_file, "XPilot NG SDL Client Debug Log\n");
            fprintf(trace_file, "Log file: %s\n", trace_path);
            fprintf(trace_file, "==============================\n\n");
            fflush(trace_file);
        } else {
            OutputDebugStringA("XPilot: could not create xpilot-debug.log (tried exe dir and %TEMP%)\n");
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
