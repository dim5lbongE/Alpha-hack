## Alpha hack 2.3.0 beta 1
Geometry Dash **2.2091**, Geode **6.0.0-alpha.1**, Windows x64 + Android ARM64 in one `.geode`.

- Speedhack: scheduler delta is scaled once during gameplay; removed the previous physics-only scaling path. Slider 0.10–5.00x with quick presets. Music pitch reapplies when channels change.
- TPS bypass: custom tick counts and fractional time accumulation, 120–1000 TPS. 240 retains native physics. Native instructions must match before enabling; incompatible signatures and Click Between Frames fail closed. Android implementation is experimental.
- Cheat indicator: separates currently enabled assists from assists used earlier in this attempt; lists Noclip, Speed, TPS and Hitboxes. Turning an assist off does not clear attempt history. Modified attempts use test mode and suppress percentage saving. This indicator tracks Alpha hack only, not other mods.
- Brighter UI with seven categories, large toggle rows, numeric FPS/TPS fields, presets and remembered tab. Desktop Tab and mobile Alpha hack button.
- FPS uses a wall clock, independently of game speed; clock and noclip accuracy remain available.

### Validation / limitations
CI runs 864 timing combinations and edge checks, builds both native targets, and verifies PE x64 / ELF ARM64 package contents before publishing. **No in-game runtime validation has been completed.** This is a prerelease, not a claim that the reported bug is confirmed fixed on a device. FPS limits remain subject to hardware/VSync; Android VSync is system-managed. TPS patching may conflict with other physics mods; unavailable TPS is displayed instead of forcing a patch.

### Quick check
Open Speed, select 0.5x then 2x on a short level; compare movement to 1x. Pause/resume and restart. In Physics compare 240 with 360; verify the availability message. Toggle assists off mid-attempt: MODIFIED should remain until a fresh normal attempt. Practice checkpoint resets retain history.
