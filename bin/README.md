# Binaries

Precompiled binaries for the target platforms will be listed here.

They include:

 * **menu** - A Linux x86_64 executable

 * **menu.tos** - Executable for the Atari ST(E) TOS platform, can be run from the GEM desktop by double clicking on it, but does not depend on the desktop or window system, i.e. it could be placed in the AUTO folder to run at boot time.
 
 * **menu.amiga** - Executable for the Amiga OS platform.
 
 * **menu.amiga.info** - Amiga icon file that allows running menu.amiga from a Workbench window. Copy *both* menu.amiga *and* menu.amiga.info to your Amiga.
 
### Assets

To run the application you must copy the entire `ASSETS` folder to the same location as the executable, this folder contains the bitmaps and fonts that the application needs. 

No other libraries or files need to be present (TOS, Amiga OS or Risc OS).