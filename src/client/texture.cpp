/* $Id: texture.cpp,v 1.7 2004/02/03 03:39:52 dick Exp $
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
 * $Log: texture.cpp,v $
 * Revision 1.7  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.6  2004/01/06 04:48:27  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 * Revision 1.5  2002/09/02 21:06:06  dick
 * Encapsulate dpy and kdpy.  Singleton ini becomes iniClient.
 *
 * Revision 1.4  2002/09/02 15:42:40  dick
 * Singleton ini becomes iniClient.
 *
 * Revision 1.3  2001/09/11 09:08:30  dick
 * Encapsulate all client resources into new class Ini (Linux cleanups)
 *
 * Revision 1.2  2001/07/27 05:47:39  dick
 * Client builds and runs in C++
 *
 */
#include "StdAfx.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#if defined(_UNIX) || defined(_CYGWIN)
# include <unistd.h>
# include <X11/Xlib.h>
#else
# include "NT/winX.h"
# include <io.h>
#endif

#include "version.h"
#include "config.h"
#include "const.h"
#include "paint.h"
#include "xinit.h"
#include "error.h"
#include "xpmread.h"
#include "texture.h"
#include "portability.h"
#include "Ini.h"


#ifndef PATH_MAX
#define PATH_MAX        1023
#endif

char texture_version[] = VERSION;

enum TextureStatus {
    TextureUnloaded = 0,
    TextureLoaded = 1,
    TextureError = 2
};

typedef struct texture_info {
    const char		*name;
    char		**filename_ptr;
    const char		**data;
    Pixmap		pixmap;
    enum TextureStatus	status;
} texture_info_t;

/*
 * XPM format pixmap data.
 */
#define static const
#include "../../lib/textures/rock4.xpm"
#include "../../lib/textures/ball.xpm"
#undef static

static texture_info_t wall_texture_info = {
    "wall",
    &iniClient.wallTextureFile,
    rock4_xpm,
    None,
    TextureUnloaded
};

static texture_info_t decor_texture_info = {
    "decor",
    &iniClient.decorTextureFile,
    rock4_xpm,
    None,
    TextureUnloaded
};

static texture_info_t ball_texture_info = {
    "ball",
    &iniClient.ballTextureFile,
    ball_xpm,
    None,
    TextureUnloaded
};

/*
 * Resolve the filename of the texture
 * and, if found, try to load it into a pixmap.
 */
static Pixmap Texture_load_from_file(char *filename)
{
#ifndef _WINDOWS
    char		*dir, *colon;
    int			len;
    char		path[PATH_MAX + 1];

    if (!filename || !*filename) {
	return None;
    }

    /*
     * Always try the plain filename first,
     * without using the texturePath.
     */
    if (access(filename, 4) == 0) {
	return xpm_pixmap_from_file(filename);
    }

    /*
     * If filename doesn't start with a slash
     * then we also try the texturePath, if it exists.
     */
    if (*filename != '/' && iniClient.texturePath != NULL) {
	for (dir = iniClient.texturePath; *dir; dir = colon) {
	    if (is_this_windows() ||
		!(colon = strchr(dir, ':'))) {
		len = strlen(dir);
		colon = &dir[len];
	    } else {
		len = colon - dir;
		colon++;
	    }
	    memcpy(path, dir, len);
	    sprintf(&path[len], "/%s", filename);
	    if (access(path, 4) == 0) {
		return xpm_pixmap_from_file(path);
	    }
	}
    }

    error("Can't find texture \"%s\"", filename);
#endif
    return None;
}

/*
 * The texture manager.
 */
static Pixmap Texture_load(texture_info_t *ti)
{
    Pixmap		pixmap = None;

#ifndef _WINDOWS
    if (ti->status == TextureLoaded) {
	pixmap = ti->pixmap;
    }
    else if (ti->status != TextureError) {
	if (*ti->filename_ptr && **ti->filename_ptr) {
	    pixmap = Texture_load_from_file(*ti->filename_ptr);
	} else {
	    pixmap = None;
	}
	if (pixmap == None) {
	    pixmap = xpm_pixmap_from_data(ti->data);
	}
	ti->status = (pixmap == None) ? TextureError : TextureLoaded;
    }
#endif
    return pixmap;
}

/*
 * Load a texture for wall drawing.
 */
Pixmap Texture_wall(void)
{
    return Texture_load(&wall_texture_info);
}

/*
 * Load a texture for decor drawing.
 */
Pixmap Texture_decor(void)
{
    return Texture_load(&decor_texture_info);
}

/*
 * Load a texture for ball drawing.
 */
Pixmap Texture_ball(void)
{
    return Texture_load(&ball_texture_info);
}

