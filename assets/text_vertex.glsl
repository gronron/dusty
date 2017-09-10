#version 450 core

layout (location = 0) in vec4 vertex_position;
layout (location = 1) in vec4 vertex_color;
layout (location = 2) in vec2 texture_coord;

out vec4 _color;
out vec2 _coord;

void main()
{
    gl_Position = vertex_position;
    _color = vertex_color;
    _coord = texture_coord;
}
