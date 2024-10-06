

in vec2 position;
in vec3 texcoord;
in vec4 color;
in int  flags;

out vec2 vs_position;
out vec3 vs_texcoord;
out vec4 vs_color;
out flat int vs_flags;

uniform vec4 canvas_box;

uniform sampler2DArray material; // texture required in VS for Half pixel correction

void main() 
{
	int vs_flags = 0;

	vs_position = position;
	vs_texcoord = HalfPixelCorrection(texcoord, flags, material);
	vs_flags = ExtractFragmentFlags(flags);
	vs_color = color;

	gl_Position.xy = position / canvas_box.zw;
	gl_Position.z = 0.0;
	gl_Position.w = 1.0;
}