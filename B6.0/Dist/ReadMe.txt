  _______ __   __ ______   ______   _______ _______ ___      ___     _   ___ 
 |       |  | |  |    _ | |    _ | |       |   _   |   |    |   |   | | |   |
 |  _____|  | |  |   | || |   | || |    ___|  |_|  |   |    |   |___| |_|   |
 | |_____|  |_|  |   |_||_|   |_||_|   |___|       |   |    |    _  |       |
 |_____  |       |    __  |    __  |    ___|       |   |___ |   | | |___    |
  _____| |       |   |  | |   |  | |   |___|   _   |       ||   |_| |   |   |
 |_______|_______|___|  |_|___|  |_|_______|__| |__|_______||_______|   |___|
 
 .:Community Edition:.


******************************************************************************
*                                                                            *
*                          .:~ Surreal64CE B6.0 ~:.                          *
*                                                                            *
******************************************************************************
                                                                              
******************************************************************************
*   Programmer :.......weinerschnitzel * Language :...C/C++,ASM,DirectX8.1   *
*   Version :.....................B6.0 * Size :..........147,316,923 bytes   *
*   Date :....................12/31/14 * Type :...............N64 Emulator   *
*   Group :........http://EmuXtras.net * OS :...............Microsoft Xbox   *
******************************************************************************
*                           http://surreal64ce.sf.net                        *
******************************************************************************
                                                                              
******************************************************************************
*....:Changes/Additions:.....................................................*
******************************************************************************
*                                                                            *
*    --Core Updates--                                                        *
*    *1964 1.1 added                                                         *
*    -Starting in 2009, freakdave, Ez0n3 and schibo all gave their           *
*    efforts to porting this to Xbox. I only finished what they started.     *
*    *PJ64 1.6 added                                                         *
*    -Before the leak gained publicity in 2012, Mudlord and Squarepusher     *
*    shared their discovery with me and the xbox port happened shortly       *
*    after.                                                                  *
*    *UltraHLE updated to match UHLE Alpha codebase                          *
*    -UHLE Alpha uses some larger buffers for sound and textures, and has    *
*    very slight changes to syncing.                                         *
*                                                                            *
*    --RSP Plugin Updates--                                                  *
*    *RSP Recompiler updated to 1.7.0.9                                      *
*    *HLE RSP Plugin updated with MusyX HLE                                  *
*    -MusyX will require use of "AudioMusyX" until a proper HLE audio        *
*    plugin is ported.                                                       *
*                                                                            *
*    --Video Plugin Updates--                                                *
*    *Rice Video 6.12 updated with several changes from mainline and some    *
*    backport fixes from Rice 5.60.                                          *
*    -Near Plane Clipping hack for Zelda                                     *
*    -HW vertex handling from Rice Video 5.60                                *
*    -Pixel Shader combiner switched to PC version (faster and complete)     *
*    -Software Vertex Clipper as an option for corner cases                  *
*    -Glide64 LOD fix for correct fog depth                                  *
*    -Moveword insert matrix fix for SSB                                     *
*    -Various renames/refactors and other less noticeable bug fixes          *
*                                                                            *
*    --Input Plugin Updates--                                                *
*    *Deadzone threshold decreased for better button-to-analog mapping       *
*                                                                            *
*                                                                            *
******************************************************************************
                                                                              
******************************************************************************
*....:Frequently Asked Questions:............................................*
******************************************************************************
*                                                                            *
*    Q: What does Range Fog and Linear Fog mean?                             *
*    A: Range Fog is the default fog method and usually correct method       *
*    fog. Linear Fog refers to depth based fog on a linear filter. It is     *
*    less costly, but requires the Software Vertex Clipper, which itself     *
*    uses more resources than the old hardware method. Linear Fog can        *
*    create a more visually appealing thickness of fog, but some less        *
*    desirable artifacts may appear. Rice 6.12 only.                         *
*                                                                            *
*    Q: Software Vertex Clipper, what?                                       *
*    A: The Software Vertex Clipper uses code that also relies on DirectX    *
*    features unsupported by XBOX. You can witness the missing side          *
*    textures on treasure chests in Legend of Zelda. The rest of the         *
*    functionality is enough to work, and it also corrects some bad          *
*    vertex clipping seen in Conker's BFD in the level "Enter the Vertex."   *
*    You can enable the Software Vertex Clipper by selecting "Software       *
*    Vertex Processing" in Video Settings. Rice 6.12 only.                   *
*                                                                            *
*    Q: 1964 1.1 and PJ64 1.6 support cheats, what gives?                    *
*    A: I never implemented a menu scheme to modify and activate cheats.     *
*    In the meantime, PJ64 1.6 can load "permanent cheats" that correct      *
*    some big issues. (i.e. Zelda SubScreen Delay Fix, and Pokemon Snap)     *
*                                                                            *
*    Q: I am experiencing graphics glitches in X game.                       *
*    A: That's not really helpful. :/ The most common graphics issues        *
*    result from Rice Video's lack of [Hardware] Framebuffer Emulation.      *
*    Garbled textures, missing effects, bad vertex clipping, and black/      *
*    missing textures can be a result of a plugin incompatibility rather     *
*    than a port incompatibility. Check your PC for compatibility with       *
*    the closest Rice Video plugin to determine if the bug is in fact a      *
*    regression or something that can be fixed. Next, you can do some        *
*    research to find out what type of bug you are encountering and if it    *
*    can be fixed. Emutalk.net forums and BigHead's compaitibility list      *
*    are helpful here. Gonetz' GLideN64 dev blog has great explanations      *
*    for N64 HLE graphics problems here: http://gliden64.blogspot.com/       *
*    After that, report the game, tell me exactly whats wrong, what          *
*    emulator and plugins you're using, include a screenshot, and I might    *
*    look into fixing it. Otherwise... Don't bother! :P                      *
*                                                                            *
*    Q: I am experiencing a crash after X in Y game.                         *
*    A: Make sure you are giving the emulator enough RAM, and try setting    *
*    Texture Memory to AUTO. Some games handle the auto texture memory       *
*    trick better than others. If crashes persist, report the game,          *
*    emulator & plugins, how to reproduce the crash, and any combination     *
*    of plugins that worked in the past.                                     *
*                                                                            *
*    Q: PJ64 1.6 doesn't play opening animations all of the time. Why?       *
*    A: There are some changes to PJ64 where previous optimizations don't    *
*    work as gracefully. The result is that some opening animations get      *
*    skipped at the cost of good emulation speed.                            *
*                                                                            *
*    Q: Turok Dinosaur Hunter doesn't work or crashes on me. Why?            *
*    A: PJ64 1.6 won't boot it due to optimization issues. 1964 1.1 will     *
*    only boot it with audioMusyX. Both emulators will eventually crash,     *
*    either randomly, or after the first key is recovered. Mupen64 would     *
*    be needed to play this game fully.                                      *
*                                                                            *
*    Q: Players 2 and 4 are unplayable in Conker's BFD deathmatch.           *
*    A: Sorry. Again, this is a core issue.                                  *
*                                                                            *
*    Q: Can I tweak the speed of a game?                                     *
*    A: Yes. Adjust the Counter Factor value in the emulator's .ini or       *
*    .rdb. A lower CF value emulates closer to a real N64, while a higher    *
*    CF value will create a better experience on old hardware like the       *
*    Xbox. Generally, adjusting CF will do a better job than Frameskip.      *
*                                                                            *
*    Q: Load/Save States as well as the In Game Menu cause me crashes.       *
*    A: Yes, these bugs have been around since Surreal64 1.0. They are a     *
*    result of the emulator and plugins running natively with the lower      *
*    level xbox functionality written into various parts of Surreal64.       *
*    Most emulators for the Xbox include a simple App that the emulator      *
*    is built ontop of.with Surreal64, when a problem occurs, the            *
*    interface becomes inaccessible.                                         *
*                                                                            *
*                                                                            *
******************************************************************************
                                                                              
******************************************************************************
*....:Greets:................................................................*
******************************************************************************
*                                                                            *
*    First, a hello to my old teammates Ez0n3 and freakdave for the hours    *
*    of fun spent hacking away at Surreal64 when we all had time for it.     *
*                                                                            *
*    Next, a very special thanks to the emulator developers that have        *
*    given me direct help and support:                                       *
*                                                                            *
*    Schibo ( 1964 author )                                                  *
*    Azimer ( HLE audio plugin author )                                      *
*    death-droid ( Rice Video plugin maintainer )                            *
*    Salvy ( DaedalusX64 developer )                                         *
*    Mudlord & SquarePusher ( for sharing hcs' leak of PJ64 1.6 )            *
*    ratop46 ( Rice Video bug fixes )                                        *
*    X-Scale ( CIC-NUS-6105 reverse engineering and Pif2.dat fixes )         *
*                                                                            *
*    I am also willingly obligated to thank the authors of the software      *
*    herein:                                                                 *
*                                                                            *
*    Schibo ( again )                                                        *
*    Rice                                                                    *
*    Zilmar                                                                  *
*    Jabo                                                                    *
*    Gent                                                                    *
*    Smiff                                                                   *
*    Witten                                                                  *
*    Azimer ( again )                                                        *
*    JttL                                                                    *
*    bobby.smiles32                                                          *
*    Hacktarux                                                               *
*    StrmnNrmn                                                               *
*                                                                            *
*    Of course, I have to thank oDD and Lantus... GogoAckman, too! :P        *
*                                                                            *
*    N64 Greets to...                                                        *
*    emu_kidid, Salvy, Wally, gonetz, Narann                                 *
*                                                                            *
*    Xbox Greets to...                                                       *
*    Madmab, hcf, gamezfan, FrankMorris, Neil222, cbagy, waal, Wimpy,        *
*    Perr, NeoBomb, Bigby, MMQ, everyone who has taken enough interest in    *
*    Surreal64 to give feedback, and the other current developers and        *
*    artists at EmuXtras.net. I could get rather impersonal with this        *
*    list, but I'll quit while I'm ahead. Sorry if I forgot you!             *
*                                                                            *
*                                                                            *
******************************************************************************
