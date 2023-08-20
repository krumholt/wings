#ifndef MOUSE_C_
#define MOUSE_C_

#include "wings/base/math.c"

enum mouse_button
{
    _mouse_button_left,
    _mouse_button_right,
    _mouse_button_middle,
};

struct mouse
{
    struct v2s  position;
    struct v2s  position_last_frame;
    struct v2s  position_delta;
    b32         button[8];
    b32         button_last_frame[8];
    s16         wheel_delta;
} mouse = {0};

inline b32
left_mouse_button_pressed(void)
{
    return(mouse.button[_mouse_button_left] && !mouse.button_last_frame[_mouse_button_left]);
}

inline b32
left_mouse_button_held(void)
{
    return(mouse.button[_mouse_button_left]);
}

inline b32
left_mouse_button_released(void)
{
    return(!mouse.button[_mouse_button_left] && mouse.button_last_frame[_mouse_button_left]);
}

inline b32
right_mouse_button_pressed(void)
{
    return(mouse.button[_mouse_button_right] && !mouse.button_last_frame[_mouse_button_right]);
}

inline b32
right_mouse_button_held(void)
{
    return(mouse.button[_mouse_button_right]);
}

inline b32
right_mouse_button_released(void)
{
    return(!mouse.button[_mouse_button_right] && mouse.button_last_frame[_mouse_button_right]);
}

inline b32
middle_mouse_button_pressed(void)
{
    return mouse.button[_mouse_button_middle] && !mouse.button_last_frame[_mouse_button_middle];
}

inline b32
middle_mouse_button_held(void)
{
    return mouse.button[_mouse_button_middle];
}

inline b32
middle_mouse_button_released(void)
{
    return !mouse.button[_mouse_button_middle] && mouse.button_last_frame[_mouse_button_middle];
}

#endif
