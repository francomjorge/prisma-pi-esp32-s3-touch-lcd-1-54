# B1 — Intro + Display + Touch + Local UI

Status: IMPLEMENTED / CI AND PHYSICAL ACCEPTANCE PENDING.

## Implemented

- B0 initialization remains as startup regression.
- Black boot frame.
- PRISMA identity frame.
- Short deterministic-bounded glitch sequence using horizontal fragments and title displacement.
- Main local screen.
- Quick Control second screen.
- CST816S read path over the existing shared I2C bus.
- Normalized tap, long-press and horizontal swipe detection at UI level.
- PLUS toggles screens.
- BOOT returns to main screen.
- PWR press is observed only; shutdown semantics remain reserved for B11.
- Backlight primitive is separated from display initialization.
- UI uses BSP framebuffer primitives and does not access GPIO/I2C/SPI directly.

## Deliberately not implemented in B1

- Audio behavior: B2. MIC/SPEAKER/BEEP entries are marked B2 and are not fake controls.
- Wi-Fi: B5.
- Power shutdown semantics: B11.
- Web UI: B7.

## Acceptance target

1. Build under ESP-IDF 5.5.1.
2. Intro completes repeatedly without reset.
3. Main screen is stable.
4. Horizontal swipe changes screen and can return.
5. PLUS changes screen; BOOT returns home.
6. Touch taps and long presses are logged without blocking the UI.
7. 30-minute navigation stress produces no watchdog/panic/reset or progressive memory failure.

B0 physical regression was explicitly waived for progression to B1 by project decision; B0 CI was green.
