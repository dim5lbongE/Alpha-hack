## Alpha hack 2.3.1 beta 1
GD 2.2091 / Geode 6.0.0-alpha.1. Windows x64 + Android ARM64.

- Larger menu labels, stronger contrast, short controls and no persistent explanatory footers.
- Removed Alpha text launchers from gameplay and the main screen. Windows uses Tab; Android uses a circular three-line button.
- Cheat indicator is now a small green/red dot at the bottom left, independent of FPS/clock text. No brand, reason list, speed or TPS text is drawn during play.
- Red history survives turning an assist off and practice checkpoint resets. A fresh normal attempt resets it. The completion dot remains visible even if the in-play indicator setting is off; its verdict is frozen at completion.

### Indicator policy
Reviewed Pointercrate's [eligibility guidelines](https://pointercrate.com/guidelines/eligibility/) and [linked allowed-mods spreadsheet](https://docs.google.com/spreadsheets/d/1evE4nXATxRAQWu2Ajs54E6cVUqHBoSid8I7JauJnOzg/edit#gid=1388959524) on 2026-11-05.

Noclip, non-1x speed, non-240 TPS and live hitboxes mark this mod's attempt red. Render FPS does not: Pointercrate permits any FPS in GD 2.2+. Practice music, icon unlocks, clock and other display-only controls do not independently mark red.

Smooth Fix is **conservatively red whenever enabled**. Pointercrate specifically rejects significant slowdown; this implementation cannot reliably measure that condition, so it does not certify a Smooth Fix attempt green. This is stricter than the guideline, not a claim that the option itself is always banned.

The official endscreen-indicator requirement is stated for Mega Hack v7+; Alpha also retains its dot on the completion screen. **Alpha hack is not Pointercrate-approved. Green only means this mod detected none of its flagged settings during the attempt. It does not validate other mods, level copies, live markers, pauses or recording requirements.**

Core timing/history checks and both native builds are required by CI. Actual in-game rendering and gameplay have not been device-tested.
