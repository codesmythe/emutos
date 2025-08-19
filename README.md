## SBC Branch

This branch contains the code for ports of headless EmuTOS to a few
68000 based single board computers (SBCs). The supported boards
include

- My own [BITSY Mark I](https://github.com/codesmythe/BITSY_MK_I)

- Steve Crompton's [Mega 68K series of boards](https://www.mega-micros.co.uk/index_68030.htm)

- Bill Shen's [Tiny68K Rev 2](https://www.retrobrewcomputers.org/doku.php?id=boards:sbc:tiny68k:tiny68k_rev2)

- The [Rosco M68K Classic V2](https://rosco-m68k.com) board

The features added in this branch relative to upstream EmuTOS include:

- Full support for the MC68681 Dual UART and modern variants like the
  XR68C681.  This includes interrupt-driven receive, flow control,
  `rsconf()`, and `bconmap()` support and the ability to run the DUART at 
  double the usual speed.

- Fixes to support 8-bit IDE interfaces and IDE interfaces that do not
  support software reset.

- Changes to serial console mode to force 80-column mode.

- Support for running the MC68901 MFP at triple speed.

- Support for the TL 16C550 UART as a source of IKBD events. 

- Support for using a I2C DS3231 real-time clock chip, bit-banged
  off MFP GPIO pins.

- Support for using a few Emacs-style control keys in EmuCON 
  for accessing command line history.

- A preliminary attempt at a CMake-based build process.

### Building

To build the ROMs, you need to have the Atari ST MiNT cross tools
installed for your operating system. See
http://vincent.riviere.free.fr/soft/m68k-atari-mint/.  The tools will
need to be on your PATH.

You will also need [CMake](https://cmake.org) and
[Git](https://git-scm.com). Then follow these steps:

```bash
# Clone this repository with SBC ports.
git clone https://github.com/codesmythe/emutos.git

# Move into the emutos directory
cd emutos

# Change to the 'sbc' branch
git checkout sbc

# Create the cmake build directory and go there
mkdir build && cd build

# Run cmake to configure
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain.cmake ..

# Build the BITSY Mark I variant.
# Other valid targets at the moment are `rosco_v2.bin`, `roberts7531.bin` and `mega-68000.bin`.

make bitsy_v1_serial.bin
```

The original EmuTOS README continues below.

## README for EmuTOS

This release has been built on Linux Mint (a Ubuntu derivative), using
Vincent Rivière's GCC 4.6.4 cross-compiler.  The custom tools used in
the build process were built with native GCC 4.8.4.

The source package and other binary packages are available at:
https://sourceforge.net/projects/emutos/files/emutos/1.2.1/

An online manual is available at the following URL:
https://emutos.github.io/manual/

The extras directory (if provided) contains:

1.  One or more alternate desktop icon sets, which you can use to replace
    the builtin ones.  You can use a standard resource editor to see what
    the replacement icons look like. To use a replacement set, move or rename 
    the existing emuicon.rsc & emuicon.def files in the root directory, then 
    copy the files containing the desired icons to the root, and rename them 
    to emuicon.rsc/emuicon.def.

2.  A sample mouse cursor set in a resource (emucurs.rsc/emucurs.def).  This
    set is the same as the builtin ones, but you can use it as a basis to
    create your own mouse cursors. To use a replacement set, copy the files
    containing the desired mouse cursors to the root, and rename them to 
    emucurs.rsc/emucurs.def.

For further information on the above, see [doc/emudesk.txt](doc/emudesk.txt).

If you want to read more about EmuTOS, please take a look at these files:

| File                                         | Description |
|----------------------------------------------|-------------|
| [doc/announce.txt](doc/announce.txt)         | Introduction and general description, including a summary of changes since the previous version |
| [doc/authors.txt](doc/authors.txt)           | A list of the authors of EmuTOS |
| [doc/bugs.txt](doc/bugs.txt)                 | Currently known bugs |
| [doc/changelog.txt](doc/changelogs.txt)      | A summarised list of changes after release 0.9.4 |
| [doc/emudesk.txt](doc/emudesk.txt)           | A brief guide to the newer features of the desktop|
| [doc/incompatible.txt](doc/incompatible.txt) | Programs incompatible with EmuTOS due to program bugs |
| [doc/license.txt](doc/license.txt)           | The FSF General Public License for EmuTOS |
| [doc/status.txt](doc/status.txt)             | What is implemented and running (or not yet) |
| [doc/todo.txt](doc/todo.txt)                 | What should be done in future versions |
| [doc/xhdi.txt](doc/xhdi.txt)                 | Current XHDI implementation status |

Additional information for developers (just in the source archive):

| File                                           | Description |
|------------------------------------------------|-------------|
| [doc/install.txt](doc/install.txt)             | How to build EmuTOS from sources |
| [doc/coding.txt](doc/coding.txt)               | EmuTOS coding standards (never used :-) ) |
| [doc/country.txt](doc/country.txt)             | An overview of i18n issues in EmuTOS |
| [doc/dual_kbd.txt](doc/dual_kbd.txt)           | An explanation of the dual keyboard layout feature |
| [doc/fat16.txt](doc/fat16.txt)                 | Notes on the FAT16 filesystem in EmuTOS |
| [doc/m54xx-cards.txt](doc/m54xx-cards.txt)     | Using CF cards on ColdFire V4e Evaluation Boards |
| [doc/memdetect.txt](doc/memdetected.txt)       | Memory bank detection during EmuTOS startup |
| [doc/nls.txt](doc/nls.txt)                     | How to add a native language or use one |
| [doc/old_changelog.txt](doc/old_changelog.txt) | A summarised list of changes up to & including release 0.9.4 |
| [doc/osmemory.txt](doc/osmemory.txt)           | All about OS internal memory in EmuTOS |
| [doc/reschange.txt](doc/reschange.txt)         | How resolution change works in the desktop |
| [doc/resource.txt](doc/resource.txt)           | Modifying resources in EmuTOS |
| [doc/startup.txt](doc/startup.txt)             | Some notes on the EmuTOS startup sequence |
| [doc/tools.txt](doc/tools.txt)                 | User tools to customise EmuTOS ROM images |
| [doc/tos14fix.txt](doc/tos14fix.txt)           | Lists bugs fixed by TOS 1.04 & their status in EmuTOS |
| [doc/version.txt](doc/version.txt)             | Determining the version of EmuTOS at run-time |

The following documents are principally of historical interest only:

| File                                           | Description |
|------------------------------------------------|-------------|
| [doc/old_code.txt](doc/old_code.txt)           | A museum of bugs due to old C language |
| [doc/vdibind.txt](doc/vdibind.txt)             | Old information on VDI bindings |

-- 
The EmuTOS development team
https://emutos.sourceforge.io/
