# $Id: configure.ac,v 1.42 2006/09/24 04:17:55 dick Exp $
# @configure_input@
#
# configure.ac for xpilot5
# ./configure.ac
#
# Process this file with autoconf to produce a configure script.
#
# Copyright 2004 by XPilot Team "xpilot@xpilot.org"
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU Library General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Library General Public License for more details.
#
# You should have received a copy of the GNU Library General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
# USA.
#
# Please report all bugs and problems to "xpilot5@buckosoft.com".
#
########################################################################################
# $Log: configure.ac,v $
# Revision 1.42  2006/09/24 04:17:55  dick
# Always define IMAGELIBS to be something
#
# Revision 1.41  2004/05/04 00:23:30  dick
# Linux needs to install client sound.conf and sounds/*.wav
#
# Revision 1.40  2004/05/02 03:08:00  dick
# Audio support.  Separate client and server audio support.
# Servers will probably always build with -DSOUND
# Clients will build -DSOUND only if OpenAL is present.
#
# Revision 1.39  2004/02/28 08:02:23  dick
# Install the control bitmap buttons from src/control/res to $/buttons
#
# Revision 1.38  2004/02/24 05:24:58  dick
# Display the C++ compiler, not the C compiler
#
# Revision 1.36  2004/02/06 15:48:08  dick
# Replace some old ++ references with 5
#
# Revision 1.35  2004/02/03 07:03:13  dick
# Install the scoreserver web pages on unix.
#
# Revision 1.34  2004/01/28 05:51:15  dick
# Don't echo the server password in ./configure because i ask people
# to mail me their output and i don't want their passwords.
#
# Revision 1.33  2004/01/26 17:21:47  dick
# Add Conf_browser which is the user's preferred browser
#
# Revision 1.32  2004/01/23 07:29:19  dick
# We need to include -lz along with libpng.
#
# Revision 1.31  2004/01/23 07:03:17  dick
# No -L on the fullpath libpng.a
#
# Revision 1.30  2004/01/23 06:44:05  dick
# XPPNGLIB = -lpng if we are using the system's libpng
#
# Revision 1.29  2004/01/23 05:02:13  dick
# Figure out if we built a private copy of libpng and link to it if need be.
#
# Revision 1.28  2004/01/22 05:08:16  dick
# Uh-oh, another reorg.  The product is XPilot5.  The name of the control
# program is also XPilot5.  No more shalt thou speak of XPilot++.
#
# Revision 1.27  2004/01/20 15:14:26  dick
# Get rid of #define WINDOWSCALING.  Window scaling is always built, it's not an option.
#
# Revision 1.26  2004/01/20 06:13:53  dick
# Add support for `make install` to a non-priv'd directory, like /home/dick/xpilot.
# If we are doing a default installation (/usr/local/xpilot) then the VARDIR
# becomes /var/xpilot .  If we are not default then use $prefix/var.
#
# Revision 1.25  2004/01/07 18:38:46  dick
# Pass OSFLAG to make depend so it won't look for w32 includes.
#
# Revision 1.24  2004/01/07 05:10:00  dick
# Unix needs to define _UNIX .
#
# Revision 1.23  2004/01/06 04:49:13  dick
# Add support for building under Cygwin.  Still needs work and the output
# is X11 and requires something like Cygwin's XWin.exe to run under.
#
# Revision 1.22  2004/01/05 05:13:36  dick
# Don't force gcc over g++.  XPilot++ is c++.  Pass includes to makedepend to shut it up.
#
# Revision 1.21  2003/12/23 01:19:06  dick
# Linux needs libfltk_images.a and libpng.a .
# XPCheckButton moved from src/control to src/common/xp-fltkwidgets .
#
# Revision 1.20  2002/09/09 23:58:08  dick
# Remove kbconfig and xpwhere from the directories we traverse.
#
# Revision 1.19  2002/08/14 21:00:29  dick
# Linux doesn't like AC_DEFAULT_PREFIX in an if clause.
#
# Revision 1.18  2002/08/14 19:43:36  dick
# Configure for cygwin/gcc
#
# Revision 1.17  2002/08/01 23:59:38  dick
# Checked in evil debug code by mistake
#
# Revision 1.16  2002/08/01 20:27:21  dick
# Better KDE detection.  Install man pages.
#
# Rename programs:
# src/         Windows              Unix
# control      XPilot.exe           xpilot
# client       XPilotClient.exe     xpilotclient
# server       XPilotServer.exe     xpilotserver
# scoreserver  XPScoreServer.exe    xpscoreserver
# replay       XPReplay.exe         xpreplay
#
# Revision 1.15  2002/07/30 07:00:06  dick
# Move install helper directories to ./inst
#
# Revision 1.14  2002/07/26 06:28:56  dick
# Install XPilot++ into the kde games menu
#
# Revision 1.13  2002/07/21 19:27:06  dick
# Configure and install the server password.
#
# Revision 1.12  2002/07/20 16:08:57  dick
# Configure and install the server's password into the server's password file.
#
# Revision 1.11  2002/07/12 16:59:27  dick
# Fix the disable-silent option
#
# Revision 1.10  2002/07/08 19:31:32  dick
# Add support for $WRITABLED, the writable database directory.
# --disable-silent makes noisier builds.
#
# Revision 1.9  2002/06/26 04:55:20  dick
# Add scoreserver to the Linux mix.
#
# Revision 1.8  2002/06/01 20:53:13  dick
# ./configure works again.  Add header comment info to Imakefiles.
#
# Revision 1.7  2002/05/28 00:16:24  jlmiller
#
# XPilot.dsw was renamed to XPilot++.dsw
#
# Revision 1.6  2002/02/17 22:33:57  dick
# Install xpilotedit's buttons to $/lib/buttons ( /usr/local/games/lib/buttons )
#
# Revision 1.5  2001/11/09 22:22:19  dick
# Support for relative or absolute LOCAL_FLTK.  Don't use the default
# C++ flags (-O2 -g); rather use the flags we want.
#
# Revision 1.4  2001/10/24 16:40:19  dick
# Add support for "make install"
#
# Revision 1.3  2001/10/11 16:31:28  dick
# ./configure supports --enable-winscaling (default = yes)
#
# Revision 1.2  2001/09/23 08:50:43  dick
# Add support for msub'ing version.h
#
# Revision 1.1  2001/09/23 06:25:26  dick
# Added support for autoconf.
# Right now, "make install" is not supported.  And config.h is rather empty.
#

AC_INIT

AC_CONFIG_SRCDIR(XPilot5.dsw)
AC_CONFIG_HEADER(config.h)

# Checks for programs.
#AC_PROG_YACC
AC_PROG_CC
AC_PROG_CPP
#AC_PROG_LEX
AC_PROG_INSTALL
#AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB


if test "$RANLIB" != ":"; then
  LIBCOMMAND="ar cr"
else
  LIBCOMMAND="ar crs"
fi

AC_SUBST(LIBCOMMAND)

DSOCOMMAND="echo"
DSONAME=""

uname=`uname`
uversion=`uname -r | sed -e 's/-.*$//g;s/[\.]//g'`
if test "$uname" = "IRIX64"; then
    uname="IRIX"
fi

##########################################################################################
#
# Check for Windows builds
#
AC_CANONICAL_BUILD
#echo s `build_cpu', `build_vendor', and `build_os'.
#echo cpu=$build_cpu
#echo vendor=$build_vendor
#echo os=^$build_os^


##########################################################################################
#
# Preset values
#
DEBUGFLAG=""
DEBUGNOTE="off"
SILENTBUILD=".SILENT:"
PICFLAG=0
CFLAGS="${CFLAGS:=}"
CXXFLAGS="${CXXFLAGS:=}"
HASPASSWORD=0
MYPASSWORD=""
IS_CYGWIN=0

# echo "1 CXXFLAGS=$CXXFLAGS"

# Checks for libraries.

##########################################################################################
#
# Check for Win32 (Windows, NT, Win2K, 95/98/ME, etc etc etc)
#
INST_WIN32=""
if test x$build_os = xcygwin; then
	echo "Do Windows (cygwin) build"
	MENUWIN32=win32
	INST_WIN32=win32
	CFLAGS="$CFLAGS -D_WINDOWS -I/usr/X11R6/include"
	CXXFLAGS="$CXXFLAGS -D_WINDOWS -I/usr/X11R6/include"
	OSFLAG="-D_CYGWIN"
fi
AC_SUBST(INST_WIN32)

if test x$build_os = xcygwin; then
	AC_PREFIX_DEFAULT("C:/XPilot5")
	echo $ECHO_N ""
else
	AC_PREFIX_DEFAULT(/usr/local/xpilot)
	echo $ECHO_N ""
fi

##########################################################################################
#
# Check for X11
#
###if test x$build_os != xcygwin; then
	AC_PATH_XTRA

	if test x$no_x = xyes; then
		AC_MSG_ERROR(Configure could not find required X11 libraries, aborting.)
	fi

	if test "x$X_PRE_LIBS" != x; then
		AC_MSG_WARN(Ignoring libraries \"$X_PRE_LIBS\" requested by configure.)
	fi

	LIBS="$LIBS$X_LIBS"
	CFLAGS="$CFLAGS$X_CFLAGS"
	CXXFLAGS="$CXXFLAGS$X_CFLAGS"
	ac_cpp="$ac_cpp -I$x_includes"
	XINCLUDES="-I$x_includes"

	AC_SUBST(XINCLUDES)
###fi
##########################################################################################
#
# Checks for header files.
#

AC_HEADER_STDC
AC_CHECK_HEADERS([errno.h fcntl.h float.h limits.h memory.h netdb.h netinet/in.h stdlib.h string.h strings.h sys/socket.h sys/time.h unistd.h values.h])

##########################################################################################
# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME

##########################################################################################
# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_SETVBUF_REVERSED
AC_FUNC_STAT
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([alarm gethostbyname gettimeofday memset pow select socket sqrt strcasecmp strchr strdup strerror strncasecmp strrchr])

##########################################################################################
#
# Enable Debugging
#
AC_ARG_ENABLE(debug, [  --enable-debug          turn on debugging [default=no]],
	[if eval "test x$enable_debug = xyes"; then
		DEBUGFLAG="-g -D_DEBUG"
		DEBUGNOTE=$DEBUGFLAG
		echo "Enabling debug builds"
	fi])

CFLAGS="$DEBUGFLAG $CFLAGS"
CXXFLAGS="$DEBUGFLAG $CXXFLAGS"
# echo "5 CXXFLAGS=$CXXFLAGS"

##########################################################################################
#
# configure C++ flags
#

# echo "2 CXXFLAGS=$CXXFLAGS"
SAVECXXFLAGS=$CXXFLAGS
AC_PROG_CXX
CXXFLAGS=$SAVECXXFLAGS

# echo "3 CXXFLAGS=$CXXFLAGS"

# DIK - Lifted this section out of the fltk configure.in
dnl Add warnings to compiler switches:
dnl do this last so messing with switches does not break tests
MAKEDEPEND="\$(CXX) -M $OSFLAG"

if test -n "$GXX"; then
    CFLAGS="-Wall $CFLAGS"
	#DIKXXX    CXXFLAGS="-Wall $CXXFLAGS"
    if test -z "$DEBUGFLAG"; then
		#
		# Note: Can't use -fomit-frame-pointer - prevents tools like
		#       libsafe from working!
		#
		#       Don't use -fforce-mem, -fforce-addr, or -fcaller-saves.
		#       They all seem to make either no difference or enlarge
		#       the code by a few hundred bytes.
		#
        CFLAGS="-O2 $CFLAGS"
        CXXFLAGS="-O2 $CXXFLAGS"
    fi
    if test $PICFLAG = 1; then
    	CFLAGS="-fPIC $CFLAGS"
    	CXXFLAGS="-fPIC $CXXFLAGS"
    fi

    # See if GCC supports -fno-exceptions...
    AC_MSG_CHECKING(Testing if GCC supports -fno-exceptions...)
    OLDCFLAGS="$CFLAGS"
    CFLAGS="$CFLAGS -fno-exceptions"
    AC_TRY_COMPILE(,,
	CXXFLAGS="$CXXFLAGS -fno-exceptions"
	AC_MSG_RESULT(yes; disabling them for FLTK...),
	CFLAGS="$OLDCFLAGS"
	AC_MSG_RESULT(no))

    # See if we are running Solaris; if so, try the -fpermissive option...
    if test $uname = SunOS; then
	AC_MSG_CHECKING(Testing if GCC supports -fpermissive...)

	OLDCFLAGS="$CFLAGS"
	CFLAGS="$CFLAGS -fpermissive"
	AC_TRY_COMPILE(,,
	    CXXFLAGS="$CXXFLAGS -fpermissive"
	    AC_MSG_RESULT(yes),
	    CFLAGS="$OLDCFLAGS"
	    AC_MSG_RESULT(no))
    fi
else
    case `(uname) 2>/dev/null` in
        IRIX*)
	    # Running some flavor of IRIX; see which version and
	    # set things up according...
	    if test "$uversion" -ge 62; then
	        # We are running IRIX 6.2 or higher; uncomment the following
		# lines if you don't have IDO 7.2 or higher:
		#
		#     CXX="CC -n32 -mips3"
		#     CC="cc -n32 -mips3"
		#     LD="ld -n32 -mips3"
		#     MAKEDEPEND="CC -M"

		if test "x`grep abi=n32 /etc/compiler.defaults`" = x; then
			AC_MSG_WARN(FOR BEST RESULTS BEFORE COMPILING: setenv SGI_ABI \"-n32 -mips3\")
		fi

        	CFLAGS="-fullwarn $CFLAGS"
        	CXXFLAGS="-fullwarn $CXXFLAGS"
	    else
        	CXXFLAGS="+w +pp $CXXFLAGS"
	    fi
	    if test -z "$DEBUGFLAG"; then
        	CFLAGS="-O2 $CFLAGS"
        	CXXFLAGS="-O2 $CXXFLAGS"
	    fi
	    ;;
	HP-UX*)
	    # Running HP-UX; these options should work for the HP compilers.
	    if test -z "$DEBUGFLAG"; then
		CFLAGS="+O2 $CFLAGS"
		CXXFLAGS="+O2 +W336,501,736,740,749,829 $CXXFLAGS"
	    fi
	    CFLAGS="+DAportable $CFLAGS"
	    CXXFLAGS="+DAportable $CXXFLAGS"
	    ;;
	SunOS*)
	    # Solaris
	    if test -z "$DEBUGFLAG"; then
		CFLAGS="-xO3"
		CXXFLAGS="-xO3"
	    fi
	    if test $PICFLAG = 1; then
		CFLAGS="-KPIC $CFLAGS"
		CXXFLAGS="-PIC $CXXFLAGS"
	    fi
	    ;;
	AIX*)
	    if test -z "$DEBUGFLAG"; then
        	CFLAGS="-O2 $CFLAGS"
        	CXXFLAGS="-O2 $CXXFLAGS"
	    fi
	    echo "WARNING: The AIX C and C++ compilers are known not to correctly"
	    echo "         compile the FLTK library.  We recommend using GCC 2.95.2"
	    echo "         under AIX."
	    ;;
	*)
	    # Running some other operating system; inform the user they
	    # should contribute the necessary options to fltk-bugs@fltk.org...
	    echo "Building FLTK with default compiler optimizations; contact"
	    echo "fltk-bugs@fltk.org with uname and compiler options needed"
	    echo "for your platform, or set the CFLAGS and CXXFLAGS options"
	    echo "before running configure."
	    ;;
    esac
fi

# echo "4 CXXFLAGS=$CXXFLAGS"

#AC_SUBST(LINKFLTK)
AC_SUBST(MAKEDEPEND)
#AC_SUBST(X_EXTRA_LIBS)

##########################################################################################
#
# If we are not CYGWIN, then we must be UNIX (since we're running configure)
#

if test "x$IS_CYGWIN" = "x0"; then
	OSFLAG="-D_UNIX"
fi

# Add the OS define to the flags
CXXFLAGS="$OSFLAG $CXXFLAGS"
CFLAGS="$OSFLAG $CFLAGS"
AC_SUBST(OSFLAG)

##########################################################################################
#
# Silent building (at least quieter)
#
AC_ARG_ENABLE(silent, [  --disable-silent        turn off silent build [default=no]],
	[if eval "test x$enable_silent = xyes"; then
		echo "Silent builds"
	else
		SILENTBUILD=""
		echo "Enabling non-silent builds"
	fi])

AC_SUBST(SILENTBUILD)

##########################################################################################
#
# Sounds of Silence - Server
#
# The default is to build the server with sound support.
# I see no reason why you would *not* want sound support in the server.
# Maybe as a bandwidth saver.
#
SERVERSOUND="-DSOUND"

AC_ARG_ENABLE(sound, [  --disable-server-sound         turn off server sound support [default=no]],
	[if eval "test x$enable_server_sound = xyes"; then
		echo "Server sound support enabled"
	else
		echo "Server sound support disabled"
		SERVERSOUND=""
	fi])

if test ! -z $SERVERSOUND; then
	echo "Server sound support enabled"
	SOUNDNOTE="server "
else
	echo "Server sound support disabled"
fi
AC_SUBST(SERVERSOUND)


##########################################################################################
#
# Sounds of Silence - Client
#
# The default is to build the client with sound support
# if OpenAL is installed on the system.
# You can override this if you choose.
# OpenAL must be installed in the system (so far), if this becomes an issue,
# maybe i'll look at doing a 'local' build like fltk.
#
CLIENTSOUND="-DSOUND"
CLIENTSOUNDLIB="-lopenal"
CLIENTSOUNDCONF="sounds.conf"
CLIENTSOUNDDIR="sounds"

SAVE_CXXFLAGS=$CXXFLAGS
SAVE_LDFLAGS=$LDFLAGS
CXXFLAGS="$CXXFLAGS $CLIENTSOUND"
LDFLAGS="$LDFLAGS $CLIENTSOUNDLIB"

echo $ECHO_N "Try building a small OpenAL program... "
AC_LANG(C++)
AC_TRY_LINK(
	[#include "AL/alut.h"],
	[alutInit(0, 0);],
	[echo "OK"],
	[echo "Failed. Disabling client sound support"
	echo "More info at http://www.buckosoft.com/xpilot/xpilot5/install/"
	CLIENTSOUND=""
	CLIENTSOUNDLIB=""
	CLIENTSOUNDCONF=""
	CLIENTSOUNDDIR=""
	]
)

AC_ARG_ENABLE(sound, [  --disable-client-sound         turn off client sound support [default=no]],
	[if eval "test x$enable_client_sound = xyes"; then
		echo "Client sound support enabled"
	else
		echo "Client sound support disabled"
		CLIENTSOUND=""
		CLIENTSOUNDLIB=""
		CLIENTSOUNDCONF=""
		CLIENTSOUNDDIR=""
	fi])

if test ! -z $CLIENTSOUND; then
	SOUNDNOTE="${SOUNDNOTE}client"
fi
AC_SUBST(CLIENTSOUND)
AC_SUBST(CLIENTSOUNDLIB)
AC_SUBST(CLIENTSOUNDCONF)
AC_SUBST(CLIENTSOUNDDIR)

CXXFLAGS=$SAVE_CXXFLAGS
LDFLAGS=$SAVE_LDFLAGS
##########################################################################################
#
# Server password
#
SRCPASSWDFILE=".serverpassword"
SRCCOOKIEFILE=".servercookie"

if test -f $SRCPASSWDFILE; then
	HASPASSWORD=1
	echo "Have password..."
	MYPASSWORD=`cat $SRCPASSWDFILE | cut -f2 -d':'`
fi

if test "x$MYPASSWORD" = "x"; then
	echo "Using random password..."
	MYPASSWORD=`echo $RANDOM$RANDOM$RANDOM$RANDOM |cut -c1-7`
	echo "Storing password in $SRCPASSWDFILE"
	echo "password:$MYPASSWORD" >$SRCPASSWDFILE
	chmod 600 $SRCPASSWDFILE
	ls -l $SRCPASSWDFILE
fi

if test ! -f $SRCCOOKIEFILE; then
	echo "Creating password cookie file..."
	echo "localhost:15345 $MYPASSWORD" >$SRCCOOKIEFILE
fi

if test -z "$HOME"; then
	echo "No \$HOME directory!"
	exit 1
fi

##########################################################################################
#
# PID directory
#
VARRUND="/var/run"
AC_ARG_ENABLE(varrun, [  --enable-varrun=DIR     put the runtime pid file someplace else (/etc maybe) [default=/var/run]],
	[if eval "test x$enable_varrun != x"; then
		VARRUND="$enable_varrun"
	fi])

VARRUNFLAG="-DVARRUN=\"\"$VARRUND\"\""
AC_SUBST(VARRUND)
echo "Using $VARRUND for runtime pid file"

##########################################################################################
#
# Writable database directory (VARDIR)
#
# By default, this is /var/xpilot.  However, if the user does not have root privileges,
# then they may want to install to say /home/dick/xpilot.  In that case, we set the writable
# dir to /home/dick/xpilot/var.
#

VARDIR="/var/xpilot"

if test x$build_os = xcygwin; then
	VARDIR="${prefix}/var"
else
	if eval "test x${prefix} != xNONE"; then
		VARDIR="${prefix}/var"
	fi
fi
AC_SUBST(VARDIR)
echo "Using $VARDIR for database directory"

##########################################################################################
#
# fltk directory
#
FLTKDIR=""
FLTKINC=""
FLTKLIB="-lfltk"
FLTKIMAGESLIB="-lfltk_images"
IMAGELIBS="-lpng -lz"

AC_ARG_ENABLE(fltkdir, [  --enable-fltkdir=DIR    use a non-system copy of fltk [default=NONE]],
	[if eval "test x$enable_fltkdir != x"; then
		 case $enable_fltkdir in
		   [/]* | ?:[\\/]* ) # Absolute
			  ;;
		   *)
			  enable_fltkdir=`pwd`/$enable_fltkdir ;;
		 esac
		FLTKDIR="$enable_fltkdir"
		FLTKLIB="$FLTKDIR/lib/libfltk.a"
		FLTKIMAGESLIB="$FLTKDIR/lib/libfltk_images.a"
		FLTKINC="-I $FLTKDIR"
		echo "Enabling local fltk dir = $enable_fltkdir"
		dnl check if we had to make libpng.a over there
		if test -f $FLTKDIR/lib/libpng.a ; then
			IMAGELIBS="$FLTKDIR/lib/libpng.a -lz"
			echo "Enabling libpng.a in our fltk/lib"
		fi
	else
		echo "Using system FLTK (better check for it...)"
	fi])

AC_SUBST(FLTKDIR)
AC_SUBST(FLTKINC)
AC_SUBST(FLTKLIB)
AC_SUBST(FLTKIMAGESLIB)
AC_SUBST(IMAGELIBS)

##########################################################################################
#
# Try building an fltk program with a Fl_PNG_Image
#
SAVE_CXXFLAGS=$CXXFLAGS
SAVE_LDFLAGS=$LDFLAGS
CXXFLAGS="$FLTKINC $CXXFLAGS"
LDFLAGS="$LDFLAGS $FLTKLIB $FLTKIMAGESLIB $IMAGELIBS $LDFLAGS"
echo $ECHO_N "Try building a small fltk program... "
AC_LANG(C++)
AC_TRY_LINK(
	[#include "FL/Fl_PNG_Image.H"],
	[Fl_PNG_Image* img;],
	[echo "OK"],
	[echo "Failed. aborting"
	echo "I cannot build an FLTK program using Fl_PNG_Image."
	echo ""
	echo "CXXFLAGS used= $CXXFLAGS"
	echo "LDFLAGS used= $LDFLAGS"
	if test "x$FLTKDIR" = "x"; then
		echo "Perhaps you need to specify the location of your FLTK"
	else
		echo "Perhaps you mis-spelled the location of your FLTK"
	fi
	echo "More info at http://www.buckosoft.com/xpilot/xpilot5/install/"
	echo "Game over"
	exit 1
	]
)
CXXFLAGS=$SAVE_CXXFLAGS
LDFLAGS=$SAVE_LDFLAGS
##########################################################################################
#
# Check for KDE
#
if test x$build_os != xcygwin; then
	INST_KDE=""
	WANTKDE="yes"
	AC_ARG_ENABLE(kde, [  --enable-kde            Install XPilot into KDE Games menu [default=yes]],
		[if eval "test x$enable_kde = xyes" ; then
			WANTKDE="yes"
		else
			WANTKDE="no"
		fi],
		[
			WANTKDE="yes"
		])
	if test x$WANTKDE = xyes; then
		KD=`kde-config --expandvars --install apps 2>/dev/null`
		if test x$KD != x; then
			if test -d $KD/Games; then
				INST_KDE=kde
				MENUKDE=kde/Games
				kdemenu=$KD/Games
				kdedir=`kde-config --prefix`
				echo "Enable KDE menu installation"
			elif test -d $KD/Amusement; then
				INST_KDE=kde
				MENUKDE=kde/Amusement
				kdemenu=$KD/Amusement
				kdedir=`kde-config --prefix`
				echo "Enable KDE menu installation"
			else
				echo "KDE will NOT be installed"
				echo "Directory $KDEDIR/share/applnk/Games NOT FOUND!"
			fi
		else
			echo "\$KDEDIR NOT FOUND!"
		fi
	else
		echo "KDE install skipped"
	fi
	AC_SUBST(INST_KDE)
	AC_SUBST(kdedir)
	AC_SUBST(kdemenu)
fi

##########################################################################################
#
# Check for a browser
#
BROWSER=""
AC_ARG_ENABLE(browser, [  --enable-browser=PATH-TO-EXE    specify the browser to use [default=NONE]],
	[if eval "test x$enable_browser != x"; then
		 case $enable_browser in
		   [/]* | ?:[\\/]* ) # Absolute
			  ;;
		   *)
			  enable_browser=`pwd`/$enable_browser ;;
		 esac
		 BROWSER=$enable_browser
	fi])
if test "x$BROWSER" = x;then
	echo "Try to find a browser to use..."
	for bro in konqi mozilla netscape lynx; do
	case $bro in
		konqi )
			echo $ECHO_N "Konqi? ... "
			# AC_PATH_PROG([INETD], [inetd], [/usr/libexec/inetd],
			#				   [$PATH:/usr/libexec:/usr/sbin:/usr/etc:etc])
			AC_PATH_PROG([KONQI], [konqueror], [],
						 [$PATH])
			if test "x$KONQI" != x; then
				browserTitle="Konqi"
				BROWSER=$KONQI
				break
			fi
			;;
		mozilla )
			echo $ECHO_N "Mozilla? ... "
			echo "(no check yet)"
			;;
		netscape )
			echo $ECHO_N "Netscape? ... "
			AC_PATH_PROG([KONQI], [netscape], [],
						 [$PATH])
			if test "x$KONQI" != x; then
				browserTitle="Netscape"
				BROWSER=$KONQI
				break
			fi
			;;
		lynx )
			echo $ECHO_N "Lynx? ... "
			AC_PATH_PROG([KONQI], [lynx], [],
						 [$PATH])
			if test "x$KONQI" != x; then
				browserTitle="Lynx"
				BROWSER=$KONQI
				break
			fi
			;;
	esac
	done
fi
AC_SUBST(BROWSER)

##########################################################################################
#
# Check for nothing to do in ./inst
#
INST_NONE=""
if test x$INST_KDE = x && test x$INST_WIN32 = x; then
	INST_NONE=none
fi
AC_SUBST(INST_NONE)

##########################################################################################
#
# Make the Makefiles
#
AC_CONFIG_FILES([makeinclude
                 Makefile
                 contrib/Makefile
                 contrib/msub/Makefile
                 doc/Makefile
                 doc/man/Makefile
                 inst/Makefile
                 inst/kde/Makefile
                 inst/win32/Makefile
                 lib/Makefile
                 lib/maps/Makefile
                 lib/scoreserver/Makefile
                 lib/scoreserver/html/Makefile
                 lib/scoreserver/html/g/Makefile
                 lib/sounds/Makefile
                 lib/textures/Makefile
                 src/Makefile
                 src/common/Makefile
                 src/common/expat/Makefile
                 src/common/expat/lib/Makefile
                 src/common/fltk-widgets/Makefile
                 src/common/fltk-widgets/src/Makefile
                 src/common/web/Makefile
                 src/common/xp-flwidgets/Makefile
                 src/common/xp-flwidgets/kbconfig/Makefile
                 src/client/Makefile
                 src/serverlib/Makefile
                 src/server/Makefile
                 src/scoreserver/Makefile
                 src/scoreserver/engines/Makefile
                 src/replay/Makefile
                 src/control/Makefile
                 src/control/buttons/Makefile
                 src/control/res/Makefile
                 ])

# these directories (which have actual source code) need to have at least an empty .depend file
echo $ECHO_N >>contrib/msub/.depend
echo $ECHO_N >>src/common/.depend
echo $ECHO_N >>src/common/expat/.depend
echo $ECHO_N >>src/common/expat/lib/.depend
echo $ECHO_N >>src/common/fltk-widgets/src/.depend
echo $ECHO_N >>src/common/web/.depend
echo $ECHO_N >>src/common/xp-flwidgets/.depend
echo $ECHO_N >>src/common/xp-flwidgets/kbconfig/.depend
echo $ECHO_N >>src/client/.depend
echo $ECHO_N >>src/scoreserver/.depend
echo $ECHO_N >>src/scoreserver/engines/.depend
echo $ECHO_N >>src/serverlib/.depend
echo $ECHO_N >>src/server/.depend
echo $ECHO_N >>src/replay/.depend
echo $ECHO_N >>src/control/.depend
echo $ECHO_N >>src/control/buttons/.depend
echo $ECHO_N >>src/control/res/.depend

# ensure version.h exists this early on in compiling
# (make depend wants to see it)
#echo $ECHO_N >>src/common/version.h

# imake processes these directories.  We will need to process some of these for "make install"
#                 src/common/NT/Makefile
#                 doc/Makefile
#                 doc/man/Makefile

# autoscan suggested these directories.  I don't think we want them.
#
#                 contrib/TkXpInterface/Makefile
#                 contrib/xpmtp/Makefile
#                 contrib/maps2image/Makefile

##########################################################################################
#
# generate config.status and launch it.
#
AC_OUTPUT

##########################################################################################
#
# make the dependencies
#
# I don't have a makedepend on my cygwin. ??
if test x$build_os != xcygwin; then
	make depend
fi

##########################################################################################
#
# Check for server password
#
if test "x$HASPASSWORD" = "x0" ; then
	echo ""
	echo "***************************************************"
	echo "* WARNING!                                        *"
	echo "* You have not set a password for administering   *"
	echo "* your server.                                    *"
	echo "* You have been given a random password.          *"
	echo "* You only need to know this if you want to       *"
	echo "* administer your server from another box         *"
	echo "* (i.e. not 127.0.0.1)                            *"
	echo "*                                                 *"
	echo "* Perhaps you want to run ./config.password       *"
	echo "* and to set the password that will be installed. *"
	echo "* (You should do this before 'make install')      *"
	echo "***************************************************"
fi

# weirdness modeled after openssh
A=`eval echo ${prefix}` ; A=`eval echo ${A}`
B=`eval echo ${bindir}` ; B=`eval echo ${B}`
G=`eval echo ${VARRUND}` ; G=`eval echo ${G}`


# make it a bit more complicated...
# for when we figure out the gnome menu
MENUINST=$MENUKDE$MENUWIN32
if test "x$MENUINST" = "x"; then
	MENUINST="none"
fi

echo ""
echo "========================================================"
echo "XPilot5 has been configured with the following options:"
echo "            Install directory : $A"
echo "        Executables directory : $B"
echo "  Writable database directory : $VARDIR"
echo "                     PID file : $VARRUND   (not used yet)"
echo "                  System Menu : $MENUINST"
#echo "              Server Password : $MYPASSWORD"
echo "                      Browser : $BROWSER"
echo ""
echo "                        Sound : $SOUNDNOTE"
echo "               fltk directory : $FLTKDIR"
echo "                     Compiler : ${CXX}"
echo "               Compiler flags : ${CXXFLAGS}"
echo "                        Debug : $DEBUGNOTE"
