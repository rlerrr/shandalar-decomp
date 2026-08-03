# SDL3 and Emscripten Port Plan

Created: 2026-06-04

This is a staged plan for converting a post-fork, post-matching Shandalar
codebase into a portable SDL3 app with an Emscripten/WebAssembly target. It
assumes the fork has intentionally dropped the MSVC 4.20/reccmp matching goals,
the matching-only stack-layout cruft, and the original one-binary-per-Windows-
module build shape where that shape gets in the way.

Use [PREFORK_SDL3_EMSCRIPTEN_CHECKLIST.md](PREFORK_SDL3_EMSCRIPTEN_CHECKLIST.md)
for preparation that can still be done in the current matching repo without
creating excessive `#ifdef` noise.

## Starting Shape

The pre-fork repo currently builds several original-style Windows binaries:

- `facemaker.exe`: small standalone UI for character portrait creation. In the
  post-fork plan this stops being a separate executable and becomes a Shandalar
  module/state.
- `shandalar.exe`: main adventure game. Its CMake target already compiles most
  Facemaker sources, most Magic sources, and selected DeckDLL sources directly.
- `magic.exe`: Magic shell and duel UI. This is the largest UI surface.
- `deck.exe` and `deckdll.dll`: deck builder wrapper and UI implementation.
- `drawcardlib.dll`, `cardartlib.dll`, `magsnd.dll`, `cdtools.dll`,
  `manalinkinterface.dll`: rendering, asset, sound, and support libraries.
- `rpbits`: MASM static library used by `.pic` decoding. The post-fork codebase
  should use a portable C implementation instead.

Important code anchors from the current tree:

- Facemaker has a Win32 message pump in `src/facemaker/src/main.c` and starts
  `FaceMakerWorkerThread` via `CreateThread`.
- Facemaker's drawing layer is centered around `DIBSurface`, `HDC`,
  `HBITMAP`, palettes, DIB sections, and named file mappings.
- Shandalar starts Facemaker through `_spawnl(..., "Facemaker.exe", "/S", 0)`
  in `RunFacemakerFlow()` and then reuses Facemaker-created page data.
- Shandalar's main entry point uses Win32 windows, `timeSetEvent`, a loader
  thread, a `GetMessage` loop, GDI palettes, and blocking input helpers such as
  `WaitForInputEvent()`.
- Shandalar launches the duel path on a separate thread in `RunDuelEngine()`
  and waits synchronously with `WaitForSingleObject`.
- Magic has many WndProc-driven virtual UI components and modal dialogs. It
  also starts a Manalink monitor thread and uses Win32 mutex/event objects.
- `rpbits` is MASM and cannot be used as-is for WebAssembly.

## Browser and SDL3 Constraints

The SDL3 Emscripten guidance creates the main architectural constraints:

- The app cannot own the main thread with a permanent `while` loop. It needs
  SDL main callbacks or an Emscripten main loop that returns quickly.
- Rendering must happen on the main thread.
- Threads are possible in browsers but complicate deployment and require
  COOP/COEP headers. The initial WebAssembly target should not require pthreads.
- UI work should be event-driven. Blocking modal flows, busy waits, `Sleep`,
  `DialogBox`, and "wait until input" loops need to become resumable states.
- `SDL_RenderPresent()` will not visibly present until the main loop iteration
  returns.
- Web audio is muted until the user interacts with the page.
- The web filesystem is not a Windows filesystem. Bundled assets should be
  preloaded or embedded, and saves/preferences should live under a persistent
  path such as SDL's Emscripten IDBFS mount.

Reference:
https://raw.githubusercontent.com/libsdl-org/sdlwiki/refs/heads/main/SDL3/README-emscripten.md

## Post-Fork Assumptions

- Matching is no longer a constraint. The code can be renamed, split, flattened,
  and mechanically simplified when doing so improves portability.
- The MSVC 4.20 C89 restriction is gone for the port fork. Pick one modern
  compiler baseline, ideally Clang for native builds and Emscripten for web.
- The matching-only `struct { ... } s;` local-grouping pattern can be removed as
  code is touched. Do this opportunistically, not as a giant unrelated rewrite.
- Facemaker is not maintained as a separate `.exe` plus `.lib` pair. Its code is
  folded into the Shandalar app as a modal flow/module.
- Windows DLL boundaries are not sacred. `cardartlib`, `drawcardlib`,
  `deckdll`, `magsnd`, and `manalinkinterface` can become ordinary modules or
  libraries inside one app.
- The target product is one SDL application with internal game states, not a
  recreation of the original process graph.
- The default web target is single-threaded. Threads can be revisited later as a
  separate deployment choice.

## Porting Principles

1. Be willing to delete compatibility scaffolding after the fork.

   The fork should not preserve old CMake/reccmp/MSVC paths unless they still
   serve development. Avoid a permanent maze of Win32-vs-SDL branches.

2. Build a platform layer before replacing screens.

   Introduce a small boundary for windowing, input, timing, files, audio, and
   presentation. The SDL implementation should become the main backend, not an
   optional alternate path buried under preprocessor checks.

3. Model old screens as state machines.

   Functions that currently block waiting for input should be converted into
   states with `enter`, `event`, `tick`, `render`, and `leave` behavior. A
   modal flow stack is likely enough for Facemaker and Shandalar menus. Magic's
   duel UI will need a richer virtual window/message system later.

4. Keep graphics data first, presentation second.

   Most adventure UI code works on indexed 8bpp pixel buffers plus palettes.
   Preserve that model in portable code, then upload the active page to an SDL
   texture for presentation. Do not start by trying to convert all drawing to
   immediate SDL rendering calls.

5. Avoid pthreads for the first web target.

   Worker threads in Facemaker, Shandalar, Magic, and Manalink should be
   replaced by main-loop work scheduling or single-thread command queues. If a
   later build adds pthreads, it should be a separate explicit target.

6. Disable or stub networked Manalink early.

   Browser networking will require a new WebSocket or web service layer. It
   should not block the single-player WebAssembly path.

## Phase 0: Build and Portability Baseline

Goal: turn the fork into a modern, portable app workspace.

Tasks:

- Remove reccmp configuration, function-address comments where they no longer
  help, MSVC 4.20-specific CMake paths, `.def` export requirements, Windows
  resource-only build paths, and matching-only target names.
- Decide the new build system shape:
  - one SDL application target,
  - ordinary internal modules for Facemaker, Shandalar, Magic, Deck, card art,
    draw-card, and audio,
  - optional small test tools for asset decoding and rendering.
- Decide whether the app stays C or moves to C++. C is viable, but C++ may make
  state-machine, ownership, and virtual-window work less painful.
- Decide how SDL3 is supplied:
  - system `find_package(SDL3)` for native development,
  - Emscripten-built SDL3 for web,
  - optional vendored SDL source only if needed.
- Define source boundaries:
  - no `<windows.h>` in portable game modules,
  - platform headers live in one place,
  - Win32-only diagnostic tools are isolated or removed.
- Define an asset root that works on Windows and web. Avoid absolute paths and
  drive-letter assumptions.
- Create a web file layout plan:
  - preload read-only game assets into MEMFS,
  - mount persistent data under `/storage`,
  - place saves, decks, options, and `PlayFace` output under SDL pref paths or
    a stable subdirectory of `/storage`.
- Inventory filename case issues because Emscripten paths are case-sensitive.
- Promote the portable `rpbits_c` implementation to the only `.pic` decode path.

Exit criteria:

- One native Clang build initializes SDL, opens a window, pumps events, and
  presents a blank frame.
- One Emscripten build does the same from a local web server.
- No core module requires MSVC 4.20 or MASM.

## Phase 1: Core Platform Layer

Goal: provide the runtime services needed by Facemaker and later Shandalar.

Platform APIs to introduce:

- Window/display:
  - create main window/canvas,
  - logical size and integer scaling,
  - fullscreen/windowed policy,
  - cursor visibility if needed.
- Input:
  - keyboard events normalized to the legacy key queue,
  - mouse position and button mask,
  - text input where name entry needs it.
- Timing:
  - monotonic ticks,
  - per-frame tick accumulation,
  - scheduled callbacks/timers replacing `SetTimer`, `timeSetEvent`, and busy
    waits.
- Graphics:
  - indexed surface allocation/free,
  - blits, scaled blits, fill rects, scanline reads/writes,
  - palette storage and palette animation,
  - present active surface as SDL texture.
- Files:
  - path normalization from `\` to `/`,
  - asset lookup rooted in the bundled asset tree,
  - writable pref/save path lookup,
  - replacement for registry-backed options.
- Dialogs:
  - in-game modal message boxes,
  - file-save choices that work without native OS dialogs in web builds.
- Audio:
  - minimal sound init/close/load/play API compatible with current call sites,
  - browser "click to enable audio" handling.

Do not try to emulate all of Win32 up front. Emulate the game concepts first:
pages, palettes, input queues, timers, and modal flows.

Exit criteria:

- The SDL backend can allocate pages, manipulate pixels, and present them.
- The app has one authoritative event/tick/render loop.

## Phase 2: Facemaker Module

Goal: fold Facemaker into the SDL app as the first real UI module.

Why Facemaker first:

- Its UI scope is small.
- It already owns a compact set of pages and palette operations.
- It exercises key requirements: image loading, fonts/text, mouse input,
  keyboard input, save output, palette fades, and present timing.
- Shandalar already shares most of its code, making this work reusable.
- It immediately removes the obsolete `Facemaker.exe /S` process boundary.

Tasks:

- Delete the standalone Facemaker executable concept in the port fork.
- Convert `WinMain` and `FaceMakerWorkerThread` into a module API:
  - `facemaker_init(config)`,
  - `facemaker_handle_event(event)`,
  - `facemaker_tick(dt)`,
  - `facemaker_render()`,
  - `facemaker_get_result(result)`,
  - `facemaker_shutdown()`.
- Replace the worker thread with main-loop stepping. No SDL calls should happen
  off the main thread.
- Convert `WaitForUiTicks()` and other busy waits into scheduled animation
  states.
- Replace `GetMessage`/WndProc input with SDL event translation into the
  existing mouse/key globals, then gradually remove direct global coupling.
- Replace `GetSaveFileNameA` with a portable save flow:
  - native SDL build can use a simple in-game filename prompt at first,
  - web build writes to `/storage/PlayFace` or SDL pref path,
  - optional later browser download/upload can be done through JavaScript.
- Convert `DIBSurface` allocation for Facemaker:
  - keep `pBits`, `width`, `height`, `bitsPerPixel`, and `rowPadding`,
  - remove reliance on `HDC`, `HBITMAP`, `HPALETTE`, and file mappings in SDL
    builds,
  - upload indexed pixels through the active palette to an SDL texture on
    present.
- Replace GDI text for Facemaker:
  - first prefer existing bitmap/font file paths where possible,
  - for TrueType paths, use SDL_ttf or a small font rasterization wrapper,
  - maintain metrics closely enough for layout.
- Use `rpbits_c` for `.pic` and `.spr` asset loading under native SDL and
  Emscripten builds.
- Add image regression tests where possible:
  - load known assets,
  - render deterministic screens,
  - compare indexed buffers or RGBA snapshots with tolerances only where text
    rendering forces them.

Exit criteria:

- The SDL app can enter and exit the Facemaker module natively.
- The SDL app can enter and exit the Facemaker module in WebAssembly without
  pthreads.
- Portrait creation, gender/name encoding, palette fades, and save output work.

## Phase 3: Shandalar Main Menu and New-Game Flow

Goal: make Shandalar's opening flow use the in-process Facemaker module.

Tasks:

- Delete `RunFacemakerFlow()`'s `_spawnl("Facemaker.exe", "/S")` behavior and
  replace it with an in-process Facemaker modal state.
- Preserve the output contract that Shandalar currently depends on:
  - selected portrait graphics,
  - name bytes,
  - gender flag,
  - page/sprite data used by the main menu and adventure stats.
- Replace the current `g_facemaker_page4_bitmap`/`g_facemaker_page4_dib`
  handoff with a portable pixel-buffer or sprite-buffer handoff.
- Convert opening menu screens to state-machine flows:
  - title/main menu,
  - load/save slot UI,
  - difficulty choice,
  - color choice,
  - new-game transition.
- Replace `WaitForInputEvent()` and `WaitForInputEventUnlessBlocked()` for this
  area with nonblocking modal states.
- Keep shell IPC disabled in web builds. The old Magic shell/MTG shell behavior
  should become in-process navigation later, not `FindWindow`, `PostMessage`, or
  `WinExec`.

Exit criteria:

- A native SDL Shandalar build can reach the main menu, start a new game, run
  Facemaker in-process, and return to Shandalar.
- The same flow works in WebAssembly without blocking the browser main thread.

## Phase 4: Deck Builder and Shared Card Rendering

Goal: make the shared deck/card UI available before broader Shandalar and Magic
work depends on it.

Why this is its own phase:

- Shandalar and Magic both depend on DeckDLL-style behavior.
- DeckDLL has many custom WndProc classes, owner-drawn controls, GDI text, and
  card rendering calls.
- Card art and card rendering libraries are shared dependencies for the Magic
  duel UI.

Tasks:

- Port `cardartlib` and `drawcardlib` rendering to the portable graphics layer:
  - decode art into portable pixel buffers,
  - draw full/small cards into portable surfaces,
  - replace HDC parameters in SDL-facing APIs with portable draw targets.
- Replace HDC-shaped rendering APIs with portable draw-target APIs. Short-lived
  adapter shims are acceptable during migration, but should not become a
  permanent second API surface.
- Convert DeckDLL windows to portable widgets or to a minimal virtual-window
  dispatcher:
  - main deck builder surface,
  - card list/filter UI,
  - full-card preview,
  - sideboard/trade surfaces,
  - buttons and scrollbars.
- Persist decks under the web save path.
- Fold DeckDLL behavior into ordinary modules unless a separate library remains
  useful for testing.

Exit criteria:

- Native SDL build can open, edit, and save decks.
- WebAssembly build can open the same deck UI and persist decks through IDBFS.
- Card rendering works without GDI.

## Phase 5: Shandalar Adventure UI Minus Duel

Goal: port the rest of the single-player adventure shell while leaving the duel
system as a later phase.

Tasks:

- Replace Shandalar `WinMain` with SDL app callbacks or a fast-returning
  main-loop adapter.
- Replace the Win32 UI timer (`timeSetEvent`) with SDL tick scheduling.
- Replace `AdventureWorkerThread` with a main-loop state runner.
- Convert blocking adventure flows into resumable states:
  - world map navigation,
  - town/city visits,
  - dungeons and lairs,
  - creature encounters up to the duel transition,
  - stats/journal/map screens,
  - hints and list pickers,
  - load/save game UI.
- Replace GDI palette/display-mode behavior with logical SDL canvas sizes:
  - 640x480,
  - 800x600,
  - 1024x768.
- Replace `ChangeDisplaySettings`, topmost-window handling, `FindWindow`, and
  shell handoffs with in-app state transitions.
- Keep the duel transition as a stub that returns a controlled result until the
  Magic duel UI is ported.
- Use bundled assets from MEMFS and save data under persistent storage.

Exit criteria:

- A WebAssembly build can start Shandalar, create/load a character, navigate
  the adventure UI, use deck/stats/map screens, save, reload, and reach duel
  handoff stubs without blocking.

## Phase 6: Magic Shell and Non-Duel UI

Goal: prepare Magic for the duel system by porting its shell and shared modal UI.

Tasks:

- Disable Manalink by default in portable builds.
- Replace registry-backed settings with portable config storage.
- Replace `WinExec("manalink.exe RUNFROMSHELL")`, Manalink window discovery,
  mutexes, events, and monitor thread behavior with no-op or future-ready
  service interfaces.
- Convert Magic shell startup and menu dialogs to SDL states.
- Convert `DialogBoxParam`, `IsDialogMessage`, and modal Win32 dialogs into
  in-game modal flows.
- Build the virtual message/window model that the duel UI can reuse:
  - stable window IDs,
  - parent/child relationships,
  - focus/capture,
  - `SendMessage` as direct dispatch,
  - `PostMessage` as queue append,
  - timers as scheduled events,
  - invalidation and repaint as dirty-region or dirty-widget flags.

Exit criteria:

- `magic_sdl` can start, show shell UI, load settings/assets, and open
  non-duel modal flows without Win32.
- Manalink paths are isolated behind portable interfaces and do not affect
  single-player web builds.

## Phase 7: Magic Duel UI

Goal: port the most complex UI surface.

Tasks:

- Convert `RunDuelEngine()` and the duel message loop to the SDL main-loop
  model. Do not run the duel UI on a separate thread in the web target.
- Replace `WaitForSingleObject(global_mutex_GameInit, INFINITE)` and similar
  blocking waits with explicit state transitions.
- Replace `Sleep`-based animations and pauses with scheduled timers.
- Port the duel window classes onto the virtual window/widget dispatcher:
  - main duel window,
  - card class,
  - territory/battlefield,
  - hand,
  - life,
  - graveyard/library,
  - full-card preview,
  - phase/attack displays,
  - spell chain,
  - mana summary,
  - scrollbars,
  - prompt/tell-user/cue-card windows.
- Convert modal duel dialogs:
  - color choice,
  - coin flip,
  - big-card choices,
  - life prompt,
  - mana burn and interface options.
- Keep game rules and card logic single-threaded while UI events are processed
  frame by frame.
- Add deterministic replay/smoke tests for common duel actions:
  - start duel,
  - draw hand,
  - play land,
  - cast simple spell,
  - attack/block,
  - win/loss transition back to Shandalar.

Exit criteria:

- Native SDL build can complete representative duels.
- WebAssembly build can complete representative single-player duels without
  pthreads and without blocking browser UI.

## Phase 8: Audio, Persistence, Packaging, and Deployment

Goal: make the web build shippable rather than merely runnable.

Tasks:

- Replace `magsnd`/DirectSound behavior with SDL audio:
  - load WAV assets,
  - preserve sound IDs and LRU behavior where needed,
  - handle one-shot effects and loops,
  - handle browser audio unlock after user interaction.
- Decide on video/statwin scope:
  - stub unavailable video/stat windows initially,
  - port only if required by target gameplay.
- Use Emscripten packaging:
  - preload game assets into the virtual filesystem,
  - keep large-asset strategy explicit to avoid excessive startup downloads,
  - mount persistent `/storage` through SDL's Emscripten persistent path support.
- Link with appropriate Emscripten options:
  - `-s ALLOW_MEMORY_GROWTH=1`,
  - a realistic maximum memory cap,
  - `-gsource-map` for debug builds,
  - no pthread flags for the default web build.
- Customize `index.html` only after the generated shell works.
- Add a local web-server helper for testing COOP/COEP only if a separate
  pthread build is later introduced.

Exit criteria:

- One command builds a native SDL debug target.
- One command builds a WebAssembly package.
- The web package runs from a local static server, loads assets, saves data,
  reloads persistent saves, and handles audio unlock correctly.

## Suggested Milestones

1. Native SDL blank-window target.
2. Emscripten blank-window target.
3. Portable `.pic` decode through `rpbits_c`.
4. Facemaker module in native SDL app.
5. Facemaker module in WebAssembly.
6. Shandalar main menu plus in-process Facemaker native SDL.
7. Shandalar main menu plus in-process Facemaker WebAssembly.
8. Deck/card rendering native SDL.
9. Shandalar adventure UI native SDL.
10. Shandalar adventure UI WebAssembly with duel stub.
11. Magic shell native SDL.
12. Magic duel native SDL.
13. Magic duel WebAssembly.

## Main Risks

- `rpbits.asm`: must be replaced before a complete web asset-loading path can
  exist.
- Blocking UI: `WaitForInputEvent`, `DialogBox`, `Sleep`, thread waits, and
  message-pump assumptions are spread across Shandalar and Magic.
- GDI text metrics: replacing GDI font rendering may affect layout. Tests should
  focus first on behavior and visual acceptability, then on pixel closeness
  where practical.
- Asset case and path handling: existing code assumes Windows paths and
  case-insensitive lookup.
- Web persistence: IDBFS sync is asynchronous under the hood. SDL can make the
  mount ready before app init, but recent writes should still be treated as
  settling over later frames.
- Magic UI scope: the duel UI uses many HWNDs as data-bearing objects. A
  virtual window/message system is likely less risky than rewriting the duel UI
  all at once into bespoke SDL widgets.
- Manalink: multiplayer requires a new design for browser networking and should
  stay out of the default single-player WebAssembly path.

## Near-Term Recommendation

After the fork, start with one SDL app target and the minimum platform layer
required to run Facemaker as an in-app module. Do not begin by porting Magic's
window classes. The first valuable proof is:

1. SDL3 native and Emscripten builds initialize.
2. `.pic` decoding works through `rpbits_c`.
3. Facemaker runs inside the SDL main loop without a worker thread.
4. Shandalar's new-game flow calls Facemaker in-process.

That sequence directly removes the first web blockers while producing reusable
graphics, input, timing, filesystem, and modal-flow code for the rest of the
project.
