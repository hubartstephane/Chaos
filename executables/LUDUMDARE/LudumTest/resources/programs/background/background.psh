out vec4 output_color; // "output_color" replaces "gl_FragColor" because glBindFragDataLocation(...) has been called

uniform sampler2D background;

in vec2 vs_position;
in vec2 vs_texcoord;
in vec4 vs_color;

void main()
{
	vec4 b1 = texture(background, vs_texcoord);
	output_color = b1;	
	output_color.a = 1.0;
}