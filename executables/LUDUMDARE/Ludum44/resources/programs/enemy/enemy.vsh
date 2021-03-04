in vec2 position;
in vec3 texcoord;
in vec4 color;
in int  flags;

out vec2 vs_position;
out vec3 vs_texcoord;
out vec4 vs_color;
out flat int vs_flags;

uniform vec2 offset;
uniform mat4 camera_transform;
uniform mat4 local_to_world;
uniform vec4 camera_box;

uniform sampler2DArray material; // texture required in VS for Half pixel correction

void main()
{
	vec2 pos = position + offset;
	
	vs_position = pos;
	vs_texcoord = HalfPixelCorrection(texcoord, flags, material);
	vs_flags = ExtractFragmentFlags(flags);
	vs_color = color;

	vec4 transformed_pos = local_to_world * camera_transform * vec4(pos.x, pos.y, 0.0, 1.0);

	gl_Position.xy = transformed_pos.xy / camera_box.zw;
	gl_Position.z = 0.0;
	gl_Position.w = 1.0;
}