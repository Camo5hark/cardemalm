#version 300 es
layout(location = 0) in vec2 attr_position;
void main(void)
{
    gl_Position = vec4(attr_position, 0.0, 1.0);
}
