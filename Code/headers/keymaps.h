#ifndef KEYS_HEADER_H
#define KEYS_HEADER_H

static const char DefaultLayout[5][4] = {
  {'-', '*', '/', KEY_NUM_LOCK}, 
  {0x00, '9', '8', '7'}, 
  {'+', '6', '5', '4'},
  {0x00, '3', '2', '1'}, 
  {KEY_RETURN, '.', 0x00, '0'} 
};

static const char FunctionLayout[5][4] = {
  {'-', '*', '/', KEY_NUM_LOCK}, 
  {0x00, KEY_F14, KEY_F15, KEY_F13},
  {KEY_F24, KEY_F18, KEY_F17, KEY_F16}, 
  {0x00, KEY_F22, KEY_F21, KEY_F20},
  {KEY_RETURN, KEY_F23, 0x00, KEY_F19}  
};


static const char PrefixKeys[4] = {
 0x00, KEY_LEFT_CTRL, KEY_LEFT_SHIFT, KEY_LEFT_ALT
};

#endif // KEYS_HEADER_H
