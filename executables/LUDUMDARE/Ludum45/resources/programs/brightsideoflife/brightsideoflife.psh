out vec4 output_color; // "output_color" replaces "gl_FragColor" because glBindFragDataLocation(...) has been called

// for fullscreen, incomming position is in [-1, 1], incomming texcoord is in [0, 1]
in vec2 vs_position;
in vec2 vs_texcoord;

uniform sampler2D brightsideoflife;

void main()
{
  vec4 c = texture(brightsideoflife, vs_texcoord);

  output_color.xyz = c.xyz;
  output_color.a = min(c.a, 0.5);
}