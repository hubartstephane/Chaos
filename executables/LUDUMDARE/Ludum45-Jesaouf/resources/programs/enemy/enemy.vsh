in vec2 position;
in vec3 texcoord;
in vec4 color;
in int  flags;

out vec2 vs_position;
out vec3 vs_texcoord;
out vec4 vs_color;
out flat int vs_flags;

uniform mat4 local_to_camera;
uniform mat4 projection_matrix;

uniform sampler2DArray material; // texture required in VS for Half pixel correction

void main()
{
	vs_position = position;
	vs_texcoord = HalfPixelCorrection(texcoord, flags, material);
	vs_flags = ExtractFragmentFlags(flags);
	vs_color = color;

	gl_Position = projection_matrix * local_to_camera * vec4(position.x, position.y, 0.0, 1.0);
}