#version 460 core

layout (location = 0) in vec3 a_position;
layout (location = 2) in vec3 a_normal;

out vec3 v_texcoord;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
	//transform position and normal to world space
	vec3 position = vec3(u_model * vec4(a_position, 1));
	vec3 normal = normalize(mat3(u_model) * a_normal);

	vec3 viewPosition = inverse(u_view)[3].xyz;
	vec3 viewDir = normalize(position - viewPosition);

	v_texcoord = reflect(viewDir, normal);
	
	gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0);
}
