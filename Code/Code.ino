#include <HID-Project.h>
#include "headers/config.h"
#include "headers/keymaps.h"

/*
 Created by:    KopyTKG
 Rewritten:     2026-07-07
 License:       CC0 v1.0

 5x4 diode-matrix numpad on a 32u4 (Arduino Micro / Leonardo / ProMicro).

 Rewrite goals over the original:
   - non-blocking per-key debounce (no delay() stalls in the scan loop)
   - clean N-layer keymap model instead of the special-cased 4-mode logic
   - NumLock stays a real NumLock; hold it + "/" to toggle the macro layer
   - each key releases exactly what it pressed (no global releaseAll)

 Uses NicoHood's HID-Project (BootKeyboard) instead of Keyboard.h so we can
 read the host's real NumLock LED state. Device identity (name / VID / PID) is
 set at build time; see the Makefile. Electrical scan is unchanged (config.h).
*/

// Per-key runtime state for debounce + release tracking.
struct KeyState {
  bool          reading;     // last raw sample
  bool          pressed;     // debounced, committed state
  unsigned long lastChange;  // millis() of the last raw change
  uint8_t       heldCode;    // keycode we actually pressed (for release)
  uint8_t       heldMod;     // modifier we actually pressed (for release)
};

static KeyState keys[NUM_ROWS][NUM_COLS];

// The active layer is derived from two independent states rather than stored.
static bool macroOn = false;                 // macro layer engaged (NumLock + /)
static bool numlockOn = NUMLOCK_ON_AT_BOOT;  // latched NumLock belief (see serviceNumlockLed)
static bool numlockSynced = false;           // host LED channel proven live this session

// NumLock is a dual-function key: tap = real NumLock, hold + "/" = macro toggle.
// We can't tell which until it is released, so we defer the decision.
static bool numlockHeld = false;      // NumLock key currently physically down
static bool numlockConsumed = false;  // this hold already fired the macro chord

// Which keymap is live right now, from the two states above.
uint8_t currentLayer() {
  if (!macroOn)  return LAYER_NUMBERS;
  return numlockOn ? LAYER_MACRO_HILITE : LAYER_MACRO_PLAIN;
}

// Read the host's real NumLock LED state over USB and latch it: drives the
// NumLock indicator and feeds currentLayer()'s highlight/plain macro choice.
//
// The latch exists because an MCU reset (brown-out / flaky micro-USB) wipes the
// host's cached LED state to 0, and the OS does NOT re-send it — a blind mirror
// would then show NumLock "off" forever while the host is still on. So:
//   - host bit SET  -> always real (a wiped cache reads 0, never a false 1):
//                      adopt it and mark the channel proven live this session.
//   - host bit CLEAR while synced -> a genuine host-off (e.g. toggled on another
//                      keyboard): follow it.
//   - host bit CLEAR before ever syncing -> possibly just a reset cache: hold the
//                      latched belief (NUMLOCK_ON_AT_BOOT after a reset) instead
//                      of dropping out.
// The pad's own NumLock tap flips `numlockOn` directly (see onRelease), so it
// still tracks correctly even in the pre-synced window.
void serviceNumlockLed() {
  bool host = (BootKeyboard.getLeds() & LED_NUM_LOCK) != 0;
  if (host) {
    numlockOn = true;
    numlockSynced = true;
  } else if (numlockSynced) {
    numlockOn = false;
  }
  digitalWrite(LEDs[LED_NUMLOCK], numlockOn ? HIGH : LOW);
}

// Boot/reset diagnostic. Runs once per MCU start: a distinctive triple-blink of
// the NumLock LED. This ONLY fires when the chip (re)starts, so if you ever see
// it flash on its own during normal use, the board is resetting (brown-out /
// power dip / flaky micro-USB) — that reset wipes the host's cached NumLock LED
// state to 0, and the OS does not re-send it, which is why the NumLock indicator
// "drops out" while the OS still shows NumLock on. If the LED instead drops with
// NO flash, the host itself reported NumLock off (an OS-side quirk, not a reset).
void bootBlink() {
  for (uint8_t i = 0; i < 3; i++) {
    digitalWrite(LEDs[LED_NUMLOCK], HIGH);
    delay(80);
    digitalWrite(LEDs[LED_NUMLOCK], LOW);
    delay(120);
  }
}

// Macro LED blinks while the macro layer is active, off otherwise. Called every
// loop; the square wave comes straight from millis() so nothing blocks.
void serviceMacroLed() {
  bool on = macroOn && ((millis() / MACRO_BLINK_MS) & 1);
  digitalWrite(LEDs[LED_MACRO], on ? HIGH : LOW);
}

// A key just went down: run its action from the current layer.
void onPress(uint8_t r, uint8_t c) {
  KeyAction a = layers[currentLayer()][r][c];
  KeyState &k = keys[r][c];

  switch (a.type) {
    case KT_NUMLOCK:
      // Don't emit yet: decide tap-vs-chord when it is released.
      numlockHeld = true;
      numlockConsumed = false;
      break;

    case KT_KEY:
      // Chord: NumLock held + the macro key toggles the macro layer, and the
      // key itself is swallowed (no keystroke sent).
      if (numlockHeld && a.code == MACRO_CHORD_CODE) {
        macroOn = !macroOn;
        numlockConsumed = true;  // suppress the NumLock tap for this hold
        break;                   // macro LED updates itself in serviceMacroLed()
      }
      if (a.mod) BootKeyboard.press((KeyboardKeycode)a.mod);
      BootKeyboard.press((KeyboardKeycode)a.code);
      k.heldCode = a.code;   // remember what to release later
      k.heldMod  = a.mod;
      break;

    case KT_NONE:
    default:
      break;
  }
}

// A key just went up: release exactly what it pressed.
void onRelease(uint8_t r, uint8_t c) {
  KeyState &k = keys[r][c];

  // The NumLock key (same position in every layer) resolves on release:
  // a plain tap sends a real NumLock; a hold that fired the chord sends nothing.
  if (layers[currentLayer()][r][c].type == KT_NUMLOCK) {
    numlockHeld = false;
    if (!numlockConsumed) {
      // A genuine tap: toggle the host and flip our belief immediately so the
      // LED + macro sub-layer track even before the host's report arrives (and
      // even in the pre-synced window right after a reset). serviceNumlockLed()
      // reconciles with the host's report on the next loop.
      BootKeyboard.press(KEY_NUM_LOCK);
      BootKeyboard.release(KEY_NUM_LOCK);
      numlockOn = !numlockOn;
    }
    numlockConsumed = false;
    return;
  }

  if (k.heldCode) BootKeyboard.release((KeyboardKeycode)k.heldCode);
  if (k.heldMod)  BootKeyboard.release((KeyboardKeycode)k.heldMod);
  k.heldCode = 0;
  k.heldMod  = 0;
}

void setup() {
  // Sense pins.
  for (uint8_t c = 0; c < NUM_COLS; c++) {
    pinMode(columnPins[c], COLUMN_PIN_MODE);
  }

  // Strobe pins, parked idle.
  for (uint8_t r = 0; r < NUM_ROWS; r++) {
    pinMode(rowPins[r], OUTPUT);
    digitalWrite(rowPins[r], STROBE_IDLE);
  }

  // LEDs.
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    pinMode(LEDs[i], OUTPUT);
    digitalWrite(LEDs[i], LOW);
  }

  bootBlink();  // diagnostic: see bootBlink() — flags spurious resets

  BootKeyboard.begin();
  serviceNumlockLed();
  serviceMacroLed();
}

void loop() {
  unsigned long now = millis();

  for (uint8_t r = 0; r < NUM_ROWS; r++) {
    digitalWrite(rowPins[r], STROBE_ACTIVE);
    delayMicroseconds(STROBE_SETTLE_US);  // let the row settle before sampling

    for (uint8_t c = 0; c < NUM_COLS; c++) {
      bool raw = (digitalRead(columnPins[c]) == KEY_ACTIVE);
      KeyState &k = keys[r][c];

      if (raw != k.reading) {
        // Raw level moved: restart this key's debounce window.
        k.reading = raw;
        k.lastChange = now;
      } else if (raw != k.pressed && (now - k.lastChange) >= DEBOUNCE_MS) {
        // Stable long enough and different from the committed state: commit.
        k.pressed = raw;
        if (raw) onPress(r, c);
        else     onRelease(r, c);
      }
    }

    digitalWrite(rowPins[r], STROBE_IDLE);
  }

  serviceNumlockLed();  // mirror the host's real NumLock LED + macro sub-layer
  serviceMacroLed();    // non-blocking blink while the macro layer is active
}
