#version 430

#define GRAYSCALE	(1 << 0)
#define COLORTINT	(1 << 1)
#define SCANLINE 	(1 << 2)

in vec2 v_texcoord;

out vec4 f_color;

// post process controls
uniform uint u_params = 0;
uniform vec3 u_colorTint = vec3(1, 1, 1);
uniform float u_time = 0;
uniform float u_intensity = 1.0;
//uniform vec3 u_tint = vec3(1, 0, 0);

uniform sampler2D u_baseMap;

void main()
{
	vec4 color = texture(u_baseMap, v_texcoord);
	vec4 postprocess = color;

	//postprocess = vec4(1) - postprocess;
	postprocess = postprocess * vec4(u_colorTint, 1);

	// apply postprocess effects
	//if((u_parameters & GRAYSCALE) != 0) applygrayscale;
	
	f_color = postprocess;
}

