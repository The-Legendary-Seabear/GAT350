#version 430

#define GRAYSCALE	(1 << 0)
#define COLORTINT	(1 << 1)
#define SCANLINE 	(1 << 2)
#define GRAIN    	(1 << 3)
#define INVERT  	(1 << 4)

in vec2 v_texcoord;

out vec4 f_color;

// post process controls
uniform uint u_parameters = 0;
uniform vec3 u_colorTint = vec3(1, 1, 1);
uniform float u_time = 0;
uniform float u_blend = 0;

uniform sampler2D u_baseMap;

float random(vec2 st) {
	return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
	vec4 color = texture(u_baseMap, v_texcoord);
	vec4 postprocess = color;

	float average = (color.r + color.g + color.b) / 3.0;

	 if ((u_parameters & GRAYSCALE) != 0u)	postprocess = vec4(vec3((postprocess.r + postprocess.g + postprocess.b) / 3.0), postprocess.a);
	 if ((u_parameters & COLORTINT) != 0u)	postprocess = postprocess * vec4(u_colorTint, 1);
	 if ((u_parameters & SCANLINE) != 0u)	postprocess = (int(gl_FragCoord.y) % 3 != 0) ? vec4(0, 0, 0, 1) : postprocess;
	 if ((u_parameters & GRAIN) != 0u)		postprocess = postprocess * random(gl_FragCoord.xy + u_time);
	 if ((u_parameters & INVERT) != 0u)		postprocess = vec4(1) - postprocess;
	 
	f_color = mix(color, postprocess, u_blend); // mix colors using u_blend (0-1)

}

