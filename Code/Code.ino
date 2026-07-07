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
static bool numlockOn = NUMLOCK_ON_AT_BOOT;  // real host NumLock state (polled)

// NumLock is a dual-function key: tap = real NumLock, hold + "/" = macro toggle.
// We can't tell which until it is released, so we defer the decision.
static bool numlockHeld = false;      // NumLock key currently physically down
static bool numlockConsumed = false;  // this hold already fired the macro chord

// Which keymap is live right now, from the two states above.
uint8_t currentLayer() {
  if (!macroOn)  return LAYER_NUMBERS;
  return numlockOn ? LAYER_MACRO_CTRL : LAYER_MACRO_PLAIN;
}

// Read the host's real NumLock LED state over USB and mirror it: drives the
// NumLock indicator and feeds currentLayer()'s Ctrl/plain macro choice. Stays
// correct even if NumLock is toggled from another keyboard.
void serviceNumlockLed() {
  numlockOn = (BootKeyboard.getLeds() & LED_NUM_LOCK) != 0;
  digitalWrite(LEDs[LED_NUMLOCK], numlockOn ? HIGH : LOW);
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
      // Just toggle the host; serviceNumlockLed() picks up the new state and
      // updates the LED + macro sub-layer on the next loop.
      BootKeyboard.press(KEY_NUM_LOCK);
      BootKeyboard.release(KEY_NUM_LOCK);
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
