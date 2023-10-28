#ifndef WINGS_INPUT_KEYBOARD_C_
#define WINGS_INPUT_KEYBOARD_C_

#if !defined(WINGS_BASE_TYPES_C_)
#include "wings/base/types.c"
#endif

struct keyboard
{
   s32 chars_typed;
   s32 chars[10];
   u16 key_state[200];
   u16 key_state_last_frame[200];
   b32 any_key_pressed;
   b32 any_key_released;
   b32 alt_key_held;
   b32 ctrl_key_held;
} keyboard = { 0 };

enum keycode
{
   keycode_Escape,
   keycode_F1,
   keycode_F2,
   keycode_F3,
   keycode_F4,
   keycode_F5,
   keycode_F6,
   keycode_F7,
   keycode_F8,
   keycode_F9,
   keycode_F10,
   keycode_F11,
   keycode_F12,

   keycode_Tilde,
   keycode_1,
   keycode_2,
   keycode_3,
   keycode_4,
   keycode_5,
   keycode_6,
   keycode_7,
   keycode_8,
   keycode_9,
   keycode_0,
   keycode_Minus,
   keycode_Equals,
   keycode_Backspace,

   keycode_Tab,
   keycode_Q,
   keycode_W,
   keycode_E,
   keycode_R,
   keycode_T,
   keycode_Y,
   keycode_U,
   keycode_I,
   keycode_O,
   keycode_P,
   keycode_LeftBracket,
   keycode_RightBracket,
   keycode_BackSlash,

   keycode_CapsLock,
   keycode_A,
   keycode_S,
   keycode_D,
   keycode_F,
   keycode_G,
   keycode_H,
   keycode_J,
   keycode_K,
   keycode_L,
   keycode_SemiColon,
   keycode_Tick,
   keycode_Enter,

   keycode_LeftShift,
   keycode_Z,
   keycode_X,
   keycode_C,
   keycode_V,
   keycode_B,
   keycode_N,
   keycode_M,
   keycode_Comma,
   keycode_Period,
   keycode_Slash,
   keycode_RightShift,

   keycode_LeftControl,
   keycode_LeftSuper,
   keycode_Alt,
   keycode_Space,
   keycode_RightAlt,
   keycode_RightSuper,
   keycode_RightMeta,
   keycode_RightControl,

   keycode_Up,
   keycode_Left,
   keycode_Down,
   keycode_Right,

   keycode_Delete,
};

b32
altkey_held(void)
{
   return keyboard.alt_key_held;
}

b32
ctrlkey_held(void)
{
   return keyboard.ctrl_key_held;
}

b32
key_held(enum keycode key)
{
   return keyboard.key_state[key] > 0;
}

b32
key_released(enum keycode key)
{
   return keyboard.key_state[key] == 0 && keyboard.key_state_last_frame[key] > 0;
}

b32
key_pressed(enum keycode key)
{
   return keyboard.key_state[key] != 0 && keyboard.key_state_last_frame[key] == 0;
}

b32
key_repeated(enum keycode key)
{
   return keyboard.key_state_last_frame[key] < keyboard.key_state[key];
}

b32
any_key_pressed(void)
{
   return keyboard.any_key_pressed;
}

b32
any_key_released(void)
{
   return keyboard.any_key_released;
}

#endif
