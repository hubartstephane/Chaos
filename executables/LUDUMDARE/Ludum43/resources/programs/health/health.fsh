out vec4 output_color; // "output_color" replaces "gl_FragColor" because glBindFragDataLocation(...) has been called

in vec2 vs_position;
in vec3 vs_texcoord;
in vec4 vs_color;

void main()
{
	vec4 color_1 = vec4(1.0, 0.0, 0.0, 0.9); 
	vec4 color_2 = vec4(0.5, 0.5, 0.5, 0.3); 
	
	float life = vs_color.a;
	if (vs_texcoord.x < life)
		output_color = color_1;
	else
		output_color = color_2;
}