in vec2 position;
in vec4 color;

in vec3 texcoord0;
in vec3 texcoord1; 
in vec2 texcoord2;


out vec2 vs_position;
out vec4 vs_color;

out vec3 vs_texcoord0;
out vec3 vs_texcoord1;
out vec2 vs_texcoord2;


uniform mat4 local_to_camera;
uniform mat4 projection_matrix;

void main()
{
	vs_position = position;
	vs_color = color;

	vs_texcoord0 = texcoord0;
	vs_texcoord1 = texcoord1;
	vs_texcoord2 = texcoord2;

	gl_Position = projection_matrix * local_to_camera * vec4(position.x, position.y, 0.0, 1.0);
}