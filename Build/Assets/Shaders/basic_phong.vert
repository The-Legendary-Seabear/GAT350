#version 460 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_texcoord;
layout (location = 2) in vec3 a_normal;
layout (location = 3) in vec3 a_tangent;

out VS_OUT
{
vec2 texcoord;
vec3 position;
vec3 normal;
mat3 tbn;
} vs_out;

uniform struct Material 
{
vec3 baseColor;
vec3 emissiveColor;
float shininess;
vec2 tiling;
vec2 offset;
uint parameters;
} u_material;

//flat out vec3 v_color;


uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
vs_out.texcoord = (a_texcoord * u_material.tiling) + u_material.offset;
	mat4 model_view = u_view * u_model;
	mat3 normal_matrix = transpose(inverse(mat3(model_view)));
	vs_out.texcoord = (a_texcoord * u_material.tiling) + u_material.offset;
	vs_out.position = vec3(model_view * vec4(a_position, 1));
	vs_out.normal = normalize(mat3(model_view) * a_normal);

	vec3 N = normalize(normal_matrix * a_normal);
	vec3 T = normalize(normal_matrix * a_tangent);
	// re-orthogonalize T with respect to N
	T = normalize(T - dot(T, N) * N);
	vec3 B = normalize(cross(N, T));
	vs_out.tbn = mat3(T, B, N);

	gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0);
}
