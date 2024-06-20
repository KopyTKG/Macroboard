#include <Keyboard.h>
#include "headers/keymaps.h"
#include "headers/config.h"

/*
 Created by: 	KopyTKG
 Date:		2024-06-20
 License:	CC0 v1.0
*/


// Layout counter
int Layout = 0;
// Key state array
bool keyStates[5][4] = {0};


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

      	// Check if the key is a Macro Switch
        if(i==0 && j == 3) {
          if(Layout < keyLayoutCap - 1) {
	    Layout++;
	  } else {
	    Layout = 0;
	  }
        }
	
	// Use function keys if the layout is not the default
        else if (FunctionLayout[i][j] != 0x00 && Layout > 0) { // Check if the key is not null
	  Keyboard.press(PrefixKeys[Layout]);
          Keyboard.press(FunctionLayout[i][j]);
        }
	// Use numeric keys if the layout is the default
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
