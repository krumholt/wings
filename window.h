#ifndef window_h
#define window_h
#include "wings/base/types.c"
#include "wings/base/math.c"


struct window;


b32  open_window(char *title, s32 width, s32 height);
void update_window(void);
void set_window_position(struct v2s new_position);

void console_create(void);
void console_destroy(void);

#endif
