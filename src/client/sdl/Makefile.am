
bin_PROGRAMS = xpilot-ng-sdl
AM_CPPFLAGS = -DCONF_DATADIR=\"$(pkgdatadir)/\" \
	-I$(top_srcdir)/src/common -I$(top_srcdir)/src/client
xpilot_ng_sdl_SOURCES = \
	asteroid_data.h DT_drawtext.c DT_drawtext.h \
	SDL_console.c SDL_console.h SDL_gfxPrimitives.c SDL_gfxPrimitives.h \
	SDL_gfxPrimitives_font.h console.c console.h \
	glwidgets.c glwidgets.h images.c images.h main.c radar.c radar.h \
	sdlevent.c sdlgui.c sdlinit.c sdlkeys.c sdlkeys.h sdlpaint.c \
	sdlpaint.h sdlwindow.c sdlwindow.h text.c text.h todo.c \
	sdlmeta.h sdlmeta.c sdlinit.h xpclient_sdl.h

if COND_SDL_GAMELOOP
  xpilot_ng_sdl_SOURCES += gameloop.c
else
  xpilot_ng_sdl_SOURCES += gameloop_x.c
endif

# On Windows, don't link X11 - use Winsock instead
if COND_WINDOWS
xpilot_ng_sdl_SOURCES += win32hacks.c
# SDL_ttf needs freetype, SDL_image needs png and zlib - must be in dependency order
xpilot_ng_sdl_LDADD = $(top_builddir)/src/client/libxpclient.a $(top_builddir)/src/common/libxpcommon.a @GL_LIBS@ @SDL_EXTRA_LIBS@ -lfreetype -lpng -lz -lws2_32 -lwinmm
else
xpilot_ng_sdl_LDADD = $(top_builddir)/src/client/libxpclient.a $(top_builddir)/src/common/libxpcommon.a @GL_LIBS@ @SDL_EXTRA_LIBS@ @X_LIBS@ @X_PRE_LIBS@ @X_EXTRA_LIBS@ -lX11
endif

EXTRA_DIST = win32hacks.c xpilot.rc xpilot_sdl.dsp xpilot_sdl.dsw
