out vec4 output_color; // "output_color" replaces "gl_FragColor" because glBindFragDataLocation(...) has been called

in vec2 vs_position;
in vec3 vs_texcoord;
in vec4 vs_color;

uniform vec3 touched_color_factor;
uniform vec3 touched_color_offset;

uniform sampler2DArray material;

void main()
{
	vec4 color = texture(material, vs_texcoord);
	output_color.rgb = vs_color.a * color.rgb + (1.0 - vs_color.a) * touched_color_factor;
	output_color.a   = color.a;		
};