in vec2 position; // incomming values are [-1, +1]

out vec2 vs_position;
out vec2 vs_texcoord;

uniform float main_time;

void main() 
{
  vs_position = position;

  vs_texcoord = 0.5 * position + vec2(0.5, 0.5) - vec2(0.0, main_time * 0.1);

  gl_Position.xy = position; // for fullscreen, we want [-1, +1]
  gl_Position.z  = 0.0;
  gl_Position.w  = 1.0;
}