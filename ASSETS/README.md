# Assets #

This folder needs to be present in the directory where the application runs. 

## Bitmap Fonts ##

Fonts are stored in bitmap format and extracted by the application and blitted into individual characters as needed.

There are two font files:

 * white.bmp *- real filename imonlcd_white.bmp*
 * black.bmp *- real filename imonlcd_black.bmp*

These are simply inverted colour versions of each other and are renamed from the original since some platforms (at least Atari ST TOS) are limited to 8 + 3 character file naming standards.

Note that in order to work properly on the Atari TOS port, the fonts had to be converted (in GIMP) to the following format:

 * BMP
 * Indexed, 2 colour black and white (removed unused colour)
 
*For some unknown reason, when left as greyscale or RGB, the second of the font bitmaps would fail to display, though worked correctly on the Linux port. I suspect something to do with limited video memory available to libSDL on the Atari version.*
 