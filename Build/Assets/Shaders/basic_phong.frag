#version 460 core

in vec2 v_texcoord;
in vec3 v_position;
in vec3 v_normal;
flat in vec3 v_color;

out vec4 f_color;

uniform struct Light
{
	vec3 position;
	vec3 color;
} u_light;

uniform vec3 u_ambient_light;
uniform sampler2D u_texture;

uniform struct Material 
{
sampler2D baseMap;
vec3 baseColor;

	//sampler2D texture;
	float shininess;
	vec2 tiling;
	vec2 offset;
} u_material;

vec3 calculateLight(in vec3 position, in vec3 normal)
{
// diffuse lighting
vec3 light_dir = normalize(u_light.position - position);
float intensity = max(dot(light_dir, normal), 0);
vec3 diffuse = u_light.color * u_material.baseColor * intensity ;

//specular
//vec3 reflection = reflect(-light_dir, normal);
vec3 view_dir = normalize(-position);
vec3 halfway = normalize(light_dir + view_dir);
intensity = max(dot(normal, halfway), 0);
intensity = pow(intensity, u_material.shininess);
vec3 specular = vec3(intensity);


return u_ambient_light + diffuse + specular;
}

//uniform sampler2D u_texture;
void main()
{
vec3 color = calculateLight(v_position, v_normal);
	f_color = texture(u_texture, v_texcoord) * vec4(color, 1);
}