/*
 Created by: 	KopyTKG
 Date:		2024-06-20
 License:	CC0 v1.0
*/

// Pin configuration
static const int columnPins[4] = {2, 4, 7, 8};
static const int rowPins[5] = {3, 5, 6, 9, 10};

// LED pins
static const int LEDs[2] = {15, 16};


// Key layout cap
static const int keyLayoutCap = 4;

// Layout counter
static int mode = 0;

// Key state array
static bool keyStates[5][4] = {0};

// Timers
// Scan delay
static const int scanDelay = 1;
// Debounce delay
static const int debounceDelay = 5;
