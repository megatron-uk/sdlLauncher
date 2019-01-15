#!/bin/bash

SRC="./GAMES/"

find $SRC -type f -print 2>/dev/null | grep -i .bmp$ | while read BMP
do
	d=`dirname $BMP`
	echo "Convert $BMP -> $d/FOLDER.BMP"
	convert "$BMP" -resize 180x135 -set colorspace Gray -separate -average -dither FloydSteinberg "$d/FOLDER.BMP"
done