# Here is Shandalar

Hey all! It's great to see some love for Shandalar here on Reddit. I've been playing this game since it was first released and a veteran of over 100 campaigns. I've noticed many people having issues with procuring a copy and getting it installed, as well as a lack of information as to which version is which and which one does what. It had been some years since I played the game so when I came back to it after upgrading from Win7 to Win10 I had some hurdles as well. First of all, where to find it:

**Microprose Duals of the Planeswalkers**

https://www.myabandonware.com/game/magic-the-gathering-duels-of-the-planeswalkers-9zu

Microprose released the original PC game in 1997 and then released an expansion called *Spells of the Ancients. Duels of the Planeswalkers* was the 1998 re-release of the entire game with the expansion included along with a few other perks, so it's the only one you need. https://MyAbandonWare.com has both rips and CD images for all three, but unless you have some old Windows 98 hardware to run it on I would suggest downloading VMware and using that to install it on Win98 instead of wrestling with WinNT.

**CCGHQ MtG 2010 Custom Installer ISO**

https://www.slightlymagic.net/forum/viewtopic.php?f=76&t=2349

This installer is awesome! Hip63 and the team did a spectacular job. I recently installed this using actual hardware on Win98, Win2K, WinXP, and Win10 using my current desktop PC and an old 800 MHz multi-boot machine that I have lying around. All of them installed wonderfully and worked without a hitch, just don't install it in the default location. I use C:\MtG\MagicTG\, just make sure it's short, simple, and doesn't require administrator privileges to access. This is the true original Shandalar experience, in all of its DOS-glitchy glory. No new cards for Shandalar but this version boasts about 700 new cards for standard duels.

**Shandalar 2012 revisited**

https://archive.org/details/shandalar-2012

Mok is the programmer who released the Manalink 1.3.2 fix for the original Microprose game. In 2011 he released an update for Shandalar.exe that fixed several issues, made it more stable on modern platforms, improve the deck builder, and allowed the use of standard image files instead of proprietary .pic files. I decided to gather up all of the necessary pieces already provided by other helpful forum denizens and used a Windows batch file to stitch it all together into an install I called *Shandalar 2012*. It's the original game just with fewer glitches, nicer graphics, and more card images. Sure it's just fluff, but it's really *nice* fluff. 

It had four releases and then I took it down because it was interfering with people successfully installing the new open version that was being translated by Korath on the SlightlyMagic forums. After Korath pulled his version from release I came back to it, improved the batch file, and released that final version. I've downloaded that file from archive.org and verified that it's the same one I posted in 2018. I've been able to run this install on WinXP, Win7, and Win10... but had to remove and reinstall all of the VC++ redistributables on my Win10 system to get it running.

**Shandalar GemcuttersGuild1 Alpha**

https://github.com/ShandalarMagic/shandalar

This is the final 2016.12.05 release from Korath before everything was pulled from the SlightlyMagic forums. I saw this pop up as the "ProJared" version a couple of weeks ago on Reddit which has sent me down my current rabbit hole. I missed the last year of releases so it was my first time to get a glimpse of what he had done. Fuckin' awesome, if you ask me. That folder on GitHub is a complete mess, though. Over half of its contents are unnecessary garbage. Complete chaos, and Magic.exe doesn't run, but Shandalar.exe does. The original game included 650-ish cards, PlayDeckAnalyser says this version has 13,233 coded cards. Damn Korath, you monster. \^_^

Don't forget you can run random duels and other fun stuff by using the command line. Open a command prompt and type *shandalar.exe --help* for more info.

If the game won't start or crashes intermittently try following this guide to fix the Visual C++ runtime packages:

https://www.technewstoday.com/fix-the-application-was-unable-to-start-correctly-0xc00007b/

https://m.majorgeeks.com/content/page/install_all_the_microsoft_c_runtimes_at_once_with_visual_c_runtime_installer.html

Hopefully this can get stickied so that others who wander into here looking for their old school Microprose Magic the Gathering fix can find it post haste.

[permalink](http://reddit.com/r/Shandalar/comments/1as2cg6/here_is_shandalar/)
by *CirothUngol* (↑ 247/ ↓ 0)