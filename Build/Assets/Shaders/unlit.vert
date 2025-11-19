#version 460 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_texcoord;

out vec2 v_texcoord;

struct Material
{
	vec3 baseColor;
	vec3 emissiveColor;
	vec2 tiling;
	vec2 offset;
	uint parameters;
};

uniform Material u_material;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
	v_texcoord = a_texcoord * u_material.tiling + u_material.offset;
	
	gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0);
}
