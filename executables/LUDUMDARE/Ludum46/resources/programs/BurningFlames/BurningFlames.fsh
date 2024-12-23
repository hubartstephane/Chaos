out vec4 output_color; // "output_color" replaces "gl_FragColor" because glBindFragDataLocation(...) has been called

in vec2 vs_position;
in vec3 vs_texcoord;
in vec4 vs_color;

uniform sampler2DArray material;

uniform int burning_decrease_step;


void main()
{
	// XXX: we get the good result for step in [0..8]



	// PIXELIZED EFFECT / no interpolation
	ivec3 size = textureSize(material, 0);
	ivec3 coord;
	coord.xy = ivec2((vs_texcoord.xy * vec2(size.xy)));
	coord.z = int(vs_texcoord.z);
	vec4 color = texelFetch(material, coord, 0);

	// discretize the color range 
	// take RGB as a direction. Get the length of the color in that direction
	// Discretize length value
	// [0, 1, 2, 3, 4, 5 ... 255] ----> [0, 10, 20, ...250]
	// recompoze the 'final' color with this new length and the old direction

	vec3 color_dir = normalize(color.rgb);

	float color_count = 12.0;

	int step = burning_decrease_step; // for fire disruption

	float color_len = length(color.rgb);
	
	color_len = float(max(0, int(color_len * color_count) - step)) / color_count;

	color.rgb = color_dir * color_len;


	// alpha the texture
	float lim = 0.3;

	float l = length(color.xyz);
	if (l < lim)
		color.a *= 1.0 - ((lim - l) / lim);

	output_color = color * vs_color;
};