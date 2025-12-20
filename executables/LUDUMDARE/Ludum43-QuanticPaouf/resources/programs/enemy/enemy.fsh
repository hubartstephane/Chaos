out vec4 output_color; // "output_color" replaces "gl_FragColor" because glBindFragDataLocation(...) has been called

in vec2 vs_position;
in vec3 vs_texcoord;
in vec4 vs_color;

in float distance_to_center;
in vec2  particle_center;

uniform float enlarge_enemy; 

uniform sampler2DArray material;

void main()
{
	vec4 color = (vs_texcoord.z < 0.0) ?
		vec4(1.0, 1.0, 1.0, 1.0) :
		texture(material, vs_texcoord);

	if (enlarge_enemy > 0.0) // enlarged zone
	{
		float ratio = clamp(1.0 - (length(vs_position - particle_center) / distance_to_center), 0.0, 1.0);

		output_color.xyz = vec3(ratio, ratio, ratio);
		output_color.a = ratio;
	}
	else // normal rendering
	{
		output_color.xyz = color.xyz * vs_color.xyz; 
		output_color.a   = vs_color.a * color.a;				
	}
};