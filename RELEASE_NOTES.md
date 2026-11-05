## Alpha hack lite 3.1.0 beta 1
GD 2.2091 / Geode 6.0.0-alpha.1. Windows x64 + Android ARM64.

Added **Copy Bypass** to Geode's standard settings. Enable it, reopen the level info page, and use the normal copy button. Copy-disabled and password-protected levels use the game's normal clone flow without a password prompt.

The source level's copy/password field is overridden only while building the info UI or entering the clone flow, then restored. Turning the toggle off delegates to normal game behavior.

Unlock Icons and Practice Music Sync remain. No custom menu, CBF, HUD or timing changes. Install and restart GD; Lite replaces the previous version using the same mod ID.

Both platform builds and binary package verification run in CI. Actual in-game copy behavior has not been device-tested.
