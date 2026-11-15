# Alpha hack lite 1.0.1
GD 2.2091 / Geode 6.0.0-alpha.2. Windows x64 + Android ARM64.

Open **Geode → Installed → Alpha hack lite → Settings**.

- Unlock Icons
- Practice Music Sync
- Copy Bypass (reopen the level info page after enabling)
- Smart StartPos (mode, speed, mini, dual; applies on reset)
- StartPos Switcher (Q / E or bottom arrows; 0 = level start)

Switcher controls appear only in levels with StartPos objects. Switching is blocked while paused and after completion. Switched runs stay in test mode; reopen the level for a normal attempt. Smart StartPos handles classic left-to-right static portals, not arbitrary 2.2 trigger/path simulation.

Update from Lite 1.0.0 normally. If upgrading from an older full 3.x package, manually replace it if Geode refuses the lower version, then restart. The mod ID remains `dim5l.practice_sync`.

[Downloads](https://github.com/dim5lbongE/Alpha-hack/releases/tag/v1.0.1) · [Release notes](RELEASE_NOTES.md)

Both builds and package headers are verified in CI. Actual in-game device testing has not been performed.

### Restart icon fix
The mod now loads early, before GD validates saved icon/color selections. Keep Unlock Icons enabled, select your icons and colors, and exit GD normally before restarting. Selections already reset and saved by the old version cannot be recovered; select them once again after updating. Disabling Unlock Icons or the mod restores normal game validation.
