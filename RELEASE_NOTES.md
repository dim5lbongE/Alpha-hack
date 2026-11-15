## Alpha hack lite 1.0.1
GD 2.2091 / Geode 6.0.0-alpha.2. Windows x64 + Android ARM64.

- Load icon/color unlock hooks before the game validates saved selections at startup. The persisted Unlock Icons setting is available during early loading. No achievements or game save data are rewritten.
- Build against Geode 6.0.0-alpha.2 and require it in the package metadata.
- Keep the same mod ID and settings so upgrading from Lite 1.0.0 retains configuration.

Keep Unlock Icons enabled, select your icons/colors, and quit normally before restarting. An already-reset selection cannot be recovered automatically. With the mod disabled or Unlock Icons off, normal game validation applies.

CI builds Windows x64 and Android ARM64 and checks package architecture, version, SDK requirement, and early-load metadata. In-game restart/device testing still requires GD; it is not performed by CI.
