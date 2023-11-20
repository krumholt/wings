#ifndef WINGS_GRAPHICS_TEXT_RENDERER_C_
#define WINGS_GRAPHICS_TEXT_RENDERER_C_

#include "wings/base/macros.c"
#include "wings/base/types.h"
#include "wings/base/math.c"
#include "wings/base/shapes.c"
#include "wings/graphics/bitmap_font.c"
#include "wings/graphics/image.c"
#include "wings/graphics/opengl.c"

const char *text_renderer_vertex_shader_text
    = "#version 330\n\n"
      "\n"
      "layout (location = 0) in vec3 position;\n"
      "layout (location = 1) in vec2 uv;\n"
      "layout (location = 2) in vec4 tint;\n"
      "\n"
      "out vertex_data_t\n"
      "{\n"
      "    vec2 uv;\n"
      "    vec4 tint;\n"
      "} vertex_data;\n"
      "uniform mat4 projection;\n"
      "uniform mat4 view;\n"
      "void main()\n"
      "{\n"
      "    vertex_data.uv = uv;\n"
      "    vertex_data.tint = tint;\n"
      "    gl_Position = projection * view * vec4(position, 1.0);\n"
      "}";

const char *text_renderer_fragment_shader_text
    = "#version 330\n\n"
      "uniform sampler2D font_texture;\n"
      "in vertex_data_t\n"
      "{\n"
      "    vec2 uv;\n"
      "    vec4 tint;\n"
      "} vertex_data;\n"
      "out vec4 color;\n"
      "void main()\n"
      "{\n"
      "    vec4 albedo = texture2D(font_texture, vertex_data.uv);\n"
      "    albedo *= vertex_data.tint;\n"
      "    if (albedo.a <= 0.1)\n"
      "        discard;\n"
      "    float gamma = 2.2;\n"
      "    color = vec4(pow(albedo.rgb, vec3(1.0/gamma)), albedo.a);\n"
      "}";

struct text_renderer_vertex
{
   struct v3 position;
   struct v2 uv;
   struct v4 color;
};

struct text_renderer_buffer
{
   struct text_renderer_vertex *vertex;
   u32                          used;
   u32                          size;

   u32                 va;
   u32                 vb;
   struct bitmap_font *font;
   u32                 texture;
};

struct text_renderer_context
{
   u32 shader_program;
   s32 projection;
   s32 view;
   b32 y_up;
   u32 font_texture;
} text_renderer_context;

error
initialise_text_renderer(void)
{
   error error = compile_shader_program(
       &text_renderer_context.shader_program,
       text_renderer_vertex_shader_text,
       text_renderer_fragment_shader_text);
   if (error)
      return (1);
   text_renderer_context.view       = glGetUniformLocation(text_renderer_context.shader_program, "view");
   text_renderer_context.projection = glGetUniformLocation(text_renderer_context.shader_program, "projection");
   text_renderer_context.y_up       = 0;
   if (text_renderer_context.view == -1 || text_renderer_context.projection == -1)
   {
      return (2);
   }
   return (0);
}

u32
make_font_texture(struct image image)
{
   u32 texture;
   glGenTextures(1, &texture);
   glBindTexture(GL_TEXTURE_2D, texture);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.raw_data);
   glGenerateMipmap(GL_TEXTURE_2D);
   return (texture);
}

struct text_renderer_buffer
make_text_renderer_buffer(u32 number_of_characters, struct bitmap_font *font, struct allocator *allocator)
{
   struct text_renderer_buffer buffer = { 0 };
   buffer.size                        = number_of_characters * 6;
   error error                        = allocate_array(&buffer.vertex, allocator, buffer.size, struct text_renderer_vertex);
   ASSERT(error == 0); //@TODO:@FIXME
   buffer.font = font;

   glGenVertexArrays(1, &buffer.va);
   glBindVertexArray(buffer.va);
   glGenBuffers(1, &buffer.vb);
   glBindBuffer(GL_ARRAY_BUFFER, buffer.vb);
   glBufferData(GL_ARRAY_BUFFER, buffer.size * (s64)sizeof(struct text_renderer_vertex), 0, GL_DYNAMIC_DRAW);

   glEnableVertexAttribArray(0);
   glEnableVertexAttribArray(1);
   glEnableVertexAttribArray(2);
   glVertexAttribPointer(0, 3, GL_FLOAT, 0, sizeof(struct text_renderer_vertex), (void *)offsetof(struct text_renderer_vertex, position));
   glVertexAttribPointer(1, 2, GL_FLOAT, 0, sizeof(struct text_renderer_vertex), (void *)offsetof(struct text_renderer_vertex, uv));
   glVertexAttribPointer(2, 4, GL_FLOAT, 0, sizeof(struct text_renderer_vertex), (void *)offsetof(struct text_renderer_vertex, color));

   glBindVertexArray(0);

   buffer.texture = make_font_texture(font->image);

   return (buffer);
}

void
upload_text_renderer_buffer(struct text_renderer_buffer buffer)
{
   glBindBuffer(GL_ARRAY_BUFFER, buffer.vb);
   glBufferSubData(GL_ARRAY_BUFFER, 0, buffer.used * (s64)sizeof(struct text_renderer_vertex), buffer.vertex);
}

void
render_text_renderer_buffer(struct text_renderer_buffer buffer, struct mat4 projection, struct mat4 view)
{
   if (buffer.used == 0)
      return;
   glUseProgram(text_renderer_context.shader_program);
   glUniformMatrix4fv(text_renderer_context.projection, 1, GL_FALSE, (f32 *)&projection);
   glUniformMatrix4fv(text_renderer_context.view, 1, GL_FALSE, (f32 *)&view);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, buffer.texture);

   glBindVertexArray(buffer.va);
   upload_text_renderer_buffer(buffer);

   glDrawArrays(GL_TRIANGLES, 0, buffer.used);
   glBindVertexArray(0);
}

void
clear_text_renderer_buffer(struct text_renderer_buffer *buffer)
{
   buffer->used = 0;
}

struct aab2
push_text_v2(struct text_renderer_buffer *buffer,
             struct v2                    position_v2,
             f32                          layer,
             char                        *text,
             int                          size,
             struct v4                    color)
{
   struct v3   position = { position_v2.x, position_v2.y, layer };
   struct aab2 outline  = {
      {position.x, position.y                            },
      { 0.0f,      position.y + buffer->font->line_height}
   };
   if (buffer->used + (size * 6) > buffer->size)
   {
      printf("Warning: push_text but text buffer is too small %d/%d\n", buffer->used, buffer->size);
      return outline;
   }

   struct text_renderer_vertex vertex = { 0 };

   struct glyph glyph = { 0 };
   for (; *text && size > 0; text += 1, size -= 1)
   {
      glyph = get_glyph(*buffer->font, *text);

      // f32       height          = glyph.size.y;
      struct v3 offset_y_down[] = {
         {glyph.offset.x + 0.0f,          glyph.offset.y + 0.0f,         0.0f},
         { glyph.offset.x + 0.0f,         glyph.offset.y + glyph.size.y, 0.0f},
         { glyph.offset.x + glyph.size.x, glyph.offset.y + 0.0f,         0.0f},
         { glyph.offset.x + glyph.size.x, glyph.offset.y + glyph.size.y, 0.0f},
         { glyph.offset.x + glyph.size.x, glyph.offset.y + 0.0f,         0.0f},
         { glyph.offset.x + 0.0f,         glyph.offset.y + glyph.size.y, 0.0f},
      };

      f32       y_offset_y_up = buffer->font->base_line_height - glyph.offset.y - glyph.size.y;
      struct v3 offset_y_up[] = {
         {glyph.offset.x + 0.0f,          y_offset_y_up + glyph.size.y, 0.0f},
         { glyph.offset.x + 0.0f,         y_offset_y_up + 0.0f,         0.0f},
         { glyph.offset.x + glyph.size.x, y_offset_y_up + glyph.size.y, 0.0f},
         { glyph.offset.x + glyph.size.x, y_offset_y_up + 0.0f,         0.0f},
         { glyph.offset.x + glyph.size.x, y_offset_y_up + glyph.size.y, 0.0f},
         { glyph.offset.x + 0.0f,         y_offset_y_up + 0.0f,         0.0f},
      };
      struct v2 uv[] = {
         {glyph.uv_min.x,  glyph.uv_min.y},
         { glyph.uv_min.x, glyph.uv_max.y},
         { glyph.uv_max.x, glyph.uv_min.y},
         { glyph.uv_max.x, glyph.uv_max.y},
         { glyph.uv_max.x, glyph.uv_min.y},
         { glyph.uv_min.x, glyph.uv_max.y},
      };
      struct v3 *offset = offset_y_down;
      if (text_renderer_context.y_up)
      {
         offset = offset_y_up;
      }
      for (u32 index = 0; index < 6; ++index)
      {
         vertex.position                = add_v3(position, offset[index]);
         vertex.uv                      = uv[index];
         vertex.color                   = color;
         buffer->vertex[buffer->used++] = vertex;
      }
      position.x += glyph.advance;
   }
   outline.max.x = position.x;
   return (outline);
}
struct aab2
push_text(struct text_renderer_buffer *buffer,
          struct v3                    position,
          char                        *text,
          int                          size,
          struct v4                    color)
{
   struct aab2 outline = {
      {position.x, position.y                            },
      { 0.0f,      position.y + buffer->font->line_height}
   };
   if (buffer->used + (size * 6) > buffer->size)
   {
      printf("Warning: push_text but text buffer is too small %d/%d\n", buffer->used, buffer->size);
      return outline;
   }

   struct text_renderer_vertex vertex = { 0 };

   struct glyph glyph = { 0 };
   for (; *text && size > 0; text += 1, size -= 1)
   {
      glyph = get_glyph(*buffer->font, *text);

      // f32       height   = glyph.size.y;
      struct v3 offset[] = {
         {glyph.offset.x + 0.0f,          glyph.offset.y + 0.0f,         0.0f},
         { glyph.offset.x + 0.0f,         glyph.offset.y + glyph.size.y, 0.0f},
         { glyph.offset.x + glyph.size.x, glyph.offset.y + 0.0f,         0.0f},
         { glyph.offset.x + glyph.size.x, glyph.offset.y + glyph.size.y, 0.0f},
         { glyph.offset.x + glyph.size.x, glyph.offset.y + 0.0f,         0.0f},
         { glyph.offset.x + 0.0f,         glyph.offset.y + glyph.size.y, 0.0f},
      };
      struct v2 uv[] = {
         {glyph.uv_min.x,  glyph.uv_min.y},
         { glyph.uv_min.x, glyph.uv_max.y},
         { glyph.uv_max.x, glyph.uv_min.y},
         { glyph.uv_max.x, glyph.uv_max.y},
         { glyph.uv_max.x, glyph.uv_min.y},
         { glyph.uv_min.x, glyph.uv_max.y},
      };
      for (u32 index = 0; index < 6; ++index)
      {
         vertex.position                = add_v3(position, offset[index]);
         vertex.uv                      = uv[index];
         vertex.color                   = color;
         buffer->vertex[buffer->used++] = vertex;
      }
      position.x += glyph.advance;
   }
   outline.max.x = position.x;
   return (outline);
}

struct aab2
push_text_limited(struct text_renderer_buffer *buffer, struct v3 position, char *text, int size, f32 limit, struct v4 color)
{
   assert(buffer->used + (size * 6) <= buffer->size);
   struct aab2 outline = {
      {position.x, position.y},
      { 0.0f,      0.0f      }
   };

   struct text_renderer_vertex vertex = { 0 };

   f32          start_x = position.x;
   f32          base    = buffer->font->base_line_height;
   struct glyph glyph   = { 0 };
   for (; *text && size > 0; text += 1, size -= 1)
   {
      glyph = get_glyph(*buffer->font, *text);
      if (position.x + glyph.size.x > limit)
      {
         if (position.x > outline.max.x)
            outline.max.x = position.x;
         position.x = start_x;
         position.y += base;
      }

      // f32       height   = glyph.size.y;
      struct v3 offset[] = {
         {glyph.offset.x + 0.0f,          glyph.offset.y + 0.0f,         0.0f},
         { glyph.offset.x + 0.0f,         glyph.offset.y + glyph.size.y, 0.0f},
         { glyph.offset.x + glyph.size.x, glyph.offset.y + 0.0f,         0.0f},
         { glyph.offset.x + glyph.size.x, glyph.offset.y + glyph.size.y, 0.0f},
         { glyph.offset.x + glyph.size.x, glyph.offset.y + 0.0f,         0.0f},
         { glyph.offset.x + 0.0f,         glyph.offset.y + glyph.size.y, 0.0f},
      };
      struct v2 uv[] = {
         {glyph.uv_min.x,  glyph.uv_min.y},
         { glyph.uv_min.x, glyph.uv_max.y},
         { glyph.uv_max.x, glyph.uv_min.y},
         { glyph.uv_max.x, glyph.uv_max.y},
         { glyph.uv_max.x, glyph.uv_min.y},
         { glyph.uv_min.x, glyph.uv_max.y},
      };
      for (u32 index = 0; index < 6; ++index)
      {
         vertex.position                = add_v3(position, offset[index]);
         vertex.uv                      = uv[index];
         vertex.color                   = color;
         buffer->vertex[buffer->used++] = vertex;
      }
      position.x += glyph.advance;
   }
   if (outline.max.x == 0.0f)
      outline.max.x = position.x;
   outline.max.y = position.y + buffer->font->line_height;
   return (outline);
}

struct aab2
push_text_limited_wrapped(struct text_renderer_buffer *buffer,
                          struct v3                    position,
                          char                        *text,
                          int                          size,
                          f32                          limit,
                          struct v4                    color)
{
   assert(buffer->used + (size * 6) <= buffer->size);
   struct aab2 outline = {
      {position.x, position.y},
      { 0.0f,      0.0f      }
   };

   struct text_renderer_vertex vertex = { 0 };

   f32          start_x = position.x;
   f32          base    = buffer->font->base_line_height;
   struct glyph glyph   = { 0 };
   for (; *text && size > 0; text += 1, size -= 1)
   {
      if (*text == '\n')
      {
         position.x = start_x;
         position.y += base;
         continue;
      }
      glyph                 = get_glyph(*buffer->font, *text);
      char *tmp             = text;
      u32   tmp_size        = size;
      f32   next_word_limit = position.x;
      while (*tmp && tmp_size)
      {
         next_word_limit += get_glyph(*buffer->font, *tmp).advance;
         if (*tmp == ' ')
         {
            next_word_limit -= get_glyph(*buffer->font, *tmp).advance;
            next_word_limit -= get_glyph(*buffer->font, *tmp).size.x;
            break;
         }
         tmp += 1;
         tmp_size -= 1;
      }
      if (next_word_limit > limit)
      {
         if (position.x > outline.max.x)
            outline.max.x = position.x;
         position.x = start_x;
         position.y += base;
      }

      // f32       height   = glyph.size.y;
      struct v3 offset[] = {
         {glyph.offset.x + 0.0f,          glyph.offset.y + 0.0f,         0.0f},
         { glyph.offset.x + 0.0f,         glyph.offset.y + glyph.size.y, 0.0f},
         { glyph.offset.x + glyph.size.x, glyph.offset.y + 0.0f,         0.0f},
         { glyph.offset.x + glyph.size.x, glyph.offset.y + glyph.size.y, 0.0f},
         { glyph.offset.x + glyph.size.x, glyph.offset.y + 0.0f,         0.0f},
         { glyph.offset.x + 0.0f,         glyph.offset.y + glyph.size.y, 0.0f},
      };
      struct v2 uv[] = {
         {glyph.uv_min.x,  glyph.uv_min.y},
         { glyph.uv_min.x, glyph.uv_max.y},
         { glyph.uv_max.x, glyph.uv_min.y},
         { glyph.uv_max.x, glyph.uv_max.y},
         { glyph.uv_max.x, glyph.uv_min.y},
         { glyph.uv_min.x, glyph.uv_max.y},
      };
      for (u32 index = 0; index < 6; ++index)
      {
         vertex.position                = add_v3(position, offset[index]);
         vertex.uv                      = uv[index];
         vertex.color                   = color;
         buffer->vertex[buffer->used++] = vertex;
      }
      position.x += glyph.advance;
   }
   if (outline.max.x == 0.0f)
      outline.max.x = position.x;
   outline.max.y = position.y + buffer->font->line_height;
   return (outline);
}

#endif
