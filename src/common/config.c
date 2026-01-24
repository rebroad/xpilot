/* 
 * XPilot NG, a multiplayer space war game.
 *
 * Copyright (C) 1991-2001 by
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "xpcommon.h"

#ifndef _WINDOWS
#include <limits.h>
#include <unistd.h>
#endif

static int Conf_dir_exists(const char *path)
{
    return (path != NULL && *path != '\0' && access(path, R_OK | X_OK) == 0);
}

static int Conf_file_exists(const char *path)
{
    return (path != NULL && *path != '\0' && access(path, R_OK) == 0);
}

static void Conf_normalize_dir(char *dst, size_t dstsz, const char *in)
{
    size_t n;

    if (dstsz == 0)
	return;
    dst[0] = '\0';
    if (in == NULL || *in == '\0')
	return;

    strlcpy(dst, in, dstsz);
    n = strlen(dst);
    if (n > 0 && dst[n - 1] != '/')
	strlcat(dst, "/", dstsz);
}

static int Conf_base_has_textures(const char *base)
{
    char p[PATH_MAX];
    int n;

    if (base == NULL)
	return 0;

    /* Require "base" textures to exist, not just a directory. */
    n = snprintf(p, sizeof p, "%stextures/", base);
    if (n < 0 || (size_t)n >= sizeof p)
	return 0;
    if (!Conf_dir_exists(p))
	return 0;

    n = snprintf(p, sizeof p, "%stextures/clouds.ppm", base);
    if (n < 0 || (size_t)n >= sizeof p || !Conf_file_exists(p))
	return 0;
    n = snprintf(p, sizeof p, "%stextures/base_down.ppm", base);
    if (n < 0 || (size_t)n >= sizeof p || !Conf_file_exists(p))
	return 0;
    n = snprintf(p, sizeof p, "%stextures/allitems.ppm", base);
    if (n < 0 || (size_t)n >= sizeof p || !Conf_file_exists(p))
	return 0;
    n = snprintf(p, sizeof p, "%stextures/bullet.ppm", base);
    if (n < 0 || (size_t)n >= sizeof p || !Conf_file_exists(p))
	return 0;

    return 1;
}

static void Conf_try_set_datadir(char *out, size_t outsz, const char *candidate)
{
    char tmp[PATH_MAX];

    Conf_normalize_dir(tmp, sizeof tmp, candidate);
    if (tmp[0] == '\0')
	return;
    if (Conf_base_has_textures(tmp))
	strlcpy(out, tmp, outsz);
}

static void Conf_try_set_datadir_suffix(char *out, size_t outsz,
					const char *base, const char *suffix)
{
    char tmp[PATH_MAX];
    int n;

    if (base == NULL || suffix == NULL)
	return;
    n = snprintf(tmp, sizeof tmp, "%s/%s", base, suffix);
    if (n < 0 || (size_t)n >= sizeof tmp)
	return;
    Conf_try_set_datadir(out, outsz, tmp);
}

#ifndef _WINDOWS
static void Conf_try_set_datadir_from_exe(char *out, size_t outsz)
{
    char exe[PATH_MAX];
    ssize_t n;
    int depth;

    if (out[0] != '\0')
	return;

    n = readlink("/proc/self/exe", exe, sizeof exe - 1);
    if (n <= 0 || (size_t)n >= sizeof exe)
	return;
    exe[n] = '\0';

    /* Walk up a few parents looking for repo/installed layouts. */
    for (depth = 0; depth < 8 && out[0] == '\0'; depth++) {
	char *slash = strrchr(exe, '/');
	if (slash == NULL)
	    break;
	*slash = '\0';

	/*
	 * When running from the build tree, the binary often lives under a
	 * path like:
	 *   <root>/build-<platform>/src/...
	 * Prefer the source tree's <root>/lib over <root>/build-<platform>/lib.
	 */
	Conf_try_set_datadir_suffix(out, outsz, exe, "../lib");
	Conf_try_set_datadir_suffix(out, outsz, exe, "../share/xpilot-ng");

	/* source tree layout: <root>/lib/textures */
	Conf_try_set_datadir_suffix(out, outsz, exe, "lib");
	/* staged install layout: <root>/share/xpilot-ng/textures */
	Conf_try_set_datadir_suffix(out, outsz, exe, "share/xpilot-ng");
    }
}
#endif

static const char *Conf_compute_datadir(void)
{
    static char datadir[PATH_MAX];
    static int inited = 0;

    if (inited)
	return datadir;
    inited = 1;

    datadir[0] = '\0';

    /* Explicit override. */
    Conf_try_set_datadir(datadir, sizeof datadir, getenv("XPILOTNG_DATADIR"));
    Conf_try_set_datadir(datadir, sizeof datadir, getenv("XPILOT_DATADIR"));

    /* Compiled-in default. */
    if (datadir[0] == '\0')
	Conf_try_set_datadir(datadir, sizeof datadir, CONF_DATADIR);

    /* Common install locations. */
    if (datadir[0] == '\0')
	Conf_try_set_datadir(datadir, sizeof datadir, "/usr/local/share/xpilot-ng/");
    if (datadir[0] == '\0')
	Conf_try_set_datadir(datadir, sizeof datadir, "/usr/share/xpilot-ng/");

#ifndef _WINDOWS
    Conf_try_set_datadir_from_exe(datadir, sizeof datadir);
#endif

    /* Last resort: keep compiled-in value even if missing. */
    if (datadir[0] == '\0')
	Conf_normalize_dir(datadir, sizeof datadir, CONF_DATADIR);

    return datadir;
}

char *Conf_datadir(void)
{
    static char conf[PATH_MAX];
    static int inited = 0;

    if (!inited) {
	inited = 1;
	strlcpy(conf, Conf_compute_datadir(), sizeof conf);
    }
    return conf;
}

char *Conf_defaults_file_name(void)
{
    static char conf[PATH_MAX];
    static int inited = 0;
    if (!inited) {
	inited = 1;
	snprintf(conf, sizeof conf, "%sdefaults.txt", Conf_datadir());
    }
    return conf;
}

char *Conf_password_file_name(void)
{
    static char conf[PATH_MAX];
    static int inited = 0;
    if (!inited) {
	inited = 1;
	snprintf(conf, sizeof conf, "%spassword.txt", Conf_datadir());
    }
    return conf;
}

#if 0
char *Conf_player_passwords_file_name(void)
{
    static char conf[] = CONF_PLAYER_PASSWORDS_FILE_NAME;

    return conf;
}
#endif

char *Conf_mapdir(void)
{
    static char conf[PATH_MAX];
    static int inited = 0;
    if (!inited) {
	inited = 1;
	snprintf(conf, sizeof conf, "%smaps/", Conf_datadir());
    }
    return conf;
}

char *Conf_fontdir(void)
{
    static char conf[PATH_MAX];
    static int inited = 0;
    if (!inited) {
	inited = 1;
	snprintf(conf, sizeof conf, "%sfonts/", Conf_datadir());
    }
    return conf;
}

char *Conf_default_map(void)
{
    static char conf[] = CONF_DEFAULT_MAP;

    return conf;
}

char *Conf_servermotdfile(void)
{
    static char conf[] = CONF_SERVERMOTDFILE;
    static char env[] = "XPILOTSERVERMOTD";
    char *filename;

    filename = getenv(env);
    if (filename == NULL)
	filename = conf;

    return filename;
}

char *Conf_localmotdfile(void)
{
    static char conf[] = CONF_LOCALMOTDFILE;

    return conf;
}

char conf_logfile_string[] = CONF_LOGFILE;

char *Conf_logfile(void)
{
    return conf_logfile_string;
}

char *Conf_ship_file(void)
{
    static char conf[PATH_MAX];
    static int inited = 0;
    if (!inited) {
	inited = 1;
	snprintf(conf, sizeof conf, "%sshipshapes.txt", Conf_datadir());
    }
    return conf;
}

char *Conf_texturedir(void)
{
    static char conf[PATH_MAX];
    static int inited = 0;
    if (!inited) {
	inited = 1;
	snprintf(conf, sizeof conf, "%stextures/", Conf_datadir());
    }
    return conf;
}

char *Conf_localguru(void)
{
    static char conf[] = CONF_LOCALGURU;

    return conf;
}

char *Conf_robotfile(void)
{
    static char conf[PATH_MAX];
    static int inited = 0;
    if (!inited) {
	inited = 1;
	snprintf(conf, sizeof conf, "%srobots.txt", Conf_datadir());
    }
    return conf;
}

char *Conf_zcat_ext(void)
{
    static char conf[] = CONF_ZCAT_EXT;

    return conf;
}

char *Conf_zcat_format(void)
{
    static char conf[] = CONF_ZCAT_FORMAT;

    return conf;
}

char *Conf_sounddir(void)
{
    static char conf[PATH_MAX];
    static int inited = 0;
    if (!inited) {
	inited = 1;
	snprintf(conf, sizeof conf, "%ssound/", Conf_datadir());
    }
    return conf;
}

char *Conf_soundfile(void)
{
    static char conf[PATH_MAX];
    static int inited = 0;
    if (!inited) {
	inited = 1;
	snprintf(conf, sizeof conf, "%ssound/sounds.txt", Conf_datadir());
    }
    return conf;
}


void Conf_print(void)
{
    warn("============================================================");
    warn("VERSION                   = %s", VERSION);
    warn("PACKAGE                   = %s", PACKAGE);

#ifdef DBE
    warn("DBE");
#endif
#ifdef MBX
    warn("MBX");
#endif
#ifdef PLOCKSERVER
    warn("PLOCKSERVER");
#endif
#ifdef DEVELOPMENT
    warn("DEVELOPMENT");
#endif

    warn("Conf_localguru()          = %s", Conf_localguru());
    warn("Conf_datadir()            = %s", Conf_datadir());
    warn("Conf_defaults_file_name() = %s", Conf_defaults_file_name());
    warn("Conf_password_file_name() = %s", Conf_password_file_name());
    warn("Conf_mapdir()             = %s", Conf_mapdir());
    warn("Conf_default_map()        = %s", Conf_default_map());
    warn("Conf_servermotdfile()     = %s", Conf_servermotdfile());
    warn("Conf_robotfile()          = %s", Conf_robotfile());
    warn("Conf_logfile()            = %s", Conf_logfile());
    warn("Conf_localmotdfile()      = %s", Conf_localmotdfile());
    warn("Conf_ship_file()          = %s", Conf_ship_file());
    warn("Conf_texturedir()         = %s", Conf_texturedir());
    warn("Conf_fontdir()            = %s", Conf_fontdir());
    warn("Conf_sounddir()           = %s", Conf_sounddir());
    warn("Conf_soundfile()          = %s", Conf_soundfile());
    warn("Conf_zcat_ext()           = %s", Conf_zcat_ext());
    warn("Conf_zcat_format()        = %s", Conf_zcat_format());
    warn("============================================================");
}
