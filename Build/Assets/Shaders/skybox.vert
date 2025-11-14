#version 460 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_texcoord;

out vec3 v_texcoord;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
	v_texcoord = a_position;

	mat4 vp = u_projection * mat4(mat3(u_view));
	gl_Position = vp * vec4(a_position, 1.0);
}
