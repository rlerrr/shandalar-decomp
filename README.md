# Shandalar Decompilation
An attempt at decompiling the 1997 video game [Magic: the Gathering](https://en.wikipedia.org/wiki/Magic:_The_Gathering_(1997_video_game)) (Version 1.3, English). It aims to be as accurate as possible, matching the recompiled instructions to the original machine code as much as possible.

This project takes inspiration from the [LEGO Island Decompilation](https://github.com/isledecomp) project which also targets a game originally released in 1997 built in MSVC 4.20.  The [reccmp](github.com/isledecomp/reccmp) project in particular is crucial for creating a matching binary.

## Source Structure
Within the `src` folder there should eventually be 1 directory for each of the game's 14 binaries (ignoring the 2 MSVCRT dlls).

* cardartlib.dll (debug) : Library for decoding the game's card art assets (.cat files)
* cdtools.dll : A small library of CD functions
* deck.exe : A trivial wrapper for showing the deck builder UI
* deckdll.dll : The deck builder UI implementation
* drawcardlib.dll (debug) : Library for rendering full card representations
* facemaker.exe : 
* gcconn.dll (debug) : 
* magic.exe : The duel UI
* magsnd.dll : 
* magvid.dll : 
* manalink.exe : 
* manalinkinterface.dll :
* shandalar.exe : The overworld and adventure UI
* statwin.dll :

## Ghidra
Within the `ghidra` folder are scripts for aiding decompilation.

See also the [Shandalar abandonware updated with 2015 magic cards](https://github.com/ShandalarMagic/Shandalar) repo.  It contains a heavily modified version of the 1.3 version we're attempting to recreate.  Of particular interest is its `src` folder which contains reimplementations of some of the dlls.

It contains a `Magic-trace.c` which includes an IDA dump of Mok's modified `magic.exe` with a lot of symbol names.  `ImportMagicTraceAndParams.java` is a Ghidra script which attempts to import that. Ghidra's version tracking should make it possible to apply some of the symbol names to the vanilla `magic.exe`.

I don't think decompiling Mok's binary is reasonably possible.  It contains a LOT of functions with nonstandard (possibly Delphi?) calling conventions that cannot be reasonably recreated in C.

## Binary versions

#### MtG 1.0 (Original 1997 release)
* DECK.EXE `md5: B10CD59F0E556FEB19842D919E43B8AB`
* DECKDLL.DLL `md5: A9243D713398FCC58DAB841D59481114`
* DUEL.EXE `md5: 55CF3E15D36334D3440E90F44F053301`
* MAGIC.EXE `md5: CE37687D2B2AE08B03C66406C8C03BAF`

#### MtG 2.0 (Spells of the Ancients)
* cardartlib.dll `md5: C39E42700F227CF3396545346DF4D6BC`
* CdTools.dll `md5: 87AADFA5C287890434057C4B06B6C595`
* DECK.EXE `md5: 2551E5F72A7B776BC7FB23A13753C19F`
* deckdll.dll `md5: 2A98DE0019D9A74A29944C88680BBC30`
* drawcardlib.dll `md5: 859B99055769D6C5DB9E08E52EA821AA`
* FaceMaker.exe `md5: ECFA59CBB99E4A93903BFA35E39A4C86`
* magic.exe `md5: 53D95542AF9E11D67C6F1E06993E4B91`
* shandalar.exe `md5: 0037214823A351106F5AB325CA64BAFD`

#### Mtg 3.0 (Duels of the Planeswalkers)
* cardartlib.dll `md5: CE6AEA0543AAC4256BA59014E6CFDA20`
* CdTools.dll `md5: C8E9907DFC1EA5D9496BAEC89B5B51EA`
* DECK.EXE `md5: 2551E5F72A7B776BC7FB23A13753C19F`
* deckdll.dll `md5: 7BC92F35CA89A65A6646227B9ADE5640`
* drawcardlib.dll `md5: 3192FCE20348CA6D1E5562B43B7F41C1`
* FaceMaker.exe `md5: ECFA59CBB99E4A93903BFA35E39A4C86`
* magic.exe `md5: 40A3036B82A67EE6752D80DC48F52E30`
* Manalink.exe `md5: E79AB6D24D77E0499F6074DFAC58BBB8`
* ManalinkInterface.dll `md5: 6F0E05D1B9DB060D9566D3CBBADC7303`
* shandalar.exe `md5: 1535EA5DFACD0A16231839244C27358C`
* gcconn.dll `md5: 613b1d750c8f6156fc703664775146e7`
* MAGSND.DLL `md5: 5a8e0dea98be58bba6d1d0ecf73cf952`
* MAGVID.DLL `md5: 5a6bd7bb72db815210e1e575b1c19eb3`
* STATWIN.DLL `MD5: 7d4310510608ea0d91b2b1feab93a4cd`

#### Manalink 1.3 (Patched via mtg_13us.exe)
* cardartlib.dll `md5: C0D4000DD51613A213DEAD84E92911F8`
* CdTools.dll `md5: 646B6AD6F55171D43B3B2860BA69BF78`
* DECK.EXE `md5: 2551E5F72A7B776BC7FB23A13753C19F`
* deckdll.dll `md5: 4D06BF82D9CE5727ADF89FF7D1D4102C`
* drawcardlib.dll `md5: 9BA848FEEC8FECC7C2FF910D42BB09B6`
* FaceMaker.exe `md5: ECFA59CBB99E4A93903BFA35E39A4C86`
* magic.exe `md5: 0C1FA7752CEEB84A09F4440916219F72`
* Manalink.exe `md5: DAC6D638FA91C049913B7861DAEEE3E9`
* ManalinkInterface.dll `md5: 3980049A8E5B8EC7AD39CF89810283D5`
* shandalar.exe `md5: 4CED8EC9392BE23AF6670E28ECB373DE`

#### Manalink 1.3 (From mtg_13us.zip)
* shandalar.exe `md5: CED8EC9392BE23AF6670E28ECB373DE`