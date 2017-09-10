#version 450 core

in vec4 _color;
in vec2 _coord;

out vec4 color;

uniform sampler2D _texture;

void main()
{
	color = _color;
	color.a *= texture2D(_texture, _coord).r;
}
