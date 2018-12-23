

in vec2 position;
in vec3 texcoord;
in vec4 color;

out vec2 vs_position;
out vec3 vs_texcoord;
out vec4 vs_color;

uniform vec4 camera_box;

void main() 
{
  vs_position = position;
  vs_texcoord = texcoord;
  vs_color    = color;

  gl_Position.xy = (position - camera_box.xy) / camera_box.zw;
  gl_Position.z  = 0.0;
  gl_Position.w  = 1.0;
}