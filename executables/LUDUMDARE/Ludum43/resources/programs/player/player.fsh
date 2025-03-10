out vec4 output_color; // "output_color" replaces "gl_FragColor" because glBindFragDataLocation(...) has been called

in vec2 vs_position;
in vec3 vs_texcoord;
in vec4 vs_color;

uniform sampler2DArray material;


void main()
{
	vec4 color = (vs_texcoord.z < 0.0) ?
		vec4(1.0, 1.0, 1.0, 1.0) :
		texture(material, vs_texcoord);
  
	//float translucency = length(color.xyz);
  
	output_color.xyz = color.xyz;
	//output_color.a   = translucency * vs_color.a * color.a;
	output_color.a   = vs_color.a * color.a;
};