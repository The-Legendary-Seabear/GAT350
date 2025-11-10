#version 460 core

#define MAX_LIGHTS 5
#define POINT 0
#define DIRECTIONAL 1
#define SPOT 2

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_texcoord;
layout (location = 2) in vec3 a_normal;

out VS_OUT
{
vec2 texcoord;
vec3 color;

} vs_out;


uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform vec3 u_ambient_light;


uniform struct Light 
{
int type;
vec3 position;
vec3 direction;
vec3 color;
float intensity;
float range;
float outerCutoff;
};

uniform struct Material 
{
sampler2D baseMap;
vec3 baseColor;

	//sampler2D texture;
	float shininess;
	vec2 tiling;
	vec2 offset;
};

uniform int u_numLights = 1;
uniform Light u_lights[MAX_LIGHTS];
uniform Material u_material;

float calculateAttenuation(in float light_distance, in float range) {
	float attenuation = max(0.0 ,(1.0 - (light_distance / range)));


	return pow(attenuation, 2);
}

vec3 calculateLight(in Light light,in vec3 position, in vec3 normal)
{
vec3 light_dir;
float attenuation;

switch (light.type) {
	case POINT:
	{
		light_dir = normalize(light.position - position);
		float light_distance = length(light.position - position);
		attenuation = calculateAttenuation(light_distance, light.range);
		}
	break;

	case DIRECTIONAL:
		light_dir = normalize(light.direction);
	break;

	case SPOT:
	{
		light_dir = normalize(light.position - position);
		float light_distance = length(light.position - position);

		attenuation = calculateAttenuation(light_distance, light.range);

		float angle = acos(dot(light_dir, light.direction));
		if (angle > light.outerCutoff) attenuation = 0.0;
		}
	break;
}

// diffuse lighting
float NdotL = max(dot(light_dir, normal), 0);
vec3 diffuse = light.color * u_material.baseColor * NdotL ;



//specular
vec3 view_dir = normalize(-position);

//blinn phong
vec3 halfway_dir = normalize(light_dir + view_dir);
float NdotH = max(dot(normal, halfway_dir), 0);
NdotH = pow(NdotH, u_material.shininess);
vec3 specular = vec3(NdotH);

//phong
//vec3 reflection = reflect(-light_dir, normal);
//float RdotV = max(dot(reflection, view_dir), 0);
//RdotV = pow(RdotV, u_material.shininess);
//vec3 specular = vec3(RdotV);



return (diffuse + specular) * light.intensity * attenuation;
}

void main()
{
	vs_out.texcoord = a_texcoord * u_material.tiling + u_material.offset;

	mat4 model_view = u_view * u_model;
	vec3 position = vec3(model_view * vec4(a_position, 1));
	vec3 normal = normalize(mat3(model_view) * a_normal);

	vs_out.color = u_ambient_light;
	for (int i = 0; i < u_numLights; i++) {
	vs_out.color += calculateLight(u_lights[i], position, normal);
	
	}
	

	gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0);
}
