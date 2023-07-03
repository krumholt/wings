#ifndef file_selector_c
#define file_selector_c

#include "color.c"
#include "file.h"
#include "file_windows.c"
#include "geometry_2d.c"
#include "memory.c"
#include "mesh.c"
#include "path.c"
#include "text_renderer.c"
#include "types.h"
#include "wings_math.c"

struct file_selector
{
    struct file_description *file;
    struct path             *selected_path;
    struct path              current_path;
    s32                      selected_file_changed;
    s32                      current_page;
    s32                      directory_size;
    u32                      files_used;
    u32                      files_size;
    b32                      show_drive_selector;
    b32                      folder_selectable;
    file_filter_function    *file_filter;
    u32                      drives;

    struct v4 heading_color;
    struct v4 text_color;
    struct v4 highlight_color;
    struct v4 background_color;
    f32       width;
};

void
update_listing(struct file_selector *file_selector)
{
    if (file_selector->selected_path)
    {
        file_selector->selected_path         = 0;
        file_selector->selected_file_changed = 1;
    }
    file_selector->files_used     = file_selector->files_size;
    file_selector->directory_size = list_directory(
        file_selector->current_path,
        file_selector->current_page * file_selector->files_size,
        file_selector->file,
        &file_selector->files_used,
        file_selector->file_filter);
    int start_index = 0;
    int end_index   = file_selector->files_used - 1;
    while (start_index < end_index)
    {
        while (start_index < end_index && file_selector->file[start_index].is_directory)
            ++start_index;
        while (start_index < end_index && !file_selector->file[end_index].is_directory)
            --end_index;
        if (start_index < end_index)
        {
            struct file_description tmp_file_description = file_selector->file[start_index];
            file_selector->file[start_index]             = file_selector->file[end_index];
            file_selector->file[end_index]               = tmp_file_description;
            --end_index;
            ++start_index;
        }
    }
}

void
set_current_path(struct file_selector *file_selector, struct path path)
{
    if (file_selector->selected_path)
    {
        file_selector->selected_path         = 0;
        file_selector->selected_file_changed = 1;
    }
    file_selector->current_page = 0;
    get_full_path_name(&file_selector->current_path, path.string);
    update_listing(file_selector);
}

struct file_selector
make_file_selector(struct memory *memory, s32 number_of_rows)
{
    struct file_selector file_selector = { 0 };
    file_selector.files_used           = 0;
    file_selector.file_filter          = 0;
    file_selector.files_size           = number_of_rows;
    file_selector.file                 = allocate_array(memory,
                                                        file_selector.files_size,
                                                        struct file_description);
    file_selector.width                = 500.0f;
    file_selector.heading_color        = color_red;
    file_selector.text_color           = color_red;
    file_selector.highlight_color      = color_barbie_pink;
    file_selector.background_color     = color_barbie_pink;

    get_current_directory(&file_selector.current_path);
    update_listing(&file_selector);

    return (file_selector);
}

void
set_page(struct file_selector *file_selector, int page_number)
{
    page_number                 = clamp_s32(page_number, 0, file_selector->directory_size / file_selector->files_size);
    file_selector->current_page = page_number;
    update_listing(file_selector);
}

void
update_file_selector(struct file_selector        *file_selector,
                     struct v2                    file_selector_position,
                     struct text_renderer_buffer *text_buffer,
                     struct bitmap_font           font,
                     struct mesh_p3c4            *mesh)
{
    file_selector->selected_file_changed = 0;
    float     row_height                 = font.line_height;
    float     file_selector_width        = file_selector->width;
    struct v4 heading_color              = file_selector->heading_color;
    struct v4 text_color                 = file_selector->text_color;
    struct v4 highlight_color            = file_selector->highlight_color;
    struct v4 background_color           = file_selector->background_color;
    f32 text_layer = 0.9f;
    f32 background_layer = 0.1f;
   
    struct aab2 row = {
        file_selector_position, {file_selector_position.x + file_selector_width, file_selector_position.y + row_height}
    };
    f32       total_height   = (file_selector->files_used + 3) * (row_height + 4.0f);
    struct v2 mouse_position = { mouse.position.x, mouse.position.y };

    struct aab2 background_aab = {
        {row.min.x - 5.0f, row.min.y               },
        { row.max.x,       row.min.y + total_height}
    };
    background_aab = grow_aab2(background_aab, 10.0f);
    mesh_p3c4_push_aab2(mesh, background_aab, background_layer, background_color);

    if (!file_selector->show_drive_selector)
    { // render current path
        s32         guessed_path_length = (file_selector_width / 12);
        char       *text                = file_selector->current_path.string;
        struct v3   text_position       = { row.min.x, row.min.y, 0.0f };
        struct aab2 outline             = push_text_limited(text_buffer, font, text_position, text, strlen(text), file_selector->width, heading_color);
        outline                         = grow_aab2(outline, 2.0f);
        mesh_p3c4_push_aab2_outline(mesh, outline, text_layer, 1.0f, heading_color);
        struct aab2 drive_selection = outline;
        drive_selection.max.x       = drive_selection.min.x + 20.0f;
        if (left_mouse_button_released()
            && is_point_in_aab2(mouse_position, drive_selection))
        {
            file_selector->show_drive_selector = 1;
            file_selector->drives              = get_drives();
        }
        row.min.y = outline.max.y;
        row.max.y = row.min.y + row_height;
    }
    else
    { // render drive selector
        struct v2 position = row.min;
        for (u32 index = 0; index < 32; ++index)
        {
            u32 drive = 1u << index;
            if (drive & file_selector->drives)
            {
                char        text[3]       = { (char)('A' + index), ':', 0 };
                struct v3   text_position = { position.x, position.y, text_layer };
                struct aab2 outline       = push_text(text_buffer,
                                                      font,
                                                      text_position,
                                                      text,
                                                      strlen(text),
                                                      text_color);
                position.x                = outline.max.x + 5.0f;
                outline                   = grow_aab2(outline, 1.0f);
                mesh_p3c4_push_aab2_outline(mesh,
                                            outline,
                                            text_layer,
                                            1.0f,
                                            heading_color);
                if (left_mouse_button_released()
                    && is_point_in_aab2(mouse_position, outline))
                {
                    file_selector->show_drive_selector = 0;
                    set_current_path(file_selector,
                                     make_path(text, 2));
                }
            }
        }
        row.min.y += row_height + 4.0f;
        row.max.y += row_height + 4.0f;
    }

    { // render path controls
        char     *text                   = 0;
        struct v4 deactivated_text_color = darken_color(color_space, 0.05f);
        s32       at_first_page          = file_selector->current_page == 0;
        s32       at_last_page           = file_selector->current_page == file_selector->directory_size / file_selector->files_size;
        text                             = " < ";
        struct v3   text_position        = { row.min.x, row.min.y, text_layer };
        struct aab2 page_left            = push_text(text_buffer,
                                                     font,
                                                     text_position,
                                                     text,
                                                     strlen(text),
                                          at_first_page ? deactivated_text_color : text_color);
        struct aab2 outline              = push_text(text_buffer,
                                                     font,
                                                     text_position,
                                                     text,
                                                     strlen(text),
                                                     text_color);
        text                             = "..";
        text_position                    = make_v3(page_left.max.x, row.min.y, text_layer);
        struct aab2 directory_up         = push_text(text_buffer,
                                                     font,
                                                     text_position,
                                                     text,
                                                     strlen(text),
                                                     text_color);
        text                             = " > ";
        text_position                    = make_v3(directory_up.max.x, row.min.y, text_layer);
        struct aab2 page_right           = push_text(text_buffer,
                                                     font,
                                                     text_position,
                                                     text,
                                                     strlen(text),
                                           at_last_page ? deactivated_text_color : text_color);

        // rmaxer_rectangle_outline(page_left,    0.3f, 1.0f, heading_color);
        // rmaxer_rectangle_outline(directory_up, 0.3f, 1.0f, heading_color);
        // rmaxer_rectangle_outline(page_right,   0.3f, 1.0f, heading_color);
        if (left_mouse_button_released())
        {
            if (is_point_in_aab2(mouse_position, page_left))
            {
                set_page(file_selector, file_selector->current_page - 1);
            }
            else if (is_point_in_aab2(mouse_position, directory_up))
            {
                set_to_parent(&file_selector->current_path);
                file_selector->current_page = 0;
                update_listing(file_selector);
            }
            else if (is_point_in_aab2(mouse_position, page_right))
            {
                set_page(file_selector, file_selector->current_page + 1);
            }
        }
    }
    row.min.y += row_height + 4.0f;
    row.max.y += row_height + 4.0f;

    for (int index = 0; index < file_selector->files_used; ++index)
    {
        struct path *p             = &file_selector->file[index].file_path;
        struct v4    current_color = text_color;
        if (p == file_selector->selected_path)
            current_color = highlight_color;
        char       *text          = path_filename(p);
        struct v3   text_position = { row.min.x, row.min.y, text_layer };
        struct aab2 outline       = push_text(text_buffer,
                                              font,
                                              text_position,
                                              text,
                                              strlen(text),
                                              current_color);
        if (file_selector->file[index].is_directory)
        {
            mesh_p3c4_push_line_v2(mesh,
                                   make_v2(outline.min.x, outline.max.y + 2.0f),
                                   make_v2(outline.max.x, outline.max.y + 2.0f),
                                   0.0f,
                                   1.0f,
                                   color_space);
        }
        mesh_p3c4_push_aab2_outline(mesh,
                                    row,
                                    -0.3f,
                                    1.0f,
                                    color_space);
        if (is_point_in_aab2(mouse_position, row))
        {
            struct aab2 selection_dot = {
                add_v2(outline.min, make_v2(-11.0f, 6.0f)),
                add_v2(outline.min, make_v2(-11.0f + 6.0f, 6.0f + 6.0f))
            };
            mesh_p3c4_push_aab2(mesh, selection_dot, 0.0f, color_space);
            if (left_mouse_button_released())
            {
                if (file_selector->file[index].is_directory)
                {
                    if (file_selector->selected_path == &file_selector->file[index].file_path)
                    {
                        set_current_path(file_selector, file_selector->file[index].file_path);
                    }
                    else
                    {
                        file_selector->selected_file_changed = 1;
                        file_selector->selected_path         = &file_selector->file[index].file_path;
                    }
                }
                else
                {
                    if (file_selector->selected_path != &file_selector->file[index].file_path)
                        file_selector->selected_file_changed = 1;
                    file_selector->selected_path = &file_selector->file[index].file_path;
                }
            }
        }
        row.min.y += row_height + 4.0f;
        row.max.y += row_height + 4.0f;
    }
}

#endif
