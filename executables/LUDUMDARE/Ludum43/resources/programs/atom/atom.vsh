in vec2 position;
in vec3 texcoord;
in vec4 color;

out vec2 vs_position;
out vec3 vs_texcoord;
out vec4 vs_color;

uniform vec2 offset;
uniform mat4 camera_transform;
uniform vec2 camera_half_size;

void main() 
{
	vec2 pos = position + offset;

	vs_position = pos;
	vs_texcoord = texcoord;
	vs_color    = color;

	vec4 transformed_pos = camera_transform * vec4(pos.x, pos.y, 0.0, 1.0);

	gl_Position.xy = transformed_pos.xy / camera_half_size;
	gl_Position.z  = 0.0;
	gl_Position.w  = 1.0;
}			