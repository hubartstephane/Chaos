in vec2 position;

out vec2 vs_position;

void main() 
{
  vs_position = position;

  gl_Position.xy = position;
  gl_Position.z  = 0.0;
  gl_Position.w  = 1.0;
}