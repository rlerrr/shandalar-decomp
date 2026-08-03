# Pre-Fork SDL3 and Emscripten Prep Checklist

Created: 2026-06-04

This checklist is for changes that can be done in the current matching-oriented
repo before creating the SDL/WebAssembly fork. The goal is not to port the app
in-place. The goal is to land low-risk compatibility work that helps the future
fork while staying sideways-compatible with MSVC 4.20 and the existing reccmp
workflow.

## Rules for Pre-Fork Work

- Do not introduce broad SDL abstractions into the matching codebase.
- Avoid large `#ifdef` trees. If a change needs heavy branching, defer it until
  after the fork.
- Keep MSVC 4.20 compatibility unless the code is guarded by `MODERN_FIXES` or
  excluded from MSVC 4.20 targets.
- Keep C89-compatible style in shared files:
  - declarations before statements,
  - no `for (int i = ...)`,
  - no required `stdint.h` assumptions in MSVC 4.20 paths.
- Do not remove matching-only stack-layout structs pre-fork unless the function
  is already non-matching or the change is explicitly accepted as a matching
  regression.
- Do not change target structure, staging behavior, or reccmp configuration
  unless the change is strictly additive.

## Highest-Value Prep

1. Add `rpbits_c` for `MODERN_FIXES` builds.

   The MASM `rpbits` target is an unavoidable WebAssembly blocker. A portable C
   implementation can be added now behind `MODERN_FIXES` or a narrow CMake
   switch while leaving the MSVC 4.20/MASM path alone.

   Acceptance criteria:

   - MSVC 4.20 matching builds still use `src/rpbits/src/rpbits.asm`.
   - Modern builds can select `rpbits_c` without MASM.
   - Existing `.pic` decode call sites keep the same public functions:
     `RpBits_ReadTables`, `RpBits_DecodeImage`, and any required helpers.
   - Add a small decode comparison test/tool if assets are available locally, or
     a byte-level unit test around known compressed fixtures if fixtures can be
     committed.

2. Tighten modern compiler warnings where compatible.

   Use modern compiler builds to find real undefined behavior and portability
   hazards, but fix only issues that do not perturb matching-sensitive codegen
   for MSVC 4.20.

   Useful categories:

   - implicit function declarations,
   - missing prototypes,
   - pointer/integer truncation hidden by 32-bit Windows assumptions,
   - signedness mismatches that affect file or image decoding,
   - uninitialized locals in non-matching or modern-only paths,
   - accidental C++ keyword collisions if later C++ wrapping is likely.

3. Isolate path handling helpers.

   Add small helpers that can normalize asset paths without changing call-site
   behavior broadly.

   Pre-fork-safe work:

   - centralize slash conversion behind a helper,
   - avoid absolute path additions,
   - document case-sensitive asset risks,
   - add tests/tools to enumerate asset references and detect case collisions.

   Defer:

   - replacing all `\\` string literals,
   - changing save-root behavior,
   - introducing SDL pref paths.

4. Isolate registry/config access.

   Identify registry-backed settings and INI reads, then add narrow wrapper
   functions where the code already has a natural boundary.

   Pre-fork-safe work:

   - document registry keys and INI files used by Magic/Shandalar.
   - add wrappers only when they avoid duplication and keep Win32 behavior
     identical.

   Defer:

   - JSON/TOML/new config formats,
   - SDL storage paths,
   - web persistence.

5. Inventory blocking UI and thread assumptions.

   Create docs or comments that map the future state-machine conversion.

   Include:

   - `WaitForInputEvent()` and callers,
   - `Sleep()` in UI/gameplay paths,
   - `DialogBoxParam` and common dialog use,
   - `CreateThread`, `_beginthread`, `WaitForSingleObject`,
   - `GetMessage` loops,
   - `SetTimer` and `timeSetEvent`.

   Defer actual conversion until after the fork.

6. Inventory GDI ownership and page APIs.

   The future SDL graphics layer needs a clear map of the current surface
   model.

   Document:

   - `DIBSurface` fields that represent real pixel-buffer state,
   - fields that are only GDI handles,
   - page ownership and lifetime assumptions,
   - where palettes are read, animated, and applied,
   - which functions touch `pBits` directly.

   Pre-fork-safe code changes:

   - add missing prototypes,
   - fix comments that misdescribe ownership,
   - add modern-only asserts around obviously invalid page indexes.

7. Add modern-only smoke tools.

   Small tools are useful because they do not force the main binaries to become
   portable yet.

   Good candidates:

   - asset reference scanner,
   - `.pic` decode smoke test using `rpbits_c`,
   - palette conversion dumper,
   - save/deck file probe,
   - card-art decode smoke test.

   Keep these out of reccmp target behavior.

## Clang Compatibility Checklist

- Build modern targets with Clang or clang-cl where practical.
- Fix include-order problems where files rely on transitive Windows headers.
- Avoid adding new MSVC-only extensions to shared code.
- Prefer standard C library calls when MSVC 4.20 already supports them.
- For functions that need replacement APIs in modern builds, keep wrappers small
  and named around game concepts, not SDL concepts.
- Avoid assuming `long`, pointer, and `int` have the same size in new code.
  Current code is 32-bit-heavy, but new prep code should not deepen that hole.
- Keep warnings actionable. Do not enable a warning level that creates so much
  noise nobody can see new regressions.

## Suggested Pre-Fork Order

1. Add `rpbits_c` behind `MODERN_FIXES` or an additive CMake option.
2. Add a decode smoke test/tool for `rpbits_c`.
3. Fix minimal prototype/include issues exposed by Clang modern builds.
4. Add path/case inventory tooling.
5. Add blocking UI/thread inventory docs.
6. Add GDI/page ownership notes.
7. Stop there and fork before doing architectural SDL work.

## Things to Avoid Before the Fork

- Do not introduce SDL into the main targets.
- Do not split Facemaker into both a new library and the existing executable.
- Do not rewrite `RunFacemakerFlow()` pre-fork unless the change is tiny and
  Windows behavior is identical.
- Do not convert message pumps or modal dialogs pre-fork.
- Do not replace DeckDLL/Magic WndProc structures pre-fork.
- Do not remove reccmp comments, matching CMake, `.def` files, or MSVC 4.20
  target settings pre-fork.
- Do not start a broad cleanup of `struct { ... } s;` locals before matching is
  intentionally abandoned.

## Fork Cutover Notes

At fork time, make a deliberate cleanup commit or short cleanup series:

- remove reccmp configuration and matching-only build files,
- remove MSVC 4.20-only CMake settings,
- delete or archive MASM `rpbits` after `rpbits_c` is promoted,
- collapse original DLL/exe boundaries into modules,
- delete Facemaker standalone executable scaffolding,
- establish the SDL app target as the main target,
- document which original binaries are now modules and which are out of scope.
