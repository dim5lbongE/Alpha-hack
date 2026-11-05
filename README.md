# Alpha hack
Geode mod for Geometry Dash 2.2091. Windows x64 and Android ARM64 share one source tree and one universal package.

Download the `.geode` asset from [Releases](https://github.com/dim5lbongE/Alpha-hack/releases). Import it into Geode 6.0.0-alpha.1. Do not install the historical build-kit ZIP as a mod.

Open with **Tab** on desktop or the **Alpha hack** button on mobile. Play / Speed / Video / Physics / HUD / Unlocks / Songs separate the controls. FPS and TPS have numeric inputs; speed uses a slider and presets. Song replacement uses Geode's file picker and keeps originals intact.

See [release notes](RELEASE_NOTES.md) for validation limits and device checks. TPS bypass is experimental and detects incompatible native code. Noclip, changed speed/TPS and hitboxes mark the attempt; the indicator covers this mod only. VSync switching is Windows-only.

## Build
Use Geode SDK v6.0.0-alpha.1 with a C++23 compiler. `GEODE_SDK` must point to the SDK checkout. GitHub Actions builds Win64 and Android64, combines them, checks binary headers and publishes a beta release after all jobs succeed.

Core checks: `g++ -std=c++23 -O2 -Wall -Wextra -Werror tests/core_test.cpp -o core_test && ./core_test`.

Technical reference: [Geode SDK](https://github.com/geode-sdk/geode) and [Eclipse Menu](https://git.eclipse.menu/EclipseMenu/EclipseMenu) were consulted for engine hook behavior and native tick-count architecture. No game executable is distributed here.
