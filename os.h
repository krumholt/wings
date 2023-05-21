#ifndef os_h
#define os_h
#include "types.h"
#include "wings_math.c"

struct keyboard
{
    s32  chars_typed;
    s32  chars[10];
    u16  key_state[200];
    u16  key_state_last_frame[200];
    b32  any_key_pressed;
    b32  any_key_released;
};

struct window;

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

b32    open_window         (char *title, s32 width, s32 height);
void   set_window_position (struct v2s new_position);

void   console_create      (void);
void   console_destroy     (void);


#endif
