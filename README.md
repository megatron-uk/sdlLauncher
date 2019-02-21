# sdlLauncher #

sdlLauncher is a full-screen game browser/launcher tool for various retro computers. It is intended to run at system boot and present the user with a list of detected games along with screenshots and any supported documentation (readme files included with the game, etc.).

Here is an example of what it looks like:

![Sample menu](docs/menu.png)

... in the example above it is configured in Atari ST / TOS mode, hence is limited to 16 color greyscale.

The inspiration for this is [iGame](http://winterland.no-ip.org/igame/screenshots.html) for the Amiga, for which there is no direct equivalent for the Atari ST or Risc OS platforms.

This is ideal for *the armchair gamer* who does not want to browse the desktop for a game!

### Functionality

 * Automatic enumeration of games in a user-defined drive or directory *- i.e set a directory and the application will search for any games found in a sub-folder*.
 
 * Scrollable list of all available games.
 
 * Automatically load any bitmaps found in the game sub-folder as its cover or screenshot.
 
 * List any .txt files found in the game sub-folder and allow the user to view them without returning to the desktop (e.g. for readme or instruction files).
 
 * Save game list and metadata to .csv file to speed up subsequent application load times.

### Supported Platforms

sdlLauncher was developed on x86 Linux, and is intended to be run on the following platforms:

 * Linux: - *testing/development only*
 
 * Atari TOS: ST(E) / Mega ST(E) -  *TT and Falcon should also work*
 
 * Amiga OS: Amiga 1200/4000 - *Not yet implemented*.
 
 * IBM PC/MS-DOS: Requires i386 cpu - *runs in 32bit protected mode*
 
 * Risc OS: Risc OS 3.6 minimum -  *Not yet implemented*

#### Functionality by Platform

The core functionality of scraping a list of games from one or more directories, browsing them and displaying artwork, launching executables and viewing text files is implemented on all platforms. However, some of the targets have slight differences in functionality (expanded colour modes, background music, etc).

| Platform | Renderer | Video | Game Covers | Control | Background Music |
| ----------- | ------------ | ------- |------------------- | --------- | -------- |
| Linux     | SDL | 320x200x8bpp | Yes, BMP 24/16/15/8bpp | Keyboard, Joystick | MP3, OGG, WAV |
| Atari ST | SDL | 320x200x4bpp | Yes, BMP, 16 shade greyscale | Keyboard | |
| Atari ST | GEM | 320x200x4bpp | Yes, GEM .IMG 16 colour | Keyboard | |
| MS-DOS | Allegro | 320x200x8bpp | Yes, BMP (all depths downsampled to 8bpp) | Keyboard, Joystick | MIDI, FM |

----

### Pre-requisites

sdlLauncher is self-contained and builds a static binary, other than the `ASSETS` folder which contains images and icons and should be placed where your binary runs from, there are *no* external dependencies to run the application.

### Building from Source

[Building the application](BUILDING.md) - Instructions on how to build the application from source for all supported targets.

----


