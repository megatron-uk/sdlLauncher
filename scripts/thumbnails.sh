#!/bin/bash

SRC="../dos/games/"

#################################################
#
# This script converts an images found in the SRC
# folder to type that can be loaded by the gfx
# library that the menu launcher code uses.
#
#################################################
find $SRC -type f -print 2>/dev/null | grep -i folder.jpg$ | grep -v folder.bmp | while read JPG
do
	d=`dirname $JPG`
	echo "Convert $JPG -> $d/folder.bmp"
	
	# This is useful for the MS-DOS / Allegro port
	# A basic BMP v3 format
	convert "$JPG" -define bmp:format=bmp3 -resize 400x300 "$d/folder.bmp"
done
