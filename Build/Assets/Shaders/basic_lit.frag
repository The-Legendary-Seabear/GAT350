#version 460 core

in VS_OUT
{
vec2 texcoord;
vec3 color;

} fs_in;

out vec4 f_color;

uniform struct Material 
{
sampler2D baseMap;
vec3 baseColor;

	//sampler2D texture;
	float shininess;
	vec2 tiling;
	vec2 offset;
} u_material;

//uniform sampler2D u_texture;
void main()
{
f_color = texture(u_material.baseMap, fs_in.texcoord) * vec4(fs_in.color, 1);
}