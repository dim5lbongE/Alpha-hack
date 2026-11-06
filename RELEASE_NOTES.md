## Alpha hack lite 1.0.0
Version numbering restarts at **1.0.0** as requested. GD 2.2091 / Geode 6.0.0-alpha.1. Windows x64 + Android ARM64.

Geode settings now include Unlock Icons, Practice Music Sync, Copy Bypass, Smart StartPos and StartPos Switcher.

- **Smart StartPos:** on reset, infer mode, speed, mini and dual from static portals to the left of each StartPos. Turning it off restores saved manual settings on the next reset. Classic levels only; it does not simulate reverse paths, moving/toggled portals, trigger logic or gravity transitions.
- **StartPos Switcher:** Q / E or the bottom arrows cycle through StartPos objects ordered by X. 0 is the level beginning. Controls appear only when enabled and StartPos objects exist. Switching clears the active checkpoint, restarts and restarts music. All switched runs remain test runs, including 0; reopen the level for a normal fresh attempt.

The version is lower than 3.x, so Geode may require manually replacing the prior package. Keep only one `dim5l.practice_sync` package and restart GD. No CBF, full mod menu or timing bypass is added.

CI builds both platforms and verifies both binaries. Actual gameplay/device testing has not been performed; the 1.0.0 number does not imply runtime validation.
