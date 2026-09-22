# CppLogiMake driver fixes

Fixes the build failure you hit running `logimake build` for the first
time (the driver self-builds on first use, and src/main.cpp didn't
compile).

## What's fixed

1. `RegisterSubmoduleIncludes`'s signature used a bare `fs::` alias that
   was never declared in main.cpp (everywhere else in the file spells
   out `std::filesystem::`). You already fixed and pushed this part
   yourself (commit a46dd82 on master) - it's included here only so the
   diff/patch applies cleanly on top of a clean master checkout.

2. `RegisterSubmoduleIncludes` called `DiscoverSubmodules()`, which only
   exists in src/submodule_scanner.cpp - a file CMakeLists.txt never
   compiles into the `logicmake` executable (only src/main.cpp is
   listed in add_executable), and nothing declares
   `DiscoverSubmodules`/`SubmoduleInfo` in a header main.cpp includes.
   Also confirmed `RegisterSubmoduleIncludes` itself is never called
   from anywhere in the codebase. Both problems together mean this was
   dead code on both ends - unreachable, and (until fix #1) not even
   compilable - so it's deleted rather than patched further, with a
   comment explaining what a real fix would need if you want that
   submodule-include-discovery feature working later.

## Verification

Built clean end-to-end in a fresh checkout: cloned CppLogiMake, recursively
initialized submodules (including the nested `external/CppProlog/External/rang`
one, which wasn't initialized recursively before - needed for
`prolog_core` to build at all), ran `cmake -B build2` then
`cmake --build build2 --target logicmake`. Full build succeeds, `main.cpp`
compiles and links, the resulting `logicmake` binary runs. This is a real
link-level build, not just a syntax check.

## Applying

From your CppLogiMake repo root:

    git apply cpplogicmake-fixes.patch

Or just replace src/main.cpp with the copy in this archive - same
content either way. Since you already pushed the `fs::` part yourself,
`git apply` may report that hunk as already applied depending on your
current branch state; if so, just check that the `RegisterSubmoduleIncludes`
function is gone from src/main.cpp and you're done.
