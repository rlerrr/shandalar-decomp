# Shandalar Decompilation
An attempt at decompiling the 1997 video game [Magic: the Gathering](https://en.wikipedia.org/wiki/Magic:_The_Gathering_(1997_video_game)) by MicroProse (Version 1.3, English). It aims to be as accurate as possible, matching the recompiled instructions to the original machine code as much as possible.

This repository does **not** contain any game assets. An existing copy of the game is required.  Since it aims to match the original as much as possible, modded assets will probably **not** work.

This project takes inspiration from the [LEGO Island Decompilation](https://github.com/isledecomp) project which also targets a game originally released in 1997 and built using the same compiler.

For the same reasons as in LEGO Islend, these binaries are not a byte-for-byte match of the original executables.  See the matching % for each binary below.  Those marked in green are thought to be effectively identical.

A lot has been decompiled with AI assistance.  Although modern LLMs are very good at decompilation, they still struggle at **matching** decompilation, so a lot of human effort is still required.

## Source Structure
Within the `src` folder there should eventually be 1 directory for each of the game's 14 binaries (ignoring the 2 MSVCRT dlls).

* cardartlib.dll (debug) ![matching](https://img.shields.io/badge/matching-98.23%25-green) : Library for decoding the game's card art assets (.cat files) Called `Nedcard` internally?
* cdtools.dll (debug) ![matching](https://img.shields.io/badge/matching-99.05%25-green) : A small library of CD functions
* deck.exe (release /Od) ![matching](https://img.shields.io/badge/matching-99.62%25-green) : A trivial wrapper for showing the deck builder UI
* deckdll.dll (release /Od) ![matching](https://img.shields.io/badge/matching-97.65%25-green) : The deck builder UI implementation, imported by deck.exe, magic.exe and shandalar.exe
* drawcardlib.dll (debug) ![matching](https://img.shields.io/badge/matching-98.32%25-green) : Library for rendering full card representations
* facemaker.exe (release partially optimized) ![matching](https://img.shields.io/badge/matching-77.41%25-green): UI for designing your character.  Invoked via shandalar.exe with shared memory space.  Shares most of its code with shandalar.exe
* gcconn.dll (debug, C++) : Seems to be netcode for manalink.  Has a ton of asserts with function/filenames embedded.
* magic.exe ![matching](https://img.shields.io/badge/matching-81.97%25-orange): The duel UI.  Shares a tremendous amount of code with shandalar.exe
* magsnd.dll (release /Od) ![matching](https://img.shields.io/badge/matching-99.15%25-green) : Small dll that wraps DirectSound
* magvid.dll (release C++) : Presumably plays video.  Only referenced by statwin.
* manalink.exe : Multiplayer.  Has a ton of debug logging with function/filenames embedded.  Called `FamiliarWS` internally?
* manalinkinterface.dll (static release /Od) ![matching](https://img.shields.io/badge/matching-100.00%25-green) : Small dll with several exports, used by magic.exe not manalink.exe
* shandalar.exe ![matching](https://img.shields.io/badge/matching-85.68%25-blue): The main game.
* statwin.dll (release /Od, C++) ![matching](https://img.shields.io/badge/matching-98.65%25-green): Renders shandalar wiz stats screen and passes through magvid.dll calls

Additionally:
* rpbits : Assembly static library for decoding part of the MicroProse `.pic` format.  Doesn't seem to be representable in C even via `__asm`

## Building

A copy of [MSVC 4.20](https://github.com/itsmattkc/MSVC420) and [MASM](https://github.com/qb40/masm), as well as a recent version of [CMake](https://cmake.org/) are required for generating the closest possible match to the original binaries.  See [LEGO Island Decompilation](https://github.com/isledecomp/isle) for more detailed instructions on building with CMake + MSVC 4.20.

Modern Visual Studio is also supported (tested with 2026 Community).  Some fixes are automatically applied for running on modern Windows.  To build with these fixes while still targeting MSVC 4.20, invoke CMake with the `-DSHANDALAR_MODERN_FIXES=ON` flag.

 See `MODERN_FIXES` flag use in code for what that means precisely.  MicroProse made several assumptions that only worked for Windows 9X.  These changes only make the game runnable, not fix other bugs.