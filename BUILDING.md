## Building sdlLauncher

Pre-built binaries for all target platforms can be found in the [bin/](bin/) folder, but it is possible to build using standard GCC cross compiler toolchains on modern Linux systems.

### Compiling from source

Instructions for each target platform are listed below. In all cases a cross compiler is used. Using native ports of GCC to build the application has *not* been tested.

### Native Linux

The following libraries are needed to build sdlLauncher from source on Linux and other unix-like platforms (including Cygwin):

 * libsdl1.2
 * GNU libc

We don't use libsdl2 since it does not have support on many of the older platforms this application is intended to run on; there are many more versions of libsdl1.2 available.

Linux platforms can run the shell script `scripts/build.unix` to compile the application.

If you need to adjust any library or include paths, please edit the file `Makefile.unix`.

A successful compile will generate the executable file `menu`.

----

### Amiga OS

Compiling for the AmigaOS m68k target is supported using standard GNU cross-compiler tools.

You will need the full amiga-gcc toolchain as maintained at https://github.com/bebbo/amiga-gcc. This includes GCC, binutils as well as various standard C library implementations.

The components needed are:

 * GCC - [https://github.com/bebbo/amiga-gcc](currently 6.2) for AmigaOS targets
 * binutils
 
In addition, there are various AmigaOS C library implementations, these include:

 * Newlib
 * libnix
 * clib2
 
 You also need libSDL, of course. Currently there are several versions of this floating around, altered by different developers. The one we need has AGA driver support, rather than only RTG driver support.
 
 * *TO DO - Add link to libSDL with AGA support*
 
 Currently all of the functionality of the *sdlLauncher* application is catered for by the libnix library.
 
 The differences in the C library implementations are listed on the [https://github.com/bebbo/amiga-gcc/wiki/Libraries](amiga-gcc wiki).

#### Pre-built Cross Compiler binaries

A pre-built set of Amiga OS cross compile toolchain binaries, as well as libSDL for Amiga OS targets that will run on Linux x68 can be found in the [tools/AmigaOS](tools/AmigaOS) sub directory.

Extract the archive to the default location of `/opt/amiga`.

#### Building

Run the shell script `scripts/build.amiga` to compile the application.

The makefile `Makefile.amiga` contains paths to the cross compiler and libraries listed above. If you've installed them to a different location to standard (which is `/opt/amiga`), make sure to edit the makefile.

Compiling using native tools on Amiga OS itself is *untested*.

#### Running

A successful build will generate a binary named `menu.amiga` which can be run from either Amiga Workbench or from an Amiga shell.

----

### Atari ST / TOS

The Atari ST target has two graphics options:

 * SDL
 * GEM
 
 The SDL target works the same as all other platforms and has no reliance on the GEM desktop. It is, however, *slow*, since all graphics are translated from PC/chunky to Atari/planar formats.
 
 The GEM target is faster, but requires that the images for game covers be converted to Atari .IMG format in advance.

Either option can be built at compile time by setting the USE_GEM or USE_SDL compiler flags.

Compiling for the Atari ST / TOS m68k target is supported using standard GNU cross-compiler toolchains.

You will need the full *m68k-atari-mint-** set of GNU tools as distributed from http//tho-otto.de. This includes:

 * [gcc & libc](http://tho-otto.de/crossmint.php) - currently 8.2.x
 * [binutils](http://tho-otto.de/crossmint.php)

In addition, you need the following `m68k-atari-mint` library to resolve several TOS/GEM symbols:

 * [GEMlib](http://tho-otto.de/crossmint.php)

libSDL 1.2 for `m68k-atari-mint` targets can be found (including a number of bug fixes) from Patrice Mandin:

 * [libSDL 1.2.15](http://pmandin.atari.org/en/index.php?post/2015/11/01/121-en-ports-libs-sdl) - this must be, at a minimum, the *SDL-devel-1.2.15-mint-20190113* release, as earlier versions have a bug in the display code

Download and install all of these (which defaults to `/usr/m68k-atari-mint/`). You can then run the shell script `scripts/build.tos` to compile the application.

#### Pre-built Cross Compiler binaries

A pre-built set of Atari ST cross compiler toolchain binaries as well as the patched libSDL for Atari that will run on Linux x64 can be found in the [tools/AtariTOS](tools/AtariTOS) sub directory.

This archive is a collection of all the packages from http//tho-otto.de, listed above.

Simply unpack the archive, it will extract to `/usr/m68k-atari-mint`. The binaries are not using standard GCC naming convention triplets; i.e. `m68k-atari-mint-gcc`, `m68k-atari-mint-strip`, etc, but short names as `gcc` and `strip`, so be sure *not* to add the bin directory to your path.

#### Building

Run the shell script `scripts/build.amiga` to compile the application.

The makefile `Makefile.tos` contains paths to the cross compiler and libraries listed above. If you've installed them to a different location, make sure to edit the makefile.

Compiling using native tools on TOS itself is *untested*.

#### Running

A successful build will generate a binary named `menu.tos` which can be run from the Atari GEM desktop, or (preferably) dropped into the `AUTO` folder to automatically run on system boot.

The application does *not* need the GEM desktop running as it uses almost no library calls to GEM itself; hence adding it to the *AUTO* folder is a good idea for an auto-booting games playing Atari ST.

----

### Risc OS

*Not yet implemented*