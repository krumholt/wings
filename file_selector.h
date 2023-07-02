#ifndef file_selector_h
#define file_selector_h

#include "file.h"
#include "memory.h"

struct file_selector_t
{
    struct memory            temporary_memory;
    struct memory            static_memory;
    struct file_description *file;
    struct path             *selected_path;
    struct path              current_path;
    s32                      selected_file_changed;
    s32                      current_page;
    s32                      directory_size;
    s32                      files_used;
    s32                      files_size;
    b32                      show_drive_selector;
    b32                      folder_selectable;
    struct file_filter      *file_filter;
    u32                      drives;
} _file_selector = {};

file_selector_t make_file_selector(struct memory *memory, int number_of_rows);
void            update_listing(struct file_selector *file_selector);
void            update_file_selector(struct file_selector *file_selector, struct v2 file_selector_position);

#endif
