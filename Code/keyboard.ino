#include <Keyboard.h>

const char keys[5][4] = {
  {'-', '*', '/', KEY_NUM_LOCK}, // Use ASCII for '-', '*', and '/'
  {0x00, '9', '8', '7'}, // Directly use character numbers for keypad numbers
  {'+', '6', '5', '4'}, // Use ASCII for '+'
  {0x00, '3', '2', '1'}, // Directly use character numbers
  {KEY_RETURN, '.', 0x00, '0'}  // Use ASCII for '.', KEY_RETURN for Enter
};

1326469879

const char functionKeys[5][4] = {
  {'-', '*', '/', KEY_NUM_LOCK},  // Same as the first row in keys
  {0x00, KEY_F14, KEY_F15, KEY_F13}, // Replacing numbers 9, 8, 7 with F14, F15, F13
  {'+', KEY_F18, KEY_F17, KEY_F16}, // Replacing numbers 6, 5, 4 with F18, F17, F16
  {0x00, KEY_F22, KEY_F21, KEY_F20}, // Replacing numbers 3, 2, 1 with F22, F21, F20
  {KEY_RETURN, '.', 0x00, KEY_F19}  // Replace number 0 with F19
};

bool functionSet = false;

bool keyStates[5][4] = {0};

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

    // Check each column
    for (int j = 0; j < 4; j++) {
      bool active = digitalRead(columnPins[j]); // Active HIGH

      if (active && !keyStates[i][j]) {
        if(i==0 && j == 3) {
          functionSet = !functionSet;
        }
        if (keys[i][j] != 0x00 && !functionSet) { // Check if the key is not null
          Keyboard.press(keys[i][j]);
        }
        else if (functionKeys[i][j] != 0x00 && functionSet) { // Check if the key is not null
          Keyboard.press(functionKeys[i][j]);
        }
        keyStates[i][j] = true;
      } else if (!active && keyStates[i][j]) {
        if (keys[i][j] != 0x00) { // Check if the key is not null
          Keyboard.release(keys[i][j]);
          Keyboard.release(functionKeys[i][j]);
        }
        keyStates[i][j] = false;
      }
    }

    digitalWrite(rowPins[i], LOW);
    delay(scanDelay); // Delay after each row scan to allow time to settle
  }
}