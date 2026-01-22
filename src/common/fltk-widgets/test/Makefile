SHELL=/bin/sh

ALL =	filedialog frametab pixbutton newslider
all:	${ALL}
include ../../fltk/makeinclude

.SUFFIXES : .C .cxx .c .o .do

filedialog: filedialog.cxx
	${CXX} ${CXXFLAGS} -I../../fltk -I../../fltk-widgets filedialog.cxx ../src/Fl_Dir.cxx ../src/Fl_File_Dialog.cxx ../src/Fl_Input_File.cxx -L../../fltk/lib -lfltk ${LDLIBS} -o $@

frametab: frametab.cxx
	${CXX} ${CXXFLAGS} -I../../fltk -I../../fltk-widgets frametab.cxx ../src/Fl_Frametab.cxx -L../../fltk/lib -lfltk ${LDLIBS} -o $@

pixbutton: pixbutton.cxx
	${CXX} ${CXXFLAGS} -I../../fltk -I../../fltk-widgets pixbutton.cxx ../src/Fl_Pix_Box.cxx ../src/Fl_Pix_Button.cxx -L../../fltk/lib -lfltk ${LDLIBS} -o $@

slider: slider.cxx
	${CXX} ${CXXFLAGS}  -I../../fltk -I../../fltk-widgets slider.cxx ../src/Fl_NSlider.cxx -L../../fltk/lib -lfltk ${LDLIBS} -o $@

testgr: testgr.cxx
	${CXX} ${CXXFLAGS} -I../../fltk -I../../fltk-widgets testgr.cxx -L../../fltk/lib -lfltk ${LDLIBS} -o $@

clean:
	-rm -f ${ALL}  *~
