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


uniform vec2 offset;
uniform mat4 camera_transform;
uniform mat4 local_to_world;
uniform vec4 camera_box;

void main()
{
	vec2 pos = position + offset;

	vs_position = pos;
	vs_color = color;

	vs_texcoord0 = texcoord0;
	vs_texcoord1 = texcoord1;
	vs_texcoord2 = texcoord2;

	vec4 transformed_pos = local_to_world * camera_transform * vec4(pos.x, pos.y, 0.0, 1.0);

	gl_Position.xy = transformed_pos.xy / camera_box.zw;
	gl_Position.z = 0.0;
	gl_Position.w = 1.0;
}