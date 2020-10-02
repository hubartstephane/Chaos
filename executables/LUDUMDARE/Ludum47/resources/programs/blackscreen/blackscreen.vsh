in vec2 position;
in vec2 texcoord;
in vec4 color;

out vec2 vs_position;
out vec2 vs_texcoord;
out vec4 vs_color;

void main()
{
	vs_position = position;
	vs_texcoord = texcoord;
	vs_color = color;

	gl_Position.xy = position;
	gl_Position.z = 0.0;
	gl_Position.w = 1.0;
}