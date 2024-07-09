/*
 Created by: 	KopyTKG
 Date:		2024-06-20
 License:	CC0 v1.0
*/

#ifndef KEYS_HEADER_H
#define KEYS_HEADER_H

static const char MACRO_KEY = 0x00;
static const char NOT_POPULATED = 0x00;


static const char Layout[5][4] = {
  {KEY_KP_MINUS, KEY_KP_ASTERISK, KEY_KP_SLASH, MACRO_KEY}, 
  {NOT_POPULATED, KEY_KP_9, KEY_KP_8, KEY_KP_7}, 
  {KEY_KP_PLUS, KEY_KP_6, KEY_KP_5, KEY_KP_4},
  {NOT_POPULATED, KEY_KP_3, KEY_KP_2, KEY_KP_1}, 
  {KEY_RETURN, KEY_KP_DOT , NOT_POPULATED, KEY_KP_0} 
};

static const char PrefixKeys[4] = {
 0x00, KEY_LEFT_CTRL, KEY_LEFT_SHIFT, KEY_LEFT_ALT
};

#endif // KEYS_HEADER_H
