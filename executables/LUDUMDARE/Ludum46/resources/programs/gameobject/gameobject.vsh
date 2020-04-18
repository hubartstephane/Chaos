


in vec2 position;
in vec3 texcoord;
in vec4 color;

out vec2 vs_position;
out vec3 vs_texcoord;
out vec4 vs_color;

uniform vec4 canvas_box;

void main() 
{
	vs_position = position;
	vs_texcoord = texcoord;
	vs_color    = color;

	gl_Position.xy = position / canvas_box.zw;
	gl_Position.z  = 0.0;
	gl_Position.w  = 1.0;
}

#if 0
in vec2 position;
in vec3 texcoord;
in vec4 color;

out vec2 vs_position;
out vec3 vs_texcoord;
out vec4 vs_color;

uniform vec2 offset;
uniform mat4 camera_transform;
uniform vec4 camera_box;

uniform vec4 canvas_box;

void main()
{
	vec2 pos = position + offset;

	vs_position = pos;
	vs_texcoord = texcoord;
	vs_color = color;

	vec4 transformed_pos = camera_transform * vec4(pos.x, pos.y, 0.0, 1.0);

	gl_Position.xy = transformed_pos.xy / camera_box.zw;

	gl_Position.xy = position / canvas_box.zw;
	gl_Position.z = 0.0;
	gl_Position.w = 1.0;
}
#endif