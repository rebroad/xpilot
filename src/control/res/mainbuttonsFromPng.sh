#!/bin/bash
# $Id: mainbuttonsFromPng.sh,v 1.2 2004/05/10 12:42:22 dick Exp $
# mainbuttonsFromPng.sh - make separate button bitmaps from a picture
# $Log: mainbuttonsFromPng.sh,v $
# Revision 1.2  2004/05/10 12:42:22  dick
# Add a 5th button (private server), so the others get shuffled a bit.
#
# Revision 1.1  2004/02/28 07:47:08  dick
# A script used to take the povray output png and cut up the individual buttons.
#

# Select U, D, or X output type
UDX=U
pngtopnm mainpanel.png |pnmcut 7  12 190 72 |pnmtopng >mainbut0$UDX.png
pngtopnm mainpanel.png |pnmcut 7  84 190 72 |pnmtopng >mainbut1$UDX.png
pngtopnm mainpanel.png |pnmcut 7 156 190 72 |pnmtopng >mainbut2$UDX.png
pngtopnm mainpanel.png |pnmcut 7 227 190 72 |pnmtopng >mainbut3$UDX.png
pngtopnm mainpanel.png |pnmcut 7 367 190 72 |pnmtopng >mainbut3$UDX.png

