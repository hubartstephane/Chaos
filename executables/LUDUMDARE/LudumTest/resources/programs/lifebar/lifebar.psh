out vec4 output_color; // "output_color" replaces "gl_FragColor" because glBindFragDataLocation(...) has been called

in vec2 vs_position;
in vec3 vs_texcoord;
in vec4 vs_color;


uniform vec4 full_life_color;
uniform vec4 lost_life_color;
uniform vec4 sold_life_color;

void main()
{
	float player_life = vs_color.x;
	float player_max_life = vs_color.y;

	bool compute_border_color = true;
	if (vs_texcoord.x > player_max_life)
	{
		output_color = sold_life_color;
		compute_border_color = false;
	}
	else
	{
		if (vs_texcoord.x > player_life)
			output_color = lost_life_color;
		else
			output_color = full_life_color;
	}

	if (compute_border_color)
	{
		float mx = 0.003;
		float my = 0.08;

		vec4 border_color = vec4(1.0, 1.0, 1.0, 1.0);
		if (vs_texcoord.x < mx || abs(vs_texcoord.x - player_max_life) < mx)
			output_color = border_color;
		else if (vs_texcoord.y < my || vs_texcoord.y >(1.0 - my))
			output_color = border_color;
	}
}