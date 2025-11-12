#version 460 core

#define MAX_LIGHTS 5
#define POINT 0
#define DIRECTIONAL 1
#define SPOT 2

#define BASE_MAP     (1 << 0)
#define	SPECULAR_MAP (1 << 1)
#define	EMISSIVE_MAP (1 << 2)
#define NORMAL_MAP	 (1 << 3)

in VS_OUT
{
vec2 texcoord;
vec3 position;
vec3 normal;
mat3 tbn;
} fs_in;

out vec4 f_color;


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
vec3 baseColor;
vec3 emissiveColor;
float shininess;
vec2 tiling;
vec2 offset;
uint parameters;
};

uniform int u_numLights = 1;
uniform Light u_lights[MAX_LIGHTS];
uniform Material u_material;
uniform sampler2D u_baseMap;
uniform sampler2D u_specularMap;
uniform sampler2D u_emissiveMap;
uniform sampler2D u_normalMap;

vec3 calculateNormal()
{
	// generate the normals from the normal map
	vec3 normal = texture(u_normalMap, fs_in.texcoord).rgb;
	// convert rgb normal (0 <-> 1) to xyx (-1 <-> 1)
	normal = normalize(normal);
	// transform normals to model view space
	normal = normalize(fs_in.tbn * normal);

	return normal;
}


float calculateAttenuation(in float light_distance, in float range) {
	float attenuation = max(0.0 ,(1.0 - (light_distance / range)));


	return pow(attenuation, 2);
}

vec3 calculateLight(in Light light,in vec3 position, in vec3 normal, in float specularMask)
{
vec3 light_dir;
float attenuation;

switch (light.type) {
	case POINT:
	{
		light_dir = normalize(light.position - fs_in.position);
		float light_distance = length(light.position - fs_in.position);
		attenuation = calculateAttenuation(light_distance, light.range);
		}
	break;

	case DIRECTIONAL:
		light_dir = normalize(light.direction);
	break;

	case SPOT:
	{
		light_dir = normalize(light.position - fs_in.position);
		float light_distance = length(light.position - fs_in.position);

		attenuation = calculateAttenuation(light_distance, light.range);

		float angle = acos(dot(light_dir, light.direction));
		if (angle > light.outerCutoff) attenuation = 0.0;
		}
	break;
}

// diffuse lighting
float NdotL = max(dot(light_dir, fs_in.normal), 0);
vec3 diffuse = light.color * u_material.baseColor * NdotL ;



//specular
vec3 view_dir = normalize(-position);

//blinn phong
vec3 halfway_dir = normalize(light_dir + view_dir);
float NdotH = max(dot(fs_in.normal, halfway_dir), 0);
NdotH = pow(NdotH, u_material.shininess);
vec3 specular = vec3(NdotH) * specularMask;

return (diffuse + specular) * light.intensity * attenuation;
}

//uniform sampler2D u_texture;
void main()
{
float specularMask = ((u_material.parameters & SPECULAR_MAP) != 0u)
  ? texture(u_specularMap, fs_in.texcoord).r
  : 1;

  vec3 normal = ((u_material.parameters & NORMAL_MAP) != 0u)
	? calculateNormal()
	: fs_in.normal;

vec3 color = u_ambient_light;
	for (int i = 0; i < u_numLights; i++) {
	color += calculateLight(u_lights[i], fs_in.position, normal, specularMask);
	}

	vec4 emissive = ((u_material.parameters & EMISSIVE_MAP) != 0u)
  ? texture(u_emissiveMap, fs_in.texcoord) * vec4(u_material.emissiveColor, 1)
  : vec4(u_material.emissiveColor, 1);

	f_color = texture(u_baseMap, fs_in.texcoord) * vec4(color, 1) + emissive;
}