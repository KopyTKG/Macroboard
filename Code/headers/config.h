/*
 Created by:    KopyTKG
 Rewritten:     2026-07-07
 License:       CC0 v1.0

 Hardware config for the 32u4 (Arduino Micro / Leonardo / ProMicro) Macroboard.
 Only pins, matrix size and timing live here. Key assignments live in keymaps.h.
*/

#ifndef CONFIG_HEADER_H
#define CONFIG_HEADER_H

// ---- Matrix geometry -------------------------------------------------------
#define NUM_ROWS 5
#define NUM_COLS 4

// ---- Pin mapping (unchanged from the original board) -----------------------
// Rows are strobed (driven) one at a time; columns are sensed.
static const uint8_t rowPins[NUM_ROWS] = { 3, 5, 6, 9, 10 };
static const uint8_t columnPins[NUM_COLS] = { 8, 7, 4, 2 };

// Two indicator LEDs, wired active-high (pin = anode via resistor, cathode to
// GND), so driving the pin HIGH lights the LED.
#define NUM_LEDS 2
static const uint8_t LEDs[NUM_LEDS] = { 15, 16 };

// LED roles (index into LEDs[]): one shows NumLock state, one shows whether the
// macro layer is active. Swap these two numbers if the physical LEDs are
// reversed from what you expect.
#define LED_NUMLOCK  0            // pin 15 -> NumLock indicator
#define LED_MACRO    1            // pin 16 -> macro-layer indicator

// Assumed NumLock state at power-on (host NumLock is usually on; tap NumLock
// once to re-sync if not).
#define NUMLOCK_ON_AT_BOOT   true

// Macro-layer LED blink half-period (ms): the LED is on for this long, then off
// for this long, while the macro layer is active. Full blink = 2x this.
#define MACRO_BLINK_MS       300

// ---- Scan electrical behavior ----------------------------------------------
// The original board reads keys as ACTIVE-HIGH: a row is driven HIGH and a
// pressed key pulls its column HIGH (external column pull-downs hold it LOW
// when idle). This preserves that working behavior.
//
// If a future board is wired the standard pull-up way instead (strobe LOW,
// sense with INPUT_PULLUP, active-LOW), flip these four lines.
#define COLUMN_PIN_MODE   INPUT_PULLUP  // sense pin mode
#define STROBE_ACTIVE     HIGH          // level driven onto the selected row
#define STROBE_IDLE       LOW           // level on all non-selected rows
#define KEY_ACTIVE        HIGH          // column read that means "pressed"

// Microseconds to let a freshly-driven row settle before reading columns.
// The original used a blocking 5 ms delay(); a few microseconds is plenty and
// keeps the scan non-blocking.
#define STROBE_SETTLE_US  15

// ---- Debounce --------------------------------------------------------------
// A key state change is only accepted after the raw reading has been stable
// for this long. Non-blocking: no delay() in the scan loop.
#define DEBOUNCE_MS  5

// ---- Layer switching -------------------------------------------------------
// The macro layer is toggled by a chord: hold the NumLock key and tap this
// key. Tapping NumLock on its own just sends a real NumLock to the host.
#define MACRO_CHORD_CODE  KEYPAD_DIVIDE

#endif // CONFIG_HEADER_H
