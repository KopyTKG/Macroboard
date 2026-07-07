/*
 Created by:    KopyTKG
 Rewritten:     2026-07-07
 License:       CC0 v1.0

 Layer keymaps for the Macroboard.

 Every physical key is one KeyAction. The active layer is derived from two
 states (see Code.ino): the macro toggle and NumLock.

   macro off              -> LAYER_NUMBERS        (numpad; host decides digit/nav)
   macro on  + NumLock on -> LAYER_MACRO_HILITE   (highlight modifier + keys)
   macro on  + NumLock off-> LAYER_MACRO_PLAIN    (plain keys, no modifier)

 The NumLock key (NL) behaves like a real NumLock; holding it and tapping
 MACRO_CHORD_CODE (the "/" key, see config.h) toggles the macro layer.

 To add a layer: bump NUM_LAYERS and add another block to `layers`.
 This table is deliberately plain data so it ports 1:1 to a future Pico build.
*/

#ifndef KEYS_HEADER_H
#define KEYS_HEADER_H

#include "config.h"

// ---- Action model ----------------------------------------------------------
enum KeyType {
  KT_NONE = 0,  // unpopulated position / do nothing
  KT_KEY,       // send `code`, optionally with modifier `mod` held
  KT_NUMLOCK    // tap = real NumLock; hold + MACRO_CHORD_CODE = toggle macros
};

typedef struct {
  uint8_t type;
  uint8_t code;  // primary keycode  (KT_KEY)
  uint8_t mod;   // modifier keycode (KT_KEY, 0 = none)
} KeyAction;

// ---- Shorthand for the tables ----------------------------------------------
#define XX        { KT_NONE,    0x00, 0x00 }          // not populated
#define KEY(c)    { KT_KEY,     (c),  0x00 }          // plain key
#define KM(c, m)  { KT_KEY,     (c),  (m)  }          // key + held modifier
#define NL        { KT_NUMLOCK, 0x00, 0x00 }          // NumLock (+chord = macros)

// The "highlight" modifier held across the whole highlighted macro layer. Right
// Alt is used instead of Left Ctrl because games/OS almost never bind it, so it
// won't misfire in-game or trigger stray Ctrl-shortcuts. Change this one line to
// retarget the layer (e.g. KEY_RIGHT_CTRL).
#define HL        KEY_RIGHT_ALT

// ---- Layers ----------------------------------------------------------------
// NUM_LAYERS blocks of NUM_ROWS x NUM_COLS. Keep NL and the "/" chord key at
// the same positions in every layer so switching works from anywhere. The
// index defines below must match the block order.
#define NUM_LAYERS         3
#define LAYER_NUMBERS      0   // macro off  -> numpad (digits/nav via host)
#define LAYER_MACRO_HILITE 1   // macro on,  NumLock on  -> highlight mod + keys
#define LAYER_MACRO_PLAIN  2   // macro on,  NumLock off -> plain keys

static const KeyAction layers[NUM_LAYERS][NUM_ROWS][NUM_COLS] = {

  // Layer 0 — Numbers (macro off)
  {
    { KEY(KEYPAD_SUBTRACT), KEY(KEYPAD_MULTIPLY), KEY(KEYPAD_DIVIDE), NL            },
    { XX,                KEY(KEYPAD_9),        KEY(KEYPAD_8),     KEY(KEYPAD_7) },
    { KEY(KEYPAD_ADD),  KEY(KEYPAD_6),        KEY(KEYPAD_5),     KEY(KEYPAD_4) },
    { XX,                KEY(KEYPAD_3),        KEY(KEYPAD_2),     KEY(KEYPAD_1) },
    { KEY(KEY_RETURN),   KEY(KEYPAD_DOT),      XX,                KEY(KEYPAD_0) },
  },

  // Layer 1 — Macro + highlight modifier (macro on, NumLock ON). The old
  // "mode 1", but every key is sent with the HL modifier (Right Alt) held
  // instead of Ctrl, which is what the Discord / app bindings expect without
  // colliding with in-game Ctrl binds. Retarget the whole layer via HL above.
  {
    { KM(KEYPAD_SUBTRACT, HL), KM(KEYPAD_MULTIPLY, HL), KM(KEYPAD_DIVIDE, HL), NL            },
    { XX,                   KM(KEY_F14, HL),         KM(KEY_F15, HL),      KM(KEY_F13, HL) },
    { KM(KEY_F24, HL),      KM(KEY_F18, HL),         KM(KEY_F17, HL),      KM(KEY_F16, HL) },
    { XX,                   KM(KEY_F22, HL),         KM(KEY_F21, HL),      KM(KEY_F20, HL) },
    { KM(KEY_RETURN, HL),   KM(KEY_F23, HL),         XX,                   KM(KEY_F19, HL) },
  },

  // Layer 2 — Macro plain (macro on, NumLock OFF). Same keys as layer 1 but
  // with no modifier, so the host sees bare F13-F24 (and the raw operators).
  {
    { KEY(KEYPAD_SUBTRACT), KEY(KEYPAD_MULTIPLY), KEY(KEYPAD_DIVIDE), NL            },
    { XX,                KEY(KEY_F14),         KEY(KEY_F15),      KEY(KEY_F13)  },
    { KEY(KEY_F24),      KEY(KEY_F18),         KEY(KEY_F17),      KEY(KEY_F16)  },
    { XX,                KEY(KEY_F22),         KEY(KEY_F21),      KEY(KEY_F20)  },
    { KEY(KEY_RETURN),   KEY(KEY_F23),         XX,                KEY(KEY_F19)  },
  },
};

#endif // KEYS_HEADER_H
