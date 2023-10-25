#ifndef WINGS_INPUT_MOUSE_C_
#define WINGS_INPUT_MOUSE_C_

#if !defined(WINGS_BASE_MATH_C_)
#include "wings/base/math.c"
#endif

enum mouse_button
{
    mouse_button__left,
    mouse_button__right,
    mouse_button__middle,
    mouse_button__four,
    mouse_button__five,
    mouse_button__six,
    mouse_button__seven,
};

struct mouse
{
    struct v2s position;
    struct v2s position_last_frame;
    struct v2s position_delta;
    b32        button[8];
    b32        button_last_frame[8];
    s16        wheel_delta;
} mouse = { 0 };

b32
mouse_button_pressed(enum  mouse_button button)
{
    return (mouse.button[button] && !mouse.button_last_frame[button]);
}

b32
left_mouse_button_pressed(void)
{
    return (mouse.button[mouse_button__left] && !mouse.button_last_frame[mouse_button__left]);
}

b32
left_mouse_button_held(void)
{
    return (mouse.button[mouse_button__left]);
}

b32
left_mouse_button_released(void)
{
    return (!mouse.button[mouse_button__left] && mouse.button_last_frame[mouse_button__left]);
}

b32
right_mouse_button_pressed(void)
{
    return (mouse.button[mouse_button__right] && !mouse.button_last_frame[mouse_button__right]);
}

b32
right_mouse_button_held(void)
{
    return (mouse.button[mouse_button__right]);
}

b32
right_mouse_button_released(void)
{
    return (!mouse.button[mouse_button__right] && mouse.button_last_frame[mouse_button__right]);
}

b32
middle_mouse_button_pressed(void)
{
    return mouse.button[mouse_button__middle] && !mouse.button_last_frame[mouse_button__middle];
}

b32
middle_mouse_button_held(void)
{
    return mouse.button[mouse_button__middle];
}

b32
middle_mouse_button_released(void)
{
    return !mouse.button[mouse_button__middle] && mouse.button_last_frame[mouse_button__middle];
}

#endif
