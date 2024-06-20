/*
 Created by: 	KopyTKG
 Date:		2024-06-20
 License:	CC0 v1.0
*/

#ifndef KEYS_HEADER_H
#define KEYS_HEADER_H

static const char MACRO_KEY = 0x00;
static const char NOT_POPULATED = 0x00;


static const char DefaultLayout[5][4] = {
  {'-', '*', '/', MACRO_KEY}, 
  {NOT_POPULATED, '9', '8', '7'}, 
  {'+', '6', '5', '4'},
  {NOT_POPULATED, '3', '2', '1'}, 
  {KEY_RETURN, '.', NOT_POPULATED, '0'} 
};

static const char FunctionLayout[5][4] = {
  {'-', '*', '/', MACRO_KEY}, 
  {NOT_POPULATED, KEY_F14, KEY_F15, KEY_F13},
  {KEY_F24, KEY_F18, KEY_F17, KEY_F16}, 
  {NOT_POPULATED, KEY_F22, KEY_F21, KEY_F20},
  {KEY_RETURN, KEY_F23, NOT_POPULATED, KEY_F19}  
};


static const char PrefixKeys[4] = {
 0x00, KEY_LEFT_CTRL, KEY_LEFT_SHIFT, KEY_LEFT_ALT
};

#endif // KEYS_HEADER_H
