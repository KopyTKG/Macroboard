#include <Keyboard.h>
#include "headers/keymaps.h"

int Layout = 0;

bool keyStates[5][4] = {0};

const int LEDs[2] = {15, 16};

// Pin configuration
const int columnPins[4] = {2, 4, 7, 8};
const int rowPins[5] = {3, 5, 6, 9, 10};

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
  int debounceDelay = 5;  // Increased debounce delay
  int scanDelay = 1;      // Adjusted scan delay

  // Loop through each row
  for (int i = 0; i < 5; i++) {
    digitalWrite(rowPins[i], HIGH);
    delay(debounceDelay); // Debounce delay
    
    // Turn keyset indicator LEDs
    for (int i = 0; i < 2; i++) {
     digitalWrite(LEDs[i], LOW);
    }
    if (Layout == 1) {
     digitalWrite(LEDs[0], HIGH);
    }
    if (Layout == 2) {
     digitalWrite(LEDs[1], HIGH);
    }
    if (Layout == 3) {
     digitalWrite(LEDs[0], HIGH);
     digitalWrite(LEDs[1], HIGH);
    }

    // Check each column
    for (int j = 0; j < 4; j++) {
      bool active = digitalRead(columnPins[j]); // Active HIGH

      if (active && !keyStates[i][j]) {
        if(i==0 && j == 3) {
          if(Layout < 3) {
	    Layout++;
	  } else {
	    Layout = 0;
	  }
        }
        else if (FunctionLayout[i][j] != 0x00 && Layout > 0) { // Check if the key is not null
	  Keyboard.press(PrefixKeys[Layout]);
          Keyboard.press(FunctionLayout[i][j]);
        }
        else if (DefaultLayout[i][j] != 0x00 && Layout == 0) { // Check if the key is not null
          Keyboard.press(DefaultLayout[i][j]);
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
