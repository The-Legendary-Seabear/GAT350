#version 460 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_texcoord;
layout (location = 2) in vec3 a_normal;

out vec2 v_texcoord;
out vec3 v_position;
out vec3 v_normal;

uniform struct Material 
{
sampler2D baseMap;
vec3 baseColor;

	//sampler2D texture;
	float shininess;
	vec2 tiling;
	vec2 offset;
} u_material;

//flat out vec3 v_color;


uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
v_texcoord = (a_texcoord * u_material.tiling) + u_material.offset;
	mat4 model_view = u_view * u_model;
	vec3 position = vec3(model_view * vec4(a_position, 1));
	vec3 normal = normalize(mat3(model_view) * a_normal);

	gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0);
}
