#ifndef TEXT_3D_RENDERER_C_
#define TEXT_3D_RENDERER_C_

#include "wings/base/types.c"
#include "wings/base/math.c"
#include "wings/base/shapes.c"
#include "wings/graphics/bitmap_font.c"
#include "wings/graphics/image.c"
#include "wings/graphics/mesh_tools.c"
#include "wings/graphics/opengl.c"

const char *text_3d_renderer_vertex_shader_text
    = "#version 330\n\n"
      "\n"
      "layout (location = 0) in vec3 position;\n"
      "layout (location = 2) in vec2 uv;\n"
      "layout (location = 3) in vec4 tint;\n"
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

const char *text_3d_renderer_fragment_shader_text
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

struct text_3d_renderer_context
{
    u32                shader_program;
    s32                projection;
    s32                view;
    b32                y_up;
    u32                font_texture;
    struct bitmap_font font;
} text_3d_renderer_context;

error
initialise_text_3d_renderer(struct bitmap_font font)
{
    error error = compile_shader_program(
        &text_3d_renderer_context.shader_program,
        text_3d_renderer_vertex_shader_text,
        text_3d_renderer_fragment_shader_text);
    if (error)
        return (1);

    text_3d_renderer_context.font       = font;
    text_3d_renderer_context.view       = glGetUniformLocation(text_3d_renderer_context.shader_program, "view");
    text_3d_renderer_context.projection = glGetUniformLocation(text_3d_renderer_context.shader_program, "projection");
    if (text_3d_renderer_context.view == -1 || text_3d_renderer_context.projection == -1)
    {
        return (2);
    }

    glGenTextures(1, &text_3d_renderer_context.font_texture);
    glBindTexture(GL_TEXTURE_2D, text_3d_renderer_context.font_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, font.image.width, font.image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, font.image.raw_data);
    glGenerateMipmap(GL_TEXTURE_2D);

    return (0);
}

void
render_text_3d(struct mesh mesh, struct mat4 projection, struct mat4 view)
{
    if (mesh.used == 0)
        return;
    glUseProgram(text_3d_renderer_context.shader_program);
    glUniformMatrix4fv(text_3d_renderer_context.projection, 1, GL_FALSE, (f32 *)&projection);
    glUniformMatrix4fv(text_3d_renderer_context.view, 1, GL_FALSE, (f32 *)&view);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, text_3d_renderer_context.font_texture);

    render_mesh(mesh);
}

void
push_text_3d(struct mesh *mesh,
             struct v3    pen_position,
             struct v3    up,
             struct v3    right,
             f32          scale,
             char        *text,
             int          text_length,
             struct v4    color)
{
    ASSERT(mesh->used + (text_length * 6) <= mesh->size);
    right = mul_f32_v3(scale, normalize_v3(right));
    up    = mul_f32_v3(scale, normalize_v3(up));

    struct glyph glyph = { 0 };
    for (; *text && text_length > 0; text += 1, text_length -= 1)
    {
        glyph = get_glyph(text_3d_renderer_context.font, *text);

        struct v3 glyph_offset_right = mul_f32_v3(glyph.offset.x, right);
        f32       y_offset           = text_3d_renderer_context.font.base_line_height - glyph.offset.y - glyph.size.y;
        struct v3 glyph_offset_up    = mul_f32_v3(y_offset, up);
        struct v3 glyph_offset       = add_v3(glyph_offset_right, glyph_offset_up);
        struct v3 glyph_size_right   = mul_f32_v3(glyph.size.x, right);
        struct v3 glyph_size_up      = mul_f32_v3(glyph.size.y, up);

        struct v3 offset[] = {
            glyph_offset,
            add_v3(glyph_offset, glyph_size_right),
            add_v3(glyph_offset, glyph_size_up),
            add_v3(add_v3(glyph_offset, glyph_size_right), glyph_size_up),
            add_v3(glyph_offset, glyph_size_up),
            add_v3(glyph_offset, glyph_size_right),
        };

        struct v2 uv[] = {
            {glyph.uv_min.x,  glyph.uv_max.y},
            { glyph.uv_max.x, glyph.uv_max.y},
            { glyph.uv_min.x, glyph.uv_min.y},
            { glyph.uv_max.x, glyph.uv_min.y},
            { glyph.uv_min.x, glyph.uv_min.y},
            { glyph.uv_max.x, glyph.uv_max.y},
        };
        struct v3 *positions = (struct v3 *)(mesh->data + mesh->positions_offset);
        struct v2 *uvs       = (struct v2 *)(mesh->data + mesh->uvs_offset);
        struct v4 *colors    = (struct v4 *)(mesh->data + mesh->colors_offset);
        for (u32 index = 0; index < 6; ++index)
        {
            positions[mesh->used + index] = add_v3(pen_position, offset[index]);
            uvs[mesh->used + index]       = uv[index];
            colors[mesh->used + index]    = color;
        }
        mesh->used += 6;
        pen_position = add_v3(pen_position, mul_f32_v3(glyph.advance, right));
    }
}

/*
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
*/

#endif
