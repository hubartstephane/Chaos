out vec4 output_color; // "output_color" replaces "gl_FragColor" because glBindFragDataLocation(...) has been called

uniform float fade_ratio;

in vec2 vs_position;
in vec2 vs_texcoord;
in vec4 vs_color;

void main()
{
	output_color = vec4(0.0, 0.0, 0.0, fade_ratio);
}