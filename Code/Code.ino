#include <Keyboard.h>
#include "headers/keymaps.h"
#include "headers/config.h"

/*
 Created by: 	KopyTKG
 Date:		2024-07-09
 License:	CC0 v1.0
*/


// Switch LED diode based on the layout mode
void checkMode(int layout) {
  for (int i = 0; i < 2; i++) {
    digitalWrite(LEDs[i], LOW);
  }

  if (layout == 1) {
    digitalWrite(LEDs[0], HIGH);
  }
  if (layout == 2) {
    digitalWrite(LEDs[1], HIGH);
  }
  if (layout == 3) {
    digitalWrite(LEDs[0], HIGH);
    digitalWrite(LEDs[1], HIGH);
  }
}

// Check if layout switch is possible
int tryModeSwitch(int i, int j, int layout) {
  if(i==0 && j == 3) {
    if(layout < keyLayoutCap - 1) {
      layout++;
    } else {
      layout = 0;
    }
  }
  return layout;
}



void setup() {
  // Initialize column pins
  for (int i = 0; i < 4; i++) {
    pinMode(columnPins[i], INPUT_PULLUP);
  }

  // Initialize row pins
  for (int i = 0; i < 5; i++) {
    pinMode(rowPins[i], OUTPUT);
    digitalWrite(rowPins[i], LOW);
  }

  // LED initialization
  for (int i = 0; i < 2; i++) {
    pinMode(LEDs[i], OUTPUT);
    digitalWrite(LEDs[i], LOW);
  }
	
  // Initialize Keyboard communication
  Keyboard.begin();
}

void loop() {

  // Loop through each row
  for (int i = 0; i < 5; i++) {
    digitalWrite(rowPins[i], HIGH);
    delay(debounceDelay); // Debounce delay
 
    checkMode(mode);

    // Check each column
    for (int j = 0; j < 4; j++) {
      bool active = digitalRead(columnPins[j]); // Active HIGH
      if (active && !keyStates[i][j]) {
	      mode = tryModeSwitch(i, j, mode);
        if (Layout[i][j] != 0x00) { 
          Keyboard.press(PrefixKeys[mode]); 
          Keyboard.press(Layout[i][j]);
        }
        keyStates[i][j] = true;
      } else if (!active && keyStates[i][j]) {
        Keyboard.releaseAll();
        keyStates[i][j] = false;
      }
    }

    digitalWrite(rowPins[i], LOW);
    delay(scanDelay); // Delay after each row scan to allow time to settle
  }
}
