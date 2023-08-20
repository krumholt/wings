#version 330

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 vertex_uv_1;
layout (location = 3) in vec2 vertex_uv_2;
layout (location = 4) in vec4 vertex_color;

uniform mat4 mvp;

out vec2 fragment_uv;
out vec4 fragment_color;

void main()
{
    fragment_uv = vertex_uv_1;
    fragment_color = vertex_color;

    gl_Position = mvp * vec4(vertex_position, 1.0);
}
