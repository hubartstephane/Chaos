in vec2 position;
in vec2 texcoord;
in vec4 color;

out vec2 vs_texcoord;

void main() 
{
  vs_texcoord = texcoord;

  gl_Position.xy = position;
  gl_Position.z  = 0.0;
  gl_Position.w  = 1.0;
}