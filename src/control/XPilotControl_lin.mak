CC            = gcc
FLAGS         =
INCLDIR       = -I/usr/local/include -I./fltk-widgets
LIBDIR        = -L/usr/local/lib -L/usr/X11R6/lib
LIBS          = -lX11 -lm -lfltk
DEFINES       =



EXE           = xpilotedit
OBJS          = errors.o file.o tools.o xpilotedit.o gui_map.o gui_main.o \
gui_prefs.o gui_toolkit.o clipboard.o wildmap.o MapDocument.o MapWindow.o ViewWindow.o XpOptionsDocument.o cstring.o
OBJS2          = Fl_Dir.o Fl_File_Dialog.o Fl_Frametab.o Fl_Input_File.o Fl_NSlider.o Fl_Pix_Box.o Fl_Pix_Button.o
SRCS          = errors.cpp file.cpp tools.cpp xpilotedit.cpp gui_map.cpp gui_main.cpp \
gui_prefs.cpp gui_toolkit.cpp clipboard.cpp wildmap.cpp MapDocument.cpp MapWindow.cpp ViewWindow.cpp XpOptionsDocument.cpp ../common/cstring.cpp
INCLUDES      = const.h default_colors.h map.h proto.h xpeTypes.h xpilotedit.h MapDocument.h MapWindow.h ViewWindow.h XpOptionsDocument.h

$(EXE): $(OBJS) $(OBJS2)
	$(CC) $(FLAGS) -o $(EXE) $(OBJS) $(OBJS2) $(INCLDIR) $(LIBDIR) $(LIBS) $(DEFINES)

$(OBJS): $(INCLUDES) $(SRCS)
	$(CC) $(FLAGS) $(INCLDIR) $(DEFINES) -c $(SRCS)

$(OBJS2): $(INCLUDES)
	$(CC) $(FLAGS) $(INCLDIR) $(DEFINES) -c ./fltk-widgets/src/*.cxx

clean:
	rm -f $(OBJS) $(OBJS2) $(EXE)



