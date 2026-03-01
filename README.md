# Shandalar Decompilation
An attempt at decompiling the 1997 video game [Magic: the Gathering](https://en.wikipedia.org/wiki/Magic:_The_Gathering_(1997_video_game)) (Version 1.3, English). It aims to be as accurate as possible, matching the recompiled instructions to the original machine code as much as possible.

This project takes inspiration from the [LEGO Island Decompilation](https://github.com/isledecomp) project which also targets a game originally released in 1997 built in MSVC 4.20.  The [reccmp](github.com/isledecomp/reccmp) project in particular is crucial for creating a matching binary.

## Source Structure
Within the `src` folder there should eventually be 1 directory for each of the game's 14 binaries (ignoring the 2 MSVCRT dlls).

* cardartlib.dll (debug) ![matching](https://img.shields.io/badge/matching-97.39%25-blue) : Library for decoding the game's card art assets (.cat files) Called `Nedcard` internally?
* cdtools.dll (debug) ![matching](https://img.shields.io/badge/matching-99.05%25-green) : A small library of CD functions
* deck.exe (release /Od) ![matching](https://img.shields.io/badge/matching-99.62%25-green) : A trivial wrapper for showing the deck builder UI
* deckdll.dll (static release /Od?) ![matching](https://img.shields.io/badge/matching-21.14%25-red) : The deck builder UI implementation, imported by deck.exe and magic.exe
* drawcardlib.dll (debug) ![matching](https://img.shields.io/badge/matching-95.98%25-blue) : Library for rendering full card representations
* facemaker.exe : 
* gcconn.dll (debug, C++) : Seems to be netcode for manalink.  Has a ton of asserts with function/filenames embedded.
* magic.exe : The duel UI
* magsnd.dll (release /Od) ![matching](https://img.shields.io/badge/matching-99.15%25-green) : Small dll that wraps DirectSound
* magvid.dll (release C++) : Presumably plays video.  Only referenced by statwin.
* manalink.exe : Multiplayer.  Has a ton of debug logging with function/filenames embedded.  Called `FamiliarWS` internally?
* manalinkinterface.dll (static release /Od) ![matching](https://img.shields.io/badge/matching-100.00%25-green) : Small dll with several exports, used by magic.exe not manalink.exe
* shandalar.exe : The overworld and adventure UI
* statwin.dll (debug?, C++?) : Used by shandalar.exe

Additionally:
* rpbits : Assembly library for decoding part of the Microprose `.pic` format.  Doesn't seem to be representable in C even via `__asm`
* sidlib : C code for decoding `.pic` and `.pcx` which gets included in several binaries.

## Building
See [LEGO Island Decompilation](https://github.com/isledecomp)

A copy of [MSVC 4.20](https://github.com/itsmattkc/MSVC420) and [MASM](https://github.com/qb40/masm) are needed, as well as a recent version of [CMake](https://cmake.org/)

## Ghidra
Within the `ghidra` folder are scripts for aiding decompilation.

See also the [Shandalar abandonware updated with 2015 magic cards](https://github.com/ShandalarMagic/Shandalar) repo.  It contains a heavily modified version of the 1.3 version we're attempting to recreate.  Of particular interest is its `src` folder which contains reimplementations of some of the dlls.

It contains a `Magic-trace.c` which includes an IDA dump of Mok's modified `magic.exe` with a lot of symbol names.  `ImportMagicTraceAndParams.java` is a Ghidra script which attempts to import that. Ghidra's version tracking should make it possible to apply some of the symbol names to the vanilla `magic.exe`.
