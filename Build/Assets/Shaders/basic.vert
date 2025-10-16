#version 460 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_color;
layout (location = 2) in vec2 a_texcoord;

out vec3 v_color;
out vec2 v_texcoord;

uniform float u_time;
uniform mat4 u_model;

void main()
{
v_color = a_color;
v_texcoord = a_texcoord;

gl_Position = u_model * vec4(a_position, 1.0);
}

