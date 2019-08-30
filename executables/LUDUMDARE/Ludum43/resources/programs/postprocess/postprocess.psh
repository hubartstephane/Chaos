out vec4 output_color; // "output_color" replaces "gl_FragColor" because glBindFragDataLocation(...) has been called

// for fullscreen, incomming position is in [-1, 1], incomming texcoord is in [0, 1]
in vec2 vs_position;
in vec2 vs_texcoord;


uniform sampler2D worldlimits;
uniform sampler2D noise;
uniform sampler2D deformed;
uniform float game_time;
uniform float decal_speed;
uniform float decal_ratio;

uniform vec3 worldlimit_color;
uniform vec3 enemy_color;

void main()
{



	// deformed : enlarged rendering + worldlimits
	vec4 data = texture(worldlimits, vs_texcoord);

	float shadow_value = data.r;
	float deformation_value = data.g;

	// compute decal value from 2 slidding noise textures
	vec2 tex1 = 2.0 * vs_texcoord;
	vec2 tex2 = 2.0 * vs_texcoord;
	tex1.y += game_time * decal_speed;
	tex2.x += game_time * decal_speed;

	vec4 decal1 = texture(noise, tex1);
	vec4 decal2 = texture(noise, tex2);

	// vec2 decal = -vec2(1.0, 1.0) + 2.0 * (decal1.xy + decal2.xy);

	vec2 decal = -vec2(1.0, 1.0) + (decal1.xy + decal2.xy);

	// the world limits are too blurry.
	// make the limit between inside and outside shadow more sharp
	// => remap values from [0.6 .. 0.7] => [0.1 .. 1.0]

	float R1 = 0.6;
	float R2 = 0.7;

	float shadow_ratio = 0.0;

	float min_shadow_ratio = 0.2; // throught the shadow we still see the universe !!

	if (shadow_value < R1)
		shadow_ratio = min_shadow_ratio;
	else if (shadow_value > R2)
		shadow_ratio = 1.0;
	else
	{
		shadow_ratio = min_shadow_ratio + (1.0 - min_shadow_ratio) * ((shadow_value - R1) / (R2 - R1));
	}

	// compute the sample coordinate
	vec2 tex = clamp(
		vs_texcoord + decal * (deformation_value + vec2(1.0 - shadow_ratio, 1.0 - shadow_ratio)) * decal_ratio,
		vec2(0.0, 0.0),
		vec2(1.0, 1.0));

	// final color : a blend between source, worldlimit_color and use a noise offset to sample the source
	vec4 previous_pass_color = texture(deformed, tex);
	output_color.xyz = previous_pass_color.xyz * shadow_ratio + worldlimit_color.xyz * (1 - shadow_ratio);

	// add some red around enemies
	//output_color.r = output_color.r + 2.0 * deformation_value;

	output_color.xyz = output_color.xyz + enemy_color * deformation_value;

	//output_color.xyz = vec3(deformation_value, deformation_value, deformation_value);	
	//output_color.xyz = vec3(shadow_value, shadow_value, shadow_value);	
	output_color.a = 1.0;






#if 0

	output_color.xy = vs_texcoord;
	output_color.z = 0.0;
	output_color.a = 1.0;
#endif
}