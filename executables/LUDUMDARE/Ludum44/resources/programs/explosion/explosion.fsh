out vec4 output_color; // "output_color" replaces "gl_FragColor" because glBindFragDataLocation(...) has been called

in vec2 vs_position;
in vec3 vs_texcoord;
in vec4 vs_color;

uniform sampler2DArray material;

void main()
{
	vec4 color = texture(material, vs_texcoord);

	

	output_color.xyz = color.xyz * vs_color.xyz;
	output_color.a   = vs_color.a * color.a;			
};