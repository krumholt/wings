#ifndef BITMAP_FONT_C_
#define BITMAP_FONT_C_

#include "wings/base/types.c"
#include "wings/base/allocators.c"

#include "wings/geometry_2d.c"
#include "image.c"
#include "wings_math.c"
#include "wings_strings.c"

struct glyph
{
    s32         id;
    struct v2   size;
    struct aab2 uv;
    struct v2   offset;
    f32         advance;
};

struct bitmap_font
{
    struct glyph *glyph;
    u32           number_of_glyphs;
    f32           line_height;
    f32           base_line_height;
    char          image_name[1024];
    struct image  image;
};

void
bitmap_font_from_text(struct bitmap_font *font, struct string text, struct allocator *allocator)
{
    s32 glyph_count = 0;
    u16 image_width = 0, image_height = 0;

    struct string context = text;
    while (context.size)
    {
        struct string line = next_line(&context);
        if (begins_with_cstring(line.base, line.size, "common", 6))
        {
            u16 unused;
            u16 line_height = 0, base_line = 0;
            sscanf_s(line.base, "common lineHeight=%hu base=%hu scaleW=%hu scaleH=%hu pages=%hu packed=%hu alphaChnl=%hu redChnl=%hu greenChnl=%hu blueChnl=%hu",
                     &line_height, &base_line, &image_width, &image_height, &unused, &unused, &unused, &unused, &unused, &unused);

            font->line_height      = line_height;
            font->base_line_height = base_line;
        }
        else if (begins_with_cstring(line.base, line.size, "chars", 5))
        {
            sscanf_s(line.base, "chars count=%d",
                     &font->number_of_glyphs);
            error error = allocate_array(&font->glyph, allocator, font->number_of_glyphs, struct glyph);
			ASSERT(error == 0);
        }

        else if (begins_with_cstring(line.base, line.size, "page", 4))
        {
            int id = 0;
            sscanf_s(
                line.base,
                "page id=%d file=\"%s",
                &id, font->image_name,
                1024);
            char *tmp = font->image_name;
            while (*tmp)
                tmp += 1;
            tmp  = tmp - 1;
            *tmp = 0;
        }
        else if (begins_with_cstring(line.base, line.size, "char", 4))
        {
            s16 Id, X, Y, Width, Height, x_offset, y_offset, XAdvance;
            sscanf_s(line.base, "char id=%hd   x=%hd   y=%hd    width=%hd     height=%hd     xoffset=%hd     yoffset=%hd     xadvance=%hd",
                     &Id, &X, &Y, &Width, &Height, &x_offset, &y_offset, &XAdvance);

            font->glyph[glyph_count].id     = Id;
            font->glyph[glyph_count].size.x = Width;
            font->glyph[glyph_count].size.y = Height;
            font->glyph[glyph_count].uv     = (struct aab2) {
                {(f32)X / image_width,            (f32)Y / image_height           },
                { (f32)(X + Width) / image_width, (f32)(Y + Height) / image_height}
            };
            font->glyph[glyph_count].offset.x = (f32)x_offset;
            font->glyph[glyph_count].offset.y = (f32)y_offset;
            font->glyph[glyph_count].advance  = XAdvance;
            glyph_count++;
        }
    }
}

b32
load_bitmap_font(struct bitmap_font *font, char *path, struct allocator *allocator)
{
	struct buffer buffer = {0};
    b32 error = 0;
    error     = read_file(&buffer, path, 1, allocator);
    if (error)
        return 1;
    struct string text = { (char *)buffer.data, buffer.size - 1 };
    bitmap_font_from_text(font, text, allocator);

    error = load_image(&font->image, font->image_name, allocator);
    if (error)
        return 2;
    return 0;
}

struct glyph
get_glyph(struct bitmap_font font, int c)
{
    s32 unknown_glyph_index = 0;
    for (u32 i = 0; i < font.number_of_glyphs; ++i)
    {
        if (font.glyph[i].id == -1)
            unknown_glyph_index = i;
        if (font.glyph[i].id == c)
            return font.glyph[i];
    }
    return font.glyph[unknown_glyph_index];
}

#endif
