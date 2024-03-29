#ifndef WINGS_BASE_BITMAP_FONT_C_
#define WINGS_BASE_BITMAP_FONT_C_

#include "wings/base/types.h"
#include "wings/base/error_codes.c"
#include "wings/base/strings.c"
#include "wings/base/math.c"
#include "wings/base/allocators.c"
#include "wings/graphics/image.c"

struct glyph
{
   s32       id;
   struct v2 size;
   struct v2 uv_min;
   struct v2 uv_max;
   struct v2 offset;
   f32       advance;
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

error
bitmap_font_from_text(struct bitmap_font *font, struct string_view text, struct allocator *allocator)
{
   s32 glyph_count = 0;
   u16 image_width = 0, image_height = 0;

   struct string_view context = text;
   while (context.length)
   {
      struct string_view line = string_view__set_to_next_line(&context);
      if (cstring__begins_with(line.start, line.length, "common", 6))
      {
         u16 unused;
         u16 line_height = 0, base_line = 0;
         sscanf_s(line.start, "common lineHeight=%hu base=%hu scaleW=%hu scaleH=%hu pages=%hu packed=%hu alphaChnl=%hu redChnl=%hu greenChnl=%hu blueChnl=%hu",
                  &line_height, &base_line, &image_width, &image_height, &unused, &unused, &unused, &unused, &unused, &unused);

         font->line_height      = line_height;
         font->base_line_height = base_line;
      }
      else if (cstring__begins_with(line.start, line.length, "chars", 5))
      {
         sscanf_s(line.start, "chars count=%d",
                  &font->number_of_glyphs);
         error error = allocate_array(&font->glyph, allocator, font->number_of_glyphs, struct glyph);
         ASSERT(error == 0);
      }

      else if (cstring__begins_with(line.start, line.length, "page", 4))
      {
         int id = 0;
         sscanf_s(
             line.start,
             "page id=%d file=\"%s",
             &id, font->image_name,
             1024);
         char *tmp = font->image_name;
         while (*tmp)
            tmp += 1;
         tmp  = tmp - 1;
         *tmp = 0;
      }
      else if (cstring__begins_with(line.start, line.length, "char", 4))
      {
         if (!font->glyph)
            return (1);
         s16 Id, X, Y, Width, Height, x_offset, y_offset, XAdvance;
         sscanf_s(line.start, "char id=%hd   x=%hd   y=%hd    width=%hd     height=%hd     xoffset=%hd     yoffset=%hd     xadvance=%hd",
                  &Id, &X, &Y, &Width, &Height, &x_offset, &y_offset, &XAdvance);

         font->glyph[glyph_count].id       = Id;
         font->glyph[glyph_count].size.x   = Width;
         font->glyph[glyph_count].size.y   = Height;
         font->glyph[glyph_count].uv_min   = (struct v2) { (f32)X / image_width, (f32)Y / image_height };
         font->glyph[glyph_count].uv_max   = (struct v2) { (f32)(X + Width) / image_width, (f32)(Y + Height) / image_height };
         font->glyph[glyph_count].offset.x = (f32)x_offset;
         font->glyph[glyph_count].offset.y = (f32)y_offset;
         font->glyph[glyph_count].advance  = XAdvance;
         glyph_count++;
      }
   }
   return (ec__no_error);
}

b32
load_bitmap_font(struct bitmap_font *font, char *path, struct allocator *allocator)
{
   struct buffer buffer = { 0 };
   b32           error  = 0;
   error                = file_read(&buffer, path, 1, allocator);
   if (error)
      return (error);
   struct string_view text = { .start = (char *)buffer.base, .length = buffer.size - 1 };
   bitmap_font_from_text(font, text, allocator);

   error = load_image(&font->image, font->image_name, allocator);
   if (error)
      return (error);
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
