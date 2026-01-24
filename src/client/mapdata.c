/*
 * XPilot NG, a multiplayer space war game.
 *
 * Copyright (C) 2001 Juha Lindstr?m <juhal@users.sourceforge.net>
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

#include "xpclient.h"

/* kps - you should be able to change this without a recompile */
#define DATADIR ".xpilot_data"
#define COPY_BUF_SIZE 8192
#define REDIRECT_CACHE_FILE "mapdata_redirects.txt"
#define MAPDATA_READ_TIMEOUT_SEC 2
#define MAPDATA_MAX_STALL_SEC 60

typedef struct {
    char *protocol;
    char *host;
    int   port;
    const char *path;
    char *query;
} URL;

static int Mapdata_extract(const char *name);
static int Mapdata_download(const URL *url, const char *filePath);
static int Mapdata_download_redirects(const URL *url, const char *filePath, int depth);
static int Url_parse(const char *urlstr, URL *url);
static void Url_free_parsed(URL *url);
static void Mapdata_header_extract_location(const char *buf, int len, char *location, size_t locsz);
static void Mapdata_set_effective_url(const char *urlstr);
static bool Mapdata_redirect_cache_lookup(const char *from, char *to, size_t tosz);
static void Mapdata_redirect_cache_save(const char *from, const char *to);
static void Mapdata_redirect_cache_path(char *path, size_t pathsz);
static void Url_to_string(const URL *url, char *buf, size_t bufsz);
static int Mapdata_download_external(const char *urlstr, const char *filePath);
static void Mapdata_home_datadir(char *path, size_t pathsz);
static const char *Mapdata_next_path(const char *p, char *out, size_t outsz);
static int Mapdata_join_path(char *out, size_t outsz, const char *dir, const char *leaf);
static bool Mapdata_find_existing(const char *name, const char *base_noext,
				  char *dir_out, size_t dirsz,
				  char *pkg_out, size_t pkgsz);
static void Mapdata_add_texture_dir(const char *dir);
static bool Mapdata_path_list_contains(const char *list, const char *dir);

static bool setup_done = false;
static char mapdata_effective_url[1024];

int Mapdata_setup(const char *urlstr)
{
    URL url;
    const char *orig_urlstr = urlstr;
    const char *use_urlstr = urlstr;
    char cached_url[1024];
    const char *name;
    char path[1024], buf[1024], *ptr;
    char base_noext[256];
    char found_dir[1024];
    char found_pkg[1024];
    char home_dir[1024];
    const char *dir = NULL;
    int rv = false;
    int n;

    if (setup_done)
	return true;

    memset(path, 0, sizeof(path));
    memset(buf, 0, sizeof(buf));

    cached_url[0] = '\0';
    if (Mapdata_redirect_cache_lookup(orig_urlstr, cached_url, sizeof cached_url)) {
	use_urlstr = cached_url;
	/* Keep terminal output minimal; the UI callback will show progress. */
	warn("Using cached redirect for map data URL.");
    }
    Mapdata_set_effective_url(use_urlstr);

    if (!Url_parse(use_urlstr, &url)) {
	warn("malformed URL: %s", use_urlstr);
	return false;
    }

    for (name = url.path + strlen(url.path) - 1; name > url.path; name--) {
	if (*(name - 1) == '/')
	    break;
    }

    if (*name == '\0') {
	warn("no file name in URL: %s", urlstr);
	goto end;
    }

    strlcpy(base_noext, name, sizeof base_noext);
    ptr = strrchr(base_noext, '.');
    if (ptr != NULL)
	*ptr = '\0';
    else
	base_noext[0] = '\0';

    /*
     * Dedupe: check multiple locations for already-downloaded data.
     * Prefer the configured/system texture dir, then other texturePath
     * entries, then ~/.xpilot_data.
     */
    found_dir[0] = '\0';
    found_pkg[0] = '\0';
    if (base_noext[0] != '\0'
	&& Mapdata_find_existing(name, base_noext,
				 found_dir, sizeof found_dir,
				 found_pkg, sizeof found_pkg)) {
	if (access(found_dir, F_OK) == 0) {
	    Mapdata_add_texture_dir(found_dir);
	    /* Found extracted directory already. */
	    rv = true;
	    setup_done = true;
	    goto end;
	}
	/* Found package file; extract it where it is. */
	if (found_pkg[0] != '\0') {
	    char pkg_dir[1024];
	    strlcpy(pkg_dir, found_pkg, sizeof pkg_dir);
	    ptr = strrchr(pkg_dir, '.');
	    if (ptr != NULL)
		*ptr = '\0';
	    Mapdata_add_texture_dir(pkg_dir);
	}
	Client_status("Extracting map data...");
	if (Mapdata_extract(found_pkg)) {
	    rv = true;
	    setup_done = true;
	    goto end;
	}
	/* Fall through to download if extraction fails. */
    }

    /*
     * Download destination policy:
     * - Prefer Conf_texturedir() (installed or source tree) if writable.
     * - Otherwise use ~/.xpilot_data.
     * Avoid build-linux/build-windows as "data roots".
     */
    if (access(Conf_texturedir(), R_OK | W_OK | X_OK) == 0) {
	dir = Conf_texturedir();
    } else {
	Mapdata_home_datadir(home_dir, sizeof home_dir);
	if (home_dir[0] == '\0') {
	    error("HOME is unset; can't create texture cache");
	    goto end;
	}
	if (access(home_dir, F_OK) != 0) {
	    if (mkdir(home_dir, S_IRWXU | S_IRWXG | S_IRWXO) == -1) {
		error("failed to create directory %s", home_dir);
		goto end;
	    }
	}
	dir = home_dir;
    }

    if (!Mapdata_join_path(path, sizeof path, dir, name)) {
	error("map data file path too long");
	goto end;
    }

    if (strrchr(path, '.') == NULL) {
	error("no extension in file name %s.", name);
	goto end;
    }

    /* temporarily make path point to the directory name */
    ptr = strrchr(path, '.');
    *ptr = '\0';

    /* add this new texture directory to texturePath */
    Mapdata_add_texture_dir(path);

    if (access(path, F_OK) == 0) {
	warn("Required bitmaps have already been downloaded.");
	rv = true;
	goto end;
    }
    /* reset path so that it points to the package file name */
    *ptr = '.';

    warn("Downloading map data from %s to %s.", use_urlstr, path);
    Client_status("Downloading map data...");

    if (!Mapdata_download(&url, path)) {
	warn("downloading map data failed");
	goto end;
    }

    Client_status("Extracting map data...");
    if (!Mapdata_extract(path)) {
	warn("extracting map data failed");
	goto end;
    }

    if (mapdata_effective_url[0] != '\0'
	&& strcmp(orig_urlstr, mapdata_effective_url) != 0) {
	Mapdata_redirect_cache_save(orig_urlstr, mapdata_effective_url);
    }

    Client_status("Map data ready.");
    rv = true;
    setup_done = true;

 end:
    Url_free_parsed(&url);
    return rv;
}

static bool Mapdata_path_list_contains(const char *list, const char *dir)
{
    char item[1024];
    const char *p = list;

    if (list == NULL || dir == NULL || *dir == '\0')
	return false;

    while ((p = Mapdata_next_path(p, item, sizeof item)) != NULL) {
	if (strcmp(item, dir) == 0)
	    return true;
	if (*p == ':')
	    p++;
    }
    return false;
}

static void Mapdata_add_texture_dir(const char *dir)
{
    char norm[1024];
    size_t n;
    char *temp;

    if (dir == NULL || *dir == '\0')
	return;

    strlcpy(norm, dir, sizeof norm);
    n = strlen(norm);
    if (n > 0 && norm[n - 1] == PATHNAME_SEP)
	norm[n - 1] = '\0';

    if (realTexturePath != NULL && Mapdata_path_list_contains(realTexturePath, norm))
	return;

    if (realTexturePath == NULL) {
	realTexturePath = strdup(norm);
	return;
    }

    temp = XMALLOC(char, strlen(realTexturePath) + strlen(norm) + 2);
    if (temp == NULL) {
	error("not enough memory to extend realTexturePath");
	return;
    }
    temp[0] = '\0';
    strlcpy(temp, realTexturePath, strlen(realTexturePath) + strlen(norm) + 2);
    strlcat(temp, ":", strlen(realTexturePath) + strlen(norm) + 2);
    strlcat(temp, norm, strlen(realTexturePath) + strlen(norm) + 2);
    free(realTexturePath);
    realTexturePath = temp;
}

static void Mapdata_home_datadir(char *path, size_t pathsz)
{
    const char *home = getenv("HOME");
    int n;

    if (pathsz == 0)
	return;
    path[0] = '\0';
    if (home == NULL)
	return;

    if (home[0] == '\0')
	n = snprintf(path, pathsz, "%s", DATADIR);
    else if (home[strlen(home) - 1] == PATHNAME_SEP)
	n = snprintf(path, pathsz, "%s%s", home, DATADIR);
    else
	n = snprintf(path, pathsz, "%s%c%s", home, PATHNAME_SEP, DATADIR);
    if (n < 0 || (size_t)n >= pathsz)
	path[0] = '\0';
}

static const char *Mapdata_next_path(const char *p, char *out, size_t outsz)
{
    size_t n = 0;

    if (outsz == 0)
	return NULL;
    out[0] = '\0';
    if (p == NULL || *p == '\0')
	return NULL;

    while (*p == ':')
	p++;
    if (*p == '\0')
	return NULL;

    while (*p != '\0' && *p != ':') {
	if (n + 1 < outsz)
	    out[n++] = *p;
	p++;
    }
    out[n] = '\0';
    return p;
}

static int Mapdata_join_path(char *out, size_t outsz, const char *dir, const char *leaf)
{
    size_t dlen;

    if (outsz == 0)
	return 0;
    if (leaf == NULL || *leaf == '\0')
	return 0;
    if (dir == NULL || *dir == '\0')
	return (snprintf(out, outsz, "%s", leaf) >= 0 && strlen(out) < outsz);

    dlen = strlen(dir);
    if (dir[dlen - 1] == PATHNAME_SEP)
	return (snprintf(out, outsz, "%s%s", dir, leaf) >= 0 && strlen(out) < outsz);
    return (snprintf(out, outsz, "%s%c%s", dir, PATHNAME_SEP, leaf) >= 0 && strlen(out) < outsz);
}

static bool Mapdata_find_existing(const char *name, const char *base_noext,
				  char *dir_out, size_t dirsz,
				  char *pkg_out, size_t pkgsz)
{
    char d[1024];
    char p[1024];
    const char *scan;

    dir_out[0] = '\0';
    pkg_out[0] = '\0';

    /* 1) Conf_texturedir() (system or repo source). */
    strlcpy(d, Conf_texturedir(), sizeof d);
    if (Mapdata_join_path(p, sizeof p, d, base_noext) && access(p, F_OK) == 0) {
	strlcpy(dir_out, p, dirsz);
	return true;
    }
    if (Mapdata_join_path(p, sizeof p, d, name) && access(p, R_OK) == 0) {
	strlcpy(pkg_out, p, pkgsz);
	return true;
    }

    /* 2) All entries in realTexturePath (readable). */
    scan = realTexturePath;
    while ((scan = Mapdata_next_path(scan, d, sizeof d)) != NULL) {
	if (Mapdata_join_path(p, sizeof p, d, base_noext) && access(p, F_OK) == 0) {
	    strlcpy(dir_out, p, dirsz);
	    return true;
	}
	if (Mapdata_join_path(p, sizeof p, d, name) && access(p, R_OK) == 0) {
	    strlcpy(pkg_out, p, pkgsz);
	    return true;
	}
	if (*scan == ':')
	    scan++;
    }

    /* 3) ~/.xpilot_data */
    Mapdata_home_datadir(d, sizeof d);
    if (d[0] != '\0') {
	if (Mapdata_join_path(p, sizeof p, d, base_noext) && access(p, F_OK) == 0) {
	    strlcpy(dir_out, p, dirsz);
	    return true;
	}
	if (Mapdata_join_path(p, sizeof p, d, name) && access(p, R_OK) == 0) {
	    strlcpy(pkg_out, p, pkgsz);
	    return true;
	}
    }

    return false;
}


static int Mapdata_extract(const char *name)
{
    gzFile in;
    FILE *out;
    int retval;
    size_t rlen, wlen;
    char dir[256], buf[COPY_BUF_SIZE], fname[256], rel[256], *ptr;
    long int size;
    int count, i;

    strlcpy(dir, name, sizeof dir);
    ptr = strrchr(dir, '.');
    if (ptr == NULL) {
	error("file name has no extension %s", dir);
	return 0;
    }
    *ptr = '\0';

    if (mkdir(dir, S_IRWXU | S_IRWXG | S_IRWXO) == -1) {
	error("failed to create directory %s", dir);
	return 0;
    }

    if ((in = gzopen(name, "rb")) == NULL) {
	error("failed to open %s for reading", name);
	return 0;
    }

    if (gzgets(in, buf, COPY_BUF_SIZE) == Z_NULL) {
	error("failed to read header from %s", name);
	gzclose(in);
	return 0;
    }

    if (sscanf(buf, "XPD %d\n", &count) != 1) {
	error("invalid header in %s", name);
	gzclose(in);
	return 0;
    }

    for (i = 0; i < count; i++) {

	if (gzgets(in, buf, COPY_BUF_SIZE) == Z_NULL) {
	    error("failed to read file info from %s", name);
	    gzclose(in);
	    return 0;
	}

	if (sscanf(buf, "%255s\n%ld\n", rel, &size) != 2) {
	    error("failed to parse file info %s", buf);
	    gzclose(in);
	    return 0;
	}

	/* security check */
	if (strchr(rel, PATHNAME_SEP) != NULL) {
	    error("file name %s is illegal", rel);
	    gzclose(in);
	    return 0;
	}

	if (snprintf(fname, sizeof fname, "%s%c%s", dir, PATHNAME_SEP, rel)
	    >= (int)sizeof fname) {
	    error("file name %s%c%s is too long", dir, PATHNAME_SEP, rel);
	    gzclose(in);
	    return 0;
	}

	warn("Extracting %s (%ld)", fname, size);

	if ((out = fopen(fname, "wb")) == NULL) {
	    error("failed to open %s for writing", buf);
	    gzclose(in);
	    return 0;
	}

	while (size > 0) {
	    retval = gzread(in, buf, MIN(COPY_BUF_SIZE, (unsigned)size));
	    if (retval == -1) {
		error("error when reading %s", name);
		gzclose(in);
		fclose(out);
		return 0;
	    }
	    if (retval == 0) {
		error("unexpected end of file %s", name);
		gzclose(in);
		fclose(out);
		return 0;
	    }

	    rlen = retval;
	    wlen = fwrite(buf, 1, rlen, out);
	    if (wlen != rlen) {
		error("failed to write to %s", fname);
		gzclose(in);
		fclose(out);
		return 0;
	    }

	    size -= rlen;
	}

	fclose(out);
    }

    gzclose(in);
    return 1;
}


static int Mapdata_download(const URL *url, const char *filePath)
{
    return Mapdata_download_redirects(url, filePath, 0);
}

static int Mapdata_download_redirects(const URL *url, const char *filePath, int depth)
{
    char buf[1024];
    int rv, header, c, len, i;
    int status = 0;
    char location[1024];
    sock_t s;
    FILE *f = NULL;
    size_t n;
    size_t total = 0;
    time_t last_progress;
    time_t last_status;

    if (depth > 3) {
	error("too many redirects");
	return false;
    }

    if (strncmp("http", url->protocol, 4) != 0) {
	error("unsupported protocol %s", url->protocol);
	return false;
    }

    if (sock_open_tcp(&s) == SOCK_IS_ERROR) {
	error("failed to create a socket");
	return false;
    }
    if (sock_connect(&s, url->host, url->port) == SOCK_IS_ERROR) {
	error("couldn't connect to download address");
	sock_close(&s);
	return false;
    }
    sock_set_timeout(&s, MAPDATA_READ_TIMEOUT_SEC, 0);
    last_progress = time(NULL);
    last_status = last_progress;

    if (url->query) {
	if (snprintf(buf, sizeof buf,
	     "GET %s?%s HTTP/1.1\r\nHost: %s:%d\r\nConnection: close\r\n\r\n",
	     url->path, url->query, url->host, url->port) == -1) {
	    error("too long URL");
	    sock_close(&s);
	    return false;
	}

    } else {
	if (snprintf(buf, sizeof buf,
	     "GET %s HTTP/1.1\r\nHost: %s:%d\r\nConnection: close\r\n\r\n",
	     url->path, url->host, url->port) == -1) {

	    error("too long URL");
	    sock_close(&s);
	    return false;
	}
    }

    if (sock_write(&s, buf, (int)strlen(buf)) == -1) {
	error("socket write failed");
	sock_close(&s);
	return false;
    }

    header = 2;
    c = 0;
    location[0] = '\0';

    for(;;) {
	len = 0;
	while (len < 100) {
	    int readable;

	    readable = sock_readable(&s);
	    if (readable == SOCK_IS_ERROR) {
		error("socket read failed");
		rv = false;
		goto done;
	    }
	    if (readable == SOCK_IS_OK) {
		time_t now = time(NULL);
		if (now - last_status >= 2) {
		    Client_status("Downloading map data... (%lu KB)", (unsigned long)(total / 1024));
		    last_status = now;
		}
		if (now - last_progress >= MAPDATA_MAX_STALL_SEC) {
		    error("download timed out");
		    rv = false;
		    goto done;
		}
		continue;
	    }

	    if ((i = sock_read(&s, buf + len, sizeof(buf) - len)) == -1) {
		error("socket read failed");
		rv = false;
		goto done;
	    }
	    if (i == 0)
		break;
	    len += i;
	    total += (size_t)i;
	    last_progress = time(NULL);
	}

	if (len == 0) {
	    rv = !header;
	    break;
	}

	if (header == 2) {
	    if (strncmp(buf, "HTTP", 4)) {
		rv = false;
		break;
	    }
	    i = 0;
	    while (buf[i] != ' ') {
		i++;
		if (i >= len - 1) {
		    rv = false;
		    goto done;
		}
	    }
	    i++;
	    status = atoi(&buf[i]);
	    header = 1;
	}

	printf("#");
	fflush(stdout);

	if (header) {
	    if (status >= 300 && status < 400 && location[0] == '\0')
		Mapdata_header_extract_location(buf, len, location, sizeof location);
	    for (i = 0; i < len; i++) {
		if (c % 2 == 0 && buf[i] == '\r')
		    c++;
		else if (c % 2 == 1 && buf[i] == '\n')
		    c++;
		else
		    c = 0;

		if (c == 4) {
		    header = 0;
		    if (status >= 300 && status < 400) {
			if (location[0] == '\0') {
			    rv = false;
			    goto done;
			}
			/* Follow redirect (best effort). */
			{
			    URL u2;
			    char locbuf[1024];
			    const char *loc = location;
			    char absbuf[1024];

			    /* Relative redirect. */
			    if (loc[0] == '/' && loc[1] != '/') {
				Url_to_string(url, absbuf, sizeof absbuf);
				/* absbuf is protocol://host[:port]/path; keep up to host[:port] */
				{
				    char *p = strstr(absbuf, "://");
				    if (p) {
					p = strchr(p + 3, '/');
					if (p) *p = '\0';
				    }
				}
				locbuf[0] = '\0';
				strlcpy(locbuf, absbuf, sizeof locbuf);
				strlcat(locbuf, loc, sizeof locbuf);
				loc = locbuf;
			    }

			    if (!strncmp(loc, "https://", 8)) {
				/* Prefer external downloader for TLS. */
				Mapdata_set_effective_url(loc);
				rv = Mapdata_download_external(loc, filePath);
				sock_close(&s);
				return rv;
			    }

			    Mapdata_set_effective_url(loc);
			    if (!Url_parse(loc, &u2)) {
				rv = false;
				goto done;
			    }
			    sock_close(&s);
			    rv = Mapdata_download_redirects(&u2, filePath, depth + 1);
			    Url_free_parsed(&u2);
			    return rv;
			}
		    }
		    if (status < 200 || status >= 300) {
			rv = false;
			goto done;
		    }
		    if ((f = fopen(filePath, "wb")) == NULL) {
			error("failed to open %s", filePath);
			rv = false;
			goto done;
		    }
		    if (i < len - 1) {
			n = len - i - 1;
			memmove(buf, buf + i + 1, n);
			len = len - i - 1;
		    } else if (i == len - 1) {
			len = 0;
		    }
		}
	    }
	}

	if (!header && len) {
	    n = len;
	    if (fwrite(buf, 1, n, f) < n) {
		error("file write failed");
		rv =  false;
		break;
	    }
	}
    }
 done:
    printf("\n");
    if (f)
	if (fclose(f) != 0)
	    error("Error closing texture file %s", filePath);
    sock_close(&s);
    return rv;
}

static void Url_to_string(const URL *url, char *buf, size_t bufsz)
{
    if (bufsz == 0)
	return;
    buf[0] = '\0';
    if (url == NULL || url->protocol == NULL || url->host == NULL || url->path == NULL)
	return;
    if (url->query && url->query[0] != '\0')
	snprintf(buf, bufsz, "%s://%s:%d%s?%s", url->protocol, url->host, url->port, url->path, url->query);
    else
	snprintf(buf, bufsz, "%s://%s:%d%s", url->protocol, url->host, url->port, url->path);
}

#ifndef _WINDOWS
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

static int Mapdata_download_external(const char *urlstr, const char *filePath)
{
#ifdef _WINDOWS
    UNUSED_PARAM(urlstr);
    UNUSED_PARAM(filePath);
    return false;
#else
    pid_t pid;
    int status;

    if (urlstr == NULL || *urlstr == '\0' || filePath == NULL || *filePath == '\0')
	return false;

    Client_status("Downloading map data (external)...");

    pid = fork();
    if (pid == -1)
	return false;
    if (pid == 0) {
	/* Try curl first. */
	execlp("curl", "curl", "-fsSL", "-L", "-o", filePath, urlstr, (char *)NULL);
	/* Then wget. */
	execlp("wget", "wget", "-q", "-O", filePath, urlstr, (char *)NULL);
	_exit(127);
    }

    if (waitpid(pid, &status, 0) == -1)
	return false;
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
	return true;
    return false;
#endif
}
static void Mapdata_set_effective_url(const char *urlstr)
{
    if (urlstr == NULL || *urlstr == '\0') {
	mapdata_effective_url[0] = '\0';
	return;
    }
    strlcpy(mapdata_effective_url, urlstr, sizeof mapdata_effective_url);
}

static void Mapdata_redirect_cache_path(char *path, size_t pathsz)
{
    char *home;
    int n;

    if (pathsz == 0)
	return;
    path[0] = '\0';

    home = getenv("HOME");
    if (home == NULL)
	return;

    if (home[0] == '\0')
	n = snprintf(path, pathsz, "%s/%s", DATADIR, REDIRECT_CACHE_FILE);
    else if (home[strlen(home) - 1] == PATHNAME_SEP)
	n = snprintf(path, pathsz, "%s%s/%s", home, DATADIR, REDIRECT_CACHE_FILE);
    else
	n = snprintf(path, pathsz, "%s%c%s/%s", home, PATHNAME_SEP, DATADIR, REDIRECT_CACHE_FILE);

    if (n < 0 || (size_t)n >= pathsz)
	path[0] = '\0';
}

static bool Mapdata_redirect_cache_lookup(const char *from, char *to, size_t tosz)
{
    char path[1024];
    FILE *f;
    char line[2048];
    bool found = false;

    if (to == NULL || tosz == 0)
	return false;
    to[0] = '\0';
    if (from == NULL || *from == '\0')
	return false;

    Mapdata_redirect_cache_path(path, sizeof path);
    if (path[0] == '\0')
	return false;

    f = fopen(path, "rb");
    if (f == NULL)
	return false;

    /* Use last matching entry (allows append-only updates). */
    while (fgets(line, sizeof line, f) != NULL) {
	char *tab;
	char *nl;
	char *lhs;
	char *rhs;

	nl = strpbrk(line, "\r\n");
	if (nl) *nl = '\0';
	if (line[0] == '\0' || line[0] == '#')
	    continue;

	tab = strchr(line, '\t');
	if (tab == NULL)
	    tab = strchr(line, ' ');
	if (tab == NULL)
	    continue;
	*tab++ = '\0';
	while (*tab == ' ' || *tab == '\t')
	    tab++;

	lhs = line;
	rhs = tab;
	if (*rhs == '\0')
	    continue;

	if (strcmp(lhs, from) == 0) {
	    strlcpy(to, rhs, tosz);
	    found = true;
	}
    }

    fclose(f);
    return found;
}

static void Mapdata_redirect_cache_save(const char *from, const char *to)
{
    char path[1024];
    char dir[1024];
    char *slash;
    FILE *f;

    if (from == NULL || *from == '\0' || to == NULL || *to == '\0')
	return;

    Mapdata_redirect_cache_path(path, sizeof path);
    if (path[0] == '\0')
	return;

    strlcpy(dir, path, sizeof dir);
    slash = strrchr(dir, PATHNAME_SEP);
    if (slash != NULL) {
	*slash = '\0';
	if (access(dir, F_OK) != 0)
	    (void)mkdir(dir, S_IRWXU | S_IRWXG | S_IRWXO);
    }

    f = fopen(path, "ab");
    if (f == NULL)
	return;

    (void)fprintf(f, "%s\t%s\n", from, to);
    (void)fclose(f);
}

static void Mapdata_header_extract_location(const char *buf, int len, char *location, size_t locsz)
{
    int i;
    const char key[] = "location:";
    const int keylen = (int)sizeof(key) - 1;

    if (location == NULL || locsz == 0)
	return;
    if (buf == NULL || len <= 0)
	return;

    for (i = 0; i + keylen < len; i++) {
	int j;
	int at_line_start = (i == 0 || buf[i - 1] == '\n');
	if (!at_line_start)
	    continue;
	for (j = 0; j < keylen; j++) {
	    char c = buf[i + j];
	    if (c >= 'A' && c <= 'Z')
		c = (char)(c - 'A' + 'a');
	    if (c != key[j])
		break;
	}
	if (j == keylen) {
	    int k = i + keylen;
	    while (k < len && (buf[k] == ' ' || buf[k] == '\t'))
		k++;
	    /* Copy until CR/LF. */
	    {
		int out = 0;
		while (k < len && buf[k] != '\r' && buf[k] != '\n') {
		    if ((size_t)(out + 1) >= locsz)
			break;
		    location[out++] = buf[k++];
		}
		location[out] = '\0';
	    }
	    return;
	}
    }
}


static int Url_parse(const char *urlstr, URL *url)
{
    int len, i, beg, doPort;
    char *buf;

    memset(url, 0, sizeof(URL));
    url->port = 80;
    url->path = "/";

    len = strlen(urlstr);
    buf = strdup(urlstr);
    if (buf == NULL) {
	error("no memory for URL");
	return false;
    }

    for (i = 0; i < len; i++) {
	if (buf[i] == ':') {
	    buf[i] = '\0';
	    url->protocol = buf;
	    break;
	}
    }

    beg = i + 3;
    if (beg >= len || buf[i + 1] != '/' || buf[i + 2] != '/') {
	free(buf);
	return false;
    }

    doPort = 0;
    for (i = beg; i < len; i++) {
	if (buf[i] == ':' || buf[i] == '/') {
	    if (buf[i] == ':') doPort = 1;
	    buf[i] = '\0';
	    break;
	}
    }

    url->host = buf + beg;
    beg = i + 1;
    if (beg >= len) return true;

    if (doPort) {
	for (i = beg; i < len; i++) {
	    if (buf[i] == '/') {
		buf[i] = '\0';
		break;
	    }
	}
	url->port = atoi(buf + beg);
	/* error detection should be added */

	beg = i + 1;
	if (beg >= len)
	    return true;
    }

    /* make space for / in the beginning of path */
    memmove(url->host - 1, url->host, strlen(url->host) + 1);
    url->host--;
    buf[beg - 1] = '/';

    for (i = beg; i < len; i++) {
	if (buf[i] == '?') {
	    buf[i] = '\0';
	    break;
	}
    }
    url->path = buf + beg - 1;

    beg = i + 1;
    if (beg >= len) return true;

    url->query = buf + beg;
    return true;
}


static void Url_free_parsed(URL *url)
{
    free(url->protocol);
}
