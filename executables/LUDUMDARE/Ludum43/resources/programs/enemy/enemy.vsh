in vec2 position0;
in vec3 texcoord;
in vec4 color;
in int  flags;
in vec2 position1;
in vec2 position2; // the center of the particle

out vec2 vs_position;
out vec3 vs_texcoord;
out vec4 vs_color;
out flat int vs_flags;

out float distance_to_center;
out vec2  particle_center;

uniform mat4 local_to_camera;
uniform mat4 projection_matrix;
uniform vec2 offset;
uniform float position_blend_ratio;

uniform sampler2DArray material; // texture required in VS for Half pixel correction

void main() 
{
	

  vec2 p = (position_blend_ratio * position0) + (1.0 - position_blend_ratio) * position1 + offset;


	// For enlarged enemies, we want to compute the circle inside the particle box (instead of reading a texture)
	//   => the texture is good used for normal rendering is good. Always white except a small blur near the limit of the circle
	//   => for enlarged enemies (and so deformations), we want a color much more blured => we compute it instead of reading the texture
	
	// By luck, the four vertices that constitute the particle QUAD are at the same distance from the particle center.
	// That means that 'distance_to_center' is a constant and the interpolation for each pixel will be a constant

	particle_center = position2;
	distance_to_center = length(p - particle_center) / 1.41; // the circle has a radius 1.41 time smaller than the quad

	vs_position = p;
	vs_texcoord = HalfPixelCorrection(texcoord, flags, material);
	vs_flags = ExtractFragmentFlags(flags);
	vs_color    = color;

	gl_Position = projection_matrix * local_to_camera * vec4(p.x, p.y, 0.0, 1.0);
}			